/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains configuration APIs doing MSPI timing tuning by MSPI DQS
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#if SOC_MEMSPI_TIMING_TUNING_BY_DQS
#include "mspi_timing_tuning_configs.h"
#include "hal/mspi_timing_tuning_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MEMSPI_TIMING_TUNING_BY_DQS

#define IS_DDR    1
#define IS_SDR    (!IS_DDR)


/**
 * Delayline
 */
typedef struct {
    uint8_t data_delayline;
    uint8_t dqs_delayline;
} __attribute__((packed)) delayline_config_t;

/**
 * MSPI timing tuning configurations
 */
typedef struct {
    mspi_ll_dqs_phase_t phase[MSPI_LL_DQS_PHASE_MAX];
    delayline_config_t delayline_table[MSPI_TIMING_CONFIG_NUM_MAX];
    union {
        uint32_t available_config_num;
        uint32_t available_phase_num;
    };
} mspi_timing_config_t;

/*-------------------------------------------------------------------------------------------------
 * Timing Required APIs
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Init MSPI for PSRAM timing tuning
 *
 * @param[in] psram_freq_mhz  PSRAM frequency in MHz
 */
void mspi_timing_psram_init(uint32_t psram_freq_mhz);

/**
 * @brief Prepare reference data buffer
 */
void mspi_timing_config_psram_prepare_reference_data(uint8_t *buf, uint32_t len);

/**
 * @brief Configure PSRAM to write data via MSPI3
 *
 * @param[in] buf         buffer
 * @param[in] addr        address
 * @param[in] len         length
 */
void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len);

/**
 * @brief Configure PSRAM to read data via MSPI3
 *
 * @param[out] buf        buffer
 * @param[in] addr        address
 * @param[in] len         length
 */
void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len);

/**
 * @brief Get PSRAM tuning configurations for phase
 *
 * @param[out] config Pointer to PSRAM tuning configurations
 */
void mspi_timing_get_psram_tuning_phases(mspi_timing_config_t *configs);

/**
 * @brief Tune PSRAM timing registers for MSPI3 accessing PSRAM
 *
 * @param[in] configs Timing configs
 * @param[in] id      Config ID
 */
void mspi_timing_config_psram_set_tuning_phase(const void *configs, uint8_t id);

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
uint32_t mspi_timing_psram_select_best_tuning_phase(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr);

/**
 * @brief Set best PSRAM tuning configs.
 *        After this, calling `mspi_timing_enter_high_speed_mode` will set these configs correctly
 *
 * @param[in] configs  Timing tuning configs
 * @param[in] best_id  Best config ID
 */
void mspi_timing_psram_set_best_tuning_phase(const void *configs, uint8_t best_id);

/**
 * @brief Get PSRAM tuning configurations for delayline
 *
 * @param[out] config Pointer to PSRAM tuning configurations
 */
void mspi_timing_get_psram_tuning_delaylines(mspi_timing_config_t *configs);

/**
 * @brief Tune PSRAM timing registers for MSPI3 accessing PSRAM
 *
 * @param[in] configs Timing configs
 * @param[in] id      Config ID
 */
void mspi_timing_config_psram_set_tuning_delayline(const void *configs, uint8_t id);

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
uint32_t mspi_timing_psram_select_best_tuning_delayline(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr);

/**
 * @brief Set best PSRAM tuning configs.
 *        After this, calling `mspi_timing_enter_high_speed_mode` will set these configs correctly
 *
 * @param[in] configs  Timing tuning configs
 * @param[in] best_id  Best config ID
 */
void mspi_timing_psram_set_best_tuning_delayline(const void *configs, uint8_t best_id);

/*-------------------------------------------------------------------------------------------------
 * General Timing APIs
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Set PSRAM timing tuning settings
 *
 * This is used when the system is going to high speed mode / MSPI needs to be run in high speed
 *
 * @param[in] control_both_mspi  Control MSPI3 as well
 */
void mspi_timing_psram_config_set_tuning_regs(bool control_both_mspi);

/**
 * @brief Clear PSRAM timing tuning settings
 *
 * This is used when the system is going into low speed mode / MSPI doesn't need to be run in high speed
 *
 * @param[in] control_both_mspi  Control MSPI3 as well
 */
void mspi_timing_psram_config_clear_tuning_regs(bool control_both_mspi);

/**
 * @brief Set Flash timing tuning settings
 *
 * This is used when the system is going to high speed mode / MSPI needs to be run in high speed
 *
 * @param[in] control_both_mspi  Control MSPI1 as well
 */
void mspi_timing_flash_config_set_tuning_regs(bool control_both_mspi);

/**
 * @brief Clear Flash timing tuning settings
 *
 * This is used when the system is going into low speed mode / MSPI doesn't need to be run in high speed
 *
 * @param[in] control_both_mspi  Control MSPI1 as well
 */
void mspi_timing_flash_config_clear_tuning_regs(bool control_both_mspi);

#endif  //#if SOC_MEMSPI_TIMING_TUNING_BY_DQS

#ifdef __cplusplus
}
#endif
