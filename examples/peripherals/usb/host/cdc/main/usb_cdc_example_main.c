/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "usb/cdc_host_types.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "usb/vcp_ch34x.h"
#include "usb/vcp_cp210x.h"
#include "usb/vcp_ftdi.h"

#define EXAMPLE_USB_HOST_PRIORITY   (20)
#define EXAMPLE_TX_STRING           ("CDC test string!")
#define EXAMPLE_TX_TIMEOUT_MS       (1000)
#define MAX_CDC_DEVICES             (5)
#define ESPRESSIF_VID               (0x303A) // 0x303A Espressif VID, used in TinyUSB devices or in USB-Serial-JTAG devices
#define APP_QUIT_PIN                CONFIG_APP_QUIT_PIN

static const char *TAG = "USB-CDC";

/** Open CDC device handles; NULL means slot is free. Needed for APP_QUIT (close all) and to clear slot on disconnect. */
static cdc_acm_dev_hdl_t cdc_devices[MAX_CDC_DEVICES] = {0};

/**
 * @brief Application queue and its message IDs
 */
static QueueHandle_t app_queue;
typedef struct {
    enum {
        APP_QUIT,                /**< Request to exit: uninstall CDC driver and USB host lib */
        APP_DEVICE_CONNECTED,    /**< New USB CDC device connected */
        APP_DEVICE_DISCONNECTED, /**< CDC device disconnected */
    } id;
    union {
        struct {
            uint16_t vid;
            uint16_t pid;
        } new_dev;            /**< VID/PID for APP_DEVICE_CONNECTED */
        int device_slot;      /**< Slot for APP_DEVICE_DISCONNECTED */
    } data;
} app_message_t;

/**
 * @brief Find a free slot in the device table.
 */
static inline int find_free_slot(void)
{
    for (int i = 0; i < MAX_CDC_DEVICES; i++) {
        if (cdc_devices[i] == NULL) {
            return i;
        }
    }
    return -1;
}

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
    int slot = (int)arg;
    ESP_LOGI(TAG, "\t- Data received (slot %d)", slot);
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);
    return true;
}

/**
 * @brief Device event callback
 *
 * Forwards disconnection to app queue
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
        if (app_queue) {
            app_message_t msg = {
                .id = APP_DEVICE_DISCONNECTED,
                .data.device_slot = (int)(intptr_t)user_ctx,
            };
            xQueueSend(app_queue, &msg, 0);
        } else {
            ESP_ERROR_CHECK(cdc_acm_host_close(event->data.cdc_hdl));
        }
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
 * @brief New USB device callback
 *
 * Gets VID/PID and posts APP_DEVICE_CONNECTED to app queue.
 * Called from USB Host context; device is closed after this callback returns.
 *
 * @param[in] usb_dev    USB device handle
 */
static void new_dev_cb(usb_device_handle_t usb_dev)
{
    const usb_device_desc_t *device_desc;
    esp_err_t err = usb_host_get_device_descriptor(usb_dev, &device_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "usb_host_get_device_descriptor failed: %s", esp_err_to_name(err));
        return;
    }

    uint16_t vid = device_desc->idVendor;
    uint16_t pid = device_desc->idProduct;
    ESP_LOGI(TAG, "New CDC device connected VID=0x%04X PID=0x%04X", vid, pid);

    if (app_queue) {
        app_message_t msg = {
            .id = APP_DEVICE_CONNECTED,
            .data.new_dev.vid = vid,
            .data.new_dev.pid = pid,
        };
        xQueueSend(app_queue, &msg, 0);
    }
}

/**
 * @brief Open CDC device by VID/PID using the appropriate driver.
 */
