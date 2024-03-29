/***********************************************************
* Name:			main.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Coded Messaging System client.
************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "header.h"
#include "CMSLibrary.h"
#include "compression.h"
#include "encryption.h"
#include "error_detection_correction.h"
#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"
#include "testing.h"
#include "contactList.h"

int	main(int argc, char* argv[])
{
	// LOCAL VARIABLE DECLARATION AND INITIALIZATION
	char cmd[3] = {'\0'};										// User command
	void* msg = NULL;											// Text message to transmit
	link q = NULL;												// Pointer to start of queue
	char sendCmd = '\0';										// Holds wether the user wants to send the audio message or not
	void* audioMsg = NULL;										// Pointer to audio message buffer
	void* msgIn = NULL;											// Pointer to recieved message buffer
	Header txHeader = {};										// Header that stores tranmit information
	Header rxHeader = {};										// Header that stores recieved information
	int numRxMsgs = 0;											// The number of messages in the Rx queue
	int returnCode = 0;											// Holds return value from functions to check success
	Item delMsg;												// Message to delete from queue
	char filename[MAX_QUOTE_LENGTH] = {'\0'};					// Name of file to transmit
	int fSize = 0;												// Number of characters in file

	// START-UP PROCESSES
	srand(time(NULL));					 						// Seed the random number generator 
	initQueue();
	initHeader(txHeader);										// Set header to default values
	initContact();
	printf("   _____          _          _                   \n");
	printf("  / ____|        | |        | |                  \n");
	printf(" | |     ___   __| | ___  __| |                  \n");
	printf(" | |    / _ \\ / _` |/ _ \\/ _` |                  \n");
	printf(" | |___| (_) | (_| |  __/ (_| |                  \n");
	printf("  \\_____\\___/ \\__,_|\\___|\\__,_|    _             \n");
	printf(" |  \\/  |                         (_)            \n");
	printf(" | \\  / | ___  ___ ___  __ _  __ _ _ _ __   __ _ \n");
	printf(" | |\\/| |/ _ \\/ __/ __|/ _` |/ _` | | '_ \\ / _` |\n");
	printf(" | |  | |  __/\\__ \\__ \\ (_| | (_| | | | | | (_| |\n");
	printf(" |_|__|_|\\___||___/___/\\__,_|\\__, |_|_| |_|\\__, |\n");
	printf("  / ____|         | |         __/ |         __/ |\n");
	printf(" | (___  _   _ ___| |_ ___ _ |___/_        |___/ \n");
	printf("  \\___ \\| | | / __| __/ _ \\ '_ ` _ \\             \n");
	printf("  ____) | |_| \\__ \\ ||  __/ | | | | |            \n");
	printf(" |_____/ \\__, |___/\\__\\___|_| |_| |_|            \n");
	printf("          __/ |                                  \n");
	printf("         |___/                                   \n");
	printf("\n Created by Wyatt Richard and Noah Grant\n");
	Sleep(3500);




	// MAIN LOOP 
	do {
		system("cls");
		printMenu(txHeader);

		// Input from user
		fflush(stdin);											// Flush input buffer before use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));	
		while (getchar() != '\n') {}							// Flush other input buffer
		
		switch (atoi(cmd)) {
			// EXIT PROGRAM
			case 0: {
				while (!isQueueEmpty()) {						// Free queue before program exits
					q = popQ();
					free(q);									// Free memory for nodes as they pop off the queue
				}
				saveUserSettings(txHeader);
				closeContacts();
				break;
			}

			// SET COM PORT
			case 1: {
				selectComPort();
				break;
			}
			
			// SET COMPRESSION TYPE
			case 2: {
				setCompression(txHeader);
				break;
			}
			
			// SET ENCRYPTION TYPE
			case 3: {
				setEncryption(txHeader);
				break;
			}

			// SET ENCRYPTION KEY
			case 4: {
				setSecretKey();
				break;
			}

			// SET AUDIO LENGTH
			case 5: {
				changeAudioSettings();
				break;
			}

			// SET RECEIVER ID
			case 6: {
				setRID(txHeader);
				break;
			}

			// SET SENDER ID
			case 7: {
				setSID(txHeader);
				break;
			}

			// ERROR DETECTION AND CORRECTION
			case 8: {
				txHeader.errorDC = !txHeader.errorDC;
				break;
			}

			// TRANSMIT TEXT MESSAGE
			case 9: {
				msg = (char*)malloc(MAX_QUOTE_LENGTH);
				if (msg == NULL) {
					printf("\nERROR: Could not malloc memory for quote buffer.\n");
					break;
				}

				printf("\nWhat message would you link to send?\n\n");
				fflush(stdin);
				scanf_s("%[^\n]s", (char*)msg, MAX_QUOTE_LENGTH);				// Reading complete strings with scanf_s: https://www.geeksforgeeks.org/difference-between-scanf-and-gets-in-c/
				while (getchar() != '\n') {}

				txHeader.payloadType = mTXT;
				txHeader.uncompressedLength = txHeader.payloadSize = strlen((char*)msg) + 1;

				// Encrypt and compress message
				returnCode = encrypt(txHeader, (char*)msg);		// +1 for \0. strlen() only counts chars, it doesn't add the +1 needed for the \0 at the end of the string
				if (returnCode == -1) {							// Secret key was not set
					break;
				}

				txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
				if (txHeader.payloadSize == -1) {
					printf("\nERROR: Compression failed\n");
					Sleep(2000);
					break;
				}

				if (txHeader.errorDC == true) {
					// Calculate checksum of message
					txHeader.checksum = Checksum(msg, txHeader.payloadSize, CHK_8BIT);
					if (txHeader.checksum == 0x11111) {
						printf("\nERROR: Checksum failed\n");
						Sleep(2000);
						break;
					}
				}

				// Transmit message
				transmitCom(&txHeader, msg);

				Sleep(2000);
				free(msg);
				msg = NULL;
				break;
			}

			// TRANSMIT AUDIO MESSAGE
			case 10: {
				// Get memory for recording
				audioMsg = (short*)malloc(numAudioBytes * sizeof(short));
				if (audioMsg == NULL) {
					printf("\nERROR: Couldn't malloc memory to record audio.\n");
					return(-1);
				}

				// Record message
				InitializeRecording();
				RecordBuffer((short*)audioMsg, numAudioBytes);
				CloseRecording();

				// Update header
				txHeader.payloadType = mAUD;
				/* numAudioBytes * 2 because audioMsg gets typecast to (char*) instead of short*.
				Shorts are 2 bytes each and chars are 1 byte each so to have the same amount
				of space it needs to be multiplied by 2. */
				txHeader.uncompressedLength = txHeader.payloadSize = numAudioBytes * 2;

				// Transmit message
				printf("\n\nWould you like to send your audio recording? (y/n): ");
				fflush(stdin);
				scanf_s("%c", &sendCmd, 1);
				while (getchar() != '\n') {}

				if (sendCmd == 'y' || sendCmd == 'Y') {
					encrypt(txHeader, audioMsg);

					txHeader.payloadSize = compress(&audioMsg, txHeader.uncompressedLength, txHeader.compression);
					if (txHeader.payloadSize == -1) {
						printf("\nERROR: Compression failed\n");
						Sleep(2000);
						break;
					}

					if (txHeader.errorDC == true) {
						// Calculate checksum of message
						txHeader.checksum = Checksum(audioMsg, txHeader.payloadSize, CHK_8BIT);
						if (txHeader.checksum == 0x11111) {
							printf("\nERROR: Checksum failed\n");
							Sleep(2000);
							break;
						}
					}

					transmitCom(&txHeader, audioMsg);
				}

				Sleep(2000);
				free(audioMsg);
				audioMsg = NULL;
				break;
			}

			// TRANSMIT *.TXT FILE
			case 11: {
				//Get file name
				printf("\nFilename to transmit (no spaces & includeing extension): ");
				fflush(stdin);
				scanf_s("%s", filename, MAX_QUOTE_LENGTH);
				while (getchar() != '\n') {}

				fSize = fileSz(filename);
				if (fSize == -1) break;

				// Malloc memory for file
				msg = (char*)calloc(1, strlen(filename) + strlen("!!") + fSize); // !! is used to denote the end of the filename and the beginning of the file data
				if (msg == NULL) {
					printf("\nERROR: Could not malloc memory for quote buffer.\n");
					break;
				}

				// Copy file to msg buffer
				fSize = copyFile((char*)msg, filename, fSize);

				// Transmit file
				txHeader.payloadType = mTXTFILE;
				txHeader.uncompressedLength = txHeader.payloadSize = fSize;

				// Encrypt and compress message
				returnCode = encrypt(txHeader, (char*)msg);
				if (returnCode == -1) {							// Secret key was not set
					break;
				}

				txHeader.payloadSize = compress(&msg, (unsigned int)txHeader.uncompressedLength, txHeader.compression);
				if (txHeader.payloadSize == -1) {
					printf("\nERROR: Compression failed\n");
					Sleep(2000);
					break;
				}

				// Calculate checksum
				if (txHeader.errorDC == true) {
					// Calculate checksum
					txHeader.checksum = Checksum(msg, txHeader.payloadSize, CHK_8BIT);
					if (txHeader.checksum == 0x11111) {
						printf("\nERROR: Checksum failed\n");
						Sleep(2000);
						break;
					}
				}

				// Transmit message
				transmitCom(&txHeader, msg);

				free(msg);
				msg = NULL;
				Sleep(2000);
				break;
			}

			// RECEIVE MESSAGE
			case 12: {
				returnCode = receiveCom(&rxHeader, &msgIn);
				if (returnCode == -1) break;

				if (rxHeader.errorDC == true) {
					// Voteon() error detection and correction on message header
					returnCode = checkHeader(rxHeader);
					if (returnCode == -1) break;

					// Checksum() error detection on received message
					if (rxHeader.checksum != Checksum(msgIn, rxHeader.payloadSize, CHK_8BIT)) {
						printf("\nERROR: Received checksum did not match transmitted checksum\n");
						Sleep(2000);
						break;
					}
				}

				// Decompress and decrypt received message
				decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
				decrypt(rxHeader, msgIn);

				if (rxHeader.rid == txHeader.sid) {
					// Play audio message
					if (rxHeader.payloadType == mAUD) {
						printf("\nPlaying received recording...\n");
						InitializePlayback();
						PlayBuffer((short*)msgIn, rxHeader.uncompressedLength / 2);			// /2 since it was *2 to send the chars but now needs to be read as shorts
						ClosePlayback();
						Sleep(500);
					}
					// Print text message
					else if (rxHeader.payloadType == mTXT) {
						printf("\nMessage Received: %s\n\n", (char*)msgIn);
						system("pause");					// Wait for user to press key before returning to main menu
					}
				}
				else {
					printf("\nMessage Received for another user\n\n");
					system("pause");					// Wait for user to press key before returning to main menu

				}

				// Save txt file
				if (rxHeader.payloadType == mTXTFILE) {
					saveFile((char*)msgIn);
					Sleep(2000);
				}
				else {
					// Queue recieved message
					qRxMsg(rxHeader, msgIn);

					// Increment the counter for number of items in recieve queue
					numRxMsgs++;
				}

				free(msgIn);
				msgIn = NULL;
				break;
			}
			
			// PRINT RECEIVED MESSAGES
			case 13: {
				if (numRxMsgs == 0) {
					printf("\nNo received messages\n");
					Sleep(1500);
				}
				else {
					delMsg.msgHeader.seqNum = 0;	// Reset the message to be deleted each time

					// Print received messages
					system("cls");
					printf("\nNumber of recieved messages: %d\n", numRxMsgs);
					printQueueSID = txHeader.sid;
					printRxMsgs();
					do {
						// Ask if user wants to delete a message
						printf("\nEnter the number of the message you want to delete, otherwise enter 0 to return to the menu\n");
						printf("\n> ");
						fflush(stdin);
						scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
						while (getchar() != '\n') {}

						// Delete message
						delMsg.msgHeader.seqNum = atoi(cmd);
						deleteMsg(listHead(), listHead()->pNext, delMsg, numRxMsgs);
					} while (atoi(cmd) != 0);

					strcpy(cmd, "15");	// Reset cmd so that program doesn't close
				}
				break;
			}

			// RECORD AUDIO MESSAGE
			case 14: {
				recordAudio();
				break;
			}

			// PLAYBACK SAVED AUDIO MESSAGE
			case 15: {
				playbackAudio();
				break;
			}

			// GENERATE RANDOM QUOTE
			case 16: {
				generateQuote();
				break;
			}

			// PRINT MESSAGES IN QUEUE
			case 17: {
				traverse(listHead(), visit);
				Sleep(4000);
				break;
			}

			// DIAGNOSTIC TRANSMIT FUNCTION
			case 18: {
				txTesting();
				break;
			}

			// DIAGNOSTIC RECEIVE FUNCTION
			case 19: {
				rxTesting();
				break;
			}

			// INVALID COMMAND
			default: {
				printf("You did not enter a valid command. Please try again.");
				Sleep(2000);
			}
		}
	} while (atoi(cmd) != 0);

	return(0);
}
