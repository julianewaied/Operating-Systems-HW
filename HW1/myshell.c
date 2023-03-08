#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define MAX_ARGS 15

// this will be used later on for 'history'
typedef struct op* nodeptr;
typedef struct op
{
    char* cmd;
    nodeptr next;
}node;


void get_args(char* argv[MAX_ARGS],char* command);
node* list_push(node*,node*);
nodeptr cmd_push(node* new_node, node* head, char* cmd);
void print_list(nodeptr current);
int main(void)
{
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
    // a linked list of commands proceeded
    node* head = NULL;

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
            print_list(head);
            continue;
        }
        
        //remove the \n from command
        command[strlen(command)-1]='\0';
        
        // dont remove this code, waiting for safaa answer
/*
        int background_task=(command[strlen(command)-1]=='&');

        //remove the '&' token if we have a background task
        if(background_task) command[strlen(command)-1] = '\0';
        
*/
        nodeptr newNode = (nodeptr)malloc(sizeof(node));
        if(newNode==NULL) exit(1);
        newNode->next=NULL;
        newNode->cmd = malloc(BUFFER_SIZE * sizeof(int));
        if(newNode->cmd==NULL) exit(1);
        head = cmd_push(newNode,head,command);
        
        

        char* argv[MAX_ARGS];
        get_args(argv,command);

        int background_task;
        int i=0;
        while(argv[i+1]!=NULL) i++;
        // print(argv);
        char* last = argv[i];
        background_task= last[strlen(last)-1]=='&';

        //remove the '&' token if we have a background task
        if(background_task) last[strlen(last)-1]='\0';
        // if it's spaced from other arguments, get rid of it.
        if(last[0]=='\0') argv[i] = NULL;

        int pid = fork();

        if(pid==0){
            execvp(argv[0], argv);
            fprintf(stdout,"%s","command execution failed!\n");
            exit(1);
        }

        if(!background_task)
            waitpid(pid,NULL,0);

        
    }
    return 0;
}
nodeptr cmd_push(node* new_node, node* head, char* cmd)
{
    nodeptr h = list_push(new_node,head);
    strcpy(h->cmd,cmd);
    return h;
}
node* list_push(node* new_node, node* head)
{
    if((!new_node)) exit(1);
    if(head == NULL) return new_node;
    new_node->next = head;
    return new_node;
}
void print_list(nodeptr curr)
{
    if(!curr) return;
    fprintf(stdout, "%s\n", curr->cmd);
    print_list(curr->next);
}

void get_args(char* argv[MAX_ARGS],char* const command){
    char* arg;
    arg=strtok(command," ");
    int i=0;
    while(arg!=NULL){
        // add the current arg
        argv[i++]=arg;
        arg=strtok(NULL," ");
    }
    argv[i]=NULL;

}
