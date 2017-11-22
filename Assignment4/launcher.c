//Camille Chow
//ECE 357 PSET 4

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

char* dictionary = "dict.txt"; //dictionary file for wordsearch

void redir(int fd_pipe, int fd_std)
{
    if (dup2(fd_pipe, fd_std) < 0) {
        fprintf(stderr, "Can’t dup2 fd %d to fd %d: %s\n", fd_std, fd_pipe, strerror(errno));
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
        _exit(-1);
    }
    if (pipe(pipe2fd) == -1)
    {
        fprintf(stderr, "Error creating second pipe: %s\n", strerror(errno));
        _exit(-1);
    }
    //forks
    int pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "Error forking: %s\n", strerror(errno));
        _exit(-1);
    }
    else if (pid == 0) //child - wordgen
    {
        redir(pipe1fd[1], 1); //redirect stdout to input of first pipe
        close(pipe1fd[0]); close(pipe1fd[1]); close(pipe2fd[0]); close(pipe2fd[1]);
        if (execlp("./wordgen","./wordgen",argv[1],(char*)NULL) < 0)
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
            _exit(-1);
        }
        else if (pid2 == 0) //child - wordsearch
        {
            redir(pipe1fd[0], 0); //redirect stdin to output of first pipe
            redir(pipe2fd[1], 1); //redirect stdout to input of second pipe
            close(pipe1fd[0]); close(pipe1fd[1]); close(pipe2fd[0]); close(pipe2fd[1]);
            if (execlp("./wordsearch","./wordsearch",dictionary,(char*)NULL) < 0)
            {
                fprintf(stderr,"Error executing wordsearch: %s\n", strerror(errno));
                _exit(-1);
            }
        }
        else //parent - launcher
        {
            int pid3 = fork();
            if (pid3 == -1)
            {
                fprintf(stderr, "Error forking: %s\n", strerror(errno));
                _exit(-1);
            }
            else if (pid3 == 0) //child - pager
            {
                redir(pipe2fd[0], 0); //redirect stdin to output of second pipe
                close(pipe1fd[0]); close(pipe1fd[1]); close(pipe2fd[0]); close(pipe2fd[1]);
                if (execlp("./pager","./pager",(char*)NULL) < 0)
                {
                    fprintf(stderr,"Error executing pager: %s\n", strerror(errno));
                    _exit(-1);
                }
            }
        }
    }
    close(pipe1fd[0]); close(pipe1fd[1]); close(pipe2fd[0]); close(pipe2fd[1]);
    int status = -1;
    int p;
    for (int i = 0; i < 3; i++)
    {
        if ((p = wait(&status)) < 0)
            fprintf(stderr,"Error waiting for child: %s\n", strerror(errno));
        else
            fprintf(stderr, "Child %d exited with status %d\n", p, status);
    }
    return 0;
}