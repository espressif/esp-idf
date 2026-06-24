/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ets_sys.h"

#include <stdbool.h>
#include <stdint.h>
#include "esp_assert.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
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
  *                          RTC  Memory & Store Register usage
  *************************************************************************************
  *     rtc memory addr         type    size            usage
  *     0x3ff61000(0x50000000)  Slow    SIZE_CP         Co-Processor code/Reset Entry
  *     0x3ff61000+SIZE_CP      Slow    8192-SIZE_CP
  *
  *     0x3ff80000(0x400c0000)  Fast    8192            deep sleep entry code
  *
  *************************************************************************************
  *     RTC store registers     usage
  *     RTC_CNTL_STORE0_REG     Reserved
  *     RTC_CNTL_STORE1_REG     RTC_SLOW_CLK calibration value
  *     RTC_CNTL_STORE2_REG     Boot time, low word
  *     RTC_CNTL_STORE3_REG     Boot time, high word
  *     RTC_CNTL_STORE4_REG     External XTAL frequency. The frequency must necessarily be even, otherwise there will be a conflict with the low bit, which is used to disable logs in the ROM code.
  *     RTC_CNTL_STORE5_REG     APB bus frequency
  *     RTC_CNTL_STORE6_REG     FAST_RTC_MEMORY_ENTRY
  *     RTC_CNTL_STORE7_REG     FAST_RTC_MEMORY_CRC
  *************************************************************************************
  */

#define RTC_SLOW_CLK_CAL_REG    RTC_CNTL_STORE1_REG
#define RTC_BOOT_TIME_LOW_REG   RTC_CNTL_STORE2_REG
#define RTC_BOOT_TIME_HIGH_REG  RTC_CNTL_STORE3_REG
#define RTC_XTAL_FREQ_REG       RTC_CNTL_STORE4_REG
#define RTC_APB_FREQ_REG        RTC_CNTL_STORE5_REG
#define RTC_ENTRY_ADDR_REG      RTC_CNTL_STORE6_REG
#define RTC_RESET_CAUSE_REG     RTC_CNTL_STORE6_REG
#define RTC_MEMORY_CRC_REG      RTC_CNTL_STORE7_REG

#define RTC_DISABLE_ROM_LOG ((1 << 0) | (1 << 16)) //!< Disable logging from the ROM code.

typedef enum {
    AWAKE = 0,             //<CPU ON
    LIGHT_SLEEP = BIT0,    //CPU waiti, PLL ON.  We don't need explicitly set this mode.
    DEEP_SLEEP  = BIT1     //CPU OFF, PLL OFF, only specific timer could wake up
} SLEEP_MODE;

typedef enum {
    NO_MEAN                =  0,
    POWERON_RESET          =  1,    /**<1, Vbat power on reset*/
    SW_RESET               =  3,    /**<3, Software reset digital core*/
    OWDT_RESET             =  4,    /**<4, Legacy watch dog reset digital core*/
    DEEPSLEEP_RESET        =  5,    /**<3, Deep Sleep reset digital core*/
    SDIO_RESET             =  6,    /**<6, Reset by SLC module, reset digital core*/
    TG0WDT_SYS_RESET       =  7,    /**<7, Timer Group0 Watch dog reset digital core*/
    TG1WDT_SYS_RESET       =  8,    /**<8, Timer Group1 Watch dog reset digital core*/
    RTCWDT_SYS_RESET       =  9,    /**<9, RTC Watch dog Reset digital core*/
    INTRUSION_RESET        = 10,    /**<10, Instrusion tested to reset CPU*/
    TGWDT_CPU_RESET        = 11,    /**<11, Time Group reset CPU*/
    SW_CPU_RESET           = 12,    /**<12, Software reset CPU*/
    RTCWDT_CPU_RESET       = 13,    /**<13, RTC Watch dog Reset CPU*/
    EXT_CPU_RESET          = 14,    /**<14, for APP CPU, reset by PRO CPU*/
    RTCWDT_BROWN_OUT_RESET = 15,    /**<15, Reset when the vdd voltage is not stable*/
    RTCWDT_RTC_RESET       = 16     /**<16, RTC Watch dog reset digital core and rtc module*/
} RESET_REASON;

