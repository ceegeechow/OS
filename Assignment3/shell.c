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
{                           //close open file descriptors? call the actual exit()???
    if (command[1] != NULL)
    {
        return command[1];
    }
    return e_status;
}

///////////////////////////////////////////////////////////////
//  Supporting functions
///////////////////////////////////////////////////////////////

void procLine(char* line)
{
    char* d = " \t";
    char* token = strtok(line,d);
    int i = 0;
    while (token != NULL)
    {
        command[i] = token;
        i++;
        token = strtok(NULL,d);
    }
    switch (command[0])
    {
        case "cd":
            cd();
            break;
            
        case "exit":
            exit();
            
        default:
            if (command[0][0] != '#') {
                execute();
            }
            break;
    }
}

int execute()
{
    int pid = fork();
    switch (pid)
    {
        case -1:
            fprintf(stderr,"Error forking: %s", strerror(errno)); //more info??
        
        case 0:            
            //e_status = exec(needs filling in);
            break;
            
        default:
            //wait(pid);
            break;
    }
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
            procLine(line);
        }
        else
        {
            fprintf(stderr, "Error reading command from stdin: %s", strerror(errno));
        }
    }
    return exit();
}