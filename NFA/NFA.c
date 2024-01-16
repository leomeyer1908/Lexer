#include "NFA.h"

void initNFANode(NFANode* nfaNode) {
	nfaNode->is_accept = 0;
	nfaNode->transitions = (HashMap*) malloc(sizeof(HashMap));
	initHashMap(nfaNode->transitions, 2, NULL, NULL);
}

void addTransitionNFANode(NFANode* nfaNode, char transitionChar, NFANode* destinationNode) {
	LinkedList* possibleDestinations;
	if (containsHashMap(nfaNode->transitions, (void*) (uintptr_t) transitionChar)) {
		possibleDestinations = (LinkedList*) getHashMap(nfaNode->transitions, (void*) (uintptr_t) transitionChar);
	}
	else {
		possibleDestinations = (LinkedList*) malloc(sizeof(LinkedList));
		initList(possibleDestinations);
		insertHashMap(nfaNode->transitions, (void*) (uintptr_t) transitionChar, (void*) possibleDestinations);
	}
	pushBackList(possibleDestinations, (void*) destinationNode);
}

void removeUnreachableNFANodes(NFA* nfa) {
	HashSet visitedNodes;
	initHashSet(&visitedNodes, nfa->nodeNum, NULL, NULL);

	Stack nodesToVisit;
	initStack(&nodesToVisit);
	pushStack(&nodesToVisit, (void*) nfa->startState);

	while (topStack(&nodesToVisit) != NULL) {
		NFANode* currentNFANode = (NFANode*) popStack(&nodesToVisit);
		if (!containsHashSet(&visitedNodes, (void*) currentNFANode)) {
			insertHashSet(&visitedNodes, (void*) currentNFANode);
			for (DoublyNode* node = currentNFANode->transitions->keyValuePairs.head; node != NULL; node = node->next) {
				char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;
				if (currentTransitionChar == '\0') {
					continue;
				}
				LinkedList* destinationNodes = (LinkedList*) ((KeyValuePair*) node->value)->value;
				for (DoublyNode* node = destinationNodes->head; node != NULL; node = node->next) {
					if (!containsHashSet(&visitedNodes, (void*) node->value)) {
						pushStack(&nodesToVisit, (void*) node->value);
					}
				}
			}
		}
	}

	size_t newNodeNum = visitedNodes.size;
	NFANode** newNFANodes = (NFANode**) malloc(newNodeNum*sizeof(NFANode*));
	size_t nodeCount = 0;
	for (size_t i = 0; i < nfa->nodeNum; i++) {
		if (containsHashSet(&visitedNodes, (void*) nfa->NFANodes[i])) {
			newNFANodes[nodeCount] = nfa->NFANodes[i];
			nodeCount++;
		}
		else {
			destroyNFANode(nfa->NFANodes[i]);
			free(nfa->NFANodes[i]);
		}
	}
	free(nfa->NFANodes);
	nfa->NFANodes = newNFANodes;
	nfa->nodeNum = newNodeNum;

	destroyHashSet(&visitedNodes);

}

void printNFA(NFA* nfa) {
	HashMap nodeToIndexMap;
	initHashMap(&nodeToIndexMap, nfa->nodeNum, NULL, NULL);
	for (size_t i = 0; i < nfa->nodeNum; i++) {
		insertHashMap(&nodeToIndexMap, (void*) nfa->NFANodes[i], (void*) i);
	}
	for (size_t i = 0; i < nfa->nodeNum; i++) {
        printf("Node Num: %zu:\n", i);
        printf("Is Accept: %d\n", nfa->NFANodes[i]->is_accept);
		
		for (DoublyNode* node = nfa->NFANodes[i]->transitions->keyValuePairs.head; node != NULL; node = node->next) {
			char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;
			LinkedList* transitionList = (LinkedList*) ((KeyValuePair*) node->value)->value;

			for (DoublyNode* node = transitionList->head; node != NULL; node = node->next) {
				NFANode* nextNode = (NFANode*) node->value;
				size_t nextNodeIndex = (size_t) getHashMap(&nodeToIndexMap, (void*) nextNode);
				printf("'%c' transitions to node #%zu\n", currentTransitionChar, nextNodeIndex);
			}
		}
        printf("\n");
	}
	destroyHashMap(&nodeToIndexMap);
}

void destroyNFANode(NFANode* nfaNode) {
	for (DoublyNode* node = nfaNode->transitions->keyValuePairs.head; node != NULL; node = node->next) {
		LinkedList* transitionList = (LinkedList*) ((KeyValuePair*) node->value)->value;
		destroyList(transitionList);
		free(transitionList);
	}
	destroyHashMap(nfaNode->transitions);
	free(nfaNode->transitions);
}

void destroyNFA(NFA* nfa) {
	for (size_t i = 0; i < nfa->nodeNum; i++) {
		destroyNFANode(nfa->NFANodes[i]);
		free(nfa->NFANodes[i]);
	}
	free(nfa->NFANodes);
}