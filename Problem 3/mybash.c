#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>

#define ARG_MAX 4096


void errorReport(char * error_message){
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(-1);
}

void runCommand(char ** command){
    pid_t process = fork(); 
    if (process == 0){
        int status = execvp(command[0], command); 
        if (status < 0){
            errorReport("Error executing command\n");
        }
    } else {
        int return_status;
        waitpid(process, &return_status, 0);
    }
    
}


int main(int argc, char* argv[]) {
    char * command[ARG_MAX];
    FILE * stream = stdin;
    if (argc  > 1){
        stream = fopen(argv[1], "r");
    }
    while(1){
        char command_line[ARG_MAX];
        int i = 0;
        char current;
        while ( (current = fgetc(stream)) != '\n'){
            command_line[i] = current;
            ++i;
        }
        command_line[i] = '\0';
        int arguments = 1;
        char * argument = strtok(command_line, " ");
        command[0] = argument;

        while ( (argument = strtok(NULL, " ")) != NULL){
            ++arguments;
            command[arguments-1] = argument; 
        }

        command[arguments] = NULL;

        runCommand(command);
    }
}
