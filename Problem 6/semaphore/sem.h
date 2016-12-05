#ifndef SEM_H
#define SEM_H
#include<unistd.h>
#include"../nproc.h"
struct sem{
    int count;
    int max_count;
    volatile char lock;
    pid_t wait_queue[N_PROC];
};

void sem_init(stuct sem* s, int count);
int sem_try(struct sem* s);
void sem_wait(struct sem* s);
void sem_inc(struct sem* s);

