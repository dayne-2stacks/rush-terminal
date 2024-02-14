#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h> // For open flags
#include <string.h>
#include <unistd.h>  // This covers chdir, fork, execv, access, dup2, close, and STDOUT_FILENO
// Include this if strsep is not available in string.h
// #include <strings.h>


#include "utilities.h"
#include "commands.h"

#define MAX_PROCESSES 10 // Adjust based on expected maximum number of parallel commands
#define PATH_INIT "/usr/bin"
#define ERROR_MESSAGE "An error has occurred\n"

// Function to print the shell prompt
void print_prompt() {
    printf("rush> ");
    fflush(stdout);
}

int main(int argc, char *argv[])
{

    /* Check if other arguments passed at runtime                                 */

    if (argc != 1)
    {
        fprintf(stderr, ERROR_MESSAGE);

        exit(1);
    }

    char *input = NULL;
    size_t input_size = 0;
    ssize_t line_size;
    pid_t pids[MAX_PROCESSES];
    int num_processes;

    // Initialize the shell path with /bin
    memset(shell_path, 0, sizeof(shell_path));
    shell_path[0] = strdup(PATH_INIT);

    // Main loop
    while (1)
    {
        print_prompt();
        num_processes = 0; // Reset the number of processes for this iteration

        // Read input
        line_size = getline(&input, &input_size, stdin);
        if (line_size <= 0)
        {
            free(input);
            continue; // Error or EOF
        }

        // Duplicate input to process commands
        char *input_copy = strdup(input);
        char *next_command = input_copy;
        char *current_command;

        // Process each command separated by '&' in parallel
        while ((current_command = strsep(&next_command, "&")) != NULL && num_processes < MAX_PROCESSES)
        {
            current_command = trim_whitespace(current_command); // Trim whitespace

            if (*current_command == '\0')
                continue; // Skip empty commands

            if (count_char_occurrences(current_command, '>') > 1)
            {
                fprintf(stderr, ERROR_MESSAGE);
                continue;
            }

            // Parse the command into arguments
            char **args = parse_input(current_command);

            if (args[0] == NULL)
            { // Skip if no arguments
                free(args);
                continue;
            }

            // Handle special commands ('exit', 'cd', 'path') directly
            if (strcmp(args[0], "exit") == 0)
            {

                if (args[1] != NULL)
                {
                    fprintf(stderr, ERROR_MESSAGE); // exit should not be called with args
                    break;
                }
                free(input_copy);
                free(input);
                exit(0);
            }
            else if (strcmp(args[0], "cd") == 0)
            {
                // if args[1] is empty throw an error
                if (args[1] == NULL)
                {
                    fprintf(stderr, ERROR_MESSAGE);
                    break;
                }

                // if there are more than one error throw error
                if (args[2])
                {
                    fprintf(stderr, ERROR_MESSAGE);
                    break;
                }
                // 'cd' is not handled in parallel since it affects the shell process itself
                if (args[1] != NULL && chdir(args[1]) != 0)
                {
                    fprintf(stderr, ERROR_MESSAGE);
                }
                free(args);
                continue;
            }
            else if (strcmp(args[0], "path") == 0)
            {
                set_shell_path(args);
                free(args);
                continue;
            }

            // Handle external commands
            if (find_executable(args) != -1)
            {
                pid_t pid = fork();
                if (pid == 0)
                { // Child process
                    // Look for redirection character ('>') in the command
                    for (int i = 0; args[i] != NULL; i++)
                    {
                        if (strcmp(args[i], ">") == 0)
                        {
                            if (args[i + 1] == NULL)
                            { // No filename specified after '>'
                                fprintf(stderr, ERROR_MESSAGE);

                                exit(1);
                            }

                            if (args[i+2] != NULL) { // checks for more arguments after >
                                fprintf(stderr, ERROR_MESSAGE);

                                exit(1);
                            }

                            // Check if file exists and is not writable
                            if (access(args[i + 1], F_OK) == 0 && access(args[i + 1], W_OK) != 0)
                            {
                                fprintf(stderr, ERROR_MESSAGE);
                                exit(1);
                            }

                            // Open the file for redirection
                            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                            if (fd == -1)
                            {
                                fprintf(stderr, ERROR_MESSAGE);
                                exit(1);
                            }

                            // Redirect stdout to the file
                            dup2(fd, STDOUT_FILENO);
                            close(fd);

                            // Remove the redirection operator and filename from the arguments list
                            args[i] = NULL; // NULL-terminate the arguments list at the position of '>'
                            break;          // Only support one redirection per command
                        }
                    }

                    execv(exec_path, args);
                    // If execv returns, it must have failed
                    fprintf(stderr, ERROR_MESSAGE);

                    exit(1);
                }
                else if (pid > 0)
                {
                    // Parent process, wait for the child if not running commands in parallel
                    if (next_command == NULL || *next_command == '\0')
                    {
                        waitpid(pid, NULL, 0);
                    }
                    else
                    {
                        pids[num_processes++] = pid; // Store the child PID for later waiting
                    }
                }
                else
                {
                    fprintf(stderr, ERROR_MESSAGE);
                }
            }
            else
            {
                fprintf(stderr, ERROR_MESSAGE);
            }

            free(args); // Free the arguments array
        }

        free(input_copy); // Free the duplicated input string

        // Wait for all child processes to finish
        for (int i = 0; i < num_processes; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
    }

    free(input); // Free the input string
    return 0;
}
