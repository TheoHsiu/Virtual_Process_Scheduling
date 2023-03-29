#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}
void test_queue_length(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Test passes if queue length correctly finds "9" */
	fprintf(stderr, "*** TEST length ***\n");
    TEST_ASSERT(queue_length(q) == 9);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
	fprintf(stderr, "*** TEST iterator ***\n");
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}

void test_dequeue(void)
{
	queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;
	int ptr;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		queue_enqueue(q, &data[i]);
	}
	while(queue_length(q)) {
		queue_dequeue(q, (void**)&ptr);
	}
	fprintf(stderr, "*** TEST dequeue ***\n");
	TEST_ASSERT(queue_length(q) == 0);
}

void test_dequeue_empty_queue(void)
{
	queue_t q;
    int ptr;
	int error;

    q = queue_create();
   	error = queue_dequeue(q, (void**)&ptr);

	fprintf(stderr, "*** TEST dequeue empty queue ***\n");
	TEST_ASSERT(error == -1);
}

void test_delete_not_in_queue(void)
{
	queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t i;
    int ptr = 42;
    int original_length;
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		queue_enqueue(q, &data[i]);
	}
    original_length = queue_length(q);
   	queue_delete(q, &ptr);

	fprintf(stderr, "*** TEST delete when not in queue ***\n");
	TEST_ASSERT(queue_length(q) == original_length);
}




int main(void)
{
	test_create();
	test_queue_simple();
    test_queue_length();
	test_iterator();
	test_dequeue();
    test_dequeue_empty_queue();
    test_delete_not_in_queue();

	return 0;
}
