/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

//+-----------------------------------------------Terminology---------------------------------------------+
//|                                                                                                       |
//| CPU Reset:    Reset CPU core only, once reset done, CPU will execute from reset vector                |
//|                                                                                                       |
//| Core Reset:   Reset the whole digital system except RTC sub-system                                    |
//|                                                                                                       |
//| System Reset: Reset the whole digital system, including RTC sub-system                                |
//|                                                                                                       |
//| Chip Reset:   Reset the whole chip, including the analog part                                         |
//|                                                                                                       |
//+-------------------------------------------------------------------------------------------------------+

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Naming conventions: RESET_REASON_{reset level}_{reset reason}
 * @note refer to TRM: <Reset and Clock> chapter
 */
typedef enum {
    RESET_REASON_CHIP_POWER_ON   = 0x01, // Power on reset
    RESET_REASON_CORE_SW         = 0x03, // Software resets the digital core
    RESET_REASON_CORE_DEEP_SLEEP = 0x05, // Deep sleep reset the digital core
    RESET_REASON_CORE_SDIO       = 0x06, // SDIO module resets the digital core
    RESET_REASON_CORE_MWDT0      = 0x07, // Main watch dog 0 resets digital core
    RESET_REASON_CORE_MWDT1      = 0x08, // Main watch dog 1 resets digital core
    RESET_REASON_CORE_RTC_WDT    = 0x09, // RTC watch dog resets digital core
    RESET_REASON_CPU0_MWDT0      = 0x0B, // Main watch dog 0 resets CPU 0
    RESET_REASON_CPU1_MWDT1      = 0x0B, // Main watch dog 1 resets CPU 1
    RESET_REASON_CPU0_SW         = 0x0C, // Software resets CPU 0
    RESET_REASON_CPU1_SW         = 0x0C, // Software resets CPU 1
    RESET_REASON_CPU0_RTC_WDT    = 0x0D, // RTC watch dog resets CPU 0
    RESET_REASON_CPU1_RTC_WDT    = 0x0D, // RTC watch dog resets CPU 1
    RESET_REASON_CPU1_CPU0       = 0x0E, // CPU0 resets CPU1 by DPORT_APPCPU_RESETTING
    RESET_REASON_SYS_BROWN_OUT   = 0x0F, // Reset when the VDD voltage is not stable
    RESET_REASON_SYS_RTC_WDT     = 0x10, // RTC watch dog resets digital core and rtc module
} soc_reset_reason_t;

/** Bitmasks of hardware reset codes belonging to each reset level. */
#define SOC_RESET_LEVEL_CPU_MASK     ((1U << RESET_REASON_CPU0_MWDT0) | \
                                      (1U << RESET_REASON_CPU0_SW) | \
                                      (1U << RESET_REASON_CPU0_RTC_WDT) | \
                                      (1U << RESET_REASON_CPU1_CPU0))
#define SOC_RESET_LEVEL_CORE_MASK    ((1U << RESET_REASON_CORE_SW) | \
                                      (1U << RESET_REASON_CORE_DEEP_SLEEP) | \
                                      (1U << RESET_REASON_CORE_SDIO) | \
                                      (1U << RESET_REASON_CORE_MWDT0) | \
                                      (1U << RESET_REASON_CORE_MWDT1) | \
                                      (1U << RESET_REASON_CORE_RTC_WDT))
#define SOC_RESET_LEVEL_SYSTEM_MASK  ((1U << RESET_REASON_SYS_BROWN_OUT) | \
                                      (1U << RESET_REASON_SYS_RTC_WDT))

#include "soc/reset_level.h"

#ifdef __cplusplus
}
#endif
