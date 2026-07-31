/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_phy_init.h"
#include "esp_timer.h"

#include "esp_private/phy.h"
#include "esp_private/phy_debug.h"

#if CONFIG_ESP_PHY_RECORD_USED_TIME

#define ESP_PHY_MODEM_COUNT_MAX         (__builtin_ffs(PHY_MODEM_MAX - 1))
#define ESP_PHY_IS_VALID_MODEM(modem)   (__builtin_popcount(modem) == 1 && __builtin_ctz(modem) < ESP_PHY_MODEM_COUNT_MAX)

static DRAM_ATTR struct {
    uint64_t used_time;
    uint64_t enabled_time;
    uint64_t disabled_time;
} s_phy_rf_used_info[ESP_PHY_MODEM_COUNT_MAX];

void IRAM_ATTR phy_record_time(bool enabled, esp_phy_modem_t modem)
{
    uint8_t index = __builtin_ctz(modem);

    if (enabled) {
        s_phy_rf_used_info[index].enabled_time = esp_timer_get_time();
    } else {
        s_phy_rf_used_info[index].disabled_time = esp_timer_get_time();
        s_phy_rf_used_info[index].used_time += s_phy_rf_used_info[index].disabled_time - s_phy_rf_used_info[index].enabled_time;
    }
}

esp_err_t phy_query_used_time(uint64_t *used_time, esp_phy_modem_t modem)
{
    if (!ESP_PHY_IS_VALID_MODEM(modem)) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t index = __builtin_ctz(modem);
    _lock_t phy_lock = phy_get_lock();
    _lock_acquire(&phy_lock);
    *used_time = s_phy_rf_used_info[index].used_time;
    if (s_phy_rf_used_info[index].disabled_time < s_phy_rf_used_info[index].enabled_time) {
        *used_time += esp_timer_get_time() - s_phy_rf_used_info[index].enabled_time;
    }
    _lock_release(&phy_lock);
    return ESP_OK;
}

esp_err_t phy_clear_used_time(esp_phy_modem_t modem)
{
    if (!ESP_PHY_IS_VALID_MODEM(modem)) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t index = __builtin_ctz(modem);
    _lock_t phy_lock = phy_get_lock();
    _lock_acquire(&phy_lock);
    if (s_phy_rf_used_info[index].enabled_time > s_phy_rf_used_info[index].disabled_time) {
        s_phy_rf_used_info[index].enabled_time = esp_timer_get_time();
    } else {
        s_phy_rf_used_info[index].enabled_time = s_phy_rf_used_info[index].disabled_time;
    }
    s_phy_rf_used_info[index].used_time = 0;
    _lock_release(&phy_lock);
    return ESP_OK;
}

#endif
