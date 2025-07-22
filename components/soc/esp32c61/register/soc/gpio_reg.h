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

/** GPIO_STRAP_REG register
 *  pad strapping register
 */
#define GPIO_STRAP_REG (DR_REG_GPIO_BASE + 0x0)
/** GPIO_STRAPPING : RO; bitpos: [15:0]; default: 0;
 *  pad strapping register
 */
#define GPIO_STRAPPING    0x0000FFFFU
#define GPIO_STRAPPING_M  (GPIO_STRAPPING_V << GPIO_STRAPPING_S)
#define GPIO_STRAPPING_V  0x0000FFFFU
#define GPIO_STRAPPING_S  0

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
 *  GPIO output register for GPIO32-33
 */
#define GPIO_OUT1_REG (DR_REG_GPIO_BASE + 0x10)
/** GPIO_OUT1_DATA_ORIG : R/W/SC/WTC; bitpos: [1:0]; default: 0;
 *  GPIO output register for GPIO32-33
 */
#define GPIO_OUT1_DATA_ORIG    0x00000003U
#define GPIO_OUT1_DATA_ORIG_M  (GPIO_OUT1_DATA_ORIG_V << GPIO_OUT1_DATA_ORIG_S)
#define GPIO_OUT1_DATA_ORIG_V  0x00000003U
#define GPIO_OUT1_DATA_ORIG_S  0

/** GPIO_OUT1_W1TS_REG register
 *  GPIO output set register for GPIO32-33
 */
#define GPIO_OUT1_W1TS_REG (DR_REG_GPIO_BASE + 0x14)
/** GPIO_OUT1_W1TS : WT; bitpos: [1:0]; default: 0;
 *  GPIO output set register for GPIO32-33
 */
#define GPIO_OUT1_W1TS    0x00000003U
#define GPIO_OUT1_W1TS_M  (GPIO_OUT1_W1TS_V << GPIO_OUT1_W1TS_S)
#define GPIO_OUT1_W1TS_V  0x00000003U
#define GPIO_OUT1_W1TS_S  0

/** GPIO_OUT1_W1TC_REG register
 *  GPIO output clear register for GPIO32-33
 */
#define GPIO_OUT1_W1TC_REG (DR_REG_GPIO_BASE + 0x18)
/** GPIO_OUT1_W1TC : WT; bitpos: [1:0]; default: 0;
 *  GPIO output clear register for GPIO32-33
 */
#define GPIO_OUT1_W1TC    0x00000003U
#define GPIO_OUT1_W1TC_M  (GPIO_OUT1_W1TC_V << GPIO_OUT1_W1TC_S)
#define GPIO_OUT1_W1TC_V  0x00000003U
#define GPIO_OUT1_W1TC_S  0

/** GPIO_ENABLE_REG register
 *  GPIO output enable register for GPIO0-31
 */
#define GPIO_ENABLE_REG (DR_REG_GPIO_BASE + 0x34)
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
#define GPIO_ENABLE_W1TS_REG (DR_REG_GPIO_BASE + 0x38)
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
#define GPIO_ENABLE_W1TC_REG (DR_REG_GPIO_BASE + 0x3c)
/** GPIO_ENABLE_W1TC : WT; bitpos: [31:0]; default: 0;
 *  GPIO output enable clear register for GPIO0-31
 */
#define GPIO_ENABLE_W1TC    0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_M  (GPIO_ENABLE_W1TC_V << GPIO_ENABLE_W1TC_S)
#define GPIO_ENABLE_W1TC_V  0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_S  0

/** GPIO_ENABLE1_REG register
 *  GPIO output enable register for GPIO32-33
 */
#define GPIO_ENABLE1_REG (DR_REG_GPIO_BASE + 0x40)
/** GPIO_ENABLE1_DATA : R/W/WTC; bitpos: [1:0]; default: 0;
 *  GPIO output enable register for GPIO32-33
 */
#define GPIO_ENABLE1_DATA    0x00000003U
#define GPIO_ENABLE1_DATA_M  (GPIO_ENABLE1_DATA_V << GPIO_ENABLE1_DATA_S)
#define GPIO_ENABLE1_DATA_V  0x00000003U
#define GPIO_ENABLE1_DATA_S  0

/** GPIO_ENABLE1_W1TS_REG register
 *  GPIO output enable set register for GPIO32-33
 */
#define GPIO_ENABLE1_W1TS_REG (DR_REG_GPIO_BASE + 0x44)
/** GPIO_ENABLE1_W1TS : WT; bitpos: [1:0]; default: 0;
 *  GPIO output enable set register for GPIO32-33
 */
#define GPIO_ENABLE1_W1TS    0x00000003U
#define GPIO_ENABLE1_W1TS_M  (GPIO_ENABLE1_W1TS_V << GPIO_ENABLE1_W1TS_S)
#define GPIO_ENABLE1_W1TS_V  0x00000003U
#define GPIO_ENABLE1_W1TS_S  0

/** GPIO_ENABLE1_W1TC_REG register
 *  GPIO output enable clear register for GPIO32-33
 */
#define GPIO_ENABLE1_W1TC_REG (DR_REG_GPIO_BASE + 0x48)
/** GPIO_ENABLE1_W1TC : WT; bitpos: [1:0]; default: 0;
 *  GPIO output enable clear register for GPIO32-33
 */
#define GPIO_ENABLE1_W1TC    0x00000003U
#define GPIO_ENABLE1_W1TC_M  (GPIO_ENABLE1_W1TC_V << GPIO_ENABLE1_W1TC_S)
#define GPIO_ENABLE1_W1TC_V  0x00000003U
#define GPIO_ENABLE1_W1TC_S  0

/** GPIO_IN_REG register
 *  GPIO input register for GPIO0-31
 */
#define GPIO_IN_REG (DR_REG_GPIO_BASE + 0x64)
/** GPIO_IN_DATA_NEXT : RO; bitpos: [31:0]; default: 0;
 *  GPIO input register for GPIO0-31
 */
#define GPIO_IN_DATA_NEXT    0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_M  (GPIO_IN_DATA_NEXT_V << GPIO_IN_DATA_NEXT_S)
#define GPIO_IN_DATA_NEXT_V  0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_S  0

/** GPIO_IN1_REG register
 *  GPIO input register for GPIO32-33
 */
#define GPIO_IN1_REG (DR_REG_GPIO_BASE + 0x68)
/** GPIO_IN1_DATA_NEXT : RO; bitpos: [1:0]; default: 0;
 *  GPIO input register for GPIO32-33
 */
#define GPIO_IN1_DATA_NEXT    0x00000003U
#define GPIO_IN1_DATA_NEXT_M  (GPIO_IN1_DATA_NEXT_V << GPIO_IN1_DATA_NEXT_S)
#define GPIO_IN1_DATA_NEXT_V  0x00000003U
#define GPIO_IN1_DATA_NEXT_S  0

/** GPIO_STATUS_REG register
 *  GPIO interrupt status register for GPIO0-31
 */
#define GPIO_STATUS_REG (DR_REG_GPIO_BASE + 0x74)
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
#define GPIO_STATUS_W1TS_REG (DR_REG_GPIO_BASE + 0x78)
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
#define GPIO_STATUS_W1TC_REG (DR_REG_GPIO_BASE + 0x7c)
/** GPIO_STATUS_W1TC : WT; bitpos: [31:0]; default: 0;
 *  GPIO interrupt status clear register for GPIO0-31
 */
#define GPIO_STATUS_W1TC    0xFFFFFFFFU
#define GPIO_STATUS_W1TC_M  (GPIO_STATUS_W1TC_V << GPIO_STATUS_W1TC_S)
#define GPIO_STATUS_W1TC_V  0xFFFFFFFFU
#define GPIO_STATUS_W1TC_S  0

/** GPIO_STATUS1_REG register
 *  GPIO interrupt status register for GPIO32-33
 */
#define GPIO_STATUS1_REG (DR_REG_GPIO_BASE + 0x80)
/** GPIO_STATUS1_INTERRUPT : R/W/WTC; bitpos: [1:0]; default: 0;
 *  GPIO interrupt status register for GPIO32-33
 */
#define GPIO_STATUS1_INTERRUPT    0x00000003U
#define GPIO_STATUS1_INTERRUPT_M  (GPIO_STATUS1_INTERRUPT_V << GPIO_STATUS1_INTERRUPT_S)
#define GPIO_STATUS1_INTERRUPT_V  0x00000003U
#define GPIO_STATUS1_INTERRUPT_S  0

/** GPIO_STATUS1_W1TS_REG register
 *  GPIO interrupt status set register for GPIO32-33
 */
#define GPIO_STATUS1_W1TS_REG (DR_REG_GPIO_BASE + 0x84)
/** GPIO_STATUS1_W1TS : WT; bitpos: [1:0]; default: 0;
 *  GPIO interrupt status set register for GPIO32-33
 */
#define GPIO_STATUS1_W1TS    0x00000003U
#define GPIO_STATUS1_W1TS_M  (GPIO_STATUS1_W1TS_V << GPIO_STATUS1_W1TS_S)
#define GPIO_STATUS1_W1TS_V  0x00000003U
#define GPIO_STATUS1_W1TS_S  0

/** GPIO_STATUS1_W1TC_REG register
 *  GPIO interrupt status clear register for GPIO32-33
 */
#define GPIO_STATUS1_W1TC_REG (DR_REG_GPIO_BASE + 0x88)
/** GPIO_STATUS1_W1TC : WT; bitpos: [1:0]; default: 0;
 *  GPIO interrupt status clear register for GPIO32-33
 */
#define GPIO_STATUS1_W1TC    0x00000003U
#define GPIO_STATUS1_W1TC_M  (GPIO_STATUS1_W1TC_V << GPIO_STATUS1_W1TC_S)
#define GPIO_STATUS1_W1TC_V  0x00000003U
#define GPIO_STATUS1_W1TC_S  0

/** GPIO_PROCPU_INT_REG register
 *  GPIO_PROCPU_INT interrupt status register for GPIO0-31
 */
#define GPIO_PROCPU_INT_REG (DR_REG_GPIO_BASE + 0xa4)
/** GPIO_PROCPU_INT : RO; bitpos: [31:0]; default: 0;
 *  GPIO_PROCPU_INT interrupt status register for GPIO0-31
 */
#define GPIO_PROCPU_INT    0xFFFFFFFFU
#define GPIO_PROCPU_INT_M  (GPIO_PROCPU_INT_V << GPIO_PROCPU_INT_S)
#define GPIO_PROCPU_INT_V  0xFFFFFFFFU
#define GPIO_PROCPU_INT_S  0

/** GPIO_SDIO_INT_REG register
 *  GPIO_SDIO_INT interrupt status register for GPIO0-31
 */
#define GPIO_SDIO_INT_REG (DR_REG_GPIO_BASE + 0xa8)
/** GPIO_SDIO_INT : RO; bitpos: [31:0]; default: 0;
 *  GPIO_SDIO_INT interrupt status register for GPIO0-31
 */
#define GPIO_SDIO_INT    0xFFFFFFFFU
#define GPIO_SDIO_INT_M  (GPIO_SDIO_INT_V << GPIO_SDIO_INT_S)
#define GPIO_SDIO_INT_V  0xFFFFFFFFU
#define GPIO_SDIO_INT_S  0

/** GPIO_PROCPU_INT1_REG register
 *  GPIO_PROCPU_INT interrupt status register for GPIO32-33
 */
#define GPIO_PROCPU_INT1_REG (DR_REG_GPIO_BASE + 0xac)
/** GPIO_PROCPU_INT1 : RO; bitpos: [1:0]; default: 0;
 *  GPIO_PROCPU_INT interrupt status register for GPIO32-33
 */
#define GPIO_PROCPU_INT1    0x00000003U
#define GPIO_PROCPU_INT1_M  (GPIO_PROCPU_INT1_V << GPIO_PROCPU_INT1_S)
#define GPIO_PROCPU_INT1_V  0x00000003U
#define GPIO_PROCPU_INT1_S  0

/** GPIO_SDIO_INT1_REG register
 *  GPIO_SDIO_INT interrupt status register for GPIO32-33
 */
#define GPIO_SDIO_INT1_REG (DR_REG_GPIO_BASE + 0xb0)
/** GPIO_SDIO_INT1 : RO; bitpos: [1:0]; default: 0;
 *  GPIO_SDIO_INT interrupt status register for GPIO32-33
 */
#define GPIO_SDIO_INT1    0x00000003U
#define GPIO_SDIO_INT1_M  (GPIO_SDIO_INT1_V << GPIO_SDIO_INT1_S)
#define GPIO_SDIO_INT1_V  0x00000003U
#define GPIO_SDIO_INT1_S  0

/** GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register for GPIO0-31
 */
#define GPIO_STATUS_NEXT_REG (DR_REG_GPIO_BASE + 0xc4)
/** GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [31:0]; default: 0;
 *  GPIO interrupt source register for GPIO0-31
 */
