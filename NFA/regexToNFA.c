#include "NFA.h"

/*Expression tree node that stores each charPtr from a RegEx*/
typedef struct CharTreeNode {
	const char* charPtr; //Must be a char* because some escape sequences like /* require 2 chars.
	struct CharTreeNode* leftChild;
	struct CharTreeNode* rightChild;
} CharTreeNode;

static void initCharTreeNode(CharTreeNode* charTreeNode, const char* charPtr) {
	charTreeNode->charPtr = charPtr;	
	charTreeNode->leftChild = NULL;
	charTreeNode->rightChild = NULL;
}

static void destroyCharTree(CharTreeNode* charTreeNode) {
	if (charTreeNode->leftChild != NULL) {
		destroyCharTree(charTreeNode->leftChild);
	}
	if (charTreeNode->rightChild != NULL) {
		destroyCharTree(charTreeNode->rightChild);
	}
	free(charTreeNode);
}

static void makeNFAFromChar(NFA* nfa, char c) {
	NFANode* startNode = (NFANode*) malloc(sizeof(NFANode));
	NFANode* acceptNode = (NFANode*) malloc(sizeof(NFANode));

	startNode->is_accept = 0;
	acceptNode->is_accept = 1;


	startNode->transitions = (HashMap*) malloc(sizeof(HashMap));
	initHashMap(startNode->transitions, 2);
	addTransitionNFANode(startNode, c, acceptNode);

	acceptNode->transitions = (HashMap*) malloc(sizeof(HashMap));
	initHashMap(acceptNode->transitions, 2);

	nfa->startState = startNode;

	nfa->nodeNum = 2;
	nfa->NFANodes = (NFANode**) malloc(nfa->nodeNum*sizeof(NFANode*));
	nfa->NFANodes[0] = startNode;
	nfa->NFANodes[1] = acceptNode;
}

/*
This funcion assumes that all NFAs were generated using Thompson's construction, thus it assumes
that there is one accept state per NFA, and that it is always the last node of the NFANodes array.
*/
static void makeNFAFromOperator(NFA* nfa, char op, NFA* nfa1, NFA* nfa2) {
	if (op == '|') {
		NFANode* startNode = (NFANode*) malloc(sizeof(NFANode));
		NFANode* acceptNode = (NFANode*) malloc(sizeof(NFANode));
		
		startNode->is_accept = 0;
		acceptNode->is_accept = 1;


		startNode->transitions = (HashMap*) malloc(sizeof(HashMap));
		initHashMap(startNode->transitions, 2);
		addTransitionNFANode(startNode, EPSILON, nfa1->startState);
		addTransitionNFANode(startNode, EPSILON, nfa2->startState);

		acceptNode->transitions = (HashMap*) malloc(sizeof(HashMap));
		initHashMap(acceptNode->transitions, 2);

		NFANode* nfa1AcceptState = nfa1->NFANodes[nfa1->nodeNum-1];
		NFANode* nfa2AcceptState = nfa2->NFANodes[nfa2->nodeNum-1];

		nfa1AcceptState->is_accept = 0;
		nfa2AcceptState->is_accept = 0;

		addTransitionNFANode(nfa1AcceptState, EPSILON, acceptNode);
		addTransitionNFANode(nfa2AcceptState, EPSILON, acceptNode);

		nfa->startState = startNode;
		nfa->nodeNum = 2 + nfa1->nodeNum + nfa2->nodeNum;
		nfa->NFANodes = (NFANode**) malloc(nfa->nodeNum*sizeof(NFANode*));
		nfa->NFANodes[0] = startNode;
		for (size_t i = 0; i < nfa1->nodeNum; i++) {
			nfa->NFANodes[1+i] = nfa1->NFANodes[i];
		}
		for (size_t i = 0; i < nfa2->nodeNum; i++) {
			nfa->NFANodes[1+nfa1->nodeNum+i] = nfa2->NFANodes[i];
		}
		nfa->NFANodes[nfa->nodeNum-1] = acceptNode;

	}
	else if (op == '+') {
		NFANode* nfa1AcceptState = nfa1->NFANodes[nfa1->nodeNum-1];
		nfa1AcceptState->is_accept = 0;
		addTransitionNFANode(nfa1AcceptState, EPSILON, nfa2->startState);

		nfa->startState = nfa1->startState;
		nfa->nodeNum = nfa1->nodeNum + nfa2->nodeNum;
		nfa->NFANodes = (NFANode**) malloc(nfa->nodeNum*sizeof(NFANode*));
		for (size_t i = 0; i < nfa1->nodeNum; i++) {
			nfa->NFANodes[i] = nfa1->NFANodes[i];
		}
		for (size_t i = 0; i < nfa2->nodeNum; i++) {
			nfa->NFANodes[nfa1->nodeNum+i] = nfa2->NFANodes[i];
		}
	}
	else if (op == '*') {
		NFANode* startNode = (NFANode*) malloc(sizeof(NFANode));
		NFANode* acceptNode = (NFANode*) malloc(sizeof(NFANode));
		
		startNode->is_accept = 0;
		acceptNode->is_accept = 1;

		startNode->transitions = (HashMap*) malloc(sizeof(HashMap));
		initHashMap(startNode->transitions, 2);
		addTransitionNFANode(startNode, EPSILON, nfa1->startState);
		addTransitionNFANode(startNode, EPSILON, acceptNode);

		acceptNode->transitions = (HashMap*) malloc(sizeof(HashMap));
		initHashMap(acceptNode->transitions, 2);
		addTransitionNFANode(acceptNode, EPSILON, startNode);

		NFANode* nfa1AcceptState = nfa1->NFANodes[nfa1->nodeNum-1];
		nfa1AcceptState->is_accept = 0;
		addTransitionNFANode(nfa1AcceptState, EPSILON, acceptNode);

		nfa->startState = startNode;
		nfa->nodeNum = 2 + nfa1->nodeNum;
		nfa->NFANodes = (NFANode**) malloc(nfa->nodeNum*sizeof(NFANode*));
		nfa->NFANodes[0] = startNode;
		for (size_t i = 0; i < nfa1->nodeNum; i++) {
			nfa->NFANodes[1+i] = nfa1->NFANodes[i];
		}
		nfa->NFANodes[nfa->nodeNum-1] = acceptNode;
	}
}

