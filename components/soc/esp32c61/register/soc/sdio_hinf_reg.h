/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HINF_CFG_DATA0_REG register
 *  Configure sdio cis content
 */
#define HINF_CFG_DATA0_REG (DR_REG_HINF_BASE + 0x0)
/** HINF_DEVICE_ID_FN1 : R/W; bitpos: [15:0]; default: 26214;
 *  configure device id of function1 in cis
 */
#define HINF_DEVICE_ID_FN1    0x0000FFFFU
#define HINF_DEVICE_ID_FN1_M  (HINF_DEVICE_ID_FN1_V << HINF_DEVICE_ID_FN1_S)
#define HINF_DEVICE_ID_FN1_V  0x0000FFFFU
#define HINF_DEVICE_ID_FN1_S  0
/** HINF_USER_ID_FN1 : R/W; bitpos: [31:16]; default: 146;
 *  configure user id of function1 in cis
 */
#define HINF_USER_ID_FN1    0x0000FFFFU
#define HINF_USER_ID_FN1_M  (HINF_USER_ID_FN1_V << HINF_USER_ID_FN1_S)
#define HINF_USER_ID_FN1_V  0x0000FFFFU
#define HINF_USER_ID_FN1_S  16

/** HINF_CFG_DATA1_REG register
 *  SDIO configuration register
 */
#define HINF_CFG_DATA1_REG (DR_REG_HINF_BASE + 0x4)
/** HINF_SDIO_ENABLE : R/W; bitpos: [0]; default: 1;
 *  Sdio clock enable
 */
#define HINF_SDIO_ENABLE    (BIT(0))
#define HINF_SDIO_ENABLE_M  (HINF_SDIO_ENABLE_V << HINF_SDIO_ENABLE_S)
#define HINF_SDIO_ENABLE_V  0x00000001U
#define HINF_SDIO_ENABLE_S  0
/** HINF_SDIO_IOREADY1 : R/W; bitpos: [1]; default: 0;
 *  sdio function1 io ready signal in cis
 */
#define HINF_SDIO_IOREADY1    (BIT(1))
#define HINF_SDIO_IOREADY1_M  (HINF_SDIO_IOREADY1_V << HINF_SDIO_IOREADY1_S)
#define HINF_SDIO_IOREADY1_V  0x00000001U
#define HINF_SDIO_IOREADY1_S  1
/** HINF_HIGHSPEED_ENABLE : R/W; bitpos: [2]; default: 0;
 *  Highspeed enable in cccr
 */
#define HINF_HIGHSPEED_ENABLE    (BIT(2))
#define HINF_HIGHSPEED_ENABLE_M  (HINF_HIGHSPEED_ENABLE_V << HINF_HIGHSPEED_ENABLE_S)
#define HINF_HIGHSPEED_ENABLE_V  0x00000001U
#define HINF_HIGHSPEED_ENABLE_S  2
/** HINF_HIGHSPEED_MODE : RO; bitpos: [3]; default: 0;
 *  highspeed mode status in cccr
 */
#define HINF_HIGHSPEED_MODE    (BIT(3))
#define HINF_HIGHSPEED_MODE_M  (HINF_HIGHSPEED_MODE_V << HINF_HIGHSPEED_MODE_S)
#define HINF_HIGHSPEED_MODE_V  0x00000001U
#define HINF_HIGHSPEED_MODE_S  3
/** HINF_SDIO_CD_ENABLE : R/W; bitpos: [4]; default: 1;
 *  sdio card detect enable
 */
#define HINF_SDIO_CD_ENABLE    (BIT(4))
#define HINF_SDIO_CD_ENABLE_M  (HINF_SDIO_CD_ENABLE_V << HINF_SDIO_CD_ENABLE_S)
#define HINF_SDIO_CD_ENABLE_V  0x00000001U
#define HINF_SDIO_CD_ENABLE_S  4
/** HINF_SDIO_IOREADY2 : R/W; bitpos: [5]; default: 0;
 *  sdio function1 io ready signal in cis
 */
