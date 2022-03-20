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
#include "esp_intr_alloc.h"
#include "esp_private/periph_ctrl.h"
#include "driver/rmt.h"
#include "driver/pulse_cnt.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_periph.h"
#include "soc/soc_caps.h"
#include "hal/rmt_types.h"
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

#if !CONFIG_IDF_TARGET_ESP32
#error "RMT+PCNT timestamp workaround is only for ESP32"
#endif

#define REF_CLOCK_RMT_CHANNEL  0  // RMT channel 0
#define REF_CLOCK_GPIO         21 // GPIO used to combine RMT out signal with PCNT input signal
#define REF_CLOCK_PRESCALER_MS 30 // PCNT high threshold interrupt fired every 30ms

static rmt_hal_context_t s_rmt_hal;
static pcnt_unit_handle_t s_pcnt_unit;
static pcnt_channel_handle_t s_pcnt_chan;
static volatile uint32_t s_milliseconds;

// RMTMEM address is declared in <target>.peripherals.ld
extern rmt_mem_t RMTMEM;

static bool on_reach_watch_point(pcnt_unit_handle_t unit, pcnt_watch_event_data_t *edata, void *user_ctx)
{
    s_milliseconds += REF_CLOCK_PRESCALER_MS;
    return false;
}

void ref_clock_init(void)
{
    // Initialize PCNT
    pcnt_unit_config_t unit_config = {
        .high_limit = REF_CLOCK_PRESCALER_MS * 1000,
        .low_limit = -100, // any minus value is OK, in this case, we don't count down
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
    // add watch point
    TEST_ESP_OK(pcnt_unit_add_watch_point(s_pcnt_unit, REF_CLOCK_PRESCALER_MS * 1000));
    // register watch event
    pcnt_event_callbacks_t cbs = {
        .on_reach = on_reach_watch_point,
    };
    TEST_ESP_OK(pcnt_unit_register_event_callbacks(s_pcnt_unit, &cbs, NULL));
    // start pcnt
    TEST_ESP_OK(pcnt_unit_start(s_pcnt_unit));

    // Route RMT output to GPIO matrix
    esp_rom_gpio_connect_out_signal(REF_CLOCK_GPIO, RMT_SIG_OUT0_IDX, false, false);

    // Initialize RMT
    periph_module_enable(PERIPH_RMT_MODULE);
    rmt_hal_init(&s_rmt_hal);

    rmt_item32_t data = {
        .duration0 = 1,
        .level0 = 1,
        .duration1 = 0,
        .level1 = 0
    };

    rmt_ll_enable_periph_clock(s_rmt_hal.regs, true);
    rmt_ll_set_group_clock_src(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, RMT_CLK_SRC_REFTICK, 1, 1, 0); // select REF_TICK (1MHz)
    rmt_ll_tx_set_channel_clock_div(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1); // channel clock = REF_TICK / 1 = 1MHz
    rmt_ll_tx_fix_idle_level(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1, true); // enable idle output, idle level: 1
    rmt_ll_tx_enable_carrier_modulation(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, true);
    rmt_ll_tx_set_carrier_high_low_ticks(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1, 1);  // set carrier to 1MHz / (1+1) = 500KHz, 50% duty cycle
    rmt_ll_tx_set_carrier_level(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1);
    rmt_ll_enable_mem_access_nonfifo(s_rmt_hal.regs, true);
    rmt_ll_tx_reset_pointer(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL);
    rmt_ll_tx_set_mem_blocks(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1);
    RMTMEM.chan[REF_CLOCK_RMT_CHANNEL].data32[0] = data;
    rmt_ll_tx_enable_loop(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, false);
    rmt_ll_tx_start(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL);

    s_milliseconds = 0;
}

void ref_clock_deinit(void)
{
    // Deinitialize PCNT
    TEST_ESP_OK(pcnt_unit_stop(s_pcnt_unit));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(s_pcnt_unit, REF_CLOCK_PRESCALER_MS * 1000));
    TEST_ESP_OK(pcnt_del_channel(s_pcnt_chan));
    TEST_ESP_OK(pcnt_del_unit(s_pcnt_unit));

    // Deinitialize RMT
    rmt_ll_tx_enable_carrier_modulation(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, false);
    periph_module_disable(PERIPH_RMT_MODULE);
}

uint64_t ref_clock_get(void)
{
    int microseconds = 0;
    TEST_ESP_OK(pcnt_unit_get_count(s_pcnt_unit, &microseconds));
    return 1000 * (uint64_t)s_milliseconds + (uint64_t)microseconds;
}
