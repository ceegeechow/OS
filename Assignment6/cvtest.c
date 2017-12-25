#include "fifo.h"
#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	struct cv* cond;
	void *map;
	if ((map = mmap(NULL, 16*MYFIFO_BUFSIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing: %s\n", strerror(errno));
        exit(255);
    }
    cond = (struct cv *) map;
    cv_init(cond);
	struct spinlock mutex;
	sp_init(&mutex);
	pid_t pid;
	for (int i = 0; i < 5; i++) {
        switch (pid = fork()) {
            case -1:
                fprintf(stderr,"Error forking: %s\n", strerror(errno));
                break;    
            case 0:
            	spin_lock(&mutex);
                cv_wait(cond,&mutex);
                printf("im awake: %d\n",getpid());
                spin_unlock(&mutex);
                _exit(0);
                break;
        }
    }
    sleep(1);
    for (int k = 0; k < 5; k++) {
        cv_signal(cond);
        sleep(1);
    }
    for (int k = 0; k < 5; k++) {
        wait(NULL);
    }
	return 0;
}