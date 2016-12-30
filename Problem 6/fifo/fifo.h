#ifndef FIFO
#define FIFO
#include"../semaphore/sem.h"
#define FIFO_SIZE 4196
struct fifo {
    struct sem wr_lock;
    struct sem rd_lock;
    struct sem resource_lock;
    unsigned long data[FIFO_SIZE];
    unsigned long rd;
    unsigned long wr;
    unsigned int max_count;
    unsigned int current_count;
};
void fifo_init(struct fifo* f);
void fifo_wr(struct fifo* f, unsigned long d);
unsigned long fifo_rd(struct fifo* f);
#endif
