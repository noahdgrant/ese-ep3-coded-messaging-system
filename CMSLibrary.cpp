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

#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"

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
wchar_t COMPORT_Tx[] = L"COM6";										// COM port used for Rx (use L"COM6" for transmit program)
wchar_t COMPORT_Rx[] = L"COM7";										// COM port used for Rx (use L"COM6" for transmit program)
const int BUFSIZE = 140;											// Buffer size
int nComRate = 460800;												// Baud (Bit) rate in bits/second 
int nComBits = 8;													// Number of bits per frame
HANDLE hComTx;														// Pointer to the selected COM port (Transmitter)
HANDLE hComRx;														// Pointer to the selected COM port (Receiver)
COMMTIMEOUTS timeout;												// A commtimeout struct variable

// Transmit message
void transmitCom(char* msgOut, unsigned long msgSz) {
	initPort(&hComTx, COMPORT_Tx, nComRate, nComBits, timeout);		// Initialize the Tx port
	Sleep(500);

	outputToPort(&hComTx, msgOut, msgSz);				// Send string to port - include space for '\0' termination
	Sleep(500);														// Allow time for signal propagation on cable 

	CloseHandle(hComTx);											// Close the handle to Tx port 
	purgePort(&hComTx);												// Purge the Tx port
	return;
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

// Receive audio message
void receiveAudioComm() {
	// RECEIVE AUDIO
	DWORD bytesRead;
	short iBigBufr[SAMPLES_SEC * RECORD_TIME];													// buffer
	extern long  lBigBufSize;												// total number of samples

	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout);				// Initialize the Rx port
	Sleep(500);

	bytesRead = inputFromPort(&hComRx, (char*)iBigBufr, lBigBufSize*2);				// Receive audio from port
	printf("Length of received msg = %d", bytesRead);

	CloseHandle(hComRx);													// Close the handle to Rx port 
	purgePort(&hComRx);														// Purge the Rx port

	// PLAY BACK AUDIO
	InitializePlayback();
	printf("\nPlaying received recording...\n");
	PlayBuffer(iBigBufr, lBigBufSize);
	ClosePlayback();
}