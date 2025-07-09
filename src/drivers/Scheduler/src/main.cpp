#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/atomic.h>
#include <autoconf.h>
#include "IMU.h"

#define ESTIMATOR_STACK_SIZE       2000
#define CONTROLLER_STACK_SIZE      2000
#define LOGGER_STACK_SIZE          2000
#define ESTIMATOR_THREAD_PRIORITY  5
#define CONTROLLER_THREAD_PRIORITY 3
#define LOGGER_THREAD_PRIORITY     10
#define RING_BUFFER_SIZE           1024 * 7

// Thread
K_THREAD_STACK_DEFINE(estimator_thread_stack_area, ESTIMATOR_STACK_SIZE);
K_THREAD_STACK_DEFINE(controller_thread_stack_area, CONTROLLER_STACK_SIZE);
K_THREAD_STACK_DEFINE(logger_thread_stack_area, LOGGER_STACK_SIZE);

struct k_thread estimator_thread, controller_thread, logger_thread;
void estimatorThreadFunction(void *p1, void *p2, void *p3);
void controllerThreadFunction(void *p1, void *p2, void *p3);
void loggerThreadFunction(void *p1, void *p2, void *p3);

// Timer
// struct k_timer timer;
void estimatorCallbackFunction(struct k_timer *timer_id);
void controllerCallbackFunction(struct k_timer *timer_id);
void loggerCallbackFunction(struct k_timer *timer_id);

K_TIMER_DEFINE(estimator_timer, estimatorCallbackFunction, NULL);
K_TIMER_DEFINE(controller_timer, controllerCallbackFunction, NULL);
K_TIMER_DEFINE(logger_timer, loggerCallbackFunction, NULL);

// Ring buffer (Data item mode)
RING_BUF_ITEM_DECLARE(log_ring_buffer, RING_BUFFER_SIZE);

atomic_t latest_measurements[7]; // Atomic array to hold the latest measurements

// Latest measurements

int main(void)
{
	/* Initialize Thread */
	k_tid_t estimator_thread_tid = k_thread_create(
		&estimator_thread, estimator_thread_stack_area,
		K_THREAD_STACK_SIZEOF(estimator_thread_stack_area), estimatorThreadFunction, NULL,
		NULL, NULL, ESTIMATOR_THREAD_PRIORITY, 0, K_FOREVER);
	k_tid_t controller_thread_tid = k_thread_create(
		&controller_thread, controller_thread_stack_area,
		K_THREAD_STACK_SIZEOF(controller_thread_stack_area), controllerThreadFunction, NULL,
		NULL, NULL, CONTROLLER_THREAD_PRIORITY, 0, K_FOREVER);
	k_tid_t logger_thread_tid = k_thread_create(&logger_thread, logger_thread_stack_area,
						    K_THREAD_STACK_SIZEOF(logger_thread_stack_area),
						    loggerThreadFunction, NULL, NULL, NULL,
						    LOGGER_THREAD_PRIORITY, 0, K_FOREVER);

	k_thread_name_set(&estimator_thread, "estimator_thread");
	k_thread_name_set(&controller_thread, "controller_thread");
	k_thread_name_set(&logger_thread, "logger_thread");

	k_thread_start(&estimator_thread);
	k_thread_start(&controller_thread);
	k_thread_start(&logger_thread);

	/* Initialize Timer */
	k_timer_start(&estimator_timer, K_NO_WAIT, K_MSEC(5));
	k_timer_start(&controller_timer, K_NO_WAIT, K_MSEC(50));
	k_timer_start(&logger_timer, K_NO_WAIT, K_MSEC(100));
}

void estimatorThreadFunction(void *p1, void *p2, void *p3)
{
	imu::IMU imu;
	std::array<uint32_t, 7> m{0};
	uint16_t type = 0x01;
	uint8_t value = 0x01;

	while (true) {
		imu.getMeasurements(m);
		ring_buf_item_put(&log_ring_buffer, type, value, reinterpret_cast<uint32_t *>(&m),
				  7);
		for (int i = 0; i < 7; ++i) {
			atomic_set(&latest_measurements[i], m[i]);
		}

		k_thread_suspend(&estimator_thread);
	}
}

void controllerThreadFunction(void *p1, void *p2, void *p3)
{
	// Placeholder for controller logic
	int32_t state[7];
	while (true) {
		// Simulate some controller work
		for (int i = 0; i < 7; ++i) {
			state[i] = (int32_t)atomic_get(&latest_measurements[i]);
		}
		printk("Measurements: [%d, %d, %d, %d, %d, %d, %d]\n", state[0], state[1], state[2],
		       state[3], state[4], state[5], state[6]);
		k_thread_suspend(&controller_thread);
	}
}

void loggerThreadFunction(void *p1, void *p2, void *p3)
{
	uint16_t type;
	uint8_t value;
	uint32_t data[7];
	uint8_t size;

	while (true) {
		// Wait for the timer to expire
		k_thread_suspend(&logger_thread);
		//		printk("%d\n", ring_buf_is_empty(&log_ring_buffer));
		//		printk("ring buffer free space: %d\n",
		// ring_buf_item_space_get(&log_ring_buffer));

		while (!ring_buf_is_empty(&log_ring_buffer)) {
			// Get the next item from the ring buffer
			if (ring_buf_item_get(&log_ring_buffer, &type, &value, data, &size) < 0) {
				printk("Failed to get item from ring buffer\n");
				continue;
			}
			if (type == 0x01) {
				double time = double((int32_t)data[0]) / 1000.0;
				double ax = double((int32_t)data[1]) / 1000000.0,
				       ay = double((int32_t)data[2]) / 1000000.0,
				       az = double((int32_t)data[3]) / 1000000.0;
				double gx = double((int32_t)data[4]) / 1000000.0,
				       gy = double((int32_t)data[5]) / 1000000.0,
				       gz = double((int32_t)data[6]) / 1000000.0;

				printk("time: [%0.6f], "
				       "imu: [%0.6f, %0.6f, %0.6f, %0.6f, %0.6f, %0.6f]\n",
				       time, ax, ay, az, gx, gy, gz);
			} else {
				/* Dump data without formatting*/
				for (int i = 0; i < size; ++i) {
					printk("%d ", data[i]);
				}
				printk("\n");
			}
		}
	}
}

void estimatorCallbackFunction(struct k_timer *timer_id)
{
	k_thread_resume(&estimator_thread);
}

void controllerCallbackFunction(struct k_timer *timer_id)
{
	k_thread_resume(&controller_thread);
}

void loggerCallbackFunction(struct k_timer *timer_id)
{
	k_thread_resume(&logger_thread);
}
