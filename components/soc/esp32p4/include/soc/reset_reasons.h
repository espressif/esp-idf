/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
    RESET_REASON_CHIP_POWER_ON    = 0x01, // Power on reset
    RESET_REASON_CORE_SW          = 0x03, // Software resets the digital core
    RESET_REASON_CORE_DEEP_SLEEP  = 0x05, // Deep sleep reset the digital core, check when doing sleep bringup if 0x5/0x6 is deepsleep wakeup TODO IDF-7529
    RESET_REASON_SYS_PMU_PWR_DOWN = 0x05, // PMU HP power down system reset
    RESET_REASON_CPU_PMU_PWR_DOWN = 0x06, // PMU HP power down CPU reset
    RESET_REASON_SYS_HP_WDT       = 0x07, // HP WDT resets system
    RESET_REASON_SYS_LP_WDT       = 0x09, // LP WDT resets system
    RESET_REASON_CORE_HP_WDT      = 0x0B, // HP WDT resets digital core
    RESET_REASON_CPU0_SW          = 0x0C, // Software resets CPU 0
    RESET_REASON_CORE_LP_WDT      = 0x0D, // LP WDT resets digital core
    RESET_REASON_SYS_BROWN_OUT    = 0x0F, // VDD voltage is not stable and resets the digital core
    RESET_REASON_CHIP_LP_WDT      = 0x10, // LP WDT resets chip
    RESET_REASON_SYS_SUPER_WDT    = 0x12, // Super watch dog resets the digital core and rtc module
    RESET_REASON_SYS_CLK_GLITCH   = 0x13, // Glitch on clock resets the digital core and rtc module
    RESET_REASON_CORE_EFUSE_CRC   = 0x14, // eFuse CRC error resets the digital core
    RESET_REASON_CORE_USB_JTAG    = 0x16, // USB Serial/JTAG controller's JTAG resets the digital core
    RESET_REASON_CORE_USB_UART    = 0x17, // USB Serial/JTAG controller's UART resets the digital core
    RESET_REASON_CPU_JTAG         = 0x18, // Glitch on power resets the digital core
} soc_reset_reason_t;


#ifdef __cplusplus
}
#endif
