#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>

#define ARG_MAX 4096


void errorReport(char * error_message){
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(-1);
}

void runCommand(char ** command){
    pid_t process = fork(); 
    
}

char ** getCommand(FILE * stream){
    char command_line[ARG_MAX];
    int i = 0;
    char current;
    while ( (current = fgetc(stream)) != '\n'){
        command_line[i] = current;
        ++i;
    }
    command_line[i] = '\0';

    int arguments = 1;
    char ** command = (char **)malloc(sizeof(char *) * arguments+1);
    char * argument = strtok(command_line, " ");
    command[0] = (char *)malloc(sizeof(char) * ARG_MAX);
    strcpy(command[0], argument);

    while ( (argument = strtok(NULL, " ")) != NULL){
        ++arguments;
        command = (char **)realloc(command, sizeof(char *) * arguments + 1);
        command[arguments-1] = (char *)malloc(sizeof(char) * ARG_MAX);
        strcpy(command[arguments-1], argument);
    }

    command[arguments] = NULL;
    return command;

}

void freeCommand(char ** command){
    char ** state = command;
    while ((*command) != NULL){
        printf("%s\n", (*command));
        free((*command));
        ++command;
    }
    free(state);
}

int main(int argc, char* argv[]) {
    if (argc  > 1){
    }
    while(1){
        char ** command = getCommand(stdin);
        runCommand(command);
        freeCommand(command);
    }
}
