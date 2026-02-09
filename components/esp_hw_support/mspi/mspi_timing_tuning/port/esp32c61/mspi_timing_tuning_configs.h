/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "esp_assert.h"
#include "esp_flash_partitions.h"

#define MSPI_TIMING_MSPI1_IS_INVOLVED                CONFIG_ESPTOOLPY_FLASHFREQ_120M   //This means esp flash driver needs to be notified
#define MSPI_TIMING_CONFIG_NUM_MAX                   32  //This should be larger than the max available timing config num
#define MSPI_TIMING_TEST_DATA_LEN                    128
#define MSPI_TIMING_PSRAM_TEST_DATA_ADDR             0x100000
#define MSPI_TIMING_FLASH_TEST_DATA_ADDR             ESP_BOOTLOADER_OFFSET

//--------------------------------------FLASH Sampling Mode --------------------------------------//
#define MSPI_TIMING_FLASH_STR_MODE                   1
//--------------------------------------FLASH Module Clock --------------------------------------//
#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               20
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               40
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               80
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               120
#endif
//------------------------------------FLASH Needs Tuning or not-------------------------------------//
#define MSPI_TIMING_FLASH_NEEDS_TUNING               (MSPI_TIMING_FLASH_MODULE_CLOCK > 80)

//--------------------------------------PSRAM Sampling Mode --------------------------------------//
#define MSPI_TIMING_PSRAM_STR_MODE                   1
//--------------------------------------PSRAM Module Clock --------------------------------------//
#if CONFIG_SPIRAM
#if CONFIG_SPIRAM_SPEED_40M
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               40
#elif CONFIG_SPIRAM_SPEED_80M
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               80
#elif CONFIG_SPIRAM_SPEED_120M
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               120
#endif
#else   //Disable PSRAM
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               10      //Define this to 10MHz
#endif
//------------------------------------PSRAM Needs Tuning or not-------------------------------------//
#if MSPI_TIMING_PSRAM_STR_MODE && !CONFIG_SECURE_ENABLE_TEE
#define MSPI_TIMING_PSRAM_NEEDS_TUNING               (MSPI_TIMING_PSRAM_MODULE_CLOCK > 40)
#endif

///////////////////////////////////// FLASH CORE CLOCK /////////////////////////////////////
//FLASH 80M
#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      80
#define MSPI_TIMING_FLASH_CONSECUTIVE_LEN_MAX        6
#endif

//FLASH 120M
#if CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      120
#define MSPI_TIMING_FLASH_CONSECUTIVE_LEN_MAX        4
#endif

///////////////////////////////////// PSRAM CORE CLOCK /////////////////////////////////////
//PSRAM 80M
#if CONFIG_SPIRAM_SPEED_80M
#define MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      80
#endif

//PSRAM 120M
#if CONFIG_SPIRAM_SPEED_120M
#define MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      120
#endif  //PSRAM 120M DTR

//------------------------------------------Determine the Core Clock-----------------------------------------------//
/**
 * @note
 * Limitation 1:
 * MSPI FLASH and PSRAM share the core clock register. Therefore,
 * the expected CORE CLOCK frequencies should be the same.
 */
#if MSPI_TIMING_FLASH_NEEDS_TUNING && MSPI_TIMING_PSRAM_NEEDS_TUNING
ESP_STATIC_ASSERT(MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ == MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ, "FLASH and PSRAM Mode configuration are not supported");
#define MSPI_TIMING_CORE_CLOCK_MHZ                   MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ

//If only FLASH needs tuning, the core clock COULD be as FLASH expected
#elif MSPI_TIMING_FLASH_NEEDS_TUNING && !MSPI_TIMING_PSRAM_NEEDS_TUNING
ESP_STATIC_ASSERT(MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ % MSPI_TIMING_PSRAM_MODULE_CLOCK == 0, "FLASH and PSRAM Mode configuration are not supported");
#define MSPI_TIMING_CORE_CLOCK_MHZ                   MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ

//If only PSRAM needs tuning, the core clock COULD be as PSRAM expected
#elif !MSPI_TIMING_FLASH_NEEDS_TUNING && MSPI_TIMING_PSRAM_NEEDS_TUNING
ESP_STATIC_ASSERT(MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ % MSPI_TIMING_FLASH_MODULE_CLOCK == 0, "FLASH and PSRAM Mode configuration are not supported");
#define MSPI_TIMING_CORE_CLOCK_MHZ                   MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ

#else
#define MSPI_TIMING_CORE_CLOCK_MHZ                   80
#endif


//------------------------------------------Helper Macros to get FLASH/PSRAM tuning configs-----------------------------------------------//
#define __GET_TUNING_CONFIG(type, core_clock, module_clock, mode) \
        (mspi_timing_config_t) { .tuning_config_table = MSPI_TIMING_##type##_CONFIG_TABLE_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode, \
                                .available_config_num = MSPI_TIMING_##type##_CONFIG_NUM_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode, \
                                .default_config_id = MSPI_TIMING_##type##_DEFAULT_CONFIG_ID_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode }

#define _GET_TUNING_CONFIG(type, core_clock, module_clock, mode) __GET_TUNING_CONFIG(type, core_clock, module_clock, mode)

#define MSPI_TIMING_FLASH_GET_TUNING_CONFIG(core_clock_mhz, module_clock_mhz, mode) _GET_TUNING_CONFIG(FLASH, core_clock_mhz, module_clock_mhz, mode)
#define MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(core_clock_mhz, module_clock_mhz, mode) _GET_TUNING_CONFIG(PSRAM, core_clock_mhz, module_clock_mhz, mode)


/**
 * Timing Tuning Parameters
 */
//FLASH: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE         {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {2, 1, 2}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {2, 1, 3}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {2, 1, 4}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE           12
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE    2

//FLASH: core clock 80M, module clock 80M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_80M_MODULE_CLK_80M_STR_MODE           {{2, 2, 1}, {2, 1, 1}, {2, 0, 1}, {0, 0, 0}, {3, 1, 2}, {2, 3, 2}, {2, 2, 2}, {2, 1, 2}, {2, 0, 1}, {0, 0, 1}, {3, 1, 3}, {2, 3, 3}, {2, 2, 3}, {2, 1, 3}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_80M_MODULE_CLK_80M_STR_MODE             14
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_80M_MODULE_CLK_80M_STR_MODE      4

//PSRAM: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE         {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {2, 1, 2}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {2, 1, 3}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {2, 1, 4}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE           12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE    4

//PSRAM: core clock 80M, module clock 80M, STR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_80M_MODULE_CLK_80M_STR_MODE          {{2, 2, 1}, {2, 1, 1}, {2, 0, 1}, {0, 0, 0}, {3, 1, 2}, {2, 3, 2}, {2, 2, 2}, {2, 1, 2}, {2, 0, 1}, {0, 0, 1}, {3, 1, 3}, {2, 3, 3}, {2, 2, 3}, {2, 1, 3}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_80M_MODULE_CLK_80M_STR_MODE            14
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_80M_MODULE_CLK_80M_STR_MODE     4
