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

enum compTypes { cERR, cNONE, cHUF, cRLE, numCompTypes };		// Compression types

/***********************************************************
* Function prototypes
************************************************************/

int compress(Header& h, void** msg);		// Compress transmitted message

int decompress(Header& h, void** msg);		// Decompress received message

void setCompression(Header& h);				// Set the type of compression to be used on transmitted message