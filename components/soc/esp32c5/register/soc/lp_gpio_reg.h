/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** LP_GPIO_OUT_REG register
 *  GPIO output register for GPIO0-6
 */
#define LP_GPIO_OUT_REG (DR_REG_LP_GPIO_BASE + 0x4)
/** LP_GPIO_OUT_DATA_ORIG : R/W/WTC; bitpos: [6:0]; default: 0;
 *  GPIO output register for GPIO0-6
 */
#define LP_GPIO_OUT_DATA_ORIG    0x0000007FU
#define LP_GPIO_OUT_DATA_ORIG_M  (LP_GPIO_OUT_DATA_ORIG_V << LP_GPIO_OUT_DATA_ORIG_S)
#define LP_GPIO_OUT_DATA_ORIG_V  0x0000007FU
#define LP_GPIO_OUT_DATA_ORIG_S  0

/** LP_GPIO_OUT_W1TS_REG register
 *  GPIO output set register for GPIO0-6
 */
#define LP_GPIO_OUT_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x8)
/** LP_GPIO_OUT_W1TS : WT; bitpos: [6:0]; default: 0;
 *  GPIO output set register for GPIO0-6
 */
#define LP_GPIO_OUT_W1TS    0x0000007FU
#define LP_GPIO_OUT_W1TS_M  (LP_GPIO_OUT_W1TS_V << LP_GPIO_OUT_W1TS_S)
#define LP_GPIO_OUT_W1TS_V  0x0000007FU
#define LP_GPIO_OUT_W1TS_S  0

/** LP_GPIO_OUT_W1TC_REG register
 *  GPIO output clear register for GPIO0-6
 */
#define LP_GPIO_OUT_W1TC_REG (DR_REG_LP_GPIO_BASE + 0xc)
/** LP_GPIO_OUT_W1TC : WT; bitpos: [6:0]; default: 0;
 *  GPIO output clear register for GPIO0-6
 */
#define LP_GPIO_OUT_W1TC    0x0000007FU
#define LP_GPIO_OUT_W1TC_M  (LP_GPIO_OUT_W1TC_V << LP_GPIO_OUT_W1TC_S)
#define LP_GPIO_OUT_W1TC_V  0x0000007FU
#define LP_GPIO_OUT_W1TC_S  0

/** LP_GPIO_ENABLE_REG register
 *  GPIO output enable register for GPIO0-6
 */
#define LP_GPIO_ENABLE_REG (DR_REG_LP_GPIO_BASE + 0x10)
/** LP_GPIO_ENABLE_DATA : R/W/WTC; bitpos: [6:0]; default: 0;
 *  GPIO output enable register for GPIO0-6
 */
#define LP_GPIO_ENABLE_DATA    0x0000007FU
#define LP_GPIO_ENABLE_DATA_M  (LP_GPIO_ENABLE_DATA_V << LP_GPIO_ENABLE_DATA_S)
#define LP_GPIO_ENABLE_DATA_V  0x0000007FU
#define LP_GPIO_ENABLE_DATA_S  0

/** LP_GPIO_ENABLE_W1TS_REG register
 *  GPIO output enable set register for GPIO0-6
 */
#define LP_GPIO_ENABLE_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x14)
/** LP_GPIO_ENABLE_W1TS : WT; bitpos: [6:0]; default: 0;
 *  GPIO output enable set register for GPIO0-6
 */
#define LP_GPIO_ENABLE_W1TS    0x0000007FU
#define LP_GPIO_ENABLE_W1TS_M  (LP_GPIO_ENABLE_W1TS_V << LP_GPIO_ENABLE_W1TS_S)
#define LP_GPIO_ENABLE_W1TS_V  0x0000007FU
#define LP_GPIO_ENABLE_W1TS_S  0

/** LP_GPIO_ENABLE_W1TC_REG register
 *  GPIO output enable clear register for GPIO0-6
 */
