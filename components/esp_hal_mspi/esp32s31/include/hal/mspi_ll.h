/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

/**
 * Background
 *
 * This file is for the MSPI related, but not Flash driver related registers, these registers:
 * - may influence both Flash and PSRAM
 * - not related or directly related to Flash controller driver
 *
 * Some hints for naming convention:
 * - For MSPI timing tuning related registers, the LL should start with `mspi_timing_ll_`
 * - For others, the LL should start with `mspi_ll_`
 */

#pragma once

#include <sys/param.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/spi_mem_struct.h"
#include "soc/spi_mem_s_struct.h"
#include "soc/spi_mem_c_reg.h"
#include "soc/spi1_mem_c_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_LL_PERIPH_NUM                            4
#define MSPI_TIMING_LL_MSPI_ID_0                      0
#define MSPI_TIMING_LL_MSPI_ID_1                      1

// PSRAM frequency should be constrained by AXI frequency to avoid FIFO underflow.
#define MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED     1

#define MSPI_LL_AXI_DISABLE_SUPPORTED                 1
#define MSPI_LL_INTR_EVENT_SUPPORTED                  1

// TODO: ["ESP32S31"] IDF-14653

/**
 * @brief Enable/Disable MSPI controller interrupt
 *
 * @param mspi_id     mspi_id
 * @param intr_mask   interrupt mask
 * @param enable      enable / disable
 */
__attribute__((always_inline))
static inline void mspi_ll_enable_intr(uint8_t spi_num, uint32_t intr_mask, bool enable)
{
    if (enable) {
        SPIMEM0.mem_int_ena.val |= intr_mask;
    } else {
        SPIMEM0.mem_int_ena.val &= ~intr_mask;
    }
}

/**
 * @brief Clear MSPI controller interrupt
 *
 * @param mspi_id     mspi_id
 * @param intr_mask   interrupt mask
 */
__attribute__((always_inline))
static inline void mspi_ll_clear_intr(uint8_t spi_num, uint32_t intr_mask)
{
    SPIMEM0.mem_int_clr.val = intr_mask;
}

/**
 * @brief Get MSPI controller interrupt raw
 *
 * @param mspi_id     mspi_id
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_get_intr_raw(uint8_t spi_num)
{
    return SPIMEM0.mem_int_raw.val;
}

/**
 * Enable AXI access to flash
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_ll_flash_enable_axi_access(uint8_t spi_num, bool enable)
{
    SPIMEM0.mem_cache_fctrl.close_axi_inf_en = !enable;
}

/**
 * Enable AXI access to PSRAM
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_ll_psram_enable_axi_access(uint8_t spi_num, bool enable)
{
    SPIMEM2.mem_cache_fctrl.close_axi_inf_en = !enable;
}

#ifdef __cplusplus
}
#endif
