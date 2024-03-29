/***********************************************************
* Name:			sound.cpp
* Author(s):	Michael Galle, Noah Grant, Wyatt Richard
* Description:	Windows sound API (sound recording & playback) implementation.
************************************************************/

#pragma comment(lib, "Ws2_32.lib")	   // Make sure we are linking against the Ws2_32.lib library
#pragma comment(lib, "Winmm.lib")      // Make sure we are linking against the Winmm.lib library - some functions/symbols from this library (Windows sound API) are used
#include <stdio.h> 
#include <windows.h>					// Contains WAVEFORMATEX structure
#include <mmsystem.h>					
#include <math.h>

#include "sound.h"
#include "CMSLibrary.h"

/***********************************************************
* Specific variables
************************************************************/

// BUFFERS
extern int recordTime;
extern long numAudioBytes;			// total number of samples

//short iBigBuf[SAMPLES_SEC * RECORD_TIME];
long  lBigBufSize = SAMPLES_SEC * recordTime;	

// output and input channel parameters 
static	int			g_nSamplesPerSec = SAMPLES_SEC;
static	int			g_nBitsPerSample = 16;
static	HWAVEOUT	HWaveOut;				/* Handle of opened WAVE Out and In device */
static  HWAVEIN		HWaveIn;
static	WAVEFORMATEX WaveFormat;			/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
static  WAVEHDR	WaveHeader[NFREQUENCIES];	/* WAVEHDR structures - 1 per buffer */
static  WAVEHDR	WaveHeaderSilence;
static  WAVEHDR WaveHeaderIn;

int recordTime = 2;												// Default record time
long numAudioBytes = SAMPLES_SEC * recordTime;					// Size of audio buffer

/* PLAYBACK FUNCTIONS */
/* ********************************************************************************************* */

int	InitializePlayback(void)
{
	int	rc;
	// set up the format structure, needed for playback (and recording)
	SetupFormat(&WaveFormat);
	// open the playback device
	rc = waveOutOpen(&HWaveOut, WAVE_MAPPER, &WaveFormat, (DWORD)NULL, 0, CALLBACK_NULL);
	if (rc) {
		printf("Unable to open Output sound Device! Error %x.", rc);
		return(0);
	}
	return(1);
}

