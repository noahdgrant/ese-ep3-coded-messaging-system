/***********************************************************
* Name:			RLE.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	RLE encryption interface.
************************************************************/

#pragma once

/***********************************************************
* Specific variables
************************************************************/

#define ESCAPE_CHARACTER '\x1b'

/***********************************************************
* Function prototypes
************************************************************/

int RLEncode(char* in, int inLen, char* out, int outMax, char cEsc);	//Compresses the input using Run Length Encoding
int RLDecode(char* in, int inLen, char* out, int outMax, char cEsc);	//Decompresses the input usinf Run Length Encoding