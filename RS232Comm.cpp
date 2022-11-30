/***********************************************************
* Name:			RS232Comm.cpp
* Author(s):	Michael Galle, Noah Grant, Wyatt Richard
* Description:	RS232 cable transmitting and receiving implementation.
************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <Windows.h>    // Includes the functions for serial communication via RS232
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "header.h"
#include "RS232Comm.h"
#include "message.h"
#include "contactList.h"

/***********************************************************
* Specific variables
************************************************************/

#define EX_FATAL 1

int nComRate = 460800;						// Baud (Bit) rate in bits/second 
int nComBits = 8;							// Number of bits per frame
int currentCom = 6;							// Default COM port
wchar_t COMPORT_Tx[] = L"COM6";				// COM port used for transmitting
wchar_t COMPORT_Rx[] = L"COM6";				// COM port used for recieving
HANDLE hComTx;								// Pointer to the selected COM port (Transmitter)
HANDLE hComRx;								// Pointer to the selected COM port (Receiver)
COMMTIMEOUTS timeout;						// A commtimeout struct variable

/****************************************************************/

// Initializes the port and sets the communication parameters
void initPort(HANDLE* hCom, wchar_t* COMPORT, int nComRate, int nComBits, COMMTIMEOUTS timeout) {
	createPortFile(hCom, COMPORT);						// Initializes hCom to point to PORT#
	purgePort(hCom);									// Purges the COM port
	SetComParms(hCom, nComRate, nComBits, timeout);		// Uses the DCB structure to set up the COM port
	purgePort(hCom);
}

// Purge any outstanding requests on the serial port (initialize)
void purgePort(HANDLE* hCom) {
	PurgeComm(*hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

// Output/Input messages to/from ports 
void outputToPort(HANDLE* hCom, LPCVOID buf, DWORD szBuf) {
	int i=0;
	DWORD NumberofBytesTransmitted;
	LPDWORD lpErrors=0;
	LPCOMSTAT lpStat=0; 

	i = WriteFile(
		*hCom,										// Write handle pointing to COM port
		buf,										// Buffer size
		szBuf,										// Size of buffer
		&NumberofBytesTransmitted,					// Written number of bytes
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);				// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
	}
	else
		printf("\nSuccessful transmission, there were %ld bytes transmitted\n", NumberofBytesTransmitted);
}

DWORD inputFromPort(HANDLE* hCom, LPVOID buf, DWORD szBuf) {
	int i = 0;
	DWORD NumberofBytesRead;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;

	i = ReadFile(
		*hCom,										// Read handle pointing to COM port
		buf,										// Buffer size
		szBuf,  									// Size of buffer - Maximum number of bytes to read
		&NumberofBytesRead,
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nRead Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.
	}
	else
		printf("\nSuccessful reception!, There were %ld bytes read\n", NumberofBytesRead);

	return(NumberofBytesRead);
}

// Sub functions called by above functions
/**************************************************************************************/
// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, open the port and set securities
void createPortFile(HANDLE* hCom, wchar_t* COMPORT) {
	// Call the CreateFile() function 
	*hCom = CreateFile(
		COMPORT,									// COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
		GENERIC_READ | GENERIC_WRITE,				// Open for read and write
		NULL,										// No sharing allowed
		NULL,										// No security
		OPEN_EXISTING,								// Opens the existing com port
		FILE_ATTRIBUTE_NORMAL,						// Do not set any file attributes --> Use synchronous operation
		NULL										// No template
	);
	
	if (*hCom == INVALID_HANDLE_VALUE) {
		printf("\nFatal Error 0x%x: Unable to open\n", GetLastError());
	}
	else {
		printf("\nCOM is now open\n");
	}
}

static int SetComParms(HANDLE* hCom, int nComRate, int nComBits, COMMTIMEOUTS timeout) {
	DCB dcb;										// Windows device control block
	// Clear DCB to start out clean, then get current settings
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(*hCom, &dcb))
		return(0);

	// Set our own parameters from Globals
	dcb.BaudRate = nComRate;						// Baud (bit) rate
	dcb.ByteSize = (BYTE)nComBits;					// Number of bits(8)
	dcb.Parity = 0;									// No parity	
	dcb.StopBits = ONESTOPBIT;						// One stop bit
	if (!SetCommState(*hCom, &dcb))
		return(0);

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	memset((void *)&timeout, 0, sizeof(timeout));
	timeout.ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout.ReadTotalTimeoutMultiplier = 1;			// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout.ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(*hCom, &timeout);
	return(1);
}

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* transmitCom() - Transmit communication.
* txHeader		- Transmit header.
* txMsg			- Transmit message.
*************************************************************************/
void transmitCom(Header* txHeader, void* txMsg) {
	initPort(&hComTx, COMPORT_Tx, nComRate, nComBits, timeout);								// Initialize the Tx port
	Sleep(500);
	outputToPort(&hComTx, txHeader, sizeof(Header));										// Send Header
	Sleep(500);
	outputToPort(&hComTx, txMsg, (*txHeader).payloadSize);									// Send string to port - include space for '\0' termination
	Sleep(500);																				// Allow time for signal propagation on cable 

	CloseHandle(hComTx);																	// Close the handle to Tx port 
	purgePort(&hComTx);																		// Purge the Tx port
	return;
}

