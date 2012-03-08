#include "mysched.h"

void preEmptorFunction(int signum)
{

}

void alarmHandlerFunction(int signum)
{
	struct itimerval value;
	getitimer(ITIMER_REAL,&value);

	mythread_scheduler();
	setitimer(ITIMER_REAL,&value,NULL);
}


static void mythread_sighandler(int sig, siginfo_t *siginfo, void *ucp)
{
	if (sig == SIGALRM)
	{
		struct itimerval value;
		getitimer(ITIMER_REAL,&value);

		mythread_scheduler();
		setitimer(ITIMER_REAL,&value,NULL);
	}
	else if (sig == SIGUSR1)
	{
		//TODO
	}
}
void mythread_init_sched(void)
{
	if (_isInit == 0)
	{
		struct sigaction userHandler,alarmHandler;
		struct itimerval value;

		sigemptyset(&userHandler.sa_mask);
		sigemptyset(&alarmHandler.sa_mask);

		userHandler.sa_flags = 0;
		alarmHandler.sa_flags = 0;

		//userHandler.sa_handler = preEmptorFunction;
		//alarmHandler.sa_handler = alarmHandlerFunction;

		userHandler.sa_sigaction = mythread_sighandler;
		alarmHandler.sa_sigaction = mythread_sighandler;

		//Take backups
		sigaction(SIGUSR1,NULL,oldUserHandler);
		sigaction(SIGALRM,NULL,oldAlarmHandler);

		sigaction(SIGUSR1,userHandler,NULL);
		sigaction(SIGALRM,alarmHandler,NULL);

		value.it_interval.tv_sec = 0;
		value.it_interval.tv_usec = 1000;
		value.it_value.tv_sec = 0;
		value.it_value.tv_usec = 1000;

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
