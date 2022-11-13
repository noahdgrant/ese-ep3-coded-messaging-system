/* message.cpp -Implementation: Functions for CMS project
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#define _CRT_SECURE_NO_DEPRECATE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h> 
#include <wchar.h>
#include <ctype.h>

#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"
#include "CMSLibrary.h"
#include "encryption.h"

char secretKey[MAX_QUOTE_LENGTH] = {};			//key used to encrypt/decrypt messages
enum encTypes {ERR, NONE, XOR, VIG, numOfEnc};
enum encTypes encType = NONE;
char recipientID[140] = {};
char senderID[140] = {};
int recordTime = RECORD_TIME;
int currentCom = 6;

wchar_t COMPORT_Tx[] = L"COM6";									// COM port used for Rx (use L"COM6" for transmit program)
wchar_t COMPORT_Rx[] = L"COM6";									// COM port used for Rx (use L"COM6" for transmit program)
int nComRate = 460800;											// Baud (Bit) rate in bits/second 
int nComBits = 8;												// Number of bits per frame
HANDLE hComTx;													// Pointer to the selected COM port (Transmitter)
HANDLE hComRx;													// Pointer to the selected COM port (Receiver)
COMMTIMEOUTS timeout;											// A commtimeout struct variable

// MENU
// Print CMS menu
void printMenu() {
	printf("\nCMS Menu\n");
	printf("1. Record\n");
	printf("2. Playblack\n");
	printf("3. Generate Quote\n");
	printf("4. View Queue\n");
	printf("5. Transmit Text Message\n");
	printf("6. Transmit Aduio Message\n");
	printf("7. Receive Message\n");
	printf("8. Select Com Port					Com Port:		COM%d\n", currentCom);
	printf("9. Change Audio Recording Length			Length:			%d\n", recordTime);
	printf("10. Set Encryption Type					Encryption Type:	");
	switch (encType) {
	case NONE:
		printf("None\n");
		break;
	case XOR:
		printf("XOR\n");
		break;
	case VIG:
		printf("Viginere\n");
		break;
	}
	printf("11. Set Encryption Key					Encryption Key:		%s\n", secretKey);
	printf("12. Set Recipient ID					RID:			%s\n", recipientID);
	printf("13. Set Sender ID					SID:			%s\n", senderID);
	printf("0. Exit\n");
	printf("\n> ");
	return;
} // printMenu()

// MESSAGES
// Generate a random quote from FortuneCookies.txt file, and ask the user if they want to add it to the queue
int generateQuote() {
	char buff[MAX_QUOTE_LENGTH] = {};							// Buffer that holds the quote from the file
	int numQuotes = 0;											// Number of quotes in the file
	link p = NULL;												// Pointers for pushing and poping the queue
	long int* quoteIndices = NULL;								// Array of quote locations in the file (index correspondes to quote number)
	int* quoteLengths = NULL;									// Array of quote lengths (index correspondes to quote number)
	int result;													// Holds return value from generateQuote()
	char cmd;													// User command

	numQuotes = fnumQuotes();									// Number of quotes
	quoteIndices = fquoteIndices(numQuotes);					// Index locations of the quotes
	quoteLengths = fquoteLength(numQuotes, quoteIndices);		// Length of each quote (up to MAX_QUOTE_LENGTH) - cut off after 

	// Get the random message from the file
	result = GetMessageFromFile(buff, frandNum(0, numQuotes), quoteIndices, quoteLengths);

	if (result != 0) {
		printf("\nERROR: Did not get message from file.\n");
		return (-1);
	}

	printf("\nQUOTE:\n");
	printf("%s\n", buff);

	fflush(stdin);
	printf("\nDo you want to save the quote to the queue? (y/n) ");
	scanf_s("%c", &cmd, 1);
	while (getchar() != '\n') {}											// Flush other input
	if (cmd == 'y' || cmd == 'Y') {
		p = (link)malloc(sizeof(Node));
		if (p == NULL) {														// Make sure memory was allocated
			return (-1);
		}

		strcpy_s(p->Data.message, buff);
		pushQ(p);
	}

	// Free heap memory
	free(quoteLengths);
	quoteLengths = NULL;
	free(quoteIndices);
	quoteIndices = NULL;

	return 0;
} // generateQuote()

// AUDIO
// Playback saved audio file
int playbackAudio() {
	extern long  lBigBufSize;												// total number of samples
	short* iBigBufNew = (short*)malloc(lBigBufSize * sizeof(short));		// buffer used for reading recorded sound from file
	FILE* f;

	if (iBigBufNew == NULL) {
		return(-1);
	}

	// replay audio recording from file -- read and store in buffer, then use playback() to play it
	/* Open input file */
	fopen_s(&f, "C:\\myfiles\\recording.dat", "rb");
	if (!f) {
		printf("unable to open %s\n", "C:\\myfiles\\recording.dat");
		return 0;
	}
	printf("Reading from sound file ...\n");
	fread(iBigBufNew, sizeof(short), lBigBufSize, f);				// Record to new buffer iBigBufNew
	fclose(f);
	InitializePlayback();
	printf("\nPlaying recording from saved file ...\n");
	PlayBuffer(iBigBufNew, lBigBufSize);
	ClosePlayback();

	free(iBigBufNew);

	return 0;
} // playbackAudio()

