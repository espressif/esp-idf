// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-S2 I2S register operations

#pragma once

#include <stdbool.h>
#include "soc/i2s_periph.h"
#include "soc/i2s_struct.h"
#include "hal/i2s_types.h"
#include "hal/hal_defs.h"

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

/**
 * @brief Reset rx fifo
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_rx_fifo(i2s_dev_t *hw)
{
    hw->conf.rx_fifo_reset = 1;
    hw->conf.rx_fifo_reset = 0;
}

/**
 * @brief Reset tx fifo
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_tx_fifo(i2s_dev_t *hw)
{
    hw->conf.tx_fifo_reset = 1;
    hw->conf.tx_fifo_reset = 0;
}

/**
 * @brief Enable rx interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_enable_rx_intr(i2s_dev_t *hw)
{
    hw->int_ena.in_suc_eof = 1;
    hw->int_ena.in_dscr_err = 1;
}

/**
 * @brief Disable rx interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_disable_rx_intr(i2s_dev_t *hw)
{
    hw->int_ena.in_suc_eof = 0;
    hw->int_ena.in_dscr_err = 0;
}

/**
 * @brief Disable tx interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_disable_tx_intr(i2s_dev_t *hw)
{
    hw->int_ena.out_eof = 0;
    hw->int_ena.out_dscr_err = 0;
}

/**
 * @brief Enable tx interrupt
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_enable_tx_intr(i2s_dev_t *hw)
{
    hw->int_ena.out_eof = 1;
    hw->int_ena.out_dscr_err = 1;
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
 * @brief Reset tx
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_tx(i2s_dev_t *hw)
{
    hw->conf.tx_reset = 1;
    hw->conf.tx_reset = 0;
}

/**
 * @brief Reset rx
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_reset_rx(i2s_dev_t *hw)
{
    hw->conf.rx_reset = 1;
    hw->conf.rx_reset = 0;
}

/**
 * @brief Start out link
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_out_link(i2s_dev_t *hw)
{
    hw->out_link.start = 1;
}

/**
 * @brief Start tx
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_tx(i2s_dev_t *hw)
{
    hw->conf.tx_start = 1;
}

/**
 * @brief Start in link
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_in_link(i2s_dev_t *hw)
{
    hw->in_link.start = 1;
}

/**
 * @brief Start rx
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_start_rx(i2s_dev_t *hw)
{
    hw->conf.rx_start = 1;
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
 * @brief Stop tx
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_tx(i2s_dev_t *hw)
{
    hw->conf.tx_start = 0;
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
 * @brief Stop rx
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_stop_rx(i2s_dev_t *hw)
{
    hw->conf.rx_start = 0;
}

/**
 * @brief Enable dma
 *
 * @param hw Peripheral I2S hardware instance address.
 */
static inline void i2s_ll_enable_dma(i2s_dev_t *hw)
{
    //Enable and configure DMA
    typeof(hw->lc_conf) lc_conf;
    lc_conf.val = 0;
    lc_conf.out_eof_mode = 1;
    hw->lc_conf.val = lc_conf.val;
}

/**
 * @brief Get I2S interrupt status
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get interrupt status
 */
static inline void i2s_ll_get_intr_status(i2s_dev_t *hw, uint32_t *val)
{
    *val = hw->int_st.val;
}

/**
 * @brief Clear I2S interrupt status
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to clear interrupt status
 */
static inline void i2s_ll_clear_intr_status(i2s_dev_t *hw, uint32_t val)
{
    hw->int_clr.val = val;
}

/**
 * @brief Get I2S out eof des address
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get out eof des address
 */
static inline void i2s_ll_get_out_eof_des_addr(i2s_dev_t *hw, uint32_t *val)
{
    *val = hw->out_eof_des_addr;
}

/**
 * @brief Get I2S in eof des address
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get in eof des address
 */
static inline void i2s_ll_get_in_eof_des_addr(i2s_dev_t *hw, uint32_t *val)
{
    *val = hw->in_eof_des_addr;
}

/**
 * @brief Get I2S tx fifo mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get tx fifo mode
 */
static inline void i2s_ll_get_tx_fifo_mod(i2s_dev_t *hw, uint32_t *val)
{
    *val = hw->fifo_conf.tx_fifo_mod;
}

/**
 * @brief Set I2S tx fifo mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx fifo mode
 */
static inline void i2s_ll_set_tx_fifo_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->fifo_conf.tx_fifo_mod = val;
}

/**
 * @brief Get I2S rx fifo mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get rx fifo mode
 */
static inline void i2s_ll_get_rx_fifo_mod(i2s_dev_t *hw, uint32_t *val)
{
    *val = hw->fifo_conf.rx_fifo_mod;
}

/**
 * @brief Set I2S rx fifo mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx fifo mode
 */
static inline void i2s_ll_set_rx_fifo_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->fifo_conf.rx_fifo_mod = val;
}

/**
 * @brief Set I2S tx chan mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx chan mode
 */
static inline void i2s_ll_set_tx_chan_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->conf_chan.tx_chan_mod = val;
}

/**
 * @brief Set I2S rx chan mode
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx chan mode
 */
static inline void i2s_ll_set_rx_chan_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->conf_chan.rx_chan_mod = val;
}

/**
 * @brief Set I2S tx dma equal
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx dma equal
 */
static inline void i2s_ll_set_tx_dma_equal(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.tx_dma_equal = val;
}

/**
 * @brief Set I2S rx dma equal
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx dma equal
 */
static inline void i2s_ll_set_rx_dma_equal(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.rx_dma_equal = val;
}

/**
 * @brief Set I2S out link address
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set out link address
 */
