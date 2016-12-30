#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<stdlib.h>
#include"../fifo/fifo.h"
#include"../nproc.h"
#define TO_WRITE 100

void writer(struct fifo* write);
void reader(struct fifo* read);

int main(char ** argv, int argc){
    struct fifo* test = mmap(NULL, sizeof(struct fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    fifo_init(test);
    printf("fifo intialized\n");
    pid_t process;
    int num_process = N_PROC;
    my_procnum = 1;
    while (my_procnum < N_PROC){
        switch( (process=fork())){
            case 0:
                writer(test);
            default:
                break;
        }
        ++my_procnum;
    }
    reader(test);
}

void writer(struct fifo* write){
    sys_procnum = getpid();
    printf("writer at process %lu \n", sys_procnum);
    unsigned long i;
    for (i = 0; i < TO_WRITE; ++i){
        fifo_wr(write, my_procnum);
    }
    exit(0);
}

void reader(struct fifo* read){
    sys_procnum = getpid();
    my_procnum = 0;
    int total_read = 0;
    int target = (N_PROC-1) * TO_WRITE;
    int value[N_PROC];
    while (total_read < target){
        unsigned long data = fifo_rd(read);
        ++value[data];
        ++total_read;
    }
    int i;
    for(i = 0; i < N_PROC; ++i)
        printf("Value of data read from process %d: %d\n", i, value[i]); 

    exit(0);
}
