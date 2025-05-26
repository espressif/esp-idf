/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define LEAKS (100)

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

    // ,--.,----.  ,-----.    ,--------.,------. ,---. ,--------.
    // |  |'.-.  |'  .--./    '--.  .--'|  .---''   .-''--.  .--'
    // |  |  .' < |  |           |  |   |  `--, `.  `-.   |  |
    // |  |/'-'  |'  '--'\       |  |   |  `---..-'    |  |  |
    // `--'`----'  `-----'       `--'   `------'`-----'   `--'

    printf(",--.,----.  ,-----.    ,--------.,------. ,---. ,--------. \n");
    printf("|  |'.-.  |'  .--./    '--.  .--'|  .---''   .-''--.  .--' \n");
    printf("|  |  .' < |  |           |  |   |  `--, `.  `-.   |  |    \n");
    printf("|  |/'-'  |'  '--'\\       |  |   |  `---..-'    |  |  |    \n");
    printf("`--'`----'  `-----'       `--'   `------'`-----'   `--'    \n");

    unity_run_menu();
}
