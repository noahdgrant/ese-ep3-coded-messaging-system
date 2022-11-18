/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#include "CMSLibrary.h"
//#include "compression.h"
#include "encryption.h"
#include "header.h"
#include "RS232Comm.h"

extern encTypes encType;
extern int currentCom;

msgTypes msgType = mNONE;
compTypes compType = cNONE;

// Initializing header to default values (or user pereference saved values)
void initHeader(Header &header) {

	header.sid = currentCom;						// Should match COM port being used
	header.rid = 0;
	header.priority = 0;
	header.seqNum = 0;
	header.payloadSize = 0;
	header.payloadType = msgType;
	header.encryption = encType;
	header.compression = compType;

	return;
}

// Need to update header.sid when comport is updated
// update encryption
// update compression