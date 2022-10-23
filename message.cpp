/* message.cpp -Implementation: Functions for File IO - Getting random messages from a file
*  By: Ian Edwards, Noah Grant, Wyatt Richard
*  Version: 01.00
*/

// Using unix encoding \n chars. They are 2 chars not 1: \n\r

#define _CRT_SECURE_NO_DEPRECATE // Need this so that fopen() works

#include <malloc.h>			// Dynamic memory allocation for arrays that store quote location and length
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "Queues.h"

// Function gets a random number between min and max (max is the number of quotes in the file)
int frandNum(int min, int max) {
	return (rand() % (max - min) + min);	// From w2 slides. Returns rand num between min and max
} // frandNum()

// Function returns number of quotes in the file (only need to run once)
int fnumQuotes(void) {
	FILE* fp = NULL;
	int numQuotes = 0;										// Number of quotes in file

	fp = fopen("FortuneCookies.txt", "r");
	if (fp == NULL) {										// Make sure the filename is correct
		printf("\nERROR: Could not open file.\n");
		return (-1);
	}

	while (!feof(fp)) {
		if (fgetc(fp) == '%' && fgetc(fp) == '%') {			// Check current char and next char
			numQuotes++;
		}
	}

	fclose(fp);
	return (numQuotes - 1);									// -1 because the end of the document is %% so it thinks there is one more quote than there actually is
} // fnumQuotes()

// Function returns an array that indicates the start of every quote in the file (number of characters from the start of the file) 
long int* fquoteIndices(int numQuotes) {
	FILE* fp = NULL;
	int i = 0;															// Current index of qStarts array
	long int* qStarts = NULL;											// Array to hold the starting indicies of the quotes
	int c = 0;

	qStarts = (long int*)malloc((numQuotes + 1) * sizeof(long int));	// numQuotes + 1 so that there is a space in the array for the last %% at the end of the file.
	if (qStarts == NULL) {												// Make sure memory was allocated
		return (long int*)(-1);
	}

	fp = fopen("FortuneCookies.txt", "r");
	if (fp == NULL) {													// Make sure the filename is correct
		printf("\nERROR: Could not open file.\n");
		return (long int*)(-1);
	}

	while (!feof(fp)) {
		if (fgetc(fp) == '%' && fgetc(fp) == '%') {						// Check current char and next char
			qStarts[i++] = ftell(fp) + strlen("\n\r");					// Move fp from %%_\n_\rX to %%\n_X_ -Where X is the first char of the quote
		}
	}

	fclose(fp);
	return qStarts;
} // fquoteIndices()

// Function returns the smaller of the actual quote length or MAX_QUOTE_LENGTH
int* fquoteLength(int numQuotes, long int* quoteIndices) {
	int len = 0;																	// Length of current quote
	int* qLens = NULL;																// Array with lengths of quotes

	qLens = (int*)malloc(numQuotes * sizeof(int));
	if (qLens == NULL) {															// Make sure memory was allocated
		return (int*)(-1);
	}

	for (int i = 0; i < numQuotes; i++) {
		len = quoteIndices[i + 1] - quoteIndices[i] - strlen("\n\r%%\n\rX");		// Have to subtract the chars that are between the quotes. X is the first char of the next quote

		if (len >= MAX_QUOTE_LENGTH) {
			len = MAX_QUOTE_LENGTH - 1;												// Need one char for '\0'
		}
		qLens[i] = len;																// Store length at current index
	}

	return (qLens);
} // fquoteLength()

// Function that gets q random quote from the FortuneCookies file 
int GetMessageFromFile(char* buff, int randNum, long int* quoteIndices, int* quoteLengths) {
	FILE* fp = NULL;

	for (int i = 0; i < MAX_QUOTE_LENGTH; i++) {		// Fill the buffer with empty chars
		buff[i] = '\0';
	}

	fp = fopen("FortuneCookies.txt", "r");
	if (fp == NULL) {									// Make sure the filename is correct
		printf("\nERROR: Could not open file.\n");
		return (-1);
	}

	fseek(fp, quoteIndices[randNum], SEEK_SET);			// Go to the starting index of the random quote
	fread(buff, quoteLengths[randNum], 1, fp);			// Copy the random quote

	fclose(fp);
	return 0;
} // GetMessageFromFile()