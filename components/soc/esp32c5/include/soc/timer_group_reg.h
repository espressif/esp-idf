/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** TIMG_T0CONFIG_REG register
 *  Timer 0 configuration register
 */
#define TIMG_T0CONFIG_REG(i) (REG_TIMG_BASE(i) + 0x0)
/** TIMG_T0_ALARM_EN : R/W/SC; bitpos: [10]; default: 0;
 *  Configures whether or not to enable the timer 0 alarm function. This bit will be
 *  automatically cleared once an alarm occurs.\\
 *  0: Disable \\
 *  1: Enable  \\
 */
#define TIMG_T0_ALARM_EN    (BIT(10))
#define TIMG_T0_ALARM_EN_M  (TIMG_T0_ALARM_EN_V << TIMG_T0_ALARM_EN_S)
#define TIMG_T0_ALARM_EN_V  0x00000001U
#define TIMG_T0_ALARM_EN_S  10
/** TIMG_T0_DIVCNT_RST : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to reset the timer 0 's clock divider counter. \\
 *  0: No effect \\
 *  1: Reset \\
 */
#define TIMG_T0_DIVCNT_RST    (BIT(12))
#define TIMG_T0_DIVCNT_RST_M  (TIMG_T0_DIVCNT_RST_V << TIMG_T0_DIVCNT_RST_S)
#define TIMG_T0_DIVCNT_RST_V  0x00000001U
#define TIMG_T0_DIVCNT_RST_S  12
/** TIMG_T0_DIVIDER : R/W; bitpos: [28:13]; default: 1;
 *  Represents the timer 0 clock (T0_clk) prescaler value.
 */
#define TIMG_T0_DIVIDER    0x0000FFFFU
#define TIMG_T0_DIVIDER_M  (TIMG_T0_DIVIDER_V << TIMG_T0_DIVIDER_S)
#define TIMG_T0_DIVIDER_V  0x0000FFFFU
#define TIMG_T0_DIVIDER_S  13
/** TIMG_T0_AUTORELOAD : R/W; bitpos: [29]; default: 1;
 *  Configures whether or not to enable the timer 0 auto-reload function at the time of
 *  alarm. \\
 *  0: No effect \\
 *  1: Enable \\
 */
#define TIMG_T0_AUTORELOAD    (BIT(29))
#define TIMG_T0_AUTORELOAD_M  (TIMG_T0_AUTORELOAD_V << TIMG_T0_AUTORELOAD_S)
#define TIMG_T0_AUTORELOAD_V  0x00000001U
#define TIMG_T0_AUTORELOAD_S  29
/** TIMG_T0_INCREASE : R/W; bitpos: [30]; default: 1;
 *  Configures the counting direction of the timer 0 time-base counter. \\
 *  0: Decrement \\
 *  1: Increment \\
 *
 */
#define TIMG_T0_INCREASE    (BIT(30))
#define TIMG_T0_INCREASE_M  (TIMG_T0_INCREASE_V << TIMG_T0_INCREASE_S)
#define TIMG_T0_INCREASE_V  0x00000001U
#define TIMG_T0_INCREASE_S  30
/** TIMG_T0_EN : R/W/SS/SC; bitpos: [31]; default: 0;
 *  Configures whether or not to enable the timer 0 time-base counter. \\
 *  0: Disable \\
 *  1: Enable \\
 */
#define TIMG_T0_EN    (BIT(31))
#define TIMG_T0_EN_M  (TIMG_T0_EN_V << TIMG_T0_EN_S)
#define TIMG_T0_EN_V  0x00000001U
#define TIMG_T0_EN_S  31

/** TIMG_T0LO_REG register
 *  Timer 0 current value, low 32 bits
 */
#define TIMG_T0LO_REG(i) (REG_TIMG_BASE(i) + 0x4)
/** TIMG_T0_LO : RO; bitpos: [31:0]; default: 0;
 *  Represents the low 32 bits of the time-base counter of timer 0. Valid only after
 *  writing to TIMG_T0UPDATE_REG. \\
 *  Measurement unit: T0_clk \\
 */
#define TIMG_T0_LO    0xFFFFFFFFU
#define TIMG_T0_LO_M  (TIMG_T0_LO_V << TIMG_T0_LO_S)
#define TIMG_T0_LO_V  0xFFFFFFFFU
#define TIMG_T0_LO_S  0

/** TIMG_T0HI_REG register
 *  Timer 0 current value, high 22 bits
 */
