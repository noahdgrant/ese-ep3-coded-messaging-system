/***********************************************************
* Name:			testing.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Diagnostic testing functions implementation.
************************************************************/

// Should probably free the memory after each test otherwise it will all leak

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "CMSLibrary.h"
#include "encryption.h"
#include "header.h"
#include "message.h"
#include "queues.h"
#include "sound.h"
#include "compression.h"
#include "RS232Comm.h"

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* txTesting() - Transmits all text cases for Coded Messaging System.
* This function return 0 if it successful. Otherwise, it return -1.
*************************************************************************/



int txTesting() {
	Header txHeader = {};
	void* msg;
	void* audioMsg = NULL;								// Pointer to audio message buffer
	int recordTime = 2;
	long numAudioBytes = SAMPLES_SEC * recordTime;		// Size of audio buffer
	strcpy(secretKey, "banana");						// Set secret key at the beginning of testing

	int compArray[] =	{cNONE,	cHUF,	cRLE,	cNONE,	cNONE,	cHUF,	cHUF,	cRLE,	cRLE};
	int encArray[] =	{NONE,	NONE,	NONE,	XOR,	VIG,	XOR,	VIG,	XOR,	VIG };
	char msgArray[][100] = { "Plain text message",
					 "Compressed with huffman", "Compressed with RLE", "Encrypted with XOR", "Encrypted with Viginere","Compressed with huffman", "Compressed with RLE", "Encrypted with XOR"};

	system("cls");
	printf("\n========== CMS DIAGNOSTIC TESTING ==========\n");

	/***********************************************************
	* TEXT MESSAGE TESTING
	************************************************************/

	msg = (char*)malloc(MAX_QUOTE_LENGTH);
	if (msg == NULL) {
		printf("\nERROR: Could not malloc memory for quote buffer.\n");
		return 0;
	}

	txHeader.payloadType = mTXT;

	printf("\nTEXT MESSAGE TESTING\n");
	//========================== Plain Text =================================//
	printf("\nTEST: Plain text\n");
	printf("Sending message: Plain text message\n");
	strcpy((char*)msg,(char*)"Plain text message");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cNONE;
	txHeader.encryption = NONE;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//====================== Text w/ Huffman ==========================//
	printf("\nTEST: Huffman\n");
	printf("Sending message: Compressed with huffman\n");
	strcpy((char*)msg, (char*)"Compressed with huffman");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cHUF;
	txHeader.encryption = NONE;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//====================== Text w/ RLE ==============================//
	printf("\nTEST: RLE\n");
	printf("Sending message: Compressed with RLE\n");
	strcpy((char*)msg, (char*)"Compressed with RLE");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cRLE;
	txHeader.encryption = NONE;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ XOR ============================//
	printf("\nTEST: XOR\n");
	printf("Sending message: Encrypted with XOR\n");
	strcpy((char*)msg, (char*)"Encrypted with XOR");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cNONE;
	txHeader.encryption = XOR;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//==================== Text w/ Viginere ===========================//
	printf("\nTEST: Viginere\n");
	printf("Sending message: Encrypted with Viginere\n");
	strcpy((char*)msg, (char*)"Encrypted with Viginere");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cNONE;
	txHeader.encryption = VIG;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//=================== Text w/ Huffman & XOR =======================//
	printf("\nTEST: Huffman & XOR\n");
	printf("Sending message: Compressed and encrypted with huffman and XOR\n");
	strcpy((char*)msg, (char*)"Compressed and encrypted with huffman and XOR");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cHUF;
	txHeader.encryption = XOR;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//================ Text w/ Huffman & Viginere =====================//
	printf("\nTEST: Huffman & Viginere\n");
	printf("Sending message: Compressed and encrypted with huffman and Viginere\n");
	strcpy((char*)msg, (char*)"Compressed and encrypted with huffman and Viginere");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cHUF;
	txHeader.encryption = VIG;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//=================== Text w/ RLE & XOR ===========================//
	printf("\nTEST: RLE & XOR\n");
	printf("Sending message: Compressed and encrypted with RLE and XOR\n");
	strcpy((char*)msg, (char*)"Compressed and encrypted with RLE and XOR");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cRLE;
	txHeader.encryption = XOR;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//==================== Text w/ RLE & Viginere =====================//
	printf("\nTEST: RLE & Viginere\n");
	printf("Sending message: Compressed and encrypted with RLE and Viginere\n");
	strcpy((char*)msg, (char*)"Compressed and encrypted with RLE and Viginere");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cRLE;
	txHeader.encryption = VIG;
	encrypt(txHeader, msg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, msg);
	printf("\n============================================================================\n");
	Sleep(1000);


	/***********************************************************
	* AUDIO MESSAGE TESTING
	************************************************************/
	printf("\nAUDIO MESSAGE TESTING\n");

	// Get memory for recording
	audioMsg = (short*)malloc(numAudioBytes * sizeof(short));
	if (audioMsg == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		return(-1);
	}

	// Record message
	InitializeRecording();
	RecordBuffer((short*)audioMsg, numAudioBytes);
	CloseRecording();

	txHeader.payloadType = mAUD;
	txHeader.payloadSize = txHeader.uncompressedLength = numAudioBytes * 2;

	//========================== Plain Audio =================================//
	printf("\nTEST: Plain audio\n");
	txHeader.compression = cNONE;
	txHeader.encryption = NONE;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ Huffman =================================//
	printf("\nTEST: Huffman\n");
	txHeader.compression = cHUF;
	txHeader.encryption = NONE;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ RLE =================================//
	printf("\nTEST: RLE\n");
	txHeader.compression = cRLE;
	txHeader.encryption = NONE;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ XOR =================================//
	printf("\nTEST: XOR\n");
	txHeader.compression = cNONE;
	txHeader.encryption = XOR;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ Viginere =================================//
	printf("\nTEST: Viginere\n");
	txHeader.compression = cNONE;
	txHeader.encryption = VIG;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ Huffman & XOR =================================//
	printf("\nTEST: Huffman & XOR\n");
	txHeader.compression = cHUF;
	txHeader.encryption = XOR;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ Huffman & Viginere =================================//
	printf("\nTEST: Huffman & Viginere\n");
	txHeader.compression = cHUF;
	txHeader.encryption = VIG;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ RLE & XOR =================================//
	printf("\nTEST: RLE & XOR\n");
	txHeader.compression = cRLE;
	txHeader.encryption = XOR;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	//========================== Audio w/ RLE & Viginere =================================//
	printf("\nTEST: RLE & Viginere\n");
	txHeader.compression = cRLE;
	txHeader.encryption = VIG;
	encrypt(txHeader, audioMsg);
	txHeader.payloadSize = compress(&msg, txHeader.uncompressedLength, txHeader.compression);
	transmitCom(&txHeader, audioMsg);
	printf("\n============================================================================\n");
	Sleep(1000);

	free(audioMsg);
	audioMsg = NULL;
	/**********************************************************************/

	strcpy(secretKey, "\0");	// Clear secret key at the end of testing
	system("pause");
	return(0);
}