#define LP_GPIO_ENABLE_W1TC_REG (DR_REG_LP_GPIO_BASE + 0x18)
/** LP_GPIO_ENABLE_W1TC : WT; bitpos: [6:0]; default: 0;
 *  GPIO output enable clear register for GPIO0-6
 */
#define LP_GPIO_ENABLE_W1TC    0x0000007FU
#define LP_GPIO_ENABLE_W1TC_M  (LP_GPIO_ENABLE_W1TC_V << LP_GPIO_ENABLE_W1TC_S)
#define LP_GPIO_ENABLE_W1TC_V  0x0000007FU
#define LP_GPIO_ENABLE_W1TC_S  0

/** LP_GPIO_IN_REG register
 *  GPIO input register for GPIO0-6
 */
#define LP_GPIO_IN_REG (DR_REG_LP_GPIO_BASE + 0x1c)
/** LP_GPIO_IN_DATA_NEXT : RO; bitpos: [6:0]; default: 0;
 *  GPIO input register for GPIO0-6
 */
#define LP_GPIO_IN_DATA_NEXT    0x0000007FU
#define LP_GPIO_IN_DATA_NEXT_M  (LP_GPIO_IN_DATA_NEXT_V << LP_GPIO_IN_DATA_NEXT_S)
#define LP_GPIO_IN_DATA_NEXT_V  0x0000007FU
#define LP_GPIO_IN_DATA_NEXT_S  0

/** LP_GPIO_STATUS_REG register
 *  GPIO interrupt status register for GPIO0-6
 */
#define LP_GPIO_STATUS_REG (DR_REG_LP_GPIO_BASE + 0x20)
/** LP_GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [6:0]; default: 0;
 *  GPIO interrupt status register for GPIO0-6
 */
#define LP_GPIO_STATUS_INTERRUPT    0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_M  (LP_GPIO_STATUS_INTERRUPT_V << LP_GPIO_STATUS_INTERRUPT_S)
#define LP_GPIO_STATUS_INTERRUPT_V  0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_S  0

/** LP_GPIO_STATUS_W1TS_REG register
 *  GPIO interrupt status set register for GPIO0-6
 */
#define LP_GPIO_STATUS_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x24)
/** LP_GPIO_STATUS_W1TS : WT; bitpos: [6:0]; default: 0;
 *  GPIO interrupt status set register for GPIO0-6
 */
#define LP_GPIO_STATUS_W1TS    0x0000007FU
#define LP_GPIO_STATUS_W1TS_M  (LP_GPIO_STATUS_W1TS_V << LP_GPIO_STATUS_W1TS_S)
#define LP_GPIO_STATUS_W1TS_V  0x0000007FU
#define LP_GPIO_STATUS_W1TS_S  0

/** LP_GPIO_STATUS_W1TC_REG register
 *  GPIO interrupt status clear register for GPIO0-6
 */
#define LP_GPIO_STATUS_W1TC_REG (DR_REG_LP_GPIO_BASE + 0x28)
/** LP_GPIO_STATUS_W1TC : WT; bitpos: [6:0]; default: 0;
 *  GPIO interrupt status clear register for GPIO0-6
 */
#define LP_GPIO_STATUS_W1TC    0x0000007FU
#define LP_GPIO_STATUS_W1TC_M  (LP_GPIO_STATUS_W1TC_V << LP_GPIO_STATUS_W1TC_S)
#define LP_GPIO_STATUS_W1TC_V  0x0000007FU
#define LP_GPIO_STATUS_W1TC_S  0

/** LP_GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register for GPIO0-6
 */
#define LP_GPIO_STATUS_NEXT_REG (DR_REG_LP_GPIO_BASE + 0x2c)
/** LP_GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [6:0]; default: 0;
 *  GPIO interrupt source register for GPIO0-6
 */
#define LP_GPIO_STATUS_INTERRUPT_NEXT    0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_NEXT_M  (LP_GPIO_STATUS_INTERRUPT_NEXT_V << LP_GPIO_STATUS_INTERRUPT_NEXT_S)
#define LP_GPIO_STATUS_INTERRUPT_NEXT_V  0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_NEXT_S  0

