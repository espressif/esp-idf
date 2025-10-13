/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_SVC_GAP_TAG "ble_svc_gap"

/**
 * Stub implementations for GAP service APIs.
 * These are compiled when CONFIG_BT_NIMBLE_GAP_SERVICE is disabled.
 */

static inline void ble_svc_gap_init(void)
{
    ESP_LOGE(BLE_SVC_GAP_TAG, "GAP service not enabled. Enable CONFIG_BT_NIMBLE_GAP_SERVICE to use this API.");
}

static inline int ble_svc_gap_device_name_set(const char *name)
{
    (void)name;
    ESP_LOGE(BLE_SVC_GAP_TAG, "GAP service not enabled. Enable CONFIG_BT_NIMBLE_GAP_SERVICE to use this API.");
    return -1;
}

static inline const char *ble_svc_gap_device_name(void)
{
    ESP_LOGE(BLE_SVC_GAP_TAG, "GAP service not enabled. Enable CONFIG_BT_NIMBLE_GAP_SERVICE to use this API.");
    return NULL;
}

static inline int ble_svc_gap_device_appearance_set(uint16_t appearance)
{
    ESP_LOGE(BLE_SVC_GAP_TAG, "GAP service not enabled. Enable CONFIG_BT_NIMBLE_GAP_SERVICE to use this API.");
    return -1;
}

static inline int ble_svc_gap_device_key_material_set(uint8_t *session_key, uint8_t *iv)
{
    ESP_LOGE(BLE_SVC_GAP_TAG, "GAP service not enabled. Enable CONFIG_BT_NIMBLE_GAP_SERVICE to use this API.");
    return -1;
}

#ifdef __cplusplus
}
#endif
