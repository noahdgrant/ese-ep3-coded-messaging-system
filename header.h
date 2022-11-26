/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#pragma once

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

// Initializing header to default values
void initHeader(Header &header);