/** LP_GPIO_PIN0_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN0_REG (DR_REG_LP_GPIO_BASE + 0x30)
/** LP_GPIO_PIN0_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN0_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN0_SYNC2_BYPASS_M  (LP_GPIO_PIN0_SYNC2_BYPASS_V << LP_GPIO_PIN0_SYNC2_BYPASS_S)
#define LP_GPIO_PIN0_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN0_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN0_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN0_PAD_DRIVER_M  (LP_GPIO_PIN0_PAD_DRIVER_V << LP_GPIO_PIN0_PAD_DRIVER_S)
#define LP_GPIO_PIN0_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN0_PAD_DRIVER_S  2
/** LP_GPIO_PIN0_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN0_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN0_SYNC1_BYPASS_M  (LP_GPIO_PIN0_SYNC1_BYPASS_V << LP_GPIO_PIN0_SYNC1_BYPASS_S)
#define LP_GPIO_PIN0_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN0_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN0_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN0_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN0_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN0_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN0_INT_TYPE    0x00000007U
#define LP_GPIO_PIN0_INT_TYPE_M  (LP_GPIO_PIN0_INT_TYPE_V << LP_GPIO_PIN0_INT_TYPE_S)
#define LP_GPIO_PIN0_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN0_INT_TYPE_S  7
/** LP_GPIO_PIN0_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN0_WAKEUP_ENABLE_M  (LP_GPIO_PIN0_WAKEUP_ENABLE_V << LP_GPIO_PIN0_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN0_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN0_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN1_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN1_REG (DR_REG_LP_GPIO_BASE + 0x34)
/** LP_GPIO_PIN1_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN1_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN1_SYNC2_BYPASS_M  (LP_GPIO_PIN1_SYNC2_BYPASS_V << LP_GPIO_PIN1_SYNC2_BYPASS_S)
#define LP_GPIO_PIN1_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN1_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN1_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN1_PAD_DRIVER_M  (LP_GPIO_PIN1_PAD_DRIVER_V << LP_GPIO_PIN1_PAD_DRIVER_S)
#define LP_GPIO_PIN1_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN1_PAD_DRIVER_S  2
/** LP_GPIO_PIN1_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN1_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN1_SYNC1_BYPASS_M  (LP_GPIO_PIN1_SYNC1_BYPASS_V << LP_GPIO_PIN1_SYNC1_BYPASS_S)
#define LP_GPIO_PIN1_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN1_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN1_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN1_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN1_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN1_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN1_INT_TYPE    0x00000007U
#define LP_GPIO_PIN1_INT_TYPE_M  (LP_GPIO_PIN1_INT_TYPE_V << LP_GPIO_PIN1_INT_TYPE_S)
#define LP_GPIO_PIN1_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN1_INT_TYPE_S  7
/** LP_GPIO_PIN1_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN1_WAKEUP_ENABLE_M  (LP_GPIO_PIN1_WAKEUP_ENABLE_V << LP_GPIO_PIN1_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN1_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN1_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN2_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN2_REG (DR_REG_LP_GPIO_BASE + 0x38)
/** LP_GPIO_PIN2_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN2_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN2_SYNC2_BYPASS_M  (LP_GPIO_PIN2_SYNC2_BYPASS_V << LP_GPIO_PIN2_SYNC2_BYPASS_S)
#define LP_GPIO_PIN2_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN2_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN2_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN2_PAD_DRIVER_M  (LP_GPIO_PIN2_PAD_DRIVER_V << LP_GPIO_PIN2_PAD_DRIVER_S)
#define LP_GPIO_PIN2_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN2_PAD_DRIVER_S  2
/** LP_GPIO_PIN2_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN2_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN2_SYNC1_BYPASS_M  (LP_GPIO_PIN2_SYNC1_BYPASS_V << LP_GPIO_PIN2_SYNC1_BYPASS_S)
#define LP_GPIO_PIN2_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN2_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN2_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN2_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN2_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN2_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN2_INT_TYPE    0x00000007U
#define LP_GPIO_PIN2_INT_TYPE_M  (LP_GPIO_PIN2_INT_TYPE_V << LP_GPIO_PIN2_INT_TYPE_S)
#define LP_GPIO_PIN2_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN2_INT_TYPE_S  7
/** LP_GPIO_PIN2_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN2_WAKEUP_ENABLE_M  (LP_GPIO_PIN2_WAKEUP_ENABLE_V << LP_GPIO_PIN2_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN2_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN2_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN3_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN3_REG (DR_REG_LP_GPIO_BASE + 0x3c)
/** LP_GPIO_PIN3_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN3_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN3_SYNC2_BYPASS_M  (LP_GPIO_PIN3_SYNC2_BYPASS_V << LP_GPIO_PIN3_SYNC2_BYPASS_S)
#define LP_GPIO_PIN3_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN3_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN3_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN3_PAD_DRIVER_M  (LP_GPIO_PIN3_PAD_DRIVER_V << LP_GPIO_PIN3_PAD_DRIVER_S)
#define LP_GPIO_PIN3_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN3_PAD_DRIVER_S  2
/** LP_GPIO_PIN3_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN3_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN3_SYNC1_BYPASS_M  (LP_GPIO_PIN3_SYNC1_BYPASS_V << LP_GPIO_PIN3_SYNC1_BYPASS_S)
#define LP_GPIO_PIN3_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN3_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN3_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN3_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN3_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN3_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN3_INT_TYPE    0x00000007U
#define LP_GPIO_PIN3_INT_TYPE_M  (LP_GPIO_PIN3_INT_TYPE_V << LP_GPIO_PIN3_INT_TYPE_S)
#define LP_GPIO_PIN3_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN3_INT_TYPE_S  7
/** LP_GPIO_PIN3_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN3_WAKEUP_ENABLE_M  (LP_GPIO_PIN3_WAKEUP_ENABLE_V << LP_GPIO_PIN3_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN3_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN3_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN4_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN4_REG (DR_REG_LP_GPIO_BASE + 0x40)
/** LP_GPIO_PIN4_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN4_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN4_SYNC2_BYPASS_M  (LP_GPIO_PIN4_SYNC2_BYPASS_V << LP_GPIO_PIN4_SYNC2_BYPASS_S)
#define LP_GPIO_PIN4_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN4_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN4_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN4_PAD_DRIVER_M  (LP_GPIO_PIN4_PAD_DRIVER_V << LP_GPIO_PIN4_PAD_DRIVER_S)
#define LP_GPIO_PIN4_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN4_PAD_DRIVER_S  2
/** LP_GPIO_PIN4_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN4_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN4_SYNC1_BYPASS_M  (LP_GPIO_PIN4_SYNC1_BYPASS_V << LP_GPIO_PIN4_SYNC1_BYPASS_S)
#define LP_GPIO_PIN4_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN4_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN4_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN4_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN4_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN4_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN4_INT_TYPE    0x00000007U
#define LP_GPIO_PIN4_INT_TYPE_M  (LP_GPIO_PIN4_INT_TYPE_V << LP_GPIO_PIN4_INT_TYPE_S)
#define LP_GPIO_PIN4_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN4_INT_TYPE_S  7
/** LP_GPIO_PIN4_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN4_WAKEUP_ENABLE_M  (LP_GPIO_PIN4_WAKEUP_ENABLE_V << LP_GPIO_PIN4_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN4_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN4_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN5_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN5_REG (DR_REG_LP_GPIO_BASE + 0x44)
/** LP_GPIO_PIN5_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN5_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN5_SYNC2_BYPASS_M  (LP_GPIO_PIN5_SYNC2_BYPASS_V << LP_GPIO_PIN5_SYNC2_BYPASS_S)
#define LP_GPIO_PIN5_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN5_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN5_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN5_PAD_DRIVER_M  (LP_GPIO_PIN5_PAD_DRIVER_V << LP_GPIO_PIN5_PAD_DRIVER_S)
#define LP_GPIO_PIN5_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN5_PAD_DRIVER_S  2
/** LP_GPIO_PIN5_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN5_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN5_SYNC1_BYPASS_M  (LP_GPIO_PIN5_SYNC1_BYPASS_V << LP_GPIO_PIN5_SYNC1_BYPASS_S)
#define LP_GPIO_PIN5_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN5_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN5_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN5_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN5_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN5_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN5_INT_TYPE    0x00000007U
#define LP_GPIO_PIN5_INT_TYPE_M  (LP_GPIO_PIN5_INT_TYPE_V << LP_GPIO_PIN5_INT_TYPE_S)
#define LP_GPIO_PIN5_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN5_INT_TYPE_S  7
/** LP_GPIO_PIN5_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN5_WAKEUP_ENABLE_M  (LP_GPIO_PIN5_WAKEUP_ENABLE_V << LP_GPIO_PIN5_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN5_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN5_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN6_REG register
 *  GPIO pin configuration register
 */
