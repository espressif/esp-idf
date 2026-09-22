/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_types_private.h"
#include "soc/apb_saradc_struct.h"
#include "soc/sens_struct.h"
#include "soc/rtc_io_struct.h"
#include "soc/apb_saradc_reg.h"

#define SOC_DAC_DC_VIA_SINTX 0
#define SOC_DAC_SINTX_HAS_TIMER_TARGET 0
#define SOC_DAC_SINTX_LUT_SIGNED 0

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    DAC pad setting
---------------------------------------------------------------*/
/**
 * @brief Power on the DAC pad and start outputting voltage.
 *
 * @note Before powering up, make sure the DAC PAD is set to RTC PAD and floating status.
 * @param channel DAC channel num.
 */
static inline void dac_ll_pad_power_on(dac_channel_t channel)
{
    SENS.sar_dac_ctrl1.dac_clkgate_en = 1;
    RTCIO.pad_dac[channel].dac_xpd_force = 1;
    RTCIO.pad_dac[channel].xpd_dac = 1;
}

/**
 * @brief Power down the DAC pad and stop outputting voltage.
 *
 * @param channel DAC channel num.
 */
static inline void dac_ll_pad_power_down(dac_channel_t channel)
{
    RTCIO.pad_dac[channel].dac_xpd_force = 0;
    RTCIO.pad_dac[channel].xpd_dac = 0;
    if (RTCIO.pad_dac[0].xpd_dac == 0 && RTCIO.pad_dac[1].xpd_dac == 0) {
        SENS.sar_dac_ctrl1.dac_clkgate_en = 0;
    }
}

/**
 * @brief Select the internal data source that drives a DAC channel output.
 *
 * @note  Since dac_dig_force is shared between the two channels, either both channels must use DMA as their data source, or neither can.
 *
 * @param channel DAC channel num.
 * @param source  Data source, see `dac_data_source_t`
 */
static inline void dac_ll_pad_set_data_source(dac_channel_t channel, dac_data_source_t source)
{
    if (source == DAC_DATA_SOURCE_DMA) {
        SENS.sar_dac_ctrl1.dac_dig_force = true;
        APB_SARADC.apb_dac_ctrl.apb_dac_trans = true;
    } else {
        bool cw_en = (source == DAC_DATA_SOURCE_COSINE);
        if (channel == DAC_CHAN_0) {
            SENS.sar_dac_ctrl2.dac_cw_en1 = cw_en;
        } else if (channel == DAC_CHAN_1) {
            SENS.sar_dac_ctrl2.dac_cw_en2 = cw_en;
        }
        SENS.sar_dac_ctrl1.dac_dig_force = false;
        APB_SARADC.apb_dac_ctrl.apb_dac_trans = false;
    }
}

/**
 * @brief Set the DAC output code (8 bit).
 *
 * @param channel DAC channel num.
 * @param code    Output code. Range: 0 ~ 255.
 *                The corresponding voltage range is 0 V ~ VDD3P3_RTC.
 */
__attribute__((always_inline))
static inline void dac_ll_pad_set_output_code(dac_channel_t channel, uint8_t code)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCIO.pad_dac[channel], dac, code);
}

/*---------------------------------------------------------------
                    DAC controller setting
---------------------------------------------------------------*/
/**
 * Reset dac by software.
 */
static inline void dac_ll_reset(void)
{
    SENS.sar_dac_ctrl1.dac_reset = 1;
    SENS.sar_dac_ctrl1.dac_reset = 0;
}

/**
 * Enable/disable the synchronization operation function of ADC1 and DAC.
 *
 * @note  If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage.
 *
 * @param enable Enable or disable adc and dac synchronization function.
 */
static inline void dac_ll_sync_by_adc(bool enable)
{
    SENS.sar_amp_ctrl3.sar1_dac_xpd_fsm = enable;
}

/*---------------------------------------------------------------
                    DAC cosine wave generator setting
---------------------------------------------------------------*/
/**
 * @brief Enable the cosine wave generator phase accumulator.
 */
static inline void dac_ll_cw_enable_tone(void)
{
    SENS.sar_dac_ctrl1.sw_tone_en = 1;
}

/**
 * @brief Disable the cosine wave generator
 */
static inline void dac_ll_cw_disable(void)
{
    SENS.sar_dac_ctrl1.sw_tone_en = 0;
}

/**
 * Set the step increment of the cosine wave generator.
 *
 * @note cosine wave frequency = (dig_clk_rtc_freq * fstep) / 2^16
 */
