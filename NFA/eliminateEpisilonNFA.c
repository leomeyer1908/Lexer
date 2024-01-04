#include "NFA.h"

/*
Creates transition table which is a Hash Map for each node pointer, that contains
another Hash Map for each character, and that contains a Hash Set for each destination node.
The first hash map for the node pointers is used for the algorithm to be able to quickly go
from one of the destination node to its corresponding entry in the table. The Hash Set is
used to avoid duplicates.
*/
static void createTransitionTable(NFA* nfa, HashMap* transitionTable) {
	initHashMap(transitionTable, nfa->nodeNum, NULL, NULL);
	for (size_t i = 0; i < nfa->nodeNum; i++) {
		HashMap* currentTransitions = (HashMap*) malloc(sizeof(HashMap));
		initHashMap(currentTransitions, nfa->NFANodes[i]->transitions->capacity, NULL, NULL);
		for (DoublyNode* node = nfa->NFANodes[i]->transitions->keyValuePairs.head; node != NULL; node = node->next) {
			char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;
			LinkedList* transitionList = (LinkedList*) ((KeyValuePair*) node->value)->value;
			if (transitionList != NULL) {
				HashSet* transitionSet = (HashSet*) malloc(sizeof(HashSet));
				initHashSet(transitionSet, 2, NULL, NULL);
				getSetFromList(transitionList, transitionSet);
				insertHashMap(currentTransitions, (void*) (uintptr_t) currentTransitionChar, (void*) transitionSet);
			}
		}
		insertHashMap(transitionTable, (void*) nfa->NFANodes[i], currentTransitions);
	}
}

static void destroyTransitionTable(HashMap* transitionTable) {
	for (DoublyNode* node = transitionTable->keyValuePairs.head; node != NULL; node = node->next) {
		HashMap* transitions = (HashMap*) ((KeyValuePair*) node->value)->value;
		for (DoublyNode* node = transitions->keyValuePairs.head; node != NULL; node = node->next) {
			HashSet* destinationNodes = (HashSet*) ((KeyValuePair*) node->value)->value;
			destroyHashSet(destinationNodes);
			free(destinationNodes);
		}
		destroyHashMap(transitions);
		free(transitions);
	}
	destroyHashMap(transitionTable);
}


/*Updates the transition table by adding non-episilon transitions from the destination
node to the current node.*/
static void addDestinationNodeTransitions(NFANode* destinationNode, NFANode* currentNFANode, HashMap* transitionTable, HashSet* nfaNodesReferenced) {
	HashMap* currentNodeUniqueTransitions = (HashMap*) getHashMap(transitionTable, (void*) currentNFANode);
	for (DoublyNode* node = destinationNode->transitions->keyValuePairs.head; node != NULL; node = node->next) {
		char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;

		HashSet* currentUniqueDestinations = (HashSet*) getHashMap(currentNodeUniqueTransitions, (void*) (uintptr_t) currentTransitionChar);
		if (currentUniqueDestinations == NULL) {
			currentUniqueDestinations = (HashSet*) malloc(sizeof(HashSet));
			initHashSet(currentUniqueDestinations, 2, NULL, NULL);
			insertHashMap(currentNodeUniqueTransitions, (void*) (uintptr_t) currentTransitionChar, (void*) currentUniqueDestinations);
		}

		LinkedList* currentDestinationNodes = (LinkedList*) getHashMap(currentNFANode->transitions, (void*) (uintptr_t) currentTransitionChar);
		if (currentDestinationNodes == NULL) {
			currentDestinationNodes = (LinkedList*) malloc(sizeof(LinkedList));
			initList(currentDestinationNodes);
			insertHashMap(currentNFANode->transitions, (void*) (uintptr_t) currentTransitionChar, (void*) currentDestinationNodes);
		}



		LinkedList* destinationDestinationNodes = (LinkedList*) ((KeyValuePair*) node->value)->value;
		for (DoublyNode* node = destinationDestinationNodes->head; node != NULL; node = node->next) {
			if (!containsHashSet(currentUniqueDestinations, node->value)) {
				insertHashSet(currentUniqueDestinations, node->value);
				pushBackList(currentDestinationNodes, node->value);
			}
			if (currentTransitionChar != '\0') {
				insertHashSet(nfaNodesReferenced, node->value);
			}
		}
	}
}

