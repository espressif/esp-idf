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
    i2s_ll_tx_clk_set_src(hal->dev, sel);
    i2s_ll_rx_clk_set_src(hal->dev, sel);
}

void i2s_hal_tx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor)
{
    i2s_ll_clk_cal_t clk_set = {0};
    i2s_hal_clk_cal(sclk, fbck, factor, &clk_set);
    i2s_ll_tx_set_clk(hal->dev, &clk_set);
    i2s_ll_tx_set_bck_div_num(hal->dev, factor);
}

void i2s_hal_rx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor)
{
    i2s_ll_clk_cal_t clk_set = {0};
    i2s_hal_clk_cal(sclk, fbck, factor, &clk_set);
    i2s_ll_rx_set_clk(hal->dev, &clk_set);
    i2s_ll_rx_set_bck_div_num(hal->dev, factor);
}

void i2s_hal_enable_master_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_tx_set_slave_mod(hal->dev, 0); //TX master
    i2s_ll_rx_set_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_enable_slave_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_tx_set_slave_mod(hal->dev, 1); //TX Slave
    i2s_ll_rx_set_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num)
{
    //Get hardware instance.
    hal->dev = I2S_LL_GET_HW(i2s_num);
    i2s_ll_enable_clock(hal->dev);
}

void i2s_hal_tx_set_pdm_mode_default(i2s_hal_context_t *hal, uint32_t sample_rate)
{
#if SOC_I2S_SUPPORTS_PDM_TX
    /* enable pdm tx mode */
    i2s_ll_tx_enable_pdm(hal->dev, true);
    /* set pdm tx default presacle */
    i2s_ll_tx_set_pdm_prescale(hal->dev, 0);
    /* set pdm tx default sacle of high pass filter */
    i2s_ll_tx_set_pdm_hp_scale(hal->dev, I2S_PDM_SIG_SCALING_MUL_1);
    /* set pdm tx default sacle of low pass filter */
    i2s_ll_tx_set_pdm_lp_scale(hal->dev, I2S_PDM_SIG_SCALING_MUL_1);
    /* set pdm tx default sacle of sinc filter */
    i2s_ll_tx_set_pdm_sinc_scale(hal->dev, I2S_PDM_SIG_SCALING_MUL_1);
    /* set pdm tx default sacle of sigma-delta filter */
    i2s_ll_tx_set_pdm_sd_scale(hal->dev, I2S_PDM_SIG_SCALING_MUL_1);
    /* set pdm tx sample rate */
    i2s_ll_tx_set_pdm_fpfs(hal->dev, 960, sample_rate / 100);

#if SOC_I2S_SUPPORTS_PDM_CODEC
    /* enable pdm high pass filter */
    i2s_ll_tx_enable_pdm_hp_filter(hal->dev, true);
    /* set pdm tx high pass filter parameters */
    i2s_ll_tx_set_pdm_hp_filter_param0(hal->dev, 6);
    i2s_ll_tx_set_pdm_hp_filter_param5(hal->dev, 7);
    /* enable pdm sigma-delta codec */
    i2s_ll_tx_enable_pdm_sd_codec(hal->dev, true);
    /* set pdm tx sigma-delta codec dither */
    i2s_ll_tx_set_pdm_sd_dither(hal->dev, 0);
    i2s_ll_tx_set_pdm_sd_dither2(hal->dev, 0);

#endif // SOC_I2S_SUPPORTS_PDM_CODEC
#endif // SOC_I2S_SUPPORTS_PDM_TX
}

void i2s_hal_rx_set_pdm_mode_default(i2s_hal_context_t *hal)
{
#if SOC_I2S_SUPPORTS_PDM_RX
    /* enable pdm rx mode */
    i2s_ll_rx_enable_pdm(hal->dev, true);
    /* set pdm rx downsample number */
    i2s_ll_rx_set_pdm_dsr(hal->dev, I2S_PDM_DSR_8S);
#endif // SOC_I2S_SUPPORTS_PDM_RX
}


void i2s_hal_tx_set_common_mode(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    /* disable pdm tx mode */
    i2s_ll_tx_enable_pdm(hal->dev, false);

#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_tx_enable_clock(hal->dev);
    i2s_ll_tx_clk_set_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
    i2s_ll_mclk_use_tx_clk(hal->dev);

    i2s_ll_tx_set_active_chan_mask(hal->dev, hal_cfg->chan_mask);
    i2s_ll_tx_enable_left_align(hal->dev, hal_cfg->left_align);
    i2s_ll_tx_enable_big_endian(hal->dev, hal_cfg->big_edin);
    i2s_ll_tx_set_bit_order(hal->dev, hal_cfg->bit_order_msb);
    i2s_ll_tx_set_skip_mask(hal->dev, hal_cfg->skip_msk);
#else
    i2s_ll_tx_enable_msb_right(hal->dev, false);
    i2s_ll_tx_enable_right_first(hal->dev, false);
    i2s_ll_tx_force_enable_fifo_mod(hal->dev, true);
#endif
}