#define TIMG_T0HI_REG(i) (REG_TIMG_BASE(i) + 0x8)
/** TIMG_T0_HI : RO; bitpos: [21:0]; default: 0;
 *  Represents the high 22 bits of the time-base counter of timer 0. Valid only after
 *  writing to TIMG_T0UPDATE_REG. \\
 *  Measurement unit: T0_clk \\
 */
#define TIMG_T0_HI    0x003FFFFFU
#define TIMG_T0_HI_M  (TIMG_T0_HI_V << TIMG_T0_HI_S)
#define TIMG_T0_HI_V  0x003FFFFFU
#define TIMG_T0_HI_S  0

/** TIMG_T0UPDATE_REG register
 *  Write to copy current timer value to TIMGn_T0LO_REG or TIMGn_T0HI_REG
 */
#define TIMG_T0UPDATE_REG(i) (REG_TIMG_BASE(i) + 0xc)
/** TIMG_T0_UPDATE : R/W/SC; bitpos: [31]; default: 0;
 *  Configures to latch the counter value. \\
 *  0: Latch \\
 *  1: Latch \\
 */
#define TIMG_T0_UPDATE    (BIT(31))
#define TIMG_T0_UPDATE_M  (TIMG_T0_UPDATE_V << TIMG_T0_UPDATE_S)
#define TIMG_T0_UPDATE_V  0x00000001U
#define TIMG_T0_UPDATE_S  31

/** TIMG_T0ALARMLO_REG register
 *  Timer 0 alarm value, low 32 bits
 */
#define TIMG_T0ALARMLO_REG(i) (REG_TIMG_BASE(i) + 0x10)
/** TIMG_T0_ALARM_LO : R/W; bitpos: [31:0]; default: 0;
 *  Configures the low 32 bits of timer 0 alarm trigger time-base counter value. Valid
 *  only when TIMG_T0_ALARM_EN is 1. \\
 *  Measurement unit: T0_clk \\
 */
#define TIMG_T0_ALARM_LO    0xFFFFFFFFU
#define TIMG_T0_ALARM_LO_M  (TIMG_T0_ALARM_LO_V << TIMG_T0_ALARM_LO_S)
#define TIMG_T0_ALARM_LO_V  0xFFFFFFFFU
#define TIMG_T0_ALARM_LO_S  0

/** TIMG_T0ALARMHI_REG register
 *  Timer 0 alarm value, high bits
 */
#define TIMG_T0ALARMHI_REG(i) (REG_TIMG_BASE(i) + 0x14)
/** TIMG_T0_ALARM_HI : R/W; bitpos: [21:0]; default: 0;
 *  Configures the high 22 bits of timer 0 alarm trigger time-base counter value. Valid
 *  only when TIMG_T0_ALARM_EN is 1. \\
 *  Measurement unit: T0_clk \\
 */
#define TIMG_T0_ALARM_HI    0x003FFFFFU
#define TIMG_T0_ALARM_HI_M  (TIMG_T0_ALARM_HI_V << TIMG_T0_ALARM_HI_S)
#define TIMG_T0_ALARM_HI_V  0x003FFFFFU
#define TIMG_T0_ALARM_HI_S  0

/** TIMG_T0LOADLO_REG register
 *  Timer 0 reload value, low 32 bits
 */
#define TIMG_T0LOADLO_REG(i) (REG_TIMG_BASE(i) + 0x18)
/** TIMG_T0_LOAD_LO : R/W; bitpos: [31:0]; default: 0;
 *  Configures low 32 bits of the value that a reload will load onto timer 0 time-base
 *  counter. \\
 *  Measurement unit: T0_clk \\
 */
#define TIMG_T0_LOAD_LO    0xFFFFFFFFU
#define TIMG_T0_LOAD_LO_M  (TIMG_T0_LOAD_LO_V << TIMG_T0_LOAD_LO_S)
#define TIMG_T0_LOAD_LO_V  0xFFFFFFFFU
#define TIMG_T0_LOAD_LO_S  0

/** TIMG_T0LOADHI_REG register
 *  Timer 0 reload value, high 22 bits
 */
#define TIMG_T0LOADHI_REG(i) (REG_TIMG_BASE(i) + 0x1c)
/** TIMG_T0_LOAD_HI : R/W; bitpos: [21:0]; default: 0;
 *  Configures high 22 bits of the value that a reload will load onto timer 0 time-base
 *  counter. \\
 *  Measurement unit: T0_clk \\
 */
