/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_utils.h"
#include "esp_cpu.h"

#define configTEST_DEFAULT_STACK_SIZE               4096
#define configTEST_UNITY_TASK_PRIORITY              UNITY_FREERTOS_PRIORITY

#define portTEST_GET_TIME()                         ((UBaseType_t) esp_cpu_get_cycle_count())
