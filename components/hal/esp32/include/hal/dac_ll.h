/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
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
#include "soc/dac_periph.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_struct.h"
#include "hal/dac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DAC_LL_CW_PHASE_0       0x02
#define DAC_LL_CW_PHASE_180     0x03

/**
 * Power on dac module and start output voltage.
 *
 * @note Before powering up, make sure the DAC PAD is set to RTC PAD and floating status.
 * @param channel DAC channel num.
 */
static inline void dac_ll_power_on(dac_channel_t channel)
{
    RTCIO.pad_dac[channel].dac_xpd_force = 1;
    RTCIO.pad_dac[channel].xpd_dac = 1;
}

/**
 * Power done dac module and stop output voltage.
 *
 * @param channel DAC channel num.
 */
static inline void dac_ll_power_down(dac_channel_t channel)
{
    RTCIO.pad_dac[channel].dac_xpd_force = 0;
    RTCIO.pad_dac[channel].xpd_dac = 0;
}

/**
 * Output voltage with value (8 bit).
 *
 * @param channel DAC channel num.
 * @param value Output value. Value range: 0 ~ 255.
 *        The corresponding range of voltage is 0v ~ VDD3P3_RTC.
 */
__attribute__((always_inline))
static inline void dac_ll_update_output_value(dac_channel_t channel, uint8_t value)
{
    if (channel == DAC_CHAN_0) {
        SENS.sar_dac_ctrl2.dac_cw_en1 = 0;
        HAL_FORCE_MODIFY_U32_REG_FIELD(RTCIO.pad_dac[channel], dac, value);
    } else if (channel == DAC_CHAN_1) {
        SENS.sar_dac_ctrl2.dac_cw_en2 = 0;
        HAL_FORCE_MODIFY_U32_REG_FIELD(RTCIO.pad_dac[channel], dac, value);
    }
}

/**
 * Enable/disable the synchronization operation function of ADC1 and DAC.
 *
 * @note  If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage.
 *
 * @param enable Enable or disable adc and dac synchronization function.
 */
static inline void dac_ll_rtc_sync_by_adc(bool enable)
{
    SENS.sar_meas_ctrl2.sar1_dac_xpd_fsm = enable;
}

/************************************/
/*  DAC cosine wave generator API's */
/************************************/
/**
 * Enable cosine wave generator output.
 */
static inline void dac_ll_cw_generator_enable(void)
{
    SENS.sar_dac_ctrl1.sw_tone_en = 1;
}

/**
 * Disable cosine wave generator output.
 */
static inline void dac_ll_cw_generator_disable(void)
{
    SENS.sar_dac_ctrl1.sw_tone_en = 0;
}

/**
 * Enable the cosine wave generator of DAC channel.
 *
 * @param channel DAC channel num.
 * @param enable
 */
static inline void dac_ll_cw_enable_channel(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHAN_0) {
        SENS.sar_dac_ctrl2.dac_cw_en1 = enable;
    } else if (channel == DAC_CHAN_1) {
        SENS.sar_dac_ctrl2.dac_cw_en2 = enable;
    }
}

/**
 * Set frequency of cosine wave generator output.
 *
 * @note We know that CLK8M is about 8M, but don't know the actual value. so this freq have limited error.
 * @param freq_hz CW generator frequency. Range: >= 130Hz, no exact ceiling limitation, but will distort when reach several MHz
 * @param rtc8m_freq the calibrated RTC 8M clock frequency
 */
static inline void dac_ll_cw_set_freq(uint32_t freq, uint32_t rtc8m_freq)
{
    uint32_t sw_freq = (uint32_t)(((uint64_t)freq << 16) / rtc8m_freq);
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl1, sw_fstep, (sw_freq > 0xFFFF) ? 0xFFFF : sw_freq);
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
 * Set the voltage value of the DC component of the cosine wave generator output.
 *
 * @note The DC offset setting should be after phase setting.
 * @note Unreasonable settings can cause the signal to be oversaturated.
 * @param channel DAC channel num.
 * @param offset DC value. Range: -128 ~ 127.
 */
static inline void dac_ll_cw_set_dc_offset(dac_channel_t channel, int8_t offset)
{
    if (channel == DAC_CHAN_0) {
        if (SENS.sar_dac_ctrl2.dac_inv1 == DAC_LL_CW_PHASE_180) {
            offset = -offset;
        }
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl2, dac_dc1, offset);
    } else if (channel == DAC_CHAN_1) {
        if (SENS.sar_dac_ctrl2.dac_inv2 == DAC_LL_CW_PHASE_180) {
            offset = -offset;
        }
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl2, dac_dc2, offset);
    }
}

/************************************/
/*           DAC DMA API's          */
/************************************/
/**
 * Enable/disable DAC output data from I2S DMA.
 * I2S_CLK connect to DAC_CLK, I2S_DATA_OUT connect to DAC_DATA.
 */
static inline void dac_ll_digi_enable_dma(bool enable)
{
    SENS.sar_dac_ctrl1.dac_dig_force = enable;
    SENS.sar_dac_ctrl1.dac_clk_inv = enable;
}

#ifdef __cplusplus
}
#endif
