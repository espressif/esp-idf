/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"

/////////////////////////// UART Handling API Test Cases ////////////////////////////////////

TEST_CASE("esp_sleep_set_console_uart_handling_mode parameter validation", "[sleep_uart]")
{
    // Test invalid handling mode
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_NO_HANDLING + 1));

    // Test valid parameters
    TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_ALWAYS_FLUSH_UART));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_ALWAYS_SUSPEND_UART));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_ALWAYS_DISCARD_UART));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_NO_HANDLING));

    // Restore default
    TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART));
}

#if SOC_LIGHT_SLEEP_SUPPORTED

/////////////////////////// pytest-based UART Output Verification Test Cases ////////////////////////////////////

/*
 * Test FLUSH mode: All data should be completely output before sleep
 * pytest verifies: FLUSH_START -> all FLUSH_DATA_X -> FLUSH_SLEEP -> FLUSH_END
 */
TEST_CASE("UART flush mode output verification", "[sleep_uart_output]")
{
    TEST_ESP_OK(esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_ALWAYS_FLUSH_UART));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1000000)); // 1s

    printf("<<<FLUSH_START>>>\n");
    for (int i = 0; i < 10; i++) {
        printf("FLUSH_DATA_%d\n", i);
    }
    printf("<<<FLUSH_SLEEP>>>\n");
    fflush(stdout);

    esp_light_sleep_start();

    printf("<<<FLUSH_END>>>\n");

    TEST_ESP_OK(esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART));
}

/*
 * Test SUSPEND mode: Data in TX FIFO should continue after wakeup
 * pytest verifies: SUSPEND_START -> all SUSPEND_DATA_X -> SUSPEND_SLEEP -> SUSPEND_END
 */
TEST_CASE("UART suspend mode output verification", "[sleep_uart_output]")
{
    TEST_ESP_OK(esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_ALWAYS_SUSPEND_UART));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1000000)); // 1s

    printf("<<<SUSPEND_START>>>\n");
    for (int i = 0; i < 10; i++) {
        printf("SUSPEND_DATA_%d\n", i);
    }
    printf("<<<SUSPEND_SLEEP>>>\n");
    fflush(stdout);

    esp_light_sleep_start();

    printf("<<<SUSPEND_END>>>\n");

    TEST_ESP_OK(esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART));
}

/*
 * Test DISCARD mode: Data in TX FIFO should be discarded before sleep
 * pytest verifies: DISCARD_START appears, DISCARD_END appears,
 *                  but DISCARD_DATA_9_SHOULD_BE_LOST marker should NOT appear (data discarded)
 */
TEST_CASE("UART discard mode output verification", "[sleep_uart_output]")
{
    TEST_ESP_OK(esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_ALWAYS_DISCARD_UART));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1000000)); // 1s

    // This marker must be flushed to ensure it appears
    printf("<<<DISCARD_START>>>\n");
    fflush(stdout);

    // Print data without flush - this will stay in FIFO and be discarded
    for (int i = 0; i < 10; i++) {
        printf("DISCARD_DATA_%02d_SHOULD_BE_LOST\n", i);
    }
    // This marker should NOT appear because FIFO will be reset
    printf("<<<DISCARD_BEFORE_SLEEP>>>\n");
    fflush(stdout);

    esp_light_sleep_start();

    // Small delay to ensure UART is stable after wakeup
    vTaskDelay(pdMS_TO_TICKS(10));
    printf("<<<DISCARD_END>>>\n");

    TEST_ESP_OK(esp_sleep_set_console_uart_handling_mode(ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART));
}

#endif // SOC_LIGHT_SLEEP_SUPPORTED
