#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////////
//  Globals
///////////////////////////////////////////////////////////////

int e_status = -1; // exit status
char** command[BUFSIZ];
char* delimit = " \t"; // whitespace delimeters


///////////////////////////////////////////////////////////////
//  Built-in commands
///////////////////////////////////////////////////////////////

int cd()
{
    if (command[1] == NULL)
    {
        fprintf(stderr, "Error: can't cd, no path specified");
        return -1;
    }
    if (chdir(command[1] < 0))
    {
        fprintf(stderr, "Error: can't cd into directory '%s': %s", command[1], strerror(errno));
        return -1;
    }
    return 0;
}

int exit()
{
    if (command[1] != NULL)
    {
        return command[1];
    }
    return e_status;
}

///////////////////////////////////////////////////////////////
//  Supporting functions
///////////////////////////////////////////////////////////////

void parseLine()
{
    
}

int execute()
{
    
}

///////////////////////////////////////////////////////////////
//  main loop
///////////////////////////////////////////////////////////////

int main() {
    
    char* line[BUFSIZ];
    
    while (true)
    {
        if (fgets(line, BUFSIZ, stdin) != NULL || feof(stdin) == 0)
        {
            parseLine(line);
            e_status = execute(command);
        }
        else
        {
            fprintf(stderr, "Error reading command from stdin: %s", strerror(errno));
        }
    }
    return exit();
}