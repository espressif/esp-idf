// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_TIMG_REG_H_
#define _SOC_TIMG_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

/* The value that needs to be written to TIMG_WDT_WKEY to write-enable the wdt registers */
#define TIMG_WDT_WKEY_VALUE 0x50D83AA1

/* Possible values for TIMG_WDT_STGx */
#define TIMG_WDT_STG_SEL_OFF 0
#define TIMG_WDT_STG_SEL_INT 1
#define TIMG_WDT_STG_SEL_RESET_CPU 2
#define TIMG_WDT_STG_SEL_RESET_SYSTEM 3

#define TIMG_WDT_RESET_LENGTH_100_NS 0
#define TIMG_WDT_RESET_LENGTH_200_NS 1
#define TIMG_WDT_RESET_LENGTH_300_NS 2
#define TIMG_WDT_RESET_LENGTH_400_NS 3
#define TIMG_WDT_RESET_LENGTH_500_NS 4
#define TIMG_WDT_RESET_LENGTH_800_NS 5
#define TIMG_WDT_RESET_LENGTH_1600_NS 6
#define TIMG_WDT_RESET_LENGTH_3200_NS 7

#define TIMG_T0CONFIG_REG(i)          (REG_TIMG_BASE(i) + 0x0000)
/* TIMG_T0_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_EN  (BIT(31))
#define TIMG_T0_EN_M  (BIT(31))
#define TIMG_T0_EN_V  0x1
#define TIMG_T0_EN_S  31
/* TIMG_T0_INCREASE : R/W ;bitpos:[30] ;default: 1'h1 ; */
/*description: */
#define TIMG_T0_INCREASE  (BIT(30))
#define TIMG_T0_INCREASE_M  (BIT(30))
#define TIMG_T0_INCREASE_V  0x1
#define TIMG_T0_INCREASE_S  30
/* TIMG_T0_AUTORELOAD : R/W ;bitpos:[29] ;default: 1'h1 ; */
/*description: */
#define TIMG_T0_AUTORELOAD  (BIT(29))
#define TIMG_T0_AUTORELOAD_M  (BIT(29))
#define TIMG_T0_AUTORELOAD_V  0x1
#define TIMG_T0_AUTORELOAD_S  29
/* TIMG_T0_DIVIDER : R/W ;bitpos:[28:13] ;default: 16'h1 ; */
/*description: */
#define TIMG_T0_DIVIDER  0x0000FFFF
#define TIMG_T0_DIVIDER_M  ((TIMG_T0_DIVIDER_V)<<(TIMG_T0_DIVIDER_S))
#define TIMG_T0_DIVIDER_V  0xFFFF
#define TIMG_T0_DIVIDER_S  13
/* TIMG_T0_DIVCNT_RST : WT ;bitpos:[12] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_DIVCNT_RST  (BIT(12))
#define TIMG_T0_DIVCNT_RST_M  (BIT(12))
#define TIMG_T0_DIVCNT_RST_V  0x1
#define TIMG_T0_DIVCNT_RST_S  12
/* TIMG_T0_ALARM_EN : R/W/SC ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_ALARM_EN  (BIT(10))
#define TIMG_T0_ALARM_EN_M  (BIT(10))
#define TIMG_T0_ALARM_EN_V  0x1
#define TIMG_T0_ALARM_EN_S  10
/* TIMG_T0_USE_XTAL : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define TIMG_T0_USE_XTAL  (BIT(9))
#define TIMG_T0_USE_XTAL_M  (BIT(9))
#define TIMG_T0_USE_XTAL_V  0x1
#define TIMG_T0_USE_XTAL_S  9

#define TIMG_T0LO_REG(i)          (REG_TIMG_BASE(i) + 0x0004)
/* TIMG_T0_LO : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define TIMG_T0_LO  0xFFFFFFFF
#define TIMG_T0_LO_M  ((TIMG_T0_LO_V)<<(TIMG_T0_LO_S))
#define TIMG_T0_LO_V  0xFFFFFFFF
#define TIMG_T0_LO_S  0

#define TIMG_T0HI_REG(i)          (REG_TIMG_BASE(i) + 0x0008)
/* TIMG_T0_HI : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define TIMG_T0_HI  0x003FFFFF
#define TIMG_T0_HI_M  ((TIMG_T0_HI_V)<<(TIMG_T0_HI_S))
#define TIMG_T0_HI_V  0x3FFFFF
#define TIMG_T0_HI_S  0

#define TIMG_T0UPDATE_REG(i)          (REG_TIMG_BASE(i) + 0x000c)
/* TIMG_T0_UPDATE : R/W/SC ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_UPDATE  (BIT(31))
#define TIMG_T0_UPDATE_M  (BIT(31))
#define TIMG_T0_UPDATE_V  0x1
#define TIMG_T0_UPDATE_S  31

#define TIMG_T0ALARMLO_REG(i)          (REG_TIMG_BASE(i) + 0x0010)
/* TIMG_T0_ALARM_LO : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define TIMG_T0_ALARM_LO  0xFFFFFFFF
#define TIMG_T0_ALARM_LO_M  ((TIMG_T0_ALARM_LO_V)<<(TIMG_T0_ALARM_LO_S))
#define TIMG_T0_ALARM_LO_V  0xFFFFFFFF
#define TIMG_T0_ALARM_LO_S  0

#define TIMG_T0ALARMHI_REG(i)          (REG_TIMG_BASE(i) + 0x0014)
/* TIMG_T0_ALARM_HI : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define TIMG_T0_ALARM_HI  0x003FFFFF
#define TIMG_T0_ALARM_HI_M  ((TIMG_T0_ALARM_HI_V)<<(TIMG_T0_ALARM_HI_S))
#define TIMG_T0_ALARM_HI_V  0x3FFFFF
#define TIMG_T0_ALARM_HI_S  0

#define TIMG_T0LOADLO_REG(i)          (REG_TIMG_BASE(i) + 0x0018)
/* TIMG_T0_LOAD_LO : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define TIMG_T0_LOAD_LO  0xFFFFFFFF
#define TIMG_T0_LOAD_LO_M  ((TIMG_T0_LOAD_LO_V)<<(TIMG_T0_LOAD_LO_S))
#define TIMG_T0_LOAD_LO_V  0xFFFFFFFF
#define TIMG_T0_LOAD_LO_S  0

#define TIMG_T0LOADHI_REG(i)          (REG_TIMG_BASE(i) + 0x001c)
/* TIMG_T0_LOAD_HI : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define TIMG_T0_LOAD_HI  0x003FFFFF
#define TIMG_T0_LOAD_HI_M  ((TIMG_T0_LOAD_HI_V)<<(TIMG_T0_LOAD_HI_S))
#define TIMG_T0_LOAD_HI_V  0x3FFFFF
#define TIMG_T0_LOAD_HI_S  0

#define TIMG_T0LOAD_REG(i)          (REG_TIMG_BASE(i) + 0x0020)
/* TIMG_T0_LOAD : WT ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define TIMG_T0_LOAD  0xFFFFFFFF
#define TIMG_T0_LOAD_M  ((TIMG_T0_LOAD_V)<<(TIMG_T0_LOAD_S))
#define TIMG_T0_LOAD_V  0xFFFFFFFF
#define TIMG_T0_LOAD_S  0

#define TIMG_WDTCONFIG0_REG(i)          (REG_TIMG_BASE(i) + 0x0048)
/* TIMG_WDT_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_EN  (BIT(31))
#define TIMG_WDT_EN_M  (BIT(31))
#define TIMG_WDT_EN_V  0x1
#define TIMG_WDT_EN_S  31
/* TIMG_WDT_STG0 : R/W ;bitpos:[30:29] ;default: 2'd0 ; */
/*description: */
#define TIMG_WDT_STG0  0x00000003
#define TIMG_WDT_STG0_M  ((TIMG_WDT_STG0_V)<<(TIMG_WDT_STG0_S))
#define TIMG_WDT_STG0_V  0x3
#define TIMG_WDT_STG0_S  29
/* TIMG_WDT_STG1 : R/W ;bitpos:[28:27] ;default: 2'd0 ; */
/*description: */
#define TIMG_WDT_STG1  0x00000003
#define TIMG_WDT_STG1_M  ((TIMG_WDT_STG1_V)<<(TIMG_WDT_STG1_S))
#define TIMG_WDT_STG1_V  0x3
#define TIMG_WDT_STG1_S  27
/* TIMG_WDT_STG2 : R/W ;bitpos:[26:25] ;default: 2'd0 ; */
/*description: */
#define TIMG_WDT_STG2  0x00000003
#define TIMG_WDT_STG2_M  ((TIMG_WDT_STG2_V)<<(TIMG_WDT_STG2_S))
#define TIMG_WDT_STG2_V  0x3
#define TIMG_WDT_STG2_S  25
/* TIMG_WDT_STG3 : R/W ;bitpos:[24:23] ;default: 2'd0 ; */
/*description: */
#define TIMG_WDT_STG3  0x00000003
#define TIMG_WDT_STG3_M  ((TIMG_WDT_STG3_V)<<(TIMG_WDT_STG3_S))
#define TIMG_WDT_STG3_V  0x3
#define TIMG_WDT_STG3_S  23
/* TIMG_WDT_CONF_UPDATE_EN : WT ;bitpos:[22] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_CONF_UPDATE_EN  (BIT(22))
#define TIMG_WDT_CONF_UPDATE_EN_M  (BIT(22))
#define TIMG_WDT_CONF_UPDATE_EN_V  0x1
#define TIMG_WDT_CONF_UPDATE_EN_S  22
/* TIMG_WDT_USE_XTAL : R/W ;bitpos:[21] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_USE_XTAL  (BIT(21))
#define TIMG_WDT_USE_XTAL_M  (BIT(21))
#define TIMG_WDT_USE_XTAL_V  0x1
#define TIMG_WDT_USE_XTAL_S  21
/* TIMG_WDT_CPU_RESET_LENGTH : R/W ;bitpos:[20:18] ;default: 3'h1 ; */
/*description: */
#define TIMG_WDT_CPU_RESET_LENGTH  0x00000007
#define TIMG_WDT_CPU_RESET_LENGTH_M  ((TIMG_WDT_CPU_RESET_LENGTH_V)<<(TIMG_WDT_CPU_RESET_LENGTH_S))
#define TIMG_WDT_CPU_RESET_LENGTH_V  0x7
#define TIMG_WDT_CPU_RESET_LENGTH_S  18
/* TIMG_WDT_SYS_RESET_LENGTH : R/W ;bitpos:[17:15] ;default: 3'h1 ; */
/*description: */
#define TIMG_WDT_SYS_RESET_LENGTH  0x00000007
#define TIMG_WDT_SYS_RESET_LENGTH_M  ((TIMG_WDT_SYS_RESET_LENGTH_V)<<(TIMG_WDT_SYS_RESET_LENGTH_S))
#define TIMG_WDT_SYS_RESET_LENGTH_V  0x7
#define TIMG_WDT_SYS_RESET_LENGTH_S  15
/* TIMG_WDT_FLASHBOOT_MOD_EN : R/W ;bitpos:[14] ;default: 1'h1 ; */
/*description: */
#define TIMG_WDT_FLASHBOOT_MOD_EN  (BIT(14))
#define TIMG_WDT_FLASHBOOT_MOD_EN_M  (BIT(14))
#define TIMG_WDT_FLASHBOOT_MOD_EN_V  0x1
#define TIMG_WDT_FLASHBOOT_MOD_EN_S  14
/* TIMG_WDT_PROCPU_RESET_EN : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define TIMG_WDT_PROCPU_RESET_EN  (BIT(13))
#define TIMG_WDT_PROCPU_RESET_EN_M  (BIT(13))
#define TIMG_WDT_PROCPU_RESET_EN_V  0x1
#define TIMG_WDT_PROCPU_RESET_EN_S  13
/* TIMG_WDT_APPCPU_RESET_EN : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define TIMG_WDT_APPCPU_RESET_EN  (BIT(12))
#define TIMG_WDT_APPCPU_RESET_EN_M  (BIT(12))
#define TIMG_WDT_APPCPU_RESET_EN_V  0x1
#define TIMG_WDT_APPCPU_RESET_EN_S  12

#define TIMG_WDTCONFIG1_REG(i)          (REG_TIMG_BASE(i) + 0x004c)
/* TIMG_WDT_CLK_PRESCALE : R/W ;bitpos:[31:16] ;default: 16'h1 ; */
/*description: */
#define TIMG_WDT_CLK_PRESCALE  0x0000FFFF
#define TIMG_WDT_CLK_PRESCALE_M  ((TIMG_WDT_CLK_PRESCALE_V)<<(TIMG_WDT_CLK_PRESCALE_S))
#define TIMG_WDT_CLK_PRESCALE_V  0xFFFF
#define TIMG_WDT_CLK_PRESCALE_S  16
/* TIMG_WDT_DIVCNT_RST : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define TIMG_WDT_DIVCNT_RST  (BIT(0))
#define TIMG_WDT_DIVCNT_RST_M  (BIT(0))
#define TIMG_WDT_DIVCNT_RST_V  0x1
#define TIMG_WDT_DIVCNT_RST_S  0

#define TIMG_WDTCONFIG2_REG(i)          (REG_TIMG_BASE(i) + 0x0050)
/* TIMG_WDT_STG0_HOLD : R/W ;bitpos:[31:0] ;default: 32'd26000000 ; */
/*description: */
#define TIMG_WDT_STG0_HOLD  0xFFFFFFFF
#define TIMG_WDT_STG0_HOLD_M  ((TIMG_WDT_STG0_HOLD_V)<<(TIMG_WDT_STG0_HOLD_S))
#define TIMG_WDT_STG0_HOLD_V  0xFFFFFFFF
#define TIMG_WDT_STG0_HOLD_S  0

