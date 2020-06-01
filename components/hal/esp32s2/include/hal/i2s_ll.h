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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-S2 I2S register operations

#pragma once

#include <stdbool.h>
#include "soc/i2s_periph.h"
#include "hal/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get I2S hardware instance with giving i2s num
#define I2S_LL_GET_HW(num) (((num) == 0) ? (&I2S0) : NULL)

#define I2S_INTR_IN_SUC_EOF   BIT(9)
#define I2S_INTR_OUT_EOF      BIT(12)
#define I2S_INTR_IN_DSCR_ERR  BIT(13)
#define I2S_INTR_OUT_DSCR_ERR BIT(14)
#define I2S_INTR_MAX          (0xFFFFFFFF)

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
    uint32_t min = ~0;
    if (freq_diff == 0) {
        return;
    }
    for (int a = 2; a <= 63; a++) {
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
    if (hw->clkm_conf.clk_en == 0) {
        hw->clkm_conf.clk_sel = 2;
        hw->clkm_conf.clk_en = 1;
        hw->conf2.val = 0;
    }
}

/**
 * @brief I2S TX module general init.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_tx_gen_init(i2s_dev_t *hw)
{
    hw->conf.tx_start = 0;
    hw->conf.tx_reset = 1;
    hw->conf.tx_reset = 0;
    hw->conf.tx_msb_right = 0;
    hw->conf.tx_right_first = 0;
    hw->conf.tx_slave_mod = 0;
    hw->fifo_conf.tx_fifo_mod_force_en = 1;
}

/**
 * @brief I2S RX module general init.
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_rx_gen_init(i2s_dev_t *hw)
{
    hw->conf.rx_start = 0;
    hw->conf.rx_reset = 1;
    hw->conf.rx_reset = 0;
    hw->conf.rx_msb_right = 0;
    hw->conf.rx_right_first = 0;
    hw->conf.rx_slave_mod = 0;
    hw->fifo_conf.rx_fifo_mod_force_en = 1;
}

/**
 * @brief Enable I2S TX slave mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param slave_en Set true to enable slave mode
 */
static inline void i2s_ll_set_tx_slave_mod(i2s_dev_t *hw, bool slave_en)
{
    hw->conf.tx_slave_mod = slave_en;
}

/**
 * @brief Enable I2S RX slave mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param slave_en Set true to enable slave mode
 */
static inline void i2s_ll_set_rx_slave_mod(i2s_dev_t *hw, bool slave_en)
{
    hw->conf.rx_slave_mod = slave_en;
}

/**
 * @brief Reset TX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_tx(i2s_dev_t *hw)
{
    hw->conf.tx_reset = 1;
    hw->conf.tx_reset = 0;
}

/**
 * @brief Reset RX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_rx(i2s_dev_t *hw)
{
    hw->conf.rx_reset = 1;
    hw->conf.rx_reset = 0;
}

/**
 * @brief Reset TX FIFO
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_tx_fifo(i2s_dev_t *hw)
{
    hw->conf.tx_fifo_reset = 1;
    hw->conf.tx_fifo_reset = 0;
}

/**
 * @brief Reset RX FIFO
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_rx_fifo(i2s_dev_t *hw)
{
    hw->conf.rx_fifo_reset = 1;
    hw->conf.rx_fifo_reset = 0;
}

/**
 * @brief Set TX source clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param src I2S source clock
 */
static inline void i2s_ll_set_tx_clk_src(i2s_dev_t *hw, i2s_clock_src_t src)
{
    hw->clkm_conf.clk_sel = (src == 1) ? 1 : 2;
}

/**
 * @brief Set RX source clock
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param src I2S source clock
 */
static inline void i2s_ll_set_rx_clk_src(i2s_dev_t *hw, i2s_clock_src_t src)
{
    hw->clkm_conf.clk_sel = (src == 1) ? 1 : 2;
}

/**
 * @brief Configure I2S TX clock devider
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param set Pointer to I2S clock devider configuration paramater
 */
static inline void i2s_ll_set_tx_clk(i2s_dev_t *hw, i2s_clk_cal_t *set)
{
    hw->clkm_conf.clkm_div_num = set->mclk_div;
    hw->clkm_conf.clkm_div_b = set->b;
    hw->clkm_conf.clkm_div_a = set->a;
    hw->sample_rate_conf.tx_bck_div_num = set->bck_div;
}

/**
 * @brief Configure I2S RX clock devider
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param set Pointer to I2S clock devider configuration paramater
 */
