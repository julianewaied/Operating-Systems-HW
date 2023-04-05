#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"
int *list_arr;
int in_read;
int out_read;
struct node
{
  // make it double linked list to make implementation easier :)
  int value;
  node *next;
  node *previous;
  pthread_mutex_t lock;
  // add more fields
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

list *create_list()
{
  list *l = malloc(sizeof(list))
  l->head = NULL;
  return l;
}

void delete_list(list *list)
{
  // add code here
}

void insert_value(list *list, int value)
{
  // add code here
}

void remove_value(list *list, int value)
{
  // add code here
}

void print_list(list *list)
{
  if(!list) exit(1);
  pthread_mutex_lock(&list->lock);
  node *current = list->head;
  pthread_mutex_unlock(&list->lock);
  int value;
  while (current)
  {
    pthread_mutex_lock(&current->lock);
    value = current->value;
    next = current->next;
    pthread_mutex_unlock(&current->lock);
    fprintf(stdout, "%d ", value);
    current = next;
  }
  printf("\n"); // DO NOT DELETE
}

void count_list(list *list, int (*predicate)(int))
{
  if(!list) exit(1);
  int count = 0; // DO NOT DELETE
  pthread_mutex_lock(&list->lock);
  node* current = list->head;
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
    count += predicate(value);
  }
  printf("%d items were counted\n", count); // DO NOT DELETE
  free(threads)
}
