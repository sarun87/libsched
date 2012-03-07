#include "myqueue.h"

mythread_queue_t mythread_ready= NULL;
mythread_queue_t mythread_run = NULL;

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
		if(((iter->item)->attribute)->attr < highest)
		{
			highestPriorityItem = iter;
			highest = ((iter->item)->attribute)->attr;
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
	if(iter == NULL)
	{
		item = NULL;
		return;
	}
	while(iter->next != NULL)
	{
		iter = iter->next;
	}
	item = iter->item;
	(iter->prev)->next = NULL;
	free(iter);
}

/* Dequeue item by priority */
void *mythread_deq_prio(mythread_queue_t *headp)
{
	void *highestPriorityItem = NULL;
	void *returnValue = NULL;
	
	highestPriorityItem = getMaxPriorityThread(headp);

	if(highestPriorityItem == NULL)
	{
		return NULL;
	}

	(highestPriorityItem->prev)->next = highestPriorityItem->next;
	(highestPriorityItem->next)->prev = highestPriorityItem->prev;

	returnValue = highestPriorityItem->item;

	if(highestPriorityItem->next == NULL && highestPriorityItem->prev == NULL)
	{
		// Only node in the queue
		*headp = NULL;
	}
	free(highestPriorityItem);

	return returValue;
}

