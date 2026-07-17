/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
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

// Multi-connection management functions
esp_err_t esp_ble_hidd_dev_set_active_conn(void *devp, uint16_t conn_id);
esp_err_t esp_ble_hidd_dev_get_connections(void *devp, esp_hidd_conn_info_t *conn_list, size_t max_count, size_t *count);
esp_err_t esp_ble_hidd_dev_set_broadcast_mode(void *devp, bool enable);
esp_err_t esp_ble_hidd_dev_get_active_conn(void *devp, uint16_t *conn_id);
esp_err_t esp_ble_hidd_dev_is_broadcast_mode(void *devp, bool *enabled);

#endif /* CONFIG_GATTS_ENABLE */

#ifdef __cplusplus
}
#endif
