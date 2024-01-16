#include "Lexer.h"


static int tokenizeInputUsingDFAs(char* input, LinkedList* dfas, LinkedList* tokens) {
    initList(tokens);

    HashMap dfaCharsConsumedCount;
    initHashMap(&dfaCharsConsumedCount, dfas->size, NULL, NULL);
    for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
        insertHashMap(&dfaCharsConsumedCount, node->value, (void*) 0);
    }

    HashMap dfaCurrentState;
    initHashMap(&dfaCurrentState, dfas->size, NULL, NULL);
    for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
        DFA* currentDFA = (DFA*) node->value;
        insertHashMap(&dfaCurrentState, (void*) currentDFA, (void*) currentDFA->startState);
    }

    HashMap dfaLastValidState;
    initHashMap(&dfaLastValidState, dfas->size, NULL, NULL);
    for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
        DFA* currentDFA = (DFA*) node->value;
        insertHashMap(&dfaLastValidState, (void*) currentDFA, (void*) INVALID_STATE);
    } 

    HashMap dfaCharsToIndexMaps;
    initHashMap(&dfaCharsToIndexMaps, dfas->size, NULL, NULL);
    for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
        DFA* currentDFA = (DFA*) node->value;
        HashMap* currentCharToIndexMap = (HashMap*) malloc(sizeof(HashMap));
        initHashMap(currentCharToIndexMap, currentDFA->colSize, NULL, NULL);
        for (size_t i = 0; i < currentDFA->colSize; i++) {
            insertHashMap(currentCharToIndexMap, (void*) (uintptr_t) currentDFA->alphabet[i], (void*) i);
        }

        insertHashMap(&dfaCharsToIndexMaps, (void*) currentDFA, (void*) currentCharToIndexMap);
    }

    size_t currentColNum = 1;
    size_t currentLineNum = 1;
    size_t lettersConsumed = 0;

    //loop until we are at the last character, but no letters have been consumed yet, meaning that we have tokenized all previous characters.
    for (size_t i = 0; !(input[i] == '\0' && lettersConsumed == 0); i++) {
        int isAnyDFAStillAlive = 0;
        for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
            DFA* currentDFA = (DFA*) node->value;
            size_t currentState = (size_t) getHashMap(&dfaCurrentState, (void*) currentDFA);
            if (currentState != INVALID_STATE) {
                HashMap* currentCharToIndexMap = (HashMap*) getHashMap(&dfaCharsToIndexMaps, (void*) currentDFA);
                size_t currentCharIndex = (size_t) getHashMap(currentCharToIndexMap, (void*) (uintptr_t) input[i]);
                
                //If the current char is not part of the DFA alphabet at all, set it to the invalid state
                if ((void*) currentCharIndex == NULL && currentDFA->alphabet[currentCharIndex] != input[i]) {
                    updateHashMap(&dfaCurrentState, (void*) currentDFA, (void*) INVALID_STATE);
                }
                else {
                    size_t destinationState = currentDFA->matrix[currentState][currentCharIndex];
                    updateHashMap(&dfaCurrentState, (void*) currentDFA, (void*) destinationState);
                    if (destinationState != INVALID_STATE) {
                        updateHashMap(&dfaLastValidState, (void*) currentDFA, (void*) destinationState);
                        size_t charsConsumedForCurrentDFA = (size_t) getHashMap(&dfaCharsConsumedCount, (void*) currentDFA);
                        updateHashMap(&dfaCharsConsumedCount, (void*) currentDFA, (void*) ++charsConsumedForCurrentDFA);
                        isAnyDFAStillAlive = 1;
                    }
                } 
            }
        }

        if (isAnyDFAStillAlive == 0) {
            size_t maxCharsConsumed = 0;
            DFA* dfaWithMaxCharsConsumed = NULL;

            for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
                DFA* currentDFA = (DFA*) node->value;
                size_t lastValidState = (size_t) getHashMap(&dfaLastValidState, (void*) currentDFA);

                if (containsHashSet(&currentDFA->acceptStates, (void*) lastValidState)) {
                    size_t charsConsumedForCurrentDFA = (size_t) getHashMap(&dfaCharsConsumedCount, (void*) currentDFA);
                    if (charsConsumedForCurrentDFA > maxCharsConsumed) {
                        maxCharsConsumed = charsConsumedForCurrentDFA;
                        dfaWithMaxCharsConsumed = currentDFA;
                    }
                }

            }

            if (dfaWithMaxCharsConsumed == NULL) {
                printf("This token spec does not match the given input.\n");
                return -1;
            }

            Token* newToken = (Token*) malloc(sizeof(Token));
            newToken->tokenName = (char*) malloc((strlen(dfaWithMaxCharsConsumed->tokenName)+1)*sizeof(char));
            strcpy(newToken->tokenName, (const char*) dfaWithMaxCharsConsumed->tokenName);
            newToken->colNum = currentColNum;
            newToken->lineNum = currentLineNum;
            newToken->value = (char*) malloc((maxCharsConsumed+1)*sizeof(char));

            i = i-lettersConsumed;
            for (int j = 0; j < maxCharsConsumed; j++) {
                newToken->value[j] = input[i];
                if (input[i] == '\n') {
                    currentLineNum++;
                    currentColNum = 0;
                } else {
                    currentColNum++;
                }
                i++;
            }
            i--;
            newToken->value[maxCharsConsumed] = '\0';

            pushBackList(tokens, (void*) newToken);

            for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
                DFA* currentDFA = (DFA*) node->value;
                updateHashMap(&dfaCharsConsumedCount, node->value, (void*) 0);
                updateHashMap(&dfaCurrentState, (void*) currentDFA, (void*) currentDFA->startState);
                updateHashMap(&dfaLastValidState, (void*) currentDFA, (void*) INVALID_STATE);
            }

            lettersConsumed = 0;
        }
        else {
            lettersConsumed++;
        }
    }

    for (DoublyNode* node = dfas->head; node != NULL; node = node->next) {
        DFA* currentDFA = (DFA*) node->value; 
        destroyDFA(currentDFA);
        free(currentDFA);
    }
    destroyList(dfas);

    destroyHashMap(&dfaCharsConsumedCount);

    destroyHashMap(&dfaCurrentState);

    destroyHashMap(&dfaLastValidState);

    for (DoublyNode* node = dfaCharsToIndexMaps.keyValuePairs.head; node != NULL; node = node->next) {
		HashMap* currentCharToIndexMap = (HashMap*) ((KeyValuePair*) node->value)->value;
        destroyHashMap(currentCharToIndexMap);
        free(currentCharToIndexMap);
    }
    destroyHashMap(&dfaCharsToIndexMaps);

    return 0;
}


int tokenizeInputUsingSpec(char* input, char* tokenSpec, LinkedList* tokens) {
    LinkedList dfas;

    int errorResult = getDFAsFromTokenSpec(tokenSpec, &dfas);
    if (errorResult == -1) {
        return -1;
    }
    
    errorResult = tokenizeInputUsingDFAs(input, &dfas, tokens);
    if (errorResult == -1) {
        return -1;
    }

    return 0;
}