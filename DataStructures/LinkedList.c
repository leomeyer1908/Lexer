#include "LinkedList.h"

void initList(LinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void pushBackList(LinkedList* list, void* value) {
    DoublyNode* newNode = getNewDoublyNode();
    newNode->value = value;
    if (list->head == NULL) {
        list->head = newNode;
    }
    else {
        newNode->prev = list->tail;
        list->tail->next = newNode;
    }
    list->tail = newNode;

    list->size++;
}

void removeNodeFromList(LinkedList* list, DoublyNode* node) {
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    free(node);

    list->size--;
}

void removeElementList(LinkedList* list, void* value) {
    DoublyNode* currentNode = list->head;
    while (currentNode != NULL) {
        if (currentNode->value == value) {
            removeNodeFromList(list, currentNode);
            break;
        }
        currentNode = currentNode->next;
    }
}

void destroyList(LinkedList* list) {
    DoublyNode* currentNode = list->head;
    DoublyNode* nextNode;
    while (currentNode != NULL) {
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void copyList(LinkedList* srcList, LinkedList* dstList) {
	if (srcList->head != NULL) {
		DoublyNode* dstNode = getNewDoublyNode();
		dstList->head = dstNode;
		DoublyNode* srcNode = srcList->head;
		while (srcNode != NULL) {
			dstNode->value = srcNode->value;
			if (srcNode->next != NULL) {
				DoublyNode* newDstNode = getNewDoublyNode();
				dstNode->next = newDstNode;
				dstNode = newDstNode;
			} 
            else {
                dstList->tail = dstNode;
            }
			srcNode = srcNode->next;
		}
        dstList->size = srcList->size;
	}
	else {
		dstList->head = NULL;
        dstList->tail = NULL;
        dstList->size = 0;
	}
}