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



	txHeader.payloadType = mTXT;

	// send text
	printf("plain text\n");
	strcpy((char*)"plain text message", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = NONE;
	txHeader.encryption = NONE;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ Huffman
	printf("Huffman\n");
	strcpy((char*)"Compressed with huffman", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cHUF;
	txHeader.encryption = NONE;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ Huffman & XOR
	printf("Huffman & XOR\n");
	strcpy((char*)"Compressed and encrypted with huffman and XOR", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cHUF;
	txHeader.encryption = XOR;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ Huffman & Viginere
	printf("Huffman & Viginere\n");
	strcpy((char*)"Compressed and encrypted with huffman and Viginere", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cHUF;
	txHeader.encryption = VIG;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ RLE
	printf("RLE\n");
	strcpy((char*)"Compressed with RLE", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cRLE;
	txHeader.encryption = NONE;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ RLE & XOR
	printf("RLE & XOR\n");
	strcpy((char*)"Compressed and encrypted with RLE and XOR", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cRLE;
	txHeader.encryption = XOR;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ RLE & Viginere
	printf("RLE & Viginere\n");
	strcpy((char*)"Compressed and encrypted with RLE and Viginere", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cRLE;
	txHeader.encryption = VIG;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ XOR
	printf("XOR\n");
	strcpy((char*)"Encrypted with XOR", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
	txHeader.compression = cNONE;
	txHeader.encryption = XOR;
	encrypt(msg, txHeader.payloadSize);
	compress(txHeader, &msg);
	transmitCom(&txHeader, msg);
	// send text w/ Viginere
	printf("Viginere\n");
	strcpy((char*)"Encrypted with Viginere", (char*)msg);
	txHeader.payloadSize = txHeader.uncompressedLength = strlen((char*)msg) + 1;
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
	return 1;
}






int testingin() {
	void* msgIn = NULL;											// Pointer to recieved message buffer
	Header rxHeader = {};

	// receive text
	printf("Plain Text\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn); 
	decrypt(rxHeader, msgIn);
	// receive text w/ Huffman
	printf("Huffman\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ Huffman & XOR
	printf("Huffman & XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ Huffman & Viginere
	printf("Huffman & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ RLE
	printf("RLE\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ RLE & XOR
	printf("RLE & XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ RLE & Viginere
	printf("RLE & Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ XOR
	printf("XOR\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
	// receive text w/ Viginere
	printf("Viginere\n");
	receiveCom(&rxHeader, &msgIn);
	decompress(rxHeader, &msgIn);
	decrypt(rxHeader, msgIn);
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