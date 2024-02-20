/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"

// Unity 2.6.0 has removed weak definitions of setUp, tearDown, suiteSetUp and suiteTearDown.
// (https://github.com/ThrowTheSwitch/Unity/pull/454)
// We need to provide them here to avoid breaking the existing test applications.

__attribute__((weak)) void setUp(void)
{
}

__attribute__((weak)) void tearDown(void)
{
}


__attribute__((weak)) void suiteSetUp(void)
{
}

__attribute__((weak)) int suiteTearDown(int num_failures)
{
    return num_failures;
}
