/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "esp_rom_serial_output.h"
#include "esp_sleep.h"
#include "esp_private/usb_phy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "tinyusb.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_default_config.h"

#if !SOC_PM_SUPPORT_USB_WAKEUP || !SOC_PM_SUPPORT_CNNT_PD
#error This example requires a target with SOC_PM_SUPPORT_USB_WAKEUP.
#endif

static const char *TAG = "tusb_cdc_acm_wakeup";
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
    (void) event;

    size_t rx_size = 0;

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

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;

    ESP_LOGI(TAG, "USB suspended, entering light sleep");
    usb_phy_set_otg_suspend_state(true);

    esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    esp_err_t err = esp_light_sleep_start();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Light sleep rejected: %s", esp_err_to_name(err));
    } else {
        uint32_t causes = esp_sleep_get_wakeup_causes();
        if (causes & BIT(ESP_SLEEP_WAKEUP_UNDEFINED)) {
            ESP_LOGW(TAG, "Woke up from an unknown source: 0x%lx", causes);
        } else if (causes & BIT(ESP_SLEEP_WAKEUP_USB)) {
            ESP_LOGI(TAG, "Woke up from: USB");
        }
    }
    usb_phy_set_otg_suspend_state(false);
    usb_phy_clear_otg_wakeup_status();
}

void tud_resume_cb(void)
{
    ESP_LOGI(TAG, "USB resumed");
}

void tud_reset_cb(void)
{
    ESP_LOGI(TAG, "USB reset");
}

void app_main(void)
{
    // Create FreeRTOS primitives
    app_queue = xQueueCreate(5, sizeof(app_message_t));
    assert(app_queue);
    app_message_t msg;

    ESP_ERROR_CHECK(esp_sleep_cpu_retention_init());
    ESP_ERROR_CHECK(esp_sleep_enable_usb_wakeup());
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_CNNT, ESP_PD_OPTION_ON));

    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
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

    ESP_LOGI(TAG, "USB initialization DONE");
    while (1) {
        if (xQueueReceive(app_queue, &msg, portMAX_DELAY)) {
            if (msg.buf_len) {
                /* Print received data */
                ESP_LOGI(TAG, "Data from channel %d:", msg.itf);
                ESP_LOG_BUFFER_HEXDUMP(TAG, msg.buf, msg.buf_len, ESP_LOG_INFO);

                /* Write back */
                tinyusb_cdcacm_write_queue(msg.itf, msg.buf, msg.buf_len);
                esp_err_t err = tinyusb_cdcacm_write_flush(msg.itf, 0);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "CDC ACM write flush error: %s", esp_err_to_name(err));
                }
            }
        }
    }
}
