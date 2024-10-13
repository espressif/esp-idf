/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "test_util_rmt_encoders.h"
#include "test_board.h"

#if CONFIG_RMT_ISR_IRAM_SAFE
#define TEST_RMT_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_RMT_CALLBACK_ATTR
#endif

typedef struct {
    TaskHandle_t task_to_notify;
    size_t received_symbol_num;
} test_rx_user_data_t;

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    test_rx_user_data_t *test_user_data = (test_rx_user_data_t *)user_data;
    rmt_symbol_word_t *remote_codes = edata->received_symbols;
    esp_rom_printf("%u symbols decoded:\r\n", edata->num_symbols);
    for (size_t i = 0; i < edata->num_symbols; i++) {
        esp_rom_printf("{%d:%d},{%d:%d}\r\n", remote_codes[i].level0, remote_codes[i].duration0, remote_codes[i].level1, remote_codes[i].duration1);
    }
    test_user_data->received_symbol_num = edata->num_symbols;
    vTaskNotifyGiveFromISR(test_user_data->task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void test_rmt_rx_nec_carrier(size_t mem_block_symbols, bool with_dma, rmt_clock_source_t clk_src)
{
    uint32_t const test_rx_buffer_symbols = 128;
    rmt_symbol_word_t *remote_codes = heap_caps_aligned_calloc(64, test_rx_buffer_symbols, sizeof(rmt_symbol_word_t),
                                                               MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(remote_codes);

    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = mem_block_symbols,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
        .flags.with_dma = with_dma,
    };
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));
    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_rx_done_callback,
    };
    test_rx_user_data_t test_user_data = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, &test_user_data));

    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .trans_queue_depth = 4,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    printf("install nec protocol encoder\r\n");
    rmt_encoder_handle_t nec_encoder = NULL;
    TEST_ESP_OK(test_rmt_new_nec_protocol_encoder(&nec_encoder));
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));
    printf("enable rx channel\r\n");
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };

    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

#if SOC_RMT_SUPPORT_RX_PINGPONG
    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send customized NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0xFF00, 0xFF00, 0xFF00, 0xFF00
    }, 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(66, test_user_data.received_symbol_num);
#else
    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send customized NEC frame without carrier\r\n");
    // the maximum symbols can receive is its memory block capacity
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00
    }, 10, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(test_user_data.received_symbol_num, mem_block_symbols);
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

#if SOC_RMT_SUPPORT_RX_DEMODULATION
    rmt_carrier_config_t carrier_cfg = {
        .duty_cycle = 0.33,
        .frequency_hz = 38000,
    };
    printf("enable modulation for tx channel\r\n");
    TEST_ESP_OK(rmt_apply_carrier(tx_channel, &carrier_cfg));
    printf("enable demodulation for rx channel\r\n");
    // need to leave a tolerance for the carrier demodulation, can't set the carrier frequency exactly to 38KHz
    // should reduce frequency to some extend
    carrier_cfg.frequency_hz = 25000;
    TEST_ESP_OK(rmt_apply_carrier(rx_channel, &carrier_cfg));

    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send NEC frame with carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

#if SOC_RMT_SUPPORT_RX_PINGPONG
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send customized frame with carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0xFF00, 0xFF00, 0xFF00, 0xFF00
    }, 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(66, test_user_data.received_symbol_num);
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

    printf("disable modulation and demodulation for tx and rx channels\r\n");
    TEST_ESP_OK(rmt_apply_carrier(tx_channel, NULL));
    TEST_ESP_OK(rmt_apply_carrier(rx_channel, NULL));
