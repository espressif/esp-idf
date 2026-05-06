/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define TAG "EXAMPLE_INIT"

static SemaphoreHandle_t example_iso_sem;

void ble_store_config_init(void);

static void example_iso_on_reset(int reason)
{
    ESP_LOGI(TAG, "Resetting state; reason=%d", reason);
}

static void example_iso_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    xSemaphoreGive(example_iso_sem);
}

static void example_iso_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");

    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

esp_err_t bluetooth_init(void)
{
    esp_err_t ret;

    example_iso_sem = xSemaphoreCreateBinary();
    if (example_iso_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create iso semaphore");
        return ESP_FAIL;
    }

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d ", ret);
        vSemaphoreDelete(example_iso_sem);
        example_iso_sem = NULL;
        return ret;
    }

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.reset_cb = example_iso_on_reset;
    ble_hs_cfg.sync_cb = example_iso_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.gatts_register_cb = NULL;
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.sm_mitm = 0;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(example_iso_host_task);

    xSemaphoreTake(example_iso_sem, portMAX_DELAY);

    return ESP_OK;
}
