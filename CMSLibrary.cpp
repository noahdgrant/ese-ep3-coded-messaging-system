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

char recipientID[140] = {};										// ID of message reciever
char senderID[140] = {};										// ID of message sender
int currentCom = 6;												// Default COM port
wchar_t COMPORT_Tx[] = L"COM6";									// COM port used for transmitting
wchar_t COMPORT_Rx[] = L"COM6";									// COM port used for recieving
int nComRate = 460800;											// Baud (Bit) rate in bits/second 
int nComBits = 8;												// Number of bits per frame
HANDLE hComTx;													// Pointer to the selected COM port (Transmitter)
HANDLE hComRx;													// Pointer to the selected COM port (Receiver)
COMMTIMEOUTS timeout;											// A commtimeout struct variable

extern char secretKey[MAX_QUOTE_LENGTH];						// Key used to encrypt/decrypt messages
extern enum encTypes { ERR, NONE, XOR, VIG, numOfEnc };			// Types of encryption
extern enum encTypes encType;									// Default encryption is NONE

int recordTime = 2;
long numAudioBytes = SAMPLES_SEC * recordTime;

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
	FILE* f;						// pointer to file
	short* playbackBuf = NULL;		// buffer used for reading recorded sound from file

	playbackBuf = (short*)malloc(numAudioBytes * sizeof(short));		
	if (playbackBuf == NULL) {
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
	fread(playbackBuf, sizeof(short), numAudioBytes, f);				// Record to new buffer iBigBufNew
	fclose(f);

	InitializePlayback();
	printf("\nPlaying recording from saved file ...\n");
	PlayBuffer(playbackBuf, numAudioBytes);
	ClosePlayback();

	free(playbackBuf);
	playbackBuf = NULL;
	return 0;
} // playbackAudio()

// Record audio, play it back to the user, and ask if they want to save the file
int recordAudio() {
	FILE* f = NULL;								// Pointer to file
	short* recordBuf = NULL;					// Pointer to record buffer
	char save = '\0';							// Holds wether or not the user wans to save the recording			

	recordBuf = (short*)malloc(numAudioBytes * sizeof(short));
	if (recordBuf == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		return(-1);
	}

	// initialize playback and recording
	InitializePlayback();
	InitializeRecording();

	// start recording
	RecordBuffer(recordBuf, numAudioBytes);
	CloseRecording();

	// playback recording 
	printf("\nPlaying recording from buffer\n");
	PlayBuffer(recordBuf, numAudioBytes);
	ClosePlayback();

	// save audio recording  
	printf("Would you like to save your audio recording? (y/n): ");
	scanf_s("%c", &save, 1);
	while (getchar() != '\n') {}										// Flush other input

	if ((save == 'y') || (save == 'Y')) {
		/* Open input file */
		fopen_s(&f, "C:\\myfiles\\recording.dat", "wb");
		if (!f) {
			printf("unable to open %s\n", "C:\\myfiles\\recording.dat");
			return 0;
		}
		printf("Writing to sound file ...\n");
		fwrite(recordBuf, sizeof(short), numAudioBytes, f);
		fclose(f);
	}

	free(recordBuf);
	recordBuf = NULL;
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
int receiveCom(short* msg, long &msgSz) {
	DWORD bytesRead;						// Number of bytes recieved from incomming message
	short* msgRe = NULL;					// Pointer to buffer where recieved message is stored
	int len = 0;							// length of received message

	msgRe = (short*)malloc(numAudioBytes * sizeof(short));
	if (msgRe == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		return(-1);
	}

	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout);					// Initialize the Rx port
	Sleep(500);

	bytesRead = inputFromPort(&hComRx, (char*)msgRe, numAudioBytes * 2);
	len = (int)bytesRead;														// For some reason the program breaks if bytesRead is changed so len is needed to alter the value

	msgSz = (long)bytesRead;

	if (len == numAudioBytes * 2) {												// If audio was recived, we only want to copy has as many bytes since msgRe is short* but the data was sent as char*
		len /= 2;
	}

	// Copy received message to message buffer that is in main.cpp
	for (int i = 0; i < len; i++) {												
		msg[i] = msgRe[i];
	}

	CloseHandle(hComRx);														// Close the handle to Rx port 
	purgePort(&hComRx);															// Purge the Rx port

	free(msgRe);
	msgRe = NULL;
	return(0);
}

// GUI OPTIONS
// Change Com port
void selectComPort() {
	char cmd[3] = {};			// Holds the COM port the user wants to use
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
	char cmd[3];				// Holds the length of time the user wants to record audio
	do {
		system("cls");
		printf("Enter a new recording length between 1 and 15 seconds\n");
		printf("\n> ");
		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) >= 1 && atoi(cmd) <= 15) {
			printf("\nThe new recording length is now %d\n", atoi(cmd));
			recordTime = atoi(cmd);								
			numAudioBytes = SAMPLES_SEC * recordTime;						
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 1 || atoi(cmd) > 15);
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

