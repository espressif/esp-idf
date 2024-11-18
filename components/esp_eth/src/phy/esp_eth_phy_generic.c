/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth.h"
#include "esp_eth_phy_802_3.h"

static const char *TAG = "eth_phy_generic";

// Default reset timing is intentionally conservative
#define DEFAULT_PHY_GENERIC_RESET_ASSERTION_TIME_US 10000
#define DEFAULT_PHY_GENERIC_POST_RESET_DELAY_MS     500

esp_eth_phy_t *esp_eth_phy_new_generic(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_802_3_t *phy_802_3 = calloc(1, sizeof(phy_802_3_t));
    eth_phy_config_t phy_802_3_config = *config;
    // default chip specific configuration if not defined by user
    if (config->hw_reset_assert_time_us == 0) {
        phy_802_3_config.hw_reset_assert_time_us = DEFAULT_PHY_GENERIC_RESET_ASSERTION_TIME_US;
    }
    if (config->post_hw_reset_delay_ms == 0) {
        phy_802_3_config.post_hw_reset_delay_ms = DEFAULT_PHY_GENERIC_POST_RESET_DELAY_MS;
    }
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(phy_802_3, &phy_802_3_config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    return &phy_802_3->parent;
err:
    return ret;
}
