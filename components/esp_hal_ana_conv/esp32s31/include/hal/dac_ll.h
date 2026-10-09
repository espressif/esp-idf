/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/assert.h"
#include "soc/dac_struct.h"
#include "soc/lp_peri_clkrst_struct.h"
#include "soc/clk_tree_defs.h"
#include "hal/dac_types.h"
#include "hal/dac_types_private.h"

#if SOC_DAC_SUPPORTED

#define SOC_DAC_DC_VIA_SINTX           1
#define SOC_DAC_SINTX_HAS_TIMER_TARGET 1
#define SOC_DAC_SINTX_LUT_SIGNED       0

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Clock setting
---------------------------------------------------------------*/
/**
 * @brief Reset the DAC module
 */
static inline void dac_ll_reset_register(void)
{
    LP_PERI_CLKRST.dac_ctrl.lp_dac_rst_en = 1;
    LP_PERI_CLKRST.dac_ctrl.lp_dac_rst_en = 0;
}

/**
 * @brief Select the source clock of the DAC functional clock (DAC_HS_CLK).
 *
 * @note  On ESP32-S31 both the Sintx (cosine) path and the GDMA path share this
 *        functional clock, so the source applies to both.
 *
 * @param clk_src Clock source, see `soc_periph_dac_digi_clk_src_t`
 */
static inline void dac_ll_clk_set_source(soc_periph_dac_digi_clk_src_t clk_src)
{
    switch (clk_src) {
    case DAC_DIGI_CLK_SRC_LP_FAST:
        LP_PERI_CLKRST.dac_ctrl.lp_dac_clk_sel = 0;
        break;
    case DAC_DIGI_CLK_SRC_XTAL:
        LP_PERI_CLKRST.dac_ctrl.lp_dac_clk_sel = 1;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Enable or disable the DAC functional clock (DAC_HS_CLK).
 *
 * @note  The DAC functional clock (DAC_HS_CLK) gates the whole LP_DAC register block.
 *        While it is disabled, NONE of the registers under `LP_DAC` can be accessed
 *        (writes are dropped and reads return invalid data). Therefore this clock must
 *        be enabled before configuring/accessing any `LP_DAC.*` register.
 *
 * @param enable true to enable, false to disable
 */
static inline void dac_ll_clk_enable(bool enable)
{
    LP_PERI_CLKRST.dac_ctrl.lp_dac_clk_en = enable;
}

/**
 * @brief Set the divider from the source clock to DAC_HS_CLK.
 *
 * @note  DAC_HS_CLK = clock_source / (hs_div + 1)
 */
static inline void dac_ll_clk_set_hs_div(uint8_t hs_div)
{
    LP_PERI_CLKRST.dac_ctrl.lp_dac_div_num = hs_div;
}

/**
 * @brief Get the divider from the source clock to DAC_HS_CLK.
 */
static inline uint8_t dac_ll_clk_get_hs_div(void)
{
    return LP_PERI_CLKRST.dac_ctrl.lp_dac_div_num;
}

/**
 * @brief Set the internal divider from DAC_HS_CLK to the low-speed pad clock (clk_ls).
 *
 * @note  clk_ls = DAC_HS_CLK / (ls_div + 1)
 */
static inline void dac_ll_clk_set_ls_div(uint16_t ls_div)
{
    LP_DAC.pad_cfg.div_num = ls_div;
}

/**
 * @brief Get the divider from DAC_HS_CLK to the low-speed pad clock (clk_ls).
 */
static inline uint16_t dac_ll_clk_get_ls_div(void)
{
    return LP_DAC.pad_cfg.div_num;
}

/*---------------------------------------------------------------
                    DAC pad setting
---------------------------------------------------------------*/
/**
 * @brief Power on the DAC pad and start outputting.
 *
 * @param channel DAC channel num
 */
static inline void dac_ll_pad_power_on(dac_channel_t channel)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.xpd_pad_0 = 1;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.xpd_pad_1 = 1;
    }
}

/**
 * @brief Power down the DAC pad and stop outputting.
 *
 * @param channel DAC channel num
 */
static inline void dac_ll_pad_power_down(dac_channel_t channel)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.xpd_pad_0 = 0;
        LP_DAC.pad_cfg.dbuf_ls_pad_0 = 0;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.xpd_pad_1 = 0;
        LP_DAC.pad_cfg.dbuf_ls_pad_1 = 0;
    }
}

