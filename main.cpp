/***********************************************************
* Name:			main.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Coded Messaging System client.
************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "CMSLibrary.h"
#include "compression.h"
#include "encryption.h"
#include "error_detection_correction.h"
#include "header.h"
#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"
#include "testing.h"
#include "contactList.h"

int	main(int argc, char* argv[])
{
	// LOCAL VARIABLE DECLARATION AND INITIALIZATION
	char cmd[3] = {};								// User command
	void* msg = NULL;								// Text message to transmit
	link q = NULL;									// Pointer to start of queue
	char sendCmd = '\0';							// Holds wether the user wants to send the audio message or not
	short* audioMsg = NULL;							// Pointer to audio message buffer
	void* msgIn = NULL;								// Pointer to recieved message buffer
	Header txHeader = {};							// Header that stores tranmit information
	Header rxHeader = {};							// Header that stores recieved information
	int numRxMsgs = 0;								// The number of messages in the Rx queue
	int returnCode = 0;								// Holds return value from functions to check success
	Item delMsg;									// Message to delete from queue
	char filename[MAX_QUOTE_LENGTH] = {};			// Name of file to transmit
	int fSize = 0;									// Number of characters in file

	// START-UP PROCESSES
	srand(time(NULL));					 						// Seed the random number generator 
	initQueue();
	initHeader(txHeader);										// Set header to default values
	initContact();

	// MAIN LOOP 
	do {
		system("cls");
		printMenu(txHeader);

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
				saveUserSettings(txHeader);
				closeContacts();
				break;
				// Change Com Port
			case 1:
				selectComPort();
				break;
				// Set Compression Type
			case 2:
				setCompression(txHeader);
				break;
				// Set Encription Type
			case 3:
				setEncryption(txHeader);
				break;
				// Set Encription Code
			case 4:
				setSecretKey();
				break;
				// Change Audio Settings
			case 5:
				changeAudioSettings();
				break;
				// Set Recipient ID
			case 6:
				setRID(txHeader);
				break;
				// Set Sender ID
			case 7:
				setSID(txHeader);
				break;
				// Enable or disable error correction and deteciton
			case 8:
				setErrorDetection(txHeader);
				break;
				// Transmit Text Message
			case 9:
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
				compress(txHeader, &msg);

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
				// I THINK WE STILL NEED TO FREE THE MEMORY IF HUFFMAN IS NOT USED
				break;
				// Transmit audio message
			case 10:
				// Get memory for recording
				audioMsg = (short*)malloc(numAudioBytes * sizeof(short));
				if (audioMsg == NULL) {
					printf("\nERROR: Couldn't malloc memory to record audio.\n");
					return(-1);
				}

				// Record message
				InitializeRecording();
				RecordBuffer(audioMsg, numAudioBytes);
				CloseRecording();

				// Update header
				txHeader.payloadType = mAUD;
				/* numAudioBytes * 2 because audioMsg gets typecast to (char*) instead of short*.
				Shorts are 2 bytes each and chars are 1 byte each so to have the same amount
				of space it needs to be multiplied by 2. */
				txHeader.payloadSize = numAudioBytes * 2;

				// Transmit message
				printf("\n\nWould you like to send your audio recording? (y/n): ");
				fflush(stdin);
				scanf_s("%c", &sendCmd, 1);
				while (getchar() != '\n') {}

				if (sendCmd == 'y' || sendCmd == 'Y') {
					encrypt(txHeader, audioMsg);
					compress(txHeader, &msg);

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
				// Transmit txt file
			case 11:
				// Get file name
				printf("\nFilename to transmit (no spaces & includeing extension): ");
				fflush(stdin);
				scanf_s("%s", filename, MAX_QUOTE_LENGTH);
				while (getchar() != '\n') {}

				fSize = fileSz(filename);

				// Malloc memory for file
				msg = (char*)calloc(1, strlen(filename) + fSize + strlen("!!")); // !! is used to denote the end of the filename and the beginning of the file data
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

				compress(txHeader, &msg);

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

				// free(msg)
				Sleep(2000);
				break;
				// Recieve message
			case 12:
				// Receive message
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
				decompress(rxHeader, &msgIn);
				decrypt(rxHeader, msgIn);

				// Play audio message
				if (rxHeader.payloadType == mAUD) {
					printf("\nPlaying received recording...\n");
					InitializePlayback();
					PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			// /2 since it was *2 to send the chars but now needs to be read as shorts
					ClosePlayback();
					Sleep(500);
				}
				// Print text message
				else if (rxHeader.payloadType == mTXT) {
					printf("\nMessage Received: %s\n\n", (char*)msgIn);
					system("pause");					// Wait for user to press key before returning to main menu
				}
				// Save txt file
				else if (rxHeader.payloadType == mTXTFILE) {
					saveFile((char*)msgIn);
					Sleep(2000);
				}

				if (rxHeader.payloadType != mTXTFILE) {
					// Queue recieved message
					qRxMsg(rxHeader, msgIn, rxHeader.payloadSize);

					// Increment the counter for number of items in recieve queue
					numRxMsgs++;
				}

				free(msgIn);
				msgIn = NULL;
				break;
				// Print recieved messages
			case 13:
				delMsg.msgHeader.seqNum = 0;	// Reset the message to be deleted each time

				// Print received messages
				system("cls");
				printf("\nNumber of recieved messages: %d\n", numRxMsgs);
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
				break;
			// Record audio message
			case 14:
				recordAudio();
				break;
			// Playback saved audio message
			case 15:
				playbackAudio();
				break;
			// Generate a random quote and save it to the queue
			case 16:
				generateQuote();
				break;
			// Print out each message in the queue
			case 17:
				traverse(listHead(), visit);
				Sleep(4000);
				break;
			
			// test function send
			case 18:
				txTesting();
				break;
			// test function receive
			case 19:
				rxTesting();
				break;
			// Invalid command
			default:
				printf("You did not enter a valid command. Please try again.");
				Sleep(2000);
		}
	} while (atoi(cmd) != 0);

	return(0);
}
