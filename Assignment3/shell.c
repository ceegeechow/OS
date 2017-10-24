#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

int status = -1; // exit status of last spawned child
char* command[BUFSIZ];

int cd()
{
    if (command[1] == NULL)
    {
        fprintf(stderr, "Error: can't cd, no path specified\n");
        return -1;
    }
    if (chdir(command[1]) < 0)
    {
        fprintf(stderr, "Error: can't cd into directory '%s': %s\n", command[1], strerror(errno));
        return -1;
    }
    return 0;
}

int ex()
{
    if (command[1] != NULL)
    {
        exit(atoi(command[1])); //exit() or _exit()???
    }
    exit(status);
}

void procLine(char* line)
{
    char* d = " \t\n";
    char* token = strtok(line,d);
    int arg_num = 0;
    struct rusage ru;
    time_t start;
    time_t end;
    time_t s;
    time_t e;
    
    while (token != NULL)
    {
        command[arg_num] = token;
        arg_num++;
        token = strtok(NULL,d);
    }
    if (strcmp(command[0],"cd") == 0)
    {
        cd();
    }
    else if (strcmp(command[0],"exit") == 0)
    {
        ex();
    }
    else if (command[0][0] != '#')
    {
        int pid;
        switch (pid = fork())
        {
            case -1:
                fprintf(stderr,"Error forking: %s\n", strerror(errno)); //more info??
                
            case 0:
                s = time(&start);
                if (execvp(command[0],command) < 0)
                {
                    fprintf(stderr,"Error executing command '%s': %s\n", command[0], strerror(errno));
                }
                //do i have to do anything to clean environment/exit child???
                break;
                
            default:
                if (wait3(&status, 0, &ru) < 0) //options???
                {
                    fprintf(stderr,"Error waiting for child (executing %s): %s\n", command[0], strerror(errno));
                }
                e = time(&end);
                fprintf(stderr, "time: %f seconds\n", difftime(e, s));
                //fprintf(stderr, "Process cosumed %ld real seconds, %ld.%.6d user, %ld.%.6d system\n", (*end - *start), ru.ru_utime.tv_sec, ru.ru_utime.tv_usec, ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
                break;
        }
    }
}

int main()
{
    char* line;
    while (1)
    {
        printf("> ");
        if (fgets(line, BUFSIZ, stdin) != NULL)
        {
            if (strcmp(line,"\n") == 0)
            {
                continue;
            }
            procLine(line);
        }
        else if (feof(stdin) == 0)
        {
            fprintf(stderr, "Error reading command from stdin: %s\n", strerror(errno));
        }
        else
        {
            break;
        }
    }
    printf("\n");
    ex();
}