#ifndef ____spinlock__
#define ____spinlock__

struct spinlock {
    char lock;
    pid_t pid;
};
void sp_init(struct spinlock *l);
int tas(volatile char* lock);
void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);

#endif