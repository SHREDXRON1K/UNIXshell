#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main() {
    char input[100];    // for user commands
    char confirm[10];   // for confirmation when exiting
    //int input_size;

    int i=0, j=0;

    int background = 0;
    int pipe_exist = 0;
    int pipefd[2];

    char *token_array[40];
    char *token_array2[40];

    pid_t child2;

    while (1) {
        printf("");
        printf("MyShell$: ");
        
        // Read the full command line
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }

        // Strip newline character
        input[strcspn(input, "\n")] = '\0';
        // input_size = sizeof(input) / sizeof(input[0]);

        // Handle 'exit' command
        if (strcmp(input, "exit") == 0) {
            printf("Are you sure you want to exit? (y/n): ");
            
            if (fgets(confirm, sizeof(confirm), stdin) == NULL) {
                printf("Error reading confirmation.\n");
                continue;
            }

            // Strip newline from confirmation
            confirm[strcspn(confirm, "\n")] = '\0';

            // Compare input
            if (strcmp(confirm, "y") == 0 || strcmp(confirm, "Y") == 0) {
                printf("Exiting shell...\n");
                break;
            }
            if (strcmp(confirm, "n") == 0 || strcmp(confirm, "N") == 0) {
                continue;
            }
            printf("Unknown input. Please enter 'y' or 'n'.\n");
            continue;
            
        }

        // Example: Handle 'pwd'
        if (strcmp(input, "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd");
            }
            continue;
        }

        //Tokenizing
        char *token = strtok(input, " ");
        while(token !=NULL && i<39){
            if(strcmp(token, "&") == 0){
                background = 1;
            }
            else if(strcmp(token, "|") == 0){
                pipe_exist = 1;

            }
            else{
                if(!pipe_exist) token_array[i++] = token;
                if(pipe_exist) token_array2[j++] = token;
                
            }
            token = strtok(NULL, " "); // iterate next token
        }

        token_array[i] = NULL;
        token_array2[j] = NULL;

        if(pipe_exist) pipe(pipefd);  // Create pipe once

        pid_t child1 = fork();
        if(child1 == 0) {
            // Left side: `ls`
            if(pipe_exist){
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);  // Close unused read end
                close(pipefd[1]);
            }
            printf("Child1 PID: %d\n", getpid());
            execvp(token_array[0], token_array);
            perror("execvp failed");
            exit(1);
        }

        if(pipe_exist){
            pid_t child2 = fork();
            if(child2 == 0) {
                // Right side: `wc -l`
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[1]);  // Close unused write end
                close(pipefd[0]);
                printf("Child2 PID: %d\n", getpid());
                execvp(token_array2[0], token_array2);
                perror("execvp failed");
                exit(1);
            }
        }


        if(pipe_exist){
            close(pipefd[0]);
            close(pipefd[1]);
        }

       
        if(!background) {
            waitpid(child1, NULL, 0);
            if(pipe_exist) waitpid(child2, NULL, 0);
        }
        else {
            printf("Starting background process with PID : %d\n", child1);
            if(pipe_exist) printf("Starting background process with PID : %d\n", child2);
        }


        memset(token_array, 0, sizeof(token_array)); //clean token_array
        memset(token_array2, 0, sizeof(token_array2));
        i=0;
        j=0;
        background=0;
        pipe_exist=0;
            
    }

    return 0;
}
