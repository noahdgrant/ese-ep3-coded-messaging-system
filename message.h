/***********************************************************
* Name:			message.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Reading fortune cookies file and generating random quote interface.
************************************************************/

#pragma once

/***********************************************************
* Specific variables
************************************************************/

#define MAX_QUOTE_LENGTH 140

/***********************************************************
* Function prototypes
************************************************************/

int generateQuote();		// Generate a random quote from FortuneCookies.txt file, and ask the user if they want to add it to the queue