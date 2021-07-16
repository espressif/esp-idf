/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#if SOC_USB_OTG_SUPPORTED

#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include <string.h>

#include "esp_intr_alloc.h"

#include "unity.h"
#include "soc/usb_wrap_struct.h"

static uint8_t tx_buf[] = "HELLO";
static uint8_t tx_buf2[] = "WORLD";
static int nb_of_responses;
static int nb_of_responses2;

void test_usb_force_conn_state(bool connected, TickType_t delay_ticks)
{
    if (delay_ticks > 0) {
        //Delay of 0 ticks causes a yield. So skip if delay_ticks is 0.
        vTaskDelay(delay_ticks);
    }
    usb_wrap_dev_t *wrap = &USB_WRAP;
    if (connected) {
        //Disable test mode to return to previous internal PHY configuration
        wrap->test_conf.test_enable = 0;
    } else {
        /*
        Mimic a disconnection by using the internal PHY's test mode.
        Force Output Enable to 1 (even if the controller isn't outputting). With test_tx_dp and test_tx_dm set to 0,
        this will look like a disconnection.
        */
        wrap->test_conf.val = 0;
        wrap->test_conf.test_usb_wrap_oe = 1;
        wrap->test_conf.test_enable = 1;
    }
}

void usb_lib_task(void *arg)
{
    // Install USB Host driver. Should only be called once in entire application
    const usb_host_config_t host_config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_install(&host_config));
    printf("USB Host installed\n");
    xTaskNotifyGive(arg);

    while (1) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients: clean up\n");
            // The device should not have been freed yet, so we expect an ESP_ERR_NOT_FINISHED
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            printf("All free: uninstall USB lib\n");
            break;
        }
    }

    // Clean up USB Host
    vTaskDelay(10); // Short delay to allow clients clean-up
    usb_host_lib_handle_events(0, NULL); // Make sure there are now pending events
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_uninstall());
    vTaskDelete(NULL);
}

void test_install_cdc_driver(void)
{
    // Create a task that will handle USB library events
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreate(usb_lib_task, "usb_lib", 4*4096, xTaskGetCurrentTaskHandle(), 10, NULL));
    ulTaskNotifyTake(false, 1000);

    printf("Installing CDC-ACM driver\n");
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_install(NULL));
}

/* ------------------------------- Callbacks -------------------------------- */
static void handle_rx(uint8_t *data, size_t data_len, void *arg)
{
    printf("Data received\n");
    nb_of_responses++;
    TEST_ASSERT_EQUAL_STRING_LEN(data, arg, data_len);
}

static void handle_rx2(uint8_t *data, size_t data_len, void *arg)
{
    printf("Data received 2\n");
    nb_of_responses2++;
    TEST_ASSERT_EQUAL_STRING_LEN(data, arg, data_len);
}

static void notif_cb(cdc_acm_dev_hdl_t cdc_hdl, const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
{
    switch (event->type) {
    case CDC_ACM_HOST_ERROR:
        printf("Error event %d\n", event->data.error);
        break;
    case CDC_ACM_HOST_SERIAL_STATE:
        break;
    case CDC_ACM_HOST_NETWORK_CONNECTION:
        break;
    case CDC_ACM_HOST_DEVICE_DISCONNECTED:
        printf("Disconnection event\n");
        TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_hdl));
        xTaskNotifyGive(user_ctx);
        break;
    default:
        assert(false);
    }
}

/* Basic test to check CDC communication:
 * open/read/write/close device
 * CDC-ACM specific commands: set/get_line_coding, set_control_line_state */
