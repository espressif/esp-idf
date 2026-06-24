/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_wifi_types.h"

#define GLOBAL_OPERATING_CLASS_81 81

#define ESP_USD_SUBSCRIBE_DEFAULT_PARAMS() { \
    .active = false, \
    .ttl = 0, \
    .freq = NAN_USD_DEFAULT_FREQ, \
    .query_period = 0, \
};

#define ESP_USD_PUBLISH_DEFAULT_PARAMS() { \
    .unsolicited = true, \
    .solicited = true, \
    .ttl = 100, \
    .fsd = true, \
    .freq = NAN_USD_DEFAULT_FREQ, \
    .freq_list = NULL, \
};

/* Note: These NaN-USD APIs are intended for internal use only.
 * For application development, use the public APIs provided in:
 * 'wifi_apps/nan_app/include/esp_nan.h'
 */
esp_err_t esp_nan_usd_init(void);

esp_err_t esp_nan_usd_deinit(void);

int esp_nan_usd_publish(const wifi_nan_publish_cfg_t *publish_cfg);

esp_err_t esp_nan_usd_cancel_publish(int publish_id);

int esp_nan_usd_subscribe(const wifi_nan_subscribe_cfg_t *subscribe_cfg);

esp_err_t esp_nan_usd_cancel_subscribe(int subscribe_id);

esp_err_t esp_nan_usd_transmit(int handle, const uint8_t *ssi, uint16_t ssi_len,
                               const uint8_t *peer_addr, uint8_t req_instance_id);

esp_err_t esp_nan_usd_cancel_service(int service_id);
#ifdef __cplusplus
}
#endif
