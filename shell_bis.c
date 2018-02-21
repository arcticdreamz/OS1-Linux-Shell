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
void cd_whitespace_dir(char** args);



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

    int nb_args = 0;
    char* token;
    
    token = strtok(line, "\n");
    token = strtok(line, " ");

    while(token != NULL){

        args[nb_args] = token;
        nb_args++;        
        token = strtok(NULL, " ");
    }

    args[nb_args] = (char*) NULL;

    return nb_args;

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
        paths[nb_paths] = path;
        nb_paths++;
        path = strtok(NULL,":"); //Parse the array for the next path delimited by ":"
    }

    return nb_paths;
}


/*************************************convert_whitespace_dir************************************
*
* Convert a directory/folder with special characters to a directory/folder with whitespaces
*
* ARGUMENT :
*   - args : an array containing all the args of the line  entered by the user
*
* SIDE EFFECTS : will clear all args except args[0] and args[1]
*
*******************************************************************************************/
void convert_whitespace_dir(char** args){

    int j=1;
    char path[256];
    strcpy(path,"");
    char* token;
    char delimiters[] = "\"\'\\";


    while(args[j] != NULL){

        //Get the first token delimited by one of the delimiters
        token = strtok(args[j], delimiters);


        while(token != NULL){
            //Add this token to the path
            strcat(path, token);
            //Get the next token
            token = strtok(NULL, delimiters);
        }

        //Add a whitespace
        strcat(path, " ");

        j++;
    }

    //Removing the last whitespace
    path[strlen(path)-1] = 0;

    //Cleaning all others arguments
    memset(&args[2], 0, sizeof(args)-2);
    
    //Copy the path to the unique argument of cd
    strcpy(args[1], path);
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
            stop = true;
            break;
        }

        //User presses "Enter"
        if(!strcmp(line,"\n"))
            continue;

        //User enters a line 
        int nb_args = split_line(line, args);

        //The command is cd
        if(!strcmp(args[0], "cd")){

            // Case 1 : cd or cd ~
            if(args[1] == NULL || !strcmp(args[1],"~"))
                args[1] = getenv("HOME");

            //Case 2 : cd ..
            else if(!strcmp(args[1],"..")){

                char* new_dir = strrchr(args[1],'/');

                if(new_dir != NULL)
                    *new_dir = '\0';
            }


            /*Case 3 :  cd FirstDir/"My directory"/DestDir
                        cd FirstDir/'My directory'/DestDir
                        cd FirstDir/My\ directory/DestDir
            */
            if(nb_args > 2){ //Means that there is/are (a) folder(s) with whitespace

                convert_whitespace_dir(args);
            }

            
            printf("\n%d",chdir(args[1]));
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
            exit(EXIT_FAILURE);
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

                char* paths[256];              

                int nb_paths = get_paths(paths);

                int j = 1;

                /*In the case of commands like mkdir/rmdir, if the first argument is a directory with whitespaces ("a b", 'a b', a\ b),
                  we need to change this directory in something understandable for the shell*/
                if(nb_args > 2){
                    if(args[1][0] == '\"' || args[1][0] == '\'')
                        convert_whitespace_dir(args);
                }

                //Taking a path from paths[] and concatenating with the command
                for(j = 0; j < nb_paths; j++){
                    char path[256] = "";
                    strcat(path,paths[j]);
                    strcat(path,"/");
                    strcat(path,args[0]);
                
                    //Check if path contains the command to execute
                    if(access(path,X_OK) == 0){
                        if(execv(path,args) == -1){
                            int errnum = errno;
                            perror("Instruction failed");
                            fprintf(stderr, "Value of errno: %d\n",errno);
                            fprintf(stderr, "Error: %s \n",strerror(errnum));
                        }

                        break;
                    }
                }
            }

            exit(EXIT_FAILURE);
        }

        //This is the father
        else{
            wait(&status);
            returnvalue = WEXITSTATUS(status);
            printf("\n%d",returnvalue);
            
        }

    }

    return 0;

}