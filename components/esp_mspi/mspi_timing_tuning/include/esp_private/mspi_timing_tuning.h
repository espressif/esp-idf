/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 * This file is for MSPI timinig tuning private APIs
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Make MSPI work under 20Mhz, remove the timing tuning required delays.
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void mspi_timing_enter_low_speed_mode(bool control_spi1);

/**
 * @brief Get PSRAM frequency in low speed mode (MHz)
 * @return PSRAM frequency in MHz when in low speed mode
 */
uint32_t mspi_timing_get_psram_low_speed_freq_mhz(void);

/**
 * @brief Make MSPI work under the frequency as users set, may add certain delays to MSPI RX direction to meet timing requirements.
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void mspi_timing_enter_high_speed_mode(bool control_spi1);

/**
 * @brief Switch MSPI to low speed while suspending external memory cache to avoid in-flight cache line fills across the
 * clock change.
 *
 * @note Early init only. Not safe for general runtime use: does not coordinate with other cores or freeze cache.
 */
void mspi_timing_enter_low_speed_early(void);

/**
 * @brief Switch MSPI to high speed while suspending external memory cache.
 *
 * @note Same usage constraints as @ref mspi_timing_enter_low_speed_early.
 */
void mspi_timing_enter_high_speed_early(void);

/**
 * @brief Switch MSPI into low speed mode / high speed mode.
 * @note This API is cache safe, it will freeze both D$ and I$ and restore them after MSPI is switched
 * @note For some of the MSPI high frequency settings (e.g. 80M DDR mode Flash or PSRAM), timing tuning is required.
 *       Certain delays will be added to the MSPI RX direction. When CPU clock switches from PLL to XTAL, should call
 *       this API first to enter MSPI low speed mode to remove the delays, and vice versa.
 */
void mspi_timing_change_speed_mode_cache_safe(bool switch_down);

/**
 * @brief Tune MSPI flash timing to make it work under high frequency
 */
void mspi_timing_flash_tuning(void);

/**
 * @brief Select the psram sampling mode that the timing tuning applies to
 *
 * The core clock is shared with the flash and is settled before the psram chip is probed, but the
 * tuning itself has to drive the chip in the line mode it was actually found in. Callers that build
 * in more than one psram line mode must call this after probing and before `mspi_timing_psram_tuning`.
 *
 * @param is_dtr  true for a DTR sampled chip (octal / hex), false for an STR sampled one (quad)
 */
void mspi_timing_psram_set_dtr_mode(bool is_dtr);

/**
 * @brief Get the psram sampling mode selected by `mspi_timing_psram_set_dtr_mode`
 *
 * @return true if the psram is sampled in DTR mode
 */
bool mspi_timing_psram_is_dtr_mode(void);

/**
 * @brief Whether the psram sampling mode selected by `mspi_timing_psram_set_dtr_mode` needs tuning
 *
 * @return true if `mspi_timing_psram_tuning` will do any work
 */
bool mspi_timing_psram_needs_tuning(void);

/**
 * @brief Tune MSPI psram timing to make it work under high frequency
 */
void mspi_timing_psram_tuning(void);

/**
 * @brief Set MSPI pin default pin drive
 */
void mspi_timing_set_pin_drive_strength(void);

#ifdef __cplusplus
}
#endif
