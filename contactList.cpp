/***********************************************************
* Name:			contactList.cpp
* Author(s):	Noah Grant, Wyatt Richard
* Description:	Contact List implementation.
************************************************************/

#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BSTLibrary.h"

char fName[30] = {}, fID[30] = {};
FILE* f;
errno_t err;
Item* p = NULL;

/*************************************************************************
* initContact() - Generates a contatc list from the phonebook.txt file.
*************************************************************************/
void initContact() {
	initBST();												// Initialize BST
	err = fopen_s(&f, "phonebook.txt", "rb");
	if (f) {
		do {
			fgets(fID, sizeof(fID), f);
			fgets(fName, sizeof(fName), f);
			if (feof(f)) break;
			//add to bst
			p = (Item*)malloc(sizeof(Item));					// Pointer to item
			if (p == NULL) {									// Make sure memory was allocated
				return;
			}
			strcpy(p->buff, fName);								// Copy to BST buffer
			p->key = atoi(fID);
			insertBST(*p);										// Insert into BST

		} while (!feof(f));
		fclose(f);
	}
	return;
}

/*************************************************************************
* addContact() - Adds a contact to the bst and phonebook.txt file
*************************************************************************/
void addContact() {
	printf("Name: ");
	for (int i = 0; i < 29; i++) {
		scanf("%c", &fName[i]);
		if (fName[i] == '\n') {
			fName[i + 1] = '\0';
			break;
		}
	}
	printf("ID: ");
	for (int i = 0; i < 9; i++) {
		scanf("%c", &fID[i]);
		if (fID[i] == '\n') {
			break;
		}
	}
	if (checkKey(atoi(fID)) == 0) {
		p = (Item*)malloc(sizeof(Item));					// Pointer to item
		if (p == NULL) {									// Make sure memory was allocated
			return;
		}
		strcpy(p->buff, fName);								// Copy to BST buffer
		p->key = atoi(fID);
		insertBST(*p);										// Insert into BST

		err = fopen_s(&f, "phonebook.txt", "ab");
		if (f) {
			fprintf(f, "%d\n%s", p->key, p->buff);
			fclose(f);
		}
	}
	else {
		printf("Contact with that ID already exists");
	}
}

/*************************************************************************
* selectContact() - selects a contact and returns their ID
*************************************************************************/
int selectContact() {
	printInorderBST();										// Print the contents of the BST
	printf("Which contact would you like to select?\n");
	char cmd[3] = {};										// User command
	scanf_s("%s", cmd, (unsigned int)sizeof(cmd));
	while (getchar() != '\n') {}							// Flush other input buffer

	return(selectFromList(atoi(cmd)));
}

/*************************************************************************
* closeContacts() - Frees the BST
*************************************************************************/
void closeContacts() {
	deleteBST();
}