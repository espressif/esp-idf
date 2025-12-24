/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ----------------------------------- */
/* BLE Log - Timestamp Synchronization */
/* ----------------------------------- */

/* INCLUDE */
#include "ble_log_ts.h"

/* VARIABLE */
BLE_LOG_STATIC bool ts_inited = false;
BLE_LOG_STATIC ble_log_ts_info_t *ts_info = NULL;

/* INTERFACE */
bool ble_log_ts_init(void)
{
    if (ts_inited) {
        return true;
    }

    /* Initialize toggle IO */
    gpio_config_t sync_io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT(CONFIG_BLE_LOG_SYNC_IO_NUM),
    };
    if (gpio_config(&sync_io_conf) != ESP_OK) {
        goto exit;
    }

    /* Initialize sync data */
    ts_info = (ble_log_ts_info_t *)BLE_LOG_MALLOC(sizeof(ble_log_ts_info_t));
    if (!ts_info) {
        goto exit;
    }
    BLE_LOG_MEMSET(ts_info, 0, sizeof(ble_log_ts_info_t));
    ts_info->int_src_code = BLE_LOG_INT_SRC_TS;

    ts_inited = true;
    return true;

exit:
    ble_log_ts_deinit();
    return false;
}

void ble_log_ts_deinit(void)
{
    ts_inited = false;

    /* Release sync data */
    if (ts_info) {
        BLE_LOG_FREE(ts_info);
        ts_info = NULL;
    }

    /* Release toggle IO */
    gpio_reset_pin(CONFIG_BLE_LOG_SYNC_IO_NUM);
}

void ble_log_ts_info_update(ble_log_ts_info_t **info)
{
    if (!ts_inited) {
        return;
    }

    BLE_LOG_ENTER_CRITICAL();
    ts_info->io_level = !ts_info->io_level;
    gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, ts_info->io_level);
    ts_info->lc_ts = BLE_LOG_GET_LC_TS;
    ts_info->esp_ts = esp_timer_get_time();
    ts_info->os_ts = pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    BLE_LOG_EXIT_CRITICAL();

    *info = ts_info;
}

void ble_log_ts_reset(bool status)
{
    if (!ts_inited) {
        return;
    }

    if (!status && !ts_info->io_level) {
        gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, 1);
    }
    ts_info->io_level = 0;
    gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, 0);
}
