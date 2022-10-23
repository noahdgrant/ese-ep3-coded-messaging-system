/* CMSLibrary.h -Interface: Header file for extra CMS functions
*  By: Ian Edwards, Noah Grant, Wyatt Richard
*  Version: 01.00
*/

#pragma once

// Print CMS menu
void printMenu();

// Record audio, play it back to the user, and ask if they want to save the file
int recordAudio();

// Playback saved audio file
int playbackAudio();

// Generate a random quote from FortuneCookies.txt file, and ask the user if they want to add it to the queue
int generateQuote();