#define TIMG_WDTCONFIG3_REG(i)          (REG_TIMG_BASE(i) + 0x0054)
/* TIMG_WDT_STG1_HOLD : R/W ;bitpos:[31:0] ;default: 32'h7ffffff ; */
/*description: */
#define TIMG_WDT_STG1_HOLD  0xFFFFFFFF
#define TIMG_WDT_STG1_HOLD_M  ((TIMG_WDT_STG1_HOLD_V)<<(TIMG_WDT_STG1_HOLD_S))
#define TIMG_WDT_STG1_HOLD_V  0xFFFFFFFF
#define TIMG_WDT_STG1_HOLD_S  0

#define TIMG_WDTCONFIG4_REG(i)          (REG_TIMG_BASE(i) + 0x0058)
/* TIMG_WDT_STG2_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfffff ; */
/*description: */
#define TIMG_WDT_STG2_HOLD  0xFFFFFFFF
#define TIMG_WDT_STG2_HOLD_M  ((TIMG_WDT_STG2_HOLD_V)<<(TIMG_WDT_STG2_HOLD_S))
#define TIMG_WDT_STG2_HOLD_V  0xFFFFFFFF
#define TIMG_WDT_STG2_HOLD_S  0

#define TIMG_WDTCONFIG5_REG(i)          (REG_TIMG_BASE(i) + 0x005c)
/* TIMG_WDT_STG3_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfffff ; */
/*description: */
#define TIMG_WDT_STG3_HOLD  0xFFFFFFFF
#define TIMG_WDT_STG3_HOLD_M  ((TIMG_WDT_STG3_HOLD_V)<<(TIMG_WDT_STG3_HOLD_S))
#define TIMG_WDT_STG3_HOLD_V  0xFFFFFFFF
#define TIMG_WDT_STG3_HOLD_S  0

