/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "unity_fixture.h"

static void run_all_tests(void)
{
    RUN_TEST_GROUP(stdatomic);
    RUN_TEST_GROUP(misc);
    RUN_TEST_GROUP(file);
}

void app_main(void)
{
    UNITY_MAIN_FUNC(run_all_tests);
}