static int isCharOperator(char c) {
	return c == '|' || c == '+' || c == '*';
}

/*This function frees the top 2 NFAs in the nfaStack and mallocs a new one to the top*/
static void mergeNFAsFromStack(Stack* nfaStack, char operator) {
	NFA* newNFA = (NFA*) malloc(sizeof(NFA));
	NFA* rightNFA = NULL;
	if (operator != '*') {
		rightNFA = (NFA*) popStack(nfaStack);
	} 	
	NFA* leftNFA = (NFA*) popStack(nfaStack);
	makeNFAFromOperator(newNFA, operator, leftNFA, rightNFA);
	pushStack(nfaStack, (void*) newNFA);
	free(leftNFA->NFANodes);
	free(leftNFA);
	if (operator != '*') {
		free(rightNFA->NFANodes);
		free(rightNFA);
	}
}

static CharTreeNode* getTreeFromStack(Stack* treeNodeStack, Stack* nfaStack) {
	CharTreeNode* currentNode = NULL;
	while (topStack(treeNodeStack) != NULL) {
		currentNode = (CharTreeNode*) popStack(treeNodeStack);
		if (topStack(treeNodeStack) == NULL) {
			break;
		}
		CharTreeNode* topOfTreeNodeStack = (CharTreeNode*) topStack(treeNodeStack);
		topOfTreeNodeStack->rightChild = currentNode;
		mergeNFAsFromStack(nfaStack, *topOfTreeNodeStack->charPtr);
	}
	return currentNode;
}

static void addOperatorToTree(char c, Stack* treeNodeStack, CharTreeNode* newTreeNode, Stack* nfaStack) {
	CharTreeNode* prevTreeNode = (CharTreeNode*) popStack(treeNodeStack);
	CharTreeNode* topOfTreeNodeStack = (CharTreeNode*) topStack(treeNodeStack);
	if ((*(prevTreeNode)->charPtr == '*' || !isCharOperator(*(prevTreeNode)->charPtr)) && topOfTreeNodeStack != NULL) {
		char topOfTreeNodeStackChar = *topOfTreeNodeStack->charPtr;
		if ((c == '|' && (topOfTreeNodeStackChar == '+' || topOfTreeNodeStackChar == '|')) || (c == '+' && topOfTreeNodeStackChar == '+')) {
			topOfTreeNodeStack->rightChild = prevTreeNode;
			prevTreeNode = (CharTreeNode*) popStack(treeNodeStack);
			mergeNFAsFromStack(nfaStack, topOfTreeNodeStackChar);
		}
	}
	if (c == '*') {
		mergeNFAsFromStack(nfaStack, c);
	}
	newTreeNode->leftChild = prevTreeNode;
}

static void addCharToTree(const char* charPtr, Stack* treeNodeStack, Stack* nfaStack) {
	CharTreeNode* newTreeNode = (CharTreeNode*) malloc(sizeof(CharTreeNode));
	initCharTreeNode(newTreeNode, charPtr);
	if (isCharOperator(*charPtr)) {
		addOperatorToTree(*charPtr, treeNodeStack, newTreeNode, nfaStack);
	}
	else {
		NFA* newNFA = (NFA*) malloc(sizeof(NFA));
		if (*charPtr != '\\') {
			makeNFAFromChar(newNFA, *charPtr);
		}
		else {
			makeNFAFromChar(newNFA, *(charPtr+1));	
		}
		pushStack(nfaStack, (void*) newNFA);
	}
	pushStack(treeNodeStack, (void*) newTreeNode);
}

