// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/soc_caps.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_conf.h"
#include "sdkconfig.h"
#include <sys/param.h>


#if CONFIG_IDF_TARGET_ESP32C3
#include "soc/gdma_channel.h"
#include "soc/soc.h"
#include "esp_rom_sys.h"
#endif

void adc_hal_init(void)
{
    // Set internal FSM wait time, fixed value.
    adc_ll_digi_set_fsm_time(SOC_ADC_FSM_RSTB_WAIT_DEFAULT, SOC_ADC_FSM_START_WAIT_DEFAULT,
                             SOC_ADC_FSM_STANDBY_WAIT_DEFAULT);
    adc_ll_set_sample_cycle(ADC_FSM_SAMPLE_CYCLE_DEFAULT);
    adc_hal_pwdet_set_cct(SOC_ADC_PWDET_CCT_DEFAULT);
    adc_ll_digi_output_invert(ADC_NUM_1, SOC_ADC_DIGI_DATA_INVERT_DEFAULT(ADC_NUM_1));
    adc_ll_digi_output_invert(ADC_NUM_2, SOC_ADC_DIGI_DATA_INVERT_DEFAULT(ADC_NUM_2));
    adc_ll_digi_set_clk_div(SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT);
}

void adc_hal_deinit(void)
{
    adc_ll_set_power_manage(ADC_POWER_SW_OFF);
}

#ifndef CONFIG_IDF_TARGET_ESP32C3
int adc_hal_convert(adc_ll_num_t adc_n, int channel, int *value)
{
    adc_ll_rtc_enable_channel(adc_n, channel);
    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true);
    *value = adc_ll_rtc_get_convert_value(adc_n);
    return (int)adc_ll_rtc_analysis_raw_data(adc_n, (uint16_t)(*value));
}
#endif

/*---------------------------------------------------------------
                    ADC calibration setting
---------------------------------------------------------------*/
#if SOC_ADC_HW_CALIBRATION_V1
// ESP32-S2 and C3 support HW offset calibration.

void adc_hal_calibration_init(adc_ll_num_t adc_n)
{
    adc_ll_calibration_init(adc_n);
}

static uint32_t s_previous_init_code[SOC_ADC_PERIPH_NUM] = {-1, -1};

void adc_hal_set_calibration_param(adc_ll_num_t adc_n, uint32_t param)
{
    if (param != s_previous_init_code[adc_n]) {
        adc_ll_set_calibration_param(adc_n, param);
        s_previous_init_code[adc_n] = param;
    }
}

#if CONFIG_IDF_TARGET_ESP32S2
static void cal_setup(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd)
{
    adc_hal_set_controller(adc_n, ADC_CTRL_RTC);    //Set controller

    /* Enable/disable internal connect GND (for calibration). */
    if (internal_gnd) {
        adc_ll_rtc_disable_channel(adc_n);
        adc_ll_set_atten(adc_n, 0, atten);  // Note: when disable all channel, HW auto select channel0 atten param.
    } else {
        adc_ll_rtc_enable_channel(adc_n, channel);
        adc_ll_set_atten(adc_n, channel, atten);
    }
}

static uint32_t read_cal_channel(adc_ll_num_t adc_n, int channel)
{
    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true);
    return (uint32_t)adc_ll_rtc_get_convert_value(adc_n);
}

#elif CONFIG_IDF_TARGET_ESP32C3
static void cal_setup(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd)
{
    adc_hal_set_controller(adc_n, ADC_CTRL_DIG);    //Set controller

    adc_digi_config_t dig_cfg = {
        .conv_limit_en = 0,
        .conv_limit_num = 250,
        .sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_HIGH,
    };
    adc_hal_digi_controller_config(&dig_cfg);

    /* Enable/disable internal connect GND (for calibration). */
    if (internal_gnd) {
        const int esp32c3_invalid_chan = (adc_n == ADC_NUM_1)? 0xF: 0x1;
        adc_ll_onetime_set_channel(adc_n, esp32c3_invalid_chan);
    } else {
        adc_ll_onetime_set_channel(adc_n, channel);
    }
    adc_ll_onetime_set_atten(atten);
    adc_hal_adc1_onetime_sample_enable((adc_n == ADC_NUM_1));
    adc_hal_adc2_onetime_sample_enable((adc_n == ADC_NUM_2));
}

static uint32_t read_cal_channel(adc_ll_num_t adc_n, int channel)
{
    adc_ll_intr_clear(ADC_LL_INTR_ADC1_DONE | ADC_LL_INTR_ADC2_DONE);
    adc_ll_onetime_start(false);
    esp_rom_delay_us(5);
    adc_ll_onetime_start(true);

    while(!adc_ll_intr_get_raw(ADC_LL_INTR_ADC1_DONE | ADC_LL_INTR_ADC2_DONE));

    uint32_t read_val = -1;
    if (adc_n == ADC_NUM_1) {
        read_val = adc_ll_adc1_read();
    } else if (adc_n == ADC_NUM_2) {
        read_val = adc_ll_adc2_read();
        if (adc_ll_analysis_raw_data(adc_n, read_val)) {
            return -1;
        }
    }
    return read_val;
}
#endif //CONFIG_IDF_TARGET_*

