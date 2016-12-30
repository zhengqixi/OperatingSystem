#include"sem.h"
#include"../tas.h"
#include"../nproc.h"
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>
#include<stdio.h>


void empty_handler(int sig){
    printf("FUCK GOD\n");
}

void sem_init(struct sem* s, int count){
    s->count = count;
    s->lock = 0;
    int i = 0;
    while (i < N_PROC){
        s->wait_queue[i] = -1;
        ++i;
    }
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigprocmask(SIG_BLOCK, &block, NULL);
    signal(SIGUSR1, empty_handler);
}

int sem_try(struct sem* s){
    if (tas(&s->lock) != 0)
        return 0;
    if (s->count == 0){
        s->lock = 0;
        return 0;
    }
    s->count -= 1;
    s->lock = 0;
    return 1;
}

void sem_wait(struct sem* s){
    lock:
    while (tas(&s->lock) != 0){
        printf("Process %lu is suspending \n", sys_procnum);
        s->wait_queue[my_procnum] = sys_procnum;
        sigset_t normal;
        sigemptyset(&normal);
        sigsuspend(&normal);
    }
    if (s->count == 0){
        s->lock = 0;
        goto lock;
    }
    s->count -= 1;
    s->wait_queue[my_procnum] = -1;
    s->lock = 0;
}

void sem_inc(struct sem* s){
    while(tas(&s->lock) != 0);
    s->count += 1;
    s->wait_queue[my_procnum] = -1;
    s->lock = 0;
    int i;
    for (i = 0; i < N_PROC; ++i){
        if (s->wait_queue[i] != -1){
            kill(s->wait_queue[i], SIGUSR1);
        }
    }

}
