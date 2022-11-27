/***********************************************************
* Name:			encryption.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Encryption/decryption interface.
************************************************************/

#pragma once

#include "message.h"
#include "header.h"

/***********************************************************
* Specific variables
************************************************************/

enum encTypes { ERR, NONE, XOR, VIG, numOfEnc };		// Types of encryption
extern char secretKey[MAX_QUOTE_LENGTH];				// Key used to encrypt/decrypt messages

/***********************************************************
* Function prototypes
************************************************************/

int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec);
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength);

void decrypt(Header h, void* msg);		// Decrypt received message
int encrypt(Header h, void* msg);		// Encrypt transmitted message

void setEncryption(Header& h);			// Set the type of encryption used on transmitted message
void setSecretKey();					// Set key for encryption/decryption