TEST_CASE("USB Host CDC-ACM driver: Basic test", "[cdc_acm][ignore]")
{
    nb_of_responses = 0;
    cdc_acm_dev_hdl_t cdc_dev = NULL;

    test_install_cdc_driver();

    const cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 500,
        .out_buffer_size = 64,
        .event_cb = notif_cb,
        .data_cb = handle_rx,
        .user_arg = tx_buf,
    };

    printf("Opening CDC-ACM device\n");
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev)); // 0x303A:0x4002 (TinyUSB Dual CDC device)
    TEST_ASSERT_NOT_NULL(cdc_dev);
    cdc_acm_host_desc_print(cdc_dev);
    vTaskDelay(100);

    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_data_tx_blocking(cdc_dev, tx_buf, sizeof(tx_buf), 1000));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_data_tx_blocking(cdc_dev, tx_buf, sizeof(tx_buf), 1000));
    vTaskDelay(100); // Wait until responses are processed

    // We sent two messages, should get two responses
    TEST_ASSERT_EQUAL(2, nb_of_responses);

    cdc_acm_line_coding_t line_coding_get;
    const cdc_acm_line_coding_t line_coding_set = {
     .dwDTERate = 9600,
     .bDataBits = 7,
     .bParityType = 1,
     .bCharFormat = 1,
    };
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_line_coding_set(cdc_dev, &line_coding_set));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_line_coding_get(cdc_dev, &line_coding_get));
    TEST_ASSERT_EQUAL_MEMORY(&line_coding_set, &line_coding_get, sizeof(cdc_acm_line_coding_t));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_set_control_line_state(cdc_dev, true, false));

    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_dev));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_uninstall());

    vTaskDelay(20); //Short delay to allow task to be cleaned up
}

/* Test communication with multiple CDC-ACM devices from one thread */
TEST_CASE("USB Host CDC-ACM driver: Multiple devices test", "[cdc_acm][ignore]")
{
    nb_of_responses = 0;
    nb_of_responses2 = 0;

    test_install_cdc_driver();

    printf("Opening 2 CDC-ACM devices\n");
    cdc_acm_dev_hdl_t cdc_dev1, cdc_dev2;
    cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = 64,
        .event_cb = notif_cb,
        .data_cb = handle_rx,
        .user_arg = tx_buf,
    };
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev1)); // 0x303A:0x4002 (TinyUSB Dual CDC device)
    dev_config.data_cb = handle_rx2;
    dev_config.user_arg = tx_buf2;
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 2, &dev_config, &cdc_dev2)); // 0x303A:0x4002 (TinyUSB Dual CDC device)
    TEST_ASSERT_NOT_NULL(cdc_dev1);
    TEST_ASSERT_NOT_NULL(cdc_dev2);

    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_data_tx_blocking(cdc_dev1, tx_buf, sizeof(tx_buf), 1000));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_data_tx_blocking(cdc_dev2, tx_buf2, sizeof(tx_buf2), 1000));

    vTaskDelay(100); // Wait for RX callbacks

    // We sent two messages, should get two responses
    TEST_ASSERT_EQUAL(1, nb_of_responses);
    TEST_ASSERT_EQUAL(1, nb_of_responses2);

    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_dev1));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_dev2));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_uninstall());

    //Short delay to allow task to be cleaned up
    vTaskDelay(20);
}

#define MULTIPLE_THREADS_TRANSFERS_NUM 5
#define MULTIPLE_THREADS_TASKS_NUM 4
void tx_task(void *arg)
{
    cdc_acm_dev_hdl_t cdc_dev = (cdc_acm_dev_hdl_t) arg;
    // Send multiple transfers to make sure that some of them will run at the same time
    for (int i = 0; i < MULTIPLE_THREADS_TRANSFERS_NUM; i++) {
        // BULK endpoints
        TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_data_tx_blocking(cdc_dev, tx_buf, sizeof(tx_buf), 1000));

        // CTRL endpoints
        cdc_acm_line_coding_t line_coding_get;
        TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_line_coding_get(cdc_dev, &line_coding_get));
        TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_set_control_line_state(cdc_dev, true, false));
    }
    vTaskDelete(NULL);
}

/**
 * @brief Multiple threads test
 *
 * In this test, one CDC device is accessed from multiple threads.
 * It has to be opened/closed just once, though.
 */
TEST_CASE("USB Host CDC-ACM driver: Multiple threads test", "[cdc_acm][ignore]")
{
    nb_of_responses = 0;
    cdc_acm_dev_hdl_t cdc_dev;
    test_install_cdc_driver();

    const cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 5000,
        .out_buffer_size = 64,
        .event_cb = notif_cb,
        .data_cb = handle_rx,
        .user_arg = tx_buf,
    };

    printf("Opening CDC-ACM device\n");
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev)); // 0x303A:0x4002 (TinyUSB Dual CDC device)
    TEST_ASSERT_NOT_NULL(cdc_dev);

    // Create two tasks that will try to access cdc_dev
    for (int i = 0; i < MULTIPLE_THREADS_TASKS_NUM; i++) {
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreate(tx_task, "CDC TX", 4096, cdc_dev, i + 3, NULL));
    }

    // Wait until all tasks finish
    vTaskDelay(pdMS_TO_TICKS(500));
    TEST_ASSERT_EQUAL(MULTIPLE_THREADS_TASKS_NUM * MULTIPLE_THREADS_TRANSFERS_NUM, nb_of_responses);

    // Clean-up
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_dev));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_uninstall());
    vTaskDelay(20);
}

