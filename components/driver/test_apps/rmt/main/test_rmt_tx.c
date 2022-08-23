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
#include "driver/rmt_tx.h"
#include "esp_timer.h"
#include "soc/soc_caps.h"
#include "test_util_rmt_encoders.h"

#if CONFIG_RMT_ISR_IRAM_SAFE
#define TEST_RMT_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_RMT_CALLBACK_ATTR
#endif

static void test_rmt_channel_single_trans(size_t mem_block_symbols, bool with_dma)
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
    rmt_channel_handle_t tx_channel_single_led = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel_single_led));
    printf("install led strip encoder\r\n");
    rmt_encoder_handle_t led_strip_encoder = NULL;
    TEST_ESP_OK(test_rmt_new_led_strip_encoder(&led_strip_encoder));
    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel_single_led));

    printf("single transmission: light up one RGB LED\r\n");
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };
    TEST_ESP_OK(rmt_transmit(tx_channel_single_led, led_strip_encoder, (uint8_t[]) {
        0x00, 0x7F, 0xFF
    }, 3, &transmit_config));
    // adding extra delay here for visualizing
    vTaskDelay(pdMS_TO_TICKS(500));
    TEST_ESP_OK(rmt_transmit(tx_channel_single_led, led_strip_encoder, (uint8_t[]) {
        0xFF, 0x00, 0x7F
    }, 3, &transmit_config));
    vTaskDelay(pdMS_TO_TICKS(500));
    TEST_ESP_OK(rmt_transmit(tx_channel_single_led, led_strip_encoder, (uint8_t[]) {
        0x7F, 0xFF, 0x00
    }, 3, &transmit_config));
    vTaskDelay(pdMS_TO_TICKS(500));

    // can't delete channel if it's not in stop state
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, rmt_del_channel(tx_channel_single_led));
    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel_single_led));
    printf("remove tx channel and led strip encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel_single_led));
    TEST_ESP_OK(rmt_del_encoder(led_strip_encoder));
}

TEST_CASE("rmt_single_trans_no_dma", "[rmt]")
{
    test_rmt_channel_single_trans(SOC_RMT_MEM_WORDS_PER_CHANNEL, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_single_trans_with_dma", "[rmt]")
{
    test_rmt_channel_single_trans(512, true);
}
#endif

static void test_rmt_ping_pong_trans(size_t mem_block_symbols, bool with_dma)
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
#define TEST_LED_NUM 100
    uint8_t leds_grb[TEST_LED_NUM * 3] = {};
    // color: Material Design Green-A200 (#69F0AE)
    for (int i = 0; i < TEST_LED_NUM * 3; i += 3) {
        leds_grb[i + 0] = 0xF0;
        leds_grb[i + 1] = 0x69;
        leds_grb[i + 2] = 0xAE;
    }
    printf("start transmission and stop immediately, only a few LEDs are light up\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, TEST_LED_NUM * 3, &transmit_config));
    // this second transmission will stay in the queue and shouldn't be dispatched until we restart the tx channel later
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, TEST_LED_NUM * 3, &transmit_config));
    esp_rom_delay_us(100);
    TEST_ESP_OK(rmt_disable(tx_channel_multi_leds));
    vTaskDelay(pdTICKS_TO_MS(500));

    printf("enable tx channel again\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel_multi_leds));
    // adding extra delay here for visualizing
    vTaskDelay(pdTICKS_TO_MS(500));
    // color: Material Design Pink-A200 (#FF4081)
    for (int i = 0; i < TEST_LED_NUM * 3; i += 3) {
        leds_grb[i + 0] = 0x40;
        leds_grb[i + 1] = 0xFF;
        leds_grb[i + 2] = 0x81;
    }
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, TEST_LED_NUM * 3, &transmit_config));
    vTaskDelay(pdTICKS_TO_MS(500));
    // color: Material Design Orange-900 (#E65100)
    for (int i = 0; i < TEST_LED_NUM * 3; i += 3) {
        leds_grb[i + 0] = 0x51;
        leds_grb[i + 1] = 0xE6;
        leds_grb[i + 2] = 0x00;
    }
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, TEST_LED_NUM * 3, &transmit_config));
    vTaskDelay(pdTICKS_TO_MS(500));

    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel_multi_leds));
    printf("remove tx channel and led strip encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel_multi_leds));
    TEST_ESP_OK(rmt_del_encoder(led_strip_encoder));
#undef TEST_LED_NUM
}

