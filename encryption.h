/* encryption.h - Interface for encryption functions.
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once
#include "message.h"
#include "header.h"

enum encTypes { ERR, NONE, XOR, VIG, numOfEnc };			// Types of encryption

// ENCRYPTION TYPES
int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec);
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength);

void setEncryption();
void setSecretKey();

// ENCRYPT/DECRYPT MESSAGE
void decrypt(Header h, void* msg);
void encrypt(void* msg, int msgSz);