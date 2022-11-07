/* CMSLibrary.h -Interface: Header file for extra CMS functions
*  By: Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

// MENU
// Print CMS menu
void printMenu();

// MESSAGES
// Generate a random quote from FortuneCookies.txt file, and ask the user if they want to add it to the queue
int generateQuote();

//AUDIO
// Playback saved audio file
int playbackAudio();

// Record audio, play it back to the user, and ask if they want to save the file
int recordAudio();

// SERIAL COMMUNICATION
void transmitCom(char* msgOut, unsigned long msgSz);
void receiveCom();

// GUI Options
void selectComPort();
void changeAudioSettings();
void setSecretKey();
void setRID();
void setSID();
void setEncryption();

// Encryption
void xorCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, void* encBuf);
void vigCipher(void* message, int messageLength, void* secretKey, int secretKeyLength, void* encBuf, bool encOrDec);