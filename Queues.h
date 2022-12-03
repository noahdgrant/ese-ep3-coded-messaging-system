/***********************************************************
* Name:			queues.h
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Queue interface.
************************************************************/

#pragma once

#include "header.h"

/***********************************************************
* Specific variables
************************************************************/

#define MAX_QUOTE_LENGTH 140

typedef struct node Node;
typedef struct item Item;
typedef Node* link;

struct item {
	Header msgHeader;					// Header of the sent message
	char message[];						// Data contained in message. Dynamic struct space URL: https://www.geeksforgeeks.org/flexible-array-members-structure-c/
};

struct node {
	link pNext;					// Pointer to next node
	Item Data;					// Data in node
};

/***********************************************************
* Function prototypes
************************************************************/

// Queue functions prototypes
void initQueue(void);			// Initailize the queue
int isQueueEmpty(void);			// Check if the queue is empty
void pushQ(link pn);			// Push item on to queue
link popQ(void);				// Pop item off of queue

// Recursive queue functions
link listHead();									// Return the pointer to the head of the queue
int count(link x);									// Counts the number of nodes in the queue
link deleteR(link parent, link child, Item v);		// Delete a node if it matches a certain item
void visit(link h);									// Print the SID of the current node
void traverse(link h, void (*visit)(link));			// Traverse the LL from head to tail
void traverseR(link h, void (*visit)(link));		// Traverse the LL from tail to head

// Queue recived messages
int qRxMsg(Header rxHeader, void* rxMsg);				// Queue recieved message
void printNode(link h, int i);										// Print current node
void printRxMsgs();													// Print recieved messages from oldest to newest
void countAndTraverse(link h, void (*visit)(link, int i), int i);
link deleteMsg(link parent, link child, Item v, int& numMsgs);