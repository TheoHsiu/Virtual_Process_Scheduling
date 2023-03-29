#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
sigset_t block_VT;
struct sigaction sigAlarm, oldAct;
struct itimerval alarm, oldTimer;

void alarm_catcher(int sig)
{
	if(sig) {
		preempt_disable();
		uthread_yield();
	}
		
}

void preempt_disable(void)
{
	sigaddset(&block_VT, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &block_VT, NULL);
}

void preempt_enable(void)
{
	sigaddset(&block_VT, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &block_VT, NULL);
}

void preempt_start(bool preempt)
{
	if(preempt) {
		// Set Up SigAction
		sigAlarm.sa_handler = alarm_catcher;
		sigemptyset(&sigAlarm.sa_mask);
		sigAlarm.sa_flags = 0;
		sigaction(SIGVTALRM, &sigAlarm, &oldAct);

		//Set Up Alarm
		alarm.it_value.tv_sec = 0;
		alarm.it_value.tv_usec = 1000000/HZ;
		alarm.it_interval.tv_sec = 0;
		alarm.it_interval.tv_usec = 1000000/HZ;
		setitimer(ITIMER_VIRTUAL, &alarm, &oldTimer);
	}	
}

void preempt_stop(void)
{
	sigaction(SIGVTALRM, &oldAct, NULL);
	setitimer(ITIMER_VIRTUAL, &oldTimer, NULL);
	preempt_disable();
}
