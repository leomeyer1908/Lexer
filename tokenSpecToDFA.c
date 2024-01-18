#include "Lexer.h"

static int isSimpleRegexEscapeChar(char c) {
    return c == '\\' || c == '+' || c == '|' || c == '*' || c == '(' || c == ')'; 
}

static int isComplexRegexEscapeChar(char c) {
    return isSimpleRegexEscapeChar(c) || c == '[' || c == ']' || c == '.' || c == '^' ||c == 'n' || c == 't' || c == ' ';
}

static int isCharOperator(char c) {
	return c == '|' || c == '+' || c == '*';
}

//Used to check if a character can come after an operand, or if those characters have an operand before
static int isValidCharAfterOperand(char c) {
	return c == ')' || isCharOperator(c) || c == '\0';
}

//Modified version of the isValidRegex function used for the NFA.
//All characters it checks for: (,),+,|,*, \\, \0, [, ], and all other chars (operands and .).
int isValidComplexRegex(char* regex) {
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
		else if (*charPtr == '\\') { //Valid after: (, ), +, |, *, \\, [, ], n, t, ., ^ 
			if (!isComplexRegexEscapeChar(*(charPtr+1))) {
				return -1;
			}
			if (!isValidCharAfterOperand(*(charPtr+2))) {
				return -1;
			}
			charPtr++;
		}
        else if (*charPtr == '[') {
            charPtr++;
            while (*charPtr != ']') {
                if (*charPtr == '\0') {
                    return -1;
                }
                if (*charPtr == '\\') { //Valid after: (, ), +, |, *, \\, [, ], n, t. 
                    if (!isComplexRegexEscapeChar(*(charPtr+1))) {
                        return -1;
                    }
                    charPtr++;
                }
                charPtr++;
            }
			if (!isValidCharAfterOperand(*(charPtr+1))) {
				return -1;
			}
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

/*currently captures the operators of +, |, *, ., (), [], and [^].
returns NULL on error. */

char* getSimpleRegexFromComplexRegex(char* complexRegex) {
    int errorResult = isValidComplexRegex(complexRegex);
    if (errorResult == -1) {
        printf("Invalid Regex from Token Spec: %s\n", complexRegex);
        return NULL;
    }

    LinkedList simpleRegexChars;
    initList(&simpleRegexChars);

    for (size_t i = 0; complexRegex[i] != '\0'; i++) {
        if (complexRegex[i] == '.') {
            for (char c = 1;; c++) {
                if (isSimpleRegexEscapeChar(c)) {
                    pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\\');
                }
                pushBackList(&simpleRegexChars, (void*) (uintptr_t) c);
                if (c != 127) {
                    pushBackList(&simpleRegexChars, (void*) (uintptr_t) '|');
                }
                else {
                    break;
                }
            }
        }
        else if (complexRegex[i] == '[') {
            pushBackList(&simpleRegexChars, (void*) (uintptr_t) '(');
            i++;
            if (complexRegex[i] == '^') {
                i++;
                HashSet lettersInBracket;
                initHashSet(&lettersInBracket, 2, NULL, NULL);
                while (complexRegex[i] != ']') {
                    if (complexRegex[i] == '\\') {
                        i++;
                        if (complexRegex[i] == 'n') {
                            insertHashSet(&lettersInBracket, (void*) (uintptr_t) '\n');
                        }
                        else if (complexRegex[i] == 't') {
                            insertHashSet(&lettersInBracket, (void*) (uintptr_t) '\t');
                        }
                        else {
                            insertHashSet(&lettersInBracket, (void*) (uintptr_t) complexRegex[i]); 
                        }
                    }
                    else {
                        insertHashSet(&lettersInBracket, (void*) (uintptr_t) complexRegex[i]); 
                    }
                    i++;
                }
                for (char c = 1;; c++) {
                    if (!containsHashSet(&lettersInBracket, (void*) (uintptr_t) c)) {
                        if (isSimpleRegexEscapeChar(c)) {
                            pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\\');
                        }
                        pushBackList(&simpleRegexChars, (void*) (uintptr_t) c);
                        if (c != 127) {
                            pushBackList(&simpleRegexChars, (void*) (uintptr_t) '|');
                        }
                        else {
                            break;
                        }
                    }
                }
                destroyHashSet(&lettersInBracket);
            } 
            else {
                while (complexRegex[i] != ']') {
                    if (complexRegex[i] == '\\') {
                        i++;
                        if (complexRegex[i] == 'n') {
                            pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\n');
                        }
                        else if (complexRegex[i] == 't') {
                            pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\t');
                        }
                        else {
                            pushBackList(&simpleRegexChars, (void*) (uintptr_t) complexRegex[i]); 
                        }
                    }
                    else {
                        if (isSimpleRegexEscapeChar(complexRegex[i])) {
                            pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\\');
                        }
                        pushBackList(&simpleRegexChars, (void*) (uintptr_t) complexRegex[i]); 
                    }
                    if (complexRegex[i+1] != ']') {
                        pushBackList(&simpleRegexChars, (void*) (uintptr_t) '|');
                    }
                    i++;
                }
            }
            pushBackList(&simpleRegexChars, (void*) (uintptr_t) ')');
        }
        else if (complexRegex[i] == '\\') {
            i++;
            if (complexRegex[i] == 'n') {
                pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\n');
            }
            else if (complexRegex[i] == 't') {
                pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\t');
            }
            else {
                if (isSimpleRegexEscapeChar(complexRegex[i])) {
                    pushBackList(&simpleRegexChars, (void*) (uintptr_t) '\\');
                }
                pushBackList(&simpleRegexChars, (void*) (uintptr_t) complexRegex[i]); 
            }
        }
        else {
            pushBackList(&simpleRegexChars, (void*) (uintptr_t) complexRegex[i]);
        }
    }

    char* simpleRegex = (char*) malloc((simpleRegexChars.size+1)*sizeof(char));
    int i = 0;
    for (DoublyNode* node = simpleRegexChars.head; node != NULL; node = node->next) {
        char currentChar = (char) (uintptr_t) node->value;
        simpleRegex[i++] = currentChar;
    }
    simpleRegex[i] = '\0';

    destroyList(&simpleRegexChars);

    return simpleRegex;
}



int detectErrorTokenSpec(char* tokenSpec) {
    int isDetectingRegex = 1;
    for (size_t i = 0; tokenSpec[i] != '\0'; i++) {
        if (isDetectingRegex) {
            if (tokenSpec[i] == ' ') {
                isDetectingRegex = 0;
            }
            else if (tokenSpec[i] == '\\') {
                i++;
                if (!(isComplexRegexEscapeChar(tokenSpec[i]))) {
                    return -1;
                }
            }
        }
        else { //if it is not detecting regex, it is detecting token
            if (tokenSpec[i] == '\n') {
                isDetectingRegex = 1;
            }
            else if (!(('a' <= tokenSpec[i] && tokenSpec[i] <= 'z') || ('A' <= tokenSpec[i] && tokenSpec[i] <= 'Z') || ('0' <= tokenSpec[i] && tokenSpec[i] <= '9') || (tokenSpec[i] == '_'))) {
                return -1;
            }
        }
    }
    if (isDetectingRegex) {
        return -1;
    } 
    return 0;
}

int getDFAsFromTokenSpec(char* tokenSpec, LinkedList* dfas) {
    int errorResult = detectErrorTokenSpec(tokenSpec);
    if (errorResult == -1) {
        return -1;
    }

    initList(dfas);

    size_t l = 0;
    size_t r = 0;
    while (1) {
        if (tokenSpec[r] == '\\') {
            r++;
        }
        else if (tokenSpec[r] == ' ') {
            size_t complexRegexSize = r-l;
            char* complexRegex = (char*) malloc((complexRegexSize+1)*sizeof(char));
            size_t i = 0;
            for (; l != r; l++) {
                complexRegex[i++] = tokenSpec[l];
            }
            complexRegex[i] = '\0';
            l++;

            char* simpleRegex = getSimpleRegexFromComplexRegex(complexRegex);
            free(complexRegex);
            if (simpleRegex == NULL) {
                return -1;
            }

            DFA* currentDFA = (DFA*) malloc(sizeof(DFA));
            int errorResult = makeDFAFromRegex(simpleRegex, currentDFA);
            free(simpleRegex);
            if (errorResult == -1) {
                return -1;
            }

            pushBackList(dfas, (void*) currentDFA);
        }
        else if (tokenSpec[r] == '\n' || tokenSpec[r] == '\0') {
            size_t tokenNameSize = r-l;
            char* tokenName = (char*) malloc((tokenNameSize+1)*sizeof(char));
            size_t i = 0;
            for (; l != r; l++) {
                tokenName[i++] = tokenSpec[l];
            }
            tokenName[i] = '\0';
            l++; 

            DFA* currentDFA = (DFA*) dfas->tail->value; 
            currentDFA->tokenName = tokenName;
            if (tokenSpec[r] == '\0') {
                return 0;
            }
        }
        r++;
    }
}