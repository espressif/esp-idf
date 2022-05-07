/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


// The HAL layer for I2S (common part)

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "hal/i2s_hal.h"

/**
 * @brief Calculate the closest sample rate clock configuration.
 *        clock relationship:
 *        Fmclk = bck_div*fbck = fsclk/(mclk_div+b/a)
 *
 * @param clk_cfg I2S clock configuration(input)
 * @param cal Point to `i2s_ll_mclk_div_t` structure(output).
 */
static void i2s_hal_mclk_div_decimal_cal(i2s_hal_clock_cfg_t *clk_cfg, i2s_ll_mclk_div_t *cal)
{
    int ma = 0;
    int mb = 0;
    cal->mclk_div = clk_cfg->mclk_div;
    cal->a = 1;
    cal->b = 0;

    uint32_t freq_diff = abs(clk_cfg->sclk - clk_cfg->mclk * cal->mclk_div);
    if (!freq_diff) {
        return;
    }
    float decimal = freq_diff / (float)clk_cfg->mclk;
    // Carry bit if the decimal is greater than 1.0 - 1.0 / (63.0 * 2) = 125.0 / 126.0
    if (decimal > 125.0 / 126.0) {
        cal->mclk_div++;
        return;
    }
    uint32_t min = ~0;
    for (int a = 2; a <= I2S_LL_MCLK_DIVIDER_MAX; a++) {
        // Calculate the closest 'b' in this loop, no need to loop 'b' to seek the closest value
        int b = (int)(a * (freq_diff / (double)clk_cfg->mclk) + 0.5);
        ma = freq_diff * a;
        mb = clk_cfg->mclk * b;
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

void i2s_hal_set_clock_src(i2s_hal_context_t *hal, i2s_clock_src_t sel)
{
    i2s_ll_tx_clk_set_src(hal->dev, sel);
    i2s_ll_rx_clk_set_src(hal->dev, sel);
}

void i2s_hal_tx_clock_config(i2s_hal_context_t *hal, i2s_hal_clock_cfg_t *clk_cfg)
{
    i2s_ll_mclk_div_t mclk_set;
    i2s_hal_mclk_div_decimal_cal(clk_cfg, &mclk_set);
    i2s_ll_tx_set_clk(hal->dev, &mclk_set);
    i2s_ll_tx_set_bck_div_num(hal->dev, clk_cfg->bclk_div);
}

void i2s_hal_rx_clock_config(i2s_hal_context_t *hal, i2s_hal_clock_cfg_t *clk_cfg)
{
    i2s_ll_mclk_div_t mclk_set;
    i2s_hal_mclk_div_decimal_cal(clk_cfg, &mclk_set);
    i2s_ll_rx_set_clk(hal->dev, &mclk_set);
    i2s_ll_rx_set_bck_div_num(hal->dev, clk_cfg->bclk_div);
}

void i2s_hal_enable_master_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_tx_set_slave_mod(hal->dev, false); //TX master
    i2s_ll_rx_set_slave_mod(hal->dev, true); //RX Slave
}

void i2s_hal_enable_slave_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_tx_set_slave_mod(hal->dev, true); //TX Slave
    i2s_ll_rx_set_slave_mod(hal->dev, true); //RX Slave
}

void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num)
{
    /* Get hardware instance */
    hal->dev = I2S_LL_GET_HW(i2s_num);
}

#if SOC_I2S_SUPPORTS_PDM_TX
void i2s_hal_tx_set_pdm_mode_default(i2s_hal_context_t *hal, uint32_t sample_rate)
{
    /* enable pdm tx mode */
    i2s_ll_tx_enable_pdm(hal->dev, true);
#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_tx_enable_clock(hal->dev);
    i2s_ll_tx_clk_set_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
    i2s_ll_mclk_use_tx_clk(hal->dev);
    /* Still need to enable the first 2 TDM channel mask to get the correct number of frame */
    i2s_ll_tx_set_active_chan_mask(hal->dev, I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1);
#else
    i2s_ll_tx_force_enable_fifo_mod(hal->dev, true);
#endif
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
}
#endif // SOC_I2S_SUPPORTS_PDM_TX

