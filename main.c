/*
NAME: ABINATH S
DATE: 13-02-2026
DESCRIPTION: Implement a minimalistic shell, mini-shell(msh) as part of the Linux Internal module.

Objective: The objective is to understand and use the system calls w.r.t process creation, signal handling, process                         synchronization, exit status, text parsing etc..

1. Provide a prompt for the user to enter commands
2. Execute the command entered by the user
3. Special Variables
4. Signal handling
5. Built-in commands
6. Background Process / Job control
7. Pipe functionality
 */



#include "header.h"

char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
    "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
    "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs", "fg", "bg", NULL};

char *external_commands[153];

char prompt_string[20];
//declare the input string
char input_string[20];

int main()
{

    //Register the SIGINT signal
    signal(SIGINT, signal_handler);

    //Register the SIGTSTP signal
    signal(SIGTSTP, signal_handler);

    //Register the SIGCHLD signal
    signal(SIGCHLD, signal_handler);

    //Extract external commands to 2D array
    extract_external_commands(external_commands);

    //clear the terminal
    system("clear");

    //declare prompt string
    strcpy(prompt_string, "mini_shell:~$ ");


    while(1)
    {
        //print the prompt string
        printf("%s",prompt_string);
        fflush(stdout);

        //clear the string or char in the input string
        bzero(input_string,sizeof(input_string));

        //read and validate string from the user
        scanf("%[^\n]",input_string);
        getchar();

        //check for empty string
        if(input_string[0] == '\0')
        {
            continue;
        }

        //check for 'PS1=' command
        if(strncmp(input_string, "PS1=", 4) == 0)
        {
            if(strchr(input_string, ' ') == NULL)//check for space in string
            {
                if(strlen(input_string)>4)
                {
                    strcpy(prompt_string, input_string+4);//assign prompt string
                }
                else
                {
                    printf("Error: Invalid Argument\nUsage: PS1=<NEW_PROMPT>\n");
                }
            }
            else
            {
                printf("Error: Invalid Argument\nUsage: PS1=<NEW_PROMPT>\n");
            }
        }

        //get command function
        char *command = get_command(input_string);

        //check command type
        int ret = check_command_type(command);
        switch(ret)
        {
            case BUILTIN:
                //execute internal commands
                execute_internal_commands(input_string);
                break;
            case EXTERNAL:
                //execute external commands
                execute_external_commands(input_string);
                break;
            case NO_COMMAND:
                //printf("No such command\n");
                break;
        }
    }
}
