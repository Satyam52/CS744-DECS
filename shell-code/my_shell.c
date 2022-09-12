#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include<signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_PROCESSES 64
int foreground_running = 0;

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line) {
    char **tokens = (char **) malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *) malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for (i = 0; i < strlen(line); i++) {

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0) {
                tokens[tokenNo] = (char *) malloc(MAX_TOKEN_SIZE * sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        } else {
            token[tokenIndex++] = readChar;
        }
    }
    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}

void sigint_handler(int signal_code) {
    if (foreground_running == 0) {
        printf("\n$ ");
    } else {
        printf("\n");
    }
    fflush(stdout);
}


int main(int argc, char *argv[]) {
    char line[MAX_INPUT_SIZE];
    char **tokens = NULL;
    int i, background_flag = 0;
    int pid_arr[MAX_PROCESSES] = {[0 ... MAX_PROCESSES - 1] = -1};
    int status;
    int bg_grp_id = -1;
    signal(SIGINT, sigint_handler);

    while (1) {

        // Free up the allocated memory before allocating again
        if (tokens != NULL) {
            for (i = 0; tokens[i] != NULL; i++) {
                free(tokens[i]);
            }
            free(tokens);
        }

        // Cleaning zombie/terminated processes at start of new command
        for (i = 0; i < MAX_PROCESSES; i++) {
            if (pid_arr[i] != -1) {
                status = waitpid(pid_arr[i], NULL, WNOHANG);
                if (status > 0) {
                    pid_arr[i] = -1;
                    if (status == bg_grp_id) {  // if grp leader is dead
                        bg_grp_id = -1;
                    }
                    printf("Shell: Background process finished \n");
                }
            }
        }

        background_flag = 0; // default foreground
        foreground_running = 0;    // foreground process running state

        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        printf("$ ");
        scanf("%[^\n]", line);
        getchar();
        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);

        /* BEGIN Checking exit or empty command */
        if (tokens[0] == NULL || (strcmp(tokens[0], "exit") == 0)) { // No command entered or exit entered
            if (tokens[0] == NULL) {
                continue; // Memory will be freed at next iteration
            } else {
                break; // Memory is getting freed at bottom
            }
        }
        /* END Checking exit or empty command */

        /* BEGIN CHECKING "&" at end of command */
        i = 0;
        while (tokens[i + 1] != NULL) i++;
        if (i != 0 && strcmp(tokens[i], "&") == 0) {
            // If found background command
            free(tokens[i]);
            tokens[i] = NULL;
            background_flag = 1;
        }
        /* END CHECKING & at end of command */

        /* BEGIN Special Case : Handling cd */
        if (strcmp(tokens[0], "cd") == 0) {
            // for cd even if command is background turning into foreground
            int res = chdir(tokens[1]);
            if (res != 0) {
                printf("Shell: Incorrect command\n");
            }
            // Memory will be freed at next iteration
            continue;
        }
        /* END Special Case : Handling cd */

        int fc = fork();

        if (fc < 0) { // Error
            fprintf(stderr, "%s\n", "Child creation failed at fork\n");
            _exit(1);
        } else if (fc == 0) {  // Child
            execvp(tokens[0], tokens);
            printf("Command execution failed\n");
            _exit(1);
        } else { // Parent
            if (background_flag == 0) { // foreground
                foreground_running = 1;
                waitpid(fc, NULL, 0);
            } else { // background process
                if (bg_grp_id == -1) {
                    bg_grp_id = fc;
                }
                status = waitpid(fc, NULL, WNOHANG);
                if (status == 0) {
                    for (i = 0; i < MAX_PROCESSES; i++) { // add the pid to reap later
                        if (pid_arr[i] < 0) {
                            pid_arr[i] = fc;
                            setpgid(fc, bg_grp_id);
                            break;
                        }
                    }
                    printf("[%d] \t %d\n", i + 1, fc);
                }
            }
        }
    }
    // Free up the allocated memory after exiting
    for (i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);

    // Kill and reap all processes
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (pid_arr[i] > 0) {
            kill(pid_arr[i], 9);
            waitpid(pid_arr[i], NULL, 0);
            printf("Killed and reaped %d \n", pid_arr[i]);
            pid_arr[i] = -1;
        }
    }
    return 0;
}
