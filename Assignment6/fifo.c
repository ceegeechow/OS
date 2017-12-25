#include "fifo.h"
#include <stdio.h>

void fifo_init(struct fifo *f) {
	f->wr_ind = 0;
	f->rd_ind = 0;
	f->item_cnt = 0;
	cv_init(&f->full);
	cv_init(&f->empty);
	sp_init(&f->mutex);
}
void fifo_wr(struct fifo *f,unsigned long d) {
	spin_lock(&f->mutex);
	while(f->item_cnt >= MYFIFO_BUFSIZE) {
		fprintf(stderr, "\nFIFO full\n");
		cv_wait(&f->full,&f->mutex);
		fprintf(stderr, "\nFIFO no longer full\n");
	}
	f->data[f->wr_ind++] = d;
	f->wr_ind %= MYFIFO_BUFSIZE;
	f->item_cnt++;
	cv_signal(&f->empty);
	spin_unlock(&f->mutex);
}
unsigned long fifo_rd(struct fifo *f) {
	unsigned long d;
	spin_lock(&f->mutex);
	while(f->item_cnt <= 0) {
		fprintf(stderr, "\nFIFO empty\n");
		cv_wait(&f->empty,&f->mutex);
		fprintf(stderr, "\nFIFO no longer empty\n");
	}
	d = f->data[f->rd_ind++];
	f->rd_ind %= MYFIFO_BUFSIZE;
	f->item_cnt--;
	cv_signal(&f->full);
	spin_unlock(&f->mutex);
	return d;
}