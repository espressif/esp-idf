/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#define MSPI_TIMING_MSPI1_IS_INVOLVED       CONFIG_ESPTOOLPY_FLASHFREQ_120M   //This means esp flash driver needs to be notified
#define MSPI_TIMING_CONFIG_NUM_MAX          32  //This should be larger than the max available timing config num
#define MSPI_TIMING_TEST_DATA_LEN           128
#define MSPI_TIMING_PSRAM_TEST_DATA_ADDR    0x80
#define MSPI_TIMING_DELAYLINE_TEST_NUMS     100
#define MSPI_TIMING_FLASH_TEST_DATA_ADDR    CONFIG_BOOTLOADER_OFFSET_IN_FLASH

#define MSPI_TIMING_CORE_CLOCK_DIV          1
#if CONFIG_SPIRAM_SPEED_250M
#define MSPI_TIMING_PSRAM_NEEDS_TUNING      1
#define MSPI_TIMING_MPLL_FREQ_MHZ           500
#elif CONFIG_SPIRAM_SPEED_200M
#define MSPI_TIMING_PSRAM_NEEDS_TUNING      1
#define MSPI_TIMING_MPLL_FREQ_MHZ           400
#elif CONFIG_SPIRAM_SPEED_80M
#define MSPI_TIMING_PSRAM_NEEDS_TUNING      1
#define MSPI_TIMING_MPLL_FREQ_MHZ           320
#else
#define MSPI_TIMING_MPLL_FREQ_MHZ           400
#endif
#define MSPI_TIMING_SPLL_FREQ_MHZ           480

#define MSPI_TIMING_PSRAM_DTR_MODE          CONFIG_SPIRAM_MODE_HEX
#define MSPI_TIMING_FLASH_STR_MODE          1

#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define MSPI_TIMING_FLASH_MODULE_CLOCK      20
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define MSPI_TIMING_FLASH_MODULE_CLOCK      40
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define MSPI_TIMING_FLASH_MODULE_CLOCK      80
#else //CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define MSPI_TIMING_FLASH_MODULE_CLOCK      120
#endif
#define MSPI_TIMING_FLASH_NEEDS_TUNING      (MSPI_TIMING_FLASH_MODULE_CLOCK > 80)

#if MSPI_TIMING_FLASH_NEEDS_TUNING
#define MSPI_TIMING_FLASH_CORE_CLOCK_MHZ    120
#else
#define MSPI_TIMING_FLASH_CORE_CLOCK_MHZ    80
#endif

//------------------------------------------Helper Macros to get FLASH/PSRAM tuning configs-----------------------------------------------//
#define __GET_TUNING_CONFIG(type, core_clock, module_clock, mode) \
        (mspi_timing_config_t) { .tuning_config_table = MSPI_TIMING_##type##_CONFIG_TABLE_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode, \
                                .available_config_num = MSPI_TIMING_##type##_CONFIG_NUM_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode, \
                                .flash_default_config_id = MSPI_TIMING_##type##_DEFAULT_CONFIG_ID_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode }

#define _GET_TUNING_CONFIG(type, core_clock, module_clock, mode) __GET_TUNING_CONFIG(type, core_clock, module_clock, mode)

#define MSPI_TIMING_FLASH_GET_TUNING_CONFIG(core_clock_mhz, module_clock_mhz, mode) _GET_TUNING_CONFIG(FLASH, core_clock_mhz, module_clock_mhz, mode)

/**
 * Timing Tuning Parameters
 */
//FLASH: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE        {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {2, 1, 2}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {2, 1, 3}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {2, 1, 4}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE   2
