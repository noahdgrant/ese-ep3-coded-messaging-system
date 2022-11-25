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

#include "CMSLibrary.h"
#include "encryption.h"
#include "header.h"
#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"
#include "huffman.h"
#include "RLE.h"

int rid = 0;													// Default receiver ID
int sid = 1;													// Default sender ID
char secretKey[MAX_QUOTE_LENGTH] = {};							// Key used to encrypt/decrypt messages
enum encTypes encType = NONE;									// Default encryption is NONE
enum compTypes compType = cNONE;								// Default compression is NONE

int recordTime = 2;												// Default record time
long numAudioBytes = SAMPLES_SEC * recordTime;					// Size of audio buffer

extern int currentCom;

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
	printf("12. Set Recipient ID					RID:			%d\n", rid);
	printf("13. Set Sender ID					SID:			%d\n", sid);
	printf("14. Set Compression Type				Compression Type:	");
	switch (compType) {
	case cNONE:
		printf("None\n");
		break;
	case cHUF:
		printf("Huffman\n");
		break;
	case cRLE:
		printf("RLE\n");
		break;
	}
	printf("15. Print recieved messages\n");
	printf("0. Exit\n");
	printf("\n> ");
	return;
} // printMenu()

// GUI OPTIONS
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

		cmd[2] = '\0';
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
	scanf_s("%d", &rid);
}

// Set the Sender ID
void setSID() {
	printf("\nEnter the sender ID: ");
	scanf_s("%d", &sid);
}

// Set encryption Type 
void setEncryption() {
	char cmd[2] = {};		// Holds the user's encryption choice
	do {
		system("cls");
		printf("Enter type of encryption/decryption\n");
		printf("1. No Encryption\n");
		printf("2. XOR\n");
		printf("3. Viginere\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) == NONE) {
			printf("\nNow using no encryption\n");
			encType = NONE;
		}
		else if (atoi(cmd) == XOR) {
			printf("\nNow using XOR encryption\n");
			encType = XOR;
		}
		else if (atoi(cmd) == VIG) {
			printf("\nNow using Viginere encryption\n");
			encType = VIG;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < NONE || atoi(cmd) > numOfEnc);
}

// set the XOR code
void setSecretKey() {
	printf("\nEnter encryption key: ");
	scanf_s("%s", secretKey, MAX_QUOTE_LENGTH - 1);
}

// decrypt message
void decrypt(Header h, void* msg) {
	// Decrypt the message (xor)
	if (h.encryption == XOR) {
		xorCipher(msg, h.payloadSize, secretKey, strlen(secretKey));
	}
	// Decrypt the message (Viginere)
	else if (h.encryption == VIG) {
		vigCipher(msg, h.payloadSize, secretKey, strlen(secretKey), false);
	}

	return;
}

// encrypt message
void encrypt(void* msg, int msgSz) {
	// XOR Encryption
	if (encType == XOR) {
		xorCipher(msg, msgSz, secretKey, strlen(secretKey));
	}
	// Viginere encryption
	else if (encType == VIG) {
		vigCipher(msg, msgSz, secretKey, strlen(secretKey), true);
	}

	return;
}

int compress(Header &h, void** msg) {
	char* compressedBuf = NULL;

	if (h.payloadType == mTXT) {
		if (h.compression == cHUF) {
			compressedBuf = (char*)malloc(h.uncompressedLength + 384);
			if (compressedBuf == NULL) {
				printf("\nERROR: Coudn't malloc memory to compress message.\n");
				return(-1);
			}

			Huffman_Compress((unsigned char*)*msg, (unsigned char*)compressedBuf, h.uncompressedLength);

			// Need to realloc since the uncompressed message will be larger than the comrpessed message.
			void* tmp = realloc(*msg, strlen(compressedBuf));
			if (tmp == NULL) {
				printf("\nERROR: Could not realloc memory to compress message buffer.\n");
				return(-1);
			}
			*msg = tmp;
			strcpy((char*)*msg, compressedBuf);
			h.payloadSize = strlen(compressedBuf);	// Update payload size so tx and rx functions know the correct buffer size
		}
		else if (h.compression == cRLE) {
			char buf[MAX_QUOTE_LENGTH];
			RLEncode((char*)*msg, strlen((const char*)*msg), buf, MAX_QUOTE_LENGTH, ESCAPE_CHARACTER);
			strcpy((char*)*msg, buf);
		}
	}
	else if (h.payloadType == mAUD) {
		if (h.compression == cHUF) {
			/*short* buf = (short*)malloc((strlen((const char*)msg) + 384) * sizeof(short));
			Huffman_Compress((unsigned char*)msg, (unsigned char*)buf, strlen((const char*)msg) * 2);
			strcpy((char*)msg, (char*)buf);
			free(buf);*/
		}
		else if (h.compression == cRLE) {
		}
	}

	if (compressedBuf != NULL) free(compressedBuf);
	compressedBuf = NULL;
	return(0);
}

int decompress(Header &h, void** msg) {
	char* uncompressedBuf = (char*)malloc(h.uncompressedLength);
	if (uncompressedBuf == NULL) {
		printf("\nERROR: Coudn't malloc memory to decompressed received message.\n");
		return(-1);
	}

	if (h.payloadType == mTXT) {
		if (h.compression == cHUF) {
			Huffman_Uncompress((unsigned char*)*msg, (unsigned char*)uncompressedBuf, h.payloadSize, h.uncompressedLength);
			if (uncompressedBuf[0] == '\0') {
				printf("\nERROR: Failed to decompress received message.\n");
				return(-1);
			}

			// Need to realloc since the uncompressed message will be larger than the comrpessed message.
			void* tmp = realloc(*msg, h.uncompressedLength);
			if (tmp == NULL) {
				printf("\nERROR: Could not realloc memory to decompress message buffer.\n");
				return(-1);
			}
			*msg = tmp;
			strcpy((char*)*msg, uncompressedBuf);
			h.payloadSize = strlen(uncompressedBuf);
		}
		else if (h.compression == cRLE) {
			char buf[MAX_QUOTE_LENGTH];
			RLDecode((char*)msg, strlen((const char*)msg), buf, MAX_QUOTE_LENGTH, ESCAPE_CHARACTER);
			strcpy((char*)msg, buf);
		}
	}
	if (h.payloadType == mAUD) {
		if (h.compression == cHUF) {
			/*short* buf = (short*)malloc((strlen((const char*)msg) + 384) * sizeof(short));
			Huffman_Uncompress((unsigned char*)msg, (unsigned char*)buf, strlen((const char*)msg) * 2, 1);
			strcpy((char*)msg, (char*)buf);
			free(buf);*/
		}
		else if (h.compression == cRLE) {
		}
	}

	free(uncompressedBuf); 
	return(0);
}
void setCompression() {
	char cmd[2] = {};		// Holds the user's encryption choice
	do {
		system("cls");
		printf("Enter type of compression/decompression\n");
		printf("1. No compression\n");
		printf("2. Huffman\n");
		printf("3. RLE\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) == cNONE) {
			printf("\nNow using no compression\n");
			compType = cNONE;
		}
		else if (atoi(cmd) == cHUF) {
			printf("\nNow using Huffman compression\n");
			compType = cHUF;
		}
		else if (atoi(cmd) == cRLE) {
			printf("\nNow using RLE compression\n");
			compType = cRLE;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < cNONE || atoi(cmd) > numCompTypes);
}