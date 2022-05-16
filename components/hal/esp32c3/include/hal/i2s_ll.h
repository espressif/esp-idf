/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I2S register operations
/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once
#include <stdbool.h>
#include "hal/misc.h"
#include "soc/i2s_periph.h"
#include "soc/i2s_struct.h"
#include "hal/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_LL_GET_HW(num)             (&I2S0)

#define I2S_LL_TDM_CH_MASK             (0xffff)
#define I2S_LL_PDM_BCK_FACTOR          (64)
#define I2S_LL_BASE_CLK                (2*APB_CLK_FREQ)

#define I2S_LL_MCLK_DIVIDER_BIT_WIDTH  (9)
#define I2S_LL_MCLK_DIVIDER_MAX        ((1 << I2S_LL_MCLK_DIVIDER_BIT_WIDTH) - 1)

/* I2S clock configuration structure */
typedef struct {
    uint16_t mclk_div; // I2S module clock devider, Fmclk = Fsclk /(mclk_div+b/a)
    uint16_t a;
    uint16_t b;        // The decimal part of module clock devider, the decimal is: b/a
} i2s_ll_mclk_div_t;

/**
 * @brief I2S module general init, enable I2S clock.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_enable_clock(i2s_dev_t *hw)
{
    hw->tx_clkm_conf.clk_en = 1;
}

/**
 * @brief I2S module disable I2S clock.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_disable_clock(i2s_dev_t *hw)
{
    hw->tx_clkm_conf.clk_en = 0;
}

/**
 * @brief Enable I2S tx module clock
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_enable_clock(i2s_dev_t *hw)
{
    hw->tx_clkm_conf.tx_clk_active = 1;
}

/**
 * @brief Enable I2S rx module clock
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_enable_clock(i2s_dev_t *hw)
{
    hw->rx_clkm_conf.rx_clk_active = 1;
}

/**
 * @brief Disable I2S tx module clock
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_disable_clock(i2s_dev_t *hw)
{
    hw->tx_clkm_conf.tx_clk_active = 0;
}

/**
 * @brief Disable I2S rx module clock
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_disable_clock(i2s_dev_t *hw)
{
    hw->rx_clkm_conf.rx_clk_active = 0;
}

/**
 * @brief I2S mclk use tx module clock
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_mclk_use_tx_clk(i2s_dev_t *hw)
{
    hw->rx_clkm_conf.mclk_sel = 0;
}

/**
 * @brief I2S mclk use rx module clock
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_mclk_use_rx_clk(i2s_dev_t *hw)
{
    hw->rx_clkm_conf.mclk_sel = 1;
}

/**
 * @brief Enable I2S TX slave mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param slave_en Set true to enable slave mode
 */
static inline void i2s_ll_tx_set_slave_mod(i2s_dev_t *hw, bool slave_en)
{
    hw->tx_conf.tx_slave_mod = slave_en;
}

/**
 * @brief Enable I2S RX slave mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param slave_en Set true to enable slave mode
 */
static inline void i2s_ll_rx_set_slave_mod(i2s_dev_t *hw, bool slave_en)
{
    hw->rx_conf.rx_slave_mod = slave_en;
}

/**
 * @brief Reset I2S TX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_reset(i2s_dev_t *hw)
{
    hw->tx_conf.tx_reset = 1;
    hw->tx_conf.tx_reset = 0;
}

/**
 * @brief Reset I2S RX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_reset(i2s_dev_t *hw)
{
    hw->rx_conf.rx_reset = 1;
    hw->rx_conf.rx_reset = 0;
}

/**
 * @brief Reset I2S TX FIFO
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_reset_fifo(i2s_dev_t *hw)
{
    hw->tx_conf.tx_fifo_reset = 1;
    hw->tx_conf.tx_fifo_reset = 0;
}

/**
 * @brief Reset I2S RX FIFO
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_reset_fifo(i2s_dev_t *hw)
{
    hw->rx_conf.rx_fifo_reset = 1;
    hw->rx_conf.rx_fifo_reset = 0;
}

/**
 * @brief Set TX source clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param src I2S source clock.
 */
static inline void i2s_ll_tx_clk_set_src(i2s_dev_t *hw, i2s_clock_src_t src)
{
    hw->tx_clkm_conf.tx_clk_sel = 2;
}

