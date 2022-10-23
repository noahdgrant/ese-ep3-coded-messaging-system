/* main.cpp : Mainline for CMS project
   Author(s): Ian Edwards, Noah Grant, Wyatt Richard
   Version: 01.00
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "CMSLibrary.h"
#include "message.h"
#include "Queues.h"
#include "sound.h"

int	main(int argc, char* argv[])
{
	char cmd = '\0';
	char c;
	link q;

	srand(time(NULL));											// Seed the random number generator
	initQueue();

	do {
		system("cls");
		printMenu();
		scanf_s("%c", &cmd, 1);									// Get command from user
		while ((c = getchar()) != '\n' && c != EOF) {}			// Flush other input
		//fflush(stdin);										// Flush input buffer after use. Good practice in C
		
		switch (cmd) {
			// Exit the program
			case '0':
				while (!isQueueEmpty()) {		// Free queue before program exits
					q = popQ();
					free(q);					// Free memory for nodes as they pop off the queue
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
			// Invalid command
			default:
				printf("You did not enter a valid command. Please try again.");
				Sleep(2000);
		}
	} while (cmd != '0');

	return(0);
}