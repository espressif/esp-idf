/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_assert.h"

#include "soc/soc.h"
#include "soc/lp_system_reg.h"
#include "soc/reset_reasons.h"

#ifdef __cplusplus
extern "C" {
#endif

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
  *                          LP  Memory & Store Register usage
  *************************************************************************************
  *     rtc memory addr         type    size            usage
  *     0x3f421000(0x50000000)  Slow    SIZE_CP         Co-Processor code/Reset Entry
  *     0x3f421000+SIZE_CP      Slow    8192-SIZE_CP
  *
  *     0x3ff80000(0x40070000)  Fast    8192            deep sleep entry code
  *
  *************************************************************************************
  *     RTC store registers     usage
  *     RTC_CNTL_STORE0_REG     Reserved
  *     RTC_CNTL_STORE1_REG     RTC_SLOW_CLK calibration value
  *     RTC_CNTL_STORE2_REG     Boot time, low word
  *     RTC_CNTL_STORE3_REG     Boot time, high word
  *     RTC_CNTL_STORE4_REG     External XTAL frequency
  *     RTC_CNTL_STORE5_REG     APB bus frequency
  *     RTC_CNTL_STORE6_REG     FAST_RTC_MEMORY_ENTRY
  *     RTC_CNTL_STORE7_REG     FAST_RTC_MEMORY_CRC
  *************************************************************************************
  */

#define RTC_SLOW_CLK_CAL_REG    LP_SYSTEM_REG_LP_STORE1_REG
#define RTC_BOOT_TIME_LOW_REG   LP_SYSTEM_REG_LP_STORE2_REG
#define RTC_BOOT_TIME_HIGH_REG  LP_SYSTEM_REG_LP_STORE3_REG
#define RTC_XTAL_FREQ_REG       LP_SYSTEM_REG_LP_STORE4_REG
#define RTC_APB_FREQ_REG        LP_SYSTEM_REG_LP_STORE5_REG
#define RTC_ENTRY_ADDR_REG      LP_SYSTEM_REG_LP_STORE6_REG
#define RTC_RESET_CAUSE_REG     LP_SYSTEM_REG_LP_STORE6_REG
#define RTC_MEMORY_CRC_REG      LP_SYSTEM_REG_LP_STORE7_REG

#define RTC_DISABLE_ROM_LOG ((1 << 0) | (1 << 16)) //!< Disable logging from the ROM code.

typedef enum {
    AWAKE = 0,             //<CPU ON
    LIGHT_SLEEP = BIT0,    //CPU waiti, PLL ON.  We don't need explicitly set this mode.
    DEEP_SLEEP  = BIT1     //CPU OFF, PLL OFF, only specific timer could wake up
} SLEEP_MODE;

typedef enum {
    NO_MEAN = 0,
    POWERON_RESET = 1,          /**<1, Vbat power on reset*/
    SW_SYS_RESET = 3,           /**<3, Software reset digital core*/
    PMU_SYS_PWR_DOWN_RESET = 5, /**<5, PMU HP system power down reset*/
    PMU_CPU_PWR_DOWN_RESET = 6, /**<6, PMU HP CPU power down reset*/
    HP_SYS_HP_WDT_RESET = 7,    /**<7, HP system reset from HP watchdog*/
    HP_SYS_LP_WDT_RESET = 9,    /**<9, HP system reset from LP watchdog*/
    HP_CORE_HP_WDT_RESET = 11,  /**<11, HP core reset from HP watchdog*/
    HP_CORE_SYS_RESET = 12,     /**<12, HP core software reset*/
    HP_CORE_LP_SYS_RESET = 13,  /**<13, HP core reset from LP watchdog*/
    BROWN_OUT_RESET = 15,       /**<15, Reset when the vdd voltage is not stable*/
    LP_WDT_CHIP_RESET = 16,     /**<16, Reset chip when LP watchdog trigger*/
    SUPER_WDT_RESET = 18,       /**<18, super watchdog reset*/
    GLITCH_RTC_RESET = 19,      /**<19, glitch reset*/
    EFUSE_CRC_ERR_RESET = 20,   /**<20, efuse ecc error reset*/
    HP_SDIO_RESET = 21,         /**<21, hp sdio chip reset*/
    HP_USB_JTAG_RESET = 22,     /**<22, hp usb jtag reset*/
    HP_USB_UART_RESET = 23,     /**<23, hp usb uart reset*/
    JTAG_RESET = 24,            /**<24, jtag reset*/
} RESET_REASON;

// Check if the reset reason defined in ROM is compatible with soc/reset_reasons.h
ESP_STATIC_ASSERT((soc_reset_reason_t)POWERON_RESET == RESET_REASON_CHIP_POWER_ON, "POWERON_RESET != RESET_REASON_CHIP_POWER_ON");
ESP_STATIC_ASSERT((soc_reset_reason_t)SW_SYS_RESET == RESET_REASON_CORE_SW, "SW_SYS_RESET != RESET_REASON_CORE_SW");
ESP_STATIC_ASSERT((soc_reset_reason_t)PMU_SYS_PWR_DOWN_RESET == RESET_REASON_SYS_PMU_PWR_DOWN, "PMU_SYS_PWR_DOWN_RESET != RESET_REASON_CORE_DEEP_SLEEP");
ESP_STATIC_ASSERT((soc_reset_reason_t)PMU_CPU_PWR_DOWN_RESET == RESET_REASON_CPU_PMU_PWR_DOWN, "PMU_CPU_PWR_DOWN_RESET != RESET_REASON_CORE_SDIO");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_SYS_HP_WDT_RESET == RESET_REASON_SYS_HP_WDT, "HP_SYS_HP_WDT_RESET != RESET_REASON_CORE_MWDT0");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_SYS_LP_WDT_RESET == RESET_REASON_SYS_LP_WDT, "HP_SYS_LP_WDT_RESET != RESET_REASON_SYS_LP_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_CORE_HP_WDT_RESET == RESET_REASON_CORE_HP_WDT, "RTCWDT_SYS_RESET != RESET_REASON_CORE_HP_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_CORE_SYS_RESET == RESET_REASON_CPU0_SW, "HP_CORE_SYS_RESET != RESET_REASON_CPU0_SW");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_CORE_LP_SYS_RESET == RESET_REASON_CORE_LP_WDT, "HP_CORE_LP_SYS_RESET != RESET_REASON_CORE_LP_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)BROWN_OUT_RESET  == RESET_REASON_SYS_BROWN_OUT, "BROWN_OUT_RESET != RESET_REASON_SYS_BROWN_OUT");
ESP_STATIC_ASSERT((soc_reset_reason_t)LP_WDT_CHIP_RESET == RESET_REASON_CHIP_LP_WDT, "LP_WDT_CHIP_RESET != RESET_REASON_CHIP_LP_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)GLITCH_RTC_RESET == RESET_REASON_SYS_CLK_GLITCH, "GLITCH_RTC_RESET != RESET_REASON_SYS_CLK_GLITCH");
ESP_STATIC_ASSERT((soc_reset_reason_t)EFUSE_CRC_ERR_RESET == RESET_REASON_CORE_EFUSE_CRC, "EFUSE_RESET != RESET_REASON_CORE_EFUSE_CRC");
ESP_STATIC_ASSERT((soc_reset_reason_t)SUPER_WDT_RESET == RESET_REASON_SYS_SUPER_WDT, "SUPER_WDT_RESET != RESET_REASON_SYS_SUPER_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_USB_JTAG_RESET == RESET_REASON_CORE_USB_JTAG, "HP_USB_JTAG_RESET != RESET_REASON_CORE_USB_JTAG");
ESP_STATIC_ASSERT((soc_reset_reason_t)HP_USB_UART_RESET == RESET_REASON_CORE_USB_UART, "HP_USB_UART_RESET != RESET_REASON_CORE_USB_UART");
ESP_STATIC_ASSERT((soc_reset_reason_t)JTAG_RESET == RESET_REASON_CPU_JTAG, "JTAG_RESET != RESET_REASON_CPU_JTAG");


typedef enum {
    NO_SLEEP        = 0,
    EXT_EVENT0_TRIG = BIT0,
    EXT_EVENT1_TRIG = BIT1,
    GPIO_TRIG       = BIT2,
    TIMER_EXPIRE    = BIT3,
    SDIO_TRIG       = BIT4,
    MAC_TRIG        = BIT5,
    UART0_TRIG      = BIT6,
    UART1_TRIG      = BIT7,
    TOUCH_TRIG      = BIT8,
    SAR_TRIG        = BIT9,
    BT_TRIG         = BIT10,
    RISCV_TRIG      = BIT11,
    XTAL_DEAD_TRIG  = BIT12,
    RISCV_TRAP_TRIG = BIT13,
    USB_TRIG        = BIT14
} WAKEUP_REASON;

typedef enum {
    DISEN_WAKEUP       = NO_SLEEP,
    EXT_EVENT0_TRIG_EN = EXT_EVENT0_TRIG,
    EXT_EVENT1_TRIG_EN = EXT_EVENT1_TRIG,
    GPIO_TRIG_EN       = GPIO_TRIG,
    TIMER_EXPIRE_EN    = TIMER_EXPIRE,
    SDIO_TRIG_EN       = SDIO_TRIG,
    MAC_TRIG_EN        = MAC_TRIG,
    UART0_TRIG_EN      = UART0_TRIG,
    UART1_TRIG_EN      = UART1_TRIG,
    TOUCH_TRIG_EN      = TOUCH_TRIG,
    SAR_TRIG_EN        = SAR_TRIG,
    BT_TRIG_EN         = BT_TRIG,
    RISCV_TRIG_EN      = RISCV_TRIG,
    XTAL_DEAD_TRIG_EN  = XTAL_DEAD_TRIG,
    RISCV_TRAP_TRIG_EN = RISCV_TRAP_TRIG,
    USB_TRIG_EN        = USB_TRIG
} WAKEUP_ENABLE;

/**
  * @brief  Get the reset reason for CPU.
  *
  * @param  int cpu_no : CPU no.
  *
  * @return RESET_REASON
  */
RESET_REASON rtc_get_reset_reason(int cpu_no);

/**
  * @brief  Get the wakeup cause for CPU.
  *
  * @param  int cpu_no : CPU no.
  *
  * @return WAKEUP_REASON
  */
WAKEUP_REASON rtc_get_wakeup_cause(void);

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
    // REG_SET_BIT(LP_SYS_LP_STORE4_REG, RTC_DISABLE_ROM_LOG);
  abort();
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
