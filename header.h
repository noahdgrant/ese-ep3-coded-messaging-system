/***********************************************************
* Name:			header.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Message header interface.
************************************************************/

#pragma once

/***********************************************************
* Specific variables
************************************************************/

typedef struct header Header;

struct header {
	int sid;					// Sender ID
	int rid;					// Receiver ID
	int priority;						
	int seqNum;			 
	long payloadSize;			// Number of bytes in payload after this header
	int payloadType;			// mERR, mNONE, mTXT, mAUD, numMsgTypes
	int encryption;				// ERR, NONE, XOR, VIG, numOfEnc
	int compression;			// cERR, cNONE, cHUF, cRLE, numCompTypes
	long uncompressedLength;	// Uncompressed message length
};

/***********************************************************
* Function prototypes
************************************************************/

void initHeader(Header &header);			// Initialize header and load user settings

void saveUserSettings(Header &header);		// Save user setting on program exit