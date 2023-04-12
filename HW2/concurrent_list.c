#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

node* make_node(int value,node* next);
void unlock(node* node);
void lock(node* node);

int *list_arr;
int in_read;
int out_read;
struct node
{
  int value;
  node *next;
  pthread_mutex_t lock;
};

struct list
{
  // we need the lock for reading the head.
  node *head;
  pthread_mutex_t lock;
};

void print_node(node *node)
{
  // DO NOT DELETE
  if (node)
  {
    printf("%d ", node->value);
  }
}

// you can use this as well
node* getFirst(list* l)
{
  // function gets a list and returns the first node after the -infinity
  if(!list) exit(1);
  pthread_mutex_lock(&list->lock);
  node *current = list->head;
  if(!current) exit(1);
  lock(current);
  node* p = current->next;
  unlock(current);
  pthread_mutex_unlock(&list->lock);
}

// checked!
void lock(node* node){
  if(node)
  pthread_mutex_lock(&(node->lock));
}

// checked!
void unlock(node* node){
  if(node)
  pthread_mutex_unlock(&(node->lock));
}

// checked!
list *create_list()
{
  list *l = malloc(sizeof(list));
  node* last=make_node(INT_MAX,NULL);
  node* first=make_node(INT_MIN,last); 
  l->head = first;
  pthread_mutex_init(&(l->lock), NULL);
  return l;
}

// checked!
void delete_list(list *list)
{
  //we want to delete the list, so we will keep it locked until it is deleted.
  pthread_mutex_lock(&(list->lock));
  lock(list->head);
  while(list->head)
  {
    node* head=list->head;
    lock(head->next);
    list->head=list->head->next;
    free(head);
  }
  
  
  pthread_mutex_unlock(&(list->lock));
  free(list);
  return;
}

// checked!
node* make_node(int value,node* next){
  node* new_node=(node*)malloc(sizeof(node));
  new_node->value=value;
  new_node->next=next;
  pthread_mutex_init(&(new_node->lock), NULL);
  return new_node;
}

// yet to check!
void insert_value(list *list, int value)
{
  node *prev, *curr;
  //init prev and curr
  pthread_mutex_lock(&(list->lock));
  prev=list->head;
  pthread_mutex_unlock(&(list->lock));
  lock(prev);
  curr=prev->next;
  lock(curr);
  while(curr){
    if((prev->value)<=value && value<= (curr->value)){
      //insert
      node* new_node=make_node(value,curr);
      prev->next=new_node;
      unlock(curr);
      unlock(prev);
      return;
    }

    //move prev and curr

    if(curr->next) lock(curr->next);
    node* p=prev;
    prev=curr;
    curr=curr->next;
    unlock(p);
  }

  unlock(prev);

  return;

}

// yet to check
void remove_value(list *list, int value)
{
  node *prev, *curr;

  //init prev and curr
  
  pthread_mutex_lock(&(list->lock));
  prev=list->head;
  pthread_mutex_unlock(&(list->lock));

  lock(prev);
  curr=prev->next;
  lock(curr);
  

  while(curr){
    if(value==curr->value){
      //remove curr

      prev->next=curr->next;
      free(curr);

      unlock(prev);
      
      return;
    }

    //move prev and curr

    if(curr->next) lock(curr->next);
    node* p=prev;
    prev=curr;
    curr=curr->next;
    unlock(p);
  }

  unlock(prev);

  return;
}

// checked!
void print_list(list *list)
{
  node* current = getFirst(list);
  lock(current);                        // loop assumption: current is locked
  int value;
  while (current->value<INT_MAX)
  {
    value = current->value;
    lock(current->next);
    node* next = current->next;
    unlock(current)
    fprintf(stdout, "%d ", value);
    current = next;
  }
  unlock(current);                      // the +infty node must be unlocked
  printf("\n");                         // DO NOT DELETE
}

// checked!
void count_list(list *list, int (*predicate)(int))
{
  if(!list) exit(1);
  int count = 0; // DO NOT DELETE
  node* current = getFirst(list)
  lock(current)
  node *next = NULL;
  int value;
  while (current->value<INT_MAX)
  {
    lock(current->next)
    value = current->value;
    next = current->next;
    unlock(current);
    current = next;
    count += predicate(value);
  }
  printf("%d items were counted\n", count); // DO NOT DELETE
}
