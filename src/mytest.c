/* Group Info - 
asriram Arun Sriraman
shyamp Shyam Prasad
vineet Vineet Krishnan
*/
#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"
#include "mysched.h"
#include "mymutex.h"

#define MAX_NUM_THREADS 20
mythread_mutex_t m;
int sum = 0;
int TCnum = 0;
int TC_PASSED = 1;

void* threadFun(void * arg)
{
	int i=1, j;
	struct sched_param param;
	mythread_t thisThread = mythread_self();
	for(i =0; i < 99999; ++i);
	for(i =0; i < 99999; ++i);
	for(i =0; i < 99999; ++i);	
	for(i = 0; i < 60000; ++i)
	{
		for(j=0; j<200; ++j);
	}
	for(i =0; i < 99999; ++i);
	for(i =0; i < 99999; ++i);
	if(TCnum == 1)
	{
		mythread_attr_getschedparam(thisThread->attribute, &param);
		if(param.__sched_priority != (thisThread->attribute)->attr)
			TC_PASSED = 0;
	}
	mythread_mutex_lock(&m);
		sum = sum +1;
	mythread_mutex_unlock(&m);
	printf("\t\tThread Exiting !: %d\n",thisThread->tid);
}

int main()
{
	mythread_t tid[MAX_NUM_THREADS];
	struct sched_param param;
	mythread_attr_t attr[MAX_NUM_THREADS];
	int i;
	
	mythread_setconcurrency(2);
	mythread_mutex_init(&m);
	printf("\nTest 1-> checking for \"sum\" with %d threads having dafault priority\n");
	for(i = 0; i < MAX_NUM_THREADS; ++i)
	{
		mythread_attr_setschedparam(&attr[i], &param);
		mythread_create(&tid[i],&attr[i], threadFun, NULL);
	}

	for(i = 0; i < MAX_NUM_THREADS; ++i)
	{
		mythread_join(tid[i],NULL);
	}

	if(sum == MAX_NUM_THREADS)
		printf("\nTest 1: PASSED\n");
	else 
		printf("\nTest 1: FAILED\n");
	sum = 0;

	printf("\nTest 2-> checking for \"sum\" with %d threads with every odd thread having priority = 1 and even thread even priorities\n");
	for(i = 0; i < MAX_NUM_THREADS; ++i)
	{
		param.__sched_priority = (i % 2 == 0 ? i : 1);
		mythread_attr_init(&attr[i]);
		mythread_attr_setschedparam(&attr[i], &param);
		mythread_create(&tid[i],&attr[i], threadFun,NULL);
	}
	for(i = 0; i < MAX_NUM_THREADS; ++i)
	{
		mythread_join(tid[i], NULL);
	}
	
	if(sum == MAX_NUM_THREADS && TC_PASSED == 1)
		printf("\nTest 2: PASSED\n");
	else 
		printf("\nTest 2: FAILED\n");
	return 0;
}
