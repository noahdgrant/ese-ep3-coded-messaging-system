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
	int payloadType;			// ERR, TXT, AUD
	int encryption;				// NONE, XOR, VIG
	int compression;			// NONE, RLE, HUF
	long uncompressedLength;	// Uncompressed message length
};

enum msgTypes { mERR, mNONE, mTXT, mAUD, numMsgTypes };

// Initializing header to default values
void initHeader(Header &header);
void updateHeaderSID(Header &header);
void updateHeaderRID(Header &header);
void updateHeaderCompression(Header &header);
void updateHeaderEncryption(Header &header);