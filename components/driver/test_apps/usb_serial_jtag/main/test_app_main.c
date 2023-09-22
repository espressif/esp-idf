/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"

#include "driver/usb_serial_jtag.h" // for the usb serial/jtag driver access
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_usb_serial_jtag.h" // for the redirection of stdio

#define BUF_SIZE (256)
#define TEST_MEMORY_LEAK_THRESHOLD (0)

static size_t before_free_8bit;
static size_t before_free_32bit;

void setUp(void)
{
    // Install USB-SERIAL-JTAG driver for interrupt-driven reads and writes
    usb_serial_jtag_driver_config_t cfg = {
        .tx_buffer_size = BUF_SIZE,
        .rx_buffer_size = BUF_SIZE,
    };

    esp_err_t ret = usb_serial_jtag_driver_install(&cfg);
    TEST_ASSERT(ret == ESP_ERR_INVALID_STATE ||
                ret == ESP_OK); // ignore initialized when restarting

    // Tell vfs to use usb-serial-jtag driver
    esp_vfs_usb_serial_jtag_use_driver();

    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("\n");
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    // _____         _     _   _   _    ____ _____
    //|_   _|__  ___| |_  | | | | / \  |  _ \_   _|
    //  | |/ _ \/ __| __| | | | |/ _ \ | |_) || |
    //  | |  __/\__ \ |_  | |_| / ___ \|  _ < | |
    //  |_|\___||___/\__|  \___/_/   \_\_| \_\|_|
    printf("\n");
    printf("     _____         _     _   _   _    ____ _____ \n");
    printf("    |_   _|__  ___| |_  | | | | / \\  |  _ \\_   _|\n");
    printf("      | |/ _ \\/ __| __| | | | |/ _ \\ | |_) || |  \n");
    printf("      | |  __/\\__ \\ |_  | |_| / ___ \\|  _ < | |  \n");
    printf("      |_|\\___||___/\\__|  \\___/_/   \\_\\_| \\_\\|_|  \n");

    unity_run_menu();
}
