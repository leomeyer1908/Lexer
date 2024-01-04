#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include "DoublyNode.h"

typedef struct LinkedList {
    DoublyNode* head;
    DoublyNode* tail;
    size_t size;
} LinkedList;

void initList(LinkedList* list);

void pushBackList(LinkedList* list, void* value);

void removeElementList(LinkedList* list, void* value);

void removeNodeFromList(LinkedList* list, DoublyNode* node);

void destroyList(LinkedList* list);

void copyList(LinkedList* srcList, LinkedList* dstList);


#endif