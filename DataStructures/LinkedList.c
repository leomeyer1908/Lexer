#include "LinkedList.h"



void initList(LinkedList* list) {
    list->head = NULL;
}

void pushBackList(LinkedList* list, void* value) {
    Node* newNode = getNewNode();
    newNode->value = value;
    if (list->head == NULL) {
        list->head = newNode;
    }
    else {
        Node* currentNode = list->head;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
        currentNode->next = newNode;
    }
}

void destroyList(LinkedList* list) {
    Node* currentNode = list->head;
    Node* nextNode;
    while (currentNode != NULL) {
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }

    list->head = NULL;
}

