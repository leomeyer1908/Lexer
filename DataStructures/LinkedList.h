#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include "Node.h"

typedef struct LinkedList {
    Node* head;
} LinkedList;

void initList(LinkedList* list);

void pushBackList(LinkedList* list, void* value);

void destroyList(LinkedList* list);


#endif