static cdc_acm_dev_hdl_t example_cdc_open(uint16_t vid, uint16_t pid,
                                          const cdc_acm_host_device_config_t *dev_config)
{
    cdc_acm_dev_hdl_t cdc_dev = NULL;
    esp_err_t err;

    switch (vid) {
    case FTDI_VID:
        err = ftdi_vcp_open(pid, 0, dev_config, &cdc_dev);
        break;
    case NANJING_QINHENG_MICROE_VID:
        err = ch34x_vcp_open(pid, 0, dev_config, &cdc_dev);
        break;
    case SILICON_LABS_VID:
        err = cp210x_vcp_open(pid, 0, dev_config, &cdc_dev);
        break;
    case ESPRESSIF_VID:
    default:
        err = cdc_acm_host_open(vid, pid, 0, dev_config, &cdc_dev);
        break;
    }

    if (err == ESP_OK) {
        return cdc_dev;
    }

    ESP_LOGE(TAG, "Failed to open device VID=0x%04X PID=0x%04X", vid, pid);
    return NULL;
}

static void free_cdc_device(int slot)
{
    if (slot < 0 || slot >= MAX_CDC_DEVICES || cdc_devices[slot] == NULL) {
        return;
    }
    ESP_LOGI(TAG, "\t- Closing CDC device in slot %d", slot);
    cdc_acm_host_close(cdc_devices[slot]);
    cdc_devices[slot] = NULL;
}

static void free_all_cdc_devices(void)
{
    for (int i = 0; i < MAX_CDC_DEVICES; i++) {
        if (cdc_devices[i] != NULL) {
            free_cdc_device(i);
        }
    }
}

/**
 * @brief Run CDC demo (print descriptor, tx, control line, line coding) for one device.
 */
static void run_cdc_demo(int slot)
{
    cdc_acm_dev_hdl_t cdc_dev = cdc_devices[slot];

    ESP_LOGI(TAG, "CDC device opened (slot %d). Descriptor:", slot);
    cdc_acm_host_desc_print(cdc_dev);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "Testing data transmission");
    ESP_ERROR_CHECK(cdc_acm_host_data_tx_blocking(cdc_dev, (const uint8_t *)EXAMPLE_TX_STRING,
                                                  strlen(EXAMPLE_TX_STRING), EXAMPLE_TX_TIMEOUT_MS));
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "Testing control line state command");
    esp_err_t err = cdc_acm_host_set_control_line_state(cdc_dev, false, false);
    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "\t- Control line state set not supported");
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "\t- Failed to set control line state");
    } else {
        vTaskDelay(pdMS_TO_TICKS(20));
        cdc_acm_host_set_control_line_state(cdc_dev, false, true);
        ESP_LOGI(TAG, "\t- Control line state set to DTR=false, RTS=true");
        vTaskDelay(pdMS_TO_TICKS(20));
        cdc_acm_host_set_control_line_state(cdc_dev, false, false);
    }

    ESP_LOGI(TAG, "Testing line coding commands");
    cdc_acm_line_coding_t line_coding;
    err = cdc_acm_host_line_coding_get(cdc_dev, &line_coding);
    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "\t- Line coding get not supported");
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "\t- Failed to get line coding");
    } else {
        ESP_LOGI(TAG, "\t- Line Coding Get: Rate: %"PRIu32", Stop bits: %"PRIu8", Parity: %"PRIu8", Databits: %"PRIu8,
                 line_coding.dwDTERate, line_coding.bCharFormat, line_coding.bParityType, line_coding.bDataBits);
        line_coding.dwDTERate = 115200;
        line_coding.bDataBits = 8;
        line_coding.bParityType = 0;
        line_coding.bCharFormat = 0;
        err = cdc_acm_host_line_coding_set(cdc_dev, &line_coding);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "\t- Failed to set line coding");
        } else {
            ESP_LOGI(TAG, "\t- Line Set: Rate: %"PRIu32", Stop bits: %"PRIu8", Parity: %"PRIu8", Databits: %"PRIu8,
                     line_coding.dwDTERate, line_coding.bCharFormat, line_coding.bParityType, line_coding.bDataBits);
        }
    }

    ESP_LOGI(TAG, "Example finished for device in slot %d. You can disconnect or connect another device.", slot);
}

