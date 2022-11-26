/* encryption.cpp - Implementation: Encryption functions
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#define _CRT_SECURE_NO_DEPRECATE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h> 

#include "CMSLibrary.h"
#include "header.h"
#include "message.h"
#include "encryption.h"

char secretKey[MAX_QUOTE_LENGTH] = {};							// Key used to encrypt/decrypt messages
encTypes encType = NONE;									// Default encryption is NONE

// ENCRYPTION TYPES
// // Viginere Encryption/Decryption
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
	enc[messageLength] = '\0';                       // Null terminate the ecrypted message
	strcpy((char*)message, enc);                    // Encrypted/decrypted buffer   

	free(enc);
	enc = NULL;
	return(0);
}

// XOR Encryption/Decryption
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
	enc[messageLength] = '\0';											// Null terminate the ecrypted message
	strcpy((char*)message, enc);												// Encrypted/decrypted buffer   

	free(enc);
	enc = NULL;
	return(0);
}

/********************************************************/

// Set encryption Type 
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

// set the XOR code
void setSecretKey() {
	printf("\nEnter encryption key: ");
	scanf_s("%s", secretKey, MAX_QUOTE_LENGTH - 1);
}

// decrypt message
void decrypt(Header h, void* msg) {
	// Decrypt the message (xor)
	if (h.encryption == XOR) {
		xorCipher(msg, h.payloadSize, secretKey, (int)strlen(secretKey));
	}
	// Decrypt the message (Viginere)
	else if (h.encryption == VIG) {
		vigCipher(msg, h.payloadSize, secretKey, (int)strlen(secretKey), false);
	}

	return;
}

// encrypt message
int encrypt(Header h, void* msg) {
	if ((secretKey[0] == '\0') || (secretKey[0] == '\n')) {
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