/*
 * SPDX-FileCopyrightText: 2010-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once


#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>
#include "esp_assert.h"

#include "soc/soc.h"
#include "soc/lp_system_reg.h"
#include "soc/reset_reasons.h"

/** \defgroup rtc_apis, rtc registers and memory related apis
  * @brief rtc apis
  */

/** @addtogroup rtc_apis
  * @{
  */

/**************************************************************************************
  *                                       Note:                                       *
  *       Some Rtc memory and registers are used, in ROM or in internal library.      *
  *          Please do not use reserved or used rtc memory or registers.              *
  *                                                                                   *
  *************************************************************************************
  *                          RTC  Memory & Store Register usage
  *************************************************************************************
  *     rtc memory addr         type    size            usage
  *     0x2E000000              Fast    0x8000          lp core code/deep sleep entry code
  *************************************************************************************
  *     RTC store registers     usage
  *     RTC_CNTL_STORE0_REG     RESERVED
  *     RTC_CNTL_STORE1_REG     RTC_SLOW_CLK calibration value
  *     RTC_CNTL_STORE2_REG     Boot time, low word
  *     RTC_CNTL_STORE3_REG     Boot time, high word
  *     RTC_CNTL_STORE4_REG     External XTAL frequency
  *     RTC_CNTL_STORE5_REG     FAST_RTC_MEMORY_LENGTH
  *     RTC_CNTL_STORE6_REG     FAST_RTC_MEMORY_ENTRY
  *     RTC_CNTL_STORE7_REG     FAST_RTC_MEMORY_CRC
  *************************************************************************************
  */

#define RTC_SLOW_CLK_CAL_REG    LP_SYSTEM_REG_LP_STORE1_REG
#define RTC_BOOT_TIME_LOW_REG   LP_SYSTEM_REG_LP_STORE2_REG
#define RTC_BOOT_TIME_HIGH_REG  LP_SYSTEM_REG_LP_STORE3_REG
#define RTC_XTAL_FREQ_REG       LP_SYSTEM_REG_LP_STORE4_REG
#define RTC_ENTRY_LENGTH_REG    LP_SYSTEM_REG_LP_STORE5_REG
#define RTC_ENTRY_ADDR_REG      LP_SYSTEM_REG_LP_STORE6_REG
#define RTC_RESET_CAUSE_REG     LP_SYSTEM_REG_LP_STORE6_REG
#define RTC_MEMORY_CRC_REG      LP_SYSTEM_REG_LP_STORE7_REG

// light sleep
/* use LP_SYS_LP_STORE8_REG to store light sleep wake stub addr and sleep mode for dualcore
 *
 * bit[31:2] wake restore func addr
 * bit[0] sleep mode:
 *    0 -- hp light sleep
 *    1 -- hp deep sleep
 */
/* this MACRO is common to hpcore and lpcore
 * they use different bitmask to check same value
 */
#define RTC_SLEEP_WAKE_STUB_ADDR_REG  LP_SYSTEM_REG_LP_STORE8_REG
#define RTC_SLEEP_MODE_REG            LP_SYSTEM_REG_LP_STORE8_REG

#define ROM_LOG_CTRL_REG              LP_SYSTEM_REG_LP_STORE4_REG
#define RTC_DISABLE_ROM_LOG           ((1 << 0) | (1 << 16)) //!< Disable logging from the ROM code.



typedef enum {
    AWAKE = 0,             //<CPU ON
    LIGHT_SLEEP = BIT0,    //CPU waiti, PLL ON.  We don't need explicitly set this mode.
    DEEP_SLEEP  = BIT1     //CPU OFF, PLL OFF, only specific timer could wake up
} SLEEP_MODE;

typedef enum {
    SW_CLR = 0,
    POWERON_RESET = 1,          /**<1, Vbat power on reset*/
    SW_SYS_RESET = 3,           /**<3, Software reset digital core*/
    PMU_SYS_PWR_DOWN_RESET = 5, /**<5, PMU HP system power down reset*/
    PMU_CPU_PWR_DOWN_RESET = 6, /**<6, PMU CPU system power down reset*/
    HP_SYS_HP_WDT0_RESET = 7,   /**<7, HP system reset from HP watchdog0*/
    HP_SYS_HP_WDT1_RESET = 8,   /**<8, HP system reset from HP watchdog1*/
    HP_SYS_LP_WDT_RESET = 9,    /**<9, HP system reset from LP watchdog*/
    HP_CORE_HP_WDT_RESET = 11,  /**<11, HP core reset from HP watchdog*/
    SW_CPU_RESET = 12,          /**<12, software reset cpu*/
    HP_CORE_LP_WDT_RESET = 13,  /**<13, HP core reset from LP watchdog*/
    BROWN_OUT_RESET = 15,       /**<15, Reset when the vdd voltage is not stable*/
    CHIP_LP_WDT_RESET = 16,     /**<16, LP watchdog chip reset*/
    SUPER_WDT_RESET = 18,       /**<18, super watchdog reset*/
    GLITCH_RTC_RESET = 19,      /**<19, glitch reset*/
    EFUSE_CRC_ERR_RESET = 20,   /**<20, efuse ecc error reset*/
    CHIP_USB_JTAG_RESET = 22,   /**<22, HP usb jtag chip reset*/
    CHIP_USB_UART_RESET = 23,   /**<23, HP usb uart chip reset*/
    JTAG_RESET = 24,            /**<24, jtag reset*/
    CPU_LOCKUP_RESET = 26,      /**<26, cpu lockup reset*/
} RESET_REASON;

