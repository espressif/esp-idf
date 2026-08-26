/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in esp_flash test, especially ccomp timer test, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (700)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{

    // ,------. ,---.  ,------.     ,------.,--.               ,--.
    // |  .---''   .-' |  .--. '    |  .---'|  | ,--,--. ,---. |  ,---.
    // |  `--, `.  `-. |  '--' |    |  `--, |  |' ,-.  |(  .-' |  .-.  |
    // |  `---..-'    ||  | --'     |  |`   |  |\ '-'  |.-'  `)|  | |  |
    // `------'`-----' `--'         `--'    `--' `--`--'`----' `--' `--'


    printf(",------. ,---.  ,------.     ,------.,--.               ,--.      \n");
    printf("|  .---''   .-' |  .--. '    |  .---'|  | ,--,--. ,---. |  ,---.  \n");
    printf("|  `--, `.  `-. |  '--' |    |  `--, |  |' ,-.  |(  .-' |  .-.  | \n");
    printf("|  `---..-'    ||  | --'     |  |`   |  |\\ '-'  |.-'  `)|  | |  | \n");
    printf("`------'`-----' `--'         `--'    `--' `--`--'`----' `--' `--' \n");


    unity_run_menu();
}
