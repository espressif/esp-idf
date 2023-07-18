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

/** GPIO_BT_SELECT_REG register
 *  GPIO bit select register
 */
#define GPIO_BT_SELECT_REG (DR_REG_GPIO_BASE + 0x0)
/** GPIO_BT_SEL : R/W; bitpos: [31:0]; default: 0;
 *  GPIO bit select register
 */
#define GPIO_BT_SEL    0xFFFFFFFFU
#define GPIO_BT_SEL_M  (GPIO_BT_SEL_V << GPIO_BT_SEL_S)
#define GPIO_BT_SEL_V  0xFFFFFFFFU
#define GPIO_BT_SEL_S  0

/** GPIO_OUT_REG register
 *  GPIO output register for GPIO0-31
 */
#define GPIO_OUT_REG (DR_REG_GPIO_BASE + 0x4)
/** GPIO_OUT_DATA_ORIG : R/W/SC/WTC; bitpos: [31:0]; default: 0;
 *  GPIO output register for GPIO0-31
 */
#define GPIO_OUT_DATA_ORIG    0xFFFFFFFFU
#define GPIO_OUT_DATA_ORIG_M  (GPIO_OUT_DATA_ORIG_V << GPIO_OUT_DATA_ORIG_S)
#define GPIO_OUT_DATA_ORIG_V  0xFFFFFFFFU
#define GPIO_OUT_DATA_ORIG_S  0

/** GPIO_OUT_W1TS_REG register
 *  GPIO output set register for GPIO0-31
 */
#define GPIO_OUT_W1TS_REG (DR_REG_GPIO_BASE + 0x8)
/** GPIO_OUT_W1TS : WT; bitpos: [31:0]; default: 0;
 *  GPIO output set register for GPIO0-31
 */
#define GPIO_OUT_W1TS    0xFFFFFFFFU
#define GPIO_OUT_W1TS_M  (GPIO_OUT_W1TS_V << GPIO_OUT_W1TS_S)
#define GPIO_OUT_W1TS_V  0xFFFFFFFFU
#define GPIO_OUT_W1TS_S  0

/** GPIO_OUT_W1TC_REG register
 *  GPIO output clear register for GPIO0-31
 */
#define GPIO_OUT_W1TC_REG (DR_REG_GPIO_BASE + 0xc)
/** GPIO_OUT_W1TC : WT; bitpos: [31:0]; default: 0;
 *  GPIO output clear register for GPIO0-31
 */
#define GPIO_OUT_W1TC    0xFFFFFFFFU
#define GPIO_OUT_W1TC_M  (GPIO_OUT_W1TC_V << GPIO_OUT_W1TC_S)
#define GPIO_OUT_W1TC_V  0xFFFFFFFFU
#define GPIO_OUT_W1TC_S  0

/** GPIO_OUT1_REG register
 *  GPIO output register for GPIO32-56
 */
#define GPIO_OUT1_REG (DR_REG_GPIO_BASE + 0x10)
/** GPIO_OUT1_DATA_ORIG : R/W/SC/WTC; bitpos: [24:0]; default: 0;
 *  GPIO output register for GPIO32-56
 */
#define GPIO_OUT1_DATA_ORIG    0x01FFFFFFU
#define GPIO_OUT1_DATA_ORIG_M  (GPIO_OUT1_DATA_ORIG_V << GPIO_OUT1_DATA_ORIG_S)
#define GPIO_OUT1_DATA_ORIG_V  0x01FFFFFFU
#define GPIO_OUT1_DATA_ORIG_S  0

/** GPIO_OUT1_W1TS_REG register
 *  GPIO output set register for GPIO32-56
 */
#define GPIO_OUT1_W1TS_REG (DR_REG_GPIO_BASE + 0x14)
/** GPIO_OUT1_W1TS : WT; bitpos: [24:0]; default: 0;
 *  GPIO output set register for GPIO32-56
 */
#define GPIO_OUT1_W1TS    0x01FFFFFFU
#define GPIO_OUT1_W1TS_M  (GPIO_OUT1_W1TS_V << GPIO_OUT1_W1TS_S)
#define GPIO_OUT1_W1TS_V  0x01FFFFFFU
#define GPIO_OUT1_W1TS_S  0

/** GPIO_OUT1_W1TC_REG register
 *  GPIO output clear register for GPIO32-56
 */
#define GPIO_OUT1_W1TC_REG (DR_REG_GPIO_BASE + 0x18)
/** GPIO_OUT1_W1TC : WT; bitpos: [24:0]; default: 0;
 *  GPIO output clear register for GPIO32-56
 */
#define GPIO_OUT1_W1TC    0x01FFFFFFU
#define GPIO_OUT1_W1TC_M  (GPIO_OUT1_W1TC_V << GPIO_OUT1_W1TC_S)
#define GPIO_OUT1_W1TC_V  0x01FFFFFFU
#define GPIO_OUT1_W1TC_S  0

/** GPIO_ENABLE_REG register
 *  GPIO output enable register for GPIO0-31
 */
#define GPIO_ENABLE_REG (DR_REG_GPIO_BASE + 0x20)
/** GPIO_ENABLE_DATA : R/W/WTC; bitpos: [31:0]; default: 0;
 *  GPIO output enable register for GPIO0-31
 */
#define GPIO_ENABLE_DATA    0xFFFFFFFFU
#define GPIO_ENABLE_DATA_M  (GPIO_ENABLE_DATA_V << GPIO_ENABLE_DATA_S)
#define GPIO_ENABLE_DATA_V  0xFFFFFFFFU
#define GPIO_ENABLE_DATA_S  0

/** GPIO_ENABLE_W1TS_REG register
 *  GPIO output enable set register for GPIO0-31
 */
#define GPIO_ENABLE_W1TS_REG (DR_REG_GPIO_BASE + 0x24)
/** GPIO_ENABLE_W1TS : WT; bitpos: [31:0]; default: 0;
 *  GPIO output enable set register for GPIO0-31
 */
#define GPIO_ENABLE_W1TS    0xFFFFFFFFU
#define GPIO_ENABLE_W1TS_M  (GPIO_ENABLE_W1TS_V << GPIO_ENABLE_W1TS_S)
#define GPIO_ENABLE_W1TS_V  0xFFFFFFFFU
#define GPIO_ENABLE_W1TS_S  0

/** GPIO_ENABLE_W1TC_REG register
 *  GPIO output enable clear register for GPIO0-31
 */
#define GPIO_ENABLE_W1TC_REG (DR_REG_GPIO_BASE + 0x28)
/** GPIO_ENABLE_W1TC : WT; bitpos: [31:0]; default: 0;
 *  GPIO output enable clear register for GPIO0-31
 */
#define GPIO_ENABLE_W1TC    0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_M  (GPIO_ENABLE_W1TC_V << GPIO_ENABLE_W1TC_S)
#define GPIO_ENABLE_W1TC_V  0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_S  0

/** GPIO_ENABLE1_REG register
 *  GPIO output enable register for GPIO32-56
 */
#define GPIO_ENABLE1_REG (DR_REG_GPIO_BASE + 0x2c)
/** GPIO_ENABLE1_DATA : R/W/WTC; bitpos: [24:0]; default: 0;
 *  GPIO output enable register for GPIO32-56
 */
#define GPIO_ENABLE1_DATA    0x01FFFFFFU
#define GPIO_ENABLE1_DATA_M  (GPIO_ENABLE1_DATA_V << GPIO_ENABLE1_DATA_S)
#define GPIO_ENABLE1_DATA_V  0x01FFFFFFU
#define GPIO_ENABLE1_DATA_S  0

/** GPIO_ENABLE1_W1TS_REG register
 *  GPIO output enable set register for GPIO32-56
 */
#define GPIO_ENABLE1_W1TS_REG (DR_REG_GPIO_BASE + 0x30)
/** GPIO_ENABLE1_W1TS : WT; bitpos: [24:0]; default: 0;
 *  GPIO output enable set register for GPIO32-56
 */
#define GPIO_ENABLE1_W1TS    0x01FFFFFFU
#define GPIO_ENABLE1_W1TS_M  (GPIO_ENABLE1_W1TS_V << GPIO_ENABLE1_W1TS_S)
#define GPIO_ENABLE1_W1TS_V  0x01FFFFFFU
#define GPIO_ENABLE1_W1TS_S  0

/** GPIO_ENABLE1_W1TC_REG register
 *  GPIO output enable clear register for GPIO32-56
 */
#define GPIO_ENABLE1_W1TC_REG (DR_REG_GPIO_BASE + 0x34)
/** GPIO_ENABLE1_W1TC : WT; bitpos: [24:0]; default: 0;
 *  GPIO output enable clear register for GPIO32-56
 */
#define GPIO_ENABLE1_W1TC    0x01FFFFFFU
#define GPIO_ENABLE1_W1TC_M  (GPIO_ENABLE1_W1TC_V << GPIO_ENABLE1_W1TC_S)
#define GPIO_ENABLE1_W1TC_V  0x01FFFFFFU
#define GPIO_ENABLE1_W1TC_S  0

/** GPIO_STRAP_REG register
 *  pad strapping register
 */
#define GPIO_STRAP_REG (DR_REG_GPIO_BASE + 0x38)
/** GPIO_STRAPPING : RO; bitpos: [15:0]; default: 0;
 *  pad strapping register
 */
#define GPIO_STRAPPING    0x0000FFFFU
#define GPIO_STRAPPING_M  (GPIO_STRAPPING_V << GPIO_STRAPPING_S)
#define GPIO_STRAPPING_V  0x0000FFFFU
#define GPIO_STRAPPING_S  0

/** GPIO_IN_REG register
 *  GPIO input register for GPIO0-31
 */
#define GPIO_IN_REG (DR_REG_GPIO_BASE + 0x3c)
/** GPIO_IN_DATA_NEXT : RO; bitpos: [31:0]; default: 0;
 *  GPIO input register for GPIO0-31
 */
#define GPIO_IN_DATA_NEXT    0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_M  (GPIO_IN_DATA_NEXT_V << GPIO_IN_DATA_NEXT_S)
#define GPIO_IN_DATA_NEXT_V  0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_S  0

/** GPIO_IN1_REG register
 *  GPIO input register for GPIO32-56
 */
#define GPIO_IN1_REG (DR_REG_GPIO_BASE + 0x40)
/** GPIO_IN1_DATA_NEXT : RO; bitpos: [24:0]; default: 0;
 *  GPIO input register for GPIO32-56
 */
#define GPIO_IN1_DATA_NEXT    0x01FFFFFFU
#define GPIO_IN1_DATA_NEXT_M  (GPIO_IN1_DATA_NEXT_V << GPIO_IN1_DATA_NEXT_S)
#define GPIO_IN1_DATA_NEXT_V  0x01FFFFFFU
#define GPIO_IN1_DATA_NEXT_S  0

/** GPIO_STATUS_REG register
 *  GPIO interrupt status register for GPIO0-31
 */
#define GPIO_STATUS_REG (DR_REG_GPIO_BASE + 0x44)
/** GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [31:0]; default: 0;
 *  GPIO interrupt status register for GPIO0-31
 */
#define GPIO_STATUS_INTERRUPT    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_M  (GPIO_STATUS_INTERRUPT_V << GPIO_STATUS_INTERRUPT_S)
#define GPIO_STATUS_INTERRUPT_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_S  0

/** GPIO_STATUS_W1TS_REG register
 *  GPIO interrupt status set register for GPIO0-31
 */
#define GPIO_STATUS_W1TS_REG (DR_REG_GPIO_BASE + 0x48)
/** GPIO_STATUS_W1TS : WT; bitpos: [31:0]; default: 0;
 *  GPIO interrupt status set register for GPIO0-31
 */
#define GPIO_STATUS_W1TS    0xFFFFFFFFU
#define GPIO_STATUS_W1TS_M  (GPIO_STATUS_W1TS_V << GPIO_STATUS_W1TS_S)
#define GPIO_STATUS_W1TS_V  0xFFFFFFFFU
#define GPIO_STATUS_W1TS_S  0

/** GPIO_STATUS_W1TC_REG register
 *  GPIO interrupt status clear register for GPIO0-31
 */
#define GPIO_STATUS_W1TC_REG (DR_REG_GPIO_BASE + 0x4c)
/** GPIO_STATUS_W1TC : WT; bitpos: [31:0]; default: 0;
 *  GPIO interrupt status clear register for GPIO0-31
 */
#define GPIO_STATUS_W1TC    0xFFFFFFFFU
#define GPIO_STATUS_W1TC_M  (GPIO_STATUS_W1TC_V << GPIO_STATUS_W1TC_S)
#define GPIO_STATUS_W1TC_V  0xFFFFFFFFU
#define GPIO_STATUS_W1TC_S  0

/** GPIO_STATUS1_REG register
 *  GPIO interrupt status register for GPIO32-56
 */
#define GPIO_STATUS1_REG (DR_REG_GPIO_BASE + 0x50)
/** GPIO_STATUS1_INTERRUPT : R/W/WTC; bitpos: [24:0]; default: 0;
 *  GPIO interrupt status register for GPIO32-56
 */
#define GPIO_STATUS1_INTERRUPT    0x01FFFFFFU
#define GPIO_STATUS1_INTERRUPT_M  (GPIO_STATUS1_INTERRUPT_V << GPIO_STATUS1_INTERRUPT_S)
#define GPIO_STATUS1_INTERRUPT_V  0x01FFFFFFU
#define GPIO_STATUS1_INTERRUPT_S  0

/** GPIO_STATUS1_W1TS_REG register
 *  GPIO interrupt status set register for GPIO32-56
 */
#define GPIO_STATUS1_W1TS_REG (DR_REG_GPIO_BASE + 0x54)
/** GPIO_STATUS1_W1TS : WT; bitpos: [24:0]; default: 0;
 *  GPIO interrupt status set register for GPIO32-56
 */
#define GPIO_STATUS1_W1TS    0x01FFFFFFU
#define GPIO_STATUS1_W1TS_M  (GPIO_STATUS1_W1TS_V << GPIO_STATUS1_W1TS_S)
#define GPIO_STATUS1_W1TS_V  0x01FFFFFFU
#define GPIO_STATUS1_W1TS_S  0

/** GPIO_STATUS1_W1TC_REG register
 *  GPIO interrupt status clear register for GPIO32-56
 */
#define GPIO_STATUS1_W1TC_REG (DR_REG_GPIO_BASE + 0x58)
/** GPIO_STATUS1_W1TC : WT; bitpos: [24:0]; default: 0;
 *  GPIO interrupt status clear register for GPIO32-56
 */
#define GPIO_STATUS1_W1TC    0x01FFFFFFU
#define GPIO_STATUS1_W1TC_M  (GPIO_STATUS1_W1TC_V << GPIO_STATUS1_W1TC_S)
#define GPIO_STATUS1_W1TC_V  0x01FFFFFFU
#define GPIO_STATUS1_W1TC_S  0

/** GPIO_INTR_0_REG register
 *  GPIO interrupt 0 status register for GPIO0-31
 */
#define GPIO_INTR_0_REG (DR_REG_GPIO_BASE + 0x5c)
/** GPIO_INT_0 : RO; bitpos: [31:0]; default: 0;
 *  GPIO interrupt 0 status register for GPIO0-31
 */
#define GPIO_INT_0    0xFFFFFFFFU
#define GPIO_INT_0_M  (GPIO_INT_0_V << GPIO_INT_0_S)
#define GPIO_INT_0_V  0xFFFFFFFFU
#define GPIO_INT_0_S  0

/** GPIO_INTR1_0_REG register
 *  GPIO interrupt 0 status register for GPIO32-56
 */
#define GPIO_INTR1_0_REG (DR_REG_GPIO_BASE + 0x60)
/** GPIO_INT1_0 : RO; bitpos: [24:0]; default: 0;
 *  GPIO interrupt 0 status register for GPIO32-56
 */
#define GPIO_INT1_0    0x01FFFFFFU
#define GPIO_INT1_0_M  (GPIO_INT1_0_V << GPIO_INT1_0_S)
#define GPIO_INT1_0_V  0x01FFFFFFU
#define GPIO_INT1_0_S  0

/** GPIO_INTR_1_REG register
 *  GPIO interrupt 1 status register for GPIO0-31
 */
#define GPIO_INTR_1_REG (DR_REG_GPIO_BASE + 0x64)
/** GPIO_INT_1 : RO; bitpos: [31:0]; default: 0;
 *  GPIO interrupt 1 status register for GPIO0-31
 */
#define GPIO_INT_1    0xFFFFFFFFU
#define GPIO_INT_1_M  (GPIO_INT_1_V << GPIO_INT_1_S)
#define GPIO_INT_1_V  0xFFFFFFFFU
#define GPIO_INT_1_S  0

/** GPIO_INTR1_1_REG register
 *  GPIO interrupt 1 status register for GPIO32-56
 */
#define GPIO_INTR1_1_REG (DR_REG_GPIO_BASE + 0x68)
/** GPIO_INT1_1 : RO; bitpos: [24:0]; default: 0;
 *  GPIO interrupt 1 status register for GPIO32-56
 */
#define GPIO_INT1_1    0x01FFFFFFU
#define GPIO_INT1_1_M  (GPIO_INT1_1_V << GPIO_INT1_1_S)
#define GPIO_INT1_1_V  0x01FFFFFFU
#define GPIO_INT1_1_S  0

/** GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register for GPIO0-31
 */
#define GPIO_STATUS_NEXT_REG (DR_REG_GPIO_BASE + 0x6c)
/** GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [31:0]; default: 0;
 *  GPIO interrupt source register for GPIO0-31
 */
#define GPIO_STATUS_INTERRUPT_NEXT    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_M  (GPIO_STATUS_INTERRUPT_NEXT_V << GPIO_STATUS_INTERRUPT_NEXT_S)
#define GPIO_STATUS_INTERRUPT_NEXT_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

/** GPIO_STATUS_NEXT1_REG register
 *  GPIO interrupt source register for GPIO32-56
 */
#define GPIO_STATUS_NEXT1_REG (DR_REG_GPIO_BASE + 0x70)
/** GPIO_STATUS_INTERRUPT_NEXT1 : RO; bitpos: [24:0]; default: 0;
 *  GPIO interrupt source register for GPIO32-56
 */
#define GPIO_STATUS_INTERRUPT_NEXT1    0x01FFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT1_M  (GPIO_STATUS_INTERRUPT_NEXT1_V << GPIO_STATUS_INTERRUPT_NEXT1_S)
#define GPIO_STATUS_INTERRUPT_NEXT1_V  0x01FFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT1_S  0

/** GPIO_PIN0_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN0_REG (DR_REG_GPIO_BASE + 0x74)
/** GPIO_PIN0_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN0_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN0_SYNC2_BYPASS_M  (GPIO_PIN0_SYNC2_BYPASS_V << GPIO_PIN0_SYNC2_BYPASS_S)
#define GPIO_PIN0_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN0_SYNC2_BYPASS_S  0
/** GPIO_PIN0_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_M  (GPIO_PIN0_PAD_DRIVER_V << GPIO_PIN0_PAD_DRIVER_S)
#define GPIO_PIN0_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN0_PAD_DRIVER_S  2
/** GPIO_PIN0_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN0_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN0_SYNC1_BYPASS_M  (GPIO_PIN0_SYNC1_BYPASS_V << GPIO_PIN0_SYNC1_BYPASS_S)
#define GPIO_PIN0_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN0_SYNC1_BYPASS_S  3
/** GPIO_PIN0_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN0_INT_TYPE    0x00000007U
#define GPIO_PIN0_INT_TYPE_M  (GPIO_PIN0_INT_TYPE_V << GPIO_PIN0_INT_TYPE_S)
#define GPIO_PIN0_INT_TYPE_V  0x00000007U
#define GPIO_PIN0_INT_TYPE_S  7
/** GPIO_PIN0_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_M  (GPIO_PIN0_WAKEUP_ENABLE_V << GPIO_PIN0_WAKEUP_ENABLE_S)
#define GPIO_PIN0_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN0_WAKEUP_ENABLE_S  10
/** GPIO_PIN0_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN0_CONFIG    0x00000003U
#define GPIO_PIN0_CONFIG_M  (GPIO_PIN0_CONFIG_V << GPIO_PIN0_CONFIG_S)
#define GPIO_PIN0_CONFIG_V  0x00000003U
#define GPIO_PIN0_CONFIG_S  11
/** GPIO_PIN0_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN0_INT_ENA    0x0000001FU
#define GPIO_PIN0_INT_ENA_M  (GPIO_PIN0_INT_ENA_V << GPIO_PIN0_INT_ENA_S)
#define GPIO_PIN0_INT_ENA_V  0x0000001FU
#define GPIO_PIN0_INT_ENA_S  13

/** GPIO_PIN1_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN1_REG (DR_REG_GPIO_BASE + 0x78)
/** GPIO_PIN1_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN1_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN1_SYNC2_BYPASS_M  (GPIO_PIN1_SYNC2_BYPASS_V << GPIO_PIN1_SYNC2_BYPASS_S)
#define GPIO_PIN1_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN1_SYNC2_BYPASS_S  0
/** GPIO_PIN1_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_M  (GPIO_PIN1_PAD_DRIVER_V << GPIO_PIN1_PAD_DRIVER_S)
#define GPIO_PIN1_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN1_PAD_DRIVER_S  2
/** GPIO_PIN1_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN1_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN1_SYNC1_BYPASS_M  (GPIO_PIN1_SYNC1_BYPASS_V << GPIO_PIN1_SYNC1_BYPASS_S)
#define GPIO_PIN1_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN1_SYNC1_BYPASS_S  3
/** GPIO_PIN1_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN1_INT_TYPE    0x00000007U
#define GPIO_PIN1_INT_TYPE_M  (GPIO_PIN1_INT_TYPE_V << GPIO_PIN1_INT_TYPE_S)
#define GPIO_PIN1_INT_TYPE_V  0x00000007U
#define GPIO_PIN1_INT_TYPE_S  7
/** GPIO_PIN1_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_M  (GPIO_PIN1_WAKEUP_ENABLE_V << GPIO_PIN1_WAKEUP_ENABLE_S)
#define GPIO_PIN1_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN1_WAKEUP_ENABLE_S  10
/** GPIO_PIN1_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN1_CONFIG    0x00000003U
#define GPIO_PIN1_CONFIG_M  (GPIO_PIN1_CONFIG_V << GPIO_PIN1_CONFIG_S)
#define GPIO_PIN1_CONFIG_V  0x00000003U
#define GPIO_PIN1_CONFIG_S  11
/** GPIO_PIN1_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN1_INT_ENA    0x0000001FU
#define GPIO_PIN1_INT_ENA_M  (GPIO_PIN1_INT_ENA_V << GPIO_PIN1_INT_ENA_S)
#define GPIO_PIN1_INT_ENA_V  0x0000001FU
#define GPIO_PIN1_INT_ENA_S  13

/** GPIO_PIN2_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN2_REG (DR_REG_GPIO_BASE + 0x7c)
/** GPIO_PIN2_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN2_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN2_SYNC2_BYPASS_M  (GPIO_PIN2_SYNC2_BYPASS_V << GPIO_PIN2_SYNC2_BYPASS_S)
#define GPIO_PIN2_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN2_SYNC2_BYPASS_S  0
/** GPIO_PIN2_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_M  (GPIO_PIN2_PAD_DRIVER_V << GPIO_PIN2_PAD_DRIVER_S)
#define GPIO_PIN2_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN2_PAD_DRIVER_S  2
/** GPIO_PIN2_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN2_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN2_SYNC1_BYPASS_M  (GPIO_PIN2_SYNC1_BYPASS_V << GPIO_PIN2_SYNC1_BYPASS_S)
#define GPIO_PIN2_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN2_SYNC1_BYPASS_S  3
/** GPIO_PIN2_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN2_INT_TYPE    0x00000007U
#define GPIO_PIN2_INT_TYPE_M  (GPIO_PIN2_INT_TYPE_V << GPIO_PIN2_INT_TYPE_S)
#define GPIO_PIN2_INT_TYPE_V  0x00000007U
#define GPIO_PIN2_INT_TYPE_S  7
/** GPIO_PIN2_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_M  (GPIO_PIN2_WAKEUP_ENABLE_V << GPIO_PIN2_WAKEUP_ENABLE_S)
#define GPIO_PIN2_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN2_WAKEUP_ENABLE_S  10
/** GPIO_PIN2_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN2_CONFIG    0x00000003U
#define GPIO_PIN2_CONFIG_M  (GPIO_PIN2_CONFIG_V << GPIO_PIN2_CONFIG_S)
#define GPIO_PIN2_CONFIG_V  0x00000003U
#define GPIO_PIN2_CONFIG_S  11
/** GPIO_PIN2_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN2_INT_ENA    0x0000001FU
#define GPIO_PIN2_INT_ENA_M  (GPIO_PIN2_INT_ENA_V << GPIO_PIN2_INT_ENA_S)
#define GPIO_PIN2_INT_ENA_V  0x0000001FU
#define GPIO_PIN2_INT_ENA_S  13

/** GPIO_PIN3_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN3_REG (DR_REG_GPIO_BASE + 0x80)
/** GPIO_PIN3_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN3_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN3_SYNC2_BYPASS_M  (GPIO_PIN3_SYNC2_BYPASS_V << GPIO_PIN3_SYNC2_BYPASS_S)
#define GPIO_PIN3_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN3_SYNC2_BYPASS_S  0
/** GPIO_PIN3_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_M  (GPIO_PIN3_PAD_DRIVER_V << GPIO_PIN3_PAD_DRIVER_S)
#define GPIO_PIN3_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN3_PAD_DRIVER_S  2
/** GPIO_PIN3_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN3_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN3_SYNC1_BYPASS_M  (GPIO_PIN3_SYNC1_BYPASS_V << GPIO_PIN3_SYNC1_BYPASS_S)
#define GPIO_PIN3_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN3_SYNC1_BYPASS_S  3
/** GPIO_PIN3_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN3_INT_TYPE    0x00000007U
#define GPIO_PIN3_INT_TYPE_M  (GPIO_PIN3_INT_TYPE_V << GPIO_PIN3_INT_TYPE_S)
#define GPIO_PIN3_INT_TYPE_V  0x00000007U
#define GPIO_PIN3_INT_TYPE_S  7
/** GPIO_PIN3_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_M  (GPIO_PIN3_WAKEUP_ENABLE_V << GPIO_PIN3_WAKEUP_ENABLE_S)
#define GPIO_PIN3_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN3_WAKEUP_ENABLE_S  10
/** GPIO_PIN3_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN3_CONFIG    0x00000003U
#define GPIO_PIN3_CONFIG_M  (GPIO_PIN3_CONFIG_V << GPIO_PIN3_CONFIG_S)
#define GPIO_PIN3_CONFIG_V  0x00000003U
#define GPIO_PIN3_CONFIG_S  11
/** GPIO_PIN3_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN3_INT_ENA    0x0000001FU
#define GPIO_PIN3_INT_ENA_M  (GPIO_PIN3_INT_ENA_V << GPIO_PIN3_INT_ENA_S)
#define GPIO_PIN3_INT_ENA_V  0x0000001FU
#define GPIO_PIN3_INT_ENA_S  13

/** GPIO_PIN4_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN4_REG (DR_REG_GPIO_BASE + 0x84)
/** GPIO_PIN4_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN4_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN4_SYNC2_BYPASS_M  (GPIO_PIN4_SYNC2_BYPASS_V << GPIO_PIN4_SYNC2_BYPASS_S)
#define GPIO_PIN4_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN4_SYNC2_BYPASS_S  0
/** GPIO_PIN4_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_M  (GPIO_PIN4_PAD_DRIVER_V << GPIO_PIN4_PAD_DRIVER_S)
#define GPIO_PIN4_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN4_PAD_DRIVER_S  2
/** GPIO_PIN4_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN4_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN4_SYNC1_BYPASS_M  (GPIO_PIN4_SYNC1_BYPASS_V << GPIO_PIN4_SYNC1_BYPASS_S)
#define GPIO_PIN4_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN4_SYNC1_BYPASS_S  3
/** GPIO_PIN4_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN4_INT_TYPE    0x00000007U
#define GPIO_PIN4_INT_TYPE_M  (GPIO_PIN4_INT_TYPE_V << GPIO_PIN4_INT_TYPE_S)
#define GPIO_PIN4_INT_TYPE_V  0x00000007U
#define GPIO_PIN4_INT_TYPE_S  7
/** GPIO_PIN4_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_M  (GPIO_PIN4_WAKEUP_ENABLE_V << GPIO_PIN4_WAKEUP_ENABLE_S)
#define GPIO_PIN4_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN4_WAKEUP_ENABLE_S  10
/** GPIO_PIN4_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN4_CONFIG    0x00000003U
#define GPIO_PIN4_CONFIG_M  (GPIO_PIN4_CONFIG_V << GPIO_PIN4_CONFIG_S)
#define GPIO_PIN4_CONFIG_V  0x00000003U
#define GPIO_PIN4_CONFIG_S  11
/** GPIO_PIN4_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN4_INT_ENA    0x0000001FU
#define GPIO_PIN4_INT_ENA_M  (GPIO_PIN4_INT_ENA_V << GPIO_PIN4_INT_ENA_S)
#define GPIO_PIN4_INT_ENA_V  0x0000001FU
#define GPIO_PIN4_INT_ENA_S  13

/** GPIO_PIN5_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN5_REG (DR_REG_GPIO_BASE + 0x88)
/** GPIO_PIN5_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN5_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN5_SYNC2_BYPASS_M  (GPIO_PIN5_SYNC2_BYPASS_V << GPIO_PIN5_SYNC2_BYPASS_S)
#define GPIO_PIN5_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN5_SYNC2_BYPASS_S  0
/** GPIO_PIN5_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_M  (GPIO_PIN5_PAD_DRIVER_V << GPIO_PIN5_PAD_DRIVER_S)
#define GPIO_PIN5_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN5_PAD_DRIVER_S  2
/** GPIO_PIN5_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN5_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN5_SYNC1_BYPASS_M  (GPIO_PIN5_SYNC1_BYPASS_V << GPIO_PIN5_SYNC1_BYPASS_S)
#define GPIO_PIN5_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN5_SYNC1_BYPASS_S  3
/** GPIO_PIN5_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN5_INT_TYPE    0x00000007U
#define GPIO_PIN5_INT_TYPE_M  (GPIO_PIN5_INT_TYPE_V << GPIO_PIN5_INT_TYPE_S)
#define GPIO_PIN5_INT_TYPE_V  0x00000007U
#define GPIO_PIN5_INT_TYPE_S  7
/** GPIO_PIN5_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_M  (GPIO_PIN5_WAKEUP_ENABLE_V << GPIO_PIN5_WAKEUP_ENABLE_S)
#define GPIO_PIN5_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN5_WAKEUP_ENABLE_S  10
/** GPIO_PIN5_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN5_CONFIG    0x00000003U
#define GPIO_PIN5_CONFIG_M  (GPIO_PIN5_CONFIG_V << GPIO_PIN5_CONFIG_S)
#define GPIO_PIN5_CONFIG_V  0x00000003U
#define GPIO_PIN5_CONFIG_S  11
/** GPIO_PIN5_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN5_INT_ENA    0x0000001FU
#define GPIO_PIN5_INT_ENA_M  (GPIO_PIN5_INT_ENA_V << GPIO_PIN5_INT_ENA_S)
#define GPIO_PIN5_INT_ENA_V  0x0000001FU
#define GPIO_PIN5_INT_ENA_S  13

/** GPIO_PIN6_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN6_REG (DR_REG_GPIO_BASE + 0x8c)
/** GPIO_PIN6_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN6_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN6_SYNC2_BYPASS_M  (GPIO_PIN6_SYNC2_BYPASS_V << GPIO_PIN6_SYNC2_BYPASS_S)
#define GPIO_PIN6_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN6_SYNC2_BYPASS_S  0
/** GPIO_PIN6_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_M  (GPIO_PIN6_PAD_DRIVER_V << GPIO_PIN6_PAD_DRIVER_S)
#define GPIO_PIN6_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN6_PAD_DRIVER_S  2
/** GPIO_PIN6_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN6_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN6_SYNC1_BYPASS_M  (GPIO_PIN6_SYNC1_BYPASS_V << GPIO_PIN6_SYNC1_BYPASS_S)
#define GPIO_PIN6_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN6_SYNC1_BYPASS_S  3
/** GPIO_PIN6_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN6_INT_TYPE    0x00000007U
#define GPIO_PIN6_INT_TYPE_M  (GPIO_PIN6_INT_TYPE_V << GPIO_PIN6_INT_TYPE_S)
#define GPIO_PIN6_INT_TYPE_V  0x00000007U
#define GPIO_PIN6_INT_TYPE_S  7
/** GPIO_PIN6_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_M  (GPIO_PIN6_WAKEUP_ENABLE_V << GPIO_PIN6_WAKEUP_ENABLE_S)
#define GPIO_PIN6_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN6_WAKEUP_ENABLE_S  10
/** GPIO_PIN6_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN6_CONFIG    0x00000003U
#define GPIO_PIN6_CONFIG_M  (GPIO_PIN6_CONFIG_V << GPIO_PIN6_CONFIG_S)
#define GPIO_PIN6_CONFIG_V  0x00000003U
#define GPIO_PIN6_CONFIG_S  11
/** GPIO_PIN6_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN6_INT_ENA    0x0000001FU
#define GPIO_PIN6_INT_ENA_M  (GPIO_PIN6_INT_ENA_V << GPIO_PIN6_INT_ENA_S)
#define GPIO_PIN6_INT_ENA_V  0x0000001FU
#define GPIO_PIN6_INT_ENA_S  13

/** GPIO_PIN7_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN7_REG (DR_REG_GPIO_BASE + 0x90)
/** GPIO_PIN7_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN7_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN7_SYNC2_BYPASS_M  (GPIO_PIN7_SYNC2_BYPASS_V << GPIO_PIN7_SYNC2_BYPASS_S)
#define GPIO_PIN7_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN7_SYNC2_BYPASS_S  0
/** GPIO_PIN7_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN7_PAD_DRIVER    (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_M  (GPIO_PIN7_PAD_DRIVER_V << GPIO_PIN7_PAD_DRIVER_S)
#define GPIO_PIN7_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN7_PAD_DRIVER_S  2
/** GPIO_PIN7_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN7_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN7_SYNC1_BYPASS_M  (GPIO_PIN7_SYNC1_BYPASS_V << GPIO_PIN7_SYNC1_BYPASS_S)
#define GPIO_PIN7_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN7_SYNC1_BYPASS_S  3
/** GPIO_PIN7_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN7_INT_TYPE    0x00000007U
#define GPIO_PIN7_INT_TYPE_M  (GPIO_PIN7_INT_TYPE_V << GPIO_PIN7_INT_TYPE_S)
#define GPIO_PIN7_INT_TYPE_V  0x00000007U
#define GPIO_PIN7_INT_TYPE_S  7
/** GPIO_PIN7_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN7_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_M  (GPIO_PIN7_WAKEUP_ENABLE_V << GPIO_PIN7_WAKEUP_ENABLE_S)
#define GPIO_PIN7_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN7_WAKEUP_ENABLE_S  10
/** GPIO_PIN7_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN7_CONFIG    0x00000003U
#define GPIO_PIN7_CONFIG_M  (GPIO_PIN7_CONFIG_V << GPIO_PIN7_CONFIG_S)
#define GPIO_PIN7_CONFIG_V  0x00000003U
#define GPIO_PIN7_CONFIG_S  11
/** GPIO_PIN7_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN7_INT_ENA    0x0000001FU
#define GPIO_PIN7_INT_ENA_M  (GPIO_PIN7_INT_ENA_V << GPIO_PIN7_INT_ENA_S)
#define GPIO_PIN7_INT_ENA_V  0x0000001FU
#define GPIO_PIN7_INT_ENA_S  13

/** GPIO_PIN8_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN8_REG (DR_REG_GPIO_BASE + 0x94)
/** GPIO_PIN8_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN8_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN8_SYNC2_BYPASS_M  (GPIO_PIN8_SYNC2_BYPASS_V << GPIO_PIN8_SYNC2_BYPASS_S)
#define GPIO_PIN8_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN8_SYNC2_BYPASS_S  0
/** GPIO_PIN8_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN8_PAD_DRIVER    (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_M  (GPIO_PIN8_PAD_DRIVER_V << GPIO_PIN8_PAD_DRIVER_S)
#define GPIO_PIN8_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN8_PAD_DRIVER_S  2
/** GPIO_PIN8_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN8_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN8_SYNC1_BYPASS_M  (GPIO_PIN8_SYNC1_BYPASS_V << GPIO_PIN8_SYNC1_BYPASS_S)
#define GPIO_PIN8_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN8_SYNC1_BYPASS_S  3
/** GPIO_PIN8_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN8_INT_TYPE    0x00000007U
#define GPIO_PIN8_INT_TYPE_M  (GPIO_PIN8_INT_TYPE_V << GPIO_PIN8_INT_TYPE_S)
#define GPIO_PIN8_INT_TYPE_V  0x00000007U
#define GPIO_PIN8_INT_TYPE_S  7
/** GPIO_PIN8_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN8_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_M  (GPIO_PIN8_WAKEUP_ENABLE_V << GPIO_PIN8_WAKEUP_ENABLE_S)
#define GPIO_PIN8_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN8_WAKEUP_ENABLE_S  10
/** GPIO_PIN8_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN8_CONFIG    0x00000003U
#define GPIO_PIN8_CONFIG_M  (GPIO_PIN8_CONFIG_V << GPIO_PIN8_CONFIG_S)
#define GPIO_PIN8_CONFIG_V  0x00000003U
#define GPIO_PIN8_CONFIG_S  11
/** GPIO_PIN8_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN8_INT_ENA    0x0000001FU
#define GPIO_PIN8_INT_ENA_M  (GPIO_PIN8_INT_ENA_V << GPIO_PIN8_INT_ENA_S)
#define GPIO_PIN8_INT_ENA_V  0x0000001FU
#define GPIO_PIN8_INT_ENA_S  13

/** GPIO_PIN9_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN9_REG (DR_REG_GPIO_BASE + 0x98)
/** GPIO_PIN9_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN9_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN9_SYNC2_BYPASS_M  (GPIO_PIN9_SYNC2_BYPASS_V << GPIO_PIN9_SYNC2_BYPASS_S)
#define GPIO_PIN9_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN9_SYNC2_BYPASS_S  0
/** GPIO_PIN9_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN9_PAD_DRIVER    (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_M  (GPIO_PIN9_PAD_DRIVER_V << GPIO_PIN9_PAD_DRIVER_S)
#define GPIO_PIN9_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN9_PAD_DRIVER_S  2
/** GPIO_PIN9_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN9_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN9_SYNC1_BYPASS_M  (GPIO_PIN9_SYNC1_BYPASS_V << GPIO_PIN9_SYNC1_BYPASS_S)
#define GPIO_PIN9_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN9_SYNC1_BYPASS_S  3
/** GPIO_PIN9_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN9_INT_TYPE    0x00000007U
#define GPIO_PIN9_INT_TYPE_M  (GPIO_PIN9_INT_TYPE_V << GPIO_PIN9_INT_TYPE_S)
#define GPIO_PIN9_INT_TYPE_V  0x00000007U
#define GPIO_PIN9_INT_TYPE_S  7
/** GPIO_PIN9_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN9_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_M  (GPIO_PIN9_WAKEUP_ENABLE_V << GPIO_PIN9_WAKEUP_ENABLE_S)
#define GPIO_PIN9_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN9_WAKEUP_ENABLE_S  10
/** GPIO_PIN9_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN9_CONFIG    0x00000003U
#define GPIO_PIN9_CONFIG_M  (GPIO_PIN9_CONFIG_V << GPIO_PIN9_CONFIG_S)
#define GPIO_PIN9_CONFIG_V  0x00000003U
#define GPIO_PIN9_CONFIG_S  11
/** GPIO_PIN9_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN9_INT_ENA    0x0000001FU
#define GPIO_PIN9_INT_ENA_M  (GPIO_PIN9_INT_ENA_V << GPIO_PIN9_INT_ENA_S)
#define GPIO_PIN9_INT_ENA_V  0x0000001FU
#define GPIO_PIN9_INT_ENA_S  13

/** GPIO_PIN10_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN10_REG (DR_REG_GPIO_BASE + 0x9c)
/** GPIO_PIN10_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN10_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN10_SYNC2_BYPASS_M  (GPIO_PIN10_SYNC2_BYPASS_V << GPIO_PIN10_SYNC2_BYPASS_S)
#define GPIO_PIN10_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN10_SYNC2_BYPASS_S  0
/** GPIO_PIN10_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN10_PAD_DRIVER    (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_M  (GPIO_PIN10_PAD_DRIVER_V << GPIO_PIN10_PAD_DRIVER_S)
#define GPIO_PIN10_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN10_PAD_DRIVER_S  2
/** GPIO_PIN10_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN10_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN10_SYNC1_BYPASS_M  (GPIO_PIN10_SYNC1_BYPASS_V << GPIO_PIN10_SYNC1_BYPASS_S)
#define GPIO_PIN10_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN10_SYNC1_BYPASS_S  3
/** GPIO_PIN10_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN10_INT_TYPE    0x00000007U
#define GPIO_PIN10_INT_TYPE_M  (GPIO_PIN10_INT_TYPE_V << GPIO_PIN10_INT_TYPE_S)
#define GPIO_PIN10_INT_TYPE_V  0x00000007U
#define GPIO_PIN10_INT_TYPE_S  7
/** GPIO_PIN10_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN10_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_M  (GPIO_PIN10_WAKEUP_ENABLE_V << GPIO_PIN10_WAKEUP_ENABLE_S)
#define GPIO_PIN10_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN10_WAKEUP_ENABLE_S  10
/** GPIO_PIN10_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN10_CONFIG    0x00000003U
#define GPIO_PIN10_CONFIG_M  (GPIO_PIN10_CONFIG_V << GPIO_PIN10_CONFIG_S)
#define GPIO_PIN10_CONFIG_V  0x00000003U
#define GPIO_PIN10_CONFIG_S  11
/** GPIO_PIN10_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN10_INT_ENA    0x0000001FU
#define GPIO_PIN10_INT_ENA_M  (GPIO_PIN10_INT_ENA_V << GPIO_PIN10_INT_ENA_S)
#define GPIO_PIN10_INT_ENA_V  0x0000001FU
#define GPIO_PIN10_INT_ENA_S  13

/** GPIO_PIN11_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN11_REG (DR_REG_GPIO_BASE + 0xa0)
/** GPIO_PIN11_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN11_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN11_SYNC2_BYPASS_M  (GPIO_PIN11_SYNC2_BYPASS_V << GPIO_PIN11_SYNC2_BYPASS_S)
#define GPIO_PIN11_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN11_SYNC2_BYPASS_S  0
/** GPIO_PIN11_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN11_PAD_DRIVER    (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_M  (GPIO_PIN11_PAD_DRIVER_V << GPIO_PIN11_PAD_DRIVER_S)
#define GPIO_PIN11_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN11_PAD_DRIVER_S  2
/** GPIO_PIN11_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN11_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN11_SYNC1_BYPASS_M  (GPIO_PIN11_SYNC1_BYPASS_V << GPIO_PIN11_SYNC1_BYPASS_S)
#define GPIO_PIN11_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN11_SYNC1_BYPASS_S  3
/** GPIO_PIN11_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN11_INT_TYPE    0x00000007U
#define GPIO_PIN11_INT_TYPE_M  (GPIO_PIN11_INT_TYPE_V << GPIO_PIN11_INT_TYPE_S)
#define GPIO_PIN11_INT_TYPE_V  0x00000007U
#define GPIO_PIN11_INT_TYPE_S  7
/** GPIO_PIN11_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN11_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_M  (GPIO_PIN11_WAKEUP_ENABLE_V << GPIO_PIN11_WAKEUP_ENABLE_S)
#define GPIO_PIN11_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN11_WAKEUP_ENABLE_S  10
/** GPIO_PIN11_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN11_CONFIG    0x00000003U
#define GPIO_PIN11_CONFIG_M  (GPIO_PIN11_CONFIG_V << GPIO_PIN11_CONFIG_S)
#define GPIO_PIN11_CONFIG_V  0x00000003U
#define GPIO_PIN11_CONFIG_S  11
/** GPIO_PIN11_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN11_INT_ENA    0x0000001FU
#define GPIO_PIN11_INT_ENA_M  (GPIO_PIN11_INT_ENA_V << GPIO_PIN11_INT_ENA_S)
#define GPIO_PIN11_INT_ENA_V  0x0000001FU
#define GPIO_PIN11_INT_ENA_S  13

/** GPIO_PIN12_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN12_REG (DR_REG_GPIO_BASE + 0xa4)
/** GPIO_PIN12_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN12_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN12_SYNC2_BYPASS_M  (GPIO_PIN12_SYNC2_BYPASS_V << GPIO_PIN12_SYNC2_BYPASS_S)
#define GPIO_PIN12_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN12_SYNC2_BYPASS_S  0
/** GPIO_PIN12_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN12_PAD_DRIVER    (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_M  (GPIO_PIN12_PAD_DRIVER_V << GPIO_PIN12_PAD_DRIVER_S)
#define GPIO_PIN12_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN12_PAD_DRIVER_S  2
/** GPIO_PIN12_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN12_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN12_SYNC1_BYPASS_M  (GPIO_PIN12_SYNC1_BYPASS_V << GPIO_PIN12_SYNC1_BYPASS_S)
#define GPIO_PIN12_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN12_SYNC1_BYPASS_S  3
/** GPIO_PIN12_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN12_INT_TYPE    0x00000007U
#define GPIO_PIN12_INT_TYPE_M  (GPIO_PIN12_INT_TYPE_V << GPIO_PIN12_INT_TYPE_S)
#define GPIO_PIN12_INT_TYPE_V  0x00000007U
#define GPIO_PIN12_INT_TYPE_S  7
/** GPIO_PIN12_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN12_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_M  (GPIO_PIN12_WAKEUP_ENABLE_V << GPIO_PIN12_WAKEUP_ENABLE_S)
#define GPIO_PIN12_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN12_WAKEUP_ENABLE_S  10
/** GPIO_PIN12_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN12_CONFIG    0x00000003U
#define GPIO_PIN12_CONFIG_M  (GPIO_PIN12_CONFIG_V << GPIO_PIN12_CONFIG_S)
#define GPIO_PIN12_CONFIG_V  0x00000003U
#define GPIO_PIN12_CONFIG_S  11
/** GPIO_PIN12_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN12_INT_ENA    0x0000001FU
#define GPIO_PIN12_INT_ENA_M  (GPIO_PIN12_INT_ENA_V << GPIO_PIN12_INT_ENA_S)
#define GPIO_PIN12_INT_ENA_V  0x0000001FU
#define GPIO_PIN12_INT_ENA_S  13

/** GPIO_PIN13_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN13_REG (DR_REG_GPIO_BASE + 0xa8)
/** GPIO_PIN13_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN13_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN13_SYNC2_BYPASS_M  (GPIO_PIN13_SYNC2_BYPASS_V << GPIO_PIN13_SYNC2_BYPASS_S)
#define GPIO_PIN13_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN13_SYNC2_BYPASS_S  0
/** GPIO_PIN13_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN13_PAD_DRIVER    (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_M  (GPIO_PIN13_PAD_DRIVER_V << GPIO_PIN13_PAD_DRIVER_S)
#define GPIO_PIN13_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN13_PAD_DRIVER_S  2
/** GPIO_PIN13_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN13_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN13_SYNC1_BYPASS_M  (GPIO_PIN13_SYNC1_BYPASS_V << GPIO_PIN13_SYNC1_BYPASS_S)
#define GPIO_PIN13_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN13_SYNC1_BYPASS_S  3
/** GPIO_PIN13_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN13_INT_TYPE    0x00000007U
#define GPIO_PIN13_INT_TYPE_M  (GPIO_PIN13_INT_TYPE_V << GPIO_PIN13_INT_TYPE_S)
#define GPIO_PIN13_INT_TYPE_V  0x00000007U
#define GPIO_PIN13_INT_TYPE_S  7
/** GPIO_PIN13_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN13_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_M  (GPIO_PIN13_WAKEUP_ENABLE_V << GPIO_PIN13_WAKEUP_ENABLE_S)
#define GPIO_PIN13_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN13_WAKEUP_ENABLE_S  10
/** GPIO_PIN13_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN13_CONFIG    0x00000003U
#define GPIO_PIN13_CONFIG_M  (GPIO_PIN13_CONFIG_V << GPIO_PIN13_CONFIG_S)
#define GPIO_PIN13_CONFIG_V  0x00000003U
#define GPIO_PIN13_CONFIG_S  11
/** GPIO_PIN13_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN13_INT_ENA    0x0000001FU
#define GPIO_PIN13_INT_ENA_M  (GPIO_PIN13_INT_ENA_V << GPIO_PIN13_INT_ENA_S)
#define GPIO_PIN13_INT_ENA_V  0x0000001FU
#define GPIO_PIN13_INT_ENA_S  13

/** GPIO_PIN14_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN14_REG (DR_REG_GPIO_BASE + 0xac)
/** GPIO_PIN14_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN14_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN14_SYNC2_BYPASS_M  (GPIO_PIN14_SYNC2_BYPASS_V << GPIO_PIN14_SYNC2_BYPASS_S)
#define GPIO_PIN14_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN14_SYNC2_BYPASS_S  0
/** GPIO_PIN14_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN14_PAD_DRIVER    (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_M  (GPIO_PIN14_PAD_DRIVER_V << GPIO_PIN14_PAD_DRIVER_S)
#define GPIO_PIN14_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN14_PAD_DRIVER_S  2
/** GPIO_PIN14_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN14_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN14_SYNC1_BYPASS_M  (GPIO_PIN14_SYNC1_BYPASS_V << GPIO_PIN14_SYNC1_BYPASS_S)
#define GPIO_PIN14_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN14_SYNC1_BYPASS_S  3
/** GPIO_PIN14_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN14_INT_TYPE    0x00000007U
#define GPIO_PIN14_INT_TYPE_M  (GPIO_PIN14_INT_TYPE_V << GPIO_PIN14_INT_TYPE_S)
#define GPIO_PIN14_INT_TYPE_V  0x00000007U
#define GPIO_PIN14_INT_TYPE_S  7
/** GPIO_PIN14_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN14_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_M  (GPIO_PIN14_WAKEUP_ENABLE_V << GPIO_PIN14_WAKEUP_ENABLE_S)
#define GPIO_PIN14_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN14_WAKEUP_ENABLE_S  10
/** GPIO_PIN14_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN14_CONFIG    0x00000003U
#define GPIO_PIN14_CONFIG_M  (GPIO_PIN14_CONFIG_V << GPIO_PIN14_CONFIG_S)
#define GPIO_PIN14_CONFIG_V  0x00000003U
#define GPIO_PIN14_CONFIG_S  11
/** GPIO_PIN14_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN14_INT_ENA    0x0000001FU
#define GPIO_PIN14_INT_ENA_M  (GPIO_PIN14_INT_ENA_V << GPIO_PIN14_INT_ENA_S)
#define GPIO_PIN14_INT_ENA_V  0x0000001FU
#define GPIO_PIN14_INT_ENA_S  13

/** GPIO_PIN15_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN15_REG (DR_REG_GPIO_BASE + 0xb0)
/** GPIO_PIN15_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN15_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN15_SYNC2_BYPASS_M  (GPIO_PIN15_SYNC2_BYPASS_V << GPIO_PIN15_SYNC2_BYPASS_S)
#define GPIO_PIN15_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN15_SYNC2_BYPASS_S  0
/** GPIO_PIN15_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN15_PAD_DRIVER    (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_M  (GPIO_PIN15_PAD_DRIVER_V << GPIO_PIN15_PAD_DRIVER_S)
#define GPIO_PIN15_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN15_PAD_DRIVER_S  2
/** GPIO_PIN15_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN15_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN15_SYNC1_BYPASS_M  (GPIO_PIN15_SYNC1_BYPASS_V << GPIO_PIN15_SYNC1_BYPASS_S)
#define GPIO_PIN15_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN15_SYNC1_BYPASS_S  3
/** GPIO_PIN15_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN15_INT_TYPE    0x00000007U
#define GPIO_PIN15_INT_TYPE_M  (GPIO_PIN15_INT_TYPE_V << GPIO_PIN15_INT_TYPE_S)
#define GPIO_PIN15_INT_TYPE_V  0x00000007U
#define GPIO_PIN15_INT_TYPE_S  7
/** GPIO_PIN15_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN15_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_M  (GPIO_PIN15_WAKEUP_ENABLE_V << GPIO_PIN15_WAKEUP_ENABLE_S)
#define GPIO_PIN15_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN15_WAKEUP_ENABLE_S  10
/** GPIO_PIN15_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN15_CONFIG    0x00000003U
#define GPIO_PIN15_CONFIG_M  (GPIO_PIN15_CONFIG_V << GPIO_PIN15_CONFIG_S)
#define GPIO_PIN15_CONFIG_V  0x00000003U
#define GPIO_PIN15_CONFIG_S  11
/** GPIO_PIN15_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN15_INT_ENA    0x0000001FU
#define GPIO_PIN15_INT_ENA_M  (GPIO_PIN15_INT_ENA_V << GPIO_PIN15_INT_ENA_S)
#define GPIO_PIN15_INT_ENA_V  0x0000001FU
#define GPIO_PIN15_INT_ENA_S  13

/** GPIO_PIN16_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN16_REG (DR_REG_GPIO_BASE + 0xb4)
/** GPIO_PIN16_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN16_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN16_SYNC2_BYPASS_M  (GPIO_PIN16_SYNC2_BYPASS_V << GPIO_PIN16_SYNC2_BYPASS_S)
#define GPIO_PIN16_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN16_SYNC2_BYPASS_S  0
/** GPIO_PIN16_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN16_PAD_DRIVER    (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_M  (GPIO_PIN16_PAD_DRIVER_V << GPIO_PIN16_PAD_DRIVER_S)
#define GPIO_PIN16_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN16_PAD_DRIVER_S  2
/** GPIO_PIN16_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN16_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN16_SYNC1_BYPASS_M  (GPIO_PIN16_SYNC1_BYPASS_V << GPIO_PIN16_SYNC1_BYPASS_S)
#define GPIO_PIN16_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN16_SYNC1_BYPASS_S  3
/** GPIO_PIN16_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN16_INT_TYPE    0x00000007U
#define GPIO_PIN16_INT_TYPE_M  (GPIO_PIN16_INT_TYPE_V << GPIO_PIN16_INT_TYPE_S)
#define GPIO_PIN16_INT_TYPE_V  0x00000007U
#define GPIO_PIN16_INT_TYPE_S  7
/** GPIO_PIN16_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN16_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_M  (GPIO_PIN16_WAKEUP_ENABLE_V << GPIO_PIN16_WAKEUP_ENABLE_S)
#define GPIO_PIN16_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN16_WAKEUP_ENABLE_S  10
/** GPIO_PIN16_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN16_CONFIG    0x00000003U
#define GPIO_PIN16_CONFIG_M  (GPIO_PIN16_CONFIG_V << GPIO_PIN16_CONFIG_S)
#define GPIO_PIN16_CONFIG_V  0x00000003U
#define GPIO_PIN16_CONFIG_S  11
/** GPIO_PIN16_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN16_INT_ENA    0x0000001FU
#define GPIO_PIN16_INT_ENA_M  (GPIO_PIN16_INT_ENA_V << GPIO_PIN16_INT_ENA_S)
#define GPIO_PIN16_INT_ENA_V  0x0000001FU
#define GPIO_PIN16_INT_ENA_S  13

/** GPIO_PIN17_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN17_REG (DR_REG_GPIO_BASE + 0xb8)
/** GPIO_PIN17_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN17_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN17_SYNC2_BYPASS_M  (GPIO_PIN17_SYNC2_BYPASS_V << GPIO_PIN17_SYNC2_BYPASS_S)
#define GPIO_PIN17_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN17_SYNC2_BYPASS_S  0
/** GPIO_PIN17_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN17_PAD_DRIVER    (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_M  (GPIO_PIN17_PAD_DRIVER_V << GPIO_PIN17_PAD_DRIVER_S)
#define GPIO_PIN17_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN17_PAD_DRIVER_S  2
/** GPIO_PIN17_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN17_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN17_SYNC1_BYPASS_M  (GPIO_PIN17_SYNC1_BYPASS_V << GPIO_PIN17_SYNC1_BYPASS_S)
#define GPIO_PIN17_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN17_SYNC1_BYPASS_S  3
/** GPIO_PIN17_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN17_INT_TYPE    0x00000007U
#define GPIO_PIN17_INT_TYPE_M  (GPIO_PIN17_INT_TYPE_V << GPIO_PIN17_INT_TYPE_S)
#define GPIO_PIN17_INT_TYPE_V  0x00000007U
#define GPIO_PIN17_INT_TYPE_S  7
/** GPIO_PIN17_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN17_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_M  (GPIO_PIN17_WAKEUP_ENABLE_V << GPIO_PIN17_WAKEUP_ENABLE_S)
#define GPIO_PIN17_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN17_WAKEUP_ENABLE_S  10
/** GPIO_PIN17_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN17_CONFIG    0x00000003U
#define GPIO_PIN17_CONFIG_M  (GPIO_PIN17_CONFIG_V << GPIO_PIN17_CONFIG_S)
#define GPIO_PIN17_CONFIG_V  0x00000003U
#define GPIO_PIN17_CONFIG_S  11
/** GPIO_PIN17_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN17_INT_ENA    0x0000001FU
#define GPIO_PIN17_INT_ENA_M  (GPIO_PIN17_INT_ENA_V << GPIO_PIN17_INT_ENA_S)
#define GPIO_PIN17_INT_ENA_V  0x0000001FU
#define GPIO_PIN17_INT_ENA_S  13

/** GPIO_PIN18_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN18_REG (DR_REG_GPIO_BASE + 0xbc)
/** GPIO_PIN18_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN18_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN18_SYNC2_BYPASS_M  (GPIO_PIN18_SYNC2_BYPASS_V << GPIO_PIN18_SYNC2_BYPASS_S)
#define GPIO_PIN18_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN18_SYNC2_BYPASS_S  0
/** GPIO_PIN18_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN18_PAD_DRIVER    (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_M  (GPIO_PIN18_PAD_DRIVER_V << GPIO_PIN18_PAD_DRIVER_S)
#define GPIO_PIN18_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN18_PAD_DRIVER_S  2
/** GPIO_PIN18_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN18_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN18_SYNC1_BYPASS_M  (GPIO_PIN18_SYNC1_BYPASS_V << GPIO_PIN18_SYNC1_BYPASS_S)
#define GPIO_PIN18_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN18_SYNC1_BYPASS_S  3
/** GPIO_PIN18_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN18_INT_TYPE    0x00000007U
#define GPIO_PIN18_INT_TYPE_M  (GPIO_PIN18_INT_TYPE_V << GPIO_PIN18_INT_TYPE_S)
#define GPIO_PIN18_INT_TYPE_V  0x00000007U
#define GPIO_PIN18_INT_TYPE_S  7
/** GPIO_PIN18_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN18_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_M  (GPIO_PIN18_WAKEUP_ENABLE_V << GPIO_PIN18_WAKEUP_ENABLE_S)
#define GPIO_PIN18_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN18_WAKEUP_ENABLE_S  10
/** GPIO_PIN18_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN18_CONFIG    0x00000003U
#define GPIO_PIN18_CONFIG_M  (GPIO_PIN18_CONFIG_V << GPIO_PIN18_CONFIG_S)
#define GPIO_PIN18_CONFIG_V  0x00000003U
#define GPIO_PIN18_CONFIG_S  11
/** GPIO_PIN18_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN18_INT_ENA    0x0000001FU
#define GPIO_PIN18_INT_ENA_M  (GPIO_PIN18_INT_ENA_V << GPIO_PIN18_INT_ENA_S)
#define GPIO_PIN18_INT_ENA_V  0x0000001FU
#define GPIO_PIN18_INT_ENA_S  13

/** GPIO_PIN19_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN19_REG (DR_REG_GPIO_BASE + 0xc0)
/** GPIO_PIN19_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN19_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN19_SYNC2_BYPASS_M  (GPIO_PIN19_SYNC2_BYPASS_V << GPIO_PIN19_SYNC2_BYPASS_S)
#define GPIO_PIN19_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN19_SYNC2_BYPASS_S  0
/** GPIO_PIN19_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN19_PAD_DRIVER    (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_M  (GPIO_PIN19_PAD_DRIVER_V << GPIO_PIN19_PAD_DRIVER_S)
#define GPIO_PIN19_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN19_PAD_DRIVER_S  2
/** GPIO_PIN19_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN19_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN19_SYNC1_BYPASS_M  (GPIO_PIN19_SYNC1_BYPASS_V << GPIO_PIN19_SYNC1_BYPASS_S)
#define GPIO_PIN19_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN19_SYNC1_BYPASS_S  3
/** GPIO_PIN19_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN19_INT_TYPE    0x00000007U
#define GPIO_PIN19_INT_TYPE_M  (GPIO_PIN19_INT_TYPE_V << GPIO_PIN19_INT_TYPE_S)
#define GPIO_PIN19_INT_TYPE_V  0x00000007U
#define GPIO_PIN19_INT_TYPE_S  7
/** GPIO_PIN19_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN19_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_M  (GPIO_PIN19_WAKEUP_ENABLE_V << GPIO_PIN19_WAKEUP_ENABLE_S)
#define GPIO_PIN19_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN19_WAKEUP_ENABLE_S  10
/** GPIO_PIN19_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN19_CONFIG    0x00000003U
#define GPIO_PIN19_CONFIG_M  (GPIO_PIN19_CONFIG_V << GPIO_PIN19_CONFIG_S)
#define GPIO_PIN19_CONFIG_V  0x00000003U
#define GPIO_PIN19_CONFIG_S  11
/** GPIO_PIN19_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN19_INT_ENA    0x0000001FU
#define GPIO_PIN19_INT_ENA_M  (GPIO_PIN19_INT_ENA_V << GPIO_PIN19_INT_ENA_S)
#define GPIO_PIN19_INT_ENA_V  0x0000001FU
#define GPIO_PIN19_INT_ENA_S  13

/** GPIO_PIN20_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN20_REG (DR_REG_GPIO_BASE + 0xc4)
/** GPIO_PIN20_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN20_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN20_SYNC2_BYPASS_M  (GPIO_PIN20_SYNC2_BYPASS_V << GPIO_PIN20_SYNC2_BYPASS_S)
#define GPIO_PIN20_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN20_SYNC2_BYPASS_S  0
/** GPIO_PIN20_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN20_PAD_DRIVER    (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_M  (GPIO_PIN20_PAD_DRIVER_V << GPIO_PIN20_PAD_DRIVER_S)
#define GPIO_PIN20_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN20_PAD_DRIVER_S  2
/** GPIO_PIN20_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN20_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN20_SYNC1_BYPASS_M  (GPIO_PIN20_SYNC1_BYPASS_V << GPIO_PIN20_SYNC1_BYPASS_S)
#define GPIO_PIN20_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN20_SYNC1_BYPASS_S  3
/** GPIO_PIN20_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN20_INT_TYPE    0x00000007U
#define GPIO_PIN20_INT_TYPE_M  (GPIO_PIN20_INT_TYPE_V << GPIO_PIN20_INT_TYPE_S)
#define GPIO_PIN20_INT_TYPE_V  0x00000007U
#define GPIO_PIN20_INT_TYPE_S  7
/** GPIO_PIN20_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN20_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_M  (GPIO_PIN20_WAKEUP_ENABLE_V << GPIO_PIN20_WAKEUP_ENABLE_S)
#define GPIO_PIN20_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN20_WAKEUP_ENABLE_S  10
/** GPIO_PIN20_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN20_CONFIG    0x00000003U
#define GPIO_PIN20_CONFIG_M  (GPIO_PIN20_CONFIG_V << GPIO_PIN20_CONFIG_S)
#define GPIO_PIN20_CONFIG_V  0x00000003U
#define GPIO_PIN20_CONFIG_S  11
/** GPIO_PIN20_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN20_INT_ENA    0x0000001FU
#define GPIO_PIN20_INT_ENA_M  (GPIO_PIN20_INT_ENA_V << GPIO_PIN20_INT_ENA_S)
#define GPIO_PIN20_INT_ENA_V  0x0000001FU
#define GPIO_PIN20_INT_ENA_S  13

/** GPIO_PIN21_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN21_REG (DR_REG_GPIO_BASE + 0xc8)
/** GPIO_PIN21_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN21_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN21_SYNC2_BYPASS_M  (GPIO_PIN21_SYNC2_BYPASS_V << GPIO_PIN21_SYNC2_BYPASS_S)
#define GPIO_PIN21_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN21_SYNC2_BYPASS_S  0
/** GPIO_PIN21_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN21_PAD_DRIVER    (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_M  (GPIO_PIN21_PAD_DRIVER_V << GPIO_PIN21_PAD_DRIVER_S)
#define GPIO_PIN21_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN21_PAD_DRIVER_S  2
/** GPIO_PIN21_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN21_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN21_SYNC1_BYPASS_M  (GPIO_PIN21_SYNC1_BYPASS_V << GPIO_PIN21_SYNC1_BYPASS_S)
#define GPIO_PIN21_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN21_SYNC1_BYPASS_S  3
/** GPIO_PIN21_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN21_INT_TYPE    0x00000007U
#define GPIO_PIN21_INT_TYPE_M  (GPIO_PIN21_INT_TYPE_V << GPIO_PIN21_INT_TYPE_S)
#define GPIO_PIN21_INT_TYPE_V  0x00000007U
#define GPIO_PIN21_INT_TYPE_S  7
/** GPIO_PIN21_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN21_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_M  (GPIO_PIN21_WAKEUP_ENABLE_V << GPIO_PIN21_WAKEUP_ENABLE_S)
#define GPIO_PIN21_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN21_WAKEUP_ENABLE_S  10
/** GPIO_PIN21_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN21_CONFIG    0x00000003U
#define GPIO_PIN21_CONFIG_M  (GPIO_PIN21_CONFIG_V << GPIO_PIN21_CONFIG_S)
#define GPIO_PIN21_CONFIG_V  0x00000003U
#define GPIO_PIN21_CONFIG_S  11
/** GPIO_PIN21_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN21_INT_ENA    0x0000001FU
#define GPIO_PIN21_INT_ENA_M  (GPIO_PIN21_INT_ENA_V << GPIO_PIN21_INT_ENA_S)
#define GPIO_PIN21_INT_ENA_V  0x0000001FU
#define GPIO_PIN21_INT_ENA_S  13

/** GPIO_PIN22_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN22_REG (DR_REG_GPIO_BASE + 0xcc)
/** GPIO_PIN22_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN22_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN22_SYNC2_BYPASS_M  (GPIO_PIN22_SYNC2_BYPASS_V << GPIO_PIN22_SYNC2_BYPASS_S)
#define GPIO_PIN22_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN22_SYNC2_BYPASS_S  0
/** GPIO_PIN22_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN22_PAD_DRIVER    (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_M  (GPIO_PIN22_PAD_DRIVER_V << GPIO_PIN22_PAD_DRIVER_S)
#define GPIO_PIN22_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN22_PAD_DRIVER_S  2
/** GPIO_PIN22_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN22_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN22_SYNC1_BYPASS_M  (GPIO_PIN22_SYNC1_BYPASS_V << GPIO_PIN22_SYNC1_BYPASS_S)
#define GPIO_PIN22_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN22_SYNC1_BYPASS_S  3
/** GPIO_PIN22_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN22_INT_TYPE    0x00000007U
#define GPIO_PIN22_INT_TYPE_M  (GPIO_PIN22_INT_TYPE_V << GPIO_PIN22_INT_TYPE_S)
#define GPIO_PIN22_INT_TYPE_V  0x00000007U
#define GPIO_PIN22_INT_TYPE_S  7
/** GPIO_PIN22_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN22_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_M  (GPIO_PIN22_WAKEUP_ENABLE_V << GPIO_PIN22_WAKEUP_ENABLE_S)
#define GPIO_PIN22_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN22_WAKEUP_ENABLE_S  10
/** GPIO_PIN22_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN22_CONFIG    0x00000003U
#define GPIO_PIN22_CONFIG_M  (GPIO_PIN22_CONFIG_V << GPIO_PIN22_CONFIG_S)
#define GPIO_PIN22_CONFIG_V  0x00000003U
#define GPIO_PIN22_CONFIG_S  11
/** GPIO_PIN22_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN22_INT_ENA    0x0000001FU
#define GPIO_PIN22_INT_ENA_M  (GPIO_PIN22_INT_ENA_V << GPIO_PIN22_INT_ENA_S)
#define GPIO_PIN22_INT_ENA_V  0x0000001FU
#define GPIO_PIN22_INT_ENA_S  13

/** GPIO_PIN23_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN23_REG (DR_REG_GPIO_BASE + 0xd0)
/** GPIO_PIN23_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN23_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN23_SYNC2_BYPASS_M  (GPIO_PIN23_SYNC2_BYPASS_V << GPIO_PIN23_SYNC2_BYPASS_S)
#define GPIO_PIN23_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN23_SYNC2_BYPASS_S  0
/** GPIO_PIN23_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN23_PAD_DRIVER    (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_M  (GPIO_PIN23_PAD_DRIVER_V << GPIO_PIN23_PAD_DRIVER_S)
#define GPIO_PIN23_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN23_PAD_DRIVER_S  2
/** GPIO_PIN23_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN23_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN23_SYNC1_BYPASS_M  (GPIO_PIN23_SYNC1_BYPASS_V << GPIO_PIN23_SYNC1_BYPASS_S)
#define GPIO_PIN23_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN23_SYNC1_BYPASS_S  3
/** GPIO_PIN23_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN23_INT_TYPE    0x00000007U
#define GPIO_PIN23_INT_TYPE_M  (GPIO_PIN23_INT_TYPE_V << GPIO_PIN23_INT_TYPE_S)
#define GPIO_PIN23_INT_TYPE_V  0x00000007U
#define GPIO_PIN23_INT_TYPE_S  7
/** GPIO_PIN23_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN23_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_M  (GPIO_PIN23_WAKEUP_ENABLE_V << GPIO_PIN23_WAKEUP_ENABLE_S)
#define GPIO_PIN23_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN23_WAKEUP_ENABLE_S  10
/** GPIO_PIN23_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN23_CONFIG    0x00000003U
#define GPIO_PIN23_CONFIG_M  (GPIO_PIN23_CONFIG_V << GPIO_PIN23_CONFIG_S)
#define GPIO_PIN23_CONFIG_V  0x00000003U
#define GPIO_PIN23_CONFIG_S  11
/** GPIO_PIN23_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN23_INT_ENA    0x0000001FU
#define GPIO_PIN23_INT_ENA_M  (GPIO_PIN23_INT_ENA_V << GPIO_PIN23_INT_ENA_S)
#define GPIO_PIN23_INT_ENA_V  0x0000001FU
#define GPIO_PIN23_INT_ENA_S  13

/** GPIO_PIN24_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN24_REG (DR_REG_GPIO_BASE + 0xd4)
/** GPIO_PIN24_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN24_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN24_SYNC2_BYPASS_M  (GPIO_PIN24_SYNC2_BYPASS_V << GPIO_PIN24_SYNC2_BYPASS_S)
#define GPIO_PIN24_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN24_SYNC2_BYPASS_S  0
/** GPIO_PIN24_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN24_PAD_DRIVER    (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_M  (GPIO_PIN24_PAD_DRIVER_V << GPIO_PIN24_PAD_DRIVER_S)
#define GPIO_PIN24_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN24_PAD_DRIVER_S  2
/** GPIO_PIN24_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN24_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN24_SYNC1_BYPASS_M  (GPIO_PIN24_SYNC1_BYPASS_V << GPIO_PIN24_SYNC1_BYPASS_S)
#define GPIO_PIN24_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN24_SYNC1_BYPASS_S  3
/** GPIO_PIN24_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN24_INT_TYPE    0x00000007U
#define GPIO_PIN24_INT_TYPE_M  (GPIO_PIN24_INT_TYPE_V << GPIO_PIN24_INT_TYPE_S)
#define GPIO_PIN24_INT_TYPE_V  0x00000007U
#define GPIO_PIN24_INT_TYPE_S  7
/** GPIO_PIN24_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN24_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_M  (GPIO_PIN24_WAKEUP_ENABLE_V << GPIO_PIN24_WAKEUP_ENABLE_S)
#define GPIO_PIN24_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN24_WAKEUP_ENABLE_S  10
/** GPIO_PIN24_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN24_CONFIG    0x00000003U
#define GPIO_PIN24_CONFIG_M  (GPIO_PIN24_CONFIG_V << GPIO_PIN24_CONFIG_S)
#define GPIO_PIN24_CONFIG_V  0x00000003U
#define GPIO_PIN24_CONFIG_S  11
/** GPIO_PIN24_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN24_INT_ENA    0x0000001FU
#define GPIO_PIN24_INT_ENA_M  (GPIO_PIN24_INT_ENA_V << GPIO_PIN24_INT_ENA_S)
#define GPIO_PIN24_INT_ENA_V  0x0000001FU
#define GPIO_PIN24_INT_ENA_S  13

/** GPIO_PIN25_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN25_REG (DR_REG_GPIO_BASE + 0xd8)
/** GPIO_PIN25_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN25_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN25_SYNC2_BYPASS_M  (GPIO_PIN25_SYNC2_BYPASS_V << GPIO_PIN25_SYNC2_BYPASS_S)
#define GPIO_PIN25_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN25_SYNC2_BYPASS_S  0
/** GPIO_PIN25_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN25_PAD_DRIVER    (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_M  (GPIO_PIN25_PAD_DRIVER_V << GPIO_PIN25_PAD_DRIVER_S)
#define GPIO_PIN25_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN25_PAD_DRIVER_S  2
/** GPIO_PIN25_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN25_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN25_SYNC1_BYPASS_M  (GPIO_PIN25_SYNC1_BYPASS_V << GPIO_PIN25_SYNC1_BYPASS_S)
#define GPIO_PIN25_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN25_SYNC1_BYPASS_S  3
/** GPIO_PIN25_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN25_INT_TYPE    0x00000007U
#define GPIO_PIN25_INT_TYPE_M  (GPIO_PIN25_INT_TYPE_V << GPIO_PIN25_INT_TYPE_S)
#define GPIO_PIN25_INT_TYPE_V  0x00000007U
#define GPIO_PIN25_INT_TYPE_S  7
/** GPIO_PIN25_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN25_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_M  (GPIO_PIN25_WAKEUP_ENABLE_V << GPIO_PIN25_WAKEUP_ENABLE_S)
#define GPIO_PIN25_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN25_WAKEUP_ENABLE_S  10
/** GPIO_PIN25_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN25_CONFIG    0x00000003U
#define GPIO_PIN25_CONFIG_M  (GPIO_PIN25_CONFIG_V << GPIO_PIN25_CONFIG_S)
#define GPIO_PIN25_CONFIG_V  0x00000003U
#define GPIO_PIN25_CONFIG_S  11
/** GPIO_PIN25_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN25_INT_ENA    0x0000001FU
#define GPIO_PIN25_INT_ENA_M  (GPIO_PIN25_INT_ENA_V << GPIO_PIN25_INT_ENA_S)
#define GPIO_PIN25_INT_ENA_V  0x0000001FU
#define GPIO_PIN25_INT_ENA_S  13

/** GPIO_PIN26_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN26_REG (DR_REG_GPIO_BASE + 0xdc)
/** GPIO_PIN26_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN26_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN26_SYNC2_BYPASS_M  (GPIO_PIN26_SYNC2_BYPASS_V << GPIO_PIN26_SYNC2_BYPASS_S)
#define GPIO_PIN26_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN26_SYNC2_BYPASS_S  0
/** GPIO_PIN26_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN26_PAD_DRIVER    (BIT(2))
#define GPIO_PIN26_PAD_DRIVER_M  (GPIO_PIN26_PAD_DRIVER_V << GPIO_PIN26_PAD_DRIVER_S)
#define GPIO_PIN26_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN26_PAD_DRIVER_S  2
/** GPIO_PIN26_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN26_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN26_SYNC1_BYPASS_M  (GPIO_PIN26_SYNC1_BYPASS_V << GPIO_PIN26_SYNC1_BYPASS_S)
#define GPIO_PIN26_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN26_SYNC1_BYPASS_S  3
/** GPIO_PIN26_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN26_INT_TYPE    0x00000007U
#define GPIO_PIN26_INT_TYPE_M  (GPIO_PIN26_INT_TYPE_V << GPIO_PIN26_INT_TYPE_S)
#define GPIO_PIN26_INT_TYPE_V  0x00000007U
#define GPIO_PIN26_INT_TYPE_S  7
/** GPIO_PIN26_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN26_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN26_WAKEUP_ENABLE_M  (GPIO_PIN26_WAKEUP_ENABLE_V << GPIO_PIN26_WAKEUP_ENABLE_S)
#define GPIO_PIN26_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN26_WAKEUP_ENABLE_S  10
/** GPIO_PIN26_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN26_CONFIG    0x00000003U
#define GPIO_PIN26_CONFIG_M  (GPIO_PIN26_CONFIG_V << GPIO_PIN26_CONFIG_S)
#define GPIO_PIN26_CONFIG_V  0x00000003U
#define GPIO_PIN26_CONFIG_S  11
/** GPIO_PIN26_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN26_INT_ENA    0x0000001FU
#define GPIO_PIN26_INT_ENA_M  (GPIO_PIN26_INT_ENA_V << GPIO_PIN26_INT_ENA_S)
#define GPIO_PIN26_INT_ENA_V  0x0000001FU
#define GPIO_PIN26_INT_ENA_S  13

/** GPIO_PIN27_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN27_REG (DR_REG_GPIO_BASE + 0xe0)
/** GPIO_PIN27_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN27_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN27_SYNC2_BYPASS_M  (GPIO_PIN27_SYNC2_BYPASS_V << GPIO_PIN27_SYNC2_BYPASS_S)
#define GPIO_PIN27_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN27_SYNC2_BYPASS_S  0
/** GPIO_PIN27_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN27_PAD_DRIVER    (BIT(2))
#define GPIO_PIN27_PAD_DRIVER_M  (GPIO_PIN27_PAD_DRIVER_V << GPIO_PIN27_PAD_DRIVER_S)
#define GPIO_PIN27_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN27_PAD_DRIVER_S  2
/** GPIO_PIN27_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN27_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN27_SYNC1_BYPASS_M  (GPIO_PIN27_SYNC1_BYPASS_V << GPIO_PIN27_SYNC1_BYPASS_S)
#define GPIO_PIN27_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN27_SYNC1_BYPASS_S  3
/** GPIO_PIN27_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN27_INT_TYPE    0x00000007U
#define GPIO_PIN27_INT_TYPE_M  (GPIO_PIN27_INT_TYPE_V << GPIO_PIN27_INT_TYPE_S)
#define GPIO_PIN27_INT_TYPE_V  0x00000007U
#define GPIO_PIN27_INT_TYPE_S  7
/** GPIO_PIN27_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN27_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN27_WAKEUP_ENABLE_M  (GPIO_PIN27_WAKEUP_ENABLE_V << GPIO_PIN27_WAKEUP_ENABLE_S)
#define GPIO_PIN27_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN27_WAKEUP_ENABLE_S  10
/** GPIO_PIN27_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN27_CONFIG    0x00000003U
#define GPIO_PIN27_CONFIG_M  (GPIO_PIN27_CONFIG_V << GPIO_PIN27_CONFIG_S)
#define GPIO_PIN27_CONFIG_V  0x00000003U
#define GPIO_PIN27_CONFIG_S  11
/** GPIO_PIN27_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN27_INT_ENA    0x0000001FU
#define GPIO_PIN27_INT_ENA_M  (GPIO_PIN27_INT_ENA_V << GPIO_PIN27_INT_ENA_S)
#define GPIO_PIN27_INT_ENA_V  0x0000001FU
#define GPIO_PIN27_INT_ENA_S  13

/** GPIO_PIN28_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN28_REG (DR_REG_GPIO_BASE + 0xe4)
/** GPIO_PIN28_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN28_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN28_SYNC2_BYPASS_M  (GPIO_PIN28_SYNC2_BYPASS_V << GPIO_PIN28_SYNC2_BYPASS_S)
#define GPIO_PIN28_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN28_SYNC2_BYPASS_S  0
/** GPIO_PIN28_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN28_PAD_DRIVER    (BIT(2))
#define GPIO_PIN28_PAD_DRIVER_M  (GPIO_PIN28_PAD_DRIVER_V << GPIO_PIN28_PAD_DRIVER_S)
#define GPIO_PIN28_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN28_PAD_DRIVER_S  2
/** GPIO_PIN28_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN28_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN28_SYNC1_BYPASS_M  (GPIO_PIN28_SYNC1_BYPASS_V << GPIO_PIN28_SYNC1_BYPASS_S)
#define GPIO_PIN28_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN28_SYNC1_BYPASS_S  3
/** GPIO_PIN28_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN28_INT_TYPE    0x00000007U
#define GPIO_PIN28_INT_TYPE_M  (GPIO_PIN28_INT_TYPE_V << GPIO_PIN28_INT_TYPE_S)
#define GPIO_PIN28_INT_TYPE_V  0x00000007U
#define GPIO_PIN28_INT_TYPE_S  7
/** GPIO_PIN28_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN28_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN28_WAKEUP_ENABLE_M  (GPIO_PIN28_WAKEUP_ENABLE_V << GPIO_PIN28_WAKEUP_ENABLE_S)
#define GPIO_PIN28_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN28_WAKEUP_ENABLE_S  10
/** GPIO_PIN28_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN28_CONFIG    0x00000003U
#define GPIO_PIN28_CONFIG_M  (GPIO_PIN28_CONFIG_V << GPIO_PIN28_CONFIG_S)
#define GPIO_PIN28_CONFIG_V  0x00000003U
#define GPIO_PIN28_CONFIG_S  11
/** GPIO_PIN28_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN28_INT_ENA    0x0000001FU
#define GPIO_PIN28_INT_ENA_M  (GPIO_PIN28_INT_ENA_V << GPIO_PIN28_INT_ENA_S)
#define GPIO_PIN28_INT_ENA_V  0x0000001FU
#define GPIO_PIN28_INT_ENA_S  13

/** GPIO_PIN29_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN29_REG (DR_REG_GPIO_BASE + 0xe8)
/** GPIO_PIN29_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN29_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN29_SYNC2_BYPASS_M  (GPIO_PIN29_SYNC2_BYPASS_V << GPIO_PIN29_SYNC2_BYPASS_S)
#define GPIO_PIN29_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN29_SYNC2_BYPASS_S  0
/** GPIO_PIN29_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN29_PAD_DRIVER    (BIT(2))
#define GPIO_PIN29_PAD_DRIVER_M  (GPIO_PIN29_PAD_DRIVER_V << GPIO_PIN29_PAD_DRIVER_S)
#define GPIO_PIN29_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN29_PAD_DRIVER_S  2
/** GPIO_PIN29_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN29_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN29_SYNC1_BYPASS_M  (GPIO_PIN29_SYNC1_BYPASS_V << GPIO_PIN29_SYNC1_BYPASS_S)
#define GPIO_PIN29_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN29_SYNC1_BYPASS_S  3
/** GPIO_PIN29_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN29_INT_TYPE    0x00000007U
#define GPIO_PIN29_INT_TYPE_M  (GPIO_PIN29_INT_TYPE_V << GPIO_PIN29_INT_TYPE_S)
#define GPIO_PIN29_INT_TYPE_V  0x00000007U
#define GPIO_PIN29_INT_TYPE_S  7
/** GPIO_PIN29_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN29_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN29_WAKEUP_ENABLE_M  (GPIO_PIN29_WAKEUP_ENABLE_V << GPIO_PIN29_WAKEUP_ENABLE_S)
#define GPIO_PIN29_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN29_WAKEUP_ENABLE_S  10
/** GPIO_PIN29_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN29_CONFIG    0x00000003U
#define GPIO_PIN29_CONFIG_M  (GPIO_PIN29_CONFIG_V << GPIO_PIN29_CONFIG_S)
#define GPIO_PIN29_CONFIG_V  0x00000003U
#define GPIO_PIN29_CONFIG_S  11
/** GPIO_PIN29_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN29_INT_ENA    0x0000001FU
#define GPIO_PIN29_INT_ENA_M  (GPIO_PIN29_INT_ENA_V << GPIO_PIN29_INT_ENA_S)
#define GPIO_PIN29_INT_ENA_V  0x0000001FU
#define GPIO_PIN29_INT_ENA_S  13

/** GPIO_PIN30_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN30_REG (DR_REG_GPIO_BASE + 0xec)
/** GPIO_PIN30_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN30_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN30_SYNC2_BYPASS_M  (GPIO_PIN30_SYNC2_BYPASS_V << GPIO_PIN30_SYNC2_BYPASS_S)
#define GPIO_PIN30_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN30_SYNC2_BYPASS_S  0
/** GPIO_PIN30_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN30_PAD_DRIVER    (BIT(2))
#define GPIO_PIN30_PAD_DRIVER_M  (GPIO_PIN30_PAD_DRIVER_V << GPIO_PIN30_PAD_DRIVER_S)
#define GPIO_PIN30_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN30_PAD_DRIVER_S  2
/** GPIO_PIN30_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN30_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN30_SYNC1_BYPASS_M  (GPIO_PIN30_SYNC1_BYPASS_V << GPIO_PIN30_SYNC1_BYPASS_S)
#define GPIO_PIN30_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN30_SYNC1_BYPASS_S  3
/** GPIO_PIN30_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN30_INT_TYPE    0x00000007U
#define GPIO_PIN30_INT_TYPE_M  (GPIO_PIN30_INT_TYPE_V << GPIO_PIN30_INT_TYPE_S)
#define GPIO_PIN30_INT_TYPE_V  0x00000007U
#define GPIO_PIN30_INT_TYPE_S  7
/** GPIO_PIN30_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN30_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN30_WAKEUP_ENABLE_M  (GPIO_PIN30_WAKEUP_ENABLE_V << GPIO_PIN30_WAKEUP_ENABLE_S)
#define GPIO_PIN30_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN30_WAKEUP_ENABLE_S  10
/** GPIO_PIN30_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN30_CONFIG    0x00000003U
#define GPIO_PIN30_CONFIG_M  (GPIO_PIN30_CONFIG_V << GPIO_PIN30_CONFIG_S)
#define GPIO_PIN30_CONFIG_V  0x00000003U
#define GPIO_PIN30_CONFIG_S  11
/** GPIO_PIN30_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN30_INT_ENA    0x0000001FU
#define GPIO_PIN30_INT_ENA_M  (GPIO_PIN30_INT_ENA_V << GPIO_PIN30_INT_ENA_S)
#define GPIO_PIN30_INT_ENA_V  0x0000001FU
#define GPIO_PIN30_INT_ENA_S  13

/** GPIO_PIN31_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN31_REG (DR_REG_GPIO_BASE + 0xf0)
/** GPIO_PIN31_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN31_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN31_SYNC2_BYPASS_M  (GPIO_PIN31_SYNC2_BYPASS_V << GPIO_PIN31_SYNC2_BYPASS_S)
#define GPIO_PIN31_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN31_SYNC2_BYPASS_S  0
/** GPIO_PIN31_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN31_PAD_DRIVER    (BIT(2))
#define GPIO_PIN31_PAD_DRIVER_M  (GPIO_PIN31_PAD_DRIVER_V << GPIO_PIN31_PAD_DRIVER_S)
#define GPIO_PIN31_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN31_PAD_DRIVER_S  2
/** GPIO_PIN31_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN31_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN31_SYNC1_BYPASS_M  (GPIO_PIN31_SYNC1_BYPASS_V << GPIO_PIN31_SYNC1_BYPASS_S)
#define GPIO_PIN31_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN31_SYNC1_BYPASS_S  3
/** GPIO_PIN31_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN31_INT_TYPE    0x00000007U
#define GPIO_PIN31_INT_TYPE_M  (GPIO_PIN31_INT_TYPE_V << GPIO_PIN31_INT_TYPE_S)
#define GPIO_PIN31_INT_TYPE_V  0x00000007U
#define GPIO_PIN31_INT_TYPE_S  7
/** GPIO_PIN31_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN31_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN31_WAKEUP_ENABLE_M  (GPIO_PIN31_WAKEUP_ENABLE_V << GPIO_PIN31_WAKEUP_ENABLE_S)
#define GPIO_PIN31_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN31_WAKEUP_ENABLE_S  10
/** GPIO_PIN31_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN31_CONFIG    0x00000003U
#define GPIO_PIN31_CONFIG_M  (GPIO_PIN31_CONFIG_V << GPIO_PIN31_CONFIG_S)
#define GPIO_PIN31_CONFIG_V  0x00000003U
#define GPIO_PIN31_CONFIG_S  11
/** GPIO_PIN31_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN31_INT_ENA    0x0000001FU
#define GPIO_PIN31_INT_ENA_M  (GPIO_PIN31_INT_ENA_V << GPIO_PIN31_INT_ENA_S)
#define GPIO_PIN31_INT_ENA_V  0x0000001FU
#define GPIO_PIN31_INT_ENA_S  13

/** GPIO_PIN32_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN32_REG (DR_REG_GPIO_BASE + 0xf4)
/** GPIO_PIN32_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN32_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN32_SYNC2_BYPASS_M  (GPIO_PIN32_SYNC2_BYPASS_V << GPIO_PIN32_SYNC2_BYPASS_S)
#define GPIO_PIN32_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN32_SYNC2_BYPASS_S  0
/** GPIO_PIN32_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN32_PAD_DRIVER    (BIT(2))
#define GPIO_PIN32_PAD_DRIVER_M  (GPIO_PIN32_PAD_DRIVER_V << GPIO_PIN32_PAD_DRIVER_S)
#define GPIO_PIN32_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN32_PAD_DRIVER_S  2
/** GPIO_PIN32_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN32_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN32_SYNC1_BYPASS_M  (GPIO_PIN32_SYNC1_BYPASS_V << GPIO_PIN32_SYNC1_BYPASS_S)
#define GPIO_PIN32_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN32_SYNC1_BYPASS_S  3
/** GPIO_PIN32_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN32_INT_TYPE    0x00000007U
#define GPIO_PIN32_INT_TYPE_M  (GPIO_PIN32_INT_TYPE_V << GPIO_PIN32_INT_TYPE_S)
#define GPIO_PIN32_INT_TYPE_V  0x00000007U
#define GPIO_PIN32_INT_TYPE_S  7
/** GPIO_PIN32_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN32_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN32_WAKEUP_ENABLE_M  (GPIO_PIN32_WAKEUP_ENABLE_V << GPIO_PIN32_WAKEUP_ENABLE_S)
#define GPIO_PIN32_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN32_WAKEUP_ENABLE_S  10
/** GPIO_PIN32_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN32_CONFIG    0x00000003U
#define GPIO_PIN32_CONFIG_M  (GPIO_PIN32_CONFIG_V << GPIO_PIN32_CONFIG_S)
#define GPIO_PIN32_CONFIG_V  0x00000003U
#define GPIO_PIN32_CONFIG_S  11
/** GPIO_PIN32_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN32_INT_ENA    0x0000001FU
#define GPIO_PIN32_INT_ENA_M  (GPIO_PIN32_INT_ENA_V << GPIO_PIN32_INT_ENA_S)
#define GPIO_PIN32_INT_ENA_V  0x0000001FU
#define GPIO_PIN32_INT_ENA_S  13

/** GPIO_PIN33_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN33_REG (DR_REG_GPIO_BASE + 0xf8)
/** GPIO_PIN33_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN33_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN33_SYNC2_BYPASS_M  (GPIO_PIN33_SYNC2_BYPASS_V << GPIO_PIN33_SYNC2_BYPASS_S)
#define GPIO_PIN33_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN33_SYNC2_BYPASS_S  0
/** GPIO_PIN33_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN33_PAD_DRIVER    (BIT(2))
#define GPIO_PIN33_PAD_DRIVER_M  (GPIO_PIN33_PAD_DRIVER_V << GPIO_PIN33_PAD_DRIVER_S)
#define GPIO_PIN33_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN33_PAD_DRIVER_S  2
/** GPIO_PIN33_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN33_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN33_SYNC1_BYPASS_M  (GPIO_PIN33_SYNC1_BYPASS_V << GPIO_PIN33_SYNC1_BYPASS_S)
#define GPIO_PIN33_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN33_SYNC1_BYPASS_S  3
/** GPIO_PIN33_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN33_INT_TYPE    0x00000007U
#define GPIO_PIN33_INT_TYPE_M  (GPIO_PIN33_INT_TYPE_V << GPIO_PIN33_INT_TYPE_S)
#define GPIO_PIN33_INT_TYPE_V  0x00000007U
#define GPIO_PIN33_INT_TYPE_S  7
/** GPIO_PIN33_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN33_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN33_WAKEUP_ENABLE_M  (GPIO_PIN33_WAKEUP_ENABLE_V << GPIO_PIN33_WAKEUP_ENABLE_S)
#define GPIO_PIN33_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN33_WAKEUP_ENABLE_S  10
/** GPIO_PIN33_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN33_CONFIG    0x00000003U
#define GPIO_PIN33_CONFIG_M  (GPIO_PIN33_CONFIG_V << GPIO_PIN33_CONFIG_S)
#define GPIO_PIN33_CONFIG_V  0x00000003U
#define GPIO_PIN33_CONFIG_S  11
/** GPIO_PIN33_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN33_INT_ENA    0x0000001FU
#define GPIO_PIN33_INT_ENA_M  (GPIO_PIN33_INT_ENA_V << GPIO_PIN33_INT_ENA_S)
#define GPIO_PIN33_INT_ENA_V  0x0000001FU
#define GPIO_PIN33_INT_ENA_S  13

/** GPIO_PIN34_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN34_REG (DR_REG_GPIO_BASE + 0xfc)
/** GPIO_PIN34_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN34_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN34_SYNC2_BYPASS_M  (GPIO_PIN34_SYNC2_BYPASS_V << GPIO_PIN34_SYNC2_BYPASS_S)
#define GPIO_PIN34_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN34_SYNC2_BYPASS_S  0
/** GPIO_PIN34_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN34_PAD_DRIVER    (BIT(2))
#define GPIO_PIN34_PAD_DRIVER_M  (GPIO_PIN34_PAD_DRIVER_V << GPIO_PIN34_PAD_DRIVER_S)
#define GPIO_PIN34_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN34_PAD_DRIVER_S  2
/** GPIO_PIN34_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN34_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN34_SYNC1_BYPASS_M  (GPIO_PIN34_SYNC1_BYPASS_V << GPIO_PIN34_SYNC1_BYPASS_S)
#define GPIO_PIN34_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN34_SYNC1_BYPASS_S  3
/** GPIO_PIN34_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN34_INT_TYPE    0x00000007U
#define GPIO_PIN34_INT_TYPE_M  (GPIO_PIN34_INT_TYPE_V << GPIO_PIN34_INT_TYPE_S)
#define GPIO_PIN34_INT_TYPE_V  0x00000007U
#define GPIO_PIN34_INT_TYPE_S  7
/** GPIO_PIN34_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN34_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN34_WAKEUP_ENABLE_M  (GPIO_PIN34_WAKEUP_ENABLE_V << GPIO_PIN34_WAKEUP_ENABLE_S)
#define GPIO_PIN34_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN34_WAKEUP_ENABLE_S  10
/** GPIO_PIN34_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN34_CONFIG    0x00000003U
#define GPIO_PIN34_CONFIG_M  (GPIO_PIN34_CONFIG_V << GPIO_PIN34_CONFIG_S)
#define GPIO_PIN34_CONFIG_V  0x00000003U
#define GPIO_PIN34_CONFIG_S  11
/** GPIO_PIN34_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN34_INT_ENA    0x0000001FU
#define GPIO_PIN34_INT_ENA_M  (GPIO_PIN34_INT_ENA_V << GPIO_PIN34_INT_ENA_S)
#define GPIO_PIN34_INT_ENA_V  0x0000001FU
#define GPIO_PIN34_INT_ENA_S  13

/** GPIO_PIN35_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN35_REG (DR_REG_GPIO_BASE + 0x100)
/** GPIO_PIN35_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN35_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN35_SYNC2_BYPASS_M  (GPIO_PIN35_SYNC2_BYPASS_V << GPIO_PIN35_SYNC2_BYPASS_S)
#define GPIO_PIN35_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN35_SYNC2_BYPASS_S  0
/** GPIO_PIN35_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN35_PAD_DRIVER    (BIT(2))
#define GPIO_PIN35_PAD_DRIVER_M  (GPIO_PIN35_PAD_DRIVER_V << GPIO_PIN35_PAD_DRIVER_S)
#define GPIO_PIN35_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN35_PAD_DRIVER_S  2
/** GPIO_PIN35_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN35_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN35_SYNC1_BYPASS_M  (GPIO_PIN35_SYNC1_BYPASS_V << GPIO_PIN35_SYNC1_BYPASS_S)
#define GPIO_PIN35_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN35_SYNC1_BYPASS_S  3
/** GPIO_PIN35_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN35_INT_TYPE    0x00000007U
#define GPIO_PIN35_INT_TYPE_M  (GPIO_PIN35_INT_TYPE_V << GPIO_PIN35_INT_TYPE_S)
#define GPIO_PIN35_INT_TYPE_V  0x00000007U
#define GPIO_PIN35_INT_TYPE_S  7
/** GPIO_PIN35_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN35_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN35_WAKEUP_ENABLE_M  (GPIO_PIN35_WAKEUP_ENABLE_V << GPIO_PIN35_WAKEUP_ENABLE_S)
#define GPIO_PIN35_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN35_WAKEUP_ENABLE_S  10
/** GPIO_PIN35_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN35_CONFIG    0x00000003U
#define GPIO_PIN35_CONFIG_M  (GPIO_PIN35_CONFIG_V << GPIO_PIN35_CONFIG_S)
#define GPIO_PIN35_CONFIG_V  0x00000003U
#define GPIO_PIN35_CONFIG_S  11
/** GPIO_PIN35_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN35_INT_ENA    0x0000001FU
#define GPIO_PIN35_INT_ENA_M  (GPIO_PIN35_INT_ENA_V << GPIO_PIN35_INT_ENA_S)
#define GPIO_PIN35_INT_ENA_V  0x0000001FU
#define GPIO_PIN35_INT_ENA_S  13

/** GPIO_PIN36_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN36_REG (DR_REG_GPIO_BASE + 0x104)
/** GPIO_PIN36_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN36_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN36_SYNC2_BYPASS_M  (GPIO_PIN36_SYNC2_BYPASS_V << GPIO_PIN36_SYNC2_BYPASS_S)
#define GPIO_PIN36_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN36_SYNC2_BYPASS_S  0
/** GPIO_PIN36_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN36_PAD_DRIVER    (BIT(2))
#define GPIO_PIN36_PAD_DRIVER_M  (GPIO_PIN36_PAD_DRIVER_V << GPIO_PIN36_PAD_DRIVER_S)
#define GPIO_PIN36_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN36_PAD_DRIVER_S  2
/** GPIO_PIN36_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN36_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN36_SYNC1_BYPASS_M  (GPIO_PIN36_SYNC1_BYPASS_V << GPIO_PIN36_SYNC1_BYPASS_S)
#define GPIO_PIN36_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN36_SYNC1_BYPASS_S  3
/** GPIO_PIN36_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN36_INT_TYPE    0x00000007U
#define GPIO_PIN36_INT_TYPE_M  (GPIO_PIN36_INT_TYPE_V << GPIO_PIN36_INT_TYPE_S)
#define GPIO_PIN36_INT_TYPE_V  0x00000007U
#define GPIO_PIN36_INT_TYPE_S  7
/** GPIO_PIN36_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN36_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN36_WAKEUP_ENABLE_M  (GPIO_PIN36_WAKEUP_ENABLE_V << GPIO_PIN36_WAKEUP_ENABLE_S)
#define GPIO_PIN36_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN36_WAKEUP_ENABLE_S  10
/** GPIO_PIN36_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN36_CONFIG    0x00000003U
#define GPIO_PIN36_CONFIG_M  (GPIO_PIN36_CONFIG_V << GPIO_PIN36_CONFIG_S)
#define GPIO_PIN36_CONFIG_V  0x00000003U
#define GPIO_PIN36_CONFIG_S  11
/** GPIO_PIN36_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN36_INT_ENA    0x0000001FU
#define GPIO_PIN36_INT_ENA_M  (GPIO_PIN36_INT_ENA_V << GPIO_PIN36_INT_ENA_S)
#define GPIO_PIN36_INT_ENA_V  0x0000001FU
#define GPIO_PIN36_INT_ENA_S  13

/** GPIO_PIN37_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN37_REG (DR_REG_GPIO_BASE + 0x108)
/** GPIO_PIN37_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN37_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN37_SYNC2_BYPASS_M  (GPIO_PIN37_SYNC2_BYPASS_V << GPIO_PIN37_SYNC2_BYPASS_S)
#define GPIO_PIN37_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN37_SYNC2_BYPASS_S  0
/** GPIO_PIN37_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN37_PAD_DRIVER    (BIT(2))
#define GPIO_PIN37_PAD_DRIVER_M  (GPIO_PIN37_PAD_DRIVER_V << GPIO_PIN37_PAD_DRIVER_S)
#define GPIO_PIN37_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN37_PAD_DRIVER_S  2
/** GPIO_PIN37_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN37_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN37_SYNC1_BYPASS_M  (GPIO_PIN37_SYNC1_BYPASS_V << GPIO_PIN37_SYNC1_BYPASS_S)
#define GPIO_PIN37_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN37_SYNC1_BYPASS_S  3
/** GPIO_PIN37_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN37_INT_TYPE    0x00000007U
#define GPIO_PIN37_INT_TYPE_M  (GPIO_PIN37_INT_TYPE_V << GPIO_PIN37_INT_TYPE_S)
#define GPIO_PIN37_INT_TYPE_V  0x00000007U
#define GPIO_PIN37_INT_TYPE_S  7
/** GPIO_PIN37_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN37_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN37_WAKEUP_ENABLE_M  (GPIO_PIN37_WAKEUP_ENABLE_V << GPIO_PIN37_WAKEUP_ENABLE_S)
#define GPIO_PIN37_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN37_WAKEUP_ENABLE_S  10
/** GPIO_PIN37_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN37_CONFIG    0x00000003U
#define GPIO_PIN37_CONFIG_M  (GPIO_PIN37_CONFIG_V << GPIO_PIN37_CONFIG_S)
#define GPIO_PIN37_CONFIG_V  0x00000003U
#define GPIO_PIN37_CONFIG_S  11
/** GPIO_PIN37_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN37_INT_ENA    0x0000001FU
#define GPIO_PIN37_INT_ENA_M  (GPIO_PIN37_INT_ENA_V << GPIO_PIN37_INT_ENA_S)
#define GPIO_PIN37_INT_ENA_V  0x0000001FU
#define GPIO_PIN37_INT_ENA_S  13

/** GPIO_PIN38_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN38_REG (DR_REG_GPIO_BASE + 0x10c)
/** GPIO_PIN38_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN38_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN38_SYNC2_BYPASS_M  (GPIO_PIN38_SYNC2_BYPASS_V << GPIO_PIN38_SYNC2_BYPASS_S)
#define GPIO_PIN38_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN38_SYNC2_BYPASS_S  0
/** GPIO_PIN38_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN38_PAD_DRIVER    (BIT(2))
#define GPIO_PIN38_PAD_DRIVER_M  (GPIO_PIN38_PAD_DRIVER_V << GPIO_PIN38_PAD_DRIVER_S)
#define GPIO_PIN38_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN38_PAD_DRIVER_S  2
/** GPIO_PIN38_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN38_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN38_SYNC1_BYPASS_M  (GPIO_PIN38_SYNC1_BYPASS_V << GPIO_PIN38_SYNC1_BYPASS_S)
#define GPIO_PIN38_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN38_SYNC1_BYPASS_S  3
/** GPIO_PIN38_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN38_INT_TYPE    0x00000007U
#define GPIO_PIN38_INT_TYPE_M  (GPIO_PIN38_INT_TYPE_V << GPIO_PIN38_INT_TYPE_S)
#define GPIO_PIN38_INT_TYPE_V  0x00000007U
#define GPIO_PIN38_INT_TYPE_S  7
/** GPIO_PIN38_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN38_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN38_WAKEUP_ENABLE_M  (GPIO_PIN38_WAKEUP_ENABLE_V << GPIO_PIN38_WAKEUP_ENABLE_S)
#define GPIO_PIN38_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN38_WAKEUP_ENABLE_S  10
/** GPIO_PIN38_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN38_CONFIG    0x00000003U
#define GPIO_PIN38_CONFIG_M  (GPIO_PIN38_CONFIG_V << GPIO_PIN38_CONFIG_S)
#define GPIO_PIN38_CONFIG_V  0x00000003U
#define GPIO_PIN38_CONFIG_S  11
/** GPIO_PIN38_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN38_INT_ENA    0x0000001FU
#define GPIO_PIN38_INT_ENA_M  (GPIO_PIN38_INT_ENA_V << GPIO_PIN38_INT_ENA_S)
#define GPIO_PIN38_INT_ENA_V  0x0000001FU
#define GPIO_PIN38_INT_ENA_S  13

/** GPIO_PIN39_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN39_REG (DR_REG_GPIO_BASE + 0x110)
/** GPIO_PIN39_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN39_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN39_SYNC2_BYPASS_M  (GPIO_PIN39_SYNC2_BYPASS_V << GPIO_PIN39_SYNC2_BYPASS_S)
#define GPIO_PIN39_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN39_SYNC2_BYPASS_S  0
/** GPIO_PIN39_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN39_PAD_DRIVER    (BIT(2))
#define GPIO_PIN39_PAD_DRIVER_M  (GPIO_PIN39_PAD_DRIVER_V << GPIO_PIN39_PAD_DRIVER_S)
#define GPIO_PIN39_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN39_PAD_DRIVER_S  2
/** GPIO_PIN39_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN39_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN39_SYNC1_BYPASS_M  (GPIO_PIN39_SYNC1_BYPASS_V << GPIO_PIN39_SYNC1_BYPASS_S)
#define GPIO_PIN39_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN39_SYNC1_BYPASS_S  3
/** GPIO_PIN39_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN39_INT_TYPE    0x00000007U
#define GPIO_PIN39_INT_TYPE_M  (GPIO_PIN39_INT_TYPE_V << GPIO_PIN39_INT_TYPE_S)
#define GPIO_PIN39_INT_TYPE_V  0x00000007U
#define GPIO_PIN39_INT_TYPE_S  7
/** GPIO_PIN39_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN39_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN39_WAKEUP_ENABLE_M  (GPIO_PIN39_WAKEUP_ENABLE_V << GPIO_PIN39_WAKEUP_ENABLE_S)
#define GPIO_PIN39_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN39_WAKEUP_ENABLE_S  10
/** GPIO_PIN39_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN39_CONFIG    0x00000003U
#define GPIO_PIN39_CONFIG_M  (GPIO_PIN39_CONFIG_V << GPIO_PIN39_CONFIG_S)
#define GPIO_PIN39_CONFIG_V  0x00000003U
#define GPIO_PIN39_CONFIG_S  11
/** GPIO_PIN39_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN39_INT_ENA    0x0000001FU
#define GPIO_PIN39_INT_ENA_M  (GPIO_PIN39_INT_ENA_V << GPIO_PIN39_INT_ENA_S)
#define GPIO_PIN39_INT_ENA_V  0x0000001FU
#define GPIO_PIN39_INT_ENA_S  13

/** GPIO_PIN40_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN40_REG (DR_REG_GPIO_BASE + 0x114)
/** GPIO_PIN40_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN40_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN40_SYNC2_BYPASS_M  (GPIO_PIN40_SYNC2_BYPASS_V << GPIO_PIN40_SYNC2_BYPASS_S)
#define GPIO_PIN40_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN40_SYNC2_BYPASS_S  0
/** GPIO_PIN40_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN40_PAD_DRIVER    (BIT(2))
#define GPIO_PIN40_PAD_DRIVER_M  (GPIO_PIN40_PAD_DRIVER_V << GPIO_PIN40_PAD_DRIVER_S)
#define GPIO_PIN40_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN40_PAD_DRIVER_S  2
/** GPIO_PIN40_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN40_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN40_SYNC1_BYPASS_M  (GPIO_PIN40_SYNC1_BYPASS_V << GPIO_PIN40_SYNC1_BYPASS_S)
#define GPIO_PIN40_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN40_SYNC1_BYPASS_S  3
/** GPIO_PIN40_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN40_INT_TYPE    0x00000007U
#define GPIO_PIN40_INT_TYPE_M  (GPIO_PIN40_INT_TYPE_V << GPIO_PIN40_INT_TYPE_S)
#define GPIO_PIN40_INT_TYPE_V  0x00000007U
#define GPIO_PIN40_INT_TYPE_S  7
/** GPIO_PIN40_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN40_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN40_WAKEUP_ENABLE_M  (GPIO_PIN40_WAKEUP_ENABLE_V << GPIO_PIN40_WAKEUP_ENABLE_S)
#define GPIO_PIN40_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN40_WAKEUP_ENABLE_S  10
/** GPIO_PIN40_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN40_CONFIG    0x00000003U
#define GPIO_PIN40_CONFIG_M  (GPIO_PIN40_CONFIG_V << GPIO_PIN40_CONFIG_S)
#define GPIO_PIN40_CONFIG_V  0x00000003U
#define GPIO_PIN40_CONFIG_S  11
/** GPIO_PIN40_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN40_INT_ENA    0x0000001FU
#define GPIO_PIN40_INT_ENA_M  (GPIO_PIN40_INT_ENA_V << GPIO_PIN40_INT_ENA_S)
#define GPIO_PIN40_INT_ENA_V  0x0000001FU
#define GPIO_PIN40_INT_ENA_S  13

/** GPIO_PIN41_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN41_REG (DR_REG_GPIO_BASE + 0x118)
/** GPIO_PIN41_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN41_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN41_SYNC2_BYPASS_M  (GPIO_PIN41_SYNC2_BYPASS_V << GPIO_PIN41_SYNC2_BYPASS_S)
#define GPIO_PIN41_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN41_SYNC2_BYPASS_S  0
/** GPIO_PIN41_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN41_PAD_DRIVER    (BIT(2))
#define GPIO_PIN41_PAD_DRIVER_M  (GPIO_PIN41_PAD_DRIVER_V << GPIO_PIN41_PAD_DRIVER_S)
#define GPIO_PIN41_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN41_PAD_DRIVER_S  2
/** GPIO_PIN41_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN41_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN41_SYNC1_BYPASS_M  (GPIO_PIN41_SYNC1_BYPASS_V << GPIO_PIN41_SYNC1_BYPASS_S)
#define GPIO_PIN41_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN41_SYNC1_BYPASS_S  3
/** GPIO_PIN41_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN41_INT_TYPE    0x00000007U
#define GPIO_PIN41_INT_TYPE_M  (GPIO_PIN41_INT_TYPE_V << GPIO_PIN41_INT_TYPE_S)
#define GPIO_PIN41_INT_TYPE_V  0x00000007U
#define GPIO_PIN41_INT_TYPE_S  7
/** GPIO_PIN41_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN41_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN41_WAKEUP_ENABLE_M  (GPIO_PIN41_WAKEUP_ENABLE_V << GPIO_PIN41_WAKEUP_ENABLE_S)
#define GPIO_PIN41_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN41_WAKEUP_ENABLE_S  10
/** GPIO_PIN41_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN41_CONFIG    0x00000003U
#define GPIO_PIN41_CONFIG_M  (GPIO_PIN41_CONFIG_V << GPIO_PIN41_CONFIG_S)
#define GPIO_PIN41_CONFIG_V  0x00000003U
#define GPIO_PIN41_CONFIG_S  11
/** GPIO_PIN41_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN41_INT_ENA    0x0000001FU
#define GPIO_PIN41_INT_ENA_M  (GPIO_PIN41_INT_ENA_V << GPIO_PIN41_INT_ENA_S)
#define GPIO_PIN41_INT_ENA_V  0x0000001FU
#define GPIO_PIN41_INT_ENA_S  13

/** GPIO_PIN42_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN42_REG (DR_REG_GPIO_BASE + 0x11c)
/** GPIO_PIN42_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN42_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN42_SYNC2_BYPASS_M  (GPIO_PIN42_SYNC2_BYPASS_V << GPIO_PIN42_SYNC2_BYPASS_S)
#define GPIO_PIN42_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN42_SYNC2_BYPASS_S  0
/** GPIO_PIN42_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN42_PAD_DRIVER    (BIT(2))
#define GPIO_PIN42_PAD_DRIVER_M  (GPIO_PIN42_PAD_DRIVER_V << GPIO_PIN42_PAD_DRIVER_S)
#define GPIO_PIN42_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN42_PAD_DRIVER_S  2
/** GPIO_PIN42_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN42_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN42_SYNC1_BYPASS_M  (GPIO_PIN42_SYNC1_BYPASS_V << GPIO_PIN42_SYNC1_BYPASS_S)
#define GPIO_PIN42_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN42_SYNC1_BYPASS_S  3
/** GPIO_PIN42_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN42_INT_TYPE    0x00000007U
#define GPIO_PIN42_INT_TYPE_M  (GPIO_PIN42_INT_TYPE_V << GPIO_PIN42_INT_TYPE_S)
#define GPIO_PIN42_INT_TYPE_V  0x00000007U
#define GPIO_PIN42_INT_TYPE_S  7
/** GPIO_PIN42_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN42_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN42_WAKEUP_ENABLE_M  (GPIO_PIN42_WAKEUP_ENABLE_V << GPIO_PIN42_WAKEUP_ENABLE_S)
#define GPIO_PIN42_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN42_WAKEUP_ENABLE_S  10
/** GPIO_PIN42_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN42_CONFIG    0x00000003U
#define GPIO_PIN42_CONFIG_M  (GPIO_PIN42_CONFIG_V << GPIO_PIN42_CONFIG_S)
#define GPIO_PIN42_CONFIG_V  0x00000003U
#define GPIO_PIN42_CONFIG_S  11
/** GPIO_PIN42_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN42_INT_ENA    0x0000001FU
#define GPIO_PIN42_INT_ENA_M  (GPIO_PIN42_INT_ENA_V << GPIO_PIN42_INT_ENA_S)
#define GPIO_PIN42_INT_ENA_V  0x0000001FU
#define GPIO_PIN42_INT_ENA_S  13

/** GPIO_PIN43_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN43_REG (DR_REG_GPIO_BASE + 0x120)
/** GPIO_PIN43_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN43_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN43_SYNC2_BYPASS_M  (GPIO_PIN43_SYNC2_BYPASS_V << GPIO_PIN43_SYNC2_BYPASS_S)
#define GPIO_PIN43_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN43_SYNC2_BYPASS_S  0
/** GPIO_PIN43_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN43_PAD_DRIVER    (BIT(2))
#define GPIO_PIN43_PAD_DRIVER_M  (GPIO_PIN43_PAD_DRIVER_V << GPIO_PIN43_PAD_DRIVER_S)
#define GPIO_PIN43_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN43_PAD_DRIVER_S  2
/** GPIO_PIN43_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN43_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN43_SYNC1_BYPASS_M  (GPIO_PIN43_SYNC1_BYPASS_V << GPIO_PIN43_SYNC1_BYPASS_S)
#define GPIO_PIN43_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN43_SYNC1_BYPASS_S  3
/** GPIO_PIN43_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN43_INT_TYPE    0x00000007U
#define GPIO_PIN43_INT_TYPE_M  (GPIO_PIN43_INT_TYPE_V << GPIO_PIN43_INT_TYPE_S)
#define GPIO_PIN43_INT_TYPE_V  0x00000007U
#define GPIO_PIN43_INT_TYPE_S  7
/** GPIO_PIN43_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN43_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN43_WAKEUP_ENABLE_M  (GPIO_PIN43_WAKEUP_ENABLE_V << GPIO_PIN43_WAKEUP_ENABLE_S)
#define GPIO_PIN43_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN43_WAKEUP_ENABLE_S  10
/** GPIO_PIN43_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN43_CONFIG    0x00000003U
#define GPIO_PIN43_CONFIG_M  (GPIO_PIN43_CONFIG_V << GPIO_PIN43_CONFIG_S)
#define GPIO_PIN43_CONFIG_V  0x00000003U
#define GPIO_PIN43_CONFIG_S  11
/** GPIO_PIN43_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN43_INT_ENA    0x0000001FU
#define GPIO_PIN43_INT_ENA_M  (GPIO_PIN43_INT_ENA_V << GPIO_PIN43_INT_ENA_S)
#define GPIO_PIN43_INT_ENA_V  0x0000001FU
#define GPIO_PIN43_INT_ENA_S  13

/** GPIO_PIN44_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN44_REG (DR_REG_GPIO_BASE + 0x124)
/** GPIO_PIN44_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN44_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN44_SYNC2_BYPASS_M  (GPIO_PIN44_SYNC2_BYPASS_V << GPIO_PIN44_SYNC2_BYPASS_S)
#define GPIO_PIN44_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN44_SYNC2_BYPASS_S  0
/** GPIO_PIN44_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN44_PAD_DRIVER    (BIT(2))
#define GPIO_PIN44_PAD_DRIVER_M  (GPIO_PIN44_PAD_DRIVER_V << GPIO_PIN44_PAD_DRIVER_S)
#define GPIO_PIN44_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN44_PAD_DRIVER_S  2
/** GPIO_PIN44_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN44_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN44_SYNC1_BYPASS_M  (GPIO_PIN44_SYNC1_BYPASS_V << GPIO_PIN44_SYNC1_BYPASS_S)
#define GPIO_PIN44_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN44_SYNC1_BYPASS_S  3
/** GPIO_PIN44_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN44_INT_TYPE    0x00000007U
#define GPIO_PIN44_INT_TYPE_M  (GPIO_PIN44_INT_TYPE_V << GPIO_PIN44_INT_TYPE_S)
#define GPIO_PIN44_INT_TYPE_V  0x00000007U
#define GPIO_PIN44_INT_TYPE_S  7
/** GPIO_PIN44_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN44_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN44_WAKEUP_ENABLE_M  (GPIO_PIN44_WAKEUP_ENABLE_V << GPIO_PIN44_WAKEUP_ENABLE_S)
#define GPIO_PIN44_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN44_WAKEUP_ENABLE_S  10
/** GPIO_PIN44_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN44_CONFIG    0x00000003U
#define GPIO_PIN44_CONFIG_M  (GPIO_PIN44_CONFIG_V << GPIO_PIN44_CONFIG_S)
#define GPIO_PIN44_CONFIG_V  0x00000003U
#define GPIO_PIN44_CONFIG_S  11
/** GPIO_PIN44_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN44_INT_ENA    0x0000001FU
#define GPIO_PIN44_INT_ENA_M  (GPIO_PIN44_INT_ENA_V << GPIO_PIN44_INT_ENA_S)
#define GPIO_PIN44_INT_ENA_V  0x0000001FU
#define GPIO_PIN44_INT_ENA_S  13

/** GPIO_PIN45_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN45_REG (DR_REG_GPIO_BASE + 0x128)
/** GPIO_PIN45_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN45_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN45_SYNC2_BYPASS_M  (GPIO_PIN45_SYNC2_BYPASS_V << GPIO_PIN45_SYNC2_BYPASS_S)
#define GPIO_PIN45_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN45_SYNC2_BYPASS_S  0
/** GPIO_PIN45_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN45_PAD_DRIVER    (BIT(2))
#define GPIO_PIN45_PAD_DRIVER_M  (GPIO_PIN45_PAD_DRIVER_V << GPIO_PIN45_PAD_DRIVER_S)
#define GPIO_PIN45_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN45_PAD_DRIVER_S  2
/** GPIO_PIN45_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN45_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN45_SYNC1_BYPASS_M  (GPIO_PIN45_SYNC1_BYPASS_V << GPIO_PIN45_SYNC1_BYPASS_S)
#define GPIO_PIN45_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN45_SYNC1_BYPASS_S  3
/** GPIO_PIN45_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN45_INT_TYPE    0x00000007U
#define GPIO_PIN45_INT_TYPE_M  (GPIO_PIN45_INT_TYPE_V << GPIO_PIN45_INT_TYPE_S)
#define GPIO_PIN45_INT_TYPE_V  0x00000007U
#define GPIO_PIN45_INT_TYPE_S  7
/** GPIO_PIN45_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN45_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN45_WAKEUP_ENABLE_M  (GPIO_PIN45_WAKEUP_ENABLE_V << GPIO_PIN45_WAKEUP_ENABLE_S)
#define GPIO_PIN45_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN45_WAKEUP_ENABLE_S  10
/** GPIO_PIN45_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN45_CONFIG    0x00000003U
#define GPIO_PIN45_CONFIG_M  (GPIO_PIN45_CONFIG_V << GPIO_PIN45_CONFIG_S)
#define GPIO_PIN45_CONFIG_V  0x00000003U
#define GPIO_PIN45_CONFIG_S  11
/** GPIO_PIN45_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN45_INT_ENA    0x0000001FU
#define GPIO_PIN45_INT_ENA_M  (GPIO_PIN45_INT_ENA_V << GPIO_PIN45_INT_ENA_S)
#define GPIO_PIN45_INT_ENA_V  0x0000001FU
#define GPIO_PIN45_INT_ENA_S  13

/** GPIO_PIN46_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN46_REG (DR_REG_GPIO_BASE + 0x12c)
/** GPIO_PIN46_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN46_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN46_SYNC2_BYPASS_M  (GPIO_PIN46_SYNC2_BYPASS_V << GPIO_PIN46_SYNC2_BYPASS_S)
#define GPIO_PIN46_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN46_SYNC2_BYPASS_S  0
/** GPIO_PIN46_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN46_PAD_DRIVER    (BIT(2))
#define GPIO_PIN46_PAD_DRIVER_M  (GPIO_PIN46_PAD_DRIVER_V << GPIO_PIN46_PAD_DRIVER_S)
#define GPIO_PIN46_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN46_PAD_DRIVER_S  2
/** GPIO_PIN46_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN46_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN46_SYNC1_BYPASS_M  (GPIO_PIN46_SYNC1_BYPASS_V << GPIO_PIN46_SYNC1_BYPASS_S)
#define GPIO_PIN46_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN46_SYNC1_BYPASS_S  3
/** GPIO_PIN46_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN46_INT_TYPE    0x00000007U
#define GPIO_PIN46_INT_TYPE_M  (GPIO_PIN46_INT_TYPE_V << GPIO_PIN46_INT_TYPE_S)
#define GPIO_PIN46_INT_TYPE_V  0x00000007U
#define GPIO_PIN46_INT_TYPE_S  7
/** GPIO_PIN46_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN46_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN46_WAKEUP_ENABLE_M  (GPIO_PIN46_WAKEUP_ENABLE_V << GPIO_PIN46_WAKEUP_ENABLE_S)
#define GPIO_PIN46_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN46_WAKEUP_ENABLE_S  10
/** GPIO_PIN46_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN46_CONFIG    0x00000003U
#define GPIO_PIN46_CONFIG_M  (GPIO_PIN46_CONFIG_V << GPIO_PIN46_CONFIG_S)
#define GPIO_PIN46_CONFIG_V  0x00000003U
#define GPIO_PIN46_CONFIG_S  11
/** GPIO_PIN46_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN46_INT_ENA    0x0000001FU
#define GPIO_PIN46_INT_ENA_M  (GPIO_PIN46_INT_ENA_V << GPIO_PIN46_INT_ENA_S)
#define GPIO_PIN46_INT_ENA_V  0x0000001FU
#define GPIO_PIN46_INT_ENA_S  13

/** GPIO_PIN47_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN47_REG (DR_REG_GPIO_BASE + 0x130)
/** GPIO_PIN47_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN47_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN47_SYNC2_BYPASS_M  (GPIO_PIN47_SYNC2_BYPASS_V << GPIO_PIN47_SYNC2_BYPASS_S)
#define GPIO_PIN47_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN47_SYNC2_BYPASS_S  0
/** GPIO_PIN47_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN47_PAD_DRIVER    (BIT(2))
#define GPIO_PIN47_PAD_DRIVER_M  (GPIO_PIN47_PAD_DRIVER_V << GPIO_PIN47_PAD_DRIVER_S)
#define GPIO_PIN47_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN47_PAD_DRIVER_S  2
/** GPIO_PIN47_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN47_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN47_SYNC1_BYPASS_M  (GPIO_PIN47_SYNC1_BYPASS_V << GPIO_PIN47_SYNC1_BYPASS_S)
#define GPIO_PIN47_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN47_SYNC1_BYPASS_S  3
/** GPIO_PIN47_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN47_INT_TYPE    0x00000007U
#define GPIO_PIN47_INT_TYPE_M  (GPIO_PIN47_INT_TYPE_V << GPIO_PIN47_INT_TYPE_S)
#define GPIO_PIN47_INT_TYPE_V  0x00000007U
#define GPIO_PIN47_INT_TYPE_S  7
/** GPIO_PIN47_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN47_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN47_WAKEUP_ENABLE_M  (GPIO_PIN47_WAKEUP_ENABLE_V << GPIO_PIN47_WAKEUP_ENABLE_S)
#define GPIO_PIN47_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN47_WAKEUP_ENABLE_S  10
/** GPIO_PIN47_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN47_CONFIG    0x00000003U
#define GPIO_PIN47_CONFIG_M  (GPIO_PIN47_CONFIG_V << GPIO_PIN47_CONFIG_S)
#define GPIO_PIN47_CONFIG_V  0x00000003U
#define GPIO_PIN47_CONFIG_S  11
/** GPIO_PIN47_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN47_INT_ENA    0x0000001FU
#define GPIO_PIN47_INT_ENA_M  (GPIO_PIN47_INT_ENA_V << GPIO_PIN47_INT_ENA_S)
#define GPIO_PIN47_INT_ENA_V  0x0000001FU
#define GPIO_PIN47_INT_ENA_S  13

/** GPIO_PIN48_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN48_REG (DR_REG_GPIO_BASE + 0x134)
/** GPIO_PIN48_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN48_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN48_SYNC2_BYPASS_M  (GPIO_PIN48_SYNC2_BYPASS_V << GPIO_PIN48_SYNC2_BYPASS_S)
#define GPIO_PIN48_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN48_SYNC2_BYPASS_S  0
/** GPIO_PIN48_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN48_PAD_DRIVER    (BIT(2))
#define GPIO_PIN48_PAD_DRIVER_M  (GPIO_PIN48_PAD_DRIVER_V << GPIO_PIN48_PAD_DRIVER_S)
#define GPIO_PIN48_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN48_PAD_DRIVER_S  2
/** GPIO_PIN48_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN48_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN48_SYNC1_BYPASS_M  (GPIO_PIN48_SYNC1_BYPASS_V << GPIO_PIN48_SYNC1_BYPASS_S)
#define GPIO_PIN48_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN48_SYNC1_BYPASS_S  3
/** GPIO_PIN48_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN48_INT_TYPE    0x00000007U
#define GPIO_PIN48_INT_TYPE_M  (GPIO_PIN48_INT_TYPE_V << GPIO_PIN48_INT_TYPE_S)
#define GPIO_PIN48_INT_TYPE_V  0x00000007U
#define GPIO_PIN48_INT_TYPE_S  7
/** GPIO_PIN48_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN48_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN48_WAKEUP_ENABLE_M  (GPIO_PIN48_WAKEUP_ENABLE_V << GPIO_PIN48_WAKEUP_ENABLE_S)
#define GPIO_PIN48_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN48_WAKEUP_ENABLE_S  10
/** GPIO_PIN48_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN48_CONFIG    0x00000003U
#define GPIO_PIN48_CONFIG_M  (GPIO_PIN48_CONFIG_V << GPIO_PIN48_CONFIG_S)
#define GPIO_PIN48_CONFIG_V  0x00000003U
#define GPIO_PIN48_CONFIG_S  11
/** GPIO_PIN48_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN48_INT_ENA    0x0000001FU
#define GPIO_PIN48_INT_ENA_M  (GPIO_PIN48_INT_ENA_V << GPIO_PIN48_INT_ENA_S)
#define GPIO_PIN48_INT_ENA_V  0x0000001FU
#define GPIO_PIN48_INT_ENA_S  13

/** GPIO_PIN49_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN49_REG (DR_REG_GPIO_BASE + 0x138)
/** GPIO_PIN49_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN49_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN49_SYNC2_BYPASS_M  (GPIO_PIN49_SYNC2_BYPASS_V << GPIO_PIN49_SYNC2_BYPASS_S)
#define GPIO_PIN49_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN49_SYNC2_BYPASS_S  0
/** GPIO_PIN49_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN49_PAD_DRIVER    (BIT(2))
#define GPIO_PIN49_PAD_DRIVER_M  (GPIO_PIN49_PAD_DRIVER_V << GPIO_PIN49_PAD_DRIVER_S)
#define GPIO_PIN49_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN49_PAD_DRIVER_S  2
/** GPIO_PIN49_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN49_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN49_SYNC1_BYPASS_M  (GPIO_PIN49_SYNC1_BYPASS_V << GPIO_PIN49_SYNC1_BYPASS_S)
#define GPIO_PIN49_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN49_SYNC1_BYPASS_S  3
/** GPIO_PIN49_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN49_INT_TYPE    0x00000007U
#define GPIO_PIN49_INT_TYPE_M  (GPIO_PIN49_INT_TYPE_V << GPIO_PIN49_INT_TYPE_S)
#define GPIO_PIN49_INT_TYPE_V  0x00000007U
#define GPIO_PIN49_INT_TYPE_S  7
/** GPIO_PIN49_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN49_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN49_WAKEUP_ENABLE_M  (GPIO_PIN49_WAKEUP_ENABLE_V << GPIO_PIN49_WAKEUP_ENABLE_S)
#define GPIO_PIN49_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN49_WAKEUP_ENABLE_S  10
/** GPIO_PIN49_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN49_CONFIG    0x00000003U
#define GPIO_PIN49_CONFIG_M  (GPIO_PIN49_CONFIG_V << GPIO_PIN49_CONFIG_S)
#define GPIO_PIN49_CONFIG_V  0x00000003U
#define GPIO_PIN49_CONFIG_S  11
/** GPIO_PIN49_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN49_INT_ENA    0x0000001FU
#define GPIO_PIN49_INT_ENA_M  (GPIO_PIN49_INT_ENA_V << GPIO_PIN49_INT_ENA_S)
#define GPIO_PIN49_INT_ENA_V  0x0000001FU
#define GPIO_PIN49_INT_ENA_S  13

/** GPIO_PIN50_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN50_REG (DR_REG_GPIO_BASE + 0x13c)
/** GPIO_PIN50_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN50_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN50_SYNC2_BYPASS_M  (GPIO_PIN50_SYNC2_BYPASS_V << GPIO_PIN50_SYNC2_BYPASS_S)
#define GPIO_PIN50_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN50_SYNC2_BYPASS_S  0
/** GPIO_PIN50_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN50_PAD_DRIVER    (BIT(2))
#define GPIO_PIN50_PAD_DRIVER_M  (GPIO_PIN50_PAD_DRIVER_V << GPIO_PIN50_PAD_DRIVER_S)
#define GPIO_PIN50_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN50_PAD_DRIVER_S  2
/** GPIO_PIN50_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN50_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN50_SYNC1_BYPASS_M  (GPIO_PIN50_SYNC1_BYPASS_V << GPIO_PIN50_SYNC1_BYPASS_S)
#define GPIO_PIN50_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN50_SYNC1_BYPASS_S  3
/** GPIO_PIN50_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN50_INT_TYPE    0x00000007U
#define GPIO_PIN50_INT_TYPE_M  (GPIO_PIN50_INT_TYPE_V << GPIO_PIN50_INT_TYPE_S)
#define GPIO_PIN50_INT_TYPE_V  0x00000007U
#define GPIO_PIN50_INT_TYPE_S  7
/** GPIO_PIN50_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN50_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN50_WAKEUP_ENABLE_M  (GPIO_PIN50_WAKEUP_ENABLE_V << GPIO_PIN50_WAKEUP_ENABLE_S)
#define GPIO_PIN50_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN50_WAKEUP_ENABLE_S  10
/** GPIO_PIN50_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN50_CONFIG    0x00000003U
#define GPIO_PIN50_CONFIG_M  (GPIO_PIN50_CONFIG_V << GPIO_PIN50_CONFIG_S)
#define GPIO_PIN50_CONFIG_V  0x00000003U
#define GPIO_PIN50_CONFIG_S  11
/** GPIO_PIN50_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN50_INT_ENA    0x0000001FU
#define GPIO_PIN50_INT_ENA_M  (GPIO_PIN50_INT_ENA_V << GPIO_PIN50_INT_ENA_S)
#define GPIO_PIN50_INT_ENA_V  0x0000001FU
#define GPIO_PIN50_INT_ENA_S  13

/** GPIO_PIN51_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN51_REG (DR_REG_GPIO_BASE + 0x140)
/** GPIO_PIN51_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN51_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN51_SYNC2_BYPASS_M  (GPIO_PIN51_SYNC2_BYPASS_V << GPIO_PIN51_SYNC2_BYPASS_S)
#define GPIO_PIN51_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN51_SYNC2_BYPASS_S  0
/** GPIO_PIN51_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN51_PAD_DRIVER    (BIT(2))
#define GPIO_PIN51_PAD_DRIVER_M  (GPIO_PIN51_PAD_DRIVER_V << GPIO_PIN51_PAD_DRIVER_S)
#define GPIO_PIN51_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN51_PAD_DRIVER_S  2
/** GPIO_PIN51_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN51_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN51_SYNC1_BYPASS_M  (GPIO_PIN51_SYNC1_BYPASS_V << GPIO_PIN51_SYNC1_BYPASS_S)
#define GPIO_PIN51_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN51_SYNC1_BYPASS_S  3
/** GPIO_PIN51_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN51_INT_TYPE    0x00000007U
#define GPIO_PIN51_INT_TYPE_M  (GPIO_PIN51_INT_TYPE_V << GPIO_PIN51_INT_TYPE_S)
#define GPIO_PIN51_INT_TYPE_V  0x00000007U
#define GPIO_PIN51_INT_TYPE_S  7
/** GPIO_PIN51_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN51_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN51_WAKEUP_ENABLE_M  (GPIO_PIN51_WAKEUP_ENABLE_V << GPIO_PIN51_WAKEUP_ENABLE_S)
#define GPIO_PIN51_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN51_WAKEUP_ENABLE_S  10
/** GPIO_PIN51_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN51_CONFIG    0x00000003U
#define GPIO_PIN51_CONFIG_M  (GPIO_PIN51_CONFIG_V << GPIO_PIN51_CONFIG_S)
#define GPIO_PIN51_CONFIG_V  0x00000003U
#define GPIO_PIN51_CONFIG_S  11
/** GPIO_PIN51_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN51_INT_ENA    0x0000001FU
#define GPIO_PIN51_INT_ENA_M  (GPIO_PIN51_INT_ENA_V << GPIO_PIN51_INT_ENA_S)
#define GPIO_PIN51_INT_ENA_V  0x0000001FU
#define GPIO_PIN51_INT_ENA_S  13

/** GPIO_PIN52_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN52_REG (DR_REG_GPIO_BASE + 0x144)
/** GPIO_PIN52_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN52_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN52_SYNC2_BYPASS_M  (GPIO_PIN52_SYNC2_BYPASS_V << GPIO_PIN52_SYNC2_BYPASS_S)
#define GPIO_PIN52_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN52_SYNC2_BYPASS_S  0
/** GPIO_PIN52_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN52_PAD_DRIVER    (BIT(2))
#define GPIO_PIN52_PAD_DRIVER_M  (GPIO_PIN52_PAD_DRIVER_V << GPIO_PIN52_PAD_DRIVER_S)
#define GPIO_PIN52_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN52_PAD_DRIVER_S  2
/** GPIO_PIN52_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN52_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN52_SYNC1_BYPASS_M  (GPIO_PIN52_SYNC1_BYPASS_V << GPIO_PIN52_SYNC1_BYPASS_S)
#define GPIO_PIN52_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN52_SYNC1_BYPASS_S  3
/** GPIO_PIN52_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN52_INT_TYPE    0x00000007U
#define GPIO_PIN52_INT_TYPE_M  (GPIO_PIN52_INT_TYPE_V << GPIO_PIN52_INT_TYPE_S)
#define GPIO_PIN52_INT_TYPE_V  0x00000007U
#define GPIO_PIN52_INT_TYPE_S  7
/** GPIO_PIN52_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN52_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN52_WAKEUP_ENABLE_M  (GPIO_PIN52_WAKEUP_ENABLE_V << GPIO_PIN52_WAKEUP_ENABLE_S)
#define GPIO_PIN52_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN52_WAKEUP_ENABLE_S  10
/** GPIO_PIN52_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN52_CONFIG    0x00000003U
#define GPIO_PIN52_CONFIG_M  (GPIO_PIN52_CONFIG_V << GPIO_PIN52_CONFIG_S)
#define GPIO_PIN52_CONFIG_V  0x00000003U
#define GPIO_PIN52_CONFIG_S  11
/** GPIO_PIN52_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN52_INT_ENA    0x0000001FU
#define GPIO_PIN52_INT_ENA_M  (GPIO_PIN52_INT_ENA_V << GPIO_PIN52_INT_ENA_S)
#define GPIO_PIN52_INT_ENA_V  0x0000001FU
#define GPIO_PIN52_INT_ENA_S  13

/** GPIO_PIN53_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN53_REG (DR_REG_GPIO_BASE + 0x148)
/** GPIO_PIN53_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN53_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN53_SYNC2_BYPASS_M  (GPIO_PIN53_SYNC2_BYPASS_V << GPIO_PIN53_SYNC2_BYPASS_S)
#define GPIO_PIN53_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN53_SYNC2_BYPASS_S  0
/** GPIO_PIN53_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN53_PAD_DRIVER    (BIT(2))
#define GPIO_PIN53_PAD_DRIVER_M  (GPIO_PIN53_PAD_DRIVER_V << GPIO_PIN53_PAD_DRIVER_S)
#define GPIO_PIN53_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN53_PAD_DRIVER_S  2
/** GPIO_PIN53_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN53_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN53_SYNC1_BYPASS_M  (GPIO_PIN53_SYNC1_BYPASS_V << GPIO_PIN53_SYNC1_BYPASS_S)
#define GPIO_PIN53_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN53_SYNC1_BYPASS_S  3
/** GPIO_PIN53_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN53_INT_TYPE    0x00000007U
#define GPIO_PIN53_INT_TYPE_M  (GPIO_PIN53_INT_TYPE_V << GPIO_PIN53_INT_TYPE_S)
#define GPIO_PIN53_INT_TYPE_V  0x00000007U
#define GPIO_PIN53_INT_TYPE_S  7
/** GPIO_PIN53_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN53_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN53_WAKEUP_ENABLE_M  (GPIO_PIN53_WAKEUP_ENABLE_V << GPIO_PIN53_WAKEUP_ENABLE_S)
#define GPIO_PIN53_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN53_WAKEUP_ENABLE_S  10
/** GPIO_PIN53_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN53_CONFIG    0x00000003U
#define GPIO_PIN53_CONFIG_M  (GPIO_PIN53_CONFIG_V << GPIO_PIN53_CONFIG_S)
#define GPIO_PIN53_CONFIG_V  0x00000003U
#define GPIO_PIN53_CONFIG_S  11
/** GPIO_PIN53_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN53_INT_ENA    0x0000001FU
#define GPIO_PIN53_INT_ENA_M  (GPIO_PIN53_INT_ENA_V << GPIO_PIN53_INT_ENA_S)
#define GPIO_PIN53_INT_ENA_V  0x0000001FU
#define GPIO_PIN53_INT_ENA_S  13

/** GPIO_PIN54_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN54_REG (DR_REG_GPIO_BASE + 0x14c)
/** GPIO_PIN54_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN54_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN54_SYNC2_BYPASS_M  (GPIO_PIN54_SYNC2_BYPASS_V << GPIO_PIN54_SYNC2_BYPASS_S)
#define GPIO_PIN54_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN54_SYNC2_BYPASS_S  0
/** GPIO_PIN54_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN54_PAD_DRIVER    (BIT(2))
#define GPIO_PIN54_PAD_DRIVER_M  (GPIO_PIN54_PAD_DRIVER_V << GPIO_PIN54_PAD_DRIVER_S)
#define GPIO_PIN54_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN54_PAD_DRIVER_S  2
/** GPIO_PIN54_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN54_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN54_SYNC1_BYPASS_M  (GPIO_PIN54_SYNC1_BYPASS_V << GPIO_PIN54_SYNC1_BYPASS_S)
#define GPIO_PIN54_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN54_SYNC1_BYPASS_S  3
/** GPIO_PIN54_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN54_INT_TYPE    0x00000007U
#define GPIO_PIN54_INT_TYPE_M  (GPIO_PIN54_INT_TYPE_V << GPIO_PIN54_INT_TYPE_S)
#define GPIO_PIN54_INT_TYPE_V  0x00000007U
#define GPIO_PIN54_INT_TYPE_S  7
/** GPIO_PIN54_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN54_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN54_WAKEUP_ENABLE_M  (GPIO_PIN54_WAKEUP_ENABLE_V << GPIO_PIN54_WAKEUP_ENABLE_S)
#define GPIO_PIN54_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN54_WAKEUP_ENABLE_S  10
/** GPIO_PIN54_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN54_CONFIG    0x00000003U
#define GPIO_PIN54_CONFIG_M  (GPIO_PIN54_CONFIG_V << GPIO_PIN54_CONFIG_S)
#define GPIO_PIN54_CONFIG_V  0x00000003U
#define GPIO_PIN54_CONFIG_S  11
/** GPIO_PIN54_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN54_INT_ENA    0x0000001FU
#define GPIO_PIN54_INT_ENA_M  (GPIO_PIN54_INT_ENA_V << GPIO_PIN54_INT_ENA_S)
#define GPIO_PIN54_INT_ENA_V  0x0000001FU
#define GPIO_PIN54_INT_ENA_S  13

/** GPIO_PIN55_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN55_REG (DR_REG_GPIO_BASE + 0x150)
/** GPIO_PIN55_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN55_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN55_SYNC2_BYPASS_M  (GPIO_PIN55_SYNC2_BYPASS_V << GPIO_PIN55_SYNC2_BYPASS_S)
#define GPIO_PIN55_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN55_SYNC2_BYPASS_S  0
/** GPIO_PIN55_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN55_PAD_DRIVER    (BIT(2))
#define GPIO_PIN55_PAD_DRIVER_M  (GPIO_PIN55_PAD_DRIVER_V << GPIO_PIN55_PAD_DRIVER_S)
#define GPIO_PIN55_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN55_PAD_DRIVER_S  2
/** GPIO_PIN55_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN55_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN55_SYNC1_BYPASS_M  (GPIO_PIN55_SYNC1_BYPASS_V << GPIO_PIN55_SYNC1_BYPASS_S)
#define GPIO_PIN55_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN55_SYNC1_BYPASS_S  3
/** GPIO_PIN55_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN55_INT_TYPE    0x00000007U
#define GPIO_PIN55_INT_TYPE_M  (GPIO_PIN55_INT_TYPE_V << GPIO_PIN55_INT_TYPE_S)
#define GPIO_PIN55_INT_TYPE_V  0x00000007U
#define GPIO_PIN55_INT_TYPE_S  7
/** GPIO_PIN55_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN55_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN55_WAKEUP_ENABLE_M  (GPIO_PIN55_WAKEUP_ENABLE_V << GPIO_PIN55_WAKEUP_ENABLE_S)
#define GPIO_PIN55_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN55_WAKEUP_ENABLE_S  10
/** GPIO_PIN55_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN55_CONFIG    0x00000003U
#define GPIO_PIN55_CONFIG_M  (GPIO_PIN55_CONFIG_V << GPIO_PIN55_CONFIG_S)
#define GPIO_PIN55_CONFIG_V  0x00000003U
#define GPIO_PIN55_CONFIG_S  11
/** GPIO_PIN55_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN55_INT_ENA    0x0000001FU
#define GPIO_PIN55_INT_ENA_M  (GPIO_PIN55_INT_ENA_V << GPIO_PIN55_INT_ENA_S)
#define GPIO_PIN55_INT_ENA_V  0x0000001FU
#define GPIO_PIN55_INT_ENA_S  13

/** GPIO_PIN56_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN56_REG (DR_REG_GPIO_BASE + 0x154)
/** GPIO_PIN56_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN56_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN56_SYNC2_BYPASS_M  (GPIO_PIN56_SYNC2_BYPASS_V << GPIO_PIN56_SYNC2_BYPASS_S)
#define GPIO_PIN56_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN56_SYNC2_BYPASS_S  0
/** GPIO_PIN56_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  set this bit to select pad driver. 1:open-drain. 0:normal.
 */
#define GPIO_PIN56_PAD_DRIVER    (BIT(2))
#define GPIO_PIN56_PAD_DRIVER_M  (GPIO_PIN56_PAD_DRIVER_V << GPIO_PIN56_PAD_DRIVER_S)
#define GPIO_PIN56_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN56_PAD_DRIVER_S  2
/** GPIO_PIN56_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
 *  posedge.
 */
#define GPIO_PIN56_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN56_SYNC1_BYPASS_M  (GPIO_PIN56_SYNC1_BYPASS_V << GPIO_PIN56_SYNC1_BYPASS_S)
#define GPIO_PIN56_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN56_SYNC1_BYPASS_S  3
/** GPIO_PIN56_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
 *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
 *  at high level
 */
#define GPIO_PIN56_INT_TYPE    0x00000007U
#define GPIO_PIN56_INT_TYPE_M  (GPIO_PIN56_INT_TYPE_V << GPIO_PIN56_INT_TYPE_S)
#define GPIO_PIN56_INT_TYPE_V  0x00000007U
#define GPIO_PIN56_INT_TYPE_S  7
/** GPIO_PIN56_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
 */
#define GPIO_PIN56_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN56_WAKEUP_ENABLE_M  (GPIO_PIN56_WAKEUP_ENABLE_V << GPIO_PIN56_WAKEUP_ENABLE_S)
#define GPIO_PIN56_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN56_WAKEUP_ENABLE_S  10
/** GPIO_PIN56_CONFIG : R/W; bitpos: [12:11]; default: 0;
 *  reserved
 */
#define GPIO_PIN56_CONFIG    0x00000003U
#define GPIO_PIN56_CONFIG_M  (GPIO_PIN56_CONFIG_V << GPIO_PIN56_CONFIG_S)
#define GPIO_PIN56_CONFIG_V  0x00000003U
#define GPIO_PIN56_CONFIG_S  11
/** GPIO_PIN56_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
 *  interrupt.
 */
#define GPIO_PIN56_INT_ENA    0x0000001FU
#define GPIO_PIN56_INT_ENA_M  (GPIO_PIN56_INT_ENA_V << GPIO_PIN56_INT_ENA_S)
#define GPIO_PIN56_INT_ENA_V  0x0000001FU
#define GPIO_PIN56_INT_ENA_S  13

/** GPIO_FUNC1_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC1_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x15c)
/** GPIO_FUNC1_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC1_IN_SEL    0x0000003FU
#define GPIO_FUNC1_IN_SEL_M  (GPIO_FUNC1_IN_SEL_V << GPIO_FUNC1_IN_SEL_S)
#define GPIO_FUNC1_IN_SEL_V  0x0000003FU
#define GPIO_FUNC1_IN_SEL_S  0
/** GPIO_FUNC1_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC1_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC1_IN_INV_SEL_M  (GPIO_FUNC1_IN_INV_SEL_V << GPIO_FUNC1_IN_INV_SEL_S)
#define GPIO_FUNC1_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_IN_INV_SEL_S  6
/** GPIO_SIG1_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG1_IN_SEL    (BIT(7))
#define GPIO_SIG1_IN_SEL_M  (GPIO_SIG1_IN_SEL_V << GPIO_SIG1_IN_SEL_S)
#define GPIO_SIG1_IN_SEL_V  0x00000001U
#define GPIO_SIG1_IN_SEL_S  7

/** GPIO_FUNC2_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC2_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x160)
/** GPIO_FUNC2_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC2_IN_SEL    0x0000003FU
#define GPIO_FUNC2_IN_SEL_M  (GPIO_FUNC2_IN_SEL_V << GPIO_FUNC2_IN_SEL_S)
#define GPIO_FUNC2_IN_SEL_V  0x0000003FU
#define GPIO_FUNC2_IN_SEL_S  0
/** GPIO_FUNC2_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC2_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC2_IN_INV_SEL_M  (GPIO_FUNC2_IN_INV_SEL_V << GPIO_FUNC2_IN_INV_SEL_S)
#define GPIO_FUNC2_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_IN_INV_SEL_S  6
/** GPIO_SIG2_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG2_IN_SEL    (BIT(7))
#define GPIO_SIG2_IN_SEL_M  (GPIO_SIG2_IN_SEL_V << GPIO_SIG2_IN_SEL_S)
#define GPIO_SIG2_IN_SEL_V  0x00000001U
#define GPIO_SIG2_IN_SEL_S  7

/** GPIO_FUNC3_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC3_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x164)
/** GPIO_FUNC3_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC3_IN_SEL    0x0000003FU
#define GPIO_FUNC3_IN_SEL_M  (GPIO_FUNC3_IN_SEL_V << GPIO_FUNC3_IN_SEL_S)
#define GPIO_FUNC3_IN_SEL_V  0x0000003FU
#define GPIO_FUNC3_IN_SEL_S  0
/** GPIO_FUNC3_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC3_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC3_IN_INV_SEL_M  (GPIO_FUNC3_IN_INV_SEL_V << GPIO_FUNC3_IN_INV_SEL_S)
#define GPIO_FUNC3_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_IN_INV_SEL_S  6
/** GPIO_SIG3_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG3_IN_SEL    (BIT(7))
#define GPIO_SIG3_IN_SEL_M  (GPIO_SIG3_IN_SEL_V << GPIO_SIG3_IN_SEL_S)
#define GPIO_SIG3_IN_SEL_V  0x00000001U
#define GPIO_SIG3_IN_SEL_S  7

/** GPIO_FUNC4_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC4_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x168)
/** GPIO_FUNC4_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC4_IN_SEL    0x0000003FU
#define GPIO_FUNC4_IN_SEL_M  (GPIO_FUNC4_IN_SEL_V << GPIO_FUNC4_IN_SEL_S)
#define GPIO_FUNC4_IN_SEL_V  0x0000003FU
#define GPIO_FUNC4_IN_SEL_S  0
/** GPIO_FUNC4_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC4_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC4_IN_INV_SEL_M  (GPIO_FUNC4_IN_INV_SEL_V << GPIO_FUNC4_IN_INV_SEL_S)
#define GPIO_FUNC4_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_IN_INV_SEL_S  6
/** GPIO_SIG4_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG4_IN_SEL    (BIT(7))
#define GPIO_SIG4_IN_SEL_M  (GPIO_SIG4_IN_SEL_V << GPIO_SIG4_IN_SEL_S)
#define GPIO_SIG4_IN_SEL_V  0x00000001U
#define GPIO_SIG4_IN_SEL_S  7

/** GPIO_FUNC5_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC5_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x16c)
/** GPIO_FUNC5_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC5_IN_SEL    0x0000003FU
#define GPIO_FUNC5_IN_SEL_M  (GPIO_FUNC5_IN_SEL_V << GPIO_FUNC5_IN_SEL_S)
#define GPIO_FUNC5_IN_SEL_V  0x0000003FU
#define GPIO_FUNC5_IN_SEL_S  0
/** GPIO_FUNC5_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC5_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC5_IN_INV_SEL_M  (GPIO_FUNC5_IN_INV_SEL_V << GPIO_FUNC5_IN_INV_SEL_S)
#define GPIO_FUNC5_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_IN_INV_SEL_S  6
/** GPIO_SIG5_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG5_IN_SEL    (BIT(7))
#define GPIO_SIG5_IN_SEL_M  (GPIO_SIG5_IN_SEL_V << GPIO_SIG5_IN_SEL_S)
#define GPIO_SIG5_IN_SEL_V  0x00000001U
#define GPIO_SIG5_IN_SEL_S  7

/** GPIO_FUNC6_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC6_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x170)
/** GPIO_FUNC6_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC6_IN_SEL    0x0000003FU
#define GPIO_FUNC6_IN_SEL_M  (GPIO_FUNC6_IN_SEL_V << GPIO_FUNC6_IN_SEL_S)
#define GPIO_FUNC6_IN_SEL_V  0x0000003FU
#define GPIO_FUNC6_IN_SEL_S  0
/** GPIO_FUNC6_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC6_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC6_IN_INV_SEL_M  (GPIO_FUNC6_IN_INV_SEL_V << GPIO_FUNC6_IN_INV_SEL_S)
#define GPIO_FUNC6_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_IN_INV_SEL_S  6
/** GPIO_SIG6_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG6_IN_SEL    (BIT(7))
#define GPIO_SIG6_IN_SEL_M  (GPIO_SIG6_IN_SEL_V << GPIO_SIG6_IN_SEL_S)
#define GPIO_SIG6_IN_SEL_V  0x00000001U
#define GPIO_SIG6_IN_SEL_S  7

/** GPIO_FUNC7_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC7_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x174)
/** GPIO_FUNC7_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC7_IN_SEL    0x0000003FU
#define GPIO_FUNC7_IN_SEL_M  (GPIO_FUNC7_IN_SEL_V << GPIO_FUNC7_IN_SEL_S)
#define GPIO_FUNC7_IN_SEL_V  0x0000003FU
#define GPIO_FUNC7_IN_SEL_S  0
/** GPIO_FUNC7_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC7_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC7_IN_INV_SEL_M  (GPIO_FUNC7_IN_INV_SEL_V << GPIO_FUNC7_IN_INV_SEL_S)
#define GPIO_FUNC7_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_IN_INV_SEL_S  6
/** GPIO_SIG7_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG7_IN_SEL    (BIT(7))
#define GPIO_SIG7_IN_SEL_M  (GPIO_SIG7_IN_SEL_V << GPIO_SIG7_IN_SEL_S)
#define GPIO_SIG7_IN_SEL_V  0x00000001U
#define GPIO_SIG7_IN_SEL_S  7

/** GPIO_FUNC8_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC8_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x178)
/** GPIO_FUNC8_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC8_IN_SEL    0x0000003FU
#define GPIO_FUNC8_IN_SEL_M  (GPIO_FUNC8_IN_SEL_V << GPIO_FUNC8_IN_SEL_S)
#define GPIO_FUNC8_IN_SEL_V  0x0000003FU
#define GPIO_FUNC8_IN_SEL_S  0
/** GPIO_FUNC8_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC8_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC8_IN_INV_SEL_M  (GPIO_FUNC8_IN_INV_SEL_V << GPIO_FUNC8_IN_INV_SEL_S)
#define GPIO_FUNC8_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_IN_INV_SEL_S  6
/** GPIO_SIG8_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG8_IN_SEL    (BIT(7))
#define GPIO_SIG8_IN_SEL_M  (GPIO_SIG8_IN_SEL_V << GPIO_SIG8_IN_SEL_S)
#define GPIO_SIG8_IN_SEL_V  0x00000001U
#define GPIO_SIG8_IN_SEL_S  7

/** GPIO_FUNC9_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC9_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x17c)
/** GPIO_FUNC9_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC9_IN_SEL    0x0000003FU
#define GPIO_FUNC9_IN_SEL_M  (GPIO_FUNC9_IN_SEL_V << GPIO_FUNC9_IN_SEL_S)
#define GPIO_FUNC9_IN_SEL_V  0x0000003FU
#define GPIO_FUNC9_IN_SEL_S  0
/** GPIO_FUNC9_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC9_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC9_IN_INV_SEL_M  (GPIO_FUNC9_IN_INV_SEL_V << GPIO_FUNC9_IN_INV_SEL_S)
#define GPIO_FUNC9_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_IN_INV_SEL_S  6
/** GPIO_SIG9_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG9_IN_SEL    (BIT(7))
#define GPIO_SIG9_IN_SEL_M  (GPIO_SIG9_IN_SEL_V << GPIO_SIG9_IN_SEL_S)
#define GPIO_SIG9_IN_SEL_V  0x00000001U
#define GPIO_SIG9_IN_SEL_S  7

/** GPIO_FUNC10_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC10_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x180)
/** GPIO_FUNC10_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC10_IN_SEL    0x0000003FU
#define GPIO_FUNC10_IN_SEL_M  (GPIO_FUNC10_IN_SEL_V << GPIO_FUNC10_IN_SEL_S)
#define GPIO_FUNC10_IN_SEL_V  0x0000003FU
#define GPIO_FUNC10_IN_SEL_S  0
/** GPIO_FUNC10_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC10_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC10_IN_INV_SEL_M  (GPIO_FUNC10_IN_INV_SEL_V << GPIO_FUNC10_IN_INV_SEL_S)
#define GPIO_FUNC10_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_IN_INV_SEL_S  6
/** GPIO_SIG10_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG10_IN_SEL    (BIT(7))
#define GPIO_SIG10_IN_SEL_M  (GPIO_SIG10_IN_SEL_V << GPIO_SIG10_IN_SEL_S)
#define GPIO_SIG10_IN_SEL_V  0x00000001U
#define GPIO_SIG10_IN_SEL_S  7

/** GPIO_FUNC11_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC11_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x184)
/** GPIO_FUNC11_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC11_IN_SEL    0x0000003FU
#define GPIO_FUNC11_IN_SEL_M  (GPIO_FUNC11_IN_SEL_V << GPIO_FUNC11_IN_SEL_S)
#define GPIO_FUNC11_IN_SEL_V  0x0000003FU
#define GPIO_FUNC11_IN_SEL_S  0
/** GPIO_FUNC11_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC11_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC11_IN_INV_SEL_M  (GPIO_FUNC11_IN_INV_SEL_V << GPIO_FUNC11_IN_INV_SEL_S)
#define GPIO_FUNC11_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_IN_INV_SEL_S  6
/** GPIO_SIG11_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG11_IN_SEL    (BIT(7))
#define GPIO_SIG11_IN_SEL_M  (GPIO_SIG11_IN_SEL_V << GPIO_SIG11_IN_SEL_S)
#define GPIO_SIG11_IN_SEL_V  0x00000001U
#define GPIO_SIG11_IN_SEL_S  7

/** GPIO_FUNC12_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC12_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x188)
/** GPIO_FUNC12_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC12_IN_SEL    0x0000003FU
#define GPIO_FUNC12_IN_SEL_M  (GPIO_FUNC12_IN_SEL_V << GPIO_FUNC12_IN_SEL_S)
#define GPIO_FUNC12_IN_SEL_V  0x0000003FU
#define GPIO_FUNC12_IN_SEL_S  0
/** GPIO_FUNC12_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC12_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC12_IN_INV_SEL_M  (GPIO_FUNC12_IN_INV_SEL_V << GPIO_FUNC12_IN_INV_SEL_S)
#define GPIO_FUNC12_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_IN_INV_SEL_S  6
/** GPIO_SIG12_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG12_IN_SEL    (BIT(7))
#define GPIO_SIG12_IN_SEL_M  (GPIO_SIG12_IN_SEL_V << GPIO_SIG12_IN_SEL_S)
#define GPIO_SIG12_IN_SEL_V  0x00000001U
#define GPIO_SIG12_IN_SEL_S  7

/** GPIO_FUNC13_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC13_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x18c)
/** GPIO_FUNC13_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC13_IN_SEL    0x0000003FU
#define GPIO_FUNC13_IN_SEL_M  (GPIO_FUNC13_IN_SEL_V << GPIO_FUNC13_IN_SEL_S)
#define GPIO_FUNC13_IN_SEL_V  0x0000003FU
#define GPIO_FUNC13_IN_SEL_S  0
/** GPIO_FUNC13_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC13_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC13_IN_INV_SEL_M  (GPIO_FUNC13_IN_INV_SEL_V << GPIO_FUNC13_IN_INV_SEL_S)
#define GPIO_FUNC13_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_IN_INV_SEL_S  6
/** GPIO_SIG13_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG13_IN_SEL    (BIT(7))
#define GPIO_SIG13_IN_SEL_M  (GPIO_SIG13_IN_SEL_V << GPIO_SIG13_IN_SEL_S)
#define GPIO_SIG13_IN_SEL_V  0x00000001U
#define GPIO_SIG13_IN_SEL_S  7

/** GPIO_FUNC14_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC14_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x190)
/** GPIO_FUNC14_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC14_IN_SEL    0x0000003FU
#define GPIO_FUNC14_IN_SEL_M  (GPIO_FUNC14_IN_SEL_V << GPIO_FUNC14_IN_SEL_S)
#define GPIO_FUNC14_IN_SEL_V  0x0000003FU
#define GPIO_FUNC14_IN_SEL_S  0
/** GPIO_FUNC14_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC14_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC14_IN_INV_SEL_M  (GPIO_FUNC14_IN_INV_SEL_V << GPIO_FUNC14_IN_INV_SEL_S)
#define GPIO_FUNC14_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_IN_INV_SEL_S  6
/** GPIO_SIG14_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG14_IN_SEL    (BIT(7))
#define GPIO_SIG14_IN_SEL_M  (GPIO_SIG14_IN_SEL_V << GPIO_SIG14_IN_SEL_S)
#define GPIO_SIG14_IN_SEL_V  0x00000001U
#define GPIO_SIG14_IN_SEL_S  7

/** GPIO_FUNC15_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC15_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x194)
/** GPIO_FUNC15_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC15_IN_SEL    0x0000003FU
#define GPIO_FUNC15_IN_SEL_M  (GPIO_FUNC15_IN_SEL_V << GPIO_FUNC15_IN_SEL_S)
#define GPIO_FUNC15_IN_SEL_V  0x0000003FU
#define GPIO_FUNC15_IN_SEL_S  0
/** GPIO_FUNC15_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC15_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC15_IN_INV_SEL_M  (GPIO_FUNC15_IN_INV_SEL_V << GPIO_FUNC15_IN_INV_SEL_S)
#define GPIO_FUNC15_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_IN_INV_SEL_S  6
/** GPIO_SIG15_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG15_IN_SEL    (BIT(7))
#define GPIO_SIG15_IN_SEL_M  (GPIO_SIG15_IN_SEL_V << GPIO_SIG15_IN_SEL_S)
#define GPIO_SIG15_IN_SEL_V  0x00000001U
#define GPIO_SIG15_IN_SEL_S  7

/** GPIO_FUNC16_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC16_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x198)
/** GPIO_FUNC16_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC16_IN_SEL    0x0000003FU
#define GPIO_FUNC16_IN_SEL_M  (GPIO_FUNC16_IN_SEL_V << GPIO_FUNC16_IN_SEL_S)
#define GPIO_FUNC16_IN_SEL_V  0x0000003FU
#define GPIO_FUNC16_IN_SEL_S  0
/** GPIO_FUNC16_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC16_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC16_IN_INV_SEL_M  (GPIO_FUNC16_IN_INV_SEL_V << GPIO_FUNC16_IN_INV_SEL_S)
#define GPIO_FUNC16_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_IN_INV_SEL_S  6
/** GPIO_SIG16_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG16_IN_SEL    (BIT(7))
#define GPIO_SIG16_IN_SEL_M  (GPIO_SIG16_IN_SEL_V << GPIO_SIG16_IN_SEL_S)
#define GPIO_SIG16_IN_SEL_V  0x00000001U
#define GPIO_SIG16_IN_SEL_S  7

/** GPIO_FUNC17_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC17_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x19c)
/** GPIO_FUNC17_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC17_IN_SEL    0x0000003FU
#define GPIO_FUNC17_IN_SEL_M  (GPIO_FUNC17_IN_SEL_V << GPIO_FUNC17_IN_SEL_S)
#define GPIO_FUNC17_IN_SEL_V  0x0000003FU
#define GPIO_FUNC17_IN_SEL_S  0
/** GPIO_FUNC17_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC17_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC17_IN_INV_SEL_M  (GPIO_FUNC17_IN_INV_SEL_V << GPIO_FUNC17_IN_INV_SEL_S)
#define GPIO_FUNC17_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_IN_INV_SEL_S  6
/** GPIO_SIG17_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG17_IN_SEL    (BIT(7))
#define GPIO_SIG17_IN_SEL_M  (GPIO_SIG17_IN_SEL_V << GPIO_SIG17_IN_SEL_S)
#define GPIO_SIG17_IN_SEL_V  0x00000001U
#define GPIO_SIG17_IN_SEL_S  7

/** GPIO_FUNC18_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC18_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1a0)
/** GPIO_FUNC18_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC18_IN_SEL    0x0000003FU
#define GPIO_FUNC18_IN_SEL_M  (GPIO_FUNC18_IN_SEL_V << GPIO_FUNC18_IN_SEL_S)
#define GPIO_FUNC18_IN_SEL_V  0x0000003FU
#define GPIO_FUNC18_IN_SEL_S  0
/** GPIO_FUNC18_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC18_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC18_IN_INV_SEL_M  (GPIO_FUNC18_IN_INV_SEL_V << GPIO_FUNC18_IN_INV_SEL_S)
#define GPIO_FUNC18_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_IN_INV_SEL_S  6
/** GPIO_SIG18_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG18_IN_SEL    (BIT(7))
#define GPIO_SIG18_IN_SEL_M  (GPIO_SIG18_IN_SEL_V << GPIO_SIG18_IN_SEL_S)
#define GPIO_SIG18_IN_SEL_V  0x00000001U
#define GPIO_SIG18_IN_SEL_S  7

/** GPIO_FUNC19_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC19_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1a4)
/** GPIO_FUNC19_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC19_IN_SEL    0x0000003FU
#define GPIO_FUNC19_IN_SEL_M  (GPIO_FUNC19_IN_SEL_V << GPIO_FUNC19_IN_SEL_S)
#define GPIO_FUNC19_IN_SEL_V  0x0000003FU
#define GPIO_FUNC19_IN_SEL_S  0
/** GPIO_FUNC19_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC19_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC19_IN_INV_SEL_M  (GPIO_FUNC19_IN_INV_SEL_V << GPIO_FUNC19_IN_INV_SEL_S)
#define GPIO_FUNC19_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_IN_INV_SEL_S  6
/** GPIO_SIG19_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG19_IN_SEL    (BIT(7))
#define GPIO_SIG19_IN_SEL_M  (GPIO_SIG19_IN_SEL_V << GPIO_SIG19_IN_SEL_S)
#define GPIO_SIG19_IN_SEL_V  0x00000001U
#define GPIO_SIG19_IN_SEL_S  7

/** GPIO_FUNC20_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC20_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1a8)
/** GPIO_FUNC20_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC20_IN_SEL    0x0000003FU
#define GPIO_FUNC20_IN_SEL_M  (GPIO_FUNC20_IN_SEL_V << GPIO_FUNC20_IN_SEL_S)
#define GPIO_FUNC20_IN_SEL_V  0x0000003FU
#define GPIO_FUNC20_IN_SEL_S  0
/** GPIO_FUNC20_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC20_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC20_IN_INV_SEL_M  (GPIO_FUNC20_IN_INV_SEL_V << GPIO_FUNC20_IN_INV_SEL_S)
#define GPIO_FUNC20_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_IN_INV_SEL_S  6
/** GPIO_SIG20_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG20_IN_SEL    (BIT(7))
#define GPIO_SIG20_IN_SEL_M  (GPIO_SIG20_IN_SEL_V << GPIO_SIG20_IN_SEL_S)
#define GPIO_SIG20_IN_SEL_V  0x00000001U
#define GPIO_SIG20_IN_SEL_S  7

/** GPIO_FUNC21_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC21_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1ac)
/** GPIO_FUNC21_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC21_IN_SEL    0x0000003FU
#define GPIO_FUNC21_IN_SEL_M  (GPIO_FUNC21_IN_SEL_V << GPIO_FUNC21_IN_SEL_S)
#define GPIO_FUNC21_IN_SEL_V  0x0000003FU
#define GPIO_FUNC21_IN_SEL_S  0
/** GPIO_FUNC21_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC21_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC21_IN_INV_SEL_M  (GPIO_FUNC21_IN_INV_SEL_V << GPIO_FUNC21_IN_INV_SEL_S)
#define GPIO_FUNC21_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_IN_INV_SEL_S  6
/** GPIO_SIG21_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG21_IN_SEL    (BIT(7))
#define GPIO_SIG21_IN_SEL_M  (GPIO_SIG21_IN_SEL_V << GPIO_SIG21_IN_SEL_S)
#define GPIO_SIG21_IN_SEL_V  0x00000001U
#define GPIO_SIG21_IN_SEL_S  7

/** GPIO_FUNC22_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC22_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1b0)
/** GPIO_FUNC22_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC22_IN_SEL    0x0000003FU
#define GPIO_FUNC22_IN_SEL_M  (GPIO_FUNC22_IN_SEL_V << GPIO_FUNC22_IN_SEL_S)
#define GPIO_FUNC22_IN_SEL_V  0x0000003FU
#define GPIO_FUNC22_IN_SEL_S  0
/** GPIO_FUNC22_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC22_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC22_IN_INV_SEL_M  (GPIO_FUNC22_IN_INV_SEL_V << GPIO_FUNC22_IN_INV_SEL_S)
#define GPIO_FUNC22_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_IN_INV_SEL_S  6
/** GPIO_SIG22_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG22_IN_SEL    (BIT(7))
#define GPIO_SIG22_IN_SEL_M  (GPIO_SIG22_IN_SEL_V << GPIO_SIG22_IN_SEL_S)
#define GPIO_SIG22_IN_SEL_V  0x00000001U
#define GPIO_SIG22_IN_SEL_S  7

/** GPIO_FUNC23_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC23_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1b4)
/** GPIO_FUNC23_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC23_IN_SEL    0x0000003FU
#define GPIO_FUNC23_IN_SEL_M  (GPIO_FUNC23_IN_SEL_V << GPIO_FUNC23_IN_SEL_S)
#define GPIO_FUNC23_IN_SEL_V  0x0000003FU
#define GPIO_FUNC23_IN_SEL_S  0
/** GPIO_FUNC23_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC23_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC23_IN_INV_SEL_M  (GPIO_FUNC23_IN_INV_SEL_V << GPIO_FUNC23_IN_INV_SEL_S)
#define GPIO_FUNC23_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_IN_INV_SEL_S  6
/** GPIO_SIG23_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG23_IN_SEL    (BIT(7))
#define GPIO_SIG23_IN_SEL_M  (GPIO_SIG23_IN_SEL_V << GPIO_SIG23_IN_SEL_S)
#define GPIO_SIG23_IN_SEL_V  0x00000001U
#define GPIO_SIG23_IN_SEL_S  7

/** GPIO_FUNC24_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC24_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1b8)
/** GPIO_FUNC24_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC24_IN_SEL    0x0000003FU
#define GPIO_FUNC24_IN_SEL_M  (GPIO_FUNC24_IN_SEL_V << GPIO_FUNC24_IN_SEL_S)
#define GPIO_FUNC24_IN_SEL_V  0x0000003FU
#define GPIO_FUNC24_IN_SEL_S  0
/** GPIO_FUNC24_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC24_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC24_IN_INV_SEL_M  (GPIO_FUNC24_IN_INV_SEL_V << GPIO_FUNC24_IN_INV_SEL_S)
#define GPIO_FUNC24_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_IN_INV_SEL_S  6
/** GPIO_SIG24_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG24_IN_SEL    (BIT(7))
#define GPIO_SIG24_IN_SEL_M  (GPIO_SIG24_IN_SEL_V << GPIO_SIG24_IN_SEL_S)
#define GPIO_SIG24_IN_SEL_V  0x00000001U
#define GPIO_SIG24_IN_SEL_S  7

/** GPIO_FUNC25_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC25_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1bc)
/** GPIO_FUNC25_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC25_IN_SEL    0x0000003FU
#define GPIO_FUNC25_IN_SEL_M  (GPIO_FUNC25_IN_SEL_V << GPIO_FUNC25_IN_SEL_S)
#define GPIO_FUNC25_IN_SEL_V  0x0000003FU
#define GPIO_FUNC25_IN_SEL_S  0
/** GPIO_FUNC25_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC25_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC25_IN_INV_SEL_M  (GPIO_FUNC25_IN_INV_SEL_V << GPIO_FUNC25_IN_INV_SEL_S)
#define GPIO_FUNC25_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_IN_INV_SEL_S  6
/** GPIO_SIG25_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG25_IN_SEL    (BIT(7))
#define GPIO_SIG25_IN_SEL_M  (GPIO_SIG25_IN_SEL_V << GPIO_SIG25_IN_SEL_S)
#define GPIO_SIG25_IN_SEL_V  0x00000001U
#define GPIO_SIG25_IN_SEL_S  7

/** GPIO_FUNC26_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC26_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1c0)
/** GPIO_FUNC26_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC26_IN_SEL    0x0000003FU
#define GPIO_FUNC26_IN_SEL_M  (GPIO_FUNC26_IN_SEL_V << GPIO_FUNC26_IN_SEL_S)
#define GPIO_FUNC26_IN_SEL_V  0x0000003FU
#define GPIO_FUNC26_IN_SEL_S  0
/** GPIO_FUNC26_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC26_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC26_IN_INV_SEL_M  (GPIO_FUNC26_IN_INV_SEL_V << GPIO_FUNC26_IN_INV_SEL_S)
#define GPIO_FUNC26_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_IN_INV_SEL_S  6
/** GPIO_SIG26_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG26_IN_SEL    (BIT(7))
#define GPIO_SIG26_IN_SEL_M  (GPIO_SIG26_IN_SEL_V << GPIO_SIG26_IN_SEL_S)
#define GPIO_SIG26_IN_SEL_V  0x00000001U
#define GPIO_SIG26_IN_SEL_S  7

/** GPIO_FUNC27_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC27_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1c4)
/** GPIO_FUNC27_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC27_IN_SEL    0x0000003FU
#define GPIO_FUNC27_IN_SEL_M  (GPIO_FUNC27_IN_SEL_V << GPIO_FUNC27_IN_SEL_S)
#define GPIO_FUNC27_IN_SEL_V  0x0000003FU
#define GPIO_FUNC27_IN_SEL_S  0
/** GPIO_FUNC27_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC27_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC27_IN_INV_SEL_M  (GPIO_FUNC27_IN_INV_SEL_V << GPIO_FUNC27_IN_INV_SEL_S)
#define GPIO_FUNC27_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_IN_INV_SEL_S  6
/** GPIO_SIG27_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG27_IN_SEL    (BIT(7))
#define GPIO_SIG27_IN_SEL_M  (GPIO_SIG27_IN_SEL_V << GPIO_SIG27_IN_SEL_S)
#define GPIO_SIG27_IN_SEL_V  0x00000001U
#define GPIO_SIG27_IN_SEL_S  7

/** GPIO_FUNC28_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC28_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1c8)
/** GPIO_FUNC28_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC28_IN_SEL    0x0000003FU
#define GPIO_FUNC28_IN_SEL_M  (GPIO_FUNC28_IN_SEL_V << GPIO_FUNC28_IN_SEL_S)
#define GPIO_FUNC28_IN_SEL_V  0x0000003FU
#define GPIO_FUNC28_IN_SEL_S  0
/** GPIO_FUNC28_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC28_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC28_IN_INV_SEL_M  (GPIO_FUNC28_IN_INV_SEL_V << GPIO_FUNC28_IN_INV_SEL_S)
#define GPIO_FUNC28_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_IN_INV_SEL_S  6
/** GPIO_SIG28_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG28_IN_SEL    (BIT(7))
#define GPIO_SIG28_IN_SEL_M  (GPIO_SIG28_IN_SEL_V << GPIO_SIG28_IN_SEL_S)
#define GPIO_SIG28_IN_SEL_V  0x00000001U
#define GPIO_SIG28_IN_SEL_S  7

/** GPIO_FUNC29_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC29_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1cc)
/** GPIO_FUNC29_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC29_IN_SEL    0x0000003FU
#define GPIO_FUNC29_IN_SEL_M  (GPIO_FUNC29_IN_SEL_V << GPIO_FUNC29_IN_SEL_S)
#define GPIO_FUNC29_IN_SEL_V  0x0000003FU
#define GPIO_FUNC29_IN_SEL_S  0
/** GPIO_FUNC29_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC29_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC29_IN_INV_SEL_M  (GPIO_FUNC29_IN_INV_SEL_V << GPIO_FUNC29_IN_INV_SEL_S)
#define GPIO_FUNC29_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_IN_INV_SEL_S  6
/** GPIO_SIG29_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG29_IN_SEL    (BIT(7))
#define GPIO_SIG29_IN_SEL_M  (GPIO_SIG29_IN_SEL_V << GPIO_SIG29_IN_SEL_S)
#define GPIO_SIG29_IN_SEL_V  0x00000001U
#define GPIO_SIG29_IN_SEL_S  7

/** GPIO_FUNC30_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC30_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1d0)
/** GPIO_FUNC30_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC30_IN_SEL    0x0000003FU
#define GPIO_FUNC30_IN_SEL_M  (GPIO_FUNC30_IN_SEL_V << GPIO_FUNC30_IN_SEL_S)
#define GPIO_FUNC30_IN_SEL_V  0x0000003FU
#define GPIO_FUNC30_IN_SEL_S  0
/** GPIO_FUNC30_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC30_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC30_IN_INV_SEL_M  (GPIO_FUNC30_IN_INV_SEL_V << GPIO_FUNC30_IN_INV_SEL_S)
#define GPIO_FUNC30_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_IN_INV_SEL_S  6
/** GPIO_SIG30_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG30_IN_SEL    (BIT(7))
#define GPIO_SIG30_IN_SEL_M  (GPIO_SIG30_IN_SEL_V << GPIO_SIG30_IN_SEL_S)
#define GPIO_SIG30_IN_SEL_V  0x00000001U
#define GPIO_SIG30_IN_SEL_S  7

/** GPIO_FUNC31_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC31_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1d4)
/** GPIO_FUNC31_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC31_IN_SEL    0x0000003FU
#define GPIO_FUNC31_IN_SEL_M  (GPIO_FUNC31_IN_SEL_V << GPIO_FUNC31_IN_SEL_S)
#define GPIO_FUNC31_IN_SEL_V  0x0000003FU
#define GPIO_FUNC31_IN_SEL_S  0
/** GPIO_FUNC31_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC31_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC31_IN_INV_SEL_M  (GPIO_FUNC31_IN_INV_SEL_V << GPIO_FUNC31_IN_INV_SEL_S)
#define GPIO_FUNC31_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_IN_INV_SEL_S  6
/** GPIO_SIG31_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG31_IN_SEL    (BIT(7))
#define GPIO_SIG31_IN_SEL_M  (GPIO_SIG31_IN_SEL_V << GPIO_SIG31_IN_SEL_S)
#define GPIO_SIG31_IN_SEL_V  0x00000001U
#define GPIO_SIG31_IN_SEL_S  7

/** GPIO_FUNC32_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC32_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1d8)
/** GPIO_FUNC32_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC32_IN_SEL    0x0000003FU
#define GPIO_FUNC32_IN_SEL_M  (GPIO_FUNC32_IN_SEL_V << GPIO_FUNC32_IN_SEL_S)
#define GPIO_FUNC32_IN_SEL_V  0x0000003FU
#define GPIO_FUNC32_IN_SEL_S  0
/** GPIO_FUNC32_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC32_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC32_IN_INV_SEL_M  (GPIO_FUNC32_IN_INV_SEL_V << GPIO_FUNC32_IN_INV_SEL_S)
#define GPIO_FUNC32_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_IN_INV_SEL_S  6
/** GPIO_SIG32_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG32_IN_SEL    (BIT(7))
#define GPIO_SIG32_IN_SEL_M  (GPIO_SIG32_IN_SEL_V << GPIO_SIG32_IN_SEL_S)
#define GPIO_SIG32_IN_SEL_V  0x00000001U
#define GPIO_SIG32_IN_SEL_S  7

/** GPIO_FUNC33_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC33_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1dc)
/** GPIO_FUNC33_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC33_IN_SEL    0x0000003FU
#define GPIO_FUNC33_IN_SEL_M  (GPIO_FUNC33_IN_SEL_V << GPIO_FUNC33_IN_SEL_S)
#define GPIO_FUNC33_IN_SEL_V  0x0000003FU
#define GPIO_FUNC33_IN_SEL_S  0
/** GPIO_FUNC33_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC33_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC33_IN_INV_SEL_M  (GPIO_FUNC33_IN_INV_SEL_V << GPIO_FUNC33_IN_INV_SEL_S)
#define GPIO_FUNC33_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_IN_INV_SEL_S  6
/** GPIO_SIG33_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG33_IN_SEL    (BIT(7))
#define GPIO_SIG33_IN_SEL_M  (GPIO_SIG33_IN_SEL_V << GPIO_SIG33_IN_SEL_S)
#define GPIO_SIG33_IN_SEL_V  0x00000001U
#define GPIO_SIG33_IN_SEL_S  7

/** GPIO_FUNC34_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC34_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1e0)
/** GPIO_FUNC34_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC34_IN_SEL    0x0000003FU
#define GPIO_FUNC34_IN_SEL_M  (GPIO_FUNC34_IN_SEL_V << GPIO_FUNC34_IN_SEL_S)
#define GPIO_FUNC34_IN_SEL_V  0x0000003FU
#define GPIO_FUNC34_IN_SEL_S  0
/** GPIO_FUNC34_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC34_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC34_IN_INV_SEL_M  (GPIO_FUNC34_IN_INV_SEL_V << GPIO_FUNC34_IN_INV_SEL_S)
#define GPIO_FUNC34_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_IN_INV_SEL_S  6
/** GPIO_SIG34_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG34_IN_SEL    (BIT(7))
#define GPIO_SIG34_IN_SEL_M  (GPIO_SIG34_IN_SEL_V << GPIO_SIG34_IN_SEL_S)
#define GPIO_SIG34_IN_SEL_V  0x00000001U
#define GPIO_SIG34_IN_SEL_S  7

/** GPIO_FUNC35_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC35_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1e4)
/** GPIO_FUNC35_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC35_IN_SEL    0x0000003FU
#define GPIO_FUNC35_IN_SEL_M  (GPIO_FUNC35_IN_SEL_V << GPIO_FUNC35_IN_SEL_S)
#define GPIO_FUNC35_IN_SEL_V  0x0000003FU
#define GPIO_FUNC35_IN_SEL_S  0
/** GPIO_FUNC35_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC35_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC35_IN_INV_SEL_M  (GPIO_FUNC35_IN_INV_SEL_V << GPIO_FUNC35_IN_INV_SEL_S)
#define GPIO_FUNC35_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_IN_INV_SEL_S  6
/** GPIO_SIG35_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG35_IN_SEL    (BIT(7))
#define GPIO_SIG35_IN_SEL_M  (GPIO_SIG35_IN_SEL_V << GPIO_SIG35_IN_SEL_S)
#define GPIO_SIG35_IN_SEL_V  0x00000001U
#define GPIO_SIG35_IN_SEL_S  7

/** GPIO_FUNC36_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC36_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1e8)
/** GPIO_FUNC36_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC36_IN_SEL    0x0000003FU
#define GPIO_FUNC36_IN_SEL_M  (GPIO_FUNC36_IN_SEL_V << GPIO_FUNC36_IN_SEL_S)
#define GPIO_FUNC36_IN_SEL_V  0x0000003FU
#define GPIO_FUNC36_IN_SEL_S  0
/** GPIO_FUNC36_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC36_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC36_IN_INV_SEL_M  (GPIO_FUNC36_IN_INV_SEL_V << GPIO_FUNC36_IN_INV_SEL_S)
#define GPIO_FUNC36_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC36_IN_INV_SEL_S  6
/** GPIO_SIG36_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG36_IN_SEL    (BIT(7))
#define GPIO_SIG36_IN_SEL_M  (GPIO_SIG36_IN_SEL_V << GPIO_SIG36_IN_SEL_S)
#define GPIO_SIG36_IN_SEL_V  0x00000001U
#define GPIO_SIG36_IN_SEL_S  7

/** GPIO_FUNC37_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC37_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1ec)
/** GPIO_FUNC37_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC37_IN_SEL    0x0000003FU
#define GPIO_FUNC37_IN_SEL_M  (GPIO_FUNC37_IN_SEL_V << GPIO_FUNC37_IN_SEL_S)
#define GPIO_FUNC37_IN_SEL_V  0x0000003FU
#define GPIO_FUNC37_IN_SEL_S  0
/** GPIO_FUNC37_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC37_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC37_IN_INV_SEL_M  (GPIO_FUNC37_IN_INV_SEL_V << GPIO_FUNC37_IN_INV_SEL_S)
#define GPIO_FUNC37_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC37_IN_INV_SEL_S  6
/** GPIO_SIG37_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG37_IN_SEL    (BIT(7))
#define GPIO_SIG37_IN_SEL_M  (GPIO_SIG37_IN_SEL_V << GPIO_SIG37_IN_SEL_S)
#define GPIO_SIG37_IN_SEL_V  0x00000001U
#define GPIO_SIG37_IN_SEL_S  7

/** GPIO_FUNC38_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC38_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1f0)
/** GPIO_FUNC38_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC38_IN_SEL    0x0000003FU
#define GPIO_FUNC38_IN_SEL_M  (GPIO_FUNC38_IN_SEL_V << GPIO_FUNC38_IN_SEL_S)
#define GPIO_FUNC38_IN_SEL_V  0x0000003FU
#define GPIO_FUNC38_IN_SEL_S  0
/** GPIO_FUNC38_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC38_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC38_IN_INV_SEL_M  (GPIO_FUNC38_IN_INV_SEL_V << GPIO_FUNC38_IN_INV_SEL_S)
#define GPIO_FUNC38_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC38_IN_INV_SEL_S  6
/** GPIO_SIG38_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG38_IN_SEL    (BIT(7))
#define GPIO_SIG38_IN_SEL_M  (GPIO_SIG38_IN_SEL_V << GPIO_SIG38_IN_SEL_S)
#define GPIO_SIG38_IN_SEL_V  0x00000001U
#define GPIO_SIG38_IN_SEL_S  7

/** GPIO_FUNC39_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC39_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1f4)
/** GPIO_FUNC39_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC39_IN_SEL    0x0000003FU
#define GPIO_FUNC39_IN_SEL_M  (GPIO_FUNC39_IN_SEL_V << GPIO_FUNC39_IN_SEL_S)
#define GPIO_FUNC39_IN_SEL_V  0x0000003FU
#define GPIO_FUNC39_IN_SEL_S  0
/** GPIO_FUNC39_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC39_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC39_IN_INV_SEL_M  (GPIO_FUNC39_IN_INV_SEL_V << GPIO_FUNC39_IN_INV_SEL_S)
#define GPIO_FUNC39_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC39_IN_INV_SEL_S  6
/** GPIO_SIG39_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG39_IN_SEL    (BIT(7))
#define GPIO_SIG39_IN_SEL_M  (GPIO_SIG39_IN_SEL_V << GPIO_SIG39_IN_SEL_S)
#define GPIO_SIG39_IN_SEL_V  0x00000001U
#define GPIO_SIG39_IN_SEL_S  7

/** GPIO_FUNC40_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC40_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1f8)
/** GPIO_FUNC40_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC40_IN_SEL    0x0000003FU
#define GPIO_FUNC40_IN_SEL_M  (GPIO_FUNC40_IN_SEL_V << GPIO_FUNC40_IN_SEL_S)
#define GPIO_FUNC40_IN_SEL_V  0x0000003FU
#define GPIO_FUNC40_IN_SEL_S  0
/** GPIO_FUNC40_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC40_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC40_IN_INV_SEL_M  (GPIO_FUNC40_IN_INV_SEL_V << GPIO_FUNC40_IN_INV_SEL_S)
#define GPIO_FUNC40_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC40_IN_INV_SEL_S  6
/** GPIO_SIG40_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG40_IN_SEL    (BIT(7))
#define GPIO_SIG40_IN_SEL_M  (GPIO_SIG40_IN_SEL_V << GPIO_SIG40_IN_SEL_S)
#define GPIO_SIG40_IN_SEL_V  0x00000001U
#define GPIO_SIG40_IN_SEL_S  7

/** GPIO_FUNC41_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC41_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1fc)
/** GPIO_FUNC41_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC41_IN_SEL    0x0000003FU
#define GPIO_FUNC41_IN_SEL_M  (GPIO_FUNC41_IN_SEL_V << GPIO_FUNC41_IN_SEL_S)
#define GPIO_FUNC41_IN_SEL_V  0x0000003FU
#define GPIO_FUNC41_IN_SEL_S  0
/** GPIO_FUNC41_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC41_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC41_IN_INV_SEL_M  (GPIO_FUNC41_IN_INV_SEL_V << GPIO_FUNC41_IN_INV_SEL_S)
#define GPIO_FUNC41_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_IN_INV_SEL_S  6
/** GPIO_SIG41_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG41_IN_SEL    (BIT(7))
#define GPIO_SIG41_IN_SEL_M  (GPIO_SIG41_IN_SEL_V << GPIO_SIG41_IN_SEL_S)
#define GPIO_SIG41_IN_SEL_V  0x00000001U
#define GPIO_SIG41_IN_SEL_S  7

/** GPIO_FUNC42_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC42_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x200)
/** GPIO_FUNC42_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC42_IN_SEL    0x0000003FU
#define GPIO_FUNC42_IN_SEL_M  (GPIO_FUNC42_IN_SEL_V << GPIO_FUNC42_IN_SEL_S)
#define GPIO_FUNC42_IN_SEL_V  0x0000003FU
#define GPIO_FUNC42_IN_SEL_S  0
/** GPIO_FUNC42_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC42_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC42_IN_INV_SEL_M  (GPIO_FUNC42_IN_INV_SEL_V << GPIO_FUNC42_IN_INV_SEL_S)
#define GPIO_FUNC42_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_IN_INV_SEL_S  6
/** GPIO_SIG42_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG42_IN_SEL    (BIT(7))
#define GPIO_SIG42_IN_SEL_M  (GPIO_SIG42_IN_SEL_V << GPIO_SIG42_IN_SEL_S)
#define GPIO_SIG42_IN_SEL_V  0x00000001U
#define GPIO_SIG42_IN_SEL_S  7

/** GPIO_FUNC43_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC43_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x204)
/** GPIO_FUNC43_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC43_IN_SEL    0x0000003FU
#define GPIO_FUNC43_IN_SEL_M  (GPIO_FUNC43_IN_SEL_V << GPIO_FUNC43_IN_SEL_S)
#define GPIO_FUNC43_IN_SEL_V  0x0000003FU
#define GPIO_FUNC43_IN_SEL_S  0
/** GPIO_FUNC43_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC43_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC43_IN_INV_SEL_M  (GPIO_FUNC43_IN_INV_SEL_V << GPIO_FUNC43_IN_INV_SEL_S)
#define GPIO_FUNC43_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_IN_INV_SEL_S  6
/** GPIO_SIG43_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG43_IN_SEL    (BIT(7))
#define GPIO_SIG43_IN_SEL_M  (GPIO_SIG43_IN_SEL_V << GPIO_SIG43_IN_SEL_S)
#define GPIO_SIG43_IN_SEL_V  0x00000001U
#define GPIO_SIG43_IN_SEL_S  7

/** GPIO_FUNC44_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC44_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x208)
/** GPIO_FUNC44_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC44_IN_SEL    0x0000003FU
#define GPIO_FUNC44_IN_SEL_M  (GPIO_FUNC44_IN_SEL_V << GPIO_FUNC44_IN_SEL_S)
#define GPIO_FUNC44_IN_SEL_V  0x0000003FU
#define GPIO_FUNC44_IN_SEL_S  0
/** GPIO_FUNC44_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC44_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC44_IN_INV_SEL_M  (GPIO_FUNC44_IN_INV_SEL_V << GPIO_FUNC44_IN_INV_SEL_S)
#define GPIO_FUNC44_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC44_IN_INV_SEL_S  6
/** GPIO_SIG44_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG44_IN_SEL    (BIT(7))
#define GPIO_SIG44_IN_SEL_M  (GPIO_SIG44_IN_SEL_V << GPIO_SIG44_IN_SEL_S)
#define GPIO_SIG44_IN_SEL_V  0x00000001U
#define GPIO_SIG44_IN_SEL_S  7

/** GPIO_FUNC45_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC45_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x20c)
/** GPIO_FUNC45_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC45_IN_SEL    0x0000003FU
#define GPIO_FUNC45_IN_SEL_M  (GPIO_FUNC45_IN_SEL_V << GPIO_FUNC45_IN_SEL_S)
#define GPIO_FUNC45_IN_SEL_V  0x0000003FU
#define GPIO_FUNC45_IN_SEL_S  0
/** GPIO_FUNC45_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC45_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC45_IN_INV_SEL_M  (GPIO_FUNC45_IN_INV_SEL_V << GPIO_FUNC45_IN_INV_SEL_S)
#define GPIO_FUNC45_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC45_IN_INV_SEL_S  6
/** GPIO_SIG45_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG45_IN_SEL    (BIT(7))
#define GPIO_SIG45_IN_SEL_M  (GPIO_SIG45_IN_SEL_V << GPIO_SIG45_IN_SEL_S)
#define GPIO_SIG45_IN_SEL_V  0x00000001U
#define GPIO_SIG45_IN_SEL_S  7

/** GPIO_FUNC47_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC47_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x214)
/** GPIO_FUNC47_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC47_IN_SEL    0x0000003FU
#define GPIO_FUNC47_IN_SEL_M  (GPIO_FUNC47_IN_SEL_V << GPIO_FUNC47_IN_SEL_S)
#define GPIO_FUNC47_IN_SEL_V  0x0000003FU
#define GPIO_FUNC47_IN_SEL_S  0
/** GPIO_FUNC47_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC47_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC47_IN_INV_SEL_M  (GPIO_FUNC47_IN_INV_SEL_V << GPIO_FUNC47_IN_INV_SEL_S)
#define GPIO_FUNC47_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_IN_INV_SEL_S  6
/** GPIO_SIG47_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG47_IN_SEL    (BIT(7))
#define GPIO_SIG47_IN_SEL_M  (GPIO_SIG47_IN_SEL_V << GPIO_SIG47_IN_SEL_S)
#define GPIO_SIG47_IN_SEL_V  0x00000001U
#define GPIO_SIG47_IN_SEL_S  7

/** GPIO_FUNC48_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC48_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x218)
/** GPIO_FUNC48_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC48_IN_SEL    0x0000003FU
#define GPIO_FUNC48_IN_SEL_M  (GPIO_FUNC48_IN_SEL_V << GPIO_FUNC48_IN_SEL_S)
#define GPIO_FUNC48_IN_SEL_V  0x0000003FU
#define GPIO_FUNC48_IN_SEL_S  0
/** GPIO_FUNC48_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC48_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC48_IN_INV_SEL_M  (GPIO_FUNC48_IN_INV_SEL_V << GPIO_FUNC48_IN_INV_SEL_S)
#define GPIO_FUNC48_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_IN_INV_SEL_S  6
/** GPIO_SIG48_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG48_IN_SEL    (BIT(7))
#define GPIO_SIG48_IN_SEL_M  (GPIO_SIG48_IN_SEL_V << GPIO_SIG48_IN_SEL_S)
#define GPIO_SIG48_IN_SEL_V  0x00000001U
#define GPIO_SIG48_IN_SEL_S  7

/** GPIO_FUNC49_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC49_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x21c)
/** GPIO_FUNC49_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC49_IN_SEL    0x0000003FU
#define GPIO_FUNC49_IN_SEL_M  (GPIO_FUNC49_IN_SEL_V << GPIO_FUNC49_IN_SEL_S)
#define GPIO_FUNC49_IN_SEL_V  0x0000003FU
#define GPIO_FUNC49_IN_SEL_S  0
/** GPIO_FUNC49_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC49_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC49_IN_INV_SEL_M  (GPIO_FUNC49_IN_INV_SEL_V << GPIO_FUNC49_IN_INV_SEL_S)
#define GPIO_FUNC49_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_IN_INV_SEL_S  6
/** GPIO_SIG49_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG49_IN_SEL    (BIT(7))
#define GPIO_SIG49_IN_SEL_M  (GPIO_SIG49_IN_SEL_V << GPIO_SIG49_IN_SEL_S)
#define GPIO_SIG49_IN_SEL_V  0x00000001U
#define GPIO_SIG49_IN_SEL_S  7

/** GPIO_FUNC50_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC50_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x220)
/** GPIO_FUNC50_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC50_IN_SEL    0x0000003FU
#define GPIO_FUNC50_IN_SEL_M  (GPIO_FUNC50_IN_SEL_V << GPIO_FUNC50_IN_SEL_S)
#define GPIO_FUNC50_IN_SEL_V  0x0000003FU
#define GPIO_FUNC50_IN_SEL_S  0
/** GPIO_FUNC50_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC50_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC50_IN_INV_SEL_M  (GPIO_FUNC50_IN_INV_SEL_V << GPIO_FUNC50_IN_INV_SEL_S)
#define GPIO_FUNC50_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_IN_INV_SEL_S  6
/** GPIO_SIG50_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG50_IN_SEL    (BIT(7))
#define GPIO_SIG50_IN_SEL_M  (GPIO_SIG50_IN_SEL_V << GPIO_SIG50_IN_SEL_S)
#define GPIO_SIG50_IN_SEL_V  0x00000001U
#define GPIO_SIG50_IN_SEL_S  7

/** GPIO_FUNC51_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC51_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x224)
/** GPIO_FUNC51_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC51_IN_SEL    0x0000003FU
#define GPIO_FUNC51_IN_SEL_M  (GPIO_FUNC51_IN_SEL_V << GPIO_FUNC51_IN_SEL_S)
#define GPIO_FUNC51_IN_SEL_V  0x0000003FU
#define GPIO_FUNC51_IN_SEL_S  0
/** GPIO_FUNC51_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC51_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC51_IN_INV_SEL_M  (GPIO_FUNC51_IN_INV_SEL_V << GPIO_FUNC51_IN_INV_SEL_S)
#define GPIO_FUNC51_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_IN_INV_SEL_S  6
/** GPIO_SIG51_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG51_IN_SEL    (BIT(7))
#define GPIO_SIG51_IN_SEL_M  (GPIO_SIG51_IN_SEL_V << GPIO_SIG51_IN_SEL_S)
#define GPIO_SIG51_IN_SEL_V  0x00000001U
#define GPIO_SIG51_IN_SEL_S  7

/** GPIO_FUNC52_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC52_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x228)
/** GPIO_FUNC52_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC52_IN_SEL    0x0000003FU
#define GPIO_FUNC52_IN_SEL_M  (GPIO_FUNC52_IN_SEL_V << GPIO_FUNC52_IN_SEL_S)
#define GPIO_FUNC52_IN_SEL_V  0x0000003FU
#define GPIO_FUNC52_IN_SEL_S  0
/** GPIO_FUNC52_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC52_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC52_IN_INV_SEL_M  (GPIO_FUNC52_IN_INV_SEL_V << GPIO_FUNC52_IN_INV_SEL_S)
#define GPIO_FUNC52_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_IN_INV_SEL_S  6
/** GPIO_SIG52_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG52_IN_SEL    (BIT(7))
#define GPIO_SIG52_IN_SEL_M  (GPIO_SIG52_IN_SEL_V << GPIO_SIG52_IN_SEL_S)
#define GPIO_SIG52_IN_SEL_V  0x00000001U
#define GPIO_SIG52_IN_SEL_S  7

/** GPIO_FUNC53_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC53_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x22c)
/** GPIO_FUNC53_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC53_IN_SEL    0x0000003FU
#define GPIO_FUNC53_IN_SEL_M  (GPIO_FUNC53_IN_SEL_V << GPIO_FUNC53_IN_SEL_S)
#define GPIO_FUNC53_IN_SEL_V  0x0000003FU
#define GPIO_FUNC53_IN_SEL_S  0
/** GPIO_FUNC53_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC53_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC53_IN_INV_SEL_M  (GPIO_FUNC53_IN_INV_SEL_V << GPIO_FUNC53_IN_INV_SEL_S)
#define GPIO_FUNC53_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_IN_INV_SEL_S  6
/** GPIO_SIG53_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG53_IN_SEL    (BIT(7))
#define GPIO_SIG53_IN_SEL_M  (GPIO_SIG53_IN_SEL_V << GPIO_SIG53_IN_SEL_S)
#define GPIO_SIG53_IN_SEL_V  0x00000001U
#define GPIO_SIG53_IN_SEL_S  7

/** GPIO_FUNC54_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC54_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x230)
/** GPIO_FUNC54_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC54_IN_SEL    0x0000003FU
#define GPIO_FUNC54_IN_SEL_M  (GPIO_FUNC54_IN_SEL_V << GPIO_FUNC54_IN_SEL_S)
#define GPIO_FUNC54_IN_SEL_V  0x0000003FU
#define GPIO_FUNC54_IN_SEL_S  0
/** GPIO_FUNC54_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC54_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC54_IN_INV_SEL_M  (GPIO_FUNC54_IN_INV_SEL_V << GPIO_FUNC54_IN_INV_SEL_S)
#define GPIO_FUNC54_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_IN_INV_SEL_S  6
/** GPIO_SIG54_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG54_IN_SEL    (BIT(7))
#define GPIO_SIG54_IN_SEL_M  (GPIO_SIG54_IN_SEL_V << GPIO_SIG54_IN_SEL_S)
#define GPIO_SIG54_IN_SEL_V  0x00000001U
#define GPIO_SIG54_IN_SEL_S  7

/** GPIO_FUNC55_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC55_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x234)
/** GPIO_FUNC55_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC55_IN_SEL    0x0000003FU
#define GPIO_FUNC55_IN_SEL_M  (GPIO_FUNC55_IN_SEL_V << GPIO_FUNC55_IN_SEL_S)
#define GPIO_FUNC55_IN_SEL_V  0x0000003FU
#define GPIO_FUNC55_IN_SEL_S  0
/** GPIO_FUNC55_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC55_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC55_IN_INV_SEL_M  (GPIO_FUNC55_IN_INV_SEL_V << GPIO_FUNC55_IN_INV_SEL_S)
#define GPIO_FUNC55_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_IN_INV_SEL_S  6
/** GPIO_SIG55_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG55_IN_SEL    (BIT(7))
#define GPIO_SIG55_IN_SEL_M  (GPIO_SIG55_IN_SEL_V << GPIO_SIG55_IN_SEL_S)
#define GPIO_SIG55_IN_SEL_V  0x00000001U
#define GPIO_SIG55_IN_SEL_S  7

/** GPIO_FUNC56_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC56_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x238)
/** GPIO_FUNC56_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC56_IN_SEL    0x0000003FU
#define GPIO_FUNC56_IN_SEL_M  (GPIO_FUNC56_IN_SEL_V << GPIO_FUNC56_IN_SEL_S)
#define GPIO_FUNC56_IN_SEL_V  0x0000003FU
#define GPIO_FUNC56_IN_SEL_S  0
/** GPIO_FUNC56_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC56_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC56_IN_INV_SEL_M  (GPIO_FUNC56_IN_INV_SEL_V << GPIO_FUNC56_IN_INV_SEL_S)
#define GPIO_FUNC56_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_IN_INV_SEL_S  6
/** GPIO_SIG56_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG56_IN_SEL    (BIT(7))
#define GPIO_SIG56_IN_SEL_M  (GPIO_SIG56_IN_SEL_V << GPIO_SIG56_IN_SEL_S)
#define GPIO_SIG56_IN_SEL_V  0x00000001U
#define GPIO_SIG56_IN_SEL_S  7

/** GPIO_FUNC57_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC57_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x23c)
/** GPIO_FUNC57_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC57_IN_SEL    0x0000003FU
#define GPIO_FUNC57_IN_SEL_M  (GPIO_FUNC57_IN_SEL_V << GPIO_FUNC57_IN_SEL_S)
#define GPIO_FUNC57_IN_SEL_V  0x0000003FU
#define GPIO_FUNC57_IN_SEL_S  0
/** GPIO_FUNC57_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC57_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC57_IN_INV_SEL_M  (GPIO_FUNC57_IN_INV_SEL_V << GPIO_FUNC57_IN_INV_SEL_S)
#define GPIO_FUNC57_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC57_IN_INV_SEL_S  6
/** GPIO_SIG57_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG57_IN_SEL    (BIT(7))
#define GPIO_SIG57_IN_SEL_M  (GPIO_SIG57_IN_SEL_V << GPIO_SIG57_IN_SEL_S)
#define GPIO_SIG57_IN_SEL_V  0x00000001U
#define GPIO_SIG57_IN_SEL_S  7

/** GPIO_FUNC58_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC58_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x240)
/** GPIO_FUNC58_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC58_IN_SEL    0x0000003FU
#define GPIO_FUNC58_IN_SEL_M  (GPIO_FUNC58_IN_SEL_V << GPIO_FUNC58_IN_SEL_S)
#define GPIO_FUNC58_IN_SEL_V  0x0000003FU
#define GPIO_FUNC58_IN_SEL_S  0
/** GPIO_FUNC58_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC58_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC58_IN_INV_SEL_M  (GPIO_FUNC58_IN_INV_SEL_V << GPIO_FUNC58_IN_INV_SEL_S)
#define GPIO_FUNC58_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC58_IN_INV_SEL_S  6
/** GPIO_SIG58_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG58_IN_SEL    (BIT(7))
#define GPIO_SIG58_IN_SEL_M  (GPIO_SIG58_IN_SEL_V << GPIO_SIG58_IN_SEL_S)
#define GPIO_SIG58_IN_SEL_V  0x00000001U
#define GPIO_SIG58_IN_SEL_S  7

/** GPIO_FUNC59_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC59_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x244)
/** GPIO_FUNC59_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC59_IN_SEL    0x0000003FU
#define GPIO_FUNC59_IN_SEL_M  (GPIO_FUNC59_IN_SEL_V << GPIO_FUNC59_IN_SEL_S)
#define GPIO_FUNC59_IN_SEL_V  0x0000003FU
#define GPIO_FUNC59_IN_SEL_S  0
/** GPIO_FUNC59_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC59_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC59_IN_INV_SEL_M  (GPIO_FUNC59_IN_INV_SEL_V << GPIO_FUNC59_IN_INV_SEL_S)
#define GPIO_FUNC59_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC59_IN_INV_SEL_S  6
/** GPIO_SIG59_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG59_IN_SEL    (BIT(7))
#define GPIO_SIG59_IN_SEL_M  (GPIO_SIG59_IN_SEL_V << GPIO_SIG59_IN_SEL_S)
#define GPIO_SIG59_IN_SEL_V  0x00000001U
#define GPIO_SIG59_IN_SEL_S  7

/** GPIO_FUNC60_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC60_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x248)
/** GPIO_FUNC60_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC60_IN_SEL    0x0000003FU
#define GPIO_FUNC60_IN_SEL_M  (GPIO_FUNC60_IN_SEL_V << GPIO_FUNC60_IN_SEL_S)
#define GPIO_FUNC60_IN_SEL_V  0x0000003FU
#define GPIO_FUNC60_IN_SEL_S  0
/** GPIO_FUNC60_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC60_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC60_IN_INV_SEL_M  (GPIO_FUNC60_IN_INV_SEL_V << GPIO_FUNC60_IN_INV_SEL_S)
#define GPIO_FUNC60_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC60_IN_INV_SEL_S  6
/** GPIO_SIG60_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG60_IN_SEL    (BIT(7))
#define GPIO_SIG60_IN_SEL_M  (GPIO_SIG60_IN_SEL_V << GPIO_SIG60_IN_SEL_S)
#define GPIO_SIG60_IN_SEL_V  0x00000001U
#define GPIO_SIG60_IN_SEL_S  7

/** GPIO_FUNC61_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC61_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x24c)
/** GPIO_FUNC61_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC61_IN_SEL    0x0000003FU
#define GPIO_FUNC61_IN_SEL_M  (GPIO_FUNC61_IN_SEL_V << GPIO_FUNC61_IN_SEL_S)
#define GPIO_FUNC61_IN_SEL_V  0x0000003FU
#define GPIO_FUNC61_IN_SEL_S  0
/** GPIO_FUNC61_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC61_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC61_IN_INV_SEL_M  (GPIO_FUNC61_IN_INV_SEL_V << GPIO_FUNC61_IN_INV_SEL_S)
#define GPIO_FUNC61_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC61_IN_INV_SEL_S  6
/** GPIO_SIG61_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG61_IN_SEL    (BIT(7))
#define GPIO_SIG61_IN_SEL_M  (GPIO_SIG61_IN_SEL_V << GPIO_SIG61_IN_SEL_S)
#define GPIO_SIG61_IN_SEL_V  0x00000001U
#define GPIO_SIG61_IN_SEL_S  7

/** GPIO_FUNC62_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC62_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x250)
/** GPIO_FUNC62_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC62_IN_SEL    0x0000003FU
#define GPIO_FUNC62_IN_SEL_M  (GPIO_FUNC62_IN_SEL_V << GPIO_FUNC62_IN_SEL_S)
#define GPIO_FUNC62_IN_SEL_V  0x0000003FU
#define GPIO_FUNC62_IN_SEL_S  0
/** GPIO_FUNC62_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC62_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC62_IN_INV_SEL_M  (GPIO_FUNC62_IN_INV_SEL_V << GPIO_FUNC62_IN_INV_SEL_S)
#define GPIO_FUNC62_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC62_IN_INV_SEL_S  6
/** GPIO_SIG62_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG62_IN_SEL    (BIT(7))
#define GPIO_SIG62_IN_SEL_M  (GPIO_SIG62_IN_SEL_V << GPIO_SIG62_IN_SEL_S)
#define GPIO_SIG62_IN_SEL_V  0x00000001U
#define GPIO_SIG62_IN_SEL_S  7

/** GPIO_FUNC63_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC63_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x254)
/** GPIO_FUNC63_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC63_IN_SEL    0x0000003FU
#define GPIO_FUNC63_IN_SEL_M  (GPIO_FUNC63_IN_SEL_V << GPIO_FUNC63_IN_SEL_S)
#define GPIO_FUNC63_IN_SEL_V  0x0000003FU
#define GPIO_FUNC63_IN_SEL_S  0
/** GPIO_FUNC63_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC63_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC63_IN_INV_SEL_M  (GPIO_FUNC63_IN_INV_SEL_V << GPIO_FUNC63_IN_INV_SEL_S)
#define GPIO_FUNC63_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC63_IN_INV_SEL_S  6
/** GPIO_SIG63_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG63_IN_SEL    (BIT(7))
#define GPIO_SIG63_IN_SEL_M  (GPIO_SIG63_IN_SEL_V << GPIO_SIG63_IN_SEL_S)
#define GPIO_SIG63_IN_SEL_V  0x00000001U
#define GPIO_SIG63_IN_SEL_S  7

/** GPIO_FUNC64_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC64_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x258)
/** GPIO_FUNC64_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC64_IN_SEL    0x0000003FU
#define GPIO_FUNC64_IN_SEL_M  (GPIO_FUNC64_IN_SEL_V << GPIO_FUNC64_IN_SEL_S)
#define GPIO_FUNC64_IN_SEL_V  0x0000003FU
#define GPIO_FUNC64_IN_SEL_S  0
/** GPIO_FUNC64_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC64_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC64_IN_INV_SEL_M  (GPIO_FUNC64_IN_INV_SEL_V << GPIO_FUNC64_IN_INV_SEL_S)
#define GPIO_FUNC64_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC64_IN_INV_SEL_S  6
/** GPIO_SIG64_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG64_IN_SEL    (BIT(7))
#define GPIO_SIG64_IN_SEL_M  (GPIO_SIG64_IN_SEL_V << GPIO_SIG64_IN_SEL_S)
#define GPIO_SIG64_IN_SEL_V  0x00000001U
#define GPIO_SIG64_IN_SEL_S  7

/** GPIO_FUNC65_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC65_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x25c)
/** GPIO_FUNC65_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC65_IN_SEL    0x0000003FU
#define GPIO_FUNC65_IN_SEL_M  (GPIO_FUNC65_IN_SEL_V << GPIO_FUNC65_IN_SEL_S)
#define GPIO_FUNC65_IN_SEL_V  0x0000003FU
#define GPIO_FUNC65_IN_SEL_S  0
/** GPIO_FUNC65_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC65_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC65_IN_INV_SEL_M  (GPIO_FUNC65_IN_INV_SEL_V << GPIO_FUNC65_IN_INV_SEL_S)
#define GPIO_FUNC65_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC65_IN_INV_SEL_S  6
/** GPIO_SIG65_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG65_IN_SEL    (BIT(7))
#define GPIO_SIG65_IN_SEL_M  (GPIO_SIG65_IN_SEL_V << GPIO_SIG65_IN_SEL_S)
#define GPIO_SIG65_IN_SEL_V  0x00000001U
#define GPIO_SIG65_IN_SEL_S  7

/** GPIO_FUNC66_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC66_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x260)
/** GPIO_FUNC66_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC66_IN_SEL    0x0000003FU
#define GPIO_FUNC66_IN_SEL_M  (GPIO_FUNC66_IN_SEL_V << GPIO_FUNC66_IN_SEL_S)
#define GPIO_FUNC66_IN_SEL_V  0x0000003FU
#define GPIO_FUNC66_IN_SEL_S  0
/** GPIO_FUNC66_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC66_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC66_IN_INV_SEL_M  (GPIO_FUNC66_IN_INV_SEL_V << GPIO_FUNC66_IN_INV_SEL_S)
#define GPIO_FUNC66_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC66_IN_INV_SEL_S  6
/** GPIO_SIG66_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG66_IN_SEL    (BIT(7))
#define GPIO_SIG66_IN_SEL_M  (GPIO_SIG66_IN_SEL_V << GPIO_SIG66_IN_SEL_S)
#define GPIO_SIG66_IN_SEL_V  0x00000001U
#define GPIO_SIG66_IN_SEL_S  7

/** GPIO_FUNC68_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC68_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x268)
/** GPIO_FUNC68_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC68_IN_SEL    0x0000003FU
#define GPIO_FUNC68_IN_SEL_M  (GPIO_FUNC68_IN_SEL_V << GPIO_FUNC68_IN_SEL_S)
#define GPIO_FUNC68_IN_SEL_V  0x0000003FU
#define GPIO_FUNC68_IN_SEL_S  0
/** GPIO_FUNC68_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC68_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC68_IN_INV_SEL_M  (GPIO_FUNC68_IN_INV_SEL_V << GPIO_FUNC68_IN_INV_SEL_S)
#define GPIO_FUNC68_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC68_IN_INV_SEL_S  6
/** GPIO_SIG68_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG68_IN_SEL    (BIT(7))
#define GPIO_SIG68_IN_SEL_M  (GPIO_SIG68_IN_SEL_V << GPIO_SIG68_IN_SEL_S)
#define GPIO_SIG68_IN_SEL_V  0x00000001U
#define GPIO_SIG68_IN_SEL_S  7

/** GPIO_FUNC69_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC69_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x26c)
/** GPIO_FUNC69_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC69_IN_SEL    0x0000003FU
#define GPIO_FUNC69_IN_SEL_M  (GPIO_FUNC69_IN_SEL_V << GPIO_FUNC69_IN_SEL_S)
#define GPIO_FUNC69_IN_SEL_V  0x0000003FU
#define GPIO_FUNC69_IN_SEL_S  0
/** GPIO_FUNC69_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC69_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC69_IN_INV_SEL_M  (GPIO_FUNC69_IN_INV_SEL_V << GPIO_FUNC69_IN_INV_SEL_S)
#define GPIO_FUNC69_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC69_IN_INV_SEL_S  6
/** GPIO_SIG69_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG69_IN_SEL    (BIT(7))
#define GPIO_SIG69_IN_SEL_M  (GPIO_SIG69_IN_SEL_V << GPIO_SIG69_IN_SEL_S)
#define GPIO_SIG69_IN_SEL_V  0x00000001U
#define GPIO_SIG69_IN_SEL_S  7

/** GPIO_FUNC70_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC70_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x270)
/** GPIO_FUNC70_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC70_IN_SEL    0x0000003FU
#define GPIO_FUNC70_IN_SEL_M  (GPIO_FUNC70_IN_SEL_V << GPIO_FUNC70_IN_SEL_S)
#define GPIO_FUNC70_IN_SEL_V  0x0000003FU
#define GPIO_FUNC70_IN_SEL_S  0
/** GPIO_FUNC70_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC70_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC70_IN_INV_SEL_M  (GPIO_FUNC70_IN_INV_SEL_V << GPIO_FUNC70_IN_INV_SEL_S)
#define GPIO_FUNC70_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC70_IN_INV_SEL_S  6
/** GPIO_SIG70_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG70_IN_SEL    (BIT(7))
#define GPIO_SIG70_IN_SEL_M  (GPIO_SIG70_IN_SEL_V << GPIO_SIG70_IN_SEL_S)
#define GPIO_SIG70_IN_SEL_V  0x00000001U
#define GPIO_SIG70_IN_SEL_S  7

/** GPIO_FUNC71_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC71_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x274)
/** GPIO_FUNC71_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC71_IN_SEL    0x0000003FU
#define GPIO_FUNC71_IN_SEL_M  (GPIO_FUNC71_IN_SEL_V << GPIO_FUNC71_IN_SEL_S)
#define GPIO_FUNC71_IN_SEL_V  0x0000003FU
#define GPIO_FUNC71_IN_SEL_S  0
/** GPIO_FUNC71_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC71_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC71_IN_INV_SEL_M  (GPIO_FUNC71_IN_INV_SEL_V << GPIO_FUNC71_IN_INV_SEL_S)
#define GPIO_FUNC71_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC71_IN_INV_SEL_S  6
/** GPIO_SIG71_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG71_IN_SEL    (BIT(7))
#define GPIO_SIG71_IN_SEL_M  (GPIO_SIG71_IN_SEL_V << GPIO_SIG71_IN_SEL_S)
#define GPIO_SIG71_IN_SEL_V  0x00000001U
#define GPIO_SIG71_IN_SEL_S  7

/** GPIO_FUNC74_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC74_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x280)
/** GPIO_FUNC74_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC74_IN_SEL    0x0000003FU
#define GPIO_FUNC74_IN_SEL_M  (GPIO_FUNC74_IN_SEL_V << GPIO_FUNC74_IN_SEL_S)
#define GPIO_FUNC74_IN_SEL_V  0x0000003FU
#define GPIO_FUNC74_IN_SEL_S  0
/** GPIO_FUNC74_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC74_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC74_IN_INV_SEL_M  (GPIO_FUNC74_IN_INV_SEL_V << GPIO_FUNC74_IN_INV_SEL_S)
#define GPIO_FUNC74_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC74_IN_INV_SEL_S  6
/** GPIO_SIG74_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG74_IN_SEL    (BIT(7))
#define GPIO_SIG74_IN_SEL_M  (GPIO_SIG74_IN_SEL_V << GPIO_SIG74_IN_SEL_S)
#define GPIO_SIG74_IN_SEL_V  0x00000001U
#define GPIO_SIG74_IN_SEL_S  7

/** GPIO_FUNC75_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC75_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x284)
/** GPIO_FUNC75_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC75_IN_SEL    0x0000003FU
#define GPIO_FUNC75_IN_SEL_M  (GPIO_FUNC75_IN_SEL_V << GPIO_FUNC75_IN_SEL_S)
#define GPIO_FUNC75_IN_SEL_V  0x0000003FU
#define GPIO_FUNC75_IN_SEL_S  0
/** GPIO_FUNC75_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC75_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC75_IN_INV_SEL_M  (GPIO_FUNC75_IN_INV_SEL_V << GPIO_FUNC75_IN_INV_SEL_S)
#define GPIO_FUNC75_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC75_IN_INV_SEL_S  6
/** GPIO_SIG75_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG75_IN_SEL    (BIT(7))
#define GPIO_SIG75_IN_SEL_M  (GPIO_SIG75_IN_SEL_V << GPIO_SIG75_IN_SEL_S)
#define GPIO_SIG75_IN_SEL_V  0x00000001U
#define GPIO_SIG75_IN_SEL_S  7

/** GPIO_FUNC76_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC76_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x288)
/** GPIO_FUNC76_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC76_IN_SEL    0x0000003FU
#define GPIO_FUNC76_IN_SEL_M  (GPIO_FUNC76_IN_SEL_V << GPIO_FUNC76_IN_SEL_S)
#define GPIO_FUNC76_IN_SEL_V  0x0000003FU
#define GPIO_FUNC76_IN_SEL_S  0
/** GPIO_FUNC76_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC76_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC76_IN_INV_SEL_M  (GPIO_FUNC76_IN_INV_SEL_V << GPIO_FUNC76_IN_INV_SEL_S)
#define GPIO_FUNC76_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC76_IN_INV_SEL_S  6
/** GPIO_SIG76_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG76_IN_SEL    (BIT(7))
#define GPIO_SIG76_IN_SEL_M  (GPIO_SIG76_IN_SEL_V << GPIO_SIG76_IN_SEL_S)
#define GPIO_SIG76_IN_SEL_V  0x00000001U
#define GPIO_SIG76_IN_SEL_S  7

/** GPIO_FUNC77_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC77_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x28c)
/** GPIO_FUNC77_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC77_IN_SEL    0x0000003FU
#define GPIO_FUNC77_IN_SEL_M  (GPIO_FUNC77_IN_SEL_V << GPIO_FUNC77_IN_SEL_S)
#define GPIO_FUNC77_IN_SEL_V  0x0000003FU
#define GPIO_FUNC77_IN_SEL_S  0
/** GPIO_FUNC77_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC77_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC77_IN_INV_SEL_M  (GPIO_FUNC77_IN_INV_SEL_V << GPIO_FUNC77_IN_INV_SEL_S)
#define GPIO_FUNC77_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC77_IN_INV_SEL_S  6
/** GPIO_SIG77_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG77_IN_SEL    (BIT(7))
#define GPIO_SIG77_IN_SEL_M  (GPIO_SIG77_IN_SEL_V << GPIO_SIG77_IN_SEL_S)
#define GPIO_SIG77_IN_SEL_V  0x00000001U
#define GPIO_SIG77_IN_SEL_S  7

/** GPIO_FUNC78_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC78_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x290)
/** GPIO_FUNC78_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC78_IN_SEL    0x0000003FU
#define GPIO_FUNC78_IN_SEL_M  (GPIO_FUNC78_IN_SEL_V << GPIO_FUNC78_IN_SEL_S)
#define GPIO_FUNC78_IN_SEL_V  0x0000003FU
#define GPIO_FUNC78_IN_SEL_S  0
/** GPIO_FUNC78_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC78_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC78_IN_INV_SEL_M  (GPIO_FUNC78_IN_INV_SEL_V << GPIO_FUNC78_IN_INV_SEL_S)
#define GPIO_FUNC78_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC78_IN_INV_SEL_S  6
/** GPIO_SIG78_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG78_IN_SEL    (BIT(7))
#define GPIO_SIG78_IN_SEL_M  (GPIO_SIG78_IN_SEL_V << GPIO_SIG78_IN_SEL_S)
#define GPIO_SIG78_IN_SEL_V  0x00000001U
#define GPIO_SIG78_IN_SEL_S  7

/** GPIO_FUNC80_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC80_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x298)
/** GPIO_FUNC80_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC80_IN_SEL    0x0000003FU
#define GPIO_FUNC80_IN_SEL_M  (GPIO_FUNC80_IN_SEL_V << GPIO_FUNC80_IN_SEL_S)
#define GPIO_FUNC80_IN_SEL_V  0x0000003FU
#define GPIO_FUNC80_IN_SEL_S  0
/** GPIO_FUNC80_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC80_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC80_IN_INV_SEL_M  (GPIO_FUNC80_IN_INV_SEL_V << GPIO_FUNC80_IN_INV_SEL_S)
#define GPIO_FUNC80_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC80_IN_INV_SEL_S  6
/** GPIO_SIG80_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG80_IN_SEL    (BIT(7))
#define GPIO_SIG80_IN_SEL_M  (GPIO_SIG80_IN_SEL_V << GPIO_SIG80_IN_SEL_S)
#define GPIO_SIG80_IN_SEL_V  0x00000001U
#define GPIO_SIG80_IN_SEL_S  7

/** GPIO_FUNC83_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC83_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2a4)
/** GPIO_FUNC83_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC83_IN_SEL    0x0000003FU
#define GPIO_FUNC83_IN_SEL_M  (GPIO_FUNC83_IN_SEL_V << GPIO_FUNC83_IN_SEL_S)
#define GPIO_FUNC83_IN_SEL_V  0x0000003FU
#define GPIO_FUNC83_IN_SEL_S  0
/** GPIO_FUNC83_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC83_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC83_IN_INV_SEL_M  (GPIO_FUNC83_IN_INV_SEL_V << GPIO_FUNC83_IN_INV_SEL_S)
#define GPIO_FUNC83_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC83_IN_INV_SEL_S  6
/** GPIO_SIG83_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG83_IN_SEL    (BIT(7))
#define GPIO_SIG83_IN_SEL_M  (GPIO_SIG83_IN_SEL_V << GPIO_SIG83_IN_SEL_S)
#define GPIO_SIG83_IN_SEL_V  0x00000001U
#define GPIO_SIG83_IN_SEL_S  7

/** GPIO_FUNC86_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC86_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2b0)
/** GPIO_FUNC86_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC86_IN_SEL    0x0000003FU
#define GPIO_FUNC86_IN_SEL_M  (GPIO_FUNC86_IN_SEL_V << GPIO_FUNC86_IN_SEL_S)
#define GPIO_FUNC86_IN_SEL_V  0x0000003FU
#define GPIO_FUNC86_IN_SEL_S  0
/** GPIO_FUNC86_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC86_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC86_IN_INV_SEL_M  (GPIO_FUNC86_IN_INV_SEL_V << GPIO_FUNC86_IN_INV_SEL_S)
#define GPIO_FUNC86_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC86_IN_INV_SEL_S  6
/** GPIO_SIG86_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG86_IN_SEL    (BIT(7))
#define GPIO_SIG86_IN_SEL_M  (GPIO_SIG86_IN_SEL_V << GPIO_SIG86_IN_SEL_S)
#define GPIO_SIG86_IN_SEL_V  0x00000001U
#define GPIO_SIG86_IN_SEL_S  7

/** GPIO_FUNC89_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC89_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2bc)
/** GPIO_FUNC89_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC89_IN_SEL    0x0000003FU
#define GPIO_FUNC89_IN_SEL_M  (GPIO_FUNC89_IN_SEL_V << GPIO_FUNC89_IN_SEL_S)
#define GPIO_FUNC89_IN_SEL_V  0x0000003FU
#define GPIO_FUNC89_IN_SEL_S  0
/** GPIO_FUNC89_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC89_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC89_IN_INV_SEL_M  (GPIO_FUNC89_IN_INV_SEL_V << GPIO_FUNC89_IN_INV_SEL_S)
#define GPIO_FUNC89_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC89_IN_INV_SEL_S  6
/** GPIO_SIG89_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG89_IN_SEL    (BIT(7))
#define GPIO_SIG89_IN_SEL_M  (GPIO_SIG89_IN_SEL_V << GPIO_SIG89_IN_SEL_S)
#define GPIO_SIG89_IN_SEL_V  0x00000001U
#define GPIO_SIG89_IN_SEL_S  7

/** GPIO_FUNC90_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC90_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c0)
/** GPIO_FUNC90_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC90_IN_SEL    0x0000003FU
#define GPIO_FUNC90_IN_SEL_M  (GPIO_FUNC90_IN_SEL_V << GPIO_FUNC90_IN_SEL_S)
#define GPIO_FUNC90_IN_SEL_V  0x0000003FU
#define GPIO_FUNC90_IN_SEL_S  0
/** GPIO_FUNC90_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC90_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC90_IN_INV_SEL_M  (GPIO_FUNC90_IN_INV_SEL_V << GPIO_FUNC90_IN_INV_SEL_S)
#define GPIO_FUNC90_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC90_IN_INV_SEL_S  6
/** GPIO_SIG90_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG90_IN_SEL    (BIT(7))
#define GPIO_SIG90_IN_SEL_M  (GPIO_SIG90_IN_SEL_V << GPIO_SIG90_IN_SEL_S)
#define GPIO_SIG90_IN_SEL_V  0x00000001U
#define GPIO_SIG90_IN_SEL_S  7

/** GPIO_FUNC91_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC91_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c4)
/** GPIO_FUNC91_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC91_IN_SEL    0x0000003FU
#define GPIO_FUNC91_IN_SEL_M  (GPIO_FUNC91_IN_SEL_V << GPIO_FUNC91_IN_SEL_S)
#define GPIO_FUNC91_IN_SEL_V  0x0000003FU
#define GPIO_FUNC91_IN_SEL_S  0
/** GPIO_FUNC91_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC91_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC91_IN_INV_SEL_M  (GPIO_FUNC91_IN_INV_SEL_V << GPIO_FUNC91_IN_INV_SEL_S)
#define GPIO_FUNC91_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC91_IN_INV_SEL_S  6
/** GPIO_SIG91_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG91_IN_SEL    (BIT(7))
#define GPIO_SIG91_IN_SEL_M  (GPIO_SIG91_IN_SEL_V << GPIO_SIG91_IN_SEL_S)
#define GPIO_SIG91_IN_SEL_V  0x00000001U
#define GPIO_SIG91_IN_SEL_S  7

/** GPIO_FUNC92_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC92_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c8)
/** GPIO_FUNC92_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC92_IN_SEL    0x0000003FU
#define GPIO_FUNC92_IN_SEL_M  (GPIO_FUNC92_IN_SEL_V << GPIO_FUNC92_IN_SEL_S)
#define GPIO_FUNC92_IN_SEL_V  0x0000003FU
#define GPIO_FUNC92_IN_SEL_S  0
/** GPIO_FUNC92_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC92_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC92_IN_INV_SEL_M  (GPIO_FUNC92_IN_INV_SEL_V << GPIO_FUNC92_IN_INV_SEL_S)
#define GPIO_FUNC92_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC92_IN_INV_SEL_S  6
/** GPIO_SIG92_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG92_IN_SEL    (BIT(7))
#define GPIO_SIG92_IN_SEL_M  (GPIO_SIG92_IN_SEL_V << GPIO_SIG92_IN_SEL_S)
#define GPIO_SIG92_IN_SEL_V  0x00000001U
#define GPIO_SIG92_IN_SEL_S  7

/** GPIO_FUNC93_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC93_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2cc)
/** GPIO_FUNC93_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC93_IN_SEL    0x0000003FU
#define GPIO_FUNC93_IN_SEL_M  (GPIO_FUNC93_IN_SEL_V << GPIO_FUNC93_IN_SEL_S)
#define GPIO_FUNC93_IN_SEL_V  0x0000003FU
#define GPIO_FUNC93_IN_SEL_S  0
/** GPIO_FUNC93_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC93_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC93_IN_INV_SEL_M  (GPIO_FUNC93_IN_INV_SEL_V << GPIO_FUNC93_IN_INV_SEL_S)
#define GPIO_FUNC93_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC93_IN_INV_SEL_S  6
/** GPIO_SIG93_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG93_IN_SEL    (BIT(7))
#define GPIO_SIG93_IN_SEL_M  (GPIO_SIG93_IN_SEL_V << GPIO_SIG93_IN_SEL_S)
#define GPIO_SIG93_IN_SEL_V  0x00000001U
#define GPIO_SIG93_IN_SEL_S  7

/** GPIO_FUNC94_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC94_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2d0)
/** GPIO_FUNC94_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC94_IN_SEL    0x0000003FU
#define GPIO_FUNC94_IN_SEL_M  (GPIO_FUNC94_IN_SEL_V << GPIO_FUNC94_IN_SEL_S)
#define GPIO_FUNC94_IN_SEL_V  0x0000003FU
#define GPIO_FUNC94_IN_SEL_S  0
/** GPIO_FUNC94_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC94_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC94_IN_INV_SEL_M  (GPIO_FUNC94_IN_INV_SEL_V << GPIO_FUNC94_IN_INV_SEL_S)
#define GPIO_FUNC94_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC94_IN_INV_SEL_S  6
/** GPIO_SIG94_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG94_IN_SEL    (BIT(7))
#define GPIO_SIG94_IN_SEL_M  (GPIO_SIG94_IN_SEL_V << GPIO_SIG94_IN_SEL_S)
#define GPIO_SIG94_IN_SEL_V  0x00000001U
#define GPIO_SIG94_IN_SEL_S  7

/** GPIO_FUNC95_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC95_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2d4)
/** GPIO_FUNC95_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC95_IN_SEL    0x0000003FU
#define GPIO_FUNC95_IN_SEL_M  (GPIO_FUNC95_IN_SEL_V << GPIO_FUNC95_IN_SEL_S)
#define GPIO_FUNC95_IN_SEL_V  0x0000003FU
#define GPIO_FUNC95_IN_SEL_S  0
/** GPIO_FUNC95_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC95_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC95_IN_INV_SEL_M  (GPIO_FUNC95_IN_INV_SEL_V << GPIO_FUNC95_IN_INV_SEL_S)
#define GPIO_FUNC95_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC95_IN_INV_SEL_S  6
/** GPIO_SIG95_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG95_IN_SEL    (BIT(7))
#define GPIO_SIG95_IN_SEL_M  (GPIO_SIG95_IN_SEL_V << GPIO_SIG95_IN_SEL_S)
#define GPIO_SIG95_IN_SEL_V  0x00000001U
#define GPIO_SIG95_IN_SEL_S  7

/** GPIO_FUNC96_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC96_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2d8)
/** GPIO_FUNC96_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC96_IN_SEL    0x0000003FU
#define GPIO_FUNC96_IN_SEL_M  (GPIO_FUNC96_IN_SEL_V << GPIO_FUNC96_IN_SEL_S)
#define GPIO_FUNC96_IN_SEL_V  0x0000003FU
#define GPIO_FUNC96_IN_SEL_S  0
/** GPIO_FUNC96_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC96_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC96_IN_INV_SEL_M  (GPIO_FUNC96_IN_INV_SEL_V << GPIO_FUNC96_IN_INV_SEL_S)
#define GPIO_FUNC96_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC96_IN_INV_SEL_S  6
/** GPIO_SIG96_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG96_IN_SEL    (BIT(7))
#define GPIO_SIG96_IN_SEL_M  (GPIO_SIG96_IN_SEL_V << GPIO_SIG96_IN_SEL_S)
#define GPIO_SIG96_IN_SEL_V  0x00000001U
#define GPIO_SIG96_IN_SEL_S  7

/** GPIO_FUNC97_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC97_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2dc)
/** GPIO_FUNC97_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC97_IN_SEL    0x0000003FU
#define GPIO_FUNC97_IN_SEL_M  (GPIO_FUNC97_IN_SEL_V << GPIO_FUNC97_IN_SEL_S)
#define GPIO_FUNC97_IN_SEL_V  0x0000003FU
#define GPIO_FUNC97_IN_SEL_S  0
/** GPIO_FUNC97_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC97_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC97_IN_INV_SEL_M  (GPIO_FUNC97_IN_INV_SEL_V << GPIO_FUNC97_IN_INV_SEL_S)
#define GPIO_FUNC97_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC97_IN_INV_SEL_S  6
/** GPIO_SIG97_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG97_IN_SEL    (BIT(7))
#define GPIO_SIG97_IN_SEL_M  (GPIO_SIG97_IN_SEL_V << GPIO_SIG97_IN_SEL_S)
#define GPIO_SIG97_IN_SEL_V  0x00000001U
#define GPIO_SIG97_IN_SEL_S  7

/** GPIO_FUNC98_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC98_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e0)
/** GPIO_FUNC98_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC98_IN_SEL    0x0000003FU
#define GPIO_FUNC98_IN_SEL_M  (GPIO_FUNC98_IN_SEL_V << GPIO_FUNC98_IN_SEL_S)
#define GPIO_FUNC98_IN_SEL_V  0x0000003FU
#define GPIO_FUNC98_IN_SEL_S  0
/** GPIO_FUNC98_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC98_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC98_IN_INV_SEL_M  (GPIO_FUNC98_IN_INV_SEL_V << GPIO_FUNC98_IN_INV_SEL_S)
#define GPIO_FUNC98_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC98_IN_INV_SEL_S  6
/** GPIO_SIG98_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG98_IN_SEL    (BIT(7))
#define GPIO_SIG98_IN_SEL_M  (GPIO_SIG98_IN_SEL_V << GPIO_SIG98_IN_SEL_S)
#define GPIO_SIG98_IN_SEL_V  0x00000001U
#define GPIO_SIG98_IN_SEL_S  7

/** GPIO_FUNC99_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC99_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e4)
/** GPIO_FUNC99_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC99_IN_SEL    0x0000003FU
#define GPIO_FUNC99_IN_SEL_M  (GPIO_FUNC99_IN_SEL_V << GPIO_FUNC99_IN_SEL_S)
#define GPIO_FUNC99_IN_SEL_V  0x0000003FU
#define GPIO_FUNC99_IN_SEL_S  0
/** GPIO_FUNC99_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC99_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC99_IN_INV_SEL_M  (GPIO_FUNC99_IN_INV_SEL_V << GPIO_FUNC99_IN_INV_SEL_S)
#define GPIO_FUNC99_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC99_IN_INV_SEL_S  6
/** GPIO_SIG99_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG99_IN_SEL    (BIT(7))
#define GPIO_SIG99_IN_SEL_M  (GPIO_SIG99_IN_SEL_V << GPIO_SIG99_IN_SEL_S)
#define GPIO_SIG99_IN_SEL_V  0x00000001U
#define GPIO_SIG99_IN_SEL_S  7

/** GPIO_FUNC100_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC100_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e8)
/** GPIO_FUNC100_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC100_IN_SEL    0x0000003FU
#define GPIO_FUNC100_IN_SEL_M  (GPIO_FUNC100_IN_SEL_V << GPIO_FUNC100_IN_SEL_S)
#define GPIO_FUNC100_IN_SEL_V  0x0000003FU
#define GPIO_FUNC100_IN_SEL_S  0
/** GPIO_FUNC100_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC100_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC100_IN_INV_SEL_M  (GPIO_FUNC100_IN_INV_SEL_V << GPIO_FUNC100_IN_INV_SEL_S)
#define GPIO_FUNC100_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC100_IN_INV_SEL_S  6
/** GPIO_SIG100_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG100_IN_SEL    (BIT(7))
#define GPIO_SIG100_IN_SEL_M  (GPIO_SIG100_IN_SEL_V << GPIO_SIG100_IN_SEL_S)
#define GPIO_SIG100_IN_SEL_V  0x00000001U
#define GPIO_SIG100_IN_SEL_S  7

/** GPIO_FUNC101_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC101_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2ec)
/** GPIO_FUNC101_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC101_IN_SEL    0x0000003FU
#define GPIO_FUNC101_IN_SEL_M  (GPIO_FUNC101_IN_SEL_V << GPIO_FUNC101_IN_SEL_S)
#define GPIO_FUNC101_IN_SEL_V  0x0000003FU
#define GPIO_FUNC101_IN_SEL_S  0
/** GPIO_FUNC101_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC101_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC101_IN_INV_SEL_M  (GPIO_FUNC101_IN_INV_SEL_V << GPIO_FUNC101_IN_INV_SEL_S)
#define GPIO_FUNC101_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC101_IN_INV_SEL_S  6
/** GPIO_SIG101_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG101_IN_SEL    (BIT(7))
#define GPIO_SIG101_IN_SEL_M  (GPIO_SIG101_IN_SEL_V << GPIO_SIG101_IN_SEL_S)
#define GPIO_SIG101_IN_SEL_V  0x00000001U
#define GPIO_SIG101_IN_SEL_S  7

/** GPIO_FUNC102_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC102_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f0)
/** GPIO_FUNC102_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC102_IN_SEL    0x0000003FU
#define GPIO_FUNC102_IN_SEL_M  (GPIO_FUNC102_IN_SEL_V << GPIO_FUNC102_IN_SEL_S)
#define GPIO_FUNC102_IN_SEL_V  0x0000003FU
#define GPIO_FUNC102_IN_SEL_S  0
/** GPIO_FUNC102_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC102_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC102_IN_INV_SEL_M  (GPIO_FUNC102_IN_INV_SEL_V << GPIO_FUNC102_IN_INV_SEL_S)
#define GPIO_FUNC102_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC102_IN_INV_SEL_S  6
/** GPIO_SIG102_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG102_IN_SEL    (BIT(7))
#define GPIO_SIG102_IN_SEL_M  (GPIO_SIG102_IN_SEL_V << GPIO_SIG102_IN_SEL_S)
#define GPIO_SIG102_IN_SEL_V  0x00000001U
#define GPIO_SIG102_IN_SEL_S  7

/** GPIO_FUNC103_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC103_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f4)
/** GPIO_FUNC103_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC103_IN_SEL    0x0000003FU
#define GPIO_FUNC103_IN_SEL_M  (GPIO_FUNC103_IN_SEL_V << GPIO_FUNC103_IN_SEL_S)
#define GPIO_FUNC103_IN_SEL_V  0x0000003FU
#define GPIO_FUNC103_IN_SEL_S  0
/** GPIO_FUNC103_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC103_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC103_IN_INV_SEL_M  (GPIO_FUNC103_IN_INV_SEL_V << GPIO_FUNC103_IN_INV_SEL_S)
#define GPIO_FUNC103_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC103_IN_INV_SEL_S  6
/** GPIO_SIG103_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG103_IN_SEL    (BIT(7))
#define GPIO_SIG103_IN_SEL_M  (GPIO_SIG103_IN_SEL_V << GPIO_SIG103_IN_SEL_S)
#define GPIO_SIG103_IN_SEL_V  0x00000001U
#define GPIO_SIG103_IN_SEL_S  7

/** GPIO_FUNC104_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC104_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f8)
/** GPIO_FUNC104_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC104_IN_SEL    0x0000003FU
#define GPIO_FUNC104_IN_SEL_M  (GPIO_FUNC104_IN_SEL_V << GPIO_FUNC104_IN_SEL_S)
#define GPIO_FUNC104_IN_SEL_V  0x0000003FU
#define GPIO_FUNC104_IN_SEL_S  0
/** GPIO_FUNC104_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC104_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC104_IN_INV_SEL_M  (GPIO_FUNC104_IN_INV_SEL_V << GPIO_FUNC104_IN_INV_SEL_S)
#define GPIO_FUNC104_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC104_IN_INV_SEL_S  6
/** GPIO_SIG104_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG104_IN_SEL    (BIT(7))
#define GPIO_SIG104_IN_SEL_M  (GPIO_SIG104_IN_SEL_V << GPIO_SIG104_IN_SEL_S)
#define GPIO_SIG104_IN_SEL_V  0x00000001U
#define GPIO_SIG104_IN_SEL_S  7

/** GPIO_FUNC105_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC105_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2fc)
/** GPIO_FUNC105_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC105_IN_SEL    0x0000003FU
#define GPIO_FUNC105_IN_SEL_M  (GPIO_FUNC105_IN_SEL_V << GPIO_FUNC105_IN_SEL_S)
#define GPIO_FUNC105_IN_SEL_V  0x0000003FU
#define GPIO_FUNC105_IN_SEL_S  0
/** GPIO_FUNC105_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC105_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC105_IN_INV_SEL_M  (GPIO_FUNC105_IN_INV_SEL_V << GPIO_FUNC105_IN_INV_SEL_S)
#define GPIO_FUNC105_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC105_IN_INV_SEL_S  6
/** GPIO_SIG105_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG105_IN_SEL    (BIT(7))
#define GPIO_SIG105_IN_SEL_M  (GPIO_SIG105_IN_SEL_V << GPIO_SIG105_IN_SEL_S)
#define GPIO_SIG105_IN_SEL_V  0x00000001U
#define GPIO_SIG105_IN_SEL_S  7

/** GPIO_FUNC106_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC106_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x300)
/** GPIO_FUNC106_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC106_IN_SEL    0x0000003FU
#define GPIO_FUNC106_IN_SEL_M  (GPIO_FUNC106_IN_SEL_V << GPIO_FUNC106_IN_SEL_S)
#define GPIO_FUNC106_IN_SEL_V  0x0000003FU
#define GPIO_FUNC106_IN_SEL_S  0
/** GPIO_FUNC106_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC106_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC106_IN_INV_SEL_M  (GPIO_FUNC106_IN_INV_SEL_V << GPIO_FUNC106_IN_INV_SEL_S)
#define GPIO_FUNC106_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC106_IN_INV_SEL_S  6
/** GPIO_SIG106_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG106_IN_SEL    (BIT(7))
#define GPIO_SIG106_IN_SEL_M  (GPIO_SIG106_IN_SEL_V << GPIO_SIG106_IN_SEL_S)
#define GPIO_SIG106_IN_SEL_V  0x00000001U
#define GPIO_SIG106_IN_SEL_S  7

/** GPIO_FUNC107_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC107_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x304)
/** GPIO_FUNC107_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC107_IN_SEL    0x0000003FU
#define GPIO_FUNC107_IN_SEL_M  (GPIO_FUNC107_IN_SEL_V << GPIO_FUNC107_IN_SEL_S)
#define GPIO_FUNC107_IN_SEL_V  0x0000003FU
#define GPIO_FUNC107_IN_SEL_S  0
/** GPIO_FUNC107_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC107_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC107_IN_INV_SEL_M  (GPIO_FUNC107_IN_INV_SEL_V << GPIO_FUNC107_IN_INV_SEL_S)
#define GPIO_FUNC107_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC107_IN_INV_SEL_S  6
/** GPIO_SIG107_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG107_IN_SEL    (BIT(7))
#define GPIO_SIG107_IN_SEL_M  (GPIO_SIG107_IN_SEL_V << GPIO_SIG107_IN_SEL_S)
#define GPIO_SIG107_IN_SEL_V  0x00000001U
#define GPIO_SIG107_IN_SEL_S  7

/** GPIO_FUNC108_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC108_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x308)
/** GPIO_FUNC108_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC108_IN_SEL    0x0000003FU
#define GPIO_FUNC108_IN_SEL_M  (GPIO_FUNC108_IN_SEL_V << GPIO_FUNC108_IN_SEL_S)
#define GPIO_FUNC108_IN_SEL_V  0x0000003FU
#define GPIO_FUNC108_IN_SEL_S  0
/** GPIO_FUNC108_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC108_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC108_IN_INV_SEL_M  (GPIO_FUNC108_IN_INV_SEL_V << GPIO_FUNC108_IN_INV_SEL_S)
#define GPIO_FUNC108_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC108_IN_INV_SEL_S  6
/** GPIO_SIG108_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG108_IN_SEL    (BIT(7))
#define GPIO_SIG108_IN_SEL_M  (GPIO_SIG108_IN_SEL_V << GPIO_SIG108_IN_SEL_S)
#define GPIO_SIG108_IN_SEL_V  0x00000001U
#define GPIO_SIG108_IN_SEL_S  7

/** GPIO_FUNC109_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC109_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x30c)
/** GPIO_FUNC109_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC109_IN_SEL    0x0000003FU
#define GPIO_FUNC109_IN_SEL_M  (GPIO_FUNC109_IN_SEL_V << GPIO_FUNC109_IN_SEL_S)
#define GPIO_FUNC109_IN_SEL_V  0x0000003FU
#define GPIO_FUNC109_IN_SEL_S  0
/** GPIO_FUNC109_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC109_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC109_IN_INV_SEL_M  (GPIO_FUNC109_IN_INV_SEL_V << GPIO_FUNC109_IN_INV_SEL_S)
#define GPIO_FUNC109_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC109_IN_INV_SEL_S  6
/** GPIO_SIG109_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG109_IN_SEL    (BIT(7))
#define GPIO_SIG109_IN_SEL_M  (GPIO_SIG109_IN_SEL_V << GPIO_SIG109_IN_SEL_S)
#define GPIO_SIG109_IN_SEL_V  0x00000001U
#define GPIO_SIG109_IN_SEL_S  7

/** GPIO_FUNC110_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC110_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x310)
/** GPIO_FUNC110_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC110_IN_SEL    0x0000003FU
#define GPIO_FUNC110_IN_SEL_M  (GPIO_FUNC110_IN_SEL_V << GPIO_FUNC110_IN_SEL_S)
#define GPIO_FUNC110_IN_SEL_V  0x0000003FU
#define GPIO_FUNC110_IN_SEL_S  0
/** GPIO_FUNC110_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC110_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC110_IN_INV_SEL_M  (GPIO_FUNC110_IN_INV_SEL_V << GPIO_FUNC110_IN_INV_SEL_S)
#define GPIO_FUNC110_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC110_IN_INV_SEL_S  6
/** GPIO_SIG110_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG110_IN_SEL    (BIT(7))
#define GPIO_SIG110_IN_SEL_M  (GPIO_SIG110_IN_SEL_V << GPIO_SIG110_IN_SEL_S)
#define GPIO_SIG110_IN_SEL_V  0x00000001U
#define GPIO_SIG110_IN_SEL_S  7

/** GPIO_FUNC111_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC111_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x314)
/** GPIO_FUNC111_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC111_IN_SEL    0x0000003FU
#define GPIO_FUNC111_IN_SEL_M  (GPIO_FUNC111_IN_SEL_V << GPIO_FUNC111_IN_SEL_S)
#define GPIO_FUNC111_IN_SEL_V  0x0000003FU
#define GPIO_FUNC111_IN_SEL_S  0
/** GPIO_FUNC111_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC111_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC111_IN_INV_SEL_M  (GPIO_FUNC111_IN_INV_SEL_V << GPIO_FUNC111_IN_INV_SEL_S)
#define GPIO_FUNC111_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC111_IN_INV_SEL_S  6
/** GPIO_SIG111_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG111_IN_SEL    (BIT(7))
#define GPIO_SIG111_IN_SEL_M  (GPIO_SIG111_IN_SEL_V << GPIO_SIG111_IN_SEL_S)
#define GPIO_SIG111_IN_SEL_V  0x00000001U
#define GPIO_SIG111_IN_SEL_S  7

/** GPIO_FUNC112_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC112_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x318)
/** GPIO_FUNC112_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC112_IN_SEL    0x0000003FU
#define GPIO_FUNC112_IN_SEL_M  (GPIO_FUNC112_IN_SEL_V << GPIO_FUNC112_IN_SEL_S)
#define GPIO_FUNC112_IN_SEL_V  0x0000003FU
#define GPIO_FUNC112_IN_SEL_S  0
/** GPIO_FUNC112_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC112_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC112_IN_INV_SEL_M  (GPIO_FUNC112_IN_INV_SEL_V << GPIO_FUNC112_IN_INV_SEL_S)
#define GPIO_FUNC112_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC112_IN_INV_SEL_S  6
/** GPIO_SIG112_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG112_IN_SEL    (BIT(7))
#define GPIO_SIG112_IN_SEL_M  (GPIO_SIG112_IN_SEL_V << GPIO_SIG112_IN_SEL_S)
#define GPIO_SIG112_IN_SEL_V  0x00000001U
#define GPIO_SIG112_IN_SEL_S  7

/** GPIO_FUNC113_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC113_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x31c)
/** GPIO_FUNC113_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC113_IN_SEL    0x0000003FU
#define GPIO_FUNC113_IN_SEL_M  (GPIO_FUNC113_IN_SEL_V << GPIO_FUNC113_IN_SEL_S)
#define GPIO_FUNC113_IN_SEL_V  0x0000003FU
#define GPIO_FUNC113_IN_SEL_S  0
/** GPIO_FUNC113_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC113_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC113_IN_INV_SEL_M  (GPIO_FUNC113_IN_INV_SEL_V << GPIO_FUNC113_IN_INV_SEL_S)
#define GPIO_FUNC113_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC113_IN_INV_SEL_S  6
/** GPIO_SIG113_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG113_IN_SEL    (BIT(7))
#define GPIO_SIG113_IN_SEL_M  (GPIO_SIG113_IN_SEL_V << GPIO_SIG113_IN_SEL_S)
#define GPIO_SIG113_IN_SEL_V  0x00000001U
#define GPIO_SIG113_IN_SEL_S  7

/** GPIO_FUNC114_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC114_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x320)
/** GPIO_FUNC114_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC114_IN_SEL    0x0000003FU
#define GPIO_FUNC114_IN_SEL_M  (GPIO_FUNC114_IN_SEL_V << GPIO_FUNC114_IN_SEL_S)
#define GPIO_FUNC114_IN_SEL_V  0x0000003FU
#define GPIO_FUNC114_IN_SEL_S  0
/** GPIO_FUNC114_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC114_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC114_IN_INV_SEL_M  (GPIO_FUNC114_IN_INV_SEL_V << GPIO_FUNC114_IN_INV_SEL_S)
#define GPIO_FUNC114_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC114_IN_INV_SEL_S  6
/** GPIO_SIG114_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG114_IN_SEL    (BIT(7))
#define GPIO_SIG114_IN_SEL_M  (GPIO_SIG114_IN_SEL_V << GPIO_SIG114_IN_SEL_S)
#define GPIO_SIG114_IN_SEL_V  0x00000001U
#define GPIO_SIG114_IN_SEL_S  7

/** GPIO_FUNC117_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC117_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x32c)
/** GPIO_FUNC117_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC117_IN_SEL    0x0000003FU
#define GPIO_FUNC117_IN_SEL_M  (GPIO_FUNC117_IN_SEL_V << GPIO_FUNC117_IN_SEL_S)
#define GPIO_FUNC117_IN_SEL_V  0x0000003FU
#define GPIO_FUNC117_IN_SEL_S  0
/** GPIO_FUNC117_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC117_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC117_IN_INV_SEL_M  (GPIO_FUNC117_IN_INV_SEL_V << GPIO_FUNC117_IN_INV_SEL_S)
#define GPIO_FUNC117_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC117_IN_INV_SEL_S  6
/** GPIO_SIG117_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG117_IN_SEL    (BIT(7))
#define GPIO_SIG117_IN_SEL_M  (GPIO_SIG117_IN_SEL_V << GPIO_SIG117_IN_SEL_S)
#define GPIO_SIG117_IN_SEL_V  0x00000001U
#define GPIO_SIG117_IN_SEL_S  7

/** GPIO_FUNC118_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC118_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x330)
/** GPIO_FUNC118_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC118_IN_SEL    0x0000003FU
#define GPIO_FUNC118_IN_SEL_M  (GPIO_FUNC118_IN_SEL_V << GPIO_FUNC118_IN_SEL_S)
#define GPIO_FUNC118_IN_SEL_V  0x0000003FU
#define GPIO_FUNC118_IN_SEL_S  0
/** GPIO_FUNC118_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC118_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC118_IN_INV_SEL_M  (GPIO_FUNC118_IN_INV_SEL_V << GPIO_FUNC118_IN_INV_SEL_S)
#define GPIO_FUNC118_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC118_IN_INV_SEL_S  6
/** GPIO_SIG118_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG118_IN_SEL    (BIT(7))
#define GPIO_SIG118_IN_SEL_M  (GPIO_SIG118_IN_SEL_V << GPIO_SIG118_IN_SEL_S)
#define GPIO_SIG118_IN_SEL_V  0x00000001U
#define GPIO_SIG118_IN_SEL_S  7

/** GPIO_FUNC126_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC126_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x350)
/** GPIO_FUNC126_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC126_IN_SEL    0x0000003FU
#define GPIO_FUNC126_IN_SEL_M  (GPIO_FUNC126_IN_SEL_V << GPIO_FUNC126_IN_SEL_S)
#define GPIO_FUNC126_IN_SEL_V  0x0000003FU
#define GPIO_FUNC126_IN_SEL_S  0
/** GPIO_FUNC126_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC126_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC126_IN_INV_SEL_M  (GPIO_FUNC126_IN_INV_SEL_V << GPIO_FUNC126_IN_INV_SEL_S)
#define GPIO_FUNC126_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC126_IN_INV_SEL_S  6
/** GPIO_SIG126_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG126_IN_SEL    (BIT(7))
#define GPIO_SIG126_IN_SEL_M  (GPIO_SIG126_IN_SEL_V << GPIO_SIG126_IN_SEL_S)
#define GPIO_SIG126_IN_SEL_V  0x00000001U
#define GPIO_SIG126_IN_SEL_S  7

/** GPIO_FUNC127_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC127_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x354)
/** GPIO_FUNC127_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC127_IN_SEL    0x0000003FU
#define GPIO_FUNC127_IN_SEL_M  (GPIO_FUNC127_IN_SEL_V << GPIO_FUNC127_IN_SEL_S)
#define GPIO_FUNC127_IN_SEL_V  0x0000003FU
#define GPIO_FUNC127_IN_SEL_S  0
/** GPIO_FUNC127_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC127_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC127_IN_INV_SEL_M  (GPIO_FUNC127_IN_INV_SEL_V << GPIO_FUNC127_IN_INV_SEL_S)
#define GPIO_FUNC127_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC127_IN_INV_SEL_S  6
/** GPIO_SIG127_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG127_IN_SEL    (BIT(7))
#define GPIO_SIG127_IN_SEL_M  (GPIO_SIG127_IN_SEL_V << GPIO_SIG127_IN_SEL_S)
#define GPIO_SIG127_IN_SEL_V  0x00000001U
#define GPIO_SIG127_IN_SEL_S  7

/** GPIO_FUNC128_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC128_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x358)
/** GPIO_FUNC128_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC128_IN_SEL    0x0000003FU
#define GPIO_FUNC128_IN_SEL_M  (GPIO_FUNC128_IN_SEL_V << GPIO_FUNC128_IN_SEL_S)
#define GPIO_FUNC128_IN_SEL_V  0x0000003FU
#define GPIO_FUNC128_IN_SEL_S  0
/** GPIO_FUNC128_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC128_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC128_IN_INV_SEL_M  (GPIO_FUNC128_IN_INV_SEL_V << GPIO_FUNC128_IN_INV_SEL_S)
#define GPIO_FUNC128_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC128_IN_INV_SEL_S  6
/** GPIO_SIG128_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG128_IN_SEL    (BIT(7))
#define GPIO_SIG128_IN_SEL_M  (GPIO_SIG128_IN_SEL_V << GPIO_SIG128_IN_SEL_S)
#define GPIO_SIG128_IN_SEL_V  0x00000001U
#define GPIO_SIG128_IN_SEL_S  7

/** GPIO_FUNC129_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC129_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x35c)
/** GPIO_FUNC129_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC129_IN_SEL    0x0000003FU
#define GPIO_FUNC129_IN_SEL_M  (GPIO_FUNC129_IN_SEL_V << GPIO_FUNC129_IN_SEL_S)
#define GPIO_FUNC129_IN_SEL_V  0x0000003FU
#define GPIO_FUNC129_IN_SEL_S  0
/** GPIO_FUNC129_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC129_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC129_IN_INV_SEL_M  (GPIO_FUNC129_IN_INV_SEL_V << GPIO_FUNC129_IN_INV_SEL_S)
#define GPIO_FUNC129_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC129_IN_INV_SEL_S  6
/** GPIO_SIG129_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG129_IN_SEL    (BIT(7))
#define GPIO_SIG129_IN_SEL_M  (GPIO_SIG129_IN_SEL_V << GPIO_SIG129_IN_SEL_S)
#define GPIO_SIG129_IN_SEL_V  0x00000001U
#define GPIO_SIG129_IN_SEL_S  7

/** GPIO_FUNC130_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC130_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x360)
/** GPIO_FUNC130_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC130_IN_SEL    0x0000003FU
#define GPIO_FUNC130_IN_SEL_M  (GPIO_FUNC130_IN_SEL_V << GPIO_FUNC130_IN_SEL_S)
#define GPIO_FUNC130_IN_SEL_V  0x0000003FU
#define GPIO_FUNC130_IN_SEL_S  0
/** GPIO_FUNC130_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC130_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC130_IN_INV_SEL_M  (GPIO_FUNC130_IN_INV_SEL_V << GPIO_FUNC130_IN_INV_SEL_S)
#define GPIO_FUNC130_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC130_IN_INV_SEL_S  6
/** GPIO_SIG130_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG130_IN_SEL    (BIT(7))
#define GPIO_SIG130_IN_SEL_M  (GPIO_SIG130_IN_SEL_V << GPIO_SIG130_IN_SEL_S)
#define GPIO_SIG130_IN_SEL_V  0x00000001U
#define GPIO_SIG130_IN_SEL_S  7

/** GPIO_FUNC131_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC131_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x364)
/** GPIO_FUNC131_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC131_IN_SEL    0x0000003FU
#define GPIO_FUNC131_IN_SEL_M  (GPIO_FUNC131_IN_SEL_V << GPIO_FUNC131_IN_SEL_S)
#define GPIO_FUNC131_IN_SEL_V  0x0000003FU
#define GPIO_FUNC131_IN_SEL_S  0
/** GPIO_FUNC131_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC131_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC131_IN_INV_SEL_M  (GPIO_FUNC131_IN_INV_SEL_V << GPIO_FUNC131_IN_INV_SEL_S)
#define GPIO_FUNC131_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC131_IN_INV_SEL_S  6
/** GPIO_SIG131_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG131_IN_SEL    (BIT(7))
#define GPIO_SIG131_IN_SEL_M  (GPIO_SIG131_IN_SEL_V << GPIO_SIG131_IN_SEL_S)
#define GPIO_SIG131_IN_SEL_V  0x00000001U
#define GPIO_SIG131_IN_SEL_S  7

/** GPIO_FUNC132_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC132_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x368)
/** GPIO_FUNC132_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC132_IN_SEL    0x0000003FU
#define GPIO_FUNC132_IN_SEL_M  (GPIO_FUNC132_IN_SEL_V << GPIO_FUNC132_IN_SEL_S)
#define GPIO_FUNC132_IN_SEL_V  0x0000003FU
#define GPIO_FUNC132_IN_SEL_S  0
/** GPIO_FUNC132_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC132_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC132_IN_INV_SEL_M  (GPIO_FUNC132_IN_INV_SEL_V << GPIO_FUNC132_IN_INV_SEL_S)
#define GPIO_FUNC132_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC132_IN_INV_SEL_S  6
/** GPIO_SIG132_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG132_IN_SEL    (BIT(7))
#define GPIO_SIG132_IN_SEL_M  (GPIO_SIG132_IN_SEL_V << GPIO_SIG132_IN_SEL_S)
#define GPIO_SIG132_IN_SEL_V  0x00000001U
#define GPIO_SIG132_IN_SEL_S  7

/** GPIO_FUNC133_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC133_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x36c)
/** GPIO_FUNC133_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC133_IN_SEL    0x0000003FU
#define GPIO_FUNC133_IN_SEL_M  (GPIO_FUNC133_IN_SEL_V << GPIO_FUNC133_IN_SEL_S)
#define GPIO_FUNC133_IN_SEL_V  0x0000003FU
#define GPIO_FUNC133_IN_SEL_S  0
/** GPIO_FUNC133_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC133_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC133_IN_INV_SEL_M  (GPIO_FUNC133_IN_INV_SEL_V << GPIO_FUNC133_IN_INV_SEL_S)
#define GPIO_FUNC133_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC133_IN_INV_SEL_S  6
/** GPIO_SIG133_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG133_IN_SEL    (BIT(7))
#define GPIO_SIG133_IN_SEL_M  (GPIO_SIG133_IN_SEL_V << GPIO_SIG133_IN_SEL_S)
#define GPIO_SIG133_IN_SEL_V  0x00000001U
#define GPIO_SIG133_IN_SEL_S  7

/** GPIO_FUNC134_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC134_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x370)
/** GPIO_FUNC134_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC134_IN_SEL    0x0000003FU
#define GPIO_FUNC134_IN_SEL_M  (GPIO_FUNC134_IN_SEL_V << GPIO_FUNC134_IN_SEL_S)
#define GPIO_FUNC134_IN_SEL_V  0x0000003FU
#define GPIO_FUNC134_IN_SEL_S  0
/** GPIO_FUNC134_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC134_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC134_IN_INV_SEL_M  (GPIO_FUNC134_IN_INV_SEL_V << GPIO_FUNC134_IN_INV_SEL_S)
#define GPIO_FUNC134_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC134_IN_INV_SEL_S  6
/** GPIO_SIG134_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG134_IN_SEL    (BIT(7))
#define GPIO_SIG134_IN_SEL_M  (GPIO_SIG134_IN_SEL_V << GPIO_SIG134_IN_SEL_S)
#define GPIO_SIG134_IN_SEL_V  0x00000001U
#define GPIO_SIG134_IN_SEL_S  7

/** GPIO_FUNC135_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC135_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x374)
/** GPIO_FUNC135_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC135_IN_SEL    0x0000003FU
#define GPIO_FUNC135_IN_SEL_M  (GPIO_FUNC135_IN_SEL_V << GPIO_FUNC135_IN_SEL_S)
#define GPIO_FUNC135_IN_SEL_V  0x0000003FU
#define GPIO_FUNC135_IN_SEL_S  0
/** GPIO_FUNC135_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC135_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC135_IN_INV_SEL_M  (GPIO_FUNC135_IN_INV_SEL_V << GPIO_FUNC135_IN_INV_SEL_S)
#define GPIO_FUNC135_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC135_IN_INV_SEL_S  6
/** GPIO_SIG135_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG135_IN_SEL    (BIT(7))
#define GPIO_SIG135_IN_SEL_M  (GPIO_SIG135_IN_SEL_V << GPIO_SIG135_IN_SEL_S)
#define GPIO_SIG135_IN_SEL_V  0x00000001U
#define GPIO_SIG135_IN_SEL_S  7

/** GPIO_FUNC136_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC136_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x378)
/** GPIO_FUNC136_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC136_IN_SEL    0x0000003FU
#define GPIO_FUNC136_IN_SEL_M  (GPIO_FUNC136_IN_SEL_V << GPIO_FUNC136_IN_SEL_S)
#define GPIO_FUNC136_IN_SEL_V  0x0000003FU
#define GPIO_FUNC136_IN_SEL_S  0
/** GPIO_FUNC136_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC136_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC136_IN_INV_SEL_M  (GPIO_FUNC136_IN_INV_SEL_V << GPIO_FUNC136_IN_INV_SEL_S)
#define GPIO_FUNC136_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC136_IN_INV_SEL_S  6
/** GPIO_SIG136_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG136_IN_SEL    (BIT(7))
#define GPIO_SIG136_IN_SEL_M  (GPIO_SIG136_IN_SEL_V << GPIO_SIG136_IN_SEL_S)
#define GPIO_SIG136_IN_SEL_V  0x00000001U
#define GPIO_SIG136_IN_SEL_S  7

/** GPIO_FUNC137_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC137_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x37c)
/** GPIO_FUNC137_IN_SEL : R/W; bitpos: [5:0]; default: 63;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC137_IN_SEL    0x0000003FU
#define GPIO_FUNC137_IN_SEL_M  (GPIO_FUNC137_IN_SEL_V << GPIO_FUNC137_IN_SEL_S)
#define GPIO_FUNC137_IN_SEL_V  0x0000003FU
#define GPIO_FUNC137_IN_SEL_S  0
/** GPIO_FUNC137_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC137_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC137_IN_INV_SEL_M  (GPIO_FUNC137_IN_INV_SEL_V << GPIO_FUNC137_IN_INV_SEL_S)
#define GPIO_FUNC137_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC137_IN_INV_SEL_S  6
/** GPIO_SIG137_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG137_IN_SEL    (BIT(7))
#define GPIO_SIG137_IN_SEL_M  (GPIO_SIG137_IN_SEL_V << GPIO_SIG137_IN_SEL_S)
#define GPIO_SIG137_IN_SEL_V  0x00000001U
#define GPIO_SIG137_IN_SEL_S  7

/** GPIO_FUNC138_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC138_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x380)
/** GPIO_FUNC138_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC138_IN_SEL    0x0000003FU
#define GPIO_FUNC138_IN_SEL_M  (GPIO_FUNC138_IN_SEL_V << GPIO_FUNC138_IN_SEL_S)
#define GPIO_FUNC138_IN_SEL_V  0x0000003FU
#define GPIO_FUNC138_IN_SEL_S  0
/** GPIO_FUNC138_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC138_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC138_IN_INV_SEL_M  (GPIO_FUNC138_IN_INV_SEL_V << GPIO_FUNC138_IN_INV_SEL_S)
#define GPIO_FUNC138_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC138_IN_INV_SEL_S  6
/** GPIO_SIG138_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG138_IN_SEL    (BIT(7))
#define GPIO_SIG138_IN_SEL_M  (GPIO_SIG138_IN_SEL_V << GPIO_SIG138_IN_SEL_S)
#define GPIO_SIG138_IN_SEL_V  0x00000001U
#define GPIO_SIG138_IN_SEL_S  7

/** GPIO_FUNC139_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC139_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x384)
/** GPIO_FUNC139_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC139_IN_SEL    0x0000003FU
#define GPIO_FUNC139_IN_SEL_M  (GPIO_FUNC139_IN_SEL_V << GPIO_FUNC139_IN_SEL_S)
#define GPIO_FUNC139_IN_SEL_V  0x0000003FU
#define GPIO_FUNC139_IN_SEL_S  0
/** GPIO_FUNC139_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC139_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC139_IN_INV_SEL_M  (GPIO_FUNC139_IN_INV_SEL_V << GPIO_FUNC139_IN_INV_SEL_S)
#define GPIO_FUNC139_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC139_IN_INV_SEL_S  6
/** GPIO_SIG139_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG139_IN_SEL    (BIT(7))
#define GPIO_SIG139_IN_SEL_M  (GPIO_SIG139_IN_SEL_V << GPIO_SIG139_IN_SEL_S)
#define GPIO_SIG139_IN_SEL_V  0x00000001U
#define GPIO_SIG139_IN_SEL_S  7

/** GPIO_FUNC140_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC140_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x388)
/** GPIO_FUNC140_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC140_IN_SEL    0x0000003FU
#define GPIO_FUNC140_IN_SEL_M  (GPIO_FUNC140_IN_SEL_V << GPIO_FUNC140_IN_SEL_S)
#define GPIO_FUNC140_IN_SEL_V  0x0000003FU
#define GPIO_FUNC140_IN_SEL_S  0
/** GPIO_FUNC140_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC140_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC140_IN_INV_SEL_M  (GPIO_FUNC140_IN_INV_SEL_V << GPIO_FUNC140_IN_INV_SEL_S)
#define GPIO_FUNC140_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC140_IN_INV_SEL_S  6
/** GPIO_SIG140_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG140_IN_SEL    (BIT(7))
#define GPIO_SIG140_IN_SEL_M  (GPIO_SIG140_IN_SEL_V << GPIO_SIG140_IN_SEL_S)
#define GPIO_SIG140_IN_SEL_V  0x00000001U
#define GPIO_SIG140_IN_SEL_S  7

/** GPIO_FUNC141_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC141_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x38c)
/** GPIO_FUNC141_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC141_IN_SEL    0x0000003FU
#define GPIO_FUNC141_IN_SEL_M  (GPIO_FUNC141_IN_SEL_V << GPIO_FUNC141_IN_SEL_S)
#define GPIO_FUNC141_IN_SEL_V  0x0000003FU
#define GPIO_FUNC141_IN_SEL_S  0
/** GPIO_FUNC141_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC141_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC141_IN_INV_SEL_M  (GPIO_FUNC141_IN_INV_SEL_V << GPIO_FUNC141_IN_INV_SEL_S)
#define GPIO_FUNC141_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC141_IN_INV_SEL_S  6
/** GPIO_SIG141_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG141_IN_SEL    (BIT(7))
#define GPIO_SIG141_IN_SEL_M  (GPIO_SIG141_IN_SEL_V << GPIO_SIG141_IN_SEL_S)
#define GPIO_SIG141_IN_SEL_V  0x00000001U
#define GPIO_SIG141_IN_SEL_S  7

/** GPIO_FUNC142_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC142_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x390)
/** GPIO_FUNC142_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC142_IN_SEL    0x0000003FU
#define GPIO_FUNC142_IN_SEL_M  (GPIO_FUNC142_IN_SEL_V << GPIO_FUNC142_IN_SEL_S)
#define GPIO_FUNC142_IN_SEL_V  0x0000003FU
#define GPIO_FUNC142_IN_SEL_S  0
/** GPIO_FUNC142_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC142_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC142_IN_INV_SEL_M  (GPIO_FUNC142_IN_INV_SEL_V << GPIO_FUNC142_IN_INV_SEL_S)
#define GPIO_FUNC142_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC142_IN_INV_SEL_S  6
/** GPIO_SIG142_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG142_IN_SEL    (BIT(7))
#define GPIO_SIG142_IN_SEL_M  (GPIO_SIG142_IN_SEL_V << GPIO_SIG142_IN_SEL_S)
#define GPIO_SIG142_IN_SEL_V  0x00000001U
#define GPIO_SIG142_IN_SEL_S  7

/** GPIO_FUNC143_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC143_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x394)
/** GPIO_FUNC143_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC143_IN_SEL    0x0000003FU
#define GPIO_FUNC143_IN_SEL_M  (GPIO_FUNC143_IN_SEL_V << GPIO_FUNC143_IN_SEL_S)
#define GPIO_FUNC143_IN_SEL_V  0x0000003FU
#define GPIO_FUNC143_IN_SEL_S  0
/** GPIO_FUNC143_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC143_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC143_IN_INV_SEL_M  (GPIO_FUNC143_IN_INV_SEL_V << GPIO_FUNC143_IN_INV_SEL_S)
#define GPIO_FUNC143_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC143_IN_INV_SEL_S  6
/** GPIO_SIG143_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG143_IN_SEL    (BIT(7))
#define GPIO_SIG143_IN_SEL_M  (GPIO_SIG143_IN_SEL_V << GPIO_SIG143_IN_SEL_S)
#define GPIO_SIG143_IN_SEL_V  0x00000001U
#define GPIO_SIG143_IN_SEL_S  7

/** GPIO_FUNC144_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC144_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x398)
/** GPIO_FUNC144_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC144_IN_SEL    0x0000003FU
#define GPIO_FUNC144_IN_SEL_M  (GPIO_FUNC144_IN_SEL_V << GPIO_FUNC144_IN_SEL_S)
#define GPIO_FUNC144_IN_SEL_V  0x0000003FU
#define GPIO_FUNC144_IN_SEL_S  0
/** GPIO_FUNC144_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC144_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC144_IN_INV_SEL_M  (GPIO_FUNC144_IN_INV_SEL_V << GPIO_FUNC144_IN_INV_SEL_S)
#define GPIO_FUNC144_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC144_IN_INV_SEL_S  6
/** GPIO_SIG144_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG144_IN_SEL    (BIT(7))
#define GPIO_SIG144_IN_SEL_M  (GPIO_SIG144_IN_SEL_V << GPIO_SIG144_IN_SEL_S)
#define GPIO_SIG144_IN_SEL_V  0x00000001U
#define GPIO_SIG144_IN_SEL_S  7

/** GPIO_FUNC145_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC145_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x39c)
/** GPIO_FUNC145_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC145_IN_SEL    0x0000003FU
#define GPIO_FUNC145_IN_SEL_M  (GPIO_FUNC145_IN_SEL_V << GPIO_FUNC145_IN_SEL_S)
#define GPIO_FUNC145_IN_SEL_V  0x0000003FU
#define GPIO_FUNC145_IN_SEL_S  0
/** GPIO_FUNC145_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC145_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC145_IN_INV_SEL_M  (GPIO_FUNC145_IN_INV_SEL_V << GPIO_FUNC145_IN_INV_SEL_S)
#define GPIO_FUNC145_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC145_IN_INV_SEL_S  6
/** GPIO_SIG145_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG145_IN_SEL    (BIT(7))
#define GPIO_SIG145_IN_SEL_M  (GPIO_SIG145_IN_SEL_V << GPIO_SIG145_IN_SEL_S)
#define GPIO_SIG145_IN_SEL_V  0x00000001U
#define GPIO_SIG145_IN_SEL_S  7

/** GPIO_FUNC146_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC146_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a0)
/** GPIO_FUNC146_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC146_IN_SEL    0x0000003FU
#define GPIO_FUNC146_IN_SEL_M  (GPIO_FUNC146_IN_SEL_V << GPIO_FUNC146_IN_SEL_S)
#define GPIO_FUNC146_IN_SEL_V  0x0000003FU
#define GPIO_FUNC146_IN_SEL_S  0
/** GPIO_FUNC146_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC146_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC146_IN_INV_SEL_M  (GPIO_FUNC146_IN_INV_SEL_V << GPIO_FUNC146_IN_INV_SEL_S)
#define GPIO_FUNC146_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC146_IN_INV_SEL_S  6
/** GPIO_SIG146_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG146_IN_SEL    (BIT(7))
#define GPIO_SIG146_IN_SEL_M  (GPIO_SIG146_IN_SEL_V << GPIO_SIG146_IN_SEL_S)
#define GPIO_SIG146_IN_SEL_V  0x00000001U
#define GPIO_SIG146_IN_SEL_S  7

/** GPIO_FUNC147_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC147_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a4)
/** GPIO_FUNC147_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC147_IN_SEL    0x0000003FU
#define GPIO_FUNC147_IN_SEL_M  (GPIO_FUNC147_IN_SEL_V << GPIO_FUNC147_IN_SEL_S)
#define GPIO_FUNC147_IN_SEL_V  0x0000003FU
#define GPIO_FUNC147_IN_SEL_S  0
/** GPIO_FUNC147_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC147_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC147_IN_INV_SEL_M  (GPIO_FUNC147_IN_INV_SEL_V << GPIO_FUNC147_IN_INV_SEL_S)
#define GPIO_FUNC147_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC147_IN_INV_SEL_S  6
/** GPIO_SIG147_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG147_IN_SEL    (BIT(7))
#define GPIO_SIG147_IN_SEL_M  (GPIO_SIG147_IN_SEL_V << GPIO_SIG147_IN_SEL_S)
#define GPIO_SIG147_IN_SEL_V  0x00000001U
#define GPIO_SIG147_IN_SEL_S  7

/** GPIO_FUNC148_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC148_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a8)
/** GPIO_FUNC148_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC148_IN_SEL    0x0000003FU
#define GPIO_FUNC148_IN_SEL_M  (GPIO_FUNC148_IN_SEL_V << GPIO_FUNC148_IN_SEL_S)
#define GPIO_FUNC148_IN_SEL_V  0x0000003FU
#define GPIO_FUNC148_IN_SEL_S  0
/** GPIO_FUNC148_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC148_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC148_IN_INV_SEL_M  (GPIO_FUNC148_IN_INV_SEL_V << GPIO_FUNC148_IN_INV_SEL_S)
#define GPIO_FUNC148_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC148_IN_INV_SEL_S  6
/** GPIO_SIG148_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG148_IN_SEL    (BIT(7))
#define GPIO_SIG148_IN_SEL_M  (GPIO_SIG148_IN_SEL_V << GPIO_SIG148_IN_SEL_S)
#define GPIO_SIG148_IN_SEL_V  0x00000001U
#define GPIO_SIG148_IN_SEL_S  7

/** GPIO_FUNC149_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC149_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3ac)
/** GPIO_FUNC149_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC149_IN_SEL    0x0000003FU
#define GPIO_FUNC149_IN_SEL_M  (GPIO_FUNC149_IN_SEL_V << GPIO_FUNC149_IN_SEL_S)
#define GPIO_FUNC149_IN_SEL_V  0x0000003FU
#define GPIO_FUNC149_IN_SEL_S  0
/** GPIO_FUNC149_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC149_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC149_IN_INV_SEL_M  (GPIO_FUNC149_IN_INV_SEL_V << GPIO_FUNC149_IN_INV_SEL_S)
#define GPIO_FUNC149_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC149_IN_INV_SEL_S  6
/** GPIO_SIG149_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG149_IN_SEL    (BIT(7))
#define GPIO_SIG149_IN_SEL_M  (GPIO_SIG149_IN_SEL_V << GPIO_SIG149_IN_SEL_S)
#define GPIO_SIG149_IN_SEL_V  0x00000001U
#define GPIO_SIG149_IN_SEL_S  7

/** GPIO_FUNC150_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC150_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b0)
/** GPIO_FUNC150_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC150_IN_SEL    0x0000003FU
#define GPIO_FUNC150_IN_SEL_M  (GPIO_FUNC150_IN_SEL_V << GPIO_FUNC150_IN_SEL_S)
#define GPIO_FUNC150_IN_SEL_V  0x0000003FU
#define GPIO_FUNC150_IN_SEL_S  0
/** GPIO_FUNC150_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC150_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC150_IN_INV_SEL_M  (GPIO_FUNC150_IN_INV_SEL_V << GPIO_FUNC150_IN_INV_SEL_S)
#define GPIO_FUNC150_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC150_IN_INV_SEL_S  6
/** GPIO_SIG150_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG150_IN_SEL    (BIT(7))
#define GPIO_SIG150_IN_SEL_M  (GPIO_SIG150_IN_SEL_V << GPIO_SIG150_IN_SEL_S)
#define GPIO_SIG150_IN_SEL_V  0x00000001U
#define GPIO_SIG150_IN_SEL_S  7

/** GPIO_FUNC151_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC151_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b4)
/** GPIO_FUNC151_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC151_IN_SEL    0x0000003FU
#define GPIO_FUNC151_IN_SEL_M  (GPIO_FUNC151_IN_SEL_V << GPIO_FUNC151_IN_SEL_S)
#define GPIO_FUNC151_IN_SEL_V  0x0000003FU
#define GPIO_FUNC151_IN_SEL_S  0
/** GPIO_FUNC151_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC151_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC151_IN_INV_SEL_M  (GPIO_FUNC151_IN_INV_SEL_V << GPIO_FUNC151_IN_INV_SEL_S)
#define GPIO_FUNC151_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC151_IN_INV_SEL_S  6
/** GPIO_SIG151_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG151_IN_SEL    (BIT(7))
#define GPIO_SIG151_IN_SEL_M  (GPIO_SIG151_IN_SEL_V << GPIO_SIG151_IN_SEL_S)
#define GPIO_SIG151_IN_SEL_V  0x00000001U
#define GPIO_SIG151_IN_SEL_S  7

/** GPIO_FUNC152_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC152_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b8)
/** GPIO_FUNC152_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC152_IN_SEL    0x0000003FU
#define GPIO_FUNC152_IN_SEL_M  (GPIO_FUNC152_IN_SEL_V << GPIO_FUNC152_IN_SEL_S)
#define GPIO_FUNC152_IN_SEL_V  0x0000003FU
#define GPIO_FUNC152_IN_SEL_S  0
/** GPIO_FUNC152_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC152_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC152_IN_INV_SEL_M  (GPIO_FUNC152_IN_INV_SEL_V << GPIO_FUNC152_IN_INV_SEL_S)
#define GPIO_FUNC152_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC152_IN_INV_SEL_S  6
/** GPIO_SIG152_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG152_IN_SEL    (BIT(7))
#define GPIO_SIG152_IN_SEL_M  (GPIO_SIG152_IN_SEL_V << GPIO_SIG152_IN_SEL_S)
#define GPIO_SIG152_IN_SEL_V  0x00000001U
#define GPIO_SIG152_IN_SEL_S  7

/** GPIO_FUNC153_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC153_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3bc)
/** GPIO_FUNC153_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC153_IN_SEL    0x0000003FU
#define GPIO_FUNC153_IN_SEL_M  (GPIO_FUNC153_IN_SEL_V << GPIO_FUNC153_IN_SEL_S)
#define GPIO_FUNC153_IN_SEL_V  0x0000003FU
#define GPIO_FUNC153_IN_SEL_S  0
/** GPIO_FUNC153_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC153_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC153_IN_INV_SEL_M  (GPIO_FUNC153_IN_INV_SEL_V << GPIO_FUNC153_IN_INV_SEL_S)
#define GPIO_FUNC153_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC153_IN_INV_SEL_S  6
/** GPIO_SIG153_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG153_IN_SEL    (BIT(7))
#define GPIO_SIG153_IN_SEL_M  (GPIO_SIG153_IN_SEL_V << GPIO_SIG153_IN_SEL_S)
#define GPIO_SIG153_IN_SEL_V  0x00000001U
#define GPIO_SIG153_IN_SEL_S  7

/** GPIO_FUNC154_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC154_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c0)
/** GPIO_FUNC154_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC154_IN_SEL    0x0000003FU
#define GPIO_FUNC154_IN_SEL_M  (GPIO_FUNC154_IN_SEL_V << GPIO_FUNC154_IN_SEL_S)
#define GPIO_FUNC154_IN_SEL_V  0x0000003FU
#define GPIO_FUNC154_IN_SEL_S  0
/** GPIO_FUNC154_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC154_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC154_IN_INV_SEL_M  (GPIO_FUNC154_IN_INV_SEL_V << GPIO_FUNC154_IN_INV_SEL_S)
#define GPIO_FUNC154_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC154_IN_INV_SEL_S  6
/** GPIO_SIG154_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG154_IN_SEL    (BIT(7))
#define GPIO_SIG154_IN_SEL_M  (GPIO_SIG154_IN_SEL_V << GPIO_SIG154_IN_SEL_S)
#define GPIO_SIG154_IN_SEL_V  0x00000001U
#define GPIO_SIG154_IN_SEL_S  7

/** GPIO_FUNC155_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC155_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c4)
/** GPIO_FUNC155_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC155_IN_SEL    0x0000003FU
#define GPIO_FUNC155_IN_SEL_M  (GPIO_FUNC155_IN_SEL_V << GPIO_FUNC155_IN_SEL_S)
#define GPIO_FUNC155_IN_SEL_V  0x0000003FU
#define GPIO_FUNC155_IN_SEL_S  0
/** GPIO_FUNC155_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC155_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC155_IN_INV_SEL_M  (GPIO_FUNC155_IN_INV_SEL_V << GPIO_FUNC155_IN_INV_SEL_S)
#define GPIO_FUNC155_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC155_IN_INV_SEL_S  6
/** GPIO_SIG155_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG155_IN_SEL    (BIT(7))
#define GPIO_SIG155_IN_SEL_M  (GPIO_SIG155_IN_SEL_V << GPIO_SIG155_IN_SEL_S)
#define GPIO_SIG155_IN_SEL_V  0x00000001U
#define GPIO_SIG155_IN_SEL_S  7

/** GPIO_FUNC156_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC156_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c8)
/** GPIO_FUNC156_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC156_IN_SEL    0x0000003FU
#define GPIO_FUNC156_IN_SEL_M  (GPIO_FUNC156_IN_SEL_V << GPIO_FUNC156_IN_SEL_S)
#define GPIO_FUNC156_IN_SEL_V  0x0000003FU
#define GPIO_FUNC156_IN_SEL_S  0
/** GPIO_FUNC156_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC156_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC156_IN_INV_SEL_M  (GPIO_FUNC156_IN_INV_SEL_V << GPIO_FUNC156_IN_INV_SEL_S)
#define GPIO_FUNC156_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC156_IN_INV_SEL_S  6
/** GPIO_SIG156_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG156_IN_SEL    (BIT(7))
#define GPIO_SIG156_IN_SEL_M  (GPIO_SIG156_IN_SEL_V << GPIO_SIG156_IN_SEL_S)
#define GPIO_SIG156_IN_SEL_V  0x00000001U
#define GPIO_SIG156_IN_SEL_S  7

/** GPIO_FUNC158_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC158_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d0)
/** GPIO_FUNC158_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC158_IN_SEL    0x0000003FU
#define GPIO_FUNC158_IN_SEL_M  (GPIO_FUNC158_IN_SEL_V << GPIO_FUNC158_IN_SEL_S)
#define GPIO_FUNC158_IN_SEL_V  0x0000003FU
#define GPIO_FUNC158_IN_SEL_S  0
/** GPIO_FUNC158_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC158_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC158_IN_INV_SEL_M  (GPIO_FUNC158_IN_INV_SEL_V << GPIO_FUNC158_IN_INV_SEL_S)
#define GPIO_FUNC158_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC158_IN_INV_SEL_S  6
/** GPIO_SIG158_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG158_IN_SEL    (BIT(7))
#define GPIO_SIG158_IN_SEL_M  (GPIO_SIG158_IN_SEL_V << GPIO_SIG158_IN_SEL_S)
#define GPIO_SIG158_IN_SEL_V  0x00000001U
#define GPIO_SIG158_IN_SEL_S  7

/** GPIO_FUNC159_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC159_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d4)
/** GPIO_FUNC159_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC159_IN_SEL    0x0000003FU
#define GPIO_FUNC159_IN_SEL_M  (GPIO_FUNC159_IN_SEL_V << GPIO_FUNC159_IN_SEL_S)
#define GPIO_FUNC159_IN_SEL_V  0x0000003FU
#define GPIO_FUNC159_IN_SEL_S  0
/** GPIO_FUNC159_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC159_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC159_IN_INV_SEL_M  (GPIO_FUNC159_IN_INV_SEL_V << GPIO_FUNC159_IN_INV_SEL_S)
#define GPIO_FUNC159_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC159_IN_INV_SEL_S  6
/** GPIO_SIG159_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG159_IN_SEL    (BIT(7))
#define GPIO_SIG159_IN_SEL_M  (GPIO_SIG159_IN_SEL_V << GPIO_SIG159_IN_SEL_S)
#define GPIO_SIG159_IN_SEL_V  0x00000001U
#define GPIO_SIG159_IN_SEL_S  7

/** GPIO_FUNC160_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC160_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d8)
/** GPIO_FUNC160_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC160_IN_SEL    0x0000003FU
#define GPIO_FUNC160_IN_SEL_M  (GPIO_FUNC160_IN_SEL_V << GPIO_FUNC160_IN_SEL_S)
#define GPIO_FUNC160_IN_SEL_V  0x0000003FU
#define GPIO_FUNC160_IN_SEL_S  0
/** GPIO_FUNC160_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC160_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC160_IN_INV_SEL_M  (GPIO_FUNC160_IN_INV_SEL_V << GPIO_FUNC160_IN_INV_SEL_S)
#define GPIO_FUNC160_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC160_IN_INV_SEL_S  6
/** GPIO_SIG160_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG160_IN_SEL    (BIT(7))
#define GPIO_SIG160_IN_SEL_M  (GPIO_SIG160_IN_SEL_V << GPIO_SIG160_IN_SEL_S)
#define GPIO_SIG160_IN_SEL_V  0x00000001U
#define GPIO_SIG160_IN_SEL_S  7

/** GPIO_FUNC161_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC161_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3dc)
/** GPIO_FUNC161_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC161_IN_SEL    0x0000003FU
#define GPIO_FUNC161_IN_SEL_M  (GPIO_FUNC161_IN_SEL_V << GPIO_FUNC161_IN_SEL_S)
#define GPIO_FUNC161_IN_SEL_V  0x0000003FU
#define GPIO_FUNC161_IN_SEL_S  0
/** GPIO_FUNC161_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC161_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC161_IN_INV_SEL_M  (GPIO_FUNC161_IN_INV_SEL_V << GPIO_FUNC161_IN_INV_SEL_S)
#define GPIO_FUNC161_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC161_IN_INV_SEL_S  6
/** GPIO_SIG161_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG161_IN_SEL    (BIT(7))
#define GPIO_SIG161_IN_SEL_M  (GPIO_SIG161_IN_SEL_V << GPIO_SIG161_IN_SEL_S)
#define GPIO_SIG161_IN_SEL_V  0x00000001U
#define GPIO_SIG161_IN_SEL_S  7

/** GPIO_FUNC162_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC162_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e0)
/** GPIO_FUNC162_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC162_IN_SEL    0x0000003FU
#define GPIO_FUNC162_IN_SEL_M  (GPIO_FUNC162_IN_SEL_V << GPIO_FUNC162_IN_SEL_S)
#define GPIO_FUNC162_IN_SEL_V  0x0000003FU
#define GPIO_FUNC162_IN_SEL_S  0
/** GPIO_FUNC162_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC162_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC162_IN_INV_SEL_M  (GPIO_FUNC162_IN_INV_SEL_V << GPIO_FUNC162_IN_INV_SEL_S)
#define GPIO_FUNC162_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC162_IN_INV_SEL_S  6
/** GPIO_SIG162_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG162_IN_SEL    (BIT(7))
#define GPIO_SIG162_IN_SEL_M  (GPIO_SIG162_IN_SEL_V << GPIO_SIG162_IN_SEL_S)
#define GPIO_SIG162_IN_SEL_V  0x00000001U
#define GPIO_SIG162_IN_SEL_S  7

/** GPIO_FUNC163_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC163_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e4)
/** GPIO_FUNC163_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC163_IN_SEL    0x0000003FU
#define GPIO_FUNC163_IN_SEL_M  (GPIO_FUNC163_IN_SEL_V << GPIO_FUNC163_IN_SEL_S)
#define GPIO_FUNC163_IN_SEL_V  0x0000003FU
#define GPIO_FUNC163_IN_SEL_S  0
/** GPIO_FUNC163_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC163_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC163_IN_INV_SEL_M  (GPIO_FUNC163_IN_INV_SEL_V << GPIO_FUNC163_IN_INV_SEL_S)
#define GPIO_FUNC163_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC163_IN_INV_SEL_S  6
/** GPIO_SIG163_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG163_IN_SEL    (BIT(7))
#define GPIO_SIG163_IN_SEL_M  (GPIO_SIG163_IN_SEL_V << GPIO_SIG163_IN_SEL_S)
#define GPIO_SIG163_IN_SEL_V  0x00000001U
#define GPIO_SIG163_IN_SEL_S  7

/** GPIO_FUNC164_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC164_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e8)
/** GPIO_FUNC164_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC164_IN_SEL    0x0000003FU
#define GPIO_FUNC164_IN_SEL_M  (GPIO_FUNC164_IN_SEL_V << GPIO_FUNC164_IN_SEL_S)
#define GPIO_FUNC164_IN_SEL_V  0x0000003FU
#define GPIO_FUNC164_IN_SEL_S  0
/** GPIO_FUNC164_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC164_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC164_IN_INV_SEL_M  (GPIO_FUNC164_IN_INV_SEL_V << GPIO_FUNC164_IN_INV_SEL_S)
#define GPIO_FUNC164_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC164_IN_INV_SEL_S  6
/** GPIO_SIG164_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG164_IN_SEL    (BIT(7))
#define GPIO_SIG164_IN_SEL_M  (GPIO_SIG164_IN_SEL_V << GPIO_SIG164_IN_SEL_S)
#define GPIO_SIG164_IN_SEL_V  0x00000001U
#define GPIO_SIG164_IN_SEL_S  7

/** GPIO_FUNC165_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC165_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3ec)
/** GPIO_FUNC165_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC165_IN_SEL    0x0000003FU
#define GPIO_FUNC165_IN_SEL_M  (GPIO_FUNC165_IN_SEL_V << GPIO_FUNC165_IN_SEL_S)
#define GPIO_FUNC165_IN_SEL_V  0x0000003FU
#define GPIO_FUNC165_IN_SEL_S  0
/** GPIO_FUNC165_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC165_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC165_IN_INV_SEL_M  (GPIO_FUNC165_IN_INV_SEL_V << GPIO_FUNC165_IN_INV_SEL_S)
#define GPIO_FUNC165_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC165_IN_INV_SEL_S  6
/** GPIO_SIG165_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG165_IN_SEL    (BIT(7))
#define GPIO_SIG165_IN_SEL_M  (GPIO_SIG165_IN_SEL_V << GPIO_SIG165_IN_SEL_S)
#define GPIO_SIG165_IN_SEL_V  0x00000001U
#define GPIO_SIG165_IN_SEL_S  7

/** GPIO_FUNC166_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC166_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f0)
/** GPIO_FUNC166_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC166_IN_SEL    0x0000003FU
#define GPIO_FUNC166_IN_SEL_M  (GPIO_FUNC166_IN_SEL_V << GPIO_FUNC166_IN_SEL_S)
#define GPIO_FUNC166_IN_SEL_V  0x0000003FU
#define GPIO_FUNC166_IN_SEL_S  0
/** GPIO_FUNC166_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC166_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC166_IN_INV_SEL_M  (GPIO_FUNC166_IN_INV_SEL_V << GPIO_FUNC166_IN_INV_SEL_S)
#define GPIO_FUNC166_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC166_IN_INV_SEL_S  6
/** GPIO_SIG166_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG166_IN_SEL    (BIT(7))
#define GPIO_SIG166_IN_SEL_M  (GPIO_SIG166_IN_SEL_V << GPIO_SIG166_IN_SEL_S)
#define GPIO_SIG166_IN_SEL_V  0x00000001U
#define GPIO_SIG166_IN_SEL_S  7

/** GPIO_FUNC167_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC167_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f4)
/** GPIO_FUNC167_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC167_IN_SEL    0x0000003FU
#define GPIO_FUNC167_IN_SEL_M  (GPIO_FUNC167_IN_SEL_V << GPIO_FUNC167_IN_SEL_S)
#define GPIO_FUNC167_IN_SEL_V  0x0000003FU
#define GPIO_FUNC167_IN_SEL_S  0
/** GPIO_FUNC167_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC167_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC167_IN_INV_SEL_M  (GPIO_FUNC167_IN_INV_SEL_V << GPIO_FUNC167_IN_INV_SEL_S)
#define GPIO_FUNC167_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC167_IN_INV_SEL_S  6
/** GPIO_SIG167_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG167_IN_SEL    (BIT(7))
#define GPIO_SIG167_IN_SEL_M  (GPIO_SIG167_IN_SEL_V << GPIO_SIG167_IN_SEL_S)
#define GPIO_SIG167_IN_SEL_V  0x00000001U
#define GPIO_SIG167_IN_SEL_S  7

/** GPIO_FUNC168_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC168_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f8)
/** GPIO_FUNC168_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC168_IN_SEL    0x0000003FU
#define GPIO_FUNC168_IN_SEL_M  (GPIO_FUNC168_IN_SEL_V << GPIO_FUNC168_IN_SEL_S)
#define GPIO_FUNC168_IN_SEL_V  0x0000003FU
#define GPIO_FUNC168_IN_SEL_S  0
/** GPIO_FUNC168_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC168_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC168_IN_INV_SEL_M  (GPIO_FUNC168_IN_INV_SEL_V << GPIO_FUNC168_IN_INV_SEL_S)
#define GPIO_FUNC168_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC168_IN_INV_SEL_S  6
/** GPIO_SIG168_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG168_IN_SEL    (BIT(7))
#define GPIO_SIG168_IN_SEL_M  (GPIO_SIG168_IN_SEL_V << GPIO_SIG168_IN_SEL_S)
#define GPIO_SIG168_IN_SEL_V  0x00000001U
#define GPIO_SIG168_IN_SEL_S  7

/** GPIO_FUNC169_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC169_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3fc)
/** GPIO_FUNC169_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC169_IN_SEL    0x0000003FU
#define GPIO_FUNC169_IN_SEL_M  (GPIO_FUNC169_IN_SEL_V << GPIO_FUNC169_IN_SEL_S)
#define GPIO_FUNC169_IN_SEL_V  0x0000003FU
#define GPIO_FUNC169_IN_SEL_S  0
/** GPIO_FUNC169_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC169_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC169_IN_INV_SEL_M  (GPIO_FUNC169_IN_INV_SEL_V << GPIO_FUNC169_IN_INV_SEL_S)
#define GPIO_FUNC169_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC169_IN_INV_SEL_S  6
/** GPIO_SIG169_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG169_IN_SEL    (BIT(7))
#define GPIO_SIG169_IN_SEL_M  (GPIO_SIG169_IN_SEL_V << GPIO_SIG169_IN_SEL_S)
#define GPIO_SIG169_IN_SEL_V  0x00000001U
#define GPIO_SIG169_IN_SEL_S  7

/** GPIO_FUNC170_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC170_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x400)
/** GPIO_FUNC170_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC170_IN_SEL    0x0000003FU
#define GPIO_FUNC170_IN_SEL_M  (GPIO_FUNC170_IN_SEL_V << GPIO_FUNC170_IN_SEL_S)
#define GPIO_FUNC170_IN_SEL_V  0x0000003FU
#define GPIO_FUNC170_IN_SEL_S  0
/** GPIO_FUNC170_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC170_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC170_IN_INV_SEL_M  (GPIO_FUNC170_IN_INV_SEL_V << GPIO_FUNC170_IN_INV_SEL_S)
#define GPIO_FUNC170_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC170_IN_INV_SEL_S  6
/** GPIO_SIG170_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG170_IN_SEL    (BIT(7))
#define GPIO_SIG170_IN_SEL_M  (GPIO_SIG170_IN_SEL_V << GPIO_SIG170_IN_SEL_S)
#define GPIO_SIG170_IN_SEL_V  0x00000001U
#define GPIO_SIG170_IN_SEL_S  7

/** GPIO_FUNC171_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC171_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x404)
/** GPIO_FUNC171_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC171_IN_SEL    0x0000003FU
#define GPIO_FUNC171_IN_SEL_M  (GPIO_FUNC171_IN_SEL_V << GPIO_FUNC171_IN_SEL_S)
#define GPIO_FUNC171_IN_SEL_V  0x0000003FU
#define GPIO_FUNC171_IN_SEL_S  0
/** GPIO_FUNC171_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC171_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC171_IN_INV_SEL_M  (GPIO_FUNC171_IN_INV_SEL_V << GPIO_FUNC171_IN_INV_SEL_S)
#define GPIO_FUNC171_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC171_IN_INV_SEL_S  6
/** GPIO_SIG171_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG171_IN_SEL    (BIT(7))
#define GPIO_SIG171_IN_SEL_M  (GPIO_SIG171_IN_SEL_V << GPIO_SIG171_IN_SEL_S)
#define GPIO_SIG171_IN_SEL_V  0x00000001U
#define GPIO_SIG171_IN_SEL_S  7

/** GPIO_FUNC172_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC172_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x408)
/** GPIO_FUNC172_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC172_IN_SEL    0x0000003FU
#define GPIO_FUNC172_IN_SEL_M  (GPIO_FUNC172_IN_SEL_V << GPIO_FUNC172_IN_SEL_S)
#define GPIO_FUNC172_IN_SEL_V  0x0000003FU
#define GPIO_FUNC172_IN_SEL_S  0
/** GPIO_FUNC172_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC172_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC172_IN_INV_SEL_M  (GPIO_FUNC172_IN_INV_SEL_V << GPIO_FUNC172_IN_INV_SEL_S)
#define GPIO_FUNC172_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC172_IN_INV_SEL_S  6
/** GPIO_SIG172_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG172_IN_SEL    (BIT(7))
#define GPIO_SIG172_IN_SEL_M  (GPIO_SIG172_IN_SEL_V << GPIO_SIG172_IN_SEL_S)
#define GPIO_SIG172_IN_SEL_V  0x00000001U
#define GPIO_SIG172_IN_SEL_S  7

/** GPIO_FUNC173_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC173_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x40c)
/** GPIO_FUNC173_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC173_IN_SEL    0x0000003FU
#define GPIO_FUNC173_IN_SEL_M  (GPIO_FUNC173_IN_SEL_V << GPIO_FUNC173_IN_SEL_S)
#define GPIO_FUNC173_IN_SEL_V  0x0000003FU
#define GPIO_FUNC173_IN_SEL_S  0
/** GPIO_FUNC173_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC173_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC173_IN_INV_SEL_M  (GPIO_FUNC173_IN_INV_SEL_V << GPIO_FUNC173_IN_INV_SEL_S)
#define GPIO_FUNC173_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC173_IN_INV_SEL_S  6
/** GPIO_SIG173_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG173_IN_SEL    (BIT(7))
#define GPIO_SIG173_IN_SEL_M  (GPIO_SIG173_IN_SEL_V << GPIO_SIG173_IN_SEL_S)
#define GPIO_SIG173_IN_SEL_V  0x00000001U
#define GPIO_SIG173_IN_SEL_S  7

/** GPIO_FUNC174_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC174_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x410)
/** GPIO_FUNC174_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC174_IN_SEL    0x0000003FU
#define GPIO_FUNC174_IN_SEL_M  (GPIO_FUNC174_IN_SEL_V << GPIO_FUNC174_IN_SEL_S)
#define GPIO_FUNC174_IN_SEL_V  0x0000003FU
#define GPIO_FUNC174_IN_SEL_S  0
/** GPIO_FUNC174_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC174_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC174_IN_INV_SEL_M  (GPIO_FUNC174_IN_INV_SEL_V << GPIO_FUNC174_IN_INV_SEL_S)
#define GPIO_FUNC174_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC174_IN_INV_SEL_S  6
/** GPIO_SIG174_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG174_IN_SEL    (BIT(7))
#define GPIO_SIG174_IN_SEL_M  (GPIO_SIG174_IN_SEL_V << GPIO_SIG174_IN_SEL_S)
#define GPIO_SIG174_IN_SEL_V  0x00000001U
#define GPIO_SIG174_IN_SEL_S  7

/** GPIO_FUNC175_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC175_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x414)
/** GPIO_FUNC175_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC175_IN_SEL    0x0000003FU
#define GPIO_FUNC175_IN_SEL_M  (GPIO_FUNC175_IN_SEL_V << GPIO_FUNC175_IN_SEL_S)
#define GPIO_FUNC175_IN_SEL_V  0x0000003FU
#define GPIO_FUNC175_IN_SEL_S  0
/** GPIO_FUNC175_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC175_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC175_IN_INV_SEL_M  (GPIO_FUNC175_IN_INV_SEL_V << GPIO_FUNC175_IN_INV_SEL_S)
#define GPIO_FUNC175_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC175_IN_INV_SEL_S  6
/** GPIO_SIG175_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG175_IN_SEL    (BIT(7))
#define GPIO_SIG175_IN_SEL_M  (GPIO_SIG175_IN_SEL_V << GPIO_SIG175_IN_SEL_S)
#define GPIO_SIG175_IN_SEL_V  0x00000001U
#define GPIO_SIG175_IN_SEL_S  7

/** GPIO_FUNC176_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC176_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x418)
/** GPIO_FUNC176_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC176_IN_SEL    0x0000003FU
#define GPIO_FUNC176_IN_SEL_M  (GPIO_FUNC176_IN_SEL_V << GPIO_FUNC176_IN_SEL_S)
#define GPIO_FUNC176_IN_SEL_V  0x0000003FU
#define GPIO_FUNC176_IN_SEL_S  0
/** GPIO_FUNC176_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC176_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC176_IN_INV_SEL_M  (GPIO_FUNC176_IN_INV_SEL_V << GPIO_FUNC176_IN_INV_SEL_S)
#define GPIO_FUNC176_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC176_IN_INV_SEL_S  6
/** GPIO_SIG176_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG176_IN_SEL    (BIT(7))
#define GPIO_SIG176_IN_SEL_M  (GPIO_SIG176_IN_SEL_V << GPIO_SIG176_IN_SEL_S)
#define GPIO_SIG176_IN_SEL_V  0x00000001U
#define GPIO_SIG176_IN_SEL_S  7

/** GPIO_FUNC177_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC177_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x41c)
/** GPIO_FUNC177_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC177_IN_SEL    0x0000003FU
#define GPIO_FUNC177_IN_SEL_M  (GPIO_FUNC177_IN_SEL_V << GPIO_FUNC177_IN_SEL_S)
#define GPIO_FUNC177_IN_SEL_V  0x0000003FU
#define GPIO_FUNC177_IN_SEL_S  0
/** GPIO_FUNC177_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC177_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC177_IN_INV_SEL_M  (GPIO_FUNC177_IN_INV_SEL_V << GPIO_FUNC177_IN_INV_SEL_S)
#define GPIO_FUNC177_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC177_IN_INV_SEL_S  6
/** GPIO_SIG177_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG177_IN_SEL    (BIT(7))
#define GPIO_SIG177_IN_SEL_M  (GPIO_SIG177_IN_SEL_V << GPIO_SIG177_IN_SEL_S)
#define GPIO_SIG177_IN_SEL_V  0x00000001U
#define GPIO_SIG177_IN_SEL_S  7

/** GPIO_FUNC178_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC178_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x420)
/** GPIO_FUNC178_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC178_IN_SEL    0x0000003FU
#define GPIO_FUNC178_IN_SEL_M  (GPIO_FUNC178_IN_SEL_V << GPIO_FUNC178_IN_SEL_S)
#define GPIO_FUNC178_IN_SEL_V  0x0000003FU
#define GPIO_FUNC178_IN_SEL_S  0
/** GPIO_FUNC178_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC178_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC178_IN_INV_SEL_M  (GPIO_FUNC178_IN_INV_SEL_V << GPIO_FUNC178_IN_INV_SEL_S)
#define GPIO_FUNC178_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC178_IN_INV_SEL_S  6
/** GPIO_SIG178_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG178_IN_SEL    (BIT(7))
#define GPIO_SIG178_IN_SEL_M  (GPIO_SIG178_IN_SEL_V << GPIO_SIG178_IN_SEL_S)
#define GPIO_SIG178_IN_SEL_V  0x00000001U
#define GPIO_SIG178_IN_SEL_S  7

/** GPIO_FUNC179_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC179_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x424)
/** GPIO_FUNC179_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC179_IN_SEL    0x0000003FU
#define GPIO_FUNC179_IN_SEL_M  (GPIO_FUNC179_IN_SEL_V << GPIO_FUNC179_IN_SEL_S)
#define GPIO_FUNC179_IN_SEL_V  0x0000003FU
#define GPIO_FUNC179_IN_SEL_S  0
/** GPIO_FUNC179_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC179_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC179_IN_INV_SEL_M  (GPIO_FUNC179_IN_INV_SEL_V << GPIO_FUNC179_IN_INV_SEL_S)
#define GPIO_FUNC179_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC179_IN_INV_SEL_S  6
/** GPIO_SIG179_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG179_IN_SEL    (BIT(7))
#define GPIO_SIG179_IN_SEL_M  (GPIO_SIG179_IN_SEL_V << GPIO_SIG179_IN_SEL_S)
#define GPIO_SIG179_IN_SEL_V  0x00000001U
#define GPIO_SIG179_IN_SEL_S  7

/** GPIO_FUNC180_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC180_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x428)
/** GPIO_FUNC180_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC180_IN_SEL    0x0000003FU
#define GPIO_FUNC180_IN_SEL_M  (GPIO_FUNC180_IN_SEL_V << GPIO_FUNC180_IN_SEL_S)
#define GPIO_FUNC180_IN_SEL_V  0x0000003FU
#define GPIO_FUNC180_IN_SEL_S  0
/** GPIO_FUNC180_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC180_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC180_IN_INV_SEL_M  (GPIO_FUNC180_IN_INV_SEL_V << GPIO_FUNC180_IN_INV_SEL_S)
#define GPIO_FUNC180_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC180_IN_INV_SEL_S  6
/** GPIO_SIG180_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG180_IN_SEL    (BIT(7))
#define GPIO_SIG180_IN_SEL_M  (GPIO_SIG180_IN_SEL_V << GPIO_SIG180_IN_SEL_S)
#define GPIO_SIG180_IN_SEL_V  0x00000001U
#define GPIO_SIG180_IN_SEL_S  7

/** GPIO_FUNC181_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC181_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x42c)
/** GPIO_FUNC181_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC181_IN_SEL    0x0000003FU
#define GPIO_FUNC181_IN_SEL_M  (GPIO_FUNC181_IN_SEL_V << GPIO_FUNC181_IN_SEL_S)
#define GPIO_FUNC181_IN_SEL_V  0x0000003FU
#define GPIO_FUNC181_IN_SEL_S  0
/** GPIO_FUNC181_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC181_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC181_IN_INV_SEL_M  (GPIO_FUNC181_IN_INV_SEL_V << GPIO_FUNC181_IN_INV_SEL_S)
#define GPIO_FUNC181_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC181_IN_INV_SEL_S  6
/** GPIO_SIG181_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG181_IN_SEL    (BIT(7))
#define GPIO_SIG181_IN_SEL_M  (GPIO_SIG181_IN_SEL_V << GPIO_SIG181_IN_SEL_S)
#define GPIO_SIG181_IN_SEL_V  0x00000001U
#define GPIO_SIG181_IN_SEL_S  7

/** GPIO_FUNC182_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC182_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x430)
/** GPIO_FUNC182_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC182_IN_SEL    0x0000003FU
#define GPIO_FUNC182_IN_SEL_M  (GPIO_FUNC182_IN_SEL_V << GPIO_FUNC182_IN_SEL_S)
#define GPIO_FUNC182_IN_SEL_V  0x0000003FU
#define GPIO_FUNC182_IN_SEL_S  0
/** GPIO_FUNC182_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC182_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC182_IN_INV_SEL_M  (GPIO_FUNC182_IN_INV_SEL_V << GPIO_FUNC182_IN_INV_SEL_S)
#define GPIO_FUNC182_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC182_IN_INV_SEL_S  6
/** GPIO_SIG182_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG182_IN_SEL    (BIT(7))
#define GPIO_SIG182_IN_SEL_M  (GPIO_SIG182_IN_SEL_V << GPIO_SIG182_IN_SEL_S)
#define GPIO_SIG182_IN_SEL_V  0x00000001U
#define GPIO_SIG182_IN_SEL_S  7

/** GPIO_FUNC183_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC183_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x434)
/** GPIO_FUNC183_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC183_IN_SEL    0x0000003FU
#define GPIO_FUNC183_IN_SEL_M  (GPIO_FUNC183_IN_SEL_V << GPIO_FUNC183_IN_SEL_S)
#define GPIO_FUNC183_IN_SEL_V  0x0000003FU
#define GPIO_FUNC183_IN_SEL_S  0
/** GPIO_FUNC183_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC183_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC183_IN_INV_SEL_M  (GPIO_FUNC183_IN_INV_SEL_V << GPIO_FUNC183_IN_INV_SEL_S)
#define GPIO_FUNC183_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC183_IN_INV_SEL_S  6
/** GPIO_SIG183_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG183_IN_SEL    (BIT(7))
#define GPIO_SIG183_IN_SEL_M  (GPIO_SIG183_IN_SEL_V << GPIO_SIG183_IN_SEL_S)
#define GPIO_SIG183_IN_SEL_V  0x00000001U
#define GPIO_SIG183_IN_SEL_S  7

/** GPIO_FUNC184_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC184_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x438)
/** GPIO_FUNC184_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC184_IN_SEL    0x0000003FU
#define GPIO_FUNC184_IN_SEL_M  (GPIO_FUNC184_IN_SEL_V << GPIO_FUNC184_IN_SEL_S)
#define GPIO_FUNC184_IN_SEL_V  0x0000003FU
#define GPIO_FUNC184_IN_SEL_S  0
/** GPIO_FUNC184_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC184_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC184_IN_INV_SEL_M  (GPIO_FUNC184_IN_INV_SEL_V << GPIO_FUNC184_IN_INV_SEL_S)
#define GPIO_FUNC184_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC184_IN_INV_SEL_S  6
/** GPIO_SIG184_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG184_IN_SEL    (BIT(7))
#define GPIO_SIG184_IN_SEL_M  (GPIO_SIG184_IN_SEL_V << GPIO_SIG184_IN_SEL_S)
#define GPIO_SIG184_IN_SEL_V  0x00000001U
#define GPIO_SIG184_IN_SEL_S  7

/** GPIO_FUNC185_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC185_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x43c)
/** GPIO_FUNC185_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC185_IN_SEL    0x0000003FU
#define GPIO_FUNC185_IN_SEL_M  (GPIO_FUNC185_IN_SEL_V << GPIO_FUNC185_IN_SEL_S)
#define GPIO_FUNC185_IN_SEL_V  0x0000003FU
#define GPIO_FUNC185_IN_SEL_S  0
/** GPIO_FUNC185_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC185_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC185_IN_INV_SEL_M  (GPIO_FUNC185_IN_INV_SEL_V << GPIO_FUNC185_IN_INV_SEL_S)
#define GPIO_FUNC185_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC185_IN_INV_SEL_S  6
/** GPIO_SIG185_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG185_IN_SEL    (BIT(7))
#define GPIO_SIG185_IN_SEL_M  (GPIO_SIG185_IN_SEL_V << GPIO_SIG185_IN_SEL_S)
#define GPIO_SIG185_IN_SEL_V  0x00000001U
#define GPIO_SIG185_IN_SEL_S  7

/** GPIO_FUNC186_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC186_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x440)
/** GPIO_FUNC186_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC186_IN_SEL    0x0000003FU
#define GPIO_FUNC186_IN_SEL_M  (GPIO_FUNC186_IN_SEL_V << GPIO_FUNC186_IN_SEL_S)
#define GPIO_FUNC186_IN_SEL_V  0x0000003FU
#define GPIO_FUNC186_IN_SEL_S  0
/** GPIO_FUNC186_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC186_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC186_IN_INV_SEL_M  (GPIO_FUNC186_IN_INV_SEL_V << GPIO_FUNC186_IN_INV_SEL_S)
#define GPIO_FUNC186_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC186_IN_INV_SEL_S  6
/** GPIO_SIG186_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG186_IN_SEL    (BIT(7))
#define GPIO_SIG186_IN_SEL_M  (GPIO_SIG186_IN_SEL_V << GPIO_SIG186_IN_SEL_S)
#define GPIO_SIG186_IN_SEL_V  0x00000001U
#define GPIO_SIG186_IN_SEL_S  7

/** GPIO_FUNC187_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC187_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x444)
/** GPIO_FUNC187_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC187_IN_SEL    0x0000003FU
#define GPIO_FUNC187_IN_SEL_M  (GPIO_FUNC187_IN_SEL_V << GPIO_FUNC187_IN_SEL_S)
#define GPIO_FUNC187_IN_SEL_V  0x0000003FU
#define GPIO_FUNC187_IN_SEL_S  0
/** GPIO_FUNC187_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC187_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC187_IN_INV_SEL_M  (GPIO_FUNC187_IN_INV_SEL_V << GPIO_FUNC187_IN_INV_SEL_S)
#define GPIO_FUNC187_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC187_IN_INV_SEL_S  6
/** GPIO_SIG187_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG187_IN_SEL    (BIT(7))
#define GPIO_SIG187_IN_SEL_M  (GPIO_SIG187_IN_SEL_V << GPIO_SIG187_IN_SEL_S)
#define GPIO_SIG187_IN_SEL_V  0x00000001U
#define GPIO_SIG187_IN_SEL_S  7

/** GPIO_FUNC188_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC188_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x448)
/** GPIO_FUNC188_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC188_IN_SEL    0x0000003FU
#define GPIO_FUNC188_IN_SEL_M  (GPIO_FUNC188_IN_SEL_V << GPIO_FUNC188_IN_SEL_S)
#define GPIO_FUNC188_IN_SEL_V  0x0000003FU
#define GPIO_FUNC188_IN_SEL_S  0
/** GPIO_FUNC188_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC188_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC188_IN_INV_SEL_M  (GPIO_FUNC188_IN_INV_SEL_V << GPIO_FUNC188_IN_INV_SEL_S)
#define GPIO_FUNC188_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC188_IN_INV_SEL_S  6
/** GPIO_SIG188_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG188_IN_SEL    (BIT(7))
#define GPIO_SIG188_IN_SEL_M  (GPIO_SIG188_IN_SEL_V << GPIO_SIG188_IN_SEL_S)
#define GPIO_SIG188_IN_SEL_V  0x00000001U
#define GPIO_SIG188_IN_SEL_S  7

/** GPIO_FUNC189_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC189_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x44c)
/** GPIO_FUNC189_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC189_IN_SEL    0x0000003FU
#define GPIO_FUNC189_IN_SEL_M  (GPIO_FUNC189_IN_SEL_V << GPIO_FUNC189_IN_SEL_S)
#define GPIO_FUNC189_IN_SEL_V  0x0000003FU
#define GPIO_FUNC189_IN_SEL_S  0
/** GPIO_FUNC189_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC189_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC189_IN_INV_SEL_M  (GPIO_FUNC189_IN_INV_SEL_V << GPIO_FUNC189_IN_INV_SEL_S)
#define GPIO_FUNC189_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC189_IN_INV_SEL_S  6
/** GPIO_SIG189_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG189_IN_SEL    (BIT(7))
#define GPIO_SIG189_IN_SEL_M  (GPIO_SIG189_IN_SEL_V << GPIO_SIG189_IN_SEL_S)
#define GPIO_SIG189_IN_SEL_V  0x00000001U
#define GPIO_SIG189_IN_SEL_S  7

/** GPIO_FUNC190_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC190_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x450)
/** GPIO_FUNC190_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC190_IN_SEL    0x0000003FU
#define GPIO_FUNC190_IN_SEL_M  (GPIO_FUNC190_IN_SEL_V << GPIO_FUNC190_IN_SEL_S)
#define GPIO_FUNC190_IN_SEL_V  0x0000003FU
#define GPIO_FUNC190_IN_SEL_S  0
/** GPIO_FUNC190_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC190_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC190_IN_INV_SEL_M  (GPIO_FUNC190_IN_INV_SEL_V << GPIO_FUNC190_IN_INV_SEL_S)
#define GPIO_FUNC190_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC190_IN_INV_SEL_S  6
/** GPIO_SIG190_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG190_IN_SEL    (BIT(7))
#define GPIO_SIG190_IN_SEL_M  (GPIO_SIG190_IN_SEL_V << GPIO_SIG190_IN_SEL_S)
#define GPIO_SIG190_IN_SEL_V  0x00000001U
#define GPIO_SIG190_IN_SEL_S  7

/** GPIO_FUNC191_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC191_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x454)
/** GPIO_FUNC191_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC191_IN_SEL    0x0000003FU
#define GPIO_FUNC191_IN_SEL_M  (GPIO_FUNC191_IN_SEL_V << GPIO_FUNC191_IN_SEL_S)
#define GPIO_FUNC191_IN_SEL_V  0x0000003FU
#define GPIO_FUNC191_IN_SEL_S  0
/** GPIO_FUNC191_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC191_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC191_IN_INV_SEL_M  (GPIO_FUNC191_IN_INV_SEL_V << GPIO_FUNC191_IN_INV_SEL_S)
#define GPIO_FUNC191_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC191_IN_INV_SEL_S  6
/** GPIO_SIG191_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG191_IN_SEL    (BIT(7))
#define GPIO_SIG191_IN_SEL_M  (GPIO_SIG191_IN_SEL_V << GPIO_SIG191_IN_SEL_S)
#define GPIO_SIG191_IN_SEL_V  0x00000001U
#define GPIO_SIG191_IN_SEL_S  7

/** GPIO_FUNC192_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC192_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x458)
/** GPIO_FUNC192_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC192_IN_SEL    0x0000003FU
#define GPIO_FUNC192_IN_SEL_M  (GPIO_FUNC192_IN_SEL_V << GPIO_FUNC192_IN_SEL_S)
#define GPIO_FUNC192_IN_SEL_V  0x0000003FU
#define GPIO_FUNC192_IN_SEL_S  0
/** GPIO_FUNC192_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC192_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC192_IN_INV_SEL_M  (GPIO_FUNC192_IN_INV_SEL_V << GPIO_FUNC192_IN_INV_SEL_S)
#define GPIO_FUNC192_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC192_IN_INV_SEL_S  6
/** GPIO_SIG192_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG192_IN_SEL    (BIT(7))
#define GPIO_SIG192_IN_SEL_M  (GPIO_SIG192_IN_SEL_V << GPIO_SIG192_IN_SEL_S)
#define GPIO_SIG192_IN_SEL_V  0x00000001U
#define GPIO_SIG192_IN_SEL_S  7

/** GPIO_FUNC193_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC193_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x45c)
/** GPIO_FUNC193_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC193_IN_SEL    0x0000003FU
#define GPIO_FUNC193_IN_SEL_M  (GPIO_FUNC193_IN_SEL_V << GPIO_FUNC193_IN_SEL_S)
#define GPIO_FUNC193_IN_SEL_V  0x0000003FU
#define GPIO_FUNC193_IN_SEL_S  0
/** GPIO_FUNC193_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC193_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC193_IN_INV_SEL_M  (GPIO_FUNC193_IN_INV_SEL_V << GPIO_FUNC193_IN_INV_SEL_S)
#define GPIO_FUNC193_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC193_IN_INV_SEL_S  6
/** GPIO_SIG193_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG193_IN_SEL    (BIT(7))
#define GPIO_SIG193_IN_SEL_M  (GPIO_SIG193_IN_SEL_V << GPIO_SIG193_IN_SEL_S)
#define GPIO_SIG193_IN_SEL_V  0x00000001U
#define GPIO_SIG193_IN_SEL_S  7

/** GPIO_FUNC194_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC194_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x460)
/** GPIO_FUNC194_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC194_IN_SEL    0x0000003FU
#define GPIO_FUNC194_IN_SEL_M  (GPIO_FUNC194_IN_SEL_V << GPIO_FUNC194_IN_SEL_S)
#define GPIO_FUNC194_IN_SEL_V  0x0000003FU
#define GPIO_FUNC194_IN_SEL_S  0
/** GPIO_FUNC194_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC194_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC194_IN_INV_SEL_M  (GPIO_FUNC194_IN_INV_SEL_V << GPIO_FUNC194_IN_INV_SEL_S)
#define GPIO_FUNC194_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC194_IN_INV_SEL_S  6
/** GPIO_SIG194_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG194_IN_SEL    (BIT(7))
#define GPIO_SIG194_IN_SEL_M  (GPIO_SIG194_IN_SEL_V << GPIO_SIG194_IN_SEL_S)
#define GPIO_SIG194_IN_SEL_V  0x00000001U
#define GPIO_SIG194_IN_SEL_S  7

/** GPIO_FUNC195_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC195_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x464)
/** GPIO_FUNC195_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC195_IN_SEL    0x0000003FU
#define GPIO_FUNC195_IN_SEL_M  (GPIO_FUNC195_IN_SEL_V << GPIO_FUNC195_IN_SEL_S)
#define GPIO_FUNC195_IN_SEL_V  0x0000003FU
#define GPIO_FUNC195_IN_SEL_S  0
/** GPIO_FUNC195_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC195_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC195_IN_INV_SEL_M  (GPIO_FUNC195_IN_INV_SEL_V << GPIO_FUNC195_IN_INV_SEL_S)
#define GPIO_FUNC195_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC195_IN_INV_SEL_S  6
/** GPIO_SIG195_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG195_IN_SEL    (BIT(7))
#define GPIO_SIG195_IN_SEL_M  (GPIO_SIG195_IN_SEL_V << GPIO_SIG195_IN_SEL_S)
#define GPIO_SIG195_IN_SEL_V  0x00000001U
#define GPIO_SIG195_IN_SEL_S  7

/** GPIO_FUNC196_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC196_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x468)
/** GPIO_FUNC196_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC196_IN_SEL    0x0000003FU
#define GPIO_FUNC196_IN_SEL_M  (GPIO_FUNC196_IN_SEL_V << GPIO_FUNC196_IN_SEL_S)
#define GPIO_FUNC196_IN_SEL_V  0x0000003FU
#define GPIO_FUNC196_IN_SEL_S  0
/** GPIO_FUNC196_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC196_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC196_IN_INV_SEL_M  (GPIO_FUNC196_IN_INV_SEL_V << GPIO_FUNC196_IN_INV_SEL_S)
#define GPIO_FUNC196_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC196_IN_INV_SEL_S  6
/** GPIO_SIG196_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG196_IN_SEL    (BIT(7))
#define GPIO_SIG196_IN_SEL_M  (GPIO_SIG196_IN_SEL_V << GPIO_SIG196_IN_SEL_S)
#define GPIO_SIG196_IN_SEL_V  0x00000001U
#define GPIO_SIG196_IN_SEL_S  7

/** GPIO_FUNC197_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC197_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x46c)
/** GPIO_FUNC197_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC197_IN_SEL    0x0000003FU
#define GPIO_FUNC197_IN_SEL_M  (GPIO_FUNC197_IN_SEL_V << GPIO_FUNC197_IN_SEL_S)
#define GPIO_FUNC197_IN_SEL_V  0x0000003FU
#define GPIO_FUNC197_IN_SEL_S  0
/** GPIO_FUNC197_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC197_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC197_IN_INV_SEL_M  (GPIO_FUNC197_IN_INV_SEL_V << GPIO_FUNC197_IN_INV_SEL_S)
#define GPIO_FUNC197_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC197_IN_INV_SEL_S  6
/** GPIO_SIG197_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG197_IN_SEL    (BIT(7))
#define GPIO_SIG197_IN_SEL_M  (GPIO_SIG197_IN_SEL_V << GPIO_SIG197_IN_SEL_S)
#define GPIO_SIG197_IN_SEL_V  0x00000001U
#define GPIO_SIG197_IN_SEL_S  7

/** GPIO_FUNC198_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC198_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x470)
/** GPIO_FUNC198_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC198_IN_SEL    0x0000003FU
#define GPIO_FUNC198_IN_SEL_M  (GPIO_FUNC198_IN_SEL_V << GPIO_FUNC198_IN_SEL_S)
#define GPIO_FUNC198_IN_SEL_V  0x0000003FU
#define GPIO_FUNC198_IN_SEL_S  0
/** GPIO_FUNC198_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC198_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC198_IN_INV_SEL_M  (GPIO_FUNC198_IN_INV_SEL_V << GPIO_FUNC198_IN_INV_SEL_S)
#define GPIO_FUNC198_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC198_IN_INV_SEL_S  6
/** GPIO_SIG198_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG198_IN_SEL    (BIT(7))
#define GPIO_SIG198_IN_SEL_M  (GPIO_SIG198_IN_SEL_V << GPIO_SIG198_IN_SEL_S)
#define GPIO_SIG198_IN_SEL_V  0x00000001U
#define GPIO_SIG198_IN_SEL_S  7

/** GPIO_FUNC199_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC199_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x474)
/** GPIO_FUNC199_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC199_IN_SEL    0x0000003FU
#define GPIO_FUNC199_IN_SEL_M  (GPIO_FUNC199_IN_SEL_V << GPIO_FUNC199_IN_SEL_S)
#define GPIO_FUNC199_IN_SEL_V  0x0000003FU
#define GPIO_FUNC199_IN_SEL_S  0
/** GPIO_FUNC199_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC199_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC199_IN_INV_SEL_M  (GPIO_FUNC199_IN_INV_SEL_V << GPIO_FUNC199_IN_INV_SEL_S)
#define GPIO_FUNC199_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC199_IN_INV_SEL_S  6
/** GPIO_SIG199_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG199_IN_SEL    (BIT(7))
#define GPIO_SIG199_IN_SEL_M  (GPIO_SIG199_IN_SEL_V << GPIO_SIG199_IN_SEL_S)
#define GPIO_SIG199_IN_SEL_V  0x00000001U
#define GPIO_SIG199_IN_SEL_S  7

/** GPIO_FUNC200_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC200_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x478)
/** GPIO_FUNC200_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC200_IN_SEL    0x0000003FU
#define GPIO_FUNC200_IN_SEL_M  (GPIO_FUNC200_IN_SEL_V << GPIO_FUNC200_IN_SEL_S)
#define GPIO_FUNC200_IN_SEL_V  0x0000003FU
#define GPIO_FUNC200_IN_SEL_S  0
/** GPIO_FUNC200_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC200_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC200_IN_INV_SEL_M  (GPIO_FUNC200_IN_INV_SEL_V << GPIO_FUNC200_IN_INV_SEL_S)
#define GPIO_FUNC200_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC200_IN_INV_SEL_S  6
/** GPIO_SIG200_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG200_IN_SEL    (BIT(7))
#define GPIO_SIG200_IN_SEL_M  (GPIO_SIG200_IN_SEL_V << GPIO_SIG200_IN_SEL_S)
#define GPIO_SIG200_IN_SEL_V  0x00000001U
#define GPIO_SIG200_IN_SEL_S  7

/** GPIO_FUNC201_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC201_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x47c)
/** GPIO_FUNC201_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC201_IN_SEL    0x0000003FU
#define GPIO_FUNC201_IN_SEL_M  (GPIO_FUNC201_IN_SEL_V << GPIO_FUNC201_IN_SEL_S)
#define GPIO_FUNC201_IN_SEL_V  0x0000003FU
#define GPIO_FUNC201_IN_SEL_S  0
/** GPIO_FUNC201_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC201_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC201_IN_INV_SEL_M  (GPIO_FUNC201_IN_INV_SEL_V << GPIO_FUNC201_IN_INV_SEL_S)
#define GPIO_FUNC201_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC201_IN_INV_SEL_S  6
/** GPIO_SIG201_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG201_IN_SEL    (BIT(7))
#define GPIO_SIG201_IN_SEL_M  (GPIO_SIG201_IN_SEL_V << GPIO_SIG201_IN_SEL_S)
#define GPIO_SIG201_IN_SEL_V  0x00000001U
#define GPIO_SIG201_IN_SEL_S  7

/** GPIO_FUNC202_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC202_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x480)
/** GPIO_FUNC202_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC202_IN_SEL    0x0000003FU
#define GPIO_FUNC202_IN_SEL_M  (GPIO_FUNC202_IN_SEL_V << GPIO_FUNC202_IN_SEL_S)
#define GPIO_FUNC202_IN_SEL_V  0x0000003FU
#define GPIO_FUNC202_IN_SEL_S  0
/** GPIO_FUNC202_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC202_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC202_IN_INV_SEL_M  (GPIO_FUNC202_IN_INV_SEL_V << GPIO_FUNC202_IN_INV_SEL_S)
#define GPIO_FUNC202_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC202_IN_INV_SEL_S  6
/** GPIO_SIG202_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG202_IN_SEL    (BIT(7))
#define GPIO_SIG202_IN_SEL_M  (GPIO_SIG202_IN_SEL_V << GPIO_SIG202_IN_SEL_S)
#define GPIO_SIG202_IN_SEL_V  0x00000001U
#define GPIO_SIG202_IN_SEL_S  7

/** GPIO_FUNC203_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC203_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x484)
/** GPIO_FUNC203_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC203_IN_SEL    0x0000003FU
#define GPIO_FUNC203_IN_SEL_M  (GPIO_FUNC203_IN_SEL_V << GPIO_FUNC203_IN_SEL_S)
#define GPIO_FUNC203_IN_SEL_V  0x0000003FU
#define GPIO_FUNC203_IN_SEL_S  0
/** GPIO_FUNC203_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC203_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC203_IN_INV_SEL_M  (GPIO_FUNC203_IN_INV_SEL_V << GPIO_FUNC203_IN_INV_SEL_S)
#define GPIO_FUNC203_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC203_IN_INV_SEL_S  6
/** GPIO_SIG203_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG203_IN_SEL    (BIT(7))
#define GPIO_SIG203_IN_SEL_M  (GPIO_SIG203_IN_SEL_V << GPIO_SIG203_IN_SEL_S)
#define GPIO_SIG203_IN_SEL_V  0x00000001U
#define GPIO_SIG203_IN_SEL_S  7

/** GPIO_FUNC214_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC214_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b0)
/** GPIO_FUNC214_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC214_IN_SEL    0x0000003FU
#define GPIO_FUNC214_IN_SEL_M  (GPIO_FUNC214_IN_SEL_V << GPIO_FUNC214_IN_SEL_S)
#define GPIO_FUNC214_IN_SEL_V  0x0000003FU
#define GPIO_FUNC214_IN_SEL_S  0
/** GPIO_FUNC214_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC214_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC214_IN_INV_SEL_M  (GPIO_FUNC214_IN_INV_SEL_V << GPIO_FUNC214_IN_INV_SEL_S)
#define GPIO_FUNC214_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC214_IN_INV_SEL_S  6
/** GPIO_SIG214_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG214_IN_SEL    (BIT(7))
#define GPIO_SIG214_IN_SEL_M  (GPIO_SIG214_IN_SEL_V << GPIO_SIG214_IN_SEL_S)
#define GPIO_SIG214_IN_SEL_V  0x00000001U
#define GPIO_SIG214_IN_SEL_S  7

/** GPIO_FUNC215_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC215_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b4)
/** GPIO_FUNC215_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC215_IN_SEL    0x0000003FU
#define GPIO_FUNC215_IN_SEL_M  (GPIO_FUNC215_IN_SEL_V << GPIO_FUNC215_IN_SEL_S)
#define GPIO_FUNC215_IN_SEL_V  0x0000003FU
#define GPIO_FUNC215_IN_SEL_S  0
/** GPIO_FUNC215_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC215_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC215_IN_INV_SEL_M  (GPIO_FUNC215_IN_INV_SEL_V << GPIO_FUNC215_IN_INV_SEL_S)
#define GPIO_FUNC215_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC215_IN_INV_SEL_S  6
/** GPIO_SIG215_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG215_IN_SEL    (BIT(7))
#define GPIO_SIG215_IN_SEL_M  (GPIO_SIG215_IN_SEL_V << GPIO_SIG215_IN_SEL_S)
#define GPIO_SIG215_IN_SEL_V  0x00000001U
#define GPIO_SIG215_IN_SEL_S  7

/** GPIO_FUNC216_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC216_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b8)
/** GPIO_FUNC216_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC216_IN_SEL    0x0000003FU
#define GPIO_FUNC216_IN_SEL_M  (GPIO_FUNC216_IN_SEL_V << GPIO_FUNC216_IN_SEL_S)
#define GPIO_FUNC216_IN_SEL_V  0x0000003FU
#define GPIO_FUNC216_IN_SEL_S  0
/** GPIO_FUNC216_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC216_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC216_IN_INV_SEL_M  (GPIO_FUNC216_IN_INV_SEL_V << GPIO_FUNC216_IN_INV_SEL_S)
#define GPIO_FUNC216_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC216_IN_INV_SEL_S  6
/** GPIO_SIG216_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG216_IN_SEL    (BIT(7))
#define GPIO_SIG216_IN_SEL_M  (GPIO_SIG216_IN_SEL_V << GPIO_SIG216_IN_SEL_S)
#define GPIO_SIG216_IN_SEL_V  0x00000001U
#define GPIO_SIG216_IN_SEL_S  7

/** GPIO_FUNC217_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC217_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4bc)
/** GPIO_FUNC217_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC217_IN_SEL    0x0000003FU
#define GPIO_FUNC217_IN_SEL_M  (GPIO_FUNC217_IN_SEL_V << GPIO_FUNC217_IN_SEL_S)
#define GPIO_FUNC217_IN_SEL_V  0x0000003FU
#define GPIO_FUNC217_IN_SEL_S  0
/** GPIO_FUNC217_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC217_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC217_IN_INV_SEL_M  (GPIO_FUNC217_IN_INV_SEL_V << GPIO_FUNC217_IN_INV_SEL_S)
#define GPIO_FUNC217_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC217_IN_INV_SEL_S  6
/** GPIO_SIG217_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG217_IN_SEL    (BIT(7))
#define GPIO_SIG217_IN_SEL_M  (GPIO_SIG217_IN_SEL_V << GPIO_SIG217_IN_SEL_S)
#define GPIO_SIG217_IN_SEL_V  0x00000001U
#define GPIO_SIG217_IN_SEL_S  7

/** GPIO_FUNC218_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC218_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4c0)
/** GPIO_FUNC218_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC218_IN_SEL    0x0000003FU
#define GPIO_FUNC218_IN_SEL_M  (GPIO_FUNC218_IN_SEL_V << GPIO_FUNC218_IN_SEL_S)
#define GPIO_FUNC218_IN_SEL_V  0x0000003FU
#define GPIO_FUNC218_IN_SEL_S  0
/** GPIO_FUNC218_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC218_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC218_IN_INV_SEL_M  (GPIO_FUNC218_IN_INV_SEL_V << GPIO_FUNC218_IN_INV_SEL_S)
#define GPIO_FUNC218_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC218_IN_INV_SEL_S  6
/** GPIO_SIG218_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG218_IN_SEL    (BIT(7))
#define GPIO_SIG218_IN_SEL_M  (GPIO_SIG218_IN_SEL_V << GPIO_SIG218_IN_SEL_S)
#define GPIO_SIG218_IN_SEL_V  0x00000001U
#define GPIO_SIG218_IN_SEL_S  7

/** GPIO_FUNC219_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC219_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4c4)
/** GPIO_FUNC219_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC219_IN_SEL    0x0000003FU
#define GPIO_FUNC219_IN_SEL_M  (GPIO_FUNC219_IN_SEL_V << GPIO_FUNC219_IN_SEL_S)
#define GPIO_FUNC219_IN_SEL_V  0x0000003FU
#define GPIO_FUNC219_IN_SEL_S  0
/** GPIO_FUNC219_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC219_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC219_IN_INV_SEL_M  (GPIO_FUNC219_IN_INV_SEL_V << GPIO_FUNC219_IN_INV_SEL_S)
#define GPIO_FUNC219_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC219_IN_INV_SEL_S  6
/** GPIO_SIG219_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG219_IN_SEL    (BIT(7))
#define GPIO_SIG219_IN_SEL_M  (GPIO_SIG219_IN_SEL_V << GPIO_SIG219_IN_SEL_S)
#define GPIO_SIG219_IN_SEL_V  0x00000001U
#define GPIO_SIG219_IN_SEL_S  7

/** GPIO_FUNC220_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC220_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4c8)
/** GPIO_FUNC220_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC220_IN_SEL    0x0000003FU
#define GPIO_FUNC220_IN_SEL_M  (GPIO_FUNC220_IN_SEL_V << GPIO_FUNC220_IN_SEL_S)
#define GPIO_FUNC220_IN_SEL_V  0x0000003FU
#define GPIO_FUNC220_IN_SEL_S  0
/** GPIO_FUNC220_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC220_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC220_IN_INV_SEL_M  (GPIO_FUNC220_IN_INV_SEL_V << GPIO_FUNC220_IN_INV_SEL_S)
#define GPIO_FUNC220_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC220_IN_INV_SEL_S  6
/** GPIO_SIG220_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG220_IN_SEL    (BIT(7))
#define GPIO_SIG220_IN_SEL_M  (GPIO_SIG220_IN_SEL_V << GPIO_SIG220_IN_SEL_S)
#define GPIO_SIG220_IN_SEL_V  0x00000001U
#define GPIO_SIG220_IN_SEL_S  7

/** GPIO_FUNC221_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC221_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4cc)
/** GPIO_FUNC221_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC221_IN_SEL    0x0000003FU
#define GPIO_FUNC221_IN_SEL_M  (GPIO_FUNC221_IN_SEL_V << GPIO_FUNC221_IN_SEL_S)
#define GPIO_FUNC221_IN_SEL_V  0x0000003FU
#define GPIO_FUNC221_IN_SEL_S  0
/** GPIO_FUNC221_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC221_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC221_IN_INV_SEL_M  (GPIO_FUNC221_IN_INV_SEL_V << GPIO_FUNC221_IN_INV_SEL_S)
#define GPIO_FUNC221_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC221_IN_INV_SEL_S  6
/** GPIO_SIG221_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG221_IN_SEL    (BIT(7))
#define GPIO_SIG221_IN_SEL_M  (GPIO_SIG221_IN_SEL_V << GPIO_SIG221_IN_SEL_S)
#define GPIO_SIG221_IN_SEL_V  0x00000001U
#define GPIO_SIG221_IN_SEL_S  7

/** GPIO_FUNC222_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC222_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4d0)
/** GPIO_FUNC222_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC222_IN_SEL    0x0000003FU
#define GPIO_FUNC222_IN_SEL_M  (GPIO_FUNC222_IN_SEL_V << GPIO_FUNC222_IN_SEL_S)
#define GPIO_FUNC222_IN_SEL_V  0x0000003FU
#define GPIO_FUNC222_IN_SEL_S  0
/** GPIO_FUNC222_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC222_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC222_IN_INV_SEL_M  (GPIO_FUNC222_IN_INV_SEL_V << GPIO_FUNC222_IN_INV_SEL_S)
#define GPIO_FUNC222_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC222_IN_INV_SEL_S  6
/** GPIO_SIG222_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG222_IN_SEL    (BIT(7))
#define GPIO_SIG222_IN_SEL_M  (GPIO_SIG222_IN_SEL_V << GPIO_SIG222_IN_SEL_S)
#define GPIO_SIG222_IN_SEL_V  0x00000001U
#define GPIO_SIG222_IN_SEL_S  7

/** GPIO_FUNC223_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC223_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4d4)
/** GPIO_FUNC223_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC223_IN_SEL    0x0000003FU
#define GPIO_FUNC223_IN_SEL_M  (GPIO_FUNC223_IN_SEL_V << GPIO_FUNC223_IN_SEL_S)
#define GPIO_FUNC223_IN_SEL_V  0x0000003FU
#define GPIO_FUNC223_IN_SEL_S  0
/** GPIO_FUNC223_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC223_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC223_IN_INV_SEL_M  (GPIO_FUNC223_IN_INV_SEL_V << GPIO_FUNC223_IN_INV_SEL_S)
#define GPIO_FUNC223_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC223_IN_INV_SEL_S  6
/** GPIO_SIG223_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG223_IN_SEL    (BIT(7))
#define GPIO_SIG223_IN_SEL_M  (GPIO_SIG223_IN_SEL_V << GPIO_SIG223_IN_SEL_S)
#define GPIO_SIG223_IN_SEL_V  0x00000001U
#define GPIO_SIG223_IN_SEL_S  7

/** GPIO_FUNC224_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC224_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4d8)
/** GPIO_FUNC224_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC224_IN_SEL    0x0000003FU
#define GPIO_FUNC224_IN_SEL_M  (GPIO_FUNC224_IN_SEL_V << GPIO_FUNC224_IN_SEL_S)
#define GPIO_FUNC224_IN_SEL_V  0x0000003FU
#define GPIO_FUNC224_IN_SEL_S  0
/** GPIO_FUNC224_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC224_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC224_IN_INV_SEL_M  (GPIO_FUNC224_IN_INV_SEL_V << GPIO_FUNC224_IN_INV_SEL_S)
#define GPIO_FUNC224_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC224_IN_INV_SEL_S  6
/** GPIO_SIG224_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG224_IN_SEL    (BIT(7))
#define GPIO_SIG224_IN_SEL_M  (GPIO_SIG224_IN_SEL_V << GPIO_SIG224_IN_SEL_S)
#define GPIO_SIG224_IN_SEL_V  0x00000001U
#define GPIO_SIG224_IN_SEL_S  7

/** GPIO_FUNC225_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC225_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4dc)
/** GPIO_FUNC225_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC225_IN_SEL    0x0000003FU
#define GPIO_FUNC225_IN_SEL_M  (GPIO_FUNC225_IN_SEL_V << GPIO_FUNC225_IN_SEL_S)
#define GPIO_FUNC225_IN_SEL_V  0x0000003FU
#define GPIO_FUNC225_IN_SEL_S  0
/** GPIO_FUNC225_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC225_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC225_IN_INV_SEL_M  (GPIO_FUNC225_IN_INV_SEL_V << GPIO_FUNC225_IN_INV_SEL_S)
#define GPIO_FUNC225_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC225_IN_INV_SEL_S  6
/** GPIO_SIG225_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG225_IN_SEL    (BIT(7))
#define GPIO_SIG225_IN_SEL_M  (GPIO_SIG225_IN_SEL_V << GPIO_SIG225_IN_SEL_S)
#define GPIO_SIG225_IN_SEL_V  0x00000001U
#define GPIO_SIG225_IN_SEL_S  7

/** GPIO_FUNC226_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC226_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4e0)
/** GPIO_FUNC226_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC226_IN_SEL    0x0000003FU
#define GPIO_FUNC226_IN_SEL_M  (GPIO_FUNC226_IN_SEL_V << GPIO_FUNC226_IN_SEL_S)
#define GPIO_FUNC226_IN_SEL_V  0x0000003FU
#define GPIO_FUNC226_IN_SEL_S  0
/** GPIO_FUNC226_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC226_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC226_IN_INV_SEL_M  (GPIO_FUNC226_IN_INV_SEL_V << GPIO_FUNC226_IN_INV_SEL_S)
#define GPIO_FUNC226_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC226_IN_INV_SEL_S  6
/** GPIO_SIG226_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG226_IN_SEL    (BIT(7))
#define GPIO_SIG226_IN_SEL_M  (GPIO_SIG226_IN_SEL_V << GPIO_SIG226_IN_SEL_S)
#define GPIO_SIG226_IN_SEL_V  0x00000001U
#define GPIO_SIG226_IN_SEL_S  7

/** GPIO_FUNC227_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC227_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4e4)
/** GPIO_FUNC227_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC227_IN_SEL    0x0000003FU
#define GPIO_FUNC227_IN_SEL_M  (GPIO_FUNC227_IN_SEL_V << GPIO_FUNC227_IN_SEL_S)
#define GPIO_FUNC227_IN_SEL_V  0x0000003FU
#define GPIO_FUNC227_IN_SEL_S  0
/** GPIO_FUNC227_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC227_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC227_IN_INV_SEL_M  (GPIO_FUNC227_IN_INV_SEL_V << GPIO_FUNC227_IN_INV_SEL_S)
#define GPIO_FUNC227_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC227_IN_INV_SEL_S  6
/** GPIO_SIG227_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG227_IN_SEL    (BIT(7))
#define GPIO_SIG227_IN_SEL_M  (GPIO_SIG227_IN_SEL_V << GPIO_SIG227_IN_SEL_S)
#define GPIO_SIG227_IN_SEL_V  0x00000001U
#define GPIO_SIG227_IN_SEL_S  7

/** GPIO_FUNC228_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC228_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4e8)
/** GPIO_FUNC228_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC228_IN_SEL    0x0000003FU
#define GPIO_FUNC228_IN_SEL_M  (GPIO_FUNC228_IN_SEL_V << GPIO_FUNC228_IN_SEL_S)
#define GPIO_FUNC228_IN_SEL_V  0x0000003FU
#define GPIO_FUNC228_IN_SEL_S  0
/** GPIO_FUNC228_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC228_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC228_IN_INV_SEL_M  (GPIO_FUNC228_IN_INV_SEL_V << GPIO_FUNC228_IN_INV_SEL_S)
#define GPIO_FUNC228_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC228_IN_INV_SEL_S  6
/** GPIO_SIG228_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG228_IN_SEL    (BIT(7))
#define GPIO_SIG228_IN_SEL_M  (GPIO_SIG228_IN_SEL_V << GPIO_SIG228_IN_SEL_S)
#define GPIO_SIG228_IN_SEL_V  0x00000001U
#define GPIO_SIG228_IN_SEL_S  7

/** GPIO_FUNC229_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC229_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4ec)
/** GPIO_FUNC229_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC229_IN_SEL    0x0000003FU
#define GPIO_FUNC229_IN_SEL_M  (GPIO_FUNC229_IN_SEL_V << GPIO_FUNC229_IN_SEL_S)
#define GPIO_FUNC229_IN_SEL_V  0x0000003FU
#define GPIO_FUNC229_IN_SEL_S  0
/** GPIO_FUNC229_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC229_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC229_IN_INV_SEL_M  (GPIO_FUNC229_IN_INV_SEL_V << GPIO_FUNC229_IN_INV_SEL_S)
#define GPIO_FUNC229_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC229_IN_INV_SEL_S  6
/** GPIO_SIG229_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG229_IN_SEL    (BIT(7))
#define GPIO_SIG229_IN_SEL_M  (GPIO_SIG229_IN_SEL_V << GPIO_SIG229_IN_SEL_S)
#define GPIO_SIG229_IN_SEL_V  0x00000001U
#define GPIO_SIG229_IN_SEL_S  7

/** GPIO_FUNC230_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC230_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4f0)
/** GPIO_FUNC230_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC230_IN_SEL    0x0000003FU
#define GPIO_FUNC230_IN_SEL_M  (GPIO_FUNC230_IN_SEL_V << GPIO_FUNC230_IN_SEL_S)
#define GPIO_FUNC230_IN_SEL_V  0x0000003FU
#define GPIO_FUNC230_IN_SEL_S  0
/** GPIO_FUNC230_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC230_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC230_IN_INV_SEL_M  (GPIO_FUNC230_IN_INV_SEL_V << GPIO_FUNC230_IN_INV_SEL_S)
#define GPIO_FUNC230_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC230_IN_INV_SEL_S  6
/** GPIO_SIG230_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG230_IN_SEL    (BIT(7))
#define GPIO_SIG230_IN_SEL_M  (GPIO_SIG230_IN_SEL_V << GPIO_SIG230_IN_SEL_S)
#define GPIO_SIG230_IN_SEL_V  0x00000001U
#define GPIO_SIG230_IN_SEL_S  7

/** GPIO_FUNC231_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC231_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4f4)
/** GPIO_FUNC231_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC231_IN_SEL    0x0000003FU
#define GPIO_FUNC231_IN_SEL_M  (GPIO_FUNC231_IN_SEL_V << GPIO_FUNC231_IN_SEL_S)
#define GPIO_FUNC231_IN_SEL_V  0x0000003FU
#define GPIO_FUNC231_IN_SEL_S  0
/** GPIO_FUNC231_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC231_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC231_IN_INV_SEL_M  (GPIO_FUNC231_IN_INV_SEL_V << GPIO_FUNC231_IN_INV_SEL_S)
#define GPIO_FUNC231_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC231_IN_INV_SEL_S  6
/** GPIO_SIG231_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG231_IN_SEL    (BIT(7))
#define GPIO_SIG231_IN_SEL_M  (GPIO_SIG231_IN_SEL_V << GPIO_SIG231_IN_SEL_S)
#define GPIO_SIG231_IN_SEL_V  0x00000001U
#define GPIO_SIG231_IN_SEL_S  7

/** GPIO_FUNC232_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC232_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4f8)
/** GPIO_FUNC232_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC232_IN_SEL    0x0000003FU
#define GPIO_FUNC232_IN_SEL_M  (GPIO_FUNC232_IN_SEL_V << GPIO_FUNC232_IN_SEL_S)
#define GPIO_FUNC232_IN_SEL_V  0x0000003FU
#define GPIO_FUNC232_IN_SEL_S  0
/** GPIO_FUNC232_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC232_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC232_IN_INV_SEL_M  (GPIO_FUNC232_IN_INV_SEL_V << GPIO_FUNC232_IN_INV_SEL_S)
#define GPIO_FUNC232_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC232_IN_INV_SEL_S  6
/** GPIO_SIG232_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG232_IN_SEL    (BIT(7))
#define GPIO_SIG232_IN_SEL_M  (GPIO_SIG232_IN_SEL_V << GPIO_SIG232_IN_SEL_S)
#define GPIO_SIG232_IN_SEL_V  0x00000001U
#define GPIO_SIG232_IN_SEL_S  7

/** GPIO_FUNC233_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC233_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4fc)
/** GPIO_FUNC233_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC233_IN_SEL    0x0000003FU
#define GPIO_FUNC233_IN_SEL_M  (GPIO_FUNC233_IN_SEL_V << GPIO_FUNC233_IN_SEL_S)
#define GPIO_FUNC233_IN_SEL_V  0x0000003FU
#define GPIO_FUNC233_IN_SEL_S  0
/** GPIO_FUNC233_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC233_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC233_IN_INV_SEL_M  (GPIO_FUNC233_IN_INV_SEL_V << GPIO_FUNC233_IN_INV_SEL_S)
#define GPIO_FUNC233_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC233_IN_INV_SEL_S  6
/** GPIO_SIG233_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG233_IN_SEL    (BIT(7))
#define GPIO_SIG233_IN_SEL_M  (GPIO_SIG233_IN_SEL_V << GPIO_SIG233_IN_SEL_S)
#define GPIO_SIG233_IN_SEL_V  0x00000001U
#define GPIO_SIG233_IN_SEL_S  7

/** GPIO_FUNC234_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC234_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x500)
/** GPIO_FUNC234_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC234_IN_SEL    0x0000003FU
#define GPIO_FUNC234_IN_SEL_M  (GPIO_FUNC234_IN_SEL_V << GPIO_FUNC234_IN_SEL_S)
#define GPIO_FUNC234_IN_SEL_V  0x0000003FU
#define GPIO_FUNC234_IN_SEL_S  0
/** GPIO_FUNC234_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC234_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC234_IN_INV_SEL_M  (GPIO_FUNC234_IN_INV_SEL_V << GPIO_FUNC234_IN_INV_SEL_S)
#define GPIO_FUNC234_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC234_IN_INV_SEL_S  6
/** GPIO_SIG234_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG234_IN_SEL    (BIT(7))
#define GPIO_SIG234_IN_SEL_M  (GPIO_SIG234_IN_SEL_V << GPIO_SIG234_IN_SEL_S)
#define GPIO_SIG234_IN_SEL_V  0x00000001U
#define GPIO_SIG234_IN_SEL_S  7

/** GPIO_FUNC235_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC235_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x504)
/** GPIO_FUNC235_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC235_IN_SEL    0x0000003FU
#define GPIO_FUNC235_IN_SEL_M  (GPIO_FUNC235_IN_SEL_V << GPIO_FUNC235_IN_SEL_S)
#define GPIO_FUNC235_IN_SEL_V  0x0000003FU
#define GPIO_FUNC235_IN_SEL_S  0
/** GPIO_FUNC235_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC235_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC235_IN_INV_SEL_M  (GPIO_FUNC235_IN_INV_SEL_V << GPIO_FUNC235_IN_INV_SEL_S)
#define GPIO_FUNC235_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC235_IN_INV_SEL_S  6
/** GPIO_SIG235_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG235_IN_SEL    (BIT(7))
#define GPIO_SIG235_IN_SEL_M  (GPIO_SIG235_IN_SEL_V << GPIO_SIG235_IN_SEL_S)
#define GPIO_SIG235_IN_SEL_V  0x00000001U
#define GPIO_SIG235_IN_SEL_S  7

/** GPIO_FUNC236_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC236_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x508)
/** GPIO_FUNC236_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC236_IN_SEL    0x0000003FU
#define GPIO_FUNC236_IN_SEL_M  (GPIO_FUNC236_IN_SEL_V << GPIO_FUNC236_IN_SEL_S)
#define GPIO_FUNC236_IN_SEL_V  0x0000003FU
#define GPIO_FUNC236_IN_SEL_S  0
/** GPIO_FUNC236_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC236_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC236_IN_INV_SEL_M  (GPIO_FUNC236_IN_INV_SEL_V << GPIO_FUNC236_IN_INV_SEL_S)
#define GPIO_FUNC236_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC236_IN_INV_SEL_S  6
/** GPIO_SIG236_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG236_IN_SEL    (BIT(7))
#define GPIO_SIG236_IN_SEL_M  (GPIO_SIG236_IN_SEL_V << GPIO_SIG236_IN_SEL_S)
#define GPIO_SIG236_IN_SEL_V  0x00000001U
#define GPIO_SIG236_IN_SEL_S  7

/** GPIO_FUNC237_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC237_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x50c)
/** GPIO_FUNC237_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC237_IN_SEL    0x0000003FU
#define GPIO_FUNC237_IN_SEL_M  (GPIO_FUNC237_IN_SEL_V << GPIO_FUNC237_IN_SEL_S)
#define GPIO_FUNC237_IN_SEL_V  0x0000003FU
#define GPIO_FUNC237_IN_SEL_S  0
/** GPIO_FUNC237_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC237_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC237_IN_INV_SEL_M  (GPIO_FUNC237_IN_INV_SEL_V << GPIO_FUNC237_IN_INV_SEL_S)
#define GPIO_FUNC237_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC237_IN_INV_SEL_S  6
/** GPIO_SIG237_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG237_IN_SEL    (BIT(7))
#define GPIO_SIG237_IN_SEL_M  (GPIO_SIG237_IN_SEL_V << GPIO_SIG237_IN_SEL_S)
#define GPIO_SIG237_IN_SEL_V  0x00000001U
#define GPIO_SIG237_IN_SEL_S  7

/** GPIO_FUNC238_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC238_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x510)
/** GPIO_FUNC238_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC238_IN_SEL    0x0000003FU
#define GPIO_FUNC238_IN_SEL_M  (GPIO_FUNC238_IN_SEL_V << GPIO_FUNC238_IN_SEL_S)
#define GPIO_FUNC238_IN_SEL_V  0x0000003FU
#define GPIO_FUNC238_IN_SEL_S  0
/** GPIO_FUNC238_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC238_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC238_IN_INV_SEL_M  (GPIO_FUNC238_IN_INV_SEL_V << GPIO_FUNC238_IN_INV_SEL_S)
#define GPIO_FUNC238_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC238_IN_INV_SEL_S  6
/** GPIO_SIG238_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG238_IN_SEL    (BIT(7))
#define GPIO_SIG238_IN_SEL_M  (GPIO_SIG238_IN_SEL_V << GPIO_SIG238_IN_SEL_S)
#define GPIO_SIG238_IN_SEL_V  0x00000001U
#define GPIO_SIG238_IN_SEL_S  7

/** GPIO_FUNC239_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC239_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x514)
/** GPIO_FUNC239_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC239_IN_SEL    0x0000003FU
#define GPIO_FUNC239_IN_SEL_M  (GPIO_FUNC239_IN_SEL_V << GPIO_FUNC239_IN_SEL_S)
#define GPIO_FUNC239_IN_SEL_V  0x0000003FU
#define GPIO_FUNC239_IN_SEL_S  0
/** GPIO_FUNC239_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC239_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC239_IN_INV_SEL_M  (GPIO_FUNC239_IN_INV_SEL_V << GPIO_FUNC239_IN_INV_SEL_S)
#define GPIO_FUNC239_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC239_IN_INV_SEL_S  6
/** GPIO_SIG239_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG239_IN_SEL    (BIT(7))
#define GPIO_SIG239_IN_SEL_M  (GPIO_SIG239_IN_SEL_V << GPIO_SIG239_IN_SEL_S)
#define GPIO_SIG239_IN_SEL_V  0x00000001U
#define GPIO_SIG239_IN_SEL_S  7

/** GPIO_FUNC240_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC240_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x518)
/** GPIO_FUNC240_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC240_IN_SEL    0x0000003FU
#define GPIO_FUNC240_IN_SEL_M  (GPIO_FUNC240_IN_SEL_V << GPIO_FUNC240_IN_SEL_S)
#define GPIO_FUNC240_IN_SEL_V  0x0000003FU
#define GPIO_FUNC240_IN_SEL_S  0
/** GPIO_FUNC240_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC240_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC240_IN_INV_SEL_M  (GPIO_FUNC240_IN_INV_SEL_V << GPIO_FUNC240_IN_INV_SEL_S)
#define GPIO_FUNC240_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC240_IN_INV_SEL_S  6
/** GPIO_SIG240_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG240_IN_SEL    (BIT(7))
#define GPIO_SIG240_IN_SEL_M  (GPIO_SIG240_IN_SEL_V << GPIO_SIG240_IN_SEL_S)
#define GPIO_SIG240_IN_SEL_V  0x00000001U
#define GPIO_SIG240_IN_SEL_S  7

/** GPIO_FUNC241_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC241_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x51c)
/** GPIO_FUNC241_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC241_IN_SEL    0x0000003FU
#define GPIO_FUNC241_IN_SEL_M  (GPIO_FUNC241_IN_SEL_V << GPIO_FUNC241_IN_SEL_S)
#define GPIO_FUNC241_IN_SEL_V  0x0000003FU
#define GPIO_FUNC241_IN_SEL_S  0
/** GPIO_FUNC241_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC241_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC241_IN_INV_SEL_M  (GPIO_FUNC241_IN_INV_SEL_V << GPIO_FUNC241_IN_INV_SEL_S)
#define GPIO_FUNC241_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC241_IN_INV_SEL_S  6
/** GPIO_SIG241_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG241_IN_SEL    (BIT(7))
#define GPIO_SIG241_IN_SEL_M  (GPIO_SIG241_IN_SEL_V << GPIO_SIG241_IN_SEL_S)
#define GPIO_SIG241_IN_SEL_V  0x00000001U
#define GPIO_SIG241_IN_SEL_S  7

/** GPIO_FUNC242_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC242_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x520)
/** GPIO_FUNC242_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC242_IN_SEL    0x0000003FU
#define GPIO_FUNC242_IN_SEL_M  (GPIO_FUNC242_IN_SEL_V << GPIO_FUNC242_IN_SEL_S)
#define GPIO_FUNC242_IN_SEL_V  0x0000003FU
#define GPIO_FUNC242_IN_SEL_S  0
/** GPIO_FUNC242_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC242_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC242_IN_INV_SEL_M  (GPIO_FUNC242_IN_INV_SEL_V << GPIO_FUNC242_IN_INV_SEL_S)
#define GPIO_FUNC242_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC242_IN_INV_SEL_S  6
/** GPIO_SIG242_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG242_IN_SEL    (BIT(7))
#define GPIO_SIG242_IN_SEL_M  (GPIO_SIG242_IN_SEL_V << GPIO_SIG242_IN_SEL_S)
#define GPIO_SIG242_IN_SEL_V  0x00000001U
#define GPIO_SIG242_IN_SEL_S  7

/** GPIO_FUNC243_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC243_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x524)
/** GPIO_FUNC243_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC243_IN_SEL    0x0000003FU
#define GPIO_FUNC243_IN_SEL_M  (GPIO_FUNC243_IN_SEL_V << GPIO_FUNC243_IN_SEL_S)
#define GPIO_FUNC243_IN_SEL_V  0x0000003FU
#define GPIO_FUNC243_IN_SEL_S  0
/** GPIO_FUNC243_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC243_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC243_IN_INV_SEL_M  (GPIO_FUNC243_IN_INV_SEL_V << GPIO_FUNC243_IN_INV_SEL_S)
#define GPIO_FUNC243_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC243_IN_INV_SEL_S  6
/** GPIO_SIG243_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG243_IN_SEL    (BIT(7))
#define GPIO_SIG243_IN_SEL_M  (GPIO_SIG243_IN_SEL_V << GPIO_SIG243_IN_SEL_S)
#define GPIO_SIG243_IN_SEL_V  0x00000001U
#define GPIO_SIG243_IN_SEL_S  7

/** GPIO_FUNC244_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC244_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x528)
/** GPIO_FUNC244_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC244_IN_SEL    0x0000003FU
#define GPIO_FUNC244_IN_SEL_M  (GPIO_FUNC244_IN_SEL_V << GPIO_FUNC244_IN_SEL_S)
#define GPIO_FUNC244_IN_SEL_V  0x0000003FU
#define GPIO_FUNC244_IN_SEL_S  0
/** GPIO_FUNC244_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC244_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC244_IN_INV_SEL_M  (GPIO_FUNC244_IN_INV_SEL_V << GPIO_FUNC244_IN_INV_SEL_S)
#define GPIO_FUNC244_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC244_IN_INV_SEL_S  6
/** GPIO_SIG244_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG244_IN_SEL    (BIT(7))
#define GPIO_SIG244_IN_SEL_M  (GPIO_SIG244_IN_SEL_V << GPIO_SIG244_IN_SEL_S)
#define GPIO_SIG244_IN_SEL_V  0x00000001U
#define GPIO_SIG244_IN_SEL_S  7

/** GPIO_FUNC245_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC245_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x52c)
/** GPIO_FUNC245_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC245_IN_SEL    0x0000003FU
#define GPIO_FUNC245_IN_SEL_M  (GPIO_FUNC245_IN_SEL_V << GPIO_FUNC245_IN_SEL_S)
#define GPIO_FUNC245_IN_SEL_V  0x0000003FU
#define GPIO_FUNC245_IN_SEL_S  0
/** GPIO_FUNC245_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC245_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC245_IN_INV_SEL_M  (GPIO_FUNC245_IN_INV_SEL_V << GPIO_FUNC245_IN_INV_SEL_S)
#define GPIO_FUNC245_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC245_IN_INV_SEL_S  6
/** GPIO_SIG245_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG245_IN_SEL    (BIT(7))
#define GPIO_SIG245_IN_SEL_M  (GPIO_SIG245_IN_SEL_V << GPIO_SIG245_IN_SEL_S)
#define GPIO_SIG245_IN_SEL_V  0x00000001U
#define GPIO_SIG245_IN_SEL_S  7

/** GPIO_FUNC246_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC246_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x530)
/** GPIO_FUNC246_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC246_IN_SEL    0x0000003FU
#define GPIO_FUNC246_IN_SEL_M  (GPIO_FUNC246_IN_SEL_V << GPIO_FUNC246_IN_SEL_S)
#define GPIO_FUNC246_IN_SEL_V  0x0000003FU
#define GPIO_FUNC246_IN_SEL_S  0
/** GPIO_FUNC246_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC246_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC246_IN_INV_SEL_M  (GPIO_FUNC246_IN_INV_SEL_V << GPIO_FUNC246_IN_INV_SEL_S)
#define GPIO_FUNC246_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC246_IN_INV_SEL_S  6
/** GPIO_SIG246_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG246_IN_SEL    (BIT(7))
#define GPIO_SIG246_IN_SEL_M  (GPIO_SIG246_IN_SEL_V << GPIO_SIG246_IN_SEL_S)
#define GPIO_SIG246_IN_SEL_V  0x00000001U
#define GPIO_SIG246_IN_SEL_S  7

/** GPIO_FUNC247_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC247_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x534)
/** GPIO_FUNC247_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC247_IN_SEL    0x0000003FU
#define GPIO_FUNC247_IN_SEL_M  (GPIO_FUNC247_IN_SEL_V << GPIO_FUNC247_IN_SEL_S)
#define GPIO_FUNC247_IN_SEL_V  0x0000003FU
#define GPIO_FUNC247_IN_SEL_S  0
/** GPIO_FUNC247_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC247_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC247_IN_INV_SEL_M  (GPIO_FUNC247_IN_INV_SEL_V << GPIO_FUNC247_IN_INV_SEL_S)
#define GPIO_FUNC247_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC247_IN_INV_SEL_S  6
/** GPIO_SIG247_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG247_IN_SEL    (BIT(7))
#define GPIO_SIG247_IN_SEL_M  (GPIO_SIG247_IN_SEL_V << GPIO_SIG247_IN_SEL_S)
#define GPIO_SIG247_IN_SEL_V  0x00000001U
#define GPIO_SIG247_IN_SEL_S  7

/** GPIO_FUNC248_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC248_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x538)
/** GPIO_FUNC248_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC248_IN_SEL    0x0000003FU
#define GPIO_FUNC248_IN_SEL_M  (GPIO_FUNC248_IN_SEL_V << GPIO_FUNC248_IN_SEL_S)
#define GPIO_FUNC248_IN_SEL_V  0x0000003FU
#define GPIO_FUNC248_IN_SEL_S  0
/** GPIO_FUNC248_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC248_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC248_IN_INV_SEL_M  (GPIO_FUNC248_IN_INV_SEL_V << GPIO_FUNC248_IN_INV_SEL_S)
#define GPIO_FUNC248_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC248_IN_INV_SEL_S  6
/** GPIO_SIG248_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG248_IN_SEL    (BIT(7))
#define GPIO_SIG248_IN_SEL_M  (GPIO_SIG248_IN_SEL_V << GPIO_SIG248_IN_SEL_S)
#define GPIO_SIG248_IN_SEL_V  0x00000001U
#define GPIO_SIG248_IN_SEL_S  7

/** GPIO_FUNC249_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC249_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x53c)
/** GPIO_FUNC249_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC249_IN_SEL    0x0000003FU
#define GPIO_FUNC249_IN_SEL_M  (GPIO_FUNC249_IN_SEL_V << GPIO_FUNC249_IN_SEL_S)
#define GPIO_FUNC249_IN_SEL_V  0x0000003FU
#define GPIO_FUNC249_IN_SEL_S  0
/** GPIO_FUNC249_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC249_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC249_IN_INV_SEL_M  (GPIO_FUNC249_IN_INV_SEL_V << GPIO_FUNC249_IN_INV_SEL_S)
#define GPIO_FUNC249_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC249_IN_INV_SEL_S  6
/** GPIO_SIG249_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG249_IN_SEL    (BIT(7))
#define GPIO_SIG249_IN_SEL_M  (GPIO_SIG249_IN_SEL_V << GPIO_SIG249_IN_SEL_S)
#define GPIO_SIG249_IN_SEL_V  0x00000001U
#define GPIO_SIG249_IN_SEL_S  7

/** GPIO_FUNC250_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC250_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x540)
/** GPIO_FUNC250_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC250_IN_SEL    0x0000003FU
#define GPIO_FUNC250_IN_SEL_M  (GPIO_FUNC250_IN_SEL_V << GPIO_FUNC250_IN_SEL_S)
#define GPIO_FUNC250_IN_SEL_V  0x0000003FU
#define GPIO_FUNC250_IN_SEL_S  0
/** GPIO_FUNC250_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC250_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC250_IN_INV_SEL_M  (GPIO_FUNC250_IN_INV_SEL_V << GPIO_FUNC250_IN_INV_SEL_S)
#define GPIO_FUNC250_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC250_IN_INV_SEL_S  6
/** GPIO_SIG250_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG250_IN_SEL    (BIT(7))
#define GPIO_SIG250_IN_SEL_M  (GPIO_SIG250_IN_SEL_V << GPIO_SIG250_IN_SEL_S)
#define GPIO_SIG250_IN_SEL_V  0x00000001U
#define GPIO_SIG250_IN_SEL_S  7

/** GPIO_FUNC251_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC251_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x544)
/** GPIO_FUNC251_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC251_IN_SEL    0x0000003FU
#define GPIO_FUNC251_IN_SEL_M  (GPIO_FUNC251_IN_SEL_V << GPIO_FUNC251_IN_SEL_S)
#define GPIO_FUNC251_IN_SEL_V  0x0000003FU
#define GPIO_FUNC251_IN_SEL_S  0
/** GPIO_FUNC251_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC251_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC251_IN_INV_SEL_M  (GPIO_FUNC251_IN_INV_SEL_V << GPIO_FUNC251_IN_INV_SEL_S)
#define GPIO_FUNC251_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC251_IN_INV_SEL_S  6
/** GPIO_SIG251_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG251_IN_SEL    (BIT(7))
#define GPIO_SIG251_IN_SEL_M  (GPIO_SIG251_IN_SEL_V << GPIO_SIG251_IN_SEL_S)
#define GPIO_SIG251_IN_SEL_V  0x00000001U
#define GPIO_SIG251_IN_SEL_S  7

/** GPIO_FUNC252_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC252_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x548)
/** GPIO_FUNC252_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC252_IN_SEL    0x0000003FU
#define GPIO_FUNC252_IN_SEL_M  (GPIO_FUNC252_IN_SEL_V << GPIO_FUNC252_IN_SEL_S)
#define GPIO_FUNC252_IN_SEL_V  0x0000003FU
#define GPIO_FUNC252_IN_SEL_S  0
/** GPIO_FUNC252_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC252_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC252_IN_INV_SEL_M  (GPIO_FUNC252_IN_INV_SEL_V << GPIO_FUNC252_IN_INV_SEL_S)
#define GPIO_FUNC252_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC252_IN_INV_SEL_S  6
/** GPIO_SIG252_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG252_IN_SEL    (BIT(7))
#define GPIO_SIG252_IN_SEL_M  (GPIO_SIG252_IN_SEL_V << GPIO_SIG252_IN_SEL_S)
#define GPIO_SIG252_IN_SEL_V  0x00000001U
#define GPIO_SIG252_IN_SEL_S  7

/** GPIO_FUNC253_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC253_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x54c)
/** GPIO_FUNC253_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC253_IN_SEL    0x0000003FU
#define GPIO_FUNC253_IN_SEL_M  (GPIO_FUNC253_IN_SEL_V << GPIO_FUNC253_IN_SEL_S)
#define GPIO_FUNC253_IN_SEL_V  0x0000003FU
#define GPIO_FUNC253_IN_SEL_S  0
/** GPIO_FUNC253_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC253_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC253_IN_INV_SEL_M  (GPIO_FUNC253_IN_INV_SEL_V << GPIO_FUNC253_IN_INV_SEL_S)
#define GPIO_FUNC253_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC253_IN_INV_SEL_S  6
/** GPIO_SIG253_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG253_IN_SEL    (BIT(7))
#define GPIO_SIG253_IN_SEL_M  (GPIO_SIG253_IN_SEL_V << GPIO_SIG253_IN_SEL_S)
#define GPIO_SIG253_IN_SEL_V  0x00000001U
#define GPIO_SIG253_IN_SEL_S  7

/** GPIO_FUNC254_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC254_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x550)
/** GPIO_FUNC254_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC254_IN_SEL    0x0000003FU
#define GPIO_FUNC254_IN_SEL_M  (GPIO_FUNC254_IN_SEL_V << GPIO_FUNC254_IN_SEL_S)
#define GPIO_FUNC254_IN_SEL_V  0x0000003FU
#define GPIO_FUNC254_IN_SEL_S  0
/** GPIO_FUNC254_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC254_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC254_IN_INV_SEL_M  (GPIO_FUNC254_IN_INV_SEL_V << GPIO_FUNC254_IN_INV_SEL_S)
#define GPIO_FUNC254_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC254_IN_INV_SEL_S  6
/** GPIO_SIG254_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG254_IN_SEL    (BIT(7))
#define GPIO_SIG254_IN_SEL_M  (GPIO_SIG254_IN_SEL_V << GPIO_SIG254_IN_SEL_S)
#define GPIO_SIG254_IN_SEL_V  0x00000001U
#define GPIO_SIG254_IN_SEL_S  7

/** GPIO_FUNC255_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC255_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x554)
/** GPIO_FUNC255_IN_SEL : R/W; bitpos: [5:0]; default: 62;
 *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
 *  high level. s=0x3E: set this port always low level.
 */
#define GPIO_FUNC255_IN_SEL    0x0000003FU
#define GPIO_FUNC255_IN_SEL_M  (GPIO_FUNC255_IN_SEL_V << GPIO_FUNC255_IN_SEL_S)
#define GPIO_FUNC255_IN_SEL_V  0x0000003FU
#define GPIO_FUNC255_IN_SEL_S  0
/** GPIO_FUNC255_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC255_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC255_IN_INV_SEL_M  (GPIO_FUNC255_IN_INV_SEL_V << GPIO_FUNC255_IN_INV_SEL_S)
#define GPIO_FUNC255_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC255_IN_INV_SEL_S  6
/** GPIO_SIG255_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG255_IN_SEL    (BIT(7))
#define GPIO_SIG255_IN_SEL_M  (GPIO_SIG255_IN_SEL_V << GPIO_SIG255_IN_SEL_S)
#define GPIO_SIG255_IN_SEL_V  0x00000001U
#define GPIO_SIG255_IN_SEL_S  7

/** GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x558)
/** GPIO_FUNC0_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC0_OUT_SEL    0x000001FFU
#define GPIO_FUNC0_OUT_SEL_M  (GPIO_FUNC0_OUT_SEL_V << GPIO_FUNC0_OUT_SEL_S)
#define GPIO_FUNC0_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC0_OUT_SEL_S  0
/** GPIO_FUNC0_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC0_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC0_OUT_INV_SEL_M  (GPIO_FUNC0_OUT_INV_SEL_V << GPIO_FUNC0_OUT_INV_SEL_S)
#define GPIO_FUNC0_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OUT_INV_SEL_S  9
/** GPIO_FUNC0_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC0_OEN_SEL    (BIT(10))
#define GPIO_FUNC0_OEN_SEL_M  (GPIO_FUNC0_OEN_SEL_V << GPIO_FUNC0_OEN_SEL_S)
#define GPIO_FUNC0_OEN_SEL_V  0x00000001U
#define GPIO_FUNC0_OEN_SEL_S  10
/** GPIO_FUNC0_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC0_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC0_OEN_INV_SEL_M  (GPIO_FUNC0_OEN_INV_SEL_V << GPIO_FUNC0_OEN_INV_SEL_S)
#define GPIO_FUNC0_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OEN_INV_SEL_S  11

/** GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x55c)
/** GPIO_FUNC1_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC1_OUT_SEL    0x000001FFU
#define GPIO_FUNC1_OUT_SEL_M  (GPIO_FUNC1_OUT_SEL_V << GPIO_FUNC1_OUT_SEL_S)
#define GPIO_FUNC1_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC1_OUT_SEL_S  0
/** GPIO_FUNC1_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC1_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC1_OUT_INV_SEL_M  (GPIO_FUNC1_OUT_INV_SEL_V << GPIO_FUNC1_OUT_INV_SEL_S)
#define GPIO_FUNC1_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OUT_INV_SEL_S  9
/** GPIO_FUNC1_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC1_OEN_SEL    (BIT(10))
#define GPIO_FUNC1_OEN_SEL_M  (GPIO_FUNC1_OEN_SEL_V << GPIO_FUNC1_OEN_SEL_S)
#define GPIO_FUNC1_OEN_SEL_V  0x00000001U
#define GPIO_FUNC1_OEN_SEL_S  10
/** GPIO_FUNC1_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC1_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC1_OEN_INV_SEL_M  (GPIO_FUNC1_OEN_INV_SEL_V << GPIO_FUNC1_OEN_INV_SEL_S)
#define GPIO_FUNC1_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OEN_INV_SEL_S  11

/** GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x560)
/** GPIO_FUNC2_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC2_OUT_SEL    0x000001FFU
#define GPIO_FUNC2_OUT_SEL_M  (GPIO_FUNC2_OUT_SEL_V << GPIO_FUNC2_OUT_SEL_S)
#define GPIO_FUNC2_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC2_OUT_SEL_S  0
/** GPIO_FUNC2_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC2_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC2_OUT_INV_SEL_M  (GPIO_FUNC2_OUT_INV_SEL_V << GPIO_FUNC2_OUT_INV_SEL_S)
#define GPIO_FUNC2_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OUT_INV_SEL_S  9
/** GPIO_FUNC2_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC2_OEN_SEL    (BIT(10))
#define GPIO_FUNC2_OEN_SEL_M  (GPIO_FUNC2_OEN_SEL_V << GPIO_FUNC2_OEN_SEL_S)
#define GPIO_FUNC2_OEN_SEL_V  0x00000001U
#define GPIO_FUNC2_OEN_SEL_S  10
/** GPIO_FUNC2_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC2_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC2_OEN_INV_SEL_M  (GPIO_FUNC2_OEN_INV_SEL_V << GPIO_FUNC2_OEN_INV_SEL_S)
#define GPIO_FUNC2_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OEN_INV_SEL_S  11

/** GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x564)
/** GPIO_FUNC3_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC3_OUT_SEL    0x000001FFU
#define GPIO_FUNC3_OUT_SEL_M  (GPIO_FUNC3_OUT_SEL_V << GPIO_FUNC3_OUT_SEL_S)
#define GPIO_FUNC3_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC3_OUT_SEL_S  0
/** GPIO_FUNC3_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC3_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC3_OUT_INV_SEL_M  (GPIO_FUNC3_OUT_INV_SEL_V << GPIO_FUNC3_OUT_INV_SEL_S)
#define GPIO_FUNC3_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OUT_INV_SEL_S  9
/** GPIO_FUNC3_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC3_OEN_SEL    (BIT(10))
#define GPIO_FUNC3_OEN_SEL_M  (GPIO_FUNC3_OEN_SEL_V << GPIO_FUNC3_OEN_SEL_S)
#define GPIO_FUNC3_OEN_SEL_V  0x00000001U
#define GPIO_FUNC3_OEN_SEL_S  10
/** GPIO_FUNC3_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC3_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC3_OEN_INV_SEL_M  (GPIO_FUNC3_OEN_INV_SEL_V << GPIO_FUNC3_OEN_INV_SEL_S)
#define GPIO_FUNC3_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OEN_INV_SEL_S  11

/** GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x568)
/** GPIO_FUNC4_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC4_OUT_SEL    0x000001FFU
#define GPIO_FUNC4_OUT_SEL_M  (GPIO_FUNC4_OUT_SEL_V << GPIO_FUNC4_OUT_SEL_S)
#define GPIO_FUNC4_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC4_OUT_SEL_S  0
/** GPIO_FUNC4_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC4_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC4_OUT_INV_SEL_M  (GPIO_FUNC4_OUT_INV_SEL_V << GPIO_FUNC4_OUT_INV_SEL_S)
#define GPIO_FUNC4_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OUT_INV_SEL_S  9
/** GPIO_FUNC4_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC4_OEN_SEL    (BIT(10))
#define GPIO_FUNC4_OEN_SEL_M  (GPIO_FUNC4_OEN_SEL_V << GPIO_FUNC4_OEN_SEL_S)
#define GPIO_FUNC4_OEN_SEL_V  0x00000001U
#define GPIO_FUNC4_OEN_SEL_S  10
/** GPIO_FUNC4_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC4_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC4_OEN_INV_SEL_M  (GPIO_FUNC4_OEN_INV_SEL_V << GPIO_FUNC4_OEN_INV_SEL_S)
#define GPIO_FUNC4_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OEN_INV_SEL_S  11

/** GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x56c)
/** GPIO_FUNC5_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC5_OUT_SEL    0x000001FFU
#define GPIO_FUNC5_OUT_SEL_M  (GPIO_FUNC5_OUT_SEL_V << GPIO_FUNC5_OUT_SEL_S)
#define GPIO_FUNC5_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC5_OUT_SEL_S  0
/** GPIO_FUNC5_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC5_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC5_OUT_INV_SEL_M  (GPIO_FUNC5_OUT_INV_SEL_V << GPIO_FUNC5_OUT_INV_SEL_S)
#define GPIO_FUNC5_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OUT_INV_SEL_S  9
/** GPIO_FUNC5_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC5_OEN_SEL    (BIT(10))
#define GPIO_FUNC5_OEN_SEL_M  (GPIO_FUNC5_OEN_SEL_V << GPIO_FUNC5_OEN_SEL_S)
#define GPIO_FUNC5_OEN_SEL_V  0x00000001U
#define GPIO_FUNC5_OEN_SEL_S  10
/** GPIO_FUNC5_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC5_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC5_OEN_INV_SEL_M  (GPIO_FUNC5_OEN_INV_SEL_V << GPIO_FUNC5_OEN_INV_SEL_S)
#define GPIO_FUNC5_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OEN_INV_SEL_S  11

/** GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x570)
/** GPIO_FUNC6_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC6_OUT_SEL    0x000001FFU
#define GPIO_FUNC6_OUT_SEL_M  (GPIO_FUNC6_OUT_SEL_V << GPIO_FUNC6_OUT_SEL_S)
#define GPIO_FUNC6_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC6_OUT_SEL_S  0
/** GPIO_FUNC6_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC6_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC6_OUT_INV_SEL_M  (GPIO_FUNC6_OUT_INV_SEL_V << GPIO_FUNC6_OUT_INV_SEL_S)
#define GPIO_FUNC6_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OUT_INV_SEL_S  9
/** GPIO_FUNC6_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC6_OEN_SEL    (BIT(10))
#define GPIO_FUNC6_OEN_SEL_M  (GPIO_FUNC6_OEN_SEL_V << GPIO_FUNC6_OEN_SEL_S)
#define GPIO_FUNC6_OEN_SEL_V  0x00000001U
#define GPIO_FUNC6_OEN_SEL_S  10
/** GPIO_FUNC6_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC6_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC6_OEN_INV_SEL_M  (GPIO_FUNC6_OEN_INV_SEL_V << GPIO_FUNC6_OEN_INV_SEL_S)
#define GPIO_FUNC6_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OEN_INV_SEL_S  11

/** GPIO_FUNC7_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC7_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x574)
/** GPIO_FUNC7_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC7_OUT_SEL    0x000001FFU
#define GPIO_FUNC7_OUT_SEL_M  (GPIO_FUNC7_OUT_SEL_V << GPIO_FUNC7_OUT_SEL_S)
#define GPIO_FUNC7_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC7_OUT_SEL_S  0
/** GPIO_FUNC7_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC7_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC7_OUT_INV_SEL_M  (GPIO_FUNC7_OUT_INV_SEL_V << GPIO_FUNC7_OUT_INV_SEL_S)
#define GPIO_FUNC7_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OUT_INV_SEL_S  9
/** GPIO_FUNC7_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC7_OEN_SEL    (BIT(10))
#define GPIO_FUNC7_OEN_SEL_M  (GPIO_FUNC7_OEN_SEL_V << GPIO_FUNC7_OEN_SEL_S)
#define GPIO_FUNC7_OEN_SEL_V  0x00000001U
#define GPIO_FUNC7_OEN_SEL_S  10
/** GPIO_FUNC7_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC7_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC7_OEN_INV_SEL_M  (GPIO_FUNC7_OEN_INV_SEL_V << GPIO_FUNC7_OEN_INV_SEL_S)
#define GPIO_FUNC7_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OEN_INV_SEL_S  11

/** GPIO_FUNC8_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC8_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x578)
/** GPIO_FUNC8_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC8_OUT_SEL    0x000001FFU
#define GPIO_FUNC8_OUT_SEL_M  (GPIO_FUNC8_OUT_SEL_V << GPIO_FUNC8_OUT_SEL_S)
#define GPIO_FUNC8_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC8_OUT_SEL_S  0
/** GPIO_FUNC8_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC8_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC8_OUT_INV_SEL_M  (GPIO_FUNC8_OUT_INV_SEL_V << GPIO_FUNC8_OUT_INV_SEL_S)
#define GPIO_FUNC8_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OUT_INV_SEL_S  9
/** GPIO_FUNC8_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC8_OEN_SEL    (BIT(10))
#define GPIO_FUNC8_OEN_SEL_M  (GPIO_FUNC8_OEN_SEL_V << GPIO_FUNC8_OEN_SEL_S)
#define GPIO_FUNC8_OEN_SEL_V  0x00000001U
#define GPIO_FUNC8_OEN_SEL_S  10
/** GPIO_FUNC8_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC8_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC8_OEN_INV_SEL_M  (GPIO_FUNC8_OEN_INV_SEL_V << GPIO_FUNC8_OEN_INV_SEL_S)
#define GPIO_FUNC8_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OEN_INV_SEL_S  11

/** GPIO_FUNC9_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC9_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x57c)
/** GPIO_FUNC9_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC9_OUT_SEL    0x000001FFU
#define GPIO_FUNC9_OUT_SEL_M  (GPIO_FUNC9_OUT_SEL_V << GPIO_FUNC9_OUT_SEL_S)
#define GPIO_FUNC9_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC9_OUT_SEL_S  0
/** GPIO_FUNC9_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC9_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC9_OUT_INV_SEL_M  (GPIO_FUNC9_OUT_INV_SEL_V << GPIO_FUNC9_OUT_INV_SEL_S)
#define GPIO_FUNC9_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OUT_INV_SEL_S  9
/** GPIO_FUNC9_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC9_OEN_SEL    (BIT(10))
#define GPIO_FUNC9_OEN_SEL_M  (GPIO_FUNC9_OEN_SEL_V << GPIO_FUNC9_OEN_SEL_S)
#define GPIO_FUNC9_OEN_SEL_V  0x00000001U
#define GPIO_FUNC9_OEN_SEL_S  10
/** GPIO_FUNC9_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC9_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC9_OEN_INV_SEL_M  (GPIO_FUNC9_OEN_INV_SEL_V << GPIO_FUNC9_OEN_INV_SEL_S)
#define GPIO_FUNC9_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OEN_INV_SEL_S  11

/** GPIO_FUNC10_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC10_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x580)
/** GPIO_FUNC10_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC10_OUT_SEL    0x000001FFU
#define GPIO_FUNC10_OUT_SEL_M  (GPIO_FUNC10_OUT_SEL_V << GPIO_FUNC10_OUT_SEL_S)
#define GPIO_FUNC10_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC10_OUT_SEL_S  0
/** GPIO_FUNC10_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC10_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC10_OUT_INV_SEL_M  (GPIO_FUNC10_OUT_INV_SEL_V << GPIO_FUNC10_OUT_INV_SEL_S)
#define GPIO_FUNC10_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OUT_INV_SEL_S  9
/** GPIO_FUNC10_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC10_OEN_SEL    (BIT(10))
#define GPIO_FUNC10_OEN_SEL_M  (GPIO_FUNC10_OEN_SEL_V << GPIO_FUNC10_OEN_SEL_S)
#define GPIO_FUNC10_OEN_SEL_V  0x00000001U
#define GPIO_FUNC10_OEN_SEL_S  10
/** GPIO_FUNC10_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC10_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC10_OEN_INV_SEL_M  (GPIO_FUNC10_OEN_INV_SEL_V << GPIO_FUNC10_OEN_INV_SEL_S)
#define GPIO_FUNC10_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OEN_INV_SEL_S  11

/** GPIO_FUNC11_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC11_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x584)
/** GPIO_FUNC11_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC11_OUT_SEL    0x000001FFU
#define GPIO_FUNC11_OUT_SEL_M  (GPIO_FUNC11_OUT_SEL_V << GPIO_FUNC11_OUT_SEL_S)
#define GPIO_FUNC11_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC11_OUT_SEL_S  0
/** GPIO_FUNC11_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC11_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC11_OUT_INV_SEL_M  (GPIO_FUNC11_OUT_INV_SEL_V << GPIO_FUNC11_OUT_INV_SEL_S)
#define GPIO_FUNC11_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OUT_INV_SEL_S  9
/** GPIO_FUNC11_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC11_OEN_SEL    (BIT(10))
#define GPIO_FUNC11_OEN_SEL_M  (GPIO_FUNC11_OEN_SEL_V << GPIO_FUNC11_OEN_SEL_S)
#define GPIO_FUNC11_OEN_SEL_V  0x00000001U
#define GPIO_FUNC11_OEN_SEL_S  10
/** GPIO_FUNC11_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC11_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC11_OEN_INV_SEL_M  (GPIO_FUNC11_OEN_INV_SEL_V << GPIO_FUNC11_OEN_INV_SEL_S)
#define GPIO_FUNC11_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OEN_INV_SEL_S  11

/** GPIO_FUNC12_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC12_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x588)
/** GPIO_FUNC12_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC12_OUT_SEL    0x000001FFU
#define GPIO_FUNC12_OUT_SEL_M  (GPIO_FUNC12_OUT_SEL_V << GPIO_FUNC12_OUT_SEL_S)
#define GPIO_FUNC12_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC12_OUT_SEL_S  0
/** GPIO_FUNC12_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC12_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC12_OUT_INV_SEL_M  (GPIO_FUNC12_OUT_INV_SEL_V << GPIO_FUNC12_OUT_INV_SEL_S)
#define GPIO_FUNC12_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OUT_INV_SEL_S  9
/** GPIO_FUNC12_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC12_OEN_SEL    (BIT(10))
#define GPIO_FUNC12_OEN_SEL_M  (GPIO_FUNC12_OEN_SEL_V << GPIO_FUNC12_OEN_SEL_S)
#define GPIO_FUNC12_OEN_SEL_V  0x00000001U
#define GPIO_FUNC12_OEN_SEL_S  10
/** GPIO_FUNC12_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC12_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC12_OEN_INV_SEL_M  (GPIO_FUNC12_OEN_INV_SEL_V << GPIO_FUNC12_OEN_INV_SEL_S)
#define GPIO_FUNC12_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OEN_INV_SEL_S  11

/** GPIO_FUNC13_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC13_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x58c)
/** GPIO_FUNC13_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC13_OUT_SEL    0x000001FFU
#define GPIO_FUNC13_OUT_SEL_M  (GPIO_FUNC13_OUT_SEL_V << GPIO_FUNC13_OUT_SEL_S)
#define GPIO_FUNC13_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC13_OUT_SEL_S  0
/** GPIO_FUNC13_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC13_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC13_OUT_INV_SEL_M  (GPIO_FUNC13_OUT_INV_SEL_V << GPIO_FUNC13_OUT_INV_SEL_S)
#define GPIO_FUNC13_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OUT_INV_SEL_S  9
/** GPIO_FUNC13_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC13_OEN_SEL    (BIT(10))
#define GPIO_FUNC13_OEN_SEL_M  (GPIO_FUNC13_OEN_SEL_V << GPIO_FUNC13_OEN_SEL_S)
#define GPIO_FUNC13_OEN_SEL_V  0x00000001U
#define GPIO_FUNC13_OEN_SEL_S  10
/** GPIO_FUNC13_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC13_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC13_OEN_INV_SEL_M  (GPIO_FUNC13_OEN_INV_SEL_V << GPIO_FUNC13_OEN_INV_SEL_S)
#define GPIO_FUNC13_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OEN_INV_SEL_S  11

/** GPIO_FUNC14_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC14_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x590)
/** GPIO_FUNC14_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC14_OUT_SEL    0x000001FFU
#define GPIO_FUNC14_OUT_SEL_M  (GPIO_FUNC14_OUT_SEL_V << GPIO_FUNC14_OUT_SEL_S)
#define GPIO_FUNC14_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC14_OUT_SEL_S  0
/** GPIO_FUNC14_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC14_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC14_OUT_INV_SEL_M  (GPIO_FUNC14_OUT_INV_SEL_V << GPIO_FUNC14_OUT_INV_SEL_S)
#define GPIO_FUNC14_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OUT_INV_SEL_S  9
/** GPIO_FUNC14_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC14_OEN_SEL    (BIT(10))
#define GPIO_FUNC14_OEN_SEL_M  (GPIO_FUNC14_OEN_SEL_V << GPIO_FUNC14_OEN_SEL_S)
#define GPIO_FUNC14_OEN_SEL_V  0x00000001U
#define GPIO_FUNC14_OEN_SEL_S  10
/** GPIO_FUNC14_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC14_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC14_OEN_INV_SEL_M  (GPIO_FUNC14_OEN_INV_SEL_V << GPIO_FUNC14_OEN_INV_SEL_S)
#define GPIO_FUNC14_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OEN_INV_SEL_S  11

/** GPIO_FUNC15_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC15_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x594)
/** GPIO_FUNC15_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC15_OUT_SEL    0x000001FFU
#define GPIO_FUNC15_OUT_SEL_M  (GPIO_FUNC15_OUT_SEL_V << GPIO_FUNC15_OUT_SEL_S)
#define GPIO_FUNC15_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC15_OUT_SEL_S  0
/** GPIO_FUNC15_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC15_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC15_OUT_INV_SEL_M  (GPIO_FUNC15_OUT_INV_SEL_V << GPIO_FUNC15_OUT_INV_SEL_S)
#define GPIO_FUNC15_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OUT_INV_SEL_S  9
/** GPIO_FUNC15_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC15_OEN_SEL    (BIT(10))
#define GPIO_FUNC15_OEN_SEL_M  (GPIO_FUNC15_OEN_SEL_V << GPIO_FUNC15_OEN_SEL_S)
#define GPIO_FUNC15_OEN_SEL_V  0x00000001U
#define GPIO_FUNC15_OEN_SEL_S  10
/** GPIO_FUNC15_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC15_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC15_OEN_INV_SEL_M  (GPIO_FUNC15_OEN_INV_SEL_V << GPIO_FUNC15_OEN_INV_SEL_S)
#define GPIO_FUNC15_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OEN_INV_SEL_S  11

/** GPIO_FUNC16_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC16_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x598)
/** GPIO_FUNC16_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC16_OUT_SEL    0x000001FFU
#define GPIO_FUNC16_OUT_SEL_M  (GPIO_FUNC16_OUT_SEL_V << GPIO_FUNC16_OUT_SEL_S)
#define GPIO_FUNC16_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC16_OUT_SEL_S  0
/** GPIO_FUNC16_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC16_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC16_OUT_INV_SEL_M  (GPIO_FUNC16_OUT_INV_SEL_V << GPIO_FUNC16_OUT_INV_SEL_S)
#define GPIO_FUNC16_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OUT_INV_SEL_S  9
/** GPIO_FUNC16_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC16_OEN_SEL    (BIT(10))
#define GPIO_FUNC16_OEN_SEL_M  (GPIO_FUNC16_OEN_SEL_V << GPIO_FUNC16_OEN_SEL_S)
#define GPIO_FUNC16_OEN_SEL_V  0x00000001U
#define GPIO_FUNC16_OEN_SEL_S  10
/** GPIO_FUNC16_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC16_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC16_OEN_INV_SEL_M  (GPIO_FUNC16_OEN_INV_SEL_V << GPIO_FUNC16_OEN_INV_SEL_S)
#define GPIO_FUNC16_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OEN_INV_SEL_S  11

/** GPIO_FUNC17_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC17_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x59c)
/** GPIO_FUNC17_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC17_OUT_SEL    0x000001FFU
#define GPIO_FUNC17_OUT_SEL_M  (GPIO_FUNC17_OUT_SEL_V << GPIO_FUNC17_OUT_SEL_S)
#define GPIO_FUNC17_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC17_OUT_SEL_S  0
/** GPIO_FUNC17_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC17_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC17_OUT_INV_SEL_M  (GPIO_FUNC17_OUT_INV_SEL_V << GPIO_FUNC17_OUT_INV_SEL_S)
#define GPIO_FUNC17_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OUT_INV_SEL_S  9
/** GPIO_FUNC17_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC17_OEN_SEL    (BIT(10))
#define GPIO_FUNC17_OEN_SEL_M  (GPIO_FUNC17_OEN_SEL_V << GPIO_FUNC17_OEN_SEL_S)
#define GPIO_FUNC17_OEN_SEL_V  0x00000001U
#define GPIO_FUNC17_OEN_SEL_S  10
/** GPIO_FUNC17_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC17_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC17_OEN_INV_SEL_M  (GPIO_FUNC17_OEN_INV_SEL_V << GPIO_FUNC17_OEN_INV_SEL_S)
#define GPIO_FUNC17_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OEN_INV_SEL_S  11

/** GPIO_FUNC18_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC18_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a0)
/** GPIO_FUNC18_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC18_OUT_SEL    0x000001FFU
#define GPIO_FUNC18_OUT_SEL_M  (GPIO_FUNC18_OUT_SEL_V << GPIO_FUNC18_OUT_SEL_S)
#define GPIO_FUNC18_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC18_OUT_SEL_S  0
/** GPIO_FUNC18_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC18_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC18_OUT_INV_SEL_M  (GPIO_FUNC18_OUT_INV_SEL_V << GPIO_FUNC18_OUT_INV_SEL_S)
#define GPIO_FUNC18_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OUT_INV_SEL_S  9
/** GPIO_FUNC18_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC18_OEN_SEL    (BIT(10))
#define GPIO_FUNC18_OEN_SEL_M  (GPIO_FUNC18_OEN_SEL_V << GPIO_FUNC18_OEN_SEL_S)
#define GPIO_FUNC18_OEN_SEL_V  0x00000001U
#define GPIO_FUNC18_OEN_SEL_S  10
/** GPIO_FUNC18_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC18_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC18_OEN_INV_SEL_M  (GPIO_FUNC18_OEN_INV_SEL_V << GPIO_FUNC18_OEN_INV_SEL_S)
#define GPIO_FUNC18_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OEN_INV_SEL_S  11

/** GPIO_FUNC19_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC19_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a4)
/** GPIO_FUNC19_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC19_OUT_SEL    0x000001FFU
#define GPIO_FUNC19_OUT_SEL_M  (GPIO_FUNC19_OUT_SEL_V << GPIO_FUNC19_OUT_SEL_S)
#define GPIO_FUNC19_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC19_OUT_SEL_S  0
/** GPIO_FUNC19_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC19_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC19_OUT_INV_SEL_M  (GPIO_FUNC19_OUT_INV_SEL_V << GPIO_FUNC19_OUT_INV_SEL_S)
#define GPIO_FUNC19_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OUT_INV_SEL_S  9
/** GPIO_FUNC19_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC19_OEN_SEL    (BIT(10))
#define GPIO_FUNC19_OEN_SEL_M  (GPIO_FUNC19_OEN_SEL_V << GPIO_FUNC19_OEN_SEL_S)
#define GPIO_FUNC19_OEN_SEL_V  0x00000001U
#define GPIO_FUNC19_OEN_SEL_S  10
/** GPIO_FUNC19_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC19_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC19_OEN_INV_SEL_M  (GPIO_FUNC19_OEN_INV_SEL_V << GPIO_FUNC19_OEN_INV_SEL_S)
#define GPIO_FUNC19_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OEN_INV_SEL_S  11

/** GPIO_FUNC20_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC20_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a8)
/** GPIO_FUNC20_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC20_OUT_SEL    0x000001FFU
#define GPIO_FUNC20_OUT_SEL_M  (GPIO_FUNC20_OUT_SEL_V << GPIO_FUNC20_OUT_SEL_S)
#define GPIO_FUNC20_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC20_OUT_SEL_S  0
/** GPIO_FUNC20_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC20_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC20_OUT_INV_SEL_M  (GPIO_FUNC20_OUT_INV_SEL_V << GPIO_FUNC20_OUT_INV_SEL_S)
#define GPIO_FUNC20_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OUT_INV_SEL_S  9
/** GPIO_FUNC20_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC20_OEN_SEL    (BIT(10))
#define GPIO_FUNC20_OEN_SEL_M  (GPIO_FUNC20_OEN_SEL_V << GPIO_FUNC20_OEN_SEL_S)
#define GPIO_FUNC20_OEN_SEL_V  0x00000001U
#define GPIO_FUNC20_OEN_SEL_S  10
/** GPIO_FUNC20_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC20_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC20_OEN_INV_SEL_M  (GPIO_FUNC20_OEN_INV_SEL_V << GPIO_FUNC20_OEN_INV_SEL_S)
#define GPIO_FUNC20_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OEN_INV_SEL_S  11

/** GPIO_FUNC21_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC21_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5ac)
/** GPIO_FUNC21_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC21_OUT_SEL    0x000001FFU
#define GPIO_FUNC21_OUT_SEL_M  (GPIO_FUNC21_OUT_SEL_V << GPIO_FUNC21_OUT_SEL_S)
#define GPIO_FUNC21_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC21_OUT_SEL_S  0
/** GPIO_FUNC21_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC21_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC21_OUT_INV_SEL_M  (GPIO_FUNC21_OUT_INV_SEL_V << GPIO_FUNC21_OUT_INV_SEL_S)
#define GPIO_FUNC21_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OUT_INV_SEL_S  9
/** GPIO_FUNC21_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC21_OEN_SEL    (BIT(10))
#define GPIO_FUNC21_OEN_SEL_M  (GPIO_FUNC21_OEN_SEL_V << GPIO_FUNC21_OEN_SEL_S)
#define GPIO_FUNC21_OEN_SEL_V  0x00000001U
#define GPIO_FUNC21_OEN_SEL_S  10
/** GPIO_FUNC21_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC21_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC21_OEN_INV_SEL_M  (GPIO_FUNC21_OEN_INV_SEL_V << GPIO_FUNC21_OEN_INV_SEL_S)
#define GPIO_FUNC21_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OEN_INV_SEL_S  11

/** GPIO_FUNC22_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC22_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b0)
/** GPIO_FUNC22_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC22_OUT_SEL    0x000001FFU
#define GPIO_FUNC22_OUT_SEL_M  (GPIO_FUNC22_OUT_SEL_V << GPIO_FUNC22_OUT_SEL_S)
#define GPIO_FUNC22_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC22_OUT_SEL_S  0
/** GPIO_FUNC22_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC22_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC22_OUT_INV_SEL_M  (GPIO_FUNC22_OUT_INV_SEL_V << GPIO_FUNC22_OUT_INV_SEL_S)
#define GPIO_FUNC22_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OUT_INV_SEL_S  9
/** GPIO_FUNC22_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC22_OEN_SEL    (BIT(10))
#define GPIO_FUNC22_OEN_SEL_M  (GPIO_FUNC22_OEN_SEL_V << GPIO_FUNC22_OEN_SEL_S)
#define GPIO_FUNC22_OEN_SEL_V  0x00000001U
#define GPIO_FUNC22_OEN_SEL_S  10
/** GPIO_FUNC22_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC22_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC22_OEN_INV_SEL_M  (GPIO_FUNC22_OEN_INV_SEL_V << GPIO_FUNC22_OEN_INV_SEL_S)
#define GPIO_FUNC22_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OEN_INV_SEL_S  11

/** GPIO_FUNC23_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC23_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b4)
/** GPIO_FUNC23_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC23_OUT_SEL    0x000001FFU
#define GPIO_FUNC23_OUT_SEL_M  (GPIO_FUNC23_OUT_SEL_V << GPIO_FUNC23_OUT_SEL_S)
#define GPIO_FUNC23_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC23_OUT_SEL_S  0
/** GPIO_FUNC23_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC23_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC23_OUT_INV_SEL_M  (GPIO_FUNC23_OUT_INV_SEL_V << GPIO_FUNC23_OUT_INV_SEL_S)
#define GPIO_FUNC23_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OUT_INV_SEL_S  9
/** GPIO_FUNC23_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC23_OEN_SEL    (BIT(10))
#define GPIO_FUNC23_OEN_SEL_M  (GPIO_FUNC23_OEN_SEL_V << GPIO_FUNC23_OEN_SEL_S)
#define GPIO_FUNC23_OEN_SEL_V  0x00000001U
#define GPIO_FUNC23_OEN_SEL_S  10
/** GPIO_FUNC23_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC23_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC23_OEN_INV_SEL_M  (GPIO_FUNC23_OEN_INV_SEL_V << GPIO_FUNC23_OEN_INV_SEL_S)
#define GPIO_FUNC23_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OEN_INV_SEL_S  11

/** GPIO_FUNC24_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC24_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b8)
/** GPIO_FUNC24_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC24_OUT_SEL    0x000001FFU
#define GPIO_FUNC24_OUT_SEL_M  (GPIO_FUNC24_OUT_SEL_V << GPIO_FUNC24_OUT_SEL_S)
#define GPIO_FUNC24_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC24_OUT_SEL_S  0
/** GPIO_FUNC24_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC24_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC24_OUT_INV_SEL_M  (GPIO_FUNC24_OUT_INV_SEL_V << GPIO_FUNC24_OUT_INV_SEL_S)
#define GPIO_FUNC24_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OUT_INV_SEL_S  9
/** GPIO_FUNC24_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC24_OEN_SEL    (BIT(10))
#define GPIO_FUNC24_OEN_SEL_M  (GPIO_FUNC24_OEN_SEL_V << GPIO_FUNC24_OEN_SEL_S)
#define GPIO_FUNC24_OEN_SEL_V  0x00000001U
#define GPIO_FUNC24_OEN_SEL_S  10
/** GPIO_FUNC24_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC24_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC24_OEN_INV_SEL_M  (GPIO_FUNC24_OEN_INV_SEL_V << GPIO_FUNC24_OEN_INV_SEL_S)
#define GPIO_FUNC24_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OEN_INV_SEL_S  11

/** GPIO_FUNC25_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC25_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5bc)
/** GPIO_FUNC25_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC25_OUT_SEL    0x000001FFU
#define GPIO_FUNC25_OUT_SEL_M  (GPIO_FUNC25_OUT_SEL_V << GPIO_FUNC25_OUT_SEL_S)
#define GPIO_FUNC25_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC25_OUT_SEL_S  0
/** GPIO_FUNC25_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC25_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC25_OUT_INV_SEL_M  (GPIO_FUNC25_OUT_INV_SEL_V << GPIO_FUNC25_OUT_INV_SEL_S)
#define GPIO_FUNC25_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OUT_INV_SEL_S  9
/** GPIO_FUNC25_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC25_OEN_SEL    (BIT(10))
#define GPIO_FUNC25_OEN_SEL_M  (GPIO_FUNC25_OEN_SEL_V << GPIO_FUNC25_OEN_SEL_S)
#define GPIO_FUNC25_OEN_SEL_V  0x00000001U
#define GPIO_FUNC25_OEN_SEL_S  10
/** GPIO_FUNC25_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC25_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC25_OEN_INV_SEL_M  (GPIO_FUNC25_OEN_INV_SEL_V << GPIO_FUNC25_OEN_INV_SEL_S)
#define GPIO_FUNC25_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OEN_INV_SEL_S  11

/** GPIO_FUNC26_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC26_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c0)
/** GPIO_FUNC26_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC26_OUT_SEL    0x000001FFU
#define GPIO_FUNC26_OUT_SEL_M  (GPIO_FUNC26_OUT_SEL_V << GPIO_FUNC26_OUT_SEL_S)
#define GPIO_FUNC26_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC26_OUT_SEL_S  0
/** GPIO_FUNC26_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC26_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC26_OUT_INV_SEL_M  (GPIO_FUNC26_OUT_INV_SEL_V << GPIO_FUNC26_OUT_INV_SEL_S)
#define GPIO_FUNC26_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OUT_INV_SEL_S  9
/** GPIO_FUNC26_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC26_OEN_SEL    (BIT(10))
#define GPIO_FUNC26_OEN_SEL_M  (GPIO_FUNC26_OEN_SEL_V << GPIO_FUNC26_OEN_SEL_S)
#define GPIO_FUNC26_OEN_SEL_V  0x00000001U
#define GPIO_FUNC26_OEN_SEL_S  10
/** GPIO_FUNC26_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC26_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC26_OEN_INV_SEL_M  (GPIO_FUNC26_OEN_INV_SEL_V << GPIO_FUNC26_OEN_INV_SEL_S)
#define GPIO_FUNC26_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OEN_INV_SEL_S  11

/** GPIO_FUNC27_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC27_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c4)
/** GPIO_FUNC27_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC27_OUT_SEL    0x000001FFU
#define GPIO_FUNC27_OUT_SEL_M  (GPIO_FUNC27_OUT_SEL_V << GPIO_FUNC27_OUT_SEL_S)
#define GPIO_FUNC27_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC27_OUT_SEL_S  0
/** GPIO_FUNC27_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC27_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC27_OUT_INV_SEL_M  (GPIO_FUNC27_OUT_INV_SEL_V << GPIO_FUNC27_OUT_INV_SEL_S)
#define GPIO_FUNC27_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OUT_INV_SEL_S  9
/** GPIO_FUNC27_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC27_OEN_SEL    (BIT(10))
#define GPIO_FUNC27_OEN_SEL_M  (GPIO_FUNC27_OEN_SEL_V << GPIO_FUNC27_OEN_SEL_S)
#define GPIO_FUNC27_OEN_SEL_V  0x00000001U
#define GPIO_FUNC27_OEN_SEL_S  10
/** GPIO_FUNC27_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC27_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC27_OEN_INV_SEL_M  (GPIO_FUNC27_OEN_INV_SEL_V << GPIO_FUNC27_OEN_INV_SEL_S)
#define GPIO_FUNC27_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OEN_INV_SEL_S  11

/** GPIO_FUNC28_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC28_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c8)
/** GPIO_FUNC28_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC28_OUT_SEL    0x000001FFU
#define GPIO_FUNC28_OUT_SEL_M  (GPIO_FUNC28_OUT_SEL_V << GPIO_FUNC28_OUT_SEL_S)
#define GPIO_FUNC28_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC28_OUT_SEL_S  0
/** GPIO_FUNC28_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC28_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC28_OUT_INV_SEL_M  (GPIO_FUNC28_OUT_INV_SEL_V << GPIO_FUNC28_OUT_INV_SEL_S)
#define GPIO_FUNC28_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OUT_INV_SEL_S  9
/** GPIO_FUNC28_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC28_OEN_SEL    (BIT(10))
#define GPIO_FUNC28_OEN_SEL_M  (GPIO_FUNC28_OEN_SEL_V << GPIO_FUNC28_OEN_SEL_S)
#define GPIO_FUNC28_OEN_SEL_V  0x00000001U
#define GPIO_FUNC28_OEN_SEL_S  10
/** GPIO_FUNC28_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC28_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC28_OEN_INV_SEL_M  (GPIO_FUNC28_OEN_INV_SEL_V << GPIO_FUNC28_OEN_INV_SEL_S)
#define GPIO_FUNC28_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OEN_INV_SEL_S  11

/** GPIO_FUNC29_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC29_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5cc)
/** GPIO_FUNC29_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC29_OUT_SEL    0x000001FFU
#define GPIO_FUNC29_OUT_SEL_M  (GPIO_FUNC29_OUT_SEL_V << GPIO_FUNC29_OUT_SEL_S)
#define GPIO_FUNC29_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC29_OUT_SEL_S  0
/** GPIO_FUNC29_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC29_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC29_OUT_INV_SEL_M  (GPIO_FUNC29_OUT_INV_SEL_V << GPIO_FUNC29_OUT_INV_SEL_S)
#define GPIO_FUNC29_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OUT_INV_SEL_S  9
/** GPIO_FUNC29_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC29_OEN_SEL    (BIT(10))
#define GPIO_FUNC29_OEN_SEL_M  (GPIO_FUNC29_OEN_SEL_V << GPIO_FUNC29_OEN_SEL_S)
#define GPIO_FUNC29_OEN_SEL_V  0x00000001U
#define GPIO_FUNC29_OEN_SEL_S  10
/** GPIO_FUNC29_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC29_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC29_OEN_INV_SEL_M  (GPIO_FUNC29_OEN_INV_SEL_V << GPIO_FUNC29_OEN_INV_SEL_S)
#define GPIO_FUNC29_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OEN_INV_SEL_S  11

/** GPIO_FUNC30_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC30_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5d0)
/** GPIO_FUNC30_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC30_OUT_SEL    0x000001FFU
#define GPIO_FUNC30_OUT_SEL_M  (GPIO_FUNC30_OUT_SEL_V << GPIO_FUNC30_OUT_SEL_S)
#define GPIO_FUNC30_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC30_OUT_SEL_S  0
/** GPIO_FUNC30_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC30_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC30_OUT_INV_SEL_M  (GPIO_FUNC30_OUT_INV_SEL_V << GPIO_FUNC30_OUT_INV_SEL_S)
#define GPIO_FUNC30_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OUT_INV_SEL_S  9
/** GPIO_FUNC30_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC30_OEN_SEL    (BIT(10))
#define GPIO_FUNC30_OEN_SEL_M  (GPIO_FUNC30_OEN_SEL_V << GPIO_FUNC30_OEN_SEL_S)
#define GPIO_FUNC30_OEN_SEL_V  0x00000001U
#define GPIO_FUNC30_OEN_SEL_S  10
/** GPIO_FUNC30_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC30_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC30_OEN_INV_SEL_M  (GPIO_FUNC30_OEN_INV_SEL_V << GPIO_FUNC30_OEN_INV_SEL_S)
#define GPIO_FUNC30_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OEN_INV_SEL_S  11

/** GPIO_FUNC31_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC31_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5d4)
/** GPIO_FUNC31_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC31_OUT_SEL    0x000001FFU
#define GPIO_FUNC31_OUT_SEL_M  (GPIO_FUNC31_OUT_SEL_V << GPIO_FUNC31_OUT_SEL_S)
#define GPIO_FUNC31_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC31_OUT_SEL_S  0
/** GPIO_FUNC31_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC31_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC31_OUT_INV_SEL_M  (GPIO_FUNC31_OUT_INV_SEL_V << GPIO_FUNC31_OUT_INV_SEL_S)
#define GPIO_FUNC31_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OUT_INV_SEL_S  9
/** GPIO_FUNC31_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC31_OEN_SEL    (BIT(10))
#define GPIO_FUNC31_OEN_SEL_M  (GPIO_FUNC31_OEN_SEL_V << GPIO_FUNC31_OEN_SEL_S)
#define GPIO_FUNC31_OEN_SEL_V  0x00000001U
#define GPIO_FUNC31_OEN_SEL_S  10
/** GPIO_FUNC31_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC31_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC31_OEN_INV_SEL_M  (GPIO_FUNC31_OEN_INV_SEL_V << GPIO_FUNC31_OEN_INV_SEL_S)
#define GPIO_FUNC31_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OEN_INV_SEL_S  11

/** GPIO_FUNC32_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC32_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5d8)
/** GPIO_FUNC32_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC32_OUT_SEL    0x000001FFU
#define GPIO_FUNC32_OUT_SEL_M  (GPIO_FUNC32_OUT_SEL_V << GPIO_FUNC32_OUT_SEL_S)
#define GPIO_FUNC32_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC32_OUT_SEL_S  0
/** GPIO_FUNC32_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC32_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC32_OUT_INV_SEL_M  (GPIO_FUNC32_OUT_INV_SEL_V << GPIO_FUNC32_OUT_INV_SEL_S)
#define GPIO_FUNC32_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OUT_INV_SEL_S  9
/** GPIO_FUNC32_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC32_OEN_SEL    (BIT(10))
#define GPIO_FUNC32_OEN_SEL_M  (GPIO_FUNC32_OEN_SEL_V << GPIO_FUNC32_OEN_SEL_S)
#define GPIO_FUNC32_OEN_SEL_V  0x00000001U
#define GPIO_FUNC32_OEN_SEL_S  10
/** GPIO_FUNC32_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC32_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC32_OEN_INV_SEL_M  (GPIO_FUNC32_OEN_INV_SEL_V << GPIO_FUNC32_OEN_INV_SEL_S)
#define GPIO_FUNC32_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OEN_INV_SEL_S  11

/** GPIO_FUNC33_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC33_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5dc)
/** GPIO_FUNC33_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC33_OUT_SEL    0x000001FFU
#define GPIO_FUNC33_OUT_SEL_M  (GPIO_FUNC33_OUT_SEL_V << GPIO_FUNC33_OUT_SEL_S)
#define GPIO_FUNC33_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC33_OUT_SEL_S  0
/** GPIO_FUNC33_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC33_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC33_OUT_INV_SEL_M  (GPIO_FUNC33_OUT_INV_SEL_V << GPIO_FUNC33_OUT_INV_SEL_S)
#define GPIO_FUNC33_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_OUT_INV_SEL_S  9
/** GPIO_FUNC33_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC33_OEN_SEL    (BIT(10))
#define GPIO_FUNC33_OEN_SEL_M  (GPIO_FUNC33_OEN_SEL_V << GPIO_FUNC33_OEN_SEL_S)
#define GPIO_FUNC33_OEN_SEL_V  0x00000001U
#define GPIO_FUNC33_OEN_SEL_S  10
/** GPIO_FUNC33_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC33_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC33_OEN_INV_SEL_M  (GPIO_FUNC33_OEN_INV_SEL_V << GPIO_FUNC33_OEN_INV_SEL_S)
#define GPIO_FUNC33_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_OEN_INV_SEL_S  11

/** GPIO_FUNC34_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC34_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5e0)
/** GPIO_FUNC34_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC34_OUT_SEL    0x000001FFU
#define GPIO_FUNC34_OUT_SEL_M  (GPIO_FUNC34_OUT_SEL_V << GPIO_FUNC34_OUT_SEL_S)
#define GPIO_FUNC34_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC34_OUT_SEL_S  0
/** GPIO_FUNC34_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC34_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC34_OUT_INV_SEL_M  (GPIO_FUNC34_OUT_INV_SEL_V << GPIO_FUNC34_OUT_INV_SEL_S)
#define GPIO_FUNC34_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_OUT_INV_SEL_S  9
/** GPIO_FUNC34_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC34_OEN_SEL    (BIT(10))
#define GPIO_FUNC34_OEN_SEL_M  (GPIO_FUNC34_OEN_SEL_V << GPIO_FUNC34_OEN_SEL_S)
#define GPIO_FUNC34_OEN_SEL_V  0x00000001U
#define GPIO_FUNC34_OEN_SEL_S  10
/** GPIO_FUNC34_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC34_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC34_OEN_INV_SEL_M  (GPIO_FUNC34_OEN_INV_SEL_V << GPIO_FUNC34_OEN_INV_SEL_S)
#define GPIO_FUNC34_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_OEN_INV_SEL_S  11

/** GPIO_FUNC35_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC35_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5e4)
/** GPIO_FUNC35_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC35_OUT_SEL    0x000001FFU
#define GPIO_FUNC35_OUT_SEL_M  (GPIO_FUNC35_OUT_SEL_V << GPIO_FUNC35_OUT_SEL_S)
#define GPIO_FUNC35_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC35_OUT_SEL_S  0
/** GPIO_FUNC35_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC35_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC35_OUT_INV_SEL_M  (GPIO_FUNC35_OUT_INV_SEL_V << GPIO_FUNC35_OUT_INV_SEL_S)
#define GPIO_FUNC35_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_OUT_INV_SEL_S  9
/** GPIO_FUNC35_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC35_OEN_SEL    (BIT(10))
#define GPIO_FUNC35_OEN_SEL_M  (GPIO_FUNC35_OEN_SEL_V << GPIO_FUNC35_OEN_SEL_S)
#define GPIO_FUNC35_OEN_SEL_V  0x00000001U
#define GPIO_FUNC35_OEN_SEL_S  10
/** GPIO_FUNC35_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC35_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC35_OEN_INV_SEL_M  (GPIO_FUNC35_OEN_INV_SEL_V << GPIO_FUNC35_OEN_INV_SEL_S)
#define GPIO_FUNC35_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_OEN_INV_SEL_S  11

/** GPIO_FUNC36_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC36_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5e8)
/** GPIO_FUNC36_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC36_OUT_SEL    0x000001FFU
#define GPIO_FUNC36_OUT_SEL_M  (GPIO_FUNC36_OUT_SEL_V << GPIO_FUNC36_OUT_SEL_S)
#define GPIO_FUNC36_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC36_OUT_SEL_S  0
/** GPIO_FUNC36_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC36_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC36_OUT_INV_SEL_M  (GPIO_FUNC36_OUT_INV_SEL_V << GPIO_FUNC36_OUT_INV_SEL_S)
#define GPIO_FUNC36_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC36_OUT_INV_SEL_S  9
/** GPIO_FUNC36_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC36_OEN_SEL    (BIT(10))
#define GPIO_FUNC36_OEN_SEL_M  (GPIO_FUNC36_OEN_SEL_V << GPIO_FUNC36_OEN_SEL_S)
#define GPIO_FUNC36_OEN_SEL_V  0x00000001U
#define GPIO_FUNC36_OEN_SEL_S  10
/** GPIO_FUNC36_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC36_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC36_OEN_INV_SEL_M  (GPIO_FUNC36_OEN_INV_SEL_V << GPIO_FUNC36_OEN_INV_SEL_S)
#define GPIO_FUNC36_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC36_OEN_INV_SEL_S  11

/** GPIO_FUNC37_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC37_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5ec)
/** GPIO_FUNC37_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC37_OUT_SEL    0x000001FFU
#define GPIO_FUNC37_OUT_SEL_M  (GPIO_FUNC37_OUT_SEL_V << GPIO_FUNC37_OUT_SEL_S)
#define GPIO_FUNC37_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC37_OUT_SEL_S  0
/** GPIO_FUNC37_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC37_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC37_OUT_INV_SEL_M  (GPIO_FUNC37_OUT_INV_SEL_V << GPIO_FUNC37_OUT_INV_SEL_S)
#define GPIO_FUNC37_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC37_OUT_INV_SEL_S  9
/** GPIO_FUNC37_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC37_OEN_SEL    (BIT(10))
#define GPIO_FUNC37_OEN_SEL_M  (GPIO_FUNC37_OEN_SEL_V << GPIO_FUNC37_OEN_SEL_S)
#define GPIO_FUNC37_OEN_SEL_V  0x00000001U
#define GPIO_FUNC37_OEN_SEL_S  10
/** GPIO_FUNC37_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC37_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC37_OEN_INV_SEL_M  (GPIO_FUNC37_OEN_INV_SEL_V << GPIO_FUNC37_OEN_INV_SEL_S)
#define GPIO_FUNC37_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC37_OEN_INV_SEL_S  11

/** GPIO_FUNC38_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC38_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5f0)
/** GPIO_FUNC38_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC38_OUT_SEL    0x000001FFU
#define GPIO_FUNC38_OUT_SEL_M  (GPIO_FUNC38_OUT_SEL_V << GPIO_FUNC38_OUT_SEL_S)
#define GPIO_FUNC38_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC38_OUT_SEL_S  0
/** GPIO_FUNC38_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC38_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC38_OUT_INV_SEL_M  (GPIO_FUNC38_OUT_INV_SEL_V << GPIO_FUNC38_OUT_INV_SEL_S)
#define GPIO_FUNC38_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC38_OUT_INV_SEL_S  9
/** GPIO_FUNC38_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC38_OEN_SEL    (BIT(10))
#define GPIO_FUNC38_OEN_SEL_M  (GPIO_FUNC38_OEN_SEL_V << GPIO_FUNC38_OEN_SEL_S)
#define GPIO_FUNC38_OEN_SEL_V  0x00000001U
#define GPIO_FUNC38_OEN_SEL_S  10
/** GPIO_FUNC38_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC38_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC38_OEN_INV_SEL_M  (GPIO_FUNC38_OEN_INV_SEL_V << GPIO_FUNC38_OEN_INV_SEL_S)
#define GPIO_FUNC38_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC38_OEN_INV_SEL_S  11

/** GPIO_FUNC39_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC39_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5f4)
/** GPIO_FUNC39_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC39_OUT_SEL    0x000001FFU
#define GPIO_FUNC39_OUT_SEL_M  (GPIO_FUNC39_OUT_SEL_V << GPIO_FUNC39_OUT_SEL_S)
#define GPIO_FUNC39_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC39_OUT_SEL_S  0
/** GPIO_FUNC39_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC39_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC39_OUT_INV_SEL_M  (GPIO_FUNC39_OUT_INV_SEL_V << GPIO_FUNC39_OUT_INV_SEL_S)
#define GPIO_FUNC39_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC39_OUT_INV_SEL_S  9
/** GPIO_FUNC39_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC39_OEN_SEL    (BIT(10))
#define GPIO_FUNC39_OEN_SEL_M  (GPIO_FUNC39_OEN_SEL_V << GPIO_FUNC39_OEN_SEL_S)
#define GPIO_FUNC39_OEN_SEL_V  0x00000001U
#define GPIO_FUNC39_OEN_SEL_S  10
/** GPIO_FUNC39_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC39_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC39_OEN_INV_SEL_M  (GPIO_FUNC39_OEN_INV_SEL_V << GPIO_FUNC39_OEN_INV_SEL_S)
#define GPIO_FUNC39_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC39_OEN_INV_SEL_S  11

/** GPIO_FUNC40_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC40_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5f8)
/** GPIO_FUNC40_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC40_OUT_SEL    0x000001FFU
#define GPIO_FUNC40_OUT_SEL_M  (GPIO_FUNC40_OUT_SEL_V << GPIO_FUNC40_OUT_SEL_S)
#define GPIO_FUNC40_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC40_OUT_SEL_S  0
/** GPIO_FUNC40_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC40_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC40_OUT_INV_SEL_M  (GPIO_FUNC40_OUT_INV_SEL_V << GPIO_FUNC40_OUT_INV_SEL_S)
#define GPIO_FUNC40_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC40_OUT_INV_SEL_S  9
/** GPIO_FUNC40_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC40_OEN_SEL    (BIT(10))
#define GPIO_FUNC40_OEN_SEL_M  (GPIO_FUNC40_OEN_SEL_V << GPIO_FUNC40_OEN_SEL_S)
#define GPIO_FUNC40_OEN_SEL_V  0x00000001U
#define GPIO_FUNC40_OEN_SEL_S  10
/** GPIO_FUNC40_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC40_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC40_OEN_INV_SEL_M  (GPIO_FUNC40_OEN_INV_SEL_V << GPIO_FUNC40_OEN_INV_SEL_S)
#define GPIO_FUNC40_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC40_OEN_INV_SEL_S  11

/** GPIO_FUNC41_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC41_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5fc)
/** GPIO_FUNC41_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC41_OUT_SEL    0x000001FFU
#define GPIO_FUNC41_OUT_SEL_M  (GPIO_FUNC41_OUT_SEL_V << GPIO_FUNC41_OUT_SEL_S)
#define GPIO_FUNC41_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC41_OUT_SEL_S  0
/** GPIO_FUNC41_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC41_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC41_OUT_INV_SEL_M  (GPIO_FUNC41_OUT_INV_SEL_V << GPIO_FUNC41_OUT_INV_SEL_S)
#define GPIO_FUNC41_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_OUT_INV_SEL_S  9
/** GPIO_FUNC41_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC41_OEN_SEL    (BIT(10))
#define GPIO_FUNC41_OEN_SEL_M  (GPIO_FUNC41_OEN_SEL_V << GPIO_FUNC41_OEN_SEL_S)
#define GPIO_FUNC41_OEN_SEL_V  0x00000001U
#define GPIO_FUNC41_OEN_SEL_S  10
/** GPIO_FUNC41_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC41_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC41_OEN_INV_SEL_M  (GPIO_FUNC41_OEN_INV_SEL_V << GPIO_FUNC41_OEN_INV_SEL_S)
#define GPIO_FUNC41_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_OEN_INV_SEL_S  11

/** GPIO_FUNC42_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC42_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x600)
/** GPIO_FUNC42_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC42_OUT_SEL    0x000001FFU
#define GPIO_FUNC42_OUT_SEL_M  (GPIO_FUNC42_OUT_SEL_V << GPIO_FUNC42_OUT_SEL_S)
#define GPIO_FUNC42_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC42_OUT_SEL_S  0
/** GPIO_FUNC42_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC42_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC42_OUT_INV_SEL_M  (GPIO_FUNC42_OUT_INV_SEL_V << GPIO_FUNC42_OUT_INV_SEL_S)
#define GPIO_FUNC42_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_OUT_INV_SEL_S  9
/** GPIO_FUNC42_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC42_OEN_SEL    (BIT(10))
#define GPIO_FUNC42_OEN_SEL_M  (GPIO_FUNC42_OEN_SEL_V << GPIO_FUNC42_OEN_SEL_S)
#define GPIO_FUNC42_OEN_SEL_V  0x00000001U
#define GPIO_FUNC42_OEN_SEL_S  10
/** GPIO_FUNC42_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC42_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC42_OEN_INV_SEL_M  (GPIO_FUNC42_OEN_INV_SEL_V << GPIO_FUNC42_OEN_INV_SEL_S)
#define GPIO_FUNC42_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_OEN_INV_SEL_S  11

/** GPIO_FUNC43_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC43_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x604)
/** GPIO_FUNC43_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC43_OUT_SEL    0x000001FFU
#define GPIO_FUNC43_OUT_SEL_M  (GPIO_FUNC43_OUT_SEL_V << GPIO_FUNC43_OUT_SEL_S)
#define GPIO_FUNC43_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC43_OUT_SEL_S  0
/** GPIO_FUNC43_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC43_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC43_OUT_INV_SEL_M  (GPIO_FUNC43_OUT_INV_SEL_V << GPIO_FUNC43_OUT_INV_SEL_S)
#define GPIO_FUNC43_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_OUT_INV_SEL_S  9
/** GPIO_FUNC43_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC43_OEN_SEL    (BIT(10))
#define GPIO_FUNC43_OEN_SEL_M  (GPIO_FUNC43_OEN_SEL_V << GPIO_FUNC43_OEN_SEL_S)
#define GPIO_FUNC43_OEN_SEL_V  0x00000001U
#define GPIO_FUNC43_OEN_SEL_S  10
/** GPIO_FUNC43_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC43_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC43_OEN_INV_SEL_M  (GPIO_FUNC43_OEN_INV_SEL_V << GPIO_FUNC43_OEN_INV_SEL_S)
#define GPIO_FUNC43_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_OEN_INV_SEL_S  11

/** GPIO_FUNC44_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC44_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x608)
/** GPIO_FUNC44_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC44_OUT_SEL    0x000001FFU
#define GPIO_FUNC44_OUT_SEL_M  (GPIO_FUNC44_OUT_SEL_V << GPIO_FUNC44_OUT_SEL_S)
#define GPIO_FUNC44_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC44_OUT_SEL_S  0
/** GPIO_FUNC44_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC44_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC44_OUT_INV_SEL_M  (GPIO_FUNC44_OUT_INV_SEL_V << GPIO_FUNC44_OUT_INV_SEL_S)
#define GPIO_FUNC44_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC44_OUT_INV_SEL_S  9
/** GPIO_FUNC44_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC44_OEN_SEL    (BIT(10))
#define GPIO_FUNC44_OEN_SEL_M  (GPIO_FUNC44_OEN_SEL_V << GPIO_FUNC44_OEN_SEL_S)
#define GPIO_FUNC44_OEN_SEL_V  0x00000001U
#define GPIO_FUNC44_OEN_SEL_S  10
/** GPIO_FUNC44_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC44_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC44_OEN_INV_SEL_M  (GPIO_FUNC44_OEN_INV_SEL_V << GPIO_FUNC44_OEN_INV_SEL_S)
#define GPIO_FUNC44_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC44_OEN_INV_SEL_S  11

/** GPIO_FUNC45_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC45_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x60c)
/** GPIO_FUNC45_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC45_OUT_SEL    0x000001FFU
#define GPIO_FUNC45_OUT_SEL_M  (GPIO_FUNC45_OUT_SEL_V << GPIO_FUNC45_OUT_SEL_S)
#define GPIO_FUNC45_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC45_OUT_SEL_S  0
/** GPIO_FUNC45_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC45_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC45_OUT_INV_SEL_M  (GPIO_FUNC45_OUT_INV_SEL_V << GPIO_FUNC45_OUT_INV_SEL_S)
#define GPIO_FUNC45_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC45_OUT_INV_SEL_S  9
/** GPIO_FUNC45_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC45_OEN_SEL    (BIT(10))
#define GPIO_FUNC45_OEN_SEL_M  (GPIO_FUNC45_OEN_SEL_V << GPIO_FUNC45_OEN_SEL_S)
#define GPIO_FUNC45_OEN_SEL_V  0x00000001U
#define GPIO_FUNC45_OEN_SEL_S  10
/** GPIO_FUNC45_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC45_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC45_OEN_INV_SEL_M  (GPIO_FUNC45_OEN_INV_SEL_V << GPIO_FUNC45_OEN_INV_SEL_S)
#define GPIO_FUNC45_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC45_OEN_INV_SEL_S  11

/** GPIO_FUNC46_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC46_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x610)
/** GPIO_FUNC46_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC46_OUT_SEL    0x000001FFU
#define GPIO_FUNC46_OUT_SEL_M  (GPIO_FUNC46_OUT_SEL_V << GPIO_FUNC46_OUT_SEL_S)
#define GPIO_FUNC46_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC46_OUT_SEL_S  0
/** GPIO_FUNC46_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC46_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC46_OUT_INV_SEL_M  (GPIO_FUNC46_OUT_INV_SEL_V << GPIO_FUNC46_OUT_INV_SEL_S)
#define GPIO_FUNC46_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_OUT_INV_SEL_S  9
/** GPIO_FUNC46_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC46_OEN_SEL    (BIT(10))
#define GPIO_FUNC46_OEN_SEL_M  (GPIO_FUNC46_OEN_SEL_V << GPIO_FUNC46_OEN_SEL_S)
#define GPIO_FUNC46_OEN_SEL_V  0x00000001U
#define GPIO_FUNC46_OEN_SEL_S  10
/** GPIO_FUNC46_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC46_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC46_OEN_INV_SEL_M  (GPIO_FUNC46_OEN_INV_SEL_V << GPIO_FUNC46_OEN_INV_SEL_S)
#define GPIO_FUNC46_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_OEN_INV_SEL_S  11

/** GPIO_FUNC47_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC47_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x614)
/** GPIO_FUNC47_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC47_OUT_SEL    0x000001FFU
#define GPIO_FUNC47_OUT_SEL_M  (GPIO_FUNC47_OUT_SEL_V << GPIO_FUNC47_OUT_SEL_S)
#define GPIO_FUNC47_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC47_OUT_SEL_S  0
/** GPIO_FUNC47_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC47_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC47_OUT_INV_SEL_M  (GPIO_FUNC47_OUT_INV_SEL_V << GPIO_FUNC47_OUT_INV_SEL_S)
#define GPIO_FUNC47_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_OUT_INV_SEL_S  9
/** GPIO_FUNC47_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC47_OEN_SEL    (BIT(10))
#define GPIO_FUNC47_OEN_SEL_M  (GPIO_FUNC47_OEN_SEL_V << GPIO_FUNC47_OEN_SEL_S)
#define GPIO_FUNC47_OEN_SEL_V  0x00000001U
#define GPIO_FUNC47_OEN_SEL_S  10
/** GPIO_FUNC47_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC47_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC47_OEN_INV_SEL_M  (GPIO_FUNC47_OEN_INV_SEL_V << GPIO_FUNC47_OEN_INV_SEL_S)
#define GPIO_FUNC47_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_OEN_INV_SEL_S  11

/** GPIO_FUNC48_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC48_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x618)
/** GPIO_FUNC48_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC48_OUT_SEL    0x000001FFU
#define GPIO_FUNC48_OUT_SEL_M  (GPIO_FUNC48_OUT_SEL_V << GPIO_FUNC48_OUT_SEL_S)
#define GPIO_FUNC48_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC48_OUT_SEL_S  0
/** GPIO_FUNC48_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC48_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC48_OUT_INV_SEL_M  (GPIO_FUNC48_OUT_INV_SEL_V << GPIO_FUNC48_OUT_INV_SEL_S)
#define GPIO_FUNC48_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_OUT_INV_SEL_S  9
/** GPIO_FUNC48_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC48_OEN_SEL    (BIT(10))
#define GPIO_FUNC48_OEN_SEL_M  (GPIO_FUNC48_OEN_SEL_V << GPIO_FUNC48_OEN_SEL_S)
#define GPIO_FUNC48_OEN_SEL_V  0x00000001U
#define GPIO_FUNC48_OEN_SEL_S  10
/** GPIO_FUNC48_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC48_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC48_OEN_INV_SEL_M  (GPIO_FUNC48_OEN_INV_SEL_V << GPIO_FUNC48_OEN_INV_SEL_S)
#define GPIO_FUNC48_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_OEN_INV_SEL_S  11

/** GPIO_FUNC49_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC49_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x61c)
/** GPIO_FUNC49_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC49_OUT_SEL    0x000001FFU
#define GPIO_FUNC49_OUT_SEL_M  (GPIO_FUNC49_OUT_SEL_V << GPIO_FUNC49_OUT_SEL_S)
#define GPIO_FUNC49_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC49_OUT_SEL_S  0
/** GPIO_FUNC49_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC49_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC49_OUT_INV_SEL_M  (GPIO_FUNC49_OUT_INV_SEL_V << GPIO_FUNC49_OUT_INV_SEL_S)
#define GPIO_FUNC49_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_OUT_INV_SEL_S  9
/** GPIO_FUNC49_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC49_OEN_SEL    (BIT(10))
#define GPIO_FUNC49_OEN_SEL_M  (GPIO_FUNC49_OEN_SEL_V << GPIO_FUNC49_OEN_SEL_S)
#define GPIO_FUNC49_OEN_SEL_V  0x00000001U
#define GPIO_FUNC49_OEN_SEL_S  10
/** GPIO_FUNC49_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC49_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC49_OEN_INV_SEL_M  (GPIO_FUNC49_OEN_INV_SEL_V << GPIO_FUNC49_OEN_INV_SEL_S)
#define GPIO_FUNC49_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_OEN_INV_SEL_S  11

/** GPIO_FUNC50_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC50_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x620)
/** GPIO_FUNC50_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC50_OUT_SEL    0x000001FFU
#define GPIO_FUNC50_OUT_SEL_M  (GPIO_FUNC50_OUT_SEL_V << GPIO_FUNC50_OUT_SEL_S)
#define GPIO_FUNC50_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC50_OUT_SEL_S  0
/** GPIO_FUNC50_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC50_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC50_OUT_INV_SEL_M  (GPIO_FUNC50_OUT_INV_SEL_V << GPIO_FUNC50_OUT_INV_SEL_S)
#define GPIO_FUNC50_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_OUT_INV_SEL_S  9
/** GPIO_FUNC50_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC50_OEN_SEL    (BIT(10))
#define GPIO_FUNC50_OEN_SEL_M  (GPIO_FUNC50_OEN_SEL_V << GPIO_FUNC50_OEN_SEL_S)
#define GPIO_FUNC50_OEN_SEL_V  0x00000001U
#define GPIO_FUNC50_OEN_SEL_S  10
/** GPIO_FUNC50_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC50_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC50_OEN_INV_SEL_M  (GPIO_FUNC50_OEN_INV_SEL_V << GPIO_FUNC50_OEN_INV_SEL_S)
#define GPIO_FUNC50_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_OEN_INV_SEL_S  11

/** GPIO_FUNC51_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC51_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x624)
/** GPIO_FUNC51_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC51_OUT_SEL    0x000001FFU
#define GPIO_FUNC51_OUT_SEL_M  (GPIO_FUNC51_OUT_SEL_V << GPIO_FUNC51_OUT_SEL_S)
#define GPIO_FUNC51_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC51_OUT_SEL_S  0
/** GPIO_FUNC51_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC51_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC51_OUT_INV_SEL_M  (GPIO_FUNC51_OUT_INV_SEL_V << GPIO_FUNC51_OUT_INV_SEL_S)
#define GPIO_FUNC51_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_OUT_INV_SEL_S  9
/** GPIO_FUNC51_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC51_OEN_SEL    (BIT(10))
#define GPIO_FUNC51_OEN_SEL_M  (GPIO_FUNC51_OEN_SEL_V << GPIO_FUNC51_OEN_SEL_S)
#define GPIO_FUNC51_OEN_SEL_V  0x00000001U
#define GPIO_FUNC51_OEN_SEL_S  10
/** GPIO_FUNC51_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC51_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC51_OEN_INV_SEL_M  (GPIO_FUNC51_OEN_INV_SEL_V << GPIO_FUNC51_OEN_INV_SEL_S)
#define GPIO_FUNC51_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_OEN_INV_SEL_S  11

/** GPIO_FUNC52_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC52_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x628)
/** GPIO_FUNC52_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC52_OUT_SEL    0x000001FFU
#define GPIO_FUNC52_OUT_SEL_M  (GPIO_FUNC52_OUT_SEL_V << GPIO_FUNC52_OUT_SEL_S)
#define GPIO_FUNC52_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC52_OUT_SEL_S  0
/** GPIO_FUNC52_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC52_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC52_OUT_INV_SEL_M  (GPIO_FUNC52_OUT_INV_SEL_V << GPIO_FUNC52_OUT_INV_SEL_S)
#define GPIO_FUNC52_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_OUT_INV_SEL_S  9
/** GPIO_FUNC52_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC52_OEN_SEL    (BIT(10))
#define GPIO_FUNC52_OEN_SEL_M  (GPIO_FUNC52_OEN_SEL_V << GPIO_FUNC52_OEN_SEL_S)
#define GPIO_FUNC52_OEN_SEL_V  0x00000001U
#define GPIO_FUNC52_OEN_SEL_S  10
/** GPIO_FUNC52_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC52_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC52_OEN_INV_SEL_M  (GPIO_FUNC52_OEN_INV_SEL_V << GPIO_FUNC52_OEN_INV_SEL_S)
#define GPIO_FUNC52_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_OEN_INV_SEL_S  11

/** GPIO_FUNC53_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC53_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x62c)
/** GPIO_FUNC53_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC53_OUT_SEL    0x000001FFU
#define GPIO_FUNC53_OUT_SEL_M  (GPIO_FUNC53_OUT_SEL_V << GPIO_FUNC53_OUT_SEL_S)
#define GPIO_FUNC53_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC53_OUT_SEL_S  0
/** GPIO_FUNC53_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC53_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC53_OUT_INV_SEL_M  (GPIO_FUNC53_OUT_INV_SEL_V << GPIO_FUNC53_OUT_INV_SEL_S)
#define GPIO_FUNC53_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_OUT_INV_SEL_S  9
/** GPIO_FUNC53_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC53_OEN_SEL    (BIT(10))
#define GPIO_FUNC53_OEN_SEL_M  (GPIO_FUNC53_OEN_SEL_V << GPIO_FUNC53_OEN_SEL_S)
#define GPIO_FUNC53_OEN_SEL_V  0x00000001U
#define GPIO_FUNC53_OEN_SEL_S  10
/** GPIO_FUNC53_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC53_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC53_OEN_INV_SEL_M  (GPIO_FUNC53_OEN_INV_SEL_V << GPIO_FUNC53_OEN_INV_SEL_S)
#define GPIO_FUNC53_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_OEN_INV_SEL_S  11

/** GPIO_FUNC54_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC54_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x630)
/** GPIO_FUNC54_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC54_OUT_SEL    0x000001FFU
#define GPIO_FUNC54_OUT_SEL_M  (GPIO_FUNC54_OUT_SEL_V << GPIO_FUNC54_OUT_SEL_S)
#define GPIO_FUNC54_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC54_OUT_SEL_S  0
/** GPIO_FUNC54_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC54_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC54_OUT_INV_SEL_M  (GPIO_FUNC54_OUT_INV_SEL_V << GPIO_FUNC54_OUT_INV_SEL_S)
#define GPIO_FUNC54_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_OUT_INV_SEL_S  9
/** GPIO_FUNC54_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC54_OEN_SEL    (BIT(10))
#define GPIO_FUNC54_OEN_SEL_M  (GPIO_FUNC54_OEN_SEL_V << GPIO_FUNC54_OEN_SEL_S)
#define GPIO_FUNC54_OEN_SEL_V  0x00000001U
#define GPIO_FUNC54_OEN_SEL_S  10
/** GPIO_FUNC54_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC54_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC54_OEN_INV_SEL_M  (GPIO_FUNC54_OEN_INV_SEL_V << GPIO_FUNC54_OEN_INV_SEL_S)
#define GPIO_FUNC54_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_OEN_INV_SEL_S  11

/** GPIO_FUNC55_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC55_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x634)
/** GPIO_FUNC55_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC55_OUT_SEL    0x000001FFU
#define GPIO_FUNC55_OUT_SEL_M  (GPIO_FUNC55_OUT_SEL_V << GPIO_FUNC55_OUT_SEL_S)
#define GPIO_FUNC55_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC55_OUT_SEL_S  0
/** GPIO_FUNC55_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC55_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC55_OUT_INV_SEL_M  (GPIO_FUNC55_OUT_INV_SEL_V << GPIO_FUNC55_OUT_INV_SEL_S)
#define GPIO_FUNC55_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_OUT_INV_SEL_S  9
/** GPIO_FUNC55_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC55_OEN_SEL    (BIT(10))
#define GPIO_FUNC55_OEN_SEL_M  (GPIO_FUNC55_OEN_SEL_V << GPIO_FUNC55_OEN_SEL_S)
#define GPIO_FUNC55_OEN_SEL_V  0x00000001U
#define GPIO_FUNC55_OEN_SEL_S  10
/** GPIO_FUNC55_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC55_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC55_OEN_INV_SEL_M  (GPIO_FUNC55_OEN_INV_SEL_V << GPIO_FUNC55_OEN_INV_SEL_S)
#define GPIO_FUNC55_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_OEN_INV_SEL_S  11

/** GPIO_FUNC56_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC56_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x638)
/** GPIO_FUNC56_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
 *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC56_OUT_SEL    0x000001FFU
#define GPIO_FUNC56_OUT_SEL_M  (GPIO_FUNC56_OUT_SEL_V << GPIO_FUNC56_OUT_SEL_S)
#define GPIO_FUNC56_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC56_OUT_SEL_S  0
/** GPIO_FUNC56_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC56_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC56_OUT_INV_SEL_M  (GPIO_FUNC56_OUT_INV_SEL_V << GPIO_FUNC56_OUT_INV_SEL_S)
#define GPIO_FUNC56_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_OUT_INV_SEL_S  9
/** GPIO_FUNC56_OEN_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC56_OEN_SEL    (BIT(10))
#define GPIO_FUNC56_OEN_SEL_M  (GPIO_FUNC56_OEN_SEL_V << GPIO_FUNC56_OEN_SEL_S)
#define GPIO_FUNC56_OEN_SEL_V  0x00000001U
#define GPIO_FUNC56_OEN_SEL_S  10
/** GPIO_FUNC56_OEN_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC56_OEN_INV_SEL    (BIT(11))
#define GPIO_FUNC56_OEN_INV_SEL_M  (GPIO_FUNC56_OEN_INV_SEL_V << GPIO_FUNC56_OEN_INV_SEL_S)
#define GPIO_FUNC56_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_OEN_INV_SEL_S  11

/** GPIO_INTR_2_REG register
 *  GPIO interrupt 2 status register for GPIO0-31
 */
#define GPIO_INTR_2_REG (DR_REG_GPIO_BASE + 0x63c)
/** GPIO_INT_2 : RO; bitpos: [31:0]; default: 0;
 *  GPIO interrupt 2 status register for GPIO0-31
 */
#define GPIO_INT_2    0xFFFFFFFFU
#define GPIO_INT_2_M  (GPIO_INT_2_V << GPIO_INT_2_S)
#define GPIO_INT_2_V  0xFFFFFFFFU
#define GPIO_INT_2_S  0

/** GPIO_INTR1_2_REG register
 *  GPIO interrupt 2 status register for GPIO32-56
 */
#define GPIO_INTR1_2_REG (DR_REG_GPIO_BASE + 0x640)
/** GPIO_INT1_2 : RO; bitpos: [24:0]; default: 0;
 *  GPIO interrupt 2 status register for GPIO32-56
 */
#define GPIO_INT1_2    0x01FFFFFFU
#define GPIO_INT1_2_M  (GPIO_INT1_2_V << GPIO_INT1_2_S)
#define GPIO_INT1_2_V  0x01FFFFFFU
#define GPIO_INT1_2_S  0

/** GPIO_INTR_3_REG register
 *  GPIO interrupt 3 status register for GPIO0-31
 */
#define GPIO_INTR_3_REG (DR_REG_GPIO_BASE + 0x644)
/** GPIO_INT_3 : RO; bitpos: [31:0]; default: 0;
 *  GPIO interrupt 3 status register for GPIO0-31
 */
#define GPIO_INT_3    0xFFFFFFFFU
#define GPIO_INT_3_M  (GPIO_INT_3_V << GPIO_INT_3_S)
#define GPIO_INT_3_V  0xFFFFFFFFU
#define GPIO_INT_3_S  0

/** GPIO_INTR1_3_REG register
 *  GPIO interrupt 3 status register for GPIO32-56
 */
#define GPIO_INTR1_3_REG (DR_REG_GPIO_BASE + 0x648)
/** GPIO_INT1_3 : RO; bitpos: [24:0]; default: 0;
 *  GPIO interrupt 3 status register for GPIO32-56
 */
#define GPIO_INT1_3    0x01FFFFFFU
#define GPIO_INT1_3_M  (GPIO_INT1_3_V << GPIO_INT1_3_S)
#define GPIO_INT1_3_V  0x01FFFFFFU
#define GPIO_INT1_3_S  0

/** GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define GPIO_CLOCK_GATE_REG (DR_REG_GPIO_BASE + 0x64c)
/** GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  set this bit to enable GPIO clock gate
 */
#define GPIO_CLK_EN    (BIT(0))
#define GPIO_CLK_EN_M  (GPIO_CLK_EN_V << GPIO_CLK_EN_S)
#define GPIO_CLK_EN_V  0x00000001U
#define GPIO_CLK_EN_S  0

/** GPIO_INT_RAW_REG register
 *  analog comparator interrupt raw
 */
#define GPIO_INT_RAW_REG (DR_REG_GPIO_BASE + 0x700)
/** GPIO_COMP0_NEG_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  analog comparator pos edge interrupt raw
 */
#define GPIO_COMP0_NEG_INT_RAW    (BIT(0))
#define GPIO_COMP0_NEG_INT_RAW_M  (GPIO_COMP0_NEG_INT_RAW_V << GPIO_COMP0_NEG_INT_RAW_S)
#define GPIO_COMP0_NEG_INT_RAW_V  0x00000001U
#define GPIO_COMP0_NEG_INT_RAW_S  0
/** GPIO_COMP0_POS_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  analog comparator neg edge interrupt raw
 */
#define GPIO_COMP0_POS_INT_RAW    (BIT(1))
#define GPIO_COMP0_POS_INT_RAW_M  (GPIO_COMP0_POS_INT_RAW_V << GPIO_COMP0_POS_INT_RAW_S)
#define GPIO_COMP0_POS_INT_RAW_V  0x00000001U
#define GPIO_COMP0_POS_INT_RAW_S  1
/** GPIO_COMP0_ALL_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  analog comparator neg or pos edge interrupt raw
 */
#define GPIO_COMP0_ALL_INT_RAW    (BIT(2))
#define GPIO_COMP0_ALL_INT_RAW_M  (GPIO_COMP0_ALL_INT_RAW_V << GPIO_COMP0_ALL_INT_RAW_S)
#define GPIO_COMP0_ALL_INT_RAW_V  0x00000001U
#define GPIO_COMP0_ALL_INT_RAW_S  2
/** GPIO_COMP1_NEG_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  analog comparator pos edge interrupt raw
 */
#define GPIO_COMP1_NEG_INT_RAW    (BIT(3))
#define GPIO_COMP1_NEG_INT_RAW_M  (GPIO_COMP1_NEG_INT_RAW_V << GPIO_COMP1_NEG_INT_RAW_S)
#define GPIO_COMP1_NEG_INT_RAW_V  0x00000001U
#define GPIO_COMP1_NEG_INT_RAW_S  3
/** GPIO_COMP1_POS_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  analog comparator neg edge interrupt raw
 */
#define GPIO_COMP1_POS_INT_RAW    (BIT(4))
#define GPIO_COMP1_POS_INT_RAW_M  (GPIO_COMP1_POS_INT_RAW_V << GPIO_COMP1_POS_INT_RAW_S)
#define GPIO_COMP1_POS_INT_RAW_V  0x00000001U
#define GPIO_COMP1_POS_INT_RAW_S  4
/** GPIO_COMP1_ALL_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  analog comparator neg or pos edge interrupt raw
 */
#define GPIO_COMP1_ALL_INT_RAW    (BIT(5))
#define GPIO_COMP1_ALL_INT_RAW_M  (GPIO_COMP1_ALL_INT_RAW_V << GPIO_COMP1_ALL_INT_RAW_S)
#define GPIO_COMP1_ALL_INT_RAW_V  0x00000001U
#define GPIO_COMP1_ALL_INT_RAW_S  5
/** GPIO_BISTOK_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  pad bistok interrupt raw
 */
#define GPIO_BISTOK_INT_RAW    (BIT(6))
#define GPIO_BISTOK_INT_RAW_M  (GPIO_BISTOK_INT_RAW_V << GPIO_BISTOK_INT_RAW_S)
#define GPIO_BISTOK_INT_RAW_V  0x00000001U
#define GPIO_BISTOK_INT_RAW_S  6
/** GPIO_BISTFAIL_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  pad bistfail interrupt raw
 */
#define GPIO_BISTFAIL_INT_RAW    (BIT(7))
#define GPIO_BISTFAIL_INT_RAW_M  (GPIO_BISTFAIL_INT_RAW_V << GPIO_BISTFAIL_INT_RAW_S)
#define GPIO_BISTFAIL_INT_RAW_V  0x00000001U
#define GPIO_BISTFAIL_INT_RAW_S  7

/** GPIO_INT_ST_REG register
 *  analog comparator interrupt status
 */
#define GPIO_INT_ST_REG (DR_REG_GPIO_BASE + 0x704)
/** GPIO_COMP0_NEG_INT_ST : RO; bitpos: [0]; default: 0;
 *  analog comparator pos edge interrupt status
 */
#define GPIO_COMP0_NEG_INT_ST    (BIT(0))
#define GPIO_COMP0_NEG_INT_ST_M  (GPIO_COMP0_NEG_INT_ST_V << GPIO_COMP0_NEG_INT_ST_S)
#define GPIO_COMP0_NEG_INT_ST_V  0x00000001U
#define GPIO_COMP0_NEG_INT_ST_S  0
/** GPIO_COMP0_POS_INT_ST : RO; bitpos: [1]; default: 0;
 *  analog comparator neg edge interrupt status
 */
#define GPIO_COMP0_POS_INT_ST    (BIT(1))
#define GPIO_COMP0_POS_INT_ST_M  (GPIO_COMP0_POS_INT_ST_V << GPIO_COMP0_POS_INT_ST_S)
#define GPIO_COMP0_POS_INT_ST_V  0x00000001U
#define GPIO_COMP0_POS_INT_ST_S  1
/** GPIO_COMP0_ALL_INT_ST : RO; bitpos: [2]; default: 0;
 *  analog comparator neg or pos edge interrupt status
 */
#define GPIO_COMP0_ALL_INT_ST    (BIT(2))
#define GPIO_COMP0_ALL_INT_ST_M  (GPIO_COMP0_ALL_INT_ST_V << GPIO_COMP0_ALL_INT_ST_S)
#define GPIO_COMP0_ALL_INT_ST_V  0x00000001U
#define GPIO_COMP0_ALL_INT_ST_S  2
/** GPIO_COMP1_NEG_INT_ST : RO; bitpos: [3]; default: 0;
 *  analog comparator pos edge interrupt status
 */
#define GPIO_COMP1_NEG_INT_ST    (BIT(3))
#define GPIO_COMP1_NEG_INT_ST_M  (GPIO_COMP1_NEG_INT_ST_V << GPIO_COMP1_NEG_INT_ST_S)
#define GPIO_COMP1_NEG_INT_ST_V  0x00000001U
#define GPIO_COMP1_NEG_INT_ST_S  3
/** GPIO_COMP1_POS_INT_ST : RO; bitpos: [4]; default: 0;
 *  analog comparator neg edge interrupt status
 */
#define GPIO_COMP1_POS_INT_ST    (BIT(4))
#define GPIO_COMP1_POS_INT_ST_M  (GPIO_COMP1_POS_INT_ST_V << GPIO_COMP1_POS_INT_ST_S)
#define GPIO_COMP1_POS_INT_ST_V  0x00000001U
#define GPIO_COMP1_POS_INT_ST_S  4
/** GPIO_COMP1_ALL_INT_ST : RO; bitpos: [5]; default: 0;
 *  analog comparator neg or pos edge interrupt status
 */
#define GPIO_COMP1_ALL_INT_ST    (BIT(5))
#define GPIO_COMP1_ALL_INT_ST_M  (GPIO_COMP1_ALL_INT_ST_V << GPIO_COMP1_ALL_INT_ST_S)
#define GPIO_COMP1_ALL_INT_ST_V  0x00000001U
#define GPIO_COMP1_ALL_INT_ST_S  5
/** GPIO_BISTOK_INT_ST : RO; bitpos: [6]; default: 0;
 *  pad bistok interrupt status
 */
#define GPIO_BISTOK_INT_ST    (BIT(6))
#define GPIO_BISTOK_INT_ST_M  (GPIO_BISTOK_INT_ST_V << GPIO_BISTOK_INT_ST_S)
#define GPIO_BISTOK_INT_ST_V  0x00000001U
#define GPIO_BISTOK_INT_ST_S  6
/** GPIO_BISTFAIL_INT_ST : RO; bitpos: [7]; default: 0;
 *  pad bistfail interrupt status
 */
#define GPIO_BISTFAIL_INT_ST    (BIT(7))
#define GPIO_BISTFAIL_INT_ST_M  (GPIO_BISTFAIL_INT_ST_V << GPIO_BISTFAIL_INT_ST_S)
#define GPIO_BISTFAIL_INT_ST_V  0x00000001U
#define GPIO_BISTFAIL_INT_ST_S  7

/** GPIO_INT_ENA_REG register
 *  analog comparator interrupt enable
 */
#define GPIO_INT_ENA_REG (DR_REG_GPIO_BASE + 0x708)
/** GPIO_COMP0_NEG_INT_ENA : R/W; bitpos: [0]; default: 1;
 *  analog comparator pos edge interrupt enable
 */
#define GPIO_COMP0_NEG_INT_ENA    (BIT(0))
#define GPIO_COMP0_NEG_INT_ENA_M  (GPIO_COMP0_NEG_INT_ENA_V << GPIO_COMP0_NEG_INT_ENA_S)
#define GPIO_COMP0_NEG_INT_ENA_V  0x00000001U
#define GPIO_COMP0_NEG_INT_ENA_S  0
/** GPIO_COMP0_POS_INT_ENA : R/W; bitpos: [1]; default: 1;
 *  analog comparator neg edge interrupt enable
 */
#define GPIO_COMP0_POS_INT_ENA    (BIT(1))
#define GPIO_COMP0_POS_INT_ENA_M  (GPIO_COMP0_POS_INT_ENA_V << GPIO_COMP0_POS_INT_ENA_S)
#define GPIO_COMP0_POS_INT_ENA_V  0x00000001U
#define GPIO_COMP0_POS_INT_ENA_S  1
/** GPIO_COMP0_ALL_INT_ENA : R/W; bitpos: [2]; default: 1;
 *  analog comparator neg or pos edge interrupt enable
 */
#define GPIO_COMP0_ALL_INT_ENA    (BIT(2))
#define GPIO_COMP0_ALL_INT_ENA_M  (GPIO_COMP0_ALL_INT_ENA_V << GPIO_COMP0_ALL_INT_ENA_S)
#define GPIO_COMP0_ALL_INT_ENA_V  0x00000001U
#define GPIO_COMP0_ALL_INT_ENA_S  2
/** GPIO_COMP1_NEG_INT_ENA : R/W; bitpos: [3]; default: 1;
 *  analog comparator pos edge interrupt enable
 */
#define GPIO_COMP1_NEG_INT_ENA    (BIT(3))
#define GPIO_COMP1_NEG_INT_ENA_M  (GPIO_COMP1_NEG_INT_ENA_V << GPIO_COMP1_NEG_INT_ENA_S)
#define GPIO_COMP1_NEG_INT_ENA_V  0x00000001U
#define GPIO_COMP1_NEG_INT_ENA_S  3
/** GPIO_COMP1_POS_INT_ENA : R/W; bitpos: [4]; default: 1;
 *  analog comparator neg edge interrupt enable
 */
#define GPIO_COMP1_POS_INT_ENA    (BIT(4))
#define GPIO_COMP1_POS_INT_ENA_M  (GPIO_COMP1_POS_INT_ENA_V << GPIO_COMP1_POS_INT_ENA_S)
#define GPIO_COMP1_POS_INT_ENA_V  0x00000001U
#define GPIO_COMP1_POS_INT_ENA_S  4
/** GPIO_COMP1_ALL_INT_ENA : R/W; bitpos: [5]; default: 1;
 *  analog comparator neg or pos edge interrupt enable
 */
#define GPIO_COMP1_ALL_INT_ENA    (BIT(5))
#define GPIO_COMP1_ALL_INT_ENA_M  (GPIO_COMP1_ALL_INT_ENA_V << GPIO_COMP1_ALL_INT_ENA_S)
#define GPIO_COMP1_ALL_INT_ENA_V  0x00000001U
#define GPIO_COMP1_ALL_INT_ENA_S  5
/** GPIO_BISTOK_INT_ENA : R/W; bitpos: [6]; default: 1;
 *  pad bistok interrupt enable
 */
#define GPIO_BISTOK_INT_ENA    (BIT(6))
#define GPIO_BISTOK_INT_ENA_M  (GPIO_BISTOK_INT_ENA_V << GPIO_BISTOK_INT_ENA_S)
#define GPIO_BISTOK_INT_ENA_V  0x00000001U
#define GPIO_BISTOK_INT_ENA_S  6
/** GPIO_BISTFAIL_INT_ENA : R/W; bitpos: [7]; default: 1;
 *  pad bistfail interrupt enable
 */
#define GPIO_BISTFAIL_INT_ENA    (BIT(7))
#define GPIO_BISTFAIL_INT_ENA_M  (GPIO_BISTFAIL_INT_ENA_V << GPIO_BISTFAIL_INT_ENA_S)
#define GPIO_BISTFAIL_INT_ENA_V  0x00000001U
#define GPIO_BISTFAIL_INT_ENA_S  7

/** GPIO_INT_CLR_REG register
 *  analog comparator interrupt clear
 */
#define GPIO_INT_CLR_REG (DR_REG_GPIO_BASE + 0x70c)
/** GPIO_COMP0_NEG_INT_CLR : WT; bitpos: [0]; default: 0;
 *  analog comparator pos edge interrupt clear
 */
#define GPIO_COMP0_NEG_INT_CLR    (BIT(0))
#define GPIO_COMP0_NEG_INT_CLR_M  (GPIO_COMP0_NEG_INT_CLR_V << GPIO_COMP0_NEG_INT_CLR_S)
#define GPIO_COMP0_NEG_INT_CLR_V  0x00000001U
#define GPIO_COMP0_NEG_INT_CLR_S  0
/** GPIO_COMP0_POS_INT_CLR : WT; bitpos: [1]; default: 0;
 *  analog comparator neg edge interrupt clear
 */
#define GPIO_COMP0_POS_INT_CLR    (BIT(1))
#define GPIO_COMP0_POS_INT_CLR_M  (GPIO_COMP0_POS_INT_CLR_V << GPIO_COMP0_POS_INT_CLR_S)
#define GPIO_COMP0_POS_INT_CLR_V  0x00000001U
#define GPIO_COMP0_POS_INT_CLR_S  1
/** GPIO_COMP0_ALL_INT_CLR : WT; bitpos: [2]; default: 0;
 *  analog comparator neg or pos edge interrupt clear
 */
#define GPIO_COMP0_ALL_INT_CLR    (BIT(2))
#define GPIO_COMP0_ALL_INT_CLR_M  (GPIO_COMP0_ALL_INT_CLR_V << GPIO_COMP0_ALL_INT_CLR_S)
#define GPIO_COMP0_ALL_INT_CLR_V  0x00000001U
#define GPIO_COMP0_ALL_INT_CLR_S  2
/** GPIO_COMP1_NEG_INT_CLR : WT; bitpos: [3]; default: 0;
 *  analog comparator pos edge interrupt clear
 */
#define GPIO_COMP1_NEG_INT_CLR    (BIT(3))
#define GPIO_COMP1_NEG_INT_CLR_M  (GPIO_COMP1_NEG_INT_CLR_V << GPIO_COMP1_NEG_INT_CLR_S)
#define GPIO_COMP1_NEG_INT_CLR_V  0x00000001U
#define GPIO_COMP1_NEG_INT_CLR_S  3
/** GPIO_COMP1_POS_INT_CLR : WT; bitpos: [4]; default: 0;
 *  analog comparator neg edge interrupt clear
 */
#define GPIO_COMP1_POS_INT_CLR    (BIT(4))
#define GPIO_COMP1_POS_INT_CLR_M  (GPIO_COMP1_POS_INT_CLR_V << GPIO_COMP1_POS_INT_CLR_S)
#define GPIO_COMP1_POS_INT_CLR_V  0x00000001U
#define GPIO_COMP1_POS_INT_CLR_S  4
/** GPIO_COMP1_ALL_INT_CLR : WT; bitpos: [5]; default: 0;
 *  analog comparator neg or pos edge interrupt clear
 */
#define GPIO_COMP1_ALL_INT_CLR    (BIT(5))
#define GPIO_COMP1_ALL_INT_CLR_M  (GPIO_COMP1_ALL_INT_CLR_V << GPIO_COMP1_ALL_INT_CLR_S)
#define GPIO_COMP1_ALL_INT_CLR_V  0x00000001U
#define GPIO_COMP1_ALL_INT_CLR_S  5
/** GPIO_BISTOK_INT_CLR : WT; bitpos: [6]; default: 0;
 *  pad bistok interrupt enable
 */
#define GPIO_BISTOK_INT_CLR    (BIT(6))
#define GPIO_BISTOK_INT_CLR_M  (GPIO_BISTOK_INT_CLR_V << GPIO_BISTOK_INT_CLR_S)
#define GPIO_BISTOK_INT_CLR_V  0x00000001U
#define GPIO_BISTOK_INT_CLR_S  6
/** GPIO_BISTFAIL_INT_CLR : WT; bitpos: [7]; default: 0;
 *  pad bistfail interrupt enable
 */
#define GPIO_BISTFAIL_INT_CLR    (BIT(7))
#define GPIO_BISTFAIL_INT_CLR_M  (GPIO_BISTFAIL_INT_CLR_V << GPIO_BISTFAIL_INT_CLR_S)
#define GPIO_BISTFAIL_INT_CLR_V  0x00000001U
#define GPIO_BISTFAIL_INT_CLR_S  7

/** GPIO_ZERO_DET0_FILTER_CNT_REG register
 *  GPIO analog comparator zero detect filter count
 */
#define GPIO_ZERO_DET0_FILTER_CNT_REG (DR_REG_GPIO_BASE + 0x710)
/** GPIO_ZERO_DET0_FILTER_CNT : R/W; bitpos: [31:0]; default: 4294967295;
 *  GPIO analog comparator zero detect filter count
 */
#define GPIO_ZERO_DET0_FILTER_CNT    0xFFFFFFFFU
#define GPIO_ZERO_DET0_FILTER_CNT_M  (GPIO_ZERO_DET0_FILTER_CNT_V << GPIO_ZERO_DET0_FILTER_CNT_S)
#define GPIO_ZERO_DET0_FILTER_CNT_V  0xFFFFFFFFU
#define GPIO_ZERO_DET0_FILTER_CNT_S  0

/** GPIO_ZERO_DET1_FILTER_CNT_REG register
 *  GPIO analog comparator zero detect filter count
 */
#define GPIO_ZERO_DET1_FILTER_CNT_REG (DR_REG_GPIO_BASE + 0x714)
/** GPIO_ZERO_DET1_FILTER_CNT : R/W; bitpos: [31:0]; default: 4294967295;
 *  GPIO analog comparator zero detect filter count
 */
#define GPIO_ZERO_DET1_FILTER_CNT    0xFFFFFFFFU
#define GPIO_ZERO_DET1_FILTER_CNT_M  (GPIO_ZERO_DET1_FILTER_CNT_V << GPIO_ZERO_DET1_FILTER_CNT_S)
#define GPIO_ZERO_DET1_FILTER_CNT_V  0xFFFFFFFFU
#define GPIO_ZERO_DET1_FILTER_CNT_S  0

/** GPIO_SEND_SEQ_REG register
 *  High speed sdio pad bist send sequence
 */
#define GPIO_SEND_SEQ_REG (DR_REG_GPIO_BASE + 0x718)
/** GPIO_SEND_SEQ : R/W; bitpos: [31:0]; default: 305419896;
 *  High speed sdio pad bist send sequence
 */
#define GPIO_SEND_SEQ    0xFFFFFFFFU
#define GPIO_SEND_SEQ_M  (GPIO_SEND_SEQ_V << GPIO_SEND_SEQ_S)
#define GPIO_SEND_SEQ_V  0xFFFFFFFFU
#define GPIO_SEND_SEQ_S  0

/** GPIO_RECIVE_SEQ_REG register
 *  High speed sdio pad bist recive sequence
 */
#define GPIO_RECIVE_SEQ_REG (DR_REG_GPIO_BASE + 0x71c)
/** GPIO_RECIVE_SEQ : RO; bitpos: [31:0]; default: 0;
 *  High speed sdio pad bist recive sequence
 */
#define GPIO_RECIVE_SEQ    0xFFFFFFFFU
#define GPIO_RECIVE_SEQ_M  (GPIO_RECIVE_SEQ_V << GPIO_RECIVE_SEQ_S)
#define GPIO_RECIVE_SEQ_V  0xFFFFFFFFU
#define GPIO_RECIVE_SEQ_S  0

/** GPIO_BISTIN_SEL_REG register
 *  High speed sdio pad bist in pad sel
 */
#define GPIO_BISTIN_SEL_REG (DR_REG_GPIO_BASE + 0x720)
/** GPIO_BISTIN_SEL : R/W; bitpos: [3:0]; default: 15;
 *  High speed sdio pad bist in pad sel 0:pad39, 1: pad40...
 */
#define GPIO_BISTIN_SEL    0x0000000FU
#define GPIO_BISTIN_SEL_M  (GPIO_BISTIN_SEL_V << GPIO_BISTIN_SEL_S)
#define GPIO_BISTIN_SEL_V  0x0000000FU
#define GPIO_BISTIN_SEL_S  0

/** GPIO_BIST_CTRL_REG register
 *  High speed sdio pad bist control
 */
#define GPIO_BIST_CTRL_REG (DR_REG_GPIO_BASE + 0x724)
/** GPIO_BIST_PAD_OE : R/W; bitpos: [0]; default: 1;
 *  High speed sdio pad bist out pad oe
 */
#define GPIO_BIST_PAD_OE    (BIT(0))
#define GPIO_BIST_PAD_OE_M  (GPIO_BIST_PAD_OE_V << GPIO_BIST_PAD_OE_S)
#define GPIO_BIST_PAD_OE_V  0x00000001U
#define GPIO_BIST_PAD_OE_S  0
/** GPIO_BIST_START : WT; bitpos: [1]; default: 0;
 *  High speed sdio pad bist start
 */
#define GPIO_BIST_START    (BIT(1))
#define GPIO_BIST_START_M  (GPIO_BIST_START_V << GPIO_BIST_START_S)
#define GPIO_BIST_START_V  0x00000001U
#define GPIO_BIST_START_S  1

/** GPIO_DATE_REG register
 *  GPIO version register
 */
#define GPIO_DATE_REG (DR_REG_GPIO_BASE + 0x7fc)
/** GPIO_DATE : R/W; bitpos: [27:0]; default: 2294787;
 *  version register
 */
#define GPIO_DATE    0x0FFFFFFFU
#define GPIO_DATE_M  (GPIO_DATE_V << GPIO_DATE_S)
#define GPIO_DATE_V  0x0FFFFFFFU
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
