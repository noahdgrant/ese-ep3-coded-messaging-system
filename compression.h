/***********************************************************
* Name:			compression.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Compression/decompression interface.
************************************************************/

#pragma once

#include "header.h"

/***********************************************************
* Specific variables
************************************************************/

enum compTypes { cNONE, cRLE, cHUF, cBOTH, numCompTypes };		// Compression types

/***********************************************************
* Function prototypes
************************************************************/

unsigned int compress(void** in, unsigned int iBufSize, int type);
int decompress(void** in, unsigned int iBufSize, unsigned int oBufSize, int type);

void setCompression(Header& h);				// Set the type of compression to be used on transmitted message