/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/asrc_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/assert.h"
#include "hal/asrc_periph.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define ASRC_LL_GET_HW()   (&ASRC)
#define ASRC_LL_STREAM_NUM (2)

/**
 * @brief  ASRC interrupt types
 */
typedef enum {
    ASRC_LL_INTR_OUTCNT_EOF = (1 << 0),  /**< Output counter EOF interrupt */
} asrc_ll_intr_type_t;

/**
 * @brief  Convert sample rate to table index
 *
 * @param[in]  rate  Sample rate in Hz (8000/16000/24000/32000/44100/48000)
 *
 * @return
 *       - Table  index
 *       - -1     if rate not supported
 */
static inline int8_t asrc_ll_get_rate_index(uint32_t rate)
{
    int32_t idx = rate / 8000;
    if (idx == 5) {
        return (rate == 44100) ? 4 : -1;
    }
    if (idx && rate == idx * 8000 && idx <= 6) {
        return idx - 1;
    }
    return -1;
}

/**
 * @brief  Reset ASRC module
 *
 * @param[in]  hw  Peripheral ASRC hardware instance address
 */
static inline void asrc_ll_reset_asrc_module(asrc_dev_t *hw)
{
    HP_SYS_CLKRST.ahb_asrc_ctrl0.reg_ahb_asrc_rst_en = 1;
    HP_SYS_CLKRST.ahb_asrc_ctrl0.reg_ahb_asrc_rst_en = 0;
}

/**
 * @brief  Enable the bus clock for the ASRC module
 *
 * @param[in]  enable  Enable the bus clock
 */
static inline void asrc_ll_enable_asrc_module(bool enable)
{
    HP_SYS_CLKRST.ahb_asrc_ctrl0.reg_ahb_asrc_sys_clk_en = enable;
}

/**
 * @brief  Force enable the register clock for the ASRC module
 *
 * @param[in]  hw      Peripheral ASRC hardware instance address
 * @param[in]  enable  Enable the register clock
 */
static inline void asrc_ll_force_enable_reg_clock(asrc_dev_t *hw, bool enable)
{
    hw->sys.clk_en = enable;
}

/**
 * @brief  Reset specific ASRC stream
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_reset_stream(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg3.reset = 1;
    hw->asrc_para[asrc_idx].cfg3.reset = 0;
}

/**
 * @brief  Start ASRC processing
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_start_stream(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg4.start = 1;
}

/**
 * @brief  Stop ASRC processing
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_stop_stream(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg4.start = 0;
}

/**
 * @brief  Set ASRC rate register
 *
 * @param[in]  hw         Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx   ASRC stream index
 * @param[in]  src_rate   Source sample rate
 * @param[in]  dest_rate  Destination sample rate
 */
static inline void asrc_ll_set_rate_reg(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t src_rate, uint32_t dest_rate)
{
    int8_t src_idx = asrc_ll_get_rate_index(src_rate);
    int8_t dest_idx = asrc_ll_get_rate_index(dest_rate);
    HAL_ASSERT(src_idx != -1 && dest_idx != -1);
    const asrc_periph_rate_config_t *config = &asrc_periph_rate_table[src_idx][dest_idx];
    hw->asrc_para[asrc_idx].cfg0.rs2_stg0_bypass = config->stg0_bypass;
    hw->asrc_para[asrc_idx].cfg0.rs2_stg1_bypass = config->stg1_bypass;
    hw->asrc_para[asrc_idx].cfg0.rs2_stg0_mode = config->stg0_mode;
    hw->asrc_para[asrc_idx].cfg0.rs2_stg1_mode = config->stg1_mode;
    hw->asrc_para[asrc_idx].cfg0.frac_bypass = config->frac_bypass;
    hw->asrc_para[asrc_idx].cfg0.frac_ahead = config->frac_ahead;
    hw->asrc_para[asrc_idx].cfg1.frac_m = config->frac_m;
    hw->asrc_para[asrc_idx].cfg1.frac_l = config->frac_l;
    hw->asrc_para[asrc_idx].cfg2.frac_recipl = config->frac_recipl;
}

/**
 * @brief  Set ASRC channel mode configuration
 *
 * @param[in]  hw          Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx    ASRC stream index
 * @param[in]  src_ch      Source channel number
 * @param[in]  dest_ch     Destination channel number
 * @param[in]  weight      Weight array
 * @param[in]  weight_len  Weight array length
 */
static inline void asrc_ll_set_channel_mode(asrc_dev_t *hw, uint8_t asrc_idx, uint8_t src_ch,
                                            uint8_t dest_ch, float *weight, uint8_t weight_len)
{
    if (src_ch == 1 && dest_ch == 1) {
        // Mode 0: Mono to Mono (1 channel -> 1 channel)
        hw->asrc_para[asrc_idx].cfg0.mode = 0;
    } else if (src_ch == 1 && dest_ch == 2) {
        // Mode 2: Mono to Dual (1 channel -> 2 channels, duplicate mono channel to both outputs)
        hw->asrc_para[asrc_idx].cfg0.mode = 2;
    } else if (src_ch == 2 && dest_ch == 1) {
        // Mode 3: Dual to Mono (2 channels -> 1 channel)
        // ch_sel determines channel selection: 0 = select channel 0 only, 1 = mix both channels
        HAL_ASSERT(weight_len == 2);
        uint32_t ch_sel = (weight[0] == 1.0f && weight[1] == 0.0f) ? 0 : 1;
        hw->asrc_para[asrc_idx].cfg0.mode = 3;
        hw->asrc_para[asrc_idx].cfg0.sel = ch_sel;
    } else {
        // Mode 1: Dual to Dual (2 channels -> 2 channels)
        hw->asrc_para[asrc_idx].cfg0.mode = 1;
    }
}

