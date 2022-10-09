/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "soc/soc_caps.h"
#include "test_util_rmt_encoders.h"

static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(10000);
}

static void test_rmt_tx_iram_safe(size_t mem_block_symbols, bool with_dma)
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .mem_block_symbols = mem_block_symbols,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000, // 10MHz, 1 tick = 0.1us (led strip needs a high resolution)
        .trans_queue_depth = 4,
        .gpio_num = 0,
        .flags.with_dma = with_dma,
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel_multi_leds = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel_multi_leds));
    printf("install led strip encoder\r\n");
    rmt_encoder_handle_t led_strip_encoder = NULL;
    TEST_ESP_OK(test_rmt_new_led_strip_encoder(&led_strip_encoder));
    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel_multi_leds));

    // Mutiple LEDs (ping-pong in the background)
    printf("ping pong transmission: light up 100 RGB LEDs\r\n");
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };

    const int test_led_num = 100;
    uint8_t leds_grb[test_led_num * 3];
    // color: Material Design Green-A200 (#69F0AE)
    for (int i = 0; i < test_led_num * 3; i += 3) {
        leds_grb[i + 0] = 0xF0;
        leds_grb[i + 1] = 0x69;
        leds_grb[i + 2] = 0xAE;
    }
    printf("start transmission and stop immediately, only a few LEDs are light up\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, test_led_num * 3, &transmit_config));
    // this second transmission will stay in the queue and shouldn't be dispatched until we restart the tx channel later
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, test_led_num * 3, &transmit_config));

    unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);

    // color: Material Design Orange-900 (#E65100)
    for (int i = 0; i < test_led_num * 3; i += 3) {
        leds_grb[i + 0] = 0x51;
        leds_grb[i + 1] = 0xE6;
        leds_grb[i + 2] = 0x00;
    }
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, test_led_num * 3, &transmit_config));
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel_multi_leds, -1));

    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel_multi_leds));
    printf("remove tx channel and led strip encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel_multi_leds));
    TEST_ESP_OK(rmt_del_encoder(led_strip_encoder));
}

TEST_CASE("rmt_tx_iram_safe_no_dma", "[rmt]")
{
    test_rmt_tx_iram_safe(SOC_RMT_MEM_WORDS_PER_CHANNEL, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_tx_iram_safe_with_dma", "[rmt]")
{
    test_rmt_tx_iram_safe(1024, true);
}
#endif


static void IRAM_ATTR test_simulate_input_post_cache_disable(void *args)
{
    int gpio_num = (int)args;
    // simulate input signal, should only be recognized as one RMT symbol
    gpio_set_level(gpio_num, 0);
    esp_rom_delay_us(50);
    gpio_set_level(gpio_num, 1);
    esp_rom_delay_us(50);
    gpio_set_level(gpio_num, 0);
    esp_rom_delay_us(20000);
}

typedef struct {
    TaskHandle_t task_to_notify;
    size_t received_symbol_num;
} test_nec_rx_user_data_t;

IRAM_ATTR
static bool test_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    test_nec_rx_user_data_t *test_user_data = (test_nec_rx_user_data_t *)user_data;
    test_user_data->received_symbol_num = edata->num_symbols;
    vTaskNotifyGiveFromISR(test_user_data->task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void test_rmt_rx_iram_safe(size_t mem_block_symbols, bool with_dma, rmt_clock_source_t clk_src)
{
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = mem_block_symbols,
        .gpio_num = 0,
        .flags.with_dma = with_dma,
        .flags.io_loop_back = true, // the GPIO will act like a loopback
    };
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    // initialize the GPIO level to low
    TEST_ESP_OK(gpio_set_level(0, 0));

    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_rx_done_callback,
    };
    test_nec_rx_user_data_t test_user_data = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, &test_user_data));

    printf("enable rx channel\r\n");
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_symbol_word_t remote_codes[128];

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };

    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));

    // disable the flash cache, and simulate input signal by GPIO
    unity_utils_run_cache_disable_stub(test_simulate_input_post_cache_disable, 0);

    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(1, test_user_data.received_symbol_num);

    printf("disable rx channels\r\n");
    TEST_ESP_OK(rmt_disable(rx_channel));
    printf("delete channels and encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(rx_channel));
}

TEST_CASE("rmt_rx_iram_safe_no_dma", "[rmt]")
{
    test_rmt_rx_iram_safe(SOC_RMT_MEM_WORDS_PER_CHANNEL, false, RMT_CLK_SRC_DEFAULT);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_rx_iram_safe_with_dma", "[rmt]")
{
    test_rmt_rx_iram_safe(128, true, RMT_CLK_SRC_DEFAULT);
}
#endif