#define ADC_HAL_CAL_TIMES        (10)
#define ADC_HAL_CAL_OFFSET_RANGE (4096)

uint32_t adc_hal_self_calibration(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd)
{
    adc_hal_set_power_manage(ADC_POWER_SW_ON);

    if (adc_n == ADC_NUM_2) {
        adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
        adc_hal_arbiter_config(&config);
    }

    cal_setup(adc_n, channel, atten, internal_gnd);

    adc_ll_calibration_prepare(adc_n, channel, internal_gnd);

    uint32_t code_list[ADC_HAL_CAL_TIMES] = {0};
    uint32_t code_sum = 0;
    uint32_t code_h = 0;
    uint32_t code_l = 0;
    uint32_t chk_code = 0;

    for (uint8_t rpt = 0 ; rpt < ADC_HAL_CAL_TIMES ; rpt ++) {
        code_h = ADC_HAL_CAL_OFFSET_RANGE;
        code_l = 0;
        chk_code = (code_h + code_l) / 2;
        adc_ll_set_calibration_param(adc_n, chk_code);
        uint32_t self_cal = read_cal_channel(adc_n, channel);
        while (code_h - code_l > 1) {
            if (self_cal == 0) {
                code_h = chk_code;
            } else {
                code_l = chk_code;
            }
            chk_code = (code_h + code_l) / 2;
            adc_ll_set_calibration_param(adc_n, chk_code);
            self_cal = read_cal_channel(adc_n, channel);
            if ((code_h - code_l == 1)) {
                chk_code += 1;
                adc_ll_set_calibration_param(adc_n, chk_code);
                self_cal = read_cal_channel(adc_n, channel);
            }
        }
        code_list[rpt] = chk_code;
        code_sum += chk_code;
    }

    code_l = code_list[0];
    code_h = code_list[0];
    for (uint8_t i = 0 ; i < ADC_HAL_CAL_TIMES ; i++) {
        code_l = MIN(code_l, code_list[i]);
        code_h = MAX(code_h, code_list[i]);
    }

    chk_code = code_h + code_l;
    uint32_t ret = ((code_sum - chk_code) % (ADC_HAL_CAL_TIMES - 2) < 4)
           ? (code_sum - chk_code) / (ADC_HAL_CAL_TIMES - 2)
           : (code_sum - chk_code) / (ADC_HAL_CAL_TIMES - 2) + 1;

    adc_ll_calibration_finish(adc_n);
    return ret;
}
#endif //SOC_ADC_HW_CALIBRATION_V1

#if CONFIG_IDF_TARGET_ESP32C3
//This feature is currently supported on ESP32C3, will be supported on other chips soon
/*---------------------------------------------------------------
                    DMA setting
---------------------------------------------------------------*/
void adc_hal_digi_dma_multi_descriptor(adc_dma_hal_config_t *dma_config, uint8_t *data_buf, uint32_t size, uint32_t num)
{
    assert(((uint32_t)data_buf % 4) == 0);
    assert((size % 4) == 0);
    dma_descriptor_t *desc = dma_config->rx_desc;
    uint32_t n = 0;

    while (num--) {
        desc[n].dw0.size = size;
        desc[n].dw0.suc_eof = 0;
        desc[n].dw0.owner = 1;
        desc[n].buffer = data_buf;
        desc[n].next = &desc[n+1];
        data_buf += size;
        n++;
    }
    desc[n-1].next = NULL;
}

void adc_hal_digi_rxdma_start(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    gdma_ll_rx_reset_channel(adc_dma_ctx->dev, dma_config->dma_chan);
    gdma_ll_rx_set_desc_addr(adc_dma_ctx->dev, dma_config->dma_chan, (uint32_t)dma_config->rx_desc);
    gdma_ll_rx_start(adc_dma_ctx->dev, dma_config->dma_chan);
}

void adc_hal_digi_rxdma_stop(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    gdma_ll_rx_stop(adc_dma_ctx->dev, dma_config->dma_chan);
}

void adc_hal_digi_ena_intr(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t mask)
{
    gdma_ll_enable_interrupt(adc_dma_ctx->dev, dma_config->dma_chan, mask, true);
}

void adc_hal_digi_clr_intr(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t mask)
{
    gdma_ll_clear_interrupt_status(adc_dma_ctx->dev, dma_config->dma_chan, mask);
}

