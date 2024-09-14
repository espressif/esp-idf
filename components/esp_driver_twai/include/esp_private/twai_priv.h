/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_twai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TWAI hardware-dependent bit-timing constant
 *
 * Used for calculating and checking bit-timing parameters
 */
typedef struct {
    uint32_t brp_min;   /* Bit-rate prescaler */
    uint32_t brp_max;
    uint8_t tseg1_min;  /* Time segment 1 = prop_seg + phase_seg1 */
    uint8_t tseg1_max;
    uint8_t tseg2_min;  /* Time segment 2 = phase_seg2 */
    uint8_t tseg2_max;
    uint8_t sjw_max;    /* Synchronisation jump width */
} twai_timing_constraint_t;

/**
 * @brief Calculate TWAI timing parameters for a given source frequency and baud rate.
 *
 * This function computes the bit timing parameters (`brp`, `prop_seg`, `tseg_1`, `tseg_2`, `sjw`)
 * based on the provided source clock frequency and desired baud rate. It also adjusts for the sample
 * point ratio if specified.
 *
 * @param source_freq The source clock frequency in Hz.
 * @param in_param Pointer to the input configuration, which includes baud rate and sample rate.
 * @param hw_limit Pointer to const of hardware register limitation.
 * @param out_param Pointer to the output structure where the calculated timing parameters will be stored.
 * @return the actual hardware adopted baudrate.
 */
uint32_t twai_node_timing_calc_param(const uint32_t source_freq, const twai_timing_basic_config_t *in_param, const twai_timing_constraint_t *hw_limit, twai_timing_advanced_config_t *out_param);

#ifdef __cplusplus
}
#endif