/**
 * @brief Boot button pressed callback - send APP_QUIT to front of queue.
 */
static void gpio_cb(void *arg)
{
    BaseType_t xTaskWoken = pdFALSE;
    app_message_t message = {
        .id = APP_QUIT,
    };

    if (app_queue) {
        xQueueSendToFrontFromISR(app_queue, &message, &xTaskWoken);
    }

    if (xTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief USB Host library handling task.
 */
static void usb_lib_task(void *arg)
{
    ESP_LOGI("usb_task", "Running USB task");
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LOWMED,
    };
    ESP_LOGI("usb_task", "\t- Installing USB Host");
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    const cdc_acm_host_driver_config_t driver_config = {
        .driver_task_stack_size = 4096,
        .driver_task_priority = EXAMPLE_USB_HOST_PRIORITY + 1,
        .xCoreID = 0,
        .new_dev_cb = new_dev_cb,
    };
    ESP_LOGI("usb_task", "\t- Installing CDC-ACM driver");
    ESP_ERROR_CHECK(cdc_acm_host_install(&driver_config));

    xTaskNotifyGive(arg); // Notify the main task that the USB host is installed

    bool has_clients = true; // We installed CDC-ACM driver, so we have clients
    while (1) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            has_clients = false;
            if (ESP_OK == usb_host_device_free_all()) {
                break;
            }
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            if (!has_clients) {
                break;
            }
        }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(usb_host_uninstall());
    ESP_LOGI("usb_task", "USB Host task completed");
    vTaskDelete(NULL);
}

void app_main(void)
{
    app_queue = xQueueCreate(10, sizeof(app_message_t));
    assert(app_queue);

    BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, xTaskGetCurrentTaskHandle(),
                                          EXAMPLE_USB_HOST_PRIORITY, NULL);
    assert(task_created == pdTRUE);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    const gpio_config_t input_pin = {
        .pin_bit_mask = BIT64(APP_QUIT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&input_pin));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LOWMED));
    ESP_ERROR_CHECK(gpio_isr_handler_add(APP_QUIT_PIN, gpio_cb, NULL));

    cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 0,
        .out_buffer_size = 512,
        .in_buffer_size = 0,
        .user_arg = NULL,
        .event_cb = handle_event,
        .data_cb = handle_rx
    };

    ESP_LOGI("app_main", "Waiting for CDC devices. Press Boot button to quit.");

    bool running = true;
    while (running) {
        app_message_t msg;
        xQueueReceive(app_queue, &msg, portMAX_DELAY);

        switch (msg.id) {
        case APP_DEVICE_CONNECTED: {
            int slot = find_free_slot();
            if (slot < 0) {
                ESP_LOGW("app_main", "No free slots for new CDC device (max %d)", MAX_CDC_DEVICES);
                continue;
            }

            dev_config.user_arg = (void *)(intptr_t)slot;
            cdc_acm_dev_hdl_t cdc_dev = example_cdc_open(msg.data.new_dev.vid, msg.data.new_dev.pid, &dev_config);
            if (cdc_dev == NULL) {
                continue;
            }

            cdc_devices[slot] = cdc_dev;
            run_cdc_demo(slot);
            break;
        }

        case APP_DEVICE_DISCONNECTED: {
            ESP_LOGI("app_main", "Device disconnected from slot %d", msg.data.device_slot);
            free_cdc_device(msg.data.device_slot);
            break;
        }

        case APP_QUIT: {
            ESP_LOGI("app_main", "Exiting example");
            free_all_cdc_devices();
            ESP_ERROR_CHECK(cdc_acm_host_uninstall());
            running = false;
            break;
        }
        default:
            ESP_LOGW("app_main", "Unknown message ID: %d", msg.id);
            break;
        }
    }

    ESP_LOGI("app_main", "\t- Exit completed");
    gpio_isr_handler_remove(APP_QUIT_PIN);
    gpio_uninstall_isr_service();
    vQueueDelete(app_queue);
}
