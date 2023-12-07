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
 *  GPIO output register for GPIO0-30
 */
#define GPIO_OUT_REG (DR_REG_GPIO_BASE + 0x4)
/** GPIO_OUT_DATA_ORIG : R/W/SC/WTC; bitpos: [30:0]; default: 0;
 *  GPIO output register for GPIO0-30
 */
#define GPIO_OUT_DATA_ORIG    0x7FFFFFFFU
#define GPIO_OUT_DATA_ORIG_M  (GPIO_OUT_DATA_ORIG_V << GPIO_OUT_DATA_ORIG_S)
#define GPIO_OUT_DATA_ORIG_V  0x7FFFFFFFU
#define GPIO_OUT_DATA_ORIG_S  0

/** GPIO_OUT_W1TS_REG register
 *  GPIO output set register for GPIO0-30
 */
#define GPIO_OUT_W1TS_REG (DR_REG_GPIO_BASE + 0x8)
/** GPIO_OUT_W1TS : WT; bitpos: [30:0]; default: 0;
 *  GPIO output set register for GPIO0-30
 */
#define GPIO_OUT_W1TS    0x7FFFFFFFU
#define GPIO_OUT_W1TS_M  (GPIO_OUT_W1TS_V << GPIO_OUT_W1TS_S)
#define GPIO_OUT_W1TS_V  0x7FFFFFFFU
#define GPIO_OUT_W1TS_S  0

/** GPIO_OUT_W1TC_REG register
 *  GPIO output clear register for GPIO0-30
 */
#define GPIO_OUT_W1TC_REG (DR_REG_GPIO_BASE + 0xc)
/** GPIO_OUT_W1TC : WT; bitpos: [30:0]; default: 0;
 *  GPIO output clear register for GPIO0-30
 */
#define GPIO_OUT_W1TC    0x7FFFFFFFU
#define GPIO_OUT_W1TC_M  (GPIO_OUT_W1TC_V << GPIO_OUT_W1TC_S)
#define GPIO_OUT_W1TC_V  0x7FFFFFFFU
#define GPIO_OUT_W1TC_S  0

/** GPIO_SDIO_SELECT_REG register
 *  GPIO sdio select register
 */
#define GPIO_SDIO_SELECT_REG (DR_REG_GPIO_BASE + 0x1c)
/** GPIO_SDIO_SEL : R/W; bitpos: [7:0]; default: 0;
 *  GPIO sdio select register
 */
#define GPIO_SDIO_SEL    0x000000FFU
#define GPIO_SDIO_SEL_M  (GPIO_SDIO_SEL_V << GPIO_SDIO_SEL_S)
#define GPIO_SDIO_SEL_V  0x000000FFU
#define GPIO_SDIO_SEL_S  0

/** GPIO_ENABLE_REG register
 *  GPIO output enable register for GPIO0-30
 */
#define GPIO_ENABLE_REG (DR_REG_GPIO_BASE + 0x20)
/** GPIO_ENABLE_DATA : R/W/WTC; bitpos: [30:0]; default: 0;
 *  GPIO output enable register for GPIO0-30
 */
#define GPIO_ENABLE_DATA    0x7FFFFFFFU
#define GPIO_ENABLE_DATA_M  (GPIO_ENABLE_DATA_V << GPIO_ENABLE_DATA_S)
#define GPIO_ENABLE_DATA_V  0x7FFFFFFFU
#define GPIO_ENABLE_DATA_S  0

/** GPIO_ENABLE_W1TS_REG register
 *  GPIO output enable set register for GPIO0-30
 */
#define GPIO_ENABLE_W1TS_REG (DR_REG_GPIO_BASE + 0x24)
/** GPIO_ENABLE_W1TS : WT; bitpos: [30:0]; default: 0;
 *  GPIO output enable set register for GPIO0-30
 */
#define GPIO_ENABLE_W1TS    0x7FFFFFFFU
#define GPIO_ENABLE_W1TS_M  (GPIO_ENABLE_W1TS_V << GPIO_ENABLE_W1TS_S)
#define GPIO_ENABLE_W1TS_V  0x7FFFFFFFU
#define GPIO_ENABLE_W1TS_S  0

/** GPIO_ENABLE_W1TC_REG register
 *  GPIO output enable clear register for GPIO0-30
 */
#define GPIO_ENABLE_W1TC_REG (DR_REG_GPIO_BASE + 0x28)
/** GPIO_ENABLE_W1TC : WT; bitpos: [30:0]; default: 0;
 *  GPIO output enable clear register for GPIO0-30
 */
#define GPIO_ENABLE_W1TC    0x7FFFFFFFU
#define GPIO_ENABLE_W1TC_M  (GPIO_ENABLE_W1TC_V << GPIO_ENABLE_W1TC_S)
#define GPIO_ENABLE_W1TC_V  0x7FFFFFFFU
#define GPIO_ENABLE_W1TC_S  0

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
 *  GPIO input register for GPIO0-30
 */
#define GPIO_IN_REG (DR_REG_GPIO_BASE + 0x3c)
/** GPIO_IN_DATA_NEXT : RO; bitpos: [30:0]; default: 0;
 *  GPIO input register for GPIO0-30
 */
#define GPIO_IN_DATA_NEXT    0x7FFFFFFFU
#define GPIO_IN_DATA_NEXT_M  (GPIO_IN_DATA_NEXT_V << GPIO_IN_DATA_NEXT_S)
#define GPIO_IN_DATA_NEXT_V  0x7FFFFFFFU
#define GPIO_IN_DATA_NEXT_S  0

/** GPIO_STATUS_REG register
 *  GPIO interrupt status register for GPIO0-30
 */
#define GPIO_STATUS_REG (DR_REG_GPIO_BASE + 0x44)
/** GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [30:0]; default: 0;
 *  GPIO interrupt status register for GPIO0-30
 */
#define GPIO_STATUS_INTERRUPT    0x7FFFFFFFU
#define GPIO_STATUS_INTERRUPT_M  (GPIO_STATUS_INTERRUPT_V << GPIO_STATUS_INTERRUPT_S)
#define GPIO_STATUS_INTERRUPT_V  0x7FFFFFFFU
#define GPIO_STATUS_INTERRUPT_S  0

/** GPIO_STATUS_W1TS_REG register
 *  GPIO interrupt status set register for GPIO0-30
 */
#define GPIO_STATUS_W1TS_REG (DR_REG_GPIO_BASE + 0x48)
/** GPIO_STATUS_W1TS : WT; bitpos: [30:0]; default: 0;
 *  GPIO interrupt status set register for GPIO0-30
 */
#define GPIO_STATUS_W1TS    0x7FFFFFFFU
#define GPIO_STATUS_W1TS_M  (GPIO_STATUS_W1TS_V << GPIO_STATUS_W1TS_S)
#define GPIO_STATUS_W1TS_V  0x7FFFFFFFU
#define GPIO_STATUS_W1TS_S  0

/** GPIO_STATUS_W1TC_REG register
 *  GPIO interrupt status clear register for GPIO0-30
 */
#define GPIO_STATUS_W1TC_REG (DR_REG_GPIO_BASE + 0x4c)
/** GPIO_STATUS_W1TC : WT; bitpos: [30:0]; default: 0;
 *  GPIO interrupt status clear register for GPIO0-30
 */
#define GPIO_STATUS_W1TC    0x7FFFFFFFU
#define GPIO_STATUS_W1TC_M  (GPIO_STATUS_W1TC_V << GPIO_STATUS_W1TC_S)
#define GPIO_STATUS_W1TC_V  0x7FFFFFFFU
#define GPIO_STATUS_W1TC_S  0

/** GPIO_PCPU_INT_REG register
 *  GPIO PRO_CPU interrupt status register for GPIO0-30
 */
#define GPIO_PCPU_INT_REG (DR_REG_GPIO_BASE + 0x5c)
/** GPIO_PROCPU_INT : RO; bitpos: [30:0]; default: 0;
 *  GPIO PRO_CPU interrupt status register for GPIO0-30
 */
#define GPIO_PROCPU_INT    0x7FFFFFFFU
#define GPIO_PROCPU_INT_M  (GPIO_PROCPU_INT_V << GPIO_PROCPU_INT_S)
#define GPIO_PROCPU_INT_V  0x7FFFFFFFU
#define GPIO_PROCPU_INT_S  0

/** GPIO_PCPU_NMI_INT_REG register
 *  GPIO PRO_CPU(not shielded) interrupt status register for GPIO0-30
 */
#define GPIO_PCPU_NMI_INT_REG (DR_REG_GPIO_BASE + 0x60)
/** GPIO_PROCPU_NMI_INT : RO; bitpos: [30:0]; default: 0;
 *  GPIO PRO_CPU(not shielded) interrupt status register for GPIO0-30
 */
#define GPIO_PROCPU_NMI_INT    0x7FFFFFFFU
#define GPIO_PROCPU_NMI_INT_M  (GPIO_PROCPU_NMI_INT_V << GPIO_PROCPU_NMI_INT_S)
#define GPIO_PROCPU_NMI_INT_V  0x7FFFFFFFU
#define GPIO_PROCPU_NMI_INT_S  0

/** GPIO_CPUSDIO_INT_REG register
 *  GPIO CPUSDIO interrupt status register for GPIO0-30
 */
#define GPIO_CPUSDIO_INT_REG (DR_REG_GPIO_BASE + 0x64)
/** GPIO_SDIO_INT : RO; bitpos: [30:0]; default: 0;
 *  GPIO CPUSDIO interrupt status register for GPIO0-30
 */
#define GPIO_SDIO_INT    0x7FFFFFFFU
#define GPIO_SDIO_INT_M  (GPIO_SDIO_INT_V << GPIO_SDIO_INT_S)
#define GPIO_SDIO_INT_V  0x7FFFFFFFU
#define GPIO_SDIO_INT_S  0

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

/** GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register for GPIO0-31
 */
#define GPIO_STATUS_NEXT_REG (DR_REG_GPIO_BASE + 0x14c)
/** GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [30:0]; default: 0;
 *  GPIO interrupt source register for GPIO0-31
 */
#define GPIO_STATUS_INTERRUPT_NEXT    0x7FFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_M  (GPIO_STATUS_INTERRUPT_NEXT_V << GPIO_STATUS_INTERRUPT_NEXT_S)
#define GPIO_STATUS_INTERRUPT_NEXT_V  0x7FFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

/** GPIO_FUNC0_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC0_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x154)
/** GPIO_FUNC0_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC0_IN_SEL    0x0000003FU
#define GPIO_FUNC0_IN_SEL_M  (GPIO_FUNC0_IN_SEL_V << GPIO_FUNC0_IN_SEL_S)
#define GPIO_FUNC0_IN_SEL_V  0x0000003FU
#define GPIO_FUNC0_IN_SEL_S  0
/** GPIO_FUNC0_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC0_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC0_IN_INV_SEL_M  (GPIO_FUNC0_IN_INV_SEL_V << GPIO_FUNC0_IN_INV_SEL_S)
#define GPIO_FUNC0_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_IN_INV_SEL_S  6
/** GPIO_SIG0_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG0_IN_SEL    (BIT(7))
#define GPIO_SIG0_IN_SEL_M  (GPIO_SIG0_IN_SEL_V << GPIO_SIG0_IN_SEL_S)
#define GPIO_SIG0_IN_SEL_V  0x00000001U
#define GPIO_SIG0_IN_SEL_S  7

/** GPIO_FUNC6_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC6_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x16c)
/** GPIO_FUNC6_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC7_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x170)
/** GPIO_FUNC7_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC8_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x174)
/** GPIO_FUNC8_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC9_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x178)
/** GPIO_FUNC9_IN_SEL : R/W; bitpos: [5:0]; default: 56;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC10_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x17c)
/** GPIO_FUNC10_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC11_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x180)
/** GPIO_FUNC11_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC12_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x184)
/** GPIO_FUNC12_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC13_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x188)
/** GPIO_FUNC13_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC14_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x18c)
/** GPIO_FUNC14_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC15_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x190)
/** GPIO_FUNC15_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC16_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x194)
/** GPIO_FUNC16_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC17_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x198)
/** GPIO_FUNC17_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC19_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC19_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1a0)
/** GPIO_FUNC19_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC21_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC21_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1a8)
/** GPIO_FUNC21_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC22_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1ac)
/** GPIO_FUNC22_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC23_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1b0)
/** GPIO_FUNC23_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC24_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1b4)
/** GPIO_FUNC24_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC28_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC28_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1c4)
/** GPIO_FUNC28_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC29_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1c8)
/** GPIO_FUNC29_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC30_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1cc)
/** GPIO_FUNC30_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC31_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1d0)
/** GPIO_FUNC31_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC32_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1d4)
/** GPIO_FUNC32_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC33_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1d8)
/** GPIO_FUNC33_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC34_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1dc)
/** GPIO_FUNC34_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC35_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1e0)
/** GPIO_FUNC35_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC40_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC40_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1f4)
/** GPIO_FUNC40_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC41_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1f8)
/** GPIO_FUNC41_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC42_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x1fc)
/** GPIO_FUNC42_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC45_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC45_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x208)
/** GPIO_FUNC45_IN_SEL : R/W; bitpos: [5:0]; default: 56;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC46_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC46_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x20c)
/** GPIO_FUNC46_IN_SEL : R/W; bitpos: [5:0]; default: 56;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC46_IN_SEL    0x0000003FU
#define GPIO_FUNC46_IN_SEL_M  (GPIO_FUNC46_IN_SEL_V << GPIO_FUNC46_IN_SEL_S)
#define GPIO_FUNC46_IN_SEL_V  0x0000003FU
#define GPIO_FUNC46_IN_SEL_S  0
/** GPIO_FUNC46_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC46_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC46_IN_INV_SEL_M  (GPIO_FUNC46_IN_INV_SEL_V << GPIO_FUNC46_IN_INV_SEL_S)
#define GPIO_FUNC46_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_IN_INV_SEL_S  6
/** GPIO_SIG46_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG46_IN_SEL    (BIT(7))
#define GPIO_SIG46_IN_SEL_M  (GPIO_SIG46_IN_SEL_V << GPIO_SIG46_IN_SEL_S)
#define GPIO_SIG46_IN_SEL_V  0x00000001U
#define GPIO_SIG46_IN_SEL_S  7

/** GPIO_FUNC47_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC47_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x210)
/** GPIO_FUNC47_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC48_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x214)
/** GPIO_FUNC48_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC49_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x218)
/** GPIO_FUNC49_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC50_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x21c)
/** GPIO_FUNC50_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC51_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x220)
/** GPIO_FUNC51_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC52_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x224)
/** GPIO_FUNC52_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC53_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x228)
/** GPIO_FUNC53_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC54_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x22c)
/** GPIO_FUNC54_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC63_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC63_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x250)
/** GPIO_FUNC63_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC64_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x254)
/** GPIO_FUNC64_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC65_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x258)
/** GPIO_FUNC65_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC66_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x25c)
/** GPIO_FUNC66_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC67_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC67_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x260)
/** GPIO_FUNC67_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC67_IN_SEL    0x0000003FU
#define GPIO_FUNC67_IN_SEL_M  (GPIO_FUNC67_IN_SEL_V << GPIO_FUNC67_IN_SEL_S)
#define GPIO_FUNC67_IN_SEL_V  0x0000003FU
#define GPIO_FUNC67_IN_SEL_S  0
/** GPIO_FUNC67_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC67_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC67_IN_INV_SEL_M  (GPIO_FUNC67_IN_INV_SEL_V << GPIO_FUNC67_IN_INV_SEL_S)
#define GPIO_FUNC67_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC67_IN_INV_SEL_S  6
/** GPIO_SIG67_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG67_IN_SEL    (BIT(7))
#define GPIO_SIG67_IN_SEL_M  (GPIO_SIG67_IN_SEL_V << GPIO_SIG67_IN_SEL_S)
#define GPIO_SIG67_IN_SEL_V  0x00000001U
#define GPIO_SIG67_IN_SEL_S  7

/** GPIO_FUNC68_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC68_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x264)
/** GPIO_FUNC68_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC69_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x268)
/** GPIO_FUNC69_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC70_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x26c)
/** GPIO_FUNC70_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC71_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x270)
/** GPIO_FUNC71_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC72_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC72_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x274)
/** GPIO_FUNC72_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC72_IN_SEL    0x0000003FU
#define GPIO_FUNC72_IN_SEL_M  (GPIO_FUNC72_IN_SEL_V << GPIO_FUNC72_IN_SEL_S)
#define GPIO_FUNC72_IN_SEL_V  0x0000003FU
#define GPIO_FUNC72_IN_SEL_S  0
/** GPIO_FUNC72_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC72_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC72_IN_INV_SEL_M  (GPIO_FUNC72_IN_INV_SEL_V << GPIO_FUNC72_IN_INV_SEL_S)
#define GPIO_FUNC72_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC72_IN_INV_SEL_S  6
/** GPIO_SIG72_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG72_IN_SEL    (BIT(7))
#define GPIO_SIG72_IN_SEL_M  (GPIO_SIG72_IN_SEL_V << GPIO_SIG72_IN_SEL_S)
#define GPIO_SIG72_IN_SEL_V  0x00000001U
#define GPIO_SIG72_IN_SEL_S  7

/** GPIO_FUNC73_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC73_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x278)
/** GPIO_FUNC73_IN_SEL : R/W; bitpos: [5:0]; default: 56;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC73_IN_SEL    0x0000003FU
#define GPIO_FUNC73_IN_SEL_M  (GPIO_FUNC73_IN_SEL_V << GPIO_FUNC73_IN_SEL_S)
#define GPIO_FUNC73_IN_SEL_V  0x0000003FU
#define GPIO_FUNC73_IN_SEL_S  0
/** GPIO_FUNC73_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC73_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC73_IN_INV_SEL_M  (GPIO_FUNC73_IN_INV_SEL_V << GPIO_FUNC73_IN_INV_SEL_S)
#define GPIO_FUNC73_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC73_IN_INV_SEL_S  6
/** GPIO_SIG73_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG73_IN_SEL    (BIT(7))
#define GPIO_SIG73_IN_SEL_M  (GPIO_SIG73_IN_SEL_V << GPIO_SIG73_IN_SEL_S)
#define GPIO_SIG73_IN_SEL_V  0x00000001U
#define GPIO_SIG73_IN_SEL_S  7

/** GPIO_FUNC77_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC77_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x288)
/** GPIO_FUNC77_IN_SEL : R/W; bitpos: [5:0]; default: 56;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC81_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC81_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x298)
/** GPIO_FUNC81_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC81_IN_SEL    0x0000003FU
#define GPIO_FUNC81_IN_SEL_M  (GPIO_FUNC81_IN_SEL_V << GPIO_FUNC81_IN_SEL_S)
#define GPIO_FUNC81_IN_SEL_V  0x0000003FU
#define GPIO_FUNC81_IN_SEL_S  0
/** GPIO_FUNC81_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC81_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC81_IN_INV_SEL_M  (GPIO_FUNC81_IN_INV_SEL_V << GPIO_FUNC81_IN_INV_SEL_S)
#define GPIO_FUNC81_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC81_IN_INV_SEL_S  6
/** GPIO_SIG81_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG81_IN_SEL    (BIT(7))
#define GPIO_SIG81_IN_SEL_M  (GPIO_SIG81_IN_SEL_V << GPIO_SIG81_IN_SEL_S)
#define GPIO_SIG81_IN_SEL_V  0x00000001U
#define GPIO_SIG81_IN_SEL_S  7

/** GPIO_FUNC82_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC82_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x29c)
/** GPIO_FUNC82_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC82_IN_SEL    0x0000003FU
#define GPIO_FUNC82_IN_SEL_M  (GPIO_FUNC82_IN_SEL_V << GPIO_FUNC82_IN_SEL_S)
#define GPIO_FUNC82_IN_SEL_V  0x0000003FU
#define GPIO_FUNC82_IN_SEL_S  0
/** GPIO_FUNC82_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC82_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC82_IN_INV_SEL_M  (GPIO_FUNC82_IN_INV_SEL_V << GPIO_FUNC82_IN_INV_SEL_S)
#define GPIO_FUNC82_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC82_IN_INV_SEL_S  6
/** GPIO_SIG82_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG82_IN_SEL    (BIT(7))
#define GPIO_SIG82_IN_SEL_M  (GPIO_SIG82_IN_SEL_V << GPIO_SIG82_IN_SEL_S)
#define GPIO_SIG82_IN_SEL_V  0x00000001U
#define GPIO_SIG82_IN_SEL_S  7

/** GPIO_FUNC83_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC83_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2a0)
/** GPIO_FUNC83_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC84_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC84_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2a4)
/** GPIO_FUNC84_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC84_IN_SEL    0x0000003FU
#define GPIO_FUNC84_IN_SEL_M  (GPIO_FUNC84_IN_SEL_V << GPIO_FUNC84_IN_SEL_S)
#define GPIO_FUNC84_IN_SEL_V  0x0000003FU
#define GPIO_FUNC84_IN_SEL_S  0
/** GPIO_FUNC84_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC84_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC84_IN_INV_SEL_M  (GPIO_FUNC84_IN_INV_SEL_V << GPIO_FUNC84_IN_INV_SEL_S)
#define GPIO_FUNC84_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC84_IN_INV_SEL_S  6
/** GPIO_SIG84_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG84_IN_SEL    (BIT(7))
#define GPIO_SIG84_IN_SEL_M  (GPIO_SIG84_IN_SEL_V << GPIO_SIG84_IN_SEL_S)
#define GPIO_SIG84_IN_SEL_V  0x00000001U
#define GPIO_SIG84_IN_SEL_S  7

/** GPIO_FUNC85_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC85_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2a8)
/** GPIO_FUNC85_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC85_IN_SEL    0x0000003FU
#define GPIO_FUNC85_IN_SEL_M  (GPIO_FUNC85_IN_SEL_V << GPIO_FUNC85_IN_SEL_S)
#define GPIO_FUNC85_IN_SEL_V  0x0000003FU
#define GPIO_FUNC85_IN_SEL_S  0
/** GPIO_FUNC85_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC85_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC85_IN_INV_SEL_M  (GPIO_FUNC85_IN_INV_SEL_V << GPIO_FUNC85_IN_INV_SEL_S)
#define GPIO_FUNC85_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC85_IN_INV_SEL_S  6
/** GPIO_SIG85_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG85_IN_SEL    (BIT(7))
#define GPIO_SIG85_IN_SEL_M  (GPIO_SIG85_IN_SEL_V << GPIO_SIG85_IN_SEL_S)
#define GPIO_SIG85_IN_SEL_V  0x00000001U
#define GPIO_SIG85_IN_SEL_S  7

/** GPIO_FUNC86_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC86_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2ac)
/** GPIO_FUNC86_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC87_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC87_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2b0)
/** GPIO_FUNC87_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC87_IN_SEL    0x0000003FU
#define GPIO_FUNC87_IN_SEL_M  (GPIO_FUNC87_IN_SEL_V << GPIO_FUNC87_IN_SEL_S)
#define GPIO_FUNC87_IN_SEL_V  0x0000003FU
#define GPIO_FUNC87_IN_SEL_S  0
/** GPIO_FUNC87_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC87_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC87_IN_INV_SEL_M  (GPIO_FUNC87_IN_INV_SEL_V << GPIO_FUNC87_IN_INV_SEL_S)
#define GPIO_FUNC87_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC87_IN_INV_SEL_S  6
/** GPIO_SIG87_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG87_IN_SEL    (BIT(7))
#define GPIO_SIG87_IN_SEL_M  (GPIO_SIG87_IN_SEL_V << GPIO_SIG87_IN_SEL_S)
#define GPIO_SIG87_IN_SEL_V  0x00000001U
#define GPIO_SIG87_IN_SEL_S  7

/** GPIO_FUNC88_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC88_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2b4)
/** GPIO_FUNC88_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC88_IN_SEL    0x0000003FU
#define GPIO_FUNC88_IN_SEL_M  (GPIO_FUNC88_IN_SEL_V << GPIO_FUNC88_IN_SEL_S)
#define GPIO_FUNC88_IN_SEL_V  0x0000003FU
#define GPIO_FUNC88_IN_SEL_S  0
/** GPIO_FUNC88_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC88_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC88_IN_INV_SEL_M  (GPIO_FUNC88_IN_INV_SEL_V << GPIO_FUNC88_IN_INV_SEL_S)
#define GPIO_FUNC88_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC88_IN_INV_SEL_S  6
/** GPIO_SIG88_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG88_IN_SEL    (BIT(7))
#define GPIO_SIG88_IN_SEL_M  (GPIO_SIG88_IN_SEL_V << GPIO_SIG88_IN_SEL_S)
#define GPIO_SIG88_IN_SEL_V  0x00000001U
#define GPIO_SIG88_IN_SEL_S  7

/** GPIO_FUNC89_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC89_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2b8)
/** GPIO_FUNC89_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC90_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2bc)
/** GPIO_FUNC90_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC91_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c0)
/** GPIO_FUNC91_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC92_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c4)
/** GPIO_FUNC92_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC93_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c8)
/** GPIO_FUNC93_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC94_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2cc)
/** GPIO_FUNC94_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC95_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2d0)
/** GPIO_FUNC95_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC97_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC97_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2d8)
/** GPIO_FUNC97_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC98_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2dc)
/** GPIO_FUNC98_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC99_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e0)
/** GPIO_FUNC99_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC100_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e4)
/** GPIO_FUNC100_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC101_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e8)
/** GPIO_FUNC101_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC102_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2ec)
/** GPIO_FUNC102_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC103_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f0)
/** GPIO_FUNC103_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC104_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f4)
/** GPIO_FUNC104_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC105_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f8)
/** GPIO_FUNC105_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC106_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2fc)
/** GPIO_FUNC106_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC107_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x300)
/** GPIO_FUNC107_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC108_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x304)
/** GPIO_FUNC108_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC109_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x308)
/** GPIO_FUNC109_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC110_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x30c)
/** GPIO_FUNC110_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC111_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x310)
/** GPIO_FUNC111_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC112_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x314)
/** GPIO_FUNC112_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC113_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x318)
/** GPIO_FUNC113_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC114_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x31c)
/** GPIO_FUNC114_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC115_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC115_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x320)
/** GPIO_FUNC115_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC115_IN_SEL    0x0000003FU
#define GPIO_FUNC115_IN_SEL_M  (GPIO_FUNC115_IN_SEL_V << GPIO_FUNC115_IN_SEL_S)
#define GPIO_FUNC115_IN_SEL_V  0x0000003FU
#define GPIO_FUNC115_IN_SEL_S  0
/** GPIO_FUNC115_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC115_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC115_IN_INV_SEL_M  (GPIO_FUNC115_IN_INV_SEL_V << GPIO_FUNC115_IN_INV_SEL_S)
#define GPIO_FUNC115_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC115_IN_INV_SEL_S  6
/** GPIO_SIG115_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG115_IN_SEL    (BIT(7))
#define GPIO_SIG115_IN_SEL_M  (GPIO_SIG115_IN_SEL_V << GPIO_SIG115_IN_SEL_S)
#define GPIO_SIG115_IN_SEL_V  0x00000001U
#define GPIO_SIG115_IN_SEL_S  7

/** GPIO_FUNC116_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC116_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x324)
/** GPIO_FUNC116_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC116_IN_SEL    0x0000003FU
#define GPIO_FUNC116_IN_SEL_M  (GPIO_FUNC116_IN_SEL_V << GPIO_FUNC116_IN_SEL_S)
#define GPIO_FUNC116_IN_SEL_V  0x0000003FU
#define GPIO_FUNC116_IN_SEL_S  0
/** GPIO_FUNC116_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC116_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC116_IN_INV_SEL_M  (GPIO_FUNC116_IN_INV_SEL_V << GPIO_FUNC116_IN_INV_SEL_S)
#define GPIO_FUNC116_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC116_IN_INV_SEL_S  6
/** GPIO_SIG116_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG116_IN_SEL    (BIT(7))
#define GPIO_SIG116_IN_SEL_M  (GPIO_SIG116_IN_SEL_V << GPIO_SIG116_IN_SEL_S)
#define GPIO_SIG116_IN_SEL_V  0x00000001U
#define GPIO_SIG116_IN_SEL_S  7

/** GPIO_FUNC117_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC117_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x328)
/** GPIO_FUNC117_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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
#define GPIO_FUNC118_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x32c)
/** GPIO_FUNC118_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
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

/** GPIO_FUNC119_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC119_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x330)
/** GPIO_FUNC119_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC119_IN_SEL    0x0000003FU
#define GPIO_FUNC119_IN_SEL_M  (GPIO_FUNC119_IN_SEL_V << GPIO_FUNC119_IN_SEL_S)
#define GPIO_FUNC119_IN_SEL_V  0x0000003FU
#define GPIO_FUNC119_IN_SEL_S  0
/** GPIO_FUNC119_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC119_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC119_IN_INV_SEL_M  (GPIO_FUNC119_IN_INV_SEL_V << GPIO_FUNC119_IN_INV_SEL_S)
#define GPIO_FUNC119_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC119_IN_INV_SEL_S  6
/** GPIO_SIG119_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG119_IN_SEL    (BIT(7))
#define GPIO_SIG119_IN_SEL_M  (GPIO_SIG119_IN_SEL_V << GPIO_SIG119_IN_SEL_S)
#define GPIO_SIG119_IN_SEL_V  0x00000001U
#define GPIO_SIG119_IN_SEL_S  7

/** GPIO_FUNC120_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC120_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x334)
/** GPIO_FUNC120_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC120_IN_SEL    0x0000003FU
#define GPIO_FUNC120_IN_SEL_M  (GPIO_FUNC120_IN_SEL_V << GPIO_FUNC120_IN_SEL_S)
#define GPIO_FUNC120_IN_SEL_V  0x0000003FU
#define GPIO_FUNC120_IN_SEL_S  0
/** GPIO_FUNC120_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC120_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC120_IN_INV_SEL_M  (GPIO_FUNC120_IN_INV_SEL_V << GPIO_FUNC120_IN_INV_SEL_S)
#define GPIO_FUNC120_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC120_IN_INV_SEL_S  6
/** GPIO_SIG120_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG120_IN_SEL    (BIT(7))
#define GPIO_SIG120_IN_SEL_M  (GPIO_SIG120_IN_SEL_V << GPIO_SIG120_IN_SEL_S)
#define GPIO_SIG120_IN_SEL_V  0x00000001U
#define GPIO_SIG120_IN_SEL_S  7

/** GPIO_FUNC121_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC121_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x338)
/** GPIO_FUNC121_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC121_IN_SEL    0x0000003FU
#define GPIO_FUNC121_IN_SEL_M  (GPIO_FUNC121_IN_SEL_V << GPIO_FUNC121_IN_SEL_S)
#define GPIO_FUNC121_IN_SEL_V  0x0000003FU
#define GPIO_FUNC121_IN_SEL_S  0
/** GPIO_FUNC121_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC121_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC121_IN_INV_SEL_M  (GPIO_FUNC121_IN_INV_SEL_V << GPIO_FUNC121_IN_INV_SEL_S)
#define GPIO_FUNC121_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC121_IN_INV_SEL_S  6
/** GPIO_SIG121_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG121_IN_SEL    (BIT(7))
#define GPIO_SIG121_IN_SEL_M  (GPIO_SIG121_IN_SEL_V << GPIO_SIG121_IN_SEL_S)
#define GPIO_SIG121_IN_SEL_V  0x00000001U
#define GPIO_SIG121_IN_SEL_S  7

/** GPIO_FUNC122_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC122_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x33c)
/** GPIO_FUNC122_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC122_IN_SEL    0x0000003FU
#define GPIO_FUNC122_IN_SEL_M  (GPIO_FUNC122_IN_SEL_V << GPIO_FUNC122_IN_SEL_S)
#define GPIO_FUNC122_IN_SEL_V  0x0000003FU
#define GPIO_FUNC122_IN_SEL_S  0
/** GPIO_FUNC122_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC122_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC122_IN_INV_SEL_M  (GPIO_FUNC122_IN_INV_SEL_V << GPIO_FUNC122_IN_INV_SEL_S)
#define GPIO_FUNC122_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC122_IN_INV_SEL_S  6
/** GPIO_SIG122_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG122_IN_SEL    (BIT(7))
#define GPIO_SIG122_IN_SEL_M  (GPIO_SIG122_IN_SEL_V << GPIO_SIG122_IN_SEL_S)
#define GPIO_SIG122_IN_SEL_V  0x00000001U
#define GPIO_SIG122_IN_SEL_S  7

/** GPIO_FUNC123_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC123_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x340)
/** GPIO_FUNC123_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC123_IN_SEL    0x0000003FU
#define GPIO_FUNC123_IN_SEL_M  (GPIO_FUNC123_IN_SEL_V << GPIO_FUNC123_IN_SEL_S)
#define GPIO_FUNC123_IN_SEL_V  0x0000003FU
#define GPIO_FUNC123_IN_SEL_S  0
/** GPIO_FUNC123_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC123_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC123_IN_INV_SEL_M  (GPIO_FUNC123_IN_INV_SEL_V << GPIO_FUNC123_IN_INV_SEL_S)
#define GPIO_FUNC123_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC123_IN_INV_SEL_S  6
/** GPIO_SIG123_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG123_IN_SEL    (BIT(7))
#define GPIO_SIG123_IN_SEL_M  (GPIO_SIG123_IN_SEL_V << GPIO_SIG123_IN_SEL_S)
#define GPIO_SIG123_IN_SEL_V  0x00000001U
#define GPIO_SIG123_IN_SEL_S  7

/** GPIO_FUNC124_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC124_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x344)
/** GPIO_FUNC124_IN_SEL : R/W; bitpos: [5:0]; default: 60;
 *  set this value: s=0-34: connect GPIO[s] to this port. s=0x38: set this port always
 *  high level. s=0x3C: set this port always low level.
 */
