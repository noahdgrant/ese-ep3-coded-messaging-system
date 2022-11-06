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


#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"
#include "CMSLibrary.h"


// MENU
// Print CMS menu
void printMenu() {
	printf("\nCMS Menu\n");
	printf("1. Record\n");
	printf("2. Playblack\n");
	printf("3. Generate Quote\n");
	printf("4. View Queue\n");
	printf("5. Transmit Text Message\n");
	printf("6. Receive Text Message\n");
	printf("7. Transmit Aduio Message\n");
	printf("8. Receive Audio Message\n");
	printf("9. Select Com Port\n");
	printf("10. Change Audio Settings\n");
	printf("11. Toggle Encription\n");
	printf("12. Set Encription Code\n");
	printf("13. Set Recipient ID\n");
	printf("14. Set Sender ID\n");
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
	extern short iBigBuf[];								// buffer
	extern long  lBigBufSize;							// total number of samples
	char save;											// Holds wether or not the user wans to save the recording			
	FILE* f;

	// initialize playback and recording
	InitializePlayback();
	InitializeRecording();

	// start recording
	RecordBuffer(iBigBuf, lBigBufSize);
	CloseRecording();

	// playback recording 
	printf("\nPlaying recording from buffer\n");
	PlayBuffer(iBigBuf, lBigBufSize);
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
		fwrite(iBigBuf, sizeof(short), lBigBufSize, f);
		fclose(f);
	}
	return 0;
} // recordAudio()

// SERIAL COMMUNIACTION
wchar_t COMPORT_Tx[] = L"COM6";									// COM port used for Rx (use L"COM6" for transmit program)
wchar_t COMPORT_Rx[] = L"COM6";									// COM port used for Rx (use L"COM6" for transmit program)




const int BUFSIZE = 140;											// Buffer size
int nComRate = 460800;												// Baud (Bit) rate in bits/second 
int nComBits = 8;													// Number of bits per frame
HANDLE hComTx;													// Pointer to the selected COM port (Transmitter)
HANDLE hComRx;													// Pointer to the selected COM port (Receiver)
COMMTIMEOUTS timeout;												// A commtimeout struct variable

// Transmit text message
void transmitTextComm() {
	char msgOut[BUFSIZE];

	printf("\nWhat message would you link to send?\n\n");
	fflush(stdin);													// Flush input buffer after use. Good practice in C
	scanf_s("%[^\n]s", msgOut, sizeof(msgOut));									// Get command from user
	while (getchar() != '\n') {}									// Flush other input

	initPort(&hComTx, COMPORT_Tx, nComRate, nComBits, timeout);		// Initialize the Tx port
	Sleep(500);

	outputToPort(&hComTx, msgOut, strlen(msgOut) + 1);				// Send string to port - include space for '\0' termination
	Sleep(500);														// Allow time for signal propagation on cable 

	CloseHandle(hComTx);											// Close the handle to Tx port 
	purgePort(&hComTx);												// Purge the Tx port
	return;
	// Reading complete strings with scanf_s: https://www.geeksforgeeks.org/difference-between-scanf-and-gets-in-c/
}

// Receive text message
void receiveTextComm() {	
	char msgIn[BUFSIZE];
	DWORD bytesRead;

	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout);		// Initialize the Rx port
	Sleep(500);

	bytesRead = inputFromPort(&hComRx, msgIn, BUFSIZE);				// Receive string from port
	printf("Length of received msg = %d", bytesRead);
	msgIn[bytesRead] = '\0';
	printf("\nMessage Received: %s\n\n", msgIn);					// Display message from port

	CloseHandle(hComRx);											// Close the handle to Rx port 
	purgePort(&hComRx);												// Purge the Rx port
	return;
}

