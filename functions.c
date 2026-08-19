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

//global variables and declaration acess using extern keyword
extern char *builtins[];
extern char *external_commands[];
extern int status;
extern char prompt_string[];
extern char input_string[];
extern int pid;

//structure initialization
stopped_process *head = NULL;

//global declaration of variables
int si_no = 1;
int no = 1;
int pid = 0;
int status = 0;

//function definition for extract external commands
void extract_external_commands(char **external_commands)
{
    FILE *fp = fopen("external_cmd.txt","r");//open the file
    for(int i=0;i<152;i++)
    {
        char buf[20];
        fscanf(fp, "%[^\n]\n", buf);//read line by line
        int len = strlen(buf);
        buf[len] = '\0';
        external_commands[i] = malloc(len+1);
        strcpy(external_commands[i], buf);//store in 2D array
    }
    external_commands[152] = NULL;//store NULL at last
}

//function definition for get the command while pass options
char *get_command(char *input_string)
{
    int len = strlen(input_string);
    char * command = malloc(len);
    strcpy(command,input_string);
    int i=0;
    while(command[i] != '\0')
    {
        if(command[i] == ' ')//check for space 
        {
            command[i] = '\0';
            break;
        }
        i++;
    }
    return command;//return the commend only
}

//function definition for check the commend type
int check_command_type(char *command)
{
    int i = 0;
    while(builtins[i] != NULL)//first check in builtins 
    {
        if(!(strcmp(builtins[i],command)))
        {
            return BUILTIN;//if command present in builtin return the macro
        }
        i++;
    }
    int j = 0;
    while(external_commands[j] != NULL)//check for commend present in external
    {
        if(!(strcmp(external_commands[j],command)))
        {
            return EXTERNAL;//return external macro
        }
        j++;
    }
    return NO_COMMAND;
}

//function definition for execute the external command
void execute_external_commands(char *input_string)
{
    pid = fork();//create the child
    if(pid > 0)//parent
    {
        pid_t wait_ret = waitpid(pid,&status,WUNTRACED);
        if (WIFSTOPPED(status))//check if stopped or not
        {
            insert_stopped_process(pid, input_string);//if stopped insert in the list and print the process
            printf("[%d]\t",no);
            no++;
            printf("Stopped\t");
            printf("%s\n",input_string);
        }
        pid = 0;
    }
    else if(pid == 0)//child
    {

        signal(SIGINT, SIG_DFL);//restore the SIGINT signal
        signal(SIGTSTP, SIG_DFL);//restore the SIGTSTP signal
        signal(SIGCHLD, SIG_DFL);//restore the SIGCHLD signal

        if(strchr(input_string, '|') == NULL)//check for pipe is presnt in input string
        {
            //execvp
            char *argv[20];//if not present 
            int i = 0;

            argv[i] = strtok(input_string, " ");//only the commend ie.., befor space
            while (argv[i] != NULL)
            {
                i++;
                argv[i] = strtok(NULL, " ");
            }

            execvp(argv[0], argv);//execute via execvp
            printf("execvp not executed\n");
            exit(1);
        }
        else//if pipe is present execute the n pipe function
        {
            //npipe
            execute_n_pipe(input_string);
            exit(0);
        }
    }
    else
    {
        //if child not created
        printf("Child creation failed\n");
        exit(0);
    }
}

//function definition for execute n pipe
void execute_n_pipe(char *input_string)
{
    char *argv[50];
    int argc = 0;

    argv[argc] = strtok(input_string, " ");
    while (argv[argc] != NULL)
    {
        argc++;
        argv[argc] = strtok(NULL, " ");
    }

    if (argc < 3)//check for commands in before and after the pipe
    {
        printf("Invalid pipe usage\n");
        return;
    }

    int cmd_idx[argc];
    cmd_idx[0] = 0;
    int idx = 1;

    for (int i = 0; i < argc; i++)//check the pipe in the command line
    {
        if (strcmp(argv[i], "|") == 0)
        {
            cmd_idx[idx++] = i + 1;  // NEXT command starts here
            argv[i] = NULL;
        }
    }

    int prev_fd = -1;
    int pip[2];

    for (int i = 0; i < idx; i++)
    {
        if (i != idx - 1)
            pipe(pip);

        pid_t pid = fork();//create the child

        if (pid == 0)
        {
            /* child */

            if (prev_fd != -1)//close the stdout to pipe
            {
                dup2(prev_fd, 0);
                close(prev_fd);
            }

            if (i != idx - 1)//close the another pipe ends
            {
                dup2(pip[1], 1);
                close(pip[0]);
                close(pip[1]);
            }

            execvp(argv[cmd_idx[i]], &argv[cmd_idx[i]]);//execute the commands via execvp
            perror("execvp");
            exit(1);
        }
        else
        {
            /* parent */

            if (prev_fd != -1)
                close(prev_fd);

            if (i != idx - 1)
            {
                close(pip[1]);
                prev_fd = pip[0];
            }
        }
    }

    /* wait for all children */
    for (int i = 0; i < idx; i++)
        wait(NULL);
}

