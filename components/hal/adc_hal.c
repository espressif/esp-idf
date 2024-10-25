/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "hal/adc_hal.h"
#include "hal/assert.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
//ADC utilises I2S0 DMA on ESP32
#include "hal/i2s_hal.h"
#include "hal/i2s_types.h"
#include "soc/i2s_struct.h"

//ESP32 ADC uses the DMA through I2S. The I2S needs to be configured.
#define I2S_BASE_CLK                                    (160 * 1000 * 1000)
#define SAMPLE_BITS                                     16
#define ADC_LL_CLKM_DIV_NUM_DEFAULT                     2
#define ADC_LL_CLKM_DIV_B_DEFAULT                       0
#define ADC_LL_CLKM_DIV_A_DEFAULT                       1

i2s_dev_t *adc_hal_i2s_dev = I2S_LL_GET_HW(ADC_HAL_DMA_I2S_HOST);

#define adc_ll_digi_dma_set_eof_num(num)                i2s_ll_rx_set_eof_num(adc_hal_i2s_dev, (num) * 4)
#define adc_ll_digi_reset() do { \
            i2s_ll_rx_reset(adc_hal_i2s_dev); \
            i2s_ll_rx_reset_fifo(adc_hal_i2s_dev); \
        } while (0)
#define adc_ll_digi_trigger_enable()                    i2s_ll_rx_start(adc_hal_i2s_dev)
#define adc_ll_digi_trigger_disable()                   i2s_ll_rx_stop(adc_hal_i2s_dev)
#define adc_ll_digi_dma_enable()                        adc_ll_digi_set_data_source(1)  //Will this influence I2S0
#define adc_ll_digi_dma_disable()                       adc_ll_digi_set_data_source(0)

#endif

void adc_hal_dma_ctx_config(adc_hal_dma_ctx_t *hal, const adc_hal_dma_config_t *config)
{
    hal->desc_dummy_head.next = hal->rx_desc;
    hal->eof_desc_num = config->eof_desc_num;
    hal->eof_step = config->eof_step;
    hal->eof_num = config->eof_num;
}

void adc_hal_digi_init(adc_hal_dma_ctx_t *hal)
{
    // Set internal FSM wait time, fixed value.
    adc_ll_digi_set_fsm_time(ADC_LL_FSM_RSTB_WAIT_DEFAULT, ADC_LL_FSM_START_WAIT_DEFAULT,
                             ADC_LL_FSM_STANDBY_WAIT_DEFAULT);
    adc_ll_set_sample_cycle(ADC_LL_SAMPLE_CYCLE_DEFAULT);
    adc_hal_pwdet_set_cct(ADC_LL_PWDET_CCT_DEFAULT);
    adc_ll_digi_output_invert(ADC_UNIT_1, ADC_LL_DIGI_DATA_INVERT_DEFAULT(ADC_UNIT_1));
    adc_ll_digi_output_invert(ADC_UNIT_2, ADC_LL_DIGI_DATA_INVERT_DEFAULT(ADC_UNIT_2));
    adc_ll_digi_set_clk_div(ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT);

    adc_ll_digi_dma_set_eof_num(hal->eof_num);
#if CONFIG_IDF_TARGET_ESP32
    i2s_ll_rx_set_sample_bit(adc_hal_i2s_dev, SAMPLE_BITS, SAMPLE_BITS);
    i2s_ll_rx_enable_mono_mode(adc_hal_i2s_dev, 1);
    i2s_ll_rx_force_enable_fifo_mod(adc_hal_i2s_dev, 1);
    i2s_ll_rx_enable_right_first(adc_hal_i2s_dev, false);
    i2s_ll_rx_enable_msb_shift(adc_hal_i2s_dev, false);
    i2s_ll_rx_set_ws_width(adc_hal_i2s_dev, 16);
    i2s_ll_rx_select_std_slot(adc_hal_i2s_dev, I2S_STD_SLOT_LEFT, false);
    i2s_ll_enable_builtin_adc_dac(adc_hal_i2s_dev, 1);
#endif

    adc_oneshot_ll_disable_all_unit();
}

void adc_hal_digi_deinit()
{
#if ADC_LL_POWER_MANAGE_SUPPORTED
    adc_ll_set_power_manage(ADC_UNIT_1, ADC_LL_POWER_SW_OFF);
    adc_ll_set_power_manage(ADC_UNIT_2, ADC_LL_POWER_SW_OFF);
#endif
    adc_ll_digi_trigger_disable();
    adc_ll_digi_dma_disable();
    adc_ll_digi_clear_pattern_table(ADC_UNIT_1);
    adc_ll_digi_clear_pattern_table(ADC_UNIT_2);
    adc_ll_digi_reset();
    adc_ll_digi_controller_clk_disable();
}