#define TIMG_WDTFEED_REG(i)          (REG_TIMG_BASE(i) + 0x0060)
/* TIMG_WDT_FEED : WT ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define TIMG_WDT_FEED  0xFFFFFFFF
#define TIMG_WDT_FEED_M  ((TIMG_WDT_FEED_V)<<(TIMG_WDT_FEED_S))
#define TIMG_WDT_FEED_V  0xFFFFFFFF
#define TIMG_WDT_FEED_S  0

#define TIMG_WDTWPROTECT_REG(i)          (REG_TIMG_BASE(i) + 0x0064)
/* TIMG_WDT_WKEY : R/W ;bitpos:[31:0] ;default: 32'h50d83aa1 ; */
/*description: */
#define TIMG_WDT_WKEY  0xFFFFFFFF
#define TIMG_WDT_WKEY_M  ((TIMG_WDT_WKEY_V)<<(TIMG_WDT_WKEY_S))
#define TIMG_WDT_WKEY_V  0xFFFFFFFF
#define TIMG_WDT_WKEY_S  0

#define TIMG_RTCCALICFG_REG(i)          (REG_TIMG_BASE(i) + 0x0068)
/* TIMG_RTC_CALI_START : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define TIMG_RTC_CALI_START  (BIT(31))
#define TIMG_RTC_CALI_START_M  (BIT(31))
#define TIMG_RTC_CALI_START_V  0x1
#define TIMG_RTC_CALI_START_S  31
/* TIMG_RTC_CALI_MAX : R/W ;bitpos:[30:16] ;default: 15'h1 ; */
/*description: */
#define TIMG_RTC_CALI_MAX  0x00007FFF
#define TIMG_RTC_CALI_MAX_M  ((TIMG_RTC_CALI_MAX_V)<<(TIMG_RTC_CALI_MAX_S))
#define TIMG_RTC_CALI_MAX_V  0x7FFF
#define TIMG_RTC_CALI_MAX_S  16
/* TIMG_RTC_CALI_RDY : RO ;bitpos:[15] ;default: 1'h0 ; */
/*description: */
#define TIMG_RTC_CALI_RDY  (BIT(15))
#define TIMG_RTC_CALI_RDY_M  (BIT(15))
#define TIMG_RTC_CALI_RDY_V  0x1
#define TIMG_RTC_CALI_RDY_S  15
/* TIMG_RTC_CALI_CLK_SEL : R/W ;bitpos:[14:13] ;default: 2'h1 ; */
/*description: */
#define TIMG_RTC_CALI_CLK_SEL  0x00000003
#define TIMG_RTC_CALI_CLK_SEL_M  ((TIMG_RTC_CALI_CLK_SEL_V)<<(TIMG_RTC_CALI_CLK_SEL_S))
#define TIMG_RTC_CALI_CLK_SEL_V  0x3
#define TIMG_RTC_CALI_CLK_SEL_S  13
/* TIMG_RTC_CALI_START_CYCLING : R/W ;bitpos:[12] ;default: 1'd1 ; */
/*description: */
#define TIMG_RTC_CALI_START_CYCLING  (BIT(12))
#define TIMG_RTC_CALI_START_CYCLING_M  (BIT(12))
#define TIMG_RTC_CALI_START_CYCLING_V  0x1
#define TIMG_RTC_CALI_START_CYCLING_S  12

