/***********************************************************
* Name:			compression.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Compression/decompression implementation.
************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "compression.h"
#include "header.h"
#include "huffman.h"
#include "message.h"
#include "RLE.h"
#include "RS232Comm.h"

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* compress() - Compress transmit buffer.
* h		- Transmit header.
* msg	- Message to be transmitted.
* This function returns 0 if compression was successful (not not needed) or -1 if it failed.
*************************************************************************/
int compress(Header& h, void** msg) {
	if (h.compression == cNONE) return(0);		// Exit function if no compression needed.

	char* compressedBuf = NULL;
	int compSz = 0;

	if (h.payloadType == mTXT || h.payloadType == mTXTFILE) {
		if (h.compression == cHUF) {
			compressedBuf = (char*)malloc((h.uncompressedLength * 104 + 50) / 100 + 384);				// The equation came from Michael's version and is based of the Huffman library documentation
			if (compressedBuf == NULL) {
				printf("\nERROR: Coudn't malloc memory to compress message.\n");
				return(-1);
			}

			compSz = Huffman_Compress((unsigned char*)*msg, (unsigned char*)compressedBuf, h.uncompressedLength);

			// Need to realloc since the uncompressed message will be larger than the comrpessed message.
			void* tmp = realloc(*msg, compSz);
			if (tmp == NULL) {
				printf("\nERROR: Could not realloc memory to compress message buffer.\n");
				return(-1);
			}
			*msg = tmp;
			strcpy((char*)*msg, compressedBuf);
			h.payloadSize = compSz;	// Update payload size so tx and rx functions know the correct buffer size
		}
		else if (h.compression == cRLE) {
			char buf[MAX_QUOTE_LENGTH];
			RLEncode((char*)*msg, strlen((const char*)*msg), buf, MAX_QUOTE_LENGTH, ESCAPE_CHARACTER);
			strcpy((char*)*msg, buf);
			h.payloadSize = strlen(buf) + 1;
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

	//if (compressedBuf != NULL) {		
	//	free(compressedBuf);			// This is giving me problems even though I should be able to free this memory
	//	compressedBuf = NULL;
	//}
	return(0);
}

/*************************************************************************
* decompress() - Decompress received buffer.
* h		- Received header.
* msg	- Received message.
* This function returns 0 if decompression was successful (or not needed) or -1 if it failed.
*************************************************************************/
int decompress(Header& h, void** msg) {
	if (h.compression == cNONE) return(0);		// Exit function if no compression needed.

	char* uncompressedBuf = NULL;

	if (h.payloadType == mTXT) {
		if (h.compression == cHUF) {
			uncompressedBuf = (char*)malloc(h.uncompressedLength);
			if (uncompressedBuf == NULL) {
				printf("\nERROR: Coudn't malloc memory to decompressed received message.\n");
				return(-1);
			}

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
			h.payloadSize = strlen(uncompressedBuf); //THIS IS NOT RETURNING THE CORRECT TRANSMITTED UNCOMPRESSED LENGTH ALL THE TIME
		}
		else if (h.compression == cRLE) {
			char buf[MAX_QUOTE_LENGTH];
			RLDecode((char*)*msg, strlen((const char*)*msg), buf, MAX_QUOTE_LENGTH, ESCAPE_CHARACTER);
			strcpy((char*)*msg, buf);
			h.payloadSize = strlen(buf);
		}
	}
	else if (h.payloadType == mAUD) {
		if (h.compression == cHUF) {
			/*short* buf = (short*)malloc((strlen((const char*)msg) + 384) * sizeof(short));
			Huffman_Uncompress((unsigned char*)msg, (unsigned char*)buf, strlen((const char*)msg) * 2, 1);
			strcpy((char*)msg, (char*)buf);
			free(buf);*/
		}
		else if (h.compression == cRLE) {
		}
	}

	if (uncompressedBuf != NULL) {
		free(uncompressedBuf);
		uncompressedBuf = NULL;
	}
	return(0);
}

/*************************************************************************
* setCompress() - Set the type of compression used on the transmitted message..
* h		- Transmit header.
*************************************************************************/
void setCompression(Header& h) {
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
			h.compression = cNONE;
		}
		else if (atoi(cmd) == cHUF) {
			printf("\nNow using Huffman compression\n");
			h.compression = cHUF;
		}
		else if (atoi(cmd) == cRLE) {
			printf("\nNow using RLE compression\n");
			h.compression = cRLE;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < cNONE || atoi(cmd) > numCompTypes);
}