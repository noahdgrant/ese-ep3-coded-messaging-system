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
	char cmd = '\0';
	link q;

	srand(time(NULL));											// Seed the random number generator 
	initQueue();

	do {
		system("cls");
		printMenu();

		// Input from user
		fflush(stdin);											// Flush input buffer after use. Good practice in C
		scanf_s("%c", &cmd, 1);									// Get command from user
		while (getchar() != '\n') {}							// Flush other input
		
		switch (cmd) {
			// Exit the program
			case '0':
				while (!isQueueEmpty()) {						// Free queue before program exits
					q = popQ();
					free(q);									// Free memory for nodes as they pop off the queue
				}
				break;
			// Record audio message
			case '1':
				recordAudio();
				break;
			// Playback saved audio message
			case '2':
				playbackAudio();
				break;
			// Generate a random quote and save it to the queue
			case '3':
				generateQuote();
				break;
			// Print out each message in the linked-list
			case '4':
				traverse(listHead(), visit);
				Sleep(4000);
				break;
			// Transmit Text Message
			case '5':
				transmitTextComm();
				break;
			// Recieve Text Message
			case '6':
				receiveTextComm();
			// Transmit audio message
			case '7':
				transmitAudioComm();
				break;
			// Recieve audio message
			case '8':
				receiveAudioComm();
				break;
			// Invalid command
			default:
				printf("You did not enter a valid command. Please try again.");
				Sleep(2000);
		}
	} while (cmd != '0');

	return(0);
}