#define HINF_SDIO_IOREADY2    (BIT(5))
#define HINF_SDIO_IOREADY2_M  (HINF_SDIO_IOREADY2_V << HINF_SDIO_IOREADY2_S)
#define HINF_SDIO_IOREADY2_V  0x00000001U
#define HINF_SDIO_IOREADY2_S  5
/** HINF_SDIO_INT_MASK : R/W; bitpos: [6]; default: 0;
 *  mask sdio interrupt in cccr, high active
 */
#define HINF_SDIO_INT_MASK    (BIT(6))
#define HINF_SDIO_INT_MASK_M  (HINF_SDIO_INT_MASK_V << HINF_SDIO_INT_MASK_S)
#define HINF_SDIO_INT_MASK_V  0x00000001U
#define HINF_SDIO_INT_MASK_S  6
/** HINF_IOENABLE2 : RO; bitpos: [7]; default: 0;
 *  ioe2 status in cccr
 */
#define HINF_IOENABLE2    (BIT(7))
#define HINF_IOENABLE2_M  (HINF_IOENABLE2_V << HINF_IOENABLE2_S)
#define HINF_IOENABLE2_V  0x00000001U
#define HINF_IOENABLE2_S  7
/** HINF_CD_DISABLE : RO; bitpos: [8]; default: 0;
 *  card disable status in cccr
 */
#define HINF_CD_DISABLE    (BIT(8))
#define HINF_CD_DISABLE_M  (HINF_CD_DISABLE_V << HINF_CD_DISABLE_S)
#define HINF_CD_DISABLE_V  0x00000001U
#define HINF_CD_DISABLE_S  8
/** HINF_FUNC1_EPS : RO; bitpos: [9]; default: 0;
 *  function1 eps status in fbr
 */
#define HINF_FUNC1_EPS    (BIT(9))
#define HINF_FUNC1_EPS_M  (HINF_FUNC1_EPS_V << HINF_FUNC1_EPS_S)
#define HINF_FUNC1_EPS_V  0x00000001U
#define HINF_FUNC1_EPS_S  9
/** HINF_EMP : RO; bitpos: [10]; default: 0;
 *  empc status in cccr
 */
#define HINF_EMP    (BIT(10))
#define HINF_EMP_M  (HINF_EMP_V << HINF_EMP_S)
#define HINF_EMP_V  0x00000001U
#define HINF_EMP_S  10
/** HINF_IOENABLE1 : RO; bitpos: [11]; default: 0;
 *  ioe1 status in cccr
 */
#define HINF_IOENABLE1    (BIT(11))
#define HINF_IOENABLE1_M  (HINF_IOENABLE1_V << HINF_IOENABLE1_S)
#define HINF_IOENABLE1_V  0x00000001U
#define HINF_IOENABLE1_S  11
/** HINF_SDIO_VER : R/W; bitpos: [23:12]; default: 562;
 *  sdio version in cccr
 */
#define HINF_SDIO_VER    0x00000FFFU
#define HINF_SDIO_VER_M  (HINF_SDIO_VER_V << HINF_SDIO_VER_S)
#define HINF_SDIO_VER_V  0x00000FFFU
#define HINF_SDIO_VER_S  12
/** HINF_FUNC2_EPS : RO; bitpos: [24]; default: 0;
 *  function2 eps status in fbr
 */
#define HINF_FUNC2_EPS    (BIT(24))
#define HINF_FUNC2_EPS_M  (HINF_FUNC2_EPS_V << HINF_FUNC2_EPS_S)
#define HINF_FUNC2_EPS_V  0x00000001U
#define HINF_FUNC2_EPS_S  24
/** HINF_SDIO20_CONF : R/W; bitpos: [31:25]; default: 0;
 *  [29],sdio negedge sample enablel.[30],sdio posedge sample enable.[31],sdio cmd/dat
 *  in delayed cycles control,0:no delay, 1:delay 1 cycle.
 *  [25]: sdio1.1 dat/cmd sending out edge control,1:negedge,0:posedge when highseed
 *  mode.
 *  [26]: sdio2.0 dat/cmd sending out edge control,1:negedge when [12]=0,0:negedge when
 *  [12]=0,posedge when highspeed mode enable.
 *  [27]: sdio interrupt sending out delay control,1:delay one cycle, 0: no delay.
 *  [28]: sdio data pad pull up enable
 */
