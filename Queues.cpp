/***********************************************************
* Name:			queues.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Queue implementation.
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "header.h"
#include "sound.h"
#include "Queues.h"
#include "RS232Comm.h"

/***********************************************************
* Specific variables
************************************************************/

static link pHead, pTail;
int printQueueSID = 0;
/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/

/*************************************************************************
* initQueue() - Initiate a new queue (linked-list).
*************************************************************************/
void initQueue(void) {
	pHead = pTail = NULL;
}

/*************************************************************************
* isQueueEmpty() - Check if queue is empty.
* This function return 1 if queue is empty. Otherwise, it return 0.
*************************************************************************/
int isQueueEmpty(void) {
	return (pHead == NULL);
}

/*************************************************************************
* pushQ() - Add new item to the tail queue.
* pn		- Pointer to the item to add to the queue.
*************************************************************************/
void pushQ(link pn) {
	if (isQueueEmpty()) {
		pHead = pTail = pn;
	}
	else {
		pTail->pNext = pn;		// Old tail not points to pn (the node we added)
		pTail = pn;				// pTail is the pointer that keeps track of the end of the node
	}
	pTail->pNext = NULL;
}

/*************************************************************************
* popQ() - Remove the item at the head of the queue.
* This function returns a pointer to the new head of the queue. If the queue is empty, it returns NULL.
*************************************************************************/
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

/*************************************************************************
* listHead() - Remove the item at the head of the queue.
* This function returns a pointer to the head of the queue.
*************************************************************************/
link listHead() {
	return pHead;
}

/*************************************************************************
* count() - Counts the number of items in the queue from a specified starting point.
* h		- The position in the queue to begin counting.
* This function returns the number of items in the queue.
*************************************************************************/
int count(link h) {
	if (h == NULL) return 0;
	return (1 + count(h->pNext));				// Recursively go to the end of the linked and add one each time
}

/*************************************************************************
* deleteR() - Delete a node from the queue based on sender ID.
* parent		- The point in the queue to start looking at.
* child			- The node after the first node.
* v				- An item with the thing you want to delete.
* This function returns returns NULL once it reaches the end of the queue.
*************************************************************************/
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
}

/*************************************************************************
* visit() - Prints the current node's message.
* h		- Current node.
*************************************************************************/
void visit(link h) {
	if (printQueueSID == h->Data.msgHeader.rid) {
		printf("\nNODE MESSAGE:\n%s\n", h->Data.message);	// Print the SID of the current node in the linked-list
	}
}

/*************************************************************************
* traverse() - Traverse the queue from head to tail.
* h			- The head of the queue.
* visit		- Function to perform on current node.
*************************************************************************/
void traverse(link h, void (*visit)(link)) {
	if (h == NULL) return;						// Reached the end of the linked-list
	(*visit)(h);								// Call visit() for the current node
	traverse(h->pNext, visit);					// Recursively go to the next node
}

/*************************************************************************
* traverseR() - Travese the queue from tail to head.
* h			- The head of the queue.
* visit		- Function to perform on current node.
*************************************************************************/
void traverseR(link h, void (*visit)(link)) {
	if (h == NULL) return;						// Reached the end of the linked-list
	traverseR(h->pNext, visit);					// Recursively go to the next node until the end is reached
	(*visit)(h);								// Call visit() for reach node as the recursion unwinds. Prints the linked-list backwards
}

/*************************************************************************
* countAndTraverse() - Count the number of item in queue, give each one a number based on it's position, and print it's message.
* h			- The head of the queue.
* visit		- Function to perform on current node.
* i			- The number to begin counting at.
*************************************************************************/
void countAndTraverse(link h, void (*visit)(link, int i), int i) {
	if (h == NULL) return;						// Reached the end of the linked-list
	(*visit)(h, i);								// Call visit() for the current node
	h->Data.msgHeader.seqNum = i;
	countAndTraverse(h->pNext, visit, i+1);		// Recursively go to the next node
}

/*************************************************************************
* qRxMsg() - Push recieved message to tail of queue.
* rxHeader		- Received header.
* rxMsg			- Received message.
* msgSz			- Message size.
* This function return 0 if successful or -1 if it's not.
*************************************************************************/
int qRxMsg(Header rxHeader, void* rxMsg) { 
	link p = NULL;															// Pointer to memory where quere node will be stored

	p = (link)calloc(1, sizeof(Node) + rxHeader.payloadSize);								// Calloc() URL: https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
	if (p == NULL) {														// Make sure memory was allocated
		printf("\nERROR: Couldn't malloc memory for recieved message.\n");
		return (-1);
	}

	p->Data.msgHeader = rxHeader;					// Copy recieved header to node
	
	for (int i = 0; i < rxHeader.payloadSize; i++) {
		p->Data.message[i] = ((char*)rxMsg)[i];		// Copy recieved message to node
	}
	pushQ(p);

	return(0);
}

/*************************************************************************
* printNode() - Display current nodes message.
* h			- The head of the queue.
* i			- The messages position in the queue.
*************************************************************************/
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

/*************************************************************************
* printRxMsgs() - Display messages in queue from head to tail (oldest to newnest).
*************************************************************************/
void printRxMsgs() {
	countAndTraverse(listHead(), printNode, 1);		// Pass the number you want to list to start counting at
}

/*************************************************************************
* deleteMsg() - Delete item in queue based on its position.
* parent		- Head of queue.
* child			- second item in the queue.
* v				- Item the holds the number of the item to be removed.
* numMsgs		- Current number of items in the queue.
* This function returns NULL once it reaches the tail of the queue.
*************************************************************************/
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