/**
 * @brief Select the internal data source that drives a DAC channel output.
 *
 * @note  The hardware data-select bit only distinguishes DMA vs Sintx. Both
 *        `DAC_DATA_SOURCE_COSINE` and `DAC_DATA_SOURCE_DIRECT` map onto the Sintx path.
 *
 * @param channel DAC channel num
 * @param source  Data source, see `dac_data_source_t`
 */
static inline void dac_ll_pad_set_data_source(dac_channel_t channel, dac_data_source_t source)
{
    const uint32_t sel = (source == DAC_DATA_SOURCE_DMA) ? 1 : 0;
    if (channel == DAC_CHAN_0) {
        LP_DAC.data_output_cfg.pad_0_data_sel = sel;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.data_output_cfg.pad_1_data_sel = sel;
    }
}

/**
 * @brief Set the DAC output code through the Sintx (direct software output) path.
 *
 * @param channel DAC channel num
 * @param code    Output code. Bit width depends on the DAC resolution and speed mode.
 */
__attribute__((always_inline))
static inline void dac_ll_pad_set_output_code(dac_channel_t channel, uint16_t code)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.sintx_data.dc_1 = code;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.sintx_data.dc_2 = code;
    }
}

/**
 * @brief Enable or disable the high-speed mode of a DAC pad.
 *
 * @param channel  DAC channel num
 * @param enable   true to enable, false to disable
 */
static inline void dac_ll_pad_enable_high_speed(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.hs_mode_pad_0 = enable;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.hs_mode_pad_1 = enable;
    }
}

/**
 * @brief Enable or bypass the analog output buffer of a DAC pad (low-speed mode only).
 *
 * @param channel DAC channel num
 * @param enable  true: enable the analog buffer; false: bypass it
 */
static inline void dac_ll_pad_enable_buffer(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.dbuf_ls_pad_0 = enable;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.dbuf_ls_pad_1 = enable;
    }
}

/**
 * @brief Select the output swing range of the analog buffer (only valid when the buffer is enabled).
 *
 * @param channel     DAC channel num
 * @param full_range  true: full-scale swing; false: reduced swing range.
 */
static inline void dac_ll_pad_set_buffer_range(dac_channel_t channel, bool full_range)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.dbuf_range_ls_pad_0 = full_range;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.dbuf_range_ls_pad_1 = full_range;
    }
}

/*---------------------------------------------------------------
                    Controller setting
---------------------------------------------------------------*/

/**
 * @brief Enable/disable the synchronization operation between ADC and DAC.
 *
 * @note  ESP32-S31 has no equivalent register bit, keep this as a no-op for API compatibility.
 */
static inline void dac_ll_sync_by_adc(bool enable)
{
    (void)enable;
}

/**
 * @brief Enable/disable the ADC mux of a DAC pad.
 *
 * @note  For internal testing only.
 *
 * @param channel DAC channel num
 * @param enable  true to enable, false to disable
 */
static inline void dac_ll_enable_adc_mux(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.en_adc_mux_pad_0 = enable;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.en_adc_mux_pad_1 = enable;
    }
}

/*---------------------------------------------------------------
                    Sample-hold setting
---------------------------------------------------------------*/

/**
 * @brief Enable/disable the sample-hold mode of a DAC pad.
 *
 * @param channel DAC channel num
 * @param enable  true to enable, false to disable
 */
static inline void dac_ll_sh_enable(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.dsamp_ls_pad_0 = enable;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.dsamp_ls_pad_1 = enable;
    }
}

/**
 * @brief Set the sample-phase wait cycle.
 *
 * @note Clocked by clk_ls.
 *
 * @param channel DAC channel num
 * @param cycle   Wait cycles
 */
static inline void dac_ll_sh_set_sample_cycle(dac_channel_t channel, uint16_t cycle)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.sample_wait_cfg.wait_target_sample_pad_0 = cycle;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.sample_wait_cfg.wait_target_sample_pad_1 = cycle;
    }
}

/**
 * @brief Set the hold-phase wait cycle.
 *
 * @note Clocked by clk_ls.
 *
 * @param channel DAC channel num
 * @param cycle   Wait cycles
 */
