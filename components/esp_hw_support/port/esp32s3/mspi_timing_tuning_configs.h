/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "esp_assert.h"
#include "esp_flash_partitions.h"

#define MSPI_TIMING_MSPI1_IS_INVOLVED                1   //This means esp flash driver needs to be notified
#define MSPI_TIMING_CONFIG_NUM_MAX                   20  //This should be larger than the max available timing config num
#define MSPI_TIMING_TEST_DATA_LEN                    64
#define MSPI_TIMING_PSRAM_TEST_DATA_ADDR             0
#define MSPI_TIMING_FLASH_TEST_DATA_ADDR             ESP_BOOTLOADER_OFFSET
/**
 * @note BACKGOURND:
 *
 * The SPI FLASH module clock and SPI PSRAM module clock is divided from the SPI core clock, core clock is from system clock:
 *
 * PLL    ----|                      |---- FLASH Module Clock
 * XTAL   ----|----> Core Clock ---->|
 * RTC8M  ----|                      |---- PSRAM Module Clock
 *
 *
 * DDR stands for double data rate, MSPI samples at both posedge and negedge. So the real spped will be doubled.
 * Speed from high to low: 120M DDR > 80M DDR > 120 SDR > 80M SDR > ...
 *
 * Module with speed lower than 120M SDR doesn't need to be tuned
 *
 * @note LIMITATION:
 * How to determine the core clock on 728. There are 2 limitations.
 *
 * 1. MSPI FLASH and PSRAM share the core clock register. Therefore:
 * MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ == MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ
 *
 * 2. DDR mode requires the core clock divider (core_clk / div = module_clk) to be power of 2.
 */
//--------------------------------------FLASH Sampling Mode --------------------------------------//
#define MSPI_TIMING_FLASH_DTR_MODE                   CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
#define MSPI_TIMING_FLASH_STR_MODE                   CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
//--------------------------------------FLASH Module Clock --------------------------------------//
#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               20
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               40
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               80
#else //CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define MSPI_TIMING_FLASH_MODULE_CLOCK               120
#endif
//------------------------------------FLASH Needs Tuning or not-------------------------------------//
#if MSPI_TIMING_FLASH_DTR_MODE
#define MSPI_TIMING_FLASH_NEEDS_TUNING               (MSPI_TIMING_FLASH_MODULE_CLOCK > 40)
#elif MSPI_TIMING_FLASH_STR_MODE
#define MSPI_TIMING_FLASH_NEEDS_TUNING               (MSPI_TIMING_FLASH_MODULE_CLOCK > 80)
#endif

//--------------------------------------PSRAM Sampling Mode --------------------------------------//
#define MSPI_TIMING_PSRAM_DTR_MODE                   CONFIG_SPIRAM_MODE_OCT
#define MSPI_TIMING_PSRAM_STR_MODE                   !CONFIG_SPIRAM_MODE_OCT
//--------------------------------------PSRAM Module Clock --------------------------------------//
#if CONFIG_SPIRAM
#if CONFIG_SPIRAM_SPEED_40M
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               40
#elif CONFIG_SPIRAM_SPEED_80M
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               80
#else //CONFIG_SPIRAM_SPEED_120M
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               120
#endif
#else   //Disable PSRAM
#define MSPI_TIMING_PSRAM_MODULE_CLOCK               10      //Define this to 10MHz, because we rely on `MSPI_TIMING_PSRAM_MODULE_CLOCK` macro for calculation and check below, see `Determine the Core Clock` chapter
#endif
//------------------------------------PSRAM Needs Tuning or not-------------------------------------//
#if MSPI_TIMING_PSRAM_DTR_MODE
#define MSPI_TIMING_PSRAM_NEEDS_TUNING               (MSPI_TIMING_PSRAM_MODULE_CLOCK > 40)
#elif MSPI_TIMING_PSRAM_STR_MODE
#define MSPI_TIMING_PSRAM_NEEDS_TUNING               (MSPI_TIMING_PSRAM_MODULE_CLOCK > 80)
#endif


/**
 * @note Define A feasible core clock below: MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ and MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ
 */
/**
 * Due to MSPI core clock is used by both MSPI Flash and PSRAM clock,
 * define the STR/DTR mode here for selecting the core clock:
 * @note If either Flash or PSRAM, or both of them are set to DTR mode, then we use DIV 2
 */
#if (MSPI_TIMING_FLASH_DTR_MODE || MSPI_TIMING_PSRAM_DTR_MODE)
#define MSPI_TIMING_CORE_CLOCK_DIV                   2
#else  //#if (MSPI_TIMING_FLASH_STR_MODE && (MSPI_TIMING_PSRAM_STR_MODE))
#define MSPI_TIMING_CORE_CLOCK_DIV                   1
#endif

///////////////////////////////////// FLASH CORE CLOCK /////////////////////////////////////
//FLASH 80M DTR
#if MSPI_TIMING_FLASH_DTR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      160
#endif

//FLASH 120M DTR
#if MSPI_TIMING_FLASH_DTR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      240
#endif

//FLASH 120M STR
#if MSPI_TIMING_FLASH_STR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_120M
#if (MSPI_TIMING_CORE_CLOCK_DIV == 2)
#define MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      240
#elif (MSPI_TIMING_CORE_CLOCK_DIV == 1)
#define MSPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      120
#endif
#endif  //FLASH 120M STR

///////////////////////////////////// PSRAM CORE CLOCK /////////////////////////////////////
//PSRAM 80M DTR
#if MSPI_TIMING_PSRAM_DTR_MODE && CONFIG_SPIRAM_SPEED_80M
#define MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      160
#endif