/*************************************************************************
* receiveCom() - Receive communication.
* rxHeader		- Received header.
* rxMsg			- Received message.
*************************************************************************/
int receiveCom(Header* rxHeader, void** rxMsg) {
	DWORD bytesRead = 0;																// Number of bytes recieved from incomming message

	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout);							// Initialize the Rx port
	Sleep(500);
	bytesRead = inputFromPort(&hComRx, rxHeader, sizeof(Header));
	// Check if header was recieved
	if (bytesRead == 0) {
		CloseHandle(hComRx);															// Close the handle to Rx port 
		purgePort(&hComRx);																// Purge the Rx port
		printf("\nERROR: Reciever timed out before message header was recieved.\n");
		Sleep(2500);
		return(-1);
	}

	*rxMsg = (void*)malloc((*rxHeader).payloadSize);
	if (rxMsg == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		Sleep(1500);
		return(-1);
	}

	bytesRead = inputFromPort(&hComRx, *rxMsg, (*rxHeader).payloadSize);

	CloseHandle(hComRx);																// Close the handle to Rx port 
	purgePort(&hComRx);																	// Purge the Rx port
	return(0);
}

/*************************************************************************
* selectComPort() - Select com port to use for communication.
*************************************************************************/
void selectComPort() {
	char cmd[3] = {};			// Holds the COM port the user wants to use
	do {
		system("cls");
		printf("Enter a number between 0 and 9 coresponding to the desired Com Port (ie. 1 -> COM1)\n");
		printf("\n> ");
		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) >= 0 && atoi(cmd) <= 9) {
			printf("\nThe new Com Port is now COM%d\n", atoi(cmd));
			switch (atoi(cmd)) {
			case 0:
				wcscpy(COMPORT_Tx, L"COM0");
				wcscpy(COMPORT_Rx, L"COM0");
				currentCom = 0;
				break;
			case 1:
				wcscpy(COMPORT_Tx, L"COM1");
				wcscpy(COMPORT_Rx, L"COM1");
				currentCom = 1;
				break;
			case 2:
				wcscpy(COMPORT_Tx, L"COM2");
				wcscpy(COMPORT_Rx, L"COM2");
				currentCom = 2;
				break;
			case 3:
				wcscpy(COMPORT_Tx, L"COM3");
				wcscpy(COMPORT_Rx, L"COM3");
				currentCom = 3;
				break;
			case 4:
				wcscpy(COMPORT_Tx, L"COM4");
				wcscpy(COMPORT_Rx, L"COM4");
				currentCom = 4;
				break;
			case 5:
				wcscpy(COMPORT_Tx, L"COM5");
				wcscpy(COMPORT_Rx, L"COM5");
				currentCom = 5;
				break;
			case 6:
				wcscpy(COMPORT_Tx, L"COM6");
				wcscpy(COMPORT_Rx, L"COM6");
				currentCom = 6;
				break;
			case 7:
				wcscpy(COMPORT_Tx, L"COM7");
				wcscpy(COMPORT_Rx, L"COM7");
				currentCom = 7;
				break;
			case 8:
				wcscpy(COMPORT_Tx, L"COM8");
				wcscpy(COMPORT_Rx, L"COM8");
				currentCom = 8;
				break;
			case 9:
				wcscpy(COMPORT_Tx, L"COM9");
				wcscpy(COMPORT_Rx, L"COM9");
				currentCom = 9;
				break;
			default:
				printf("Something went wrong with the com assignment");
				Sleep(2000);
			}
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 0 || atoi(cmd) > 9);
}