static void eliminateEpisilonNFANode(NFANode* currentNFANode, HashSet* nfaNodesVisited, HashSet* nfaNodesReferenced, HashMap* transitionTable) {
	//Copies hashset to ensure that each recursion call of the function has a different HashSet
	HashSet newNFANodesVisited;
	copyHashSet(nfaNodesVisited, &newNFANodesVisited);
	nfaNodesVisited = &newNFANodesVisited;

	insertHashSet(nfaNodesVisited, (void*) currentNFANode);
	LinkedList* episilonTransitions = (LinkedList*) getHashMap(currentNFANode->transitions, (void*) (uintptr_t) '\0');

	HashMap* currentNodeUniqueTransitions = (HashMap*) getHashMap(transitionTable, (void*) currentNFANode);
	HashSet* uniqueEpisilonTransitions = (HashSet*) getHashMap(currentNodeUniqueTransitions, (void*) (uintptr_t) '\0');
	if (episilonTransitions != NULL) {
		DoublyNode* node = episilonTransitions->head;
		while (node != NULL) {
			DoublyNode* nextNode = node->next;
			NFANode* destinationNode = (NFANode*) node->value;
			if (destinationNode == currentNFANode) {
				removeNodeFromList(episilonTransitions, node);
				removeElementHashSet(uniqueEpisilonTransitions, node->value);
			}
			else if (!containsHashSet(nfaNodesVisited, (void*) destinationNode)) {
				eliminateEpisilonNFANode(destinationNode, nfaNodesVisited, nfaNodesReferenced, transitionTable);
				addDestinationNodeTransitions(destinationNode, currentNFANode, transitionTable, nfaNodesReferenced); 
				if (destinationNode->is_accept) {
					currentNFANode->is_accept = 1;
				}
				removeNodeFromList(episilonTransitions, (void*) node);
				removeElementHashSet(uniqueEpisilonTransitions, node->value);
			}		
			node = nextNode;
		}
	}
	destroyHashSet(nfaNodesVisited);
}

//the rule is to loop through each node index, and look at each episilon transitions
//then go into the episilon transition number for that node, and go to that transition. (if it is itself, delete it)
//then add it to a list of previous hashsets. Then we loop through each of the new transitions of that node using the same rules, but
//if we see an element already on the hashset we skip it. After we are done with a transition, we take all of its transitions and copy over
//to its parents, but ensuring no duplicates. If it is an accept state, we also make the parent an accept state.
//In the end, we loop through and ensure that there are no nodes that are not pointed to by anyone.
void eliminateEpisilonNFA(NFA* nfa) {
	HashMap transitionTable;
	createTransitionTable(nfa, &transitionTable);

	HashSet nfaNodesReferenced; //Nodes referenced through non-epision transitions
	initHashSet(&nfaNodesReferenced, 2, NULL, NULL);
	insertHashSet(&nfaNodesReferenced, (void*) nfa->startState);

	for (size_t i = 0; i < nfa->nodeNum; i++) {
		HashSet nfaNodesVisited; //Nodes visited through episilon transitions
		initHashSet(&nfaNodesVisited, 2, NULL, NULL);

		eliminateEpisilonNFANode(nfa->NFANodes[i], &nfaNodesVisited, &nfaNodesReferenced, &transitionTable);

		destroyHashSet(&nfaNodesVisited);
	}
	destroyTransitionTable(&transitionTable);

	removeUnreachableNFANodes(nfa);

	for (size_t i = 0; i < nfa->nodeNum; i++) {
		if (containsHashMap(nfa->NFANodes[i]->transitions, (void*) (uintptr_t) '\0')) {
			LinkedList* transitionList = (LinkedList*) getHashMap(nfa->NFANodes[i]->transitions, (void*) (uintptr_t) '\0');
			free(transitionList);
			removeElementHashMap(nfa->NFANodes[i]->transitions, (void*) (uintptr_t) '\0');
		}
	}

	destroyHashSet(&nfaNodesReferenced);
}