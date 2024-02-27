/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

// Some resources are lazy allocated in I2C driver, so we reserved this threshold when checking memory leak
// A better way to check a potential memory leak is running a same case by twice, for the second time, the memory usage delta should be zero
#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP
#define LEAKS (1200) // For 802154 usage
#else
#define LEAKS (400)
#endif

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(LEAKS);
}

void app_main(void)
{

    //  ___ ____   ____   _____         _
    // |_ _|___ \ / ___| |_   _|__  ___| |_
    //  | |  __) | |       | |/ _ \/ __| __|
    //  | | / __/| |___    | |  __/\__ \ |_
    // |___|_____|\____|   |_|\___||___/\__|

    printf(" ___ ____   ____   _____         _   \n");
    printf("|_ _|___ \\ / ___| |_   _|__  ___| |_ \n");
    printf(" | |  __) | |       | |/ _ \\/ __| __|\n");
    printf(" | | / __/| |___    | |  __/\\__ \\ |_ \n");
    printf("|___|_____|\\____|   |_|\\___||___/\\__|\n");

    unity_run_menu();
}
