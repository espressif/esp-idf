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

/** LP_PWR_LPCPU_CFG_REG register
 *  config register for lpcpu power control
 */
#define LP_PWR_LPCPU_CFG_REG (DR_REG_LP_PWR_BASE + 0x0)
/** LP_PWR_LPCPU_STALL_WAIT_TIMER : R/W; bitpos: [7:0]; default: 0;
 *  stall wait timer for lpcpu during lpcpu power off sequence
 */
#define LP_PWR_LPCPU_STALL_WAIT_TIMER    0x000000FFU
#define LP_PWR_LPCPU_STALL_WAIT_TIMER_M  (LP_PWR_LPCPU_STALL_WAIT_TIMER_V << LP_PWR_LPCPU_STALL_WAIT_TIMER_S)
#define LP_PWR_LPCPU_STALL_WAIT_TIMER_V  0x000000FFU
#define LP_PWR_LPCPU_STALL_WAIT_TIMER_S  0
/** LP_PWR_LPCPU_STALL_FLAG_EN : R/W; bitpos: [8]; default: 1;
 *  1: wait for stall rdy input during lpcpu power off sequence
 *  0:do not wait for stall rdy during lpcpu power off sequence
 */
#define LP_PWR_LPCPU_STALL_FLAG_EN    (BIT(8))
#define LP_PWR_LPCPU_STALL_FLAG_EN_M  (LP_PWR_LPCPU_STALL_FLAG_EN_V << LP_PWR_LPCPU_STALL_FLAG_EN_S)
#define LP_PWR_LPCPU_STALL_FLAG_EN_V  0x00000001U
#define LP_PWR_LPCPU_STALL_FLAG_EN_S  8
/** LP_PWR_LPCPU_STALL_EN : R/W; bitpos: [9]; default: 1;
 *  1: enable lpcpu stall during lpcpu power off sequence
 *  0: disable lpcpu stall during lpcpu power off sequence
 */
#define LP_PWR_LPCPU_STALL_EN    (BIT(9))
#define LP_PWR_LPCPU_STALL_EN_M  (LP_PWR_LPCPU_STALL_EN_V << LP_PWR_LPCPU_STALL_EN_S)
#define LP_PWR_LPCPU_STALL_EN_V  0x00000001U
#define LP_PWR_LPCPU_STALL_EN_S  9
/** LP_PWR_LPCPU_RESET_EN : R/W; bitpos: [10]; default: 1;
 *  1: enable lpcpu reset during lpcpu power off sequence
 *  0: disable lpcpu reset during lpcpu power off sequence
 */
#define LP_PWR_LPCPU_RESET_EN    (BIT(10))
#define LP_PWR_LPCPU_RESET_EN_M  (LP_PWR_LPCPU_RESET_EN_V << LP_PWR_LPCPU_RESET_EN_S)
#define LP_PWR_LPCPU_RESET_EN_V  0x00000001U
#define LP_PWR_LPCPU_RESET_EN_S  10
/** LP_PWR_LPCPU_SLP_WAITI_FLAG_EN : R/W; bitpos: [11]; default: 0;
 *  1: validates lp cpu waiti rdy signal during lpcpu power off sequence
 *  0: invalidates lp cpu waiti rdy signal during lpcpu power off sequence
 */
#define LP_PWR_LPCPU_SLP_WAITI_FLAG_EN    (BIT(11))
#define LP_PWR_LPCPU_SLP_WAITI_FLAG_EN_M  (LP_PWR_LPCPU_SLP_WAITI_FLAG_EN_V << LP_PWR_LPCPU_SLP_WAITI_FLAG_EN_S)
#define LP_PWR_LPCPU_SLP_WAITI_FLAG_EN_V  0x00000001U
#define LP_PWR_LPCPU_SLP_WAITI_FLAG_EN_S  11
/** LP_PWR_LPCPU_SLP_BYPASS_INTR_EN : R/W; bitpos: [12]; default: 0;
 *  1: enable interrupt bypass signal for lpcpu during lpcpu power off sequence
 *  0: disable interrupt bypass signal for lpcpu during lpcpu power off sequence
 */
#define LP_PWR_LPCPU_SLP_BYPASS_INTR_EN    (BIT(12))
#define LP_PWR_LPCPU_SLP_BYPASS_INTR_EN_M  (LP_PWR_LPCPU_SLP_BYPASS_INTR_EN_V << LP_PWR_LPCPU_SLP_BYPASS_INTR_EN_S)
#define LP_PWR_LPCPU_SLP_BYPASS_INTR_EN_V  0x00000001U
#define LP_PWR_LPCPU_SLP_BYPASS_INTR_EN_S  12

/** LP_PWR_LPCPU_STS_REG register
 *  status register for LPCPU PWR
 */
#define LP_PWR_LPCPU_STS_REG (DR_REG_LP_PWR_BASE + 0x4)
/** LP_PWR_LPCPU_CUR_ST : RO; bitpos: [3:0]; default: 4;
 *  current power state register for lpcpu
 */
#define LP_PWR_LPCPU_CUR_ST    0x0000000FU
#define LP_PWR_LPCPU_CUR_ST_M  (LP_PWR_LPCPU_CUR_ST_V << LP_PWR_LPCPU_CUR_ST_S)
#define LP_PWR_LPCPU_CUR_ST_V  0x0000000FU
#define LP_PWR_LPCPU_CUR_ST_S  0
/** LP_PWR_LPCPU_IN_POWER_ON : RO; bitpos: [4]; default: 0;
 *  1:lpcpu current in power on mode
 *  0:lpcpu not in power on mode
 */
#define LP_PWR_LPCPU_IN_POWER_ON    (BIT(4))
#define LP_PWR_LPCPU_IN_POWER_ON_M  (LP_PWR_LPCPU_IN_POWER_ON_V << LP_PWR_LPCPU_IN_POWER_ON_S)
#define LP_PWR_LPCPU_IN_POWER_ON_V  0x00000001U
#define LP_PWR_LPCPU_IN_POWER_ON_S  4
/** LP_PWR_LPCPU_STALL_RDY : RO; bitpos: [5]; default: 0;
 *  1:lpcpu stall not ready
 *  0:lpcpu stall ready
 */
#define LP_PWR_LPCPU_STALL_RDY    (BIT(5))
#define LP_PWR_LPCPU_STALL_RDY_M  (LP_PWR_LPCPU_STALL_RDY_V << LP_PWR_LPCPU_STALL_RDY_S)
#define LP_PWR_LPCPU_STALL_RDY_V  0x00000001U
#define LP_PWR_LPCPU_STALL_RDY_S  5
/** LP_PWR_LPCPU_WAITI_RDY : RO; bitpos: [6]; default: 0;
 *  1:lpcpu stall waiti ready
 *  0:lpcpu stall waiti ready
 */
#define LP_PWR_LPCPU_WAITI_RDY    (BIT(6))
#define LP_PWR_LPCPU_WAITI_RDY_M  (LP_PWR_LPCPU_WAITI_RDY_V << LP_PWR_LPCPU_WAITI_RDY_S)
#define LP_PWR_LPCPU_WAITI_RDY_V  0x00000001U
#define LP_PWR_LPCPU_WAITI_RDY_S  6

/** LP_PWR_LPCPU_CTRL_REG register
 *  ctrl register for lpcpu power control
 */
#define LP_PWR_LPCPU_CTRL_REG (DR_REG_LP_PWR_BASE + 0x8)
/** LP_PWR_LPCPU_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for lpcpu
 */
#define LP_PWR_LPCPU_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_LPCPU_SW_SLEEP_REQ_M  (LP_PWR_LPCPU_SW_SLEEP_REQ_V << LP_PWR_LPCPU_SW_SLEEP_REQ_S)
#define LP_PWR_LPCPU_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_LPCPU_SW_SLEEP_REQ_S  0
/** LP_PWR_LPCPU_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for lpcpu
 */
#define LP_PWR_LPCPU_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_LPCPU_SW_WAKEUP_REQ_M  (LP_PWR_LPCPU_SW_WAKEUP_REQ_V << LP_PWR_LPCPU_SW_WAKEUP_REQ_S)
#define LP_PWR_LPCPU_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_LPCPU_SW_WAKEUP_REQ_S  1
/** LP_PWR_LPCPU_FORCE_STALL : R/W; bitpos: [2]; default: 0;
 *  1: force stall lpcpu
 *  0: no change
 */
#define LP_PWR_LPCPU_FORCE_STALL    (BIT(2))
#define LP_PWR_LPCPU_FORCE_STALL_M  (LP_PWR_LPCPU_FORCE_STALL_V << LP_PWR_LPCPU_FORCE_STALL_S)
#define LP_PWR_LPCPU_FORCE_STALL_V  0x00000001U
#define LP_PWR_LPCPU_FORCE_STALL_S  2
/** LP_PWR_LPCPU_FORCE_RESET : R/W; bitpos: [3]; default: 0;
 *  1: force reset lpcpu
 *  0: no change
 */
