/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdatomic.h>

typedef struct {
    uint8_t head;
    uint8_t tail;
    atomic_uint_fast8_t used;
} esp_openthread_circular_queue_info_t;
