#include <cstdio>
#include <zephyr/kernel.h>
#include <autoconf.h>

#define STACK_SIZE      500
#define THREAD_PRIORITY 5

// Semaphore
struct k_sem sem;

// Thread
K_THREAD_STACK_DEFINE(thread_stack_area, STACK_SIZE);
struct k_thread thread;
void threadFunction(void *p1, void *p2, void *p3);

// Timer
// struct k_timer timer;
void timerCallbackFunction(struct k_timer *timer_id);
K_TIMER_DEFINE(timer, timerCallbackFunction, NULL);

int main(void)
{
	/* Initialize Thread */
	k_tid_t thread_tid = k_thread_create(
		&thread, thread_stack_area, K_THREAD_STACK_SIZEOF(thread_stack_area),
		threadFunction, NULL, NULL, NULL, THREAD_PRIORITY, 0, K_FOREVER);
	k_thread_name_set(&thread, "test_thread");
	k_thread_start(&thread); // start after naming thread

	/* Initialize Timer */
	k_timer_start(&timer, K_NO_WAIT, K_MSEC(1000));
}

void threadFunction(void *p1, void *p2, void *p3)
{
	while (true) {
		printf("Thread running\n");
		k_sleep(K_MSEC(100));
		k_thread_suspend(&thread);
	}
}

void timerCallbackFunction(struct k_timer *timer_id)
{
	k_thread_resume(&thread);
}
