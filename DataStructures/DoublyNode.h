#ifndef DOUBLY_NODE_H
#define DOUBLY_NODE_H

#include <stdlib.h>

typedef struct DoublyNode {
    void* value;
    struct DoublyNode* prev;
    struct DoublyNode* next;
} DoublyNode;

DoublyNode* getNewDoublyNode();



#endif