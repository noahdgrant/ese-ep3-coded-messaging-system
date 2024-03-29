/***********************************************************
* Name:			encryption.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Encryption/decryption implementation.
************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h> 

#include "CMSLibrary.h"
#include "encryption.h"
#include "header.h"
#include "message.h"

/***********************************************************
* Specific variables
************************************************************/

char secretKey[MAX_QUOTE_LENGTH] = {};			// Key used to encrypt/decrypt messages
encTypes encType = NONE;						// Default encryption is NONE

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* vigCipher() - Encrypt message with Viginere encryption.
* message			- Transmitted message.
* messageLength		- Length of transmitted message.
* secretKey			- Encryption key.
* secretKeyLength	- Length of secret key.
* encOrDec			- Encrypt message (true) or decrypt message (false).
* This function returns 0 if encryption was successful or -1 if it failed.
*************************************************************************/
int vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, bool encOrDec) {
	// encOrDec determines if the function is used for encryption or decryption
	char* enc = NULL;								// Pointer to encryption buffer
	char* msg, * key;								// Cast buffers to single byte (char) arrays 
	char n;											// Shift amount
	msg = (char*)message;
	key = (char*)secretKey;

	// Check that there is a secret key
	if (secretKeyLength == 0) {
		printf("\nERROR: Secret key length is 0.\n");
		Sleep(3000);
		return(-1);
	}

	// Get memmory for encryption buffer
	enc = (char*)malloc(sizeof(short) * messageLength);
	if (enc == NULL) {
		printf("\nERROR: Unable to malloc memory for encryption/decryption.\n");
		Sleep(3000);
		return(-1);
	}

	for (int i = 0; i < messageLength; i++) {
		// Convert key to upper case and get n from (distance from 'A' % 26)
		// key[i % secretKeyLength] just loops around the key as many times as necessary to match the legth of hte message
		n = (toupper(key[i % secretKeyLength]) - 'A') % 26;
		if (encOrDec == true) {
			enc[i] = (msg[i] + n);                   // Viginere encrypt - if true
		}
		else {
			enc[i] = (msg[i] - n);                   // Viginere decrypt - if false
		}
	}
	enc[messageLength - 1] = '\0';                       // Null terminate the ecrypted message
	strcpy((char*)message, enc);                    // Encrypted/decrypted buffer   

	free(enc);
	enc = NULL;
	return(0);
}

/*************************************************************************
* xorCipher() - Encrypt message with XOR encryption.
* message			- Transmitted message.
* messageLength		- Length of transmitted message.
* secretKey			- Encryption key.
* secretKeyLength	- Length of secret key.
* This function returns 0 if encryption was successful or -1 if it failed.
*************************************************************************/
int xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength) {
	short* encBuf = NULL;								// Pointer to encryption buffer
	char* msg, * key, * enc;							// Cast buffers to single byte (char) arrays 
	msg = (char*)message;
	key = (char*)secretKey;

	// Check that there is a secent key
	if (secretKeyLength == 0) {
		printf("\nERROR: Secret key length is 0.\n");
		Sleep(3000);
		return(-1);
	}

	// Get memmory for encryption buffer
	encBuf = (short*)malloc(sizeof(short) * messageLength);
	if (encBuf == NULL) {
		printf("\nERROR: Unable to malloc memory for encryption/decryption.\n");
		Sleep(3000);
		return(-1);
	}
	enc = (char*)encBuf;

	// Encrypt message
	for (int i = 0; i < messageLength; i++) {
		enc[i] = msg[i] ^ key[i % secretKeyLength];						// XOR encrypt bytewise (loop the key as many times as required
	}
	enc[messageLength - 1] = '\0';										// Null terminate the ecrypted message
	strcpy((char*)message, enc);										// Encrypted/decrypted buffer   

	free(enc);
	enc = NULL;
	return(0);
}

/*************************************************************************
* setEncryption() - Set the type of encryption to be used on the transmitted message.
* h		- Transmit header.
*************************************************************************/
void setEncryption(Header& h) {
	char cmd[2] = {};		// Holds the user's encryption choice
	do {
		system("cls");
		printf("Enter type of encryption/decryption\n");
		printf("1. No Encryption\n");
		printf("2. XOR\n");
		printf("3. Viginere\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) == NONE) {
			printf("\nNow using no encryption\n");
			h.encryption = NONE;
		}
		else if (atoi(cmd) == XOR) {
			printf("\nNow using XOR encryption\n");
			h.encryption = XOR;
		}
		else if (atoi(cmd) == VIG) {
			printf("\nNow using Viginere encryption\n");
			h.encryption = VIG;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < NONE || atoi(cmd) > numOfEnc);
}

/*************************************************************************
* setSecretKey() - Set encryption/decryption key.
*************************************************************************/
void setSecretKey() {
	printf("\nEnter encryption key: ");
	scanf_s("%s", secretKey, MAX_QUOTE_LENGTH - 1);
}

/*************************************************************************
* decrypt() - Checks what type of encryption was used on received message and passed the message to the appropriate decrypt function.
* h		- Received header.
* msg	- Received message.
*************************************************************************/
void decrypt(Header h, void* msg) {
	// Exit function if no encryption is needed
	if (h.encryption == NONE) return;
	// Decrypt the message (xor)
	else if (h.encryption == XOR) {
		xorCipher(msg, h.payloadSize, secretKey, (int)strlen(secretKey));
	}
	// Decrypt the message (Viginere)
	else if (h.encryption == VIG) {
		vigCipher(msg, h.payloadSize, secretKey, (int)strlen(secretKey), false);
	}

	return;
}

/*************************************************************************
* encrypt() - Checks how user wants to encrypt message and passed the message to the appropriate encrypt function.
* h		- Received header.
* msg	- Received message.
* This function returns 0 if encryption was successful (or not needed) or -1 if no encryption key was set.
*************************************************************************/
int encrypt(Header h, void* msg) {
	// Exit function if no encryption is needed
	if (h.encryption == NONE) return(0);

	// Make sure secret key is set
	else if ((secretKey[0] == '\0') || (secretKey[0] == '\n')) {
		printf("\nERROR: Secret key not set.\n");
		Sleep(1500);
		return(-1);
	}

	// XOR Encryption
	if (h.encryption == XOR) {
		xorCipher(msg, h.payloadSize, secretKey, (int)strlen(secretKey));
	}
	// Viginere encryption
	else if (h.encryption == VIG) {
		vigCipher(msg, h.payloadSize, secretKey, (int)strlen(secretKey), true);
	}

	return(0);
}