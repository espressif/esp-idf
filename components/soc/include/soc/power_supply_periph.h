/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t irq;
} power_supply_signal_conn_t;

extern const power_supply_signal_conn_t power_supply_periph_signal;

#ifdef __cplusplus
}
#endif