#define LP_PWR_LPCPU_FORCE_RESET    (BIT(3))
#define LP_PWR_LPCPU_FORCE_RESET_M  (LP_PWR_LPCPU_FORCE_RESET_V << LP_PWR_LPCPU_FORCE_RESET_S)
#define LP_PWR_LPCPU_FORCE_RESET_V  0x00000001U
#define LP_PWR_LPCPU_FORCE_RESET_S  3
/** LP_PWR_LPCPU_FORCE_NO_RESET : R/W; bitpos: [4]; default: 0;
 *  1: force no reset lpcpu
 *  0: no change
 */
#define LP_PWR_LPCPU_FORCE_NO_RESET    (BIT(4))
#define LP_PWR_LPCPU_FORCE_NO_RESET_M  (LP_PWR_LPCPU_FORCE_NO_RESET_V << LP_PWR_LPCPU_FORCE_NO_RESET_S)
#define LP_PWR_LPCPU_FORCE_NO_RESET_V  0x00000001U
#define LP_PWR_LPCPU_FORCE_NO_RESET_S  4

/** LP_PWR_LPCPU_WAKEUP_SRC_REG register
 *  wakeup source register for lpcpu
 */
#define LP_PWR_LPCPU_WAKEUP_SRC_REG (DR_REG_LP_PWR_BASE + 0xc)
/** LP_PWR_LPCPU_WAKEUP_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  wakeup source enable signal for lpcpu
 */
#define LP_PWR_LPCPU_WAKEUP_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_LPCPU_WAKEUP_SOURCE_EN_M  (LP_PWR_LPCPU_WAKEUP_SOURCE_EN_V << LP_PWR_LPCPU_WAKEUP_SOURCE_EN_S)
#define LP_PWR_LPCPU_WAKEUP_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_LPCPU_WAKEUP_SOURCE_EN_S  0

/** LP_PWR_LPCPU_REJECT_SRC_REG register
 *  reject source register for lpcpu
 */
#define LP_PWR_LPCPU_REJECT_SRC_REG (DR_REG_LP_PWR_BASE + 0x10)
/** LP_PWR_LPCPU_REJECT_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  reject source enable signal for lpcpu
 */
#define LP_PWR_LPCPU_REJECT_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_LPCPU_REJECT_SOURCE_EN_M  (LP_PWR_LPCPU_REJECT_SOURCE_EN_V << LP_PWR_LPCPU_REJECT_SOURCE_EN_S)
#define LP_PWR_LPCPU_REJECT_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_LPCPU_REJECT_SOURCE_EN_S  0
/** LP_PWR_LPCPU_SLEEP_REJECT_EN : R/W; bitpos: [31]; default: 0;
 *  1: sleep request can be rejected
 *  0: sleep request cannot be rejected
 */
#define LP_PWR_LPCPU_SLEEP_REJECT_EN    (BIT(31))
#define LP_PWR_LPCPU_SLEEP_REJECT_EN_M  (LP_PWR_LPCPU_SLEEP_REJECT_EN_V << LP_PWR_LPCPU_SLEEP_REJECT_EN_S)
#define LP_PWR_LPCPU_SLEEP_REJECT_EN_V  0x00000001U
#define LP_PWR_LPCPU_SLEEP_REJECT_EN_S  31

/** LP_PWR_LPCPU_WAKEUP_CAUSE_REG register
 *  wakeup cause register for lpcpu
 */
#define LP_PWR_LPCPU_WAKEUP_CAUSE_REG (DR_REG_LP_PWR_BASE + 0x14)
/** LP_PWR_LPCPU_WAKEUP_CAUSE : RO; bitpos: [30:0]; default: 0;
 *  wakeup cause for lpcpu
 */
#define LP_PWR_LPCPU_WAKEUP_CAUSE    0x7FFFFFFFU
#define LP_PWR_LPCPU_WAKEUP_CAUSE_M  (LP_PWR_LPCPU_WAKEUP_CAUSE_V << LP_PWR_LPCPU_WAKEUP_CAUSE_S)
#define LP_PWR_LPCPU_WAKEUP_CAUSE_V  0x7FFFFFFFU
#define LP_PWR_LPCPU_WAKEUP_CAUSE_S  0

/** LP_PWR_LPCPU_REJECT_CAUSE_REG register
 *  reject cause register for lpcpu
 */
#define LP_PWR_LPCPU_REJECT_CAUSE_REG (DR_REG_LP_PWR_BASE + 0x18)
/** LP_PWR_LPCPU_REJECT_CAUSE : RO; bitpos: [30:0]; default: 0;
 *  reject cause for lpcpu
 */
#define LP_PWR_LPCPU_REJECT_CAUSE    0x7FFFFFFFU
#define LP_PWR_LPCPU_REJECT_CAUSE_M  (LP_PWR_LPCPU_REJECT_CAUSE_V << LP_PWR_LPCPU_REJECT_CAUSE_S)
#define LP_PWR_LPCPU_REJECT_CAUSE_V  0x7FFFFFFFU
#define LP_PWR_LPCPU_REJECT_CAUSE_S  0

/** LP_PWR_LPCPU_CAUSE_CLR_REG register
 *  cause clear register for lpcpu
 */
#define LP_PWR_LPCPU_CAUSE_CLR_REG (DR_REG_LP_PWR_BASE + 0x1c)
/** LP_PWR_LPCPU_REJECT_CAUSE_CLR : WT; bitpos: [0]; default: 0;
 *  reject cause clear signal for lpcpu
 */
#define LP_PWR_LPCPU_REJECT_CAUSE_CLR    (BIT(0))
#define LP_PWR_LPCPU_REJECT_CAUSE_CLR_M  (LP_PWR_LPCPU_REJECT_CAUSE_CLR_V << LP_PWR_LPCPU_REJECT_CAUSE_CLR_S)
#define LP_PWR_LPCPU_REJECT_CAUSE_CLR_V  0x00000001U
#define LP_PWR_LPCPU_REJECT_CAUSE_CLR_S  0
/** LP_PWR_LPCPU_WAKEUP_CAUSE_CLR : WT; bitpos: [1]; default: 0;
 *  wakeup cause clear signal for lpcpu
 */
#define LP_PWR_LPCPU_WAKEUP_CAUSE_CLR    (BIT(1))
#define LP_PWR_LPCPU_WAKEUP_CAUSE_CLR_M  (LP_PWR_LPCPU_WAKEUP_CAUSE_CLR_V << LP_PWR_LPCPU_WAKEUP_CAUSE_CLR_S)
#define LP_PWR_LPCPU_WAKEUP_CAUSE_CLR_V  0x00000001U
#define LP_PWR_LPCPU_WAKEUP_CAUSE_CLR_S  1

/** LP_PWR_APPWR_PWR_CFG_REG register
 *  config register for apsys pwr and clk mode
 */
#define LP_PWR_APPWR_PWR_CFG_REG (DR_REG_LP_PWR_BASE + 0x20)
/** LP_PWR_CPU_PWR_MODE : R/W; bitpos: [1:0]; default: 2;
 *  cpu pwr mode config register for apsys
 */
#define LP_PWR_CPU_PWR_MODE    0x00000003U
#define LP_PWR_CPU_PWR_MODE_M  (LP_PWR_CPU_PWR_MODE_V << LP_PWR_CPU_PWR_MODE_S)
#define LP_PWR_CPU_PWR_MODE_V  0x00000003U
#define LP_PWR_CPU_PWR_MODE_S  0
/** LP_PWR_TOP_PWR_MODE : R/W; bitpos: [3:2]; default: 2;
 *  top pwr mode config register for apsys
 */
#define LP_PWR_TOP_PWR_MODE    0x00000003U
#define LP_PWR_TOP_PWR_MODE_M  (LP_PWR_TOP_PWR_MODE_V << LP_PWR_TOP_PWR_MODE_S)
#define LP_PWR_TOP_PWR_MODE_V  0x00000003U
#define LP_PWR_TOP_PWR_MODE_S  2
/** LP_PWR_CNNT_PWR_MODE : R/W; bitpos: [5:4]; default: 2;
 *  cnnt pwr mode config register for apsys
 */
#define LP_PWR_CNNT_PWR_MODE    0x00000003U
#define LP_PWR_CNNT_PWR_MODE_M  (LP_PWR_CNNT_PWR_MODE_V << LP_PWR_CNNT_PWR_MODE_S)
#define LP_PWR_CNNT_PWR_MODE_V  0x00000003U
#define LP_PWR_CNNT_PWR_MODE_S  4
/** LP_PWR_HPALIVE_PWR_MODE : R/W; bitpos: [7:6]; default: 2;
 *  hpalive pwr mode config register for apsys
 */
#define LP_PWR_HPALIVE_PWR_MODE    0x00000003U
#define LP_PWR_HPALIVE_PWR_MODE_M  (LP_PWR_HPALIVE_PWR_MODE_V << LP_PWR_HPALIVE_PWR_MODE_S)
#define LP_PWR_HPALIVE_PWR_MODE_V  0x00000003U
#define LP_PWR_HPALIVE_PWR_MODE_S  6
/** LP_PWR_MEM_0_PWR_MODE : R/W; bitpos: [9:8]; default: 2;
 *  mem_0 pwr mode config register for apsys
 */
