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

// int background_flag = 0;
int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];
	char** tokens;
	int i, background_flag = 0, exit_flag = 0;
	int pid_arr[64] = { [0 ... 63] = -1 };
	int status;
	char* ampersand = "&";


	while (1) {
		// Cleaning Stuffs at start of new command
		for (int i = 0;i < 64;i++) {
			if (pid_arr[i] != -1) {
				status = waitpid(pid_arr[i], NULL, WNOHANG);
				if (status > 0) {
					pid_arr[i] = -1;
					printf("Shell: Background process finished \n");
				}
			}
		}

		background_flag = 0; // default foreground

		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		for (i = 0;tokens[i] != NULL;i++) {
			printf("found token %s (remove this debug output later)\n", tokens[i]);
		}
		// **********************START***************************
		printf("*****************************************Start*******************************************\n");

		//do whatever you want with the commands, here we just print them

		/* BEGIN Checking exit or empty command */
		if (tokens[0] == NULL || (strcmp(tokens[0], "exit") == 0)) { // No command entered
			if (tokens[0] == NULL) {
				continue;
			}
			else {
				exit_flag = 1;
				break;
			}
		}
		/* END Checking exit or empty command */

		/* BEGIN CHECKING "&" at end of command */
		i = 0;
		while (tokens[i + 1] != NULL) i++;
		if (strcmp(tokens[i], ampersand) == 0) {
			tokens[i] = NULL; // If found background command making ampersand NULL
			background_flag = 1;
		}
		/* END CHECKING & at end of command */


		/* BEGIN Special Case : Handling cd */
		if (strcmp(tokens[0], "cd") == 0) {
			// for cd even if command is background turning into foreground
			background_flag = 0;
			int res = chdir(tokens[1]);
			if (res != 0) {
				printf("Shell: Incorrect command\n");
			}
			continue;
		}
		/* END Special Case : Handling cd */

		int fc = fork();

		if (fc < 0) { // Error
			fprintf(stderr, "%s\n", "Child creation failed at fork\n");
			_exit(1);
		}
		else if (fc == 0) {  // Child
			execvp(tokens[0], tokens);
			printf("Command execution failed\n");
			_exit(1);

		}
		else { // Parent
			if (background_flag == 0) {
				wait(NULL);
			}
			else { // background process
				status = waitpid(fc, NULL, WNOHANG);
				if (status == 0) {
					for (int i = 0;i < 64;i++) { // add the pid to reap later
						if (pid_arr[i] < 0) {
							pid_arr[i] = fc;
							break;
						}
					}
					printf("[1] \t %d\n $ ", fc);
				}
			}
		}

		printf("**************************************************END****************************************\n");


		// Freeing the allocated memory	
		for (i = 0;tokens[i] != NULL;i++) {
			free(tokens[i]);
		}
		free(tokens);

	}
	if (exit_flag == 1) {
		// Freeing the allocated memory	
		for (i = 0;tokens[i] != NULL;i++) {
			free(tokens[i]);
		}
		free(tokens);
		printf("Memory freed \n");

		// Kill all processes
		for (int i = 0;i < 64;i++) {
			// printf("PID %d \n", pid_arr[i]);
			if (pid_arr[i] > 0) {
				kill(pid_arr[i], 9);
				printf("Killed %d \n", pid_arr[i]);
				pid_arr[i] = -1;
			}
		}
	}
	// **********************END***************************
	return 0;
}