TEST_CASE("rmt_ping_pong_trans_no_dma", "[rmt]")
{
    test_rmt_ping_pong_trans(SOC_RMT_MEM_WORDS_PER_CHANNEL, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_ping_pong_trans_with_dma", "[rmt]")
{
    test_rmt_ping_pong_trans(1024, true);
}
#endif

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_tx_done_cb_check_event_data(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
{
    uint32_t *p_expected_encoded_size = (uint32_t *)user_data;
    TEST_ASSERT_EQUAL(*p_expected_encoded_size, edata->num_symbols);
    return false;
}

static void test_rmt_trans_done_event(size_t mem_block_symbols, bool with_dma)
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .mem_block_symbols = mem_block_symbols,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000, // 10MHz, 1 tick = 0.1us (led strip needs a high resolution)
        .trans_queue_depth = 1,
        .gpio_num = 0,
        .flags.with_dma = with_dma,
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel_multi_leds = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel_multi_leds));
    printf("install led strip encoder\r\n");
    rmt_encoder_handle_t led_strip_encoder = NULL;
    TEST_ESP_OK(test_rmt_new_led_strip_encoder(&led_strip_encoder));

    printf("register trans done event callback\r\n");
    rmt_tx_event_callbacks_t cbs = {
        .on_trans_done = test_rmt_tx_done_cb_check_event_data,
    };
    uint32_t expected_encoded_size = 0;
    TEST_ESP_OK(rmt_tx_register_event_callbacks(tx_channel_multi_leds, &cbs, &expected_encoded_size));

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel_multi_leds));

    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };

    printf("transmit dynamic number of LEDs\r\n");
#define TEST_LED_NUM 40
    uint8_t leds_grb[TEST_LED_NUM * 3] = {};
    // color: Material Design Purple-800 (6A1B9A)
    for (int i = 0; i < TEST_LED_NUM * 3; i += 3) {
        leds_grb[i + 0] = 0x1B;
        leds_grb[i + 1] = 0x6A;
        leds_grb[i + 2] = 0x9A;
    }
    for (int i = 1; i <= TEST_LED_NUM; i++) {
        expected_encoded_size = 2 + i * 24; // 2 = 1 reset symbol + 1 eof symbol, 24 = 8*3(RGB)
        TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, i * 3, &transmit_config));
        // wait for the transmission finished and recycled
        TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel_multi_leds, -1));
    }

    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel_multi_leds));
    printf("remove tx channel and led strip encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel_multi_leds));
    TEST_ESP_OK(rmt_del_encoder(led_strip_encoder));
#undef TEST_LED_NUM
}

