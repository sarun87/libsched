#include "sched.h"
#include "mythread.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <malloc.h>
#include <sys/syscall.h>
#include <signal.h>

static struct sigaction newAction, oldAction;
sigset_t oldSet, newSet;
struct itimerval value;
static bool _isInit = false;

int mythread_attr_init(mythread_attr_t *attr)
{
        attr->attr = DEFAULT_ATTR;
        return 0;
}

int mythread_attr_destroy(mythread_attr_t *attr)
{
        attr->attr = -1;
        return 0;
}
int mythread_attr_getschedparam(const mythread_attr_t *attr, struct sched_param *param)
{
	if(attr == NULL ||  param == NULL)
		return -1;
        param->__sched_priority = attr->attr;
        return 0;
}
int mythread_attr_setschedparam(mythread_attr_t *attr, const struct sched_param *param)
{
	if(attr == NULL || param == NULL)
		return -1;
        attr->attr = param->__sched_priority;
        return 0;
}


static int mythread_scheduler()
{
	mythread_t mytcb = mythread_self();
	if(mytcb->reschedule == 1)
	{
		mythread_queue_t readyQueue = *mythread_readyq();
		if(readyQueue == NULL)
		{
			mytcb->reschedule = 0;
			mythread_leave_kernel_nonpreemptive();
			return 0;
		}
		mythread_t highPrioItem = (mythread_t) mythread_deq_prio(&readyQueue);
		if(highPrioItem->attribute == NULL)
		{
			highPrioItem->attribute = malloc(sizeof(mythread_attr_t));
			highPrioItem->attribute->attr = DEFAULT_ATTR;
		}
		if(mytcb->attribute == NULL)
		{
			mytcb->attribute = malloc(sizeof(mythread_attr_t));
			mytcb->attribute->attr = DEFAULT_ATTR;
		}
		if(mytcb->attribute->attr >= highPrioItem->attribute->attr)
		{
			mytcb->reschedule = 0;
			mythread_block(mythread_readyq(), 1);
			return 0;
		}
	}
	mythread_leave_kernel_nonpreemptive();
	return -1;
}

static void mythread_sighandler(int sig, siginfo_t *siginfo, void *ucp)
{
	mythread_t mytcb = mythread_self();
	if(mythread_tryenter_kernel())
	{
		// Check if signal got by thread in run q. if not, don't do anytihng
		mythread_queue_t runQ = *mythread_runq();
		bool present = false;
		while(runQ!=NULL)
		{
			if(mytcb->tid == ((mythread_t)runQ->item)->tid)
			{
				//printf("-\n--- HERE--");
				present = true;
				break;
			}
			runQ=runQ->next;
		}
		if(present == false)
		{
			printf("\n---HERE");
			mythread_leave_kernel_nonpreemptive();
			return;
		}
		// If sigalarm, send SIGUSR1 to other threads. call scheduler
		if(sig == SIGVTALRM)
		{
			//printf("\nAlarm received by:%d",mytcb->tid);
			mythread_queue_t runQueue = *mythread_runq();
			while(runQueue!=NULL)
			{	mythread_t item = (mythread_t) runQueue->item;
				if(mytcb->tid != item->tid)
					syscall(SYS_tkill,item->tid, SIGUSR1);
				runQueue=runQueue->next; 
			}
		}
		mytcb->reschedule = 1;
		mythread_scheduler();		
	}
	else
	{
		// Unable to enter kernel. Set flag and quit.
		mytcb->reschedule = 1;
	}
}

void mythread_init_sched(void)
{
	newAction.sa_sigaction = mythread_sighandler;
	sigemptyset(&newAction.sa_mask);
	newAction.sa_flags |= SA_RESTART | SA_SIGINFO;
	sigemptyset(&newSet);
	sigaddset(&newSet,SIGUSR1);
	sigaddset(&newSet,SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &newSet, &oldSet);

	sigaction(SIGVTALRM, &newAction, &oldAction);
	sigaction(SIGUSR1, &newAction, NULL);

	if(_isInit == false)
	{
		value.it_interval.tv_sec = 0;
		value.it_interval.tv_usec = 10000;
		value.it_value.tv_sec = 0;
		value.it_value.tv_usec = 10000;
		setitimer(ITIMER_VIRTUAL, &value, NULL);
		_isInit = true;		
	}
}

void mythread_exit_sched()
{
	// Restore old signal mask
	sigprocmask(SIG_BLOCK, &oldSet, NULL);
}

void mythread_leave_kernel()
{
	mythread_t mytcb = mythread_self();
	if(mytcb == NULL)
		printf("Error! Should never get here!\n");
	else
	{
		if(mytcb->reschedule == 1)
			mythread_scheduler();
		else
			mythread_leave_kernel_nonpreemptive();
	}
}

void printStuff(){
        printf("\nReady Queue::\n");
        printQueue(mythread_readyq());
        printf("\nRun Queue::\n");
        printQueue(mythread_runq());
}