#define TIMG_T0_LOAD_HI    0x003FFFFFU
#define TIMG_T0_LOAD_HI_M  (TIMG_T0_LOAD_HI_V << TIMG_T0_LOAD_HI_S)
#define TIMG_T0_LOAD_HI_V  0x003FFFFFU
#define TIMG_T0_LOAD_HI_S  0

/** TIMG_T0LOAD_REG register
 *  Write to reload timer from TIMG_T0LOADLO_REG or TIMG_T0LOADHI_REG
 */
#define TIMG_T0LOAD_REG(i) (REG_TIMG_BASE(i) + 0x20)
/** TIMG_T0_LOAD : WT; bitpos: [31:0]; default: 0;
 *  Write any value to trigger a timer 0 time-base counter reload.
 *
 */
#define TIMG_T0_LOAD    0xFFFFFFFFU
#define TIMG_T0_LOAD_M  (TIMG_T0_LOAD_V << TIMG_T0_LOAD_S)
#define TIMG_T0_LOAD_V  0xFFFFFFFFU
#define TIMG_T0_LOAD_S  0

/** TIMG_WDTCONFIG0_REG register
 *  Watchdog timer configuration register
 */
#define TIMG_WDTCONFIG0_REG(i) (REG_TIMG_BASE(i) + 0x48)
/** TIMG_WDT_APPCPU_RESET_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether to mask the CPU reset generated by MWDT. Valid only when write
 *  protection is disabled. \\
 *  0: Mask \\
 *  1: Unmask \\
 */
#define TIMG_WDT_APPCPU_RESET_EN    (BIT(12))
#define TIMG_WDT_APPCPU_RESET_EN_M  (TIMG_WDT_APPCPU_RESET_EN_V << TIMG_WDT_APPCPU_RESET_EN_S)
#define TIMG_WDT_APPCPU_RESET_EN_V  0x00000001U
#define TIMG_WDT_APPCPU_RESET_EN_S  12
/** TIMG_WDT_PROCPU_RESET_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether to mask the CPU reset generated by MWDT. Valid only when write
 *  protection is disabled. \\
 *  0: Mask \\
 *  1: Unmask \\
 *
 */
#define TIMG_WDT_PROCPU_RESET_EN    (BIT(13))
#define TIMG_WDT_PROCPU_RESET_EN_M  (TIMG_WDT_PROCPU_RESET_EN_V << TIMG_WDT_PROCPU_RESET_EN_S)
#define TIMG_WDT_PROCPU_RESET_EN_V  0x00000001U
#define TIMG_WDT_PROCPU_RESET_EN_S  13
/** TIMG_WDT_FLASHBOOT_MOD_EN : R/W; bitpos: [14]; default: 1;
 *  Configures whether to enable flash boot protection.\\
 *  0: Disable \\
 *  1: Enable \\
 */
#define TIMG_WDT_FLASHBOOT_MOD_EN    (BIT(14))
#define TIMG_WDT_FLASHBOOT_MOD_EN_M  (TIMG_WDT_FLASHBOOT_MOD_EN_V << TIMG_WDT_FLASHBOOT_MOD_EN_S)
#define TIMG_WDT_FLASHBOOT_MOD_EN_V  0x00000001U
#define TIMG_WDT_FLASHBOOT_MOD_EN_S  14
/** TIMG_WDT_SYS_RESET_LENGTH : R/W; bitpos: [17:15]; default: 1;
 *  Configures the system reset signal length. Valid only when write protection is
 *  disabled. \\
 *  Measurement unit: mwdt_clk
 *  \begin{multicols}{2}
 *  0: 8 \\
 *  1: 16 \\
 *  2: 24 \\
 *  3: 32 \\
 *  4: 40 \\
 *  5: 64 \\
 *  6: 128 \\
 *  7: 256 \\
 *  \end{multicols}
 */
#define TIMG_WDT_SYS_RESET_LENGTH    0x00000007U
#define TIMG_WDT_SYS_RESET_LENGTH_M  (TIMG_WDT_SYS_RESET_LENGTH_V << TIMG_WDT_SYS_RESET_LENGTH_S)
#define TIMG_WDT_SYS_RESET_LENGTH_V  0x00000007U
#define TIMG_WDT_SYS_RESET_LENGTH_S  15
/** TIMG_WDT_CPU_RESET_LENGTH : R/W; bitpos: [20:18]; default: 1;
 *  Configures the CPU reset signal length. Valid only when write protection is
 *  disabled.\\
 *  Measurement unit: mwdt_clk \\
 *  \begin{multicols}{2}
 *  0: 8 \\
 *  1: 16 \\
 *  2: 24 \\
 *  3: 32 \\
 *  4: 40 \\
 *  5: 64 \\
 *  6: 128 \\
 *  7: 256 \\
 *  \end{multicols}
 */