//function definition for execute internal commands
void execute_internal_commands(char *input_string)
{
    //for exit
    if (strcmp(input_string, "exit") == 0)
    {
        exit(0);
    }

    //for pwd
    if (strcmp(input_string, "pwd") == 0)
    {
        char buf[100];
        getcwd(buf, sizeof(buf));
        printf("%s\n", buf);
    }

    //for echo
    else if (strncmp(input_string, "echo", 4) == 0)
    {
        if(input_string[4] == '\0')
        {
            char buf[100];
            scanf(" %[^\n]",buf);
            printf("%s\n",buf);
        }
        //for echo with empty option
        else if(input_string[4] == ' ' && input_string[5] == '\0')
        {
            printf("\n");
        }
        //for echo with $$
        else if(strcmp(input_string+5, "$$") == 0)
        {
            printf("%d\n", getpid());
        }
        //for echo with $?
        else if(strcmp(input_string+5, "$?") == 0)
        {
            if(WIFEXITED(status))
            {
                printf("%d\n",WEXITSTATUS(status));
            }
        }
        //for echo with $SHELL
        else if(strcmp(input_string+5, "$SHELL") == 0)
        {
            char *shell = getenv("SHELL");
            if(shell != NULL)
            {
                printf("%s\n",shell);
            }
            else
            {
                printf("\n");
            }
        }
        else
        {
            printf("%s\n",input_string+5);
        }
    }

    //for cd
    else if (strncmp(input_string, "cd", 2) == 0)
    {
        if (input_string[2] == '\0')
        {
            chdir("/home/abinath");   // or home, depends on requirement
        }
        else
        {
            // skip "cd "
            if (chdir(input_string + 3) != 0)
            {
                perror("cd");
            }
        }
    }

    //for clear
    else if(strcmp(input_string, "clear") == 0)
    {
        system("clear");
    }

    //for jobs
    else if (strcmp(input_string, "jobs") == 0)
    {
        stopped_process *temp = head;
        while (temp)//traverse the linked list to print the details
        {
            printf("[%d]\t",si_no);
            si_no++;
            printf("Stopped\t");
            printf("%s\n",temp->cmd);
            temp = temp -> link;
        }
        si_no = 1;
    }

    //for fg
    else if (strcmp(input_string, "fg") == 0)
    {
        if(head != NULL)
        {
            stopped_process *temp = head;
            printf("%s\n",temp->cmd);
            kill(head->pid, SIGCONT);//send the signal
            waitpid(head->pid, &status, WUNTRACED);
            head = head->link;//after execute the last stopped process delete from list
            free(temp);
        }
    }

    //for bg
    else if (strcmp(input_string, "bg") == 0)
    {
        if(head != NULL)
        {
            signal(SIGCHLD, signal_handler);
            stopped_process *bg = head;
            strcpy(bg->cmd,head->cmd);
            printf("%s\n",bg->cmd);
            kill(head->pid, SIGCONT);//send the signal
                                     //delete at first
            head = head->link;
            free(bg);
        }
    }
}

//function definition for signal handler
void signal_handler(int sig_num)
{
    //for SIGINT signal
    if(sig_num == SIGINT)
    {
        if(pid == 0)
        {
            printf("\n%s",prompt_string);
            fflush(stdout);
        }
    }
    //for SIGTSTP signal
    if(sig_num == SIGTSTP)
    {
        if(pid == 0)
        {
            printf("\n%s",prompt_string);
            fflush(stdout);
        }
    }

    //for SIGCHLD signal
    if(sig_num == SIGCHLD)
    {
        int child_status;
        pid_t child_pid;

        // Reap all finished children (non-blocking)
        while ((child_pid = waitpid(-1, &child_status, WNOHANG)) > 0)
        {
            // Child is collected here
            // No zombie will remain
        }
    }
}

//function definition for insert stopped process
void insert_stopped_process(int pid, char *cmd)
{
    stopped_process *new = malloc(sizeof(stopped_process));

    new->pid = pid;//store pid
    strcpy(new->cmd, cmd);//store the command

    new->link = head;//store the node in head
    head = new;
}