// Record audio, play it back to the user, and ask if they want to save the file
int recordAudio() {
	short audioMsg[SAMPLES_SEC * RECORD_TIME] = {};
	long msgSz = SAMPLES_SEC * RECORD_TIME;
	char save = '\0';											// Holds wether or not the user wans to save the recording			
	FILE* f = NULL;

	// initialize playback and recording
	InitializePlayback();
	InitializeRecording();

	// start recording
	RecordBuffer(audioMsg, msgSz);
	CloseRecording();

	// playback recording 
	printf("\nPlaying recording from buffer\n");
	PlayBuffer(audioMsg, msgSz);
	ClosePlayback();

	// save audio recording  
	printf("Would you like to save your audio recording? (y/n): ");
	scanf_s("%c", &save, 1);
	while (getchar() != '\n') {}								// Flush other input
	if ((save == 'y') || (save == 'Y')) {
		/* Open input file */
		fopen_s(&f, "C:\\myfiles\\recording.dat", "wb");
		if (!f) {
			printf("unable to open %s\n", "C:\\myfiles\\recording.dat");
			return 0;
		}
		printf("Writing to sound file ...\n");
		fwrite(audioMsg, sizeof(short), msgSz, f);
		fclose(f);
	}
	return 0;
} // recordAudio()

// SERIAL COMMUNIACTION
// Transmit text message
void transmitCom(short* msgOut, long msgSz) {
	initPort(&hComTx, COMPORT_Tx, nComRate, nComBits, timeout);								// Initialize the Tx port
	Sleep(500);

	outputToPort(&hComTx, msgOut, msgSz);													// Send string to port - include space for '\0' termination
	Sleep(500);																				// Allow time for signal propagation on cable 
		
	CloseHandle(hComTx);																	// Close the handle to Tx port 
	purgePort(&hComTx);																		// Purge the Tx port
	return;
}

// Receive audio message
void receiveCom(short* msg, long &msgSz) {
	DWORD bytesRead;
	short msgIn[SAMPLES_SEC * RECORD_TIME] = {};
	int len = 0;

	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout);		// Initialize the Rx port
	Sleep(500);

	bytesRead = inputFromPort(&hComRx, (char*)msgIn, (SAMPLES_SEC * RECORD_TIME) * 2);
	len = (int)bytesRead;

	msgSz = (long)bytesRead;

	if (len == SAMPLES_SEC * RECORD_TIME * 2) {
		len /= 2;
	}

	for (int i = 0; i < len; i++) {
		msg[i] = msgIn[i];
	}

	//printf("Length of received msg = %d", bytesRead);

	CloseHandle(hComRx);											// Close the handle to Rx port 
	purgePort(&hComRx);												// Purge the Rx port

	return;
}

