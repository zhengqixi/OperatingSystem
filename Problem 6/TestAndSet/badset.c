#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<sys/wait.h>

void increment(unsigned long* to_increment);

int main(){
    unsigned long* test = (unsigned long*)mmap(NULL, sizeof(unsigned long), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    (*test) = 0;
    int child_process_count = 4096;
    while (child_process_count > 0){
        pid_t me = fork();
        if (me == 0){
            int race = 1000;
            while (race > 0){
                increment(test);
                --race;
            }
            return 0;
        }
        --child_process_count;
    }
    while(wait(NULL) != -1);
    printf("%lu\n", (*test));
    return 0;
}

void increment(unsigned long* to_increment){
    (*to_increment) += 1;
}
