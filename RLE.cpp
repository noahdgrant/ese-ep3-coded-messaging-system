/***********************************************************
* Name:			RLE.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	RLE compression/decompression implementation.
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RLE.h"

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* RLEncode() - Compresses input buffer using RLE compression.
* in		- Input (uncompressed) buffer.
* inLen		- Length of input buffer.
* out		- Output (compressed) buffer.
* outMax	- Maximum possible length of compressed buffer
* cEsc		- Character to put before compressed data.
* This function returns the length of the compressed buffer.
*************************************************************************/
int RLEncode(char* in, int inLen, char* out, int outMax, char cEsc) {
	char charac; // repeated character
	char output[5]; // temp buffer with room for RLE sequence
	out[0] = '\0'; // Set so can use strcat()
	int i = 0, count;
	while (i < inLen) {
		count = 1;
		while ((in[i] == in[i + 1]) && (i < inLen) && count <= 255) {
			charac = in[i]; // repeated character
			count++;
			i++;
		}
		if (count >= 3) {
			sprintf_s(output,5, "%c%02x%c", cEsc, count, charac);
			strcat_s(out, outMax, output);
		}
		else if (count == 2) {
			sprintf_s(output, 3, "%c%c", in[i],in[i]);
			strcat_s(out, outMax, output);
		}
		else if (count == 1) {
			sprintf_s(output, 2, "%c", in[i]);
			strcat_s(out, outMax, output);
		}
		i++; // Advance to next character
	}
	return(strlen(out)); // Returns length of output
}

/*************************************************************************
* RLDecode() - Decompress input buffer using RLE compression.
* in		- Input (uncompressed) buffer.
* inLen		- Length of input buffer.
* out		- Output (compressed) buffer.
* outMax	- Maximum possible length of compressed buffer
* cEsc		- Character to put before compressed data.
* This function returns the length of the decompressed buffer.
*************************************************************************/
int RLDecode(char* in, int inLen, char* out, int outMax, char cEsc) {
	int i = 0, j = 0, repeats = 0;
	char HEX[3];							// String rep of HEX number of repeats
	char charac[2];							// repeated character
	out[0] = '\0';							// To use strcat()
	for (i = 0; i < inLen; i++) {			// Build the out buffer using in buffer
		if (in[i] != cEsc) {				// No RLE sequence (no repeats)
			charac[0] = in[i];
			charac[1] = '\0';
			strcat_s(out, outMax, charac);
		}
		else {								// RLE sequence (repeats encountered)
			HEX[0] = in[++i];
			HEX[1] = in[++i];
			HEX[2] = '\0';					// repeats string (e.g. x12 is HEX[1]=‘1’HEX[2]=’2’

			charac[0] = in[++i];
			charac[1] = '\0';				// repeated char

			repeats = (int)strtol(HEX, NULL, 16); // convert repeats (HEX string) to a number
			for (j = 0; j < repeats; j++) strcat_s(out, outMax, charac); //strcat(out, charac);
		}
	}
	if (strlen(out) > outMax) { printf("OVERFLOW"); return 0; }
	return(strlen(out)); // Return length of output buffer on success
}
