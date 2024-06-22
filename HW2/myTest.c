#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "concurrent_list.h"

#define MAX_THREAD_COUNT 252


pthread_t threads[MAX_THREAD_COUNT];
int thread_count = 0;
list* mylist = NULL;

void* delete_list_task(void* arg)
{
	delete_list(mylist);
	mylist = NULL;
	return 0;
}

void* print_list_task(void* arg)
{
	print_list(mylist);
	return 0;
}

void* insert_value_task(void* arg)
{
	int value = (int)arg;
	insert_value(mylist, value);
	return 0;
}

void* remove_value_task(void* arg)
{
	int value = (int)arg;
	remove_value(mylist, value);
	return 0;
}

void* count_greater_task(void* arg)
{
	int threshold = (int)arg;
	int pred(int value)
	{
		return value > threshold;
	}	
	count_list(mylist, pred);
	return 0;
}


int main(int argc, const char** argv)
{

		mylist = create_list();

		pthread_create(&threads[thread_count], NULL, delete_list_task, NULL);
		thread_count++;

		pthread_create(&threads[thread_count], NULL, print_list_task, NULL);
		thread_count++;

		pthread_create(&threads[thread_count], NULL, insert_value_task, (void*)value);
		thread_count++;

		pthread_create(&threads[thread_count], NULL, remove_value_task, (void*)value);
		thread_count++;

		pthread_create(&threads[thread_count], NULL, count_greater_task, (void*)value);
		thread_count++;

		for(int i = 0; i < thread_count; i++)
		{
			pthread_join(threads[i], NULL);
		}
		thread_count = 0;
	return 0;
}
