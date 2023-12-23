#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

typedef struct Node {
    void* value;
    struct Node* next;
} Node;

Node* getNewNode();



#endif