#endif // SOC_RMT_SUPPORT_RX_DEMODULATION

    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, -1));

    // test rmt receive with unaligned buffer
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, rmt_receive(rx_channel, remote_codes, 13, &receive_config));

    printf("disable tx and rx channels\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    TEST_ESP_OK(rmt_disable(rx_channel));
    printf("delete channels and encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_encoder(nec_encoder));
    free(remote_codes);
}

TEST_CASE("rmt rx nec with carrier", "[rmt]")
{
    // test width different clock sources
    rmt_clock_source_t clk_srcs[] = SOC_RMT_CLKS;
    for (size_t i = 0; i < sizeof(clk_srcs) / sizeof(clk_srcs[0]); i++) {
        test_rmt_rx_nec_carrier(SOC_RMT_MEM_WORDS_PER_CHANNEL, false, clk_srcs[i]);
    }
#if SOC_RMT_SUPPORT_DMA
    test_rmt_rx_nec_carrier(128, true, RMT_CLK_SRC_DEFAULT);
#endif
}

#if SOC_RMT_SUPPORT_RX_PINGPONG
#define TEST_RMT_SYMBOLS 10000   // a very long frame, contains 10000 symbols

static void pwm_bit_bang(int gpio_num)
{
    for (int i = 0; i < TEST_RMT_SYMBOLS; i++) {
        gpio_set_level(gpio_num, 1);
        esp_rom_delay_us(50);
        gpio_set_level(gpio_num, 0);
        esp_rom_delay_us(50);
    }
}

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_partial_receive_done(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    test_rx_user_data_t *test_user_data = (test_rx_user_data_t *)user_data;
    test_user_data->received_symbol_num += edata->num_symbols;
    // when receive done, notify the task to check the received data
    if (edata->flags.is_last) {
        vTaskNotifyGiveFromISR(test_user_data->task_to_notify, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

static void test_rmt_partial_receive(size_t mem_block_symbols, bool with_dma, rmt_clock_source_t clk_src)
{
    uint32_t const test_rx_buffer_symbols = 128; // the user buffer is small, it can't hold all the received symbols
    rmt_symbol_word_t *receive_user_buf = heap_caps_aligned_calloc(64, test_rx_buffer_symbols, sizeof(rmt_symbol_word_t),
                                                                   MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(receive_user_buf);

    gpio_config_t sig_simulator_io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_RMT_GPIO_NUM_A,
    };
    TEST_ESP_OK(gpio_config(&sig_simulator_io_conf));

    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = mem_block_symbols,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
        .flags.with_dma = with_dma,
    };
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    // initialize the GPIO level to low
    TEST_ESP_OK(gpio_set_level(TEST_RMT_GPIO_NUM_A, 0));

    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_partial_receive_done,
    };
    test_rx_user_data_t test_user_data = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
        .received_symbol_num = 0,
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, &test_user_data));

    printf("enable rx channel\r\n");
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_receive_config_t rx_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
        .flags.en_partial_rx = true,  // enable partial receive
    };
    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, receive_user_buf, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &rx_config));

    // simulate input signal by GPIO
    pwm_bit_bang(TEST_RMT_GPIO_NUM_A);

    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(2000)));
    printf("received %zu symbols\r\n", test_user_data.received_symbol_num);
    TEST_ASSERT_EQUAL(TEST_RMT_SYMBOLS, test_user_data.received_symbol_num);
    // verify the received data
    for (int i = 0; i < 10; i++) {
        printf("{%d:%d},{%d:%d}\r\n", receive_user_buf[i].level0, receive_user_buf[i].duration0, receive_user_buf[i].level1, receive_user_buf[i].duration1);
        TEST_ASSERT_EQUAL(1, receive_user_buf[i].level0);
        TEST_ASSERT_INT_WITHIN(20, 50, receive_user_buf[i].duration0);
        TEST_ASSERT_EQUAL(0, receive_user_buf[i].level1);
        TEST_ASSERT_INT_WITHIN(20, 50, receive_user_buf[i].duration1);
    }

    printf("disable rx channels\r\n");
    TEST_ESP_OK(rmt_disable(rx_channel));
    printf("delete channels and encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    free(receive_user_buf);
}