static inline void i2s_ll_set_rx_clk(i2s_dev_t *hw, i2s_clk_cal_t *set)
{
    hw->clkm_conf.clkm_div_num = set->mclk_div;
    hw->clkm_conf.clkm_div_b = set->b;
    hw->clkm_conf.clkm_div_a = set->a;
    hw->sample_rate_conf.rx_bck_div_num = set->bck_div;
}

/**
 * @brief Enable TX interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_enable_tx_intr(i2s_dev_t *hw)
{
    hw->int_ena.out_eof = 1;
    hw->int_ena.out_dscr_err = 1;
}

/**
 * @brief Disable TX interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_disable_tx_intr(i2s_dev_t *hw)
{
    hw->int_ena.out_eof = 0;
    hw->int_ena.out_dscr_err = 0;
}

/**
 * @brief Enable RX interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_enable_rx_intr(i2s_dev_t *hw)
{
    hw->int_ena.in_suc_eof = 1;
    hw->int_ena.in_dscr_err = 1;
}

/**
 * @brief Disable RX interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_disable_rx_intr(i2s_dev_t *hw)
{
    hw->int_ena.in_suc_eof = 0;
    hw->int_ena.in_dscr_err = 0;
}

/**
 * @brief Get I2S interrupt status
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param intr_mask Pointer to accept interrupt status
 */
static inline void i2s_ll_get_intr_status(i2s_dev_t *hw, uint32_t *intr_mask)
{
    *intr_mask = hw->int_st.val;
}

/**
 * @brief Clear I2S interrupt status
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param clr_mask Interrupt mask to be cleared.
 */
static inline void i2s_ll_clear_intr_status(i2s_dev_t *hw, uint32_t clr_mask)
{
    hw->int_clr.val = clr_mask;
}

/**
 * @brief Reset dma out
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_dma_out(i2s_dev_t *hw)
{
    hw->lc_conf.out_rst = 1;
    hw->lc_conf.out_rst = 0;
}

/**
 * @brief Reset dma in
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_dma_in(i2s_dev_t *hw)
{
    hw->lc_conf.in_rst = 1;
    hw->lc_conf.in_rst = 0;
}

/**
 * @brief Start TX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_tx(i2s_dev_t *hw)
{
    hw->conf.tx_start = 1;
}

/**
 * @brief Start RX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_rx(i2s_dev_t *hw)
{
    hw->conf.rx_start = 1;
}

/**
 * @brief Configure TX DMA descriptor address and start TX DMA
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param link_addr DMA descriptor link address.
 */
static inline void i2s_ll_start_tx_link(i2s_dev_t *hw, uint32_t link_addr)
{
    hw->out_link.addr = link_addr;
    hw->out_link.start = 1;
}

/**
 * @brief Configure RX DMA descriptor address and start TX DMA
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param link_addr DMA descriptor link address.
 */
static inline void i2s_ll_start_rx_link(i2s_dev_t *hw, uint32_t link_addr)
{
    hw->in_link.addr = link_addr;
    hw->in_link.start = 1;
}

/**
 * @brief Stop TX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_tx(i2s_dev_t *hw)
{
    hw->conf.tx_start = 0;
}

/**
 * @brief Stop RX module
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_rx(i2s_dev_t *hw)
{
    hw->conf.rx_start = 0;
}

/**
 * @brief Stop out link
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_out_link(i2s_dev_t *hw)
{
    hw->out_link.stop = 1;
}

/**
 * @brief Stop in link
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_in_link(i2s_dev_t *hw)
{
    hw->in_link.stop = 1;
}

/**
 * @brief Get I2S out eof descriptor address
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param eof_addr Pointer to accept out eof des address
 */
static inline void i2s_ll_get_out_eof_des_addr(i2s_dev_t *hw, uint32_t *eof_addr)
{
    *eof_addr = hw->out_eof_des_addr;
}

/**
 * @brief Get I2S in eof descriptor address
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param eof_addr Pointer to accept in eof des address
 */
static inline void i2s_ll_get_in_eof_des_addr(i2s_dev_t *hw, uint32_t *eof_addr)
{
    *eof_addr = hw->in_eof_des_addr;
}

/**
 * @brief Configure the received length to trigger in_suc_eof interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param eof_num the byte length to trigger in_suc_eof interrupt
 */
static inline void i2s_ll_set_rx_eof_num(i2s_dev_t *hw, uint32_t eof_num)
{
    hw->rx_eof_num = eof_num;
}

/**
 * @brief Congfigure TX slot bit and audio data bit, on ESP32-S2, sample_bit should equals to data_bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sample_bit The slot bit width
 * @param data_bit The audio data bit width
 */
