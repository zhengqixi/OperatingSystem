#include"sem.h"
#include"../tas.h"
#include"../nproc.h"
#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>

int main(int argc, char**argv){
    sys_procnum = getpid();
    my_procnum = 4;
    struct sem test_sem;
    sem_init(&test_sem, 10);
    printf("Trying to increment: ");
    test_sem.lock = 1;
    sem_inc(&test_sem);
    printf("out");
}