// Check if the reset reason defined in ROM is compatible with soc/reset_reasons.h
ESP_STATIC_ASSERT((soc_reset_reason_t)POWERON_RESET == RESET_REASON_CHIP_POWER_ON, "POWERON_RESET != RESET_REASON_CHIP_POWER_ON");
ESP_STATIC_ASSERT((soc_reset_reason_t)SW_RESET == RESET_REASON_CORE_SW, "SW_RESET != RESET_REASON_CORE_SW");
ESP_STATIC_ASSERT((soc_reset_reason_t)DEEPSLEEP_RESET == RESET_REASON_CORE_DEEP_SLEEP, "DEEPSLEEP_RESET != RESET_REASON_CORE_DEEP_SLEEP");
ESP_STATIC_ASSERT((soc_reset_reason_t)TG0WDT_SYS_RESET == RESET_REASON_CORE_MWDT0, "TG0WDT_SYS_RESET != RESET_REASON_CORE_MWDT0");
ESP_STATIC_ASSERT((soc_reset_reason_t)TG1WDT_SYS_RESET == RESET_REASON_CORE_MWDT1, "TG1WDT_SYS_RESET != RESET_REASON_CORE_MWDT1");
ESP_STATIC_ASSERT((soc_reset_reason_t)RTCWDT_SYS_RESET == RESET_REASON_CORE_RTC_WDT, "RTCWDT_SYS_RESET != RESET_REASON_CORE_RTC_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)TGWDT_CPU_RESET == RESET_REASON_CPU0_MWDT0, "TGWDT_CPU_RESET != RESET_REASON_CPU0_MWDT0");
ESP_STATIC_ASSERT((soc_reset_reason_t)SW_CPU_RESET == RESET_REASON_CPU0_SW, "SW_CPU_RESET != RESET_REASON_CPU0_SW");
ESP_STATIC_ASSERT((soc_reset_reason_t)RTCWDT_CPU_RESET == RESET_REASON_CPU0_RTC_WDT, "RTCWDT_CPU_RESET != RESET_REASON_CPU0_RTC_WDT");
ESP_STATIC_ASSERT((soc_reset_reason_t)RTCWDT_BROWN_OUT_RESET == RESET_REASON_SYS_BROWN_OUT, "RTCWDT_BROWN_OUT_RESET != RESET_REASON_SYS_BROWN_OUT");
ESP_STATIC_ASSERT((soc_reset_reason_t)RTCWDT_RTC_RESET == RESET_REASON_SYS_RTC_WDT, "RTCWDT_RTC_RESET != RESET_REASON_SYS_RTC_WDT");

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
    BT_TRIG         = BIT10
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
    BT_TRIG_EN         = BT_TRIG
} WAKEUP_ENABLE;

typedef enum {
    NO_INT             = 0,
    WAKEUP_INT         = BIT0,
    REJECT_INT         = BIT1,
    SDIO_IDLE_INT      = BIT2,
    RTC_WDT_INT        = BIT3,
    RTC_TIME_VALID_INT = BIT4
} RTC_INT_REASON;

typedef enum {
    DISEN_INT             = 0,
    WAKEUP_INT_EN         = WAKEUP_INT,
    REJECT_INT_EN         = REJECT_INT,
    SDIO_IDLE_INT_EN      = SDIO_IDLE_INT,
    RTC_WDT_INT_EN        = RTC_WDT_INT,
    RTC_TIME_VALID_INT_EN = RTC_TIME_VALID_INT
} RTC_INT_EN;

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
  * @brief Get CRC for Fast RTC Memory.
  *
  * @param  uint32_t start_addr : 0 - 0x7ff for Fast RTC Memory.
  *
  * @param  uint32_t crc_len : 0 - 0x7ff, 0 for 4 byte, 0x7ff for 0x2000 byte.
  *
  * @return uint32_t : CRC32 result
  */
uint32_t calc_rtc_memory_crc(uint32_t start_addr, uint32_t crc_len);

/**
  * @brief Set CRC of Fast RTC memory 0-0x7ff into RTC STORE7.
  *
  * @param  None
  *
  * @return None
  */
void set_rtc_memory_crc(void);

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
    REG_SET_BIT(RTC_CNTL_STORE4_REG, RTC_DISABLE_ROM_LOG);
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
void __attribute__((__noreturn__)) software_reset(void);

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
