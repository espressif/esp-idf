/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/soc_caps.h"
#if !CONFIG_IDF_TARGET_ESP32C6 // TODO: IDF-5310
#include "soc/syscon_struct.h"
#endif

#if SOC_ADC_RTC_CTRL_SUPPORTED
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#endif

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#include "soc/rtc_io_struct.h"
#endif
#if !CONFIG_IDF_TARGET_ESP32C6 // TODO: IDF-5310
#include "soc/rtc_cntl_struct.h"
#endif
#include "soc/adc_channel.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Store IO number corresponding to the ADC channel number.
 *
 * @value
 *      - >=0 : GPIO number index.
 *      - -1  : Not support.
 */
extern const int adc_channel_io_map[SOC_ADC_PERIPH_NUM][SOC_ADC_MAX_CHANNEL_NUM];

#ifdef __cplusplus
}
#endif
