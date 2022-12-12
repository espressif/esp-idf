/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_usb_common.h"
#include "test_usb_mock_msc.h"
#include "usb/usb_host.h"

void setUp(void)
{
    mock_msc_scsi_init_reference_descriptors();
    unity_utils_record_free_mem();
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing
    //Install USB Host
    usb_host_config_t host_config = {
        .skip_phy_setup = true,     //test_usb_init_phy() will already have setup the internal USB PHY for us
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    printf("USB Host installed\n");
}

void tearDown(void)
{
    //Short delay to allow task to be cleaned up
    vTaskDelay(10);
    //Clean up USB Host
    ESP_ERROR_CHECK(usb_host_uninstall());
    test_usb_deinit_phy();  //Deinitialize the internal USB PHY after testing
    unity_utils_evaluate_leaks();
}

void app_main(void)
{
    //  ____ ___  ___________________    __                   __
    // |    |   \/   _____/\______   \ _/  |_  ____   _______/  |_
    // |    |   /\_____  \  |    |  _/ \   __\/ __ \ /  ___/\   __\.
    // |    |  / /        \ |    |   \  |  | \  ___/ \___ \  |  |
    // |______/ /_______  / |______  /  |__|  \___  >____  > |__|
    //                  \/         \/             \/     \/
    printf(" ____ ___  ___________________    __                   __   \r\n");
    printf("|    |   \\/   _____/\\______   \\ _/  |_  ____   _______/  |_ \r\n");
    printf("|    |   /\\_____  \\  |    |  _/ \\   __\\/ __ \\ /  ___/\\   __\\\r\n");
    printf("|    |  / /        \\ |    |   \\  |  | \\  ___/ \\___ \\  |  |  \r\n");
    printf("|______/ /_______  / |______  /  |__|  \\___  >____  > |__|  \r\n");
    printf("                 \\/         \\/             \\/     \\/        \r\n");

    unity_utils_setup_heap_record(80);
    unity_utils_set_leak_level(128);
    unity_run_menu();
}
