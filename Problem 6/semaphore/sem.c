#include"sem.h"
#include"../tas.h"
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>

pid_t sys_procnum;
pid_t my_procnum;
sigset_t mask;

void sem_init(struct sem* s, int count){
    s->count = count;
    s->max_count = count;
    s->lock = 0;
    int i = 0;
    while (i < N_PROC){
        s->wait_queue[i] = -1;
        ++i;
    }
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    sigprocmask(SIG_BLOCK, &block, &mask);
}

int sem_try(struct sem* s){
    if (tas(&s->lock) != 0)
        return 0;
    if (s->count == 0){
        s->lock = 0;
        return 0;
    }
    s->count -= 1;
    return 1;
}

void sem_wait(struct sem* s){
    while (sem_try(s) == 0){
        s->wait_queue[my_procnum] = sys_procnum;
        sigsuspend(&mask);
    }
    s->wait_queue[my_procnum] = -1;
    s->lock = 0;
}

void sem_inc(struct sem* s){
    while(tas(&s->lock) != 0){
        lock:
        s->wait_queue[my_procnum] = sys_procnum;
        sigsuspend(&mask);
    }
    if (s->count == s->max_count){
        s->lock = 0;
        goto lock;
    }
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