#define GPIO_STATUS_INTERRUPT_NEXT    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_M  (GPIO_STATUS_INTERRUPT_NEXT_V << GPIO_STATUS_INTERRUPT_NEXT_S)
#define GPIO_STATUS_INTERRUPT_NEXT_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

/** GPIO_STATUS_NEXT1_REG register
 *  GPIO interrupt source register for GPIO32-33
 */
#define GPIO_STATUS_NEXT1_REG (DR_REG_GPIO_BASE + 0xc8)
/** GPIO_STATUS_INTERRUPT_NEXT1 : RO; bitpos: [1:0]; default: 0;
 *  GPIO interrupt source register for GPIO32-33
 */
#define GPIO_STATUS_INTERRUPT_NEXT1    0x00000003U
#define GPIO_STATUS_INTERRUPT_NEXT1_M  (GPIO_STATUS_INTERRUPT_NEXT1_V << GPIO_STATUS_INTERRUPT_NEXT1_S)
#define GPIO_STATUS_INTERRUPT_NEXT1_V  0x00000003U
#define GPIO_STATUS_INTERRUPT_NEXT1_S  0

/** GPIO_PIN0_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN0_REG (DR_REG_GPIO_BASE + 0xd4)
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
/** GPIO_PIN0_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN0_INT_ENA    0x0000001FU
#define GPIO_PIN0_INT_ENA_M  (GPIO_PIN0_INT_ENA_V << GPIO_PIN0_INT_ENA_S)
#define GPIO_PIN0_INT_ENA_V  0x0000001FU
#define GPIO_PIN0_INT_ENA_S  13

/** GPIO_PIN1_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN1_REG (DR_REG_GPIO_BASE + 0xd8)
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
/** GPIO_PIN1_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN1_INT_ENA    0x0000001FU
#define GPIO_PIN1_INT_ENA_M  (GPIO_PIN1_INT_ENA_V << GPIO_PIN1_INT_ENA_S)
#define GPIO_PIN1_INT_ENA_V  0x0000001FU
#define GPIO_PIN1_INT_ENA_S  13

/** GPIO_PIN2_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN2_REG (DR_REG_GPIO_BASE + 0xdc)
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
/** GPIO_PIN2_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN2_INT_ENA    0x0000001FU
#define GPIO_PIN2_INT_ENA_M  (GPIO_PIN2_INT_ENA_V << GPIO_PIN2_INT_ENA_S)
#define GPIO_PIN2_INT_ENA_V  0x0000001FU
#define GPIO_PIN2_INT_ENA_S  13

/** GPIO_PIN3_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN3_REG (DR_REG_GPIO_BASE + 0xe0)
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
/** GPIO_PIN3_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN3_INT_ENA    0x0000001FU
#define GPIO_PIN3_INT_ENA_M  (GPIO_PIN3_INT_ENA_V << GPIO_PIN3_INT_ENA_S)
#define GPIO_PIN3_INT_ENA_V  0x0000001FU
#define GPIO_PIN3_INT_ENA_S  13

/** GPIO_PIN4_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN4_REG (DR_REG_GPIO_BASE + 0xe4)
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
/** GPIO_PIN4_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN4_INT_ENA    0x0000001FU
#define GPIO_PIN4_INT_ENA_M  (GPIO_PIN4_INT_ENA_V << GPIO_PIN4_INT_ENA_S)
#define GPIO_PIN4_INT_ENA_V  0x0000001FU
#define GPIO_PIN4_INT_ENA_S  13

/** GPIO_PIN5_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN5_REG (DR_REG_GPIO_BASE + 0xe8)
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
/** GPIO_PIN5_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN5_INT_ENA    0x0000001FU
#define GPIO_PIN5_INT_ENA_M  (GPIO_PIN5_INT_ENA_V << GPIO_PIN5_INT_ENA_S)
#define GPIO_PIN5_INT_ENA_V  0x0000001FU
#define GPIO_PIN5_INT_ENA_S  13

/** GPIO_PIN6_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN6_REG (DR_REG_GPIO_BASE + 0xec)
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
/** GPIO_PIN6_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN6_INT_ENA    0x0000001FU
#define GPIO_PIN6_INT_ENA_M  (GPIO_PIN6_INT_ENA_V << GPIO_PIN6_INT_ENA_S)
#define GPIO_PIN6_INT_ENA_V  0x0000001FU
#define GPIO_PIN6_INT_ENA_S  13

/** GPIO_PIN7_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN7_REG (DR_REG_GPIO_BASE + 0xf0)
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
/** GPIO_PIN7_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN7_INT_ENA    0x0000001FU
#define GPIO_PIN7_INT_ENA_M  (GPIO_PIN7_INT_ENA_V << GPIO_PIN7_INT_ENA_S)
#define GPIO_PIN7_INT_ENA_V  0x0000001FU
#define GPIO_PIN7_INT_ENA_S  13

/** GPIO_PIN8_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN8_REG (DR_REG_GPIO_BASE + 0xf4)
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
/** GPIO_PIN8_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN8_INT_ENA    0x0000001FU
#define GPIO_PIN8_INT_ENA_M  (GPIO_PIN8_INT_ENA_V << GPIO_PIN8_INT_ENA_S)
#define GPIO_PIN8_INT_ENA_V  0x0000001FU
#define GPIO_PIN8_INT_ENA_S  13

/** GPIO_PIN9_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN9_REG (DR_REG_GPIO_BASE + 0xf8)
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
/** GPIO_PIN9_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN9_INT_ENA    0x0000001FU
#define GPIO_PIN9_INT_ENA_M  (GPIO_PIN9_INT_ENA_V << GPIO_PIN9_INT_ENA_S)
#define GPIO_PIN9_INT_ENA_V  0x0000001FU
#define GPIO_PIN9_INT_ENA_S  13

/** GPIO_PIN10_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN10_REG (DR_REG_GPIO_BASE + 0xfc)
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
/** GPIO_PIN10_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN10_INT_ENA    0x0000001FU
#define GPIO_PIN10_INT_ENA_M  (GPIO_PIN10_INT_ENA_V << GPIO_PIN10_INT_ENA_S)
#define GPIO_PIN10_INT_ENA_V  0x0000001FU
#define GPIO_PIN10_INT_ENA_S  13

/** GPIO_PIN11_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN11_REG (DR_REG_GPIO_BASE + 0x100)
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
/** GPIO_PIN11_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN11_INT_ENA    0x0000001FU
#define GPIO_PIN11_INT_ENA_M  (GPIO_PIN11_INT_ENA_V << GPIO_PIN11_INT_ENA_S)
#define GPIO_PIN11_INT_ENA_V  0x0000001FU
#define GPIO_PIN11_INT_ENA_S  13

/** GPIO_PIN12_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN12_REG (DR_REG_GPIO_BASE + 0x104)
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
/** GPIO_PIN12_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN12_INT_ENA    0x0000001FU
#define GPIO_PIN12_INT_ENA_M  (GPIO_PIN12_INT_ENA_V << GPIO_PIN12_INT_ENA_S)
#define GPIO_PIN12_INT_ENA_V  0x0000001FU
#define GPIO_PIN12_INT_ENA_S  13

/** GPIO_PIN13_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN13_REG (DR_REG_GPIO_BASE + 0x108)
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
/** GPIO_PIN13_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN13_INT_ENA    0x0000001FU
#define GPIO_PIN13_INT_ENA_M  (GPIO_PIN13_INT_ENA_V << GPIO_PIN13_INT_ENA_S)
#define GPIO_PIN13_INT_ENA_V  0x0000001FU
#define GPIO_PIN13_INT_ENA_S  13

/** GPIO_PIN14_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN14_REG (DR_REG_GPIO_BASE + 0x10c)
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
/** GPIO_PIN14_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN14_INT_ENA    0x0000001FU
#define GPIO_PIN14_INT_ENA_M  (GPIO_PIN14_INT_ENA_V << GPIO_PIN14_INT_ENA_S)
#define GPIO_PIN14_INT_ENA_V  0x0000001FU
#define GPIO_PIN14_INT_ENA_S  13

/** GPIO_PIN15_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN15_REG (DR_REG_GPIO_BASE + 0x110)
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
/** GPIO_PIN15_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN15_INT_ENA    0x0000001FU
#define GPIO_PIN15_INT_ENA_M  (GPIO_PIN15_INT_ENA_V << GPIO_PIN15_INT_ENA_S)
#define GPIO_PIN15_INT_ENA_V  0x0000001FU
#define GPIO_PIN15_INT_ENA_S  13

/** GPIO_PIN16_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN16_REG (DR_REG_GPIO_BASE + 0x114)
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
/** GPIO_PIN16_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN16_INT_ENA    0x0000001FU
#define GPIO_PIN16_INT_ENA_M  (GPIO_PIN16_INT_ENA_V << GPIO_PIN16_INT_ENA_S)
#define GPIO_PIN16_INT_ENA_V  0x0000001FU
#define GPIO_PIN16_INT_ENA_S  13

/** GPIO_PIN17_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN17_REG (DR_REG_GPIO_BASE + 0x118)
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
/** GPIO_PIN17_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN17_INT_ENA    0x0000001FU
#define GPIO_PIN17_INT_ENA_M  (GPIO_PIN17_INT_ENA_V << GPIO_PIN17_INT_ENA_S)
#define GPIO_PIN17_INT_ENA_V  0x0000001FU
#define GPIO_PIN17_INT_ENA_S  13

/** GPIO_PIN18_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN18_REG (DR_REG_GPIO_BASE + 0x11c)
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
/** GPIO_PIN18_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN18_INT_ENA    0x0000001FU
#define GPIO_PIN18_INT_ENA_M  (GPIO_PIN18_INT_ENA_V << GPIO_PIN18_INT_ENA_S)
#define GPIO_PIN18_INT_ENA_V  0x0000001FU
#define GPIO_PIN18_INT_ENA_S  13

/** GPIO_PIN19_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN19_REG (DR_REG_GPIO_BASE + 0x120)
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
/** GPIO_PIN19_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN19_INT_ENA    0x0000001FU
#define GPIO_PIN19_INT_ENA_M  (GPIO_PIN19_INT_ENA_V << GPIO_PIN19_INT_ENA_S)
#define GPIO_PIN19_INT_ENA_V  0x0000001FU
#define GPIO_PIN19_INT_ENA_S  13

/** GPIO_PIN20_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN20_REG (DR_REG_GPIO_BASE + 0x124)
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
/** GPIO_PIN20_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN20_INT_ENA    0x0000001FU
#define GPIO_PIN20_INT_ENA_M  (GPIO_PIN20_INT_ENA_V << GPIO_PIN20_INT_ENA_S)
#define GPIO_PIN20_INT_ENA_V  0x0000001FU
#define GPIO_PIN20_INT_ENA_S  13

/** GPIO_PIN21_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN21_REG (DR_REG_GPIO_BASE + 0x128)
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
/** GPIO_PIN21_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN21_INT_ENA    0x0000001FU
#define GPIO_PIN21_INT_ENA_M  (GPIO_PIN21_INT_ENA_V << GPIO_PIN21_INT_ENA_S)
#define GPIO_PIN21_INT_ENA_V  0x0000001FU
#define GPIO_PIN21_INT_ENA_S  13

/** GPIO_PIN22_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN22_REG (DR_REG_GPIO_BASE + 0x12c)
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
/** GPIO_PIN22_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN22_INT_ENA    0x0000001FU
#define GPIO_PIN22_INT_ENA_M  (GPIO_PIN22_INT_ENA_V << GPIO_PIN22_INT_ENA_S)
#define GPIO_PIN22_INT_ENA_V  0x0000001FU
#define GPIO_PIN22_INT_ENA_S  13

/** GPIO_PIN23_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN23_REG (DR_REG_GPIO_BASE + 0x130)
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
/** GPIO_PIN23_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN23_INT_ENA    0x0000001FU
#define GPIO_PIN23_INT_ENA_M  (GPIO_PIN23_INT_ENA_V << GPIO_PIN23_INT_ENA_S)
#define GPIO_PIN23_INT_ENA_V  0x0000001FU
#define GPIO_PIN23_INT_ENA_S  13

/** GPIO_PIN24_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN24_REG (DR_REG_GPIO_BASE + 0x134)
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
/** GPIO_PIN24_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN24_INT_ENA    0x0000001FU
#define GPIO_PIN24_INT_ENA_M  (GPIO_PIN24_INT_ENA_V << GPIO_PIN24_INT_ENA_S)
#define GPIO_PIN24_INT_ENA_V  0x0000001FU
#define GPIO_PIN24_INT_ENA_S  13

/** GPIO_PIN25_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN25_REG (DR_REG_GPIO_BASE + 0x138)
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
/** GPIO_PIN25_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN25_INT_ENA    0x0000001FU
#define GPIO_PIN25_INT_ENA_M  (GPIO_PIN25_INT_ENA_V << GPIO_PIN25_INT_ENA_S)
#define GPIO_PIN25_INT_ENA_V  0x0000001FU
#define GPIO_PIN25_INT_ENA_S  13

/** GPIO_PIN26_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN26_REG (DR_REG_GPIO_BASE + 0x13c)
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
/** GPIO_PIN26_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN26_INT_ENA    0x0000001FU
#define GPIO_PIN26_INT_ENA_M  (GPIO_PIN26_INT_ENA_V << GPIO_PIN26_INT_ENA_S)
#define GPIO_PIN26_INT_ENA_V  0x0000001FU
#define GPIO_PIN26_INT_ENA_S  13

/** GPIO_PIN27_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN27_REG (DR_REG_GPIO_BASE + 0x140)
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
/** GPIO_PIN27_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN27_INT_ENA    0x0000001FU
#define GPIO_PIN27_INT_ENA_M  (GPIO_PIN27_INT_ENA_V << GPIO_PIN27_INT_ENA_S)
#define GPIO_PIN27_INT_ENA_V  0x0000001FU
#define GPIO_PIN27_INT_ENA_S  13

/** GPIO_PIN28_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN28_REG (DR_REG_GPIO_BASE + 0x144)
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
/** GPIO_PIN28_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN28_INT_ENA    0x0000001FU
#define GPIO_PIN28_INT_ENA_M  (GPIO_PIN28_INT_ENA_V << GPIO_PIN28_INT_ENA_S)
#define GPIO_PIN28_INT_ENA_V  0x0000001FU
#define GPIO_PIN28_INT_ENA_S  13

/** GPIO_PIN29_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN29_REG (DR_REG_GPIO_BASE + 0x148)
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
/** GPIO_PIN29_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN29_INT_ENA    0x0000001FU
#define GPIO_PIN29_INT_ENA_M  (GPIO_PIN29_INT_ENA_V << GPIO_PIN29_INT_ENA_S)
#define GPIO_PIN29_INT_ENA_V  0x0000001FU
#define GPIO_PIN29_INT_ENA_S  13

/** GPIO_PIN30_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN30_REG (DR_REG_GPIO_BASE + 0x14c)
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
/** GPIO_PIN30_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN30_INT_ENA    0x0000001FU
#define GPIO_PIN30_INT_ENA_M  (GPIO_PIN30_INT_ENA_V << GPIO_PIN30_INT_ENA_S)
#define GPIO_PIN30_INT_ENA_V  0x0000001FU
#define GPIO_PIN30_INT_ENA_S  13

/** GPIO_PIN31_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN31_REG (DR_REG_GPIO_BASE + 0x150)
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
/** GPIO_PIN31_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN31_INT_ENA    0x0000001FU
#define GPIO_PIN31_INT_ENA_M  (GPIO_PIN31_INT_ENA_V << GPIO_PIN31_INT_ENA_S)
#define GPIO_PIN31_INT_ENA_V  0x0000001FU
#define GPIO_PIN31_INT_ENA_S  13

/** GPIO_PIN32_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN32_REG (DR_REG_GPIO_BASE + 0x154)
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
/** GPIO_PIN32_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN32_INT_ENA    0x0000001FU
#define GPIO_PIN32_INT_ENA_M  (GPIO_PIN32_INT_ENA_V << GPIO_PIN32_INT_ENA_S)
#define GPIO_PIN32_INT_ENA_V  0x0000001FU
#define GPIO_PIN32_INT_ENA_S  13

/** GPIO_PIN33_REG register
 *  GPIO pin configuration register
 */
