#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define MAX_ARGS 15

typedef struct op* nodeptr;
typedef struct op
{
    char* cmd;
    nodeptr next;
    int serial_number;
}node;


void get_args(char* argv[MAX_ARGS],char* command);
node* list_push(node*,node*);
nodeptr cmd_push(node* head, char* cmd);
void print_list(nodeptr current);
int is_background(char* argv[MAX_ARGS]);
void activate(char* command);
int main(void)
{
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
    node* head = NULL;
    while (1)
    {
        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);
        fgets(command, BUFFER_SIZE, stdin);
        if(strncmp(command, "exit", 4) == 0)
            break;
        command[strlen(command)-1] = '\0';
        if(strncmp(command, "history", 7) == 0)
            print_list(head);
        else activate(command);
    }
    return 0;
}
nodeptr cmd_push(node* head, char* cmd)
{
    nodeptr new_node = (nodeptr)malloc(sizeof(node));
    if(new_node==NULL) exit(1);
    new_node->next=NULL;
    new_node->cmd = malloc(BUFFER_SIZE * sizeof(int));
    if(new_node->cmd==NULL) exit(1);
    nodeptr h = list_push(new_node,head);
    strcpy(h->cmd,cmd);
    return h;
}
node* list_push(node* new_node, node* head)
{
    if((!new_node)) exit(1);
    if(head == NULL) 
    {
        new_node->serial_number = 1;
        return new_node;
    }
    new_node->next = head;
    new_node->serial_number = head->serial_number + 1;
    return new_node;
}
void print_list(nodeptr curr)
{
    if(!curr) return;
    fprintf(stdout, "%d %s\n", curr->serial_number,curr->cmd);
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
void activate(char* command)
{
    char* argv[MAX_ARGS];
    get_args(argv,command);
    if(argv[0][strlen(argv[0])-1]=='\n')
        argv[0][strlen(argv[0])-1]='\0';
    int background_task = is_background(argv);
    pid_t pid = fork();
    if(pid==0)
    {
        execvp(argv[0], argv);
        perror("error");
        exit(1);
    }
    if(!background_task)
        waitpid(pid,NULL,0);
}
int is_background(char* argv[MAX_ARGS])
{
    int i=0;
    int background_task;
    while(argv[i+1]!=NULL) i++;
    char* last = argv[i];
    background_task= last[strlen(last)-1]=='&';
    //remove the '&' token if we have a background task
    if(background_task) last[strlen(last)-1]='\0';
    // if it's spaced from other arguments, get rid of it.
    if(last[0]=='\0') argv[i] = NULL;
    return background_task;
}