#define GPIO_FUNC124_IN_SEL    0x0000003FU
#define GPIO_FUNC124_IN_SEL_M  (GPIO_FUNC124_IN_SEL_V << GPIO_FUNC124_IN_SEL_S)
#define GPIO_FUNC124_IN_SEL_V  0x0000003FU
#define GPIO_FUNC124_IN_SEL_S  0
/** GPIO_FUNC124_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC124_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC124_IN_INV_SEL_M  (GPIO_FUNC124_IN_INV_SEL_V << GPIO_FUNC124_IN_INV_SEL_S)
#define GPIO_FUNC124_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC124_IN_INV_SEL_S  6
/** GPIO_SIG124_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG124_IN_SEL    (BIT(7))
#define GPIO_SIG124_IN_SEL_M  (GPIO_SIG124_IN_SEL_V << GPIO_SIG124_IN_SEL_S)
#define GPIO_SIG124_IN_SEL_V  0x00000001U
#define GPIO_SIG124_IN_SEL_S  7

/** GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x554)
/** GPIO_FUNC0_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC0_OUT_SEL    0x000000FFU
#define GPIO_FUNC0_OUT_SEL_M  (GPIO_FUNC0_OUT_SEL_V << GPIO_FUNC0_OUT_SEL_S)
#define GPIO_FUNC0_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC0_OUT_SEL_S  0
/** GPIO_FUNC0_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC0_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC0_OUT_INV_SEL_M  (GPIO_FUNC0_OUT_INV_SEL_V << GPIO_FUNC0_OUT_INV_SEL_S)
#define GPIO_FUNC0_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OUT_INV_SEL_S  8
/** GPIO_FUNC0_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC0_OEN_SEL    (BIT(9))
#define GPIO_FUNC0_OEN_SEL_M  (GPIO_FUNC0_OEN_SEL_V << GPIO_FUNC0_OEN_SEL_S)
#define GPIO_FUNC0_OEN_SEL_V  0x00000001U
#define GPIO_FUNC0_OEN_SEL_S  9
/** GPIO_FUNC0_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC0_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC0_OEN_INV_SEL_M  (GPIO_FUNC0_OEN_INV_SEL_V << GPIO_FUNC0_OEN_INV_SEL_S)
#define GPIO_FUNC0_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OEN_INV_SEL_S  10

/** GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x558)
/** GPIO_FUNC1_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC1_OUT_SEL    0x000000FFU
#define GPIO_FUNC1_OUT_SEL_M  (GPIO_FUNC1_OUT_SEL_V << GPIO_FUNC1_OUT_SEL_S)
#define GPIO_FUNC1_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC1_OUT_SEL_S  0
/** GPIO_FUNC1_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC1_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC1_OUT_INV_SEL_M  (GPIO_FUNC1_OUT_INV_SEL_V << GPIO_FUNC1_OUT_INV_SEL_S)
#define GPIO_FUNC1_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OUT_INV_SEL_S  8
/** GPIO_FUNC1_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC1_OEN_SEL    (BIT(9))
#define GPIO_FUNC1_OEN_SEL_M  (GPIO_FUNC1_OEN_SEL_V << GPIO_FUNC1_OEN_SEL_S)
#define GPIO_FUNC1_OEN_SEL_V  0x00000001U
#define GPIO_FUNC1_OEN_SEL_S  9
/** GPIO_FUNC1_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC1_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC1_OEN_INV_SEL_M  (GPIO_FUNC1_OEN_INV_SEL_V << GPIO_FUNC1_OEN_INV_SEL_S)
#define GPIO_FUNC1_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OEN_INV_SEL_S  10

/** GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x55c)
/** GPIO_FUNC2_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC2_OUT_SEL    0x000000FFU
#define GPIO_FUNC2_OUT_SEL_M  (GPIO_FUNC2_OUT_SEL_V << GPIO_FUNC2_OUT_SEL_S)
#define GPIO_FUNC2_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC2_OUT_SEL_S  0
/** GPIO_FUNC2_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC2_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC2_OUT_INV_SEL_M  (GPIO_FUNC2_OUT_INV_SEL_V << GPIO_FUNC2_OUT_INV_SEL_S)
#define GPIO_FUNC2_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OUT_INV_SEL_S  8
/** GPIO_FUNC2_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC2_OEN_SEL    (BIT(9))
#define GPIO_FUNC2_OEN_SEL_M  (GPIO_FUNC2_OEN_SEL_V << GPIO_FUNC2_OEN_SEL_S)
#define GPIO_FUNC2_OEN_SEL_V  0x00000001U
#define GPIO_FUNC2_OEN_SEL_S  9
/** GPIO_FUNC2_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC2_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC2_OEN_INV_SEL_M  (GPIO_FUNC2_OEN_INV_SEL_V << GPIO_FUNC2_OEN_INV_SEL_S)
#define GPIO_FUNC2_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OEN_INV_SEL_S  10

/** GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x560)
/** GPIO_FUNC3_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC3_OUT_SEL    0x000000FFU
#define GPIO_FUNC3_OUT_SEL_M  (GPIO_FUNC3_OUT_SEL_V << GPIO_FUNC3_OUT_SEL_S)
#define GPIO_FUNC3_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC3_OUT_SEL_S  0
/** GPIO_FUNC3_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC3_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC3_OUT_INV_SEL_M  (GPIO_FUNC3_OUT_INV_SEL_V << GPIO_FUNC3_OUT_INV_SEL_S)
#define GPIO_FUNC3_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OUT_INV_SEL_S  8
/** GPIO_FUNC3_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC3_OEN_SEL    (BIT(9))
#define GPIO_FUNC3_OEN_SEL_M  (GPIO_FUNC3_OEN_SEL_V << GPIO_FUNC3_OEN_SEL_S)
#define GPIO_FUNC3_OEN_SEL_V  0x00000001U
#define GPIO_FUNC3_OEN_SEL_S  9
/** GPIO_FUNC3_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC3_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC3_OEN_INV_SEL_M  (GPIO_FUNC3_OEN_INV_SEL_V << GPIO_FUNC3_OEN_INV_SEL_S)
#define GPIO_FUNC3_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OEN_INV_SEL_S  10

/** GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x564)
/** GPIO_FUNC4_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC4_OUT_SEL    0x000000FFU
#define GPIO_FUNC4_OUT_SEL_M  (GPIO_FUNC4_OUT_SEL_V << GPIO_FUNC4_OUT_SEL_S)
#define GPIO_FUNC4_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC4_OUT_SEL_S  0
/** GPIO_FUNC4_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC4_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC4_OUT_INV_SEL_M  (GPIO_FUNC4_OUT_INV_SEL_V << GPIO_FUNC4_OUT_INV_SEL_S)
#define GPIO_FUNC4_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OUT_INV_SEL_S  8
/** GPIO_FUNC4_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC4_OEN_SEL    (BIT(9))
#define GPIO_FUNC4_OEN_SEL_M  (GPIO_FUNC4_OEN_SEL_V << GPIO_FUNC4_OEN_SEL_S)
#define GPIO_FUNC4_OEN_SEL_V  0x00000001U
#define GPIO_FUNC4_OEN_SEL_S  9
/** GPIO_FUNC4_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC4_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC4_OEN_INV_SEL_M  (GPIO_FUNC4_OEN_INV_SEL_V << GPIO_FUNC4_OEN_INV_SEL_S)
#define GPIO_FUNC4_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OEN_INV_SEL_S  10

/** GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x568)
/** GPIO_FUNC5_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC5_OUT_SEL    0x000000FFU
#define GPIO_FUNC5_OUT_SEL_M  (GPIO_FUNC5_OUT_SEL_V << GPIO_FUNC5_OUT_SEL_S)
#define GPIO_FUNC5_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC5_OUT_SEL_S  0
/** GPIO_FUNC5_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC5_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC5_OUT_INV_SEL_M  (GPIO_FUNC5_OUT_INV_SEL_V << GPIO_FUNC5_OUT_INV_SEL_S)
#define GPIO_FUNC5_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OUT_INV_SEL_S  8
/** GPIO_FUNC5_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC5_OEN_SEL    (BIT(9))
#define GPIO_FUNC5_OEN_SEL_M  (GPIO_FUNC5_OEN_SEL_V << GPIO_FUNC5_OEN_SEL_S)
#define GPIO_FUNC5_OEN_SEL_V  0x00000001U
#define GPIO_FUNC5_OEN_SEL_S  9
/** GPIO_FUNC5_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC5_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC5_OEN_INV_SEL_M  (GPIO_FUNC5_OEN_INV_SEL_V << GPIO_FUNC5_OEN_INV_SEL_S)
#define GPIO_FUNC5_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OEN_INV_SEL_S  10

/** GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x56c)
/** GPIO_FUNC6_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC6_OUT_SEL    0x000000FFU
#define GPIO_FUNC6_OUT_SEL_M  (GPIO_FUNC6_OUT_SEL_V << GPIO_FUNC6_OUT_SEL_S)
#define GPIO_FUNC6_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC6_OUT_SEL_S  0
/** GPIO_FUNC6_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC6_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC6_OUT_INV_SEL_M  (GPIO_FUNC6_OUT_INV_SEL_V << GPIO_FUNC6_OUT_INV_SEL_S)
#define GPIO_FUNC6_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OUT_INV_SEL_S  8
/** GPIO_FUNC6_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC6_OEN_SEL    (BIT(9))
#define GPIO_FUNC6_OEN_SEL_M  (GPIO_FUNC6_OEN_SEL_V << GPIO_FUNC6_OEN_SEL_S)
#define GPIO_FUNC6_OEN_SEL_V  0x00000001U
#define GPIO_FUNC6_OEN_SEL_S  9
/** GPIO_FUNC6_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC6_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC6_OEN_INV_SEL_M  (GPIO_FUNC6_OEN_INV_SEL_V << GPIO_FUNC6_OEN_INV_SEL_S)
#define GPIO_FUNC6_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OEN_INV_SEL_S  10

/** GPIO_FUNC7_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC7_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x570)
/** GPIO_FUNC7_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC7_OUT_SEL    0x000000FFU
#define GPIO_FUNC7_OUT_SEL_M  (GPIO_FUNC7_OUT_SEL_V << GPIO_FUNC7_OUT_SEL_S)
#define GPIO_FUNC7_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC7_OUT_SEL_S  0
/** GPIO_FUNC7_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC7_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC7_OUT_INV_SEL_M  (GPIO_FUNC7_OUT_INV_SEL_V << GPIO_FUNC7_OUT_INV_SEL_S)
#define GPIO_FUNC7_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OUT_INV_SEL_S  8
/** GPIO_FUNC7_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC7_OEN_SEL    (BIT(9))
#define GPIO_FUNC7_OEN_SEL_M  (GPIO_FUNC7_OEN_SEL_V << GPIO_FUNC7_OEN_SEL_S)
#define GPIO_FUNC7_OEN_SEL_V  0x00000001U
#define GPIO_FUNC7_OEN_SEL_S  9
/** GPIO_FUNC7_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC7_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC7_OEN_INV_SEL_M  (GPIO_FUNC7_OEN_INV_SEL_V << GPIO_FUNC7_OEN_INV_SEL_S)
#define GPIO_FUNC7_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OEN_INV_SEL_S  10

/** GPIO_FUNC8_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC8_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x574)
/** GPIO_FUNC8_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC8_OUT_SEL    0x000000FFU
#define GPIO_FUNC8_OUT_SEL_M  (GPIO_FUNC8_OUT_SEL_V << GPIO_FUNC8_OUT_SEL_S)
#define GPIO_FUNC8_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC8_OUT_SEL_S  0
/** GPIO_FUNC8_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC8_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC8_OUT_INV_SEL_M  (GPIO_FUNC8_OUT_INV_SEL_V << GPIO_FUNC8_OUT_INV_SEL_S)
#define GPIO_FUNC8_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OUT_INV_SEL_S  8
/** GPIO_FUNC8_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC8_OEN_SEL    (BIT(9))
#define GPIO_FUNC8_OEN_SEL_M  (GPIO_FUNC8_OEN_SEL_V << GPIO_FUNC8_OEN_SEL_S)
#define GPIO_FUNC8_OEN_SEL_V  0x00000001U
#define GPIO_FUNC8_OEN_SEL_S  9
/** GPIO_FUNC8_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC8_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC8_OEN_INV_SEL_M  (GPIO_FUNC8_OEN_INV_SEL_V << GPIO_FUNC8_OEN_INV_SEL_S)
#define GPIO_FUNC8_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OEN_INV_SEL_S  10

/** GPIO_FUNC9_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC9_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x578)
/** GPIO_FUNC9_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC9_OUT_SEL    0x000000FFU
#define GPIO_FUNC9_OUT_SEL_M  (GPIO_FUNC9_OUT_SEL_V << GPIO_FUNC9_OUT_SEL_S)
#define GPIO_FUNC9_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC9_OUT_SEL_S  0
/** GPIO_FUNC9_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC9_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC9_OUT_INV_SEL_M  (GPIO_FUNC9_OUT_INV_SEL_V << GPIO_FUNC9_OUT_INV_SEL_S)
#define GPIO_FUNC9_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OUT_INV_SEL_S  8
/** GPIO_FUNC9_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC9_OEN_SEL    (BIT(9))
#define GPIO_FUNC9_OEN_SEL_M  (GPIO_FUNC9_OEN_SEL_V << GPIO_FUNC9_OEN_SEL_S)
#define GPIO_FUNC9_OEN_SEL_V  0x00000001U
#define GPIO_FUNC9_OEN_SEL_S  9
/** GPIO_FUNC9_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC9_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC9_OEN_INV_SEL_M  (GPIO_FUNC9_OEN_INV_SEL_V << GPIO_FUNC9_OEN_INV_SEL_S)
#define GPIO_FUNC9_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OEN_INV_SEL_S  10

/** GPIO_FUNC10_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC10_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x57c)
/** GPIO_FUNC10_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC10_OUT_SEL    0x000000FFU
#define GPIO_FUNC10_OUT_SEL_M  (GPIO_FUNC10_OUT_SEL_V << GPIO_FUNC10_OUT_SEL_S)
#define GPIO_FUNC10_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC10_OUT_SEL_S  0
/** GPIO_FUNC10_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC10_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC10_OUT_INV_SEL_M  (GPIO_FUNC10_OUT_INV_SEL_V << GPIO_FUNC10_OUT_INV_SEL_S)
#define GPIO_FUNC10_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OUT_INV_SEL_S  8
/** GPIO_FUNC10_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC10_OEN_SEL    (BIT(9))
#define GPIO_FUNC10_OEN_SEL_M  (GPIO_FUNC10_OEN_SEL_V << GPIO_FUNC10_OEN_SEL_S)
#define GPIO_FUNC10_OEN_SEL_V  0x00000001U
#define GPIO_FUNC10_OEN_SEL_S  9
/** GPIO_FUNC10_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC10_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC10_OEN_INV_SEL_M  (GPIO_FUNC10_OEN_INV_SEL_V << GPIO_FUNC10_OEN_INV_SEL_S)
#define GPIO_FUNC10_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OEN_INV_SEL_S  10

/** GPIO_FUNC11_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC11_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x580)
/** GPIO_FUNC11_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC11_OUT_SEL    0x000000FFU
#define GPIO_FUNC11_OUT_SEL_M  (GPIO_FUNC11_OUT_SEL_V << GPIO_FUNC11_OUT_SEL_S)
#define GPIO_FUNC11_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC11_OUT_SEL_S  0
/** GPIO_FUNC11_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC11_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC11_OUT_INV_SEL_M  (GPIO_FUNC11_OUT_INV_SEL_V << GPIO_FUNC11_OUT_INV_SEL_S)
#define GPIO_FUNC11_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OUT_INV_SEL_S  8
/** GPIO_FUNC11_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC11_OEN_SEL    (BIT(9))
#define GPIO_FUNC11_OEN_SEL_M  (GPIO_FUNC11_OEN_SEL_V << GPIO_FUNC11_OEN_SEL_S)
#define GPIO_FUNC11_OEN_SEL_V  0x00000001U
#define GPIO_FUNC11_OEN_SEL_S  9
/** GPIO_FUNC11_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC11_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC11_OEN_INV_SEL_M  (GPIO_FUNC11_OEN_INV_SEL_V << GPIO_FUNC11_OEN_INV_SEL_S)
#define GPIO_FUNC11_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OEN_INV_SEL_S  10

/** GPIO_FUNC12_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC12_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x584)
/** GPIO_FUNC12_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC12_OUT_SEL    0x000000FFU
#define GPIO_FUNC12_OUT_SEL_M  (GPIO_FUNC12_OUT_SEL_V << GPIO_FUNC12_OUT_SEL_S)
#define GPIO_FUNC12_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC12_OUT_SEL_S  0
/** GPIO_FUNC12_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC12_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC12_OUT_INV_SEL_M  (GPIO_FUNC12_OUT_INV_SEL_V << GPIO_FUNC12_OUT_INV_SEL_S)
#define GPIO_FUNC12_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OUT_INV_SEL_S  8
/** GPIO_FUNC12_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC12_OEN_SEL    (BIT(9))
#define GPIO_FUNC12_OEN_SEL_M  (GPIO_FUNC12_OEN_SEL_V << GPIO_FUNC12_OEN_SEL_S)
#define GPIO_FUNC12_OEN_SEL_V  0x00000001U
#define GPIO_FUNC12_OEN_SEL_S  9
/** GPIO_FUNC12_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC12_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC12_OEN_INV_SEL_M  (GPIO_FUNC12_OEN_INV_SEL_V << GPIO_FUNC12_OEN_INV_SEL_S)
#define GPIO_FUNC12_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OEN_INV_SEL_S  10

/** GPIO_FUNC13_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC13_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x588)
/** GPIO_FUNC13_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC13_OUT_SEL    0x000000FFU
#define GPIO_FUNC13_OUT_SEL_M  (GPIO_FUNC13_OUT_SEL_V << GPIO_FUNC13_OUT_SEL_S)
#define GPIO_FUNC13_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC13_OUT_SEL_S  0
/** GPIO_FUNC13_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC13_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC13_OUT_INV_SEL_M  (GPIO_FUNC13_OUT_INV_SEL_V << GPIO_FUNC13_OUT_INV_SEL_S)
#define GPIO_FUNC13_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OUT_INV_SEL_S  8
/** GPIO_FUNC13_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC13_OEN_SEL    (BIT(9))
#define GPIO_FUNC13_OEN_SEL_M  (GPIO_FUNC13_OEN_SEL_V << GPIO_FUNC13_OEN_SEL_S)
#define GPIO_FUNC13_OEN_SEL_V  0x00000001U
#define GPIO_FUNC13_OEN_SEL_S  9
/** GPIO_FUNC13_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC13_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC13_OEN_INV_SEL_M  (GPIO_FUNC13_OEN_INV_SEL_V << GPIO_FUNC13_OEN_INV_SEL_S)
#define GPIO_FUNC13_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OEN_INV_SEL_S  10

/** GPIO_FUNC14_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC14_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x58c)
/** GPIO_FUNC14_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC14_OUT_SEL    0x000000FFU
#define GPIO_FUNC14_OUT_SEL_M  (GPIO_FUNC14_OUT_SEL_V << GPIO_FUNC14_OUT_SEL_S)
#define GPIO_FUNC14_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC14_OUT_SEL_S  0
/** GPIO_FUNC14_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC14_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC14_OUT_INV_SEL_M  (GPIO_FUNC14_OUT_INV_SEL_V << GPIO_FUNC14_OUT_INV_SEL_S)
#define GPIO_FUNC14_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OUT_INV_SEL_S  8
/** GPIO_FUNC14_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC14_OEN_SEL    (BIT(9))
#define GPIO_FUNC14_OEN_SEL_M  (GPIO_FUNC14_OEN_SEL_V << GPIO_FUNC14_OEN_SEL_S)
#define GPIO_FUNC14_OEN_SEL_V  0x00000001U
#define GPIO_FUNC14_OEN_SEL_S  9
/** GPIO_FUNC14_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC14_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC14_OEN_INV_SEL_M  (GPIO_FUNC14_OEN_INV_SEL_V << GPIO_FUNC14_OEN_INV_SEL_S)
#define GPIO_FUNC14_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OEN_INV_SEL_S  10

/** GPIO_FUNC15_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC15_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x590)
/** GPIO_FUNC15_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC15_OUT_SEL    0x000000FFU
#define GPIO_FUNC15_OUT_SEL_M  (GPIO_FUNC15_OUT_SEL_V << GPIO_FUNC15_OUT_SEL_S)
#define GPIO_FUNC15_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC15_OUT_SEL_S  0
/** GPIO_FUNC15_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC15_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC15_OUT_INV_SEL_M  (GPIO_FUNC15_OUT_INV_SEL_V << GPIO_FUNC15_OUT_INV_SEL_S)
#define GPIO_FUNC15_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OUT_INV_SEL_S  8
/** GPIO_FUNC15_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC15_OEN_SEL    (BIT(9))
#define GPIO_FUNC15_OEN_SEL_M  (GPIO_FUNC15_OEN_SEL_V << GPIO_FUNC15_OEN_SEL_S)
#define GPIO_FUNC15_OEN_SEL_V  0x00000001U
#define GPIO_FUNC15_OEN_SEL_S  9
/** GPIO_FUNC15_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC15_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC15_OEN_INV_SEL_M  (GPIO_FUNC15_OEN_INV_SEL_V << GPIO_FUNC15_OEN_INV_SEL_S)
#define GPIO_FUNC15_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OEN_INV_SEL_S  10

/** GPIO_FUNC16_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC16_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x594)
/** GPIO_FUNC16_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC16_OUT_SEL    0x000000FFU
#define GPIO_FUNC16_OUT_SEL_M  (GPIO_FUNC16_OUT_SEL_V << GPIO_FUNC16_OUT_SEL_S)
#define GPIO_FUNC16_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC16_OUT_SEL_S  0
/** GPIO_FUNC16_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC16_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC16_OUT_INV_SEL_M  (GPIO_FUNC16_OUT_INV_SEL_V << GPIO_FUNC16_OUT_INV_SEL_S)
#define GPIO_FUNC16_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OUT_INV_SEL_S  8
/** GPIO_FUNC16_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC16_OEN_SEL    (BIT(9))
#define GPIO_FUNC16_OEN_SEL_M  (GPIO_FUNC16_OEN_SEL_V << GPIO_FUNC16_OEN_SEL_S)
#define GPIO_FUNC16_OEN_SEL_V  0x00000001U
#define GPIO_FUNC16_OEN_SEL_S  9
/** GPIO_FUNC16_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC16_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC16_OEN_INV_SEL_M  (GPIO_FUNC16_OEN_INV_SEL_V << GPIO_FUNC16_OEN_INV_SEL_S)
#define GPIO_FUNC16_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OEN_INV_SEL_S  10

/** GPIO_FUNC17_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC17_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x598)
/** GPIO_FUNC17_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC17_OUT_SEL    0x000000FFU
#define GPIO_FUNC17_OUT_SEL_M  (GPIO_FUNC17_OUT_SEL_V << GPIO_FUNC17_OUT_SEL_S)
#define GPIO_FUNC17_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC17_OUT_SEL_S  0
/** GPIO_FUNC17_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC17_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC17_OUT_INV_SEL_M  (GPIO_FUNC17_OUT_INV_SEL_V << GPIO_FUNC17_OUT_INV_SEL_S)
#define GPIO_FUNC17_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OUT_INV_SEL_S  8
/** GPIO_FUNC17_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC17_OEN_SEL    (BIT(9))
#define GPIO_FUNC17_OEN_SEL_M  (GPIO_FUNC17_OEN_SEL_V << GPIO_FUNC17_OEN_SEL_S)
#define GPIO_FUNC17_OEN_SEL_V  0x00000001U
#define GPIO_FUNC17_OEN_SEL_S  9
/** GPIO_FUNC17_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC17_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC17_OEN_INV_SEL_M  (GPIO_FUNC17_OEN_INV_SEL_V << GPIO_FUNC17_OEN_INV_SEL_S)
#define GPIO_FUNC17_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OEN_INV_SEL_S  10

/** GPIO_FUNC18_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC18_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x59c)
/** GPIO_FUNC18_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC18_OUT_SEL    0x000000FFU
#define GPIO_FUNC18_OUT_SEL_M  (GPIO_FUNC18_OUT_SEL_V << GPIO_FUNC18_OUT_SEL_S)
#define GPIO_FUNC18_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC18_OUT_SEL_S  0
/** GPIO_FUNC18_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC18_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC18_OUT_INV_SEL_M  (GPIO_FUNC18_OUT_INV_SEL_V << GPIO_FUNC18_OUT_INV_SEL_S)
#define GPIO_FUNC18_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OUT_INV_SEL_S  8
/** GPIO_FUNC18_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC18_OEN_SEL    (BIT(9))
#define GPIO_FUNC18_OEN_SEL_M  (GPIO_FUNC18_OEN_SEL_V << GPIO_FUNC18_OEN_SEL_S)
#define GPIO_FUNC18_OEN_SEL_V  0x00000001U
#define GPIO_FUNC18_OEN_SEL_S  9
/** GPIO_FUNC18_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC18_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC18_OEN_INV_SEL_M  (GPIO_FUNC18_OEN_INV_SEL_V << GPIO_FUNC18_OEN_INV_SEL_S)
#define GPIO_FUNC18_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OEN_INV_SEL_S  10

/** GPIO_FUNC19_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC19_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a0)
/** GPIO_FUNC19_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC19_OUT_SEL    0x000000FFU
#define GPIO_FUNC19_OUT_SEL_M  (GPIO_FUNC19_OUT_SEL_V << GPIO_FUNC19_OUT_SEL_S)
#define GPIO_FUNC19_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC19_OUT_SEL_S  0
/** GPIO_FUNC19_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC19_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC19_OUT_INV_SEL_M  (GPIO_FUNC19_OUT_INV_SEL_V << GPIO_FUNC19_OUT_INV_SEL_S)
#define GPIO_FUNC19_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OUT_INV_SEL_S  8
/** GPIO_FUNC19_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC19_OEN_SEL    (BIT(9))
#define GPIO_FUNC19_OEN_SEL_M  (GPIO_FUNC19_OEN_SEL_V << GPIO_FUNC19_OEN_SEL_S)
#define GPIO_FUNC19_OEN_SEL_V  0x00000001U
#define GPIO_FUNC19_OEN_SEL_S  9
/** GPIO_FUNC19_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC19_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC19_OEN_INV_SEL_M  (GPIO_FUNC19_OEN_INV_SEL_V << GPIO_FUNC19_OEN_INV_SEL_S)
#define GPIO_FUNC19_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OEN_INV_SEL_S  10

/** GPIO_FUNC20_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC20_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a4)
/** GPIO_FUNC20_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC20_OUT_SEL    0x000000FFU
#define GPIO_FUNC20_OUT_SEL_M  (GPIO_FUNC20_OUT_SEL_V << GPIO_FUNC20_OUT_SEL_S)
#define GPIO_FUNC20_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC20_OUT_SEL_S  0
/** GPIO_FUNC20_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC20_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC20_OUT_INV_SEL_M  (GPIO_FUNC20_OUT_INV_SEL_V << GPIO_FUNC20_OUT_INV_SEL_S)
#define GPIO_FUNC20_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OUT_INV_SEL_S  8
/** GPIO_FUNC20_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC20_OEN_SEL    (BIT(9))
#define GPIO_FUNC20_OEN_SEL_M  (GPIO_FUNC20_OEN_SEL_V << GPIO_FUNC20_OEN_SEL_S)
#define GPIO_FUNC20_OEN_SEL_V  0x00000001U
#define GPIO_FUNC20_OEN_SEL_S  9
/** GPIO_FUNC20_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC20_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC20_OEN_INV_SEL_M  (GPIO_FUNC20_OEN_INV_SEL_V << GPIO_FUNC20_OEN_INV_SEL_S)
#define GPIO_FUNC20_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OEN_INV_SEL_S  10

/** GPIO_FUNC21_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC21_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a8)
/** GPIO_FUNC21_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC21_OUT_SEL    0x000000FFU
#define GPIO_FUNC21_OUT_SEL_M  (GPIO_FUNC21_OUT_SEL_V << GPIO_FUNC21_OUT_SEL_S)
#define GPIO_FUNC21_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC21_OUT_SEL_S  0
/** GPIO_FUNC21_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC21_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC21_OUT_INV_SEL_M  (GPIO_FUNC21_OUT_INV_SEL_V << GPIO_FUNC21_OUT_INV_SEL_S)
#define GPIO_FUNC21_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OUT_INV_SEL_S  8
/** GPIO_FUNC21_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC21_OEN_SEL    (BIT(9))
#define GPIO_FUNC21_OEN_SEL_M  (GPIO_FUNC21_OEN_SEL_V << GPIO_FUNC21_OEN_SEL_S)
#define GPIO_FUNC21_OEN_SEL_V  0x00000001U
#define GPIO_FUNC21_OEN_SEL_S  9
/** GPIO_FUNC21_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC21_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC21_OEN_INV_SEL_M  (GPIO_FUNC21_OEN_INV_SEL_V << GPIO_FUNC21_OEN_INV_SEL_S)
#define GPIO_FUNC21_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OEN_INV_SEL_S  10

/** GPIO_FUNC22_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC22_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5ac)
/** GPIO_FUNC22_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC22_OUT_SEL    0x000000FFU
#define GPIO_FUNC22_OUT_SEL_M  (GPIO_FUNC22_OUT_SEL_V << GPIO_FUNC22_OUT_SEL_S)
#define GPIO_FUNC22_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC22_OUT_SEL_S  0
/** GPIO_FUNC22_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC22_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC22_OUT_INV_SEL_M  (GPIO_FUNC22_OUT_INV_SEL_V << GPIO_FUNC22_OUT_INV_SEL_S)
#define GPIO_FUNC22_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OUT_INV_SEL_S  8
/** GPIO_FUNC22_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC22_OEN_SEL    (BIT(9))
#define GPIO_FUNC22_OEN_SEL_M  (GPIO_FUNC22_OEN_SEL_V << GPIO_FUNC22_OEN_SEL_S)
#define GPIO_FUNC22_OEN_SEL_V  0x00000001U
#define GPIO_FUNC22_OEN_SEL_S  9
/** GPIO_FUNC22_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC22_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC22_OEN_INV_SEL_M  (GPIO_FUNC22_OEN_INV_SEL_V << GPIO_FUNC22_OEN_INV_SEL_S)
#define GPIO_FUNC22_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OEN_INV_SEL_S  10

/** GPIO_FUNC23_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC23_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b0)
/** GPIO_FUNC23_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC23_OUT_SEL    0x000000FFU
#define GPIO_FUNC23_OUT_SEL_M  (GPIO_FUNC23_OUT_SEL_V << GPIO_FUNC23_OUT_SEL_S)
#define GPIO_FUNC23_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC23_OUT_SEL_S  0
/** GPIO_FUNC23_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC23_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC23_OUT_INV_SEL_M  (GPIO_FUNC23_OUT_INV_SEL_V << GPIO_FUNC23_OUT_INV_SEL_S)
#define GPIO_FUNC23_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OUT_INV_SEL_S  8
/** GPIO_FUNC23_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC23_OEN_SEL    (BIT(9))
#define GPIO_FUNC23_OEN_SEL_M  (GPIO_FUNC23_OEN_SEL_V << GPIO_FUNC23_OEN_SEL_S)
#define GPIO_FUNC23_OEN_SEL_V  0x00000001U
#define GPIO_FUNC23_OEN_SEL_S  9
/** GPIO_FUNC23_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC23_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC23_OEN_INV_SEL_M  (GPIO_FUNC23_OEN_INV_SEL_V << GPIO_FUNC23_OEN_INV_SEL_S)
#define GPIO_FUNC23_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OEN_INV_SEL_S  10

/** GPIO_FUNC24_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC24_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b4)
/** GPIO_FUNC24_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC24_OUT_SEL    0x000000FFU
#define GPIO_FUNC24_OUT_SEL_M  (GPIO_FUNC24_OUT_SEL_V << GPIO_FUNC24_OUT_SEL_S)
#define GPIO_FUNC24_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC24_OUT_SEL_S  0
/** GPIO_FUNC24_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC24_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC24_OUT_INV_SEL_M  (GPIO_FUNC24_OUT_INV_SEL_V << GPIO_FUNC24_OUT_INV_SEL_S)
#define GPIO_FUNC24_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OUT_INV_SEL_S  8
/** GPIO_FUNC24_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC24_OEN_SEL    (BIT(9))
#define GPIO_FUNC24_OEN_SEL_M  (GPIO_FUNC24_OEN_SEL_V << GPIO_FUNC24_OEN_SEL_S)
#define GPIO_FUNC24_OEN_SEL_V  0x00000001U
#define GPIO_FUNC24_OEN_SEL_S  9
/** GPIO_FUNC24_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC24_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC24_OEN_INV_SEL_M  (GPIO_FUNC24_OEN_INV_SEL_V << GPIO_FUNC24_OEN_INV_SEL_S)
#define GPIO_FUNC24_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OEN_INV_SEL_S  10

/** GPIO_FUNC25_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC25_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b8)
/** GPIO_FUNC25_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC25_OUT_SEL    0x000000FFU
#define GPIO_FUNC25_OUT_SEL_M  (GPIO_FUNC25_OUT_SEL_V << GPIO_FUNC25_OUT_SEL_S)
#define GPIO_FUNC25_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC25_OUT_SEL_S  0
/** GPIO_FUNC25_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC25_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC25_OUT_INV_SEL_M  (GPIO_FUNC25_OUT_INV_SEL_V << GPIO_FUNC25_OUT_INV_SEL_S)
#define GPIO_FUNC25_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OUT_INV_SEL_S  8
/** GPIO_FUNC25_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC25_OEN_SEL    (BIT(9))
#define GPIO_FUNC25_OEN_SEL_M  (GPIO_FUNC25_OEN_SEL_V << GPIO_FUNC25_OEN_SEL_S)
#define GPIO_FUNC25_OEN_SEL_V  0x00000001U
#define GPIO_FUNC25_OEN_SEL_S  9
/** GPIO_FUNC25_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC25_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC25_OEN_INV_SEL_M  (GPIO_FUNC25_OEN_INV_SEL_V << GPIO_FUNC25_OEN_INV_SEL_S)
#define GPIO_FUNC25_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OEN_INV_SEL_S  10

/** GPIO_FUNC26_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC26_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5bc)
/** GPIO_FUNC26_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC26_OUT_SEL    0x000000FFU
#define GPIO_FUNC26_OUT_SEL_M  (GPIO_FUNC26_OUT_SEL_V << GPIO_FUNC26_OUT_SEL_S)
#define GPIO_FUNC26_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC26_OUT_SEL_S  0
/** GPIO_FUNC26_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC26_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC26_OUT_INV_SEL_M  (GPIO_FUNC26_OUT_INV_SEL_V << GPIO_FUNC26_OUT_INV_SEL_S)
#define GPIO_FUNC26_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OUT_INV_SEL_S  8
/** GPIO_FUNC26_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC26_OEN_SEL    (BIT(9))
#define GPIO_FUNC26_OEN_SEL_M  (GPIO_FUNC26_OEN_SEL_V << GPIO_FUNC26_OEN_SEL_S)
#define GPIO_FUNC26_OEN_SEL_V  0x00000001U
#define GPIO_FUNC26_OEN_SEL_S  9
/** GPIO_FUNC26_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC26_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC26_OEN_INV_SEL_M  (GPIO_FUNC26_OEN_INV_SEL_V << GPIO_FUNC26_OEN_INV_SEL_S)
#define GPIO_FUNC26_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OEN_INV_SEL_S  10

/** GPIO_FUNC27_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC27_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c0)
/** GPIO_FUNC27_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC27_OUT_SEL    0x000000FFU
#define GPIO_FUNC27_OUT_SEL_M  (GPIO_FUNC27_OUT_SEL_V << GPIO_FUNC27_OUT_SEL_S)
#define GPIO_FUNC27_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC27_OUT_SEL_S  0
/** GPIO_FUNC27_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC27_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC27_OUT_INV_SEL_M  (GPIO_FUNC27_OUT_INV_SEL_V << GPIO_FUNC27_OUT_INV_SEL_S)
#define GPIO_FUNC27_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OUT_INV_SEL_S  8
/** GPIO_FUNC27_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC27_OEN_SEL    (BIT(9))
#define GPIO_FUNC27_OEN_SEL_M  (GPIO_FUNC27_OEN_SEL_V << GPIO_FUNC27_OEN_SEL_S)
#define GPIO_FUNC27_OEN_SEL_V  0x00000001U
#define GPIO_FUNC27_OEN_SEL_S  9
/** GPIO_FUNC27_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC27_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC27_OEN_INV_SEL_M  (GPIO_FUNC27_OEN_INV_SEL_V << GPIO_FUNC27_OEN_INV_SEL_S)
#define GPIO_FUNC27_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OEN_INV_SEL_S  10

/** GPIO_FUNC28_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC28_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c4)
/** GPIO_FUNC28_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC28_OUT_SEL    0x000000FFU
#define GPIO_FUNC28_OUT_SEL_M  (GPIO_FUNC28_OUT_SEL_V << GPIO_FUNC28_OUT_SEL_S)
#define GPIO_FUNC28_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC28_OUT_SEL_S  0
/** GPIO_FUNC28_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC28_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC28_OUT_INV_SEL_M  (GPIO_FUNC28_OUT_INV_SEL_V << GPIO_FUNC28_OUT_INV_SEL_S)
#define GPIO_FUNC28_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OUT_INV_SEL_S  8
/** GPIO_FUNC28_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC28_OEN_SEL    (BIT(9))
#define GPIO_FUNC28_OEN_SEL_M  (GPIO_FUNC28_OEN_SEL_V << GPIO_FUNC28_OEN_SEL_S)
#define GPIO_FUNC28_OEN_SEL_V  0x00000001U
#define GPIO_FUNC28_OEN_SEL_S  9
/** GPIO_FUNC28_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC28_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC28_OEN_INV_SEL_M  (GPIO_FUNC28_OEN_INV_SEL_V << GPIO_FUNC28_OEN_INV_SEL_S)
#define GPIO_FUNC28_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OEN_INV_SEL_S  10

/** GPIO_FUNC29_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC29_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c8)
/** GPIO_FUNC29_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC29_OUT_SEL    0x000000FFU
#define GPIO_FUNC29_OUT_SEL_M  (GPIO_FUNC29_OUT_SEL_V << GPIO_FUNC29_OUT_SEL_S)
#define GPIO_FUNC29_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC29_OUT_SEL_S  0
/** GPIO_FUNC29_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC29_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC29_OUT_INV_SEL_M  (GPIO_FUNC29_OUT_INV_SEL_V << GPIO_FUNC29_OUT_INV_SEL_S)
#define GPIO_FUNC29_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OUT_INV_SEL_S  8
/** GPIO_FUNC29_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC29_OEN_SEL    (BIT(9))
#define GPIO_FUNC29_OEN_SEL_M  (GPIO_FUNC29_OEN_SEL_V << GPIO_FUNC29_OEN_SEL_S)
#define GPIO_FUNC29_OEN_SEL_V  0x00000001U
#define GPIO_FUNC29_OEN_SEL_S  9
/** GPIO_FUNC29_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC29_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC29_OEN_INV_SEL_M  (GPIO_FUNC29_OEN_INV_SEL_V << GPIO_FUNC29_OEN_INV_SEL_S)
#define GPIO_FUNC29_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OEN_INV_SEL_S  10

/** GPIO_FUNC30_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC30_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5cc)
/** GPIO_FUNC30_OUT_SEL : R/W/SC; bitpos: [7:0]; default: 128;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
 *  GPIO_OUT_REG[n].
 */