#define TIMG_WDT_CPU_RESET_LENGTH    0x00000007U
#define TIMG_WDT_CPU_RESET_LENGTH_M  (TIMG_WDT_CPU_RESET_LENGTH_V << TIMG_WDT_CPU_RESET_LENGTH_S)
#define TIMG_WDT_CPU_RESET_LENGTH_V  0x00000007U
#define TIMG_WDT_CPU_RESET_LENGTH_S  18
/** TIMG_WDT_CONF_UPDATE_EN : WT; bitpos: [22]; default: 0;
 *  Configures to update the WDT configuration registers.\\
 *  0: No effect \\
 *  1: Update \\
 */
#define TIMG_WDT_CONF_UPDATE_EN    (BIT(22))
#define TIMG_WDT_CONF_UPDATE_EN_M  (TIMG_WDT_CONF_UPDATE_EN_V << TIMG_WDT_CONF_UPDATE_EN_S)
#define TIMG_WDT_CONF_UPDATE_EN_V  0x00000001U
#define TIMG_WDT_CONF_UPDATE_EN_S  22
/** TIMG_WDT_STG3 : R/W; bitpos: [24:23]; default: 0;
 *  Configures the timeout action of stage 3. See details in TIMG_WDT_STG0. Valid only
 *  when write protection is disabled.
 */
#define TIMG_WDT_STG3    0x00000003U
#define TIMG_WDT_STG3_M  (TIMG_WDT_STG3_V << TIMG_WDT_STG3_S)
#define TIMG_WDT_STG3_V  0x00000003U
#define TIMG_WDT_STG3_S  23
/** TIMG_WDT_STG2 : R/W; bitpos: [26:25]; default: 0;
 *  Configures the  timeout action of stage 2. See details in TIMG_WDT_STG0. Valid only
 *  when write protection is disabled.
 */
#define TIMG_WDT_STG2    0x00000003U
#define TIMG_WDT_STG2_M  (TIMG_WDT_STG2_V << TIMG_WDT_STG2_S)
#define TIMG_WDT_STG2_V  0x00000003U
#define TIMG_WDT_STG2_S  25
/** TIMG_WDT_STG1 : R/W; bitpos: [28:27]; default: 0;
 *  Configures the  timeout action of stage 1. See details in TIMG_WDT_STG0. Valid only
 *  when write protection is disabled.
 */
#define TIMG_WDT_STG1    0x00000003U
#define TIMG_WDT_STG1_M  (TIMG_WDT_STG1_V << TIMG_WDT_STG1_S)
#define TIMG_WDT_STG1_V  0x00000003U
#define TIMG_WDT_STG1_S  27
/** TIMG_WDT_STG0 : R/W; bitpos: [30:29]; default: 0;
 *  Configures the timeout action of stage 0. Valid only when write protection is
 *  disabled. \\
 *  0: No effect \\
 *  1: Interrupt \\
 *  2: Reset CPU \\
 *  3: Reset system \\
 */
#define TIMG_WDT_STG0    0x00000003U
#define TIMG_WDT_STG0_M  (TIMG_WDT_STG0_V << TIMG_WDT_STG0_S)
#define TIMG_WDT_STG0_V  0x00000003U
#define TIMG_WDT_STG0_S  29
/** TIMG_WDT_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to enable the MWDT. Valid only when write protection is
 *  disabled. \\
 *  0: Disable \\
 *  1: Enable \\
 */
#define TIMG_WDT_EN    (BIT(31))
#define TIMG_WDT_EN_M  (TIMG_WDT_EN_V << TIMG_WDT_EN_S)
#define TIMG_WDT_EN_V  0x00000001U
#define TIMG_WDT_EN_S  31

/** TIMG_WDTCONFIG1_REG register
 *  Watchdog timer prescaler register
 */
#define TIMG_WDTCONFIG1_REG(i) (REG_TIMG_BASE(i) + 0x4c)
/** TIMG_WDT_DIVCNT_RST : WT; bitpos: [0]; default: 0;
 *  Configures whether to reset WDT 's clock divider counter. \\
 *  0: No effect \\
 *  1: Reset \\
 */
