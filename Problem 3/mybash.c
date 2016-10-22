#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>

#define ARG_MAX 4096


void errorReport(char * error_message){
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(-1);
}

void redirect(char ** command){
    while ((*command) != NULL){
        char * value;
        if ( (value = strpbrk((*command), "<>")) == NULL)
            continue;
        if ( (*value) == '<'){
            value++;
            int fd = open(value, O_RDONLY);    
            if (fd < 0){
                errorReport("Unable to redirect stdin");
            }
            int success = dup2(); //TODO, FIGURE OUT DUP2
            if (success < 0){
                errorReport("Unable to redirect stdin");
            }
        } else {
        }
        (*command) = NULL;
        ++command;
    }
    
}

void runCommand(char ** command){
    pid_t process = fork(); 
    if (process == 0){
        redirect(command);
        int status = execvp(command[0], command); 
        if (status < 0)
            errorReport("Error executing command");
    } else {
        int return_status;
        waitpid(process, &return_status, 0);
        fprintf(stderr, "Exit status of process: %d\n", WEXITSTATUS(return_status));
    }
    
}


int main(int argc, char* argv[]) {
    char * command[ARG_MAX];
    FILE * stream = stdin;
    if (argc  > 1){
        stream = fopen(argv[1], "r");
        if (stream == NULL){
            errorReport("Cannot open input file");
        }
    }
    while(1){
        char command_line[ARG_MAX];
        int i = 0;
        char current;
        while ( (current = fgetc(stream)) != '\n'){
            if (feof(stream)){
                return 0;
            }
            command_line[i] = current;
            ++i;
        }
        command_line[i] = '\0';
        if (command_line[0] == '#'){
            continue;
        }
        int arguments = 1;
        char * argument = strtok(command_line, " ");
        command[0] = argument;

        while ( (argument = strtok(NULL, " ")) != NULL){
            ++arguments;
            command[arguments-1] = argument; 
        }

        command[arguments] = NULL;

        time_t start = time(NULL);

        runCommand(command);

        time_t end = time(NULL);



    }
}