int PlayBuffer(short *piBuf, long lSamples)
{
	static	WAVEFORMATEX WaveFormat;	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	static  WAVEHDR	WaveHeader;			/* WAVEHDR structure for this buffer */
	MMRESULT mmErr;
	int	rc;

	// stop previous note (just in case)
	waveOutReset(HWaveOut);   // is this good?

	// get the header ready for playback
	WaveHeader.lpData = (char *)piBuf;
	WaveHeader.dwBufferLength = lSamples * sizeof(short);
	rc = waveOutPrepareHeader(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	if (rc) {
		printf("Failed preparing WAVEHDR, error 0x%x.", rc);
		return(0);
	}
	WaveHeader.dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);

	// play the buffer. This is NON-blocking.
	mmErr = waveOutWrite(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	// wait for completion
	rc = WaitOnHeader(&WaveHeader, 0);
	// give back resources
	waveOutUnprepareHeader(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	return(rc);
}

void ClosePlayback(void)
{
	int		i;
	for (i = 0; i < NFREQUENCIES; ++i) {
		waveOutUnprepareHeader(HWaveOut, &WaveHeader[i], sizeof(WAVEHDR));
	}
	// close the playback device
	waveOutClose(HWaveOut);
	return;
}

/* RECORDING FUNCTIONS */
/* ********************************************************************************************* */
int InitializeRecording(void)
{
	MMRESULT rc;
	short* iBigBuf;

	iBigBuf = (short*)malloc(numAudioBytes * sizeof(short));
	if (iBigBuf == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		return(-1);
	}

	// set up the format structure, needed for recording.
	SetupFormat(&WaveFormat);

	// open the recording device
	rc = waveInOpen(&HWaveIn, WAVE_MAPPER, &WaveFormat, (DWORD)NULL, 0, CALLBACK_NULL);
	if (rc) {
		printf("Unable to open Input sound Device! Error %x.", rc);
		return(0);
	}

	// prepare the buffer header for use later on
	WaveHeaderIn.lpData = (char *)iBigBuf;
	WaveHeaderIn.dwBufferLength = numAudioBytes * sizeof(short);
	rc = waveInPrepareHeader(HWaveIn, &WaveHeaderIn, sizeof(WAVEHDR));
	if (rc) {
		printf("Failed preparing input WAVEHDR, error 0x%x.", rc);
		return(-1);
	}

	return(0);
}

int	RecordBuffer(short *piBuf, long lBufSize)
{
	static	WAVEFORMATEX WaveFormat;	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	static  WAVEHDR	WaveHeader;			/* WAVEHDR structure for this buffer */
	MMRESULT	mmErr;
	int		rc;

	printf("Recording now.....");

	// stop previous recording (just in case)
	waveInReset(HWaveIn);   // is this good?

	// get the header ready for recording.  This should not be needed here AND in init.
	WaveHeader.lpData = (char *)piBuf;
	WaveHeader.dwBufferLength = lBufSize * sizeof(short);
	rc = waveInPrepareHeader(HWaveIn, &WaveHeader, sizeof(WAVEHDR));
	if (rc) {
		printf("Failed preparing WAVEHDR, error 0x%x.", rc);
		return(0);
	}
	WaveHeader.dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);

	// Give the buffer to the recording device to fill.
	mmErr = waveInAddBuffer(HWaveIn, &WaveHeader, sizeof(WAVEHDR));
	// Record the buffer. This is NON-blocking.
	mmErr = waveInStart(HWaveIn);
	// wait for completion
	rc = WaitOnHeader(&WaveHeader, '.');
	// stop input
	waveInStop(HWaveIn);
	return(rc);
}

void CloseRecording(void)
{
	waveInUnprepareHeader(HWaveIn, &WaveHeaderIn, sizeof(WAVEHDR));
	// close the playback device
	waveInClose(HWaveIn);
	return;
}

/* SUPPORT FUNCTIONS USED BY PLAYBACK FUNCTIONS - Updated 2021 */
/* ********************************************************************************************* */
// Function needed by InitializePlayback() - SetupFormat() initializes a WAVEFORMATEX structure to the required parameters (sample rate, bits per sample, etc) 
static void SetupFormat(WAVEFORMATEX* wf)
{
	wf->wFormatTag = WAVE_FORMAT_PCM;
	wf->nChannels = 1;
	wf->nSamplesPerSec = g_nSamplesPerSec;
	wf->wBitsPerSample = g_nBitsPerSample;
	wf->nBlockAlign = wf->nChannels * (wf->wBitsPerSample / 8);
	wf->nAvgBytesPerSec = wf->nSamplesPerSec * wf->nBlockAlign;
	wf->cbSize = 0;
	return;
}

// Function used by Playbuffer() to wait for whatever is playing to finish or quit after waiting 10 seconds
static int WaitOnHeader(WAVEHDR* wh, char cDit)
{
	long	lTime = 0;
	// wait for whatever is being played, to finish. Quit after 10 seconds.
	for (; ; ) {
		if (wh->dwFlags & WHDR_DONE) return(1);
		// idle for a bit so as to free CPU
		Sleep(100L);
		lTime += 100;
		if (lTime >= 10000) return(0);  // timeout period
		if (cDit) printf("%c", cDit);
	}
}

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* playbackAudio() - Play saved audio file.
* This function return 0 if it successful. Otherwise, it return -1.
*************************************************************************/
int playbackAudio() {
	FILE* f;						// pointer to file
	short* playbackBuf = NULL;		// buffer used for reading recorded sound from file

	playbackBuf = (short*)malloc(numAudioBytes * sizeof(short));
	if (playbackBuf == NULL) {
		return(-1);
	}

	// replay audio recording from file -- read and store in buffer, then use playback() to play it
	/* Open input file */
	fopen_s(&f, "C:\\myfiles\\recording.dat", "rb");
	if (!f) {
		printf("unable to open %s\n", "C:\\myfiles\\recording.dat");
		Sleep(1500);
		return 0;
	}
	printf("Reading from sound file ...\n");
	fread(playbackBuf, sizeof(short), numAudioBytes, f);				// Record to new buffer iBigBufNew
	fclose(f);

	InitializePlayback();
	printf("\nPlaying recording from saved file ...\n");
	PlayBuffer(playbackBuf, numAudioBytes);
	ClosePlayback();

	free(playbackBuf);
	playbackBuf = NULL;
	return 0;
}

/*************************************************************************
* recordAudio() - Record audio, play it back to the user, and ask if they want to save the file.
* This function return 0 if it successful. Otherwise, it return -1.
*************************************************************************/
int recordAudio() {
	FILE* f = NULL;								// Pointer to file
	short* recordBuf = NULL;					// Pointer to record buffer
	char save = '\0';							// Holds wether or not the user wans to save the recording			

	recordBuf = (short*)malloc(numAudioBytes * sizeof(short));
	if (recordBuf == NULL) {
		printf("\nERROR: Couldn't malloc memory to record audio.\n");
		return(-1);
	}

	// initialize playback and recording
	InitializePlayback();
	InitializeRecording();

	// start recording
	RecordBuffer(recordBuf, numAudioBytes);
	CloseRecording();

	// playback recording 
	printf("\nPlaying recording from buffer\n");
	PlayBuffer(recordBuf, numAudioBytes);
	ClosePlayback();

	// save audio recording  
	printf("Would you like to save your audio recording? (y/n): ");
	scanf_s("%c", &save, 1);
	while (getchar() != '\n') {}										// Flush other input

	if ((save == 'y') || (save == 'Y')) {
		/* Open input file */
		fopen_s(&f, "C:\\myfiles\\recording.dat", "wb");
		if (!f) {
			printf("unable to open %s\n", "C:\\myfiles\\recording.dat");
			Sleep(1500);
			return 0;
		}
		printf("Writing to sound file ...\n");
		fwrite(recordBuf, sizeof(short), numAudioBytes, f);
		fclose(f);
	}

	free(recordBuf);
	recordBuf = NULL;
	return 0;
} // recordAudio()

/*************************************************************************
* changeAudioSettings() - Change the length of recorded audio message (in seconds).
*************************************************************************/
void changeAudioSettings() {
	char cmd[3];				// Holds the length of time the user wants to record audio
	do {
		system("cls");
		printf("Enter a new recording length between 1 and 15 seconds\n");
		printf("\n> ");
		fflush(stdin);														// Flush input buffer after use. Good practice in C
		scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
		while (getchar() != '\n') {}										// Flush other input buffer

		cmd[2] = '\0';
		if (atoi(cmd) >= 1 && atoi(cmd) <= 15) {
			printf("\nThe new recording length is now %d\n", atoi(cmd));
			recordTime = atoi(cmd);
			numAudioBytes = SAMPLES_SEC * recordTime;
		}
		else {
			printf("You did not enter a valid command. Please try again.");
		}
		Sleep(2000);

	} while (atoi(cmd) < 1 || atoi(cmd) > 15);
}