#define LP_PWR_MEM_0_PWR_MODE    0x00000003U
#define LP_PWR_MEM_0_PWR_MODE_M  (LP_PWR_MEM_0_PWR_MODE_V << LP_PWR_MEM_0_PWR_MODE_S)
#define LP_PWR_MEM_0_PWR_MODE_V  0x00000003U
#define LP_PWR_MEM_0_PWR_MODE_S  8
/** LP_PWR_MEM_1_PWR_MODE : R/W; bitpos: [11:10]; default: 2;
 *  mem_1 pwr mode config register for apsys
 */
#define LP_PWR_MEM_1_PWR_MODE    0x00000003U
#define LP_PWR_MEM_1_PWR_MODE_M  (LP_PWR_MEM_1_PWR_MODE_V << LP_PWR_MEM_1_PWR_MODE_S)
#define LP_PWR_MEM_1_PWR_MODE_V  0x00000003U
#define LP_PWR_MEM_1_PWR_MODE_S  10
/** LP_PWR_MEM_2_PWR_MODE : R/W; bitpos: [13:12]; default: 2;
 *  mem_2 pwr mode config register for apsys
 */
#define LP_PWR_MEM_2_PWR_MODE    0x00000003U
#define LP_PWR_MEM_2_PWR_MODE_M  (LP_PWR_MEM_2_PWR_MODE_V << LP_PWR_MEM_2_PWR_MODE_S)
#define LP_PWR_MEM_2_PWR_MODE_V  0x00000003U
#define LP_PWR_MEM_2_PWR_MODE_S  12
/** LP_PWR_MEM_3_PWR_MODE : R/W; bitpos: [15:14]; default: 2;
 *  mem_3 pwr mode config register for apsys
 */
#define LP_PWR_MEM_3_PWR_MODE    0x00000003U
#define LP_PWR_MEM_3_PWR_MODE_M  (LP_PWR_MEM_3_PWR_MODE_V << LP_PWR_MEM_3_PWR_MODE_S)
#define LP_PWR_MEM_3_PWR_MODE_V  0x00000003U
#define LP_PWR_MEM_3_PWR_MODE_S  14

/** LP_PWR_APPWR_CLK_CFG_REG register
 *  config register for apsys pwr and clk mode
 */
#define LP_PWR_APPWR_CLK_CFG_REG (DR_REG_LP_PWR_BASE + 0x24)
/** LP_PWR_CPU_CLK_MODE : R/W; bitpos: [1:0]; default: 3;
 *  cpu clk mode config register for apsys
 */
#define LP_PWR_CPU_CLK_MODE    0x00000003U
#define LP_PWR_CPU_CLK_MODE_M  (LP_PWR_CPU_CLK_MODE_V << LP_PWR_CPU_CLK_MODE_S)
#define LP_PWR_CPU_CLK_MODE_V  0x00000003U
#define LP_PWR_CPU_CLK_MODE_S  0
/** LP_PWR_TOP_CLK_MODE : R/W; bitpos: [3:2]; default: 3;
 *  top clk mode config register for apsys
 */
#define LP_PWR_TOP_CLK_MODE    0x00000003U
#define LP_PWR_TOP_CLK_MODE_M  (LP_PWR_TOP_CLK_MODE_V << LP_PWR_TOP_CLK_MODE_S)
#define LP_PWR_TOP_CLK_MODE_V  0x00000003U
#define LP_PWR_TOP_CLK_MODE_S  2
/** LP_PWR_CNNT_CLK_MODE : R/W; bitpos: [5:4]; default: 3;
 *  cnnt clk mode config register for apsys
 */
#define LP_PWR_CNNT_CLK_MODE    0x00000003U
#define LP_PWR_CNNT_CLK_MODE_M  (LP_PWR_CNNT_CLK_MODE_V << LP_PWR_CNNT_CLK_MODE_S)
#define LP_PWR_CNNT_CLK_MODE_V  0x00000003U
#define LP_PWR_CNNT_CLK_MODE_S  4
/** LP_PWR_HPALIVE_CLK_MODE : R/W; bitpos: [7:6]; default: 3;
 *  hpalive clk mode config register for apsys
 */
#define LP_PWR_HPALIVE_CLK_MODE    0x00000003U
#define LP_PWR_HPALIVE_CLK_MODE_M  (LP_PWR_HPALIVE_CLK_MODE_V << LP_PWR_HPALIVE_CLK_MODE_S)
#define LP_PWR_HPALIVE_CLK_MODE_V  0x00000003U
#define LP_PWR_HPALIVE_CLK_MODE_S  6
/** LP_PWR_MEM_0_CLK_MODE : R/W; bitpos: [9:8]; default: 3;
 *  mem_0 clk mode config register for apsys
 */
#define LP_PWR_MEM_0_CLK_MODE    0x00000003U
#define LP_PWR_MEM_0_CLK_MODE_M  (LP_PWR_MEM_0_CLK_MODE_V << LP_PWR_MEM_0_CLK_MODE_S)
#define LP_PWR_MEM_0_CLK_MODE_V  0x00000003U
#define LP_PWR_MEM_0_CLK_MODE_S  8
/** LP_PWR_MEM_1_CLK_MODE : R/W; bitpos: [11:10]; default: 3;
 *  mem_1 clk mode config register for apsys
 */
#define LP_PWR_MEM_1_CLK_MODE    0x00000003U
#define LP_PWR_MEM_1_CLK_MODE_M  (LP_PWR_MEM_1_CLK_MODE_V << LP_PWR_MEM_1_CLK_MODE_S)
#define LP_PWR_MEM_1_CLK_MODE_V  0x00000003U
#define LP_PWR_MEM_1_CLK_MODE_S  10
/** LP_PWR_MEM_2_CLK_MODE : R/W; bitpos: [13:12]; default: 3;
 *  mem_2 clk mode config register for apsys
 */
#define LP_PWR_MEM_2_CLK_MODE    0x00000003U
#define LP_PWR_MEM_2_CLK_MODE_M  (LP_PWR_MEM_2_CLK_MODE_V << LP_PWR_MEM_2_CLK_MODE_S)
#define LP_PWR_MEM_2_CLK_MODE_V  0x00000003U
#define LP_PWR_MEM_2_CLK_MODE_S  12
/** LP_PWR_MEM_3_CLK_MODE : R/W; bitpos: [15:14]; default: 3;
 *  mem_3 clk mode config register for apsys
 */
#define LP_PWR_MEM_3_CLK_MODE    0x00000003U
#define LP_PWR_MEM_3_CLK_MODE_M  (LP_PWR_MEM_3_CLK_MODE_V << LP_PWR_MEM_3_CLK_MODE_S)
#define LP_PWR_MEM_3_CLK_MODE_V  0x00000003U
#define LP_PWR_MEM_3_CLK_MODE_S  14

/** LP_PWR_APPWR_CFG_REG register
 *  config register for apsys pwr
 */
#define LP_PWR_APPWR_CFG_REG (DR_REG_LP_PWR_BASE + 0x28)
/** LP_PWR_STALL_WAIT_TIMER : R/W; bitpos: [7:0]; default: 0;
 *  the wait timer config for ap sys
 */
#define LP_PWR_STALL_WAIT_TIMER    0x000000FFU
#define LP_PWR_STALL_WAIT_TIMER_M  (LP_PWR_STALL_WAIT_TIMER_V << LP_PWR_STALL_WAIT_TIMER_S)
#define LP_PWR_STALL_WAIT_TIMER_V  0x000000FFU
#define LP_PWR_STALL_WAIT_TIMER_S  0
/** LP_PWR_BACKUP_EN : R/W; bitpos: [8]; default: 0;
 *  the backup enable config register for ap sys
 */
#define LP_PWR_BACKUP_EN    (BIT(8))
#define LP_PWR_BACKUP_EN_M  (LP_PWR_BACKUP_EN_V << LP_PWR_BACKUP_EN_S)
#define LP_PWR_BACKUP_EN_V  0x00000001U
#define LP_PWR_BACKUP_EN_S  8
/** LP_PWR_RESTORE_MODE : R/W; bitpos: [13:9]; default: 0;
 *  restore mode config register for apsys
 */
#define LP_PWR_RESTORE_MODE    0x0000001FU
#define LP_PWR_RESTORE_MODE_M  (LP_PWR_RESTORE_MODE_V << LP_PWR_RESTORE_MODE_S)
#define LP_PWR_RESTORE_MODE_V  0x0000001FU
#define LP_PWR_RESTORE_MODE_S  9
/** LP_PWR_BACKUP_MODE : R/W; bitpos: [18:14]; default: 0;
 *  backup mode config register for apsys
 */