/**
 * @brief Set RX source clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param src I2S source clock,  ESP32-C3 only support `I2S_CLK_D2CLK`
 */
static inline void i2s_ll_rx_clk_set_src(i2s_dev_t *hw, i2s_clock_src_t src)
{
    hw->rx_clkm_conf.rx_clk_sel = 2;
}

/**
 * @brief Set I2S tx bck div num
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx bck div num
 */
static inline void i2s_ll_tx_set_bck_div_num(i2s_dev_t *hw, uint32_t val)
{
    hw->tx_conf1.tx_bck_div_num = val - 1;
}

/**
 * @brief Configure I2S TX clock devider
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param set Pointer to I2S clock devider configuration paramater
 */
static inline void i2s_ll_tx_set_clk(i2s_dev_t *hw, i2s_ll_mclk_div_t *set)
{
    if (set->a == 0 || set->b == 0) {
        hw->tx_clkm_div_conf.tx_clkm_div_x = 0;
        hw->tx_clkm_div_conf.tx_clkm_div_y = 0;
        hw->tx_clkm_div_conf.tx_clkm_div_z = 0;
    } else {
        if (set->b > set->a / 2) {
            hw->tx_clkm_div_conf.tx_clkm_div_x = set->a / (set->a - set->b) - 1;
            hw->tx_clkm_div_conf.tx_clkm_div_y = set->a % (set->a - set->b);
            hw->tx_clkm_div_conf.tx_clkm_div_z = set->a - set->b;
            hw->tx_clkm_div_conf.tx_clkm_div_yn1 = 1;
        } else {
            hw->tx_clkm_div_conf.tx_clkm_div_x = set->a / set->b - 1;
            hw->tx_clkm_div_conf.tx_clkm_div_y = set->a % set->b + 1;
            hw->tx_clkm_div_conf.tx_clkm_div_z = set->b;
            hw->tx_clkm_div_conf.tx_clkm_div_yn1 = 0;
        }
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->tx_clkm_conf, tx_clkm_div_num, set->mclk_div);
}

/**
 * @brief Set I2S rx bck div num
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx bck div num
 */
static inline void i2s_ll_rx_set_bck_div_num(i2s_dev_t *hw, uint32_t val)
{
    hw->rx_conf1.rx_bck_div_num = val - 1;
}

/**
 * @brief Configure I2S RX clock devider
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param set Pointer to I2S clock devider configuration paramater
 */
static inline void i2s_ll_rx_set_clk(i2s_dev_t *hw, i2s_ll_mclk_div_t *set)
{
    if (set->a == 0 || set->b == 0) {
        hw->rx_clkm_div_conf.rx_clkm_div_x = 0;
        hw->rx_clkm_div_conf.rx_clkm_div_y = 0;
        hw->rx_clkm_div_conf.rx_clkm_div_z = 0;
    } else {
        if (set->b > set->a / 2) {
            hw->rx_clkm_div_conf.rx_clkm_div_x = set->a / (set->a - set->b) - 1;
            hw->rx_clkm_div_conf.rx_clkm_div_y = set->a % (set->a - set->b);
            hw->rx_clkm_div_conf.rx_clkm_div_z = set->a - set->b;
            hw->rx_clkm_div_conf.rx_clkm_div_yn1 = 1;
        } else {
            hw->rx_clkm_div_conf.rx_clkm_div_x = set->a / set->b - 1;
            hw->rx_clkm_div_conf.rx_clkm_div_y = set->a % set->b + 1;
            hw->rx_clkm_div_conf.rx_clkm_div_z = set->b;
            hw->rx_clkm_div_conf.rx_clkm_div_yn1 = 0;
        }
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->rx_clkm_conf, rx_clkm_div_num, set->mclk_div);
}

