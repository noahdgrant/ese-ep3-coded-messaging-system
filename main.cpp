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

int	main(int argc, char* argv[])
{
	// LOCAL VARIABLE DECLARATION AND INITIALIZATION
	char cmd[3] = {};											// Holds the user's command
	extern short iBigBuf[];										// Buffer that holds audio recording
	extern long  lBigBufSize;									// Size of audio buffer
	char msg[MAX_QUOTE_LENGTH] = {};							// Text message to transmit
	link q = NULL;												// Pointer to start of queue
	char sendMsg[3] = {};										// Holds wether the user wants to send the audio message or not
	short audioMsg[SAMPLES_SEC * RECORD_TIME] = {};

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
				//encrypt(msg)
				//compress(msg)
				transmitCom((short*)msg, strlen(msg) + 1);
				Sleep(4000);
				break;
			// Transmit audio message
			case 6:
				// RECORD MESSAGE
				InitializeRecording();
				RecordBuffer(audioMsg, lBigBufSize);
				CloseRecording();

				// SEND AUDIO MESSAGE
				printf("\n\nWould you like to send your audio recording? (y/n): ");
				fflush(stdin);
				scanf_s("%s", sendMsg, 2);
				while (getchar() != '\n') {}										
				if (sendMsg[0] == 'y' || sendMsg[0] == 'Y') {
					transmitCom(audioMsg, lBigBufSize * 2);
				}
				Sleep(4000);
				break;
			// Recieve message
			case 7:
				receiveCom();
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
