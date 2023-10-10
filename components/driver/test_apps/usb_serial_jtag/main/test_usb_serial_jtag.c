/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "driver/usb_serial_jtag.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "driver/usb_serial_jtag.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define PRINT_TIMES  (300)
static const char TAG[] = "usb_serial_test";

static void test_task_driver1(void *pvParameters)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)pvParameters;
    for (int i = 0; i < PRINT_TIMES; i++) {
        ESP_LOGI(TAG, "Oh, hello world 1, this test is for testing message and parse in python, time %d", i);
        vTaskDelay(11);
    }
    xSemaphoreGive(sem);
    vTaskDelete(NULL);
}

static void test_task_driver2(void *pvParameters)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)pvParameters;
    for (int i = 0; i < PRINT_TIMES; i++) {
        ESP_LOGI(TAG, "Oh, hello world 2, this test is for testing message and parse in python, time %d", i);
        vTaskDelay(10);
    }
    xSemaphoreGive(sem);
    vTaskDelete(NULL);
}


TEST_CASE("test print via usb_serial_jtag driver multiple times in different tasks", "[usb_serial_jtag]")
{
    usb_serial_jtag_driver_config_t cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    SemaphoreHandle_t sem = xSemaphoreCreateCounting(2, 0);

    TEST_ESP_OK(usb_serial_jtag_driver_install(&cfg));

    // Tell vfs to use usb-serial-jtag driver
    esp_vfs_usb_serial_jtag_use_driver();

    xTaskCreate(test_task_driver2, "usj_print_1", 4096, sem, 10, NULL);
    xTaskCreate(test_task_driver1, "usj_print_2", 4096, sem, 10, NULL);
    for (int i = 0; i < 2; i++) {
        xSemaphoreTake(sem, portMAX_DELAY);
    }

    vSemaphoreDelete(sem);
    vTaskDelay(5);

    esp_vfs_usb_serial_jtag_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
}
