#include "cv.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

void handler(int sig) {
}
void cv_init(struct cv* cv) {
	for (int i = 0; i < CV_MAXPROC; i++) {
		cv->waitlist[i] = 0;
	}
	sp_init(&cv->sl);
	cv->wait_cnt = 0;
}
void cv_wait(struct cv* cv, struct spinlock* mutex) {
	sigset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0)
    {
        fprintf(stderr, "Error with sigprocmask: %s\n", strerror(errno));
        _exit(255);
    }
    signal(SIGUSR1, handler);
	spin_lock(&cv->sl);
	if (cv->wait_cnt >= CV_MAXPROC) {
		fprintf(stderr, "Error: too many processes!\n");
        _exit(255);
	}
	cv->waitlist[cv->wait_cnt++] = getpid();
	spin_unlock(mutex);
	spin_unlock(&cv->sl);
	fprintf(stderr,"going to sleep: %d\n", getpid());
	sigsuspend(&oldmask);
	fprintf(stderr, "awake! %d\n", getpid());
	spin_lock(mutex);
}
int cv_broadcast(struct cv* cv) {
	spin_lock(&cv->sl);
	for (int i = 0; i < cv->wait_cnt; i++) {
		if (kill(cv->waitlist[i],SIGUSR1) < 0) {
			fprintf(stderr, "Error killing process %d: %s\n", cv->waitlist[i], strerror(errno));
        	_exit(255);
		}
		cv->waitlist[i] = 0;
	}
	int sleepers = cv->wait_cnt;
	cv->wait_cnt = 0;
	spin_unlock(&cv->sl);
	return sleepers;
}
int cv_signal(struct cv* cv) {
	if (cv->wait_cnt > 0) {
		spin_lock(&cv->sl);
		if (kill(cv->waitlist[cv->wait_cnt - 1],SIGUSR1) < 0) {
			fprintf(stderr, "Error killing process %d: %s\n", cv->waitlist[cv->wait_cnt - 1], strerror(errno));
        	_exit(255);
		}
		cv->waitlist[cv->wait_cnt - 1] = 0;
		cv->wait_cnt--;
		spin_unlock(&cv->sl);
		return 1;
	}
	return 0;
}