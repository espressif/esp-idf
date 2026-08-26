/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity_test_utils_memory.h"
#include "unity_test_utils_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Delete task ensuring dynamic memory (for stack, tcb etc.) gets freed up immediately
 *
 * @param[in] thandle    Handle of task to be deleted (should not be NULL or self handle)
 */
void unity_utils_task_delete(TaskHandle_t thandle);

#ifdef __cplusplus
}
#endif
