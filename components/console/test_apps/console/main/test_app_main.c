/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include <sys/time.h>

// Some resources are lazy allocated (newlib locks) in the console code, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (150)

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
    /* Preallocate some newlib locks to avoid it from
       registering as memory leaks */

    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);

    printf("Running console component tests\n");
    unity_run_menu();
}
