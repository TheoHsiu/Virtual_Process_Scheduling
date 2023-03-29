# ECS 150 Project 2: Thread Library

## Introduction

The Thread Library project was made to create a basic user-level thread 
library for Linux, that is capable of creating and running threads 
concurrently. To do so, the project was designed to be able to:
Create new threads
Use a queue to schedule threads to execute in a FIFO structure
Be preemptive, which would allow it to provide an interrupt-based scheduler
Implement semaphores, which provide for thread synchronization

## Queue API

Before starting on threads, we first created a simple FIFO Queue API. The 
queue keeps track of nodes that contain data which are used to hold 
information. The queue would keep pointers on the front and back of the queue. 

The `rear` pointer is necessary to allow for enqueue to put new nodes into 
the back of the queue, while ‘front’ is used in dequeue, to allocate whatever 
is currently first in line into another pointer, and then change front to 
whatever is next in the queue.

## Makefile

The makefile was simple to implement. Since the apps makefile did most of the 
work, all we had to do was compile and create the library and then compile its 
dependencies. First we had to define the libraries dependencies. We used a 
patsubst which parsed through the folder and figured out the object 
dependencies. Then we just compiled the library with the objs.

## uthread API

The second phase was the uthread API, which was made of a thread struct, and 
several functions that can keep track of threads in a queue, and yield the 
current thread to the next thread waiting in the queue. The `uthread_tcb` 
struct is made up of 3 things, the current state, a stack, and a context for 
the backup of CPU registers.

First, when `uthread_run` is called, it registers the original thread as the 
idle thread and initially creates a main thread. The thread then initializes 
a new thread with the given function inside. This thread becomes the initial 
thread, which has access to all the functions in the library. Then, the main 
thread stays in an infinite loop, where it will either return once no more 
threads are in the system, yield to the next available thread, or clear any 
exited threads out of the system.

Our implementation of  `uthread_yield` is made to switch the pointer of the 
current thread, or `curr_thread`, to the thread in the front of our thread 
queue. The previous `curr_thread` is moved to the back of the queue. 

Then, the thread is dequeued from the queue and put into the current thread 
pointer, before finally switching contexts to that of the newly assigned 
current thread.

## Semaphore API

The semaphore API is used to control the access to commonly distributed 
resources between threads. Semaphores do this with either `sem_up` or 
`sem_down`. 

`sem_up` adds a resource to the semaphore, and also keeps track of what 
threads are currently waiting for the resource to become available again. 
If the resource goes from 0 to 1, it takes the waiting blocked thread from 
the semaphore’s queue, unblocks it, and switches contexts to the one in that 
thread.

On the other hand, `sem_down` takes a resource from the semaphore. If there 
are no resources left to give, `sem_down` will instead block the current 
thread, and keep track of it in the semaphore queue of waiting threads.

To get around a specific corner case where a thread we will call “C” is called 
which would snatch another thread’s resource, we’ll call “A”  before it is 
able to wake up. Our thread A runs enqueue in the same loop as block, so that 
as long as there are no resources in the semaphore, our thread A will be able 
to be put back into the queue to wait for the resource once again.

## Preemption

Preemption is used to prevent starvation in the system by setting up a timer 
that fires an alarm 100 times a second. This alarm will fire `SIGVTALRM` which 
will yield the thread. 

When `preempt_start` is called, the code sets up `SigAction`, which is what 
handles the signals coming from the alarm. Then, it creates the alarm. Then, 
our alarm catcher will yield when it detects a signal.

However, this alarm is disabled during critical sections of the thread library 
code. Sections that access global structures or add new threads to the system 
are examples of these critical sections in which preemption is temporarily 
disabled.

## Preemption Test Code

Our preemption test code, `test_preempt` has 2 threads. Thread 1 is designed 
to loop forever unless a certain value is changed. Thread 2 is designed to 
change the value used to break the loop in thread 1. The issue is, thread 1 
will never yield in this loop, so without preemption, this code would loop 
forever.

However, with preemption, the code fires a signal in order to break out of 
thread 1 and enter thread 2. Thread 2 changes the value used to lock the 
thread 1 while loop, and then yields to thread 1. Thread 1 can now break 
out of the infinite loop, and the program actually ends.

## Helpful Resources

Help from the project came from the Class Piazza page and Discord server. 
Other resources used were the GNU library and the project2.html page. 
