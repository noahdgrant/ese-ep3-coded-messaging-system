/***********************************************************
* Name:			CMSLibrary.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	CMSLibrary functions implementation.
************************************************************/

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

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* printMenu() - Print Coded Messaging System main menu.
* h		- Transmit header.
*************************************************************************/
void printMenu(Header& h) {
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
	switch (h.encryption) {
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
	printf("12. Set Recipient ID					RID:			%d\n", h.rid);
	printf("13. Set Sender ID					SID:			%d\n", h.sid);
	printf("14. Set Compression Type				Compression Type:	");
	switch (h.compression) {
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
	printf("16. Transmit testing diagnostic messages\n");
	printf("17. Receive testing diagnostic messages\n");
	printf("18. Transmit text file\n");
	printf("19. Error detection and correction                      Error detection: ");
	switch (h.errorDC) {
	case 0:
		printf("OFF\n");
		break;
	case 1:
		printf("ON\n");
		break;
	}
	printf("0. Exit\n");
	printf("\n> ");
	return;
}