#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void redir(int fd_pipe, int fd_std)
{
    if (dup2(fd_pipe, fd_std) < 0) {
        fprintf(stderr, "Can’t dup2 fd %d to fd %d: %s\n", fd, fd_pipe, strerror(errno));
        _exit(-1);
    }
    if (close(fd_pipe) != 0) {
        fprintf(stderr, "Error closing file descriptor %d: %s\n", fd, strerror(errno));
        _exit(-1);
    }
}

int main(int argc, char** argv)
{
    //input arguments
    char* arg = NULL;
    if (argc > 1)
        arg = argv[1];
    //create pipes
    int pipe1fd[2];
    int pipe2fd[2];
    if (pipe(pipe1fd) == -1)
    {
        fprintf(stderr, "Error creating first pipe: %s\n", strerror(errno));
        return -1;
    }
    if (pipe(pipe2fd) == -1)
    {
        fprintf(stderr, "Error creating second pipe: %s\n", strerror(errno));
        return -1;
    }
    //forks
    int pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "Error forking: %s\n", strerror(errno));
        return -1;
    }
    else if (pid == 0) //child - wordgen
    {
        redir(pipe1fd[0], 1); //redirect stdout to input of first pipe
        if (execlp("wordgen","wordgen",arg) < 0)
        {
            fprintf(stderr,"Error executing wordgen: %s\n", strerror(errno));
            _exit(-1);
        }
    }
    else //parent - launcher
    {
        int pid2 = fork();
        if (pid2 == -1)
        {
            fprintf(stderr, "Error forking: %s\n", strerror(errno));
            return -1;
        }
        else if (pid2 == 0) //child - wordsearch
        {
            
        }
    }
    return 0;
}