static inline void i2s_ll_set_out_link_addr(i2s_dev_t *hw, uint32_t val)
{
    hw->out_link.addr = val;
}

/**
 * @brief Set I2S in link address
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set in link address
 */
static inline void i2s_ll_set_in_link_addr(i2s_dev_t *hw, uint32_t val)
{
    hw->in_link.addr = val;
}

/**
 * @brief Set I2S rx eof num
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx eof num
 */
static inline void i2s_ll_set_rx_eof_num(i2s_dev_t *hw, uint32_t val)
{
    hw->rx_eof_num = val;
}

/**
 * @brief Set I2S clkm div num
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set clkm div num
 */
static inline void i2s_ll_set_clkm_div_num(i2s_dev_t *hw, uint32_t val)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clkm_conf, clkm_div_num, val);
}

/**
 * @brief Set I2S clkm div b
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set clkm div b
 */
static inline void i2s_ll_set_clkm_div_b(i2s_dev_t *hw, uint32_t val)
{
    hw->clkm_conf.clkm_div_b = val;
}

/**
 * @brief Set I2S clkm div a
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set clkm div a
 */
static inline void i2s_ll_set_clkm_div_a(i2s_dev_t *hw, uint32_t val)
{
    hw->clkm_conf.clkm_div_a = val;
}

/**
 * @brief Set I2S tx bck div num
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx bck div num
 */
static inline void i2s_ll_set_tx_bck_div_num(i2s_dev_t *hw, uint32_t val)
{
    hw->sample_rate_conf.tx_bck_div_num = val;
}

/**
 * @brief Set I2S rx bck div num
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx bck div num
 */
static inline void i2s_ll_set_rx_bck_div_num(i2s_dev_t *hw, uint32_t val)
{
    hw->sample_rate_conf.rx_bck_div_num = val;
}

/**
 * @brief Set I2S clk sel
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set clk sel
 */
static inline void i2s_ll_set_clk_sel(i2s_dev_t *hw, uint32_t val)
{
    hw->clkm_conf.clk_sel = (val == 1) ? 1 : 2;
}

/**
 * @brief Set I2S tx bits mod
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx bits mod
 */
static inline void i2s_ll_set_tx_bits_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->sample_rate_conf.tx_bits_mod = val;
}

/**
 * @brief Set I2S rx bits mod
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx bits mod
 */
static inline void i2s_ll_set_rx_bits_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->sample_rate_conf.rx_bits_mod = val;
}

/**
 * @brief Set I2S dscr en
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set dscr en
 */
static inline void i2s_ll_set_dscr_en(i2s_dev_t *hw, bool val)
{
    hw->fifo_conf.dscr_en = val;
}

/**
 * @brief Set I2S lcd en
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set lcd en
 */
static inline void i2s_ll_set_lcd_en(i2s_dev_t *hw, bool val)
{
    hw->conf2.lcd_en = val;
}

/**
 * @brief Set I2S camera en
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set camera en
 */
static inline void i2s_ll_set_camera_en(i2s_dev_t *hw, bool val)
{
    hw->conf2.camera_en = val;
}

/**
 * @brief Set I2S tx fifo mod force en
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx fifo mod force en
 */
static inline void i2s_ll_set_tx_fifo_mod_force_en(i2s_dev_t *hw, bool val)
{
    hw->fifo_conf.tx_fifo_mod_force_en = val;
}

/**
 * @brief Set I2S rx fifo mod force en
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx fifo mod force en
 */
static inline void i2s_ll_set_rx_fifo_mod_force_en(i2s_dev_t *hw, bool val)
{
    hw->fifo_conf.rx_fifo_mod_force_en = val;
}

/**
 * @brief Set I2S tx right first
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx right first
 */
static inline void i2s_ll_set_tx_right_first(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.tx_right_first = val;
}

/**
 * @brief Set I2S rx right first
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx right first
 */
static inline void i2s_ll_set_rx_right_first(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.rx_right_first = val;
}

/**
 * @brief Set I2S tx slave mod
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx slave mod
 */
static inline void i2s_ll_set_tx_slave_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.tx_slave_mod = val;
}

/**
 * @brief Set I2S rx slave mod
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx slave mod
 */
static inline void i2s_ll_set_rx_slave_mod(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.rx_slave_mod = val;
}

/**
 * @brief Get I2S tx msb right
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get tx msb right
 */
static inline void i2s_ll_get_tx_msb_right(i2s_dev_t *hw, uint32_t *val)
{
     *val = hw->conf.tx_msb_right;
}

/**
 * @brief Get I2S rx msb right
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to get rx msb right
 */
static inline void i2s_ll_get_rx_msb_right(i2s_dev_t *hw, uint32_t *val)
{
     *val = hw->conf.rx_msb_right;
}

/**
 * @brief Set I2S tx msb right
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx msb right
 */
static inline void i2s_ll_set_tx_msb_right(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.tx_msb_right = val;
}

/**
 * @brief Set I2S rx msb right
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx msb right
 */
static inline void i2s_ll_set_rx_msb_right(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.rx_msb_right = val;
}

/**
 * @brief Set I2S tx mono
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set tx mono
 */
static inline void i2s_ll_set_tx_mono(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.tx_mono = val;
}

/**
 * @brief Set I2S rx mono
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set rx mono
 */
static inline void i2s_ll_set_rx_mono(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.rx_mono = val;
}

/**
 * @brief Set I2S sig loopback
 *
 * @param hw Peripheral I2S hardware instance address.
 * @param val value to set sig loopback
 */
static inline void i2s_ll_set_sig_loopback(i2s_dev_t *hw, uint32_t val)
{
    hw->conf.sig_loopback = val;
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

#ifdef __cplusplus
}
#endif