void adc_hal_digi_dis_intr(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t mask)
{
    gdma_ll_enable_interrupt(adc_dma_ctx->dev, dma_config->dma_chan, mask, false);
}

void adc_hal_digi_set_eof_num(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t num)
{
    adc_ll_digi_dma_set_eof_num(num);
}

void adc_hal_digi_start(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    //Set to 1: the ADC data will be sent to the DMA
    adc_ll_digi_dma_enable();
    //enable sar adc timer
    adc_ll_digi_trigger_enable();
    //reset the adc state
    adc_ll_digi_reset();
}

void adc_hal_digi_stop(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    //Set to 0: the ADC data won't be sent to the DMA
    adc_ll_digi_dma_disable();
    //disable sar adc timer
    adc_ll_digi_trigger_disable();
}

void adc_hal_digi_init(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    adc_dma_ctx->dev = &GDMA;
    gdma_ll_enable_clock(adc_dma_ctx->dev, true);
    gdma_ll_clear_interrupt_status(adc_dma_ctx->dev, dma_config->dma_chan, UINT32_MAX);
    gdma_ll_rx_connect_to_periph(adc_dma_ctx->dev, dma_config->dma_chan, SOC_GDMA_TRIG_PERIPH_ADC0);

    adc_ll_adc1_onetime_sample_enable(false);
    adc_ll_adc2_onetime_sample_enable(false);
}

/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
void adc_hal_onetime_start(adc_digi_config_t *adc_digi_config)
{
    /**
     * There is a hardware limitation. If the APB clock frequency is high, the step of this reg signal: ``onetime_start`` may not be captured by the
     * ADC digital controller (when its clock frequency is too slow). A rough estimate for this step should be at least 3 ADC digital controller
     * clock cycle.
     *
     * This limitation will be removed in hardware future versions.
     *
     */
    uint32_t digi_clk = APB_CLK_FREQ / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1);
    //Convert frequency to time (us). Since decimals are removed by this division operation. Add 1 here in case of the fact that delay is not enough.
    uint32_t delay = (1000 * 1000) / digi_clk + 1;
    //3 ADC digital controller clock cycle
    delay = delay * 3;
    //This coefficient (8) is got from test. When digi_clk is not smaller than ``APB_CLK_FREQ/8``, no delay is needed.
    if (digi_clk >= APB_CLK_FREQ/8) {
        delay = 0;
    }

    adc_ll_onetime_start(false);
    esp_rom_delay_us(delay);
    adc_ll_onetime_start(true);
    //No need to delay here. Becuase if the start signal is not seen, there won't be a done intr.
}

void adc_hal_adc1_onetime_sample_enable(bool enable)
{
    adc_ll_adc1_onetime_sample_enable(enable);
}

void adc_hal_adc2_onetime_sample_enable(bool enable)
{
    adc_ll_adc2_onetime_sample_enable(enable);
}

void adc_hal_onetime_channel(adc_ll_num_t unit, adc_channel_t channel)
{
    adc_ll_onetime_set_channel(unit, channel);
}

void adc_hal_set_onetime_atten(adc_atten_t atten)
{
    adc_ll_onetime_set_atten(atten);
}

esp_err_t adc_hal_single_read(adc_ll_num_t unit, int *out_raw)
{
    if (unit == ADC_NUM_1) {
        *out_raw = adc_ll_adc1_read();
    } else if (unit == ADC_NUM_2) {
        *out_raw = adc_ll_adc2_read();
        if (adc_ll_analysis_raw_data(unit, *out_raw)) {
            return ESP_ERR_INVALID_STATE;
        }
    }
    return ESP_OK;
}

//--------------------INTR-------------------------------
static adc_ll_intr_t get_event_intr(adc_event_t event)
{
    adc_ll_intr_t intr_mask = 0;
    if (event & ADC_EVENT_ADC1_DONE) {
        intr_mask |= ADC_LL_INTR_ADC1_DONE;
    }
    if (event & ADC_EVENT_ADC2_DONE) {
        intr_mask |= ADC_LL_INTR_ADC2_DONE;
    }
    return intr_mask;
}

void adc_hal_intr_enable(adc_event_t event)
{
    adc_ll_intr_enable(get_event_intr(event));
}

void adc_hal_intr_disable(adc_event_t event)
{
    adc_ll_intr_disable(get_event_intr(event));
}

void adc_hal_intr_clear(adc_event_t event)
{
    adc_ll_intr_clear(get_event_intr(event));
}

bool adc_hal_intr_get_raw(adc_event_t event)
{
    return adc_ll_intr_get_raw(get_event_intr(event));
}

bool adc_hal_intr_get_status(adc_event_t event)
{
    return adc_ll_intr_get_status(get_event_intr(event));
}
#endif
