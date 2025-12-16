/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"

#define EXAMPLE_USB_HOST_PRIORITY   (20)
#define EXAMPLE_TX_STRING           ("CDC test string!")
#define EXAMPLE_TX_TIMEOUT_MS       (1000)

static const char *TAG = "USB-CDC";
static SemaphoreHandle_t device_disconnected_sem;

/**
 * @brief Data received callback
 *
 * @param[in] data     Pointer to received data
 * @param[in] data_len Length of received data in bytes
 * @param[in] arg      Argument we passed to the device open function
 * @return
 *   true:  We have processed the received data
 *   false: We expect more data
 */
static bool handle_rx(const uint8_t *data, size_t data_len, void *arg)
{
    ESP_LOGI(TAG, "Data received");
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);
    return true;
}

/**
 * @brief Device event callback
 *
 * Apart from handling device disconnection it doesn't do anything useful
 *
 * @param[in] event    Device event type and data
 * @param[in] user_ctx Argument we passed to the device open function
 */
static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
{
    switch (event->type) {
    case CDC_ACM_HOST_ERROR:
        ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %i", event->data.error);
        break;
    case CDC_ACM_HOST_DEVICE_DISCONNECTED:
        ESP_LOGI(TAG, "Device suddenly disconnected");
        ESP_ERROR_CHECK(cdc_acm_host_close(event->data.cdc_hdl));
        xSemaphoreGive(device_disconnected_sem);
        break;
    case CDC_ACM_HOST_SERIAL_STATE:
        ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
        break;
    default:
        ESP_LOGW(TAG, "Unsupported CDC event: %d (possibly suspend/resume)", event->type);
        break;
    }
}

/**
 * @brief USB Host library handling task
 *
 * @param arg Unused
 */
static void usb_lib_task(void *arg)
{
    while (1) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            ESP_ERROR_CHECK(usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            ESP_LOGI(TAG, "USB: All devices freed");
            // Continue handling USB events to allow device reconnection
        }
    }
}

/**
 * @brief Main application
 *
 * Here we open a USB CDC device and send some data to it
 */
void app_main(void)
{
    device_disconnected_sem = xSemaphoreCreateBinary();
    assert(device_disconnected_sem);

    // Install USB Host driver. Should only be called once in entire application
    ESP_LOGI(TAG, "Installing USB Host");
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LOWMED,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // Create a task that will handle USB library events
    BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, EXAMPLE_USB_HOST_PRIORITY, NULL);
    assert(task_created == pdTRUE);

    ESP_LOGI(TAG, "Installing CDC-ACM driver");
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

    const cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = 512,
        .in_buffer_size = 512,
        .user_arg = NULL,
        .event_cb = handle_event,
        .data_cb = handle_rx
    };

    while (true) {
        cdc_acm_dev_hdl_t cdc_dev = NULL;

        // Open any CDC-ACM device
        // This call always opens the first CDC-ACM device and first interface it finds
        ESP_LOGI(TAG, "Opening CDC ACM device...");
        esp_err_t err = cdc_acm_host_open(CDC_HOST_ANY_VID, CDC_HOST_ANY_PID, 0, &dev_config, &cdc_dev);
        if (ESP_OK != err) {
            ESP_LOGI(TAG, "Failed to open device");
            vTaskDelay(pdMS_TO_TICKS(1000)); // To prevent infinite loop of failed attempts to open the device
            continue;
        }
        cdc_acm_host_desc_print(cdc_dev);
        vTaskDelay(pdMS_TO_TICKS(100));

        // Test sending and receiving: responses are handled in handle_rx callback
        ESP_ERROR_CHECK(cdc_acm_host_data_tx_blocking(cdc_dev, (const uint8_t *)EXAMPLE_TX_STRING, strlen(EXAMPLE_TX_STRING), EXAMPLE_TX_TIMEOUT_MS));
        vTaskDelay(pdMS_TO_TICKS(100));

        ESP_LOGI(TAG, "Testing control line state command");
        err = cdc_acm_host_set_control_line_state(cdc_dev, false, false);
        if (ESP_ERR_NOT_SUPPORTED == err) {
            ESP_LOGW(TAG, "\t- Control line state set not supported");
        } else if (ESP_OK != err) {
            ESP_LOGE(TAG, "\t- Failed to set control line state");
        } else {
            // In case you connected ESP board with USB-Serial-JTAG, this procedure will reset the board
            vTaskDelay(pdMS_TO_TICKS(20));
            cdc_acm_host_set_control_line_state(cdc_dev, false, true);
            ESP_LOGI(TAG, "\t- Control line state set to DTR=false, RTS=true");
            vTaskDelay(pdMS_TO_TICKS(20));
            cdc_acm_host_set_control_line_state(cdc_dev, false, false);
        }

        // Test Line Coding commands: Get current line coding and change it to 115200 8N1
        ESP_LOGI(TAG, "Testing line coding commands");

        cdc_acm_line_coding_t line_coding;
        err = cdc_acm_host_line_coding_get(cdc_dev, &line_coding);
        if (ESP_ERR_NOT_SUPPORTED == err) {
            ESP_LOGW(TAG, "\t- Line coding get not supported");
        } else if (ESP_OK != err) {
            ESP_LOGE(TAG, "\t- Failed to get line coding");
        } else {
            ESP_LOGI(TAG, "\t- Line Coding Get: Rate: %"PRIu32", Stop bits: %"PRIu8", Parity: %"PRIu8", Databits: %"PRIu8"",
                     line_coding.dwDTERate, line_coding.bCharFormat, line_coding.bParityType, line_coding.bDataBits);

            // Set new line coding
            line_coding.dwDTERate = 115200;
            line_coding.bDataBits = 8;
            line_coding.bParityType = 0;
            line_coding.bCharFormat = 0;
            err = cdc_acm_host_line_coding_set(cdc_dev, &line_coding);
            if (ESP_OK != err) {
                ESP_LOGE(TAG, "\t- Failed to set line coding");
            } else {
                ESP_LOGI(TAG, "\t- Line Set: Rate: %"PRIu32", Stop bits: %"PRIu8", Parity: %"PRIu8", Databits: %"PRIu8"",
                         line_coding.dwDTERate, line_coding.bCharFormat, line_coding.bParityType, line_coding.bDataBits);
            }
        }

        // We are done. Wait for device disconnection and start over
        ESP_LOGI(TAG, "Example finished successfully! You can reconnect the device to run again.");
        xSemaphoreTake(device_disconnected_sem, portMAX_DELAY);
    }
}
