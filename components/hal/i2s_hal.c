// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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


// The HAL layer for I2S (common part)

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "hal/i2s_hal.h"

#define I2S_MODE_I2S (I2S_MODE_MASTER|I2S_MODE_SLAVE|I2S_MODE_TX|I2S_MODE_RX) /*!< I2S normal mode*/

/**
 * @brief Calculate the closest sample rate clock configuration.
 *        clock relationship:
 *        Fmclk = bck_div*fbck = fsclk/(mclk_div+b/a)
 *
 * @param fsclk I2S source clock freq.
 * @param fbck BCK freuency.
 * @param bck_div The BCK devider of bck. Generally, set bck_div to 8.
 * @param cal Point to `i2s_ll_clk_cal_t` structure.
 */
static void i2s_hal_clk_cal(uint32_t fsclk, uint32_t fbck, int bck_div, i2s_ll_clk_cal_t *cal)
{
    int ma = 0;
    int mb = 0;
    uint32_t mclk = fbck * bck_div;
    cal->mclk_div = fsclk / mclk;
    cal->bck_div = bck_div;
    cal->a = 1;
    cal->b = 0;
    uint32_t freq_diff = fsclk - mclk * cal->mclk_div;
    uint32_t min = ~0;
    if (freq_diff == 0) {
        return;
    }
    for (int a = 2; a <= I2S_LL_MCLK_DIVIDER_MAX; a++) {
        for (int b = 1; b < a; b++) {
            ma = freq_diff * a;
            mb = mclk * b;
            if (ma == mb) {
                cal->a = a;
                cal->b = b;
                return;
            }
            if (abs((mb - ma)) < min) {
                cal->a = a;
                cal->b = b;
                min = abs(mb - ma);
            }
        }
    }
}

void i2s_hal_set_clock_src(i2s_hal_context_t *hal, i2s_clock_src_t sel)
{
    i2s_ll_set_tx_clk_src(hal->dev, sel);
    i2s_ll_set_rx_clk_src(hal->dev, sel);
}

void i2s_hal_tx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor)
{
    i2s_ll_clk_cal_t clk_set = {0};
    i2s_hal_clk_cal(sclk, fbck, factor, &clk_set);
    i2s_ll_set_tx_clk(hal->dev, &clk_set);
}

void i2s_hal_rx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor)
{
    i2s_ll_clk_cal_t clk_set = {0};
    i2s_hal_clk_cal(sclk, fbck, factor, &clk_set);
    i2s_ll_set_rx_clk(hal->dev, &clk_set);
}

void i2s_hal_enable_master_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_set_tx_slave_mod(hal->dev, 0); //TX master
    i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_enable_slave_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_set_tx_slave_mod(hal->dev, 1); //TX Slave
    i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num)
{
    //Get hardware instance.
    hal->dev = I2S_LL_GET_HW(i2s_num);
    i2s_ll_enable_clock(hal->dev);
}

