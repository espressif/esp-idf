/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdexcept>
#include <vector>
#include "unity.h"

extern "C" void setUp()
{
}

extern "C" void tearDown()
{
}

TEST_CASE("std::out_of_range exception when -fno-exceptions", "[cxx][reset=abort,SW_CPU_RESET]")
{
    std::vector<int> v(10);
// suppress warning because of this test
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    v.at(20) = 42;
#pragma GCC diagnostic pop
    TEST_FAIL_MESSAGE("Unreachable because we are aborted on the line above");
}

TEST_CASE("std::bad_alloc exception when -fno-exceptions", "[cxx][reset=abort,SW_CPU_RESET]")
{
    std::string s = std::string(2000000000, 'a');
    (void)s;
    TEST_FAIL_MESSAGE("Unreachable because we are aborted on the line above");
}

extern "C" void app_main(void)
{
    printf("CXX NO EXCEPTION TEST\n");
    unity_run_menu();
}
