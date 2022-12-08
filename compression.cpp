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

#include "compression.h"
#include "header.h"
#include "huffman.h"
#include "rle.h"

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* compress() - Compress transmit buffer.
* in		- Input (uncompressed) buffer.
* iBufSize	- Input buffer size.
* type		- The type of compression to use on the message.
* This function returns the size of the compressed buffer, or -1 if it failed.
*************************************************************************/
unsigned int compress(void** in, unsigned int iBufSize, int type) {
	unsigned char* out = NULL;
	unsigned char* out2 = NULL;
	unsigned int oBufSize = 0;

	switch (type) {
		// No compression
		case 0:
			return(iBufSize);
		// RLE
		case 1: {
			out = (unsigned char*)malloc(iBufSize * (257 / 256) + 1);
			if (out == NULL) return(-1);
			oBufSize = RLE_Compress((unsigned char*)*in, out, iBufSize);
			out[oBufSize - 1] = '\0';
			free(*in);
			*in = out;
			break;
		}
		// Huffman
		case 2: {
			out = (unsigned char*)malloc(iBufSize * (101 / 100) + 384);
			if (out == NULL) return(-1);
			oBufSize = Huffman_Compress((unsigned char*)*in, out, iBufSize);			// Huffman_Compress() has a bug where it sometimes returns the wrong size and the last character is lost
			out[oBufSize - 1] = '\0';
			free(*in);
			*in = out;
			break;
		}
		// Both
		/* Won't work until we find a good day to tell Huffman on the receive side how big the compressed RLE
		buffer should be after Huffman is uncompressed */
		case 3: {
			// RLE first
			out = (unsigned char*)malloc(iBufSize * (257 / 256) + 1);
			if (out == NULL) return(-1);
			oBufSize = RLE_Compress((unsigned char*)*in, out, iBufSize);
			out[oBufSize - 1] = '\0';

			// Huffman second
			out2 = (unsigned char*)malloc(iBufSize * (101 / 100) + 384);
			if (out2 == NULL) return(-1);
			oBufSize = Huffman_Compress(out, out2, oBufSize);
			out2[oBufSize - 1] = '\0';
			free(out);
			out = NULL;
			free(*in);
			*in = out2;
			break;
		}
		default: {
			return(-1);
		}
	}

	return(oBufSize);
}

/*************************************************************************
* decompress() - Decompress received buffer.
* in		- Input (compressed) buffer.
* iBufSize	- Size of input buffer.
* oBufSize	- Size of output (uncompressed) buffer.
* This function returns 0 if decompression was successful (or not needed), or -1 if it failed.
*************************************************************************/
int decompress(void** in, unsigned int iBufSize, unsigned int oBufSize, int type) {
	unsigned char* out = NULL;
	unsigned char* out2 = NULL;

	switch (type) {
		// No compression
		case 0:
			return(0);
		// RLE
		case 1: {
			out = (unsigned char*)malloc(oBufSize);
			if (out == NULL) return(-1);
			RLE_Uncompress((unsigned char*)*in, out, iBufSize);
			out[oBufSize - 1] = '\0';
			free(*in);
			*in = out;
			break;
		}
		// Huffman
		case 2: {
			out = (unsigned char*)malloc(oBufSize);
			if (out == NULL) return(-1);
			Huffman_Uncompress((unsigned char*)*in, out, iBufSize, oBufSize);
			out[oBufSize - 1] = '\0';
			free(*in);
			*in = out;
			break;
		}
		// Both
		case 3: {
			// Huffman first
			out = (unsigned char*)calloc(1, oBufSize);
			if (out == NULL) return(-1);
			/* oBufSize can't be the last parameter in Huffman_Uncompress since the message still needs to be decompressed by RLE.
			Need to find a way to get the right value. 67 is a magic number right now for the current string.
			We could pass the parameter to the function or store it in the header. I don't have any better
			ideas right now.*/
			Huffman_Uncompress((unsigned char*)*in, out, iBufSize, 67);
			out[strlen((char*)out)] = '\0';

			// RLE second
			out2 = (unsigned char*)calloc(1, oBufSize);
			if (out2 == NULL) return(-1);
			RLE_Uncompress(out, out2, strlen((char*)out) + 1);
			out2[oBufSize - 1] = '\0';
			free(out);
			out = NULL;
			free(*in);
			*in = out2;
			break;
		}
		default: {
			return(-1);
		}
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
		printf("2. RLE\n");
		printf("3. Huffman\n");
		printf("4. Huffman & RLE (currently not working)\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd)-1 == cNONE) {
			printf("\nNow using no compression\n");
			h.compression = cNONE;
		}
		else if (atoi(cmd)-1 == cRLE) {
			printf("\nNow using RLE compression\n");
			h.compression = cRLE;
		}
		else if (atoi(cmd)-1 == cHUF) {
			printf("\nNow using Huffman compression\n");
			h.compression = cHUF;
		}
		else if (atoi(cmd)-1 == cBOTH) {
			printf("\nNow using RLE compression\n");
			h.compression = cBOTH;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd)-1 < cNONE || atoi(cmd)-1 > numCompTypes);
}