static inline void i2s_ll_set_tx_sample_bit(i2s_dev_t *hw, uint8_t sample_bit, int data_bit)
{
    hw->fifo_conf.tx_fifo_mod = (sample_bit <= I2S_BITS_PER_SAMPLE_16BIT ? 0 : 2);
    hw->sample_rate_conf.tx_bits_mod = data_bit;
}

/**
 * @brief Congfigure RX slot bit and audio data bit, on ESP32-S2, sample_bit should equals to data_bit
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param sample_bit The slot bit width
 * @param data_bit The audio data bit width
 */
static inline void i2s_ll_set_rx_sample_bit(i2s_dev_t *hw, uint8_t sample_bit, int data_bit)
{
    hw->fifo_conf.rx_fifo_mod = (sample_bit <= I2S_BITS_PER_SAMPLE_16BIT ? 0 : 2);
    hw->sample_rate_conf.rx_bits_mod = data_bit;
}

/**
 * @brief Enable I2S DMA
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param ena Set true to enable DMA
 */
static inline void i2s_ll_dma_enable(i2s_dev_t *hw, bool ena)
{
    if (ena && !hw->fifo_conf.dscr_en) {
        hw->fifo_conf.dscr_en = 1;
    } else if (!ena && hw->fifo_conf.dscr_en) {
        hw->fifo_conf.dscr_en = 0;
    }
}

/**
 * @brief Set I2S TX to philip standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_tx_format_philip(i2s_dev_t *hw)
{
    hw->conf.tx_short_sync = 0;
    hw->conf.tx_msb_shift = 1;
}

/**
 * @brief Set I2S RX to philip standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_rx_format_philip(i2s_dev_t *hw)
{
    hw->conf.rx_short_sync = 0;
    hw->conf.rx_msb_shift = 1;
}

/**
 * @brief Set I2S TX to MSB Alignment Standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_tx_format_msb_align(i2s_dev_t *hw)
{
    hw->conf.tx_short_sync = 0;
    hw->conf.tx_msb_shift = 0;
}

/**
 * @brief Set I2S RX to MSB Alignment Standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_rx_format_msb_align(i2s_dev_t *hw)
{
    hw->conf.rx_short_sync = 0;
    hw->conf.rx_msb_shift = 0;
}

/**
 * @brief Set I2S TX to PCM short standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_tx_pcm_short(i2s_dev_t *hw)
{
    hw->conf.tx_short_sync = 1;
    hw->conf.tx_msb_shift = 0;
}

/**
 * @brief Set I2S RX to PCM short standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_rx_pcm_short(i2s_dev_t *hw)
{
    hw->conf.rx_short_sync = 1;
    hw->conf.rx_msb_shift = 0;
}

/**
 * @brief Set I2S TX to PCM long standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_tx_pcm_long(i2s_dev_t *hw)
{
    hw->conf.tx_short_sync = 0;
    hw->conf.tx_msb_shift = 0;
}

/**
 * @brief Set I2S RX to PCM long standard
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_set_rx_pcm_long(i2s_dev_t *hw)
{
    hw->conf.rx_short_sync = 0;
    hw->conf.rx_msb_shift = 0;
}

/**
 * @brief Enable TX mono mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param mono_ena Set true to enable mono mde.
 */
static inline void i2s_ll_tx_mono_mode_ena(i2s_dev_t *hw, bool mono_ena)
{
    int data_bit = hw->sample_rate_conf.tx_bits_mod;
    if (data_bit <= I2S_BITS_PER_SAMPLE_16BIT) {
        hw->fifo_conf.tx_fifo_mod = 0 + mono_ena;
    } else {
        hw->fifo_conf.tx_fifo_mod = 2 + mono_ena;
    }
    hw->conf.tx_dma_equal = mono_ena;
    hw->conf_chan.tx_chan_mod = mono_ena;
}

/**
 * @brief Enable RX mono mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param mono_ena Set true to enable mono mde.
 */
static inline void i2s_ll_rx_mono_mode_ena(i2s_dev_t *hw, bool mono_ena)
{
    int data_bit = hw->sample_rate_conf.rx_bits_mod;
    if (data_bit <= I2S_BITS_PER_SAMPLE_16BIT) {
        hw->fifo_conf.rx_fifo_mod = 0 + mono_ena;
    } else {
        hw->fifo_conf.rx_fifo_mod = 2 + mono_ena;
    }
    hw->conf.rx_dma_equal = mono_ena;
    hw->conf_chan.rx_chan_mod = mono_ena;
}

/**
 * @brief Enable I2S loopback mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param loopback_en Set true to enable loopback mode.
 */
static inline void i2s_ll_loop_back_ena(i2s_dev_t *hw, bool loopback_en)
{
    hw->conf.sig_loopback = loopback_en;
}

#ifdef __cplusplus
}
#endif
