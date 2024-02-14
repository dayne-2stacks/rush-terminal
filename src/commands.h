#ifndef COMMANDS_H
#define COMMANDS_H

#include "utilities.h" // For utility function declarations

#define MAX_ARGS 10
#define MAX_INPUT 255

extern char *shell_path[MAX_ARGS]; // Shell search paths
extern char exec_path[MAX_INPUT];  // Buffer for the executable path

void set_shell_path(char **args);
int find_executable(char **args);

#endif