static void i2s_hal_format_config(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
#if !SOC_I2S_SUPPORTS_TDM
    switch (hal_cfg->comm_fmt) {
    case I2S_COMM_FORMAT_STAND_MSB:
        if (hal_cfg->mode & I2S_MODE_TX) {
            i2s_ll_set_tx_format_msb_align(hal->dev);
        }
        if (hal_cfg->mode & I2S_MODE_RX) {
            i2s_ll_set_rx_format_msb_align(hal->dev);
        }
        break;
    case I2S_COMM_FORMAT_STAND_PCM_SHORT:
        if (hal_cfg->mode & I2S_MODE_TX) {
            i2s_ll_set_tx_pcm_short(hal->dev);
        }
        if (hal_cfg->mode & I2S_MODE_RX) {
            i2s_ll_set_rx_pcm_short(hal->dev);
        }
        break;
    case I2S_COMM_FORMAT_STAND_PCM_LONG:
        if (hal_cfg->mode & I2S_MODE_TX) {
            i2s_ll_set_tx_pcm_long(hal->dev);
        }
        if (hal_cfg->mode & I2S_MODE_RX) {
            i2s_ll_set_rx_pcm_long(hal->dev);
        }
        break;
    default: //I2S_COMM_FORMAT_STAND_I2S
        if (hal_cfg->mode & I2S_MODE_TX) {
            i2s_ll_set_tx_format_philip(hal->dev);
        }
        if (hal_cfg->mode & I2S_MODE_RX) {
            i2s_ll_set_rx_format_philip(hal->dev);
        }
        break;
    }
    if (hal_cfg->ch == I2S_CHANNEL_MONO) {
        if (hal_cfg->mode & I2S_MODE_TX) {
            i2s_ll_tx_mono_mode_ena(hal->dev, true);
        }
        if (hal_cfg->mode & I2S_MODE_RX) {
            i2s_ll_rx_mono_mode_ena(hal->dev, true);
        }
    }
#else
    int slot_bits = hal_cfg->bits_cfg.chan_bits;
    int slot_num = hal_cfg->chan_cfg.total_chan;
    bool msb_shift_en = false;
    int tdm_ws_width = 0;
    switch (hal_cfg->comm_fmt) {
    case I2S_COMM_FORMAT_STAND_MSB:
        msb_shift_en = false;
        tdm_ws_width = slot_num * slot_bits / 2;
        break;
    case I2S_COMM_FORMAT_STAND_PCM_SHORT:
        msb_shift_en = false;
        tdm_ws_width = 1;
        break;
    case I2S_COMM_FORMAT_STAND_PCM_LONG:
        msb_shift_en = false;
        tdm_ws_width = slot_bits;
        break;
    default: //I2S_COMM_FORMAT_STAND_I2S
        msb_shift_en = true;
        tdm_ws_width = slot_num * slot_bits / 2;
        break;
    }
    if (hal_cfg->mode & I2S_MODE_TX) {
        i2s_ll_tx_msb_shift_enable(hal->dev, msb_shift_en);
        i2s_ll_set_tx_tdm_ws_width(hal->dev, tdm_ws_width);
        i2s_ll_set_tx_half_sample_bit(hal->dev, slot_num * slot_bits / 2);
    }
    if (hal_cfg->mode & I2S_MODE_RX) {
        i2s_ll_rx_msb_shift_enable(hal->dev, msb_shift_en);
        i2s_ll_set_rx_tdm_ws_width(hal->dev, tdm_ws_width);
        i2s_ll_set_rx_half_sample_bit(hal->dev, slot_num * slot_bits / 2);
    }
#endif
}

void i2s_hal_samples_config(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    int data_bits = hal_cfg->bits_cfg.sample_bits;
    int slot_bits = hal_cfg->bits_cfg.chan_bits;
#if SOC_I2S_SUPPORTS_TDM
    int slot_num = hal_cfg->chan_cfg.total_chan;
    if (hal_cfg->mode & I2S_MODE_TX) {
        i2s_ll_set_tx_slot_num(hal->dev, slot_num);
        i2s_ll_set_tx_sample_bit(hal->dev, slot_bits, data_bits);
    }
    if (hal_cfg->mode & I2S_MODE_RX) {
        i2s_ll_set_rx_slot_num(hal->dev, slot_num);
        i2s_ll_set_rx_sample_bit(hal->dev, slot_bits, data_bits);
    }
#else
    if (hal_cfg->mode & I2S_MODE_TX) {
        i2s_ll_set_tx_sample_bit(hal->dev, slot_bits, data_bits);
    }
    if (hal_cfg->mode & I2S_MODE_RX) {
        i2s_ll_set_rx_sample_bit(hal->dev, slot_bits, data_bits);
    }
#endif
    //I2S standards config: Philip, MSB or PCM, Only I2S mode should do this configuration.
    if ((hal_cfg->mode & (~(I2S_MODE_I2S))) == 0) {
        i2s_hal_format_config(hal, hal_cfg);
    }
}

