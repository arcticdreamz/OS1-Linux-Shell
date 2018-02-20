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
int split_line(char* line, char** args);
int getPaths(char** paths);
char* cd_cmd_whitespace(char** args, char c);



/*************************************split_line*****************************************
*
* Split the line line entered by the user
*
* ARGUMENT :
*   - line : a string entered by the user as a line line
*
* RETURN : an array of strings reprensenting each token entered by the user
*
*******************************************************************************************/
int split_line(char* line, char** args){

    int args_cnt = 0;
    char* token;

    if(!args){
        fprintf((stderr), "Allocation of args array failed.\n");
        exit(1);
    }
    
    token = strtok(line, "\n");
    token = strtok(line, " ");

    while(token != NULL){

        token = malloc(256*sizeof(char));
        if(token == NULL)
            return -1;

        args[args_cnt] = token;
        args_cnt++;

        token = strtok(NULL, " ");
    }

    args[args_cnt] = (char*) NULL;

    return 0;
}


/*************************************get_paths****************************************
*
* Split the full path into all possible paths and get the number of total paths
*
* ARGUMENT :
*   - paths : an array that will contain all the paths
*
* RETURN : the number of paths
*
*******************************************************************************************/
int get_paths(char** paths) {

    char* pathstring = getenv("PATH"); //get the $PATH environment variable

    int nb_paths = 0;

    char* path = strtok(pathstring,":"); //Parse the string for a path delimited by ":"

    while(path != NULL){

        path = malloc(sizeof(char*)); 
        
        if(path == NULL){
            free(paths);
            exit(1);
        }

        paths[nb_paths] = path;
        nb_paths++;    

        path = strtok(NULL,":"); //Parse the array for the next path delimited by ":"
    }

    return nb_paths;
}


/*************************************cd_cmd_whitespace************************************
*
* Deal with the changing of the directory of a folder with whitespaces.
*
* ARGUMENT :
*   - args : an array containing all the args of the line line entered by the user
*
* RETURN : the path of the directory to go
*
*******************************************************************************************/
char* cd_cmd_whitespace(char** args, char c){

    int j = 1;

    char* temp_dir;
    strcpy(temp_dir,"");

    while(args[j] != NULL){

        if (args[j][strlen(args[j])-1] == c)
            args[j][strlen(args[j])-1] = 0;
        
        if (j!=1)
            strcat(temp_dir, " ");
        strcat(temp_dir,args[j]);
        j++;
    }

    return temp_dir;

}


/******************************************main**********************************************/
int main(int argc, char** argv){

    bool stop = false;
    int returnvalue;

    char line[65536]; 
    char* args[256];
    
    pid_t pid;
    int status;
    

    while(!stop){

        //Clear the variables
        strcpy(line,"");
        memset(args, 0, sizeof(args));

        //Prompt
        printf("> ");
        fflush(stdout);

        //User wants to quit (using Ctrl+D or exit())
        if(fgets(line,sizeof(line),stdin) == NULL || !strcmp(line,"exit\n")){
            printf("\n");
            stop = true;
            break;
        }

        //User presses "Enter"
        if(!strcmp(line,"\n"))
            continue;

        //User enters a line 
        if(split_line(line, args) != 0){
            free(args);
            printf("Allocation in split went wrong\n");
            exit(1);
        }

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

            //Case 3 : cd "My directory" or cd 'My Directory'
            else if (args[1][0] == '"' || args[1][0] == '\''){

                char c = args[1][0]; // " or '

                //Removing first " or '
                memmove(args[1], args[1]+1, strlen(args[1]));

                //Removing last " or '
                args[1] = cd_cmd_whitespace(args, c);

           }

            //Case 4 : cd My\ Directory
            else if (args[1][strlen(args[1])-1] == '\\'){

                char c = args[1][strlen(args[1])-1];
                args[1] = cd_cmd_whitespace(args, c);

            }


            printf("%d",chdir(args[1]));
            /*int errnum = errno;
            fprintf(stderr, "Value of errno: %d\n",errno);
            fprintf(stderr, "Error: %s \n",strerror(errnum));
            */
            continue;
        }        


        //The line isn't a built-in line
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

            //Absolute path of command
            if(args[0][0] == '/'){
                if(execv(args[0],args) == -1){
                    int errnum = errno;
                    perror("Instruction failed");
                    fprintf(stderr, "Value of errno: %d\n",errno);
                    fprintf(stderr, "Error: %s \n",strerror(errnum));
                }
            }
            //Relative path -- Need to check the $PATH environment variable
            else{

                char** paths = malloc(sizeof(char*));
                if(paths == NULL)
                    return -1;
                

                int nb_paths = get_paths(paths);

                int j = 0;

                while(j < nb_paths){
                    char path[256] = "";
                    strcat(path,paths[j]);
                    strcat(path,"/");
                    strcat(path,args[0]);


                    j++;
                
                    //Check if path contains the command to execute
                    if(access(path,X_OK) == 0){
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
            }

            exit(1);
        }

        //This is the father
        else{
            wait(&status);
            returnvalue = WEXITSTATUS(status);
            printf("\n%d",returnvalue);
            
        }

    free(args);

    }

    return 0;

}


