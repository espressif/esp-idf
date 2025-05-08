/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hcd_common.h"
#include "hub_common.h"
#include "ext_port_common.h"

static hcd_port_handle_t _root_port_hdl;

void setUp(void)
{
    unity_utils_record_free_mem();
    // Install HCD port
    _root_port_hdl = test_hcd_setup();
    // Set root port handle to the hub
    hub_set_root_port(_root_port_hdl);
    // Setup the External Port Driver
    test_ext_port_setup();
    printf("External Port Driver installed\n");
}

void tearDown(void)
{
    // Short delay to allow task to be cleaned up
    vTaskDelay(10);
    // Clean up USB Host
    printf("External Port Driver uninstall\n");
    test_ext_port_teardown();
    test_hcd_teardown(_root_port_hdl);
    // Short delay to allow task to be cleaned up after client uninstall
    vTaskDelay(10);
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