TEST_CASE("rmt_trans_done_event_callback_no_dma", "[rmt]")
{
    test_rmt_trans_done_event(SOC_RMT_MEM_WORDS_PER_CHANNEL, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_trans_done_event_callback_with_dma", "[rmt]")
{
    test_rmt_trans_done_event(332, true);
}
#endif

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_loop_done_cb_check_event_data(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
{
    uint32_t *p_expected_encoded_size = (uint32_t *)user_data;
    TEST_ASSERT_EQUAL(*p_expected_encoded_size, edata->num_symbols);
    return false;
}

static void test_rmt_loop_trans(size_t mem_block_symbols, bool with_dma)
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

    printf("register loop done event callback\r\n");
    rmt_tx_event_callbacks_t cbs = {
        .on_trans_done = test_rmt_loop_done_cb_check_event_data,
    };
    uint32_t expected_encoded_size = 0;
    TEST_ESP_OK(rmt_tx_register_event_callbacks(tx_channel_multi_leds, &cbs, &expected_encoded_size));

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel_multi_leds));

    printf("loop transmission: light up RGB LEDs in a loop\r\n");
    rmt_transmit_config_t transmit_config = {
        .loop_count = 5,
    };
#define TEST_LED_NUM 3
    uint8_t leds_grb[TEST_LED_NUM * 3] = {};
    for (int i = 0; i < TEST_LED_NUM * 3; i++) {
        leds_grb[i] = 0x10 + i;
    }
    expected_encoded_size = 2 + 24 * TEST_LED_NUM;
    TEST_ESP_OK(rmt_transmit(tx_channel_multi_leds, led_strip_encoder, leds_grb, TEST_LED_NUM * 3, &transmit_config));
    vTaskDelay(pdTICKS_TO_MS(100));

    printf("wait for loop transactions done\r\n");
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel_multi_leds, -1));
    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel_multi_leds));
    printf("remove tx channel and led strip encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel_multi_leds));
    TEST_ESP_OK(rmt_del_encoder(led_strip_encoder));
#undef TEST_LED_NUM
}

TEST_CASE("rmt_loop_trans_no_dma", "[rmt]")
{
    test_rmt_loop_trans(SOC_RMT_MEM_WORDS_PER_CHANNEL * 2, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_loop_trans_with_dma", "[rmt]")
{
    test_rmt_loop_trans(128, true);
}
#endif // SOC_RMT_SUPPORT_DMA
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT

TEST_CASE("rmt_infinite_loop_trans", "[rmt]")
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .gpio_num = 2,
        .trans_queue_depth = 3,
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));
    printf("install step motor encoder\r\n");
    // stepper encoder is as simple as a copy encoder
    rmt_encoder_t *copy_encoder = NULL;
    rmt_copy_encoder_config_t copy_encoder_config = {};
    TEST_ESP_OK(rmt_new_copy_encoder(&copy_encoder_config, &copy_encoder));

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));

    rmt_transmit_config_t transmit_config = {
        .loop_count = -1, // infinite loop transmission
    };

    printf("infinite loop transmission: keep spinning stepper motor\r\n");
    uint32_t step_motor_frequency_hz = 1000; // 1KHz
    uint32_t rmt_raw_symbol_duration = 1000000 / step_motor_frequency_hz / 2;
    // 1KHz PWM, Period: 1ms
    rmt_symbol_word_t stepper_motor_rmt_symbol = {
        .level0 = 0,
        .duration0 = rmt_raw_symbol_duration,
        .level1 = 1,
        .duration1 = rmt_raw_symbol_duration,
    };
    TEST_ESP_OK(rmt_transmit(tx_channel, copy_encoder, &stepper_motor_rmt_symbol, sizeof(stepper_motor_rmt_symbol), &transmit_config));
    // not trans done event should be triggered
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, rmt_tx_wait_all_done(tx_channel, 500));

    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    // the flush operation should return immediately, as there's not pending transactions and the TX machine has stopped
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, 0));

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    printf("enable tx channel again\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));

    printf("finite loop transmission: spinning stepper motor with various number of loops\r\n");
#define TEST_RMT_LOOPS 5
    uint32_t pwm_freq[TEST_RMT_LOOPS] = {};
    rmt_symbol_word_t pwm_rmt_symbols[TEST_RMT_LOOPS] = {};
    for (int i = 0; i < TEST_RMT_LOOPS; i++) {
        transmit_config.loop_count = 100 * i;
        pwm_freq[i] = 1000 * (i + 1);
        uint32_t pwm_symbol_duration = 1000000 / pwm_freq[i] / 2;
        // 1KHz PWM, Period: 1ms
        pwm_rmt_symbols[i] = (rmt_symbol_word_t) {
            .level0 = 0,
            .duration0 = pwm_symbol_duration,
            .level1 = 1,
            .duration1 = pwm_symbol_duration,
        };
        TEST_ESP_OK(rmt_transmit(tx_channel, copy_encoder, &pwm_rmt_symbols[i], sizeof(rmt_symbol_word_t), &transmit_config));
    }

    printf("wait for loop transactions done\r\n");
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, -1)); // wait forever
    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
