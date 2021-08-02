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

//-------------------------------------------FLASH Operation Mode and Corresponding Timing Tuning Parameter Table --------------------------------------//
#define SPI_TIMING_FLASH_DTR_MODE         (CONFIG_ESPTOOLPY_FLASHMODE_OPI_DTR || CONFIG_ESPTOOLPY_FLASHMODE_OIO_DTR)
#define SPI_TIMING_FLASH_STR_MODE         (CONFIG_ESPTOOLPY_FLASHMODE_OPI_STR || CONFIG_ESPTOOLPY_FLASHMODE_OIO_STR || CONFIG_ESPTOOLPY_FLASHMODE_OOUT_STR)

/* Determine A feasible core clock below: SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ and SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ*/
//OCTAL FLASH
#if CONFIG_ESPTOOLPY_OCT_FLASH

//OCT FLASH 80M DTR
#if SPI_TIMING_FLASH_DTR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ                        160
#endif

//OCT FLASH 120M DTR
#if SPI_TIMING_FLASH_DTR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ                        240
#endif

//OCT FLASH 120M STR
#if SPI_TIMING_FLASH_STR_MODE && CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ                        120
#endif

#endif  //#if CONFIG_ESPTOOLPY_OCT_FLASH
/* QUAD FLASH Operation Mode should be added here if needed */


//---------------------------------------PSRAM Operation Mode and Corresponding Timing Tuning Parameter Table--------------------------------------//
#define SPI_TIMING_PSRAM_DTR_MODE       1   //Currently we only support DTR Octal PSRAM
#define SPI_TIMING_PSRAM_STR_MODE       0

//OCTAL PSRAM
#if CONFIG_SPIRAM_MODE_OCT

//OCT 80M PSRAM
#if SPI_TIMING_PSRAM_DTR_MODE && CONFIG_SPIRAM_SPEED_80M
#define SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ                        160
#endif

#endif  //#if CONFIG_SPIRAM_MODE_OCT
/* QUAD PSRAM Operation Mode should be added here if needed */


//------------------------------------------Get the timing tuning config-----------------------------------------------//
#ifdef SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ
//FLASH needs tuning, and it expects this core clock: SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ
#define SPI_TIMING_FLASH_NEEDS_TUNING   1
#endif

#ifdef SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ
//PSRAM needs tuning, and it expects this core clock: SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ
#define SPI_TIMING_PSRAM_NEEDS_TUNING   1
#endif

//If both FLASH and PSRAM need tuning, the core clock should be same
#if SPI_TIMING_FLASH_NEEDS_TUNING && SPI_TIMING_PSRAM_NEEDS_TUNING
_Static_assert(SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ == SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ, "FLASH and PSRAM Mode configuration are not supported");
#define SPI_TIMING_CORE_CLOCK_MHZ   SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ

//If only FLASH needs tuning, the core clock could be as FLASH expected
#elif SPI_TIMING_FLASH_NEEDS_TUNING && !SPI_TIMING_PSRAM_NEEDS_TUNING
#define SPI_TIMING_CORE_CLOCK_MHZ   SPI_TIMING_FLASH_EXPECTED_CORE_CLK_MHZ

//If only PSRAM needs tuning, the core clock could be as PSRAM expected
#elif !SPI_TIMING_FLASH_NEEDS_TUNING && SPI_TIMING_PSRAM_NEEDS_TUNING
#define SPI_TIMING_CORE_CLOCK_MHZ   SPI_TIMING_PSRAM_EXPECTED_CORE_CLK_MHZ

#else
#define SPI_TIMING_CORE_CLOCK_MHZ   80
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

#ifdef __cplusplus
}
#endif