#define TIMG_RTCCALICFG1_REG(i)          (REG_TIMG_BASE(i) + 0x006c)
/* TIMG_RTC_CALI_VALUE : RO ;bitpos:[31:7] ;default: 25'h0 ; */
/*description: */
#define TIMG_RTC_CALI_VALUE  0x01FFFFFF
#define TIMG_RTC_CALI_VALUE_M  ((TIMG_RTC_CALI_VALUE_V)<<(TIMG_RTC_CALI_VALUE_S))
#define TIMG_RTC_CALI_VALUE_V  0x1FFFFFF
#define TIMG_RTC_CALI_VALUE_S  7
/* TIMG_RTC_CALI_CYCLING_DATA_VLD : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define TIMG_RTC_CALI_CYCLING_DATA_VLD  (BIT(0))
#define TIMG_RTC_CALI_CYCLING_DATA_VLD_M  (BIT(0))
#define TIMG_RTC_CALI_CYCLING_DATA_VLD_V  0x1
#define TIMG_RTC_CALI_CYCLING_DATA_VLD_S  0

#define TIMG_INT_ENA_TIMERS_REG(i)          (REG_TIMG_BASE(i) + 0x0070)
/* TIMG_WDT_INT_ENA : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_INT_ENA  (BIT(1))
#define TIMG_WDT_INT_ENA_M  (BIT(1))
#define TIMG_WDT_INT_ENA_V  0x1
#define TIMG_WDT_INT_ENA_S  1
/* TIMG_T0_INT_ENA : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_INT_ENA  (BIT(0))
#define TIMG_T0_INT_ENA_M  (BIT(0))
#define TIMG_T0_INT_ENA_V  0x1
#define TIMG_T0_INT_ENA_S  0

#define TIMG_INT_RAW_TIMERS_REG(i)          (REG_TIMG_BASE(i) + 0x0074)
/* TIMG_WDT_INT_RAW : R/SS/WTC ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_INT_RAW  (BIT(1))
#define TIMG_WDT_INT_RAW_M  (BIT(1))
#define TIMG_WDT_INT_RAW_V  0x1
#define TIMG_WDT_INT_RAW_S  1
/* TIMG_T0_INT_RAW : R/SS/WTC ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_INT_RAW  (BIT(0))
#define TIMG_T0_INT_RAW_M  (BIT(0))
#define TIMG_T0_INT_RAW_V  0x1
#define TIMG_T0_INT_RAW_S  0

#define TIMG_INT_ST_TIMERS_REG(i)          (REG_TIMG_BASE(i) + 0x0078)
/* TIMG_WDT_INT_ST : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_INT_ST  (BIT(1))
#define TIMG_WDT_INT_ST_M  (BIT(1))
#define TIMG_WDT_INT_ST_V  0x1
#define TIMG_WDT_INT_ST_S  1
/* TIMG_T0_INT_ST : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_INT_ST  (BIT(0))
#define TIMG_T0_INT_ST_M  (BIT(0))
#define TIMG_T0_INT_ST_V  0x1
#define TIMG_T0_INT_ST_S  0

#define TIMG_INT_CLR_TIMERS_REG(i)          (REG_TIMG_BASE(i) + 0x007c)
/* TIMG_WDT_INT_CLR : WT ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define TIMG_WDT_INT_CLR  (BIT(1))
#define TIMG_WDT_INT_CLR_M  (BIT(1))
#define TIMG_WDT_INT_CLR_V  0x1
#define TIMG_WDT_INT_CLR_S  1
/* TIMG_T0_INT_CLR : WT ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define TIMG_T0_INT_CLR  (BIT(0))
#define TIMG_T0_INT_CLR_M  (BIT(0))
#define TIMG_T0_INT_CLR_V  0x1
#define TIMG_T0_INT_CLR_S  0

#define TIMG_RTCCALICFG2_REG(i)          (REG_TIMG_BASE(i) + 0x0080)
/* TIMG_RTC_CALI_TIMEOUT_THRES : R/W ;bitpos:[31:7] ;default: 25'h1ffffff ; */
/*description: timeout if cali value counts over threshold*/
#define TIMG_RTC_CALI_TIMEOUT_THRES  0x01FFFFFF
#define TIMG_RTC_CALI_TIMEOUT_THRES_M  ((TIMG_RTC_CALI_TIMEOUT_THRES_V)<<(TIMG_RTC_CALI_TIMEOUT_THRES_S))
#define TIMG_RTC_CALI_TIMEOUT_THRES_V  0x1FFFFFF
#define TIMG_RTC_CALI_TIMEOUT_THRES_S  7
/* TIMG_RTC_CALI_TIMEOUT_RST_CNT : R/W ;bitpos:[6:3] ;default: 4'd3 ; */
/*description: Cycles that release calibration timeout reset*/
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT  0x0000000F
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT_M  ((TIMG_RTC_CALI_TIMEOUT_RST_CNT_V)<<(TIMG_RTC_CALI_TIMEOUT_RST_CNT_S))
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT_V  0xF
#define TIMG_RTC_CALI_TIMEOUT_RST_CNT_S  3
/* TIMG_RTC_CALI_TIMEOUT : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: timeout indicator*/
#define TIMG_RTC_CALI_TIMEOUT  (BIT(0))
#define TIMG_RTC_CALI_TIMEOUT_M  (BIT(0))
#define TIMG_RTC_CALI_TIMEOUT_V  0x1
#define TIMG_RTC_CALI_TIMEOUT_S  0

