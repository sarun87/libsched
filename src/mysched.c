#include <stdbool.h>
#include <sys/syscall.h>
#include "mysched.h"
#include "mythread.h"
int _isInit;


void printBothQueues();

//Original signal handlers for SIGUSR1 and SIGALRM
struct sigaction oldUserHandler,oldAlarmHandler;

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
	param->__sched_priority = attr->attr;
	return 0;
}
int mythread_attr_setschedparam(mythread_attr_t *attr, const struct sched_param *param)
{
	attr->attr = param->__sched_priority;
	return 0;
}

int mythread_scheduler()
{
	mythread_t mytcb = mythread_self();
	if(mytcb->reschedule == 1)
	{
		mytcb->reschedule = 0;
		mythread_queue_t readyQueue = *mythread_readyq();
		if(readyQueue!=NULL)
		{
			mythread_t highestPrio = (mythread_t) mythread_deq_prio(&readyQueue);
			if(highestPrio != NULL)
			{
				if(mytcb->attribute == NULL)
				{
					(mytcb->attribute) = malloc(sizeof(mythread_attr_t));
					(mytcb->attribute)->attr = DEFAULT_ATTR;
				}
				if(highestPrio->attribute == NULL)
				{
					highestPrio->attribute = malloc(sizeof(mythread_attr_t));
					highestPrio->attribute->attr = DEFAULT_ATTR;
				}
				if((highestPrio->attribute)->attr  <= (mytcb->attribute)->attr){
				// Preempt this thread.
					//printf("\nReceived signal. Switching ID:%d with %d",mytcb->tid,highestPrio->tid);
					//printf("\nReadyQueue - ");
					//printQueue(&readyQueue);
					//printf("\nRunQueue - ");
					//printQueue(mythread_runq());
					printf("\nReceived signal. Switching ID:%d with %d\n",mytcb->tid,highestPrio->tid);
					printf("\nmythread_scheduler:");
					printBothQueues();					
					mythread_block(mythread_runq(), 1);
					return 0;
				}
			}
		}
	}
	mythread_leave_kernel_nonpreemptive();
	return -1;
}

void mythread_leave_kernel()
{
	mythread_t mytcb = mythread_self();
	if(mytcb->reschedule == 1)
	{
		printf("\nIM HERE!!!!!!!!!!!!!!!!!!!!!");
		mytcb->reschedule = 0;
		mythread_scheduler();
	}
	else
	{
		mythread_leave_kernel_nonpreemptive();
	}
}
static int qCount = 0;
static void mythread_sighandler(int sig, siginfo_t *siginfo, void *ucp)
{
		mythread_t mytcb = mythread_self();
		struct itimerval value;
		getitimer(ITIMER_REAL,&value);
		//printf("\nThread Woken up is:%d",mythread_self()->tid);
		//printStuff();
		//printStuff();
		//return;
		
		// If main gets the alarm, send it to all threads on run queue.
		/*if(mytcb == NULL)
		{
			// Main got sigalarm
			printf("\nNULLLL TCBB!!\n");	
			mythread_queue_t runQueueIter = *mythread_runq();
			while(runQueueIter!=NULL)
			{ 
				if(runQueueIter->item != mytcb){
					//tgkill(-1, ((mythread_t)runQueueIter->item)->tid,SIGUSR1);
					syscall(SYS_tkill, ((mythread_t)runQueueIter->item)->tid, SIGUSR1);
				}
				runQueueIter = runQueueIter->next;
			}
			return;
		}
	*/
		if (mytcb == NULL)
		{
			printf("\nERROR:Couldn't get tcb!\n");
			exit(-1);
		}
		//if(sig == SIGALRM)
		//{
		//	printf("\nThread ID: %d got SIGALRM", mythread_self()->tid);
		//}
		//else if(sig == SIGUSR1)
		//{
		//	printf("\nThread ID: %d got SIGUSR1\n",mytcb->tid);
		//}
		//printf("\nReadyqueue:=");printQueue(mythread_readyq());
		//printf("\nRunqueue:-");printQueue(mythread_runq());
		//qCount++;
		//if(qCount ==4)
		//	exit(0);		
		if(mythread_tryenter_kernel() == true)
		{
			int present = false;
			mythread_queue_t runQueueIter2 = *mythread_runq();
			while(runQueueIter2!=NULL)
			{ 
				if(runQueueIter2->item == mytcb){
					present = true;	//tgkill(-1, ((mythread_t)runQueueIter->item)->tid,SIGUSR1);
					break;
				}
				runQueueIter2 = runQueueIter2->next;
			}
			if(present == false)
			{
				//printf("\nNON-RUN Thread Woken UP!!!");
				mythread_leave_kernel_nonpreemptive();
				runQueueIter2 = *mythread_runq();
				while(runQueueIter2!=NULL)
				{ 
					if(runQueueIter2->item != mytcb){
					//tgkill(-1, ((mythread_t)runQueueIter->item)->tid,SIGUSR1);
						syscall(SYS_tkill, ((mythread_t)runQueueIter2->item)->tid, SIGUSR1);
					}
					runQueueIter2 = runQueueIter2->next;
				}
			}
			else{
			
			
			mytcb->reschedule = 1;
			// Send signal to all threads in run q
			mythread_queue_t runQueueIter = *mythread_runq();
			while(runQueueIter!=NULL)
			{ 
				if(runQueueIter->item != mytcb){
					//tgkill(-1, ((mythread_t)runQueueIter->item)->tid,SIGUSR1);
					syscall(SYS_tkill, ((mythread_t)runQueueIter->item)->tid, SIGUSR1);
				}
				runQueueIter = runQueueIter->next;
			}
			mytcb->reschedule = 1;
			// Call scheduler
			mythread_scheduler();
			}
		}
		else
		{
			// If unable to enter the kernel, set the reschedule flag and return
			mytcb->reschedule = 1;
		}
	//	setitimer(ITIMER_REAL,&value,NULL);
}

