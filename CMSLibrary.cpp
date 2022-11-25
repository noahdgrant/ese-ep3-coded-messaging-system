/* message.cpp -Implementation: Functions for CMS project
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#define _CRT_SECURE_NO_DEPRECATE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h> 
#include <wchar.h>		// Don't remember what this is needed for
#include <ctype.h>		// Don't remember what this is needed for

#include "CMSLibrary.h"
#include "compression.h"
#include "encryption.h"
#include "header.h"
#include "message.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"

extern int currentCom;
extern char secretKey[MAX_QUOTE_LENGTH];			// Key used to encrypt/decrypt messages
extern encTypes encType;							// Default encryption is NONE
extern int rid;
extern int sid;
extern int recordTime;								// Default record time
extern long numAudioBytes;							// Size of audio buffer
extern compTypes compType;							// Default compression is NONE

// MENU
// Print CMS menu
void printMenu() {
	printf("\nCMS Menu\n");
	printf("1. Record\n");
	printf("2. Playblack\n");
	printf("3. Generate Quote\n");
	printf("4. View Queue\n");
	printf("5. Transmit Text Message\n");
	printf("6. Transmit Aduio Message\n");
	printf("7. Receive Message\n");
	printf("8. Select Com Port					Com Port:		COM%d\n", currentCom);
	printf("9. Change Audio Recording Length			Length:			%d\n", recordTime);
	printf("10. Set Encryption Type					Encryption Type:	");
	switch (encType) {
	case NONE:
		printf("None\n");
		break;
	case XOR:
		printf("XOR\n");
		break;
	case VIG:
		printf("Viginere\n");
		break;
	}
	printf("11. Set Encryption Key					Encryption Key:		%s\n", secretKey);
	printf("12. Set Recipient ID					RID:			%d\n", rid);
	printf("13. Set Sender ID					SID:			%d\n", sid);
	printf("14. Set Compression Type				Compression Type:	");
	switch (compType) {
	case cNONE:
		printf("None\n");
		break;
	case cHUF:
		printf("Huffman\n");
		break;
	case cRLE:
		printf("RLE\n");
		break;
	}
	printf("15. Print recieved messages\n");
	printf("0. Exit\n");
	printf("\n> ");
	return;
} // printMenu()