#define TIMG_NTIMERS_DATE_REG(i)          (REG_TIMG_BASE(i) + 0x00f8)
/* TIMG_NTIMERS_DATE : R/W ;bitpos:[27:0] ;default: 28'h2006191 ; */
/*description: */
#define TIMG_NTIMERS_DATE  0x0FFFFFFF
#define TIMG_NTIMERS_DATE_M  ((TIMG_NTIMERS_DATE_V)<<(TIMG_NTIMERS_DATE_S))
#define TIMG_NTIMERS_DATE_V  0xFFFFFFF
#define TIMG_NTIMERS_DATE_S  0

#define TIMG_CLK_REG(i)          (REG_TIMG_BASE(i) + 0x00fc)
/* TIMG_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define TIMG_CLK_EN  (BIT(31))
#define TIMG_CLK_EN_M  (BIT(31))
#define TIMG_CLK_EN_V  0x1
#define TIMG_CLK_EN_S  31
/* TIMG_TIMER_CLK_IS_ACTIVE : R/W ;bitpos:[30] ;default: 1'h1 ; */
/*description: */
#define TIMG_TIMER_CLK_IS_ACTIVE  (BIT(30))
#define TIMG_TIMER_CLK_IS_ACTIVE_M  (BIT(30))
#define TIMG_TIMER_CLK_IS_ACTIVE_V  0x1
#define TIMG_TIMER_CLK_IS_ACTIVE_S  30
/* TIMG_WDT_CLK_IS_ACTIVE : R/W ;bitpos:[29] ;default: 1'h1 ; */
/*description: */
#define TIMG_WDT_CLK_IS_ACTIVE  (BIT(29))
#define TIMG_WDT_CLK_IS_ACTIVE_M  (BIT(29))
#define TIMG_WDT_CLK_IS_ACTIVE_V  0x1
#define TIMG_WDT_CLK_IS_ACTIVE_S  29

#ifdef __cplusplus
}
#endif



#endif /*_SOC_TIMG_REG_H_ */
