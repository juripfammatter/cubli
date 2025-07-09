//
// Created by juri on 09.07.25.
//
#include "Synchronization.h"

// Ring buffer (Data item mode)
RING_BUF_ITEM_DECLARE(log_ring_buffer, RING_BUFFER_SIZE);

atomic_t latest_measurements[7]; // Atomic array to hold the latest measurements
atomic_t latest_state[3];
