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
	printf("1. Select Com Port					Com Port:		COM%d\n", currentCom);
	printf("2. Set Compression Type					Compression Type:	");
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
	printf("3. Set Encryption Type					Encryption Type:	");
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
	printf("4. Set Encryption Key					Encryption Key:		%s\n", secretKey);
	printf("5. Change Audio Recording Length			Length:			%d\n", recordTime);
	printf("6. Set Recipient ID					RID:			%d\n", h.rid);
	printf("7. Set Sender ID					SID:			%d\n", h.sid);
	printf("8. Toggle Error Detection\n");
	printf("9. Transmit Text Message\n");
	printf("10. Transmit Aduio Message\n");
	printf("11. Transmit text file\n");
	printf("12. Receive Message\n");
	printf("13. Print recieved messages\n");

	printf("14. Record\n");
	printf("15. Playblack\n");
	printf("16. Generate Quote\n");
	printf("17. View Queue\n");
	printf("18. Transmit testing diagnostic messages\n");
	printf("19. Receive testing diagnostic messages\n");
	printf("0. Exit\n");
	printf("\n> ");
	return;
}