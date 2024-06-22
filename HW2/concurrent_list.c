#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
  int value;
  node* next;
  pthread_mutex_t lock;
};

struct list {
  node* head;
  pthread_mutex_t list_lock;
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
  list* new_list = (list*)malloc(sizeof(list));
  if (new_list == NULL) {
    return NULL;
  }
  new_list->head = NULL;
  pthread_mutex_init(&new_list->list_lock, NULL);
  return new_list;
}

void delete_list(list* list)
{
  if (list == NULL) {
    return;
  }
  // wait for all threads to finish and lock the list
  pthread_mutex_lock(&list->list_lock);

  node* current = list->head;
  while (current != NULL) {
    pthread_mutex_lock(&current->lock);
    node* next = current->next;
    pthread_mutex_destroy(&current->lock);
    free(current);
    current = next;
  }
  pthread_mutex_destroy(&list->list_lock);
  free(list);
}

void insert_value(list* list, int value)
{
  
  if (list == NULL) {
    return;
  }
  
  node* new_node = (node*)malloc(sizeof(node));
  if (new_node == NULL) {
    return;
  }
  new_node->value = value;
  pthread_mutex_init(&new_node->lock, NULL);

  // lock the list
  pthread_mutex_lock(&list->list_lock);

  // check if the list is empty or the value is less than the head
  if (list->head == NULL || list->head->value > value) {
    new_node->next = list->head;
    list->head = new_node;
    pthread_mutex_unlock(&list->list_lock);
    return;
  }

  // if not get the lock the head node and unlock the list lock
  node *current = list->head;
  pthread_mutex_lock(&current->lock);
  pthread_mutex_unlock(&list->list_lock);

  // iterate through the list to find the correct position
  while (current->next != NULL && current->next->value < value) {
    node *next = current->next;
    pthread_mutex_lock(&next->lock);
    pthread_mutex_unlock(&current->lock);
    current = next;
  }

  // insert the new node
  new_node->next = current->next;
  current->next = new_node;
  pthread_mutex_unlock(&current->lock);

}

void remove_value(list* list, int value)
{
  if (list == NULL) {
    return;
  }

  // lock the list
  pthread_mutex_lock(&list->list_lock);

  // check if the list is empty or the value is less than the head
  if (list->head == NULL || list->head->value > value) {
    pthread_mutex_unlock(&list->list_lock);
    return;
  }

  // if not get the lock the head node and unlock the list lock
  node *current = list->head;
  pthread_mutex_lock(&current->lock);

  // check if the head node is the one to be removed
  if (current->value == value) {
    list->head = current->next;
    pthread_mutex_unlock(&current->lock);
    pthread_mutex_destroy(&current->lock);
    free(current);
    pthread_mutex_unlock(&list->list_lock);
    return;
  }
  
  pthread_mutex_unlock(&list->list_lock);

  // iterate through the list to find the correct position
  while (current->next != NULL && current->next->value < value) {
    node *next = current->next;
    pthread_mutex_lock(&next->lock);
    pthread_mutex_unlock(&current->lock);
    current = next;
  }

  // remove the node
  if (current->next != NULL && current->next->value == value) {
    node *next = current->next;
    current->next = next->next;
    pthread_mutex_unlock(&next->lock);
    pthread_mutex_destroy(&next->lock);
    free(next);
  }

  pthread_mutex_unlock(&current->lock);

}

void print_list(list* list)
{
  if (list == NULL) {
    return;
  }

  // lock the list and lock the head node
  pthread_mutex_lock(&list->list_lock);
  node* current = list->head;
  if (current == NULL) {
    pthread_mutex_unlock(&list->list_lock);
    return;
  }
  pthread_mutex_lock(&current->lock);
  pthread_mutex_unlock(&list->list_lock);

  // iterate through the list and print the nodes
  while (current != NULL) {
    print_node(current);
    node* next = current->next;
    if (next != NULL) {
      pthread_mutex_lock(&next->lock);
    }
    pthread_mutex_unlock(&current->lock);
    current = next;
  }

  printf("\n"); // DO NOT DELETE
}

void count_list(list* list, int (*predicate)(int))
{
  int count = 0; // DO NOT DELETE

  if (list == NULL) {
    printf("%d items were counted\n", count);
    return;
  }

  // lock the list and lock the head node
  pthread_mutex_lock(&list->list_lock);
  node* current = list->head;
  if (current == NULL) {
    pthread_mutex_unlock(&list->list_lock);
    printf("%d items were counted\n", count);
    return;
  }
  pthread_mutex_lock(&current->lock);
  pthread_mutex_unlock(&list->list_lock);

  // iterate through the list and print the nodes
  while (current != NULL) {
    if (predicate(current->value)) {
      count++;
    }
    node* next = current->next;
    if (next != NULL) {
      pthread_mutex_lock(&next->lock);
    }
    pthread_mutex_unlock(&current->lock);
    current = next;
  }


  
  printf("%d items were counted\n", count); // DO NOT DELETE
}