/* Test CDC driver reaction to USB device sudden disconnection */
TEST_CASE("USB Host CDC-ACM driver: Sudden disconnection test", "[cdc_acm][ignore]")
{
    test_install_cdc_driver();

    cdc_acm_dev_hdl_t cdc_dev;
    cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = 64,
        .event_cb = notif_cb,
        .data_cb = handle_rx
    };
    dev_config.user_arg = xTaskGetCurrentTaskHandle();
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev));
    TEST_ASSERT_NOT_NULL(cdc_dev);

    test_usb_force_conn_state(false, pdMS_TO_TICKS(10));
    // Notify will succeed only if CDC_ACM_HOST_DEVICE_DISCONNECTED notification was generated
    TEST_ASSERT_EQUAL(1, ulTaskNotifyTake(false, pdMS_TO_TICKS(100)));

    test_usb_force_conn_state(true, 0); // Switch back to real PHY
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_uninstall());
    vTaskDelay(20); //Short delay to allow task to be cleaned up
}

/**
 * @brief CDC-ACM error handling test
 *
 * There are multiple erroneous scenarios checked in this test:
 *
 * -# Install CDC-ACM driver without USB Host
 * -# Open device without installed driver
 * -# Uninstall driver before installing it
 * -# Open non-existent device
 * -# Open the same device twice
 * -# Uninstall driver with open devices
 * -# Send data that is too large
 * -# Send unsupported CDC request
 * -# Write to read-only device
 */
TEST_CASE("USB Host CDC-ACM driver: Error handling", "[cdc_acm][ignore]")
{
    cdc_acm_dev_hdl_t cdc_dev;
    cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 500,
        .out_buffer_size = 64,
        .event_cb = notif_cb,
        .data_cb = handle_rx
    };

    // Install CDC-ACM driver without USB Host
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, cdc_acm_host_install(NULL));

    // Open device without installed driver
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev));

    // Uninstall driver before installing it
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, cdc_acm_host_uninstall());

    // Properly install USB and CDC drivers
    test_install_cdc_driver();

    // Open non-existent device
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, cdc_acm_host_open(0x303A, 0x1234, 0, &dev_config, &cdc_dev)); // 0x303A:0x1234 this device is not connected to USB Host
    TEST_ASSERT_NULL(cdc_dev);

    // Open regular device
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev));
    TEST_ASSERT_NOT_NULL(cdc_dev);

    // Open one CDC-ACM device twice //@todo this test is commented out due to bug in usb_host
    //cdc_acm_dev_hdl_t cdc_dev_test;
    //TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev_test));
    //TEST_ASSERT_NULL(cdc_dev_test);

    // Uninstall driver with open devices
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, cdc_acm_host_uninstall());

    // Send data that is too large and NULL data
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, cdc_acm_host_data_tx_blocking(cdc_dev, tx_buf, 1024, 1000));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, cdc_acm_host_data_tx_blocking(cdc_dev, NULL, 10, 1000));

    // Change mode to read-only and try to write to it
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_dev));
    dev_config.out_buffer_size = 0; // Read-only device
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_open(0x303A, 0x4002, 0, &dev_config, &cdc_dev));
    TEST_ASSERT_NOT_NULL(cdc_dev);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, cdc_acm_host_data_tx_blocking(cdc_dev, tx_buf, sizeof(tx_buf), 1000));

    // Send unsupported CDC request (TinyUSB accepts SendBreak command, eventhough it doesn't support it)
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_send_break(cdc_dev, 100));

    // Clean-up
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_close(cdc_dev));
    TEST_ASSERT_EQUAL(ESP_OK, cdc_acm_host_uninstall());
    vTaskDelay(20);
}

#endif