#define LP_PWR_BACKUP_MODE    0x0000001FU
#define LP_PWR_BACKUP_MODE_M  (LP_PWR_BACKUP_MODE_V << LP_PWR_BACKUP_MODE_S)
#define LP_PWR_BACKUP_MODE_V  0x0000001FU
#define LP_PWR_BACKUP_MODE_S  14
/** LP_PWR_HPCPU_STALL_EN : R/W; bitpos: [19]; default: 1;
 *  1: enable hpcpu stall
 *  0: disable hpcpu stall
 */
#define LP_PWR_HPCPU_STALL_EN    (BIT(19))
#define LP_PWR_HPCPU_STALL_EN_M  (LP_PWR_HPCPU_STALL_EN_V << LP_PWR_HPCPU_STALL_EN_S)
#define LP_PWR_HPCPU_STALL_EN_V  0x00000001U
#define LP_PWR_HPCPU_STALL_EN_S  19

/** LP_PWR_APPWR_STS_REG register
 *  status register for apsys pwr
 */
#define LP_PWR_APPWR_STS_REG (DR_REG_LP_PWR_BASE + 0x2c)
/** LP_PWR_APPWR_CUR_ST : RO; bitpos: [3:0]; default: 6;
 *  current power state register for apsys
 */
#define LP_PWR_APPWR_CUR_ST    0x0000000FU
#define LP_PWR_APPWR_CUR_ST_M  (LP_PWR_APPWR_CUR_ST_V << LP_PWR_APPWR_CUR_ST_S)
#define LP_PWR_APPWR_CUR_ST_V  0x0000000FU
#define LP_PWR_APPWR_CUR_ST_S  0
/** LP_PWR_APSYS_IN_POWER_ON : RO; bitpos: [4]; default: 0;
 *  1:apsys current in power on mode
 *  0:apsys not in power on mode
 */
#define LP_PWR_APSYS_IN_POWER_ON    (BIT(4))
#define LP_PWR_APSYS_IN_POWER_ON_M  (LP_PWR_APSYS_IN_POWER_ON_V << LP_PWR_APSYS_IN_POWER_ON_S)
#define LP_PWR_APSYS_IN_POWER_ON_V  0x00000001U
#define LP_PWR_APSYS_IN_POWER_ON_S  4

/** LP_PWR_APPWR_CTRL_REG register
 *  ctrl register for appwr power control
 */
#define LP_PWR_APPWR_CTRL_REG (DR_REG_LP_PWR_BASE + 0x30)
/** LP_PWR_APPWR_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for appwr
 */
#define LP_PWR_APPWR_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_APPWR_SW_SLEEP_REQ_M  (LP_PWR_APPWR_SW_SLEEP_REQ_V << LP_PWR_APPWR_SW_SLEEP_REQ_S)
#define LP_PWR_APPWR_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_APPWR_SW_SLEEP_REQ_S  0
/** LP_PWR_APPWR_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for appwr
 */
#define LP_PWR_APPWR_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_APPWR_SW_WAKEUP_REQ_M  (LP_PWR_APPWR_SW_WAKEUP_REQ_V << LP_PWR_APPWR_SW_WAKEUP_REQ_S)
#define LP_PWR_APPWR_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_APPWR_SW_WAKEUP_REQ_S  1
/** LP_PWR_HPCPU_FORCE_STALL : R/W; bitpos: [2]; default: 0;
 *  1: force stall hpcpu
 *  0: no change
 */
#define LP_PWR_HPCPU_FORCE_STALL    (BIT(2))
#define LP_PWR_HPCPU_FORCE_STALL_M  (LP_PWR_HPCPU_FORCE_STALL_V << LP_PWR_HPCPU_FORCE_STALL_S)
#define LP_PWR_HPCPU_FORCE_STALL_V  0x00000001U
#define LP_PWR_HPCPU_FORCE_STALL_S  2

/** LP_PWR_APPWR_WAKEUP_SRC_REG register
 *  wakeup source register for appwr
 */
#define LP_PWR_APPWR_WAKEUP_SRC_REG (DR_REG_LP_PWR_BASE + 0x34)
/** LP_PWR_APPWR_WAKEUP_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  wakeup enable signal for appwr
 */
#define LP_PWR_APPWR_WAKEUP_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_APPWR_WAKEUP_SOURCE_EN_M  (LP_PWR_APPWR_WAKEUP_SOURCE_EN_V << LP_PWR_APPWR_WAKEUP_SOURCE_EN_S)
#define LP_PWR_APPWR_WAKEUP_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_APPWR_WAKEUP_SOURCE_EN_S  0

/** LP_PWR_APPWR_REJECT_SRC_REG register
 *  reject source register for appwr
 */
#define LP_PWR_APPWR_REJECT_SRC_REG (DR_REG_LP_PWR_BASE + 0x38)
/** LP_PWR_APPWR_REJECT_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  reject enable signal for appwr
 */
#define LP_PWR_APPWR_REJECT_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_APPWR_REJECT_SOURCE_EN_M  (LP_PWR_APPWR_REJECT_SOURCE_EN_V << LP_PWR_APPWR_REJECT_SOURCE_EN_S)
#define LP_PWR_APPWR_REJECT_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_APPWR_REJECT_SOURCE_EN_S  0
/** LP_PWR_APPWR_SLEEP_REJECT_EN : R/W; bitpos: [31]; default: 0;
 *  1: sleep request can be rejected
 *  0: sleep request cannot be rejected
 */
#define LP_PWR_APPWR_SLEEP_REJECT_EN    (BIT(31))
#define LP_PWR_APPWR_SLEEP_REJECT_EN_M  (LP_PWR_APPWR_SLEEP_REJECT_EN_V << LP_PWR_APPWR_SLEEP_REJECT_EN_S)
#define LP_PWR_APPWR_SLEEP_REJECT_EN_V  0x00000001U
#define LP_PWR_APPWR_SLEEP_REJECT_EN_S  31

/** LP_PWR_APPWR_WAKEUP_CAUSE_REG register
 *  wakeup cause register for appwr
 */
#define LP_PWR_APPWR_WAKEUP_CAUSE_REG (DR_REG_LP_PWR_BASE + 0x3c)
/** LP_PWR_APPWR_WAKEUP_CAUSE : RO; bitpos: [30:0]; default: 0;
 *  wakeup cause for appwr
 */
#define LP_PWR_APPWR_WAKEUP_CAUSE    0x7FFFFFFFU
#define LP_PWR_APPWR_WAKEUP_CAUSE_M  (LP_PWR_APPWR_WAKEUP_CAUSE_V << LP_PWR_APPWR_WAKEUP_CAUSE_S)
#define LP_PWR_APPWR_WAKEUP_CAUSE_V  0x7FFFFFFFU
#define LP_PWR_APPWR_WAKEUP_CAUSE_S  0

/** LP_PWR_APPWR_REJECT_CAUSE_REG register
 *  reject cause register for appwr
 */
#define LP_PWR_APPWR_REJECT_CAUSE_REG (DR_REG_LP_PWR_BASE + 0x40)
/** LP_PWR_APPWR_REJECT_CAUSE : RO; bitpos: [30:0]; default: 0;
 *  reject cause for appwr
 */
#define LP_PWR_APPWR_REJECT_CAUSE    0x7FFFFFFFU
#define LP_PWR_APPWR_REJECT_CAUSE_M  (LP_PWR_APPWR_REJECT_CAUSE_V << LP_PWR_APPWR_REJECT_CAUSE_S)
#define LP_PWR_APPWR_REJECT_CAUSE_V  0x7FFFFFFFU
#define LP_PWR_APPWR_REJECT_CAUSE_S  0

/** LP_PWR_APPWR_CAUSE_CLR_REG register
 *  cause clear register for appwr
 */
#define LP_PWR_APPWR_CAUSE_CLR_REG (DR_REG_LP_PWR_BASE + 0x44)
/** LP_PWR_APPWR_REJECT_CAUSE_CLR : WT; bitpos: [0]; default: 0;
 *  reject cause clear signal for appwr
 */
#define LP_PWR_APPWR_REJECT_CAUSE_CLR    (BIT(0))
#define LP_PWR_APPWR_REJECT_CAUSE_CLR_M  (LP_PWR_APPWR_REJECT_CAUSE_CLR_V << LP_PWR_APPWR_REJECT_CAUSE_CLR_S)
#define LP_PWR_APPWR_REJECT_CAUSE_CLR_V  0x00000001U
#define LP_PWR_APPWR_REJECT_CAUSE_CLR_S  0
/** LP_PWR_APPWR_WAKEUP_CAUSE_CLR : WT; bitpos: [1]; default: 0;
 *  wakeup cause clear signal for appwr
 */
#define LP_PWR_APPWR_WAKEUP_CAUSE_CLR    (BIT(1))
#define LP_PWR_APPWR_WAKEUP_CAUSE_CLR_M  (LP_PWR_APPWR_WAKEUP_CAUSE_CLR_V << LP_PWR_APPWR_WAKEUP_CAUSE_CLR_S)
#define LP_PWR_APPWR_WAKEUP_CAUSE_CLR_V  0x00000001U
#define LP_PWR_APPWR_WAKEUP_CAUSE_CLR_S  1