#define HINF_SDIO20_CONF    0x0000007FU
#define HINF_SDIO20_CONF_M  (HINF_SDIO20_CONF_V << HINF_SDIO20_CONF_S)
#define HINF_SDIO20_CONF_V  0x0000007FU
#define HINF_SDIO20_CONF_S  25

/** HINF_CFG_TIMING_REG register
 *  Timing configuration registers
 */
#define HINF_CFG_TIMING_REG (DR_REG_HINF_BASE + 0x8)
/** HINF_NCRC : R/W; bitpos: [2:0]; default: 2;
 *  configure Ncrc parameter in sdr50/104 mode, no more than 6.
 */
#define HINF_NCRC    0x00000007U
#define HINF_NCRC_M  (HINF_NCRC_V << HINF_NCRC_S)
#define HINF_NCRC_V  0x00000007U
#define HINF_NCRC_S  0
/** HINF_PST_END_CMD_LOW_VALUE : R/W; bitpos: [9:3]; default: 2;
 *  configure cycles to lower cmd after voltage is changed to 1.8V.
 */
#define HINF_PST_END_CMD_LOW_VALUE    0x0000007FU
#define HINF_PST_END_CMD_LOW_VALUE_M  (HINF_PST_END_CMD_LOW_VALUE_V << HINF_PST_END_CMD_LOW_VALUE_S)
#define HINF_PST_END_CMD_LOW_VALUE_V  0x0000007FU
#define HINF_PST_END_CMD_LOW_VALUE_S  3
/** HINF_PST_END_DATA_LOW_VALUE : R/W; bitpos: [15:10]; default: 2;
 *  configure cycles to lower data after voltage is changed to 1.8V.
 */
#define HINF_PST_END_DATA_LOW_VALUE    0x0000003FU
#define HINF_PST_END_DATA_LOW_VALUE_M  (HINF_PST_END_DATA_LOW_VALUE_V << HINF_PST_END_DATA_LOW_VALUE_S)
#define HINF_PST_END_DATA_LOW_VALUE_V  0x0000003FU
#define HINF_PST_END_DATA_LOW_VALUE_S  10
/** HINF_SDCLK_STOP_THRES : R/W; bitpos: [26:16]; default: 1400;
 *  Configure the number of cycles of module clk to judge sdclk has stopped
 */
#define HINF_SDCLK_STOP_THRES    0x000007FFU
#define HINF_SDCLK_STOP_THRES_M  (HINF_SDCLK_STOP_THRES_V << HINF_SDCLK_STOP_THRES_S)
#define HINF_SDCLK_STOP_THRES_V  0x000007FFU
#define HINF_SDCLK_STOP_THRES_S  16
/** HINF_SAMPLE_CLK_DIVIDER : R/W; bitpos: [31:28]; default: 1;
 *  module clk divider to sample sdclk
 */
#define HINF_SAMPLE_CLK_DIVIDER    0x0000000FU
#define HINF_SAMPLE_CLK_DIVIDER_M  (HINF_SAMPLE_CLK_DIVIDER_V << HINF_SAMPLE_CLK_DIVIDER_S)
#define HINF_SAMPLE_CLK_DIVIDER_V  0x0000000FU
#define HINF_SAMPLE_CLK_DIVIDER_S  28

/** HINF_CFG_UPDATE_REG register
 *  update sdio configurations
 */
#define HINF_CFG_UPDATE_REG (DR_REG_HINF_BASE + 0xc)
/** HINF_CONF_UPDATE : WT; bitpos: [0]; default: 0;
 *  update the timing configurations
 */
#define HINF_CONF_UPDATE    (BIT(0))
#define HINF_CONF_UPDATE_M  (HINF_CONF_UPDATE_V << HINF_CONF_UPDATE_S)
#define HINF_CONF_UPDATE_V  0x00000001U
#define HINF_CONF_UPDATE_S  0

/** HINF_CFG_DATA7_REG register
 *  SDIO configuration register
 */
#define HINF_CFG_DATA7_REG (DR_REG_HINF_BASE + 0x1c)
/** HINF_PIN_STATE : R/W; bitpos: [7:0]; default: 0;
 *  configure cis addr 318 and 574
 */