/*************************************************************************
* setRID() - Set the receiver ID.
*************************************************************************/
void setRID(Header& h) {
	char cmd[2] = {};		// Holds the user's encryption choice
	do {
		system("cls");
		printf("1. Chose from contact list\n");
		printf("2. Add new contact\n");
		printf("3. Chose an ID not assigned to a contact\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) == 1) {//chose from contact list
			h.rid = selectContact();
		}
		else if (atoi(cmd) == 2) {//add new contact
			addContact();
		}
		else if (atoi(cmd) == 3) {//chose an ID not assigned to a contact
			printf("\nEnter the recipient ID: ");
			scanf_s("%d", &h.rid);
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 0 || atoi(cmd) > 3);
  
  	h.rid2 = h.rid3 = h.rid;				// Used for voteOn() error detection and correction
    return;
}

/*************************************************************************
* setSID() - Set the sender ID.
*************************************************************************/
void setSID(Header& h) {
	char cmd[2] = {};		// Holds the user's encryption choice
	do {
		system("cls");
		printf("1. Chose from contact list\n");
		printf("2. Add new contact\n");
		printf("3. Chose an ID not assigned to a contact\n");
		printf("\n> ");

		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		if (atoi(cmd) == 1) {//chose from contact list
			h.sid = selectContact();
		}
		else if (atoi(cmd) == 2) {//add new contact
			addContact();
		}
		else if (atoi(cmd) == 3) {//chose an ID not assigned to a contact
			printf("\nEnter the recipient ID: ");
			scanf_s("%d", &h.sid);
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 0 || atoi(cmd) > 3);

}

/*************************************************************************
* fileSz() - Counts the number of characters in file
* file		- The file to count the size of.
* This function returns the number of characters in a file if successful. Otherwise, it returns -1.
*************************************************************************/
int fileSz(char* file) {
	FILE* fp = NULL;
	int size = 0;										// Number of characters in file.
	char c = '\0';

	fp = fopen(file, "r");
	if (fp == NULL) {									// Make sure the filename is correct
		printf("\nERROR: Could not open file.\n");
		return(-1);
	}

	while (!feof(fp)) {
		fgetc(fp);
		size++;
	}

	fclose(fp);
	return(size);
}

/*************************************************************************
* copyFile() - Copies into file to transmit buffer.
* txMsg		- Message to be transmitted.
* filename	- File name of transmit file.
* fileSz	- Size of transmit file.
* This function returns the number of characters in the final transmit message buffer. Otherwise, it returns -1.
*************************************************************************/
int copyFile(char* txMsg, char* filename, int fileSz) {
	FILE* fp = NULL;
	int offset = 0;

	fp = fopen(filename, "r");
	if (fp == NULL) {									// Make sure the filename is correct
		printf("\nERROR: Could not open file.\n");
		return(-1);
	}

	offset = strlen(filename) + strlen("!!");

	strcat(txMsg, filename);
	strcat(txMsg, "!!");

	fread(txMsg + offset, fileSz, 1, fp);

	txMsg[offset + fileSz] = '\0';

	fclose(fp);
	return(strlen(txMsg) + 1); // +1 for \0. strlen() only counts chars, it doesn't add the +1 needed for the \0 at the end of the string
}

/*************************************************************************
* saveFile() - Saves received file to *.txt file.
* rxMsg		- Message to be saved.
*************************************************************************/
void saveFile(char* rxMsg) {
	FILE* f;
	errno_t err;
	char filename[MAX_QUOTE_LENGTH] = {};
	int i = 0;
	int offset = 0;

	while (rxMsg[i] != '!' && rxMsg[i + 1] != '!') {
		filename[i] = rxMsg[i];
		i++;
	}
	filename[i] = rxMsg[i];				// Get the last character before !!
	filename[i + 1] = '\0';

	offset = strlen(filename) + strlen("!!");

	err = fopen_s(&f, filename, "wb");
	if (f) {
		fprintf(f, "%s", rxMsg + offset);	// Copy the buffer to the file starting after the offset
		fclose(f);
	}

	printf("\n%s successfully saved.\n", filename);
	return;
}