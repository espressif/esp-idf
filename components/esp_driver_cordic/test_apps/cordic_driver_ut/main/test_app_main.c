/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in CORDIC driver, so we reserved this threshold when checking memory leak
// A better way to check a potential memory leak is running a same case by twice, for the second time, the memory usage delta should be zero
#define LEAKS (400)

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
    // ,-----. ,-----. ,------. ,------.  ,--. ,-----.
    // '  .--./'  .-.  '|  .--. '|  .-.  \ |  |'  .--./
    // |  |    |  | |  ||  '--'.'|  |  \  :|  ||  |
    // '  '--'\'  '-'  '|  |\  \ |  '--'  /|  |'  '--'
    //  `-----' `-----' `--' '--'`-------' `--' `-----'

    printf(" ,-----. ,-----. ,------. ,------.  ,--. ,-----. \n");
    printf("'  .--./'  .-.  '|  .--. '|  .-.  \\ |  |'  .--./ \n");
    printf("|  |    |  | |  ||  '--'.'|  |  \\  :|  ||  |     \n");
    printf("'  '--'\\'  '-'  '|  |\\  \\ |  '--'  /|  |'  '--'\\\n");
    printf(" `-----' `-----' `--' '--'`-------' `--' `-----'\n");

    unity_run_menu();
}
