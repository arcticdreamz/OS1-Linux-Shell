#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


/***********************************FUNCTIONS***************************************/
/*
char* read_command(){

    char* command;
    fgets(command,256,stdin);

    return command;
}
*/


char** split_command(char* command){
    int token_cnt = 0;
    char* token;
    char** tokens = malloc(sizeof(char*));

    if(!tokens){
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
    tokens[token_cnt] = (char *) NULL;

    return tokens;
}



int getPaths(char** tokens, char** paths) {

    char* pathstring = getenv("PATH"); //get the $PATH environment variable
    //printf(" All Paths : %s \n",pathstring);

    int nb_paths = 0;

    char* path = strtok(pathstring,":"); //Parse the string for a path delimited by ":"

    while(path != NULL){
        paths = realloc(paths,(nb_paths+1)*sizeof(char*)); //For each new found path, increase the array size
        paths[nb_paths] = path;
        
        //printf("Paths[%d]  : %s \n",nb_paths,paths[nb_paths]);

        path = strtok(NULL,":"); //Parse the array for the next path delimited by ":"
        nb_paths++;
    }

    return nb_paths;
}





/******************************************MAIN**********************************/
int main(int argc, char** argv){

    bool stop = false;
    
    int returnvalue;
    
    pid_t pid;
    int status;
    

    while(!stop){

        //Prompt
        printf("> ");
        fflush(stdout);

        //Read the command line
        //char* command = read_command();

        char* command;
        fgets(command,256,stdin);

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
        if(!strcmp(args[0], "cd")){

            // **3.1.1** : There is only "cd"
            if(args[1] == NULL)
                args[1] = getenv("HOME");

            // **3.1.2** : There is a path to cd
            else if(chdir(args[1]) != 0)
                fprintf((stderr), "bad path entered to cd \n");
        }

        //printf("%d",chdir(args[1]));


        // **3.2** : The command isn't a built-in command


        pid = fork();

        if(pid < 0){
            int errnum = errno;

            perror("Process creation failed");
            fprintf(stderr, "Value of errno: %d\n",errno);
            fprintf(stderr, "Error: %s \n",strerror(errnum));
            exit(1);
        }
        if(pid == 0){ //This is the son

            char** paths = malloc(sizeof(char*)); 

            int nb_paths = getPaths(args,paths);

            int j = 0;

            while(j < nb_paths){
                char path[256] = "";
                strcat(path,paths[j]);
                strcat(path,"/");
                strcat(path,args[0]);
                //printf("Path %d : %s \n",j,path);


                j++;
            
                //printf("access of path %d : %d \n",j,access(path,X_OK));

                if(access(path,X_OK) == 0){
                    //printf("Executable path: %s \n",path);
                    if(execv(path,args) == -1){
                        int errnum = errno;
                        perror("Instruction failed");
                        fprintf(stderr, "Value of errno: %d\n",errno);
                        fprintf(stderr, "Error: %s \n",strerror(errnum));
                    }
                    free(paths);
                    break;
                }

            }

            exit(1);
        }

        else{//This is the father
            wait(&status);
            returnvalue = WEXITSTATUS(status);
            printf("%d",returnvalue);
            
        }


    }

    return 0;

}