/*************************************************************************
* rxTesting() - Receives all text cases for Coded Messaging System.
* This function return 0 if it successful. Otherwise, it return -1.
*************************************************************************/
int rxTesting() {
	void* msgIn = NULL;					// Pointer to recieved message buffer
	Header rxHeader = {};

	strcpy(secretKey, "banana");		// Set secret key at the beginning of testing

	system("cls");
	printf("\n========== CMS DIAGNOSTIC TESTING ==========\n");

	/***********************************************************
	* TEXT MESSAGE TESTING
	************************************************************/
	printf("\nTEXT MESSAGE TESTING\n");
	//======================== Plain Text ================================//
	printf("\nTEST: Plain Text\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ Huffman ============================//
	printf("\nTEST: Huffman\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ RLE ============================//
	printf("\nTEST: RLE\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ XOR ============================//
	printf("\nTEST: XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ Viginere ============================//
	printf("\nTEST: Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ Huffman & XOR ============================//
	printf("\nTEST: Huffman & XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ Huffman & Viginere ============================//
	printf("\nTEST: Huffman & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ RLE & XOR ============================//
	printf("\nTEST: RLE & XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Text w/ RLE & Viginere ============================//
	printf("\nTEST: RLE & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nReceived message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	Sleep(1000);

	/*****************************************************************************************/

	/***********************************************************
	* AUDIO MESSAGE TESTING
	************************************************************/
	printf("\nAUDIO MESSAGE TESTING\n");
	//======================== Plane audio ============================//
	printf("\nTEST: Plane audio\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ Huffman ============================//
	printf("\nTEST: Huffman\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ RLE ============================//
	printf("\nTEST: RLE\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ XOR ============================//
	printf("\nTEST: XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ Viginere ============================//
	printf("\nTEST: Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ Huffman & XOR ============================//
	printf("\nTEST: Huffman & XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ Huffman & Viginere ============================//
	printf("\nTEST: Huffman & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ RLE & XOR ============================//
	printf("\nTEST: RLE & XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	//======================== Audio w/ RLE & Viginere ============================//
	printf("\nTEST: RLE & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(&msgIn, rxHeader.payloadSize, rxHeader.uncompressedLength, rxHeader.compression);
	decrypt(rxHeader, msgIn);
	printf("\nPlaying received recording...\n");
	InitializePlayback();
	PlayBuffer((short*)msgIn, rxHeader.payloadSize / 2);			
	ClosePlayback();
	printf("\n============================================================================\n");
	Sleep(1000);

	/*******************************************************************************/

	strcpy(secretKey, "\0");		// Clear secret key at the end of testing
	system("pause");
	return(0);
}