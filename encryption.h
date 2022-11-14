/* encryption.h - Interface for encryption functions.
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

// ENCRYPTION TYPES
int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec);
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength);

// ENCRYPT/DECRYPT MESSAGE
void decrypt(void* msg, int msgSz);
void encrypt(void* msg, int msgSz);

// SETTINGS
void setEncryption();
void setSecretKey();