#ifndef ____cv__
#define ____cv__

#include "spinlock.h"

#define CV_MAXPROC 64

struct cv {
	struct spinlock sl;
	pid_t waitlist[CV_MAXPROC];
	int wait_cnt;
};
void handler(int sig);
void cv_init(struct cv* cv);
void cv_wait(struct cv* cv, struct spinlock* mutex);
int cv_broadcast(struct cv* cv);
int cv_signal(struct cv* cv);

#endif