#undef TEST_RMT_LOOPS
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT

    printf("remove tx channel and motor encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_encoder(copy_encoder));
}

static void test_rmt_tx_nec_carrier(size_t mem_block_symbols, bool with_dma)
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = mem_block_symbols,
        .gpio_num = 2,
        .trans_queue_depth = 4,
        .flags.with_dma = with_dma,
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));
    printf("install nec protocol encoder\r\n");
    rmt_encoder_handle_t nec_encoder = NULL;
    TEST_ESP_OK(test_rmt_new_nec_protocol_encoder(&nec_encoder));

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));

    printf("transmit nec frame without carrier\r\n");
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, -1));

    printf("transmit nec frame with carrier\r\n");
    rmt_carrier_config_t carrier_cfg = {
        .duty_cycle = 0.33,
        .frequency_hz = 38000,
    };
    TEST_ESP_OK(rmt_apply_carrier(tx_channel, &carrier_cfg));
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, -1));

    printf("remove carrier\r\n");
    TEST_ESP_OK(rmt_apply_carrier(tx_channel, NULL));

    printf("transmit nec frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, -1));

    printf("disable tx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    printf("remove tx channel and nec encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_encoder(nec_encoder));
}

TEST_CASE("rmt_tx_nec_carrier_no_dma", "[rmt]")
{
    test_rmt_tx_nec_carrier(SOC_RMT_MEM_WORDS_PER_CHANNEL, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_tx_nec_carrier_with_dma", "[rmt]")
{
    test_rmt_tx_nec_carrier(128, true);
}
#endif

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_tx_done_cb_record_time(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
{
    int64_t *record_time = (int64_t *)user_data;
    *record_time = esp_timer_get_time();
    return false;
}

static void test_rmt_multi_channels_trans(size_t channel0_mem_block_symbols, size_t channel1_mem_block_symbols, bool channel0_with_dma, bool channel1_with_dma)
{
#define TEST_RMT_CHANS 2
#define TEST_LED_NUM   24
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000, // 10MHz, 1 tick = 0.1us (led strip needs a high resolution)
        .trans_queue_depth = 4,
    };
    printf("install tx channels\r\n");
    rmt_channel_handle_t tx_channels[TEST_RMT_CHANS] = {NULL};
    int gpio_nums[TEST_RMT_CHANS] = {0, 2};
    size_t mem_blk_syms[TEST_RMT_CHANS] = {channel0_mem_block_symbols, channel1_mem_block_symbols};
    bool dma_flags[TEST_RMT_CHANS] = {channel0_with_dma, channel1_with_dma};
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        tx_channel_cfg.gpio_num = gpio_nums[i];
        tx_channel_cfg.mem_block_symbols = mem_blk_syms[i];
        tx_channel_cfg.flags.with_dma = dma_flags[i];
        TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[i]));
    }

    printf("install led strip encoders\r\n");
    rmt_encoder_handle_t led_strip_encoders[TEST_RMT_CHANS] = {NULL};
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(test_rmt_new_led_strip_encoder(&led_strip_encoders[i]));
    }

    printf("register tx event callback\r\n");
    rmt_tx_event_callbacks_t cbs = {
        .on_trans_done = test_rmt_tx_done_cb_record_time
    };
    int64_t record_stop_time[TEST_RMT_CHANS] = {};
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_tx_register_event_callbacks(tx_channels[i], &cbs, &record_stop_time[i]));
    }

    printf("enable tx channels\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_enable(tx_channels[i]));
    }

    uint8_t leds_grb[TEST_LED_NUM * 3] = {};
    // color: Material Design Green-A200 (#69F0AE)
    for (int i = 0; i < TEST_LED_NUM * 3; i += 3) {
        leds_grb[i + 0] = 0xF0;
        leds_grb[i + 1] = 0x69;
        leds_grb[i + 2] = 0xAE;
    }

    printf("transmit without synchronization\r\n");
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };
    // the channels should work independently, without synchronization
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_transmit(tx_channels[i], led_strip_encoders[i], leds_grb, TEST_LED_NUM * 3, &transmit_config));
    }
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_tx_wait_all_done(tx_channels[i], -1));
    }
    printf("stop time (no sync):\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        printf("\t%lld\r\n", record_stop_time[i]);
    }
    // without synchronization, there will be obvious time shift
    TEST_ASSERT((record_stop_time[1] - record_stop_time[0]) < 100);

    printf("install sync manager\r\n");
    rmt_sync_manager_handle_t synchro = NULL;
    rmt_sync_manager_config_t synchro_config = {
        .tx_channel_array = tx_channels,
        .array_size = TEST_RMT_CHANS,
    };
