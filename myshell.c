#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

// this will be used later on for 'history'
typedef struct op* nodeptr;
typedef struct op
{
    char* cmd;
    nodeptr next;
}node;

node* list_push(node*,node*);
nodeptr cmd_push(node* new_node, node* head, char* cmd);
void print_list(nodeptr current);
//can u see this change
//another comment
int main(void)
{
    //close(2);
    //dup(1);
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

        nodeptr newNode = (nodeptr)malloc(sizeof(node));
        if(newNode==NULL) exit(1);
        newNode->next=NULL;
        newNode->cmd = malloc(BUFFER_SIZE * sizeof(int));
        if(newNode->cmd==NULL) exit(1);
        head = cmd_push(newNode,head,command);
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
    fprintf(stdout, "%s", curr->cmd);
    print_list(curr->next);
}