#define TIMG_WDT_DIVCNT_RST    (BIT(0))
#define TIMG_WDT_DIVCNT_RST_M  (TIMG_WDT_DIVCNT_RST_V << TIMG_WDT_DIVCNT_RST_S)
#define TIMG_WDT_DIVCNT_RST_V  0x00000001U
#define TIMG_WDT_DIVCNT_RST_S  0
/** TIMG_WDT_CLK_PRESCALE : R/W; bitpos: [31:16]; default: 1;
 *  Configures MWDT clock prescaler value. Valid only when write protection is
 *  disabled. \\
 *  MWDT clock period = MWDT's clock source period * TIMG_WDT_CLK_PRESCALE. \\
 */
#define TIMG_WDT_CLK_PRESCALE    0x0000FFFFU
#define TIMG_WDT_CLK_PRESCALE_M  (TIMG_WDT_CLK_PRESCALE_V << TIMG_WDT_CLK_PRESCALE_S)
#define TIMG_WDT_CLK_PRESCALE_V  0x0000FFFFU
#define TIMG_WDT_CLK_PRESCALE_S  16

/** TIMG_WDTCONFIG2_REG register
 *  Watchdog timer stage 0 timeout value
 */
#define TIMG_WDTCONFIG2_REG(i) (REG_TIMG_BASE(i) + 0x50)
/** TIMG_WDT_STG0_HOLD : R/W; bitpos: [31:0]; default: 26000000;
 *  Configures the stage 0 timeout value. Valid only when write protection is disabled.
 *  \\
 *  Measurement unit: mwdt_clk \\
 */
#define TIMG_WDT_STG0_HOLD    0xFFFFFFFFU
#define TIMG_WDT_STG0_HOLD_M  (TIMG_WDT_STG0_HOLD_V << TIMG_WDT_STG0_HOLD_S)
#define TIMG_WDT_STG0_HOLD_V  0xFFFFFFFFU
#define TIMG_WDT_STG0_HOLD_S  0

/** TIMG_WDTCONFIG3_REG register
 *  Watchdog timer stage 1 timeout value
 */
#define TIMG_WDTCONFIG3_REG(i) (REG_TIMG_BASE(i) + 0x54)
/** TIMG_WDT_STG1_HOLD : R/W; bitpos: [31:0]; default: 134217727;
 *  Configures the stage 1 timeout value. Valid only when write protection is
 *  disabled.\\
 *  Measurement unit: mwdt_clk \\
 */
#define TIMG_WDT_STG1_HOLD    0xFFFFFFFFU
#define TIMG_WDT_STG1_HOLD_M  (TIMG_WDT_STG1_HOLD_V << TIMG_WDT_STG1_HOLD_S)
#define TIMG_WDT_STG1_HOLD_V  0xFFFFFFFFU
#define TIMG_WDT_STG1_HOLD_S  0

/** TIMG_WDTCONFIG4_REG register
 *  Watchdog timer stage 2 timeout value
 */
#define TIMG_WDTCONFIG4_REG(i) (REG_TIMG_BASE(i) + 0x58)
/** TIMG_WDT_STG2_HOLD : R/W; bitpos: [31:0]; default: 1048575;
 *  Configures the stage 2 timeout value. Valid only when write protection is disabled.
 *  \\
 *  Measurement unit: mwdt_clk \\
 */
#define TIMG_WDT_STG2_HOLD    0xFFFFFFFFU
#define TIMG_WDT_STG2_HOLD_M  (TIMG_WDT_STG2_HOLD_V << TIMG_WDT_STG2_HOLD_S)
#define TIMG_WDT_STG2_HOLD_V  0xFFFFFFFFU
#define TIMG_WDT_STG2_HOLD_S  0

/** TIMG_WDTCONFIG5_REG register
 *  Watchdog timer stage 3 timeout value
 */
#define TIMG_WDTCONFIG5_REG(i) (REG_TIMG_BASE(i) + 0x5c)
/** TIMG_WDT_STG3_HOLD : R/W; bitpos: [31:0]; default: 1048575;
 *  Configures the stage 3 timeout value. Valid only when write protection is disabled.
 *  \\
 *  Measurement unit: mwdt_clk \\
 */
#define TIMG_WDT_STG3_HOLD    0xFFFFFFFFU
#define TIMG_WDT_STG3_HOLD_M  (TIMG_WDT_STG3_HOLD_V << TIMG_WDT_STG3_HOLD_S)
#define TIMG_WDT_STG3_HOLD_V  0xFFFFFFFFU
#define TIMG_WDT_STG3_HOLD_S  0

/** TIMG_WDTFEED_REG register
 *  Write to feed the watchdog timer
 */
