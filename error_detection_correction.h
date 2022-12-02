/***********************************************************
* Name:			error_detection_correction.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Error correction and detection interface.
************************************************************/

#pragma once
#include "header.h"

/***********************************************************
* Specific Variables
************************************************************/

enum CHECKSUM
{
    CHK_16BIT,
    CHK_8BIT,
    CHK_ERROR
};

/***********************************************************
* Function prototypes
************************************************************/

unsigned int Checksum(void* buf, int iBufLen, CHECKSUM iType);

int checkHeader(Header& h);