struct sigaction userHandler,alarmHandler;
sigset_t signalSet,oldSet;

void mythread_init_sched(void)
{
	struct itimerval timer;
	struct timeval timerval;


	userHandler.sa_flags = 0;
	alarmHandler.sa_flags = 0;

	userHandler.sa_sigaction = mythread_sighandler;
	alarmHandler.sa_sigaction = mythread_sighandler;

	//Take backups
	sigaction(SIGUSR1,NULL,&oldUserHandler);
	sigaction(SIGALRM,NULL,&oldAlarmHandler);
	
	sigemptyset(&signalSet);
	sigaddset(&signalSet,SIGALRM);
	sigaddset(&signalSet,SIGUSR1);
	
	if (sigaction(SIGUSR1,&userHandler,NULL) == -1)
	{
		printf("\nERROR:Failed to set SIGUSR1 for thread %d\n",mythread_self()->tid);
		exit(-1);
	}
	if (sigaction(SIGALRM,&alarmHandler,NULL) == -1)
	{
		printf("\nERROR:Failed to set SIGALRM for thread %d\n",mythread_self()->tid);
		exit(-1);
	}
	
	sigprocmask(SIG_UNBLOCK, &signalSet,&oldSet);
	if(_isInit == 0)
	{
		timerval.tv_sec = 1;
		timerval.tv_usec = 0;
		timer.it_interval = timerval;
		timer.it_value = timerval;
		setitimer(ITIMER_REAL, &timer, NULL);
		//setitimer(ITIMER_REAL,&value,NULL);
		_isInit = 1;
	}
}

void mythread_exit_sched(void)
{
	//Restore the signal handlers.
	sigaction(SIGUSR1,&oldUserHandler,NULL);
	sigaction(SIGALRM,&oldAlarmHandler,NULL);
	sigprocmask(SIG_SETMASK, &oldSet,&signalSet);
	printf("\nmythread_exit_sched:");
	//printBothQueues();*/

}

void printBothQueues()
{

	printf("\nReadyQueue - ");
	printQueue(mythread_readyq());
	printf("\nRunQueue - ");
	printQueue(mythread_runq());
}
