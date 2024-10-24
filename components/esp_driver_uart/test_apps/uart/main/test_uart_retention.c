/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/uart.h"
#include "esp_pm.h"
#include "esp_private/sleep_cpu.h"
#include "esp_clk_tree.h"
#include "esp_sleep.h"

// UART retention test only need to be done on HP UART

static const uart_port_t uart_num = UART_NUM_1;

static void uart_init(bool allow_pd)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
        .flags.allow_pd = allow_pd,
    };

    TEST_ESP_OK(uart_driver_install(uart_num, 256, 0, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_set_loop_back(uart_num, true));
}

TEST_CASE("uart restored correctly after auto light sleep", "[uart][hp-uart-only]")
{
    // Configure dynamic frequency scaling:
    // maximum and minimum frequencies are set in sdkconfig,
    // automatic light sleep is enabled if tickless idle support is enabled.
    uint32_t xtal_hz = 0;
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_XTAL, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &xtal_hz);
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = xtal_hz / 1000000,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true,
#endif
    };
    TEST_ESP_OK(esp_pm_configure(&pm_config));

    uart_init(true);

    // Ensure UART is fully idle before starting loopback RX/TX test
    TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(20)); // make sure last byte has flushed from TX FIFO
    TEST_ESP_OK(uart_flush_input(uart_num));

    for (int i = 0; i < 5; i++) {
        char tx_data[20] = {0};
        char rx_data[20] = {0};
        int len = sprintf(tx_data, "Hello World %d!\n", i);
        uart_write_bytes(uart_num, tx_data, len);
        int size = 0;
        // Polling to read the data back to avoid getting into auto light sleep
        while (size < len) {
            int bytes = uart_read_bytes(uart_num, (void *)((uint32_t)rx_data + size), 1, 0);
            size += bytes;
        }
        rx_data[len] = '\0';
        printf("%s", rx_data);
        TEST_ASSERT_TRUE(strcmp(tx_data, rx_data) == 0);

        vTaskDelay(pdMS_TO_TICKS(1000)); // auto light sleep
    }

    TEST_ESP_OK(uart_driver_delete(uart_num));

    pm_config.light_sleep_enable = false;
    TEST_ESP_OK(esp_pm_configure(&pm_config));
}

TEST_CASE("uart restored correctly after manually enter light sleep", "[uart][hp-uart-only]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    sleep_cpu_configure(true);

    uart_init(true);

    // Ensure UART is fully idle before starting loopback RX/TX test
    TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(20)); // make sure last byte has flushed from TX FIFO
    TEST_ESP_OK(uart_flush_input(uart_num));

    for (int i = 0; i < 5; i++) {
        char tx_data[20] = {0};
        char rx_data[20] = {0};
        int len = sprintf(tx_data, "Hello World %d!\n", i);
        uart_write_bytes(uart_num, tx_data, len);
        int size = uart_read_bytes(uart_num, rx_data, len, pdMS_TO_TICKS(20));
        TEST_ASSERT_EQUAL(len, size);
        rx_data[len] = '\0';
        printf("%s", rx_data);
        TEST_ASSERT_TRUE(strcmp(tx_data, rx_data) == 0);

        printf("Going into sleep...\n");
        TEST_ESP_OK(esp_light_sleep_start());
        printf("Waked up!\n");
    }

    TEST_ESP_OK(uart_driver_delete(uart_num));
    TEST_ESP_OK(sleep_cpu_configure(false));
}

TEST_CASE("uart won't be powered down in light sleep if retention not created", "[uart][hp-uart-only]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    sleep_cpu_configure(true);

    uart_init(false); // allow_pd set to false, sleep retention module will be inited, but not created

    // Ensure UART is fully idle before starting loopback RX/TX test
    TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(20)); // make sure last byte has flushed from TX FIFO
    TEST_ESP_OK(uart_flush_input(uart_num));

    for (int i = 0; i < 3; i++) {
        char tx_data[20] = {0};
        char rx_data[20] = {0};
        int len = sprintf(tx_data, "Hello World %d!\n", i);
        uart_write_bytes(uart_num, tx_data, len);
        int size = uart_read_bytes(uart_num, rx_data, len, pdMS_TO_TICKS(20));
        TEST_ASSERT_EQUAL(len, size);
        rx_data[len] = '\0';
        printf("%s", rx_data);
        TEST_ASSERT_TRUE(strcmp(tx_data, rx_data) == 0);

        printf("Going into sleep...\n");
        TEST_ESP_OK(esp_light_sleep_start()); // sleep without powering down TOP domain
        printf("Waked up!\n");
    }

    TEST_ESP_OK(uart_driver_delete(uart_num));
    TEST_ESP_OK(sleep_cpu_configure(false));
}