void i2s_hal_rx_set_common_mode(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    /* disable pdm rx mode */
    i2s_ll_rx_enable_pdm(hal->dev, false);

#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_rx_enable_clock(hal->dev);
    i2s_ll_rx_clk_set_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
    i2s_ll_mclk_use_rx_clk(hal->dev);

    i2s_ll_rx_set_active_chan_mask(hal->dev, hal_cfg->chan_mask);
    i2s_ll_rx_enable_left_align(hal->dev, hal_cfg->left_align);
    i2s_ll_rx_enable_big_endian(hal->dev, hal_cfg->big_edin);
    i2s_ll_rx_set_bit_order(hal->dev, hal_cfg->bit_order_msb);
#else
    i2s_ll_rx_enable_msb_right(hal->dev, false);
    i2s_ll_rx_enable_right_first(hal->dev, false);
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
#endif
}

static uint32_t i2s_hal_get_ws_bit(i2s_comm_format_t fmt, uint32_t chan_num, uint32_t chan_bits)
{
    switch (fmt) {
    case I2S_COMM_FORMAT_STAND_MSB:
        return chan_num * chan_bits / 2;
    case I2S_COMM_FORMAT_STAND_PCM_SHORT:
        return 1;
    case I2S_COMM_FORMAT_STAND_PCM_LONG:
        return chan_bits;
    default: //I2S_COMM_FORMAT_STAND_I2S
        return chan_num * chan_bits / 2;
    }
}

void i2s_hal_tx_set_channel_style(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    uint32_t chan_num = 2;
    uint32_t chan_bits = hal_cfg->bits_cfg.chan_bits;
    uint32_t data_bits = hal_cfg->bits_cfg.sample_bits;

    /* Set channel number and valid data bits */
#if SOC_I2S_SUPPORTS_TDM
    chan_num = hal_cfg->total_chan;
    i2s_ll_tx_set_chan_num(hal->dev, chan_num);
#endif
    i2s_ll_tx_set_sample_bit(hal->dev, chan_bits, data_bits);

    /* Set communication format */
    bool shift_en = hal_cfg->comm_fmt == I2S_COMM_FORMAT_STAND_I2S ? true : false;
    uint32_t ws_width = i2s_hal_get_ws_bit(hal_cfg->comm_fmt, chan_num, chan_bits);
    i2s_ll_tx_enable_msb_shift(hal->dev, shift_en);
    i2s_ll_tx_set_ws_width(hal->dev, ws_width);
#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_tx_set_half_sample_bit(hal->dev, chan_num * chan_bits / 2);
#endif
}

void i2s_hal_rx_set_channel_style(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    uint32_t chan_num = 2;
    uint32_t chan_bits = hal_cfg->bits_cfg.chan_bits;
    uint32_t data_bits = hal_cfg->bits_cfg.sample_bits;

#if SOC_I2S_SUPPORTS_TDM
    chan_num = hal_cfg->total_chan;
    i2s_ll_rx_set_chan_num(hal->dev, chan_num);
#endif
    i2s_ll_rx_set_sample_bit(hal->dev, chan_bits, data_bits);

    /* Set communication format */
    bool shift_en = hal_cfg->comm_fmt == I2S_COMM_FORMAT_STAND_I2S ? true : false;
    uint32_t ws_width = i2s_hal_get_ws_bit(hal_cfg->comm_fmt, chan_num, chan_bits);
    i2s_ll_rx_enable_msb_shift(hal->dev, shift_en);
    i2s_ll_rx_set_ws_width(hal->dev, ws_width);
#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_rx_set_half_sample_bit(hal->dev, chan_num * chan_bits / 2);
#endif
}

void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    if (hal_cfg->mode & I2S_MODE_TX) {
        i2s_ll_tx_stop(hal->dev);
        i2s_ll_tx_reset(hal->dev);
        i2s_ll_tx_set_slave_mod(hal->dev, (hal_cfg->mode & I2S_MODE_SLAVE) != 0); //TX Slave
        if (hal_cfg->mode & I2S_MODE_PDM) {
            /* Set tx pdm mode */
            i2s_hal_tx_set_pdm_mode_default(hal, hal_cfg->sample_rate);
        } else {
            /* Set tx common mode */
            i2s_hal_tx_set_common_mode(hal, hal_cfg);
            i2s_hal_tx_set_channel_style(hal, hal_cfg);
        }
    }
    if (hal_cfg->mode & I2S_MODE_RX) {
        i2s_ll_rx_stop(hal->dev);
        i2s_ll_rx_reset(hal->dev);
        i2s_ll_rx_set_slave_mod(hal->dev, (hal_cfg->mode & I2S_MODE_SLAVE) != 0); //RX Slave
        if (hal_cfg->mode & I2S_MODE_PDM) {
            /* Set rx pdm mode */
            i2s_hal_rx_set_pdm_mode_default(hal);
        } else {
            /* Set rx common mode */
            i2s_hal_rx_set_common_mode(hal, hal_cfg);
            i2s_hal_rx_set_channel_style(hal, hal_cfg);
        }
    }
}
