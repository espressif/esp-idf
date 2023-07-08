/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HUK_CLK_REG register
 *  HUK Generator clock gate control register
 */
#define HUK_CLK_REG (DR_REG_HUK_BASE + 0x4)
/** HUK_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Write 1 to force on register clock gate.
 */
#define HUK_CLK_EN    (BIT(0))
#define HUK_CLK_EN_M  (HUK_CLK_EN_V << HUK_CLK_EN_S)
#define HUK_CLK_EN_V  0x00000001U
#define HUK_CLK_EN_S  0
/** HUK_MEM_CG_FORCE_ON : R/W; bitpos: [1]; default: 0;
 *  Write 1 to force on memory clock gate.
 */
#define HUK_MEM_CG_FORCE_ON    (BIT(1))
#define HUK_MEM_CG_FORCE_ON_M  (HUK_MEM_CG_FORCE_ON_V << HUK_MEM_CG_FORCE_ON_S)
#define HUK_MEM_CG_FORCE_ON_V  0x00000001U
#define HUK_MEM_CG_FORCE_ON_S  1

/** HUK_INT_RAW_REG register
 *  HUK Generator interrupt raw register, valid in level.
 */
#define HUK_INT_RAW_REG (DR_REG_HUK_BASE + 0x8)
/** HUK_PREP_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status bit  for the huk_prep_done_int interrupt
 */
#define HUK_PREP_DONE_INT_RAW    (BIT(0))
#define HUK_PREP_DONE_INT_RAW_M  (HUK_PREP_DONE_INT_RAW_V << HUK_PREP_DONE_INT_RAW_S)
#define HUK_PREP_DONE_INT_RAW_V  0x00000001U
#define HUK_PREP_DONE_INT_RAW_S  0
/** HUK_PROC_DONE_INT_RAW : RO/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status bit  for the huk_proc_done_int interrupt
 */
#define HUK_PROC_DONE_INT_RAW    (BIT(1))
#define HUK_PROC_DONE_INT_RAW_M  (HUK_PROC_DONE_INT_RAW_V << HUK_PROC_DONE_INT_RAW_S)
#define HUK_PROC_DONE_INT_RAW_V  0x00000001U
#define HUK_PROC_DONE_INT_RAW_S  1
/** HUK_POST_DONE_INT_RAW : RO/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status bit  for the huk_post_done_int interrupt
 */
#define HUK_POST_DONE_INT_RAW    (BIT(2))
#define HUK_POST_DONE_INT_RAW_M  (HUK_POST_DONE_INT_RAW_V << HUK_POST_DONE_INT_RAW_S)
#define HUK_POST_DONE_INT_RAW_V  0x00000001U
#define HUK_POST_DONE_INT_RAW_S  2

/** HUK_INT_ST_REG register
 *  HUK Generator interrupt status register.
 */
#define HUK_INT_ST_REG (DR_REG_HUK_BASE + 0xc)
/** HUK_PREP_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit  for the huk_prep_done_int interrupt
 */
#define HUK_PREP_DONE_INT_ST    (BIT(0))
#define HUK_PREP_DONE_INT_ST_M  (HUK_PREP_DONE_INT_ST_V << HUK_PREP_DONE_INT_ST_S)
#define HUK_PREP_DONE_INT_ST_V  0x00000001U
#define HUK_PREP_DONE_INT_ST_S  0
/** HUK_PROC_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit  for the huk_proc_done_int interrupt
 */
#define HUK_PROC_DONE_INT_ST    (BIT(1))
#define HUK_PROC_DONE_INT_ST_M  (HUK_PROC_DONE_INT_ST_V << HUK_PROC_DONE_INT_ST_S)
#define HUK_PROC_DONE_INT_ST_V  0x00000001U
#define HUK_PROC_DONE_INT_ST_S  1
/** HUK_POST_DONE_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit  for the huk_post_done_int interrupt
 */
#define HUK_POST_DONE_INT_ST    (BIT(2))
#define HUK_POST_DONE_INT_ST_M  (HUK_POST_DONE_INT_ST_V << HUK_POST_DONE_INT_ST_S)
#define HUK_POST_DONE_INT_ST_V  0x00000001U
#define HUK_POST_DONE_INT_ST_S  2

/** HUK_INT_ENA_REG register
 *  HUK Generator interrupt enable register.
 */
#define HUK_INT_ENA_REG (DR_REG_HUK_BASE + 0x10)
/** HUK_PREP_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit  for the huk_prep_done_int interrupt
 */
#define HUK_PREP_DONE_INT_ENA    (BIT(0))
#define HUK_PREP_DONE_INT_ENA_M  (HUK_PREP_DONE_INT_ENA_V << HUK_PREP_DONE_INT_ENA_S)
#define HUK_PREP_DONE_INT_ENA_V  0x00000001U
#define HUK_PREP_DONE_INT_ENA_S  0
/** HUK_PROC_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit  for the huk_proc_done_int interrupt
 */
#define HUK_PROC_DONE_INT_ENA    (BIT(1))
#define HUK_PROC_DONE_INT_ENA_M  (HUK_PROC_DONE_INT_ENA_V << HUK_PROC_DONE_INT_ENA_S)
#define HUK_PROC_DONE_INT_ENA_V  0x00000001U
#define HUK_PROC_DONE_INT_ENA_S  1
/** HUK_POST_DONE_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit  for the huk_post_done_int interrupt
 */
#define HUK_POST_DONE_INT_ENA    (BIT(2))
#define HUK_POST_DONE_INT_ENA_M  (HUK_POST_DONE_INT_ENA_V << HUK_POST_DONE_INT_ENA_S)
#define HUK_POST_DONE_INT_ENA_V  0x00000001U
#define HUK_POST_DONE_INT_ENA_S  2

