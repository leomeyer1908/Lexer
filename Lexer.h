#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include "DataStructures/HashMap.h"
#include "DataStructures/HashSet.h"
#include "DataStructures/LinkedList.h"
#include "NFA/NFA.h"

//Uses SIZE_MAX as the invalid state, since size_t is the type, and thus -1 cannot be used
#define INVALID_STATE SIZE_MAX

typedef struct DFA {
    size_t rowSize;
    size_t colSize;
    char* alphabet;
    size_t** matrix;
    size_t startState;
    HashSet acceptStates;
    char* tokenName;
} DFA;

typedef struct Token {
    char* tokenName;
    char* value;
    size_t lineNum;
    size_t colNum;
} Token;


int detectErrorTokenSpec(char* tokenSpec);
/*Creates a linked list of DFAs based on a tokenSpec. Return value is 0 if no error, and -1 if error.*/
int getDFAsFromTokenSpec(char* tokenSpec, LinkedList* dfas);

int isValidComplexRegex(char* regex);
char* getSimpleRegexFromComplexRegex(char* complexRegex);
int tokenizeInputUsingSpec(char* input, char* tokenSpec, LinkedList* tokens);


/*Takes a simple Regex and returns an DFA*/
int makeDFAFromRegex(char* regex, DFA* dfa);
void destroyDFA(DFA* dfa);

/*Tokenizes input from a source file using a token spec*/
int tokenizeInputUsingSpec(char* input, char* tokenSpec, LinkedList* tokens);


#endif //LEXER_H