/*---------------------------------------------------------------
                    DMA read
---------------------------------------------------------------*/
static adc_ll_digi_convert_mode_t get_convert_mode(adc_digi_convert_mode_t convert_mode)
{
#if CONFIG_IDF_TARGET_ESP32 || SOC_ADC_DIGI_CONTROLLER_NUM == 1
    return ADC_LL_DIGI_CONV_ONLY_ADC1;
#elif (SOC_ADC_DIGI_CONTROLLER_NUM >= 2)
    switch (convert_mode) {
        case ADC_CONV_SINGLE_UNIT_1:
            return ADC_LL_DIGI_CONV_ONLY_ADC1;
        case ADC_CONV_SINGLE_UNIT_2:
            return ADC_LL_DIGI_CONV_ONLY_ADC2;
        case ADC_CONV_BOTH_UNIT:
            return ADC_LL_DIGI_CONV_BOTH_UNIT;
        case ADC_CONV_ALTER_UNIT:
            return ADC_LL_DIGI_CONV_ALTER_UNIT;
        default:
            abort();
    }
#endif
}

/**
 * For esp32s2 and later chips
 * - Set ADC digital controller clock division factor. The clock is divided from `APLL` or `APB` clock.
 *   Expression: controller_clk = APLL/APB * (div_num  + div_a / div_b + 1).
 * - Enable clock and select clock source for ADC digital controller.
 * For esp32, use I2S clock
 */
static void adc_hal_digi_sample_freq_config(adc_hal_dma_ctx_t *hal, adc_continuous_clk_src_t clk_src, uint32_t clk_src_freq_hz, uint32_t sample_freq_hz)
{
#if !CONFIG_IDF_TARGET_ESP32
    uint32_t interval = clk_src_freq_hz / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1) / 2 / sample_freq_hz;
    //set sample interval
    adc_ll_digi_set_trigger_interval(interval);
    //Here we set the clock divider factor to make the digital clock to 5M Hz
    adc_ll_digi_controller_clk_div(ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT);
    adc_ll_digi_clk_sel(clk_src);
#else
    i2s_ll_rx_clk_set_src(adc_hal_i2s_dev, I2S_CLK_SRC_DEFAULT);    /*!< Clock from PLL_D2_CLK(160M)*/
    uint32_t bclk_div = 16;
    uint32_t bclk = sample_freq_hz * 2;
    uint32_t mclk = bclk * bclk_div;
    hal_utils_clk_div_t mclk_div = {};
    i2s_hal_calc_mclk_precise_division(I2S_BASE_CLK, mclk, &mclk_div);
    i2s_ll_rx_set_mclk(adc_hal_i2s_dev, &mclk_div);
    i2s_ll_rx_set_bck_div_num(adc_hal_i2s_dev, bclk_div);
#endif
}

void adc_hal_digi_controller_config(adc_hal_dma_ctx_t *hal, const adc_hal_digi_ctrlr_cfg_t *cfg)
{
#if (SOC_ADC_DIGI_CONTROLLER_NUM == 1)
    //Only one pattern table, this variable is for readability
    const int pattern_both = 0;

    adc_ll_digi_clear_pattern_table(pattern_both);
    adc_ll_digi_set_pattern_table_len(pattern_both, cfg->adc_pattern_len);
    for (int i = 0; i < cfg->adc_pattern_len; i++) {
        adc_ll_digi_set_pattern_table(pattern_both, i, cfg->adc_pattern[i]);
    }
#if ADC_LL_POWER_MANAGE_SUPPORTED
    adc_ll_set_power_manage(0, ADC_LL_POWER_SW_ON);
#endif

#elif (SOC_ADC_DIGI_CONTROLLER_NUM >= 2)
    uint32_t adc1_pattern_idx = 0;
    uint32_t adc2_pattern_idx = 0;

    adc_ll_digi_clear_pattern_table(ADC_UNIT_1);
    adc_ll_digi_clear_pattern_table(ADC_UNIT_2);

    for (int i = 0; i < cfg->adc_pattern_len; i++) {
        if (cfg->adc_pattern[i].unit == ADC_UNIT_1) {
#if ADC_LL_POWER_MANAGE_SUPPORTED
            adc_ll_set_power_manage(ADC_UNIT_1, ADC_LL_POWER_SW_ON);
#endif
            adc_ll_digi_set_pattern_table(ADC_UNIT_1, adc1_pattern_idx, cfg->adc_pattern[i]);
            adc1_pattern_idx++;
        } else if (cfg->adc_pattern[i].unit == ADC_UNIT_2) {
#if ADC_LL_POWER_MANAGE_SUPPORTED
            adc_ll_set_power_manage(ADC_UNIT_2, ADC_LL_POWER_SW_ON);
#endif
            adc_ll_digi_set_pattern_table(ADC_UNIT_2, adc2_pattern_idx, cfg->adc_pattern[i]);
            adc2_pattern_idx++;
        } else {
            abort();
        }
    }
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_1, adc1_pattern_idx);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, adc2_pattern_idx);

