// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

/* Unit tests need to have access to reliable timestamps even if CPU and APB
 * clock frequencies change over time. This reference clock is built upon two
 * peripherals: one RMT channel and one PCNT channel, plus one GPIO to connect
 * these peripherals.
 *
 * RMT channel is configured to use REF_TICK as clock source, which is a 1 MHz
 * clock derived from APB_CLK using a set of dividers. The divider is changed
 * automatically by hardware depending on the current clock source of APB_CLK.
 * For example, if APB_CLK is derived from PLL, one divider is used, and when
 * APB_CLK is derived from XTAL, another divider is used. RMT channel clocked
 * by REF_TICK is configured to generate a continuous 0.5 MHz signal, which is
 * connected to a GPIO. PCNT takes the input signal from this GPIO and counts
 * the edges (which occur at 1MHz frequency). PCNT counter is only 16 bit wide,
 * so an interrupt is configured to trigger when the counter reaches 30000,
 * incrementing a 32-bit millisecond counter maintained by software.
 * Together these two counters may be used at any time to obtain the timestamp.
 */

#include "test_utils.h"
#include "soc/soc.h"
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"
#include "soc/pcnt_caps.h"
#include "hal/pcnt_hal.h"
#include "soc/gpio_periph.h"
#include "soc/dport_reg.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "driver/periph_ctrl.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/gpio.h"
#endif
#include "sdkconfig.h"

/* Select which RMT and PCNT channels, and GPIO to use */
#define REF_CLOCK_RMT_CHANNEL   SOC_RMT_CHANNELS_NUM - 1
#define REF_CLOCK_PCNT_UNIT     0
#define REF_CLOCK_GPIO          21

#define REF_CLOCK_PRESCALER_MS  30

static void IRAM_ATTR pcnt_isr(void* arg);

static intr_handle_t s_intr_handle;
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t s_milliseconds;


static int get_pcnt_sig(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return (REF_CLOCK_PCNT_UNIT < 5) ?
            PCNT_SIG_CH0_IN0_IDX + 4 * REF_CLOCK_PCNT_UNIT :
            PCNT_SIG_CH0_IN5_IDX + 4 * (REF_CLOCK_PCNT_UNIT - 5);
#elif CONFIG_IDF_TARGET_ESP32S2
    return PCNT_SIG_CH0_IN0_IDX + 4 * REF_CLOCK_PCNT_UNIT;
#endif
}

static rmt_hal_context_t  s_rmt;
static pcnt_hal_context_t s_pcnt;