/**
 * @brief Start I2S TX
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_start(i2s_dev_t *hw)
{
    hw->tx_conf.tx_update = 0;
    hw->tx_conf.tx_update = 1;
    hw->tx_conf.tx_start = 1;
}

/**
 * @brief Start I2S RX
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_start(i2s_dev_t *hw)
{
    hw->rx_conf.rx_update = 0;
    hw->rx_conf.rx_update = 1;
    hw->rx_conf.rx_start = 1;
}

/**
 * @brief Stop I2S TX
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_stop(i2s_dev_t *hw)
{
    hw->tx_conf.tx_start = 0;
}

/**
 * @brief Stop I2S RX
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_stop(i2s_dev_t *hw)
{
    hw->rx_conf.rx_start = 0;
}

/**
 * @brief Configure TX WS signal width
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param width WS width in BCK cycle
 */
static inline void i2s_ll_tx_set_ws_width(i2s_dev_t *hw, int width)
{
    hw->tx_conf1.tx_tdm_ws_width = width - 1;
}

/**
 * @brief Configure RX WS signal width
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param width WS width in BCK cycle
 */
static inline void i2s_ll_rx_set_ws_width(i2s_dev_t *hw, int width)
{
    hw->rx_conf1.rx_tdm_ws_width = width - 1;
}

/**
 * @brief Configure the received length to trigger in_suc_eof interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param eof_num the byte length to trigger in_suc_eof interrupt
 */
static inline void i2s_ll_rx_set_eof_num(i2s_dev_t *hw, int eof_num)
{
    hw->rx_eof_num.rx_eof_num = eof_num;
}

/**
 * @brief Congfigure TX chan bit and audio data bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param chan_bit The chan bit width
 * @param data_bit The audio data bit width
 */
static inline void i2s_ll_tx_set_sample_bit(i2s_dev_t *hw, uint8_t chan_bit, int data_bit)
{
    hw->tx_conf1.tx_bits_mod = data_bit - 1;
    hw->tx_conf1.tx_tdm_chan_bits = chan_bit - 1;
}

/**
 * @brief Congfigure RX chan bit and audio data bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param chan_bit The chan bit width
 * @param data_bit The audio data bit width
 */
static inline void i2s_ll_rx_set_sample_bit(i2s_dev_t *hw, uint8_t chan_bit, int data_bit)
{
    hw->rx_conf1.rx_bits_mod = data_bit - 1;
    hw->rx_conf1.rx_tdm_chan_bits = chan_bit - 1;
}

/**
 * @brief Configure RX half_sample_bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param half_sample_bits half sample bit width
 */
static inline void i2s_ll_tx_set_half_sample_bit(i2s_dev_t *hw, int half_sample_bits)
{
    hw->tx_conf1.tx_half_sample_bits = half_sample_bits - 1;
}

/**
 * @brief Configure RX half_sample_bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param half_sample_bits half sample bit width
 */
static inline void i2s_ll_rx_set_half_sample_bit(i2s_dev_t *hw, int half_sample_bits)
{
    hw->rx_conf1.rx_half_sample_bits = half_sample_bits - 1;
}

/**
 * @brief Enable TX MSB shift, the data will be launch at the first BCK clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param msb_shift_enable Set true to enable MSB shift
 */
static inline void i2s_ll_tx_enable_msb_shift(i2s_dev_t *hw, bool msb_shift_enable)
{
    hw->tx_conf1.tx_msb_shift = msb_shift_enable;
}

/**
 * @brief Enable RX MSB shift, the data will be launch at the first BCK clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param msb_shift_enable Set true to enable MSB shift
 */
static inline void i2s_ll_rx_enable_msb_shift(i2s_dev_t *hw, bool msb_shift_enable)
{
    hw->rx_conf1.rx_msb_shift = msb_shift_enable;
}

/**
 * @brief Configure TX total chan number
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param total_num Total chan number
 */
static inline void i2s_ll_tx_set_chan_num(i2s_dev_t *hw, int total_num)
{
    hw->tx_tdm_ctrl.tx_tdm_tot_chan_num = total_num - 1;
}

/**
 * @brief Configure RX total chan number
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param total_num Total chan number
 */
static inline void i2s_ll_rx_set_chan_num(i2s_dev_t *hw, int total_num)
{
    hw->rx_tdm_ctrl.rx_tdm_tot_chan_num = total_num - 1;
}

/**
 * @brief Set the bimap of the active TX chan, only the active chan can launch audio data.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param chan_mask mask of tx active chan
 */
