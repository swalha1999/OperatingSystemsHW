#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

/*command struct to save the history of the commands*/
typedef struct Command* CommandPtr;
typedef struct Command
{
    char *rawCommamd;
    CommandPtr next_node;    
    CommandPtr prev_node;
} Command;

/*save a pointer to the satrt and the end of the double linkedlist*/
typedef struct CommandHistory* CommandHistoryPtr;
typedef struct CommandHistory
{
    CommandPtr head;
    CommandPtr tail;
    int len;
} CommandHistory;

CommandPtr createCommand(char* rawCommand);
void addCommandToHistory(CommandHistoryPtr commandsHistory, CommandPtr newCommand);
void printCommandHistory(CommandHistoryPtr commandHistory);
void freeCommandHistory(CommandHistoryPtr commandHistory);
void executeCommand(char* command, char** args, int argc);
void executeCommandAsync(char* command, char** args);
void executeCommandSync(char* command, char** args);
void parseCommand(char* command, char** out_command, char** out_args, int* argc);


int main(void)
{
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
    char* out_args[BUFFER_SIZE]; /* array to save all the command and its args */
    char* out_command;
    int argc;
    CommandHistory commandHistory = {NULL, NULL, 0}; /*the pointers to the start and the end of the array*/
    while (1)
    {
        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);
        fgets(command, BUFFER_SIZE, stdin);
        
        if(strncmp(command, "exit", 4) == 0)
        {
            break;
        }
        
        command[strcspn(command, "\n")] = '\0'; /*remove the new line from the end of the command*/
        addCommandToHistory(&commandHistory, createCommand(command)); /*add the command to the history*/
        
        /*check if the command is history and if so print all the commands history*/
        if(strncmp(command, "history", 7) == 0)
        {
            printCommandHistory(&commandHistory);
            continue;
        }
        
        /*parse the command and get the command and all its args this will change the command (will change the input)*/
        parseCommand(command, &out_command, out_args, &argc);
        /*excute the command eather sync or async */
        executeCommand(out_command, out_args, argc);
        
    }

    /*free all the memory before the exit of the main prosses*/
    freeCommandHistory(&commandHistory);
    return 0;
}

void executeCommand(char* command, char** args, int argc)
{
    if (argc == 0)
    {
        return;
    }

    /*check if there is & at the end if so run the command async(non-blocking) else wait for  the command to finish*/
    if(strncmp(args[argc-1], "&", 2) == 0)
    {   
        args[argc-1] = NULL;
        executeCommandAsync(command, args);
    }
    else
    {
        executeCommandSync(command, args);
    }
}

// run the command async (non-blocking) 
void executeCommandAsync(char* command, char** args)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        exit(1);
    }
    else
    if(pid == 0)
    {
        execvp(command, args);
        perror("error");
        exit(0);
    }

}

// run the command sync (blocking)
void executeCommandSync(char* command, char** args)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        exit(1);
    }
    if (pid == 0)
    {
        execvp(command, args);
        perror("error");
        exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}

CommandPtr createCommand(char* rawCommand)
{
    CommandPtr newCommand = (CommandPtr)malloc(sizeof(Command));
    newCommand->rawCommamd = (char*)malloc(BUFFER_SIZE);
    strcpy(newCommand->rawCommamd, rawCommand);
    newCommand->next_node = NULL;
    newCommand->prev_node = NULL;
    return newCommand;
}

/* add a commad to end of the list */
void addCommandToHistory(CommandHistoryPtr commandsHistory, CommandPtr newCommand)
{
    if (newCommand == NULL) return;
    if(commandsHistory->head == NULL)
    {
        commandsHistory->head = newCommand;
        commandsHistory->tail = newCommand;
        commandsHistory->len = 1;
        return;
    }
 
    commandsHistory->tail->next_node = newCommand;
    newCommand->prev_node = commandsHistory->tail;
    commandsHistory->tail = newCommand;
    commandsHistory->len++;
}

void printCommandHistory(CommandHistoryPtr commandHistory)
{
    CommandPtr temp = commandHistory->tail;
    int len = commandHistory->len;
    while(temp != NULL)
    {
        printf("%d %s\n", len--, temp->rawCommamd);
        temp = temp->prev_node;

    }
}

/*free the double linked list of the commands array*/
void freeCommandHistory(CommandHistoryPtr commandHistory)
{
    CommandPtr temp = commandHistory->head;
    CommandPtr next;
    while(temp != NULL)
    {
        next = temp->next_node;
        free(temp->rawCommamd);
        free(temp);
        temp = next;
    }
}

/*parse the command to main command and args array you must allocate the out_args array*/
void parseCommand(char* command, char** out_command, char** out_args, int* argc){
    char* token;
    int i;
    out_args[0] = strtok(command, " ");
    *out_command = out_args[0];
    if (out_args[0] == NULL)
    {
        *argc = 0;
        return;
    }
    i = 1;
    token = strtok(NULL, " ");
    while(token != NULL)
    {
        out_args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    out_args[i] = NULL;
    *argc = i;
}

