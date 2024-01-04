#ifndef NFA_H
#define NFA_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "../DataStructures/Stack.h"
#include "../DataStructures/HashMap.h"
#include "../DataStructures/LinkedList.h"
#include "../DataStructures/HashSet.h"

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

void eliminateEpisilonNFA(NFA* nfa);

void initNFANode(NFANode* nfaNode);

void addTransitionNFANode(NFANode* nfaNode, char transitionChar, NFANode* destinationNode);

/*Uses a DFS traversal to find all reachable nodes and remove the ones not found*/
void removeUnreachableNFANodes(NFA* nfa);

void printNFA(NFA* nfa);

void createDFAFromNFA(const NFA* nfa, NFA* dfa);

void destroyNFANode(NFANode* nfaNode);

void destroyNFA(NFA* nfa);

#endif //NFA_H