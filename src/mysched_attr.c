#include "mysched.h"
#include "mythread.h"

int mythread_attr_init(mythread_attr_t *attr)
{
	attr->attr = 10;
}
int mythread_attr_destroy(mythread_attr_t *attr)
{
	//TODO: set a negative (invalid) value?
}
int mythread_attr_getschedparam(const mythread_attr_t *attr, struct sched_param *param)
{
	param->__sched_priority = attr->attr;
}
int mythread_attr_setschedparam(mythread_attr_t *attr, const struct sched_param *param)
{
	attr->attr = param->__sched_priority;
}
