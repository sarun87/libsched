#include <stdbool.h>
#include "myqueue.h"
#include "mythread.h"

void printQueue(mythread_queue_t *headp)
{
	mythread_queue_t curr = *headp;

	if(*headp == NULL)
	{
		printf("\nQueue Empty!\n");
		return;
	}
	while (curr != NULL)
	{
		if(((mythread_t)curr->item)->attribute == NULL)
		{
			printf("%d (%d) --> ",((mythread_t)curr->item)->tid,10);
		}
		else
		{
			printf("%d (%d) --> ",((mythread_t)curr->item)->tid,((mythread_t)curr->item)->attribute->attr);
		}
		curr = curr->next;

	}
	printf("END\n");
}

mythread_queue_t getMaxPriorityThread(mythread_queue_t *headp)
{
	int highest = 999;
	mythread_queue_t iter = *headp;
	mythread_queue_t highestPriorityItem = NULL;
	if(*headp == NULL)
	{
		printf("\nQueue Empty!!\n");
		return NULL;
	}
	int threadPriority = DEFAULT_ATTR;
	while(iter!=NULL)
	{	
		if(((mythread_t)iter->item)->attribute != NULL)
		{
			threadPriority = (((mythread_t)iter->item)->attribute)->attr;
		}
		else
		{
			threadPriority = DEFAULT_ATTR;
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
		return (((mythread_t)highestPriorityItem->item)->attribute)->attr;
}

/* Initialize the queue */
void mythread_q_init(mythread_queue_t *headp, void *item)
{
	if(*headp == NULL)
	{
		//headp = malloc(sizeof(struct mythread_queue*));
		mythread_queue_t newNode = malloc(sizeof(struct mythread_queue));
		newNode->prev = NULL;
		newNode->next = NULL;
		newNode->item = item;
		*headp = newNode;
	}
	else
	{
		printf("\nError!! Re-initialization of already initialized queue!");
	}
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
	newNode->next = NULL;
	if(*headp == NULL)
		*headp = newNode;
	else
	{
		mythread_queue_t iter = *headp;
		while(iter->next !=NULL)
		{
			iter = iter->next;
		}
		iter->next = newNode;
		newNode->prev = iter;
	}
	if(*headp == *mythread_readyq())
	{
		printf("\nEnqueued in ready queue. Queue:");
		printQueue(headp);
	}
	else if(*headp == *mythread_runq())
	{
		printf("\nEnqueued in run queue. Queue:");
		printQueue(headp);
	}
	else
	{
		printf("\nEnqueued in some other queue");
		printQueue(headp);
	}
}

/* Remove element from the queue */
void mythread_deq(mythread_queue_t *headp, void *item)
{
	if(*headp == *mythread_runq())
	{
		//printf("\nDequeue item from run queue:%d",((mythread_t)item)->tid);
	}
	mythread_queue_t iter;
	iter = *headp;
	while(iter!=NULL)
	{
		if(iter->item == item)
		{
			// If it's the first element
			if(iter == *headp){
				(*headp) = (*headp)->next;
				// If item is not the only element in the queue
				if((*headp) !=NULL){
					// Set the prev pointer to null.
					(*headp)->prev = NULL;
				}
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
			//free(iter);
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
		printf("\nReturning NULL\n");
		return NULL;
	}
	returnValue = highestPriorityItem->item;
	return returnValue;
}

