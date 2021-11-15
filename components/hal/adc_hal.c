/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_conf.h"
#include "hal/assert.h"
#include "sdkconfig.h"


#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP8684
#include "soc/gdma_channel.h"
#include "soc/soc.h"
#include "esp_rom_sys.h"

typedef enum {
    ADC_EVENT_ADC1_DONE = BIT(0),
    ADC_EVENT_ADC2_DONE = BIT(1),
} adc_hal_event_t;

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

#if SOC_ADC_ARBITER_SUPPORTED
void adc_hal_arbiter_config(adc_arbiter_t *config)
{
    adc_ll_set_arbiter_work_mode(config->mode);
    adc_ll_set_arbiter_priority(config->rtc_pri, config->dig_pri, config->pwdet_pri);
}
#endif

/*---------------------------------------------------------------
                    ADC calibration setting
---------------------------------------------------------------*/
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
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

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
static void cal_setup(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd)
{
#if CONFIG_IDF_TARGET_ESP32S2
    adc_hal_set_controller(adc_n, ADC_CTRL_RTC);    //Set controller
#else
    adc_hal_set_controller(adc_n, ADC_LL_CTRL_ARB);    //Set controller
#endif

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

#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP8684
static void cal_setup(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd)
{
    adc_ll_onetime_sample_enable(ADC_NUM_1, false);
    adc_ll_onetime_sample_enable(ADC_NUM_2, false);
    /* Enable/disable internal connect GND (for calibration). */
    if (internal_gnd) {
        const int esp32c3_invalid_chan = (adc_n == ADC_NUM_1) ? 0xF : 0x1;
        adc_ll_onetime_set_channel(adc_n, esp32c3_invalid_chan);
    } else {
        adc_ll_onetime_set_channel(adc_n, channel);
    }
    adc_ll_onetime_set_atten(atten);
    adc_ll_onetime_sample_enable(adc_n, true);
}

static uint32_t read_cal_channel(adc_ll_num_t adc_n, int channel)
{
    adc_ll_intr_clear(ADC_LL_INTR_ADC1_DONE | ADC_LL_INTR_ADC2_DONE);
    adc_ll_onetime_start(false);
    esp_rom_delay_us(5);
    adc_ll_onetime_start(true);

    while (!adc_ll_intr_get_raw(ADC_LL_INTR_ADC1_DONE | ADC_LL_INTR_ADC2_DONE));

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
#endif //SOC_ADC_CALIBRATION_V1_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP8684
//This feature is currently supported on ESP32C3, will be supported on other chips soon
/*---------------------------------------------------------------
                    DMA setting
---------------------------------------------------------------*/
void adc_hal_context_config(adc_hal_context_t *hal, const adc_hal_config_t *config)
{
    hal->dev = &GDMA;
    hal->desc_dummy_head.next = hal->rx_desc;
    hal->desc_max_num = config->desc_max_num;
    hal->dma_chan = config->dma_chan;
    hal->eof_num = config->eof_num;
}

void adc_hal_digi_init(adc_hal_context_t *hal)
{
    gdma_ll_rx_clear_interrupt_status(hal->dev, hal->dma_chan, UINT32_MAX);
    gdma_ll_rx_enable_interrupt(hal->dev, hal->dma_chan, GDMA_LL_EVENT_RX_SUC_EOF, true);
    adc_ll_digi_dma_set_eof_num(hal->eof_num);
    adc_ll_onetime_sample_enable(ADC_NUM_1, false);
    adc_ll_onetime_sample_enable(ADC_NUM_2, false);
}

void adc_hal_fifo_reset(adc_hal_context_t *hal)
{
    adc_ll_digi_reset();
    gdma_ll_rx_reset_channel(hal->dev, hal->dma_chan);
}

static void adc_hal_digi_dma_link_descriptors(dma_descriptor_t *desc, uint8_t *data_buf, uint32_t size, uint32_t num)
{
    HAL_ASSERT(((uint32_t)data_buf % 4) == 0);
    HAL_ASSERT((size % 4) == 0);
    uint32_t n = 0;

    while (num--) {
        desc[n].dw0.size = size;
        desc[n].dw0.suc_eof = 0;
        desc[n].dw0.owner = 1;
        desc[n].buffer = data_buf;
        desc[n].next = &desc[n + 1];
        data_buf += size;
        n++;
    }
    desc[n - 1].next = NULL;
}

void adc_hal_digi_rxdma_start(adc_hal_context_t *hal, uint8_t *data_buf)
{
    //reset the current descriptor address
    hal->cur_desc_ptr = &hal->desc_dummy_head;
    adc_hal_digi_dma_link_descriptors(hal->rx_desc, data_buf, hal->eof_num * ADC_HAL_DATA_LEN_PER_CONV, hal->desc_max_num);
    gdma_ll_rx_set_desc_addr(hal->dev, hal->dma_chan, (uint32_t)hal->rx_desc);
    gdma_ll_rx_start(hal->dev, hal->dma_chan);
}

void adc_hal_digi_start(adc_hal_context_t *hal)
{
    //the ADC data will be sent to the DMA
    adc_ll_digi_dma_enable();
    //enable sar adc timer
    adc_ll_digi_trigger_enable();
}

adc_hal_dma_desc_status_t adc_hal_get_reading_result(adc_hal_context_t *hal, const intptr_t eof_desc_addr, dma_descriptor_t **cur_desc)
{
    HAL_ASSERT(hal->cur_desc_ptr);
    if (!hal->cur_desc_ptr->next) {
        return ADC_HAL_DMA_DESC_NULL;
    }
    if ((intptr_t)hal->cur_desc_ptr == eof_desc_addr) {
        return ADC_HAL_DMA_DESC_WAITING;
    }

    hal->cur_desc_ptr = hal->cur_desc_ptr->next;
    *cur_desc = hal->cur_desc_ptr;

    return ADC_HAL_DMA_DESC_VALID;
}

void adc_hal_digi_rxdma_stop(adc_hal_context_t *hal)
{
    gdma_ll_rx_stop(hal->dev, hal->dma_chan);
}

void adc_hal_digi_clr_intr(adc_hal_context_t *hal, uint32_t mask)
{
    gdma_ll_rx_clear_interrupt_status(hal->dev, hal->dma_chan, mask);
}

void adc_hal_digi_dis_intr(adc_hal_context_t *hal, uint32_t mask)
{
    gdma_ll_rx_enable_interrupt(hal->dev, hal->dma_chan, mask, false);
}

void adc_hal_digi_stop(adc_hal_context_t *hal)
{
    //Set to 0: the ADC data won't be sent to the DMA
    adc_ll_digi_dma_disable();
    //disable sar adc timer
    adc_ll_digi_trigger_disable();
}

/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/

//--------------------INTR-------------------------------//
static adc_ll_intr_t get_event_intr(adc_hal_event_t event)
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

static void adc_hal_intr_clear(adc_hal_event_t event)
{
    adc_ll_intr_clear(get_event_intr(event));
}

static bool adc_hal_intr_get_raw(adc_hal_event_t event)
{
    return adc_ll_intr_get_raw(get_event_intr(event));
}

//--------------------Single Read-------------------------------//
static void adc_hal_onetime_start(void)
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
    if (digi_clk >= APB_CLK_FREQ / 8) {
        delay = 0;
    }

    adc_ll_onetime_start(false);
    esp_rom_delay_us(delay);
    adc_ll_onetime_start(true);

    //No need to delay here. Becuase if the start signal is not seen, there won't be a done intr.
}

static esp_err_t adc_hal_single_read(adc_ll_num_t adc_n, int *out_raw)
{
    if (adc_n == ADC_NUM_1) {
        *out_raw = adc_ll_adc1_read();
    } else if (adc_n == ADC_NUM_2) {
        *out_raw = adc_ll_adc2_read();
        if (adc_ll_analysis_raw_data(adc_n, *out_raw)) {
            return ESP_ERR_INVALID_STATE;
       }
    }
    return ESP_OK;
}

esp_err_t adc_hal_convert(adc_ll_num_t adc_n, int channel, int *out_raw)
{
    esp_err_t ret;
    adc_hal_event_t event;

    if (adc_n == ADC_NUM_1) {
        event = ADC_EVENT_ADC1_DONE;
    } else {
        event = ADC_EVENT_ADC2_DONE;
    }

    adc_hal_intr_clear(event);
    adc_ll_onetime_sample_enable(ADC_NUM_1, false);
    adc_ll_onetime_sample_enable(ADC_NUM_2, false);
    adc_ll_onetime_sample_enable(adc_n, true);
    adc_ll_onetime_set_channel(adc_n, channel);

    //Trigger single read.
    adc_hal_onetime_start();
    while (!adc_hal_intr_get_raw(event));
    ret = adc_hal_single_read(adc_n, out_raw);
    //HW workaround: when enabling periph clock, this should be false
    adc_ll_onetime_sample_enable(adc_n, false);

    return ret;
}
#else // !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP8684
esp_err_t adc_hal_convert(adc_ll_num_t adc_n, int channel, int *out_raw)
{
    adc_ll_rtc_enable_channel(adc_n, channel);
    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true);
    *out_raw = adc_ll_rtc_get_convert_value(adc_n);

    if ((int)adc_ll_rtc_analysis_raw_data(adc_n, (uint16_t)(*out_raw))) {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}
#endif  //#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP8684
