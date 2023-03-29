#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

static int READY = 0;
static int RUNNING = 1;
static int BLOCKED = 2;
static int EXITED = -1;

queue_t thread_queue;
queue_t exit_queue;

struct uthread_tcb {
	uthread_ctx_t * mycontext;
	void* stack;
	int state;
};

struct uthread_tcb* curr_thread;

struct uthread_tcb *uthread_current(void)
{
	return curr_thread;
}

void uthread_yield(void)
{
	if(queue_length(thread_queue)) {
		struct uthread_tcb *prev_thread = uthread_current();
		// makes sure thread is "ready" and can be run
		if (prev_thread->state != EXITED && prev_thread->state != BLOCKED) {
			prev_thread->state = READY;
			queue_enqueue(thread_queue, (void*)prev_thread);
		}
		queue_dequeue(thread_queue, (void**)&curr_thread);
		uthread_current()->state = RUNNING;
		uthread_ctx_switch(prev_thread->mycontext, uthread_current()->mycontext);
	}
	preempt_enable();
}

void uthread_exit(void)
{
	// disable preempt before accessing global queue
	preempt_disable();
	uthread_current()->state = EXITED;
	// places thread into the exit queue to be removed later
	queue_enqueue(exit_queue, curr_thread);
	uthread_yield();
}

int uthread_create(uthread_func_t func, void *arg)
{
	preempt_disable();
	// Error Handling
	struct uthread_tcb *mythread = malloc(sizeof(struct uthread_tcb));
	if(mythread == NULL)
		return -1;

	mythread->mycontext = (uthread_ctx_t *)malloc(sizeof(uthread_ctx_t));
	if(mythread->mycontext == NULL)
		return -1;
	
	mythread->stack = uthread_ctx_alloc_stack();
	mythread->state = READY;
	if(uthread_ctx_init(mythread->mycontext, mythread->stack, func, arg) == -1)
		return -1;

	// disable preempt before accessing global queue
	preempt_disable();
	queue_enqueue(thread_queue, mythread);
	preempt_enable();
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	thread_queue = queue_create();
	exit_queue = queue_create();

	// Error Handling
	struct uthread_tcb *main_thread = malloc(sizeof(struct uthread_tcb));
	if(main_thread == NULL)
		return -1;

	main_thread->mycontext = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if(main_thread->mycontext == NULL)
		return -1;

	main_thread->stack = uthread_ctx_alloc_stack();
	main_thread->state = READY;

	curr_thread = main_thread;

	// Additional Error Handling
	if(uthread_create(func, arg) == -1)
		return -1;

	bool threads_ready = true;
	preempt_start(preempt);
	// Loop to keep yielding until all threads are over
	while(threads_ready)
	{
		// disable preempt before accessing global queue
		preempt_disable();
		// goes through exit queue and deletes "EXITED" threads
		if (queue_length(exit_queue))
			queue_iterate(exit_queue, (void*) queue_delete);

		uthread_yield();

		if (!queue_length(thread_queue))
		{
			queue_destroy(thread_queue);
			queue_destroy(exit_queue);
			threads_ready = false;
		}

	}
	if(preempt)
		preempt_stop();
	return 0;
}

void uthread_block(void)
{
	if(uthread_current()->state != BLOCKED)
		uthread_current()->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;
	queue_enqueue(thread_queue, uthread);
	uthread_yield();
}