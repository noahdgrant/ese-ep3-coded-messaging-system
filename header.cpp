/* header.h - Define header portion of frame
* By: Noah Grant, Wyatt Richard
* Version: 1.00
*/

#include <stdio.h>
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


    FILE* f;
    errno_t err;
    err = fopen_s(&f, "usersettings.txt", "rb");
    if(f) {
        char cSid[11], cRid[11], cEncryption[3], cCompression[3], cCurrentCom[3];
        fgets(cSid, sizeof(cSid), f);
        fgets(cRid, sizeof(cRid), f);
        fgets(cEncryption, sizeof(cEncryption), f);
        fgets(cCompression, sizeof(cCompression), f);
        fgets(cCurrentCom, sizeof(cCurrentCom), f);
        fgets(secretKey, sizeof(secretKey), f);
        fclose(f);
        header.sid = atoi(cSid);
        header.rid = atoi(cRid);
        header.encryption = atoi(cEncryption);
        header.compression = atoi(cCompression);
        currentCom = atoi(cCurrentCom);

    }

	return;
}

void saveUserSettings(Header& header) {
    FILE* f;
    errno_t err;
    err = fopen_s(&f, "usersettings.txt", "wb");
    if(f) {
        fprintf(f, "%d\n%d\n%d\n%d\n%d\n%s", header.sid, header.rid, header.encryption, header.compression, currentCom, secretKey);
        fclose(f);
    }
}