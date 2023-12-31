/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in pulse_cnt driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (400)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    esp_reent_cleanup();    //clean up some of the newlib's lazy allocations
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    //  ____                 _ _      _   ___ ___    _____         _
    // |  _ \ __ _ _ __ __ _| | | ___| | |_ _/ _ \  |_   _|__  ___| |_
    // | |_) / _` | '__/ _` | | |/ _ \ |  | | | | |   | |/ _ \/ __| __|
    // |  __/ (_| | | | (_| | | |  __/ |  | | |_| |   | |  __/\__ \ |_
    // |_|   \__,_|_|  \__,_|_|_|\___|_| |___\___/    |_|\___||___/\__|
    printf(" ____                 _ _      _   ___ ___    _____         _\r\n");
    printf("|  _ \\ __ _ _ __ __ _| | | ___| | |_ _/ _ \\  |_   _|__  ___| |_\r\n");
    printf("| |_) / _` | '__/ _` | | |/ _ \\ |  | | | | |   | |/ _ \\/ __| __|\r\n");
    printf("|  __/ (_| | | | (_| | | |  __/ |  | | |_| |   | |  __/\\__ \\ |_\r\n");
    printf("|_|   \\__,_|_|  \\__,_|_|_|\\___|_| |___\\___/    |_|\\___||___/\\__|\r\n");
    unity_run_menu();
}
