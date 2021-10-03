// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdlib.h>
#include "hal/misc.h"
#include "soc/dac_periph.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_struct.h"
#include "hal/dac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
static inline void dac_ll_update_output_value(dac_channel_t channel, uint8_t value)
{
    if (channel == DAC_CHANNEL_1) {
        SENS.sar_dac_ctrl2.dac_cw_en1 = 0;
        HAL_FORCE_MODIFY_U32_REG_FIELD(RTCIO.pad_dac[channel], dac, value);
    } else if (channel == DAC_CHANNEL_2) {
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
static inline void dac_ll_cw_set_channel(dac_channel_t channel, bool enable)
{
    if (channel == DAC_CHANNEL_1) {
        SENS.sar_dac_ctrl2.dac_cw_en1 = enable;
    } else if (channel == DAC_CHANNEL_2) {
        SENS.sar_dac_ctrl2.dac_cw_en2 = enable;
    }
}

/**
 * Set frequency of cosine wave generator output.
 *
 * @note We know that CLK8M is about 8M, but don't know the actual value. so this freq have limited error.
 * @param freq_hz CW generator frequency. Range: 130(130Hz) ~ 55000(100KHz).
 */
static inline void dac_ll_cw_set_freq(uint32_t freq)
{
    uint32_t sw_freq = freq * 0xFFFF / RTC_FAST_CLK_FREQ_APPROX;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl1, sw_fstep, (sw_freq > 0xFFFF) ? 0xFFFF : sw_freq);
}

/**
 * Set the amplitude of the cosine wave generator output.
 *
 * @param channel DAC channel num.
 * @param scale The multiple of the amplitude. The max amplitude is VDD3P3_RTC.
 */
static inline void dac_ll_cw_set_scale(dac_channel_t channel, dac_cw_scale_t scale)
{
    if (channel == DAC_CHANNEL_1) {
        SENS.sar_dac_ctrl2.dac_scale1 = scale;
    } else if (channel == DAC_CHANNEL_2) {
        SENS.sar_dac_ctrl2.dac_scale2 = scale;
    }
}

/**
 * Set the phase of the cosine wave generator output.
 *
 * @param channel DAC channel num.
 * @param scale Phase value.
 */
static inline void dac_ll_cw_set_phase(dac_channel_t channel, dac_cw_phase_t phase)
{
    if (channel == DAC_CHANNEL_1) {
        SENS.sar_dac_ctrl2.dac_inv1 = phase;
    } else if (channel == DAC_CHANNEL_2) {
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
    if (channel == DAC_CHANNEL_1) {
        if (SENS.sar_dac_ctrl2.dac_inv1 == DAC_CW_PHASE_180) {
            offset = 0 - offset;
        }
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl2, dac_dc1, offset ? offset : (-128 - offset));
    } else if (channel == DAC_CHANNEL_2) {
        if (SENS.sar_dac_ctrl2.dac_inv2 == DAC_CW_PHASE_180) {
            offset = 0 - offset;
        }
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_dac_ctrl2, dac_dc2, offset ? offset : (-128 - offset));
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
