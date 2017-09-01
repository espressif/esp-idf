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
#include "soc/rmt_struct.h"
#include "soc/pcnt_struct.h"
#include "soc/pcnt_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/dport_reg.h"
#include "rom/gpio.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"

/* Select which RMT and PCNT channels, and GPIO to use */
#define REF_CLOCK_RMT_CHANNEL   7
#define REF_CLOCK_PCNT_UNIT     0
#define REF_CLOCK_GPIO          21

#define REF_CLOCK_PRESCALER_MS  30

static void IRAM_ATTR pcnt_isr(void* arg);

static intr_handle_t s_intr_handle;
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t s_milliseconds;

void ref_clock_init()
{
    assert(s_intr_handle == NULL && "already initialized");

    // Route RMT output to GPIO matrix
    gpio_matrix_out(REF_CLOCK_GPIO, RMT_SIG_OUT0_IDX + REF_CLOCK_RMT_CHANNEL, false, false);


    // Initialize RMT
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);
    RMT.apb_conf.fifo_mask = 1;
    rmt_item32_t data = {
            .duration0 = 1,
            .level0 = 1,
            .duration1 = 0,
            .level1 = 0
    };
    RMTMEM.chan[REF_CLOCK_RMT_CHANNEL].data32[0] = data;
    RMTMEM.chan[REF_CLOCK_RMT_CHANNEL].data32[1].val = 0;


    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf0.clk_en = 1;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.tx_start = 0;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.mem_owner = 0;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.mem_rd_rst = 1;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.apb_mem_rst = 1;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf0.carrier_en = 0;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf0.div_cnt = 1;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf0.mem_size = 1;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.ref_always_on = 0;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.tx_conti_mode = 1;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.tx_start = 1;

    // Route signal to PCNT
    int pcnt_sig_idx = (REF_CLOCK_PCNT_UNIT < 5) ?
            PCNT_SIG_CH0_IN0_IDX + 4 * REF_CLOCK_PCNT_UNIT :
            PCNT_SIG_CH0_IN5_IDX + 4 * (REF_CLOCK_PCNT_UNIT - 5);
    gpio_matrix_in(REF_CLOCK_GPIO, pcnt_sig_idx, false);
    if (REF_CLOCK_GPIO != 20) {
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[REF_CLOCK_GPIO]);
    } else {
        PIN_INPUT_ENABLE(PERIPHS_IO_MUX_GPIO20_U);
    }

    // Initialize PCNT
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_PCNT_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_PCNT_RST);

    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.ch0_hctrl_mode = 0;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.ch0_lctrl_mode = 0;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.ch0_pos_mode = 1;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.ch0_neg_mode = 1;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.thr_l_lim_en = 0;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.thr_h_lim_en = 1;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.thr_zero_en = 0;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.thr_thres0_en = 0;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf0.thr_thres1_en = 0;
    PCNT.conf_unit[REF_CLOCK_PCNT_UNIT].conf2.cnt_h_lim = REF_CLOCK_PRESCALER_MS * 1000;

    // Enable PCNT and wait for it to start counting
    PCNT.ctrl.val &= ~(BIT(REF_CLOCK_PCNT_UNIT * 2 + 1));
    PCNT.ctrl.val |= BIT(REF_CLOCK_PCNT_UNIT * 2);
    PCNT.ctrl.val &= ~BIT(REF_CLOCK_PCNT_UNIT * 2);

    // Enable interrupt
    s_milliseconds = 0;
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_PCNT_INTR_SOURCE, ESP_INTR_FLAG_IRAM, pcnt_isr, NULL, &s_intr_handle));
    PCNT.int_clr.val = BIT(REF_CLOCK_PCNT_UNIT);
    PCNT.int_ena.val = BIT(REF_CLOCK_PCNT_UNIT);
}

static void IRAM_ATTR pcnt_isr(void* arg)
{
    portENTER_CRITICAL(&s_lock);
    PCNT.int_clr.val = BIT(REF_CLOCK_PCNT_UNIT);
    s_milliseconds += REF_CLOCK_PRESCALER_MS;
    portEXIT_CRITICAL(&s_lock);
}

void ref_clock_deinit()
{
    assert(s_intr_handle && "deinit called without init");

    // Disable interrupt
    PCNT.int_ena.val &= ~BIT(REF_CLOCK_PCNT_UNIT);
    esp_intr_free(s_intr_handle);
    s_intr_handle = NULL;

    // Disable RMT
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf1.tx_start = 0;
    RMT.conf_ch[REF_CLOCK_RMT_CHANNEL].conf0.clk_en = 0;
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN);
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);

    // Disable PCNT
    PCNT.ctrl.val |= ~(BIT(REF_CLOCK_PCNT_UNIT * 2 + 1));
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_PCNT_CLK_EN);
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_PCNT_RST);
}

uint64_t ref_clock_get()
{
    portENTER_CRITICAL(&s_lock);
    uint32_t microseconds = PCNT.cnt_unit[REF_CLOCK_PCNT_UNIT].cnt_val;
    uint32_t milliseconds = s_milliseconds;
    if (PCNT.int_st.val & BIT(REF_CLOCK_PCNT_UNIT)) {
        milliseconds += REF_CLOCK_PRESCALER_MS;
    }
    portEXIT_CRITICAL(&s_lock);
    return 1000 * (uint64_t) milliseconds + (uint64_t) microseconds;
}
