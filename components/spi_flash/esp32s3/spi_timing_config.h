/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_flash_partitions.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "mspi_timing_tuning_configs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SPI_TIMING_CONFIG_NUM_DEFAULT               20  //This should be larger than the max available timing config num
#define SPI_TIMING_TEST_DATA_LEN                    64
#define SPI_TIMING_PSRAM_TEST_DATA_ADDR             0
#define SPI_TIMING_FLASH_TEST_DATA_ADDR             ESP_BOOTLOADER_OFFSET
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
 * SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ == SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ
 *
 * 2. DDR mode requires the core clock divider (core_clk / div = module_clk) to be power of 2.
 */
//--------------------------------------FLASH Sampling Mode --------------------------------------//
#define SPI_TIMING_FLASH_DTR_MODE                   CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
#define SPI_TIMING_FLASH_STR_MODE                   CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
//--------------------------------------FLASH Module Clock --------------------------------------//
#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define SPI_TIMING_FLASH_MODULE_CLOCK               20
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define SPI_TIMING_FLASH_MODULE_CLOCK               40
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define SPI_TIMING_FLASH_MODULE_CLOCK               80
#else //CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define SPI_TIMING_FLASH_MODULE_CLOCK               120
#endif
//------------------------------------FLASH Needs Tuning or not-------------------------------------//
#if SPI_TIMING_FLASH_DTR_MODE
#define SPI_TIMING_FLASH_NEEDS_TUNING               (SPI_TIMING_FLASH_MODULE_CLOCK > 40)
#elif SPI_TIMING_FLASH_STR_MODE
#define SPI_TIMING_FLASH_NEEDS_TUNING               (SPI_TIMING_FLASH_MODULE_CLOCK > 80)
#endif

//--------------------------------------PSRAM Sampling Mode --------------------------------------//
#define SPI_TIMING_PSRAM_DTR_MODE                   CONFIG_SPIRAM_MODE_OCT
#define SPI_TIMING_PSRAM_STR_MODE                   !CONFIG_SPIRAM_MODE_OCT
//--------------------------------------PSRAM Module Clock --------------------------------------//
#if CONFIG_SPIRAM
#if CONFIG_SPIRAM_SPEED_40M
#define SPI_TIMING_PSRAM_MODULE_CLOCK               40
#elif CONFIG_SPIRAM_SPEED_80M
#define SPI_TIMING_PSRAM_MODULE_CLOCK               80
#else //CONFIG_SPIRAM_SPEED_120M
#define SPI_TIMING_PSRAM_MODULE_CLOCK               120
#endif
#else   //Disable PSRAM
#define SPI_TIMING_PSRAM_MODULE_CLOCK               10      //Define this to 10MHz, because we rely on `SPI_TIMING_PSRAM_MODULE_CLOCK` macro for calculation and check below, see `Determine the Core Clock` chapter
#endif
//------------------------------------PSRAM Needs Tuning or not-------------------------------------//
#if SPI_TIMING_PSRAM_DTR_MODE
#define SPI_TIMING_PSRAM_NEEDS_TUNING               (SPI_TIMING_PSRAM_MODULE_CLOCK > 40)
#elif SPI_TIMING_PSRAM_STR_MODE
#define SPI_TIMING_PSRAM_NEEDS_TUNING               (SPI_TIMING_PSRAM_MODULE_CLOCK > 80)
#endif


/**
 * @note Define A feasible core clock below: SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ and SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ
 */
/**
 * Due to MSPI core clock is used by both MSPI Flash and PSRAM clock,
 * define the STR/DTR mode here for selecting the core clock:
 * @note If either Flash or PSRAM, or both of them are set to DTR mode, then we use DIV 2
 */
#if (SPI_TIMING_FLASH_DTR_MODE || SPI_TIMING_PSRAM_DTR_MODE)
#define SPI_TIMING_CORE_CLOCK_DIV                   2
#else  //#if (SPI_TIMING_FLASH_STR_MODE && (SPI_TIMING_PSRAM_STR_MODE))
#define SPI_TIMING_CORE_CLOCK_DIV                   1
#endif

