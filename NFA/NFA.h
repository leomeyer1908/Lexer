#ifndef EXPRESSION_TREE_H
#define EXPRESSION_TREE_H 

#include <stdlib.h>
#include <stdio.h>
#include "../DataStructures/Stack.h"
#include "../DataStructures/HashMap.h"
#include "../DataStructures/LinkedList.h"

#define EPSILON '\0'

typedef struct NFANode {
    int is_accept;
    HashMap* transitions;
} NFANode;

typedef struct NFA {
    NFANode* startState;
	size_t nodeNum;
	NFANode** NFANodes;
} NFA;


/*
Takes a RegEx with the operators '+', '|', and '*' in the first argument and creates an NFA, which
is stored in the second argument. The NFA is made by first creating an intermediate expression tree 
from the regex rexpression. Then, Thompson's construction is used to build up an NFA by traversing 
thorugh the tree in post order. The function returns 0 for succesful output, and -1 for error.
*/
int createNFAFromRegex(const char* regex, NFA* nfa);

/*Detect if the Regex has any syntax errors. Returns 0 if it has no errors and -1 if it does.*/
int detectErrorRegEx(const char* regex);

void addTransitionNFANode(NFANode* nfaNode, char transitionChar, NFANode* destinationNode);

void printNFA(NFA* nfa);

int getIndexNFANode(NFA* nfa, NFANode* nfaNode);

void destroyNFANode(NFANode* nfaNode);

void destroyNFA(NFA* nfa);

#endif //EXPRESSION_TREE_H