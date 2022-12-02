/***********************************************************
* Name:			header.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Message header implementation.
************************************************************/

#include <stdio.h>

#include "CMSLibrary.h"
#include "compression.h"
#include "encryption.h"
#include "header.h"
#include "RS232Comm.h"
#include "sound.h"

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* initHeader() - Initialize tranmit header values to default or saved user pereferences.
* header	- Transmit header.
*************************************************************************/
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
    header.checksum = 0;
    header.errorDC = false;

    FILE* f;
    errno_t err;
    err = fopen_s(&f, "usersettings.txt", "rb");
    if(f) {
        char cSid[11], cRid[11], cEncryption[3], cCompression[3], cCurrentCom[3], cRecordTime[4];
        fgets(cSid, sizeof(cSid), f);
        fgets(cRid, sizeof(cRid), f);
        fgets(cEncryption, sizeof(cEncryption), f);
        fgets(cCompression, sizeof(cCompression), f);
        fgets(cCurrentCom, sizeof(cCurrentCom), f);
        fgets(secretKey, sizeof(secretKey), f);
        fgets(cRecordTime, sizeof(cRecordTime), f);
        
        
        if (secretKey[strlen(secretKey) - 1] == '\n') {
            secretKey[strlen(secretKey) - 1] = 0;
        }
        if (!strcmp(secretKey, "")) {
            strcpy_s(secretKey, "default");
        }

        if (!atoi(cCompression)) {
            header.compression = cNONE;
        }
        else {
            header.compression = atoi(cCompression);
        }

        if (!atoi(cEncryption)) {
            header.encryption = NONE;
        }
        else {
            header.encryption = atoi(cCompression);
        }

        fclose(f);
        header.sid = atoi(cSid);
        header.rid = atoi(cRid);
        currentCom = atoi(cCurrentCom);
        recordTime = atoi(cRecordTime);
    }

	return;
}

/*************************************************************************
* saveUserSettings() - Saves the current users setting when the program exits.
* header	- Transmit header.
*************************************************************************/
void saveUserSettings(Header& header) {
    FILE* f;
    errno_t err;
    err = fopen_s(&f, "usersettings.txt", "wb");
    if(f) {
        fprintf(f, "%d\n%d\n%d\n%d\n%d\n%s\n%d", header.sid, header.rid, header.encryption, header.compression, currentCom, secretKey, recordTime);
        fclose(f);
    }
}