static inline void dac_ll_sh_set_hold_cycle(dac_channel_t channel, uint16_t cycle)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.hold_wait_cfg.wait_target_hold_pad_0 = cycle;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.hold_wait_cfg.wait_target_hold_pad_1 = cycle;
    }
}

/**
 * @brief Set the refresh-phase wait cycle.
 *
 * @note Clocked by clk_ls.
 *
 * @param channel DAC channel num
 * @param cycle   Wait cycles
 */
static inline void dac_ll_sh_set_refresh_cycle(dac_channel_t channel, uint16_t cycle)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.refresh_wait_cfg.wait_target_refresh_pad_0 = cycle;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.refresh_wait_cfg.wait_target_refresh_pad_1 = cycle;
    }
}

/*---------------------------------------------------------------
                    Calibration setting
---------------------------------------------------------------*/
/**
 * @brief Enable/disable the calibration of a DAC pad buffer.
 *
 * @note Valid only when the analog buffer is enabled.
 *
 * @param channel DAC channel num
 * @param enable  true to enable, false to disable
 */
static inline void dac_ll_cali_enable(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.pad_cfg.dcal_ls_pad_0 = enable;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.pad_cfg.dcal_ls_pad_1 = enable;
    }
}

/**
 * @brief Get the calibration comparison result of a DAC pad buffer.
 *
 * @param channel DAC channel num
 * @return Calibration comparison result
 */
static inline bool dac_ll_cali_get_comparison_result(dac_channel_t channel)
{
    if (channel == DAC_CHAN_0) {
        return LP_DAC.cali.cali_out_pad_0;
    } else if (channel == DAC_CHAN_1) {
        return LP_DAC.cali.cali_out_pad_1;
    } else {
        HAL_ASSERT(false);
        return false;
    }
}

/**
 * @brief Set the calibration result of a DAC pad buffer.
 *
 * @param channel DAC channel num
 * @param result Calibration result, 12-bit
 */
static inline void dac_ll_cali_set_result(dac_channel_t channel, uint16_t result)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.cali_result.cali_result_pad_0 = result;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.cali_result.cali_result_pad_1 = result;
    }
}

/**
 * @brief Get the calibration result of a DAC pad buffer.
 *
 * @param channel DAC channel num
 * @return Calibration result, 12-bit
 */
static inline uint16_t dac_ll_cali_get_result(dac_channel_t channel)
{
    if (channel == DAC_CHAN_0) {
        return LP_DAC.cali_result.cali_result_pad_0;
    } else if (channel == DAC_CHAN_1) {
        return LP_DAC.cali_result.cali_result_pad_1;
    } else {
        HAL_ASSERT(false);
        return 0;
    }
}

/*---------------------------------------------------------------
                    Cosine (Sintx) path setting
---------------------------------------------------------------*/
/**
 * @brief Enable the cosine wave generator (Sintx tone mode).
 *
 * @note  Starts the phase accumulator and the Sintx timer that pushes samples to the pad.
 */
static inline void dac_ll_cw_enable_tone(void)
{
    LP_DAC.sintx_cfg.sw_tone = 1;
    LP_DAC.sintx_timer_cfg.sintx_timer_en = 1;
}

/**
 * @brief Enable the Sintx generator for direct (DC) software output.
 *
 * @note  The DC value written by `dac_ll_pad_set_output_code()` is pushed to the pad
 *        by the Sintx timer; the phase accumulator is left disabled.
 */
static inline void dac_ll_cw_enable_dc(void)
{
    LP_DAC.sintx_cfg.sw_tone = 0;
    LP_DAC.sintx_timer_cfg.sintx_timer_en = 1;
}

/**
 * @brief Disable the cosine wave / Sintx generator.
 */
static inline void dac_ll_cw_disable(void)
{
    LP_DAC.sintx_cfg.sw_tone = 0;
    LP_DAC.sintx_timer_cfg.sintx_timer_en = 0;
}

/**
 * @brief Set the timer target of the cosine wave (Sintx) generator timer.
 *
 * @note timer_target: 24-bit, range 0 ~ 0xFFFFFF.
 *       Effective divider is timer_target, except that 0 is treated as 1
 *       (i.e. dac_hs_clk / max(timer_target, 1)).
 */