/** LP_PWR_WIFI_CTRL_REG register
 *  ctrl register for wifi power control
 */
#define LP_PWR_WIFI_CTRL_REG (DR_REG_LP_PWR_BASE + 0x48)
/** LP_PWR_WIFI_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for wifi
 */
#define LP_PWR_WIFI_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_WIFI_SW_SLEEP_REQ_M  (LP_PWR_WIFI_SW_SLEEP_REQ_V << LP_PWR_WIFI_SW_SLEEP_REQ_S)
#define LP_PWR_WIFI_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_WIFI_SW_SLEEP_REQ_S  0
/** LP_PWR_WIFI_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for wifi
 */
#define LP_PWR_WIFI_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_WIFI_SW_WAKEUP_REQ_M  (LP_PWR_WIFI_SW_WAKEUP_REQ_V << LP_PWR_WIFI_SW_WAKEUP_REQ_S)
#define LP_PWR_WIFI_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_WIFI_SW_WAKEUP_REQ_S  1

/** LP_PWR_WIFI_WAKEUP_SRC_REG register
 *  wakeup source register for wifi
 */
#define LP_PWR_WIFI_WAKEUP_SRC_REG (DR_REG_LP_PWR_BASE + 0x4c)
/** LP_PWR_WIFI_WAKEUP_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  wakeup enable signal for wifi
 */
#define LP_PWR_WIFI_WAKEUP_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_WIFI_WAKEUP_SOURCE_EN_M  (LP_PWR_WIFI_WAKEUP_SOURCE_EN_V << LP_PWR_WIFI_WAKEUP_SOURCE_EN_S)
#define LP_PWR_WIFI_WAKEUP_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_WIFI_WAKEUP_SOURCE_EN_S  0

/** LP_PWR_BLE_CTRL_REG register
 *  ctrl register for ble power control
 */
#define LP_PWR_BLE_CTRL_REG (DR_REG_LP_PWR_BASE + 0x50)
/** LP_PWR_BLE_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for ble
 */
#define LP_PWR_BLE_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_BLE_SW_SLEEP_REQ_M  (LP_PWR_BLE_SW_SLEEP_REQ_V << LP_PWR_BLE_SW_SLEEP_REQ_S)
#define LP_PWR_BLE_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_BLE_SW_SLEEP_REQ_S  0
/** LP_PWR_BLE_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for ble
 */
#define LP_PWR_BLE_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_BLE_SW_WAKEUP_REQ_M  (LP_PWR_BLE_SW_WAKEUP_REQ_V << LP_PWR_BLE_SW_WAKEUP_REQ_S)
#define LP_PWR_BLE_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_BLE_SW_WAKEUP_REQ_S  1

/** LP_PWR_BLE_WAKEUP_SRC_REG register
 *  wakeup source register for ble
 */
#define LP_PWR_BLE_WAKEUP_SRC_REG (DR_REG_LP_PWR_BASE + 0x54)
/** LP_PWR_BLE_WAKEUP_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  wakeup enable signal for ble
 */
#define LP_PWR_BLE_WAKEUP_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_BLE_WAKEUP_SOURCE_EN_M  (LP_PWR_BLE_WAKEUP_SOURCE_EN_V << LP_PWR_BLE_WAKEUP_SOURCE_EN_S)
#define LP_PWR_BLE_WAKEUP_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_BLE_WAKEUP_SOURCE_EN_S  0

/** LP_PWR_ZB_CTRL_REG register
 *  ctrl register for zb power control
 */
#define LP_PWR_ZB_CTRL_REG (DR_REG_LP_PWR_BASE + 0x58)
/** LP_PWR_ZB_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for zb
 */
#define LP_PWR_ZB_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_ZB_SW_SLEEP_REQ_M  (LP_PWR_ZB_SW_SLEEP_REQ_V << LP_PWR_ZB_SW_SLEEP_REQ_S)
#define LP_PWR_ZB_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_ZB_SW_SLEEP_REQ_S  0
/** LP_PWR_ZB_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for zb
 */
#define LP_PWR_ZB_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_ZB_SW_WAKEUP_REQ_M  (LP_PWR_ZB_SW_WAKEUP_REQ_V << LP_PWR_ZB_SW_WAKEUP_REQ_S)
#define LP_PWR_ZB_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_ZB_SW_WAKEUP_REQ_S  1

/** LP_PWR_ZB_WAKEUP_SRC_REG register
 *  wakeup source register for zb
 */
#define LP_PWR_ZB_WAKEUP_SRC_REG (DR_REG_LP_PWR_BASE + 0x5c)
/** LP_PWR_ZB_WAKEUP_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  wakeup enazb signal for zb
 */
#define LP_PWR_ZB_WAKEUP_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_ZB_WAKEUP_SOURCE_EN_M  (LP_PWR_ZB_WAKEUP_SOURCE_EN_V << LP_PWR_ZB_WAKEUP_SOURCE_EN_S)
#define LP_PWR_ZB_WAKEUP_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_ZB_WAKEUP_SOURCE_EN_S  0

/** LP_PWR_MODEMPWR_CTRL_REG register
 *  ctrl register for modempwr power control
 */
#define LP_PWR_MODEMPWR_CTRL_REG (DR_REG_LP_PWR_BASE + 0x60)
/** LP_PWR_MODEMPWR_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for modempwr
 */
#define LP_PWR_MODEMPWR_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_MODEMPWR_SW_SLEEP_REQ_M  (LP_PWR_MODEMPWR_SW_SLEEP_REQ_V << LP_PWR_MODEMPWR_SW_SLEEP_REQ_S)
#define LP_PWR_MODEMPWR_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_MODEMPWR_SW_SLEEP_REQ_S  0
/** LP_PWR_MODEMPWR_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for modempwr
 */
#define LP_PWR_MODEMPWR_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_MODEMPWR_SW_WAKEUP_REQ_M  (LP_PWR_MODEMPWR_SW_WAKEUP_REQ_V << LP_PWR_MODEMPWR_SW_WAKEUP_REQ_S)
#define LP_PWR_MODEMPWR_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_MODEMPWR_SW_WAKEUP_REQ_S  1

/** LP_PWR_MODEMPWR_WAKEUP_SRC_REG register
 *  wakeup source register for modempwr
 */
#define LP_PWR_MODEMPWR_WAKEUP_SRC_REG (DR_REG_LP_PWR_BASE + 0x64)
/** LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN : R/W; bitpos: [30:0]; default: 0;
 *  wakeup enable signal for modempwr
 */
#define LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN    0x7FFFFFFFU
#define LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN_M  (LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN_V << LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN_S)
#define LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN_V  0x7FFFFFFFU
#define LP_PWR_MODEMPWR_WAKEUP_SOURCE_EN_S  0

/** LP_PWR_PERI_PAD_WAKE_SLP_EN_REG register
 *  used for future potential eco, others don't care
 */
#define LP_PWR_PERI_PAD_WAKE_SLP_EN_REG (DR_REG_LP_PWR_BASE + 0x68)
/** LP_PWR_UART_WAKEUP_EN : R/W; bitpos: [0]; default: 0;
 *  1:enables uart wakeup after hpcpu stall
 *  0:disables uart wakeup after hpcpu stall
 */
#define LP_PWR_UART_WAKEUP_EN    (BIT(0))
#define LP_PWR_UART_WAKEUP_EN_M  (LP_PWR_UART_WAKEUP_EN_V << LP_PWR_UART_WAKEUP_EN_S)
#define LP_PWR_UART_WAKEUP_EN_V  0x00000001U
#define LP_PWR_UART_WAKEUP_EN_S  0
/** LP_PWR_HP_PAD_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  1:hppad will sleep after hpcpu stall
 *  0:hppad will not sleep after hpcpu stall
 */
#define LP_PWR_HP_PAD_SLP_SEL    (BIT(1))
#define LP_PWR_HP_PAD_SLP_SEL_M  (LP_PWR_HP_PAD_SLP_SEL_V << LP_PWR_HP_PAD_SLP_SEL_S)
#define LP_PWR_HP_PAD_SLP_SEL_V  0x00000001U
#define LP_PWR_HP_PAD_SLP_SEL_S  1
/** LP_PWR_LP_PAD_SLP_SEL : R/W; bitpos: [2]; default: 0;
 *  1:hppad will sleep after hpcpu stall
 *  0:hppad will not sleep after hpcpu stall
 */
#define LP_PWR_LP_PAD_SLP_SEL    (BIT(2))
#define LP_PWR_LP_PAD_SLP_SEL_M  (LP_PWR_LP_PAD_SLP_SEL_V << LP_PWR_LP_PAD_SLP_SEL_S)
#define LP_PWR_LP_PAD_SLP_SEL_V  0x00000001U
#define LP_PWR_LP_PAD_SLP_SEL_S  2

/** LP_PWR_TOUCH_CTRL_REG register
 *  ctrl register for touch power control
 */
#define LP_PWR_TOUCH_CTRL_REG (DR_REG_LP_PWR_BASE + 0x6c)
/** LP_PWR_TOUCH_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for touch
 */
