/***********************************************************
* Name:			message.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Reading fortune cookies file and generating random quote implementation.
************************************************************/

// Using unix encoding \n chars. They are 2 chars not 1: \n\r

#define _CRT_SECURE_NO_DEPRECATE // Need this so that fopen() works

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "Queues.h"

/*************************************************************************
*                            PRIVATE FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* frandNum() - Gets a random number between min and max (max is the number of quotes in the file).
* min		- Minimum random number.
* max		- Maximum random number.
* This function returns a randomly generated number.
*************************************************************************/
static int frandNum(int min, int max) {
	return (rand() % (max - min) + min);					// From w2 slides. Returns rand num between min and max
}

/*************************************************************************
* fnumQuotes() - Calculates the number of quotes in the fortune cookies file.
* This function returns the number of quotes in the fortune cookies file.
*************************************************************************/
static int fnumQuotes(void) {
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
	return (numQuotes - 1);												// -1 because the end of the document is %% so it thinks there is one more quote than there actually is
}

/*************************************************************************
* fquoteIndices() - Calculates the number of quotes in the fortune cookies file.
* numQuotes		- The number of quotes in the fortune cookies file.
* This function returns an array that indicates the start of every quote in the file (number of characters from the start of the file).
*************************************************************************/
static long int* fquoteIndices(int numQuotes) {
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

/*************************************************************************
* fquoteLength() - Calculates the number of quotes in the fortune cookies file.
* numQuotes			- The number of quotes in the fortune cookies file.
* quoteIndicies		- An array that indicates the start of every quote in the fortune cookies file (number of characters from the start of the file).
* This function returns an array of the smaller of the actual quote length or MAX_QUOTE_LENGTH.
*************************************************************************/
static int* fquoteLength(int numQuotes, long int* quoteIndices) {
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
}

/*************************************************************************
* GetMessageFromFile() - Gets a random quote from the fortunecookies.txt file.
* buff				- Buffer to store the random quote.
* randNum			- Randomly generated number.
* quoteIndicies		- An array that indicates the start of every quote in the fortune cookies file (number of characters from the start of the file).
* quoteLengths		- AN array that contains all the quote lengths.
* This function return 0 if it successful got a quote from the fortunecookies.txt file. Otherwise, it return -1.
*************************************************************************/
static int GetMessageFromFile(char* buff, int randNum, long int* quoteIndices, int* quoteLengths) {
	FILE* fp = NULL;
	char c;												// Hold the last char of quote to check for \n

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

	// Remove the last char of the quote if it is a part of the unix newline character
	c = buff[quoteLengths[randNum] - 1];				// Last char before \0
	if ((c == '\n') || (c == '\r')) {
		buff[quoteLengths[randNum] - 1] = '\0';
	}

	fclose(fp);
	return 0;
} // GetMessageFromFile()

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* generateQuote() - Generate a random quote from FortuneCookies.txt file, and ask the user if they want to add it to the queue.
* This function return 0 if it successful got a quote from the fortunecookies.txt file. Otherwise, it return -1.
*************************************************************************/
int generateQuote() {
	char buff[MAX_QUOTE_LENGTH] = {};							// Buffer that holds the quote from the file
	int numQuotes = 0;											// Number of quotes in the file
	link p = NULL;												// Pointers for pushing and poping the queue
	long int* quoteIndices = NULL;								// Array of quote locations in the file (index correspondes to quote number)
	int* quoteLengths = NULL;									// Array of quote lengths (index correspondes to quote number)
	int result;													// Holds return value from generateQuote()
	char cmd;													// User command

	numQuotes = fnumQuotes();									// Number of quotes
	quoteIndices = fquoteIndices(numQuotes);					// Index locations of the quotes
	quoteLengths = fquoteLength(numQuotes, quoteIndices);		// Length of each quote (up to MAX_QUOTE_LENGTH) - cut off after 

	// Get the random message from the file
	result = GetMessageFromFile(buff, frandNum(0, numQuotes), quoteIndices, quoteLengths);
	if (result != 0) {
		printf("\nERROR: Did not get message from file.\n");
		return(-1);
	}

	printf("\nQUOTE:\n");
	printf("%s\n", buff);

	fflush(stdin);
	printf("\nDo you want to save the quote to the queue? (y/n) ");
	scanf_s("%c", &cmd, 1);
	while (getchar() != '\n') {}											// Flush other input
	if (cmd == 'y' || cmd == 'Y') {
		int z = strlen(buff);
		int y = sizeof(char);
		p = (link)calloc(1, sizeof(Node) + strlen(buff));
		if (p == NULL) {														// Make sure memory was allocated
			return (-1);
		}

		for (int i = 0; i < strlen(buff); i++) {
			p->Data.message[i] = buff[i];
		}
		pushQ(p);
	}

	// Free heap memory
	free(quoteLengths);
	quoteLengths = NULL;
	free(quoteIndices);
	quoteIndices = NULL;

	return(0);
}