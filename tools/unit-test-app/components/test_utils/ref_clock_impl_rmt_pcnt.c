/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Some unit test cases need to have access to reliable timestamps even when CPU and APB clock frequencies change over time.
 * This reference clock is built upon two peripherals: one RMT channel and one PCNT channel (hopefully we can have these two peripherals in all ESP chips).
 *
 * +---------------------+        500KHz Square Wave          +--------------------------+
 * | RMT (channel 0, TX) +----------------------------------->+ PCNT (unit 0, channel 0) |
 * +---------------------+                                    +--------------------------+
 *
 * RMT TX channel is configured to use a fixed clock (e.g. REF_TICK, XTAL) as clock source, so that our ref clock won't be affected during APB/CPU clock switch.
 * Configure RMT channel to generate a 500KHz square wave (using carrier feature) to one GPIO.
 * PCNT takes the input signal from the GPIO and counts the edges (which occur at 1MHz frequency).
 * PCNT counter is only 16 bit wide, an interrupt is configured to trigger when the counter reaches 30000,
 * incrementing a 32-bit millisecond counter maintained by software.
 */

#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "driver/pulse_cnt.h"
#include "driver/rmt_tx.h"

#if !CONFIG_IDF_TARGET_ESP32
#error "RMT+PCNT timestamp workaround is only for ESP32"
#endif

#define REF_CLOCK_GPIO         0  // GPIO used to combine RMT out signal with PCNT input signal
#define REF_CLOCK_PRESCALER_MS 30 // PCNT high threshold interrupt fired every 30ms

// peripheral driver handles
static pcnt_unit_handle_t s_pcnt_unit;
static pcnt_channel_handle_t s_pcnt_chan;
static rmt_channel_handle_t s_rmt_chan;
static rmt_encoder_handle_t s_rmt_encoder;

void ref_clock_init(void)
{
    // Initialize PCNT
    pcnt_unit_config_t unit_config = {
        .high_limit = REF_CLOCK_PRESCALER_MS * 1000,
        .low_limit = -100, // any minus value is OK, in this case, we don't count down
        .flags.accum_count = true, // accumulate the counter value
    };
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &s_pcnt_unit));
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = REF_CLOCK_GPIO,
        .level_gpio_num = -1,
        .flags.io_loop_back = true,
    };
    TEST_ESP_OK(pcnt_new_channel(s_pcnt_unit, &chan_config, &s_pcnt_chan));
    // increase count on both edges
    TEST_ESP_OK(pcnt_channel_set_edge_action(s_pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    // don't care level change
    TEST_ESP_OK(pcnt_channel_set_level_action(s_pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    // add watch point: high limit
    TEST_ESP_OK(pcnt_unit_add_watch_point(s_pcnt_unit, REF_CLOCK_PRESCALER_MS * 1000));
    // enable pcnt
    TEST_ESP_OK(pcnt_unit_enable(s_pcnt_unit));
    // start pcnt
    TEST_ESP_OK(pcnt_unit_start(s_pcnt_unit));

    // Initialize RMT
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_REF_TICK, // REF_TICK clock source
        .gpio_num = REF_CLOCK_GPIO,
        .mem_block_symbols = 64,
        .resolution_hz = 10000, // channel resolution doesn't really matter, because we only utilize the carrier
        .trans_queue_depth = 1,
        .flags.io_loop_back = true,
    };
    TEST_ESP_OK(rmt_new_tx_channel(&tx_chan_config, &s_rmt_chan));
    // set carrier configuration
    rmt_carrier_config_t carrier_config = {
        .duty_cycle = 0.5,
        .frequency_hz = 500 * 1000, // 500 KHz
    };
    TEST_ESP_OK(rmt_apply_carrier(s_rmt_chan, &carrier_config));
    // enable rmt channel
    TEST_ESP_OK(rmt_enable(s_rmt_chan));
    // create a copy encoder to copy the RMT symbol into RMT HW memory
    rmt_copy_encoder_config_t encoder_config = {};
    TEST_ESP_OK(rmt_new_copy_encoder(&encoder_config, &s_rmt_encoder));

    // control the tx channel to output a fixed high level by constructing the following RMT symbol
    // the carrier is modulated to the high level by default, which results in a 500KHz carrier on the `REF_CLOCK_GPIO`
    rmt_symbol_word_t data = {
        .level0 = 1,
        .duration0 = 1,
        .level1 = 1,
        .duration1 = 0,
    };
    rmt_transmit_config_t trans_config = {
        .loop_count = 0, // no loop
        .flags.eot_level = 1,
    };
    TEST_ESP_OK(rmt_transmit(s_rmt_chan, s_rmt_encoder, &data, sizeof(data), &trans_config));
}

void ref_clock_deinit(void)
{
    // Deinitialize PCNT
    TEST_ESP_OK(pcnt_unit_stop(s_pcnt_unit));
    TEST_ESP_OK(pcnt_unit_disable(s_pcnt_unit));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(s_pcnt_unit, REF_CLOCK_PRESCALER_MS * 1000));
    TEST_ESP_OK(pcnt_del_channel(s_pcnt_chan));
    TEST_ESP_OK(pcnt_del_unit(s_pcnt_unit));

    // Deinitialize RMT
    TEST_ESP_OK(rmt_disable(s_rmt_chan));
    TEST_ESP_OK(rmt_del_channel(s_rmt_chan));
    TEST_ESP_OK(rmt_del_encoder(s_rmt_encoder));
}

uint64_t ref_clock_get(void)
{
    int microseconds = 0;
    TEST_ESP_OK(pcnt_unit_get_count(s_pcnt_unit, &microseconds));
    // because the PCNT is configured to always count up, it's impossible to get a negative value
    return (uint64_t)microseconds;
}