static inline void dac_ll_cw_set_fstep(uint16_t fstep)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl1, sw_fstep, fstep);
}

/**
 * Set the amplitude of the cosine wave generator output.
 *
 * @param channel DAC channel num.
 * @param atten The attenuation of the amplitude. The max amplitude is VDD3P3_RTC.
 *              0: attenuation = 1, amplitude = VDD3P3_RTC / attenuation,
 *              1: attenuation = 2, amplitude = VDD3P3_RTC / attenuation,
 *              2: attenuation = 4, amplitude = VDD3P3_RTC / attenuation,
 *              3: attenuation = 8, amplitude = VDD3P3_RTC / attenuation
 */
static inline void dac_ll_cw_set_atten(dac_channel_t channel, dac_cosine_atten_t atten)
{
    if (channel == DAC_CHAN_0) {
        SENS.sar_dac_ctrl2.dac_scale1 = atten;
    } else if (channel == DAC_CHAN_1) {
        SENS.sar_dac_ctrl2.dac_scale2 = atten;
    }
}

/**
 * Set the phase of the cosine wave generator output.
 *
 * @param channel DAC channel num.
 * @param phase Phase value. 0: 0x02 180: 0x03.
 */
static inline void dac_ll_cw_set_phase(dac_channel_t channel, dac_cosine_phase_t phase)
{
    if (channel == DAC_CHAN_0) {
        SENS.sar_dac_ctrl2.dac_inv1 = phase;
    } else if (channel == DAC_CHAN_1) {
        SENS.sar_dac_ctrl2.dac_inv2 = phase;
    }
}

/**
 * @brief Set the DC offset of the cosine wave generator output.
 *
 * @note Unreasonable settings can cause the signal to be oversaturated.
 * @note On ESP32-S2, dac_inv also inverts the DC component. The caller must
 *       compensate (e.g. negate) the offset for 180° phase before calling this.
 * @param channel DAC channel num.
 * @param offset  DC offset. Range: -128 ~ 127.
 */
static inline void dac_ll_cw_set_offset(dac_channel_t channel, int8_t offset)
{
    if (channel == DAC_CHAN_0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl2, dac_dc1, offset);
    } else if (channel == DAC_CHAN_1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl2, dac_dc2, offset);
    }
}

/*---------------------------------------------------------------
                    DAC DMA setting
---------------------------------------------------------------*/
/**
 * @brief Enable/disable invert the DAC DMA clock signal.
 *
 * @param enable true to invert, false otherwise
 *
 * @note  On ESP32-S2, when the DAC is driven through DMA, enabling inv_clk is necessary to avoid glitches in the output waveform.
 */
static inline void dac_ll_dma_clk_inv(bool enable)
{
    SENS.sar_dac_ctrl1.dac_clk_inv = enable;
}

/**
 * @brief Set the DMA path timer target.
 *
 * @note  The clocks of the DAC digital controller use the ADC digital controller clock divider.
 * @note  DMA output frequency = controller_clk / timer_target.
 *
 * @param timer_target Number of divided-clock cycles between DAC outputs.
 */
static inline void dac_ll_dma_set_timer_target(uint32_t timer_target)
{
    APB_SARADC.apb_dac_ctrl.dac_timer_target = timer_target;
}

/**
 * @brief Enable/disable the DAC DMA output timer.
 *
 * @param enable true to enable, false to disable
 */
static inline void dac_ll_dma_enable_timer(bool enable)
{
    APB_SARADC.apb_dac_ctrl.dac_timer_en = enable;
}

/**
 * @brief Enable/disable the alternate (ping-pong) output mode of the DMA path.
 *
 * @param enable true to route consecutive samples alternately to the two channels
 */
static inline void dac_ll_dma_enable_alternate_mode(bool enable)
{
    APB_SARADC.apb_dac_ctrl.apb_dac_alter_mode = enable;
}

/**
 * @brief Reset the DAC DMA FIFO.
 */
static inline void dac_ll_dma_reset_fifo(void)
{
    APB_SARADC.apb_dac_ctrl.dac_reset_fifo = 1;
    APB_SARADC.apb_dac_ctrl.dac_reset_fifo = 0;
}

/**
 * @brief Reset the DAC DMA FSM, i.e. the DAC-side consumer of DMA samples (timer, alter-mode demux).
 */
static inline void dac_ll_dma_reset_fsm(void)
{
    APB_SARADC.apb_dac_ctrl.apb_dac_rst = 1;
    APB_SARADC.apb_dac_ctrl.apb_dac_rst = 0;
}

#ifdef __cplusplus
}
#endif
