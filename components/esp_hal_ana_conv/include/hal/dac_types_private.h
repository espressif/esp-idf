/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

/**
 * @brief The internal data source that drives a DAC channel output
 *
 * @note The number of distinct sources differs per chip:
 *       - ESP32/ESP32-S2: cosine generator, DMA, and direct register output are three separate states.
 *       - ESP32-S31: the Sintx path serves both the cosine generator and the direct software output.
 */
typedef enum {
    DAC_DATA_SOURCE_COSINE = 0,  /*!< Channel output driven by the on-chip cosine wave generator (Sintx) */
    DAC_DATA_SOURCE_DMA    = 1,  /*!< Channel output driven by the digital DMA path */
    DAC_DATA_SOURCE_DIRECT = 2,  /*!< Channel output driven by the direct register value */
} dac_data_source_t;

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
