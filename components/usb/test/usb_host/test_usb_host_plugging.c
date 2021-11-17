/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "test_usb_common.h"
#include "test_usb_mock_classes.h"
#include "msc_client.h"
#include "ctrl_client.h"
#include "usb/usb_host.h"
#include "unity.h"
#include "test_utils.h"

// --------------------------------------------------- Test Cases ------------------------------------------------------

//Safe approximation of time it takes to connect and enumerate the device
#define TEST_FORCE_DCONN_DELAY_MS           400

static void trigger_dconn_timer_cb(TimerHandle_t xTimer)
{
    printf("Forcing Sudden Disconnect\n");
    test_usb_set_phy_state(false, 0);
}

TEST_CASE("Test USB Host sudden disconnection (no client)", "[usb_host][ignore]")
{
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing
    //Install USB Host Library
    usb_host_config_t host_config = {
        .skip_phy_setup = true,     //test_usb_init_phy() will already have setup the internal USB PHY for us
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    printf("Installed\n");

    //Allocate timer to force disconnection after a short delay
    TimerHandle_t timer_hdl = xTimerCreate("dconn",
                                           pdMS_TO_TICKS(TEST_FORCE_DCONN_DELAY_MS),
                                           pdFALSE,
                                           NULL,
                                           trigger_dconn_timer_cb);
    TEST_ASSERT_NOT_EQUAL(NULL, timer_hdl);
    TEST_ASSERT_EQUAL(pdPASS, xTimerStart(timer_hdl, portMAX_DELAY));

    while (1) {
        //Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            printf("All devices cleaned up\n");
            break;
        }
    }

    //Cleanup timer
    TEST_ASSERT_EQUAL(pdPASS, xTimerDelete(timer_hdl, portMAX_DELAY));
    //Clean up USB Host
    ESP_ERROR_CHECK(usb_host_uninstall());
    test_usb_deinit_phy();  //Deinitialize the internal USB PHY after testing
}

#define TEST_FORCE_DCONN_NUM_TRANSFERS      3
#define TEST_MSC_SCSI_TAG                   0xDEADBEEF

TEST_CASE("Test USB Host sudden disconnection (single client)", "[usb_host][ignore]")
{
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing
    //Install USB Host
    usb_host_config_t host_config = {
        .skip_phy_setup = true,     //test_usb_init_phy() will already have setup the internal USB PHY for us
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    printf("Installed\n");

    //Create task to run client that communicates with MSC SCSI interface
    msc_client_test_param_t params = {
        .num_sectors_to_read = 1,   //Unused by disconnect MSC client
        .num_sectors_per_xfer = TEST_FORCE_DCONN_NUM_TRANSFERS * MOCK_MSC_SCSI_SECTOR_SIZE,
        .msc_scsi_xfer_tag = TEST_MSC_SCSI_TAG,
        .idVendor = MOCK_MSC_SCSI_DEV_ID_VENDOR,
        .idProduct = MOCK_MSC_SCSI_DEV_ID_PRODUCT,
    };
    TaskHandle_t task_hdl;
    xTaskCreatePinnedToCore(msc_client_async_dconn_task, "async", 4096, (void *)&params, 2, &task_hdl, 0);
    //Start the task
    xTaskNotifyGive(task_hdl);

    bool all_clients_gone = false;
    bool all_dev_free = false;
    while (!all_clients_gone || !all_dev_free) {
        //Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients\n");
            all_clients_gone = true;
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            printf("All device's freed\n");
            all_dev_free = true;
        }
    }

    //Short delay to allow task to be cleaned up
    vTaskDelay(10);
    //Clean up USB Host
    ESP_ERROR_CHECK(usb_host_uninstall());
    test_usb_deinit_phy();  //Deinitialize the internal USB PHY after testing
}
