/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "freertos/portmacro.h"
#include "esp_phy_init.h"
#include "esp_private/phy.h"
#include "esp_timer.h"
#include "esp_private/periph_ctrl.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT
#include "esp_private/esp_modem_clock.h"
#endif
#include "phy_init_deps.h"

#ifndef PHY_INIT_MODEM_CLOCK_REQUIRED_BITS
#warning "PHY_INIT_MODEM_CLOCK_REQUIRED_BITS not defined; using default value 0"
#define PHY_INIT_MODEM_CLOCK_REQUIRED_BITS 0
#endif

#define PHY_ENABLE_VERSION_PRINT 1

static DRAM_ATTR portMUX_TYPE s_phy_int_mux = portMUX_INITIALIZER_UNLOCKED;

extern void phy_version_print(void);
static _lock_t s_phy_access_lock;

/* Reference count of enabling PHY */
static bool s_phy_is_enabled = false;

#if CONFIG_ESP_PHY_RECORD_USED_TIME
#define ESP_PHY_MODEM_COUNT_MAX         (__builtin_ffs(PHY_MODEM_MAX - 1))
#define ESP_PHY_IS_VALID_MODEM(modem)   (__builtin_popcount(modem) == 1 && __builtin_ctz(modem) < ESP_PHY_MODEM_COUNT_MAX)

static DRAM_ATTR struct {
    uint64_t used_time;
    uint64_t enabled_time;
    uint64_t disabled_time;
} s_phy_rf_used_info[ESP_PHY_MODEM_COUNT_MAX];

static IRAM_ATTR void phy_record_time(bool enabled, esp_phy_modem_t modem) {
    uint8_t index = __builtin_ctz(modem);
    if (enabled) {
        s_phy_rf_used_info[index].enabled_time = esp_timer_get_time();
    } else {
        s_phy_rf_used_info[index].disabled_time = esp_timer_get_time();
        s_phy_rf_used_info[index].used_time += s_phy_rf_used_info[index].disabled_time - s_phy_rf_used_info[index].enabled_time;
    }
}

esp_err_t phy_query_used_time(uint64_t *used_time, esp_phy_modem_t modem) {
    if (!ESP_PHY_IS_VALID_MODEM(modem)) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t index = __builtin_ctz(modem);
    _lock_acquire(&s_phy_access_lock);
    *used_time = s_phy_rf_used_info[index].used_time;
    if (s_phy_rf_used_info[index].disabled_time < s_phy_rf_used_info[index].enabled_time) {
        // phy is being used
        *used_time += esp_timer_get_time() - s_phy_rf_used_info[index].enabled_time;
    }
    _lock_release(&s_phy_access_lock);
    return ESP_OK;
}

esp_err_t phy_clear_used_time(esp_phy_modem_t modem) {
    if (!ESP_PHY_IS_VALID_MODEM(modem)) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t index = __builtin_ctz(modem);
    _lock_acquire(&s_phy_access_lock);
    if (s_phy_rf_used_info[index].enabled_time > s_phy_rf_used_info[index].disabled_time) {
        // phy is being used
        s_phy_rf_used_info[index].enabled_time = esp_timer_get_time();
    } else {
        s_phy_rf_used_info[index].enabled_time = s_phy_rf_used_info[index].disabled_time;
    }
    s_phy_rf_used_info[index].used_time = 0;
    _lock_release(&s_phy_access_lock);
    return ESP_OK;
}
#endif

uint32_t IRAM_ATTR phy_enter_critical(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&s_phy_int_mux);

    } else {
        portENTER_CRITICAL(&s_phy_int_mux);
    }
    // Interrupt level will be stored in current tcb, so always return zero.
    return 0;
}

void IRAM_ATTR phy_exit_critical(uint32_t level)
{
    // Param level don't need any more, ignore it.
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&s_phy_int_mux);
    } else {
        portEXIT_CRITICAL(&s_phy_int_mux);
    }
}

void esp_phy_enable(esp_phy_modem_t modem)
{
    _lock_acquire(&s_phy_access_lock);
    if (phy_get_modem_flag() == 0) {
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
        modem_clock_module_enable(PERIPH_PHY_MODULE);
#endif
        phy_module_enable();
        assert(phy_module_has_clock_bits(PHY_INIT_MODEM_CLOCK_REQUIRED_BITS));
        if (!s_phy_is_enabled) {
            register_chipv7_phy(NULL, NULL, PHY_RF_CAL_FULL);
            phy_version_print();
            s_phy_is_enabled = true;
        } else {
            phy_wakeup_init();
        }
#if !CONFIG_ESP_PHY_DISABLE_PLL_TRACK
        phy_track_pll_init();
#endif
        phy_module_disable();
    }
    phy_set_modem_flag(modem);
    // Immediately track pll when phy enabled.
#if !CONFIG_ESP_PHY_DISABLE_PLL_TRACK
    phy_track_pll();
#endif
#if CONFIG_ESP_PHY_RECORD_USED_TIME
    phy_record_time(true, modem);
#endif
    _lock_release(&s_phy_access_lock);
}

void esp_phy_disable(esp_phy_modem_t modem)
{
    _lock_acquire(&s_phy_access_lock);
#if CONFIG_ESP_PHY_RECORD_USED_TIME
    phy_record_time(false, modem);
#endif
    esp_phy_modem_t saved_modem = phy_get_modem_flag();
    phy_clr_modem_flag(modem);
    if (saved_modem == modem) {

#if !CONFIG_ESP_PHY_DISABLE_PLL_TRACK
        phy_track_pll_deinit();
#endif
        phy_close_rf();
        phy_xpd_tsens();
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
        modem_clock_module_disable(PERIPH_PHY_MODULE);
#endif
    }
    _lock_release(&s_phy_access_lock);
}

_lock_t phy_get_lock(void)
{
    return s_phy_access_lock;
}
