/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

/* Extra wait inside LP main. The LP timer already fires after HP has started
 * deep sleep; this covers the tail of sleep entry before the pin is touched. */
#define LP_SLEEP_SETTLE_US          100000

/* Releasing a pad hold when HP wakes can produce one level change, so only a
 * pulse train proves that LP drove the pin while HP was asleep. */
#define LP_SLEEP_GPIO_PULSES        3
#define LP_SLEEP_GPIO_PULSE_US      100000
#define LP_SLEEP_GPIO_WAIT_US       3000000
