#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int start_shell(char** arg){
	// argv[0] = (char *) NULL ; Must be null terminated
	// 
	// 
	pid_t pid;
	int status;

 	pid = fork();
 
    if(pid < 0){
		int errnum = errno;
    	perror("Process creation failed");
        fprintf(stderr, "Value of errno: %d\n",errno);
        fprintf(stderr, "Error: %s \n",strerror(errnum));
        exit(1);
    }
    if(pid == 0){ //This is the son
    	fprintf("Child : %d",pid);
    	if(execvp(arg[0],arg == -1)){
    		int errnum = errno;
        	perror("Instruction failed");
            fprintf(stderr, "Value of errno: %d\n",errno);
            fprintf(stderr, "Error: %s \n",strerror(errnum));
        }

    	exit(1);
    }

    else{//This is the father
    	fprintf("Parent :%d",pid);
    	while(true){
		    wait(&status);
    		if(WIFEXITED(status))
    			break;
    	}
    }

    return 1;
}

int main(/*int argc, char* argv[]*/){

	char* args[2];
	args[0] = "ls";
	args[1] = (char*) NULL;

	start_shell(args);

	return 0;

}