#define GPIO_PIN33_REG (DR_REG_GPIO_BASE + 0x158)
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
/** GPIO_PIN33_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
 */
#define GPIO_PIN33_INT_ENA    0x0000001FU
#define GPIO_PIN33_INT_ENA_M  (GPIO_PIN33_INT_ENA_V << GPIO_PIN33_INT_ENA_S)
#define GPIO_PIN33_INT_ENA_V  0x0000001FU
#define GPIO_PIN33_INT_ENA_S  13

/** GPIO_FUNC0_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC0_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2d4)
/** GPIO_FUNC0_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC0_IN_SEL    0x0000007FU
#define GPIO_FUNC0_IN_SEL_M  (GPIO_FUNC0_IN_SEL_V << GPIO_FUNC0_IN_SEL_S)
#define GPIO_FUNC0_IN_SEL_V  0x0000007FU
#define GPIO_FUNC0_IN_SEL_S  0
/** GPIO_FUNC0_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC0_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC0_IN_INV_SEL_M  (GPIO_FUNC0_IN_INV_SEL_V << GPIO_FUNC0_IN_INV_SEL_S)
#define GPIO_FUNC0_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_IN_INV_SEL_S  7
/** GPIO_SIG0_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG0_IN_SEL    (BIT(8))
#define GPIO_SIG0_IN_SEL_M  (GPIO_SIG0_IN_SEL_V << GPIO_SIG0_IN_SEL_S)
#define GPIO_SIG0_IN_SEL_V  0x00000001U
#define GPIO_SIG0_IN_SEL_S  8

/** GPIO_FUNC6_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC6_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2ec)
/** GPIO_FUNC6_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC6_IN_SEL    0x0000007FU
#define GPIO_FUNC6_IN_SEL_M  (GPIO_FUNC6_IN_SEL_V << GPIO_FUNC6_IN_SEL_S)
#define GPIO_FUNC6_IN_SEL_V  0x0000007FU
#define GPIO_FUNC6_IN_SEL_S  0
/** GPIO_FUNC6_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC6_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC6_IN_INV_SEL_M  (GPIO_FUNC6_IN_INV_SEL_V << GPIO_FUNC6_IN_INV_SEL_S)
#define GPIO_FUNC6_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_IN_INV_SEL_S  7
/** GPIO_SIG6_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG6_IN_SEL    (BIT(8))
#define GPIO_SIG6_IN_SEL_M  (GPIO_SIG6_IN_SEL_V << GPIO_SIG6_IN_SEL_S)
#define GPIO_SIG6_IN_SEL_V  0x00000001U
#define GPIO_SIG6_IN_SEL_S  8

/** GPIO_FUNC7_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC7_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f0)
/** GPIO_FUNC7_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC7_IN_SEL    0x0000007FU
#define GPIO_FUNC7_IN_SEL_M  (GPIO_FUNC7_IN_SEL_V << GPIO_FUNC7_IN_SEL_S)
#define GPIO_FUNC7_IN_SEL_V  0x0000007FU
#define GPIO_FUNC7_IN_SEL_S  0
/** GPIO_FUNC7_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC7_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC7_IN_INV_SEL_M  (GPIO_FUNC7_IN_INV_SEL_V << GPIO_FUNC7_IN_INV_SEL_S)
#define GPIO_FUNC7_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_IN_INV_SEL_S  7
/** GPIO_SIG7_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG7_IN_SEL    (BIT(8))
#define GPIO_SIG7_IN_SEL_M  (GPIO_SIG7_IN_SEL_V << GPIO_SIG7_IN_SEL_S)
#define GPIO_SIG7_IN_SEL_V  0x00000001U
#define GPIO_SIG7_IN_SEL_S  8

/** GPIO_FUNC8_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC8_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f4)
/** GPIO_FUNC8_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC8_IN_SEL    0x0000007FU
#define GPIO_FUNC8_IN_SEL_M  (GPIO_FUNC8_IN_SEL_V << GPIO_FUNC8_IN_SEL_S)
#define GPIO_FUNC8_IN_SEL_V  0x0000007FU
#define GPIO_FUNC8_IN_SEL_S  0
/** GPIO_FUNC8_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC8_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC8_IN_INV_SEL_M  (GPIO_FUNC8_IN_INV_SEL_V << GPIO_FUNC8_IN_INV_SEL_S)
#define GPIO_FUNC8_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_IN_INV_SEL_S  7
/** GPIO_SIG8_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG8_IN_SEL    (BIT(8))
#define GPIO_SIG8_IN_SEL_M  (GPIO_SIG8_IN_SEL_V << GPIO_SIG8_IN_SEL_S)
#define GPIO_SIG8_IN_SEL_V  0x00000001U
#define GPIO_SIG8_IN_SEL_S  8

/** GPIO_FUNC9_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC9_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f8)
/** GPIO_FUNC9_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC9_IN_SEL    0x0000007FU
#define GPIO_FUNC9_IN_SEL_M  (GPIO_FUNC9_IN_SEL_V << GPIO_FUNC9_IN_SEL_S)
#define GPIO_FUNC9_IN_SEL_V  0x0000007FU
#define GPIO_FUNC9_IN_SEL_S  0
/** GPIO_FUNC9_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC9_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC9_IN_INV_SEL_M  (GPIO_FUNC9_IN_INV_SEL_V << GPIO_FUNC9_IN_INV_SEL_S)
#define GPIO_FUNC9_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_IN_INV_SEL_S  7
/** GPIO_SIG9_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG9_IN_SEL    (BIT(8))
#define GPIO_SIG9_IN_SEL_M  (GPIO_SIG9_IN_SEL_V << GPIO_SIG9_IN_SEL_S)
#define GPIO_SIG9_IN_SEL_V  0x00000001U
#define GPIO_SIG9_IN_SEL_S  8

/** GPIO_FUNC10_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC10_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2fc)
/** GPIO_FUNC10_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC10_IN_SEL    0x0000007FU
#define GPIO_FUNC10_IN_SEL_M  (GPIO_FUNC10_IN_SEL_V << GPIO_FUNC10_IN_SEL_S)
#define GPIO_FUNC10_IN_SEL_V  0x0000007FU
#define GPIO_FUNC10_IN_SEL_S  0
/** GPIO_FUNC10_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC10_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC10_IN_INV_SEL_M  (GPIO_FUNC10_IN_INV_SEL_V << GPIO_FUNC10_IN_INV_SEL_S)
#define GPIO_FUNC10_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_IN_INV_SEL_S  7
/** GPIO_SIG10_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG10_IN_SEL    (BIT(8))
#define GPIO_SIG10_IN_SEL_M  (GPIO_SIG10_IN_SEL_V << GPIO_SIG10_IN_SEL_S)
#define GPIO_SIG10_IN_SEL_V  0x00000001U
#define GPIO_SIG10_IN_SEL_S  8

/** GPIO_FUNC11_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC11_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x300)
/** GPIO_FUNC11_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC11_IN_SEL    0x0000007FU
#define GPIO_FUNC11_IN_SEL_M  (GPIO_FUNC11_IN_SEL_V << GPIO_FUNC11_IN_SEL_S)
#define GPIO_FUNC11_IN_SEL_V  0x0000007FU
#define GPIO_FUNC11_IN_SEL_S  0
/** GPIO_FUNC11_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC11_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC11_IN_INV_SEL_M  (GPIO_FUNC11_IN_INV_SEL_V << GPIO_FUNC11_IN_INV_SEL_S)
#define GPIO_FUNC11_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_IN_INV_SEL_S  7
/** GPIO_SIG11_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG11_IN_SEL    (BIT(8))
#define GPIO_SIG11_IN_SEL_M  (GPIO_SIG11_IN_SEL_V << GPIO_SIG11_IN_SEL_S)
#define GPIO_SIG11_IN_SEL_V  0x00000001U
#define GPIO_SIG11_IN_SEL_S  8

/** GPIO_FUNC12_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC12_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x304)
/** GPIO_FUNC12_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC12_IN_SEL    0x0000007FU
#define GPIO_FUNC12_IN_SEL_M  (GPIO_FUNC12_IN_SEL_V << GPIO_FUNC12_IN_SEL_S)
#define GPIO_FUNC12_IN_SEL_V  0x0000007FU
#define GPIO_FUNC12_IN_SEL_S  0
/** GPIO_FUNC12_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC12_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC12_IN_INV_SEL_M  (GPIO_FUNC12_IN_INV_SEL_V << GPIO_FUNC12_IN_INV_SEL_S)
#define GPIO_FUNC12_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_IN_INV_SEL_S  7
/** GPIO_SIG12_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG12_IN_SEL    (BIT(8))
#define GPIO_SIG12_IN_SEL_M  (GPIO_SIG12_IN_SEL_V << GPIO_SIG12_IN_SEL_S)
#define GPIO_SIG12_IN_SEL_V  0x00000001U
#define GPIO_SIG12_IN_SEL_S  8

/** GPIO_FUNC13_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC13_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x308)
/** GPIO_FUNC13_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC13_IN_SEL    0x0000007FU
#define GPIO_FUNC13_IN_SEL_M  (GPIO_FUNC13_IN_SEL_V << GPIO_FUNC13_IN_SEL_S)
#define GPIO_FUNC13_IN_SEL_V  0x0000007FU
#define GPIO_FUNC13_IN_SEL_S  0
/** GPIO_FUNC13_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC13_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC13_IN_INV_SEL_M  (GPIO_FUNC13_IN_INV_SEL_V << GPIO_FUNC13_IN_INV_SEL_S)
#define GPIO_FUNC13_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_IN_INV_SEL_S  7
/** GPIO_SIG13_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG13_IN_SEL    (BIT(8))
#define GPIO_SIG13_IN_SEL_M  (GPIO_SIG13_IN_SEL_V << GPIO_SIG13_IN_SEL_S)
#define GPIO_SIG13_IN_SEL_V  0x00000001U
#define GPIO_SIG13_IN_SEL_S  8

/** GPIO_FUNC14_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC14_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x30c)
/** GPIO_FUNC14_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC14_IN_SEL    0x0000007FU
#define GPIO_FUNC14_IN_SEL_M  (GPIO_FUNC14_IN_SEL_V << GPIO_FUNC14_IN_SEL_S)
#define GPIO_FUNC14_IN_SEL_V  0x0000007FU
#define GPIO_FUNC14_IN_SEL_S  0
/** GPIO_FUNC14_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC14_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC14_IN_INV_SEL_M  (GPIO_FUNC14_IN_INV_SEL_V << GPIO_FUNC14_IN_INV_SEL_S)
#define GPIO_FUNC14_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_IN_INV_SEL_S  7
/** GPIO_SIG14_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG14_IN_SEL    (BIT(8))
#define GPIO_SIG14_IN_SEL_M  (GPIO_SIG14_IN_SEL_V << GPIO_SIG14_IN_SEL_S)
#define GPIO_SIG14_IN_SEL_V  0x00000001U
#define GPIO_SIG14_IN_SEL_S  8

/** GPIO_FUNC15_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC15_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x310)
/** GPIO_FUNC15_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC15_IN_SEL    0x0000007FU
#define GPIO_FUNC15_IN_SEL_M  (GPIO_FUNC15_IN_SEL_V << GPIO_FUNC15_IN_SEL_S)
#define GPIO_FUNC15_IN_SEL_V  0x0000007FU
#define GPIO_FUNC15_IN_SEL_S  0
/** GPIO_FUNC15_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC15_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC15_IN_INV_SEL_M  (GPIO_FUNC15_IN_INV_SEL_V << GPIO_FUNC15_IN_INV_SEL_S)
#define GPIO_FUNC15_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_IN_INV_SEL_S  7
/** GPIO_SIG15_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG15_IN_SEL    (BIT(8))
#define GPIO_SIG15_IN_SEL_M  (GPIO_SIG15_IN_SEL_V << GPIO_SIG15_IN_SEL_S)
#define GPIO_SIG15_IN_SEL_V  0x00000001U
#define GPIO_SIG15_IN_SEL_S  8

/** GPIO_FUNC16_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC16_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x314)
/** GPIO_FUNC16_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC16_IN_SEL    0x0000007FU
#define GPIO_FUNC16_IN_SEL_M  (GPIO_FUNC16_IN_SEL_V << GPIO_FUNC16_IN_SEL_S)
#define GPIO_FUNC16_IN_SEL_V  0x0000007FU
#define GPIO_FUNC16_IN_SEL_S  0
/** GPIO_FUNC16_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC16_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC16_IN_INV_SEL_M  (GPIO_FUNC16_IN_INV_SEL_V << GPIO_FUNC16_IN_INV_SEL_S)
#define GPIO_FUNC16_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_IN_INV_SEL_S  7
/** GPIO_SIG16_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG16_IN_SEL    (BIT(8))
#define GPIO_SIG16_IN_SEL_M  (GPIO_SIG16_IN_SEL_V << GPIO_SIG16_IN_SEL_S)
#define GPIO_SIG16_IN_SEL_V  0x00000001U
#define GPIO_SIG16_IN_SEL_S  8

/** GPIO_FUNC17_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC17_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x318)
/** GPIO_FUNC17_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC17_IN_SEL    0x0000007FU
#define GPIO_FUNC17_IN_SEL_M  (GPIO_FUNC17_IN_SEL_V << GPIO_FUNC17_IN_SEL_S)
#define GPIO_FUNC17_IN_SEL_V  0x0000007FU
#define GPIO_FUNC17_IN_SEL_S  0
/** GPIO_FUNC17_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC17_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC17_IN_INV_SEL_M  (GPIO_FUNC17_IN_INV_SEL_V << GPIO_FUNC17_IN_INV_SEL_S)
#define GPIO_FUNC17_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_IN_INV_SEL_S  7
/** GPIO_SIG17_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG17_IN_SEL    (BIT(8))
#define GPIO_SIG17_IN_SEL_M  (GPIO_SIG17_IN_SEL_V << GPIO_SIG17_IN_SEL_S)
#define GPIO_SIG17_IN_SEL_V  0x00000001U
#define GPIO_SIG17_IN_SEL_S  8

/** GPIO_FUNC27_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC27_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x340)
/** GPIO_FUNC27_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC27_IN_SEL    0x0000007FU
#define GPIO_FUNC27_IN_SEL_M  (GPIO_FUNC27_IN_SEL_V << GPIO_FUNC27_IN_SEL_S)
#define GPIO_FUNC27_IN_SEL_V  0x0000007FU
#define GPIO_FUNC27_IN_SEL_S  0
/** GPIO_FUNC27_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC27_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC27_IN_INV_SEL_M  (GPIO_FUNC27_IN_INV_SEL_V << GPIO_FUNC27_IN_INV_SEL_S)
#define GPIO_FUNC27_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_IN_INV_SEL_S  7
/** GPIO_SIG27_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG27_IN_SEL    (BIT(8))
#define GPIO_SIG27_IN_SEL_M  (GPIO_SIG27_IN_SEL_V << GPIO_SIG27_IN_SEL_S)
#define GPIO_SIG27_IN_SEL_V  0x00000001U
#define GPIO_SIG27_IN_SEL_S  8

/** GPIO_FUNC28_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC28_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x344)
/** GPIO_FUNC28_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC28_IN_SEL    0x0000007FU
#define GPIO_FUNC28_IN_SEL_M  (GPIO_FUNC28_IN_SEL_V << GPIO_FUNC28_IN_SEL_S)
#define GPIO_FUNC28_IN_SEL_V  0x0000007FU
#define GPIO_FUNC28_IN_SEL_S  0
/** GPIO_FUNC28_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC28_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC28_IN_INV_SEL_M  (GPIO_FUNC28_IN_INV_SEL_V << GPIO_FUNC28_IN_INV_SEL_S)
#define GPIO_FUNC28_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_IN_INV_SEL_S  7
/** GPIO_SIG28_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG28_IN_SEL    (BIT(8))
#define GPIO_SIG28_IN_SEL_M  (GPIO_SIG28_IN_SEL_V << GPIO_SIG28_IN_SEL_S)
#define GPIO_SIG28_IN_SEL_V  0x00000001U
#define GPIO_SIG28_IN_SEL_S  8

/** GPIO_FUNC29_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC29_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x348)
/** GPIO_FUNC29_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC29_IN_SEL    0x0000007FU
#define GPIO_FUNC29_IN_SEL_M  (GPIO_FUNC29_IN_SEL_V << GPIO_FUNC29_IN_SEL_S)
#define GPIO_FUNC29_IN_SEL_V  0x0000007FU
#define GPIO_FUNC29_IN_SEL_S  0
/** GPIO_FUNC29_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC29_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC29_IN_INV_SEL_M  (GPIO_FUNC29_IN_INV_SEL_V << GPIO_FUNC29_IN_INV_SEL_S)
#define GPIO_FUNC29_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_IN_INV_SEL_S  7
/** GPIO_SIG29_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG29_IN_SEL    (BIT(8))
#define GPIO_SIG29_IN_SEL_M  (GPIO_SIG29_IN_SEL_V << GPIO_SIG29_IN_SEL_S)
#define GPIO_SIG29_IN_SEL_V  0x00000001U
#define GPIO_SIG29_IN_SEL_S  8

/** GPIO_FUNC30_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC30_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x34c)
/** GPIO_FUNC30_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC30_IN_SEL    0x0000007FU
#define GPIO_FUNC30_IN_SEL_M  (GPIO_FUNC30_IN_SEL_V << GPIO_FUNC30_IN_SEL_S)
#define GPIO_FUNC30_IN_SEL_V  0x0000007FU
#define GPIO_FUNC30_IN_SEL_S  0
/** GPIO_FUNC30_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC30_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC30_IN_INV_SEL_M  (GPIO_FUNC30_IN_INV_SEL_V << GPIO_FUNC30_IN_INV_SEL_S)
#define GPIO_FUNC30_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_IN_INV_SEL_S  7
/** GPIO_SIG30_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG30_IN_SEL    (BIT(8))
#define GPIO_SIG30_IN_SEL_M  (GPIO_SIG30_IN_SEL_V << GPIO_SIG30_IN_SEL_S)
#define GPIO_SIG30_IN_SEL_V  0x00000001U
#define GPIO_SIG30_IN_SEL_S  8

/** GPIO_FUNC31_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC31_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x350)
/** GPIO_FUNC31_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC31_IN_SEL    0x0000007FU
#define GPIO_FUNC31_IN_SEL_M  (GPIO_FUNC31_IN_SEL_V << GPIO_FUNC31_IN_SEL_S)
#define GPIO_FUNC31_IN_SEL_V  0x0000007FU
#define GPIO_FUNC31_IN_SEL_S  0
/** GPIO_FUNC31_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC31_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC31_IN_INV_SEL_M  (GPIO_FUNC31_IN_INV_SEL_V << GPIO_FUNC31_IN_INV_SEL_S)
#define GPIO_FUNC31_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_IN_INV_SEL_S  7
/** GPIO_SIG31_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG31_IN_SEL    (BIT(8))
#define GPIO_SIG31_IN_SEL_M  (GPIO_SIG31_IN_SEL_V << GPIO_SIG31_IN_SEL_S)
#define GPIO_SIG31_IN_SEL_V  0x00000001U
#define GPIO_SIG31_IN_SEL_S  8

/** GPIO_FUNC32_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC32_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x354)
/** GPIO_FUNC32_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC32_IN_SEL    0x0000007FU
#define GPIO_FUNC32_IN_SEL_M  (GPIO_FUNC32_IN_SEL_V << GPIO_FUNC32_IN_SEL_S)
#define GPIO_FUNC32_IN_SEL_V  0x0000007FU
#define GPIO_FUNC32_IN_SEL_S  0
/** GPIO_FUNC32_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC32_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC32_IN_INV_SEL_M  (GPIO_FUNC32_IN_INV_SEL_V << GPIO_FUNC32_IN_INV_SEL_S)
#define GPIO_FUNC32_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_IN_INV_SEL_S  7
/** GPIO_SIG32_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG32_IN_SEL    (BIT(8))
#define GPIO_SIG32_IN_SEL_M  (GPIO_SIG32_IN_SEL_V << GPIO_SIG32_IN_SEL_S)
#define GPIO_SIG32_IN_SEL_V  0x00000001U
#define GPIO_SIG32_IN_SEL_S  8

/** GPIO_FUNC33_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC33_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x358)
/** GPIO_FUNC33_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC33_IN_SEL    0x0000007FU
#define GPIO_FUNC33_IN_SEL_M  (GPIO_FUNC33_IN_SEL_V << GPIO_FUNC33_IN_SEL_S)
#define GPIO_FUNC33_IN_SEL_V  0x0000007FU
#define GPIO_FUNC33_IN_SEL_S  0
/** GPIO_FUNC33_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC33_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC33_IN_INV_SEL_M  (GPIO_FUNC33_IN_INV_SEL_V << GPIO_FUNC33_IN_INV_SEL_S)
#define GPIO_FUNC33_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_IN_INV_SEL_S  7
/** GPIO_SIG33_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG33_IN_SEL    (BIT(8))
#define GPIO_SIG33_IN_SEL_M  (GPIO_SIG33_IN_SEL_V << GPIO_SIG33_IN_SEL_S)
#define GPIO_SIG33_IN_SEL_V  0x00000001U
#define GPIO_SIG33_IN_SEL_S  8

/** GPIO_FUNC34_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC34_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x35c)
/** GPIO_FUNC34_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC34_IN_SEL    0x0000007FU
#define GPIO_FUNC34_IN_SEL_M  (GPIO_FUNC34_IN_SEL_V << GPIO_FUNC34_IN_SEL_S)
#define GPIO_FUNC34_IN_SEL_V  0x0000007FU
#define GPIO_FUNC34_IN_SEL_S  0
/** GPIO_FUNC34_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC34_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC34_IN_INV_SEL_M  (GPIO_FUNC34_IN_INV_SEL_V << GPIO_FUNC34_IN_INV_SEL_S)
#define GPIO_FUNC34_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_IN_INV_SEL_S  7
/** GPIO_SIG34_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG34_IN_SEL    (BIT(8))
#define GPIO_SIG34_IN_SEL_M  (GPIO_SIG34_IN_SEL_V << GPIO_SIG34_IN_SEL_S)
#define GPIO_SIG34_IN_SEL_V  0x00000001U
#define GPIO_SIG34_IN_SEL_S  8

/** GPIO_FUNC35_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC35_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x360)
/** GPIO_FUNC35_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC35_IN_SEL    0x0000007FU
#define GPIO_FUNC35_IN_SEL_M  (GPIO_FUNC35_IN_SEL_V << GPIO_FUNC35_IN_SEL_S)
#define GPIO_FUNC35_IN_SEL_V  0x0000007FU
#define GPIO_FUNC35_IN_SEL_S  0
/** GPIO_FUNC35_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC35_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC35_IN_INV_SEL_M  (GPIO_FUNC35_IN_INV_SEL_V << GPIO_FUNC35_IN_INV_SEL_S)
#define GPIO_FUNC35_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_IN_INV_SEL_S  7
/** GPIO_SIG35_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG35_IN_SEL    (BIT(8))
#define GPIO_SIG35_IN_SEL_M  (GPIO_SIG35_IN_SEL_V << GPIO_SIG35_IN_SEL_S)
#define GPIO_SIG35_IN_SEL_V  0x00000001U
#define GPIO_SIG35_IN_SEL_S  8

/** GPIO_FUNC41_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC41_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x378)
/** GPIO_FUNC41_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC41_IN_SEL    0x0000007FU
#define GPIO_FUNC41_IN_SEL_M  (GPIO_FUNC41_IN_SEL_V << GPIO_FUNC41_IN_SEL_S)
#define GPIO_FUNC41_IN_SEL_V  0x0000007FU
#define GPIO_FUNC41_IN_SEL_S  0
/** GPIO_FUNC41_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC41_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC41_IN_INV_SEL_M  (GPIO_FUNC41_IN_INV_SEL_V << GPIO_FUNC41_IN_INV_SEL_S)
#define GPIO_FUNC41_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_IN_INV_SEL_S  7
/** GPIO_SIG41_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG41_IN_SEL    (BIT(8))
#define GPIO_SIG41_IN_SEL_M  (GPIO_SIG41_IN_SEL_V << GPIO_SIG41_IN_SEL_S)
#define GPIO_SIG41_IN_SEL_V  0x00000001U
#define GPIO_SIG41_IN_SEL_S  8

/** GPIO_FUNC42_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC42_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x37c)
/** GPIO_FUNC42_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC42_IN_SEL    0x0000007FU
#define GPIO_FUNC42_IN_SEL_M  (GPIO_FUNC42_IN_SEL_V << GPIO_FUNC42_IN_SEL_S)
#define GPIO_FUNC42_IN_SEL_V  0x0000007FU
#define GPIO_FUNC42_IN_SEL_S  0
/** GPIO_FUNC42_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC42_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC42_IN_INV_SEL_M  (GPIO_FUNC42_IN_INV_SEL_V << GPIO_FUNC42_IN_INV_SEL_S)
#define GPIO_FUNC42_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_IN_INV_SEL_S  7
/** GPIO_SIG42_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG42_IN_SEL    (BIT(8))
#define GPIO_SIG42_IN_SEL_M  (GPIO_SIG42_IN_SEL_V << GPIO_SIG42_IN_SEL_S)
#define GPIO_SIG42_IN_SEL_V  0x00000001U
#define GPIO_SIG42_IN_SEL_S  8

/** GPIO_FUNC43_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC43_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x380)
/** GPIO_FUNC43_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC43_IN_SEL    0x0000007FU
#define GPIO_FUNC43_IN_SEL_M  (GPIO_FUNC43_IN_SEL_V << GPIO_FUNC43_IN_SEL_S)
#define GPIO_FUNC43_IN_SEL_V  0x0000007FU
#define GPIO_FUNC43_IN_SEL_S  0
/** GPIO_FUNC43_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC43_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC43_IN_INV_SEL_M  (GPIO_FUNC43_IN_INV_SEL_V << GPIO_FUNC43_IN_INV_SEL_S)
#define GPIO_FUNC43_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_IN_INV_SEL_S  7
/** GPIO_SIG43_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG43_IN_SEL    (BIT(8))
#define GPIO_SIG43_IN_SEL_M  (GPIO_SIG43_IN_SEL_V << GPIO_SIG43_IN_SEL_S)
#define GPIO_SIG43_IN_SEL_V  0x00000001U
#define GPIO_SIG43_IN_SEL_S  8

/** GPIO_FUNC46_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC46_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x38c)
/** GPIO_FUNC46_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC46_IN_SEL    0x0000007FU
#define GPIO_FUNC46_IN_SEL_M  (GPIO_FUNC46_IN_SEL_V << GPIO_FUNC46_IN_SEL_S)
#define GPIO_FUNC46_IN_SEL_V  0x0000007FU
#define GPIO_FUNC46_IN_SEL_S  0
/** GPIO_FUNC46_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC46_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC46_IN_INV_SEL_M  (GPIO_FUNC46_IN_INV_SEL_V << GPIO_FUNC46_IN_INV_SEL_S)
#define GPIO_FUNC46_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_IN_INV_SEL_S  7
/** GPIO_SIG46_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG46_IN_SEL    (BIT(8))
#define GPIO_SIG46_IN_SEL_M  (GPIO_SIG46_IN_SEL_V << GPIO_SIG46_IN_SEL_S)
#define GPIO_SIG46_IN_SEL_V  0x00000001U
#define GPIO_SIG46_IN_SEL_S  8

/** GPIO_FUNC47_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC47_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x390)
/** GPIO_FUNC47_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC47_IN_SEL    0x0000007FU
#define GPIO_FUNC47_IN_SEL_M  (GPIO_FUNC47_IN_SEL_V << GPIO_FUNC47_IN_SEL_S)
#define GPIO_FUNC47_IN_SEL_V  0x0000007FU
#define GPIO_FUNC47_IN_SEL_S  0
/** GPIO_FUNC47_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC47_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC47_IN_INV_SEL_M  (GPIO_FUNC47_IN_INV_SEL_V << GPIO_FUNC47_IN_INV_SEL_S)
#define GPIO_FUNC47_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_IN_INV_SEL_S  7
/** GPIO_SIG47_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG47_IN_SEL    (BIT(8))
#define GPIO_SIG47_IN_SEL_M  (GPIO_SIG47_IN_SEL_V << GPIO_SIG47_IN_SEL_S)
#define GPIO_SIG47_IN_SEL_V  0x00000001U
#define GPIO_SIG47_IN_SEL_S  8

/** GPIO_FUNC64_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC64_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d4)
/** GPIO_FUNC64_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC64_IN_SEL    0x0000007FU
#define GPIO_FUNC64_IN_SEL_M  (GPIO_FUNC64_IN_SEL_V << GPIO_FUNC64_IN_SEL_S)
#define GPIO_FUNC64_IN_SEL_V  0x0000007FU
#define GPIO_FUNC64_IN_SEL_S  0
/** GPIO_FUNC64_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC64_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC64_IN_INV_SEL_M  (GPIO_FUNC64_IN_INV_SEL_V << GPIO_FUNC64_IN_INV_SEL_S)
#define GPIO_FUNC64_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC64_IN_INV_SEL_S  7
/** GPIO_SIG64_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG64_IN_SEL    (BIT(8))
#define GPIO_SIG64_IN_SEL_M  (GPIO_SIG64_IN_SEL_V << GPIO_SIG64_IN_SEL_S)
#define GPIO_SIG64_IN_SEL_V  0x00000001U
#define GPIO_SIG64_IN_SEL_S  8

/** GPIO_FUNC65_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC65_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d8)
/** GPIO_FUNC65_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC65_IN_SEL    0x0000007FU
#define GPIO_FUNC65_IN_SEL_M  (GPIO_FUNC65_IN_SEL_V << GPIO_FUNC65_IN_SEL_S)
#define GPIO_FUNC65_IN_SEL_V  0x0000007FU
#define GPIO_FUNC65_IN_SEL_S  0
/** GPIO_FUNC65_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC65_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC65_IN_INV_SEL_M  (GPIO_FUNC65_IN_INV_SEL_V << GPIO_FUNC65_IN_INV_SEL_S)
#define GPIO_FUNC65_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC65_IN_INV_SEL_S  7
/** GPIO_SIG65_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG65_IN_SEL    (BIT(8))
#define GPIO_SIG65_IN_SEL_M  (GPIO_SIG65_IN_SEL_V << GPIO_SIG65_IN_SEL_S)
#define GPIO_SIG65_IN_SEL_V  0x00000001U
#define GPIO_SIG65_IN_SEL_S  8

/** GPIO_FUNC66_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC66_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3dc)
/** GPIO_FUNC66_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC66_IN_SEL    0x0000007FU
#define GPIO_FUNC66_IN_SEL_M  (GPIO_FUNC66_IN_SEL_V << GPIO_FUNC66_IN_SEL_S)
#define GPIO_FUNC66_IN_SEL_V  0x0000007FU
#define GPIO_FUNC66_IN_SEL_S  0
/** GPIO_FUNC66_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC66_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC66_IN_INV_SEL_M  (GPIO_FUNC66_IN_INV_SEL_V << GPIO_FUNC66_IN_INV_SEL_S)
#define GPIO_FUNC66_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC66_IN_INV_SEL_S  7
/** GPIO_SIG66_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG66_IN_SEL    (BIT(8))
#define GPIO_SIG66_IN_SEL_M  (GPIO_SIG66_IN_SEL_V << GPIO_SIG66_IN_SEL_S)
#define GPIO_SIG66_IN_SEL_V  0x00000001U
#define GPIO_SIG66_IN_SEL_S  8

/** GPIO_FUNC67_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC67_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e0)
/** GPIO_FUNC67_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC67_IN_SEL    0x0000007FU
#define GPIO_FUNC67_IN_SEL_M  (GPIO_FUNC67_IN_SEL_V << GPIO_FUNC67_IN_SEL_S)
#define GPIO_FUNC67_IN_SEL_V  0x0000007FU
#define GPIO_FUNC67_IN_SEL_S  0
/** GPIO_FUNC67_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC67_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC67_IN_INV_SEL_M  (GPIO_FUNC67_IN_INV_SEL_V << GPIO_FUNC67_IN_INV_SEL_S)
#define GPIO_FUNC67_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC67_IN_INV_SEL_S  7
/** GPIO_SIG67_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG67_IN_SEL    (BIT(8))
#define GPIO_SIG67_IN_SEL_M  (GPIO_SIG67_IN_SEL_V << GPIO_SIG67_IN_SEL_S)
#define GPIO_SIG67_IN_SEL_V  0x00000001U
#define GPIO_SIG67_IN_SEL_S  8

/** GPIO_FUNC68_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC68_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e4)
/** GPIO_FUNC68_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC68_IN_SEL    0x0000007FU
#define GPIO_FUNC68_IN_SEL_M  (GPIO_FUNC68_IN_SEL_V << GPIO_FUNC68_IN_SEL_S)
#define GPIO_FUNC68_IN_SEL_V  0x0000007FU
#define GPIO_FUNC68_IN_SEL_S  0
/** GPIO_FUNC68_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC68_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC68_IN_INV_SEL_M  (GPIO_FUNC68_IN_INV_SEL_V << GPIO_FUNC68_IN_INV_SEL_S)
#define GPIO_FUNC68_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC68_IN_INV_SEL_S  7
/** GPIO_SIG68_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG68_IN_SEL    (BIT(8))
#define GPIO_SIG68_IN_SEL_M  (GPIO_SIG68_IN_SEL_V << GPIO_SIG68_IN_SEL_S)
#define GPIO_SIG68_IN_SEL_V  0x00000001U
#define GPIO_SIG68_IN_SEL_S  8

/** GPIO_FUNC69_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC69_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e8)
/** GPIO_FUNC69_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC69_IN_SEL    0x0000007FU
#define GPIO_FUNC69_IN_SEL_M  (GPIO_FUNC69_IN_SEL_V << GPIO_FUNC69_IN_SEL_S)
#define GPIO_FUNC69_IN_SEL_V  0x0000007FU
#define GPIO_FUNC69_IN_SEL_S  0
/** GPIO_FUNC69_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC69_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC69_IN_INV_SEL_M  (GPIO_FUNC69_IN_INV_SEL_V << GPIO_FUNC69_IN_INV_SEL_S)
#define GPIO_FUNC69_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC69_IN_INV_SEL_S  7
/** GPIO_SIG69_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG69_IN_SEL    (BIT(8))
#define GPIO_SIG69_IN_SEL_M  (GPIO_SIG69_IN_SEL_V << GPIO_SIG69_IN_SEL_S)
#define GPIO_SIG69_IN_SEL_V  0x00000001U
#define GPIO_SIG69_IN_SEL_S  8

/** GPIO_FUNC72_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC72_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f4)
/** GPIO_FUNC72_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC72_IN_SEL    0x0000007FU
#define GPIO_FUNC72_IN_SEL_M  (GPIO_FUNC72_IN_SEL_V << GPIO_FUNC72_IN_SEL_S)
#define GPIO_FUNC72_IN_SEL_V  0x0000007FU
#define GPIO_FUNC72_IN_SEL_S  0
/** GPIO_FUNC72_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC72_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC72_IN_INV_SEL_M  (GPIO_FUNC72_IN_INV_SEL_V << GPIO_FUNC72_IN_INV_SEL_S)
#define GPIO_FUNC72_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC72_IN_INV_SEL_S  7
/** GPIO_SIG72_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG72_IN_SEL    (BIT(8))
#define GPIO_SIG72_IN_SEL_M  (GPIO_SIG72_IN_SEL_V << GPIO_SIG72_IN_SEL_S)
#define GPIO_SIG72_IN_SEL_V  0x00000001U
#define GPIO_SIG72_IN_SEL_S  8

/** GPIO_FUNC73_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC73_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f8)
/** GPIO_FUNC73_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC73_IN_SEL    0x0000007FU
#define GPIO_FUNC73_IN_SEL_M  (GPIO_FUNC73_IN_SEL_V << GPIO_FUNC73_IN_SEL_S)
#define GPIO_FUNC73_IN_SEL_V  0x0000007FU
#define GPIO_FUNC73_IN_SEL_S  0
/** GPIO_FUNC73_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC73_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC73_IN_INV_SEL_M  (GPIO_FUNC73_IN_INV_SEL_V << GPIO_FUNC73_IN_INV_SEL_S)
#define GPIO_FUNC73_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC73_IN_INV_SEL_S  7
/** GPIO_SIG73_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG73_IN_SEL    (BIT(8))
#define GPIO_SIG73_IN_SEL_M  (GPIO_SIG73_IN_SEL_V << GPIO_SIG73_IN_SEL_S)
#define GPIO_SIG73_IN_SEL_V  0x00000001U
#define GPIO_SIG73_IN_SEL_S  8

/** GPIO_FUNC74_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC74_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3fc)
/** GPIO_FUNC74_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC74_IN_SEL    0x0000007FU
#define GPIO_FUNC74_IN_SEL_M  (GPIO_FUNC74_IN_SEL_V << GPIO_FUNC74_IN_SEL_S)
#define GPIO_FUNC74_IN_SEL_V  0x0000007FU
#define GPIO_FUNC74_IN_SEL_S  0
/** GPIO_FUNC74_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC74_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC74_IN_INV_SEL_M  (GPIO_FUNC74_IN_INV_SEL_V << GPIO_FUNC74_IN_INV_SEL_S)
#define GPIO_FUNC74_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC74_IN_INV_SEL_S  7
/** GPIO_SIG74_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG74_IN_SEL    (BIT(8))
#define GPIO_SIG74_IN_SEL_M  (GPIO_SIG74_IN_SEL_V << GPIO_SIG74_IN_SEL_S)
#define GPIO_SIG74_IN_SEL_V  0x00000001U
#define GPIO_SIG74_IN_SEL_S  8

/** GPIO_FUNC82_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC82_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x41c)
/** GPIO_FUNC82_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC82_IN_SEL    0x0000007FU
#define GPIO_FUNC82_IN_SEL_M  (GPIO_FUNC82_IN_SEL_V << GPIO_FUNC82_IN_SEL_S)
#define GPIO_FUNC82_IN_SEL_V  0x0000007FU
#define GPIO_FUNC82_IN_SEL_S  0
/** GPIO_FUNC82_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC82_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC82_IN_INV_SEL_M  (GPIO_FUNC82_IN_INV_SEL_V << GPIO_FUNC82_IN_INV_SEL_S)
#define GPIO_FUNC82_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC82_IN_INV_SEL_S  7
/** GPIO_SIG82_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG82_IN_SEL    (BIT(8))
#define GPIO_SIG82_IN_SEL_M  (GPIO_SIG82_IN_SEL_V << GPIO_SIG82_IN_SEL_S)
#define GPIO_SIG82_IN_SEL_V  0x00000001U
#define GPIO_SIG82_IN_SEL_S  8

/** GPIO_FUNC83_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC83_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x420)
/** GPIO_FUNC83_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC83_IN_SEL    0x0000007FU
#define GPIO_FUNC83_IN_SEL_M  (GPIO_FUNC83_IN_SEL_V << GPIO_FUNC83_IN_SEL_S)
#define GPIO_FUNC83_IN_SEL_V  0x0000007FU
#define GPIO_FUNC83_IN_SEL_S  0
/** GPIO_FUNC83_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC83_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC83_IN_INV_SEL_M  (GPIO_FUNC83_IN_INV_SEL_V << GPIO_FUNC83_IN_INV_SEL_S)
#define GPIO_FUNC83_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC83_IN_INV_SEL_S  7
/** GPIO_SIG83_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG83_IN_SEL    (BIT(8))
#define GPIO_SIG83_IN_SEL_M  (GPIO_SIG83_IN_SEL_V << GPIO_SIG83_IN_SEL_S)
#define GPIO_SIG83_IN_SEL_V  0x00000001U
#define GPIO_SIG83_IN_SEL_S  8

/** GPIO_FUNC97_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC97_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x458)
/** GPIO_FUNC97_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC97_IN_SEL    0x0000007FU
#define GPIO_FUNC97_IN_SEL_M  (GPIO_FUNC97_IN_SEL_V << GPIO_FUNC97_IN_SEL_S)
#define GPIO_FUNC97_IN_SEL_V  0x0000007FU
#define GPIO_FUNC97_IN_SEL_S  0
/** GPIO_FUNC97_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC97_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC97_IN_INV_SEL_M  (GPIO_FUNC97_IN_INV_SEL_V << GPIO_FUNC97_IN_INV_SEL_S)
#define GPIO_FUNC97_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC97_IN_INV_SEL_S  7
/** GPIO_SIG97_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG97_IN_SEL    (BIT(8))
#define GPIO_SIG97_IN_SEL_M  (GPIO_SIG97_IN_SEL_V << GPIO_SIG97_IN_SEL_S)
#define GPIO_SIG97_IN_SEL_V  0x00000001U
#define GPIO_SIG97_IN_SEL_S  8

/** GPIO_FUNC98_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC98_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x45c)
/** GPIO_FUNC98_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC98_IN_SEL    0x0000007FU
#define GPIO_FUNC98_IN_SEL_M  (GPIO_FUNC98_IN_SEL_V << GPIO_FUNC98_IN_SEL_S)
#define GPIO_FUNC98_IN_SEL_V  0x0000007FU
#define GPIO_FUNC98_IN_SEL_S  0
/** GPIO_FUNC98_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC98_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC98_IN_INV_SEL_M  (GPIO_FUNC98_IN_INV_SEL_V << GPIO_FUNC98_IN_INV_SEL_S)
#define GPIO_FUNC98_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC98_IN_INV_SEL_S  7
/** GPIO_SIG98_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG98_IN_SEL    (BIT(8))
#define GPIO_SIG98_IN_SEL_M  (GPIO_SIG98_IN_SEL_V << GPIO_SIG98_IN_SEL_S)
#define GPIO_SIG98_IN_SEL_V  0x00000001U
#define GPIO_SIG98_IN_SEL_S  8

/** GPIO_FUNC99_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC99_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x460)
/** GPIO_FUNC99_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC99_IN_SEL    0x0000007FU
#define GPIO_FUNC99_IN_SEL_M  (GPIO_FUNC99_IN_SEL_V << GPIO_FUNC99_IN_SEL_S)
#define GPIO_FUNC99_IN_SEL_V  0x0000007FU
#define GPIO_FUNC99_IN_SEL_S  0
/** GPIO_FUNC99_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC99_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC99_IN_INV_SEL_M  (GPIO_FUNC99_IN_INV_SEL_V << GPIO_FUNC99_IN_INV_SEL_S)
#define GPIO_FUNC99_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC99_IN_INV_SEL_S  7
/** GPIO_SIG99_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG99_IN_SEL    (BIT(8))
#define GPIO_SIG99_IN_SEL_M  (GPIO_SIG99_IN_SEL_V << GPIO_SIG99_IN_SEL_S)
#define GPIO_SIG99_IN_SEL_V  0x00000001U
#define GPIO_SIG99_IN_SEL_S  8

/** GPIO_FUNC100_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC100_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x464)
/** GPIO_FUNC100_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC100_IN_SEL    0x0000007FU
#define GPIO_FUNC100_IN_SEL_M  (GPIO_FUNC100_IN_SEL_V << GPIO_FUNC100_IN_SEL_S)
#define GPIO_FUNC100_IN_SEL_V  0x0000007FU
#define GPIO_FUNC100_IN_SEL_S  0
/** GPIO_FUNC100_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC100_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC100_IN_INV_SEL_M  (GPIO_FUNC100_IN_INV_SEL_V << GPIO_FUNC100_IN_INV_SEL_S)
#define GPIO_FUNC100_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC100_IN_INV_SEL_S  7
/** GPIO_SIG100_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG100_IN_SEL    (BIT(8))
#define GPIO_SIG100_IN_SEL_M  (GPIO_SIG100_IN_SEL_V << GPIO_SIG100_IN_SEL_S)
#define GPIO_SIG100_IN_SEL_V  0x00000001U
#define GPIO_SIG100_IN_SEL_S  8

/** GPIO_FUNC118_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC118_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4ac)
/** GPIO_FUNC118_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC118_IN_SEL    0x0000007FU
#define GPIO_FUNC118_IN_SEL_M  (GPIO_FUNC118_IN_SEL_V << GPIO_FUNC118_IN_SEL_S)
#define GPIO_FUNC118_IN_SEL_V  0x0000007FU
#define GPIO_FUNC118_IN_SEL_S  0
/** GPIO_FUNC118_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC118_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC118_IN_INV_SEL_M  (GPIO_FUNC118_IN_INV_SEL_V << GPIO_FUNC118_IN_INV_SEL_S)
#define GPIO_FUNC118_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC118_IN_INV_SEL_S  7
/** GPIO_SIG118_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG118_IN_SEL    (BIT(8))
#define GPIO_SIG118_IN_SEL_M  (GPIO_SIG118_IN_SEL_V << GPIO_SIG118_IN_SEL_S)
#define GPIO_SIG118_IN_SEL_V  0x00000001U
#define GPIO_SIG118_IN_SEL_S  8

/** GPIO_FUNC119_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC119_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b0)
/** GPIO_FUNC119_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC119_IN_SEL    0x0000007FU
#define GPIO_FUNC119_IN_SEL_M  (GPIO_FUNC119_IN_SEL_V << GPIO_FUNC119_IN_SEL_S)
#define GPIO_FUNC119_IN_SEL_V  0x0000007FU
#define GPIO_FUNC119_IN_SEL_S  0
/** GPIO_FUNC119_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC119_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC119_IN_INV_SEL_M  (GPIO_FUNC119_IN_INV_SEL_V << GPIO_FUNC119_IN_INV_SEL_S)
#define GPIO_FUNC119_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC119_IN_INV_SEL_S  7
/** GPIO_SIG119_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG119_IN_SEL    (BIT(8))
#define GPIO_SIG119_IN_SEL_M  (GPIO_SIG119_IN_SEL_V << GPIO_SIG119_IN_SEL_S)
#define GPIO_SIG119_IN_SEL_V  0x00000001U
#define GPIO_SIG119_IN_SEL_S  8

/** GPIO_FUNC120_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC120_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b4)
/** GPIO_FUNC120_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC120_IN_SEL    0x0000007FU
#define GPIO_FUNC120_IN_SEL_M  (GPIO_FUNC120_IN_SEL_V << GPIO_FUNC120_IN_SEL_S)
#define GPIO_FUNC120_IN_SEL_V  0x0000007FU
#define GPIO_FUNC120_IN_SEL_S  0
/** GPIO_FUNC120_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC120_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC120_IN_INV_SEL_M  (GPIO_FUNC120_IN_INV_SEL_V << GPIO_FUNC120_IN_INV_SEL_S)
#define GPIO_FUNC120_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC120_IN_INV_SEL_S  7
/** GPIO_SIG120_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG120_IN_SEL    (BIT(8))
#define GPIO_SIG120_IN_SEL_M  (GPIO_SIG120_IN_SEL_V << GPIO_SIG120_IN_SEL_S)
#define GPIO_SIG120_IN_SEL_V  0x00000001U
#define GPIO_SIG120_IN_SEL_S  8

/** GPIO_FUNC121_IN_SEL_CFG_REG register
 *  GPIO input function configuration register
 */
