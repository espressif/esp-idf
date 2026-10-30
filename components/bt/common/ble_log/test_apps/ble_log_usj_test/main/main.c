/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ble_log.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "ble_log_usj_test"

#define SMOKE_PAYLOAD_LEN 8
#define SMOKE_INTERVAL_MS 100

#if CONFIG_BLE_LOG_USJ_TEST_PERF_MODE
#define PERF_PAYLOAD_LEN CONFIG_BLE_LOG_USJ_TEST_PERF_PAYLOAD_LEN
#define PERF_DURATION_S  CONFIG_BLE_LOG_USJ_TEST_PERF_DURATION_S
#endif

static uint32_t s_seq;

#if !CONFIG_BLE_LOG_USJ_TEST_PERF_MODE
static void smoke_payload_fill(uint8_t *buf, size_t len)
{
    buf[0] = (uint8_t)(s_seq);
    buf[1] = (uint8_t)(s_seq >> 8);
    buf[2] = (uint8_t)(s_seq >> 16);
    buf[3] = (uint8_t)(s_seq >> 24);
    buf[4] = 0xA5;
    buf[5] = 0x5A;
    for (size_t i = 6; i < len; i++) {
        buf[i] = (uint8_t)i;
    }
}

void app_main(void)
{
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "ble_log_init failed");
        return;
    }
    if (!ble_log_enable(true)) {
        ESP_LOGE(TAG, "ble_log_enable failed");
        return;
    }

    ESP_LOGI(TAG, "USJ log running; open the ROM CDC-ACM port (VID 0x303A PID 0x1001)");
    while (1) {
        uint8_t payload[SMOKE_PAYLOAD_LEN];
        smoke_payload_fill(payload, sizeof(payload));
        (void)ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload));
        s_seq++;
        vTaskDelay(pdMS_TO_TICKS(SMOKE_INTERVAL_MS));
    }
}
#else
static uint32_t s_written;
static uint32_t s_rejected;

void app_main(void)
{
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "ble_log_init failed");
        return;
    }
    if (!ble_log_enable(true)) {
        ESP_LOGE(TAG, "ble_log_enable failed");
        return;
    }

    ESP_LOGI(TAG,
             "perf: payload %d bytes, duration %d s; open the port and read",
             PERF_PAYLOAD_LEN, PERF_DURATION_S);

    const TickType_t deadline = xTaskGetTickCount() +
                                pdMS_TO_TICKS(PERF_DURATION_S * 1000);
    while (xTaskGetTickCount() < deadline) {
        uint8_t payload[PERF_PAYLOAD_LEN];
        s_seq++;
        payload[0] = (uint8_t)(s_seq);
        payload[1] = (uint8_t)(s_seq >> 8);
        payload[2] = (uint8_t)(s_seq >> 16);
        payload[3] = (uint8_t)(s_seq >> 24);
        payload[4] = 0xA5;
        payload[5] = 0x5A;
        for (int i = 6; i < PERF_PAYLOAD_LEN; i++) {
            payload[i] = (uint8_t)i;
        }
        if (ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload))) {
            s_written++;
        } else {
            s_rejected++;
        }
    }

    ESP_LOGI(TAG, "perf: %lu frames accepted, %lu rejected in %d s",
             (unsigned long)s_written, (unsigned long)s_rejected,
             PERF_DURATION_S);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