#define TIMG_WDTFEED_REG(i) (REG_TIMG_BASE(i) + 0x60)
/** TIMG_WDT_FEED : WT; bitpos: [31:0]; default: 0;
 *  Write any value to feed the MWDT. Valid only when write protection is disabled.
 */
#define TIMG_WDT_FEED    0xFFFFFFFFU
#define TIMG_WDT_FEED_M  (TIMG_WDT_FEED_V << TIMG_WDT_FEED_S)
#define TIMG_WDT_FEED_V  0xFFFFFFFFU
#define TIMG_WDT_FEED_S  0

/** TIMG_WDTWPROTECT_REG register
 *  Watchdog write protect register
 */
#define TIMG_WDTWPROTECT_REG(i) (REG_TIMG_BASE(i) + 0x64)
/** TIMG_WDT_WKEY : R/W; bitpos: [31:0]; default: 1356348065;
 *  Configures a different value than its reset value to enable write protection.
 */
#define TIMG_WDT_WKEY    0xFFFFFFFFU
#define TIMG_WDT_WKEY_M  (TIMG_WDT_WKEY_V << TIMG_WDT_WKEY_S)
#define TIMG_WDT_WKEY_V  0xFFFFFFFFU
#define TIMG_WDT_WKEY_S  0

/** TIMG_RTCCALICFG_REG register
 *  RTC frequency calculation configuration register 0
 */
#define TIMG_RTCCALICFG_REG(i) (REG_TIMG_BASE(i) + 0x68)
/** TIMG_RTC_CALI_START_CYCLING : R/W; bitpos: [12]; default: 1;
 *  Configures the frequency calculation mode.  \\
 *  0: one-shot frequency calculation \\
 *  1: periodic frequency calculation \\
 */
#define TIMG_RTC_CALI_START_CYCLING    (BIT(12))
#define TIMG_RTC_CALI_START_CYCLING_M  (TIMG_RTC_CALI_START_CYCLING_V << TIMG_RTC_CALI_START_CYCLING_S)
#define TIMG_RTC_CALI_START_CYCLING_V  0x00000001U
#define TIMG_RTC_CALI_START_CYCLING_S  12
/** TIMG_RTC_CALI_RDY : RO; bitpos: [15]; default: 0;
 *  Represents whether one-shot frequency calculation is done.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define TIMG_RTC_CALI_RDY    (BIT(15))
#define TIMG_RTC_CALI_RDY_M  (TIMG_RTC_CALI_RDY_V << TIMG_RTC_CALI_RDY_S)
#define TIMG_RTC_CALI_RDY_V  0x00000001U
#define TIMG_RTC_CALI_RDY_S  15
/** TIMG_RTC_CALI_MAX : R/W; bitpos: [30:16]; default: 1;
 *  Configures the time to calculate RTC slow clock's frequency. \\
 *  Measurement unit: XTAL_CLK \\
 */
#define TIMG_RTC_CALI_MAX    0x00007FFFU
#define TIMG_RTC_CALI_MAX_M  (TIMG_RTC_CALI_MAX_V << TIMG_RTC_CALI_MAX_S)
#define TIMG_RTC_CALI_MAX_V  0x00007FFFU
#define TIMG_RTC_CALI_MAX_S  16
/** TIMG_RTC_CALI_START : R/W; bitpos: [31]; default: 0;
 *  Configures whether to enable one-shot frequency calculation. \\
 *  0: Disable \\
 *  1: Enable \\
 */
#define TIMG_RTC_CALI_START    (BIT(31))
#define TIMG_RTC_CALI_START_M  (TIMG_RTC_CALI_START_V << TIMG_RTC_CALI_START_S)
#define TIMG_RTC_CALI_START_V  0x00000001U
#define TIMG_RTC_CALI_START_S  31

/** TIMG_RTCCALICFG1_REG register
 *  RTC frequency calculation configuration register 1
 */
#define TIMG_RTCCALICFG1_REG(i) (REG_TIMG_BASE(i) + 0x6c)
/** TIMG_RTC_CALI_CYCLING_DATA_VLD : RO; bitpos: [0]; default: 0;
 *  Represents whether periodic frequency calculation is done. \\
 *  0: Not done \\
 *  1: Done \\
 */
