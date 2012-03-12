#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"
#include "mysched.h"
#include "mymutex.h"
//#include <sched.h>

mythread_mutex_t m;
int sum = 0;

void* threadFun(void * arg)
{
	int i=1;
	//printf("\nStart thread Fun! ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	//printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	//printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);	
	//sleep(2);
	//while(1)
//	{
//		;//printf("Infinite!!");
//	}
	//printStuff();
	printStuff();
	//printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	//printf("\nStill working ID:%d",mythread_self()->tid);
	for(i =0; i < 99999; ++i);
	//sleep(2);
	printf("\n-------------Done thread Fun!:%d",mythread_self()->tid);
	mythread_mutex_lock(&m);
		sum = sum +1;
	mythread_mutex_unlock(&m);
}

int main()
{
	mythread_t tid[20];
	int i;
	struct sched_param param;
	mythread_attr_t attr[20];
	mythread_setconcurrency(1);
	mythread_mutex_init(&m);
	for(i = 0; i < 20; ++i)
	{
		param.__sched_priority = (i%2 ==0 ?  i:3);
		mythread_attr_init(&attr[i]);
		mythread_attr_setschedparam(&attr[i],&param);
		mythread_create(&tid[i],&attr[i], threadFun,i);
		if(i == 5);
		//	break;	
		printf("Tester:created thread %d\n",tid[i]->tid);
	}
	for(i = 0; i < 20; ++i)
	{
		mythread_join(tid[i],NULL);
		if(i==5);
		//	break;
	}
	printf("\nSum = %d",sum);
	printStuff();
	return 0;
}
