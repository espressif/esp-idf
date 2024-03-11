/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/soc_caps.h"
#if SOC_ADC_SUPPORTED
#include "soc/adc_channel.h"
#endif

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
