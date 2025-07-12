//
// Created by juri on 09.07.25.
//
#include "Synchronization.h"

// Ring buffer (Data item mode)
RING_BUF_ITEM_DECLARE(log_ring_buffer, RING_BUFFER_SIZE);

atomic_t latest_imu_measurements[7];
atomic_t latest_motor_measurements[1];
atomic_t latest_state[4];
atomic_t latest_reference[4];
atomic_t latest_input[1];
