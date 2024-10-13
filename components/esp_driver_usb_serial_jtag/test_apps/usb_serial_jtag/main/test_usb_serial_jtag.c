/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_timer.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "esp_rom_sys.h"

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
    usb_serial_jtag_vfs_use_driver();

    xTaskCreate(test_task_driver2, "usj_print_1", 4096, sem, 10, NULL);
    xTaskCreate(test_task_driver1, "usj_print_2", 4096, sem, 10, NULL);
    for (int i = 0; i < 2; i++) {
        xSemaphoreTake(sem, portMAX_DELAY);
    }

    vSemaphoreDelete(sem);
    vTaskDelay(5);

    usb_serial_jtag_vfs_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
}

/*
It's hard to properly test if fsync() is working as there's not really a way to see if there's still
data dribbling out to the host if it is not. We can use some heuristics, though: fsync() should be
pretty fast when there's no data sent in a while, while it is slower when the USB-JTAG-serial device
still is sending out data. Big issues like e.g. a timeout in the fsync logic, or the fsync logic not
syncing up at all, should be easy to detect like this.
*/
#define FSYNC_DATA_SZ 4096

TEST_CASE("see if fsync appears to work", "[usb_serial_jtag]")
{
    usb_serial_jtag_driver_config_t cfg = {
        .tx_buffer_size = FSYNC_DATA_SZ,
        .rx_buffer_size = 128
    };

    TEST_ESP_OK(usb_serial_jtag_driver_install(&cfg));

    // Tell vfs to use usb-serial-jtag driver
    usb_serial_jtag_vfs_use_driver();

    char *buf = malloc(FSYNC_DATA_SZ);
    //fill with NULL bytes as they won't be printed to the terminal but will be sent over USB
    memset(buf, 0, FSYNC_DATA_SZ);

    //make sure caches are warmed up by doing some dummy stuff
    uint64_t start_us = esp_timer_get_time();
    fsync(0);
    uint64_t end_us = esp_timer_get_time();
    usb_serial_jtag_write_bytes(buf, FSYNC_DATA_SZ, 1);

    vTaskDelay(pdMS_TO_TICKS(200));

    //Measure with USB-serial-JTAG idle
    start_us = esp_timer_get_time();
    fsync(0);
    end_us = esp_timer_get_time();
    printf("No data in queue: %d us\n", (int)(end_us - start_us));
    TEST_ASSERT_LESS_THAN_INT(100, end_us - start_us);

    vTaskDelay(pdMS_TO_TICKS(200));

    //Measure with USB-serial-JTAG active
    usb_serial_jtag_write_bytes(buf, FSYNC_DATA_SZ, portMAX_DELAY);
    start_us = esp_timer_get_time();
    fsync(0);
    end_us = esp_timer_get_time();
    printf("With data in queue: %d us\n", (int)(end_us - start_us));
    TEST_ASSERT_GREATER_THAN_INT(1000, end_us - start_us);
    TEST_ASSERT_LESS_THAN_INT(45000, end_us - start_us); //50ms means fsync hit a timeout

    free(buf);
    usb_serial_jtag_vfs_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
}

TEST_CASE("test rom printf work after driver installed", "[usb_serial_jtag]")
{
    usb_serial_jtag_driver_config_t cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    TEST_ESP_OK(usb_serial_jtag_driver_install(&cfg));

    // Tell vfs to use usb-serial-jtag driver
    usb_serial_jtag_vfs_use_driver();

    esp_rom_printf("hi, espressif1\n");
    printf("hi, espressif2");

    usb_serial_jtag_vfs_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
}