#define TIMG_RTC_CALI_CYCLING_DATA_VLD    (BIT(0))
#define TIMG_RTC_CALI_CYCLING_DATA_VLD_M  (TIMG_RTC_CALI_CYCLING_DATA_VLD_V << TIMG_RTC_CALI_CYCLING_DATA_VLD_S)
#define TIMG_RTC_CALI_CYCLING_DATA_VLD_V  0x00000001U
#define TIMG_RTC_CALI_CYCLING_DATA_VLD_S  0
/** TIMG_RTC_CALI_VALUE : RO; bitpos: [31:7]; default: 0;
 *  Represents the value countered by XTAL_CLK when one-shot or periodic frequency
 *  calculation is done. It is used to calculate RTC slow clock's frequency.
 */
#define TIMG_RTC_CALI_VALUE    0x01FFFFFFU
#define TIMG_RTC_CALI_VALUE_M  (TIMG_RTC_CALI_VALUE_V << TIMG_RTC_CALI_VALUE_S)
#define TIMG_RTC_CALI_VALUE_V  0x01FFFFFFU
#define TIMG_RTC_CALI_VALUE_S  7

/** TIMG_INT_ENA_TIMERS_REG register
 *  Interrupt enable bits
 */
#define TIMG_INT_ENA_TIMERS_REG(i) (REG_TIMG_BASE(i) + 0x70)
/** TIMG_T0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable the TIMG_T0_INT interrupt.
 */
#define TIMG_T0_INT_ENA    (BIT(0))
#define TIMG_T0_INT_ENA_M  (TIMG_T0_INT_ENA_V << TIMG_T0_INT_ENA_S)
#define TIMG_T0_INT_ENA_V  0x00000001U
#define TIMG_T0_INT_ENA_S  0
/** TIMG_WDT_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable the TIMG_WDT_INT interrupt.
 */
#define TIMG_WDT_INT_ENA    (BIT(2))
#define TIMG_WDT_INT_ENA_M  (TIMG_WDT_INT_ENA_V << TIMG_WDT_INT_ENA_S)
#define TIMG_WDT_INT_ENA_V  0x00000001U
#define TIMG_WDT_INT_ENA_S  2

/** TIMG_INT_RAW_TIMERS_REG register
 *  Raw interrupt status
 */
#define TIMG_INT_RAW_TIMERS_REG(i) (REG_TIMG_BASE(i) + 0x74)
/** TIMG_T0_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  The raw interrupt status bit of the TIMG_T0_INT interrupt.
 */
#define TIMG_T0_INT_RAW    (BIT(0))
#define TIMG_T0_INT_RAW_M  (TIMG_T0_INT_RAW_V << TIMG_T0_INT_RAW_S)
#define TIMG_T0_INT_RAW_V  0x00000001U
#define TIMG_T0_INT_RAW_S  0
/** TIMG_WDT_INT_RAW : R/SS/WTC; bitpos: [2]; default: 0;
 *  The raw interrupt status bit of the TIMG_WDT_INT interrupt.
 */
#define TIMG_WDT_INT_RAW    (BIT(2))
#define TIMG_WDT_INT_RAW_M  (TIMG_WDT_INT_RAW_V << TIMG_WDT_INT_RAW_S)
#define TIMG_WDT_INT_RAW_V  0x00000001U
#define TIMG_WDT_INT_RAW_S  2

/** TIMG_INT_ST_TIMERS_REG register
 *  Masked interrupt status
 */
#define TIMG_INT_ST_TIMERS_REG(i) (REG_TIMG_BASE(i) + 0x78)
/** TIMG_T0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit of the TIMG_T0_INT interrupt.
 */
#define TIMG_T0_INT_ST    (BIT(0))
#define TIMG_T0_INT_ST_M  (TIMG_T0_INT_ST_V << TIMG_T0_INT_ST_S)
#define TIMG_T0_INT_ST_V  0x00000001U
#define TIMG_T0_INT_ST_S  0
/** TIMG_WDT_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit of the TIMG_WDT_INT interrupt.
 */
#define TIMG_WDT_INT_ST    (BIT(2))
#define TIMG_WDT_INT_ST_M  (TIMG_WDT_INT_ST_V << TIMG_WDT_INT_ST_S)
#define TIMG_WDT_INT_ST_V  0x00000001U
#define TIMG_WDT_INT_ST_S  2

/** TIMG_INT_CLR_TIMERS_REG register
 *  Interrupt clear bits
 */
#define TIMG_INT_CLR_TIMERS_REG(i) (REG_TIMG_BASE(i) + 0x7c)
/** TIMG_T0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear the TIMG_T0_INT interrupt.
 */
