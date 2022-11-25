/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#include "CMSLibrary.h"
#include "compression.h"
#include "encryption.h"
#include "header.h"
#include "RS232Comm.h"

extern encTypes encType;
extern int sid;
extern int rid;

extern msgTypes msgType;
extern compTypes compType;

// Initializing header to default values (or user pereference saved values)
void initHeader(Header &header) {

	header.sid = sid;						// Should match COM port being used
	header.rid = rid;
	header.priority = 0;
	header.seqNum = 0;
	header.payloadSize = 0;
	header.payloadType = msgType;
	header.encryption = encType;
	header.compression = compType;
	header.uncompressedLength = 0;

	return;
}

void updateHeaderSID(Header &header) {
	header.sid = sid;
}

void updateHeaderRID(Header &header) {
	header.rid = rid;
}

void updateHeaderCompression(Header &header) {
	header.compression = compType;
}

void updateHeaderEncryption(Header &header) {
	header.encryption = encType;
}