#define LP_PWR_TOUCH_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_TOUCH_SW_SLEEP_REQ_M  (LP_PWR_TOUCH_SW_SLEEP_REQ_V << LP_PWR_TOUCH_SW_SLEEP_REQ_S)
#define LP_PWR_TOUCH_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_TOUCH_SW_SLEEP_REQ_S  0
/** LP_PWR_TOUCH_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for touch
 */
#define LP_PWR_TOUCH_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_TOUCH_SW_WAKEUP_REQ_M  (LP_PWR_TOUCH_SW_WAKEUP_REQ_V << LP_PWR_TOUCH_SW_WAKEUP_REQ_S)
#define LP_PWR_TOUCH_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_TOUCH_SW_WAKEUP_REQ_S  1

/** LP_PWR_TOUCH_CFG_REG register
 *  ctrl register for touch power control
 */
#define LP_PWR_TOUCH_CFG_REG (DR_REG_LP_PWR_BASE + 0x70)
/** LP_PWR_TOUCH_SLEEP_CYCLES : R/W; bitpos: [25:0]; default: 100;
 *  sleep cycles config for touch
 */
#define LP_PWR_TOUCH_SLEEP_CYCLES    0x03FFFFFFU
#define LP_PWR_TOUCH_SLEEP_CYCLES_M  (LP_PWR_TOUCH_SLEEP_CYCLES_V << LP_PWR_TOUCH_SLEEP_CYCLES_S)
#define LP_PWR_TOUCH_SLEEP_CYCLES_V  0x03FFFFFFU
#define LP_PWR_TOUCH_SLEEP_CYCLES_S  0
/** LP_PWR_TOUCH_SLEEP_TIMER_EN : R/W; bitpos: [26]; default: 0;
 *  1:enables touch timer wakeup source
 *  0:disables touch timer wakeup source
 */
#define LP_PWR_TOUCH_SLEEP_TIMER_EN    (BIT(26))
#define LP_PWR_TOUCH_SLEEP_TIMER_EN_M  (LP_PWR_TOUCH_SLEEP_TIMER_EN_V << LP_PWR_TOUCH_SLEEP_TIMER_EN_S)
#define LP_PWR_TOUCH_SLEEP_TIMER_EN_V  0x00000001U
#define LP_PWR_TOUCH_SLEEP_TIMER_EN_S  26
/** LP_PWR_TOUCH_FORCE_DONE : R/W; bitpos: [27]; default: 0;
 *  1: force touch task to done state, turn off touch
 *  0: no effect
 */
#define LP_PWR_TOUCH_FORCE_DONE    (BIT(27))
#define LP_PWR_TOUCH_FORCE_DONE_M  (LP_PWR_TOUCH_FORCE_DONE_V << LP_PWR_TOUCH_FORCE_DONE_S)
#define LP_PWR_TOUCH_FORCE_DONE_V  0x00000001U
#define LP_PWR_TOUCH_FORCE_DONE_S  27

/** LP_PWR_TOUCH_STS_REG register
 *  status register for TOUCH PWR
 */
#define LP_PWR_TOUCH_STS_REG (DR_REG_LP_PWR_BASE + 0x74)
/** LP_PWR_TOUCH_CUR_ST : RO; bitpos: [2:0]; default: 4;
 *  current power state register for touch
 */
#define LP_PWR_TOUCH_CUR_ST    0x00000007U
#define LP_PWR_TOUCH_CUR_ST_M  (LP_PWR_TOUCH_CUR_ST_V << LP_PWR_TOUCH_CUR_ST_S)
#define LP_PWR_TOUCH_CUR_ST_V  0x00000007U
#define LP_PWR_TOUCH_CUR_ST_S  0
/** LP_PWR_TOUCH_IN_POWER_ON : RO; bitpos: [3]; default: 0;
 *  1:touch current in power on mode
 *  0:touch not in power on mode
 */
#define LP_PWR_TOUCH_IN_POWER_ON    (BIT(3))
#define LP_PWR_TOUCH_IN_POWER_ON_M  (LP_PWR_TOUCH_IN_POWER_ON_V << LP_PWR_TOUCH_IN_POWER_ON_S)
#define LP_PWR_TOUCH_IN_POWER_ON_V  0x00000001U
#define LP_PWR_TOUCH_IN_POWER_ON_S  3

/** LP_PWR_CALI_CTRL_REG register
 *  ctrl register for cali power control
 */
#define LP_PWR_CALI_CTRL_REG (DR_REG_LP_PWR_BASE + 0x78)
/** LP_PWR_CALI_SW_SLEEP_REQ : WT; bitpos: [0]; default: 0;
 *  software sleep request config for cali
 */
#define LP_PWR_CALI_SW_SLEEP_REQ    (BIT(0))
#define LP_PWR_CALI_SW_SLEEP_REQ_M  (LP_PWR_CALI_SW_SLEEP_REQ_V << LP_PWR_CALI_SW_SLEEP_REQ_S)
#define LP_PWR_CALI_SW_SLEEP_REQ_V  0x00000001U
#define LP_PWR_CALI_SW_SLEEP_REQ_S  0
/** LP_PWR_CALI_SW_WAKEUP_REQ : WT; bitpos: [1]; default: 0;
 *  software wakeup request config for cali
 */
#define LP_PWR_CALI_SW_WAKEUP_REQ    (BIT(1))
#define LP_PWR_CALI_SW_WAKEUP_REQ_M  (LP_PWR_CALI_SW_WAKEUP_REQ_V << LP_PWR_CALI_SW_WAKEUP_REQ_S)
#define LP_PWR_CALI_SW_WAKEUP_REQ_V  0x00000001U
#define LP_PWR_CALI_SW_WAKEUP_REQ_S  1

/** LP_PWR_CALI_STS_REG register
 *  status register for CALI PWR
 */
#define LP_PWR_CALI_STS_REG (DR_REG_LP_PWR_BASE + 0x7c)
/** LP_PWR_CALI_CUR_ST : RO; bitpos: [2:0]; default: 4;
 *  current power state register for cali
 */
#define LP_PWR_CALI_CUR_ST    0x00000007U
#define LP_PWR_CALI_CUR_ST_M  (LP_PWR_CALI_CUR_ST_V << LP_PWR_CALI_CUR_ST_S)
#define LP_PWR_CALI_CUR_ST_V  0x00000007U
#define LP_PWR_CALI_CUR_ST_S  0
/** LP_PWR_CALI_IN_POWER_ON : RO; bitpos: [3]; default: 0;
 *  1:cali current in power on mode
 *  0:cali not in power on mode
 */
#define LP_PWR_CALI_IN_POWER_ON    (BIT(3))
#define LP_PWR_CALI_IN_POWER_ON_M  (LP_PWR_CALI_IN_POWER_ON_V << LP_PWR_CALI_IN_POWER_ON_S)
#define LP_PWR_CALI_IN_POWER_ON_V  0x00000001U
#define LP_PWR_CALI_IN_POWER_ON_S  3

/** LP_PWR_LPPERI_PWR_CLK_CFG_REG register
 *  ctrl register for lpcpu power clock mode control
 */
#define LP_PWR_LPPERI_PWR_CLK_CFG_REG (DR_REG_LP_PWR_BASE + 0x80)
/** LP_PWR_LPPERI_SW_CLK_MODE : R/W; bitpos: [1:0]; default: 3;
 *  software configs LPPERI clk mode
 */
#define LP_PWR_LPPERI_SW_CLK_MODE    0x00000003U
#define LP_PWR_LPPERI_SW_CLK_MODE_M  (LP_PWR_LPPERI_SW_CLK_MODE_V << LP_PWR_LPPERI_SW_CLK_MODE_S)
#define LP_PWR_LPPERI_SW_CLK_MODE_V  0x00000003U
#define LP_PWR_LPPERI_SW_CLK_MODE_S  0
/** LP_PWR_LPPERI_SW_PWR_MODE : R/W; bitpos: [3:2]; default: 2;
 *  software configs LPPERI pwr mode
 */
#define LP_PWR_LPPERI_SW_PWR_MODE    0x00000003U
#define LP_PWR_LPPERI_SW_PWR_MODE_M  (LP_PWR_LPPERI_SW_PWR_MODE_V << LP_PWR_LPPERI_SW_PWR_MODE_S)
#define LP_PWR_LPPERI_SW_PWR_MODE_V  0x00000003U
#define LP_PWR_LPPERI_SW_PWR_MODE_S  2

/** LP_PWR_LPPERI_CTRL_REG register
 *  ctrl register for lpperi power control
 */
#define LP_PWR_LPPERI_CTRL_REG (DR_REG_LP_PWR_BASE + 0x84)
/** LP_PWR_LPPERI_SW_FORCE_ON : R/W; bitpos: [0]; default: 1;
 *  1: software wake lpperi
 */
