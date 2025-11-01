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

    int i=0;
    int background = 0;

    char *token_array[40];

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
            else{
                token_array[i++] = token;
            }
            token = strtok(NULL, " "); // iterate next token
        }

        token_array[i] = NULL;

        pid_t child = fork(); //make new child process

        if(child == 0) { // if child
            printf("Child PID : %d\n", getpid());
            execvp(token_array[0], token_array);

            perror("execvp failed");
            exit(1);
        }
        else if(child > 0){ //if parent
            if(!background) waitpid(child, NULL, 0);
            else printf("Starting backgroung process with PID : %d\n", child);
        }

        memset(token_array, 0, sizeof(token_array));
        i=0;
        background=0;
            
    }

    return 0;
}
