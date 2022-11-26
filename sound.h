/*	Filename: sound.h 
	Author: Michael Galle
	Date: Updated 2022
	Details: Interface - Contains prototypes for Windows sound API (sound recording and playback functions)
*/
#pragma once

#include <windows.h>					// Contains WAVEFORMATEX structure

// CONSTANTS - SET RECORDING TIME AND RATE
#define	DEFAULT_NSAMPLES	4000	
#define MIN_BUFSIZE			1000
#define RECORD_TIME			2		// seconds to record from mic
#define SAMPLES_SEC			8000	// number of samples per second
#define	NFREQUENCIES		96		// number of frequencies used - leave this alone
extern int global;
extern int recordTime;								// Default record time
extern long numAudioBytes;							// Size of audio buffer

// FUNCTION PROTOTYPES
/* ************************ */
// Playback
int InitializePlayback(void);
int PlayBuffer(short *piBuf, long lSamples);
void ClosePlayback(void);

// Recording
int InitializeRecording(void);
int	RecordBuffer(short *piBuf, long lBufSize);
void CloseRecording(void);

// Support functions for Playback functions (updated 2021)
void SetupFormat(WAVEFORMATEX* wf);			// Used by InitializePlayback()					
int WaitOnHeader(WAVEHDR* wh, char cDit);	// Used by Playbuffer()

//AUDIO
int playbackAudio();		// Playback saved audio file
int recordAudio();			// Record audio, play it back to the user, and ask if they want to save the file

void changeAudioSettings();