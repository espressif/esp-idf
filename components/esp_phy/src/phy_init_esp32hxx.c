/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "freertos/portmacro.h"
#include "esp_phy_init.h"
#include "esp_private/phy.h"
#include "esp_timer.h"
#include "esp_private/periph_ctrl.h"
#include "esp_log.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT
#include "esp_private/esp_modem_clock.h"
#endif
#include "phy_init_deps.h"
#include "esp_private/phy_debug.h"
#if SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY
#include "esp_private/sleep_modem.h"
#endif // SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY

#ifndef PHY_INIT_MODEM_CLOCK_REQUIRED_BITS
#warning "PHY_INIT_MODEM_CLOCK_REQUIRED_BITS not defined; using default value 0"
#define PHY_INIT_MODEM_CLOCK_REQUIRED_BITS 0
#endif

static const char* TAG = "phy_init";

static DRAM_ATTR portMUX_TYPE s_phy_int_mux = portMUX_INITIALIZER_UNLOCKED;

extern void phy_version_print(void);
static _lock_t s_phy_access_lock;

/* Reference count of enabling PHY */
static bool s_phy_is_enabled = false;

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
#if CONFIG_ESP_PHY_ENABLE_VERSION_PRINT
            phy_version_print();
#endif
#if CONFIG_ESP_PHY_PLL_TRACK_TEMP_DEBUG
            phy_track_temp_debug(CONFIG_ESP_PHY_PLL_TRACK_TEMP_DEBUG_FLAG, CONFIG_ESP_PHY_PLL_TRACK_TEMP_DELTA);
#endif
            s_phy_is_enabled = true;
        } else {
#if SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY
            if (sleep_modem_phy_link_enabled() && sleep_modem_phy_link_done()) {
                sleep_modem_do_phy_retention(true, false, SLEEP_MODEM_SKIP_I2C_MST_CLK_RETENTION);
            } else
#endif // SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY
            {
                phy_wakeup_init();
            }
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
#if SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY
        if (sleep_modem_phy_link_enabled()) {
            sleep_modem_do_phy_retention(false, false, SLEEP_MODEM_SKIP_I2C_MST_CLK_RETENTION);
        } else
#endif /* SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY */
        {
            phy_close_rf();
            phy_xpd_tsens();
            phy_wait_freq_hw_hop_done();
        }
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

void esp_phy_modem_init(uint8_t modem)
{
#if CONFIG_ESP_PHY_HW_SWITCH_RF
    _lock_acquire(&s_phy_access_lock);
    if (sleep_modem_phy_init(modem) != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize sleep modem phy");
    }
    _lock_release(&s_phy_access_lock);
#endif // CONFIG_ESP_PHY_HW_SWITCH_RF
}

void esp_phy_modem_deinit(uint8_t modem)
{
#if CONFIG_ESP_PHY_HW_SWITCH_RF
    _lock_acquire(&s_phy_access_lock);
    sleep_modem_phy_deinit(modem);
    _lock_release(&s_phy_access_lock);
#endif // CONFIG_ESP_PHY_HW_SWITCH_RF
}