#define HINF_PIN_STATE    0x000000FFU
#define HINF_PIN_STATE_M  (HINF_PIN_STATE_V << HINF_PIN_STATE_S)
#define HINF_PIN_STATE_V  0x000000FFU
#define HINF_PIN_STATE_S  0
/** HINF_CHIP_STATE : R/W; bitpos: [15:8]; default: 0;
 *  configure cis addr 312, 315, 568 and 571
 */
#define HINF_CHIP_STATE    0x000000FFU
#define HINF_CHIP_STATE_M  (HINF_CHIP_STATE_V << HINF_CHIP_STATE_S)
#define HINF_CHIP_STATE_V  0x000000FFU
#define HINF_CHIP_STATE_S  8
/** HINF_SDIO_RST : R/W; bitpos: [16]; default: 0;
 *  soft reset control for sdio module
 */
#define HINF_SDIO_RST    (BIT(16))
#define HINF_SDIO_RST_M  (HINF_SDIO_RST_V << HINF_SDIO_RST_S)
#define HINF_SDIO_RST_V  0x00000001U
#define HINF_SDIO_RST_S  16
/** HINF_SDIO_IOREADY0 : R/W; bitpos: [17]; default: 1;
 *  sdio io ready, high enable
 */
#define HINF_SDIO_IOREADY0    (BIT(17))
#define HINF_SDIO_IOREADY0_M  (HINF_SDIO_IOREADY0_V << HINF_SDIO_IOREADY0_S)
#define HINF_SDIO_IOREADY0_V  0x00000001U
#define HINF_SDIO_IOREADY0_S  17
/** HINF_SDIO_MEM_PD : R/W; bitpos: [18]; default: 0;
 *  sdio memory power down, high active
 */
#define HINF_SDIO_MEM_PD    (BIT(18))
#define HINF_SDIO_MEM_PD_M  (HINF_SDIO_MEM_PD_V << HINF_SDIO_MEM_PD_S)
#define HINF_SDIO_MEM_PD_V  0x00000001U
#define HINF_SDIO_MEM_PD_S  18
/** HINF_ESDIO_DATA1_INT_EN : R/W; bitpos: [19]; default: 0;
 *  enable sdio interrupt on data1 line
 */
#define HINF_ESDIO_DATA1_INT_EN    (BIT(19))
#define HINF_ESDIO_DATA1_INT_EN_M  (HINF_ESDIO_DATA1_INT_EN_V << HINF_ESDIO_DATA1_INT_EN_S)
#define HINF_ESDIO_DATA1_INT_EN_V  0x00000001U
#define HINF_ESDIO_DATA1_INT_EN_S  19
/** HINF_SDIO_SWITCH_VOLT_SW : R/W; bitpos: [20]; default: 0;
 *  control switch voltage change to 1.8V by software. 0:3.3V,1:1.8V
 */
#define HINF_SDIO_SWITCH_VOLT_SW    (BIT(20))
#define HINF_SDIO_SWITCH_VOLT_SW_M  (HINF_SDIO_SWITCH_VOLT_SW_V << HINF_SDIO_SWITCH_VOLT_SW_S)
#define HINF_SDIO_SWITCH_VOLT_SW_V  0x00000001U
#define HINF_SDIO_SWITCH_VOLT_SW_S  20
/** HINF_DDR50_BLK_LEN_FIX_EN : R/W; bitpos: [21]; default: 0;
 *  enable block length to be fixed to 512 bytes in ddr50 mode
 */
#define HINF_DDR50_BLK_LEN_FIX_EN    (BIT(21))
#define HINF_DDR50_BLK_LEN_FIX_EN_M  (HINF_DDR50_BLK_LEN_FIX_EN_V << HINF_DDR50_BLK_LEN_FIX_EN_S)
#define HINF_DDR50_BLK_LEN_FIX_EN_V  0x00000001U
#define HINF_DDR50_BLK_LEN_FIX_EN_S  21
/** HINF_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  sdio apb clock for configuration force on control:0-gating,1-force on.
 */
#define HINF_CLK_EN    (BIT(22))
#define HINF_CLK_EN_M  (HINF_CLK_EN_V << HINF_CLK_EN_S)
#define HINF_CLK_EN_V  0x00000001U
#define HINF_CLK_EN_S  22
/** HINF_SDDR50 : R/W; bitpos: [23]; default: 1;
 *  configure if support sdr50 mode in cccr
 */
