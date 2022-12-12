/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_eth_driver.h"
#include "eth_phy_regs_struct.h"

static const char *TAG = "esp_eth.phy";

esp_err_t esp_eth_detect_phy_addr(esp_eth_mediator_t *eth, int *detected_addr)
{
    if (!eth || !detected_addr) {
        ESP_LOGE(TAG, "eth and detected_addr can't be null");
        return ESP_ERR_INVALID_ARG;
    }
    int addr_try = 0;
    uint32_t reg_value = 0;
    for (; addr_try < 16; addr_try++) {
        eth->phy_reg_read(eth, addr_try, ETH_PHY_IDR1_REG_ADDR, &reg_value);
        if (reg_value != 0xFFFF && reg_value != 0x00) {
            *detected_addr = addr_try;
            break;
        }
    }
    if (addr_try < 16) {
        ESP_LOGD(TAG, "Found PHY address: %d", addr_try);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "No PHY device detected");
        return ESP_ERR_NOT_FOUND;
    }
}