void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    bool is_slave = ((hal_cfg->mode & I2S_MODE_SLAVE) > 0);

    if (hal_cfg->mode & I2S_MODE_TX) {
        i2s_ll_stop_tx(hal->dev);
        i2s_ll_reset_tx(hal->dev);
#if SOC_I2S_SUPPORTS_TDM
        i2s_ll_set_tx_pdm_en(hal->dev, false);
        i2s_ll_enable_tx_clock(hal->dev);
        i2s_ll_set_tx_clk_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
        i2s_ll_mclk_use_tx_clk(hal->dev);

        i2s_ll_set_tx_active_slot_mask(hal->dev, hal_cfg->chan_cfg.chan_mask);
        i2s_ll_tx_left_align_enable(hal->dev, hal_cfg->flags.left_align_en);
        i2s_ll_tx_big_endian_enable(hal->dev, hal_cfg->flags.big_edin_en);
        i2s_ll_tx_set_bit_order(hal->dev, hal_cfg->flags.bit_order_msb_en);
        i2s_ll_tx_set_skip_mask(hal->dev, hal_cfg->flags.skip_msk_en);
#else
        i2s_ll_tx_msb_right_en(hal->dev, false);
        i2s_ll_tx_right_first_en(hal->dev, false);
        i2s_ll_tx_fifo_mod_force_en(hal->dev, true);
#endif
        i2s_ll_set_tx_slave_mod(hal->dev, is_slave); //TX Slave
    }


    if (hal_cfg->mode & I2S_MODE_RX) {
        i2s_ll_stop_rx(hal->dev);
        i2s_ll_reset_rx(hal->dev);
#if SOC_I2S_SUPPORTS_TDM
        i2s_ll_set_rx_pdm_en(hal->dev, false);
        i2s_ll_enable_rx_clock(hal->dev);
        i2s_ll_set_rx_clk_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
        i2s_ll_mclk_use_rx_clk(hal->dev);

        i2s_ll_set_rx_active_slot_mask(hal->dev, hal_cfg->chan_cfg.chan_mask);
        i2s_ll_rx_left_align_enable(hal->dev, hal_cfg->flags.left_align_en);
        i2s_ll_rx_big_endian_enable(hal->dev, hal_cfg->flags.big_edin_en);
        i2s_ll_rx_set_bit_order(hal->dev, hal_cfg->flags.bit_order_msb_en);
#else
        i2s_ll_rx_msb_right_en(hal->dev, false);
        i2s_ll_rx_right_first_en(hal->dev, false);
        i2s_ll_rx_fifo_mod_force_en(hal->dev, true);
#endif
        i2s_ll_set_rx_slave_mod(hal->dev, is_slave); //RX Slave
    }
#if SOC_I2S_SUPPORTS_ADC_DAC
    if (hal_cfg->mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN)) {
        if (hal_cfg->mode & I2S_MODE_DAC_BUILT_IN) {
            i2s_ll_build_in_dac_ena(hal->dev);
        }
        if (hal_cfg->mode & I2S_MODE_ADC_BUILT_IN) {
            i2s_ll_build_in_adc_ena(hal->dev);
        }
        // Buildin ADC and DAC have nothing to do with communication format configuration.
        return;
    }
#endif

#if SOC_I2S_SUPPORTS_PDM
    bool is_pdm = ((hal_cfg->mode & I2S_MODE_PDM) > 0);
#if SOC_I2S_SUPPORTS_PDM_TX
    if (hal_cfg->mode & I2S_MODE_TX) {
        if (is_pdm) {
            i2s_ll_tx_pdm_cfg(hal->dev, hal_cfg->sample_rate);
        } else {
            i2s_ll_set_tx_pdm_en(hal->dev, false);
        }
    }
#endif // SOC_I2S_SUPPORTS_PDM_TX
#if SOC_I2S_SUPPORTS_PDM_RX
    if (hal_cfg->mode & I2S_MODE_RX) {
        if (is_pdm) {
            i2s_ll_rx_pdm_cfg(hal->dev);
        } else {
            i2s_ll_set_rx_pdm_en(hal->dev, false);
        }
    }
#endif // SOC_I2S_SUPPORTS_PDM_RX
#endif // SOC_I2S_SUPPORTS_PDM
    //Configure I2S slot number,sample bit.
    i2s_hal_samples_config(hal, hal_cfg);

}