static inline void i2s_ll_tx_set_active_chan_mask(i2s_dev_t *hw, uint32_t chan_mask)
{
    typeof(hw->tx_tdm_ctrl) tdm_ctrl_reg = hw->tx_tdm_ctrl;
    tdm_ctrl_reg.val &= ~I2S_LL_TDM_CH_MASK;
    tdm_ctrl_reg.val |= chan_mask & I2S_LL_TDM_CH_MASK;
    hw->tx_tdm_ctrl.val = tdm_ctrl_reg.val;
}

/**
 * @brief Set the bimap of the active RX chan, only the active chan can receive audio data.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param chan_mask mask of rx active chan
 */
static inline void i2s_ll_rx_set_active_chan_mask(i2s_dev_t *hw, uint32_t chan_mask)
{
    typeof(hw->rx_tdm_ctrl) tdm_ctrl_reg = hw->rx_tdm_ctrl;
    tdm_ctrl_reg.val &= ~I2S_LL_TDM_CH_MASK;
    tdm_ctrl_reg.val |= chan_mask & I2S_LL_TDM_CH_MASK;
    hw->rx_tdm_ctrl.val = tdm_ctrl_reg.val;
}

/**
 * @brief Set TX WS signal pol level
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ws_pol_level pin level of WS(output) when receiving left channel data
 */
static inline void i2s_ll_tx_set_ws_idle_pol(i2s_dev_t *hw, bool ws_pol_level)
{
    hw->tx_conf.tx_ws_idle_pol = ws_pol_level;
}

/**
 * @brief Set RX WS signal pol level
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ws_pol_level pin level of WS(input) when receiving left channel data
 */
static inline void i2s_ll_rx_set_ws_idle_pol(i2s_dev_t *hw, bool ws_pol_level)
{
    hw->rx_conf.rx_ws_idle_pol = ws_pol_level;
}

/**
 * @brief Enable TX PDM mode.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pdm_enable Set true to TX enable PDM mode
 */
static inline void i2s_ll_tx_enable_pdm(i2s_dev_t *hw, bool pdm_enable)
{
    hw->tx_conf.tx_pdm_en = pdm_enable;
    hw->tx_conf.tx_tdm_en = !pdm_enable;
    hw->tx_pcm2pdm_conf.pcm2pdm_conv_en = pdm_enable;
}

/**
 * @brief Set I2S TX PDM prescale
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param prescale I2S TX PDM prescale
 */
static inline void i2s_ll_tx_set_pdm_prescale(i2s_dev_t *hw, bool prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->tx_pcm2pdm_conf, tx_pdm_prescale, prescale);
}

/**
 * @brief Set I2S TX PDM high pass filter scaling
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sig_scale I2S TX PDM signal scaling before transmit to the filter
 */
static inline void i2s_ll_tx_set_pdm_hp_scale(i2s_dev_t *hw, i2s_pdm_sig_scale_t sig_scale)
{
    hw->tx_pcm2pdm_conf.tx_pdm_hp_in_shift = sig_scale;
}

/**
 * @brief Set I2S TX PDM low pass filter scaling
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sig_scale I2S TX PDM signal scaling before transmit to the filter
 */
static inline void i2s_ll_tx_set_pdm_lp_scale(i2s_dev_t *hw, i2s_pdm_sig_scale_t sig_scale)
{
    hw->tx_pcm2pdm_conf.tx_pdm_lp_in_shift = sig_scale;
}

/**
 * @brief Set I2S TX PDM sinc filter scaling
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sig_scale I2S TX PDM signal scaling before transmit to the filter
 */
static inline void i2s_ll_tx_set_pdm_sinc_scale(i2s_dev_t *hw, i2s_pdm_sig_scale_t sig_scale)
{
    hw->tx_pcm2pdm_conf.tx_pdm_sinc_in_shift = sig_scale;
}

/**
 * @brief Set I2S TX PDM sigma-delta filter scaling
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sig_scale I2S TX PDM signal scaling before transmit to the filter
 */
static inline void i2s_ll_tx_set_pdm_sd_scale(i2s_dev_t *hw, i2s_pdm_sig_scale_t sig_scale)
{
    hw->tx_pcm2pdm_conf.tx_pdm_sigmadelta_in_shift = sig_scale;
}

