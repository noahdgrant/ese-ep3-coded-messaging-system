#pragma once

#include "header.h"

enum compTypes { cERR, cNONE, cHUF, cRLE, numCompTypes };		// Types of compression

// COMPRESS/DECOMPRESS MESSAGE
int compress(Header& h, void** msg);
int decompress(Header& h, void** msg);
void setCompression();