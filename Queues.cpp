/* Queues.cpp - Implementation for the Queue functionality
*  Author: Ian Edwards, Noah Grant, Wyatt Richard
*  Version: 01.00
*/
#include <stdio.h>
#include <stdlib.h>

#include "Queues.h"

static link pHead, pTail;

// Start a new linked-list
void initQueue(void) {
	pHead = pTail = NULL;
} // initQueue

// Check if the linked-list is empty
int isQueueEmpty(void) {
	return (pHead == NULL);
} // isQueueEmpty

// Add a new item to the front of the queue
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

// Delete a node in the linked-list
link deleteR(link parent, link child, Item v) {
	if (child == NULL) return(NULL);
	if (child->Data.sid == v.sid) {				// If the SID of the child matches what we are looking for, delete the child node
		parent->pNext = child->pNext;			// Set the parent to point at the node the child currently points to
		free(child);
		deleteR(parent, parent->pNext, v);		// Recursively check the new child element to see if it matches the search
	}
	else {
		deleteR(child, child->pNext, v);		// If the child data doesn't match, update the parent to the current child, and the child to the next element
	}
} // deleteR

// Prints the SID of the current node
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