#define HINF_SDDR50    (BIT(23))
#define HINF_SDDR50_M  (HINF_SDDR50_V << HINF_SDDR50_S)
#define HINF_SDDR50_V  0x00000001U
#define HINF_SDDR50_S  23
/** HINF_SSDR104 : R/W; bitpos: [24]; default: 1;
 *  configure if support sdr104 mode in cccr
 */
#define HINF_SSDR104    (BIT(24))
#define HINF_SSDR104_M  (HINF_SSDR104_V << HINF_SSDR104_S)
#define HINF_SSDR104_V  0x00000001U
#define HINF_SSDR104_S  24
/** HINF_SSDR50 : R/W; bitpos: [25]; default: 1;
 *  configure if support ddr50 mode in cccr
 */
#define HINF_SSDR50    (BIT(25))
#define HINF_SSDR50_M  (HINF_SSDR50_V << HINF_SSDR50_S)
#define HINF_SSDR50_V  0x00000001U
#define HINF_SSDR50_S  25
/** HINF_SDTD : R/W; bitpos: [26]; default: 0;
 *  configure if support driver type D in cccr
 */
#define HINF_SDTD    (BIT(26))
#define HINF_SDTD_M  (HINF_SDTD_V << HINF_SDTD_S)
#define HINF_SDTD_V  0x00000001U
#define HINF_SDTD_S  26
/** HINF_SDTA : R/W; bitpos: [27]; default: 0;
 *  configure if support driver type A in cccr
 */
#define HINF_SDTA    (BIT(27))
#define HINF_SDTA_M  (HINF_SDTA_V << HINF_SDTA_S)
#define HINF_SDTA_V  0x00000001U
#define HINF_SDTA_S  27
/** HINF_SDTC : R/W; bitpos: [28]; default: 0;
 *  configure if support driver type C in cccr
 */
#define HINF_SDTC    (BIT(28))
#define HINF_SDTC_M  (HINF_SDTC_V << HINF_SDTC_S)
#define HINF_SDTC_V  0x00000001U
#define HINF_SDTC_S  28
/** HINF_SAI : R/W; bitpos: [29]; default: 1;
 *  configure if support asynchronous interrupt in cccr
 */
#define HINF_SAI    (BIT(29))
#define HINF_SAI_M  (HINF_SAI_V << HINF_SAI_S)
#define HINF_SAI_V  0x00000001U
#define HINF_SAI_S  29
/** HINF_SDIO_WAKEUP_CLR : R/W; bitpos: [30]; default: 0;
 *  clear sdio_wake_up signal after the chip wakes up
 */
#define HINF_SDIO_WAKEUP_CLR    (BIT(30))
#define HINF_SDIO_WAKEUP_CLR_M  (HINF_SDIO_WAKEUP_CLR_V << HINF_SDIO_WAKEUP_CLR_S)
#define HINF_SDIO_WAKEUP_CLR_V  0x00000001U
#define HINF_SDIO_WAKEUP_CLR_S  30

/** HINF_CIS_CONF_W0_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W0_REG (DR_REG_HINF_BASE + 0x20)
/** HINF_CIS_CONF_W0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 39~36
 */
#define HINF_CIS_CONF_W0    0xFFFFFFFFU
#define HINF_CIS_CONF_W0_M  (HINF_CIS_CONF_W0_V << HINF_CIS_CONF_W0_S)
#define HINF_CIS_CONF_W0_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W0_S  0

/** HINF_CIS_CONF_W1_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W1_REG (DR_REG_HINF_BASE + 0x24)
/** HINF_CIS_CONF_W1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 43~40
 */
#define HINF_CIS_CONF_W1    0xFFFFFFFFU
#define HINF_CIS_CONF_W1_M  (HINF_CIS_CONF_W1_V << HINF_CIS_CONF_W1_S)
#define HINF_CIS_CONF_W1_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W1_S  0

/** HINF_CIS_CONF_W2_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W2_REG (DR_REG_HINF_BASE + 0x28)
/** HINF_CIS_CONF_W2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 47~44
 */
