#ifndef ____fifo__
#define ____fifo__

#include "cv.h"

#define MYFIFO_BUFSIZE 10

struct fifo {
	unsigned long data[MYFIFO_BUFSIZE];
	int wr_ind;
	int rd_ind;
	int item_cnt;
	struct cv full, empty;
	struct spinlock mutex;
};
void fifo_init(struct fifo *f);
void fifo_wr(struct fifo *f,unsigned long d);
unsigned long fifo_rd(struct fifo *f);

#endif