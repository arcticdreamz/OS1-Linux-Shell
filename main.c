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


    pid_t pid;
    int status;

    while(!stop){

        printf("> ");
        fflush(stdout);


        // getting the input from stdin

        // 1) Exit the shell (using Ctrl+D or exit())
        if(fgets(command,256,stdin) == NULL || !strcmp(command,"exit\n")){
            stop = true;
            break;
        }
        // Rerun the loop if we press 'Enter'
        if(!strcmp(command,"\n"))
            continue;
            
        printf("command : %s\n",command);

/**************************************************************************************


TU DOIS T'OCCUPER DE CETTE PARTIE ANTOINE : LE PARSING
        arg[0] = command; ne marche pas, je ne sais pas pq
        Genre le path est le même, mais il ne veut pas etre exécuté

**************************************************************************************/
        arg[0] = "ls";
        printf("arg[0] : %s \n",arg[0]);
        arg[1] = (char*) NULL;

        //2) The user enters a command
            // 2).1 : the command is cd

        // 3)the command isn't a built-in command


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
        	
        }


    }

    return 0;

}
