/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for MODEM CLOCK

#pragma once

#include "soc/soc_caps.h"
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
#include "hal/modem_syscon_ll.h"
#include "hal/modem_lpcon_ll.h"
#include "hal/modem_clock_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    modem_syscon_dev_t *syscon_dev;
    modem_lpcon_dev_t  *lpcon_dev;
} modem_clock_hal_context_t;

#if MAC_SUPPORT_PMU_MODEM_STATE
void modem_clock_hal_set_clock_domain_icg_bitmap(modem_clock_hal_context_t *hal, modem_clock_domain_t domain, uint32_t bitmap);
uint32_t modem_clock_hal_get_clock_domain_icg_bitmap(modem_clock_hal_context_t *hal, modem_clock_domain_t domain);
#endif

void modem_clock_hal_enable_modem_adc_common_fe_clock(modem_clock_hal_context_t *hal, bool enable);
void modem_clock_hal_enable_modem_private_fe_clock(modem_clock_hal_context_t *hal, bool enable);

#if SOC_BT_SUPPORTED
void modem_clock_hal_set_ble_rtc_timer_divisor_value(modem_clock_hal_context_t *hal, uint32_t divider);
void modem_clock_hal_enable_ble_rtc_timer_clock(modem_clock_hal_context_t *hal, bool enable);
void modem_clock_hal_select_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src);
void modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal);
#endif

void modem_clock_hal_select_coex_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src);
void modem_clock_hal_deselect_all_coex_lpclk_source(modem_clock_hal_context_t *hal);

#if SOC_WIFI_SUPPORTED
void modem_clock_hal_select_wifi_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src);
void modem_clock_hal_deselect_all_wifi_lpclk_source(modem_clock_hal_context_t *hal);
void modem_clock_hal_enable_wifipwr_clock(modem_clock_hal_context_t *hal, bool enable);
#endif

#ifdef __cplusplus
}
#endif

#endif  // SOC_MODEM_CLOCK_IS_INDEPENDENT
