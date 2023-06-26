/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity_fixture.h"
#include "unity_fixture_extras.h"

static void run_all_tests(void)
{
    RUN_TEST_GROUP(transport_basic);
    RUN_TEST_GROUP(transport_connect);
}

void app_main(void)
{
    UNITY_MAIN_FUNC(run_all_tests);
}
