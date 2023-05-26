/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains configuration APIs doing MSPI timing tuning by MSPI delay
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
#include "mspi_timing_tuning_configs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY

#define IS_DDR    1
#define IS_SDR    (!IS_DDR)

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
    mspi_timing_tuning_param_t tuning_config_table[MSPI_TIMING_CONFIG_NUM_DEFAULT];   // Available timing tuning configs
    uint32_t available_config_num;                                                    // Available timing tuning config numbers
    uint32_t default_config_id;                                                       // If tuning fails, we use this one as default
} mspi_timing_config_t;


#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
/*-------------------------------------------------------------------------------------------------
 * Timing Required APIs
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Get Flash tuning all configurations
 *
 * @param[out] config Pointer to Flash tuning configurations
 */
void mspi_timing_get_flash_tuning_configs(mspi_timing_config_t *config);

/**
 * @brief Init MSPI for Flash timing tuning
 *
 * @param[in] flash_freq_mhz  Flash frequency in MHz
 */
void mspi_timing_flash_init(uint32_t flash_freq_mhz);

/**
 * @brief Tune Flash timing registers for SPI1 accessing Flash
 *
 * @param[in] params Timing parameters
 */
void mspi_timing_config_flash_set_tuning_regs(const void *timing_params);

/**
 * @brief Configure Flash to read data via SPI1
 *
 * @param[out] buf        buffer
 * @param[in] addr        address
 * @param[in] len         length
 */
void mspi_timing_config_flash_read_data(uint8_t *buf, uint32_t addr, uint32_t len);

/**
 * @brief Get PSRAM tuning all configurations
 *
 * @param[out] config Pointer to PSRAM tuning configurations
 */
void mspi_timing_get_psram_tuning_configs(mspi_timing_config_t *config);

/**
 * @brief Init MSPI for PSRAM timing tuning
 *
 * @param[in] psram_freq_mhz  PSRAM frequency in MHz
 */
void mspi_timing_psram_init(uint32_t psram_freq_mhz);

/**
 * @brief Tune PSRAM timing registers for SPI1 accessing PSRAM
 *
 * @param[in] params Timing parameters
 */
void mspi_timing_config_psram_set_tuning_regs(const void *timing_params);

/**
 * @brief Configure PSRAM to write data via SPI1
 *
 * @param[in] buf         buffer
 * @param[in] addr        address
 * @param[in] len         length
 */
void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len);

/**
 * @brief Configure PSRAM to read data via SPI1
 *
 * @param[out] buf        buffer
 * @param[in] addr        address
 * @param[in] len         length
 */
void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len);


/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Selection
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Select Flash best tuning configuration
 *
 * @param[in] configs             Timing tuning configuration table
 * @param[in] consecutive_length  Length of the consecutive successful sample results
 * @param[in] end                 End of the consecutive successful sample results
 * @param[in] reference_data      Reference data
 * @param[in] is_ddr              DDR or SDR
 *
 * @return  Best config ID
 */
uint32_t mspi_timing_flash_select_best_tuning_config(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr);

/**
 * @brief Set best Flash tuning configs.
 *        After this, calling `mspi_timing_enter_high_speed_mode` will set these configs correctly
 *
 * @param[in] timing_params  Timing tuning parameters
 */
void mspi_timing_flash_set_best_tuning_config(const void *timing_params);

/**
 * @brief Select PSRAM best tuning configuration
 *
 * @param[in] configs             Timing tuning configuration table
 * @param[in] consecutive_length  Length of the consecutive successful sample results
 * @param[in] end                 End of the consecutive successful sample results
 * @param[in] reference_data      Reference data
 * @param[in] is_ddr              DDR or SDR
 *
 * @return  Best config ID
 */
uint32_t mspi_timing_psram_select_best_tuning_config(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr);

/**
 * @brief Set best PSRAM tuning configs.
 *        After this, calling `mspi_timing_enter_high_speed_mode` will set these configs correctly
 *
 * @param[in] timing_params  Timing tuning parameters
 */
void mspi_timing_psram_set_best_tuning_config(const void *timing_params);


/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Clear / Set
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Clear Flash timing tuning settings
 *
 * This is used when the system is going into low speed mode / MSPI doesn't need to be run in high speed
 *
 * @param[in] control_both_mspi  Control SPI1 as well
 */
void mspi_timing_flash_config_clear_tuning_regs(bool control_both_mspi);

/**
 * @brief Set Flash timing tuning settings
 *
 * This is used when the system is going to high speed mode / MSPI needs to be run in high speed
 *
 * @param[in] control_both_mspi  Control SPI1 as well
 */
void mspi_timing_flash_config_set_tuning_regs(bool control_both_mspi);

/**
 * @brief Clear PSRAM timing tuning settings
 *
 * This is used when the system is going into low speed mode / MSPI doesn't need to be run in high speed
 *
 * @param[in] control_both_mspi  Control SPI1 as well
 */
void mspi_timing_psram_config_clear_tuning_regs(bool control_both_mspi);

/**
 * @brief Set PSRAM timing tuning settings
 *
 * This is used when the system is going to high speed mode / MSPI needs to be run in high speed
 *
 * @param[in] control_both_mspi  Control SPI1 as well
 */
void mspi_timing_psram_config_set_tuning_regs(bool control_both_mspi);


/*-------------------------------------------------------------------------------------------------
 * APIs for coordination with ESP Flash driver
 *-------------------------------------------------------------------------------------------------*/
/**
 * SPI1 register info get APIs. These APIs inform `spi_flash_timing_tuning.c` (driver layer) of the SPI1 flash settings.
 * In this way, other components (e.g.: esp_flash driver) can get the info from it (`spi_flash_timing_tuning.c`).
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

/**
 * @brief Get Flash extra dummy len
 *
 * @return Flash extra dummy
 */
uint8_t mspi_timing_config_get_flash_extra_dummy(void);
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING

#endif  //#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY

#ifdef __cplusplus
}
#endif
