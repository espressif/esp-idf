/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_cpu_intr.h"

#include "unity.h"
#include "sdkconfig.h"

/**
 * @brief Test M-mode interrupts in M-mode
 *
 * This test verifies interrupt handling behavior in M-mode:
 * - Sets up a timer interrupt firing every 10ms (priority 2) in M-mode
 * - Triggers INTPRI interrupt every 100ms (priority 1) in M-mode
 * - Verifies that both interrupts are handled correctly
 * - Checks interrupt counts match expected values
 */
TEST_CASE("Test M-mode interrupts in M-mode", "[CPU]")
{
    test_m_mode_intr_in_m_mode();
}

/**
 * @brief Test U-mode interrupts in U-mode
 *
 * This test verifies interrupt handling behavior in U-mode:
 * - Sets up a timer interrupt firing every 10ms (priority 2) in U-mode
 * - Triggers INTPRI interrupt every 100ms (priority 1) in U-mode
 * - Verifies that both interrupts are handled correctly
 * - Checks interrupt counts match expected values
 */
TEST_CASE("Test U-mode interrupts in U-mode", "[CPU]")
{
    test_u_mode_intr_in_u_mode();
}

/**
 * @brief Test M-mode interrupts in U-mode
 *
 * This test verifies interrupt handling behavior in U-mode:
 * - Sets up a timer interrupt firing every 10ms (priority 2) in U-mode
 * - Triggers INTPRI interrupt every 100ms (priority 1) in M-mode
 * - Verifies that both interrupts are handled correctly
 * - Checks interrupt counts match expected values
 */
TEST_CASE("Test M-mode interrupts in U-mode", "[CPU]")
{
    test_m_mode_intr_in_u_mode();
}

/**
 * @brief Test U-mode interrupts in M-mode
 *
 * This test verifies interrupt handling behavior in M-mode:
 * - Sets up a timer interrupt firing every 10ms (priority 2) in M-mode
 * - Triggers INTPRI interrupt every 100ms (priority 1) in U-mode
 * - Verifies that both interrupts are handled correctly
 * - Checks interrupt counts match expected values
 */
TEST_CASE("Test U-mode interrupts in M-mode", "[CPU]")
{
    test_u_mode_intr_in_m_mode();
}
