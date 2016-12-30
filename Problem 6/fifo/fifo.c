#include"../semaphore/sem.h"
#include"fifo.h"
#include<stdio.h>

void fifo_init(struct fifo* f){
    sem_init(&f->wr_lock, 1);
    sem_init(&f->rd_lock, 1);
    sem_init(&f->resource_lock, 1);
    f->rd = 0;
    f->wr = 0;
    f->max_count = FIFO_SIZE;
    f->current_count = 0;
}

void fifo_wr(struct fifo* f, unsigned long d){
    //Attempt to gain locks, step out in order if unable to gain lock
    lock:
    while(sem_try(&f->wr_lock) == 0); //Spin for the write lock
    if (sem_try(&f->resource_lock) == 0){//Step out if unable to obtain resource lock
        sem_inc(&f->wr_lock);
        goto lock;
    }
    if (f->current_count == f->max_count){//Step out if full
        sem_inc(&f->resource_lock);
        sem_inc(&f->wr_lock);
        goto lock;
    }
    f->data[f->wr] = d;
    f->wr = (f->wr+1)%FIFO_SIZE;

    ++f->current_count;
    sem_inc(&f->resource_lock);
    sem_inc(&f->wr_lock);
}

unsigned long fifo_rd (struct fifo* f){
    //Same thing as fifo_wr for the most part
    lock:
    while(sem_try(&f->rd_lock) == 0);
    if (sem_try(&f->resource_lock) == 0){
        sem_inc(&f->rd_lock);
        goto lock;
    }
    if (f->current_count == 0){//Step out of there is nothing left
        sem_inc(&f->resource_lock);
        sem_inc(&f->rd_lock);
        goto lock;
    }
    
    unsigned long d = f->data[f->rd];
    f->rd = (f->rd+1)%FIFO_SIZE;
    --f->current_count;
    sem_inc(&f->resource_lock);
    sem_inc(&f->rd_lock);
    return d;
}
