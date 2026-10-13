/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/gpio_types.h"

/* LP timer wakes the coprocessor after HP has entered deep sleep.
 * ulp_lp_core_run() does not keep the LP core running across sleep entry. */
#define LP_SLEEP_TIMER_US 500000

void lp_sleep_keep_pin(gpio_num_t pin);
void lp_sleep_run(const uint8_t *bin_start, const uint8_t *bin_end, uint32_t wakeup_source, bool load_binary);
void hp_core_deep_sleep_enter(void);
void lp_sleep_expect_ulp_wakeup(void);