/** HUK_INT_CLR_REG register
 *  HUK Generator interrupt clear register.
 */
#define HUK_INT_CLR_REG (DR_REG_HUK_BASE + 0x14)
/** HUK_PREP_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the huk_prep_done_int interrupt
 */
#define HUK_PREP_DONE_INT_CLR    (BIT(0))
#define HUK_PREP_DONE_INT_CLR_M  (HUK_PREP_DONE_INT_CLR_V << HUK_PREP_DONE_INT_CLR_S)
#define HUK_PREP_DONE_INT_CLR_V  0x00000001U
#define HUK_PREP_DONE_INT_CLR_S  0
/** HUK_PROC_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the huk_proc_done_int interrupt
 */
#define HUK_PROC_DONE_INT_CLR    (BIT(1))
#define HUK_PROC_DONE_INT_CLR_M  (HUK_PROC_DONE_INT_CLR_V << HUK_PROC_DONE_INT_CLR_S)
#define HUK_PROC_DONE_INT_CLR_V  0x00000001U
#define HUK_PROC_DONE_INT_CLR_S  1
/** HUK_POST_DONE_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the huk_post_done_int interrupt
 */
#define HUK_POST_DONE_INT_CLR    (BIT(2))
#define HUK_POST_DONE_INT_CLR_M  (HUK_POST_DONE_INT_CLR_V << HUK_POST_DONE_INT_CLR_S)
#define HUK_POST_DONE_INT_CLR_V  0x00000001U
#define HUK_POST_DONE_INT_CLR_S  2

/** HUK_CONF_REG register
 *  HUK Generator configuration register
 */
#define HUK_CONF_REG (DR_REG_HUK_BASE + 0x20)
/** HUK_MODE : R/W; bitpos: [0]; default: 0;
 *  Set this field to choose the huk process. 1: process huk generate mode. 0: process
 *  huk recovery mode.
 */
#define HUK_MODE    (BIT(0))
#define HUK_MODE_M  (HUK_MODE_V << HUK_MODE_S)
#define HUK_MODE_V  0x00000001U
#define HUK_MODE_S  0

/** HUK_START_REG register
 *  HUK Generator control register
 */
#define HUK_START_REG (DR_REG_HUK_BASE + 0x24)
/** HUK_START : WT; bitpos: [0]; default: 0;
 *  Write 1 to continue HUK Generator operation at LOAD/GAIN state.
 */
#define HUK_START    (BIT(0))
#define HUK_START_M  (HUK_START_V << HUK_START_S)
#define HUK_START_V  0x00000001U
#define HUK_START_S  0
/** HUK_CONTINUE : WT; bitpos: [1]; default: 0;
 *  Write 1 to start HUK Generator at IDLE state.
 */
#define HUK_CONTINUE    (BIT(1))
#define HUK_CONTINUE_M  (HUK_CONTINUE_V << HUK_CONTINUE_S)
#define HUK_CONTINUE_V  0x00000001U
#define HUK_CONTINUE_S  1

/** HUK_STATE_REG register
 *  HUK Generator state register
 */
#define HUK_STATE_REG (DR_REG_HUK_BASE + 0x28)
/** HUK_STATE : RO; bitpos: [1:0]; default: 0;
 *  The state of HUK Generator. 0: IDLE. 1: LOAD. 2: GAIN. 3: BUSY.
 */
#define HUK_STATE    0x00000003U
#define HUK_STATE_M  (HUK_STATE_V << HUK_STATE_S)
#define HUK_STATE_V  0x00000003U
#define HUK_STATE_S  0

/** HUK_STATUS_REG register
 *  HUK Generator HUK status register
 */
#define HUK_STATUS_REG (DR_REG_HUK_BASE + 0x34)
/** HUK_STATUS : RO; bitpos: [1:0]; default: 0;
 *  The HUK generation status. 0: HUK is not generated. 1: HUK is generated and valid.
 *  2: HUK is generated but invalid. 3: reserved.
 */
#define HUK_STATUS    0x00000003U
#define HUK_STATUS_M  (HUK_STATUS_V << HUK_STATUS_S)
#define HUK_STATUS_V  0x00000003U
#define HUK_STATUS_S  0
/** HUK_RISK_LEVEL : RO; bitpos: [4:2]; default: 0;
 *  The risk level of HUK. 0-6: the higher the risk level is, the more error bits there
 *  are in the PUF SRAM. 7: Error Level, HUK is invalid.
 */
#define HUK_RISK_LEVEL    0x00000007U
#define HUK_RISK_LEVEL_M  (HUK_RISK_LEVEL_V << HUK_RISK_LEVEL_S)
#define HUK_RISK_LEVEL_V  0x00000007U
#define HUK_RISK_LEVEL_S  2

/** HUK_DATE_REG register
 *  Version control register
 */
#define HUK_DATE_REG (DR_REG_HUK_BASE + 0xfc)
/** HUK_DATE : R/W; bitpos: [27:0]; default: 36720704;
 *  HUK Generator version control register.
 */
#define HUK_DATE    0x0FFFFFFFU
#define HUK_DATE_M  (HUK_DATE_V << HUK_DATE_S)
#define HUK_DATE_V  0x0FFFFFFFU
#define HUK_DATE_S  0

/** HUK_INFO_MEM register
 *  The memory that stores HUK info.
 */
#define HUK_INFO_MEM (DR_REG_HUK_BASE + 0x100)
#define HUK_INFO_MEM_SIZE_BYTES 384

#ifdef __cplusplus
}
#endif