static void updateTreeNodeStackFromChar(const char* charPtr, Stack* treeNodeStack, Stack* stackOfPrevStacks, Stack* nfaStack) {
	if (*charPtr == '(') {
		Stack* currentTreeNodeStackCopy = (Stack*) malloc(sizeof(Stack)); 
		*currentTreeNodeStackCopy = *treeNodeStack;
		pushStack(stackOfPrevStacks, (void*) currentTreeNodeStackCopy);
		initStack(treeNodeStack);
	}
	else if (*charPtr == ')') {
		CharTreeNode* currentTree = getTreeFromStack(treeNodeStack, nfaStack);
		destroyStack(treeNodeStack);
		Stack* oldTreeNodeStack = (Stack*) popStack(stackOfPrevStacks);
		*treeNodeStack = *oldTreeNodeStack;
		free(oldTreeNodeStack);
		CharTreeNode* topOfTreeNodeStack = (CharTreeNode*) topStack(treeNodeStack);
		if (topOfTreeNodeStack != NULL && ((*(charPtr+1) == '|') || (*(charPtr+1) == '+' && *topOfTreeNodeStack->charPtr == '+'))) {
			topOfTreeNodeStack->rightChild = currentTree;
			mergeNFAsFromStack(nfaStack, *topOfTreeNodeStack->charPtr);
		}
		else {
			pushStack(treeNodeStack, (void*) currentTree);
		}
	}
	else {
		addCharToTree(charPtr, treeNodeStack, nfaStack);
	}
}

//Used to check if a character can come after an operand, or if those characters have an operand before
static int isValidCharAfterOperand(char c) {
	return c == ')' || isCharOperator(c) || c == '\0';
}

//All characters it checks for: (,),+,|,*, \\, \0, and all other chars (operands).
int detectErrorRegEx(const char* regex) {
	char firstChar = *regex;
	//An operand cannot come before firstChar, so gives error if it is a valid character for it
	if (isValidCharAfterOperand(firstChar)) {
		return -1;
	}

	int parensCount = 0;
	for (const char* charPtr = regex; *charPtr != '\0'; charPtr++) {
		if (*charPtr == '(') { // Valid after: (, \\, and everything else.
			if (isValidCharAfterOperand(*(charPtr+1))) {
				return -1;
			}
			parensCount++;
		}
		else if (*charPtr == ')') {// Valid after: ), +, |, *, \0.
			if (!isValidCharAfterOperand(*(charPtr+1))) {
				return -1;
			}
			parensCount--;
			if (parensCount < 0) {
				return -1;
			}
		}
		else if (*charPtr == '+' || *charPtr == '|') {// Valid after: (, \\, and everything else.
			if (isValidCharAfterOperand(*(charPtr+1))) {
				return -1;
			}
		}
		else if (*charPtr == '*') { //Valid after: ), +, |, *, \0.
			if (!isValidCharAfterOperand(*(charPtr+1))) {
				return -1;
			}
		}
		else if (*charPtr == '\\') { //Valid after: (, ), +, |, *, \\. 
			if (!(*(charPtr+1) == '(' || *(charPtr+1) == ')' || isCharOperator(*(charPtr+1)) || *(charPtr+1) == '\\')) {
				return -1;
			}
			if (!isValidCharAfterOperand(*(charPtr+2))) {
				return -1;
			}
			charPtr++;
		}
		else { // Valid after: ), +, |, *, \0
			if (!isValidCharAfterOperand(*(charPtr+1))) {
				return -1;
			}
		}
	}

	if (parensCount != 0) {
		return -1;
	}
	return 0;
}

int createNFAFromRegex(const char* regex, NFA* nfa) {
	int errorResult = detectErrorRegEx(regex);
	if (errorResult == -1) {
		return -1;
	}

	Stack treeNodeStack;
	initStack(&treeNodeStack);

	Stack stackOfPrevStacks;
	initStack(&stackOfPrevStacks);

	Stack nfaStack;
	initStack(&nfaStack);

	for (const char* charPtr = regex; *charPtr != '\0'; charPtr++) {
		updateTreeNodeStackFromChar(charPtr, &treeNodeStack, &stackOfPrevStacks, &nfaStack);
		if (*charPtr == '\\') {
			charPtr++;
		}
	}

	CharTreeNode* expressionTreeRoot = getTreeFromStack(&treeNodeStack, &nfaStack);

	destroyCharTree(expressionTreeRoot);

	destroyStack(&treeNodeStack);
	destroyStack(&stackOfPrevStacks);

	NFA* finalNFA = (NFA*) popStack(&nfaStack);

	*nfa = *finalNFA;

	free(finalNFA);

	destroyStack(&nfaStack);

	return 0;	
}