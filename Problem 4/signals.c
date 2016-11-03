#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>

#define process_count 10
#define signal_per_process 10


int num_received;
pid_t master;

void signal_handler(int signal);

void send_signal(int sig_num);

int main(){
    num_received = 0;
    master = getpid();

    signal(SIGURG, signal_handler);
    send_signal(SIGURG);
    printf("Linux defined: Sent signals: %d\n", process_count * signal_per_process);
    printf("Received signals: %d\n", num_received); 

    num_received = 0;
    signal(50, signal_handler);
    send_signal(50);
    printf("Real time: Sent signals: %d\n", process_count * signal_per_process);
    printf("Received signals: %d\n", num_received); 
    return 0;
}

void signal_handler(int signal){
    ++num_received;
}

void send_signal(int sig_num){
    int forked = 0;
    while (getpid() == master && forked < process_count){  
        fork();    
        ++forked;
    } 
    if (getpid() == master){
        errno = 0;
        while(errno != ECHILD){
            wait(NULL);
        }
        return;
    } else {
        int sig_count = 0;
        while (sig_count < signal_per_process){
            union sigval bs; 
            sigqueue(master, sig_num, bs);
            ++sig_count;
        }
        exit(0);
    }
}
