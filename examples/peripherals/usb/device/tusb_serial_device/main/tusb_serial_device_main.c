/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#if CONFIG_TINYUSB_PM
#include "esp_pm.h"
#include "esp_sleep.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tinyusb_cdc_acm.h"

static const char *TAG = "example";
static uint8_t rx_buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

/**
 * @brief Application Queue
 */
static QueueHandle_t app_queue;
typedef struct {
    uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];     // Data buffer
    size_t buf_len;                                     // Number of bytes received
    uint8_t itf;                                        // Index of CDC device interface
} app_message_t;

/**
 * @brief CDC device RX callback
 *
 * CDC device signals, that new data were received
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, rx_buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {

        app_message_t tx_msg = {
            .buf_len = rx_size,
            .itf = itf,
        };

        memcpy(tx_msg.buf, rx_buf, rx_size);
        xQueueSend(app_queue, &tx_msg, 0);
    } else {
        ESP_LOGE(TAG, "Read Error");
    }
}

/**
 * @brief CDC device line change callback
 *
 * CDC device signals, that the DTR, RTS states changed
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

#if CONFIG_TINYUSB_PM
static void device_event_handler(tinyusb_event_t *event, void *arg)
{
    (void) arg;

    switch (event->id) {
    case TINYUSB_EVENT_SUSPENDED:
        ESP_LOGI(TAG, "USB suspended with remote wakeup %s; automatic light sleep is now allowed",
                 event->suspended.remote_wakeup ? "enabled" : "disabled");
        break;
    case TINYUSB_EVENT_RESUMED:
        ESP_LOGI(TAG, "USB resumed");
        break;
    default:
        break;
    }
}
#endif

void app_main(void)
{
    // Create FreeRTOS primitives
    app_queue = xQueueCreate(5, sizeof(app_message_t));
    assert(app_queue);
    app_message_t msg;

#if CONFIG_TINYUSB_PM
    // Configure Power Management for automatic light sleep
    const esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .light_sleep_enable = true,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_CNNT, ESP_PD_OPTION_ON));

    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG(device_event_handler);
    tusb_cfg.pm_lock_enable = true;
#else
    const tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
#endif

    ESP_LOGI(TAG, "USB initialization");
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .cdc_port = TINYUSB_CDC_ACM_0,
        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tinyusb_cdcacm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));

#if (CONFIG_TINYUSB_CDC_COUNT > 1)
    acm_cfg.cdc_port = TINYUSB_CDC_ACM_1;
    ESP_ERROR_CHECK(tinyusb_cdcacm_init(&acm_cfg));
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_1,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));
#endif

    ESP_LOGI(TAG, "USB initialization DONE");
    while (1) {
        if (xQueueReceive(app_queue, &msg, portMAX_DELAY)) {
            if (msg.buf_len) {

                /* Print received data*/
                ESP_LOGI(TAG, "Data from channel %d:", msg.itf);
                ESP_LOG_BUFFER_HEXDUMP(TAG, msg.buf, msg.buf_len, ESP_LOG_INFO);

                /* write back */
                tinyusb_cdcacm_write_queue(msg.itf, msg.buf, msg.buf_len);
                esp_err_t err = tinyusb_cdcacm_write_flush(msg.itf, 0);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "CDC ACM write flush error: %s", esp_err_to_name(err));
                }
            }
        }
    }
}
