/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#include "esp_ble_iso_common_api.h"

#include "scan.h"

static SemaphoreHandle_t scan_sem;

/* Controller status latched by gap_event_handler for EXAMPLE_WAIT_API_CHECK. */
static esp_bt_status_t scan_op_status;

#define WAIT_API(_call) EXAMPLE_WAIT_API_CHECK(_call, scan_sem, portMAX_DELAY, scan_op_status)

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK,
    .uncoded_cfg = {
        .scan_type = BLE_SCAN_TYPE_PASSIVE,
        .scan_interval = SCAN_INTERVAL,
        .scan_window = SCAN_WINDOW,
    },
};

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param)
{
    /* Data events (EXT_ADV_REPORT / PA_SYNC_ESTAB / PA_SYNC_LOST) are
     * posted to the iso task by the lib's bluedroid gap.c directly from
     * BTU context, so we only handle BTC-side completion events here. */
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        scan_op_status = param->set_ext_scan_params.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        scan_op_status = param->ext_scan_start.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        scan_op_status = param->ext_scan_stop.status;
        xSemaphoreGive(scan_sem);
        break;

    default:
        break;
    }
}

int app_host_init(void)
{
    esp_err_t err;

    scan_sem = xSemaphoreCreateBinary();
    if (scan_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create scan semaphore");
        return -1;
    }

    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err) {
        ESP_LOGE(TAG, "Failed to register GAP callback, err %d", err);
        vSemaphoreDelete(scan_sem);
        return err;
    }

    return 0;
}

int ext_scan_start(void)
{
    WAIT_API(esp_ble_gap_set_ext_scan_params(&ext_scan_params));
    WAIT_API(esp_ble_gap_start_ext_scan(0, 0));

    ESP_LOGI(TAG, "Scanning for broadcaster...");
    return 0;
}

int ext_scan_stop(void)
{
    WAIT_API(esp_ble_gap_stop_ext_scan());
    return 0;
}

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid)
{
    esp_ble_gap_periodic_adv_sync_params_t params = {
        .filter_policy = 0,
        .sid = sid,
        .addr_type = addr_type,
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
    };

    memcpy(params.addr, addr, sizeof(params.addr));

    /* Fire-and-forget: sync establishment (PERIODIC_ADV_SYNC_ESTAB_EVT) is
     * air-dependent and surfaces asynchronously. */
    return esp_ble_gap_periodic_adv_create_sync(&params);
}
