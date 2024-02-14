#include "commands.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

char *shell_path[MAX_ARGS]; // Shell paths paths to search for executables
char exec_path[MAX_INPUT];  // Buffer for the executable path

// Function to update the shell's search path
void set_shell_path(char **args)
{
    memset(shell_path, 0, sizeof(shell_path)); // Clear existing paths
    int i = 1;                                 // Start from args[1] to skip the 'path' command itself
    int j = 0;                                 // Index for shell_path
    while (args[i] != NULL && j < MAX_ARGS - 1)  // loop through each path excluding path command
    {
        shell_path[j] = strdup(args[i]); // Copy path to shell_path
        i++;
        j++; // Increment number of ARGS
    }
    shell_path[j] = NULL; // NULL-terminate the array
}

// Function to find the executable in the shell's search paths
int find_executable(char **args)
{
    for (int i = 0; shell_path[i] != NULL; i++) // loop through each defined shell path
    {
        snprintf(exec_path, sizeof(exec_path), "%s/%s", shell_path[i], args[0]); // create executable path by combining shell path and argument
        if (access(exec_path, X_OK) == 0)
        {
            return 0; // Executable found
        }
    }
    return -1; // Executable not found
}