#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include "Node.h"

typedef struct Stack {
    Node* top;
} Stack;

void initStack(Stack* stack);
void pushStack(Stack* stack, void* value);
void* popStack(Stack* stack);
void* topStack(Stack* stack);
void destroyStack(Stack* stack);

#endif //STACK_H