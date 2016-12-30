#include<unistd.h>
#include<sys/types.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include"sem.h"
#include"../tas.h"
#include"../nproc.h"

void empty_handler(int sig){}
void exit_with_error();

void sem_init(struct sem* s, int count){
    s->count = count;
    s->lock = 0;
    int i = 0;
    while (i < N_PROC){
        s->wait_queue[i] = -1;
        ++i;
    }
    sigset_t block;

    if (sigemptyset(&block) == -1){
        exit_with_error();
    }

    if (sigaddset(&block, SIGUSR1) == -1){
        exit_with_error();
    }

    if (sigprocmask(SIG_BLOCK, &block, NULL) == -1){
        exit_with_error();
    }

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
        s->wait_queue[my_procnum] = sys_procnum;
        sigset_t normal;

        if (sigemptyset(&normal) == -1){
            exit_with_error();
        }

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
            if (kill(s->wait_queue[i], SIGUSR1) == -1)
                perror("Error sending signal"); //No need to exit...
        }
    }

}

void exit_with_error(){
    perror("Error in semaphores: ");
    exit(-1);
}
