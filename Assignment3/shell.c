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

//problems with cwd (ls and pwd sometimes don't work)
//IO issues (stderr doesn't work, pretty version doesn't work)
//exiting correctly?
//always echos 0 if not given a value
//make io commands global? make command local?

int status = -1;
char* command[BUFSIZ];//smaller???
char* IOcommands[BUFSIZ];

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
    else if (arg[0] == '2' && arg[0] == '>')
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
    int fd;
    char* filename;
    char* std_stream;
    char* mode = "writing";
    int std_fd;
    int offset;
    int flags;
    
    for (int i = 0; IOcommands[i] != NULL; i++)
    {
        fprintf(stderr, "io redirect: %s\n", IOcommands[i]);
        switch (isRedir(IOcommands[i]))
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
        filename = &IOcommands[i][offset];

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
    struct timeval start;
    struct timeval stop;
    
    
    //parse non-IO redirection arguments
    while (token != NULL && isRedir(token) == 0)
    {
        command[ac1] = token;
        ac1++;
        token = strtok(NULL,d);
    }
    //parse IO redirection arguments
    while (token != NULL && isRedir(token) != 0)           //can this be better?
    {
        IOcommands[ac2] = token;
        ac2++;
        token = strtok(NULL,d);
    }
    //check for built-in commands
    if (strcmp(command[0],"cd") == 0)
    {
        cd();
    }
    else if (strcmp(command[0],"exit") == 0)
    {
        ex();
    }
    //check for comment
    else if (command[0][0] != '#')
    {
        int pid;
        //start timing execution (real time)
        gettimeofday(&start, NULL);
        switch (pid = fork())
        {
            case -1: //error
                fprintf(stderr,"Error forking: %s\n", strerror(errno));
                
            case 0: //child
                //redirect IO if necessary
                redir();
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
                //stop timing once wait returns
                gettimeofday(&stop, NULL);
                fprintf(stderr, "Process cosumed %ld.%.6d real seconds, %ld.%.6d user, %ld.%.6d system\n", stop.tv_sec - start.tv_sec, stop.tv_usec - start.tv_usec, ru.ru_utime.tv_sec, ru.ru_utime.tv_usec, ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
                break;
        }
    }
    //clear command buffers
    for (int i = 0; i < ac1; i++) {
        command[i] = NULL;
    }
    for (int j = 0; j < ac2; j++) {
        IOcommands[j] = NULL;
    }
}

int main()
{
    char* line;
    while (1)
    {
//        fprintf(stderr,"contents of command: %s, %s, %s\n", command[0], command[1], command[2]);
//        fprintf(stderr,"contents of IOcommand: %s, %s, %s\n", IOcommands[0], IOcommands[1], IOcommands[2]);
        printf("> ");
        if (fgets(line, BUFSIZ, stdin) != NULL)
        {
            if (strcmp(line,"\n") == 0)
                continue;
            procLine(line);
        }
        else if (feof(stdin) == 0)
            fprintf(stderr, "Error reading command from stdin: %s\n", strerror(errno));
        else
            break;
    }
    printf("\n");
    ex();
}