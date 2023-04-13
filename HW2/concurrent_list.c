#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

node* make_node(int value,node* next);
void unlock(node* node);
void lock(node* node);
node* getFirst(list*);

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

// prints a node data
void print_node(node *node)
{
  // DO NOT DELETE
  if (node)
  {
    printf("%d ", node->value);
  }
}

// gets a list and returns the first number after the -infinity node safely with locks
node* getFirst(list* list)
{
  if(!list) 
   return NULL;
  pthread_mutex_lock(&(list->lock));
  node *current = list->head;
  if(!current)
  {
    pthread_mutex_unlock(&list->lock);
    return NULL;
  } 
  lock(current);
  node* p = current->next;
  unlock(current);
  pthread_mutex_unlock(&list->lock);
  return p;
}

// locks a node if it is no NULL
void lock(node* node){
  if(node)
  pthread_mutex_lock(&(node->lock));
}

// unlocks a node if it is not NULL
void unlock(node* node){
  if(node)
  pthread_mutex_unlock(&(node->lock));
}

// creates an empty list including only +infinity and -infinity
list *create_list()
{
  list *l = malloc(sizeof(list));
  node* last=make_node(INT_MAX,NULL);
  node* first=make_node(INT_MIN,last); 
  l->head = first;
  pthread_mutex_init(&(l->lock), NULL);
  return l;
}

// deletes the whole list
void delete_list(list *list)
{
  //we want to delete the list, so we will keep it locked until it is deleted.
  if(!list) return;
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

// makes a node and intiates its lock
node* make_node(int value,node* next){
  node* new_node=(node*)malloc(sizeof(node));
  new_node->value=value;
  new_node->next=next;
  pthread_mutex_init(&(new_node->lock), NULL);
  return new_node;
}

// inserts the value in its sorted position to list
void insert_value(list *list, int value)
{
  if(!list) return;
  node *prev, *curr;
  //init prev and curr
  pthread_mutex_lock(&(list->lock));
  prev=list->head;
  pthread_mutex_unlock(&(list->lock));
  lock(prev);
  curr=prev->next;
  lock(curr);
  while(1)
  {
    if(!curr)
    {
      unlock(curr);
      unlock(prev);
      return;
    }
    if((prev->value)<=value && value<= (curr->value)){
      //insert
      node* new_node=make_node(value,curr);
      prev->next=new_node;
      unlock(curr);
      unlock(prev);
      return;
    }

    //move prev and curr

    lock(curr->next);
    node* p=prev;
    prev=curr;
    curr=curr->next;
    unlock(p);
  }
  unlock(prev);
  return;

}

// removes first appearance of value from list
void remove_value(list *list, int value)
{
  node *prev, *curr;
  if(!list) return;
  //init prev and curr
  pthread_mutex_lock(&(list->lock));
  prev=list->head;
  pthread_mutex_unlock(&(list->lock));
  lock(prev);
  curr=prev->next;
  lock(curr);
  

  while(curr){
    // check for corruption
    if(!prev || !curr) 
    {
      unlock(prev);
      unlock(curr);
      return;
    }
    if(value==curr->value){
      //remove curr

      prev->next=curr->next;
      free(curr);

      unlock(prev);
      
      return;
    }

    //move prev and curr

    lock(curr->next);
    node* p=prev;
    prev=curr;
    curr=curr->next;
    unlock(p);
  }

  unlock(prev);

  return;
}

// prints the list serially 
void print_list(list *list)
{
  node* current = getFirst(list);
  if(!current) return;
  lock(current);                        // loop assumption: current is locked
  int value;
  while (current->value<INT_MAX)
  {
    printf("test in core");
    value = current->value;
    lock(current->next);
    node* next = current->next;
    unlock(current);
    printf("%d ", value);
    current = next;
  }
  unlock(current);                      // the +infty node must be unlocked
  printf("\n");                         // DO NOT DELETE
}

// prints the number of values in list where predicate returns 1
void count_list(list *list, int (*predicate)(int))
{
  if(!list) return;
  int count = 0; // DO NOT DELETE
  node* current = getFirst(list);
  if(!current) return;
  lock(current);
  node *next = NULL;
  int value;
  while (current->value<INT_MAX)
  {
    lock(current->next);
    value = current->value;
    next = current->next;
    unlock(current);
    current = next;
    count += predicate(value);
  }
  unlock(current);
  printf("%d items were counted\n", count); // DO NOT DELETE
}