TEST_CASE("rmt rx long frame partially", "[rmt]")
{
    test_rmt_partial_receive(SOC_RMT_MEM_WORDS_PER_CHANNEL, false, RMT_CLK_SRC_DEFAULT);
#if SOC_RMT_SUPPORT_DMA
    test_rmt_partial_receive(256, true, RMT_CLK_SRC_DEFAULT);
#endif
}
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_received_done(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    test_rx_user_data_t *test_user_data = (test_rx_user_data_t *)user_data;
    test_user_data->received_symbol_num += edata->num_symbols;
    // when receive done, notify the task to check the received data
    if (edata->flags.is_last) {
        vTaskNotifyGiveFromISR(test_user_data->task_to_notify, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

static void test_rmt_receive_filter(rmt_clock_source_t clk_src)
{
    uint32_t const test_rx_buffer_symbols = 32;
    rmt_symbol_word_t *receive_user_buf = heap_caps_aligned_calloc(64, test_rx_buffer_symbols, sizeof(rmt_symbol_word_t),
                                                                   MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(receive_user_buf);

    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
    };
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_received_done,
    };
    test_rx_user_data_t test_user_data = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
        .received_symbol_num = 0,
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, &test_user_data));

    // use TX channel to simulate the input signal
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .trans_queue_depth = 4,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    printf("install a simple copy encoder\r\n");
    rmt_encoder_handle_t copy_encoder = NULL;
    rmt_copy_encoder_config_t encoder_cfg = {};
    TEST_ESP_OK(rmt_new_copy_encoder(&encoder_cfg, &copy_encoder));
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));
    printf("enable rx channel\r\n");
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_receive_config_t rx_config = {
        .signal_range_min_ns = 3000, // filter out the pulses shorter than 3us
        .signal_range_max_ns = 12000000,
    };
    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, receive_user_buf, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &rx_config));

    // generate short pulse of width 2us, should be filtered out
    printf("send a short pulse\r\n");
    rmt_symbol_word_t short_pulse = {
        .level0 = 1,
        .duration0 = 2, // pulse width 2us
        .level1 = 0,
        .duration1 = 1,
    };
    TEST_ESP_OK(rmt_transmit(tx_channel, copy_encoder, &short_pulse, sizeof(short_pulse), &transmit_config));
    TEST_ASSERT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    printf("received %zu symbols\r\n", test_user_data.received_symbol_num);
    TEST_ASSERT_EQUAL(0, test_user_data.received_symbol_num);

    printf("send a long pulse\r\n");
    // generate long pulse of width 10us, should be received
    rmt_symbol_word_t long_pulse = {
        .level0 = 1,
        .duration0 = 10, // pulse width 10us
        .level1 = 0,
        .duration1 = 1,
    };
    TEST_ESP_OK(rmt_transmit(tx_channel, copy_encoder, &long_pulse, sizeof(long_pulse), &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    printf("received %zu symbols\r\n", test_user_data.received_symbol_num);
    TEST_ASSERT_EQUAL(1, test_user_data.received_symbol_num);

    // verify the received data
    printf("{%d:%d},{%d:%d}\r\n", receive_user_buf[0].level0, receive_user_buf[0].duration0, receive_user_buf[0].level1, receive_user_buf[0].duration1);
    TEST_ASSERT_EQUAL(1, receive_user_buf[0].level0);
    TEST_ASSERT_INT_WITHIN(2, 10, receive_user_buf[0].duration0);
    TEST_ASSERT_EQUAL(0, receive_user_buf[0].level1);

    printf("disable tx and rx channels\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    TEST_ESP_OK(rmt_disable(rx_channel));
    printf("delete channels and encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_encoder(copy_encoder));
    free(receive_user_buf);
}

TEST_CASE("rmt rx filter functionality", "[rmt]")
{
    // test width different clock sources
    rmt_clock_source_t clk_srcs[] = SOC_RMT_CLKS;
    for (size_t i = 0; i < sizeof(clk_srcs) / sizeof(clk_srcs[0]); i++) {
        test_rmt_receive_filter(clk_srcs[i]);
    }
}
