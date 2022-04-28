/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
    RESET_REASON_CORE_SW         = 0x03, // Software resets the digital core by RTC_CNTL_SW_SYS_RST
    RESET_REASON_CORE_DEEP_SLEEP = 0x05, // Deep sleep reset the digital core
    RESET_REASON_CORE_MWDT0      = 0x07, // Main watch dog 0 resets digital core
    RESET_REASON_CORE_RTC_WDT    = 0x09, // RTC watch dog resets digital core
    RESET_REASON_CPU0_MWDT0      = 0x0B, // Main watch dog 0 resets CPU 0
    RESET_REASON_CPU0_SW         = 0x0C, // Software resets CPU 0 by RTC_CNTL_SW_PROCPU_RST
    RESET_REASON_CPU0_RTC_WDT    = 0x0D, // RTC watch dog resets CPU 0
    RESET_REASON_SYS_BROWN_OUT   = 0x0F, // VDD voltage is not stable and resets the digital core
    RESET_REASON_SYS_RTC_WDT     = 0x10, // RTC watch dog resets digital core and rtc module
    RESET_REASON_SYS_SUPER_WDT   = 0x12, // Super watch dog resets the digital core and rtc module
    RESET_REASON_SYS_CLK_GLITCH  = 0x13, // Glitch on clock resets the digital core and rtc module
    RESET_REASON_CORE_EFUSE_CRC  = 0x14, // eFuse CRC error resets the digital core
    RESET_REASON_CPU0_JTAG       = 0x18, // JTAG resets the CPU 0
} soc_reset_reason_t;


#ifdef __cplusplus
}
#endif
