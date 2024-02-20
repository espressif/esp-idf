/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_timer.h"
#include "esp_phy_init.h"
#include "esp_private/phy.h"
#include <stdint.h>

static volatile uint16_t s_phy_modem_flag = 0;

extern void phy_param_track_tot(bool en_wifi, bool en_ble_154);
static esp_timer_handle_t phy_track_pll_timer;
#if CONFIG_ESP_WIFI_ENABLED
static volatile int64_t s_wifi_prev_timestamp;
#endif
#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
static volatile int64_t s_bt_154_prev_timestamp;
#endif
#define PHY_TRACK_PLL_PERIOD_IN_US 1000000

#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED || CONFIG_ESP_WIFI_ENABLED
bool phy_enabled_modem_contains(esp_phy_modem_t modem)
{
    return (s_phy_modem_flag & modem) != 0;
}
#endif

static void phy_track_pll(void)
{
    bool wifi_track_pll = false;
    bool ble_154_track_pll = false;
#if CONFIG_ESP_WIFI_ENABLED
    if (phy_enabled_modem_contains(PHY_MODEM_WIFI)) {
        wifi_track_pll = true;
        s_wifi_prev_timestamp = esp_timer_get_time();
    }
#endif

#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
    if (phy_enabled_modem_contains(PHY_MODEM_BT | PHY_MODEM_IEEE802154)) {
        ble_154_track_pll = true;
        s_bt_154_prev_timestamp = esp_timer_get_time();
    }
#endif
    if (wifi_track_pll || ble_154_track_pll) {
        phy_param_track_tot(wifi_track_pll, ble_154_track_pll);
    }
}

static void phy_track_pll_timer_callback(void* arg)
{
    _lock_t phy_lock = phy_get_lock();
    _lock_acquire(&phy_lock);
    phy_track_pll();
    _lock_release(&phy_lock);
}

void phy_track_pll_init(void)
{
    // Light sleep scenario: enabling and disabling PHY frequently, the timer will not get triggered.
    // Using a variable to record the previously tracked time when PLL was last called.
    // If the duration is larger than PHY_TRACK_PLL_PERIOD_IN_US, then track PLL.
    bool need_track_pll = false;
#if CONFIG_ESP_WIFI_ENABLED
    need_track_pll = need_track_pll || ((esp_timer_get_time() - s_wifi_prev_timestamp) > PHY_TRACK_PLL_PERIOD_IN_US);
#endif
#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
    need_track_pll = need_track_pll || ((esp_timer_get_time() - s_bt_154_prev_timestamp) > PHY_TRACK_PLL_PERIOD_IN_US);
#endif
    if (need_track_pll) {
        phy_track_pll();
    }

    const esp_timer_create_args_t phy_track_pll_timer_args = {
            .callback = &phy_track_pll_timer_callback,
            .name = "phy-track-pll-timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&phy_track_pll_timer_args, &phy_track_pll_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(phy_track_pll_timer, PHY_TRACK_PLL_PERIOD_IN_US));
}

void phy_track_pll_deinit(void)
{
    ESP_ERROR_CHECK(esp_timer_stop(phy_track_pll_timer));
    ESP_ERROR_CHECK(esp_timer_delete(phy_track_pll_timer));
}

void phy_set_modem_flag(esp_phy_modem_t modem)
{
    s_phy_modem_flag |= modem;
}

void phy_clr_modem_flag(esp_phy_modem_t modem)
{
    s_phy_modem_flag &= ~modem;
}

esp_phy_modem_t phy_get_modem_flag(void)
{
    return s_phy_modem_flag;
}
