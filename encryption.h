/* encryption.h - Interface for encryption functions.
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

// ENCRYPTION TYPES
int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec);
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength);