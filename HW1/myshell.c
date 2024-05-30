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
    char *name;
    char **args;
    CommandPtr next_node;    
    CommandPtr prev_node;
} Command;

typedef struct CommandHistory* CommandHistoryPtr;
typedef struct CommandHistory
{
    CommandPtr head;
    CommandPtr tail;
} CommandHistory;

CommandPtr createCommand(char *name, char **args);
void addCommandToHistory(CommandHistoryPtr commandsHistory, CommandPtr newCommand);
void printCommandHistory(CommandHistoryPtr commandHistory);
void freeCommandHistory(CommandHistoryPtr commandHistory);

int main(void)
{
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
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

        /* code starts here*/
        addCommandToHistory(&commandHistory, createCommand("ls", NULL));
        addCommandToHistory(&commandHistory, createCommand("cd", NULL));
        addCommandToHistory(&commandHistory, createCommand("pwd", NULL));

        printCommandHistory(&commandHistory);
        
        /* code ends here*/
    }

    freeCommandHistory(&commandHistory);
    return 0;
}

CommandPtr createCommand(char *name, char **args)
{
    CommandPtr newCommand = (CommandPtr)malloc(sizeof(Command));
    newCommand->name = name;
    newCommand->args = args;
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
        fprintf(stdout, "%s\n", temp->name);
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
        free(temp);
        temp = next;
    }
}