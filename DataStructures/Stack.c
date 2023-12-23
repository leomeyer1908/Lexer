#include "Stack.h"

void initStack(Stack* stack) {
    stack->top = NULL;
}

void pushStack(Stack* stack, void* value) {
    Node* newNode = getNewNode();
    newNode->value = value;
    newNode->next = stack->top;
    stack->top = newNode;
}

void* popStack(Stack* stack) {
    if (stack->top == NULL) {
        return NULL;
    }
    Node* topOfStack = stack->top;
    void* topOfStackValue = topOfStack->value;
    stack->top = stack->top->next;
    free(topOfStack);
    return topOfStackValue;
}

void* topStack(Stack* stack) {
    if (stack->top == NULL) {
        return NULL;
    }
    return stack->top->value;
}
void destroyStack(Stack* stack) {
    while (stack->top != NULL) {
        Node* topOfStack = stack->top; 
        stack->top = stack->top->next;
        free(topOfStack);
    }
}