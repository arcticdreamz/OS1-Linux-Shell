
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


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

int main(int argc, char** argv){

    bool stop = false;

    char line[65536]; 
    char* args[256];
    
    

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

        int j = 1;

        //If the argument is a path ("..."), like mkdir/rmdir
        if(args[1][0] == '\"'){

        char delimiters[] = "\"\'\\";

        char* token;
        char path[256];
        strcpy(path,"");

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
        
        //Copy the path to the unique argument of cd
        strcpy(args[1], path);
    }   
}
}