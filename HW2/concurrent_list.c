#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"
int* list_arr;
int in_read;
int out_read;
struct node {
  int value;
  node* next;
  node* previous;
  // add more fields
};

struct list {
  // add fields
  node* head;
  int size;
};

void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
    printf("%d ", node->value);
  }
}

list* create_list()
{
  // add code here
  list* l = malloc(sizeof(list))
  l->head = NULL;
  return l; // REPLACE WITH YOUR OWN CODE
}

void delete_list(list* list)
{
  // add code here
}

void insert_value(list* list, int value)
{
  // add code here
}

void remove_value(list* list, int value)
{
  // add code here
}

void* scan(void* argv)
{
  node* current = (node*) argv;
  while(current)
  {
    // bounded buffer algorithm
    while (in_read+1==out_read);
    int value = current->value;
    
    list_arr[in_read] = value;
    current = current->next;
    in_read++;
  }
}

void print_list(list* list)
{
  // implementing bounded buffer without cyclic arrays
  node* current = list->head;
  list_arr = malloc((list->size)*sizeof(int));
  in_read =0;
  out_read = 0;
  pthread_t* scanner;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(scanner,&attr,scan,(void*)current);
  int i = 0 ;
  for(i=0;i<list->size;i++)
  {
    while(in_read == out_read);                         // bounded buffer
    fprintf(stdout,"%d ", list_arr[out_read]);
    out_read = (out_read + 1);
  }
  printf("\n"); // DO NOT DELETE
  free(list_arr)
}

void count_list(list* list, int (*predicate)(int))
{
  // notice! this is a read only task, meaning, locks are not needed.
  int count = 0; // DO NOT DELETE
  node* current = list->head;
  pthread_t* threads = malloc(sizeof(pthread_t)*list->size);
  int i=0;
  while(current)
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(threads+i,&attr,predicate,current->value);
    i++;
  }
  for(i=0;i<list->size;i++)
  {
    count += pthread_join(threads[i],NULL);
  }
  printf("%d items were counted\n", count); // DO NOT DELETE
  free(threads)
}
