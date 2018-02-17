#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/***********************************FUNCTIONS***************************************/
char* read_command(){
    char* command = NULL;

    fgets(command,256,stdin);

    return command;
}



char** split_command(char* command){
    int token_cnt = 0;
    char* token;
    char** tokens = malloc(sizeof(char*));

    if(!token){
        fprintf((stderr), "Allocation of tokens array failed.\n");
        exit(1);
    }
    
    token = strtok(command, "\n");
    token = strtok(command, " ");

    while(token != NULL){
        tokens = realloc(tokens, (token_cnt+1)*sizeof(char*)); //Realloc space for each new token

        tokens[token_cnt] = token;
        token_cnt++;

        token = strtok(NULL, " ");
    }
    tokens[token_cnt] = NULL;

    return tokens;

}




/******************************************MAIN**********************************/
int main(int argc, char** argv){

    char* arg[2];

    bool stop = false;
    
    int returnvalue;
    
    pid_t pid;
    int status;
    

    while(!stop){

        //Prompt
        printf("> ");
        fflush(stdout);

        //Read the command line
        char* command = read_command();

        //User wants to quit (using Ctrl+D or exit())
        if(command == NULL || !strcmp(command,"exit\n")){
            stop = true;
            break;
        }

        //User presses "Enter"
        if(!strcmp(command,"\n"))
            continue;


        //User enters a command line
        char** args = split_command(command);


        //The command is cd
        if(strcmp(tokens[0], "cd")){

            // **3.1.1** : There is only "cd"
            if(tokens[1] == NULL)
                tokens[1] = getenv("HOME");

            // **3.1.2** : There is a path to cd
            else if(chdir(tokens[1]) != 0)
                fprintf((stderr), "bad path entered to cd \n");
        }


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
