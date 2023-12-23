#include "NFA.h"

void addTransitionNFANode(NFANode* nfaNode, char transitionChar, NFANode* destinationNode) {
	LinkedList* possibleDestinations;
	if (containsHashMap(nfaNode->transitions, (int) transitionChar)) {
		possibleDestinations = (LinkedList*) getHashMap(nfaNode->transitions, (int) transitionChar);
	}
	else {
		possibleDestinations = (LinkedList*) malloc(sizeof(LinkedList));
		initList(possibleDestinations);
		insertHashMap(nfaNode->transitions, (int) transitionChar, (void*) possibleDestinations);
	}
	pushBackList(possibleDestinations, (void*) destinationNode);
}

int getIndexNFANode(NFA* nfa, NFANode* nfaNode) {
    for (int i = 0; i < nfa->nodeNum; i++) {
        if (nfa->NFANodes[i] == nfaNode) {
            return i;
        } 
    }
    return -1;
}

void printNFA(NFA* nfa) {
	for (int i = 0; i < nfa->nodeNum; i++) {
        printf("Node Num: %d:\n", i);
        printf("Is Accept: %d\n", nfa->NFANodes[i]->is_accept);
		for (int j = 0; j < nfa->NFANodes[i]->transitions->capacity; j++) {
			for (Node* node = nfa->NFANodes[i]->transitions->array[j].head; node != NULL; node = node->next) {
				char currentTransitionChar = (char) ((KeyValuePair*) node->value)->key;
                LinkedList* transitionList = (LinkedList*) ((KeyValuePair*) node->value)->value;
                if (transitionList != NULL) {
                    for (Node* node = transitionList->head; node != NULL; node = node->next) {
                        NFANode* nextNode = (NFANode*) node->value;
                        printf("'%c' transitions to node #%d\n", currentTransitionChar, getIndexNFANode(nfa, nextNode));
                    }
                }
			}
		}
        printf("\n");
	}
}

void destroyNFANode(NFANode* nfaNode) {
	for (size_t i = 0; i < nfaNode->transitions->capacity; i++) {
		for (Node* node = nfaNode->transitions->array[i].head; node != NULL; node = node->next) {
			LinkedList* transitionList = (LinkedList*) ((KeyValuePair*) node->value)->value;
			if (transitionList != NULL) {
				free(transitionList);
			}
		}
	}
	destroyHashMap(nfaNode->transitions);
	free(nfaNode->transitions);
}

void destroyNFA(NFA* nfa) {
	for (size_t i = 0; i < nfa->nodeNum; i++) {
		destroyNFANode(nfa->NFANodes[i]);
		free(nfa->NFANodes[i]);
	}
}