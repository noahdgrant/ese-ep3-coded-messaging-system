/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#pragma once

typedef struct Header header;

struct Header {
	int sid;					// Sender ID
	int rid;					// Receiver ID
	char priority;						
	int seqNum;			 
	int payloadSize;			// Number of bytes in payload after this header
	char payloadType;			// ERR, TXT, AUD
	char encryption;			// NONE, XOR, VIG
	char compression;			// NONE, RLE, HUF
};

// Initializing header to default values
void initHeader();