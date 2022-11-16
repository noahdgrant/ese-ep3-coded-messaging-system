/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#include "CMSLibrary.h"
//#include "compression.h"
#include "encryption.h"
#include "header.h"
#include "RS232Comm.h"

// Initializing header to default values
void initHeader() {
	header txHeader = {};

	txHeader.sid = 1;
	txHeader.rid = 2;
	txHeader.priority = '\0';
	txHeader.seqNum = 0;
	txHeader.payloadSize = 0;
	txHeader.payloadType = ERR;
	txHeader.encryption = NONE;
	txHeader.compression = NONE;

	return;
}