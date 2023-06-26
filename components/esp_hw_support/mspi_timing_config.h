/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_private/mspi_timing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------------------------------
 * Generic Config APIs
 *-------------------------------------------------------------------------------------------------*/
/**
 * @brief Set Flash module clock
 *
 * @param flash_freq_mhz     Flash clock frequency in MHz
 * @param speed_mode         Speed mode
 * @param control_both_mspi  Control SPI1 as well
 */
void mspi_timing_config_set_flash_clock(uint32_t flash_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi);

/**
 * @brief Set PSRAM module clock
 *
 * @param psram_freq_mhz     PSRAM clock frequency in MHz
 * @param speed_mode         Speed mode
 * @param control_both_mspi  Not used, for compatibility
 */
void mspi_timing_config_set_psram_clock(uint32_t psram_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi);

#ifdef __cplusplus
}
#endif