///////////////////////////////////// FLASH CORE CLOCK /////////////////////////////////////
//FLASH 80M DTR
#if SPI_TIMING_FLASH_DTR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      160
#endif

//FLASH 120M DTR
#if SPI_TIMING_FLASH_DTR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      240
#endif

//FLASH 120M STR
#if SPI_TIMING_FLASH_STR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_120M
#if (SPI_TIMING_CORE_CLOCK_DIV == 2)
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      240
#elif (SPI_TIMING_CORE_CLOCK_DIV == 1)
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ      120
#endif
#endif  //FLASH 120M STR

///////////////////////////////////// PSRAM CORE CLOCK /////////////////////////////////////
//PSRAM 80M DTR
#if SPI_TIMING_PSRAM_DTR_MODE && CONFIG_SPIRAM_SPEED_80M
#define SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      160
#endif

//PSRAM 120M STR
#if SPI_TIMING_PSRAM_STR_MODE && CONFIG_SPIRAM_SPEED_120M
#if (SPI_TIMING_CORE_CLOCK_DIV == 2)
#define SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      240
#elif (SPI_TIMING_CORE_CLOCK_DIV == 1)
#define SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ      120
#endif
#endif  //PSRAM 120M STR


//------------------------------------------Determine the Core Clock-----------------------------------------------//
/**
 * @note
 * Limitation 1:
 * On 728, MSPI FLASH and PSRAM share the core clock register. Therefore,
 * the expected CORE CLOCK frequencies should be the same.
 */
#if SPI_TIMING_FLASH_NEEDS_TUNING && SPI_TIMING_PSRAM_NEEDS_TUNING
_Static_assert(SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ == SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ, "FLASH and PSRAM Mode configuration are not supported");
#define SPI_TIMING_CORE_CLOCK_MHZ                   SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ

//If only FLASH needs tuning, the core clock COULD be as FLASH expected
#elif SPI_TIMING_FLASH_NEEDS_TUNING && !SPI_TIMING_PSRAM_NEEDS_TUNING
_Static_assert(SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ % SPI_TIMING_PSRAM_MODULE_CLOCK == 0, "FLASH and PSRAM Mode configuration are not supported");
#define SPI_TIMING_CORE_CLOCK_MHZ                   SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ

//If only PSRAM needs tuning, the core clock COULD be as PSRAM expected
#elif !SPI_TIMING_FLASH_NEEDS_TUNING && SPI_TIMING_PSRAM_NEEDS_TUNING
_Static_assert(SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ % SPI_TIMING_FLASH_MODULE_CLOCK == 0, "FLASH and PSRAM Mode configuration are not supported");
#define SPI_TIMING_CORE_CLOCK_MHZ                   SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ

#else
#define SPI_TIMING_CORE_CLOCK_MHZ   80
#endif

/**
 * @note
 * Limitation 2: DDR mode requires the core clock divider (core_clk / div = module_clk) to be power of 2.
 */
#define CHECK_POWER_OF_2(n)                         ((((n) & ((~(n)) + 1))) == (n))

#if SPI_TIMING_FLASH_DTR_MODE
_Static_assert(CHECK_POWER_OF_2(SPI_TIMING_CORE_CLOCK_MHZ / SPI_TIMING_FLASH_MODULE_CLOCK), "FLASH and PSRAM Mode configuration are not supported");
#endif
#if SPI_TIMING_PSRAM_DTR_MODE
_Static_assert(CHECK_POWER_OF_2(SPI_TIMING_CORE_CLOCK_MHZ / SPI_TIMING_PSRAM_MODULE_CLOCK), "FLASH and PSRAM Mode configuration are not supported");
#endif


