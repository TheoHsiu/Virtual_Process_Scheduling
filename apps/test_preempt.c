/*
 * Infinite loop preempt test
 * 
 * Test preempt by printing a message in a loop in thread1. Thread2 exists, 
 * but because there is no yield call in thread1, we can not break out of the 
 * while-loop. If preempt is properly working, the infinite loop of thread1 will break. if 
 * preempt is not working, it will loop forever.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>


// this variable is only changeable in thread2
int unreachable = 1;


// thread2 is not reachable unless preempt is enabled.
void thread2(void *arg)
{
	(void)arg;

	printf("thanks to preempt we got to thread2\n");
	unreachable = 0;
	uthread_yield();
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);

	while (unreachable) {
		printf("thread1 will NEVER yield\n");
	}
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}
