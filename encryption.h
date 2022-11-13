/* encryption.h -Implementation:Encryption functions
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

// Encryption
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength);
int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec);