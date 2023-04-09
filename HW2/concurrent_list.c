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
  node* first=make_node(-INT_MAX,last);
  
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
<<<<<<< Updated upstream
    
    if(head->next)lock(head->next);
=======
    lock(head->next);
>>>>>>> Stashed changes
    list->head=list->head->next;
    free(head);
  }
  free(list);
  return;
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
// checked!
node* make_node(int value,node* next){
  node* new_node=(node*)malloc(sizeof(node));
  new_node->value=value;
  new_node->next=next;

  pthread_mutex_init(&(new_node->lock), NULL);

  return new_node;

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
// to be updated
void print_list(list *list)
{
  if(!list) exit(1);
  pthread_mutex_lock(&list->lock);
  node *current = list->head->next;
  pthread_mutex_unlock(&list->lock);
  int value;
  while (current)
  {
    pthread_mutex_lock(&current->lock);
    value = current->value;
    node* next = current->next;
    pthread_mutex_unlock(&current->lock);
    if(next)fprintf(stdout, "%d ", value);
    current = next;
  }
  printf("\n"); // DO NOT DELETE
}
// to be updated!
void count_list(list *list, int (*predicate)(int))
{
  if(!list) exit(1);
  int count = 0; // DO NOT DELETE
  pthread_mutex_lock(&list->lock);
  node* current = list->head->next;
  pthread_mutex_unlock(&list->lock);
  node *next = NULL;
  int value;
  while (current)
  {
    pthread_mutex_lock(&current->lock);
    value = current->value;
    next = current->next;
    pthread_mutex_unlock(&current->lock);
    current = next;
    if(next)count += predicate(value);
  }
  printf("%d items were counted\n", count); // DO NOT DELETE
  //free(threads); this is old right? idk what its doing here
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