// GUI OPTIONS
// Change Com port
void selectComPort() {
	char cmd[3] = {};
	do {
		system("cls");
		printf("Enter a number between 0 and 9 coresponding to the desired Com Port (ie. 1 -> COM1)\n");
		printf("\n> ");
		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer
		if (atoi(cmd) >= 0 && atoi(cmd) <= 9) {
			printf("\nThe new Com Port is now COM%d\n", atoi(cmd));
			switch (atoi(cmd)) {
			case 0:
				wcscpy(COMPORT_Tx, L"COM0");
				wcscpy(COMPORT_Rx, L"COM0");
				currentCom = 0;
				break;
			case 1:
				wcscpy(COMPORT_Tx, L"COM1");
				wcscpy(COMPORT_Rx, L"COM1");
				currentCom = 1;
				break;
			case 2:
				wcscpy(COMPORT_Tx, L"COM2");
				wcscpy(COMPORT_Rx, L"COM2");
				currentCom = 2;
				break;
			case 3:
				wcscpy(COMPORT_Tx, L"COM3");
				wcscpy(COMPORT_Rx, L"COM3");
				currentCom = 3;
				break;
			case 4:
				wcscpy(COMPORT_Tx, L"COM4");
				wcscpy(COMPORT_Rx, L"COM4");
				currentCom = 4;
				break;
			case 5:
				wcscpy(COMPORT_Tx, L"COM5");
				wcscpy(COMPORT_Rx, L"COM5");
				currentCom = 5;
				break;
			case 6:
				wcscpy(COMPORT_Tx, L"COM6");
				wcscpy(COMPORT_Rx, L"COM6");
				currentCom = 6;
				break;
			case 7:
				wcscpy(COMPORT_Tx, L"COM7");
				wcscpy(COMPORT_Rx, L"COM7");
				currentCom = 7;
				break;
			case 8:
				wcscpy(COMPORT_Tx, L"COM8");
				wcscpy(COMPORT_Rx, L"COM8");
				currentCom = 8;
				break;
			case 9:
				wcscpy(COMPORT_Tx, L"COM9");
				wcscpy(COMPORT_Rx, L"COM9");
				currentCom = 9;
				break;
			default:
				printf("Something went wrong with the com assignment");
				Sleep(2000);
			}
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 0 || atoi(cmd) > 9);
}

// Change Audio Settings
void changeAudioSettings() {
	char cmd[3];
	do {
		system("cls");
		printf("Enter a new recording length between 1 and 15 seconds\n");
		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) >= 1 && atoi(cmd) <= 15) {
			printf("The new recording length is now %d\n", atoi(cmd));
			recordTime = atoi(cmd);
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 1 || atoi(cmd) > 15);
}

// Set encryption Type 
void setEncryption() {
	char cmd[2] = {};
	do {
		system("cls");
		printf("Enter type of encryption/decryption\n");
		printf("1. No Encryption\n");
		printf("2. XOR\n");
		printf("3. Viginere\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) == NONE) {
			printf("\nNow using no encryption\n");
			encType = NONE;
		}
		else if (atoi(cmd) == XOR) {
			printf("\nNow using XOR encryption\n");
			encType = XOR;
		}
		else if (atoi(cmd) == VIG) {
			printf("\nNow using Viginere encryption\n");
			encType = VIG;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < NONE || atoi(cmd) > numOfEnc);
}

// set the XOR code
void setSecretKey() {
	printf("\nEnter encryption key: ");
	scanf_s("%s", secretKey, MAX_QUOTE_LENGTH - 1);
}

// Set the recipient ID
void setRID() {
	printf("\nEnter the recipient ID: ");
	scanf_s("%s", recipientID, MAX_QUOTE_LENGTH - 1);
}

// Set the Sender ID
void setSID() {
	printf("\nEnter the sender ID: ");
	scanf_s("%s", senderID, MAX_QUOTE_LENGTH - 1);
}

void encrypt(void* msg, int msgSz) {
	// XOR Encryption
	if (encType == XOR) {
		xorCipher(msg, msgSz, secretKey, strlen(secretKey));
	}
	// Viginere encryption
	else if (encType == VIG) {
		vigCipher(msg, msgSz, secretKey, strlen(secretKey), true);
	}

	return;
}

void decrypt(void* msg, int msgSz) {
	// Decrypt the message (xor)
	if (encType == XOR) {
		xorCipher(msg, msgSz, secretKey, strlen(secretKey));
	}
	// Decrypt the message (Viginere)
	else if (encType == VIG) {
		vigCipher(msg, msgSz, secretKey, strlen(secretKey), false);
	}

	return;
}