//PSRAM 120M STR
#if MSPI_TIMING_PSRAM_STR_MODE && CONFIG_SPIRAM_SPEED_120M
#if (MSPI_TIMING_CORE_CLOCK_DIV == 2)
#define MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      240
#elif (MSPI_TIMING_CORE_CLOCK_DIV == 1)
#define MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      120
#endif
#endif  //PSRAM 120M STR

//PSRAM 120M STR
#if MSPI_TIMING_PSRAM_DTR_MODE && CONFIG_SPIRAM_SPEED_120M
#define MSPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      240
#endif  //PSRAM 120M DTR


//------------------------------------------Determine the Core Clock-----------------------------------------------//
/**
 * @note
 * Limitation 1:
 * On 728, MSPI FLASH and PSRAM share the core clock register. Therefore,
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
#define MSPI_TIMING_CORE_CLOCK_MHZ   80
#endif

/**
 * @note
 * Limitation 2: DDR mode requires the core clock divider (core_clk / div = module_clk) to be power of 2.
 */
#define CHECK_POWER_OF_2(n)                         ((((n) & ((~(n)) + 1))) == (n))

#if MSPI_TIMING_FLASH_DTR_MODE
ESP_STATIC_ASSERT(CHECK_POWER_OF_2(MSPI_TIMING_CORE_CLOCK_MHZ / MSPI_TIMING_FLASH_MODULE_CLOCK), "FLASH and PSRAM Mode configuration are not supported");
#endif
#if MSPI_TIMING_PSRAM_DTR_MODE
ESP_STATIC_ASSERT(CHECK_POWER_OF_2(MSPI_TIMING_CORE_CLOCK_MHZ / MSPI_TIMING_PSRAM_MODULE_CLOCK), "FLASH and PSRAM Mode configuration are not supported");
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
//FLASH: core clock 160M, module clock 40M, DTR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_40M_DTR_MODE         {{1, 0, 0}, {0, 0, 0}, {2, 1, 1}, {2, 0, 1}, {2, 2, 2}, {2, 1, 2}, {1, 0, 1}, {0, 0, 1}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_40M_DTR_MODE           8
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_40M_DTR_MODE    2

//FLASH: core clock 160M, module clock 80M, DTR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE         {{0, 0, 0}, {4, 2, 2}, {2, 1, 2}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 2, 3}, {2, 1, 3}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 2, 4}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE           14
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE    1

//FLASH: core clock 240M, module clock 120M, DTR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE        {{0, 0, 0}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 1, 4}, {1, 0, 3}, {4, 0, 4}, {0, 0, 3}, {4, 1, 5}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE          14
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE   1

//FLASH: core clock 160M, module clock 80M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_80M_STR_MODE         {{1, 0, 0}, {0, 0, 0}, {2, 1, 1}, {2, 0, 1}, {2, 2, 2}, {2, 1, 2}, {1, 0, 1}, {0, 0, 1}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_80M_STR_MODE           8
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_80M_STR_MODE    2

//FLASH: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE        {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {1, 0, 1}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {1, 0, 2}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {1, 0, 3}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE   2

//FLASH: core clock 240M, module clock 120M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE        {{1, 0, 0}, {0, 0, 0}, {1, 1, 1}, {2, 3, 2}, {1, 0, 1}, {0, 0, 1}, {1, 1, 2}, {2, 3, 3}, {1, 0, 2}, {0, 0, 2}, {1, 1, 3}, {2, 3, 4}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE   2

//PSRAM: core clock 80M, module clock 40M, DTR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_80M_MODULE_CLK_40M_DTR_MODE          {{1, 0, 0}, {2, 1, 1}, {2, 0, 1}, {0, 0, 0}, {3, 1, 1}, {3, 0, 1}, {1, 0, 1}, {2, 1, 2}, {2, 0, 2}, {0, 0, 1}, {3, 1, 2}, {3, 0, 2}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_80M_MODULE_CLK_40M_DTR_MODE            12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_80M_MODULE_CLK_40M_DTR_MODE     4

//PSRAM: core clock 160M, module clock 80M, DTR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE         {{0, 0, 0}, {4, 2, 2}, {2, 1, 2}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 2, 3}, {2, 1, 3}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 2, 4}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE           14
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE    5

//PSRAM: core clock 240M, module clock 120M, STR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE        {{1, 0, 0}, {0, 0, 0}, {1, 1, 1}, {2, 3, 2}, {1, 0, 1}, {0, 0, 1}, {1, 1, 2}, {2, 3, 3}, {1, 0, 2}, {0, 0, 2}, {1, 1, 3}, {2, 3, 4}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE   2

//PSRAM: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE        {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {1, 0, 1}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {1, 0, 2}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {1, 0, 3}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE   2


//PSRAM: core clock 240M, module clock 120M, DTR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE        {{0, 0, 0}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 1, 4}, {1, 0, 3}, {4, 0, 4}, {0, 0, 3}, {4, 1, 5}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE          14
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE   1

//------------------------------------------Frequency Scanning Related-----------------------------------------------//
/**
 * On ESP32S3, only module clock 120M, DDR mode needs frequency scan. Frequency scanning is to get the max workable PLL
 * frequency under each successfull timing tuning configuration. PLL frequency may fluctuate under high temperature,
 * this method is to get the tuning configuration that can work under higher PLL frequency.
 */
#define MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MIN                                      440
#define MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX                                      600
#define MSPI_TIMING_PLL_FREQ_SCAN_THRESH_MHZ_LOW                                     448
#define MSPI_TIMING_PLL_FREQ_SCAN_THRESH_MHZ_HIGH                                    520
#define MSPI_TIMING_PLL_FREQ_SCAN_STEP_MHZ_MODULE_CLK_120M                           8
