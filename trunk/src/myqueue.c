#include <stdbool.h>
#include "myqueue.h"

mythread_queue_t getMaxPriorityThread(mythread_queue_t *headp)
{
	int highest = 999;
	mythread_queue_t iter = *headp;
	if(*headp == NULL)
	{
		return NULL;
	}
	while(iter->next!=NULL)
	{	
		int threadPriority = DEFAULT_ATTR;
		if((iter->item)->attribute == NULL)
		{
			threadPriority = ((iter->item)->attribute)->attr;
		}
		if(threadPriority < highest)
		{
			highestPriorityItem = iter;
			highest = threadPriority;
		}
		iter = iter->next;
	}
	return highestPriorityItem;
}

int getIntMaxPriorityThread(mythread_queue_t *headp)
{
	mythread_queue_t highestPriorityItem = getMaxPriorityThread(headp);
	if(highestPriorityItem == NULL)
		return -1;
	else
		return ((highestPriorityItem->item)->attribute)->attr;
}

/* Initialize the queue */
void mythread_q_init(mythread_queue_t *headp, void *item)
{
	if(headp == NULL)
	{
		//headp = malloc(sizeof(struct mythread_queue*));
		mythread_queue_t newNode = malloc(sizeof(struct mythread_queue));
		newNode->prev = NULL;
		newNode->next = NULL;
		newNode->item = item;
		*headp = newNode;
		return 0;
	}
	else
	{
		printf("\nError!! Re-initialization of already initialized queue!");
	}
	return -1;
}

/* Test if item in Q, return TRUE if so, FALSE o/w */
int mythread_inq(mythread_queue_t *headp, void *item){
	mythread_queue_t iter = *headp;
	int returnValue = false;
	while(iter!=NULL)
	{
		if(iter->item == item)
		{
			returnValue = true;
			break;
		}
		iter = iter->next;
	}
	return returnValue;
}

/* Enqueue the new item */
void mythread_enq(mythread_queue_t *headp, void *item){
	mythread_queue_t newNode = malloc(sizeof(struct mythread_queue));
	newNode->item = item;
	newNode->prev = NULL;
	if(*headp == NULL)
		*headp = newNode;
	else
	{
		newNode->next = *headp;
		(*headp)->prev = newNode;
		(*headp) = newNode;
	}
}

/* Remove element from the queue */
void mythread_deq(mythread_queue_t *headp, void *item)
{
	mythread_queue_t iter;
	iter = *headp;
	while(iter!=NULL)
	{
		if(iter->item == item)
		{
			// If it's the first element
			if(iter == *headp){
				(*headp) = (*headp)->next;
				(*headp)->prev = NULL;
			}
			// last element of queue
			else if(iter->next == NULL)
			{
				(iter->prev)->next = NULL;
			}
			// Middle element of queue
			else
			{
				(iter->prev)->next = iter->next;
				(iter->next)->prev = iter->prev;
			}
			free(iter);
			break;
		}
		iter = iter->next;
	}
}

/* Dequeue item by priority */
void *mythread_deq_prio(mythread_queue_t *headp)
{
	mythread_queue_t highestPriorityItem = NULL;
	void *returnValue = NULL;
	
	highestPriorityItem = getMaxPriorityThread(headp);

	if(highestPriorityItem == NULL)
	{
		return NULL;
	}
	returnValue = (void*) highestPriorityItem->item;
	return returValue;
}

