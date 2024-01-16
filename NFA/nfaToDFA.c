#include "NFA.h"

/*Struct that stores node address in an immutable array and is used as a key in the
NFAtoDFANodes hash map to get the node that corresponds to these nodes in the new DFA*/
typedef struct NFANodeCombination {
    const NFANode** nodes;
    size_t nodeNum;
} NFANodeCombination;

static size_t nfaNodeCombinationHashFunc(const void* key, const size_t capacity) {
    NFANodeCombination* nfaNodeCombination = (NFANodeCombination*) key;

    size_t totalPointerSum = 0;
    for (size_t i = 0; i < nfaNodeCombination->nodeNum; i++) {
        totalPointerSum += (size_t) nfaNodeCombination->nodes[i];
    }

    return totalPointerSum % capacity;
}

static int nfaNodeCombinationCmpFunc(const void* key1, const void* key2) {
    NFANodeCombination* nfaNodeCombination1 = (NFANodeCombination*) key1;
    NFANodeCombination* nfaNodeCombination2 = (NFANodeCombination*) key2;

    if (nfaNodeCombination1->nodeNum != nfaNodeCombination2->nodeNum) {
        return 0;
    }

    for (size_t i = 0; i < nfaNodeCombination1->nodeNum; i++) {
        if (nfaNodeCombination1->nodes[i] != nfaNodeCombination2->nodes[i]) {
            return 0;
        }
    }

    return 1;
}

/*Creates a NFA Node combination for a given NFA Node set that will always
contain the same immutable array for a given set. This is achieved by always
putting the nodes in the order in which they appear in the NFANodes array in the nfa*/
static void makeNFANodeCombinationsFromNodesSet(const NFA* nfa, HashSet* nfaNodesSet, NFANodeCombination* nfaNodeCombination) {
    NFANode** nfaNodesArray = (NFANode**) malloc(nfaNodesSet->size*sizeof(NFANode*)); 

    size_t nfaNodesArrayIndex = 0;
    for (size_t i = 0; i < nfa->nodeNum; i++) {
        if (containsHashSet(nfaNodesSet, (void*) nfa->NFANodes[i])) {
            nfaNodesArray[nfaNodesArrayIndex] = nfa->NFANodes[i];
            nfaNodesArrayIndex++;
        }
    }

    nfaNodeCombination->nodes = (const NFANode**) nfaNodesArray;
    nfaNodeCombination->nodeNum = nfaNodesSet->size;
}

static void destroyNFANodeCombination(NFANodeCombination* nfaNodeCombination) {
    free(nfaNodeCombination->nodes);
}


void createDFAFromNFA(const NFA* nfa, NFA* dfa) {
    //Maps node combination from NFA to a node on the DFA
    HashMap nfaToDFANodes;
    initHashMap(&nfaToDFANodes, nfa->nodeNum, &nfaNodeCombinationHashFunc, &nfaNodeCombinationCmpFunc);

    LinkedList newNodeCombinations;
    initList(&newNodeCombinations);

    //Create a new start node and its NFA Node combination to add to newNodes and nfaToDFANodes
    NFANode* newStartNFANode = (NFANode*) malloc(sizeof(NFANode));
    initNFANode(newStartNFANode);
    NFANodeCombination* startNodeCombination = (NFANodeCombination*) malloc(sizeof(NFANodeCombination));
    const NFANode** startStatePtr = (const NFANode**) malloc(sizeof(NFANode*));
    *startStatePtr = nfa->startState;
    startNodeCombination->nodes = startStatePtr;
    startNodeCombination->nodeNum = 1;
    insertHashMap(&nfaToDFANodes, (void*) startNodeCombination, (void*) newStartNFANode);
    pushBackList(&newNodeCombinations, (void*) startNodeCombination);
    

    for (DoublyNode* node = newNodeCombinations.head; node != NULL; node = node->next) {
        NFANodeCombination* currentNodeCombination = (NFANodeCombination*) node->value;

        //Holds each transition for the current node as a hashset for the value
        HashMap transitionsHashMap;
        initHashMap(&transitionsHashMap, 2, NULL, NULL);

        NFANode* correspondingDFANode = getHashMap(&nfaToDFANodes, (void*) currentNodeCombination);

        for (size_t i = 0; i < currentNodeCombination->nodeNum; i++) {
            const NFANode* currentNFANode = currentNodeCombination->nodes[i];

            for (DoublyNode* node = currentNFANode->transitions->keyValuePairs.head; node != NULL; node = node->next) {
                char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;
                LinkedList* transitionList = (LinkedList*) ((KeyValuePair*) node->value)->value;
                HashSet* transitionSet;
                if (containsHashMap(&transitionsHashMap, (void*) (uintptr_t) currentTransitionChar)) {
                    transitionSet = getHashMap(&transitionsHashMap, (void*) (uintptr_t) currentTransitionChar);
                }
                else {
                    transitionSet = (HashSet*) malloc(sizeof(HashSet));
                    initHashSet(transitionSet, 2, NULL, NULL);
                    insertHashMap(&transitionsHashMap, (void*) (uintptr_t) currentTransitionChar, (void*) transitionSet);
                }
                for (DoublyNode* node = transitionList->head; node != NULL; node = node->next) {
                    insertHashSet(transitionSet, node->value);
                }
            }

            if (currentNFANode->is_accept) {
                correspondingDFANode->is_accept = 1;
            }
        }

        for (DoublyNode* node = transitionsHashMap.keyValuePairs.head; node != NULL; node = node->next) {
            char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;
            HashSet* transitionSet = (HashSet*) ((KeyValuePair*) node->value)->value;

            NFANodeCombination* currentNFANodeCombination = (NFANodeCombination*) malloc(sizeof(NFANodeCombination));
            makeNFANodeCombinationsFromNodesSet(nfa, transitionSet, currentNFANodeCombination);

            NFANode* destinationDFANode;

            if (!containsHashMap(&nfaToDFANodes, (void*) currentNFANodeCombination)) {
                destinationDFANode = (NFANode*) malloc(sizeof(NFANode));
                initNFANode(destinationDFANode);
                insertHashMap(&nfaToDFANodes, (void*) currentNFANodeCombination, (void*) destinationDFANode);
                pushBackList(&newNodeCombinations, (void*) currentNFANodeCombination);
            } else {
                destinationDFANode = (NFANode*) getHashMap(&nfaToDFANodes, (void*) currentNFANodeCombination);
                destroyNFANodeCombination(currentNFANodeCombination);
                free(currentNFANodeCombination);
            }

            addTransitionNFANode(correspondingDFANode, currentTransitionChar, (void*) destinationDFANode);
            destroyHashSet(transitionSet);
            free(transitionSet);
        }
        destroyHashMap(&transitionsHashMap);
    }

    dfa->nodeNum = newNodeCombinations.size;
    dfa->NFANodes = (NFANode**) malloc(nfa->nodeNum*sizeof(NFANode*));
    size_t currentNodeIndex = 0;
    for(DoublyNode* node = newNodeCombinations.head; node != NULL; node = node->next) {
        dfa->NFANodes[currentNodeIndex] = (NFANode*) getHashMap(&nfaToDFANodes, node->value);
        currentNodeIndex++;
    }
    dfa->startState = dfa->NFANodes[0];

    destroyHashMap(&nfaToDFANodes);

    for (DoublyNode* node = newNodeCombinations.head; node != NULL; node = node->next) {
        NFANodeCombination* currentNodeCombination = (NFANodeCombination*) node->value;
        destroyNFANodeCombination(currentNodeCombination);
        free(currentNodeCombination);
    }
    destroyList(&newNodeCombinations);
}