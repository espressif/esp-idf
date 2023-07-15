/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_timer.h"
#include <stdint.h>


extern void bt_track_pll_cap(void);
static esp_timer_handle_t phy_track_pll_timer;
static volatile int64_t s_previous_timestamp;
#define PHY_TRACK_PLL_PERIOD_IN_US 1000000

static void phy_track_pll_timer_callback(void* arg)
{
#if IEEE802154_ENABLED || BT_ENABLED
    bt_track_pll_cap();
#endif
    s_previous_timestamp = esp_timer_get_time();
}

void phy_track_pll_init(void)
{
    // Light sleep scenario: enabling and disabling PHY frequently, the timer will not get triggered.
    // Using a variable to record the previously tracked time when PLL was last called.
    // If the duration is larger than PHY_TRACK_PLL_PERIOD_IN_US, then track PLL.
    int64_t now = esp_timer_get_time();
    if (now - s_previous_timestamp > PHY_TRACK_PLL_PERIOD_IN_US) {
        phy_track_pll_timer_callback((void* )0);
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
