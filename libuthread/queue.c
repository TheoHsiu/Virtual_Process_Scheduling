#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"



struct node {
	void *data;
	struct node *next;
};

struct queue {
	int len;
	struct node *front;
	struct node *rear;
};

// Variables for corner case for delete and iterate
struct node * skip;
int deleted = 0;

queue_t queue_create(void)
{
	struct queue *newQueue;
	newQueue = (struct queue *) malloc(sizeof(struct queue));
	newQueue->len = 0;
	return newQueue;
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL || queue->front != NULL)
		return -1;

	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL)
		return -1;

	struct node * new;
	new = (struct node *) malloc(sizeof(struct node));
	new->data = data;
	// enqueue for a new queue
	if(queue->front == NULL) {
		queue->front = new;
		queue->rear = new;
		new->next = NULL;
	}
	else {
		// enqueue for an existing queue
		queue->rear->next = new;
		queue->rear = new;
	}

	queue->len++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || data == NULL || !queue_length(queue))
		return -1;
	
	*data = queue->front->data;

	struct node * ptr = queue->front;
	queue->front = queue->front->next;
	free(ptr);
	queue->len--;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL)
		return -1;
	
	struct node * prev = queue->front;
	struct node * current = queue->front;
	struct node * next;
	while(current != NULL) {
		if(data == current->data) {
			if(current == queue->front) {
				// frees front node if data matches
				next = queue->front;
				queue->front = queue->front->next;
				free(next);
			}
			else if(current == queue->rear) {
				// frees back node if data matches
				queue->rear = prev;
				queue->rear->next = NULL;
				skip = current->next;
				free(current);
				deleted++;
			}
			else {
				// frees a node in the middle if it matches
				next = current->next;
				skip = current->next;
				free(current);
				prev->next = next;
				deleted++;
			}
			queue->len--;
			break;
		}
		if(current->next == NULL)
			return -1;
		if(current != prev)
			prev = current;
		current = current->next;
	}
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if(queue == NULL || func == NULL)
		return -1;
	
	struct node * current = queue->front;

  for(int i = 0; i < queue->len; i++) {
		// code to keep track of deletions and iterations, as freed code
		// is not accessible
		if (!deleted) {
			func(queue, current->data);
			current = current->next;
		}
		else {
			current = skip;
			deleted--;
		}
	}
	return 0;
}

int queue_length(queue_t queue)
{
	if(queue == NULL)
		return -1;

	return queue->len;
}
