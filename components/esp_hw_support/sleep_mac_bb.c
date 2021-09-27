/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"
#include "soc/soc_caps.h"
#include "esp_private/sleep_mac_bb.h"
#include "sdkconfig.h"

#if CONFIG_MAC_BB_PD

#define MAC_BB_POWER_DOWN_CB_NO     (2)
#define MAC_BB_POWER_UP_CB_NO       (2)

static DRAM_ATTR mac_bb_power_down_cb_t s_mac_bb_power_down_cb[MAC_BB_POWER_DOWN_CB_NO];
static DRAM_ATTR mac_bb_power_up_cb_t   s_mac_bb_power_up_cb[MAC_BB_POWER_UP_CB_NO];

esp_err_t esp_register_mac_bb_pd_callback(mac_bb_power_down_cb_t cb)
{
    int index = MAC_BB_POWER_DOWN_CB_NO;
    for (int i = MAC_BB_POWER_DOWN_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_down_cb[i] == cb) {
            return ESP_OK;
        }

        if (s_mac_bb_power_down_cb[i] == NULL) {
            index = i;
        }
    }

    if (index < MAC_BB_POWER_DOWN_CB_NO) {
        s_mac_bb_power_down_cb[index] = cb;
        return ESP_OK;
    }

    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_mac_bb_pd_callback(mac_bb_power_down_cb_t cb)
{
    for (int i = MAC_BB_POWER_DOWN_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_down_cb[i] == cb) {
            s_mac_bb_power_down_cb[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void IRAM_ATTR mac_bb_power_down_cb_execute(void)
{
    for (int i = 0; i < MAC_BB_POWER_DOWN_CB_NO; i++) {
        if (s_mac_bb_power_down_cb[i]) {
            s_mac_bb_power_down_cb[i]();
        }
    }
}

esp_err_t esp_register_mac_bb_pu_callback(mac_bb_power_up_cb_t cb)
{
    int index = MAC_BB_POWER_UP_CB_NO;
    for (int i = MAC_BB_POWER_UP_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_up_cb[i] == cb) {
            return ESP_OK;
        }

        if (s_mac_bb_power_up_cb[i] == NULL) {
            index = i;
        }
    }

    if (index < MAC_BB_POWER_UP_CB_NO) {
        s_mac_bb_power_up_cb[index] = cb;
        return ESP_OK;
    }

    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_mac_bb_pu_callback(mac_bb_power_up_cb_t cb)
{
    for (int i = MAC_BB_POWER_UP_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_up_cb[i] == cb) {
            s_mac_bb_power_up_cb[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void IRAM_ATTR mac_bb_power_up_cb_execute(void)
{
    for (int i = 0; i < MAC_BB_POWER_UP_CB_NO; i++) {
        if (s_mac_bb_power_up_cb[i]) {
            s_mac_bb_power_up_cb[i]();
        }
    }
}

#endif ///CONFIG_MAC_BB_PD
