/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains types for MSPI timing tuning
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#if SOC_MEMSPI_TIMING_TUNING_BY_DQS || SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY || SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
#include "hal/mspi_ll.h"
#include "mspi_timing_tuning_configs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MEMSPI_TIMING_TUNING_BY_DQS || SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY
/**
 * Delayline
 */
typedef struct {
    uint8_t data_delayline;
    uint8_t dqs_delayline;
} __attribute__((packed)) delayline_config_t;

/**
 * MSPI timing tuning registers.
 */
typedef struct {
    uint8_t spi_din_mode;    // input signal delay mode
    uint8_t spi_din_num;     // input signal delay number
    uint8_t extra_dummy_len; // extra dummy length
} mspi_timing_tuning_param_t;

/**
 * MSPI timing tuning configurations
 */
typedef struct {
    //for psram
    mspi_ll_dqs_phase_t phase[MSPI_LL_DQS_PHASE_MAX];
    delayline_config_t delayline_table[MSPI_TIMING_CONFIG_NUM_MAX];
    //for flash
    mspi_timing_tuning_param_t tuning_config_table[MSPI_TIMING_CONFIG_NUM_MAX];
    uint32_t flash_default_config_id;
    //common
    union {
        uint32_t available_config_num;
        uint32_t available_phase_num;
    };
} mspi_timing_config_t;
#endif  //#if SOC_MEMSPI_TIMING_TUNING_BY_DQS || SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY


#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
/**
 * MSPI timing tuning registers.
 * Upper layer rely on these 3 registers to tune the timing.
 */
typedef struct {
    uint8_t spi_din_mode;    // input signal delay mode
    uint8_t spi_din_num;     // input signal delay number
    uint8_t extra_dummy_len; // extra dummy length
} mspi_timing_tuning_param_t;

/**
 * MSPI timing tuning configurations
 */
typedef struct {
    mspi_timing_tuning_param_t tuning_config_table[MSPI_TIMING_CONFIG_NUM_MAX];       // Available timing tuning configs
    uint32_t available_config_num;                                                    // Available timing tuning config numbers
    uint32_t default_config_id;                                                       // If tuning fails, we use this one as default
} mspi_timing_config_t;
#endif

#ifdef __cplusplus
}
#endif