#define GPIO_FUNC121_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b8)
/** GPIO_FUNC121_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
 *  high level. s=0x60: set this port always low level.
 */
#define GPIO_FUNC121_IN_SEL    0x0000007FU
#define GPIO_FUNC121_IN_SEL_M  (GPIO_FUNC121_IN_SEL_V << GPIO_FUNC121_IN_SEL_S)
#define GPIO_FUNC121_IN_SEL_V  0x0000007FU
#define GPIO_FUNC121_IN_SEL_S  0
/** GPIO_FUNC121_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  set this bit to invert input signal. 1:invert. 0:not invert.
 */
#define GPIO_FUNC121_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC121_IN_INV_SEL_M  (GPIO_FUNC121_IN_INV_SEL_V << GPIO_FUNC121_IN_INV_SEL_S)
#define GPIO_FUNC121_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC121_IN_INV_SEL_S  7
/** GPIO_SIG121_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
 */
#define GPIO_SIG121_IN_SEL    (BIT(8))
#define GPIO_SIG121_IN_SEL_M  (GPIO_SIG121_IN_SEL_V << GPIO_SIG121_IN_SEL_S)
#define GPIO_SIG121_IN_SEL_V  0x00000001U
#define GPIO_SIG121_IN_SEL_S  8

/** GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xad4)
/** GPIO_FUNC0_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC0_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC0_OE_SEL    (BIT(10))
#define GPIO_FUNC0_OE_SEL_M  (GPIO_FUNC0_OE_SEL_V << GPIO_FUNC0_OE_SEL_S)
#define GPIO_FUNC0_OE_SEL_V  0x00000001U
#define GPIO_FUNC0_OE_SEL_S  10
/** GPIO_FUNC0_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC0_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC0_OE_INV_SEL_M  (GPIO_FUNC0_OE_INV_SEL_V << GPIO_FUNC0_OE_INV_SEL_S)
#define GPIO_FUNC0_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OE_INV_SEL_S  11

/** GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xad8)
/** GPIO_FUNC1_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC1_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC1_OE_SEL    (BIT(10))
#define GPIO_FUNC1_OE_SEL_M  (GPIO_FUNC1_OE_SEL_V << GPIO_FUNC1_OE_SEL_S)
#define GPIO_FUNC1_OE_SEL_V  0x00000001U
#define GPIO_FUNC1_OE_SEL_S  10
/** GPIO_FUNC1_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC1_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC1_OE_INV_SEL_M  (GPIO_FUNC1_OE_INV_SEL_V << GPIO_FUNC1_OE_INV_SEL_S)
#define GPIO_FUNC1_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OE_INV_SEL_S  11

/** GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xadc)
/** GPIO_FUNC2_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC2_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC2_OE_SEL    (BIT(10))
#define GPIO_FUNC2_OE_SEL_M  (GPIO_FUNC2_OE_SEL_V << GPIO_FUNC2_OE_SEL_S)
#define GPIO_FUNC2_OE_SEL_V  0x00000001U
#define GPIO_FUNC2_OE_SEL_S  10
/** GPIO_FUNC2_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC2_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC2_OE_INV_SEL_M  (GPIO_FUNC2_OE_INV_SEL_V << GPIO_FUNC2_OE_INV_SEL_S)
#define GPIO_FUNC2_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OE_INV_SEL_S  11

/** GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xae0)
/** GPIO_FUNC3_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC3_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC3_OE_SEL    (BIT(10))
#define GPIO_FUNC3_OE_SEL_M  (GPIO_FUNC3_OE_SEL_V << GPIO_FUNC3_OE_SEL_S)
#define GPIO_FUNC3_OE_SEL_V  0x00000001U
#define GPIO_FUNC3_OE_SEL_S  10
/** GPIO_FUNC3_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC3_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC3_OE_INV_SEL_M  (GPIO_FUNC3_OE_INV_SEL_V << GPIO_FUNC3_OE_INV_SEL_S)
#define GPIO_FUNC3_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OE_INV_SEL_S  11

/** GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xae4)
/** GPIO_FUNC4_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC4_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC4_OE_SEL    (BIT(10))
#define GPIO_FUNC4_OE_SEL_M  (GPIO_FUNC4_OE_SEL_V << GPIO_FUNC4_OE_SEL_S)
#define GPIO_FUNC4_OE_SEL_V  0x00000001U
#define GPIO_FUNC4_OE_SEL_S  10
/** GPIO_FUNC4_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC4_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC4_OE_INV_SEL_M  (GPIO_FUNC4_OE_INV_SEL_V << GPIO_FUNC4_OE_INV_SEL_S)
#define GPIO_FUNC4_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OE_INV_SEL_S  11

/** GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xae8)
/** GPIO_FUNC5_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC5_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC5_OE_SEL    (BIT(10))
#define GPIO_FUNC5_OE_SEL_M  (GPIO_FUNC5_OE_SEL_V << GPIO_FUNC5_OE_SEL_S)
#define GPIO_FUNC5_OE_SEL_V  0x00000001U
#define GPIO_FUNC5_OE_SEL_S  10
/** GPIO_FUNC5_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC5_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC5_OE_INV_SEL_M  (GPIO_FUNC5_OE_INV_SEL_V << GPIO_FUNC5_OE_INV_SEL_S)
#define GPIO_FUNC5_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OE_INV_SEL_S  11

/** GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaec)
/** GPIO_FUNC6_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC6_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC6_OE_SEL    (BIT(10))
#define GPIO_FUNC6_OE_SEL_M  (GPIO_FUNC6_OE_SEL_V << GPIO_FUNC6_OE_SEL_S)
#define GPIO_FUNC6_OE_SEL_V  0x00000001U
#define GPIO_FUNC6_OE_SEL_S  10
/** GPIO_FUNC6_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC6_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC6_OE_INV_SEL_M  (GPIO_FUNC6_OE_INV_SEL_V << GPIO_FUNC6_OE_INV_SEL_S)
#define GPIO_FUNC6_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OE_INV_SEL_S  11

/** GPIO_FUNC7_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC7_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf0)
/** GPIO_FUNC7_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC7_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC7_OE_SEL    (BIT(10))
#define GPIO_FUNC7_OE_SEL_M  (GPIO_FUNC7_OE_SEL_V << GPIO_FUNC7_OE_SEL_S)
#define GPIO_FUNC7_OE_SEL_V  0x00000001U
#define GPIO_FUNC7_OE_SEL_S  10
/** GPIO_FUNC7_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC7_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC7_OE_INV_SEL_M  (GPIO_FUNC7_OE_INV_SEL_V << GPIO_FUNC7_OE_INV_SEL_S)
#define GPIO_FUNC7_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OE_INV_SEL_S  11

/** GPIO_FUNC8_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC8_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf4)
/** GPIO_FUNC8_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC8_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC8_OE_SEL    (BIT(10))
#define GPIO_FUNC8_OE_SEL_M  (GPIO_FUNC8_OE_SEL_V << GPIO_FUNC8_OE_SEL_S)
#define GPIO_FUNC8_OE_SEL_V  0x00000001U
#define GPIO_FUNC8_OE_SEL_S  10
/** GPIO_FUNC8_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC8_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC8_OE_INV_SEL_M  (GPIO_FUNC8_OE_INV_SEL_V << GPIO_FUNC8_OE_INV_SEL_S)
#define GPIO_FUNC8_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OE_INV_SEL_S  11

/** GPIO_FUNC9_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC9_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf8)
/** GPIO_FUNC9_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC9_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC9_OE_SEL    (BIT(10))
#define GPIO_FUNC9_OE_SEL_M  (GPIO_FUNC9_OE_SEL_V << GPIO_FUNC9_OE_SEL_S)
#define GPIO_FUNC9_OE_SEL_V  0x00000001U
#define GPIO_FUNC9_OE_SEL_S  10
/** GPIO_FUNC9_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC9_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC9_OE_INV_SEL_M  (GPIO_FUNC9_OE_INV_SEL_V << GPIO_FUNC9_OE_INV_SEL_S)
#define GPIO_FUNC9_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OE_INV_SEL_S  11

/** GPIO_FUNC10_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC10_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xafc)
/** GPIO_FUNC10_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC10_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC10_OE_SEL    (BIT(10))
#define GPIO_FUNC10_OE_SEL_M  (GPIO_FUNC10_OE_SEL_V << GPIO_FUNC10_OE_SEL_S)
#define GPIO_FUNC10_OE_SEL_V  0x00000001U
#define GPIO_FUNC10_OE_SEL_S  10
/** GPIO_FUNC10_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC10_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC10_OE_INV_SEL_M  (GPIO_FUNC10_OE_INV_SEL_V << GPIO_FUNC10_OE_INV_SEL_S)
#define GPIO_FUNC10_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OE_INV_SEL_S  11

/** GPIO_FUNC11_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC11_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb00)
/** GPIO_FUNC11_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC11_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC11_OE_SEL    (BIT(10))
#define GPIO_FUNC11_OE_SEL_M  (GPIO_FUNC11_OE_SEL_V << GPIO_FUNC11_OE_SEL_S)
#define GPIO_FUNC11_OE_SEL_V  0x00000001U
#define GPIO_FUNC11_OE_SEL_S  10
/** GPIO_FUNC11_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC11_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC11_OE_INV_SEL_M  (GPIO_FUNC11_OE_INV_SEL_V << GPIO_FUNC11_OE_INV_SEL_S)
#define GPIO_FUNC11_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OE_INV_SEL_S  11

/** GPIO_FUNC12_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC12_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb04)
/** GPIO_FUNC12_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC12_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC12_OE_SEL    (BIT(10))
#define GPIO_FUNC12_OE_SEL_M  (GPIO_FUNC12_OE_SEL_V << GPIO_FUNC12_OE_SEL_S)
#define GPIO_FUNC12_OE_SEL_V  0x00000001U
#define GPIO_FUNC12_OE_SEL_S  10
/** GPIO_FUNC12_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC12_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC12_OE_INV_SEL_M  (GPIO_FUNC12_OE_INV_SEL_V << GPIO_FUNC12_OE_INV_SEL_S)
#define GPIO_FUNC12_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OE_INV_SEL_S  11

/** GPIO_FUNC13_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC13_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb08)
/** GPIO_FUNC13_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC13_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC13_OE_SEL    (BIT(10))
#define GPIO_FUNC13_OE_SEL_M  (GPIO_FUNC13_OE_SEL_V << GPIO_FUNC13_OE_SEL_S)
#define GPIO_FUNC13_OE_SEL_V  0x00000001U
#define GPIO_FUNC13_OE_SEL_S  10
/** GPIO_FUNC13_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC13_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC13_OE_INV_SEL_M  (GPIO_FUNC13_OE_INV_SEL_V << GPIO_FUNC13_OE_INV_SEL_S)
#define GPIO_FUNC13_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OE_INV_SEL_S  11

/** GPIO_FUNC14_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC14_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb0c)
/** GPIO_FUNC14_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC14_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC14_OE_SEL    (BIT(10))
#define GPIO_FUNC14_OE_SEL_M  (GPIO_FUNC14_OE_SEL_V << GPIO_FUNC14_OE_SEL_S)
#define GPIO_FUNC14_OE_SEL_V  0x00000001U
#define GPIO_FUNC14_OE_SEL_S  10
/** GPIO_FUNC14_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC14_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC14_OE_INV_SEL_M  (GPIO_FUNC14_OE_INV_SEL_V << GPIO_FUNC14_OE_INV_SEL_S)
#define GPIO_FUNC14_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OE_INV_SEL_S  11

/** GPIO_FUNC15_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC15_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb10)
/** GPIO_FUNC15_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC15_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC15_OE_SEL    (BIT(10))
#define GPIO_FUNC15_OE_SEL_M  (GPIO_FUNC15_OE_SEL_V << GPIO_FUNC15_OE_SEL_S)
#define GPIO_FUNC15_OE_SEL_V  0x00000001U
#define GPIO_FUNC15_OE_SEL_S  10
/** GPIO_FUNC15_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC15_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC15_OE_INV_SEL_M  (GPIO_FUNC15_OE_INV_SEL_V << GPIO_FUNC15_OE_INV_SEL_S)
#define GPIO_FUNC15_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OE_INV_SEL_S  11

/** GPIO_FUNC16_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC16_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb14)
/** GPIO_FUNC16_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC16_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC16_OE_SEL    (BIT(10))
#define GPIO_FUNC16_OE_SEL_M  (GPIO_FUNC16_OE_SEL_V << GPIO_FUNC16_OE_SEL_S)
#define GPIO_FUNC16_OE_SEL_V  0x00000001U
#define GPIO_FUNC16_OE_SEL_S  10
/** GPIO_FUNC16_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC16_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC16_OE_INV_SEL_M  (GPIO_FUNC16_OE_INV_SEL_V << GPIO_FUNC16_OE_INV_SEL_S)
#define GPIO_FUNC16_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OE_INV_SEL_S  11

/** GPIO_FUNC17_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC17_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb18)
/** GPIO_FUNC17_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC17_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC17_OE_SEL    (BIT(10))
#define GPIO_FUNC17_OE_SEL_M  (GPIO_FUNC17_OE_SEL_V << GPIO_FUNC17_OE_SEL_S)
#define GPIO_FUNC17_OE_SEL_V  0x00000001U
#define GPIO_FUNC17_OE_SEL_S  10
/** GPIO_FUNC17_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC17_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC17_OE_INV_SEL_M  (GPIO_FUNC17_OE_INV_SEL_V << GPIO_FUNC17_OE_INV_SEL_S)
#define GPIO_FUNC17_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OE_INV_SEL_S  11

/** GPIO_FUNC18_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC18_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb1c)
/** GPIO_FUNC18_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC18_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC18_OE_SEL    (BIT(10))
#define GPIO_FUNC18_OE_SEL_M  (GPIO_FUNC18_OE_SEL_V << GPIO_FUNC18_OE_SEL_S)
#define GPIO_FUNC18_OE_SEL_V  0x00000001U
#define GPIO_FUNC18_OE_SEL_S  10
/** GPIO_FUNC18_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC18_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC18_OE_INV_SEL_M  (GPIO_FUNC18_OE_INV_SEL_V << GPIO_FUNC18_OE_INV_SEL_S)
#define GPIO_FUNC18_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OE_INV_SEL_S  11

/** GPIO_FUNC19_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC19_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb20)
/** GPIO_FUNC19_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC19_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC19_OE_SEL    (BIT(10))
#define GPIO_FUNC19_OE_SEL_M  (GPIO_FUNC19_OE_SEL_V << GPIO_FUNC19_OE_SEL_S)
#define GPIO_FUNC19_OE_SEL_V  0x00000001U
#define GPIO_FUNC19_OE_SEL_S  10
/** GPIO_FUNC19_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC19_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC19_OE_INV_SEL_M  (GPIO_FUNC19_OE_INV_SEL_V << GPIO_FUNC19_OE_INV_SEL_S)
#define GPIO_FUNC19_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OE_INV_SEL_S  11

/** GPIO_FUNC20_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC20_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb24)
/** GPIO_FUNC20_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC20_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC20_OE_SEL    (BIT(10))
#define GPIO_FUNC20_OE_SEL_M  (GPIO_FUNC20_OE_SEL_V << GPIO_FUNC20_OE_SEL_S)
#define GPIO_FUNC20_OE_SEL_V  0x00000001U
#define GPIO_FUNC20_OE_SEL_S  10
/** GPIO_FUNC20_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC20_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC20_OE_INV_SEL_M  (GPIO_FUNC20_OE_INV_SEL_V << GPIO_FUNC20_OE_INV_SEL_S)
#define GPIO_FUNC20_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OE_INV_SEL_S  11

/** GPIO_FUNC21_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC21_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb28)
/** GPIO_FUNC21_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC21_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC21_OE_SEL    (BIT(10))
#define GPIO_FUNC21_OE_SEL_M  (GPIO_FUNC21_OE_SEL_V << GPIO_FUNC21_OE_SEL_S)
#define GPIO_FUNC21_OE_SEL_V  0x00000001U
#define GPIO_FUNC21_OE_SEL_S  10
/** GPIO_FUNC21_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC21_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC21_OE_INV_SEL_M  (GPIO_FUNC21_OE_INV_SEL_V << GPIO_FUNC21_OE_INV_SEL_S)
#define GPIO_FUNC21_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OE_INV_SEL_S  11

/** GPIO_FUNC22_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC22_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb2c)
/** GPIO_FUNC22_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC22_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC22_OE_SEL    (BIT(10))
#define GPIO_FUNC22_OE_SEL_M  (GPIO_FUNC22_OE_SEL_V << GPIO_FUNC22_OE_SEL_S)
#define GPIO_FUNC22_OE_SEL_V  0x00000001U
#define GPIO_FUNC22_OE_SEL_S  10
/** GPIO_FUNC22_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC22_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC22_OE_INV_SEL_M  (GPIO_FUNC22_OE_INV_SEL_V << GPIO_FUNC22_OE_INV_SEL_S)
#define GPIO_FUNC22_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OE_INV_SEL_S  11

/** GPIO_FUNC23_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC23_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb30)
/** GPIO_FUNC23_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC23_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC23_OE_SEL    (BIT(10))
#define GPIO_FUNC23_OE_SEL_M  (GPIO_FUNC23_OE_SEL_V << GPIO_FUNC23_OE_SEL_S)
#define GPIO_FUNC23_OE_SEL_V  0x00000001U
#define GPIO_FUNC23_OE_SEL_S  10
/** GPIO_FUNC23_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC23_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC23_OE_INV_SEL_M  (GPIO_FUNC23_OE_INV_SEL_V << GPIO_FUNC23_OE_INV_SEL_S)
#define GPIO_FUNC23_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OE_INV_SEL_S  11

/** GPIO_FUNC24_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC24_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb34)
/** GPIO_FUNC24_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC24_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC24_OE_SEL    (BIT(10))
#define GPIO_FUNC24_OE_SEL_M  (GPIO_FUNC24_OE_SEL_V << GPIO_FUNC24_OE_SEL_S)
#define GPIO_FUNC24_OE_SEL_V  0x00000001U
#define GPIO_FUNC24_OE_SEL_S  10
/** GPIO_FUNC24_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC24_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC24_OE_INV_SEL_M  (GPIO_FUNC24_OE_INV_SEL_V << GPIO_FUNC24_OE_INV_SEL_S)
#define GPIO_FUNC24_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OE_INV_SEL_S  11

/** GPIO_FUNC25_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC25_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb38)
/** GPIO_FUNC25_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC25_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC25_OE_SEL    (BIT(10))
#define GPIO_FUNC25_OE_SEL_M  (GPIO_FUNC25_OE_SEL_V << GPIO_FUNC25_OE_SEL_S)
#define GPIO_FUNC25_OE_SEL_V  0x00000001U
#define GPIO_FUNC25_OE_SEL_S  10
/** GPIO_FUNC25_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC25_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC25_OE_INV_SEL_M  (GPIO_FUNC25_OE_INV_SEL_V << GPIO_FUNC25_OE_INV_SEL_S)
#define GPIO_FUNC25_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OE_INV_SEL_S  11

/** GPIO_FUNC26_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC26_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb3c)
/** GPIO_FUNC26_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC26_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC26_OE_SEL    (BIT(10))
#define GPIO_FUNC26_OE_SEL_M  (GPIO_FUNC26_OE_SEL_V << GPIO_FUNC26_OE_SEL_S)
#define GPIO_FUNC26_OE_SEL_V  0x00000001U
#define GPIO_FUNC26_OE_SEL_S  10
/** GPIO_FUNC26_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC26_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC26_OE_INV_SEL_M  (GPIO_FUNC26_OE_INV_SEL_V << GPIO_FUNC26_OE_INV_SEL_S)
#define GPIO_FUNC26_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OE_INV_SEL_S  11

/** GPIO_FUNC27_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC27_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb40)
/** GPIO_FUNC27_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC27_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC27_OE_SEL    (BIT(10))
#define GPIO_FUNC27_OE_SEL_M  (GPIO_FUNC27_OE_SEL_V << GPIO_FUNC27_OE_SEL_S)
#define GPIO_FUNC27_OE_SEL_V  0x00000001U
#define GPIO_FUNC27_OE_SEL_S  10
/** GPIO_FUNC27_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC27_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC27_OE_INV_SEL_M  (GPIO_FUNC27_OE_INV_SEL_V << GPIO_FUNC27_OE_INV_SEL_S)
#define GPIO_FUNC27_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OE_INV_SEL_S  11

/** GPIO_FUNC28_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC28_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb44)
/** GPIO_FUNC28_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC28_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC28_OE_SEL    (BIT(10))
#define GPIO_FUNC28_OE_SEL_M  (GPIO_FUNC28_OE_SEL_V << GPIO_FUNC28_OE_SEL_S)
#define GPIO_FUNC28_OE_SEL_V  0x00000001U
#define GPIO_FUNC28_OE_SEL_S  10
/** GPIO_FUNC28_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC28_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC28_OE_INV_SEL_M  (GPIO_FUNC28_OE_INV_SEL_V << GPIO_FUNC28_OE_INV_SEL_S)
#define GPIO_FUNC28_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OE_INV_SEL_S  11

/** GPIO_FUNC29_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC29_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb48)
/** GPIO_FUNC29_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC29_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC29_OE_SEL    (BIT(10))
#define GPIO_FUNC29_OE_SEL_M  (GPIO_FUNC29_OE_SEL_V << GPIO_FUNC29_OE_SEL_S)
#define GPIO_FUNC29_OE_SEL_V  0x00000001U
#define GPIO_FUNC29_OE_SEL_S  10
/** GPIO_FUNC29_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC29_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC29_OE_INV_SEL_M  (GPIO_FUNC29_OE_INV_SEL_V << GPIO_FUNC29_OE_INV_SEL_S)
#define GPIO_FUNC29_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OE_INV_SEL_S  11

/** GPIO_FUNC30_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC30_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb4c)
/** GPIO_FUNC30_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC30_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC30_OE_SEL    (BIT(10))
#define GPIO_FUNC30_OE_SEL_M  (GPIO_FUNC30_OE_SEL_V << GPIO_FUNC30_OE_SEL_S)
#define GPIO_FUNC30_OE_SEL_V  0x00000001U
#define GPIO_FUNC30_OE_SEL_S  10
/** GPIO_FUNC30_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC30_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC30_OE_INV_SEL_M  (GPIO_FUNC30_OE_INV_SEL_V << GPIO_FUNC30_OE_INV_SEL_S)
#define GPIO_FUNC30_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OE_INV_SEL_S  11

/** GPIO_FUNC31_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC31_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb50)
/** GPIO_FUNC31_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC31_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC31_OE_SEL    (BIT(10))
#define GPIO_FUNC31_OE_SEL_M  (GPIO_FUNC31_OE_SEL_V << GPIO_FUNC31_OE_SEL_S)
#define GPIO_FUNC31_OE_SEL_V  0x00000001U
#define GPIO_FUNC31_OE_SEL_S  10
/** GPIO_FUNC31_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC31_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC31_OE_INV_SEL_M  (GPIO_FUNC31_OE_INV_SEL_V << GPIO_FUNC31_OE_INV_SEL_S)
#define GPIO_FUNC31_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OE_INV_SEL_S  11

/** GPIO_FUNC32_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC32_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb54)
/** GPIO_FUNC32_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC32_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC32_OE_SEL    (BIT(10))
#define GPIO_FUNC32_OE_SEL_M  (GPIO_FUNC32_OE_SEL_V << GPIO_FUNC32_OE_SEL_S)
#define GPIO_FUNC32_OE_SEL_V  0x00000001U
#define GPIO_FUNC32_OE_SEL_S  10
/** GPIO_FUNC32_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC32_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC32_OE_INV_SEL_M  (GPIO_FUNC32_OE_INV_SEL_V << GPIO_FUNC32_OE_INV_SEL_S)
#define GPIO_FUNC32_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OE_INV_SEL_S  11

/** GPIO_FUNC33_OUT_SEL_CFG_REG register
 *  GPIO output function select register
 */
