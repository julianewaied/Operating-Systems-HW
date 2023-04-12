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

// checked
void lock(node* node){
  if(node)
  pthread_mutex_lock(&(node->lock));
}
// checked
void unlock(node* node){
  if(node)
  pthread_mutex_unlock(&(node->lock));
}
// checked
list *create_list()
{
  list *l = malloc(sizeof(list));

  node* last=make_node(INT_MAX,NULL);
  node* first=make_node(INT_MIN,last); 
  
  l->head = first;
  pthread_mutex_init(&(l->lock), NULL);
  return l;
}
// problematic - you need to unlock before freeing
void delete_list(list *list)
{
  //we want to delete the list, so we will keep it locked until it is deleted.
  pthread_mutex_lock(&(list->lock));
  lock(list->head);
  while(list->head){
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
// to be checked
void print_list(list *list)
{
  if(!list) exit(1);
  pthread_mutex_lock(&list->lock);
  node *current = list->head;
  if(!current) exit(1);
  lock(current);
  lock(current->next); // why lock here and then later use an if to make sure it isnt locked? u already have 
                      //the first node locked so u dont need to lock the one after it
  pthread_mutex_unlock(&list->lock);
  int value;
  while (current->value<INT_MAX)
  {
    value = current->value;
    // the if condition is to make sure we're not locking twice
    if(value>INT_MIN&& value<INT_MAX) lock(current->next); //here the first case value=min u didnt need to lock the second node
                                                            //and value cant be INT_MAX anyways becuase then u dont enter the loop so this if statement does nothing
    node* next = current->next;
    unlock(current)
    if(next)fprintf(stdout, "%d ", value); // here the first one will just get printed 
    current = next;
  }
  printf("\n"); // DO NOT DELETE
}
// to be checked
void count_list(list *list, int (*predicate)(int))
{
  if(!list) exit(1);
  int count = 0; // DO NOT DELETE
  pthread_mutex_lock(&list->lock);
  // I suspect this might lead to a problem but I'll think abt it
  node* current = list->head->next; //this does actually lead to problems
  lock(current)
  pthread_mutex_unlock(&list->lock);
  node *next = NULL;
  int value;
  while (current)
  {
    if(current->value<INT_MAX) lock(current->next)
    value = current->value;
    next = current->next;
    unlock(current);
    current = next;
    if(next)count += predicate(value);
  }
  printf("%d items were counted\n", count); // DO NOT DELETE
}

// this doesn't work!! you gotta get and return void*
int greater_10(int val){
  return val>10;
}

/*int main(){
  list* list=create_list();
  insert_value(list,15);
  insert_value(list,7);
  insert_value(list,20);
  insert_value(list,1);
  remove_value(list,7);
  remove_value(list,19);
  remove_value(list,19);

  count_list(list,greater_10);
  
  print_list(list);
  delete_list(list);
  

}*/
