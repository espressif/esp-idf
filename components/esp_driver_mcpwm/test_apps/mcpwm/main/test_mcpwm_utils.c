/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include "esp_private/mcpwm.h"
#include "test_mcpwm_utils.h"

void check_mcpwm_timer_phase(mcpwm_timer_handle_t *timers, size_t num_timers,
                             uint32_t expected_count, mcpwm_timer_direction_t expected_direction)
{
    uint32_t count_value;
    mcpwm_timer_direction_t direction;
    for (size_t i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_get_phase(timers[i], &count_value, &direction));
        TEST_ASSERT_INT_WITHIN(1, expected_count, count_value);
        TEST_ASSERT_EQUAL(expected_direction, direction);
    }
}