#define LP_PWR_LPPERI_SW_FORCE_ON    (BIT(0))
#define LP_PWR_LPPERI_SW_FORCE_ON_M  (LP_PWR_LPPERI_SW_FORCE_ON_V << LP_PWR_LPPERI_SW_FORCE_ON_S)
#define LP_PWR_LPPERI_SW_FORCE_ON_V  0x00000001U
#define LP_PWR_LPPERI_SW_FORCE_ON_S  0
/** LP_PWR_LPPERI_SW_REQ : WT; bitpos: [1]; default: 0;
 *  software req  pulse for following mode register
 */
#define LP_PWR_LPPERI_SW_REQ    (BIT(1))
#define LP_PWR_LPPERI_SW_REQ_M  (LP_PWR_LPPERI_SW_REQ_V << LP_PWR_LPPERI_SW_REQ_S)
#define LP_PWR_LPPERI_SW_REQ_V  0x00000001U
#define LP_PWR_LPPERI_SW_REQ_S  1

/** LP_PWR_LP_PAD_CFG_REG register
 *  config register for lppad hold signal
 */
#define LP_PWR_LP_PAD_CFG_REG (DR_REG_LP_PWR_BASE + 0x88)
/** LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN : R/W; bitpos: [0]; default: 0;
 *  1: hold lppad when hpcpu stalls
 *  0: not hold lppad when hpcpu stalls
 */
#define LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN    (BIT(0))
#define LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN_M  (LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN_V << LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN_S)
#define LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN_V  0x00000001U
#define LP_PWR_LPPAD_HOLD_CPU_PAUSE_EN_S  0
/** LP_PWR_FORCE_HIGH_LP_PAD_HOLD : R/W; bitpos: [1]; default: 0;
 *  1:force pull up lppad hold signal
 *  0:no effect
 */
#define LP_PWR_FORCE_HIGH_LP_PAD_HOLD    (BIT(1))
#define LP_PWR_FORCE_HIGH_LP_PAD_HOLD_M  (LP_PWR_FORCE_HIGH_LP_PAD_HOLD_V << LP_PWR_FORCE_HIGH_LP_PAD_HOLD_S)
#define LP_PWR_FORCE_HIGH_LP_PAD_HOLD_V  0x00000001U
#define LP_PWR_FORCE_HIGH_LP_PAD_HOLD_S  1
/** LP_PWR_FORCE_LOW_LP_PAD_HOLD : R/W; bitpos: [2]; default: 0;
 *  1:force pull low lppad hold signal
 *  0:no effect
 */
#define LP_PWR_FORCE_LOW_LP_PAD_HOLD    (BIT(2))
#define LP_PWR_FORCE_LOW_LP_PAD_HOLD_M  (LP_PWR_FORCE_LOW_LP_PAD_HOLD_V << LP_PWR_FORCE_LOW_LP_PAD_HOLD_S)
#define LP_PWR_FORCE_LOW_LP_PAD_HOLD_V  0x00000001U
#define LP_PWR_FORCE_LOW_LP_PAD_HOLD_S  2

/** LP_PWR_CNNT_PAD_CFG_REG register
 *  config register for cnntpad hold signal
 */
#define LP_PWR_CNNT_PAD_CFG_REG (DR_REG_LP_PWR_BASE + 0x8c)
/** LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN : R/W; bitpos: [0]; default: 0;
 *  1: hold cnntpad when hpcpu stalls
 *  0: not hold cnntpad when hpcpu stalls
 */
#define LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN    (BIT(0))
#define LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN_M  (LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN_V << LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN_S)
#define LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN_V  0x00000001U
#define LP_PWR_CNNT_PAD_HOLD_CPU_PAUSE_EN_S  0
/** LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD : R/W; bitpos: [1]; default: 0;
 *  1:force pull up cnntpad hold signal
 *  0:no effect
 */
#define LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD    (BIT(1))
#define LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD_M  (LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD_V << LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD_S)
#define LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD_V  0x00000001U
#define LP_PWR_FORCE_HIGH_CNNT_PAD_HOLD_S  1
/** LP_PWR_FORCE_LOW_CNNT_PAD_HOLD : R/W; bitpos: [2]; default: 0;
 *  1:force pull low cnntpad hold signal
 *  0:no effect
 */
#define LP_PWR_FORCE_LOW_CNNT_PAD_HOLD    (BIT(2))
#define LP_PWR_FORCE_LOW_CNNT_PAD_HOLD_M  (LP_PWR_FORCE_LOW_CNNT_PAD_HOLD_V << LP_PWR_FORCE_LOW_CNNT_PAD_HOLD_S)
#define LP_PWR_FORCE_LOW_CNNT_PAD_HOLD_V  0x00000001U
#define LP_PWR_FORCE_LOW_CNNT_PAD_HOLD_S  2

/** LP_PWR_HP_PAD_CFG_REG register
 *  config register for hppad hold signal
 */
#define LP_PWR_HP_PAD_CFG_REG (DR_REG_LP_PWR_BASE + 0x90)
/** LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN : R/W; bitpos: [0]; default: 0;
 *  1: hold hppad when hpcpu stalls
 *  0: not hold hppad when hpcpu stalls
 */
#define LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN    (BIT(0))
#define LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN_M  (LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN_V << LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN_S)
#define LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN_V  0x00000001U
#define LP_PWR_HPPAD_HOLD_CPU_PAUSE_EN_S  0
/** LP_PWR_FORCE_HIGH_HP_PAD_HOLD : R/W; bitpos: [1]; default: 0;
 *  1:force pull up hppad hold signal
 *  0:no effect
 */
#define LP_PWR_FORCE_HIGH_HP_PAD_HOLD    (BIT(1))
#define LP_PWR_FORCE_HIGH_HP_PAD_HOLD_M  (LP_PWR_FORCE_HIGH_HP_PAD_HOLD_V << LP_PWR_FORCE_HIGH_HP_PAD_HOLD_S)
#define LP_PWR_FORCE_HIGH_HP_PAD_HOLD_V  0x00000001U
#define LP_PWR_FORCE_HIGH_HP_PAD_HOLD_S  1
/** LP_PWR_FORCE_LOW_HP_PAD_HOLD : R/W; bitpos: [2]; default: 0;
 *  1:force pull low hppad hold signal
 *  0:no effect
 */
#define LP_PWR_FORCE_LOW_HP_PAD_HOLD    (BIT(2))
#define LP_PWR_FORCE_LOW_HP_PAD_HOLD_M  (LP_PWR_FORCE_LOW_HP_PAD_HOLD_V << LP_PWR_FORCE_LOW_HP_PAD_HOLD_S)
#define LP_PWR_FORCE_LOW_HP_PAD_HOLD_V  0x00000001U
#define LP_PWR_FORCE_LOW_HP_PAD_HOLD_S  2

/** LP_PWR_INT_RAW_REG register
 *  need_des
 */
#define LP_PWR_INT_RAW_REG (DR_REG_LP_PWR_BASE + 0x94)
/** LP_PWR_ZB_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [24]; default: 0;
 *  need_des
 */
