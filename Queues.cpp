/* Queues.cpp - Implementation for the Queue functionality
*  Author: Ian Edwards, Noah Grant, Wyatt Richard
*  Version: 01.00
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "header.h"
#include "sound.h"
#include "Queues.h"
#include "RS232Comm.h"

static link pHead, pTail;

// BASIC QUEUE FUNCTIONALITY
// Start a new linked-list
void initQueue(void) {
	pHead = pTail = NULL;
} // initQueue

// Check if the linked-list is empty
int isQueueEmpty(void) {
	return (pHead == NULL);
} // isQueueEmpty

// Add new item to head of queue
void pushQ(link pn) {
	if (isQueueEmpty()) {
		pHead = pTail = pn;
	}
	else {
		pTail->pNext = pn;		// Old tail not points to pn (the node we added)
		pTail = pn;				// pTail is the pointer that keeps track of the end of the node
	}
	pTail->pNext = NULL;
} // pushQ

// Remove the first item from the queue
link popQ(void) {
	link pTemp;					// Hold current head
	if (isQueueEmpty()) {
		return (NULL);
	}
	else {
		pTemp = pHead;			// pHead is returned and then incremented to next node in list
		pHead = pHead->pNext;
		return (pTemp);
	}
} // popQ

// Recursive queue functions
// Return a pointer to the head of the linked-list
link listHead() {
	return pHead;
} // returnHead

// Count the number of nodes in the linked-list
int count(link h) {
	if (h == NULL) return 0;
	return (1 + count(h->pNext));				// Recursively go to the end of the linked and add one each time
} // count

// Delete a node in the linked-list based on SID
link deleteR(link parent, link child, Item v) {
	if (child == NULL) return(NULL);
	if (child->Data.msgHeader.sid == v.msgHeader.sid) {				// If the SID of the child matches what we are looking for, delete the child node
		parent->pNext = child->pNext;			// Set the parent to point at the node the child currently points to
		free(child);
		deleteR(parent, parent->pNext, v);		// Recursively check the new child element to see if it matches the search
	}
	else {
		deleteR(child, child->pNext, v);		// If the child data doesn't match, update the parent to the current child, and the child to the next element
	}
} // deleteR

// Prints the message of the current node
void visit(link h) {
	printf("\nNODE MESSAGE:\n%s\n", h->Data.message);	// Print the SID of the current node in the linked-list
}

// Traverse the linked-list from head to tail
void traverse(link h, void (*visit)(link)) {
	if (h == NULL) return;						// Reached the end of the linked-list
	(*visit)(h);								// Call visit() for the current node
	traverse(h->pNext, visit);					// Recursively go to the next node
} // traverse

// Traverse the linked-list from tail to head
void traverseR(link h, void (*visit)(link)) {
	if (h == NULL) return;						// Reached the end of the linked-list
	traverseR(h->pNext, visit);					// Recursively go to the next node until the end is reached
	(*visit)(h);								// Call visit() for reach node as the recursion unwinds. Prints the linked-list backwards
} // traverse R

/****************************************************/

// Traverse linked-list and keep track of the current position
void countAndTraverse(link h, void (*visit)(link, int i), int i) {
	if (h == NULL) return;						// Reached the end of the linked-list
	(*visit)(h, i);								// Call visit() for the current node
	h->Data.msgHeader.seqNum = i;
	countAndTraverse(h->pNext, visit, i+1);		// Recursively go to the next node
}

// Queue recieved message
int qRxMsg(Header rxHeader, void* rxMsg, int msgSz) {
	link p = NULL;															// Pointer to memory where quere node will be stored

	p = (link)calloc(1, sizeof(Node) + msgSz);								// Calloc() URL: https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
	if (p == NULL) {														// Make sure memory was allocated
		printf("\nERROR: Couldn't malloc memory for recieved message.\n");
		return (-1);
	}

	p->Data.msgHeader = rxHeader;					// Copy recieved header to node
	
	for (int i = 0; i < msgSz; i++) {
		p->Data.message[i] = ((char*)rxMsg)[i];		// Copy recieved message to node
	}
	pushQ(p);

	return(0);
}

// Print current node
void printNode(link h, int i) {
	char cmd = '\0';

	printf("\nMESSAGE #%d\n", i);

	// Print message header
	printf("\nMESSAGE HEADER\n");
	printf("SID: %d\n", h->Data.msgHeader.sid);
	printf("RID: %d\n", h->Data.msgHeader.rid);
	printf("Priority: %d\n", h->Data.msgHeader.priority);
	if (h->Data.msgHeader.payloadType == mTXT) {
		printf("Payload Type: Text\n");
	}
	else if (h->Data.msgHeader.payloadType == mAUD) {
		printf("Payload Type: Audio\n");
	}
	printf("Payload Size: %d\n", h->Data.msgHeader.payloadSize);

	// Print message
	printf("\nMESSAGE:\n");
	if (h->Data.msgHeader.payloadType == mTXT) {
		printf("%s\n", h->Data.message);
	}
	else if (h->Data.msgHeader.payloadType == mAUD) {
		printf("\nDo you want to listen to stored audio message?\n");
		printf("> ");
		scanf_s("%c", &cmd, 1);
		while (getchar() != '\n') {}
		if (cmd == 'y' || cmd == 'Y') {
			printf("\nPlaying audio message...\n");
			InitializePlayback();
			PlayBuffer((short*)h->Data.message, h->Data.msgHeader.payloadSize / 2);			// /2 since it was *2 to send the chars but now needs to be read as shorts
			ClosePlayback();
			Sleep(250);
		}
		else {
			printf("\nAudio message.\n");
		}
	}
	printf("\n*********************************\n");
}

// Print recieved messages from oldest to newest
void printRxMsgs() {
	countAndTraverse(listHead(), printNode, 1);		// Pass the number you want to list to start counting at
}

// Delete a node in the linked-list based on sequence number
link deleteMsg(link parent, link child, Item v, int &numMsgs) {
	if (child == NULL) {
		printf("\nMessage #%d not found.\n", v.msgHeader.seqNum);
		return(NULL);
	}
	if (child->Data.msgHeader.seqNum == v.msgHeader.seqNum) {				// If the sequence number of the child matches what we are looking for, delete the child node
		parent->pNext = child->pNext;										// Set the parent to point at the node the child currently points to
		free(child);
		printf("Message #%d successfully deleted!\n", v.msgHeader.seqNum);
		numMsgs--;
	}
	else {
		deleteMsg(child, child->pNext, v, numMsgs);							// If the child data doesn't match, update the parent to the current child, and the child to the next element
	}
} // deleteR