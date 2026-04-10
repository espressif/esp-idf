/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for MODEM CLOCK (ESP32-H2 specific part)
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "hal/modem_clock_hal.h"
#include "hal/lp_clkrst_ll.h"
#include "hal/modem_clock_types.h"
#include "hal/assert.h"

void IRAM_ATTR modem_clock_hal_enable_modem_common_fe_clock(modem_clock_hal_context_t *hal, bool enable)
{
    modem_syscon_ll_enable_fe_apb_clock(hal->syscon_dev, enable);
    modem_syscon_ll_enable_fe_32m_clock(hal->syscon_dev, enable);
}

bool IRAM_ATTR modem_clock_hal_modem_common_fe_clock_is_enabled(modem_clock_hal_context_t *hal)
{
    return modem_syscon_ll_fe_apb_clock_is_enabled(hal->syscon_dev) &&
           modem_syscon_ll_fe_32m_clock_is_enabled(hal->syscon_dev);
}

void IRAM_ATTR modem_clock_hal_enable_modem_private_fe_clock(modem_clock_hal_context_t *hal, bool enable)
{
    modem_lpcon_ll_enable_fe_mem_clock(hal->lpcon_dev, enable);
    modem_syscon_ll_enable_fe_sdm_clock(hal->syscon_dev, enable);
    modem_syscon_ll_enable_fe_adc_clock(hal->syscon_dev, enable);
    modem_syscon_ll_enable_fe_16m_clock(hal->syscon_dev, enable);
}

bool IRAM_ATTR modem_clock_hal_modem_private_fe_clock_is_enabled(modem_clock_hal_context_t *hal)
{
    return modem_lpcon_ll_fe_mem_clock_is_enabled(hal->lpcon_dev) &&
           modem_syscon_ll_fe_sdm_clock_is_enabled(hal->syscon_dev) &&
           modem_syscon_ll_fe_adc_clock_is_enabled(hal->syscon_dev) &&
           modem_syscon_ll_fe_16m_clock_is_enabled(hal->syscon_dev);
}

void modem_clock_hal_set_ble_rtc_timer_divisor_value(modem_clock_hal_context_t *hal, uint32_t divider)
{
    lp_clkrst_ll_set_ble_rtc_timer_divisor_value(&LP_CLKRST, divider);
}

void modem_clock_hal_enable_ble_rtc_timer_clock(modem_clock_hal_context_t *hal, bool enable)
{
    // No clock gate on ESP32-H2
}

void modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal)
{
    lp_clkrst_ll_enable_ble_rtc_timer_slow_osc(&LP_CLKRST, false);
    lp_clkrst_ll_enable_ble_rtc_timer_fast_osc(&LP_CLKRST, false);
    lp_clkrst_ll_enable_ble_rtc_timer_main_xtal(&LP_CLKRST, false);
    lp_clkrst_ll_enable_ble_rtc_timer_32k_xtal(&LP_CLKRST, false);
}

void modem_clock_hal_select_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src)
{
    HAL_ASSERT(src < MODEM_CLOCK_LPCLK_SRC_MAX);

    switch (src)
    {
    case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
        lp_clkrst_ll_enable_ble_rtc_timer_slow_osc(&LP_CLKRST, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC_FAST:
        lp_clkrst_ll_enable_ble_rtc_timer_fast_osc(&LP_CLKRST, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
        lp_clkrst_ll_enable_ble_rtc_timer_main_xtal(&LP_CLKRST, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC32K:
        lp_clkrst_ll_enable_ble_rtc_timer_32k_xtal(&LP_CLKRST, true);
        lp_clkrst_ll_select_modem_32k_clock_source(&LP_CLKRST, MODEM_CLOCK_32K_SRC_RC32K);
        break;
    case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
        lp_clkrst_ll_enable_ble_rtc_timer_32k_xtal(&LP_CLKRST, true);
        lp_clkrst_ll_select_modem_32k_clock_source(&LP_CLKRST, MODEM_CLOCK_32K_SRC_XTAL32K);
        break;
    case MODEM_CLOCK_LPCLK_SRC_EXT32K:
        lp_clkrst_ll_enable_ble_rtc_timer_32k_xtal(&LP_CLKRST, true);
        lp_clkrst_ll_select_modem_32k_clock_source(&LP_CLKRST, MODEM_CLOCK_32K_SRC_EXT32K);
        break;
    default:
        HAL_ASSERT(0);
    }
}

void modem_clock_hal_deselect_all_coex_lpclk_source(modem_clock_hal_context_t *hal)
{
    modem_lpcon_ll_enable_coex_lpclk_slow_osc(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_coex_lpclk_fast_osc(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_coex_lpclk_32k_xtal(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_coex_lpclk_main_xtal(hal->lpcon_dev, false);
}

void modem_clock_hal_select_coex_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src)
{
    HAL_ASSERT(src < MODEM_CLOCK_LPCLK_SRC_MAX);

    switch (src)
    {
    case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
        modem_lpcon_ll_enable_coex_lpclk_slow_osc(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC_FAST:
        modem_lpcon_ll_enable_coex_lpclk_fast_osc(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
        modem_lpcon_ll_enable_coex_lpclk_main_xtal(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC32K:
        modem_lpcon_ll_enable_coex_lpclk_32k_xtal(hal->lpcon_dev, true);
        lp_clkrst_ll_select_modem_32k_clock_source(&LP_CLKRST, MODEM_CLOCK_32K_SRC_RC32K);
        break;
    case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
        modem_lpcon_ll_enable_coex_lpclk_32k_xtal(hal->lpcon_dev, true);
        lp_clkrst_ll_select_modem_32k_clock_source(&LP_CLKRST, MODEM_CLOCK_32K_SRC_XTAL32K);
        break;
    case MODEM_CLOCK_LPCLK_SRC_EXT32K:
        modem_lpcon_ll_enable_coex_lpclk_32k_xtal(hal->lpcon_dev, true);
        lp_clkrst_ll_select_modem_32k_clock_source(&LP_CLKRST, MODEM_CLOCK_32K_SRC_EXT32K);
        break;
    default:
        HAL_ASSERT(0);
    }
}
