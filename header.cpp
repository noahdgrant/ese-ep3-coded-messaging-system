/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#include "CMSLibrary.h"
#include "compression.h"
#include "encryption.h"
#include "header.h"
#include "RS232Comm.h"

// Initializing header to default values (or user pereference saved values)
void initHeader(Header &header) {

	header.sid = 0;						// Should match COM port being used
	header.rid = 0;
	header.priority = 0;
	header.seqNum = 0;
	header.payloadSize = 0;
	header.payloadType = mNONE;
	header.encryption = NONE;
	header.compression = cNONE;
	header.uncompressedLength = 0;

	return;
}