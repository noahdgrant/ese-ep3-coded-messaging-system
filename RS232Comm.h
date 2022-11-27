/***********************************************************
* Name:			RS232Comm.h
* Author(s):	Michael Galle, Noah Grant, Wyatt Richard
* Description:	RS232 cable transmitting and receiving interface.
************************************************************/

#pragma once

#include <Windows.h>

/***********************************************************
* Specific variables
************************************************************/

enum msgTypes { mERR, mNONE, mTXT, mAUD, numMsgTypes };
extern int currentCom;

/***********************************************************
* Function prototypes
************************************************************/

// Prototype the functions to be used
void initPort(HANDLE* hCom, wchar_t* COMPORT, int nComRate, int nComBits, COMMTIMEOUTS timeout);
void purgePort(HANDLE* hCom);
void outputToPort(HANDLE* hCom, LPCVOID buf, DWORD szBuf);
DWORD inputFromPort(HANDLE* hCom, LPVOID buf, DWORD szBuf);

// Sub functions
void createPortFile(HANDLE* hCom, wchar_t* COMPORT);
static int SetComParms(HANDLE* hCom, int nComRate, int nComBits, COMMTIMEOUTS timeout);

// SERIAL COMMUNICATION
void transmitCom(Header* txHeader, void* txMsg);
int receiveCom(Header* rxHeader, void** rxMsg);

// Communication settings functions
void selectComPort();
void setRID(Header& h);
void setSID(Header& h);