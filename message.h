/* message.h -Interface: Header file for File IO for getting messages from the FortuneCookies.txt file randomly
*  By: Ian Edwards, Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

#define MAX_QUOTE_LENGTH 140

// These functions will produce the inputs to the GetMessageFromFile() function
int frandNum(int min, int max);								// Function gets a random number between min and max (max is the number of quotes in the file)
int fnumQuotes(void);										// Function returns number of quotes in the file (only need to run once)
long int* fquoteIndices(int numQuotes);						// Function returns an array that indicates the start of every quote in the file (number of characters from the start of the file) 
int* fquoteLength(int numQuotes, long int* quoteIndices);	// Function returns the smaller of the actual quote length or MAX_QUOTE_LENGTH

// Function that gets q random quote from the FortuneCookies file 
int GetMessageFromFile(char* buff, int randNum, long int* quoteIndices, int* quoteLengths);  // buff is a character array (need top pass the node element that is a character array) 

// Generate a random quote from FortuneCookies.txt file, and ask the user if they want to add it to the queue
int generateQuote();