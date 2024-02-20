/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_hidd.h"
#include "esp_err.h"
#include "esp_hid_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_GATTS_ENABLE || CONFIG_BT_NIMBLE_ENABLED

esp_err_t esp_ble_hidd_dev_init(esp_hidd_dev_t *dev, const esp_hid_device_config_t *config, esp_event_handler_t callback);

#endif /* CONFIG_GATTS_ENABLE */

#ifdef __cplusplus
}
#endif
