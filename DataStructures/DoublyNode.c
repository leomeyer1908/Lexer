#include "DoublyNode.h"


DoublyNode* getNewDoublyNode() {
    DoublyNode* node = (DoublyNode*) malloc(sizeof(DoublyNode));
    node->value = NULL;
    node->prev = NULL;
    node->next = NULL;
    return node;
}