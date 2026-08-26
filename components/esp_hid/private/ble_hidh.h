/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_hidh.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_GATTC_ENABLE || CONFIG_BT_NIMBLE_ENABLED

esp_err_t esp_ble_hidh_init(const esp_hidh_config_t *config);
esp_err_t esp_ble_hidh_deinit(void);

#if CONFIG_BT_NIMBLE_ENABLED
esp_hidh_dev_t *esp_ble_hidh_dev_open(uint8_t *bda, uint8_t address_type);
#else
esp_hidh_dev_t *esp_ble_hidh_dev_open(esp_bd_addr_t bda, esp_ble_addr_type_t address_type);
#endif

#endif /* CONFIG_GATTC_ENABLE */

#ifdef __cplusplus
}
#endif
