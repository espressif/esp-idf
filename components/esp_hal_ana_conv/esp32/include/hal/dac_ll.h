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
#include "soc/rtc_io_struct.h"
#include "soc/sens_struct.h"
#include "hal/dac_types.h"
#include "hal/dac_types_private.h"

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
}

/**
 * @brief Select the internal data source that drives a DAC channel output.
 *
 * @note  Since dac_dig_force is shared between the two channels, either both channels must use DMA as their data source, or neither can.
 * @note  When selecting the DMA source, the DAC output data comes from the I2S DMA.
 *
 * @param channel DAC channel num.
 * @param source  Data source, see `dac_data_source_t`
 */
static inline void dac_ll_pad_set_data_source(dac_channel_t channel, dac_data_source_t source)
{
    if (source == DAC_DATA_SOURCE_DMA) {
        SENS.sar_dac_ctrl1.dac_dig_force = true;
    } else {
        bool cw_en = (source == DAC_DATA_SOURCE_COSINE);
        if (channel == DAC_CHAN_0) {
            SENS.sar_dac_ctrl2.dac_cw_en1 = cw_en;
        } else if (channel == DAC_CHAN_1) {
            SENS.sar_dac_ctrl2.dac_cw_en2 = cw_en;
        }
        SENS.sar_dac_ctrl1.dac_dig_force = false;
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
 * Enable/disable the synchronization operation function of ADC1 and DAC.
 *
 * @note  If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage.
 *
 * @param enable Enable or disable adc and dac synchronization function.
 */
static inline void dac_ll_sync_by_adc(bool enable)
{
    SENS.sar_meas_ctrl2.sar1_dac_xpd_fsm = enable;
}

/*---------------------------------------------------------------
                    Cosine wave generator setting
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
 * @note On ESP32, dac_inv also inverts the DC component. The caller must
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
 */
static inline void dac_ll_dma_clk_inv(bool enable)
{
    SENS.sar_dac_ctrl1.dac_clk_inv = enable;
}

#ifdef __cplusplus
}
#endif