#define GPIO_FUNC33_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb58)
/** GPIO_FUNC33_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
 *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
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
/** GPIO_FUNC33_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
 *  enable signal.0:use peripheral output enable signal.
 */
#define GPIO_FUNC33_OE_SEL    (BIT(10))
#define GPIO_FUNC33_OE_SEL_M  (GPIO_FUNC33_OE_SEL_V << GPIO_FUNC33_OE_SEL_S)
#define GPIO_FUNC33_OE_SEL_V  0x00000001U
#define GPIO_FUNC33_OE_SEL_S  10
/** GPIO_FUNC33_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  set this bit to invert output enable signal.1:invert.0:not invert.
 */
#define GPIO_FUNC33_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC33_OE_INV_SEL_M  (GPIO_FUNC33_OE_INV_SEL_V << GPIO_FUNC33_OE_INV_SEL_S)
#define GPIO_FUNC33_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_OE_INV_SEL_S  11

/** GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define GPIO_CLOCK_GATE_REG (DR_REG_GPIO_BASE + 0xdf8)
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
#define GPIO_DATE_REG (DR_REG_GPIO_BASE + 0xdfc)
/** GPIO_DATE : R/W; bitpos: [27:0]; default: 37823120;
 *  version register
 */
#define GPIO_DATE    0x0FFFFFFFU
#define GPIO_DATE_M  (GPIO_DATE_V << GPIO_DATE_S)
#define GPIO_DATE_V  0x0FFFFFFFU
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
