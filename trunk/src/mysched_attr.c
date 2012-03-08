#include "mysched.h"
#include "mythread.h"

int mythread_attr_init(mythread_attr_t *attr)
{
	attr->attr = DEFAULT_ATTR;
	return 0;
}
int mythread_attr_destroy(mythread_attr_t *attr)
{
	attr->attr = DEFAULT_ATTR;
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