#define LP_GPIO_PIN6_REG (DR_REG_LP_GPIO_BASE + 0x48)
/** LP_GPIO_PIN6_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN6_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN6_SYNC2_BYPASS_M  (LP_GPIO_PIN6_SYNC2_BYPASS_V << LP_GPIO_PIN6_SYNC2_BYPASS_S)
#define LP_GPIO_PIN6_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN6_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN6_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define LP_GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN6_PAD_DRIVER_M  (LP_GPIO_PIN6_PAD_DRIVER_V << LP_GPIO_PIN6_PAD_DRIVER_S)
#define LP_GPIO_PIN6_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN6_PAD_DRIVER_S  2
/** LP_GPIO_PIN6_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define LP_GPIO_PIN6_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN6_SYNC1_BYPASS_M  (LP_GPIO_PIN6_SYNC1_BYPASS_V << LP_GPIO_PIN6_SYNC1_BYPASS_S)
#define LP_GPIO_PIN6_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN6_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN6_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  GPIO wakeup clear register.
 */
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN6_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN6_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN6_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define LP_GPIO_PIN6_INT_TYPE    0x00000007U
#define LP_GPIO_PIN6_INT_TYPE_M  (LP_GPIO_PIN6_INT_TYPE_V << LP_GPIO_PIN6_INT_TYPE_S)
#define LP_GPIO_PIN6_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN6_INT_TYPE_S  7
/** LP_GPIO_PIN6_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define LP_GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN6_WAKEUP_ENABLE_M  (LP_GPIO_PIN6_WAKEUP_ENABLE_V << LP_GPIO_PIN6_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN6_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN6_WAKEUP_ENABLE_S  10

/** LP_GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2b0)
/** LP_GPIO_FUNC0_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC0_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC0_OUT_INV_SEL_M  (LP_GPIO_FUNC0_OUT_INV_SEL_V << LP_GPIO_FUNC0_OUT_INV_SEL_S)
#define LP_GPIO_FUNC0_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC0_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC0_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC0_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC0_OE_INV_SEL_M  (LP_GPIO_FUNC0_OE_INV_SEL_V << LP_GPIO_FUNC0_OE_INV_SEL_S)
#define LP_GPIO_FUNC0_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC0_OE_INV_SEL_S  2

/** LP_GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2b4)
/** LP_GPIO_FUNC1_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC1_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC1_OUT_INV_SEL_M  (LP_GPIO_FUNC1_OUT_INV_SEL_V << LP_GPIO_FUNC1_OUT_INV_SEL_S)
#define LP_GPIO_FUNC1_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC1_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC1_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC1_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC1_OE_INV_SEL_M  (LP_GPIO_FUNC1_OE_INV_SEL_V << LP_GPIO_FUNC1_OE_INV_SEL_S)
#define LP_GPIO_FUNC1_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC1_OE_INV_SEL_S  2

/** LP_GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2b8)
/** LP_GPIO_FUNC2_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC2_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC2_OUT_INV_SEL_M  (LP_GPIO_FUNC2_OUT_INV_SEL_V << LP_GPIO_FUNC2_OUT_INV_SEL_S)
#define LP_GPIO_FUNC2_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC2_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC2_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC2_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC2_OE_INV_SEL_M  (LP_GPIO_FUNC2_OE_INV_SEL_V << LP_GPIO_FUNC2_OE_INV_SEL_S)
#define LP_GPIO_FUNC2_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC2_OE_INV_SEL_S  2

/** LP_GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2bc)
/** LP_GPIO_FUNC3_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC3_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC3_OUT_INV_SEL_M  (LP_GPIO_FUNC3_OUT_INV_SEL_V << LP_GPIO_FUNC3_OUT_INV_SEL_S)
#define LP_GPIO_FUNC3_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC3_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC3_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC3_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC3_OE_INV_SEL_M  (LP_GPIO_FUNC3_OE_INV_SEL_V << LP_GPIO_FUNC3_OE_INV_SEL_S)
#define LP_GPIO_FUNC3_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC3_OE_INV_SEL_S  2

/** LP_GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2c0)
/** LP_GPIO_FUNC4_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC4_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC4_OUT_INV_SEL_M  (LP_GPIO_FUNC4_OUT_INV_SEL_V << LP_GPIO_FUNC4_OUT_INV_SEL_S)
#define LP_GPIO_FUNC4_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC4_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC4_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC4_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC4_OE_INV_SEL_M  (LP_GPIO_FUNC4_OE_INV_SEL_V << LP_GPIO_FUNC4_OE_INV_SEL_S)
#define LP_GPIO_FUNC4_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC4_OE_INV_SEL_S  2

/** LP_GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2c4)
/** LP_GPIO_FUNC5_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC5_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC5_OUT_INV_SEL_M  (LP_GPIO_FUNC5_OUT_INV_SEL_V << LP_GPIO_FUNC5_OUT_INV_SEL_S)
#define LP_GPIO_FUNC5_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC5_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC5_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC5_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC5_OE_INV_SEL_M  (LP_GPIO_FUNC5_OE_INV_SEL_V << LP_GPIO_FUNC5_OE_INV_SEL_S)
#define LP_GPIO_FUNC5_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC5_OE_INV_SEL_S  2

/** LP_GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define LP_GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2c8)
/** LP_GPIO_FUNC6_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC6_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC6_OUT_INV_SEL_M  (LP_GPIO_FUNC6_OUT_INV_SEL_V << LP_GPIO_FUNC6_OUT_INV_SEL_S)
#define LP_GPIO_FUNC6_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC6_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC6_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define LP_GPIO_FUNC6_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC6_OE_INV_SEL_M  (LP_GPIO_FUNC6_OE_INV_SEL_V << LP_GPIO_FUNC6_OE_INV_SEL_S)
#define LP_GPIO_FUNC6_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC6_OE_INV_SEL_S  2

/** LP_GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define LP_GPIO_CLOCK_GATE_REG (DR_REG_LP_GPIO_BASE + 0x3f8)
/** LP_GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  set this bit to enable GPIO clock gate
 */
#define LP_GPIO_CLK_EN    (BIT(0))
#define LP_GPIO_CLK_EN_M  (LP_GPIO_CLK_EN_V << LP_GPIO_CLK_EN_S)
#define LP_GPIO_CLK_EN_V  0x00000001U
#define LP_GPIO_CLK_EN_S  0

/** LP_GPIO_DATE_REG register
 *  GPIO version register
 */
#define LP_GPIO_DATE_REG (DR_REG_LP_GPIO_BASE + 0x3fc)
/** LP_GPIO_DATE : R/W; bitpos: [27:0]; default: 37814832;
 *  version register
 */
#define LP_GPIO_DATE    0x0FFFFFFFU
#define LP_GPIO_DATE_M  (LP_GPIO_DATE_V << LP_GPIO_DATE_S)
#define LP_GPIO_DATE_V  0x0FFFFFFFU
#define LP_GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
