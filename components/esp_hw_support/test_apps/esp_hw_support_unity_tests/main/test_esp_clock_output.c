/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_clock_output.h"
#include "hal/gpio_hal.h"

#include "soc/rtc.h"

#define TEST_LOOPS 100

#if CONFIG_IDF_TARGET_ESP32
static const int test_clk_out_io[] = {0, 1, 3};
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
static const int test_clk_out_io[] = {18, 19, 20};
#elif CONFIG_IDF_TARGET_ESP32P4
static const int test_clk_out_io[] = {7, 8};
#else
static const int test_clk_out_io[] = {3, 4, 5, 6};
#endif

static soc_clkout_sig_id_t test_clk_out_sig[] = {CLKOUT_SIG_XTAL, CLKOUT_SIG_RC_SLOW, CLKOUT_SIG_RC_FAST, CLKOUT_SIG_INVALID};

static SemaphoreHandle_t test_done_semphr;

void output_clock_1(void *pvParameter)
{
    esp_clock_output_mapping_handle_t clkout_mapping_hdl;
    for (int i = 0; i < TEST_LOOPS; ++i) {
        TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[0], test_clk_out_io[0], &clkout_mapping_hdl));
#if SOC_CLOCKOUT_SUPPORT_CHANNEL_DIVIDER
        TEST_ESP_OK(esp_clock_output_set_divider(clkout_mapping_hdl, 8));
#endif
        vTaskDelay(3);
        TEST_ESP_OK(esp_clock_output_stop(clkout_mapping_hdl));
        vTaskDelay(7);
    }
    xSemaphoreGive(test_done_semphr);
    vTaskDelete(NULL);
}

void output_clock_2(void *pvParameter)
{
    esp_clock_output_mapping_handle_t clkout_mapping_hdl;
    for (int i = 0; i < TEST_LOOPS; ++i) {
        TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[1], test_clk_out_io[1], &clkout_mapping_hdl));
        vTaskDelay(4);
        TEST_ESP_OK(esp_clock_output_stop(clkout_mapping_hdl));
        vTaskDelay(6);
    }
    xSemaphoreGive(test_done_semphr);
    vTaskDelete(NULL);
}

#if SOC_GPIO_CLOCKOUT_CHANNEL_NUM >= 3
void output_clock_3(void *pvParameter)
{
    rtc_dig_clk8m_enable();
    esp_clock_output_mapping_handle_t clkout_mapping_hdl;
    for (int i = 0; i < TEST_LOOPS; ++i) {
        TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[2], test_clk_out_io[2], &clkout_mapping_hdl));
        vTaskDelay(5);
        TEST_ESP_OK(esp_clock_output_stop(clkout_mapping_hdl));
        vTaskDelay(5);
    }
    xSemaphoreGive(test_done_semphr);
    vTaskDelete(NULL);
}
#endif

// This case is now tested only manually
TEST_CASE("GPIO output internal clock", "[gpio_output_clock][ignore]")
{
    test_done_semphr = xSemaphoreCreateCounting(3, 0);
    xTaskCreate(&output_clock_1, "output_clock_1", 4096, NULL, 4, NULL);
    xTaskCreate(&output_clock_2, "output_clock_2", 4096, NULL, 4, NULL);
#if SOC_GPIO_CLOCKOUT_CHANNEL_NUM >= 3
    xTaskCreate(&output_clock_3, "output_clock_3", 4096, NULL, 4, NULL);
#endif

    int cnt = 0;
    while (cnt < SOC_GPIO_CLOCKOUT_CHANNEL_NUM) {
        if (xSemaphoreTake(test_done_semphr, portMAX_DELAY) == pdTRUE) {
            cnt++;
        }
    }
    vTaskDelay(1);

    vSemaphoreDelete(test_done_semphr);

#if CONFIG_IDF_TARGET_ESP32
    /* ESP32 clock out channel pin reuses UART TX/RX pin, restore its default
       configuration at the end of the test */
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);
#endif
}

#if SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX
TEST_CASE("GPIO output internal clock one-to-many", "[gpio_output_clock][ignore]")
{
    // Test NULL handle assertion
    TEST_ESP_ERR(esp_clock_output_start(test_clk_out_sig[0], test_clk_out_io[0], NULL), ESP_ERR_INVALID_ARG);

    // Test first mapping
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_0;
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[0], test_clk_out_io[0], &clkout_mapping_hdl_0));    // [0]:0(1) [1]:x [2]:x

    // Test the same mapping allocate the same handle
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_1;
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[0], test_clk_out_io[0], &clkout_mapping_hdl_1));    // [0]:0(2) [1]:x [2]:x
    TEST_ASSERT(clkout_mapping_hdl_0 == clkout_mapping_hdl_1);

    // Test map multi signal to one IO assertion
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_2;
    TEST_ESP_ERR(esp_clock_output_start(test_clk_out_sig[1], test_clk_out_io[0], &clkout_mapping_hdl_2), ESP_ERR_INVALID_ARG);

    // Test the same signal mapping to multi GPIO
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[0], test_clk_out_io[1], &clkout_mapping_hdl_2));    // [0]:0(2) 1 [1]:x [2]:x
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_3;
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[0], test_clk_out_io[2], &clkout_mapping_hdl_3));    // [0]:0(2) 1  2 [1]:x [2]:x

    // Test the clkout mapping ref_cnt validity
    esp_clock_output_stop(clkout_mapping_hdl_0);                                                            // [0]:0(1) 1  2 [1]:x [2]:x
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_4;
    TEST_ESP_ERR(esp_clock_output_start(test_clk_out_sig[1], test_clk_out_io[0], &clkout_mapping_hdl_4), ESP_ERR_INVALID_ARG);
    esp_clock_output_stop(clkout_mapping_hdl_1);                                                            // [0]:1  2 [1]:x [2]:x
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[1], test_clk_out_io[0], &clkout_mapping_hdl_4));    // [0]:1  2 [1]:0 [2]:x

#if CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
    test_clk_out_sig[3] = CLKOUT_SIG_PLL_F80M;
#else
    test_clk_out_sig[3] = CLKOUT_SIG_RC_32K;
#endif

    // Test the excess clock channel request assertion
    esp_clock_output_stop(clkout_mapping_hdl_3);                                                            // [0]:1  [1]:0 [2]:x
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_5;
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[2], test_clk_out_io[2], &clkout_mapping_hdl_5));    // [0]:1  [1]:0 [2]:2
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_6;
    TEST_ASSERT(ESP_FAIL == esp_clock_output_start(test_clk_out_sig[3], test_clk_out_io[3], &clkout_mapping_hdl_6));

    // Test the clock out channel recycle
    esp_clock_output_stop(clkout_mapping_hdl_4);                                                            // [0]:1  [1]:x [2]:2
    esp_clock_output_mapping_handle_t clkout_mapping_hdl_7;
    TEST_ESP_OK(esp_clock_output_start(test_clk_out_sig[3], test_clk_out_io[0], &clkout_mapping_hdl_7));    // [0]:1  [1]:0 [2]:2

    // Stop all
    esp_clock_output_stop(clkout_mapping_hdl_2);
    esp_clock_output_stop(clkout_mapping_hdl_5);
    esp_clock_output_stop(clkout_mapping_hdl_6);
    esp_clock_output_stop(clkout_mapping_hdl_7);
}
#endif
