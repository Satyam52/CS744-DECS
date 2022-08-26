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

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

int lastChar(char **token){
	int i = 0;
	while(token[i]!=NULL){
		i++;
	}
	return i;
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;


	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
		for(i=0;tokens[i]!=NULL;i++){
			printf("found token %s (remove this debug output later)\n", tokens[i]);
		}
		printf("*****************************************Start*******************************************\n\n");

       //do whatever you want with the commands, here we just print them

		if(tokens[0]==NULL){ // No command entered
			continue;
		}

		int fc = fork();

		if (fc < 0) {
            fprintf(stderr,"%s\n","Child creation failed at fork\n");
            _exit(1);
        }
        else if (fc == 0) {
			// if(strcmp( tokens[lastChar(tokens)], "&" )){ // background test case
			// 		continue;
			

			// }else{
				if(strcmp(tokens[0],"cd")==0){ // for cd
					int res = chdir(tokens[1]);
					if(res!=0){
						printf("Shell: Incorrect command\n");
					}
				}else{                         // without cd
					execvp(tokens[0],tokens);
					printf("Command execution failed\n");
					_exit(1);
				}
			}
        // }
        else {
            int wc = waitpid(fc,NULL,0); // wait to reap to child
            printf("Reaped the Child %d\n",WNOHANG);
        }

		printf("\n**************************************************END****************************************\n");

       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