// Transmit audio message
void transmitAudioComm() {
	// RECORD MESSAGE
	extern short iBigBuf[];								// buffer
	extern long  lBigBufSize;							// total number of samples
	char cmd;

	// initialize playback and recording
	InitializeRecording();
	InitializePlayback();

	// start recording
	RecordBuffer(iBigBuf, lBigBufSize);
	CloseRecording();

	// playback recording 
	printf("\nPlaying recording from buffer\n");
	PlayBuffer(iBigBuf, lBigBufSize);
	ClosePlayback();

	// save audio recording  
	printf("Would you like to send your audio recording? (y/n): ");
	scanf_s("%c", &cmd, 1);
	while (getchar() != '\n') {}								// Flush other input
	if ((cmd == 'y') || (cmd == 'Y')) {
		// TRANSMIT MESSAGE
		initPort(&hComTx, COMPORT_Tx, nComRate, nComBits, timeout);		// Initialize the Tx port
		Sleep(500);

		outputToPort(&hComTx, iBigBuf, lBigBufSize);						// Send string to port - include space for '\0' termination
		Sleep(500);														// Allow time for signal propagation on cable 

		CloseHandle(hComTx);											// Close the handle to Tx port 
		purgePort(&hComTx);												// Purge the Tx port
	}

	return;
}

// Receive audio message
void receiveAudioComm() {
	// RECEIVE AUDIO
	DWORD bytesRead;
	extern short iBigBuf[];													// buffer
	extern long  lBigBufSize;												// total number of samples

	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout);				// Initialize the Rx port
	Sleep(500);

	bytesRead = inputFromPort(&hComRx, iBigBuf, lBigBufSize);				// Receive audio from port
	printf("Length of received msg = %d", bytesRead);

	CloseHandle(hComRx);													// Close the handle to Rx port 
	purgePort(&hComRx);														// Purge the Rx port

	// PLAY BACK AUDIO
	InitializePlayback();
	printf("\nPlaying received recording...\n");
	PlayBuffer(iBigBuf, lBigBufSize);
	ClosePlayback();
}


// GUI Options

//Change Com port
void selectComPort() {
	char cmd[3];
	do {
		system("cls");
		printf("Enter a number between 0 and 9 coresponding to the desired Com Port (ie. 1 -> COM1)\n");
		fflush(stdin);											// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, sizeof(cmd));
		if (atoi(cmd) >= 0 && atoi(cmd) <= 9) {
			printf("The new Com Port is now COM%d\n", atoi(cmd));
			switch (atoi(cmd)) {
			case 0:
				wcscpy(COMPORT_Tx, L"COM0");
				wcscpy(COMPORT_Rx, L"COM0");
				break;
			case 1:
				wcscpy(COMPORT_Tx, L"COM1");
				wcscpy(COMPORT_Rx, L"COM1");
				break;
			case 2:
				wcscpy(COMPORT_Tx, L"COM2");
				wcscpy(COMPORT_Rx, L"COM2");
				break;
			case 3:
				wcscpy(COMPORT_Tx, L"COM3");
				wcscpy(COMPORT_Rx, L"COM3");
				break;
			case 4:
				wcscpy(COMPORT_Tx, L"COM4");
				wcscpy(COMPORT_Rx, L"COM4");
				break;
			case 5:
				wcscpy(COMPORT_Tx, L"COM5");
				wcscpy(COMPORT_Rx, L"COM5");
				break;
			case 6:
				wcscpy(COMPORT_Tx, L"COM6");
				wcscpy(COMPORT_Rx, L"COM6");
				break;
			case 7:
				wcscpy(COMPORT_Tx, L"COM7");
				wcscpy(COMPORT_Rx, L"COM7");
				break;
			case 8:
				wcscpy(COMPORT_Tx, L"COM8");
				wcscpy(COMPORT_Rx, L"COM8");
				break;
			case 9:
				wcscpy(COMPORT_Tx, L"COM9");
				wcscpy(COMPORT_Rx, L"COM9");
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

//change Audio Settings
void changeAudioSettings() {
	int recordTime;
	char cmd[3];
	do {
		system("cls");
		printf("Enter a new recording length between 1 and 15 seconds\n");
		fflush(stdin);											// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, sizeof(cmd));
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

//toggle XOR encryption
void toggleXOR() {
	printf("\ntoggle XOR\n\n");
	Sleep(4000);
}

//set the XOR code
void setXOR() {
	printf("\nset XOR\n\n");
	Sleep(4000);
}

//set the Recpeint ID
void setRID() {
	printf("\nset RID\n\n");
	Sleep(4000);
}

//set the Sender ID
void setSID() {
	printf("\nSet SID\n\n");
	Sleep(4000);

}