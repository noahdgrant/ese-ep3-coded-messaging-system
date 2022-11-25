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

int testingout() {
	Header txHeader = {};
	void* msg;
	short* audioMsg = NULL;										// Pointer to audio message buffer
	int recordTime = 5;
	long  numAudioBytes;									// Size of audio buffer

	msg = (char*)malloc(MAX_QUOTE_LENGTH);
	if (msg == NULL) {
		printf("\nERROR: Could not malloc memory for quote buffer.\n");
		return 0;
	}

	txHeader.payloadType = mTXT;

	// send text
	printf("\nplain text\n");
	strcpy((char*)msg,(char*)"plain text message");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = NONE;
	txHeader.encryption = NONE;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ Huffman
	printf("\nHuffman\n");
	strcpy((char*)msg, (char*)"Compressed with huffman");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cHUF;
	txHeader.encryption = NONE;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ Huffman & XOR
	//printf("\nHuffman & XOR\n");
	//strcpy((char*)msg, (char*)"Compressed and encrypted with huffman and XOR");
	//txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	//txHeader.compression = cHUF;
	//txHeader.encryption = XOR;
	//encrypt(msg, txHeader.payloadSize);
	//compress(txHeader, &msg);
	//transmitCom(&txHeader, msg);
	// send text w/ Huffman & Viginere
	printf("\nHuffman & Viginere\n");
	strcpy((char*)msg, (char*)"Compressed and encrypted with huffman and Viginere");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cHUF;
	txHeader.encryption = VIG;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ RLE
	printf("\nRLE\n");
	strcpy((char*)msg, (char*)"Compressed with RLE");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cRLE;
	txHeader.encryption = NONE;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ RLE & XOR
	//printf("\nRLE & XOR\n");
	//strcpy((char*)msg, (char*)"Compressed and encrypted with RLE and XOR");
	//txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	//txHeader.compression = cRLE;
	//txHeader.encryption = XOR;
	//encrypt(msg, txHeader.payloadSize);
	//compress(txHeader, &msg);
	//transmitCom(&txHeader, msg);
	// send text w/ RLE & Viginere
	printf("\nRLE & Viginere\n");
	strcpy((char*)msg, (char*)"Compressed and encrypted with RLE and Viginere");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cRLE;
	txHeader.encryption = VIG;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ XOR
	//printf("\nXOR\n");
	//strcpy((char*)msg, (char*)"Encrypted with XOR");
	//txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	//txHeader.compression = cNONE;
	//txHeader.encryption = XOR;
	//encrypt(msg, txHeader.payloadSize);
	//compress(txHeader, &msg);
	//transmitCom(&txHeader, msg);
	// send text w/ Viginere
	printf("\nViginere\n");
	strcpy((char*)msg, (char*)"Encrypted with Viginere");
	txHeader.payloadSize = txHeader.uncompressedLength = (long)(strlen((char*)msg) + 1);
	txHeader.compression = cNONE;
	txHeader.encryption = VIG;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	/*

	// Get memory for recording
	audioMsg = (short*)malloc(numAudioBytes * sizeof(short));
	if (audioMsg == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		return(-1);
	}

	// Record message
	InitializeRecording();
	RecordBuffer(audioMsg, numAudioBytes);
	CloseRecording();


	txHeader.payloadType = mAUD;
	txHeader.uncompressedLength = numAudioBytes * 2;


	// send audio
	txHeader.compression = cNONE;
	txHeader.encryption = NONE;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ Huffman
	txHeader.compression = cHUF;
	txHeader.encryption = NONE;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ Huffman & XOR
	txHeader.compression = cHUF;
	txHeader.encryption = XOR;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ Huffman & Viginere
	txHeader.compression = cHUF;
	txHeader.encryption = VIG;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ RLE
	txHeader.compression = cRLE;
	txHeader.encryption = NONE;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ RLE & XOR
	txHeader.compression = cRLE;
	txHeader.encryption = XOR;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ RLE & Viginere
	txHeader.compression = cRLE;
	txHeader.encryption = VIG;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ XOR
	txHeader.compression = cNONE;
	txHeader.encryption = XOR;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);
	// send audio w/ Viginere
	txHeader.compression = cNONE;
	txHeader.encryption = VIG;
	encrypt(audioMsg, numAudioBytes * 2);
	compress(txHeader, &msg);
	transmitCom(&txHeader, audioMsg);

	free(audioMsg);
	audioMsg = NULL;
	*/
	free(msg);
	return 1;
}






int testingin() {
	void* msgIn = NULL;											// Pointer to recieved message buffer
	Header rxHeader = {};

	// receive text
	printf("\nPlain Text\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn); 
	decrypt(rxHeader, msgIn);
	printf("Received message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");
	// receive text w/ Huffman
	printf("\nHuffman\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	printf("Received message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");



	// receive text w/ Huffman & XOR
	//printf("\nHuffman & XOR\n");
	//receiveCom(&rxHeader, &msgIn);
	//decompress(rxHeader, &msgIn);
	//decrypt(rxHeader, msgIn);
	//printf("Received message: %s\n", (char*)msgIn);
	//printf("\n============================================================================\n");


	//receive text w/ Huffman & Viginere
	printf("\nHuffman & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	printf("Received message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");


	// receive text w/ RLE
	printf("\nRLE\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	printf("Received message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");


	// receive text w/ RLE & XOR
	//printf("\nRLE & XOR\n");
	//receiveCom(&rxHeader, &msgIn);
	//decompress(rxHeader, &msgIn);
	//decrypt(rxHeader, msgIn);
	//printf("Received message: %s\n", (char*)msgIn);
	//printf("\n============================================================================\n");


	// receive text w/ RLE & Viginere
	printf("\nRLE & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	printf("Received message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");


	// receive text w/ XOR
	//printf("\nXOR\n");
	//receiveCom(&rxHeader, &msgIn);
	//decompress(rxHeader, &msgIn);
	//decrypt(rxHeader, msgIn);
	//printf("Received message: %s\n", (char*)msgIn);
	//printf("\n============================================================================\n");


	// receive text w/ Viginere
	printf("\nViginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	printf("Received message: %s\n", (char*)msgIn);
	printf("\n============================================================================\n");


	/*
	Sleep(5);

	// receive audio
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ Huffman
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ Huffman & XOR
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ Huffman & Viginere
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ RLE
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ RLE & XOR
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ RLE & Viginere
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ XOR
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive audio w/ Viginere
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	*/
	system("pause");
	return 1;
}