#if SOC_RMT_SUPPORT_TX_SYNCHRO
    TEST_ESP_OK(rmt_new_sync_manager(&synchro_config, &synchro));
#else
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, rmt_new_sync_manager(&synchro_config, &synchro));
#endif // SOC_RMT_SUPPORT_TX_SYNCHRO

#if SOC_RMT_SUPPORT_TX_SYNCHRO
    printf("transmit with synchronization\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_transmit(tx_channels[i], led_strip_encoders[i], leds_grb, TEST_LED_NUM * 3, &transmit_config));
        // manually introduce the delay, to show the managed channels are indeed in sync
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_tx_wait_all_done(tx_channels[i], -1));
    }
    printf("stop time (with sync):\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        printf("\t%lld\r\n", record_stop_time[i]);
    }
    // because of synchronization, the managed channels will stop at the same time
    // but call of `esp_timer_get_time` won't happen at the same time, so there still be time drift, very small
    TEST_ASSERT((record_stop_time[1] - record_stop_time[0]) < 10);

    printf("reset sync manager\r\n");
    TEST_ESP_OK(rmt_sync_reset(synchro));
    printf("transmit with synchronization again\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_transmit(tx_channels[i], led_strip_encoders[i], leds_grb, TEST_LED_NUM * 3, &transmit_config));
        // manually introduce the delay, ensure the channels get synchronization
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_tx_wait_all_done(tx_channels[i], -1));
    }
    printf("stop time (with sync):\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        printf("\t%lld\r\n", record_stop_time[i]);
    }
    TEST_ASSERT((record_stop_time[1] - record_stop_time[0]) < 10);

    printf("delete sync manager\r\n");
    TEST_ESP_OK(rmt_del_sync_manager(synchro));
#endif // SOC_RMT_SUPPORT_TX_SYNCHRO

    printf("disable tx channels\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_disable(tx_channels[i]));
    }
    printf("delete channels and encoders\r\n");
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_del_channel(tx_channels[i]));
    }
    for (int i = 0; i < TEST_RMT_CHANS; i++) {
        TEST_ESP_OK(rmt_del_encoder(led_strip_encoders[i]));
    }
#undef TEST_LED_NUM
#undef TEST_RMT_CHANS
}

TEST_CASE("rmt_multi_channels_trans_no_dma", "[rmt]")
{
    test_rmt_multi_channels_trans(SOC_RMT_MEM_WORDS_PER_CHANNEL, SOC_RMT_MEM_WORDS_PER_CHANNEL, false, false);
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_multi_channels_trans_with_dma", "[rmt]")
{
    test_rmt_multi_channels_trans(1024, SOC_RMT_MEM_WORDS_PER_CHANNEL, true, false);
}
#endif
