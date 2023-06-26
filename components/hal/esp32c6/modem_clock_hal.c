/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for MODEM CLOCK (ESP32-C6 specific part)
#include <stdbool.h>
#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/modem_clock_hal.h"
#include "hal/modem_clock_types.h"
#include "hal/assert.h"

typedef enum {
    MODEM_CLOCK_XTAL32K_CODE = 0,
    MODEM_CLOCK_RC32K_CODE   = 1,
    MODEM_CLOCK_EXT32K_CODE  = 2
} modem_clock_32k_clk_src_code_t;

void modem_clock_hal_set_clock_domain_icg_bitmap(modem_clock_hal_context_t *hal, modem_clock_domain_t domain, uint32_t bitmap)
{
    HAL_ASSERT(domain < MODEM_CLOCK_DOMAIN_MAX);
    switch (domain)
    {
    case MODEM_CLOCK_DOMAIN_MODEM_APB:
        modem_syscon_ll_set_modem_apb_icg_bitmap(hal->syscon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_MODEM_PERIPH:
        modem_syscon_ll_set_modem_periph_icg_bitmap(hal->syscon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_WIFI:
        modem_syscon_ll_set_wifi_icg_bitmap(hal->syscon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_BT:
        modem_syscon_ll_set_bt_icg_bitmap(hal->syscon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_FE:
        modem_syscon_ll_set_fe_icg_bitmap(hal->syscon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_IEEE802154:
        modem_syscon_ll_set_ieee802154_icg_bitmap(hal->syscon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_LP_APB:
        modem_lpcon_ll_set_lp_apb_icg_bitmap(hal->lpcon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_I2C_MASTER:
        modem_lpcon_ll_set_i2c_master_icg_bitmap(hal->lpcon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_COEX:
        modem_lpcon_ll_set_coex_icg_bitmap(hal->lpcon_dev, bitmap);
        break;
    case MODEM_CLOCK_DOMAIN_WIFIPWR:
        modem_lpcon_ll_set_wifipwr_icg_bitmap(hal->lpcon_dev, bitmap);
        break;
    default:
        break;
    }
}

void modem_clock_hal_enable_fe_clock(modem_clock_hal_context_t *hal, bool enable)
{
    if (enable) {
        modem_syscon_ll_enable_fe_apb_clock(hal->syscon_dev, enable);
        modem_syscon_ll_enable_fe_cal_160m_clock(hal->syscon_dev, enable);
        modem_syscon_ll_enable_fe_160m_clock(hal->syscon_dev, enable);
        modem_syscon_ll_enable_fe_80m_clock(hal->syscon_dev, enable);
    }
}

void modem_clock_hal_set_ble_rtc_timer_divisor_value(modem_clock_hal_context_t *hal, uint32_t  divider)
{
    modem_lpcon_ll_set_ble_rtc_timer_divisor_value(hal->lpcon_dev, divider);
}

void modem_clock_hal_enable_ble_rtc_timer_clock(modem_clock_hal_context_t *hal, bool enable)
{
    modem_lpcon_ll_enable_ble_rtc_timer_clock(hal->lpcon_dev, enable);
}

void modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal)
{
    modem_lpcon_ll_enable_ble_rtc_timer_slow_osc(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_ble_rtc_timer_fast_osc(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_ble_rtc_timer_32k_xtal(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_ble_rtc_timer_main_xtal(hal->lpcon_dev, false);
}

void modem_clock_hal_select_ble_rtc_timer_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src)
{
    HAL_ASSERT(src < MODEM_CLOCK_LPCLK_SRC_MAX);

    switch (src)
    {
    case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
        modem_lpcon_ll_enable_ble_rtc_timer_slow_osc(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC_FAST:
        modem_lpcon_ll_enable_ble_rtc_timer_fast_osc(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
        modem_lpcon_ll_enable_ble_rtc_timer_main_xtal(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC32K:
        modem_lpcon_ll_enable_ble_rtc_timer_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_RC32K_CODE);
        break;
    case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
        modem_lpcon_ll_enable_ble_rtc_timer_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_XTAL32K_CODE);
        break;
    case MODEM_CLOCK_LPCLK_SRC_EXT32K:
        modem_lpcon_ll_enable_ble_rtc_timer_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_EXT32K_CODE);
        break;
    default:
        break;
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
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_RC32K_CODE);
        break;
    case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
        modem_lpcon_ll_enable_coex_lpclk_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_XTAL32K_CODE);
        break;
    case MODEM_CLOCK_LPCLK_SRC_EXT32K:
        modem_lpcon_ll_enable_coex_lpclk_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_EXT32K_CODE);
        break;
    default:
        break;
    }
}

void modem_clock_hal_deselect_all_wifi_lpclk_source(modem_clock_hal_context_t *hal)
{
    modem_lpcon_ll_enable_wifi_lpclk_slow_osc(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_wifi_lpclk_fast_osc(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_wifi_lpclk_32k_xtal(hal->lpcon_dev, false);
    modem_lpcon_ll_enable_wifi_lpclk_main_xtal(hal->lpcon_dev, false);
}

void modem_clock_hal_select_wifi_lpclk_source(modem_clock_hal_context_t *hal, modem_clock_lpclk_src_t src)
{
    HAL_ASSERT(src < MODEM_CLOCK_LPCLK_SRC_MAX);

    switch (src)
    {
    case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
        modem_lpcon_ll_enable_wifi_lpclk_slow_osc(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC_FAST:
        modem_lpcon_ll_enable_wifi_lpclk_fast_osc(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
        modem_lpcon_ll_enable_wifi_lpclk_main_xtal(hal->lpcon_dev, true);
        break;
    case MODEM_CLOCK_LPCLK_SRC_RC32K:
        modem_lpcon_ll_enable_wifi_lpclk_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_RC32K_CODE);
        break;
    case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
        modem_lpcon_ll_enable_wifi_lpclk_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_XTAL32K_CODE);
        break;
    case MODEM_CLOCK_LPCLK_SRC_EXT32K:
        modem_lpcon_ll_enable_wifi_lpclk_32k_xtal(hal->lpcon_dev, true);
        modem_lpcon_ll_select_modem_32k_clock_source(hal->lpcon_dev, MODEM_CLOCK_EXT32K_CODE);
        break;
    default:
        break;
    }
}