#define LP_PWR_ZB_WAKE_PROCESS_INT_RAW    (BIT(24))
#define LP_PWR_ZB_WAKE_PROCESS_INT_RAW_M  (LP_PWR_ZB_WAKE_PROCESS_INT_RAW_V << LP_PWR_ZB_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_ZB_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_ZB_WAKE_PROCESS_INT_RAW_S  24
/** LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [25]; default: 0;
 *  need_des
 */
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW    (BIT(25))
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW_M  (LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW_V << LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_RAW_S  25
/** LP_PWR_BLE_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [26]; default: 0;
 *  need_des
 */
#define LP_PWR_BLE_WAKE_PROCESS_INT_RAW    (BIT(26))
#define LP_PWR_BLE_WAKE_PROCESS_INT_RAW_M  (LP_PWR_BLE_WAKE_PROCESS_INT_RAW_V << LP_PWR_BLE_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_BLE_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_BLE_WAKE_PROCESS_INT_RAW_S  26
/** LP_PWR_WIFI_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_PWR_WIFI_WAKE_PROCESS_INT_RAW    (BIT(27))
#define LP_PWR_WIFI_WAKE_PROCESS_INT_RAW_M  (LP_PWR_WIFI_WAKE_PROCESS_INT_RAW_V << LP_PWR_WIFI_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_WIFI_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_WIFI_WAKE_PROCESS_INT_RAW_S  27
/** LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW    (BIT(28))
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW_M  (LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW_V << LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_RAW_S  28
/** LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW    (BIT(29))
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW_M  (LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW_V << LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_RAW_S  29
/** LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW    (BIT(30))
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW_M  (LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW_V << LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_RAW_S  30
/** LP_PWR_APPWR_WAKE_PROCESS_INT_RAW : R/WTC/SS; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_PWR_APPWR_WAKE_PROCESS_INT_RAW    (BIT(31))
#define LP_PWR_APPWR_WAKE_PROCESS_INT_RAW_M  (LP_PWR_APPWR_WAKE_PROCESS_INT_RAW_V << LP_PWR_APPWR_WAKE_PROCESS_INT_RAW_S)
#define LP_PWR_APPWR_WAKE_PROCESS_INT_RAW_V  0x00000001U
#define LP_PWR_APPWR_WAKE_PROCESS_INT_RAW_S  31

/** LP_PWR_INT_ST_REG register
 *  need_des
 */
#define LP_PWR_INT_ST_REG (DR_REG_LP_PWR_BASE + 0x98)
/** LP_PWR_ZB_WAKE_PROCESS_INT_ST : RO; bitpos: [24]; default: 0;
 *  need_des
 */
#define LP_PWR_ZB_WAKE_PROCESS_INT_ST    (BIT(24))
#define LP_PWR_ZB_WAKE_PROCESS_INT_ST_M  (LP_PWR_ZB_WAKE_PROCESS_INT_ST_V << LP_PWR_ZB_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_ZB_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_ZB_WAKE_PROCESS_INT_ST_S  24
/** LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST : RO; bitpos: [25]; default: 0;
 *  need_des
 */
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST    (BIT(25))
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST_M  (LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST_V << LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ST_S  25
/** LP_PWR_BLE_WAKE_PROCESS_INT_ST : RO; bitpos: [26]; default: 0;
 *  need_des
 */
#define LP_PWR_BLE_WAKE_PROCESS_INT_ST    (BIT(26))
#define LP_PWR_BLE_WAKE_PROCESS_INT_ST_M  (LP_PWR_BLE_WAKE_PROCESS_INT_ST_V << LP_PWR_BLE_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_BLE_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_BLE_WAKE_PROCESS_INT_ST_S  26
/** LP_PWR_WIFI_WAKE_PROCESS_INT_ST : RO; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ST    (BIT(27))
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ST_M  (LP_PWR_WIFI_WAKE_PROCESS_INT_ST_V << LP_PWR_WIFI_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ST_S  27
/** LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST : RO; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST    (BIT(28))
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST_M  (LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST_V << LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ST_S  28
/** LP_PWR_TOUCH_WAKE_PROCESS_INT_ST : RO; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ST    (BIT(29))
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ST_M  (LP_PWR_TOUCH_WAKE_PROCESS_INT_ST_V << LP_PWR_TOUCH_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ST_S  29
/** LP_PWR_LPCPU_WAKE_PROCESS_INT_ST : RO; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ST    (BIT(30))
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ST_M  (LP_PWR_LPCPU_WAKE_PROCESS_INT_ST_V << LP_PWR_LPCPU_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ST_S  30
/** LP_PWR_APPWR_WAKE_PROCESS_INT_ST : RO; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ST    (BIT(31))
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ST_M  (LP_PWR_APPWR_WAKE_PROCESS_INT_ST_V << LP_PWR_APPWR_WAKE_PROCESS_INT_ST_S)
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ST_V  0x00000001U
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ST_S  31

/** LP_PWR_INT_ENA_REG register
 *  need_des
 */
#define LP_PWR_INT_ENA_REG (DR_REG_LP_PWR_BASE + 0x9c)
/** LP_PWR_ZB_WAKE_PROCESS_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  need_des
 */
#define LP_PWR_ZB_WAKE_PROCESS_INT_ENA    (BIT(24))
#define LP_PWR_ZB_WAKE_PROCESS_INT_ENA_M  (LP_PWR_ZB_WAKE_PROCESS_INT_ENA_V << LP_PWR_ZB_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_ZB_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_ZB_WAKE_PROCESS_INT_ENA_S  24
/** LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA : R/W; bitpos: [25]; default: 0;
 *  need_des
 */
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA    (BIT(25))
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA_M  (LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA_V << LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_ENA_S  25
/** LP_PWR_BLE_WAKE_PROCESS_INT_ENA : R/W; bitpos: [26]; default: 0;
 *  need_des
 */
#define LP_PWR_BLE_WAKE_PROCESS_INT_ENA    (BIT(26))
#define LP_PWR_BLE_WAKE_PROCESS_INT_ENA_M  (LP_PWR_BLE_WAKE_PROCESS_INT_ENA_V << LP_PWR_BLE_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_BLE_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_BLE_WAKE_PROCESS_INT_ENA_S  26
/** LP_PWR_WIFI_WAKE_PROCESS_INT_ENA : R/W; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ENA    (BIT(27))
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ENA_M  (LP_PWR_WIFI_WAKE_PROCESS_INT_ENA_V << LP_PWR_WIFI_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_WIFI_WAKE_PROCESS_INT_ENA_S  27
/** LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA : R/W; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA    (BIT(28))
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA_M  (LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA_V << LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_ENA_S  28
/** LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA : R/W; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA    (BIT(29))
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA_M  (LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA_V << LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_ENA_S  29
/** LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA : R/W; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA    (BIT(30))
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA_M  (LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA_V << LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_ENA_S  30
/** LP_PWR_APPWR_WAKE_PROCESS_INT_ENA : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ENA    (BIT(31))
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ENA_M  (LP_PWR_APPWR_WAKE_PROCESS_INT_ENA_V << LP_PWR_APPWR_WAKE_PROCESS_INT_ENA_S)
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ENA_V  0x00000001U
#define LP_PWR_APPWR_WAKE_PROCESS_INT_ENA_S  31

/** LP_PWR_INT_CLR_REG register
 *  need_des
 */
#define LP_PWR_INT_CLR_REG (DR_REG_LP_PWR_BASE + 0xa0)
/** LP_PWR_ZB_WAKE_PROCESS_INT_CLR : WT; bitpos: [24]; default: 0;
 *  need_des
 */
#define LP_PWR_ZB_WAKE_PROCESS_INT_CLR    (BIT(24))
#define LP_PWR_ZB_WAKE_PROCESS_INT_CLR_M  (LP_PWR_ZB_WAKE_PROCESS_INT_CLR_V << LP_PWR_ZB_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_ZB_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_ZB_WAKE_PROCESS_INT_CLR_S  24
/** LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR : WT; bitpos: [25]; default: 0;
 *  need_des
 */
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR    (BIT(25))
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR_M  (LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR_V << LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_MODEMPWR_WAKE_PROCESS_INT_CLR_S  25
/** LP_PWR_BLE_WAKE_PROCESS_INT_CLR : WT; bitpos: [26]; default: 0;
 *  need_des
 */
#define LP_PWR_BLE_WAKE_PROCESS_INT_CLR    (BIT(26))
#define LP_PWR_BLE_WAKE_PROCESS_INT_CLR_M  (LP_PWR_BLE_WAKE_PROCESS_INT_CLR_V << LP_PWR_BLE_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_BLE_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_BLE_WAKE_PROCESS_INT_CLR_S  26
/** LP_PWR_WIFI_WAKE_PROCESS_INT_CLR : WT; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_PWR_WIFI_WAKE_PROCESS_INT_CLR    (BIT(27))
#define LP_PWR_WIFI_WAKE_PROCESS_INT_CLR_M  (LP_PWR_WIFI_WAKE_PROCESS_INT_CLR_V << LP_PWR_WIFI_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_WIFI_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_WIFI_WAKE_PROCESS_INT_CLR_S  27
/** LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR : WT; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR    (BIT(28))
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR_M  (LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR_V << LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_XTAL_CALI_WAKE_PROCESS_INT_CLR_S  28
/** LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR : WT; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR    (BIT(29))
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR_M  (LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR_V << LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_TOUCH_WAKE_PROCESS_INT_CLR_S  29
/** LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR : WT; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR    (BIT(30))
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR_M  (LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR_V << LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_LPCPU_WAKE_PROCESS_INT_CLR_S  30
/** LP_PWR_APPWR_WAKE_PROCESS_INT_CLR : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_PWR_APPWR_WAKE_PROCESS_INT_CLR    (BIT(31))
#define LP_PWR_APPWR_WAKE_PROCESS_INT_CLR_M  (LP_PWR_APPWR_WAKE_PROCESS_INT_CLR_V << LP_PWR_APPWR_WAKE_PROCESS_INT_CLR_S)
#define LP_PWR_APPWR_WAKE_PROCESS_INT_CLR_V  0x00000001U
#define LP_PWR_APPWR_WAKE_PROCESS_INT_CLR_S  31

/** LP_PWR_VER_DATE_REG register
 *  need_des
 */
#define LP_PWR_VER_DATE_REG (DR_REG_LP_PWR_BASE + 0xa4)
/** LP_PWR_VER_DATE : R/W; bitpos: [30:0]; default: 2426648;
 *  need_des
 */
#define LP_PWR_VER_DATE    0x7FFFFFFFU
#define LP_PWR_VER_DATE_M  (LP_PWR_VER_DATE_V << LP_PWR_VER_DATE_S)
#define LP_PWR_VER_DATE_V  0x7FFFFFFFU
#define LP_PWR_VER_DATE_S  0
/** LP_PWR_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_PWR_CLK_EN    (BIT(31))
#define LP_PWR_CLK_EN_M  (LP_PWR_CLK_EN_V << LP_PWR_CLK_EN_S)
#define LP_PWR_CLK_EN_V  0x00000001U
#define LP_PWR_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
