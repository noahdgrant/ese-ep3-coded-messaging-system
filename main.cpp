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
	char cmd[3];
	link q;

	srand(time(NULL));											// Seed the random number generator 
	initQueue();

	do {
		system("cls");
		printMenu();

		// Input from user
		fflush(stdin);											// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, sizeof(cmd));	

		while (getchar() != '\n') {}							// Flush other input
		
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
				fflush(stdin);													// Flush input buffer after use. Good practice in C
				scanf_s("%[^\n]s", msg, sizeof(msg));						// Reading complete strings with scanf_s: https://www.geeksforgeeks.org/difference-between-scanf-and-gets-in-c/
				while (getchar() != '\n') {}									// Flush other input

				transmitCom(msg, strlen(msg) + 1);
				Sleep(4000);
				break;
			// Recieve Text Message
			case 6:
				receiveCom();
				Sleep(4000);
				break;
			// Transmit audio message
			case 7:
				// RECORD MESSAGE
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
				while (getchar() != '\n') {}										// Flush other input
				if ((cmd == 'y') || (cmd == 'Y')) {
					transmitCom((char*) iBigBuf, (unsigned long) lBigBufSize*2);
					Sleep(4000);
				}
				break;
			// Recieve audio message
			case 8:
				receiveCom();
				Sleep(4000);
				break;
			// Change Com Port
			case 9:
				selectComPort();
				break;
			// Change Audio Settings
			case 10:
				changeAudioSettings();
				break;
			// Set Encription Type
			case 11:
				setEncryption();
				break;
			// Set Encription Code
			case 12:
				setSecretKey();
				break;
			// Set Recipient ID
			case 13:
				setRID();
				break;
			// Set Sender ID
			case 14:
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
