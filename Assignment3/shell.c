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

int status = -1;
char* command[BUFSIZ];
char* IO_buff[BUFSIZ];

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
        exit(atoi(command[1]));
    }
    //fprintf(stderr, "exit status: %d\t %d\t%d\n", status, status&0377, 0377);
    exit(status);
}

int isRedir(char* arg)
{
    if (arg[0] == '<')
    {
        return 1; // <filename: redirect stdin
    }
    else if (arg[0] == '>')
    {
        if (arg[1] == '>')
        {
            return 4; // >>filename: redirect stdout (with append)
        }
        return 2; // >filename: redirect stdout (with truncate)
    }
    else if (arg[0] == '2' && arg[1] == '>')
    {
        if (arg[2] == '>')
        {
            return 5;  // 2>>filename: redirect stderr (with append)
        }
        return 3;  // 2>filename: redirect stderr (with truncate)
    }
    return 0;
}

int redir()
{
    char* filename, *std_stream;
    char* mode = "writing";
    int fd, std_fd, offset, flags;
    
    for (int i = 0; IO_buff[i] != NULL; i++)
    {
        switch (isRedir(IO_buff[i]))
        {
            case 1:
                std_stream = "stdin";
                std_fd = 0;
                offset = 1;
                flags = O_RDONLY;
                mode = "reading";
                break;
            case 2:
                std_stream = "stdout";
                std_fd = 1;
                offset = 1;
                flags = O_CREAT|O_TRUNC|O_WRONLY;
                break;
            case 3:
                std_stream = "stderr";
                std_fd = 2;
                offset = 2;
                flags = O_CREAT|O_TRUNC|O_WRONLY;
                break;
            case 4:
                std_stream = "stdout";
                std_fd = 1;
                offset = 2;
                flags = O_CREAT|O_APPEND|O_WRONLY;
                break;
            case 5:
                std_stream = "stderr";
                std_fd = 2;
                offset = 3;
                flags = O_CREAT|O_APPEND|O_WRONLY;
                break;
            default:
                return -1;
        }
        filename = &IO_buff[i][offset];

        if ((fd=open(filename,flags,0666)) < 0)
        {
            fprintf(stderr,"Can’t open file %s for %s: %s\n",filename,mode,strerror(errno));
            return -1;
        }
        if (dup2(fd,std_fd) < 0) {
            fprintf(stderr, "Can’t dup2 %s to %s: %s\n", filename, std_stream, strerror(errno));
            return -1;
        }
        if (close(fd) != 0) {
            fprintf(stderr, "Error closing file '%s': %s\n", filename, strerror(errno));
            return -1;
        }
    }
    return 0;
}

void procLine(char* line)
{
    char* d = " \t\n";
    char* token = strtok(line,d);
    int ac1 = 0;
    int ac2 = 0;
    struct rusage ru;
    struct timeval start, stop;
    
    //parse non-IO redirection arguments
    while (token != NULL && isRedir(token) == 0)
    {
        command[ac1] = token;
        ac1++;
        token = strtok(NULL,d);
    }
    //parse IO redirection arguments
    while (token != NULL && isRedir(token) != 0)
    {
        IO_buff[ac2] = token;
        ac2++;
        token = strtok(NULL,d);
    }
    //check for built-in commands
    if (strcmp(command[0],"cd") == 0)
        cd();

    else if (strcmp(command[0],"exit") == 0)
        ex();

    //check for comment
    else if (command[0][0] != '#')
    {
        int pid;
        gettimeofday(&start, NULL); //start timing execution (real time)
        //fork, execute, and wait
        switch (pid = fork())
        {
            case -1: //error
                fprintf(stderr,"Error forking: %s\n", strerror(errno));
                break;
                
            case 0: //child
                if (redir() < 0) //redirect IO if necessary
                {
                    fprintf(stderr,"Error establishing IO redirection, command not launched\n");
                    _exit(-1);
                }
                if (execvp(command[0],command) < 0)
                {
                    fprintf(stderr,"Error executing command '%s': %s\n", command[0], strerror(errno));
                    _exit(-1);
                }
                break;
                
            default: //parent
                if (wait3(&status, 0, &ru) < 0)
                {
                    fprintf(stderr,"Error waiting for child (executing %s): %s\n", command[0], strerror(errno));
                    break;
                }
                gettimeofday(&stop, NULL); //stop timing once wait returns
                fprintf(stderr, "Process cosumed %ld.%.6d real seconds, %ld.%.6d user, %ld.%.6d system\n", stop.tv_sec - start.tv_sec, stop.tv_usec - start.tv_usec, ru.ru_utime.tv_sec, ru.ru_utime.tv_usec, ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
                break;
        }
    }
    //clear command buffers
    for (int i = 0; i < ac1; i++)
        command[i] = NULL;
    
    for (int j = 0; j < ac2; j++)
        IO_buff[j] = NULL;
}

int main(int argc, char* argv[])
{
    FILE* input = stdin;
    char* line;
    if (argv[1] != NULL)
    {
        printf("arg found\n");
        if ((input = fopen(argv[1],"r")) == NULL)
        {
            fprintf(stderr, "Error opening %s for reading: %s\nEntering regular shell mode\n", argv[1], strerror(errno));
            input = stdin;
        }
    }
    else {
        printf("no args\n");
        
    }
    while (1)
    {
        printf("> ");
        if (fgets(line, BUFSIZ, input) != NULL)
        {
            printf("got line\n");
            if (strcmp(line,"\n") == 0)
                continue;
            procLine(line);
        }
        else if (feof(input) == 0)
            fprintf(stderr, "Error reading command from stdin: %s\n", strerror(errno));
        else
            break;
    }
    printf("\n");
    ex();
}