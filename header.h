/***********************************************************
* Name:			header.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Message header interface.
************************************************************/

#pragma once

#include <Windows.h>

/***********************************************************
* Specific variables
************************************************************/

typedef struct header Header;

struct header {
	BYTE sid;					// Sender ID
	BYTE rid;					// Receiver ID
	BYTE rid2;					// Used for voteOn() error detection and correction
	BYTE rid3;					// Used for voteOn() error detection and correction
	BYTE priority;				// Message's pirority
	int seqNum;					// Message's position in the queue
	long payloadSize;			// Number of bytes in payload after this header
	BYTE payloadType;			// mERR, mNONE, mTXT, mAUD, numMsgTypes
	BYTE encryption;				// ERR, NONE, XOR, VIG, numOfEnc
	BYTE compression;			// cERR, cNONE, cHUF, cRLE, numCompTypes
	long uncompressedLength;	// Uncompressed message length
	int checksum;				// Checksum for payload error detection
	bool errorDC;				// Enable or disable error detection and correciton
};

/***********************************************************
* Function prototypes
************************************************************/

void initHeader(Header &header);			// Initialize header and load user settings

void saveUserSettings(Header &header);		// Save user setting on program exit