#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX 111
char** tokenize(char* line)
			{
    char** tokens = (char**)malloc(MAX_NUM_TOKENS * sizeof(char*));
    char* token = (char*)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for (i = 0; i < strlen(line); i++) {

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0) {
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE * sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        }
        else {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}

int main(int argc, char* argv[]) {
    char  line[MAX_INPUT_SIZE];
    char** tokens;
    int i;
    bzero(line, sizeof(line));
    printf("$ ");
    scanf("%[^\n]", line);
    getchar();
    line[strlen(line)] = '\n';
    tokens = tokenize(line);

    for (i = 0;tokens[i] != NULL;i++) {
        printf("found token %s (remove this debug output later)\n", tokens[i]);
    }
    printf("\n *****************************************\n");
    for (i = 0;tokens[i] != NULL;i++) {
        printf("found token %s (remove this debug output later)\n", tokens[i]);
    }

    return 0;
}
