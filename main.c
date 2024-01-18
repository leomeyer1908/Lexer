#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"

void printMatrix(size_t** matrix, size_t rowSize, size_t colSize) {
    for (size_t i = 0; i < rowSize; i++) {
        printf("[");
        for (size_t j = 0; j < colSize; j++) {
            printf("%zu", matrix[i][j]);
            if (j != colSize-1) {
                printf(",\t");
            }
        }
        if (i != rowSize-1) {
            printf("],\n");
        }
        else {
            printf("]\n");
        }
    }

}

char* getFileText(const char* filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc((file_size+1)*sizeof(char));

    fread(buffer, 1, file_size, file);

    buffer[file_size] = '\0';

    fclose(file);

    return buffer;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("usage: <token spec> <input>\n");
        return 1;
    }

    char* tokenSpec = getFileText(argv[1]);
    if (tokenSpec == NULL) {
        return 1;
    }
    char* input = getFileText(argv[2]);
    if (input == NULL) {
        return 1;
    }

    LinkedList tokens;
    initList(&tokens);

    int errorResult = tokenizeInputUsingSpec(input, tokenSpec, &tokens);
    if (errorResult == -1) {
        printf("An error occured!\n");
    }
    else {
        printf("Tokens:\n");
        for (DoublyNode* node = tokens.head; node != NULL; node = node->next) {
            Token* currentToken = (Token*) node->value;
            printf("%s:%s [%zu, %zu]\n", currentToken->tokenName, currentToken->value, currentToken->lineNum, currentToken->colNum);
        }
    }

    free(tokenSpec);
    free(input);

    return errorResult;

    
}