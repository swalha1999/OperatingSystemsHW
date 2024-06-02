#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

typedef struct Command* CommandPtr;
typedef struct Command
{
    char *rawCommamd;
    CommandPtr next_node;    
    CommandPtr prev_node;
} Command;

typedef struct CommandHistory* CommandHistoryPtr;
typedef struct CommandHistory
{
    CommandPtr head;
    CommandPtr tail;
} CommandHistory;

CommandPtr createCommand(char* rawCommand);
void addCommandToHistory(CommandHistoryPtr commandsHistory, CommandPtr newCommand);
void printCommandHistory(CommandHistoryPtr commandHistory);
void freeCommandHistory(CommandHistoryPtr commandHistory);
void executeCommand(char* command, char** args, int argsc);
void executeCommandAsync(char* command);
void executeCommandSync(char* command);
void parseCommand(char* command, char* out_command, char** out_args, int* argsc);

char* TrimStr(char* str);


int main(void)
{
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
    char out_command[BUFFER_SIZE];
    char* out_args[BUFFER_SIZE];
    int argsc;
    CommandHistory commandHistory = {NULL, NULL};
    while (1)
    {
        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);
        fgets(command, BUFFER_SIZE, stdin);
        if(strncmp(command, "exit", 4) == 0)
        {
            break;
        }

        if(strncmp(command, "history", 7) == 0)
        {
            printCommandHistory(&commandHistory);
            addCommandToHistory(&commandHistory, createCommand(command));
            continue;
        }

        
        addCommandToHistory(&commandHistory, createCommand(command));

        
    }

    freeCommandHistory(&commandHistory);
    return 0;
}

void executeCommand(char* command, char** args, int argsc)
{
    if(args[argsc] == "&")
    {   
        executeCommandAsync(command);
    }
    else
    {
        executeCommandSync(command);
    }
}

void executeCommandAsync(char* command)
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
        char* args[] = {"/bin/sh", "-c", command, NULL};
        execvp(args[0], args);
        perror("error");
        exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}

void executeCommandSync(char* command)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        exit(1);
    }
    if (pid == 0)
    {
        char* args[] = {"/bin/sh", "-c", command, NULL};
        execvp(args[0], args);
        perror("error");
        exit(0);
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
    if(commandsHistory->head == NULL)
    {
        commandsHistory->head = newCommand;
        commandsHistory->tail = newCommand;
        return;
    }
 
    commandsHistory->tail->next_node = newCommand;
    newCommand->prev_node = commandsHistory->tail;
    commandsHistory->tail = newCommand;
    
}

void printCommandHistory(CommandHistoryPtr commandHistory)
{
    CommandPtr temp = commandHistory->head;
    while(temp != NULL)
    {
        fprintf(stdout, "%s\n", temp->rawCommamd);
        temp = temp->next_node;
    }
}

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


void parseCommand(char* command, char* out_command, char** out_args, int* argsc){
    char* token = strtok(command, " ");
    strcpy(out_command, token);
    int i = 0;
    while(token != NULL)
    {
        out_args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    out_args[i] = NULL;
    *argsc = i;
}