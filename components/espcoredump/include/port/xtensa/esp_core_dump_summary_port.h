/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "sdkconfig.h"
#include <stdint.h>
#include <xtensa/config/core-isa.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

#define EPCx_REGISTER_COUNT XCHAL_NUM_INTLEVELS

/**
 * @brief Backtrace information.
 *
 * For Xtensa, backtrace can be generated on device due to windowed register ABI.
 */
typedef struct {
    uint32_t bt[16];        /*!< Backtrace (array of PC) */
    uint32_t depth;         /*!< Number of backtrace entries */
    bool corrupted;         /*!< Status flag for backtrace is corrupt or not */
} esp_core_dump_bt_info_t;

/**
 * @brief Xtensa architecture specific extra information
 */
typedef struct {
    uint32_t exc_cause;                 /*!< Cause of exception */
    uint32_t exc_vaddr;                 /*!< Virtual address of exception */
    uint32_t exc_a[16];                 /*!< a register set when the exception caused */
    uint32_t epcx[EPCx_REGISTER_COUNT]; /*!< PC register address at exception level(1 to 7) */
    uint8_t epcx_reg_bits;              /*!< Bit mask of available EPCx registers */
} esp_core_dump_summary_extra_info_t;

#endif /* CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

#ifdef __cplusplus
}
#endif
