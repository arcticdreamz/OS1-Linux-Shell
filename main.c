#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int start_shell(char** arg){
	// argv[0] = (char *) NULL ; Must be null terminated
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
    	
    	if(execvp(arg[0],arg)){
    		int errnum = errno;
        	perror("Instruction failed");
            fprintf(stderr, "Value of errno: %d\n",errno);
            fprintf(stderr, "Error: %s \n",strerror(errnum));
        }

    	exit(1);
    }

    else{//This is the father

    	while(true){
		    wait(&status);
    		if(WIFEXITED(status))
    			break;
    	}
    }

    return 1;
}

int main(int argc, char* argv[]){







}