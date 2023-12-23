#include "Node.h"


Node* getNewNode() {
    Node* node = (Node*) malloc(sizeof(Node));
    node->value = NULL;
    node->next = NULL;
    return node;
}