/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_attr.h"
#include "driver/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32P4
#define TEST_PWMA_GPIO   0
#define TEST_CAP_GPIO    0
#define TEST_FAULT_GPIO  0
#define TEST_PWMB_GPIO   20
#define TEST_FAULT_GPIO0 20
#define TEST_FAULT_GPIO1 21
#define TEST_FAULT_GPIO2 22
#define TEST_SYNC_GPIO   23
#else
#define TEST_PWMA_GPIO   0
#define TEST_CAP_GPIO    0
#define TEST_FAULT_GPIO  0
#define TEST_PWMB_GPIO   2
#define TEST_FAULT_GPIO0 2
#define TEST_FAULT_GPIO1 4
#define TEST_FAULT_GPIO2 5
#define TEST_SYNC_GPIO   12
#endif

#if CONFIG_MCPWM_ISR_IRAM_SAFE
#define TEST_MCPWM_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_MCPWM_CALLBACK_ATTR
#endif // CONFIG_MCPWM_ISR_IRAM_SAFE

void check_mcpwm_timer_phase(mcpwm_timer_handle_t *timers, size_t num_timers,
                             uint32_t expected_count, mcpwm_timer_direction_t expected_direction);

#ifdef __cplusplus
}
#endif
