/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"

#if SOC_USB_OTG_SUPPORTED

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_sig_map.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tusb.h"
#include "mock_dev.h"

#define DEVICE_MOUNT_TIMEOUT_MS         5000
#define DEVICE_TASK_SIZE                4096
#define DEVICE_TASK_PRIO                5
#define DEVICE_TASK_AFFINITY            0x7FFFFFFF
#define DEVICE_MAX_DESC_BUF_SIZE        32

#if (CONFIG_IDF_TARGET_ESP32P4)
#define USB_SRP_BVALID_IN_IDX       USB_SRP_BVALID_PAD_IN_IDX
#endif // CONFIG_IDF_TARGET_ESP32P4

//
// Test configuration
//

static SemaphoreHandle_t mock_device_test_stop = NULL;
static TaskHandle_t mock_device_task_hdl = NULL;
static mock_dev_cfg_t *mock_dev_cfg;

//
// Device connect/disconnect emulation
//
static void mock_dev_insert(void)
{
    if (mock_dev_cfg && mock_dev_cfg->conn_dconn.enabled) {
        // HIGH to emulate connect USB device
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_SRP_BVALID_IN_IDX, false);
        vTaskDelay(pdMS_TO_TICKS(mock_dev_cfg->conn_dconn.delay_ms));
    }
}

static void mock_dev_remove(void)
{
    if (mock_dev_cfg && mock_dev_cfg->conn_dconn.enabled) {
        // LOW to emulate disconnect USB device
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_SRP_BVALID_IN_IDX, false);
        vTaskDelay(pdMS_TO_TICKS(mock_dev_cfg->conn_dconn.delay_ms));
    }
}

//
// TinyUSB driver related logic
//
#ifndef tusb_teardown
#warning "Teardown feature is not available, please use tinyUSB component with tusb_teardown() support"
#endif // tusb_teardown

static void mock_device_task(void *arg)
{
    // RTOS forever loop
    while (1) {
        tud_task();
    }
}

bool mock_dev_setup(mock_dev_cfg_t *cfg)
{
    // Create semaphore
    mock_device_test_stop = xSemaphoreCreateBinary();
    if (mock_device_test_stop == NULL) {
        return false;
    }
    // Init tusb device stack
    if (!tusb_init()) {
        return false;
    }
    // Create task for device stack handling
    xTaskCreatePinnedToCore(mock_device_task,
                            "TinyUSB Device task",
                            DEVICE_TASK_SIZE,
                            NULL,
                            DEVICE_TASK_PRIO,
                            &mock_device_task_hdl,
                            DEVICE_TASK_AFFINITY);
    if (mock_device_task_hdl == NULL) {
        return false;
    }
    // Configure mock USB device
    mock_dev_cfg = cfg;
    mock_dev_cfg->enumeration.curr_stage = MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC;

    // Insert mock USB device
    mock_dev_insert();
    return true;
}

esp_err_t mock_dev_wait_flag(void)
{
    if (!mock_device_test_stop) {
        return ESP_ERR_INVALID_STATE;
    }

    return (xSemaphoreTake(mock_device_test_stop, pdMS_TO_TICKS(DEVICE_MOUNT_TIMEOUT_MS))
            ? ESP_OK
            : ESP_ERR_TIMEOUT);
}

void mock_dev_release(void)
{
    // Short delay to allow device to show up
    vTaskDelay(10);
    // Remove mock USB device
    mock_dev_remove();
    // Uninstall TinyUSB driver
    tusb_teardown();

    // Delete device handling task
    if (mock_device_task_hdl) {
        vTaskDelete(mock_device_task_hdl);
        mock_device_task_hdl = NULL;
    }
    // Delete semaphore
    if (mock_device_test_stop) {
        vSemaphoreDelete(mock_device_test_stop);
    }

    mock_dev_cfg = NULL;
}

//
// TinyUSB callbacks
//

// Basic
uint8_t const *tud_descriptor_device_cb(void)
{
    printf("\t <- Device Descriptor request\n");
    mock_dev_stage_data_t *stage_config = &mock_dev_cfg->enumeration.stage_config[mock_dev_cfg->enumeration.curr_stage];
    if (stage_config->stop_test) {
        xSemaphoreGive(mock_device_test_stop);
    }
    mock_dev_cfg->enumeration.curr_stage++;
    return stage_config->desc;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    printf("\t <- Configuration Descriptor request, index=%d\n", index);
    mock_dev_stage_data_t *stage_config = &mock_dev_cfg->enumeration.stage_config[mock_dev_cfg->enumeration.curr_stage];
    if (stage_config->stop_test) {
        xSemaphoreGive(mock_device_test_stop);
    }
    mock_dev_cfg->enumeration.curr_stage++;
    return stage_config->desc;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    uint8_t chr_count;
    static uint16_t _desc_str[DEVICE_MAX_DESC_BUF_SIZE];

    printf("\t <- String Descriptor request, index=%d, langid=%#x\n", index, langid);

    mock_dev_stage_data_t *stage_config = &mock_dev_cfg->enumeration.stage_config[mock_dev_cfg->enumeration.curr_stage];
    if (stage_config->stop_test) {
        xSemaphoreGive(mock_device_test_stop);
    }
    mock_dev_cfg->enumeration.curr_stage++;

    if (stage_config->str_desc == NULL) {
        // Move to Set configuration stage
        mock_dev_cfg->enumeration.curr_stage = MOCK_DEV_STAGE_SET_CONFIG;
        return NULL;
    }

    if (index == 0) {
        memcpy(&_desc_str[1], stage_config->str_desc, 2);
        chr_count = 1;
    } else {
        const char *str = stage_config->str_desc;
        chr_count = strnlen(str, DEVICE_MAX_DESC_BUF_SIZE - 1); // Buffer len - header

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // First byte is length in bytes (including header), second byte is descriptor type (TUSB_DESC_STRING)
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}

// Advanced
uint8_t const * tud_descriptor_bos_cb(void)
{
    printf("\t <- BOS Descriptor request\n");
    return NULL;
}

#if (TUD_OPT_HIGH_SPEED)
uint8_t const *tud_descriptor_device_qualifier_cb(void)
{
    return NULL;
}

uint8_t const *tud_descriptor_other_speed_configuration_cb(uint8_t index)
{
    return NULL;
}
#endif // TUD_OPT_HIGH_SPEED

void tud_mount_cb(void)
{
    printf("\t <- Device enabled\n");
    if (mock_device_test_stop) {
        xSemaphoreGive(mock_device_test_stop);
    }
}

void tud_umount_cb(void)
{
    printf("\t <- Device disabled\n");
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    printf("\t <- Device suspend\n");
}

void tud_resume_cb(void)
{
    printf("\t <- Device resume\n");
}

#endif // SOC_USB_OTG_SUPPORTED
