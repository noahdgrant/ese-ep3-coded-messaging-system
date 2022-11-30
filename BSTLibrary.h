/***********************************************************
* Name:			BSTLibrary.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	BSTLibrary interface.
************************************************************/

#pragma once

// Tree node structure and functions
const int maxL = 140;
typedef struct item Item;
typedef struct BSTNode* link;

// Define BST item and node structure
struct item {
	char buff[maxL] = "\0";					// Holds string in BST node.
	int key = 0;							// 'Key' for the BST.
	int listNum = 0;
};

struct BSTNode {
	Item msg;								// msg contains the nodes data
	link pLeft;								// Left subtree
	link pRight;							// Right subtree
};

/***********************************************************
* Function prototypes
************************************************************/

int _countBST(link node);						// PRIVATE: Returns number of nodes in BST
int countBST(void);								// PUBLIC: Returns height the BST
static void _deleteBST(link node);				// PRIVATE: Delete function called by 'deleteBST()'
void deleteBST(void);							// PUBLIC: Frees each node in the BST
link getRoot(void);								// Returns a pointer to the root of the BST
int _heightBST(link node);						// PRIVATE: Returns height of BST
int heightBST(void);							// PUBLIC: Print the height of the BST
link _insertBST(link node, Item item);			// PRIVATE: Insert function called by 'insertBST()'
void insertBST(Item item);						// PUBLIC: BST insert function
void initBST(void);								// Initialize a BST
link NEW(Item item, link left, link right);		// Creates a new BST node
void _printInorderBST(link node);				// PRIVATE: Print function called by 'printBST()'. 'Inorder' traversal
void printInorderBST(void);						// PUBLIC: print BST
Item _searchBST(link node, char* searchKey);	// PRIVATE: Search function called by 'searchBST()'
Item searchBST(char* searchKey);				// PUBLIC: BST search function
static int _checkKey(link node, int searchKey);
int checkKey(int searchKey);
static int _selectFromList(link node, int searchKey);
int selectFromList(int searchKey);
