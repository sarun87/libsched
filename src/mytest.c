#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"
#include "mysched.h"

void* threadFun(void * arg)
{
	int i=1;
	printf("\nStart thread Fun! ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);	
	sleep(2);
	printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	sleep(2);
	printf("\nDone thread Fun!:%d",mythread_self()->tid);
}

int main()
{
	mythread_t tid[20];
	int i;
	mythread_attr_t attr[20];
	mythread_setconcurrency(2);

	for(i = 0; i < 20; ++i)
	{
		mythread_attr_init(&attr[i]);
		mythread_create(&tid[i],&attr[i], threadFun,i);	
		printf("Tester:created thread %d\n",tid[i]->tid);
	}
	for(i = 0; i < 20; ++i)
	{
		mythread_join(tid[i],NULL);
	}
	return 0;
}
