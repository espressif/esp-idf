/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief  ASRC rate conversion register configuration structure
 */
typedef struct {
    uint32_t  stg1_bypass;  /*!< Stage 1 bypass control
                                - 0: Enable stage 1 processing
                                - 1: Bypass stage 1 (pass-through) */
    uint32_t  stg0_bypass;  /*!< Stage 0 bypass control
                                - 0: Enable stage 0 processing
                                - 1: Bypass stage 0 (pass-through) */
    uint32_t  frac_bypass;  /*!< Fractional resampler bypass control
                                - 0: Enable fractional resampler
                                - 1: Bypass fractional resampler */
    uint32_t  stg1_mode;    /*!< Stage 1 operation mode
                                - 1: Down-sampling by 2 (decimation)
                                - 0: Up-sampling by 2 (interpolation) */
    uint32_t  stg0_mode;    /*!< Stage 0 operation mode
                                - 1: Down-sampling by 2 (decimation)
                                - 0: Up-sampling by 2 (interpolation) */
    uint32_t  frac_ahead;   /*!< Fractional resampler advance flag
                                - 0: Normal operation
                                - 1: Advance mode (implementation specific) */
    uint32_t  frac_m;       /*!< Fractional resampler numerator
                                 Used in ratio calculation: frac_m/frac_l
                                 Range: Depends on sample rate conversion ratio */
    uint32_t  frac_l;       /*!< Fractional resampler denominator
                                 Used in ratio calculation: frac_m/frac_l
                                 Range: Depends on sample rate conversion ratio */
    uint32_t  frac_recipl;  /*!< Fractional resampler reciprocal value
                                 Pre-calculated reciprocal for hardware efficiency
                                 Formula: ((1 << 20) + frac_l) / (2 * frac_l) */
} asrc_periph_rate_config_t;

extern const asrc_periph_rate_config_t asrc_periph_rate_table[6][6];

#ifdef __cplusplus
}
#endif  /* __cplusplus */
