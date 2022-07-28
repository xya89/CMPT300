#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



// built-in commands function declarationsfire

int cshell_exit(char **args);
int cshell_print(char **args);
int cshell_theme(char **args);
int cshell_log(char **args);


time_t rawtime;
struct tm * timeinfo;

# define ENV_ARR_SIZE 1024

typedef struct{
    char *name;
    char*value;
}EnvVar;


char env_var_symbol = '$';

int envVar_iter = 0;


EnvVar env_array[ENV_ARR_SIZE];






# define LOG_SIZE 10
# define COMMAND_ARR_SIZE 1024


typedef struct command{
    char* name;
    struct tm* time;
    int code;
} Command;

int command_iter = 0;
Command cmd_array[COMMAND_ARR_SIZE];







int compare_string(char *first, char *second)
{
   while(*first==*second)
   {
      if ( *first == '\0' || *second == '\0' )
         break;
 
      first++;
      second++;
   }
   if( *first == '\0' && *second == '\0' )
      return 0;
   else
      return -1;
}



// list of themes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m" 
#define ANSI_COLOR_BLU     "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"


char *theme_list[] = {
    "red",
    "green",
    "blue",
};

char *theme_clr[] = {
    ANSI_COLOR_RED,
    ANSI_COLOR_GREEN,
    ANSI_COLOR_BLU,
};




// list of built-in commands
char *builtin_str[] = {
    "exit",
    "print",
    "theme",
    "log",
};


int (*builtin_func[]) (char**) = {
    &cshell_exit,
    &cshell_print,
    &cshell_theme,
    &cshell_log,
};

int cshell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


// Build-in function implementations.


/**
    @brief Built-in command: exit
    @param args List of args.
    @return print "Bye!" amd returns 0, to terminate execution.
**/
int cshell_exit(char **args){
    printf("Bye! \n");
    return 0;
}






/**
    @brief Built-in command: print
    @param args List of args.
    @return print the args and return 1, to continue execution.
**/

int cshell_print(char *args[]){


    if (args[1] != NULL)
    {

        if (args[1][0]== '$' && args[1][1] != '\0')
        {

            
            char*token = strtok(args[1], "$");
            

            for (int k = 0; k < ENV_ARR_SIZE; k++)
            {
                if (env_array[k].name == NULL)
                {
                    printf("Variable not found!!\n");
                    break;
                }
                


                if (strcmp(token, env_array[k].name) == 0)
                {
                    //printf("***IN PRINT**** Name is: %s\n", env_array[k].name);
                    //printf("***IN PRINT**** Value is: %s\n", env_array[k].value);
                    printf("%s\n", env_array[k].value);
                    return 1;
                }

                
            }
            
            
            
        }

        else if (args[1][0] == '$' && args[1][1] == '\0')
        {
            printf("Expect variable name!\n");
            return 1;
        }
        
       // return 1;
        
    }




    int i = 1;
    while (args[i] != NULL){
        printf("%s ",args[i]);
        i++;
    }
    printf("\n");


    return 1;

}



/**
    @brief Built-in command: theme
    @param args takes an args 
    @return change the color of the text and return 1, to continue execution.
**/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m" 
#define ANSI_COLOR_BLU     "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int cshell_theme(char **args){

   

    if (args[1] == NULL)
    {
        printf("Specify a theme!\n");
        return 1;
    }

    /*
    for(int i = 0; i < 3; i++){
        if(strcmp(args[1], theme_list[i]) != 0) {
            
           printf("theme not supported!\n");
           break;
        }
       
    }
    */

   if ( (strcmp(args[1], "red") && (strcmp(args[1], "green")) && (strcmp(args[1], "blue"))) != 0 )
   {
        printf("theme not supported!\n");
   }
   


    for(int i = 0; i < 3; i++){
        if(strcmp(args[1], theme_list[i]) == 0) {
            
            printf("%s",theme_clr[i]);
            return 1;
        }
        
    }


    

}



/**
    @brief Built-in command: log
    @param args nothing
    @return print the history of called commands and return 1, to continue execution.
**/


int cshell_log(char **args){

    //printf("eneters\n");

    for (int i = 0; i < command_iter; i++)
    {
        printf("\n%s", asctime (cmd_array[i].time));
        printf("%s", cmd_array[i].name);
        printf(" %d\n",cmd_array[i].code);
    }
    
       
    printf("\n");
    return 1;
    


}