#if SOC_I2S_SUPPORTS_PDM_RX
void i2s_hal_rx_set_pdm_mode_default(i2s_hal_context_t *hal)
{
    /* enable pdm rx mode */
    i2s_ll_rx_enable_pdm(hal->dev, true);
    /* set pdm rx downsample number */
    i2s_ll_rx_set_pdm_dsr(hal->dev, I2S_PDM_DSR_8S);
#if !SOC_I2S_SUPPORTS_TDM
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
#endif
#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_rx_enable_clock(hal->dev);
    i2s_ll_rx_clk_set_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
    i2s_ll_mclk_use_rx_clk(hal->dev);
    /* Still need to enable the first 2 TDM channel mask to get the correct number of frame */
    i2s_ll_rx_set_active_chan_mask(hal->dev, I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1);
#else
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
#endif
}
#endif // SOC_I2S_SUPPORTS_PDM_RX


void i2s_hal_tx_set_common_mode(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg)
{
    /* Disable PDM tx mode and enable TDM mode (if support) */
    i2s_ll_tx_enable_pdm(hal->dev, false);

#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_tx_enable_clock(hal->dev);
    i2s_ll_tx_clk_set_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
    i2s_ll_mclk_use_tx_clk(hal->dev);

    // In TDM mode(more than 2 channels), the ws polarity should be high first.
    if (hal_cfg->total_chan > 2) {
        i2s_ll_tx_set_ws_idle_pol(hal->dev, true);
    }
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
    /* Disable PDM rx mode and enable TDM rx mode (if support)*/
    i2s_ll_rx_enable_pdm(hal->dev, false);

#if SOC_I2S_SUPPORTS_TDM
    i2s_ll_rx_enable_clock(hal->dev);
    i2s_ll_rx_clk_set_src(hal->dev, I2S_CLK_D2CLK); // Set I2S_CLK_D2CLK as default
    i2s_ll_mclk_use_rx_clk(hal->dev);

    // In TDM mode(more than 2 channels), the ws polarity should be high first.
    if (hal_cfg->total_chan > 2) {
        i2s_ll_rx_set_ws_idle_pol(hal->dev, true);
    }
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
    uint32_t chan_bits = hal_cfg->chan_bits;
    uint32_t data_bits = hal_cfg->sample_bits;
    bool is_mono = (hal_cfg->chan_fmt == I2S_CHANNEL_FMT_ONLY_RIGHT) ||
                   (hal_cfg->chan_fmt == I2S_CHANNEL_FMT_ONLY_LEFT);

    /* Set channel number and valid data bits */
#if SOC_I2S_SUPPORTS_TDM
    chan_num = hal_cfg->total_chan;
    i2s_ll_tx_set_active_chan_mask(hal->dev, hal_cfg->chan_mask >> 16);
    i2s_ll_tx_set_chan_num(hal->dev, chan_num);
#else
    i2s_ll_tx_set_chan_mod(hal->dev, hal_cfg->chan_fmt < I2S_CHANNEL_FMT_ONLY_RIGHT ? hal_cfg->chan_fmt : (hal_cfg->chan_fmt >> 1)); // 0-two channel;1-right;2-left;3-righ;4-left
#endif
    i2s_ll_tx_set_sample_bit(hal->dev, chan_bits, data_bits);
    i2s_ll_tx_enable_mono_mode(hal->dev, is_mono);

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
    uint32_t chan_bits = hal_cfg->chan_bits;
    uint32_t data_bits = hal_cfg->sample_bits;
    bool is_mono = (hal_cfg->chan_fmt == I2S_CHANNEL_FMT_ONLY_RIGHT) ||
                   (hal_cfg->chan_fmt == I2S_CHANNEL_FMT_ONLY_LEFT);

#if SOC_I2S_SUPPORTS_TDM
    chan_num = hal_cfg->total_chan;
    i2s_ll_rx_set_active_chan_mask(hal->dev, hal_cfg->chan_mask >> 16);
    i2s_ll_rx_set_chan_num(hal->dev, chan_num);
#else
    i2s_ll_rx_set_chan_mod(hal->dev, hal_cfg->chan_fmt < I2S_CHANNEL_FMT_ONLY_RIGHT ? hal_cfg->chan_fmt : (hal_cfg->chan_fmt >> 1)); // 0-two channel;1-right;2-left;3-righ;4-left
#endif
    i2s_ll_rx_set_sample_bit(hal->dev, chan_bits, data_bits);
    i2s_ll_rx_enable_mono_mode(hal->dev, is_mono);

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
#if SOC_I2S_SUPPORTS_ADC
    if (hal_cfg->mode & I2S_MODE_ADC_BUILT_IN) {
        /* In ADC built-in mode, we need to call i2s_set_adc_mode to initialize the specific ADC channel.
         * In the current stage, we only support ADC1 and single channel mode.
         * In default data mode, the ADC data is in 12-bit resolution mode.
         */
        i2s_ll_enable_builtin_adc(hal->dev, true);
        return;
    }
    i2s_ll_enable_builtin_adc(hal->dev, false);
#endif
#if SOC_I2S_SUPPORTS_DAC
    if (hal_cfg->mode & I2S_MODE_DAC_BUILT_IN) {
        i2s_ll_enable_builtin_dac(hal->dev, true);
        return;
    }
    i2s_ll_enable_builtin_dac(hal->dev, false);
#endif
    /* Set configurations for TX mode */
    if (hal_cfg->mode & I2S_MODE_TX) {
        i2s_ll_tx_stop(hal->dev);
        i2s_ll_tx_reset(hal->dev);
        i2s_ll_tx_set_slave_mod(hal->dev, (hal_cfg->mode & I2S_MODE_SLAVE) != 0); //TX Slave
#if SOC_I2S_SUPPORTS_PDM_TX
        if (hal_cfg->mode & I2S_MODE_PDM) {
            /* Set tx pdm mode */
            i2s_hal_tx_set_pdm_mode_default(hal, hal_cfg->sample_rate);
        } else
#endif
        {
            /* Set tx common mode */
            i2s_hal_tx_set_common_mode(hal, hal_cfg);
        }
        i2s_hal_tx_set_channel_style(hal, hal_cfg);
    }

    /* Set configurations for RX mode */
    if (hal_cfg->mode & I2S_MODE_RX) {
        i2s_ll_rx_stop(hal->dev);
        i2s_ll_rx_reset(hal->dev);
        i2s_ll_rx_set_slave_mod(hal->dev, (hal_cfg->mode & I2S_MODE_SLAVE) != 0); //RX Slave
#if SOC_I2S_SUPPORTS_PDM_RX
        if (hal_cfg->mode & I2S_MODE_PDM) {
            /* Set rx pdm mode */
            i2s_hal_rx_set_pdm_mode_default(hal);
        } else
#endif
        {
            /* Set rx common mode */
            i2s_hal_rx_set_common_mode(hal, hal_cfg);
        }
        i2s_hal_rx_set_channel_style(hal, hal_cfg);
    }

    /* Set configurations for full-duplex mode */
    if ((hal_cfg->mode & I2S_MODE_RX) && (hal_cfg->mode & I2S_MODE_TX)) {
        i2s_ll_share_bck_ws(hal->dev, true);
        if (hal_cfg->mode & I2S_MODE_MASTER) {
            i2s_hal_enable_master_fd_mode(hal);
        } else {
            i2s_hal_enable_slave_fd_mode(hal);
        }
    }
}

void i2s_hal_start_tx(i2s_hal_context_t *hal)
{
    i2s_ll_tx_start(hal->dev);
}

void i2s_hal_start_rx(i2s_hal_context_t *hal)
{
    i2s_ll_rx_start(hal->dev);
}

void i2s_hal_stop_tx(i2s_hal_context_t *hal)
{
    i2s_ll_tx_stop(hal->dev);
}

void i2s_hal_stop_rx(i2s_hal_context_t *hal)
{
    i2s_ll_rx_stop(hal->dev);
}
