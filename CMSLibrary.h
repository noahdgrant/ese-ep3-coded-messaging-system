/* CMSLibrary.h -Interface: Header file for extra CMS functions
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

#include "header.h"

enum encTypes { ERR, NONE, XOR, VIG, numOfEnc };				// Types of encryption
enum compTypes { cERR, cNONE, cHUF, cRLE, numCompTypes };		// Types of compression

// MENU
// Print CMS menu
void printMenu();

// GUI Options
void selectComPort();
void changeAudioSettings();
void setRID();
void setSID();
void setEncryption();
void setSecretKey();

// ENCRYPT/DECRYPT MESSAGE
void decrypt(void* msg, int msgSz);
void encrypt(void* msg, int msgSz);

// COMPRESS/DECOMPRESS MESSAGE
void compress(void* msg, int compType, int payloadType);
void decompress(void* msg, int compType, int payloadType, int payloadSize);
void setCompression();