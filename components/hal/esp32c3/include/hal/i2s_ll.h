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

// The LL layer for I2C register operations
/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once
#include <stdbool.h>
#include "soc/i2s_periph.h"
#include "hal/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define I2S_LL_GET_HW(num) (&I2S0)
#define I2S_INTR_OUT_DSCR_ERR (0x1 << 6)
#define I2S_INTR_OUT_EOF      (0x1 << 4)
#define I2S_INTR_IN_DSCR_ERR  (0x1 << 5)
#define I2S_INTR_IN_SUC_EOF   (0x1 << 1)
#define I2S_INTR_MAX          (~0)

#define I2S_TDM_CH_MASK (0xffff)

/* I2S clock configuration structure */
typedef struct {
    uint16_t mclk_div; // I2S module clock devider, Fmclk = Fsclk /(mclk_div+b/a)
    uint16_t a;
    uint16_t b;        // The decimal part of module clock devider, the decimal is: b/a
    uint16_t bck_div;  // The BCK devider, Fbck = Fmclk / bck_div
} i2s_clk_cal_t;

/**
 * @brief Calculate the closest sample rate clock configuration.
 *        clock relationship:
 *        Fmclk = bck_div*fbck = fsclk/(mclk_div+b/a)
 *
 * @param fsclk I2S source clock freq.
 * @param fbck BCK freuency.
 * @param bck_div The BCK devider of bck. Generally, set bck_div to 8.
 * @param cal Point to `i2s_clk_cal_t` structure.
 */
