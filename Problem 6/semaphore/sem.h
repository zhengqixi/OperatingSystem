#ifndef SEM_H
#define SEM_H
#include<unistd.h>
#include"../nproc.h"

struct sem{
    unsigned int count;
    volatile char lock;
    pid_t wait_queue[N_PROC];
};

void sem_init(struct sem* s, int count);
int sem_try(struct sem* s);
void sem_wait(struct sem* s);
void sem_inc(struct sem* s);
#endif
