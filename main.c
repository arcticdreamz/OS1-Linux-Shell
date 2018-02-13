#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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

       	printf("Child : %d \n",pid);

        char* pathstring = getenv("PATH"); //get the $PATH environment variable
        //printf(" All Paths : %s \n",pathstring);
        char** paths = malloc(sizeof(char*)); 

        int i = 0;
        char* token = strtok(pathstring,":"); //Parse the string for a path delimited by ":"

        while(token != NULL){
            paths = realloc(paths,(i+1)*sizeof(char*)); //For each new found path, increase the array size
            paths[i] = token;
            
            //printf("Paths[%d]  : %s \n",i,paths[i]);

            token = strtok(NULL,":"); //Parse the array for the next path delimited by ":"
            i++;
        }

        //printf("arg[0] : %s \n",arg[0]);

        int j = 0;
        

        while(j < i){
            char  path[255] = "";
            strcat(path,paths[j]);
            strcat(path,"/");
            strcat(path,arg[0]);
            //printf("Path %d : %s \n",j,path);


            j++;
        
            //printf("access of path %d : %d \n",j,access(path,X_OK));

           if(access(path,X_OK) == 0){
                //printf("Executable pathPath: %s \n",path);
            	if(execvp(path,arg) == -1){
                    int errnum = errno;
                    perror("Instruction failed");
                    fprintf(stderr, "Value of errno: %d\n",errno);
                    fprintf(stderr, "Error: %s \n",strerror(errnum));
                    //continue;
                }
                break;
            }

        }



    	exit(1);
    }

    else{//This is the father
    	printf("Parent : %d \n",pid);
    	while(true){
		    wait(&status);
    		if(WIFEXITED(status))
    			break;
    	}
    }

    return 1;
}

int main(/*int argc, char* argv[]*/){

	char* args[3];
    printf(">");
    
    //char s[2];
    //scanf("%s",s);
	

/**************************************************************************************


TU DOIS T'OCCUPER DE CETTE PARTIE ANTOINE : LE PARSING
**************************************************************************************/


    args[0] = "ls";
	args[1] = "-l";
	args[2] = (char*) NULL;

	start_shell(args);

	return 0;

}
