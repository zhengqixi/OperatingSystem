#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<stdlib.h>
#include"../fifo/fifo.h"
#include"../nproc.h"

void writer(struct fifo* write);
void reader(struct fifo* read);

int main(char ** argv, int argc){
    struct fifo* test = mmap(NULL, sizeof(struct fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    fifo_init(test);
    printf("fifo intialized\n");
    pid_t process;
    switch( (process=fork())){
        case 0:
            writer(test);
        default:
            reader(test);
    }
}

void writer(struct fifo* write){
    sys_procnum = getpid();
    my_procnum = 0;
    printf("writer at process %lu \n", sys_procnum);
    unsigned long i;
    for (i = 1; i <= 8000; ++i){
        fifo_wr(write, i);
    }
    fifo_wr(write, 9000);
    exit(0);
}

void reader(struct fifo* read){
    sys_procnum = getpid();
    my_procnum = 1;
    printf("reader at process %lu \n", sys_procnum);
    unsigned long result = 0;
    unsigned long data;
    while((data = fifo_rd(read)) != 9000){ 
        result += data;
    }
    printf("total: %lu \n", result);
    exit(0);
}
