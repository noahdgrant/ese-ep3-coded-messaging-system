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
void transmitCom(void* txMsg, long txMsgSz);
int receiveCom(void** rxMsg, long &rxMsgSz);

// GUI Options
void selectComPort();
void changeAudioSettings();
void setRID();
void setSID();
void setEncryption();
void setSecretKey();

// ENCRYPT/DECRYPT MESSAGE
void decrypt(void* msg, int msgSz);
void encrypt(void* msg, int msgSz);

// COMPRESS/DECOMPRESS MESSAGE
void compress(void* msg);
void decompress(void* msg);
void setCompression();