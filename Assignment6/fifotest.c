#include "fifo.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char**argv) {
	if (argc < 3)
    {
        fprintf(stderr, "not enough input arguments\n");
        return 0;
    }
    int nwriters = atoi(argv[1]);
    int nitems = atoi(argv[2]);
	struct fifo* f;
	void *map;
	if ((map = mmap(NULL, 16*MYFIFO_BUFSIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing: %s\n", strerror(errno));
        exit(255);
    }
    f = (struct fifo *) map;
    fifo_init(f);
	pid_t pid;
	for (int i = 0; i < nwriters; i++) { //create writers
        switch (pid = fork()) {
            case -1:
                fprintf(stderr,"Error forking: %s\n", strerror(errno));
                break;    
            case 0:
                for (long i = 0; i < nitems; i++) {
                    fifo_wr(f,i);
                    
                }
                fprintf(stderr, "writer %d completed\n", i);
                _exit(0);
                break;
        }
    }
    switch (pid = fork()) { //create reader
        case -1:
            fprintf(stderr,"Error forking: %s\n", strerror(errno));
            break;    
        case 0:
        	for (long i = 0; i < nitems*nwriters; i++) {
                fprintf(stderr, "%d", fifo_rd(f));
            }
            fprintf(stderr, "\nreader completed\n");
            _exit(0);
            break;
    }
    for (int k = 0; k < nwriters + 1; k++) {
        wait(NULL);
    }
	return 0;
}