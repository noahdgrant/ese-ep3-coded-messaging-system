/***********************************************************
* Name:			BSTLibrary.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	BSTLibrary implementation.
************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BSTLibrary.h"

// Private variables
static link root;														// Root of the BST. Private to this file

// FUNCTION IMPLEMENTATION
// PRIVATE: Returns number of nodes in BST
static int _countBST(link node) {
	if (node == NULL) return (0);										// Base case
	return(_countBST(node->pLeft) + _countBST(node->pRight) + 1);
}

// PUBLIC: Returns height the BST
int countBST(void) {
	return(_countBST(root));
}

// PRIVATE: Delete function called by 'deleteBST()'
static void _deleteBST(link node) {
	if (node == NULL) return;											// Base case

	// Recursive subcalls
	_deleteBST(node->pLeft);
	_deleteBST(node->pRight);

	//printf("\nDeleting from BST: %s\n", node->msg.buff);				// Just for testing purposes

	free(node);
	return;
}

// PUBLIC: Frees each node in the BST
void deleteBST(void) {
	_deleteBST(root);
	return;
}

// Returns a pointer to the root of the BST
link getRoot(void) {
	return(root);
}

// PRIVATE: Returns height of BST
static int _heightBST(link node) {
	int lHeight = 0;													// Height of left side of BST
	int rHeight = 0;													// Height of right side of BST

	if (node == NULL) return(0);										// Base case. Leaf reached

	// Recursive subcalls
	lHeight = _heightBST(node->pLeft);
	rHeight = _heightBST(node->pRight);

	if (lHeight > rHeight) return(lHeight + 1);
	else return(rHeight + 1);
}

// PUBLIC: Print the height of the BST
int heightBST(void) {
	return(_heightBST(root));
}

// Initialize a BST
void initBST(void) {												
	root = NULL;														// Initialize empty BST
}	

// PRIVATE: Insert function called by 'insertBST()'
static link _insertBST(link node, Item item) {									
	if (node == NULL) return(NEW(item, NULL, NULL));

	// Recursive subcalls for traversing the tree to get to a leaf
	if (item.key < node->msg.key) node->pLeft = _insertBST(node->pLeft, item);
	else node->pRight = _insertBST(node->pRight, item);
	return(node);														// Returns pointer to the node that is created
}	

// PUBLIC: BST insert function
void insertBST(Item item) {											
	root = _insertBST(root, item);										// Insert the new item, starting at the root
	return;
}	

// Creates a new BST node
link NEW(Item item, link left, link right) {
	link pNew = (link)malloc(sizeof(BSTNode));
	pNew->msg = item;
	pNew->pLeft = left;
	pNew->pRight = right;
	return(pNew);														// Return pointer to new BST node
}

int count;
// PRIVATE: Print function called by 'printBST()'. 'Inorder' traversal
static void _printInorderBST(link node) {
	if (node == NULL) return;

	// Recursive subcalls
	_printInorderBST(node->pLeft);								// Print left
	node->msg.listNum = count++;
	printf("\n%d: ID: %d, Name: %s\n", node->msg.listNum, node->msg.key, node->msg.buff);		// Print centre
	_printInorderBST(node->pRight);								// Print right
}

// PUBLIC: print BST
void printInorderBST(void) {
	count = 1;
	_printInorderBST(root);
	return;
}

// PRIVATE: Search function called by 'searchBST()'
static Item _searchBST(link node, char* searchKey) {							
	int i = 0;															// Holds strcmp() return value

	if (node == NULL) return(Item{ NULL });								// Recursion base case
	i = strcmp(searchKey, node->msg.buff);
	if (i == 0) return(node->msg);										// Return the message if they match

	// Recursive subcalls
	if (i < 0) return (_searchBST(node->pLeft, searchKey));				// Check all left branches
	else return(_searchBST(node->pRight, searchKey));					// Check all right branches
}

// PUBLIC: BST search function
Item searchBST(char* searchKey) {											
	return(_searchBST(root, searchKey));
}	

// PRIVATE: Search function called by 'checkKey()'
static int _checkKey(link node, int searchKey) {
	if (node == NULL) return(0);								// Recursion base case

	if (node->msg.key == searchKey) return(node->msg.key);										// Return the message if they match

	// Recursive subcalls
	if (searchKey < node->msg.key) return (_checkKey(node->pLeft, searchKey));				// Check all left branches
	else return(_checkKey(node->pRight, searchKey));					// Check all right branches
}

// PUBLIC: BST search function
int checkKey(int searchKey) {
	return(_checkKey(root, searchKey));
}

/*************************************************************************
* _selectFromList() - finds the node with the listNume that is equal to searchkey and returns the nodes Key
* noce			- root of the bst.
* searchKey		- the desired list number.
*************************************************************************/
static int _selectFromList(link node, int searchKey) {
	if (node == NULL) return(0);													// Recursion base case

	if (node->msg.listNum == searchKey) return(node->msg.key);						// Return the message if they match

	// Recursive subcalls
	if (searchKey < node->msg.listNum) return (_selectFromList(node->pLeft, searchKey));		// Check all left branches
	else return(_selectFromList(node->pRight, searchKey));								// Check all right branches
}

/*************************************************************************
* selectFromList()	- calls the private function _selectFromList
* searchKey			- the desired list number
*************************************************************************/
int selectFromList(int searchKey) {
	return(_selectFromList(root, searchKey));
}