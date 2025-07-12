#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/atomic.h>
#include <autoconf.h>
#include "IMU.h"
#include "Estimator.h"
#include "Controller.h"
#include "Synchronization.h"
#include "Roller485.h"

#include "../lib/eigen/Eigen/Core"

using Eigen::VectorXf;

#define ESTIMATOR_STACK_SIZE       2000
#define CONTROLLER_STACK_SIZE      2000
#define LOGGER_STACK_SIZE          2000
#define ESTIMATOR_THREAD_PRIORITY  5
#define CONTROLLER_THREAD_PRIORITY 3
#define LOGGER_THREAD_PRIORITY     10

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

int main(void)
{
	/* Initialize Thread */
	k_thread_create(&estimator_thread, estimator_thread_stack_area,
			K_THREAD_STACK_SIZEOF(estimator_thread_stack_area), estimatorThreadFunction,
			NULL, NULL, NULL, ESTIMATOR_THREAD_PRIORITY, 0, K_FOREVER);
	k_thread_create(&controller_thread, controller_thread_stack_area,
			K_THREAD_STACK_SIZEOF(controller_thread_stack_area),
			controllerThreadFunction, NULL, NULL, NULL, CONTROLLER_THREAD_PRIORITY, 0,
			K_FOREVER);
	k_thread_create(&logger_thread, logger_thread_stack_area,
			K_THREAD_STACK_SIZEOF(logger_thread_stack_area), loggerThreadFunction, NULL,
			NULL, NULL, LOGGER_THREAD_PRIORITY, 0, K_FOREVER);

	k_thread_name_set(&estimator_thread, "estimator_thread");
	k_thread_name_set(&controller_thread, "controller_thread");
	k_thread_name_set(&logger_thread, "logger_thread");

	k_thread_start(&estimator_thread);
	k_thread_start(&controller_thread);
	k_thread_start(&logger_thread);

	/* Initialize Timer */
	k_timer_start(&estimator_timer, K_NO_WAIT, K_MSEC(5));
	k_timer_start(&controller_timer, K_NO_WAIT, K_MSEC(10));
	k_timer_start(&logger_timer, K_NO_WAIT, K_MSEC(100));
}

void estimatorThreadFunction(void *p1, void *p2, void *p3)
{
	imu::IMU imu;
	estimator::Estimator estimator;
	std::array<uint32_t, 7> m_imu{0};
	std::array<uint32_t, 8> m{0};
	std::array<uint32_t, 15> log_data{0};
	uint16_t type = 0x01; // measurements
	uint8_t value = 0x01;

	uint32_t x1, x2, x3, r1, r2, r3, u1;

	while (true) {

		imu.getMeasurements(m_imu);
		for (int i = 0; i < 7; ++i) {
			atomic_set(&latest_imu_measurements[i], m[i]);
		}

		uint32_t motor_speed = atomic_get(&latest_motor_measurements[0]);
		m = {m_imu[0], m_imu[1], m_imu[2], m_imu[3],
		     m_imu[4], m_imu[5], m_imu[6], motor_speed};

		estimator.estimateState(m);

		x1 = atomic_get(&latest_state[0]);
		x2 = atomic_get(&latest_state[1]);
		x3 = atomic_get(&latest_state[2]);
		r1 = atomic_get(&latest_reference[0]);
		r2 = atomic_get(&latest_reference[1]);
		r3 = atomic_get(&latest_reference[2]);
		u1 = atomic_get(&latest_input[0]);

		log_data = {m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7],
			    x1,   x2,   x3,   r1,   r2,   r3,   u1};

		ring_buf_item_put(&log_ring_buffer, type, value,
				  reinterpret_cast<uint32_t *>(&log_data), 15);

		k_thread_suspend(&estimator_thread);
	}
}

void controllerThreadFunction(void *p1, void *p2, void *p3)
{
	uint8_t i2c_address = 0x64;
	controller::Controller controller(i2c_address);

	int32_t motor_speed{};
	VectorXf reference(1);
	VectorXf state(1);
	VectorXf input(1);

	reference << 500.0f;
	uint32_t counter = 0;

	auto rpm2rads = [](float rpm) -> float { return (rpm * 2.0f * 3.14f) / 60.0f; };

	while (true) {
		if (counter++ == 250) {
			reference << 0.0f;
		}
		k_thread_suspend(&controller_thread);
		state << controller.motor.getSpeed();
		motor_speed = (int32_t)(rpm2rads(state[0]) * 1000000.0f);
		atomic_set(&latest_motor_measurements[0], motor_speed);

		controller.compute_input(reference, state, input);
		atomic_set(&latest_reference[0], (int32_t)(rpm2rads(reference[0]) * 1000000.0f));
		atomic_set(&latest_input[0], (int32_t)(rpm2rads(input[0]) * 1000000.0f));

		controller.motor.setSpeed(input[0]);
	}
}

void loggerThreadFunction(void *p1, void *p2, void *p3)
{
	uint16_t type;
	uint8_t value;
	uint32_t data[15];
	uint8_t size;

	while (true) {
		k_thread_suspend(&logger_thread);

		while (!ring_buf_is_empty(&log_ring_buffer)) {
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
				double motor_speed = double((int32_t)data[7]) / 1000000.0;
				double x1 = double((int32_t)data[8]) / 1000000.0,
				       x2 = double((int32_t)data[9]) / 1000000.0,
				       x3 = double((int32_t)data[10]) / 1000000.0;
				double r1 = double((int32_t)data[11]) / 1000000.0,
				       r2 = double((int32_t)data[12]) / 1000000.0,
				       r3 = double((int32_t)data[13]) / 1000000.0;
				double u1 = double((int32_t)data[14]) / 1000000.0;

				printk("time: [%0.6f], "
				       "imu: [%0.6f, %0.6f, %0.6f, %0.6f, %0.6f, %0.6f, %0.6f], "
				       "state: [%0.6f, %0.6f, %0.6f], "
				       "reference: [%0.6f, %0.6f, %0.6f], "
				       "input: [%0.6f]\n",
				       time, ax, ay, az, gx, gy, gz, motor_speed, x1, x2, x3, r1,
				       r2, r3, u1);
			} else {
				// Dump data without formatting
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