// Check if the reset reason defined in ROM is compatible with soc/reset_reasons.h
ESP_STATIC_ASSERT((soc_reset_reason_t)POWERON_RESET           == RESET_REASON_CHIP_POWER_ON,      "POWERON_RESET != RESET_REASON_CHIP_POWER_ON");
ESP_STATIC_ASSERT((soc_reset_reason_t)SW_SYS_RESET            == RESET_REASON_CORE_SW,            "SW_SYS_RESET != RESET_REASON_CORE_SW");
ESP_STATIC_ASSERT((soc_reset_reason_t)PMU_SYS_PWR_DOWN_RESET  == RESET_REASON_CORE_DEEP_SLEEP,    "PMU_SYS_PWR_DOWN_RESET != RESET_REASON_CORE_DEEP_SLEEP");
ESP_STATIC_ASSERT((soc_reset_reason_t)PMU_SYS_PWR_DOWN_RESET  == RESET_REASON_CORE_PMU_PWR_DOWN,  "PMU_SYS_PWR_DOWN_RESET != RESET_REASON_CORE_PMU_PWR_DOWN");
ESP_STATIC_ASSERT((soc_reset_reason_t)PMU_CPU_PWR_DOWN_RESET  == RESET_REASON_CPU_PMU_PWR_DOWN,   "PMU_CPU_PWR_DOWN_RESET != RESET_REASON_CPU_PMU_PWR_DOWN");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_SYS_HP_WDT0_RESET    == RESET_REASON_CORE_MWDT0,         "HP_SYS_HP_WDT0_RESET != RESET_REASON_CORE_MWDT0");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_SYS_HP_WDT1_RESET    == RESET_REASON_CORE_MWDT1,         "HP_SYS_HP_WDT1_RESET != RESET_REASON_CORE_MWDT1");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_SYS_LP_WDT_RESET     == RESET_REASON_CORE_RWDT,          "HP_SYS_LP_WDT_RESET != RESET_REASON_CORE_RWDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_CORE_HP_WDT_RESET    == RESET_REASON_CPU_MWDT,           "HP_CORE_HP_WDT_RESET != RESET_REASON_CPU_MWDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)SW_CPU_RESET            == RESET_REASON_CPU_SW,             "SW_CPU_RESET != RESET_REASON_CPU_SW");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_CORE_LP_WDT_RESET    == RESET_REASON_CPU_RWDT,           "HP_CORE_LP_WDT_RESET != RESET_REASON_CPU_RWDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)BROWN_OUT_RESET         == RESET_REASON_SYS_BROWN_OUT,      "BROWN_OUT_RESET != RESET_REASON_SYS_BROWN_OUT");
ESP_STATIC_ASSERT((soc_reset_reason_t)CHIP_LP_WDT_RESET       == RESET_REASON_SYS_RWDT,           "CHIP_LP_WDT_RESET != RESET_REASON_SYS_RWDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)SUPER_WDT_RESET         == RESET_REASON_SYS_SUPER_WDT,      "SUPER_WDT_RESET != RESET_REASON_SYS_SUPER_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)GLITCH_RTC_RESET        == RESET_REASON_CORE_PWR_GLITCH,    "GLITCH_RTC_RESET != RESET_REASON_CORE_PWR_GLITCH");
ESP_STATIC_ASSERT((soc_reset_reason_t)EFUSE_CRC_ERR_RESET     == RESET_REASON_CORE_EFUSE_CRC,     "EFUSE_CRC_ERR_RESET != RESET_REASON_CORE_EFUSE_CRC");
ESP_STATIC_ASSERT((soc_reset_reason_t)CHIP_USB_JTAG_RESET     == RESET_REASON_CORE_USB_JTAG,      "CHIP_USB_JTAG_RESET != RESET_REASON_CORE_USB_JTAG");
ESP_STATIC_ASSERT((soc_reset_reason_t)CHIP_USB_UART_RESET     == RESET_REASON_CORE_USB_UART,      "CHIP_USB_UART_RESET != RESET_REASON_CORE_USB_UART");
ESP_STATIC_ASSERT((soc_reset_reason_t)JTAG_RESET              == RESET_REASON_CPU_JTAG,           "JTAG_RESET != RESET_REASON_CPU_JTAG");
ESP_STATIC_ASSERT((soc_reset_reason_t)CPU_LOCKUP_RESET        == RESET_REASON_CPU_LOCKUP,         "CPU_LOCKUP_RESET != RESET_REASON_CPU_LOCKUP");

/**
  * @brief  Get the reset reason for CPU.
  *
  * @param  int cpu_no : CPU no.
  *
  * @return RESET_REASON
  */
RESET_REASON rtc_get_reset_reason(int cpu_no);

/**
  * @brief Suppress ROM log by setting specific RTC control register.
  * @note This is not a permanent disable of ROM logging since the RTC register can not retain after chip reset.
  *
  * @param  None
  *
  * @return None
  */
static inline void rtc_suppress_rom_log(void)
{
    /* To disable logging in the ROM, only the least significant bit of the register is used,
     * but since this register is also used to store the frequency of the main crystal (RTC_XTAL_FREQ_REG),
     * you need to write to this register in the same format.
     * Namely, the upper 16 bits and lower should be the same.
     */
    REG_SET_BIT(LP_SYSTEM_REG_LP_STORE4_REG, RTC_DISABLE_ROM_LOG);
}

/**
  * @brief Software Reset digital core.
  *
  * It is not recommended to use this function in esp-idf, use
  * esp_restart() instead.
  *
  * @param  None
  *
  * @return None
  */
void software_reset(void);

/**
  * @brief Software Reset digital core.
  *
  * It is not recommended to use this function in esp-idf, use
  * esp_restart() instead.
  *
  * @param  int cpu_no : The CPU to reset, 0 for PRO CPU, 1 for APP CPU.
  *
  * @return None
  */
void software_reset_cpu(int cpu_no);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
