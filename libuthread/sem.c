#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	int count;
	struct queue *q;
};

sem_t sem_create(size_t count)
{
	preempt_disable();
	struct semaphore * sem = (struct semaphore *) malloc(sizeof(struct semaphore));
	if(sem == NULL)
		return NULL;

	sem->q = queue_create();
	preempt_enable();
	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem)
{
	if(sem == NULL)
		return -1;

	queue_destroy(sem->q);
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	if(sem == NULL)
		return -1;
	// keeps the thread in a queue if no resources
	while(sem->count == 0) {
		preempt_disable();
		queue_enqueue(sem->q, uthread_current());
		uthread_block();
	}
	sem->count--;
	return 0;
}

int sem_up(sem_t sem)
{
	if(sem == NULL)
		return -1;

	sem->count++;
	// checks for any threads waiting on the newly added resource
	if(queue_length(sem->q)) {
		preempt_disable();
		struct uthread_tcb *front_thread;
		queue_dequeue(sem->q, (void**) &front_thread);
		uthread_unblock(front_thread);
	}
	return 0;

}
