#include "mysched.h"

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
			mythread_t highestPrio = mythread_queue_deq_prio(&readyQueue);
			if((highestPrio->attribute)->attr  <= (mytcb->attribute)->attr)
			{
				// Preempt this thread.
				mythread_block(&readyQueue, NULL);
				return 0;
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
		mytcb->reschedule = 0;
		mythread_scheduler();
	}
	else
	{
		mythread_leave_kernel_nonpreemptive();
	}
}

static void mythread_sighandler(int sig, siginfo_t *siginfo, void *ucp)
{
		mythread_t mytcb = mythread_self();
		struct itimerval value;
		getitimer(ITIMER_REAL,&value);
		

		if(mythread_tryenter_kernel() == true)
		{
			// Send signal to all threads in run q
			mythread_queue_t runQueueIter = *mythread_runq();
			while(runQueueIter!=NULL)
			{ 
				if(runQueueIter->item != mytcb){
					tgkill(-1, ((mythread_t)runQueue->item)->tid,SIGUSR1);
				}
				runQueueIter = runQueueIter->next;
			}
			mytcb->rechedule = 1;
			// Call scheduler
			mythread_scheduler();
		}
		else
		{
			// If unable to enter the kernel, set the reschedule flag and return
			mytcb->rechedule = 1;
		}
		setitimer(ITIMER_REAL,&value,NULL);
}
void mythread_init_sched(void)
{
	if (_isInit == 0)
	{
		struct sigaction userHandler,alarmHandler;
		struct itimerval value;
		sigset_t userSignalSet,alarmSignalSet;

		sigemptyset(&userSignalSet);
		sigemptyset(&alarmSignalSet);

		userHandler.sa_flags = 0;
		alarmHandler.sa_flags = 0;

		userHandler.sa_sigaction = mythread_sighandler;
		alarmHandler.sa_sigaction = mythread_sighandler;

		//Take backups
		//Need to take backups of sigset_t's also?
		//TODO: Shouldn't this be outside the _isInit?
		sigaction(SIGUSR1,NULL,oldUserHandler);
		sigaction(SIGALRM,NULL,oldAlarmHandler);
		
		sigaddset(SIGALRM,&alarmSignalSet);
		sigaddset(SIGUSR1,&userSignalSet);

		sigaction(SIGUSR1,userHandler,NULL);
		sigaction(SIGALRM,alarmHandler,NULL);

		value.it_interval.tv_sec = 0;
		value.it_interval.tv_usec = 10000;
		value.it_value.tv_sec = 0;
		value.it_value.tv_usec = 10000;

		setitimer(ITIMER_REAL,&value,NULL);
		_isInit = 1;
	}
}

void mythread_exit_sched(void)
{
	//Restore the signal handlers.
	if (_isInit = 1)
	{
		sigaction(SIGUSR1,oldUserHandler,NULL);
		sigaction(SIGALRM,oldAlarmHandler,NULL);
		_isInit = 0;
	}
}