#define TIMG_T0_INT_CLR    (BIT(0))
#define TIMG_T0_INT_CLR_M  (TIMG_T0_INT_CLR_V << TIMG_T0_INT_CLR_S)
#define TIMG_T0_INT_CLR_V  0x00000001U
#define TIMG_T0_INT_CLR_S  0
/** TIMG_WDT_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear the TIMG_WDT_INT interrupt.
 */
#define TIMG_WDT_INT_CLR    (BIT(2))
#define TIMG_WDT_INT_CLR_M  (TIMG_WDT_INT_CLR_V << TIMG_WDT_INT_CLR_S)
#define TIMG_WDT_INT_CLR_V  0x00000001U
#define TIMG_WDT_INT_CLR_S  2

/** TIMG_RTCCALICFG2_REG register
 *  RTC frequency calculation configuration register 2
 */
#define TIMG_RTCCALICFG2_REG(i) (REG_TIMG_BASE(i) + 0x80)
/** TIMG_RTC_CALI_TIMEOUT : RO; bitpos: [0]; default: 0;
 *  Represents whether RTC frequency calculation is timeout. \\
 *  0: No timeout \\
 *  1: Timeout \\
 */
#define TIMG_RTC_CALI_TIMEOUT    (BIT(0))
#define TIMG_RTC_CALI_TIMEOUT_M  (TIMG_RTC_CALI_TIMEOUT_V << TIMG_RTC_CALI_TIMEOUT_S)
#define TIMG_RTC_CALI_TIMEOUT_V  0x00000001U
#define TIMG_RTC_CALI_TIMEOUT_S  0
/** TIMG_RTC_CALI_TIMEOUT_RST_CNT : R/W; bitpos: [6:3]; default: 3;
 *  Configures the cycles that reset frequency calculation timeout. \\
 *  Measurement unit: XTAL_CLK \\
 */
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT    0x0000000FU
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT_M  (TIMG_RTC_CALI_TIMEOUT_RST_CNT_V << TIMG_RTC_CALI_TIMEOUT_RST_CNT_S)
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT_V  0x0000000FU
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT_S  3
/** TIMG_RTC_CALI_TIMEOUT_THRES : R/W; bitpos: [31:7]; default: 33554431;
 *  Configures the threshold value for the RTC frequency calculation timer. If the
 *  timer's value exceeds this threshold, a timeout is triggered.\\
 *  Measurement unit: XTAL_CLK  \\
 */
#define TIMG_RTC_CALI_TIMEOUT_THRES    0x01FFFFFFU
#define TIMG_RTC_CALI_TIMEOUT_THRES_M  (TIMG_RTC_CALI_TIMEOUT_THRES_V << TIMG_RTC_CALI_TIMEOUT_THRES_S)
#define TIMG_RTC_CALI_TIMEOUT_THRES_V  0x01FFFFFFU
#define TIMG_RTC_CALI_TIMEOUT_THRES_S  7

/** TIMG_NTIMERS_DATE_REG register
 *  Timer version control register
 */
#define TIMG_NTIMERS_DATE_REG(i) (REG_TIMG_BASE(i) + 0xf8)
/** TIMG_NTIMGS_DATE : R/W; bitpos: [27:0]; default: 35688770;
 *  Version control register
 */
#define TIMG_NTIMGS_DATE    0x0FFFFFFFU
#define TIMG_NTIMGS_DATE_M  (TIMG_NTIMGS_DATE_V << TIMG_NTIMGS_DATE_S)
#define TIMG_NTIMGS_DATE_V  0x0FFFFFFFU
#define TIMG_NTIMGS_DATE_S  0

/** TIMG_REGCLK_REG register
 *  Timer group clock gate register
 */
#define TIMG_REGCLK_REG(i) (REG_TIMG_BASE(i) + 0xfc)
/** TIMG_ETM_EN : R/W; bitpos: [28]; default: 1;
 *  Configures whether to enable timer's ETM task and event. \\
 *  0: Disable \\
 *  1: Enable \\
 */
#define TIMG_ETM_EN    (BIT(28))
#define TIMG_ETM_EN_M  (TIMG_ETM_EN_V << TIMG_ETM_EN_S)
#define TIMG_ETM_EN_V  0x00000001U
#define TIMG_ETM_EN_S  28
/** TIMG_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether to enable gate clock signal for registers. \\
 *  0: Force clock on for registers \\
 *  1: Support clock only when registers are read or written to by software. \\
 */
#define TIMG_CLK_EN    (BIT(31))
#define TIMG_CLK_EN_M  (TIMG_CLK_EN_V << TIMG_CLK_EN_S)
#define TIMG_CLK_EN_V  0x00000001U
#define TIMG_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
