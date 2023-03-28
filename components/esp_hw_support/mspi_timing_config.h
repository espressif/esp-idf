/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "mspi_timing_tuning_configs.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * SPI timing tuning registers.
 * Upper layer rely on these 3 registers to tune the timing.
 */
typedef struct {
    uint8_t spi_din_mode;    /*!< input signal delay mode*/
    uint8_t spi_din_num;     /*!< input signal delay number */
    uint8_t extra_dummy_len; /*!< extra dummy length*/
} mspi_timing_tuning_param_t;

typedef struct {
    mspi_timing_tuning_param_t tuning_config_table[MSPI_TIMING_CONFIG_NUM_DEFAULT];   //available timing tuning configs
    uint32_t available_config_num;
    uint32_t default_config_id; //If tuning fails, we use this one as default
} mspi_timing_config_t;

/**
 * The SPI FLASH module clock and SPI PSRAM module clock is divided from the SPI core clock, core clock is from system clock:
 *
 * PLL    ----|                      |---- FLASH Module Clock
 * XTAL   ----|----> Core Clock ---->|
 * RTC8M  ----|                      |---- PSRAM Module Clock
 *
 */
typedef enum {
    MSPI_TIMING_CONFIG_CORE_CLOCK_80M,
    MSPI_TIMING_CONFIG_CORE_CLOCK_120M,
    MSPI_TIMING_CONFIG_CORE_CLOCK_160M,
    MSPI_TIMING_CONFIG_CORE_CLOCK_240M
} mspi_timing_config_core_clock_t;


//-------------------------------------- Generic Config APIs --------------------------------------//
/**
 * @brief Get required core clock, under current sdkconfig (Flash / PSRAM mode, speed, etc.)
 */
mspi_timing_config_core_clock_t mspi_timing_config_get_core_clock(void);

/**
 * @brief Set MSPI core clock
 *
 * @param spi_num     SPI0 / 1
 * @param core_clock  core clock
 */
void mspi_timing_config_set_core_clock(uint8_t spi_num, mspi_timing_config_core_clock_t core_clock);

/**
 * @brief Set MSPI Flash module clock
 *
 * @param spi_num     SPI0 / 1
 * @param freqdiv     Freq divider
 */
void mspi_timing_config_set_flash_clock(uint8_t spi_num, uint32_t freqdiv);

/**
 * @brief Set MSPI Flash Din Mode and Din Num
 *
 * @param spi_num     SPI0 / 1
 * @param din_mode    Din mode
 * @param din_num     Din num
 */
void mspi_timing_config_flash_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num);

/**
 * @brief Set MSPI Flash extra dummy
 *
 * @param spi_num     SPI0 / 1
 * @param extra_dummy extra dummy
 */
void mspi_timing_config_flash_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy);

/**
 * @brief Configure Flash to read data via SPI1
 *
 * @param buf         buffer
 * @param addr        address
 * @param len         length
 */
void mspi_timing_config_flash_read_data(uint8_t *buf, uint32_t addr, uint32_t len);

/**
 * @brief Set MSPI PSRAM module clock
 *
 * @param spi_num     SPI0 / 1
 * @param freqdiv     Freq divider
 */
void mspi_timing_config_set_psram_clock(uint8_t spi_num, uint32_t freqdiv);

/**
 * @brief Set MSPI PSRAM Din Mode and Din Num
 *
 * @param spi_num     SPI0 / 1
 * @param din_mode    Din mode
 * @param din_num     Din num
 */
void mspi_timing_config_psram_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num);

/**
 * @brief Set MSPI PSRAM extra dummy
 *
 * @param spi_num     SPI0 / 1
 * @param extra_dummy extra dummy
 */
void mspi_timing_config_psram_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy);

/**
 * @brief Configure PSRAM to write data via SPI1
 *
 * @param buf         buffer
 * @param addr        address
 * @param len         length
 */
void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len);

/**
 * @brief Configure PSRAM to read data via SPI1
 *
 * @param buf         buffer
 * @param addr        address
 * @param len         length
 */
void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len);

/*-------------------------------------------------------------------------------------------------
 * SPI1 Timing Tuning APIs
 *
 * These APIs are only used in `mspi_timing_tuning.c` for configuring SPI1 timing
 * tuning related registers to find best tuning parameter for Flash and PSRAM
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Tune Flash timing registers for SPI1 accessing Flash
 *
 * @param[in] params Timing parameters
 */
void mspi_timing_config_flash_set_tuning_regs(const mspi_timing_tuning_param_t *params);

/**
 * @brief Tune PSRAM timing registers for SPI1 accessing PSRAM
 *
 * @param[in] params Timing parameters
 */
void mspi_timing_config_psram_set_tuning_regs(const mspi_timing_tuning_param_t *params);


/*-------------------------------------------------------------------------------------------------
 * APIs for coordination with ESP Flash driver
 *-------------------------------------------------------------------------------------------------*/
/**
 * SPI1 register info get APIs. These APIs inform `mspi_timing_tuning.c` (driver layer) of the SPI1 flash settings.
 * In this way, other components (e.g.: esp_flash driver) can get the info from it (`mspi_timing_tuning.c`).
 */

/**
 * @brief Get CS timing
 *
 * @param[out]  setup_time  Setup time
 * @param[out]  hold_time   Hold time
 */
void mspi_timing_config_get_cs_timing(uint8_t *setup_time, uint32_t *hold_time);

/**
 * @brief Get Flash clock reg val
 *
 * @return Flash clock reg val
 */
uint32_t mspi_timing_config_get_flash_clock_reg(void);

#ifdef __cplusplus
}
#endif
