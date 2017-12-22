#include "spinlock.h"
#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char**argv) {
    if (argc < 3)
    {
        fprintf(stderr, "not enough input arguments\n");
        return 0;
    }
    int nchild = atoi(argv[1]);
    int niter = atoi(argv[2]);
    struct spinlock sl;
    int* map;
    sp_init(&sl);
    if ((map = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing: %s\n", strerror(errno));
        exit(255);
    }
    map[0] = 0;
    pid_t pid;
    for (int i = 0; i < nchild; i++) {
        switch (pid = fork()) {
            case -1:
                fprintf(stderr,"Error forking: %s\n", strerror(errno));
                break;
                
            case 0:
                spin_lock(&sl);
                for (int j = 0; j < niter; j++) {
                    map[0]++;
                }
                spin_unlock(&sl);
                _exit(0);
                break;
        }
    }
    for (int k = 0; k < nchild; k++) {
        wait(NULL);
    }
    printf("map[0]: %d\n", map[0]);
    return 0;
}