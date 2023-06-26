/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos_test_utils.h"

#define configTEST_DEFAULT_STACK_SIZE               4096
#define configTEST_UNITY_TASK_PRIORITY              CONFIG_UNITY_FREERTOS_PRIORITY

#define portTEST_REF_CLOCK_TYPE                     uint64_t
#define portTEST_REF_CLOCK_INIT()
#define portTEST_REF_CLOCK_DEINIT()
#define portTEST_REF_CLOCK_GET_TIME()               ((uint64_t) ref_clock_get())
#define portTEST_TICKS_TO_REF_CLOCK(ticks)          ((ticks) * (1000000/configTICK_RATE_HZ))
