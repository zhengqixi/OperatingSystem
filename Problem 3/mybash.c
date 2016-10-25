#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/resource.h>

#define ARG_MAX 4096


void runCommand(char ** command);
void redirect(char ** command);
void errorReport(char * error_message);

time_t user_whole_second;
long int user_micro_second;
time_t system_whole_second;
long int system_micro_second;

int main(int argc, char* argv[]) {
    user_whole_second = user_micro_second = system_whole_second = system_micro_second = 0;
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
            if (current == EOF){
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
        
        fprintf(stderr, "Actual time: %ld seconds\n", end-start);



    }
}

void errorReport(char * error_message){
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(-1);
}

void redirect(char ** command){
    while ((*command) != NULL){
        char * value;
        if ( (value = strpbrk((*command), "<>")) == NULL){
            ++command;
            continue;
        }
        if ( (*value) == '<'){
            int pre_length;
            if  ((pre_length = strcspn((*command), "<")) > 0){
                errorReport("Invalid redirection syntax");
            }
            value++;
            int fd = open(value, O_RDONLY);    
            if (fd < 0){
                errorReport("Unable to redirect stdin");
            }
            int success = dup2(fd, STDIN_FILENO);
            if (success < 0){
                errorReport("Unable to redirect stdin");
            }
        } else {
            int redirect_stream = STDOUT_FILENO;

            int pre_length;
            if ( (pre_length = strcspn((*command), ">")) > 1){ 
                errorReport("Invalid redirection syntax");
            } else if (pre_length == 1){
                if ((*command)[0] == '2'){
                    redirect_stream = STDERR_FILENO;
                } else {
                    errorReport("Invalid redirection syntax");
                }
            }
            
            unsigned int flags = O_CREAT | O_WRONLY; 
            ++value;
            if ((*value) == '>'){
                ++value;
                flags = flags | O_APPEND;
            } else {
                flags = flags | O_TRUNC; 
            }

            int fd = open(value, flags, S_IRWXU);
            if (fd < 0){
                errorReport("Unable to redirect");
            }
            int success = dup2(fd, redirect_stream);
            if (success < 0){
                errorReport("Unable to redirect");
            }
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
        struct rusage usage;
        int success = getrusage(RUSAGE_CHILDREN, &usage);
        if (success < 0){
            fprintf(stderr, "Cannot get usage...: %s\n", strerror(errno));
        }
        if (usage.ru_utime.tv_sec == user_whole_second){
            fprintf(stderr, "User time: %ld microseconds\n", usage.ru_utime.tv_usec-user_micro_second);
            user_micro_second = usage.ru_utime.tv_usec;
        } else {
            fprintf(stderr, "User time: %ld seconds\n", usage.ru_utime.tv_sec-user_whole_second);
            user_micro_second = usage.ru_utime.tv_usec;
            user_whole_second = usage.ru_utime.tv_sec;
        }

        if (usage.ru_stime.tv_sec == user_whole_second){
            fprintf(stderr, "System time: %ld microseconds\n", usage.ru_stime.tv_usec-system_micro_second);
            system_micro_second = usage.ru_stime.tv_usec;
        } else {
            fprintf(stderr, "System time: %ld seconds\n", usage.ru_stime.tv_sec-system_whole_second);
            system_micro_second = usage.ru_stime.tv_usec;
            system_whole_second = usage.ru_stime.tv_sec;
        }

    }
    
}
