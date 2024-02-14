#include "utilities.h"
#include "commands.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



// Function to trim whitespace from the start and end of a string
char *trim_whitespace(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str; // All spaces

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0'; // Write new null terminator

    return str;
}

// Function to parse the input string into arguments
char **parse_input(char *input)
{
    // Allocate memory to hold the parsed arguments.
    char **args = malloc(MAX_ARGS * sizeof(char *));

    // Pointer to hold each arg as we parse through the input string
    char *token; 

    // Index for filling the args array with tokens
    int i = 0;

    // Extract the first arg from the input string. Each arg is separated by space or newline
    token = strtok(input, " \n");

    // Loop through the input string and extract the rest of the tokens 
    while (token != NULL && i < MAX_ARGS - 1)
    {
        // Assign the current token to the args array and increment the index.
        args[i++] = token;

        // Extract the next token from the input string, using NULL as the first argument to continue from where strtok left off.
        token = strtok(NULL, " \n");
    }

    // Null terminate
    args[i] = NULL;

    // Return the array of parsed arguments.
    return args;
}

// Function to count the number of occurrences of a character in a string
int count_char_occurrences(const char *str, char ch)
{
    int count = 0;
    for (; *str != '\0'; str++)
    {
        if (*str == ch)
        {
            count++;
        }
    }
    return count;
}