#define HINF_CIS_CONF_W2    0xFFFFFFFFU
#define HINF_CIS_CONF_W2_M  (HINF_CIS_CONF_W2_V << HINF_CIS_CONF_W2_S)
#define HINF_CIS_CONF_W2_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W2_S  0

/** HINF_CIS_CONF_W3_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W3_REG (DR_REG_HINF_BASE + 0x2c)
/** HINF_CIS_CONF_W3 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 51~48
 */
#define HINF_CIS_CONF_W3    0xFFFFFFFFU
#define HINF_CIS_CONF_W3_M  (HINF_CIS_CONF_W3_V << HINF_CIS_CONF_W3_S)
#define HINF_CIS_CONF_W3_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W3_S  0

/** HINF_CIS_CONF_W4_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W4_REG (DR_REG_HINF_BASE + 0x30)
/** HINF_CIS_CONF_W4 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 55~52
 */
#define HINF_CIS_CONF_W4    0xFFFFFFFFU
#define HINF_CIS_CONF_W4_M  (HINF_CIS_CONF_W4_V << HINF_CIS_CONF_W4_S)
#define HINF_CIS_CONF_W4_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W4_S  0

/** HINF_CIS_CONF_W5_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W5_REG (DR_REG_HINF_BASE + 0x34)
/** HINF_CIS_CONF_W5 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 59~56
 */
#define HINF_CIS_CONF_W5    0xFFFFFFFFU
#define HINF_CIS_CONF_W5_M  (HINF_CIS_CONF_W5_V << HINF_CIS_CONF_W5_S)
#define HINF_CIS_CONF_W5_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W5_S  0

/** HINF_CIS_CONF_W6_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W6_REG (DR_REG_HINF_BASE + 0x38)
/** HINF_CIS_CONF_W6 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 63~60
 */
#define HINF_CIS_CONF_W6    0xFFFFFFFFU
#define HINF_CIS_CONF_W6_M  (HINF_CIS_CONF_W6_V << HINF_CIS_CONF_W6_S)
#define HINF_CIS_CONF_W6_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W6_S  0

/** HINF_CIS_CONF_W7_REG register
 *  SDIO cis configuration register
 */
#define HINF_CIS_CONF_W7_REG (DR_REG_HINF_BASE + 0x3c)
/** HINF_CIS_CONF_W7 : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configure cis addr 67~64
 */
#define HINF_CIS_CONF_W7    0xFFFFFFFFU
#define HINF_CIS_CONF_W7_M  (HINF_CIS_CONF_W7_V << HINF_CIS_CONF_W7_S)
#define HINF_CIS_CONF_W7_V  0xFFFFFFFFU
#define HINF_CIS_CONF_W7_S  0

/** HINF_CFG_DATA16_REG register
 *  SDIO cis configuration register
 */
#define HINF_CFG_DATA16_REG (DR_REG_HINF_BASE + 0x40)
/** HINF_DEVICE_ID_FN2 : R/W; bitpos: [15:0]; default: 30583;
 *  configure device id of function2 in cis
 */
#define HINF_DEVICE_ID_FN2    0x0000FFFFU
#define HINF_DEVICE_ID_FN2_M  (HINF_DEVICE_ID_FN2_V << HINF_DEVICE_ID_FN2_S)
#define HINF_DEVICE_ID_FN2_V  0x0000FFFFU
#define HINF_DEVICE_ID_FN2_S  0
/** HINF_USER_ID_FN2 : R/W; bitpos: [31:16]; default: 146;
 *  configure user id of function2 in cis
 */
#define HINF_USER_ID_FN2    0x0000FFFFU
#define HINF_USER_ID_FN2_M  (HINF_USER_ID_FN2_V << HINF_USER_ID_FN2_S)
#define HINF_USER_ID_FN2_V  0x0000FFFFU
#define HINF_USER_ID_FN2_S  16

/** HINF_CFG_UHS1_INT_MODE_REG register
 *  configure int to start and end ahead of time in uhs1 mode
 */
#define HINF_CFG_UHS1_INT_MODE_REG (DR_REG_HINF_BASE + 0x44)
/** HINF_INTOE_END_AHEAD_MODE : R/W; bitpos: [1:0]; default: 0;
 *  intoe on dat1 end ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
 */
