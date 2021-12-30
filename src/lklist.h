#ifndef _LKLIST_H
#define _LKLIST_H

#include <stdlib.h>
#include "commons.h"

//Smart element of the linked list.
typedef struct node {
    void * data;
    struct node* previous;
    struct node* next;
} node;

//Linked list structure. Stores wich are the first and last node to 
//be used as different data structures.
typedef struct {
    node* first;
    node* last;
}lklist;

//Functions to manage linked lists.

//Initialize a linked list.
void lkinit(lklist* a);

#define LKLIST(name) lklist name; lkinit(&name)

//Append at the end of the list.
void lkappend(lklist *list, void * value);

//Remove from the start of the list.
void * lkquit(lklist *list);

//Get the size of the list.
size_t lklen(lklist *list);

//Get the node according to the index given.
node *lkndat(lklist *list, size_t index);

//Returns true if the list is empty
#define lkisempty(listptr) ((*list).first==(*list).last && (*list).last==NULL)

//Return true if the list has only one element
#define lkhasone(listptr) ((*list).first==(*list).last && (*list).last!=NULL)

//Inserts a value in the position given.
void lkins(lklist *list, size_t index, void * value);

//Remove the value at index.
void *lkrm(lklist *list, size_t index);

//Execute the given function on every element of the linked list.
//The function must recieve two arguments, the index of the node in 
//the linked list and the node itself. It's pretty fast.
void lkfor(lklist *list, void fn(size_t*,node*));

//Erase every node of the list safely and is data is true free memory of
//data stored.
void lkfree(lklist *list, BOOL data);

#endif