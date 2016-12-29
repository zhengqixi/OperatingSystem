#ifndef SEM_H
#define SEM_H
#define N_PROC 64
#include<unistd.h>
#include<signal.h>

pid_t sys_procnum;
pid_t my_procnum;


struct sem{
    int count;
    int max_count;
    volatile char lock;
    pid_t wait_queue[N_PROC];
};

void sem_init(struct sem* s, int count);
int sem_try(struct sem* s);
void sem_wait(struct sem* s);
void sem_inc(struct sem* s);

#endif