static inline void i2s_ll_clk_cal(uint32_t fsclk, uint32_t fbck, int bck_div, i2s_clk_cal_t *cal)
{
    int ma = 0;
    int mb = 0;
    uint32_t mclk = fbck*bck_div;
    cal->mclk_div = fsclk / mclk;
    cal->bck_div = bck_div;
    cal->a = 1;
    cal->b = 0;
    uint32_t freq_diff = fsclk - mclk * cal->mclk_div;
    uint32_t min = freq_diff * 512;
    if (freq_diff == 0) {
        return;
    }
    for (int a = 2; a <= 512; a++) {
        for (int b = 1; b < a; b++) {
            ma = freq_diff*a;
            mb = mclk*b;
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

/**
 * @brief I2S module general init, enable I2S clock.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_general_init(i2s_dev_t *hw)
{
    hw->tx_clkm_conf.clk_en = 1;
}

/**
 * @brief Reset I2S TX module, enable default source clock and set to TDM mode.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_gen_init(i2s_dev_t *hw)
{
    hw->tx_conf.tx_start = 0;
    hw->tx_conf.tx_reset = 1;
    hw->tx_conf.tx_reset = 0;
    hw->tx_conf.tx_slave_mod = 0;
    hw->tx_conf.tx_tdm_en = 1;
    hw->tx_conf.tx_pdm_en = 0;
    hw->tx_clkm_conf.tx_clk_active = 1;
    hw->tx_clkm_conf.tx_clk_sel = 2;
}

/**
 * @brief Reset I2S RX module, enable default source clock and set to TDM mode.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_gen_init(i2s_dev_t *hw)
{
    hw->rx_conf.rx_start = 0;
    hw->rx_conf.rx_reset = 1;
    hw->rx_conf.rx_reset = 0;
    hw->rx_conf.rx_slave_mod = 0;
    hw->rx_conf.rx_tdm_en = 1;
    hw->rx_conf.rx_pdm_en = 0;
    hw->rx_clkm_conf.rx_clk_active = 1;
    hw->rx_clkm_conf.rx_clk_sel = 2;
    hw->rx_clkm_conf.mclk_sel = 0;
}

/**
 * @brief Enable I2S TX slave mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param slave_en Set true to enable slave mode
 */
static inline void i2s_ll_set_tx_slave_mod(i2s_dev_t *hw, bool slave_en)
{
    hw->tx_conf.tx_slave_mod = slave_en;
}

/**
 * @brief Enable I2S RX slave mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param slave_en Set true to enable slave mode
 */
static inline void i2s_ll_set_rx_slave_mod(i2s_dev_t *hw, bool slave_en)
{
    hw->rx_conf.rx_slave_mod = slave_en;
}

/**
 * @brief Reset I2S TX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_tx(i2s_dev_t *hw)
{
    hw->tx_conf.tx_reset = 1;
    hw->tx_conf.tx_reset = 0;
}

/**
 * @brief Reset I2S RX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_rx(i2s_dev_t *hw)
{
    hw->rx_conf.rx_reset = 1;
    hw->rx_conf.rx_reset = 0;
}

/**
 * @brief Reset I2S TX FIFO
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_tx_fifo(i2s_dev_t *hw)
{
    hw->tx_conf.tx_fifo_reset = 1;
    hw->tx_conf.tx_fifo_reset = 0;
}

/**
 * @brief Reset I2S RX FIFO
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_rx_fifo(i2s_dev_t *hw)
{
    hw->rx_conf.rx_fifo_reset = 1;
    hw->rx_conf.rx_fifo_reset = 0;
}

/**
 * @brief Set TX source clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param src I2S source clock,  ESP32-S3 only support `I2S_CLK_D2CLK`
 */
static inline void i2s_ll_set_tx_clk_src(i2s_dev_t *hw, i2s_clock_src_t src)
{
    hw->tx_clkm_conf.tx_clk_sel = 2;
}

/**
 * @brief Set RX source clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param src I2S source clock,  ESP32-S3 only support `I2S_CLK_D2CLK`
 */
static inline void i2s_ll_set_rx_clk_src(i2s_dev_t *hw, i2s_clock_src_t src)
{
    hw->rx_clkm_conf.rx_clk_sel = 2;
}

/**
 * @brief Configure I2S TX clock devider
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param set Pointer to I2S clock devider configuration paramater
 */
static inline void i2s_ll_set_tx_clk(i2s_dev_t *hw, i2s_clk_cal_t *set)
{
    if (set->a == 0 || set->b == 0) {
        hw->tx_clkm_div_conf.tx_clkm_div_x = 0;
        hw->tx_clkm_div_conf.tx_clkm_div_y = 0;
        hw->tx_clkm_div_conf.tx_clkm_div_z = 0;
    } else {
        if(set->b > set->a/2) {
            hw->tx_clkm_div_conf.tx_clkm_div_x = set->a/(set->a-set->b) -1;
            hw->tx_clkm_div_conf.tx_clkm_div_y = set->a%(set->a-set->b);
            hw->tx_clkm_div_conf.tx_clkm_div_z = set->a-set->b;
            hw->tx_clkm_div_conf.tx_clkm_div_yn1 = 1;
        } else {
            hw->tx_clkm_div_conf.tx_clkm_div_x = set->a/set->b-1;
            hw->tx_clkm_div_conf.tx_clkm_div_y = set->a%set->b+1;
            hw->tx_clkm_div_conf.tx_clkm_div_z = set->b;
            hw->tx_clkm_div_conf.tx_clkm_div_yn1 = 0;
        }
    }
    hw->tx_clkm_conf.tx_clkm_div_num = set->mclk_div;
    hw->tx_conf1.tx_bck_div_num = set->bck_div-1;
}

/**
 * @brief Configure I2S RX clock devider
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param set Pointer to I2S clock devider configuration paramater
 */
static inline void i2s_ll_set_rx_clk(i2s_dev_t *hw, i2s_clk_cal_t *set)
{
    if (set->a == 0 || set->b == 0) {
        hw->rx_clkm_div_conf.rx_clkm_div_x = 0;
        hw->rx_clkm_div_conf.rx_clkm_div_y = 0;
        hw->rx_clkm_div_conf.rx_clkm_div_z = 0;
    } else {
        if(set->b > set->a/2) {
            hw->rx_clkm_div_conf.rx_clkm_div_x = set->a/(set->a-set->b)-1;
            hw->rx_clkm_div_conf.rx_clkm_div_y = set->a%(set->a-set->b);
            hw->rx_clkm_div_conf.rx_clkm_div_z = set->a-set->b;
            hw->rx_clkm_div_conf.rx_clkm_div_yn1 = 1;
        } else {
            hw->rx_clkm_div_conf.rx_clkm_div_x = set->a/set->b-1;
            hw->rx_clkm_div_conf.rx_clkm_div_y = set->a%set->b+1;
            hw->rx_clkm_div_conf.rx_clkm_div_z = set->b;
            hw->rx_clkm_div_conf.rx_clkm_div_yn1 = 0;
        }
    }
    hw->rx_clkm_conf.rx_clkm_div_num = set->mclk_div;
    hw->rx_conf1.rx_bck_div_num = set->bck_div-1;
}

/**
 * @brief Start I2S TX
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_tx(i2s_dev_t *hw)
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
static inline void i2s_ll_start_rx(i2s_dev_t *hw)
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
static inline void i2s_ll_stop_tx(i2s_dev_t *hw)
{
    hw->tx_conf.tx_start = 0;
}

/**
 * @brief Stop I2S RX
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_rx(i2s_dev_t *hw)
{
    hw->rx_conf.rx_start = 0;
}

/**
 * @brief Configure TX WS signal width
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param width WS width in BCK cycle
 */
static inline void i2s_ll_set_tx_tdm_ws_width(i2s_dev_t *hw, int width)
{
    hw->tx_conf1.tx_tdm_ws_width = width-1;
}

/**
 * @brief Configure RX WS signal width
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param width WS width in BCK cycle
 */
static inline void i2s_ll_set_rx_tdm_ws_width(i2s_dev_t *hw, int width)
{
    hw->rx_conf1.rx_tdm_ws_width = width-1;
}

/**
 * @brief Configure the received length to trigger in_suc_eof interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param eof_num the byte length to trigger in_suc_eof interrupt
 */
static inline void i2s_ll_set_rx_eof_num(i2s_dev_t *hw, int eof_num)
{
    hw->rx_eof_num.rx_eof_num = eof_num;
}

/**
 * @brief Congfigure TX slot bit and audio data bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sample_bit The slot bit width
 * @param data_bit The audio data bit width
 */
static inline void i2s_ll_set_tx_sample_bit(i2s_dev_t *hw, uint8_t sample_bit, int data_bit)
{
    hw->tx_conf1.tx_bits_mod = data_bit - 1;
    hw->tx_conf1.tx_tdm_chan_bits = sample_bit-1;
}

/**
 * @brief Congfigure RX slot bit and audio data bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sample_bit The slot bit width
 * @param data_bit The audio data bit width
 */
static inline void i2s_ll_set_rx_sample_bit(i2s_dev_t *hw, uint8_t sample_bit, int data_bit)
{
    hw->rx_conf1.rx_bits_mod = data_bit - 1;
    hw->rx_conf1.rx_tdm_chan_bits = sample_bit-1;
}

/**
 * @brief Configure RX half_sample_bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param half_sample_bits half sample bit width
 */
static inline void i2s_ll_set_tx_half_sample_bit(i2s_dev_t *hw, int half_sample_bits)
{
    hw->tx_conf1.tx_half_sample_bits = half_sample_bits-1;
}

/**
 * @brief Configure RX half_sample_bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param half_sample_bits half sample bit width
 */
static inline void i2s_ll_set_rx_half_sample_bit(i2s_dev_t *hw, int half_sample_bits)
{
    hw->rx_conf1.rx_half_sample_bits = half_sample_bits-1;
}

/**
 * @brief Enable TX MSB shift, the data will be launch at the first BCK clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param msb_shift_enable Set true to enable MSB shift
 */
static inline void i2s_ll_tx_msb_shift_enable(i2s_dev_t *hw, bool msb_shift_enable)
{
    hw->tx_conf1.tx_msb_shift = msb_shift_enable;
}

/**
 * @brief Enable RX MSB shift, the data will be launch at the first BCK clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param msb_shift_enable Set true to enable MSB shift
 */
static inline void i2s_ll_rx_msb_shift_enable(i2s_dev_t *hw, bool msb_shift_enable)
{
    hw->rx_conf1.rx_msb_shift = msb_shift_enable;
}

/**
 * @brief Configure TX total slot number
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param total_num Total slot number
 */
static inline void i2s_ll_set_tx_slot_mun(i2s_dev_t *hw, int total_num)
{
    hw->tx_tdm_ctrl.tx_tdm_tot_chan_num = total_num-1;
}

/**
 * @brief Configure RX total slot number
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param total_num Total slot number
 */
static inline void i2s_ll_set_rx_slot_mun(i2s_dev_t *hw, int total_num)
{
    hw->rx_tdm_ctrl.rx_tdm_tot_chan_num = total_num-1;
}

/**
 * @brief Set the bimap of the active TX slot, only the active slot can launch audio data.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param
 */
static inline void i2s_ll_set_tx_active_slot_mask(i2s_dev_t *hw, uint32_t slot_mask)
{
    typeof(hw->tx_tdm_ctrl) tdm_ctrl_reg = hw->tx_tdm_ctrl;
    tdm_ctrl_reg.val &= ~I2S_TDM_CH_MASK;
    tdm_ctrl_reg.val |= slot_mask&I2S_TDM_CH_MASK;
    hw->tx_tdm_ctrl.val = tdm_ctrl_reg.val;
}

/**
 * @brief Set the bimap of the active RX slot, only the active slot can receive audio data.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param
 */
static inline void i2s_ll_set_rx_active_slot_mask(i2s_dev_t *hw, uint32_t slot_mask)
{
    typeof(hw->rx_tdm_ctrl) tdm_ctrl_reg = hw->rx_tdm_ctrl;
    tdm_ctrl_reg.val &= ~I2S_TDM_CH_MASK;
    tdm_ctrl_reg.val |= slot_mask&I2S_TDM_CH_MASK;
    hw->rx_tdm_ctrl.val = tdm_ctrl_reg.val;
}

/**
 * @brief Set TX WS signal pol level
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param
 */
static inline void i2s_set_tx_ws_idle_pol(i2s_dev_t *hw, int ws_pol_level)
{
    hw->tx_conf.tx_ws_idle_pol = ws_pol_level;
}

/**
 * @brief Set RX WS signal pol level
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param
 */
static inline void i2s_set_rx_ws_idle_pol(i2s_dev_t *hw, int ws_pol_level)
{
    hw->rx_conf.rx_ws_idle_pol = ws_pol_level;
}

/**
 * @brief Enable TX PDM mode.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pdm_enable Set true to TX enable PDM mode
 */
static inline void i2s_ll_set_tx_pdm_en(i2s_dev_t *hw, bool pdm_enable)
{
    if (pdm_enable) {
        hw->tx_conf.tx_tdm_en = 0;
        hw->tx_conf.tx_pdm_en = 1;
    } else {
        hw->tx_conf.tx_pdm_en = 0;
        hw->tx_conf.tx_tdm_en = 1;
    }
}

/**
 * @brief Configure I2S TX pdm
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sample_rate The sample rate to be set.
 */
static inline void i2s_ll_tx_pdm_cfg(i2s_dev_t *hw, uint32_t sample_rate)
{
    uint32_t fp = 960;
    uint32_t fs = sample_rate / 100;
    typeof(hw->tx_pcm2pdm_conf) pdm_conf_reg = hw->tx_pcm2pdm_conf;
    typeof(hw->tx_pcm2pdm_conf1) pdm_conf1_reg = hw->tx_pcm2pdm_conf1;
    pdm_conf_reg.pcm2pdm_conv_en = 1;
    pdm_conf_reg.tx_pdm_prescale = 0;
    pdm_conf_reg.tx_pdm_hp_in_shift = 1;
    pdm_conf_reg.tx_pdm_lp_in_shift = 1;
    pdm_conf_reg.tx_pdm_sinc_in_shift = 1;
    pdm_conf_reg.tx_pdm_sigmadelta_in_shift = 1;
    pdm_conf_reg.tx_pdm_sinc_osr2 =  fp/fs;
    pdm_conf_reg.tx_pdm_dac_mode_en =  1;
    pdm_conf_reg.tx_pdm_sigmadelta_dither = 0;
    pdm_conf_reg.tx_pdm_sigmadelta_dither2 = 0;
    pdm_conf_reg.tx_pdm_dac_2out_en = 1;
    pdm_conf1_reg.tx_pdm_fp = fp;
    pdm_conf1_reg.tx_pdm_fs = fs;
    pdm_conf1_reg.tx_iir_hp_mult12_5 = 7;
    pdm_conf1_reg.tx_iir_hp_mult12_0 =6;
    pdm_conf_reg.tx_pdm_hp_bypass = 0;
    hw->tx_pcm2pdm_conf = pdm_conf_reg;
    hw->tx_pcm2pdm_conf1 = pdm_conf1_reg;
}

/**
 * @brief Configure I2S TX PDM sample rate
 *        Fpdm = 64*Fpcm*fp/fs
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param fp The fp value of TX PDM filter module group0.
 * @param fs The fs value of TX PDM filter module group0.
 */
static inline void i2s_ll_set_tx_pdm_fpfs(i2s_dev_t *hw, uint32_t fp, uint32_t fs)
{
    hw->tx_pcm2pdm_conf1.tx_pdm_fp = fp;
    hw->tx_pcm2pdm_conf1.tx_pdm_fs = fs;
    hw->tx_pcm2pdm_conf.tx_pdm_sinc_osr2 = fp / fs;
}

/**
 * @brief Get I2S TX PDM configuration
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param fp Pointer to accept TX PDM fp configuration paramater
 * @param fs Pointer to accept TX PDM fs configuration paramater
 */
static inline void i2s_ll_get_tx_pdm_fpfs(i2s_dev_t *hw, uint32_t *fp, uint32_t *fs)
{
    *fp = hw->tx_pcm2pdm_conf1.tx_pdm_fp;
    *fs = hw->tx_pcm2pdm_conf1.tx_pdm_fs;
}

/**
 * @brief Enable RX PDM mode.
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pdm_enable Set true to RX enable PDM mode
 */
static inline void i2s_ll_set_rx_pdm_en(i2s_dev_t *hw, bool pdm_enable)
{
    if (pdm_enable) {
        hw->rx_conf.rx_tdm_en = 0;
        hw->rx_conf.rx_pdm_en = 1;
    } else {
        hw->rx_conf.rx_pdm_en = 0;
        hw->rx_conf.rx_tdm_en = 1;
    }
}

/**
 * @brief Configura TX a/u-law decompress or compress
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pcm_cfg PCM configuration paramater
 */
static inline void i2s_ll_tx_pcm_cfg(i2s_dev_t *hw, i2s_pcm_cfg_t pcm_cfg)
{
    if (pcm_cfg == I2S_PCM_DISABLE) {
        hw->tx_conf.tx_pcm_bypass = 1;
    } else {
        hw->tx_conf.tx_pcm_conf = pcm_cfg;
        hw->tx_conf.tx_pcm_bypass = 0;
    }
}

/**
 * @brief Configure RX a/u-law decompress or compress
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param pcm_cfg PCM configuration paramater
 */
static inline void i2s_ll_rx_pcm_cfg(i2s_dev_t *hw, i2s_pcm_cfg_t pcm_cfg)
{
    if (pcm_cfg == I2S_PCM_DISABLE) {
        hw->rx_conf.rx_pcm_bypass = 1;
    } else {
        hw->rx_conf.rx_pcm_conf = pcm_cfg;
        hw->rx_conf.rx_pcm_bypass = 0;
    }
}

/**
 * @brief Enable TX audio data left alignment
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable left alignment
 */
static inline void i2s_ll_tx_left_align_enable(i2s_dev_t *hw, bool ena)
{
    hw->tx_conf.tx_left_align = ena;
}

/**
 * @brief Enable RX audio data left alignment
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable left alignment
 */
static inline void i2s_ll_rx_left_align_enable(i2s_dev_t *hw, bool ena)
{
    hw->rx_conf.rx_left_align = ena;
}

/**
 * @brief Enable TX big endian mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable big endian mode
 */
static inline void i2s_ll_rx_big_endian_enable(i2s_dev_t *hw, bool ena)
{
    hw->rx_conf.rx_big_endian = ena;
}

/**
 * @brief Enable RX big endian mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable big endian mode
 */
static inline void i2s_ll_tx_big_endian_enable(i2s_dev_t *hw, bool ena)
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
 * @brief Configure single data
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param data Single data to be set
 */
static inline void i2s_ll_single_data_config(i2s_dev_t *hw, uint32_t data)
{
    hw->conf_single_data = data;
}

/**
 * @brief Enable loopback mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable loopback mode.
 */
static inline void i2s_ll_loop_back_ena(i2s_dev_t *hw, bool ena)
{
    hw->tx_conf.sig_loopback = ena;
}

#ifdef __cplusplus
}
#endif
