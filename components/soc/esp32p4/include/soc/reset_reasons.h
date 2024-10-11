/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
    RESET_REASON_CHIP_POWER_ON     = 0x01, // Power on reset
    RESET_REASON_CORE_SW           = 0x03, // Software resets the digital core
    RESET_REASON_CORE_DEEP_SLEEP   = 0x05, // Deep sleep reset the digital core, check when doing sleep bringup
    RESET_REASON_CORE_PMU_PWR_DOWN = 0x05, // PMU HP power down core reset
    RESET_REASON_CORE_MWDT         = 0x07, // MWDT core reset
    RESET_REASON_CORE_RWDT         = 0x09, // RWDT core reset
    RESET_REASON_CPU_MWDT          = 0x0B, // MWDT HP CPU 0/1 reset
    RESET_REASON_CPU_SW            = 0x0C, // Software resets HP CPU 0/1
    RESET_REASON_CPU0_SW           = 0x0C, // Software resets HP CPU 0, kept to be compatible with older chips
    RESET_REASON_CPU_RWDT          = 0x0D, // RWDT resets digital core
    RESET_REASON_SYS_BROWN_OUT     = 0x0F, // VDD voltage is not stable and resets the digital core
    RESET_REASON_SYS_RWDT          = 0x10, // RWDT system reset
    RESET_REASON_SYS_SUPER_WDT     = 0x12, // Super watch dog resets the digital core and rtc module
    RESET_REASON_CORE_PWR_GLITCH   = 0x13, // Glitch on power resets the digital core and rtc module
    RESET_REASON_CORE_EFUSE_CRC    = 0x14, // eFuse CRC error resets the digital core
    RESET_REASON_CORE_USB_JTAG     = 0x16, // USB Serial/JTAG controller's JTAG resets the digital core
    RESET_REASON_CORE_USB_UART     = 0x17, // USB Serial/JTAG controller's UART resets the digital core
    RESET_REASON_CPU_JTAG          = 0x18, // Triggered when a reset command from JTAG is received
    RESET_REASON_CPU_LOCKUP        = 0x1A, // Triggered when the CPU enters lockup (exception inside the exception handler would cause this)
} soc_reset_reason_t;


#ifdef __cplusplus
}
#endif
