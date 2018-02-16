#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int main(){

    char* arg[2];


    bool stop = false;
    char command[255];
    strcpy(command,"");
    int returnvalue;
    char* token;
    char* tokens[256];
    int i = 0;

    pid_t pid;
    int status;

    while(!stop){

        //Prompt
        printf("> ");
        fflush(stdout);


        /*************************************User interaction***************************/

        // **1** : User wants to quit (using Ctrl+D or exit())
        if(fgets(command,256,stdin) == NULL || !strcmp(command,"exit\n")){
            stop = true;
            break;
        }


        // **2** : User presses "Enter"
        if(!strcmp(command,"\n"))
            continue;


        // **3** : User enters a command line
        token = strtok(command, "\n");
        token = strok(command, " ");

        while(token != NULL){
            tokens[i] = token;
            i++;

            token = strok(NULL, " ");
        }
        tokens[i] = NULL;


        // **3.1** : The command is cd


        // **3.2** : The command isn't a built-in command
        arg[0] = "ls";
        printf("arg[0] : %s \n",arg[0]);
        arg[1] = (char*) NULL;

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

            printf("arg[0] : %s \n",arg[0]);

            int j = 0;
            

            while(j < i){
                char path[255] = "";
                strcat(path,paths[j]);
                strcat(path,"/");
                strcat(path,arg[0]);
                //printf("Path %d : %s \n",j,path);


                j++;
            
                //printf("access of path %d : %d \n",j,access(path,X_OK));

               if(access(path,X_OK) == 0){
                    printf("Executable path: %s \n",path);
                	if(execv(path,arg) == -1){
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
    	    wait(&status);
            returnvalue = WEXITSTATUS(status);
            printf("%d",returnvalue);

            /*//Wait for the son to end its process before continuing with the father
            wait (&status) ;
            if (WIFEXITED (status))
                printf ("Son ended normally: status = %d\n", WEXITSTATUS (status));
            else
                printf ("Son ended anormally\n") ;*/
        	
        }


    }

    return 0;

}
