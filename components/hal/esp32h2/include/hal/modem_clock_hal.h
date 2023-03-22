/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for MODEM CLOCK

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/modem_syscon_ll.h"
#include "hal/modem_lpcon_ll.h"
#include "hal/modem_clock_types.h"

typedef struct {
    modem_syscon_dev_t *syscon_dev;
    modem_lpcon_dev_t  *lpcon_dev;
} modem_clock_hal_context_t;

void modem_clock_hal_set_clock_domain_icg_bitmap(modem_clock_hal_context_t *hal, modem_clock_domain_t domain, uint32_t bitmap);

void modem_clock_hal_enable_fe_clock(modem_clock_hal_context_t *hal, bool enable);

void modem_clock_hal_set_ble_rtc_timer_divisor_value(modem_clock_hal_context_t *hal, uint32_t  divider);

void modem_clock_hal_enable_ble_rtc_timer_clock(modem_clock_hal_context_t *hal, bool enable);

void modem_clock_hal_select_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src);

void modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal);

void modem_clock_hal_select_coex_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src);

void modem_clock_hal_deselect_all_coex_lpclk_source(modem_clock_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