/**
 * @brief  Clear input samples counter register
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_clear_incnt_counter(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg5.in_cnt_clr = 1;
}

/**
 * @brief  Enable input samples counter register
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_enable_incnt_counter(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg5.in_cnt_ena = 1;
}

/**
 * @brief  Set expected input samples count register
 *
 * @param[in]  hw            Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx      ASRC stream index
 * @param[in]  in_bytes_num  Number of input samples expected
 */
static inline void asrc_ll_set_in_bytes_num(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t in_bytes_num)
{
    hw->asrc_para[asrc_idx].cfg5.in_samples = in_bytes_num;
}

/**
 * @brief  Clear output samples counter register
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_clear_outcnt_counter(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg6.out_cnt_clr = 1;
}

/**
 * @brief  Enable output samples counter register
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_enable_outcnt_counter(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg6.out_cnt_ena = 1;
}

/**
 * @brief  Enable output samples counter compensation register
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_enable_outsample_comp(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].cfg6.out_samples_comp = 1;
}

/**
 * @brief  Set expected output sample count register
 *
 * @param[in]  hw             Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx       ASRC stream index
 * @param[in]  out_bytes_num  Number of output samples expected
 */
static inline void asrc_ll_set_out_bytes_num(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t out_bytes_num)
{
    hw->asrc_para[asrc_idx].cfg6.out_samples = out_bytes_num;
}

/**
 * @brief  Set EOF (End of Frame) generation mode
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 * @param[in]  eof_mode  EOF generation mode
 */
static inline void asrc_ll_set_eof_mode(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t eof_mode)
{
    hw->asrc_para[asrc_idx].cfg6.out_eof_gen_mode = eof_mode;
}

/**
 * @brief  Reset input fifo
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_reset_input_fifo(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].fifo_ctrl.infifo_reset = 1;
    hw->asrc_para[asrc_idx].fifo_ctrl.infifo_reset = 0;
}

/**
 * @brief  Reset output fifo
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 */
static inline void asrc_ll_reset_output_fifo(asrc_dev_t *hw, uint8_t asrc_idx)
{
    hw->asrc_para[asrc_idx].fifo_ctrl.outfifo_reset = 1;
    hw->asrc_para[asrc_idx].fifo_ctrl.outfifo_reset = 0;
}

/**
 * @brief  Enable ASRC interrupt
 *
 * @param[in]  hw        Pointer to ASRC hardware.
 * @param[in]  asrc_idx  ASRC stream index
 * @param[in]  mask      Interrupt mask to enable
 */
static inline void asrc_ll_enable_intr_mask(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t mask)
{
    hw->asrc_para[asrc_idx].int_ena.val |= mask;
}

/**
 * @brief  Clear ASRC interrupt
 *
 * @param[in]  hw        Pointer to ASRC hardware.
 * @param[in]  asrc_idx  ASRC stream index
 * @param[in]  mask      Interrupt mask to clear
 */
static inline void asrc_ll_clear_intr_mask(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t mask)
{
    hw->asrc_para[asrc_idx].int_clr.val = mask;
}

/**
 * @brief  Disable ASRC interrupt
 *
 * @param[in]  hw        Pointer to ASRC hardware.
 * @param[in]  asrc_idx  ASRC stream index
 * @param[in]  mask      Interrupt mask to disable
 */
static inline void asrc_ll_disable_intr_mask(asrc_dev_t *hw, uint8_t asrc_idx, uint32_t mask)
{
    hw->asrc_para[asrc_idx].int_ena.val &= (~mask);
}

/**
 * @brief  Get ASRC interrupt status
 *
 * @param[in]  hw        Pointer to ASRC hardware.
 * @param[in]  asrc_idx  ASRC stream index
 *
 * @return
 *       - Interrupt  status (masked)
 */
static inline uint32_t asrc_ll_get_intr_status(asrc_dev_t *hw, uint8_t asrc_idx)
{
    return hw->asrc_para[asrc_idx].int_st.val;
}

/**
 * @brief  Get current output sample count
 *
 * @param[in]  hw        Peripheral ASRC hardware instance address
 * @param[in]  asrc_idx  ASRC stream index
 *
 * @return
 *       - Current  output samples count
 */
static inline uint32_t asrc_ll_get_outbytes_cnt(asrc_dev_t *hw, uint8_t asrc_idx)
{
    uint32_t out_cnt = hw->asrc_para[asrc_idx].out_cnt.out_cnt;
    return out_cnt;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
