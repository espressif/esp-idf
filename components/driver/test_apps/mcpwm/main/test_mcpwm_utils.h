/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_attr.h"
#include "driver/mcpwm_types.h"

#if CONFIG_MCPWM_ISR_IRAM_SAFE
#define TEST_MCPWM_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_MCPWM_CALLBACK_ATTR
#endif // CONFIG_MCPWM_ISR_IRAM_SAFE

void check_mcpwm_timer_phase(mcpwm_timer_handle_t *timers, size_t num_timers,
                             uint32_t expected_count, mcpwm_timer_direction_t expected_direction);