#define HINF_INTOE_END_AHEAD_MODE    0x00000003U
#define HINF_INTOE_END_AHEAD_MODE_M  (HINF_INTOE_END_AHEAD_MODE_V << HINF_INTOE_END_AHEAD_MODE_S)
#define HINF_INTOE_END_AHEAD_MODE_V  0x00000003U
#define HINF_INTOE_END_AHEAD_MODE_S  0
/** HINF_INT_END_AHEAD_MODE : R/W; bitpos: [3:2]; default: 0;
 *  int on dat1 end ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
 */
#define HINF_INT_END_AHEAD_MODE    0x00000003U
#define HINF_INT_END_AHEAD_MODE_M  (HINF_INT_END_AHEAD_MODE_V << HINF_INT_END_AHEAD_MODE_S)
#define HINF_INT_END_AHEAD_MODE_V  0x00000003U
#define HINF_INT_END_AHEAD_MODE_S  2
/** HINF_INTOE_ST_AHEAD_MODE : R/W; bitpos: [5:4]; default: 0;
 *  intoe on dat1 start ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
 */
#define HINF_INTOE_ST_AHEAD_MODE    0x00000003U
#define HINF_INTOE_ST_AHEAD_MODE_M  (HINF_INTOE_ST_AHEAD_MODE_V << HINF_INTOE_ST_AHEAD_MODE_S)
#define HINF_INTOE_ST_AHEAD_MODE_V  0x00000003U
#define HINF_INTOE_ST_AHEAD_MODE_S  4
/** HINF_INT_ST_AHEAD_MODE : R/W; bitpos: [7:6]; default: 0;
 *  int on dat1 start ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
 */
#define HINF_INT_ST_AHEAD_MODE    0x00000003U
#define HINF_INT_ST_AHEAD_MODE_M  (HINF_INT_ST_AHEAD_MODE_V << HINF_INT_ST_AHEAD_MODE_S)
#define HINF_INT_ST_AHEAD_MODE_V  0x00000003U
#define HINF_INT_ST_AHEAD_MODE_S  6

/** HINF_CONF_STATUS_REG register
 *  func0 config0 status
 */
#define HINF_CONF_STATUS_REG (DR_REG_HINF_BASE + 0x54)
/** HINF_FUNC0_CONFIG0 : RO; bitpos: [7:0]; default: 0;
 *  func0 config0 (addr: 0x20f0 ) status
 */
#define HINF_FUNC0_CONFIG0    0x000000FFU
#define HINF_FUNC0_CONFIG0_M  (HINF_FUNC0_CONFIG0_V << HINF_FUNC0_CONFIG0_S)
#define HINF_FUNC0_CONFIG0_V  0x000000FFU
#define HINF_FUNC0_CONFIG0_S  0
/** HINF_SDR25_ST : RO; bitpos: [8]; default: 0;
 *  sdr25 status
 */
#define HINF_SDR25_ST    (BIT(8))
#define HINF_SDR25_ST_M  (HINF_SDR25_ST_V << HINF_SDR25_ST_S)
#define HINF_SDR25_ST_V  0x00000001U
#define HINF_SDR25_ST_S  8
/** HINF_SDR50_ST : RO; bitpos: [9]; default: 0;
 *  sdr50 status
 */
#define HINF_SDR50_ST    (BIT(9))
#define HINF_SDR50_ST_M  (HINF_SDR50_ST_V << HINF_SDR50_ST_S)
#define HINF_SDR50_ST_V  0x00000001U
#define HINF_SDR50_ST_S  9
/** HINF_SDR104_ST : RO; bitpos: [10]; default: 0;
 *  sdr104 status
 */
#define HINF_SDR104_ST    (BIT(10))
#define HINF_SDR104_ST_M  (HINF_SDR104_ST_V << HINF_SDR104_ST_S)
#define HINF_SDR104_ST_V  0x00000001U
#define HINF_SDR104_ST_S  10
/** HINF_DDR50_ST : RO; bitpos: [11]; default: 0;
 *  ddr50 status
 */
