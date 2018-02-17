#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int getPaths(char** args, char** paths) {

    char* pathstring = getenv("PATH"); //get the $PATH environment variable
    printf(" All Paths : %s \n",pathstring);

    int nb_paths = 0;

    char* path = strtok(pathstring,":"); //Parse the string for a path delimited by ":"

    while(path != NULL){
        paths = realloc(paths,(nb_paths+1)*sizeof(char*)); //For each new found path, increase the array size
        paths[nb_paths] = path;
        
        printf("Paths[%d]  : %s \n",nb_paths,paths[nb_paths]);

        path = strtok(NULL,":"); //Parse the array for the next path delimited by ":"
        nb_paths++;
    }


    return nb_paths;
}



int main(){

    char* args[2];


    bool stop = false;
    char command[255];
    strcpy(command,"");
    int returnvalue;
    char* token;
    char* tokens[256];
    int token_cnt = 0;
    char* path;
    char* paths[20];
    int path_cnt = 0;

    pid_t pid;
    int status;



    while(!stop){

        //Prompt
        printf("> ");
        fflush(stdout);

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
        token = strtok(command, " ");

        while(token != NULL){
            tokens[token_cnt] = token;
            token_cnt++;

            token = strtok(NULL, " ");
        }
        tokens[token_cnt] = NULL;


        // **3.1** : The command is cd
        if(strcmp(tokens[0], "cd")){

            // **3.1.1** : There is only "cd"
            if(tokens[1] == NULL)
                tokens[1] = getenv("HOME");

            // **3.1.2** : There is a path to cd
            else if(chdir(tokens[1]) != 0)
                fprintf((stderr), "bad path entered to cd \n");
        }


        // **3.2** : The command isn't a built-in command
        args[0] = "ls";
        printf("args[0] : %s \n",args[0]);
        args[1] = (char*) NULL;

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

            char** paths = malloc(sizeof(char*)); 

            int nb_paths = getPaths(args,paths);

            int j = 0;

            while(j < nb_paths){
                char path[256] = "";
                strcat(path,paths[j]);
                strcat(path,"/");
                strcat(path,args[0]);
                printf("Path %d : %s \n",j,path);


                j++;
            
                printf("access of path %d : %d \n",j,access(path,X_OK));

               if(access(path,X_OK) == 0){
                    printf("Executable path: %s \n",path);
                    if(execv(path,args) == -1){
                        int errnum = errno;
                        perror("Instruction failed");
                        fprintf(stderr, "Value of errno: %d\n",errno);
                        fprintf(stderr, "Error: %s \n",strerror(errnum));
                        //continue;
                    }
                    free(paths);
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


