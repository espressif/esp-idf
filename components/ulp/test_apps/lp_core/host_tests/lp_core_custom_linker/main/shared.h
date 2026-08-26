/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

typedef struct {
    volatile uint32_t marker;
    volatile uint32_t lp_heartbeat;
} custom_linker_shared_t;
