#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include"../tas.h"

struct test {
    unsigned long data;
    volatile char lock;
};

void increment(struct test* to_increment);

int main(){
    struct test* subject = (struct test*)mmap(NULL, sizeof(struct test), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    subject->data = 0;
    subject->lock = 0;
    int child_process_count = 4096;
    while (child_process_count > 0){
        pid_t me = fork();
        if (me == 0){
            int race = 1000;
            while (race > 0){
                increment(subject);
                --race;
            }
            return 0;
        }
        --child_process_count;
    }
    while(wait(NULL) != -1);
    printf("%lu\n", subject->data);
    return 0;
}

void increment(struct test* to_increment){
    while(tas(&to_increment->lock) != 0);
    to_increment->data += 1;
    to_increment->lock = 0;
}
