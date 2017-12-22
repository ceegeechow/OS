#include "spinlock.h"

void sp_init(struct spinlock *l) {
    l->lock = 0;
    l->pid = 0;
}
void spin_lock(struct spinlock *l) {
    while (tas(&(l->lock))==1){}
    l->lock = 1;
    l->pid = getpid();
}
void spin_unlock(struct spinlock *l) {
    l->lock = 0;
}