/**
 * @brief Set I2S TX PDM high pass filter param0
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param param The fourth parameter of PDM TX IIR_HP filter stage 1 is (504 + I2S_TX_IIR_HP_MULT12_0[2:0])
 */
static inline void i2s_ll_tx_set_pdm_hp_filter_param0(i2s_dev_t *hw, uint32_t param)
{
    hw->tx_pcm2pdm_conf1.tx_iir_hp_mult12_0 = param;
}

/**
 * @brief Set I2S TX PDM high pass filter param5
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param param The fourth parameter of PDM TX IIR_HP filter stage 2 is (504 + I2S_TX_IIR_HP_MULT12_5[2:0])
 */
static inline void i2s_ll_tx_set_pdm_hp_filter_param5(i2s_dev_t *hw, uint32_t param)
{
    hw->tx_pcm2pdm_conf1.tx_iir_hp_mult12_5 = param;
}

/**
 * @brief Enable I2S TX PDM high pass filter
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param enable Set true to enable I2S TX PDM high pass filter, set false to bypass it
 */
static inline void i2s_ll_tx_enable_pdm_hp_filter(i2s_dev_t *hw, bool enable)
{
    hw->tx_pcm2pdm_conf.tx_pdm_hp_bypass = !enable;
}

/**
 * @brief Enable I2S TX PDM sigma-delta codec
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param dither I2S TX PDM sigmadelta dither value
 */
static inline void i2s_ll_tx_enable_pdm_sd_codec(i2s_dev_t *hw, bool enable)
{
    hw->tx_pcm2pdm_conf.tx_pdm_dac_2out_en = enable;
    hw->tx_pcm2pdm_conf.tx_pdm_dac_mode_en = enable;
}

/**
 * @brief Set I2S TX PDM sigma-delta codec dither
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param dither I2S TX PDM sigmadelta dither value
 */
static inline void i2s_ll_tx_set_pdm_sd_dither(i2s_dev_t *hw, uint32_t dither)
{
    hw->tx_pcm2pdm_conf.tx_pdm_sigmadelta_dither = dither;
}

/**
 * @brief Set I2S TX PDM sigma-delta codec dither
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param dither2 I2S TX PDM sigmadelta dither2 value
 */
static inline void i2s_ll_tx_set_pdm_sd_dither2(i2s_dev_t *hw, uint32_t dither2)
{
    hw->tx_pcm2pdm_conf.tx_pdm_sigmadelta_dither2 = dither2;
}

/**
 * @brief Configure I2S TX PDM sample rate
 *        Fpdm = 64*Fpcm*fp/fs
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param fp The fp value of TX PDM filter module group0.
 * @param fs The fs value of TX PDM filter module group0.
 */
static inline void i2s_ll_tx_set_pdm_fpfs(i2s_dev_t *hw, uint32_t fp, uint32_t fs)
{
    hw->tx_pcm2pdm_conf1.tx_pdm_fp = fp;
    hw->tx_pcm2pdm_conf1.tx_pdm_fs = fs;
    hw->tx_pcm2pdm_conf.tx_pdm_sinc_osr2 = fp / fs;
}

/**
 * @brief Get I2S TX PDM fp configuration paramater
 *
 * @param hw Peripheral I2S hardware instance address.
 * @return
 *        - fp configuration paramater
 */
static inline uint32_t i2s_ll_tx_get_pdm_fp(i2s_dev_t *hw)
{
    return hw->tx_pcm2pdm_conf1.tx_pdm_fp;
}

/**
 * @brief Get I2S TX PDM fs configuration paramater
 *
 * @param hw Peripheral I2S hardware instance address.
 * @return
 *        - fs configuration paramater
 */
static inline uint32_t i2s_ll_tx_get_pdm_fs(i2s_dev_t *hw)
{
    return hw->tx_pcm2pdm_conf1.tx_pdm_fs;
}

/**
 * @brief Enable RX PDM mode.
 * @note  ESP32-C3 doesn't support pdm in rx mode, disable anyway
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pdm_enable Set true to RX enable PDM mode (ignored)
 */
static inline void i2s_ll_rx_enable_pdm(i2s_dev_t *hw, bool pdm_enable)
{
    hw->rx_conf.rx_pdm_en = 0;
    hw->rx_conf.rx_tdm_en = 1;
}

