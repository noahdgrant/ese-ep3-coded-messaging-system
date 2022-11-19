#pragma once

#define escapeCharacter '\x1b'

int RLEncode(char* in, int inLen, char* out, int outMax, char cEsc);	//Compresses the input using Run Length Encoding
int RLDecode(char* in, int inLen, char* out, int outMax, char cEsc);	//Decompresses the input usinf Run Length Encoding