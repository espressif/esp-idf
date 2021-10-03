// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_periph.h"
#include "soc/soc_caps.h"
#include "hal/rmt_types.h"
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"
#include "hal/pcnt_hal.h"
#include "hal/pcnt_ll.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

#define REF_CLOCK_RMT_CHANNEL 0 // RMT channel 0
#define REF_CLOCK_PCNT_UNIT 0   // PCNT unit 0
#define REF_CLOCK_PCNT_CHANNEL 0// PCNT channel 0
#define REF_CLOCK_GPIO 21       // GPIO used to combine RMT out signal with PCNT input signal

#define REF_CLOCK_PRESCALER_MS 30 // PCNT high threshold interrupt fired every 30ms

static void IRAM_ATTR pcnt_isr(void *arg);

static intr_handle_t s_intr_handle;
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t s_milliseconds;

static rmt_hal_context_t s_rmt_hal;
static pcnt_hal_context_t s_pcnt_hal;

void ref_clock_init(void)
{
    assert(s_intr_handle == NULL && "ref clock already initialized");

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

    rmt_ll_enable_drive_clock(s_rmt_hal.regs, true);
#if SOC_RMT_SUPPORT_XTAL
    rmt_ll_set_group_clock_src(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, RMT_BASECLK_XTAL, 39, 0, 0); // XTAL(40MHz), rmt_sclk => 1MHz (40/(1+39))
#elif SOC_RMT_SUPPORT_REF_TICK
    rmt_ll_set_group_clock_src(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, RMT_BASECLK_REF, 0, 0, 0); // select REF_TICK (1MHz)
#endif
    rmt_hal_tx_set_channel_clock(&s_rmt_hal, REF_CLOCK_RMT_CHANNEL, 1000000, 1000000); // counter clock: 1MHz
    rmt_ll_tx_enable_idle(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, true); // enable idle output
    rmt_ll_tx_set_idle_level(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1); // idle level: 1
    rmt_ll_tx_enable_carrier_modulation(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, true);
#if !CONFIG_IDF_TARGET_ESP32
    rmt_ll_tx_set_carrier_always_on(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, true);
#endif
    rmt_hal_set_carrier_clock(&s_rmt_hal, REF_CLOCK_RMT_CHANNEL, 1000000, 500000, 0.5); // set carrier to 500KHz
    rmt_ll_tx_set_carrier_level(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1);
    rmt_ll_enable_mem_access(s_rmt_hal.regs, true);
    rmt_ll_tx_reset_pointer(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL);
    rmt_ll_tx_set_mem_blocks(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, 1);
    rmt_ll_write_memory(s_rmt_hal.mem, REF_CLOCK_RMT_CHANNEL, &data, 1, 0);
    rmt_ll_tx_enable_loop(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, false);
    rmt_ll_tx_start(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL);

    // Route signal to PCNT
    esp_rom_gpio_connect_in_signal(REF_CLOCK_GPIO, PCNT_SIG_CH0_IN0_IDX, false);
    if (REF_CLOCK_GPIO != 20) {
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[REF_CLOCK_GPIO]);
    } else {
        PIN_INPUT_ENABLE(PERIPHS_IO_MUX_GPIO20_U);
    }

    // Initialize PCNT
    periph_module_enable(PERIPH_PCNT_MODULE);
    pcnt_hal_init(&s_pcnt_hal, REF_CLOCK_PCNT_UNIT);

    pcnt_ll_set_edge_action(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT, REF_CLOCK_PCNT_CHANNEL,
                            PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_ll_set_level_action(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT, REF_CLOCK_PCNT_CHANNEL,
                             PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP);
    pcnt_ll_disable_all_events(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT);
    pcnt_ll_set_high_limit_value(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT, REF_CLOCK_PRESCALER_MS * 1000);
    pcnt_ll_enable_high_limit_event(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT, true);

    // Enable PCNT and wait for it to start counting
    pcnt_ll_start_count(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT);
    pcnt_ll_clear_count(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT);

    esp_rom_delay_us(10000);

    // Enable interrupt
    s_milliseconds = 0;
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_PCNT_INTR_SOURCE, ESP_INTR_FLAG_IRAM, pcnt_isr, NULL, &s_intr_handle));
    pcnt_ll_clear_intr_status(s_pcnt_hal.dev, BIT(REF_CLOCK_PCNT_UNIT));
    pcnt_ll_enable_intr(s_pcnt_hal.dev, 1 << REF_CLOCK_PCNT_UNIT, true);
}

static void IRAM_ATTR pcnt_isr(void *arg)
{
    portENTER_CRITICAL_ISR(&s_lock);
    pcnt_ll_clear_intr_status(s_pcnt_hal.dev, BIT(REF_CLOCK_PCNT_UNIT));
    s_milliseconds += REF_CLOCK_PRESCALER_MS;
    portEXIT_CRITICAL_ISR(&s_lock);
}

void ref_clock_deinit()
{
    assert(s_intr_handle && "ref clock deinit called without init");

    // Disable interrupt
    pcnt_ll_enable_intr(s_pcnt_hal.dev, 1 << REF_CLOCK_PCNT_UNIT, false);
    esp_intr_free(s_intr_handle);
    s_intr_handle = NULL;

    // Disable RMT
    rmt_ll_tx_enable_carrier_modulation(s_rmt_hal.regs, REF_CLOCK_RMT_CHANNEL, false);
    periph_module_disable(PERIPH_RMT_MODULE);

    // Disable PCNT
    pcnt_ll_stop_count(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT);
    periph_module_disable(PERIPH_PCNT_MODULE);
}

uint64_t ref_clock_get()
{
    portENTER_CRITICAL(&s_lock);
    int microseconds = 0;
    microseconds = pcnt_ll_get_count(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT);
    uint32_t milliseconds = s_milliseconds;
    uint32_t intr_status = pcnt_ll_get_intr_status(s_pcnt_hal.dev);
    if (intr_status & BIT(REF_CLOCK_PCNT_UNIT)) {
        // refresh counter value, in case the overflow has happened after reading cnt_val
        microseconds = pcnt_ll_get_count(s_pcnt_hal.dev, REF_CLOCK_PCNT_UNIT);
        milliseconds += REF_CLOCK_PRESCALER_MS;
    }
    portEXIT_CRITICAL(&s_lock);
    return 1000 * (uint64_t)milliseconds + (uint64_t)microseconds;
}