void ref_clock_init()
{
    assert(s_intr_handle == NULL && "already initialized");

    // Route RMT output to GPIO matrix
    gpio_matrix_out(REF_CLOCK_GPIO, RMT_SIG_OUT0_IDX + REF_CLOCK_RMT_CHANNEL, false, false);

    // Initialize RMT
    periph_module_enable(PERIPH_RMT_MODULE);
    rmt_hal_init(&s_rmt);
    rmt_ll_enable_mem_access(s_rmt.regs, true);
    rmt_item32_t data = {
            .duration0 = 1,
            .level0 = 1,
            .duration1 = 0,
            .level1 = 0
    };
    rmt_hal_transmit(&s_rmt, REF_CLOCK_RMT_CHANNEL, &data, 1, 0);
    rmt_ll_start_tx(s_rmt.regs, REF_CLOCK_RMT_CHANNEL);
    rmt_ll_set_mem_owner(s_rmt.regs, REF_CLOCK_RMT_CHANNEL, 0);
    rmt_ll_reset_tx_pointer(s_rmt.regs, REF_CLOCK_RMT_CHANNEL);
    rmt_ll_enable_carrier(s_rmt.regs, REF_CLOCK_RMT_CHANNEL, false);
    rmt_ll_set_counter_clock_div(s_rmt.regs, REF_CLOCK_RMT_CHANNEL, 1);
    rmt_ll_set_mem_blocks(s_rmt.regs, REF_CLOCK_RMT_CHANNEL, 1);
    rmt_ll_set_counter_clock_src(s_rmt.regs, REF_CLOCK_RMT_CHANNEL, 0);
    rmt_ll_enable_tx_loop(s_rmt.regs, REF_CLOCK_RMT_CHANNEL, true);
    rmt_ll_start_tx(s_rmt.regs, REF_CLOCK_RMT_CHANNEL);

    // Route signal to PCNT
    int pcnt_sig_idx = get_pcnt_sig();
    gpio_matrix_in(REF_CLOCK_GPIO, pcnt_sig_idx, false);
    if (REF_CLOCK_GPIO != 20) {
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[REF_CLOCK_GPIO]);
    } else {
        PIN_INPUT_ENABLE(PERIPHS_IO_MUX_GPIO20_U);
    }

    // Initialize PCNT
    periph_module_enable(PERIPH_PCNT_MODULE);
    pcnt_hal_init(&s_pcnt, REF_CLOCK_PCNT_UNIT);

    pcnt_ll_set_mode(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_CHANNEL_0,
                        PCNT_COUNT_INC, PCNT_COUNT_INC,
                        PCNT_MODE_KEEP, PCNT_MODE_KEEP);
    pcnt_ll_event_disable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_EVT_L_LIM);
    pcnt_ll_event_enable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_EVT_H_LIM);
    pcnt_ll_event_disable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_EVT_ZERO);
    pcnt_ll_event_disable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_EVT_THRES_0);
    pcnt_ll_event_disable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_EVT_THRES_1);
    pcnt_ll_set_event_value(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, PCNT_EVT_H_LIM, REF_CLOCK_PRESCALER_MS * 1000);

    // Enable PCNT and wait for it to start counting
    pcnt_ll_counter_resume(s_pcnt.dev, REF_CLOCK_PCNT_UNIT);
    pcnt_ll_counter_clear(s_pcnt.dev, REF_CLOCK_PCNT_UNIT);

    ets_delay_us(10000);

    // Enable interrupt
    s_milliseconds = 0;
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_PCNT_INTR_SOURCE, ESP_INTR_FLAG_IRAM, pcnt_isr, NULL, &s_intr_handle));
    pcnt_ll_clear_intr_status(s_pcnt.dev, BIT(REF_CLOCK_PCNT_UNIT));
    pcnt_ll_intr_enable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT);
}

static void IRAM_ATTR pcnt_isr(void* arg)
{
    portENTER_CRITICAL_ISR(&s_lock);
    pcnt_ll_clear_intr_status(s_pcnt.dev, BIT(REF_CLOCK_PCNT_UNIT));
    s_milliseconds += REF_CLOCK_PRESCALER_MS;
    portEXIT_CRITICAL_ISR(&s_lock);
}

void ref_clock_deinit()
{
    assert(s_intr_handle && "deinit called without init");

    // Disable interrupt
    pcnt_ll_intr_disable(s_pcnt.dev, REF_CLOCK_PCNT_UNIT);
    esp_intr_free(s_intr_handle);
    s_intr_handle = NULL;

    // Disable RMT
    rmt_ll_stop_tx(s_rmt.regs, REF_CLOCK_RMT_CHANNEL);
    periph_module_disable(PERIPH_RMT_MODULE);

    // Disable PCNT
    pcnt_ll_counter_pause(s_pcnt.dev, REF_CLOCK_PCNT_UNIT);
    periph_module_disable(PERIPH_PCNT_MODULE);
}

uint64_t ref_clock_get()
{
    portENTER_CRITICAL(&s_lock);
    int16_t microseconds = 0;
    pcnt_ll_get_counter_value(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, &microseconds);
    uint32_t milliseconds = s_milliseconds;
    uint32_t intr_status = 0;
    pcnt_ll_get_intr_status(s_pcnt.dev, &intr_status);
    if (intr_status & BIT(REF_CLOCK_PCNT_UNIT)) {
        // refresh counter value, in case the overflow has happened after reading cnt_val
        pcnt_ll_get_counter_value(s_pcnt.dev, REF_CLOCK_PCNT_UNIT, &microseconds);
        milliseconds += REF_CLOCK_PRESCALER_MS;
    }
    portEXIT_CRITICAL(&s_lock);
    return 1000 * (uint64_t) milliseconds + (uint64_t) microseconds;
}
