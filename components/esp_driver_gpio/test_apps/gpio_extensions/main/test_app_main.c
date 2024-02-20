/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in the driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (300)

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
    //   ____ ____ ___ ___    _____      _     _____         _
    //  / ___|  _ \_ _/ _ \  | ____|_  _| |_  |_   _|__  ___| |_
    // | |  _| |_) | | | | | |  _| \ \/ / __|   | |/ _ \/ __| __|
    // | |_| |  __/| | |_| | | |___ >  <| |_    | |  __/\__ \ |_
    //  \____|_|  |___\___/  |_____/_/\_\\__|   |_|\___||___/\__|
    printf("  ____ ____ ___ ___    _____      _     _____         _\r\n");
    printf(" / ___|  _ \\_ _/ _ \\  | ____|_  _| |_  |_   _|__  ___| |_\r\n");
    printf("| |  _| |_) | | | | | |  _| \\ \\/ / __|   | |/ _ \\/ __| __|\r\n");
    printf("| |_| |  __/| | |_| | | |___ >  <| |_    | |  __/\\__ \\ |_\r\n");
    printf(" \\____|_|  |___\\___/  |_____/_/\\_\\\\__|   |_|\\___||___/\\__|\r\n");
    unity_run_menu();
}
