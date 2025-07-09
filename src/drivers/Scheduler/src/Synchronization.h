//
// Created by juri on 09.07.25.
//

#ifndef SCHEDULER_SYNCHRONIZATION_H
#define SCHEDULER_SYNCHRONIZATION_H

#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/atomic.h>

#define RING_BUFFER_SIZE (1024 * 7)
extern struct ring_buf log_ring_buffer; // Ring buffer for logging

extern atomic_t latest_imu_measurements[7];   // Atomic array to hold the latest measurements
extern atomic_t latest_motor_measurements[1]; // Atomic array for motor measurements
extern atomic_t latest_state[3];              // Atomic array to hold the latest state estimates
extern atomic_t latest_reference[3];          // Atomic array to hold the latest reference values
extern atomic_t latest_input[1];              // Atomic array to hold the latest input values

#endif // SCHEDULER_SYNCHRONIZATION_H