//------------------------------------------Helper Macros to get FLASH/PSRAM tuning configs-----------------------------------------------//
#define __GET_TUNING_CONFIG(type, core_clock, module_clock, mode) \
        (spi_timing_config_t) { .tuning_config_table = MSPI_TIMING_##type##_CONFIG_TABLE_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode, \
                                .available_config_num = MSPI_TIMING_##type##_CONFIG_NUM_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode, \
                                .default_config_id = MSPI_TIMING_##type##_DEFAULT_CONFIG_ID_CORE_CLK_##core_clock##M_MODULE_CLK_##module_clock##M_##mode }

#define _GET_TUNING_CONFIG(type, core_clock, module_clock, mode) __GET_TUNING_CONFIG(type, core_clock, module_clock, mode)

#define SPI_TIMING_FLASH_GET_TUNING_CONFIG(core_clock_mhz, module_clock_mhz, mode) _GET_TUNING_CONFIG(FLASH, core_clock_mhz, module_clock_mhz, mode)
#define SPI_TIMING_PSRAM_GET_TUNING_CONFIG(core_clock_mhz, module_clock_mhz, mode) _GET_TUNING_CONFIG(PSRAM, core_clock_mhz, module_clock_mhz, mode)


/**
 * SPI timing tuning registers. The macro `SPI_TIMING_FLASH_CONFIG_TABLE` below is the corresponding register value table.
 * Upper layer rely on these 3 registers to tune the timing.
 */
typedef struct {
    uint8_t spi_din_mode;    /*!< input signal delay mode*/
    uint8_t spi_din_num;     /*!< input signal delay number */
    uint8_t extra_dummy_len; /*!< extra dummy length*/
} spi_timing_tuning_param_t;

typedef struct {
    spi_timing_tuning_param_t tuning_config_table[SPI_TIMING_CONFIG_NUM_DEFAULT];   //available timing tuning configs
    uint32_t available_config_num;
    uint32_t default_config_id; //If tuning fails, we use this one as default
} spi_timing_config_t;

/**
 * The SPI FLASH module clock and SPI PSRAM module clock is divided from the SPI core clock, core clock is from system clock:
 *
 * PLL    ----|                      |---- FLASH Module Clock
 * XTAL   ----|----> Core Clock ---->|
 * RTC8M  ----|                      |---- PSRAM Module Clock
 *
 */
typedef enum {
    SPI_TIMING_CONFIG_CORE_CLOCK_80M,
    SPI_TIMING_CONFIG_CORE_CLOCK_120M,
    SPI_TIMING_CONFIG_CORE_CLOCK_160M,
    SPI_TIMING_CONFIG_CORE_CLOCK_240M
} spi_timing_config_core_clock_t;


spi_timing_config_core_clock_t spi_timing_config_get_core_clock(void);
void spi_timing_config_set_core_clock(uint8_t spi_num, spi_timing_config_core_clock_t core_clock);

void spi_timing_config_set_flash_clock(uint8_t spi_num, uint32_t freqdiv);
void spi_timing_config_flash_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num);
void spi_timing_config_flash_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy);
void spi_timing_config_flash_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len);

void spi_timing_config_set_psram_clock(uint8_t spi_num, uint32_t freqdiv);
void spi_timing_config_psram_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num);
void spi_timing_config_psram_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy);
void spi_timing_config_psram_write_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len);
void spi_timing_config_psram_read_data(uint8_t spi_num,uint8_t *buf, uint32_t addr, uint32_t len);

/*-------------------------------------------------------------------------------------------------
 * SPI1 Timing Tuning APIs
 * These APIs are only used in `spi_flash_timing_tuning.c/sweep_for_success_sample_points()` for
 * configuring SPI1 timing tuning related registers to find best tuning parameter
 *-------------------------------------------------------------------------------------------------*/
void spi_timing_config_flash_tune_din_num_mode(uint8_t din_mode, uint8_t din_num);
void spi_timing_config_flash_tune_dummy(uint8_t extra_dummy);
void spi_timing_config_psram_tune_din_num_mode(uint8_t din_mode, uint8_t din_num);
void spi_timing_config_psram_tune_dummy(uint8_t extra_dummy);

/**
 * SPI1 register info get APIs. These APIs inform `spi_flash_timing_tuning.c` (driver layer) of the SPI1 flash settings.
 * In this way, other components (e.g.: esp_flash driver) can get the info from it (`spi_flash_timing_tuning.c`).
 */
void spi_timing_config_get_cs_timing(uint8_t *setup_time, uint32_t *hold_time);
uint32_t spi_timing_config_get_flash_clock_reg(void);

#ifdef __cplusplus
}
#endif
