/* main.cpp : Mainline for CMS project
   Author(s): Noah Grant, Wyatt Richard
   Version: 01.00
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "CMSLibrary.h"
#include "message.h"
#include "queues.h"
#include "sound.h"
#include "encryption.h"

int	main(int argc, char* argv[])
{
	// LOCAL VARIABLE DECLARATION AND INITIALIZATION
	char cmd[3] = {};											// Holds the user's command
	extern long  numAudioBytes;									// Size of audio buffer
	char msg[MAX_QUOTE_LENGTH] = {};							// Text message to transmit
	link q = NULL;												// Pointer to start of queue
	char sendCmd[3] = {};										// Holds wether the user wants to send the audio message or not
	short* audioMsg = NULL;
	short* msgIn = NULL;
	long msgInSz = 0;

	// START-UP PROCESSES
	srand(time(NULL));					 						// Seed the random number generator 
	initQueue();

	// MAIN LOOP
	do {
		system("cls");
		printMenu();

		// Input from user
		fflush(stdin);											// Flush input buffer before use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));	
		while (getchar() != '\n') {}							// Flush other input buffer
		
		switch (atoi(cmd)) {
			// Exit the program
			case 0:
				while (!isQueueEmpty()) {						// Free queue before program exits
					q = popQ();
					free(q);									// Free memory for nodes as they pop off the queue
				}
				break;
			// Record audio message
			case 1:
				recordAudio();
				break;
			// Playback saved audio message
			case 2:
				playbackAudio();
				break;
			// Generate a random quote and save it to the queue
			case 3:
				generateQuote();
				break;
			// Print out each message in the linked-list
			case 4:
				traverse(listHead(), visit);
				Sleep(4000);
				break;
			// Transmit Text Message
			case 5:
				printf("\nWhat message would you link to send?\n\n");
				fflush(stdin);													
				scanf_s("%[^\n]s", msg, (unsigned int)sizeof(msg));				// Reading complete strings with scanf_s: https://www.geeksforgeeks.org/difference-between-scanf-and-gets-in-c/
				while (getchar() != '\n') {}									
				
				encrypt(msg, strlen(msg) + 1);
				transmitCom((short*)msg, strlen(msg) + 1);
				Sleep(4000);
				break;
			// Transmit audio message
			case 6:
				// Get memory for recording
				audioMsg = (short*)malloc(numAudioBytes * sizeof(short));
				if (audioMsg == NULL) {
					printf("\nERROR: Couldn't malloc memory to record audio.\n");
					return(-1);
				}

				// RECORD MESSAGE
				InitializeRecording();
				RecordBuffer(audioMsg, numAudioBytes);
				CloseRecording();

				// SEND AUDIO MESSAGE
				printf("\n\nWould you like to send your audio recording? (y/n): ");
				fflush(stdin);
				scanf_s("%s", sendCmd, 2);
				while (getchar() != '\n') {}		

				if (sendCmd[0] == 'y' || sendCmd[0] == 'Y') {
					encrypt(audioMsg, numAudioBytes * 2);
					transmitCom(audioMsg, numAudioBytes * 2);
				}

				Sleep(4000);
				free(audioMsg);
				break;
			// Recieve message
			case 7:
				// Get memory for recording
				msgIn = (short*)malloc(numAudioBytes * sizeof(short));
				if (msgIn == NULL) {
					printf("\nERROR: Couldn't malloc memory to record audio.\n");
					return(-1);
				}

				receiveCom(msgIn, msgInSz);
				decrypt(msgIn, (int)msgInSz);
				// Play audio message
				if (msgInSz == numAudioBytes * 2) {
					printf("\nPlaying received recording...\n");
					InitializePlayback();
					PlayBuffer(msgIn, numAudioBytes);
					ClosePlayback();
				}
				// Print text message
				else {
					msgIn[msgInSz] = '\0';
					printf("\nMessage Received: %s\n\n", (char*)msgIn);												// Display message from port
				}

				free(msgIn);
				Sleep(4000);
				break;
			// Change Com Port
			case 8:
				selectComPort();
				break;
			// Change Audio Settings
			case 9:
				changeAudioSettings();
				break;
			// Set Encription Type
			case 10:
				setEncryption();
				break;
			// Set Encription Code
			case 11:
				setSecretKey();
				break;
			// Set Recipient ID
			case 12:
				setRID();
				break;
			// Set Sender ID
			case 13:
				setSID();
				break;
			// Invalid command
			default:
				printf("You did not enter a valid command. Please try again.");
				Sleep(2000);
		}
	} while (atoi(cmd) != 0);

	return(0);
}
