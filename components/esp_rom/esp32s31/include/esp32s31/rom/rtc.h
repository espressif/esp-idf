/*
 * SPDX-FileCopyrightText: 2010-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_RTC_H_
#define _ROM_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/lp_system_reg.h"
#include "soc/lp_clkrst_reg.h"

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
#define RTC_CNTL_STORE0_REG  LP_SYSTEM_REG_LP_STORE0_REG
#define RTC_CNTL_STORE1_REG  LP_SYSTEM_REG_LP_STORE1_REG
#define RTC_CNTL_STORE2_REG  LP_SYSTEM_REG_LP_STORE2_REG
#define RTC_CNTL_STORE3_REG  LP_SYSTEM_REG_LP_STORE3_REG
#define RTC_CNTL_STORE4_REG  LP_SYSTEM_REG_LP_STORE4_REG
#define RTC_CNTL_STORE5_REG  LP_SYSTEM_REG_LP_STORE5_REG
#define RTC_CNTL_STORE6_REG  LP_SYSTEM_REG_LP_STORE6_REG
#define RTC_CNTL_STORE7_REG  LP_SYSTEM_REG_LP_STORE7_REG
#define RTC_CNTL_STORE8_REG  LP_SYSTEM_REG_LP_STORE8_REG
#define RTC_CNTL_STORE9_REG  LP_SYSTEM_REG_LP_STORE9_REG
#define RTC_CNTL_STORE10_REG LP_SYSTEM_REG_LP_STORE10_REG
#define RTC_CNTL_STORE11_REG LP_SYSTEM_REG_LP_STORE11_REG
#define RTC_CNTL_STORE12_REG LP_SYSTEM_REG_LP_STORE12_REG
#define RTC_CNTL_STORE13_REG LP_SYSTEM_REG_LP_STORE13_REG
#define RTC_CNTL_STORE14_REG LP_SYSTEM_REG_LP_STORE14_REG
#define RTC_CNTL_STORE15_REG LP_SYSTEM_REG_LP_STORE15_REG
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
#define SLEEP_MODE_LIGHT_SLEEP 0
// hpcore use bit0 to check the mode
#define SLEEP_MODE_MASK BIT(0)

#define RTC_LIGHT_SLEEP_WAKE_STUB_ADDR_REG     RTC_CNTL_STORE8_REG
#define RTC_LIGHT_SLEEP_RESTORE_FUNC_ADDR_MASK 0xFFFFFFFC
#define SLEEP_MODE_REG                         RTC_CNTL_STORE8_REG
// system
#define RTC_SLOW_CLK_CAL_REG   RTC_CNTL_STORE1_REG
#define RTC_BOOT_TIME_LOW_REG  RTC_CNTL_STORE2_REG
#define RTC_BOOT_TIME_HIGH_REG RTC_CNTL_STORE3_REG
#define RTC_XTAL_FREQ_REG      RTC_CNTL_STORE4_REG
// deep sleep
#define RTC_ENTRY_LENGTH_REG RTC_CNTL_STORE5_REG
#define RTC_ENTRY_ADDR_REG   RTC_CNTL_STORE6_REG
/* use RTC_ENTRY_ADDR_REG[0] to store if we need do stub crc check
 * 0: we do not need do crc check, then RTC_MEMORY_CRC_REG and RTC_ENTRY_LENGTH_REG can be freed
 * 1: we need do crc check
*/
#define RTC_DEEP_SLEEP_STUB_CHECK_CRC 0x1
#define RTC_MEMORY_CRC_REG            RTC_CNTL_STORE7_REG
#define ROM_LOG_CTRL_REG              RTC_XTAL_FREQ_REG

/* Reset reason hint register - same as RTC_ENTRY_ADDR_REG, can be used for both
 * deep sleep wake stub entry address and reset reason hint, since wake stub
 * is only used for deep sleep reset.
 */
#define RTC_RESET_CAUSE_REG RTC_ENTRY_ADDR_REG

// used to check if app core need hot boot in start.S
#define APP_CORE_BOOT_ADDR_REG LP_SYS_BOOT_ADDR_HP_CORE1_REG
// used to check if we need do software retention in start.S
#define RESET_REASON_REG       LP_AONCLKRST_HPCORE0_RESET_CAUSE_REG
#define RESET_REASON_MASK      LP_AONCLKRST_HPCORE0_RESET_CAUSE
#define CORE0_RESET_REASON_POS LP_AONCLKRST_HPCORE0_RESET_CAUSE_S

#define RESET_REASON1_REG      LP_AONCLKRST_HPCORE0_RESET_CAUSE_REG
#define RESET_REASON1_MASK     LP_AONCLKRST_HPCORE0_RESET_CAUSE
#define CORE1_RESET_REASON_POS LP_AONCLKRST_HPCORE1_RESET_CAUSE_S
/**
 * start.S check reset reason and sleep mode register to decide if doing software retention
 */
#define RESET_REASON_TOP_SLEEP_WAKEUP 5
#define RESET_REASON_CPU_SLEEP_WAKEUP 6 // wake up from CPU domain power-down sleep

#ifdef __cplusplus
}
#endif

#endif /* _ROM_RTC_H_ */
