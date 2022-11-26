/* encryption.h - Interface for encryption functions.
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once
#include "message.h"
#include "header.h"

enum encTypes { ERR, NONE, XOR, VIG, numOfEnc };		// Types of encryption
extern char secretKey[MAX_QUOTE_LENGTH];				// Key used to encrypt/decrypt messages

// ENCRYPTION TYPES
int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec);
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength);

// ENCRYPT/DECRYPT MESSAGE
void decrypt(Header h, void* msg);
void encrypt(Header h, void* msg);

// Encryption settings functions
void setEncryption(Header& h);
void setSecretKey();
