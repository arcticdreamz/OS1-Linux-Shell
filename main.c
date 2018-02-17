#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


/*************************************Prototypes*********************************************
*
********************************************************************************************/
char** split_command(char* command, char** args);
int getPaths(char** paths);
char* cd_cmd_whitespace(char** args, char c);



/*************************************split_command*****************************************
*
* Split the command line entered by the user
*
* ARGUMENT :
*   - command : a string entered by the user as command line
*
* RETURN : an array of string reprensenting each token entered by the user
*
*******************************************************************************************/
char** split_command(char* command, char** args){

    int args_cnt = 0;
    char* token;

    if(!args){
        fprintf((stderr), "Allocation of args array failed.\n");
        exit(1);
    }
    
    token = strtok(command, "\n");
    token = strtok(command, " ");

    while(token != NULL){
        args = realloc(args, (args_cnt+1)*sizeof(char*)); //Realloc space for each new token

        args[args_cnt] = token;
        args_cnt++;

        token = strtok(NULL, " ");
    }
    args[args_cnt] = (char *) NULL;

    return args;
}


/*************************************get_paths****************************************
*
* Split the full path into all the paths and get the number of total paths
*
* ARGUMENT :
*   - paths : an array to contain all the paths
*
* RETURN : the number of paths
*
*******************************************************************************************/
int get_paths(char** paths) {

    char* pathstring = getenv("PATH"); //get the $PATH environment variable

    int nb_paths = 0;

    char* path = strtok(pathstring,":"); //Parse the string for a path delimited by ":"

    while(path != NULL){

        paths = realloc(paths,(nb_paths+1)*sizeof(char*)); //For each new found path, increase the array size
        paths[nb_paths] = path;
        

        path = strtok(NULL,":"); //Parse the array for the next path delimited by ":"
        nb_paths++;
    }

    return nb_paths;
}


/*************************************cd_cmd_whitespace************************************
*
* Deal with the changing directory of a folder with whitespaces.
*
* ARGUMENT :
*   - args : an array containing all the args of the command line entered by the user
*
* RETURN : the path of the directory to go
*
*******************************************************************************************/
char* cd_cmd_whitespace(char** args, char c){

    int j = 1;

    char* tmp_dir;
    strcpy(tmp_dir, getcwd(NULL,0));
    
    strcat(tmp_dir, "/");

    while(args[j] != NULL){

        if (args[j][strlen(args[j])-1] == c)
            args[j][strlen(args[j])-1] = 0;
        
        if (j!=1)
            strcat(tmp_dir, " ");
        strcat(tmp_dir,args[j++]);
    }

    return tmp_dir;

}


/******************************************main**********************************************/
int main(int argc, char** argv){

    bool stop = false;
    
    int returnvalue;

    char command[255];
    char** args;
    
    pid_t pid;
    int status;
    

    while(!stop){

        //Prompt
        printf("> ");
        fflush(stdout);

        //User wants to quit (using Ctrl+D or exit())
        if(fgets(command,sizeof(command),stdin) == NULL || !strcmp(command,"exit\n")){
            printf("\n");
            stop = true;
            break;
        }

        //User presses "Enter"
        if(!strcmp(command,"\n"))
            continue;

        //User enters a command line
        char** args = malloc(sizeof(char*));
        split_command(command, args);


        //The command is cd
        if(!strcmp(args[0], "cd")){

            // Case 1 : cd
            if(args[1] == NULL || !strcmp(args[1],"~"))
                args[1] = getenv("HOME");

            //Case 2 : cd ..
            else if(!strcmp(args[1],"..")){

                char* new_dir = strrchr(args[1],'/');

                if(new_dir != NULL)
                    *new_dir = '\0';
            }

            //Case 3 : cd "My directory" ; cd 'My Directory'
            else if (args[1][0] == '"' || args[1][0] == '\''){

                char c = args[1][0];
                args[1] = strtok(args[1], (char*) c);
                args[1] = cd_cmd_whitespace(args, c);

           }

            //Case 4 : cd My\ Directory
            else if (args[1][strlen(args[1])-1] == '\\'){

                char c = args[1][strlen(args[1])-1];
                args[1] = cd_cmd_whitespace(args, c);

            }


            printf("%d",chdir(args[1]));
            continue;
        }

        


        //The command isn't a built-in command
        pid = fork();

        //Error
        if(pid < 0){
            int errnum = errno;

            perror("Process creation failed");
            fprintf(stderr, "Value of errno: %d\n",errno);
            fprintf(stderr, "Error: %s \n",strerror(errnum));
            exit(1);
        }

        //This is the son
        if(pid == 0){ 

            char** paths = malloc(sizeof(char*)); 

            int nb_paths = get_paths(paths);

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

        //This is the father
        else{
            wait(&status);
            returnvalue = WEXITSTATUS(status);
            printf("%d",returnvalue);
            
        }
    }

    free(args);

    return 0;

}