static inline void dac_ll_cw_set_timer_target(uint32_t timer_target)
{
    LP_DAC.sintx_timer_cfg.sintx_timer_target = timer_target;
}

/**
 * @brief Set the step increment of the cosine wave generator.
 *
 * @note A 16-bit phase accumulator freq_acc is incremented by DAC_SW_FSTEP on every Sintx trigger edge.
 *       The upper 8 bits of the accumulator (freq_acc[15:8]) form the angle index freq_idx.
 * @note sintx output frequency = (dac_hs_clk / max(timer_target, 1)) * (fstep / 2^16)
 */
static inline void dac_ll_cw_set_fstep(uint16_t fstep)
{
    LP_DAC.sintx_cfg.sw_fstep = fstep;
}

/**
 * @brief Set the attenuation of the cosine wave generator output.
 *
 * @param channel DAC channel num
 * @param atten   Attenuation config
 */
static inline void dac_ll_cw_set_atten(dac_channel_t channel, dac_cosine_atten_t atten)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.sintx_cfg.scale_1 = atten;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.sintx_cfg.scale_2 = atten;
    }
}

/**
 * @brief Set the phase of the cosine wave generator output.
 *
 * @note  On ESP32-S31 the sine LUT stores unsigned values, so phase 0/180 maps to 0/1 directly.
 *
 * @param channel DAC channel num
 * @param phase   Phase value
 */
static inline void dac_ll_cw_set_phase(dac_channel_t channel, dac_cosine_phase_t phase)
{
    uint8_t dac_inv = phase == DAC_COSINE_PHASE_180 ? 0x01 : 0x00;
    if (channel == DAC_CHAN_0) {
        LP_DAC.sintx_cfg.inv_1 = dac_inv;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.sintx_cfg.inv_2 = dac_inv;
    }
}

/**
 * @brief Set the offset of the cosine wave generator output.
 *
 * @note  On ESP32-S31 the offset is a 12-bit unsigned value added to the scaled sample with saturation.
 *
 * @param channel DAC channel num
 * @param offset  DC offset. Range: 0 ~ 4095 (LS) or 0 ~ 1023 (HS)
 */
static inline void dac_ll_cw_set_offset(dac_channel_t channel, int16_t offset)
{
    if (channel == DAC_CHAN_0) {
        LP_DAC.sintx_data.dc_1 = offset;
    } else if (channel == DAC_CHAN_1) {
        LP_DAC.sintx_data.dc_2 = offset;
    }
}

/*---------------------------------------------------------------
                    DMA path setting
---------------------------------------------------------------*/
/**
 * @brief Set the DMA path timer target.
 *
 * @param timer_target 24-bit timer target, range 0 ~ 0xFFFFFF.
 *                     Effective divider is timer_target, except that 0 is treated as 1.
 *
 * @note Output rate = dac_hs_clk / max(timer_target, 1)
 */
static inline void dac_ll_dma_set_timer_target(uint32_t timer_target)
{
    LP_DAC.pdma_timer_cfg.pdma_timer_target = timer_target;
}

/**
 * @brief Enable/disable the DAC DMA output timer and transfer.
 *
 * @param enable true to enable, false to disable
 */
static inline void dac_ll_dma_enable_timer(bool enable)
{
    if (enable) {
        LP_DAC.pdma_timer_cfg.pdma_timer_en = 1;
        LP_DAC.pdma_cfg.pdma_trans = 1;
    } else {
        LP_DAC.pdma_cfg.pdma_trans = 0;
        LP_DAC.pdma_timer_cfg.pdma_timer_en = 0;
    }
}

/**
 * @brief Enable/disable the alternate (ping-pong) output mode of the DMA path.
 *
 * @param enable true to route consecutive samples alternately to PAD0/PAD1
 */
static inline void dac_ll_dma_enable_alternate_mode(bool enable)
{
    LP_DAC.pdma_cfg.pdma_alter_mode = enable;
}

/**
 * @brief Reset the DAC DMA FIFO.
 */
static inline void dac_ll_dma_reset_fifo(void)
{
    LP_DAC.pdma_cfg.pdma_reset_fifo = 1;
}

#ifdef __cplusplus
}
#endif

#endif // SOC_DAC_SUPPORTED