#define HINF_DDR50_ST    (BIT(11))
#define HINF_DDR50_ST_M  (HINF_DDR50_ST_V << HINF_DDR50_ST_S)
#define HINF_DDR50_ST_V  0x00000001U
#define HINF_DDR50_ST_S  11
/** HINF_TUNE_ST : RO; bitpos: [14:12]; default: 0;
 *  tune_st fsm status
 */
#define HINF_TUNE_ST    0x00000007U
#define HINF_TUNE_ST_M  (HINF_TUNE_ST_V << HINF_TUNE_ST_S)
#define HINF_TUNE_ST_V  0x00000007U
#define HINF_TUNE_ST_S  12
/** HINF_SDIO_SWITCH_VOLT_ST : RO; bitpos: [15]; default: 0;
 *  sdio switch voltage status:0-3.3V, 1-1.8V.
 */
#define HINF_SDIO_SWITCH_VOLT_ST    (BIT(15))
#define HINF_SDIO_SWITCH_VOLT_ST_M  (HINF_SDIO_SWITCH_VOLT_ST_V << HINF_SDIO_SWITCH_VOLT_ST_S)
#define HINF_SDIO_SWITCH_VOLT_ST_V  0x00000001U
#define HINF_SDIO_SWITCH_VOLT_ST_S  15
/** HINF_SDIO_SWITCH_END : RO; bitpos: [16]; default: 0;
 *  sdio switch voltage ldo ready
 */
#define HINF_SDIO_SWITCH_END    (BIT(16))
#define HINF_SDIO_SWITCH_END_M  (HINF_SDIO_SWITCH_END_V << HINF_SDIO_SWITCH_END_S)
#define HINF_SDIO_SWITCH_END_V  0x00000001U
#define HINF_SDIO_SWITCH_END_S  16

/** HINF_SDIO_SLAVE_LDO_CONF_REG register
 *  sdio slave ldo control register
 */
#define HINF_SDIO_SLAVE_LDO_CONF_REG (DR_REG_HINF_BASE + 0xb0)
/** HINF_LDO_READY_CTL_IN_EN : R/W; bitpos: [0]; default: 0;
 *  control ldo ready signal by sdio slave itself
 */
#define HINF_LDO_READY_CTL_IN_EN    (BIT(0))
#define HINF_LDO_READY_CTL_IN_EN_M  (HINF_LDO_READY_CTL_IN_EN_V << HINF_LDO_READY_CTL_IN_EN_S)
#define HINF_LDO_READY_CTL_IN_EN_V  0x00000001U
#define HINF_LDO_READY_CTL_IN_EN_S  0
/** HINF_LDO_READY_THRES : R/W; bitpos: [5:1]; default: 10;
 *  configure ldo ready counting threshold value, the actual counting target is
 *  2^(ldo_ready_thres)-1
 */
#define HINF_LDO_READY_THRES    0x0000001FU
#define HINF_LDO_READY_THRES_M  (HINF_LDO_READY_THRES_V << HINF_LDO_READY_THRES_S)
#define HINF_LDO_READY_THRES_V  0x0000001FU
#define HINF_LDO_READY_THRES_S  1
/** HINF_LDO_READY_IGNORE_EN : R/W; bitpos: [6]; default: 0;
 *  ignore ldo ready signal
 */
#define HINF_LDO_READY_IGNORE_EN    (BIT(6))
#define HINF_LDO_READY_IGNORE_EN_M  (HINF_LDO_READY_IGNORE_EN_V << HINF_LDO_READY_IGNORE_EN_S)
#define HINF_LDO_READY_IGNORE_EN_V  0x00000001U
#define HINF_LDO_READY_IGNORE_EN_S  6

/** HINF_SDIO_DATE_REG register
 *  ******* Description ***********
 */
#define HINF_SDIO_DATE_REG (DR_REG_HINF_BASE + 0xfc)
/** HINF_SDIO_DATE : R/W; bitpos: [31:0]; default: 37822544;
 *  sdio version date.
 */
#define HINF_SDIO_DATE    0xFFFFFFFFU
#define HINF_SDIO_DATE_M  (HINF_SDIO_DATE_V << HINF_SDIO_DATE_S)
#define HINF_SDIO_DATE_V  0xFFFFFFFFU
#define HINF_SDIO_DATE_S  0

#ifdef __cplusplus
}
#endif