#endif

    adc_ll_digi_convert_limit_enable(ADC_LL_DEFAULT_CONV_LIMIT_EN);
    adc_ll_digi_set_convert_limit_num(ADC_LL_DEFAULT_CONV_LIMIT_NUM);
    adc_ll_digi_set_convert_mode(get_convert_mode(cfg->conv_mode));

    //clock and sample frequency
    adc_hal_digi_sample_freq_config(hal, cfg->clk_src, cfg->clk_src_freq_hz, cfg->sample_freq_hz);
}


void adc_hal_digi_dma_link(adc_hal_dma_ctx_t *hal, uint8_t *data_buf)
{
    dma_descriptor_t *desc = hal->rx_desc;
    uint32_t per_eof_size = hal->eof_num * SOC_ADC_DIGI_DATA_BYTES_PER_CONV;
    uint32_t eof_step = hal->eof_step;
    uint32_t eof_num = hal->eof_desc_num;

    HAL_ASSERT(((uint32_t)data_buf % 4) == 0);
    HAL_ASSERT((per_eof_size % 4) == 0);
    uint32_t n = 0;
    dma_descriptor_t *desc_head = desc;

    hal->cur_desc_ptr = &hal->desc_dummy_head;
    while (eof_num--) {
        uint32_t eof_size = per_eof_size;

        for (int i = 0; i < eof_step; i++) {
            uint32_t this_len = eof_size;
            if (this_len > DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED) {
                this_len = DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
            }

            desc[n] = (dma_descriptor_t) {
                .dw0.size = this_len,
                .dw0.length = 0,
                .dw0.suc_eof = 0,
                .dw0.owner = 1,
                .buffer = data_buf,
                .next = &desc[n+1]
            };
            eof_size -= this_len;
            data_buf += this_len;
            n++;
        }
    }
    desc[n-1].next = desc_head;
}

adc_hal_dma_desc_status_t adc_hal_get_reading_result(adc_hal_dma_ctx_t *hal, const intptr_t eof_desc_addr, uint8_t **buffer, uint32_t *len)
{
    HAL_ASSERT(hal->cur_desc_ptr);

    if (!hal->cur_desc_ptr->next) {
        return ADC_HAL_DMA_DESC_NULL;
    }

    if ((intptr_t)hal->cur_desc_ptr == eof_desc_addr) {
        return ADC_HAL_DMA_DESC_WAITING;
    }

    uint8_t *buffer_start = NULL;
    uint32_t eof_len = 0;
    dma_descriptor_t *eof_desc = hal->cur_desc_ptr;

    //Find the eof list start
    eof_desc = eof_desc->next;
    eof_desc->dw0.owner = 1;
    buffer_start = eof_desc->buffer;
    eof_len += eof_desc->dw0.length;
    if ((intptr_t)eof_desc == eof_desc_addr) {
        goto valid;
    }

    //Find the eof list end
    for (int i = 1; i < hal->eof_step; i++) {
        eof_desc = eof_desc->next;
        eof_desc->dw0.owner = 1;
        eof_len += eof_desc->dw0.length;
        if ((intptr_t)eof_desc == eof_desc_addr) {
            goto valid;
        }
    }

valid:
    hal->cur_desc_ptr = eof_desc;
    *buffer = buffer_start;
    *len = eof_len;

    return ADC_HAL_DMA_DESC_VALID;
}

void adc_hal_digi_enable(bool enable)
{
    if (enable) {
        adc_ll_digi_trigger_enable();
    } else {
        adc_ll_digi_trigger_disable();
    }
}

void adc_hal_digi_connect(bool enable)
{
    if (enable) {
        adc_ll_digi_dma_enable();
    } else {
        adc_ll_digi_dma_disable();
    }
}

void adc_hal_digi_reset(void)
{
    adc_ll_digi_reset();
}

#if ADC_LL_WORKAROUND_CLEAR_EOF_COUNTER
void adc_hal_digi_clr_eof(void)
{
    adc_ll_digi_dma_clr_eof();
}
#endif

#if SOC_ADC_MONITOR_SUPPORTED
void adc_hal_digi_monitor_set_thres(adc_monitor_id_t monitor_id, adc_unit_t adc_n, uint8_t adc_ch, int32_t h_thres, int32_t l_thres)
{
    adc_ll_digi_monitor_set_thres(monitor_id, adc_n, adc_ch, h_thres, l_thres);

    // enable peripheral intr_ena accordingly
    adc_ll_digi_monitor_enable_intr(monitor_id, ADC_MONITOR_MODE_HIGH, (h_thres >= 0));
    adc_ll_digi_monitor_enable_intr(monitor_id, ADC_MONITOR_MODE_LOW, (l_thres >= 0));
}
#endif  //SOC_ADC_MONITOR_SUPPORTED