/**
 * @brief Configura TX a/u-law decompress or compress
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pcm_cfg PCM configuration paramater
 */
static inline void i2s_ll_tx_set_pcm_type(i2s_dev_t *hw, i2s_pcm_compress_t pcm_cfg)
{
    hw->tx_conf.tx_pcm_conf = pcm_cfg;
    hw->tx_conf.tx_pcm_bypass = !pcm_cfg;
}

/**
 * @brief Configure RX a/u-law decompress or compress
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pcm_cfg PCM configuration paramater
 */
static inline void i2s_ll_rx_set_pcm_type(i2s_dev_t *hw, i2s_pcm_compress_t pcm_cfg)
{
    hw->rx_conf.rx_pcm_conf = pcm_cfg;
    hw->rx_conf.rx_pcm_bypass = !pcm_cfg;
}

/**
 * @brief Enable TX audio data left alignment
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable left alignment
 */
static inline void i2s_ll_tx_enable_left_align(i2s_dev_t *hw, bool ena)
{
    hw->tx_conf.tx_left_align = ena;
}

/**
 * @brief Enable RX audio data left alignment
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable left alignment
 */
static inline void i2s_ll_rx_enable_left_align(i2s_dev_t *hw, bool ena)
{
    hw->rx_conf.rx_left_align = ena;
}

/**
 * @brief Enable TX big endian mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable big endian mode
 */
static inline void i2s_ll_rx_enable_big_endian(i2s_dev_t *hw, bool ena)
{
    hw->rx_conf.rx_big_endian = ena;
}

/**
 * @brief Enable RX big endian mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable big endian mode
 */
static inline void i2s_ll_tx_enable_big_endian(i2s_dev_t *hw, bool ena)
{
    hw->tx_conf.tx_big_endian = ena;
}

/**
 * @brief Configure TX bit order
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param lsb_order_ena Set true to enable LSB bit order
 */
static inline void i2s_ll_tx_set_bit_order(i2s_dev_t *hw, bool lsb_order_ena)
{
    hw->tx_conf.tx_bit_order = lsb_order_ena;
}

/**
 * @brief Configure RX bit order
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param lsb_order_ena Set true to enable LSB bit order
 */
static inline void i2s_ll_rx_set_bit_order(i2s_dev_t *hw, bool lsb_order_ena)
{
    hw->rx_conf.rx_bit_order = lsb_order_ena;
}

/**
 * @brief Configure TX skip mask enable
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param skip_mask_ena Set true to skip inactive channels.
 */
static inline void i2s_ll_tx_set_skip_mask(i2s_dev_t *hw, bool skip_mask_ena)
{
    hw->tx_tdm_ctrl.tx_tdm_skip_msk_en = skip_mask_ena;
}


/**
 * @brief Configure single data
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param data Single data to be set
 */
static inline void i2s_ll_set_single_data(i2s_dev_t *hw, uint32_t data)
{
    hw->conf_single_data = data;
}

/**
 * @brief Enable TX mono mode
 * @note MONO in hardware means only one channel got data, but another doesn't
 *       MONO in software means two channel share same data
 *       This function aims to use MONO in software meaning
 *       so 'tx_mono' and 'tx_chan_equal' should be enabled at the same time
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param mono_ena Set true to enable mono mde.
 */
static inline void i2s_ll_tx_enable_mono_mode(i2s_dev_t *hw, bool mono_ena)
{
    hw->tx_conf.tx_mono = mono_ena;
    hw->tx_conf.tx_chan_equal = mono_ena;
}

/**
 * @brief Enable RX mono mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param mono_ena Set true to enable mono mde.
 */
static inline void i2s_ll_rx_enable_mono_mode(i2s_dev_t *hw, bool mono_ena)
{
    hw->rx_conf.rx_mono = mono_ena;
    hw->rx_conf.rx_mono_fst_vld = mono_ena;
}

/**
 * @brief Enable loopback mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to share BCK and WS signal for tx module and rx module.
 */
static inline void i2s_ll_share_bck_ws(i2s_dev_t *hw, bool ena)
{
    hw->tx_conf.sig_loopback = ena;
}

#ifdef __cplusplus
}
#endif