/**
    @brief Create a process and wait for it to terminate. 
    @return returns 1, to continue execution.
**/
int cshell_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if(execvp(args[0], args) == -1){
            perror("cshell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0){
        perror("cshell");
    }
    else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;

}

/**
    @brief Execute shell built-in or launch program.
    @return 1 if continuing execution, 0 if terminate.
**/
int cshell_execute(char **args){
    int i;

    if(args[0] == NULL){
        return 1;
    }else if (*args[0] == '$')
    {
        return 1;
    }
    

    for(i = 0; i < cshell_num_builtins(); i++){
        if(strcmp(args[0], builtin_str[i]) == 0) {
            rawtime = time(0);
            // time_t rawtime;
            cmd_array[command_iter].name = args[0];
            cmd_array[command_iter].time = localtime(&rawtime);
            cmd_array[command_iter].code =0;

            command_iter++;

            return(*builtin_func[i])(args);
        }
    }

    return cshell_launch(args);
}












/**
    @brief A fucntion to read line and split it into tokens
    @return The line from stdin.
**/
# define READ_LINE_BUFFERSIZE 1024
char *read_line(void){
    int buffersize = READ_LINE_BUFFERSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffersize);
    int c;

    // allocation failure
    if (!buffer){
        fprintf(stderr, "cshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        // read a character
        c = getchar();

        // When enter EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        } 
        position++;

        // handles environment variables
        





        // If entering line has exceeded the buffer, reallocate.
        if (position >= buffersize){
            buffersize += READ_LINE_BUFFERSIZE;
            buffer = realloc(buffer, buffersize);

            // allocation failure
            if(!buffer){
                fprintf(stderr, "cshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }




    }

}



/**
    @brief A function to split the line into tokens 
    @param line a line.
    @return an array of tokens
**/
# define TOKEN_BUFFERSIZE 64
# define SPLITTER " \t\r\n\a"


char **split_line(char *line){

    int buffersize = TOKEN_BUFFERSIZE;
    int position = 0;
    char **token_array = malloc(buffersize * sizeof(char*));
    char *token;

    // allocation failure
    if(!token_array) {
        fprintf(stderr, "cshell: allocation error\n");
        exit(EXIT_FAILURE);
    }


    // Dealing with env Var.
    if (line[0] == '$' && line[1] != '\0')
    {
        //printf("eneted in split line!!!!!!\n");

        char*env_var_name = strtok(line, "$=");
        char*last = strtok(NULL, "$=");

                    
            env_array[envVar_iter].name = &env_var_name[0];
            //printf("&env_var_name[0] is: %s\n", &env_var_name[0]);
            //printf("name is double check: %s\n", env_array[envVar_iter].name);


            env_array[envVar_iter].value = last;
            
            //printf("value is: %s\n", last);
            //printf("value is double check: %s\n", env_array[envVar_iter].value);

        
            envVar_iter++;
    }
    






    token = strtok(line, SPLITTER);


    while (token != NULL) {
        token_array[position] = token;
        position++;

        // reallocate if more tokens than allocated size
        if (position >= TOKEN_BUFFERSIZE){
            buffersize += TOKEN_BUFFERSIZE;
            token_array = realloc(token_array, buffersize * sizeof(char*));

            // allocation failure
             if(!token_array) {
                fprintf(stderr, "cshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SPLITTER);

    }

    token_array[position] = NULL;
    return token_array;
}


/**
    @brief function to handle inputs and execute them.
**/
void cshell_main_loop(void){
    char *line;
    char **args;
    int status;

    


    do {
        printf("cshell$ ");
        line = read_line();
        args = split_line(line);
        status = cshell_execute(args);
        
            //time_t rawtime;
            // cmd_array[command_iter].name = args[0];
            // cmd_array[command_iter].time = timeinfo;
            // cmd_array[command_iter].code =0;

            // command_iter++;



    } while(status);

    free(line);
    free(args);
        

}


/**
    @brief Main function
**/

int main(int argc, char**argv){

    //time_t rawtime;
    //rawtime = time(0);
    timeinfo = localtime (&rawtime); 


    EnvVar env_array[ENV_ARR_SIZE];

    cshell_main_loop();

    printf(ANSI_COLOR_RESET);




   
    

    return EXIT_SUCCESS;
}















