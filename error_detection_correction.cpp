/***********************************************************
* Name:			error_detection_correction.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Error correction and detection implementation.
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "error_detection_correction.h"
#include "header.h"

/*************************************************************************
*                            PRIVATE FUNCTIONS                           *
*************************************************************************/

/*************************************************************************
* voteOn() - Performs vote on error detection on the three passed in votes.
* votes		- Array of the votes to vote on.
* nVotes	- Number of votes.
* This function returns the vote is the majority, or -1 if all votes are different.
*************************************************************************/
static int voteOn(int votes[], int nVotes) {
    int maxCount = 0;                                   // Max number of repeats for current item
    int curCount;                                       // Current number of repeats
    int maxIndex = -1;                                  // The index of the item in the majority

    for (int i = 0; i < nVotes; i++)
    {
        curCount = 0;                                   // Reset counter for each loop

        for (int j = i + 1; j < nVotes; j++)
        {
            if (votes[i] == votes[j])                   // Compare memory blocks to see if they are the same
            {
                curCount++;
            }
        }

        if (curCount > maxCount)                        // Element at index i is in the majority
        {
            maxCount = curCount;
            maxIndex = i;
        }
    }

    return maxIndex;
}

/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* Checksum() - Performs vote on error detection on the three passed in votes.
* buf		- Input buffer to perform checksum operation on.
* iBufLen	- Length of input buffer.
* iType     - The type of checksum to be performed on the buffer (8 or 16 bit).
* This function returns the checksum.
*************************************************************************/
unsigned int Checksum(void* buf, int iBufLen, CHECKSUM iType)
{
    unsigned short* newBuf16; // short data type is 16 bits
    unsigned char* newBuf8;   // char data type is 8 bits
    unsigned int sum = 0;

    switch (iType)
    {
    case CHK_16BIT:
        newBuf16 = (unsigned short*)buf;

        for (int i = 0; i < iBufLen; i++)
        {
            sum += *newBuf16++; // Move to next buff space in memory after each sum
        }
        return (sum % 0x10000); // Or could use (sum & 0xFFFF)

    case CHK_8BIT:
        newBuf8 = (unsigned char*)buf;

        for (int i = 0; i < iBufLen; i++)
        {
            sum += *newBuf8++; // Move to next buff space in memory after each sum
        }
        return (sum % 0x100); // Or could use (sum & 0xFF)

    default:
        return (0x11111); // Signal error
    }
}

/*************************************************************************
* checkHeader() - Passes the received header to voteOn() for error detection and correction.
* h		- Received header.
* This function returns -1 if voteOn() fails (no majority), otherwise 0.
*************************************************************************/
int checkHeader(Header& h) {
    int votes[3] = {h.rid, h.rid2, h.rid3};				// Number of votes for voteOn()
    int voteResult = 0;					                // VoteOn() result

    voteResult = voteOn(votes, sizeof(votes) / sizeof(int));
    if (voteResult == -1) {
        printf("\nERROR: Receiver ID corrupted during transmission.\n");
        Sleep(2000);
        return(-1);
    }
    else {
        h.rid = voteResult;
    }
    return(0);
}