#define GPIO_FUNC30_OUT_SEL    0x000000FFU
#define GPIO_FUNC30_OUT_SEL_M  (GPIO_FUNC30_OUT_SEL_V << GPIO_FUNC30_OUT_SEL_S)
#define GPIO_FUNC30_OUT_SEL_V  0x000000FFU
#define GPIO_FUNC30_OUT_SEL_S  0
/** GPIO_FUNC30_OUT_INV_SEL : R/W/SC; bitpos: [8]; default: 0;
 *  set this bit to invert output signal.1:invert.0:not invert.
 */
#define GPIO_FUNC30_OUT_INV_SEL    (BIT(8))
#define GPIO_FUNC30_OUT_INV_SEL_M  (GPIO_FUNC30_OUT_INV_SEL_V << GPIO_FUNC30_OUT_INV_SEL_S)
#define GPIO_FUNC30_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OUT_INV_SEL_S  8
/** GPIO_FUNC30_OEN_SEL : R/W; bitpos: [9]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC30_OEN_SEL    (BIT(9))
#define GPIO_FUNC30_OEN_SEL_M  (GPIO_FUNC30_OEN_SEL_V << GPIO_FUNC30_OEN_SEL_S)
#define GPIO_FUNC30_OEN_SEL_V  0x00000001U
#define GPIO_FUNC30_OEN_SEL_S  9
/** GPIO_FUNC30_OEN_INV_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC30_OEN_INV_SEL    (BIT(10))
#define GPIO_FUNC30_OEN_INV_SEL_M  (GPIO_FUNC30_OEN_INV_SEL_V << GPIO_FUNC30_OEN_INV_SEL_S)
#define GPIO_FUNC30_OEN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OEN_INV_SEL_S  10

/** GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define GPIO_CLOCK_GATE_REG (DR_REG_GPIO_BASE + 0x62c)
/** GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  set this bit to enable GPIO clock gate
 */
#define GPIO_CLK_EN    (BIT(0))
#define GPIO_CLK_EN_M  (GPIO_CLK_EN_V << GPIO_CLK_EN_S)
#define GPIO_CLK_EN_V  0x00000001U
#define GPIO_CLK_EN_S  0

/** GPIO_DATE_REG register
 *  GPIO version register
 */
#define GPIO_DATE_REG (DR_REG_GPIO_BASE + 0x6fc)
/** GPIO_DATE : R/W; bitpos: [27:0]; default: 36704512;
 *  version register
 */
#define GPIO_DATE    0x0FFFFFFFU
#define GPIO_DATE_M  (GPIO_DATE_V << GPIO_DATE_S)
#define GPIO_DATE_V  0x0FFFFFFFU
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
