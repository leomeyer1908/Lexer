#include "Lexer.h"

int makeDFAFromRegex(char* regex, DFA* dfa) {
    NFA nfa;
    NFA dfaNFA;
    int errorResult = createNFAFromRegex(regex, &nfa);
    if (errorResult == -1) {
        printf("Error while making the NFA from Regex. The Regex that gave error is: %s\n", regex);
        return -1;
    } 
    eliminateEpisilonNFA(&nfa);
    createDFAFromNFA(&nfa, &dfaNFA);
    destroyNFA(&nfa);

    HashMap nodeToIndexMap;
	initHashMap(&nodeToIndexMap, dfaNFA.nodeNum, NULL, NULL);
	for (size_t i = 0; i < dfaNFA.nodeNum; i++) {
		insertHashMap(&nodeToIndexMap, (void*) dfaNFA.NFANodes[i], (void*) i);
	}

    HashSet alphabet;
    initHashSet(&alphabet, 2, NULL, NULL);
    for (size_t i = 0; i < dfaNFA.nodeNum; i++) {
        for (DoublyNode* node = dfaNFA.NFANodes[i]->transitions->keyValuePairs.head; node != NULL; node = node->next) {
            char currentTransitionChar = (char) (uintptr_t) ((KeyValuePair*) node->value)->key;
            insertHashSet(&alphabet, (void*) (uintptr_t) currentTransitionChar);
        }
    }


    dfa->rowSize = dfaNFA.nodeNum;
    dfa->colSize = alphabet.size;

    dfa->alphabet = (char*) malloc(alphabet.size*sizeof(char));
    size_t i = 0;
    for (DoublyNode* node = alphabet.keys.head; node != NULL; node = node->next) {
        char currentTransitionChar = (char) (uintptr_t) node->value; 
        dfa->alphabet[i++] = currentTransitionChar;
    }
    destroyHashSet(&alphabet);

    dfa->startState = (size_t) getHashMap(&nodeToIndexMap, (void*) dfaNFA.startState);

    initHashSet(&dfa->acceptStates, 2, NULL, NULL);

    dfa->matrix = (size_t**) malloc(dfa->rowSize*sizeof(size_t*));
    for (size_t i = 0; i < dfa->rowSize; i++) {
        dfa->matrix[i] = (size_t*) malloc(dfa->colSize*sizeof(size_t));
        for (size_t j = 0; j < dfa->colSize; j++) {
            LinkedList* destinationList = (LinkedList*) getHashMap(dfaNFA.NFANodes[i]->transitions, (void*) (uintptr_t) dfa->alphabet[j]);
            if (destinationList != NULL) {
                NFANode* destinationNode = (NFANode*) destinationList->head->value;
                dfa->matrix[i][j] = (size_t) getHashMap(&nodeToIndexMap, (void*) destinationNode);
            }
            else {
                dfa->matrix[i][j] = INVALID_STATE;
            }
        }

        if (dfaNFA.NFANodes[i]->is_accept) {
            insertHashSet(&dfa->acceptStates, getHashMap(&nodeToIndexMap, (void*) dfaNFA.NFANodes[i]));
        }
    }

    dfa->tokenName = NULL;


    destroyNFA(&dfaNFA);
    destroyHashMap(&nodeToIndexMap);
    return 0;
}

void destroyDFA(DFA* dfa) {
    free(dfa->alphabet);
    for (size_t i = 0; i < dfa->rowSize; i++) {
        free(dfa->matrix[i]);
    }
    free(dfa->matrix);
    destroyHashSet(&dfa->acceptStates);
    free(dfa->tokenName);
}