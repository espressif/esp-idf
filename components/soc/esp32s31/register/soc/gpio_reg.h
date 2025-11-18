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
 *  Strapping pin register
 */
#define GPIO_STRAP_REG (DR_REG_GPIO_BASE + 0x0)
/** GPIO_STRAPPING : RO; bitpos: [15:0]; default: 0;
 *  Represents the values of GPIO strapping pins. (need update the description, for
 *  example)
 *
 *  - bit0: invalid
 *  - bit1: MTMS
 *  - bit2: MTDI
 *  - bit3: GPIO27
 *  - bit4: GPIO28
 *  - bit5: GPIO7
 *  - bit6 ~ bit15: invalid
 */
#define GPIO_STRAPPING    0x0000FFFFU
#define GPIO_STRAPPING_M  (GPIO_STRAPPING_V << GPIO_STRAPPING_S)
#define GPIO_STRAPPING_V  0x0000FFFFU
#define GPIO_STRAPPING_S  0

/** GPIO_OUT_REG register
 *  GPIO output register
 */
#define GPIO_OUT_REG (DR_REG_GPIO_BASE + 0x4)
/** GPIO_OUT_DATA_ORIG : R/W/SC/WTC; bitpos: [31:0]; default: 0;
 *  Configures the output value of GPIO0 ~ 31 output in simple GPIO output mode.
 *  0: Low level
 *  1: High level
 *  The value of bit0 ~ bit31 correspond to the output value of GPIO0 ~ GPIO31
 *  respectively. Bitxx ~ bitxx is invalid.
 */
#define GPIO_OUT_DATA_ORIG    0xFFFFFFFFU
#define GPIO_OUT_DATA_ORIG_M  (GPIO_OUT_DATA_ORIG_V << GPIO_OUT_DATA_ORIG_S)
#define GPIO_OUT_DATA_ORIG_V  0xFFFFFFFFU
#define GPIO_OUT_DATA_ORIG_S  0

/** GPIO_OUT_W1TS_REG register
 *  GPIO output set register
 */
#define GPIO_OUT_W1TS_REG (DR_REG_GPIO_BASE + 0x8)
/** GPIO_OUT_W1TS : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to set the output register GPIO_OUT_REG of GPIO0 ~ GPIO31.
 *  0: Not set
 *  1: The corresponding bit in GPIO_OUT_REG will be set to 1
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to set GPIO_OUT_REG.
 */
#define GPIO_OUT_W1TS    0xFFFFFFFFU
#define GPIO_OUT_W1TS_M  (GPIO_OUT_W1TS_V << GPIO_OUT_W1TS_S)
#define GPIO_OUT_W1TS_V  0xFFFFFFFFU
#define GPIO_OUT_W1TS_S  0

/** GPIO_OUT_W1TC_REG register
 *  GPIO output clear register
 */
#define GPIO_OUT_W1TC_REG (DR_REG_GPIO_BASE + 0xc)
/** GPIO_OUT_W1TC : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to clear the output register GPIO_OUT_REG of GPIO0 ~
 *  GPIO31 output.
 *  0: Not clear
 *  1: The corresponding bit in GPIO_OUT_REG will be cleared.
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to clear GPIO_OUT_REG.
 */
#define GPIO_OUT_W1TC    0xFFFFFFFFU
#define GPIO_OUT_W1TC_M  (GPIO_OUT_W1TC_V << GPIO_OUT_W1TC_S)
#define GPIO_OUT_W1TC_V  0xFFFFFFFFU
#define GPIO_OUT_W1TC_S  0

/** GPIO_OUT1_REG register
 *  GPIO output register
 */
#define GPIO_OUT1_REG (DR_REG_GPIO_BASE + 0x10)
/** GPIO_OUT1_DATA_ORIG : R/W/SC/WTC; bitpos: [31:0]; default: 0;
 *  Configures the output value of GPIO32 ~ 63 output in simple GPIO output mode.
 *  0: Low level
 *  1: High level
 *  The value of bit32 ~ bit63 correspond to the output value of GPIO32 ~ GPIO63
 *  respectively. Bitxx ~ bitxx is invalid.
 */
#define GPIO_OUT1_DATA_ORIG    0xFFFFFFFFU
#define GPIO_OUT1_DATA_ORIG_M  (GPIO_OUT1_DATA_ORIG_V << GPIO_OUT1_DATA_ORIG_S)
#define GPIO_OUT1_DATA_ORIG_V  0xFFFFFFFFU
#define GPIO_OUT1_DATA_ORIG_S  0

/** GPIO_OUT1_W1TS_REG register
 *  GPIO output set register
 */
#define GPIO_OUT1_W1TS_REG (DR_REG_GPIO_BASE + 0x14)
/** GPIO_OUT1_W1TS : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to set the output register GPIO_OUT1_REG of GPIO32 ~
 *  GPIO63.
 *  0: Not set
 *  1: The corresponding bit in GPIO_OUT1_REG will be set to 1
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to set GPIO_OUT1_REG.
 */
#define GPIO_OUT1_W1TS    0xFFFFFFFFU
#define GPIO_OUT1_W1TS_M  (GPIO_OUT1_W1TS_V << GPIO_OUT1_W1TS_S)
#define GPIO_OUT1_W1TS_V  0xFFFFFFFFU
#define GPIO_OUT1_W1TS_S  0

/** GPIO_OUT1_W1TC_REG register
 *  GPIO output clear register
 */
#define GPIO_OUT1_W1TC_REG (DR_REG_GPIO_BASE + 0x18)
/** GPIO_OUT1_W1TC : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to clear the output register GPIO_OUT1_REG of GPIO32 ~
 *  GPIO63 output.
 *  0: Not clear
 *  1: The corresponding bit in GPIO_OUT1_REG will be cleared.
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to clear GPIO_OUT1_REG.
 */
#define GPIO_OUT1_W1TC    0xFFFFFFFFU
#define GPIO_OUT1_W1TC_M  (GPIO_OUT1_W1TC_V << GPIO_OUT1_W1TC_S)
#define GPIO_OUT1_W1TC_V  0xFFFFFFFFU
#define GPIO_OUT1_W1TC_S  0

/** GPIO_OUT2_REG register
 *  GPIO output register
 */
#define GPIO_OUT2_REG (DR_REG_GPIO_BASE + 0x1c)
/** GPIO_OUT2_DATA_ORIG : R/W/SC/WTC; bitpos: [2:0]; default: 0;
 *  Configures the output value of GPIO64 ~ 66 output in simple GPIO output mode.
 *  0: Low level
 *  1: High level
 *  The value of bit64 ~ bit66 correspond to the output value of GPIO64 ~ GPIO66
 *  respectively. Bitxx ~ bitxx is invalid.
 */
#define GPIO_OUT2_DATA_ORIG    0x00000007U
#define GPIO_OUT2_DATA_ORIG_M  (GPIO_OUT2_DATA_ORIG_V << GPIO_OUT2_DATA_ORIG_S)
#define GPIO_OUT2_DATA_ORIG_V  0x00000007U
#define GPIO_OUT2_DATA_ORIG_S  0

/** GPIO_OUT2_W1TS_REG register
 *  GPIO output set register
 */
#define GPIO_OUT2_W1TS_REG (DR_REG_GPIO_BASE + 0x20)
/** GPIO_OUT2_W1TS : WT; bitpos: [2:0]; default: 0;
 *  Configures whether or not to set the output register GPIO_OUT2_REG of GPIO64 ~
 *  GPIO66.
 *  0: Not set
 *  1: The corresponding bit in GPIO_OUT2_REG will be set to 1
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to set GPIO_OUT2_REG.
 */
#define GPIO_OUT2_W1TS    0x00000007U
#define GPIO_OUT2_W1TS_M  (GPIO_OUT2_W1TS_V << GPIO_OUT2_W1TS_S)
#define GPIO_OUT2_W1TS_V  0x00000007U
#define GPIO_OUT2_W1TS_S  0

/** GPIO_OUT2_W1TC_REG register
 *  GPIO output clear register
 */
#define GPIO_OUT2_W1TC_REG (DR_REG_GPIO_BASE + 0x24)
/** GPIO_OUT2_W1TC : WT; bitpos: [2:0]; default: 0;
 *  Configures whether or not to clear the output register GPIO_OUT2_REG of GPIO64 ~
 *  GPIO66 output.
 *  0: Not clear
 *  1: The corresponding bit in GPIO_OUT2_REG will be cleared.
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to clear GPIO_OUT2_REG.
 */
#define GPIO_OUT2_W1TC    0x00000007U
#define GPIO_OUT2_W1TC_M  (GPIO_OUT2_W1TC_V << GPIO_OUT2_W1TC_S)
#define GPIO_OUT2_W1TC_V  0x00000007U
#define GPIO_OUT2_W1TC_S  0

/** GPIO_ENABLE_REG register
 *  GPIO output enable register
 */
#define GPIO_ENABLE_REG (DR_REG_GPIO_BASE + 0x34)
/** GPIO_ENABLE_DATA : R/W/WTC; bitpos: [31:0]; default: 0;
 *  Configures whether or not to enable the output of GPIO0 ~ GPIO31.
 *  0: Not enable
 *  1: Enable
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 */
#define GPIO_ENABLE_DATA    0xFFFFFFFFU
#define GPIO_ENABLE_DATA_M  (GPIO_ENABLE_DATA_V << GPIO_ENABLE_DATA_S)
#define GPIO_ENABLE_DATA_V  0xFFFFFFFFU
#define GPIO_ENABLE_DATA_S  0

/** GPIO_ENABLE_W1TS_REG register
 *  GPIO output enable set register
 */
#define GPIO_ENABLE_W1TS_REG (DR_REG_GPIO_BASE + 0x38)
/** GPIO_ENABLE_W1TS : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to set the output enable register GPIO_ENABLE_REG of
 *  GPIO0 ~ GPIO31.
 *  0: Not set
 *  1: The corresponding bit in GPIO_ENABLE_REG will be set to 1
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to set GPIO_ENABLE_REG.
 */
#define GPIO_ENABLE_W1TS    0xFFFFFFFFU
#define GPIO_ENABLE_W1TS_M  (GPIO_ENABLE_W1TS_V << GPIO_ENABLE_W1TS_S)
#define GPIO_ENABLE_W1TS_V  0xFFFFFFFFU
#define GPIO_ENABLE_W1TS_S  0

/** GPIO_ENABLE_W1TC_REG register
 *  GPIO output enable clear register
 */
#define GPIO_ENABLE_W1TC_REG (DR_REG_GPIO_BASE + 0x3c)
/** GPIO_ENABLE_W1TC : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to clear the output enable register GPIO_ENABLE_REG of
 *  GPIO0 ~ GPIO31.
 *  0: Not clear
 *  1: The corresponding bit in GPIO_ENABLE_REG will be cleared
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to clear GPIO_ENABLE_REG.
 */
#define GPIO_ENABLE_W1TC    0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_M  (GPIO_ENABLE_W1TC_V << GPIO_ENABLE_W1TC_S)
#define GPIO_ENABLE_W1TC_V  0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_S  0

/** GPIO_ENABLE1_REG register
 *  GPIO output enable register
 */
#define GPIO_ENABLE1_REG (DR_REG_GPIO_BASE + 0x40)
/** GPIO_ENABLE1_DATA : R/W/WTC; bitpos: [31:0]; default: 0;
 *  Configures whether or not to enable the output of GPIO32 ~ GPIO63.
 *  0: Not enable
 *  1: Enable
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 */
#define GPIO_ENABLE1_DATA    0xFFFFFFFFU
#define GPIO_ENABLE1_DATA_M  (GPIO_ENABLE1_DATA_V << GPIO_ENABLE1_DATA_S)
#define GPIO_ENABLE1_DATA_V  0xFFFFFFFFU
#define GPIO_ENABLE1_DATA_S  0

/** GPIO_ENABLE1_W1TS_REG register
 *  GPIO output enable set register
 */
#define GPIO_ENABLE1_W1TS_REG (DR_REG_GPIO_BASE + 0x44)
/** GPIO_ENABLE1_W1TS : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to set the output enable register GPIO_ENABLE1_REG of
 *  GPIO32 ~ GPIO63.
 *  0: Not set
 *  1: The corresponding bit in GPIO_ENABLE1_REG will be set to 1
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to set GPIO_ENABLE1_REG.
 */
#define GPIO_ENABLE1_W1TS    0xFFFFFFFFU
#define GPIO_ENABLE1_W1TS_M  (GPIO_ENABLE1_W1TS_V << GPIO_ENABLE1_W1TS_S)
#define GPIO_ENABLE1_W1TS_V  0xFFFFFFFFU
#define GPIO_ENABLE1_W1TS_S  0

/** GPIO_ENABLE1_W1TC_REG register
 *  GPIO output enable clear register
 */
#define GPIO_ENABLE1_W1TC_REG (DR_REG_GPIO_BASE + 0x48)
/** GPIO_ENABLE1_W1TC : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to clear the output enable register GPIO_ENABLE1_REG of
 *  GPIO32 ~ GPIO63.
 *  0: Not clear
 *  1: The corresponding bit in GPIO_ENABLE1_REG will be cleared
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to clear GPIO_ENABLE1_REG.
 */
#define GPIO_ENABLE1_W1TC    0xFFFFFFFFU
#define GPIO_ENABLE1_W1TC_M  (GPIO_ENABLE1_W1TC_V << GPIO_ENABLE1_W1TC_S)
#define GPIO_ENABLE1_W1TC_V  0xFFFFFFFFU
#define GPIO_ENABLE1_W1TC_S  0

/** GPIO_ENABLE2_REG register
 *  GPIO output enable register
 */
#define GPIO_ENABLE2_REG (DR_REG_GPIO_BASE + 0x4c)
/** GPIO_ENABLE2_DATA : R/W/WTC; bitpos: [2:0]; default: 0;
 *  Configures whether or not to enable the output of GPIO64 ~ GPIO66.
 *  0: Not enable
 *  1: Enable
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 */
#define GPIO_ENABLE2_DATA    0x00000007U
#define GPIO_ENABLE2_DATA_M  (GPIO_ENABLE2_DATA_V << GPIO_ENABLE2_DATA_S)
#define GPIO_ENABLE2_DATA_V  0x00000007U
#define GPIO_ENABLE2_DATA_S  0

/** GPIO_ENABLE2_W1TS_REG register
 *  GPIO output enable set register
 */
#define GPIO_ENABLE2_W1TS_REG (DR_REG_GPIO_BASE + 0x50)
/** GPIO_ENABLE2_W1TS : WT; bitpos: [2:0]; default: 0;
 *  Configures whether or not to set the output enable register GPIO_ENABLE2_REG of
 *  GPIO64 ~ GPIO66.
 *  0: Not set
 *  1: The corresponding bit in GPIO_ENABLE2_REG will be set to 1
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to set GPIO_ENABLE2_REG.
 */
#define GPIO_ENABLE2_W1TS    0x00000007U
#define GPIO_ENABLE2_W1TS_M  (GPIO_ENABLE2_W1TS_V << GPIO_ENABLE2_W1TS_S)
#define GPIO_ENABLE2_W1TS_V  0x00000007U
#define GPIO_ENABLE2_W1TS_S  0

/** GPIO_ENABLE2_W1TC_REG register
 *  GPIO output enable clear register
 */
#define GPIO_ENABLE2_W1TC_REG (DR_REG_GPIO_BASE + 0x54)
/** GPIO_ENABLE2_W1TC : WT; bitpos: [2:0]; default: 0;
 *  Configures whether or not to clear the output enable register GPIO_ENABLE2_REG of
 *  GPIO64 ~ GPIO66.
 *  0: Not clear
 *  1: The corresponding bit in GPIO_ENABLE2_REG will be cleared
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  Recommended operation: use this register to clear GPIO_ENABLE2_REG.
 */
#define GPIO_ENABLE2_W1TC    0x00000007U
#define GPIO_ENABLE2_W1TC_M  (GPIO_ENABLE2_W1TC_V << GPIO_ENABLE2_W1TC_S)
#define GPIO_ENABLE2_W1TC_V  0x00000007U
#define GPIO_ENABLE2_W1TC_S  0

/** GPIO_IN_REG register
 *  GPIO input register
 */
#define GPIO_IN_REG (DR_REG_GPIO_BASE + 0x64)
/** GPIO_IN_DATA_NEXT : RO; bitpos: [31:0]; default: 0;
 *  Represents the input value of GPIO0 ~ GPIO31. Each bit represents a pin input value:
 *  0: Low level
 *  1: High level
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 */
#define GPIO_IN_DATA_NEXT    0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_M  (GPIO_IN_DATA_NEXT_V << GPIO_IN_DATA_NEXT_S)
#define GPIO_IN_DATA_NEXT_V  0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_S  0

/** GPIO_IN1_REG register
 *  GPIO input register
 */
#define GPIO_IN1_REG (DR_REG_GPIO_BASE + 0x68)
/** GPIO_IN1_DATA_NEXT : RO; bitpos: [31:0]; default: 0;
 *  Represents the input value of GPIO32 ~ GPIO63. Each bit represents a pin input
 *  value:
 *  0: Low level
 *  1: High level
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 */
#define GPIO_IN1_DATA_NEXT    0xFFFFFFFFU
#define GPIO_IN1_DATA_NEXT_M  (GPIO_IN1_DATA_NEXT_V << GPIO_IN1_DATA_NEXT_S)
#define GPIO_IN1_DATA_NEXT_V  0xFFFFFFFFU
#define GPIO_IN1_DATA_NEXT_S  0

/** GPIO_IN2_REG register
 *  GPIO input register
 */
#define GPIO_IN2_REG (DR_REG_GPIO_BASE + 0x6c)
/** GPIO_IN2_DATA_NEXT : RO; bitpos: [2:0]; default: 0;
 *  Represents the input value of GPIO64 ~ GPIO66. Each bit represents a pin input
 *  value:
 *  0: Low level
 *  1: High level
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 */
#define GPIO_IN2_DATA_NEXT    0x00000007U
#define GPIO_IN2_DATA_NEXT_M  (GPIO_IN2_DATA_NEXT_V << GPIO_IN2_DATA_NEXT_S)
#define GPIO_IN2_DATA_NEXT_V  0x00000007U
#define GPIO_IN2_DATA_NEXT_S  0

/** GPIO_STATUS_REG register
 *  GPIO interrupt status register
 */
#define GPIO_STATUS_REG (DR_REG_GPIO_BASE + 0x74)
/** GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [31:0]; default: 0;
 *  The interrupt status of GPIO0 ~ GPIO31, can be configured by the software.
 *
 *  - Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  - Each bit represents the status of its corresponding GPIO:
 *
 *  - 0: Represents the GPIO does not generate the interrupt configured by
 *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
 *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
 *  or this bit is configured to 1 by the software.
 *
 */
#define GPIO_STATUS_INTERRUPT    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_M  (GPIO_STATUS_INTERRUPT_V << GPIO_STATUS_INTERRUPT_S)
#define GPIO_STATUS_INTERRUPT_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_S  0

/** GPIO_STATUS_W1TS_REG register
 *  GPIO interrupt status set register
 */
#define GPIO_STATUS_W1TS_REG (DR_REG_GPIO_BASE + 0x78)
/** GPIO_STATUS_W1TS : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to set the interrupt status register
 *  GPIO_STATUS_INTERRUPT of GPIO0 ~ GPIO31.
 *
 *  - Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be set to 1. \item Recommended operation: use this
 *  register to set GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS_W1TS    0xFFFFFFFFU
#define GPIO_STATUS_W1TS_M  (GPIO_STATUS_W1TS_V << GPIO_STATUS_W1TS_S)
#define GPIO_STATUS_W1TS_V  0xFFFFFFFFU
#define GPIO_STATUS_W1TS_S  0

/** GPIO_STATUS_W1TC_REG register
 *  GPIO interrupt status clear register
 */
#define GPIO_STATUS_W1TC_REG (DR_REG_GPIO_BASE + 0x7c)
/** GPIO_STATUS_W1TC : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to clear the interrupt status register
 *  GPIO_STATUS_INTERRUPT of GPIO0 ~ GPIO31.
 *
 *  - Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS_W1TC    0xFFFFFFFFU
#define GPIO_STATUS_W1TC_M  (GPIO_STATUS_W1TC_V << GPIO_STATUS_W1TC_S)
#define GPIO_STATUS_W1TC_V  0xFFFFFFFFU
#define GPIO_STATUS_W1TC_S  0

/** GPIO_STATUS1_REG register
 *  GPIO interrupt status register
 */
#define GPIO_STATUS1_REG (DR_REG_GPIO_BASE + 0x80)
/** GPIO_STATUS1_INTERRUPT : R/W/WTC; bitpos: [31:0]; default: 0;
 *  The interrupt status of GPIO32 ~ GPIO63, can be configured by the software.
 *
 *  - Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  - Each bit represents the status of its corresponding GPIO:
 *
 *  - 0: Represents the GPIO does not generate the interrupt configured by
 *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
 *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
 *  or this bit is configured to 1 by the software.
 *
 */
#define GPIO_STATUS1_INTERRUPT    0xFFFFFFFFU
#define GPIO_STATUS1_INTERRUPT_M  (GPIO_STATUS1_INTERRUPT_V << GPIO_STATUS1_INTERRUPT_S)
#define GPIO_STATUS1_INTERRUPT_V  0xFFFFFFFFU
#define GPIO_STATUS1_INTERRUPT_S  0

/** GPIO_STATUS1_W1TS_REG register
 *  GPIO interrupt status set register
 */
#define GPIO_STATUS1_W1TS_REG (DR_REG_GPIO_BASE + 0x84)
/** GPIO_STATUS1_W1TS : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to set the interrupt status register
 *  GPIO_STATUS1_INTERRUPT of GPIO32 ~ GPIO63.
 *
 *  - Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS1_INTERRUPT will be set to 1. \item Recommended operation: use this
 *  register to set GPIO_STATUS1_INTERRUPT.
 */
#define GPIO_STATUS1_W1TS    0xFFFFFFFFU
#define GPIO_STATUS1_W1TS_M  (GPIO_STATUS1_W1TS_V << GPIO_STATUS1_W1TS_S)
#define GPIO_STATUS1_W1TS_V  0xFFFFFFFFU
#define GPIO_STATUS1_W1TS_S  0

/** GPIO_STATUS1_W1TC_REG register
 *  GPIO interrupt status clear register
 */
#define GPIO_STATUS1_W1TC_REG (DR_REG_GPIO_BASE + 0x88)
/** GPIO_STATUS1_W1TC : WT; bitpos: [31:0]; default: 0;
 *  Configures whether or not to clear the interrupt status register
 *  GPIO_STATUS1_INTERRUPT of GPIO32 ~ GPIO63.
 *
 *  - Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS1_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear GPIO_STATUS1_INTERRUPT.
 */
#define GPIO_STATUS1_W1TC    0xFFFFFFFFU
#define GPIO_STATUS1_W1TC_M  (GPIO_STATUS1_W1TC_V << GPIO_STATUS1_W1TC_S)
#define GPIO_STATUS1_W1TC_V  0xFFFFFFFFU
#define GPIO_STATUS1_W1TC_S  0

/** GPIO_STATUS2_REG register
 *  GPIO interrupt status register
 */
#define GPIO_STATUS2_REG (DR_REG_GPIO_BASE + 0x8c)
/** GPIO_STATUS2_INTERRUPT : R/W/WTC; bitpos: [2:0]; default: 0;
 *  The interrupt status of GPIO64 ~ GPIO66, can be configured by the software.
 *
 *  - Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  - Each bit represents the status of its corresponding GPIO:
 *
 *  - 0: Represents the GPIO does not generate the interrupt configured by
 *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
 *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
 *  or this bit is configured to 1 by the software.
 *
 */
#define GPIO_STATUS2_INTERRUPT    0x00000007U
#define GPIO_STATUS2_INTERRUPT_M  (GPIO_STATUS2_INTERRUPT_V << GPIO_STATUS2_INTERRUPT_S)
#define GPIO_STATUS2_INTERRUPT_V  0x00000007U
#define GPIO_STATUS2_INTERRUPT_S  0

/** GPIO_STATUS2_W1TS_REG register
 *  GPIO interrupt status set register
 */
#define GPIO_STATUS2_W1TS_REG (DR_REG_GPIO_BASE + 0x90)
/** GPIO_STATUS2_W1TS : WT; bitpos: [2:0]; default: 0;
 *  Configures whether or not to set the interrupt status register
 *  GPIO_STATUS2_INTERRUPT of GPIO64 ~ GPIO66.
 *
 *  - Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS2_INTERRUPT will be set to 1. \item Recommended operation: use this
 *  register to set GPIO_STATUS2_INTERRUPT.
 */
#define GPIO_STATUS2_W1TS    0x00000007U
#define GPIO_STATUS2_W1TS_M  (GPIO_STATUS2_W1TS_V << GPIO_STATUS2_W1TS_S)
#define GPIO_STATUS2_W1TS_V  0x00000007U
#define GPIO_STATUS2_W1TS_S  0

/** GPIO_STATUS2_W1TC_REG register
 *  GPIO interrupt status clear register
 */
#define GPIO_STATUS2_W1TC_REG (DR_REG_GPIO_BASE + 0x94)
/** GPIO_STATUS2_W1TC : WT; bitpos: [2:0]; default: 0;
 *  Configures whether or not to clear the interrupt status register
 *  GPIO_STATUS2_INTERRUPT of GPIO64 ~ GPIO66.
 *
 *  - Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS2_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear GPIO_STATUS2_INTERRUPT.
 */
#define GPIO_STATUS2_W1TC    0x00000007U
#define GPIO_STATUS2_W1TC_M  (GPIO_STATUS2_W1TC_V << GPIO_STATUS2_W1TC_S)
#define GPIO_STATUS2_W1TC_V  0x00000007U
#define GPIO_STATUS2_W1TC_S  0

/** GPIO_INT_0_REG register
 *  GPIO_INT_0 interrupt status register
 */
#define GPIO_INT_0_REG (DR_REG_GPIO_BASE + 0xa4)
/** GPIO_INT_0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_0 interrupt status of GPIO0 ~ GPIO31. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_0 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_0 interrupt is enabled.
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_0    0xFFFFFFFFU
#define GPIO_INT_0_M  (GPIO_INT_0_V << GPIO_INT_0_S)
#define GPIO_INT_0_V  0xFFFFFFFFU
#define GPIO_INT_0_S  0

/** GPIO_INT_1_REG register
 *  GPIO_INT_1 interrupt status register
 */
#define GPIO_INT_1_REG (DR_REG_GPIO_BASE + 0xa8)
/** GPIO_INT_1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_1 interrupt status of GPIO0 ~ GPIO31. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_1 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_1 interrupt is enabled.
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_1    0xFFFFFFFFU
#define GPIO_INT_1_M  (GPIO_INT_1_V << GPIO_INT_1_S)
#define GPIO_INT_1_V  0xFFFFFFFFU
#define GPIO_INT_1_S  0

/** GPIO_INT_2_REG register
 *  GPIO_INT_2 interrupt status register
 */
#define GPIO_INT_2_REG (DR_REG_GPIO_BASE + 0xac)
/** GPIO_INT_2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_2 interrupt status of GPIO0 ~ GPIO31. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_2 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_2 interrupt is enabled.
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_2    0xFFFFFFFFU
#define GPIO_INT_2_M  (GPIO_INT_2_V << GPIO_INT_2_S)
#define GPIO_INT_2_V  0xFFFFFFFFU
#define GPIO_INT_2_S  0

/** GPIO_INT_3_REG register
 *  GPIO_INT_3 interrupt status register
 */
#define GPIO_INT_3_REG (DR_REG_GPIO_BASE + 0xb0)
/** GPIO_INT_3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_3 interrupt status of GPIO0 ~ GPIO31. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_3 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_3 interrupt is enabled.
 *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_3    0xFFFFFFFFU
#define GPIO_INT_3_M  (GPIO_INT_3_V << GPIO_INT_3_S)
#define GPIO_INT_3_V  0xFFFFFFFFU
#define GPIO_INT_3_S  0

/** GPIO_INT_01_REG register
 *  GPIO_INT_0 interrupt status register
 */
#define GPIO_INT_01_REG (DR_REG_GPIO_BASE + 0xb4)
/** GPIO_INT_01 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_0 interrupt status of GPIO32 ~ GPIO63. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_0 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_0 interrupt is enabled.
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_01    0xFFFFFFFFU
#define GPIO_INT_01_M  (GPIO_INT_01_V << GPIO_INT_01_S)
#define GPIO_INT_01_V  0xFFFFFFFFU
#define GPIO_INT_01_S  0

/** GPIO_INT_11_REG register
 *  GPIO_INT_1 interrupt status register
 */
#define GPIO_INT_11_REG (DR_REG_GPIO_BASE + 0xb8)
/** GPIO_INT_11 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_1 interrupt status of GPIO32 ~ GPIO63. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_1 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_1 interrupt is enabled.
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_11    0xFFFFFFFFU
#define GPIO_INT_11_M  (GPIO_INT_11_V << GPIO_INT_11_S)
#define GPIO_INT_11_V  0xFFFFFFFFU
#define GPIO_INT_11_S  0

/** GPIO_INT_21_REG register
 *  GPIO_INT_2 interrupt status register
 */
#define GPIO_INT_21_REG (DR_REG_GPIO_BASE + 0xbc)
/** GPIO_INT_21 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_2 interrupt status of GPIO32 ~ GPIO63. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_2 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_2 interrupt is enabled.
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_21    0xFFFFFFFFU
#define GPIO_INT_21_M  (GPIO_INT_21_V << GPIO_INT_21_S)
#define GPIO_INT_21_V  0xFFFFFFFFU
#define GPIO_INT_21_S  0

/** GPIO_INT_31_REG register
 *  GPIO_INT_3 interrupt status register
 */
#define GPIO_INT_31_REG (DR_REG_GPIO_BASE + 0xc0)
/** GPIO_INT_31 : RO; bitpos: [31:0]; default: 0;
 *  Represents the GPIO_INT_3 interrupt status of GPIO32 ~ GPIO63. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_3 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_3 interrupt is enabled.
 *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_31    0xFFFFFFFFU
#define GPIO_INT_31_M  (GPIO_INT_31_V << GPIO_INT_31_S)
#define GPIO_INT_31_V  0xFFFFFFFFU
#define GPIO_INT_31_S  0

/** GPIO_INT_02_REG register
 *  GPIO_INT_0 interrupt status register
 */
#define GPIO_INT_02_REG (DR_REG_GPIO_BASE + 0xc4)
/** GPIO_INT_02 : RO; bitpos: [2:0]; default: 0;
 *  Represents the GPIO_INT_0 interrupt status of GPIO64 ~ GPIO66. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_0 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_0 interrupt is enabled.
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_02    0x00000007U
#define GPIO_INT_02_M  (GPIO_INT_02_V << GPIO_INT_02_S)
#define GPIO_INT_02_V  0x00000007U
#define GPIO_INT_02_S  0

/** GPIO_INT_12_REG register
 *  GPIO_INT_1 interrupt status register
 */
#define GPIO_INT_12_REG (DR_REG_GPIO_BASE + 0xc8)
/** GPIO_INT_12 : RO; bitpos: [2:0]; default: 0;
 *  Represents the GPIO_INT_1 interrupt status of GPIO64 ~ GPIO66. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_1 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_1 interrupt is enabled.
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_12    0x00000007U
#define GPIO_INT_12_M  (GPIO_INT_12_V << GPIO_INT_12_S)
#define GPIO_INT_12_V  0x00000007U
#define GPIO_INT_12_S  0

/** GPIO_INT_22_REG register
 *  GPIO_INT_2 interrupt status register
 */
#define GPIO_INT_22_REG (DR_REG_GPIO_BASE + 0xcc)
/** GPIO_INT_22 : RO; bitpos: [2:0]; default: 0;
 *  Represents the GPIO_INT_2 interrupt status of GPIO64 ~ GPIO66. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_2 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_2 interrupt is enabled.
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_22    0x00000007U
#define GPIO_INT_22_M  (GPIO_INT_22_V << GPIO_INT_22_S)
#define GPIO_INT_22_V  0x00000007U
#define GPIO_INT_22_S  0

/** GPIO_INT_32_REG register
 *  GPIO_INT_3 interrupt status register
 */
#define GPIO_INT_32_REG (DR_REG_GPIO_BASE + 0xd0)
/** GPIO_INT_32 : RO; bitpos: [2:0]; default: 0;
 *  Represents the GPIO_INT_3 interrupt status of GPIO64 ~ GPIO66. Each bit
 *  represents:(need update in different project)
 *  0: Represents GPIO_INT_3 interrupt is not enabled, or the GPIO does not generate
 *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the GPIO_INT_3 interrupt is enabled.
 *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG).
 */
#define GPIO_INT_32    0x00000007U
#define GPIO_INT_32_M  (GPIO_INT_32_V << GPIO_INT_32_S)
#define GPIO_INT_32_V  0x00000007U
#define GPIO_INT_32_S  0

/** GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register
 */
#define GPIO_STATUS_NEXT_REG (DR_REG_GPIO_BASE + 0xe4)
/** GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [31:0]; default: 0;
 *  Represents the interrupt source signal of GPIO0 ~ GPIO31.
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. Each
 *  bit represents:
 *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.
 */
#define GPIO_STATUS_INTERRUPT_NEXT    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_M  (GPIO_STATUS_INTERRUPT_NEXT_V << GPIO_STATUS_INTERRUPT_NEXT_S)
#define GPIO_STATUS_INTERRUPT_NEXT_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

/** GPIO_STATUS_NEXT1_REG register
 *  GPIO interrupt source register
 */
#define GPIO_STATUS_NEXT1_REG (DR_REG_GPIO_BASE + 0xe8)
/** GPIO_STATUS_INTERRUPT_NEXT1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the interrupt source signal of GPIO32 ~ GPIO63.
 *  Bit0 ~ bit24 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. Each
 *  bit represents:
 *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.
 */
#define GPIO_STATUS_INTERRUPT_NEXT1    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT1_M  (GPIO_STATUS_INTERRUPT_NEXT1_V << GPIO_STATUS_INTERRUPT_NEXT1_S)
#define GPIO_STATUS_INTERRUPT_NEXT1_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT1_S  0

/** GPIO_STATUS_NEXT2_REG register
 *  GPIO interrupt source register
 */
#define GPIO_STATUS_NEXT2_REG (DR_REG_GPIO_BASE + 0xec)
/** GPIO_STATUS_INTERRUPT_NEXT2 : RO; bitpos: [2:0]; default: 0;
 *  Represents the interrupt source signal of GPIO64 ~ GPIO66.
 *  Bit0 ~ bit24 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. Each
 *  bit represents:
 *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.
 */
#define GPIO_STATUS_INTERRUPT_NEXT2    0x00000007U
#define GPIO_STATUS_INTERRUPT_NEXT2_M  (GPIO_STATUS_INTERRUPT_NEXT2_V << GPIO_STATUS_INTERRUPT_NEXT2_S)
#define GPIO_STATUS_INTERRUPT_NEXT2_V  0x00000007U
#define GPIO_STATUS_INTERRUPT_NEXT2_S  0

/** GPIO_PIN0_REG register
 *  GPIO0 configuration register
 */
#define GPIO_PIN0_REG (DR_REG_GPIO_BASE + 0xf4)
/** GPIO_PIN0_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN0_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN0_SYNC2_BYPASS_M  (GPIO_PIN0_SYNC2_BYPASS_V << GPIO_PIN0_SYNC2_BYPASS_S)
#define GPIO_PIN0_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN0_SYNC2_BYPASS_S  0
/** GPIO_PIN0_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_M  (GPIO_PIN0_PAD_DRIVER_V << GPIO_PIN0_PAD_DRIVER_S)
#define GPIO_PIN0_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN0_PAD_DRIVER_S  2
/** GPIO_PIN0_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN0_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN0_SYNC1_BYPASS_M  (GPIO_PIN0_SYNC1_BYPASS_V << GPIO_PIN0_SYNC1_BYPASS_S)
#define GPIO_PIN0_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN0_SYNC1_BYPASS_S  3
/** GPIO_PIN0_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN0_INT_TYPE    0x00000007U
#define GPIO_PIN0_INT_TYPE_M  (GPIO_PIN0_INT_TYPE_V << GPIO_PIN0_INT_TYPE_S)
#define GPIO_PIN0_INT_TYPE_V  0x00000007U
#define GPIO_PIN0_INT_TYPE_S  7
/** GPIO_PIN0_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_M  (GPIO_PIN0_WAKEUP_ENABLE_V << GPIO_PIN0_WAKEUP_ENABLE_S)
#define GPIO_PIN0_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN0_WAKEUP_ENABLE_S  10
/** GPIO_PIN0_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN0_INT_ENA    0x0000001FU
#define GPIO_PIN0_INT_ENA_M  (GPIO_PIN0_INT_ENA_V << GPIO_PIN0_INT_ENA_S)
#define GPIO_PIN0_INT_ENA_V  0x0000001FU
#define GPIO_PIN0_INT_ENA_S  13

/** GPIO_PIN1_REG register
 *  GPIO1 configuration register
 */
#define GPIO_PIN1_REG (DR_REG_GPIO_BASE + 0xf8)
/** GPIO_PIN1_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN1_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN1_SYNC2_BYPASS_M  (GPIO_PIN1_SYNC2_BYPASS_V << GPIO_PIN1_SYNC2_BYPASS_S)
#define GPIO_PIN1_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN1_SYNC2_BYPASS_S  0
/** GPIO_PIN1_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_M  (GPIO_PIN1_PAD_DRIVER_V << GPIO_PIN1_PAD_DRIVER_S)
#define GPIO_PIN1_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN1_PAD_DRIVER_S  2
/** GPIO_PIN1_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN1_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN1_SYNC1_BYPASS_M  (GPIO_PIN1_SYNC1_BYPASS_V << GPIO_PIN1_SYNC1_BYPASS_S)
#define GPIO_PIN1_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN1_SYNC1_BYPASS_S  3
/** GPIO_PIN1_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN1_INT_TYPE    0x00000007U
#define GPIO_PIN1_INT_TYPE_M  (GPIO_PIN1_INT_TYPE_V << GPIO_PIN1_INT_TYPE_S)
#define GPIO_PIN1_INT_TYPE_V  0x00000007U
#define GPIO_PIN1_INT_TYPE_S  7
/** GPIO_PIN1_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_M  (GPIO_PIN1_WAKEUP_ENABLE_V << GPIO_PIN1_WAKEUP_ENABLE_S)
#define GPIO_PIN1_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN1_WAKEUP_ENABLE_S  10
/** GPIO_PIN1_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN1_INT_ENA    0x0000001FU
#define GPIO_PIN1_INT_ENA_M  (GPIO_PIN1_INT_ENA_V << GPIO_PIN1_INT_ENA_S)
#define GPIO_PIN1_INT_ENA_V  0x0000001FU
#define GPIO_PIN1_INT_ENA_S  13

/** GPIO_PIN2_REG register
 *  GPIO2 configuration register
 */
#define GPIO_PIN2_REG (DR_REG_GPIO_BASE + 0xfc)
/** GPIO_PIN2_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN2_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN2_SYNC2_BYPASS_M  (GPIO_PIN2_SYNC2_BYPASS_V << GPIO_PIN2_SYNC2_BYPASS_S)
#define GPIO_PIN2_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN2_SYNC2_BYPASS_S  0
/** GPIO_PIN2_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_M  (GPIO_PIN2_PAD_DRIVER_V << GPIO_PIN2_PAD_DRIVER_S)
#define GPIO_PIN2_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN2_PAD_DRIVER_S  2
/** GPIO_PIN2_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN2_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN2_SYNC1_BYPASS_M  (GPIO_PIN2_SYNC1_BYPASS_V << GPIO_PIN2_SYNC1_BYPASS_S)
#define GPIO_PIN2_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN2_SYNC1_BYPASS_S  3
/** GPIO_PIN2_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN2_INT_TYPE    0x00000007U
#define GPIO_PIN2_INT_TYPE_M  (GPIO_PIN2_INT_TYPE_V << GPIO_PIN2_INT_TYPE_S)
#define GPIO_PIN2_INT_TYPE_V  0x00000007U
#define GPIO_PIN2_INT_TYPE_S  7
/** GPIO_PIN2_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_M  (GPIO_PIN2_WAKEUP_ENABLE_V << GPIO_PIN2_WAKEUP_ENABLE_S)
#define GPIO_PIN2_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN2_WAKEUP_ENABLE_S  10
/** GPIO_PIN2_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN2_INT_ENA    0x0000001FU
#define GPIO_PIN2_INT_ENA_M  (GPIO_PIN2_INT_ENA_V << GPIO_PIN2_INT_ENA_S)
#define GPIO_PIN2_INT_ENA_V  0x0000001FU
#define GPIO_PIN2_INT_ENA_S  13

/** GPIO_PIN3_REG register
 *  GPIO3 configuration register
 */
#define GPIO_PIN3_REG (DR_REG_GPIO_BASE + 0x100)
/** GPIO_PIN3_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN3_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN3_SYNC2_BYPASS_M  (GPIO_PIN3_SYNC2_BYPASS_V << GPIO_PIN3_SYNC2_BYPASS_S)
#define GPIO_PIN3_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN3_SYNC2_BYPASS_S  0
/** GPIO_PIN3_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_M  (GPIO_PIN3_PAD_DRIVER_V << GPIO_PIN3_PAD_DRIVER_S)
#define GPIO_PIN3_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN3_PAD_DRIVER_S  2
/** GPIO_PIN3_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN3_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN3_SYNC1_BYPASS_M  (GPIO_PIN3_SYNC1_BYPASS_V << GPIO_PIN3_SYNC1_BYPASS_S)
#define GPIO_PIN3_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN3_SYNC1_BYPASS_S  3
/** GPIO_PIN3_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN3_INT_TYPE    0x00000007U
#define GPIO_PIN3_INT_TYPE_M  (GPIO_PIN3_INT_TYPE_V << GPIO_PIN3_INT_TYPE_S)
#define GPIO_PIN3_INT_TYPE_V  0x00000007U
#define GPIO_PIN3_INT_TYPE_S  7
/** GPIO_PIN3_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_M  (GPIO_PIN3_WAKEUP_ENABLE_V << GPIO_PIN3_WAKEUP_ENABLE_S)
#define GPIO_PIN3_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN3_WAKEUP_ENABLE_S  10
/** GPIO_PIN3_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN3_INT_ENA    0x0000001FU
#define GPIO_PIN3_INT_ENA_M  (GPIO_PIN3_INT_ENA_V << GPIO_PIN3_INT_ENA_S)
#define GPIO_PIN3_INT_ENA_V  0x0000001FU
#define GPIO_PIN3_INT_ENA_S  13

/** GPIO_PIN4_REG register
 *  GPIO4 configuration register
 */
#define GPIO_PIN4_REG (DR_REG_GPIO_BASE + 0x104)
/** GPIO_PIN4_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN4_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN4_SYNC2_BYPASS_M  (GPIO_PIN4_SYNC2_BYPASS_V << GPIO_PIN4_SYNC2_BYPASS_S)
#define GPIO_PIN4_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN4_SYNC2_BYPASS_S  0
/** GPIO_PIN4_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_M  (GPIO_PIN4_PAD_DRIVER_V << GPIO_PIN4_PAD_DRIVER_S)
#define GPIO_PIN4_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN4_PAD_DRIVER_S  2
/** GPIO_PIN4_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN4_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN4_SYNC1_BYPASS_M  (GPIO_PIN4_SYNC1_BYPASS_V << GPIO_PIN4_SYNC1_BYPASS_S)
#define GPIO_PIN4_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN4_SYNC1_BYPASS_S  3
/** GPIO_PIN4_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN4_INT_TYPE    0x00000007U
#define GPIO_PIN4_INT_TYPE_M  (GPIO_PIN4_INT_TYPE_V << GPIO_PIN4_INT_TYPE_S)
#define GPIO_PIN4_INT_TYPE_V  0x00000007U
#define GPIO_PIN4_INT_TYPE_S  7
/** GPIO_PIN4_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_M  (GPIO_PIN4_WAKEUP_ENABLE_V << GPIO_PIN4_WAKEUP_ENABLE_S)
#define GPIO_PIN4_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN4_WAKEUP_ENABLE_S  10
/** GPIO_PIN4_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN4_INT_ENA    0x0000001FU
#define GPIO_PIN4_INT_ENA_M  (GPIO_PIN4_INT_ENA_V << GPIO_PIN4_INT_ENA_S)
#define GPIO_PIN4_INT_ENA_V  0x0000001FU
#define GPIO_PIN4_INT_ENA_S  13

/** GPIO_PIN5_REG register
 *  GPIO5 configuration register
 */
#define GPIO_PIN5_REG (DR_REG_GPIO_BASE + 0x108)
/** GPIO_PIN5_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN5_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN5_SYNC2_BYPASS_M  (GPIO_PIN5_SYNC2_BYPASS_V << GPIO_PIN5_SYNC2_BYPASS_S)
#define GPIO_PIN5_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN5_SYNC2_BYPASS_S  0
/** GPIO_PIN5_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_M  (GPIO_PIN5_PAD_DRIVER_V << GPIO_PIN5_PAD_DRIVER_S)
#define GPIO_PIN5_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN5_PAD_DRIVER_S  2
/** GPIO_PIN5_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN5_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN5_SYNC1_BYPASS_M  (GPIO_PIN5_SYNC1_BYPASS_V << GPIO_PIN5_SYNC1_BYPASS_S)
#define GPIO_PIN5_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN5_SYNC1_BYPASS_S  3
/** GPIO_PIN5_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN5_INT_TYPE    0x00000007U
#define GPIO_PIN5_INT_TYPE_M  (GPIO_PIN5_INT_TYPE_V << GPIO_PIN5_INT_TYPE_S)
#define GPIO_PIN5_INT_TYPE_V  0x00000007U
#define GPIO_PIN5_INT_TYPE_S  7
/** GPIO_PIN5_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_M  (GPIO_PIN5_WAKEUP_ENABLE_V << GPIO_PIN5_WAKEUP_ENABLE_S)
#define GPIO_PIN5_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN5_WAKEUP_ENABLE_S  10
/** GPIO_PIN5_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN5_INT_ENA    0x0000001FU
#define GPIO_PIN5_INT_ENA_M  (GPIO_PIN5_INT_ENA_V << GPIO_PIN5_INT_ENA_S)
#define GPIO_PIN5_INT_ENA_V  0x0000001FU
#define GPIO_PIN5_INT_ENA_S  13

/** GPIO_PIN6_REG register
 *  GPIO6 configuration register
 */
#define GPIO_PIN6_REG (DR_REG_GPIO_BASE + 0x10c)
/** GPIO_PIN6_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN6_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN6_SYNC2_BYPASS_M  (GPIO_PIN6_SYNC2_BYPASS_V << GPIO_PIN6_SYNC2_BYPASS_S)
#define GPIO_PIN6_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN6_SYNC2_BYPASS_S  0
/** GPIO_PIN6_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_M  (GPIO_PIN6_PAD_DRIVER_V << GPIO_PIN6_PAD_DRIVER_S)
#define GPIO_PIN6_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN6_PAD_DRIVER_S  2
/** GPIO_PIN6_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN6_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN6_SYNC1_BYPASS_M  (GPIO_PIN6_SYNC1_BYPASS_V << GPIO_PIN6_SYNC1_BYPASS_S)
#define GPIO_PIN6_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN6_SYNC1_BYPASS_S  3
/** GPIO_PIN6_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN6_INT_TYPE    0x00000007U
#define GPIO_PIN6_INT_TYPE_M  (GPIO_PIN6_INT_TYPE_V << GPIO_PIN6_INT_TYPE_S)
#define GPIO_PIN6_INT_TYPE_V  0x00000007U
#define GPIO_PIN6_INT_TYPE_S  7
/** GPIO_PIN6_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_M  (GPIO_PIN6_WAKEUP_ENABLE_V << GPIO_PIN6_WAKEUP_ENABLE_S)
#define GPIO_PIN6_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN6_WAKEUP_ENABLE_S  10
/** GPIO_PIN6_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN6_INT_ENA    0x0000001FU
#define GPIO_PIN6_INT_ENA_M  (GPIO_PIN6_INT_ENA_V << GPIO_PIN6_INT_ENA_S)
#define GPIO_PIN6_INT_ENA_V  0x0000001FU
#define GPIO_PIN6_INT_ENA_S  13

/** GPIO_PIN7_REG register
 *  GPIO7 configuration register
 */
#define GPIO_PIN7_REG (DR_REG_GPIO_BASE + 0x110)
/** GPIO_PIN7_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN7_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN7_SYNC2_BYPASS_M  (GPIO_PIN7_SYNC2_BYPASS_V << GPIO_PIN7_SYNC2_BYPASS_S)
#define GPIO_PIN7_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN7_SYNC2_BYPASS_S  0
/** GPIO_PIN7_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN7_PAD_DRIVER    (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_M  (GPIO_PIN7_PAD_DRIVER_V << GPIO_PIN7_PAD_DRIVER_S)
#define GPIO_PIN7_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN7_PAD_DRIVER_S  2
/** GPIO_PIN7_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN7_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN7_SYNC1_BYPASS_M  (GPIO_PIN7_SYNC1_BYPASS_V << GPIO_PIN7_SYNC1_BYPASS_S)
#define GPIO_PIN7_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN7_SYNC1_BYPASS_S  3
/** GPIO_PIN7_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN7_INT_TYPE    0x00000007U
#define GPIO_PIN7_INT_TYPE_M  (GPIO_PIN7_INT_TYPE_V << GPIO_PIN7_INT_TYPE_S)
#define GPIO_PIN7_INT_TYPE_V  0x00000007U
#define GPIO_PIN7_INT_TYPE_S  7
/** GPIO_PIN7_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN7_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_M  (GPIO_PIN7_WAKEUP_ENABLE_V << GPIO_PIN7_WAKEUP_ENABLE_S)
#define GPIO_PIN7_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN7_WAKEUP_ENABLE_S  10
/** GPIO_PIN7_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN7_INT_ENA    0x0000001FU
#define GPIO_PIN7_INT_ENA_M  (GPIO_PIN7_INT_ENA_V << GPIO_PIN7_INT_ENA_S)
#define GPIO_PIN7_INT_ENA_V  0x0000001FU
#define GPIO_PIN7_INT_ENA_S  13

/** GPIO_PIN8_REG register
 *  GPIO8 configuration register
 */
#define GPIO_PIN8_REG (DR_REG_GPIO_BASE + 0x114)
/** GPIO_PIN8_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN8_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN8_SYNC2_BYPASS_M  (GPIO_PIN8_SYNC2_BYPASS_V << GPIO_PIN8_SYNC2_BYPASS_S)
#define GPIO_PIN8_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN8_SYNC2_BYPASS_S  0
/** GPIO_PIN8_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN8_PAD_DRIVER    (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_M  (GPIO_PIN8_PAD_DRIVER_V << GPIO_PIN8_PAD_DRIVER_S)
#define GPIO_PIN8_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN8_PAD_DRIVER_S  2
/** GPIO_PIN8_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN8_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN8_SYNC1_BYPASS_M  (GPIO_PIN8_SYNC1_BYPASS_V << GPIO_PIN8_SYNC1_BYPASS_S)
#define GPIO_PIN8_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN8_SYNC1_BYPASS_S  3
/** GPIO_PIN8_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN8_INT_TYPE    0x00000007U
#define GPIO_PIN8_INT_TYPE_M  (GPIO_PIN8_INT_TYPE_V << GPIO_PIN8_INT_TYPE_S)
#define GPIO_PIN8_INT_TYPE_V  0x00000007U
#define GPIO_PIN8_INT_TYPE_S  7
/** GPIO_PIN8_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN8_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_M  (GPIO_PIN8_WAKEUP_ENABLE_V << GPIO_PIN8_WAKEUP_ENABLE_S)
#define GPIO_PIN8_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN8_WAKEUP_ENABLE_S  10
/** GPIO_PIN8_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN8_INT_ENA    0x0000001FU
#define GPIO_PIN8_INT_ENA_M  (GPIO_PIN8_INT_ENA_V << GPIO_PIN8_INT_ENA_S)
#define GPIO_PIN8_INT_ENA_V  0x0000001FU
#define GPIO_PIN8_INT_ENA_S  13

/** GPIO_PIN9_REG register
 *  GPIO9 configuration register
 */
#define GPIO_PIN9_REG (DR_REG_GPIO_BASE + 0x118)
/** GPIO_PIN9_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN9_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN9_SYNC2_BYPASS_M  (GPIO_PIN9_SYNC2_BYPASS_V << GPIO_PIN9_SYNC2_BYPASS_S)
#define GPIO_PIN9_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN9_SYNC2_BYPASS_S  0
/** GPIO_PIN9_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN9_PAD_DRIVER    (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_M  (GPIO_PIN9_PAD_DRIVER_V << GPIO_PIN9_PAD_DRIVER_S)
#define GPIO_PIN9_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN9_PAD_DRIVER_S  2
/** GPIO_PIN9_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN9_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN9_SYNC1_BYPASS_M  (GPIO_PIN9_SYNC1_BYPASS_V << GPIO_PIN9_SYNC1_BYPASS_S)
#define GPIO_PIN9_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN9_SYNC1_BYPASS_S  3
/** GPIO_PIN9_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN9_INT_TYPE    0x00000007U
#define GPIO_PIN9_INT_TYPE_M  (GPIO_PIN9_INT_TYPE_V << GPIO_PIN9_INT_TYPE_S)
#define GPIO_PIN9_INT_TYPE_V  0x00000007U
#define GPIO_PIN9_INT_TYPE_S  7
/** GPIO_PIN9_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN9_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_M  (GPIO_PIN9_WAKEUP_ENABLE_V << GPIO_PIN9_WAKEUP_ENABLE_S)
#define GPIO_PIN9_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN9_WAKEUP_ENABLE_S  10
/** GPIO_PIN9_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN9_INT_ENA    0x0000001FU
#define GPIO_PIN9_INT_ENA_M  (GPIO_PIN9_INT_ENA_V << GPIO_PIN9_INT_ENA_S)
#define GPIO_PIN9_INT_ENA_V  0x0000001FU
#define GPIO_PIN9_INT_ENA_S  13

/** GPIO_PIN10_REG register
 *  GPIO10 configuration register
 */
#define GPIO_PIN10_REG (DR_REG_GPIO_BASE + 0x11c)
/** GPIO_PIN10_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN10_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN10_SYNC2_BYPASS_M  (GPIO_PIN10_SYNC2_BYPASS_V << GPIO_PIN10_SYNC2_BYPASS_S)
#define GPIO_PIN10_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN10_SYNC2_BYPASS_S  0
/** GPIO_PIN10_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN10_PAD_DRIVER    (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_M  (GPIO_PIN10_PAD_DRIVER_V << GPIO_PIN10_PAD_DRIVER_S)
#define GPIO_PIN10_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN10_PAD_DRIVER_S  2
/** GPIO_PIN10_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN10_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN10_SYNC1_BYPASS_M  (GPIO_PIN10_SYNC1_BYPASS_V << GPIO_PIN10_SYNC1_BYPASS_S)
#define GPIO_PIN10_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN10_SYNC1_BYPASS_S  3
/** GPIO_PIN10_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN10_INT_TYPE    0x00000007U
#define GPIO_PIN10_INT_TYPE_M  (GPIO_PIN10_INT_TYPE_V << GPIO_PIN10_INT_TYPE_S)
#define GPIO_PIN10_INT_TYPE_V  0x00000007U
#define GPIO_PIN10_INT_TYPE_S  7
/** GPIO_PIN10_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN10_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_M  (GPIO_PIN10_WAKEUP_ENABLE_V << GPIO_PIN10_WAKEUP_ENABLE_S)
#define GPIO_PIN10_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN10_WAKEUP_ENABLE_S  10
/** GPIO_PIN10_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN10_INT_ENA    0x0000001FU
#define GPIO_PIN10_INT_ENA_M  (GPIO_PIN10_INT_ENA_V << GPIO_PIN10_INT_ENA_S)
#define GPIO_PIN10_INT_ENA_V  0x0000001FU
#define GPIO_PIN10_INT_ENA_S  13

/** GPIO_PIN11_REG register
 *  GPIO11 configuration register
 */
#define GPIO_PIN11_REG (DR_REG_GPIO_BASE + 0x120)
/** GPIO_PIN11_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN11_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN11_SYNC2_BYPASS_M  (GPIO_PIN11_SYNC2_BYPASS_V << GPIO_PIN11_SYNC2_BYPASS_S)
#define GPIO_PIN11_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN11_SYNC2_BYPASS_S  0
/** GPIO_PIN11_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN11_PAD_DRIVER    (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_M  (GPIO_PIN11_PAD_DRIVER_V << GPIO_PIN11_PAD_DRIVER_S)
#define GPIO_PIN11_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN11_PAD_DRIVER_S  2
/** GPIO_PIN11_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN11_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN11_SYNC1_BYPASS_M  (GPIO_PIN11_SYNC1_BYPASS_V << GPIO_PIN11_SYNC1_BYPASS_S)
#define GPIO_PIN11_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN11_SYNC1_BYPASS_S  3
/** GPIO_PIN11_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN11_INT_TYPE    0x00000007U
#define GPIO_PIN11_INT_TYPE_M  (GPIO_PIN11_INT_TYPE_V << GPIO_PIN11_INT_TYPE_S)
#define GPIO_PIN11_INT_TYPE_V  0x00000007U
#define GPIO_PIN11_INT_TYPE_S  7
/** GPIO_PIN11_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN11_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_M  (GPIO_PIN11_WAKEUP_ENABLE_V << GPIO_PIN11_WAKEUP_ENABLE_S)
#define GPIO_PIN11_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN11_WAKEUP_ENABLE_S  10
/** GPIO_PIN11_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN11_INT_ENA    0x0000001FU
#define GPIO_PIN11_INT_ENA_M  (GPIO_PIN11_INT_ENA_V << GPIO_PIN11_INT_ENA_S)
#define GPIO_PIN11_INT_ENA_V  0x0000001FU
#define GPIO_PIN11_INT_ENA_S  13

/** GPIO_PIN12_REG register
 *  GPIO12 configuration register
 */
#define GPIO_PIN12_REG (DR_REG_GPIO_BASE + 0x124)
/** GPIO_PIN12_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN12_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN12_SYNC2_BYPASS_M  (GPIO_PIN12_SYNC2_BYPASS_V << GPIO_PIN12_SYNC2_BYPASS_S)
#define GPIO_PIN12_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN12_SYNC2_BYPASS_S  0
/** GPIO_PIN12_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN12_PAD_DRIVER    (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_M  (GPIO_PIN12_PAD_DRIVER_V << GPIO_PIN12_PAD_DRIVER_S)
#define GPIO_PIN12_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN12_PAD_DRIVER_S  2
/** GPIO_PIN12_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN12_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN12_SYNC1_BYPASS_M  (GPIO_PIN12_SYNC1_BYPASS_V << GPIO_PIN12_SYNC1_BYPASS_S)
#define GPIO_PIN12_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN12_SYNC1_BYPASS_S  3
/** GPIO_PIN12_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN12_INT_TYPE    0x00000007U
#define GPIO_PIN12_INT_TYPE_M  (GPIO_PIN12_INT_TYPE_V << GPIO_PIN12_INT_TYPE_S)
#define GPIO_PIN12_INT_TYPE_V  0x00000007U
#define GPIO_PIN12_INT_TYPE_S  7
/** GPIO_PIN12_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN12_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_M  (GPIO_PIN12_WAKEUP_ENABLE_V << GPIO_PIN12_WAKEUP_ENABLE_S)
#define GPIO_PIN12_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN12_WAKEUP_ENABLE_S  10
/** GPIO_PIN12_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN12_INT_ENA    0x0000001FU
#define GPIO_PIN12_INT_ENA_M  (GPIO_PIN12_INT_ENA_V << GPIO_PIN12_INT_ENA_S)
#define GPIO_PIN12_INT_ENA_V  0x0000001FU
#define GPIO_PIN12_INT_ENA_S  13

/** GPIO_PIN13_REG register
 *  GPIO13 configuration register
 */
#define GPIO_PIN13_REG (DR_REG_GPIO_BASE + 0x128)
/** GPIO_PIN13_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN13_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN13_SYNC2_BYPASS_M  (GPIO_PIN13_SYNC2_BYPASS_V << GPIO_PIN13_SYNC2_BYPASS_S)
#define GPIO_PIN13_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN13_SYNC2_BYPASS_S  0
/** GPIO_PIN13_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN13_PAD_DRIVER    (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_M  (GPIO_PIN13_PAD_DRIVER_V << GPIO_PIN13_PAD_DRIVER_S)
#define GPIO_PIN13_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN13_PAD_DRIVER_S  2
/** GPIO_PIN13_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN13_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN13_SYNC1_BYPASS_M  (GPIO_PIN13_SYNC1_BYPASS_V << GPIO_PIN13_SYNC1_BYPASS_S)
#define GPIO_PIN13_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN13_SYNC1_BYPASS_S  3
/** GPIO_PIN13_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN13_INT_TYPE    0x00000007U
#define GPIO_PIN13_INT_TYPE_M  (GPIO_PIN13_INT_TYPE_V << GPIO_PIN13_INT_TYPE_S)
#define GPIO_PIN13_INT_TYPE_V  0x00000007U
#define GPIO_PIN13_INT_TYPE_S  7
/** GPIO_PIN13_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN13_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_M  (GPIO_PIN13_WAKEUP_ENABLE_V << GPIO_PIN13_WAKEUP_ENABLE_S)
#define GPIO_PIN13_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN13_WAKEUP_ENABLE_S  10
/** GPIO_PIN13_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN13_INT_ENA    0x0000001FU
#define GPIO_PIN13_INT_ENA_M  (GPIO_PIN13_INT_ENA_V << GPIO_PIN13_INT_ENA_S)
#define GPIO_PIN13_INT_ENA_V  0x0000001FU
#define GPIO_PIN13_INT_ENA_S  13

/** GPIO_PIN14_REG register
 *  GPIO14 configuration register
 */
#define GPIO_PIN14_REG (DR_REG_GPIO_BASE + 0x12c)
/** GPIO_PIN14_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN14_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN14_SYNC2_BYPASS_M  (GPIO_PIN14_SYNC2_BYPASS_V << GPIO_PIN14_SYNC2_BYPASS_S)
#define GPIO_PIN14_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN14_SYNC2_BYPASS_S  0
/** GPIO_PIN14_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN14_PAD_DRIVER    (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_M  (GPIO_PIN14_PAD_DRIVER_V << GPIO_PIN14_PAD_DRIVER_S)
#define GPIO_PIN14_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN14_PAD_DRIVER_S  2
/** GPIO_PIN14_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN14_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN14_SYNC1_BYPASS_M  (GPIO_PIN14_SYNC1_BYPASS_V << GPIO_PIN14_SYNC1_BYPASS_S)
#define GPIO_PIN14_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN14_SYNC1_BYPASS_S  3
/** GPIO_PIN14_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN14_INT_TYPE    0x00000007U
#define GPIO_PIN14_INT_TYPE_M  (GPIO_PIN14_INT_TYPE_V << GPIO_PIN14_INT_TYPE_S)
#define GPIO_PIN14_INT_TYPE_V  0x00000007U
#define GPIO_PIN14_INT_TYPE_S  7
/** GPIO_PIN14_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN14_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_M  (GPIO_PIN14_WAKEUP_ENABLE_V << GPIO_PIN14_WAKEUP_ENABLE_S)
#define GPIO_PIN14_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN14_WAKEUP_ENABLE_S  10
/** GPIO_PIN14_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN14_INT_ENA    0x0000001FU
#define GPIO_PIN14_INT_ENA_M  (GPIO_PIN14_INT_ENA_V << GPIO_PIN14_INT_ENA_S)
#define GPIO_PIN14_INT_ENA_V  0x0000001FU
#define GPIO_PIN14_INT_ENA_S  13

/** GPIO_PIN15_REG register
 *  GPIO15 configuration register
 */
#define GPIO_PIN15_REG (DR_REG_GPIO_BASE + 0x130)
/** GPIO_PIN15_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN15_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN15_SYNC2_BYPASS_M  (GPIO_PIN15_SYNC2_BYPASS_V << GPIO_PIN15_SYNC2_BYPASS_S)
#define GPIO_PIN15_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN15_SYNC2_BYPASS_S  0
/** GPIO_PIN15_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN15_PAD_DRIVER    (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_M  (GPIO_PIN15_PAD_DRIVER_V << GPIO_PIN15_PAD_DRIVER_S)
#define GPIO_PIN15_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN15_PAD_DRIVER_S  2
/** GPIO_PIN15_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN15_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN15_SYNC1_BYPASS_M  (GPIO_PIN15_SYNC1_BYPASS_V << GPIO_PIN15_SYNC1_BYPASS_S)
#define GPIO_PIN15_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN15_SYNC1_BYPASS_S  3
/** GPIO_PIN15_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN15_INT_TYPE    0x00000007U
#define GPIO_PIN15_INT_TYPE_M  (GPIO_PIN15_INT_TYPE_V << GPIO_PIN15_INT_TYPE_S)
#define GPIO_PIN15_INT_TYPE_V  0x00000007U
#define GPIO_PIN15_INT_TYPE_S  7
/** GPIO_PIN15_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN15_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_M  (GPIO_PIN15_WAKEUP_ENABLE_V << GPIO_PIN15_WAKEUP_ENABLE_S)
#define GPIO_PIN15_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN15_WAKEUP_ENABLE_S  10
/** GPIO_PIN15_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN15_INT_ENA    0x0000001FU
#define GPIO_PIN15_INT_ENA_M  (GPIO_PIN15_INT_ENA_V << GPIO_PIN15_INT_ENA_S)
#define GPIO_PIN15_INT_ENA_V  0x0000001FU
#define GPIO_PIN15_INT_ENA_S  13

/** GPIO_PIN16_REG register
 *  GPIO16 configuration register
 */
#define GPIO_PIN16_REG (DR_REG_GPIO_BASE + 0x134)
/** GPIO_PIN16_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN16_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN16_SYNC2_BYPASS_M  (GPIO_PIN16_SYNC2_BYPASS_V << GPIO_PIN16_SYNC2_BYPASS_S)
#define GPIO_PIN16_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN16_SYNC2_BYPASS_S  0
/** GPIO_PIN16_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN16_PAD_DRIVER    (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_M  (GPIO_PIN16_PAD_DRIVER_V << GPIO_PIN16_PAD_DRIVER_S)
#define GPIO_PIN16_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN16_PAD_DRIVER_S  2
/** GPIO_PIN16_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN16_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN16_SYNC1_BYPASS_M  (GPIO_PIN16_SYNC1_BYPASS_V << GPIO_PIN16_SYNC1_BYPASS_S)
#define GPIO_PIN16_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN16_SYNC1_BYPASS_S  3
/** GPIO_PIN16_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN16_INT_TYPE    0x00000007U
#define GPIO_PIN16_INT_TYPE_M  (GPIO_PIN16_INT_TYPE_V << GPIO_PIN16_INT_TYPE_S)
#define GPIO_PIN16_INT_TYPE_V  0x00000007U
#define GPIO_PIN16_INT_TYPE_S  7
/** GPIO_PIN16_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN16_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_M  (GPIO_PIN16_WAKEUP_ENABLE_V << GPIO_PIN16_WAKEUP_ENABLE_S)
#define GPIO_PIN16_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN16_WAKEUP_ENABLE_S  10
/** GPIO_PIN16_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN16_INT_ENA    0x0000001FU
#define GPIO_PIN16_INT_ENA_M  (GPIO_PIN16_INT_ENA_V << GPIO_PIN16_INT_ENA_S)
#define GPIO_PIN16_INT_ENA_V  0x0000001FU
#define GPIO_PIN16_INT_ENA_S  13

/** GPIO_PIN17_REG register
 *  GPIO17 configuration register
 */
#define GPIO_PIN17_REG (DR_REG_GPIO_BASE + 0x138)
/** GPIO_PIN17_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN17_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN17_SYNC2_BYPASS_M  (GPIO_PIN17_SYNC2_BYPASS_V << GPIO_PIN17_SYNC2_BYPASS_S)
#define GPIO_PIN17_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN17_SYNC2_BYPASS_S  0
/** GPIO_PIN17_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN17_PAD_DRIVER    (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_M  (GPIO_PIN17_PAD_DRIVER_V << GPIO_PIN17_PAD_DRIVER_S)
#define GPIO_PIN17_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN17_PAD_DRIVER_S  2
/** GPIO_PIN17_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN17_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN17_SYNC1_BYPASS_M  (GPIO_PIN17_SYNC1_BYPASS_V << GPIO_PIN17_SYNC1_BYPASS_S)
#define GPIO_PIN17_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN17_SYNC1_BYPASS_S  3
/** GPIO_PIN17_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN17_INT_TYPE    0x00000007U
#define GPIO_PIN17_INT_TYPE_M  (GPIO_PIN17_INT_TYPE_V << GPIO_PIN17_INT_TYPE_S)
#define GPIO_PIN17_INT_TYPE_V  0x00000007U
#define GPIO_PIN17_INT_TYPE_S  7
/** GPIO_PIN17_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN17_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_M  (GPIO_PIN17_WAKEUP_ENABLE_V << GPIO_PIN17_WAKEUP_ENABLE_S)
#define GPIO_PIN17_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN17_WAKEUP_ENABLE_S  10
/** GPIO_PIN17_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN17_INT_ENA    0x0000001FU
#define GPIO_PIN17_INT_ENA_M  (GPIO_PIN17_INT_ENA_V << GPIO_PIN17_INT_ENA_S)
#define GPIO_PIN17_INT_ENA_V  0x0000001FU
#define GPIO_PIN17_INT_ENA_S  13

/** GPIO_PIN18_REG register
 *  GPIO18 configuration register
 */
#define GPIO_PIN18_REG (DR_REG_GPIO_BASE + 0x13c)
/** GPIO_PIN18_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN18_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN18_SYNC2_BYPASS_M  (GPIO_PIN18_SYNC2_BYPASS_V << GPIO_PIN18_SYNC2_BYPASS_S)
#define GPIO_PIN18_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN18_SYNC2_BYPASS_S  0
/** GPIO_PIN18_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN18_PAD_DRIVER    (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_M  (GPIO_PIN18_PAD_DRIVER_V << GPIO_PIN18_PAD_DRIVER_S)
#define GPIO_PIN18_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN18_PAD_DRIVER_S  2
/** GPIO_PIN18_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN18_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN18_SYNC1_BYPASS_M  (GPIO_PIN18_SYNC1_BYPASS_V << GPIO_PIN18_SYNC1_BYPASS_S)
#define GPIO_PIN18_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN18_SYNC1_BYPASS_S  3
/** GPIO_PIN18_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN18_INT_TYPE    0x00000007U
#define GPIO_PIN18_INT_TYPE_M  (GPIO_PIN18_INT_TYPE_V << GPIO_PIN18_INT_TYPE_S)
#define GPIO_PIN18_INT_TYPE_V  0x00000007U
#define GPIO_PIN18_INT_TYPE_S  7
/** GPIO_PIN18_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN18_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_M  (GPIO_PIN18_WAKEUP_ENABLE_V << GPIO_PIN18_WAKEUP_ENABLE_S)
#define GPIO_PIN18_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN18_WAKEUP_ENABLE_S  10
/** GPIO_PIN18_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN18_INT_ENA    0x0000001FU
#define GPIO_PIN18_INT_ENA_M  (GPIO_PIN18_INT_ENA_V << GPIO_PIN18_INT_ENA_S)
#define GPIO_PIN18_INT_ENA_V  0x0000001FU
#define GPIO_PIN18_INT_ENA_S  13

/** GPIO_PIN19_REG register
 *  GPIO19 configuration register
 */
#define GPIO_PIN19_REG (DR_REG_GPIO_BASE + 0x140)
/** GPIO_PIN19_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN19_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN19_SYNC2_BYPASS_M  (GPIO_PIN19_SYNC2_BYPASS_V << GPIO_PIN19_SYNC2_BYPASS_S)
#define GPIO_PIN19_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN19_SYNC2_BYPASS_S  0
/** GPIO_PIN19_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN19_PAD_DRIVER    (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_M  (GPIO_PIN19_PAD_DRIVER_V << GPIO_PIN19_PAD_DRIVER_S)
#define GPIO_PIN19_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN19_PAD_DRIVER_S  2
/** GPIO_PIN19_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN19_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN19_SYNC1_BYPASS_M  (GPIO_PIN19_SYNC1_BYPASS_V << GPIO_PIN19_SYNC1_BYPASS_S)
#define GPIO_PIN19_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN19_SYNC1_BYPASS_S  3
/** GPIO_PIN19_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN19_INT_TYPE    0x00000007U
#define GPIO_PIN19_INT_TYPE_M  (GPIO_PIN19_INT_TYPE_V << GPIO_PIN19_INT_TYPE_S)
#define GPIO_PIN19_INT_TYPE_V  0x00000007U
#define GPIO_PIN19_INT_TYPE_S  7
/** GPIO_PIN19_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN19_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_M  (GPIO_PIN19_WAKEUP_ENABLE_V << GPIO_PIN19_WAKEUP_ENABLE_S)
#define GPIO_PIN19_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN19_WAKEUP_ENABLE_S  10
/** GPIO_PIN19_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN19_INT_ENA    0x0000001FU
#define GPIO_PIN19_INT_ENA_M  (GPIO_PIN19_INT_ENA_V << GPIO_PIN19_INT_ENA_S)
#define GPIO_PIN19_INT_ENA_V  0x0000001FU
#define GPIO_PIN19_INT_ENA_S  13

/** GPIO_PIN20_REG register
 *  GPIO20 configuration register
 */
#define GPIO_PIN20_REG (DR_REG_GPIO_BASE + 0x144)
/** GPIO_PIN20_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN20_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN20_SYNC2_BYPASS_M  (GPIO_PIN20_SYNC2_BYPASS_V << GPIO_PIN20_SYNC2_BYPASS_S)
#define GPIO_PIN20_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN20_SYNC2_BYPASS_S  0
/** GPIO_PIN20_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN20_PAD_DRIVER    (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_M  (GPIO_PIN20_PAD_DRIVER_V << GPIO_PIN20_PAD_DRIVER_S)
#define GPIO_PIN20_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN20_PAD_DRIVER_S  2
/** GPIO_PIN20_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN20_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN20_SYNC1_BYPASS_M  (GPIO_PIN20_SYNC1_BYPASS_V << GPIO_PIN20_SYNC1_BYPASS_S)
#define GPIO_PIN20_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN20_SYNC1_BYPASS_S  3
/** GPIO_PIN20_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN20_INT_TYPE    0x00000007U
#define GPIO_PIN20_INT_TYPE_M  (GPIO_PIN20_INT_TYPE_V << GPIO_PIN20_INT_TYPE_S)
#define GPIO_PIN20_INT_TYPE_V  0x00000007U
#define GPIO_PIN20_INT_TYPE_S  7
/** GPIO_PIN20_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN20_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_M  (GPIO_PIN20_WAKEUP_ENABLE_V << GPIO_PIN20_WAKEUP_ENABLE_S)
#define GPIO_PIN20_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN20_WAKEUP_ENABLE_S  10
/** GPIO_PIN20_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN20_INT_ENA    0x0000001FU
#define GPIO_PIN20_INT_ENA_M  (GPIO_PIN20_INT_ENA_V << GPIO_PIN20_INT_ENA_S)
#define GPIO_PIN20_INT_ENA_V  0x0000001FU
#define GPIO_PIN20_INT_ENA_S  13

/** GPIO_PIN21_REG register
 *  GPIO21 configuration register
 */
#define GPIO_PIN21_REG (DR_REG_GPIO_BASE + 0x148)
/** GPIO_PIN21_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN21_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN21_SYNC2_BYPASS_M  (GPIO_PIN21_SYNC2_BYPASS_V << GPIO_PIN21_SYNC2_BYPASS_S)
#define GPIO_PIN21_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN21_SYNC2_BYPASS_S  0
/** GPIO_PIN21_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN21_PAD_DRIVER    (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_M  (GPIO_PIN21_PAD_DRIVER_V << GPIO_PIN21_PAD_DRIVER_S)
#define GPIO_PIN21_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN21_PAD_DRIVER_S  2
/** GPIO_PIN21_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN21_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN21_SYNC1_BYPASS_M  (GPIO_PIN21_SYNC1_BYPASS_V << GPIO_PIN21_SYNC1_BYPASS_S)
#define GPIO_PIN21_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN21_SYNC1_BYPASS_S  3
/** GPIO_PIN21_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN21_INT_TYPE    0x00000007U
#define GPIO_PIN21_INT_TYPE_M  (GPIO_PIN21_INT_TYPE_V << GPIO_PIN21_INT_TYPE_S)
#define GPIO_PIN21_INT_TYPE_V  0x00000007U
#define GPIO_PIN21_INT_TYPE_S  7
/** GPIO_PIN21_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN21_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_M  (GPIO_PIN21_WAKEUP_ENABLE_V << GPIO_PIN21_WAKEUP_ENABLE_S)
#define GPIO_PIN21_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN21_WAKEUP_ENABLE_S  10
/** GPIO_PIN21_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN21_INT_ENA    0x0000001FU
#define GPIO_PIN21_INT_ENA_M  (GPIO_PIN21_INT_ENA_V << GPIO_PIN21_INT_ENA_S)
#define GPIO_PIN21_INT_ENA_V  0x0000001FU
#define GPIO_PIN21_INT_ENA_S  13

/** GPIO_PIN22_REG register
 *  GPIO22 configuration register
 */
#define GPIO_PIN22_REG (DR_REG_GPIO_BASE + 0x14c)
/** GPIO_PIN22_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN22_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN22_SYNC2_BYPASS_M  (GPIO_PIN22_SYNC2_BYPASS_V << GPIO_PIN22_SYNC2_BYPASS_S)
#define GPIO_PIN22_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN22_SYNC2_BYPASS_S  0
/** GPIO_PIN22_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN22_PAD_DRIVER    (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_M  (GPIO_PIN22_PAD_DRIVER_V << GPIO_PIN22_PAD_DRIVER_S)
#define GPIO_PIN22_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN22_PAD_DRIVER_S  2
/** GPIO_PIN22_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN22_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN22_SYNC1_BYPASS_M  (GPIO_PIN22_SYNC1_BYPASS_V << GPIO_PIN22_SYNC1_BYPASS_S)
#define GPIO_PIN22_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN22_SYNC1_BYPASS_S  3
/** GPIO_PIN22_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN22_INT_TYPE    0x00000007U
#define GPIO_PIN22_INT_TYPE_M  (GPIO_PIN22_INT_TYPE_V << GPIO_PIN22_INT_TYPE_S)
#define GPIO_PIN22_INT_TYPE_V  0x00000007U
#define GPIO_PIN22_INT_TYPE_S  7
/** GPIO_PIN22_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN22_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_M  (GPIO_PIN22_WAKEUP_ENABLE_V << GPIO_PIN22_WAKEUP_ENABLE_S)
#define GPIO_PIN22_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN22_WAKEUP_ENABLE_S  10
/** GPIO_PIN22_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN22_INT_ENA    0x0000001FU
#define GPIO_PIN22_INT_ENA_M  (GPIO_PIN22_INT_ENA_V << GPIO_PIN22_INT_ENA_S)
#define GPIO_PIN22_INT_ENA_V  0x0000001FU
#define GPIO_PIN22_INT_ENA_S  13

/** GPIO_PIN23_REG register
 *  GPIO23 configuration register
 */
#define GPIO_PIN23_REG (DR_REG_GPIO_BASE + 0x150)
/** GPIO_PIN23_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN23_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN23_SYNC2_BYPASS_M  (GPIO_PIN23_SYNC2_BYPASS_V << GPIO_PIN23_SYNC2_BYPASS_S)
#define GPIO_PIN23_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN23_SYNC2_BYPASS_S  0
/** GPIO_PIN23_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN23_PAD_DRIVER    (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_M  (GPIO_PIN23_PAD_DRIVER_V << GPIO_PIN23_PAD_DRIVER_S)
#define GPIO_PIN23_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN23_PAD_DRIVER_S  2
/** GPIO_PIN23_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN23_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN23_SYNC1_BYPASS_M  (GPIO_PIN23_SYNC1_BYPASS_V << GPIO_PIN23_SYNC1_BYPASS_S)
#define GPIO_PIN23_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN23_SYNC1_BYPASS_S  3
/** GPIO_PIN23_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN23_INT_TYPE    0x00000007U
#define GPIO_PIN23_INT_TYPE_M  (GPIO_PIN23_INT_TYPE_V << GPIO_PIN23_INT_TYPE_S)
#define GPIO_PIN23_INT_TYPE_V  0x00000007U
#define GPIO_PIN23_INT_TYPE_S  7
/** GPIO_PIN23_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN23_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_M  (GPIO_PIN23_WAKEUP_ENABLE_V << GPIO_PIN23_WAKEUP_ENABLE_S)
#define GPIO_PIN23_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN23_WAKEUP_ENABLE_S  10
/** GPIO_PIN23_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN23_INT_ENA    0x0000001FU
#define GPIO_PIN23_INT_ENA_M  (GPIO_PIN23_INT_ENA_V << GPIO_PIN23_INT_ENA_S)
#define GPIO_PIN23_INT_ENA_V  0x0000001FU
#define GPIO_PIN23_INT_ENA_S  13

/** GPIO_PIN24_REG register
 *  GPIO24 configuration register
 */
#define GPIO_PIN24_REG (DR_REG_GPIO_BASE + 0x154)
/** GPIO_PIN24_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN24_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN24_SYNC2_BYPASS_M  (GPIO_PIN24_SYNC2_BYPASS_V << GPIO_PIN24_SYNC2_BYPASS_S)
#define GPIO_PIN24_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN24_SYNC2_BYPASS_S  0
/** GPIO_PIN24_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN24_PAD_DRIVER    (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_M  (GPIO_PIN24_PAD_DRIVER_V << GPIO_PIN24_PAD_DRIVER_S)
#define GPIO_PIN24_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN24_PAD_DRIVER_S  2
/** GPIO_PIN24_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN24_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN24_SYNC1_BYPASS_M  (GPIO_PIN24_SYNC1_BYPASS_V << GPIO_PIN24_SYNC1_BYPASS_S)
#define GPIO_PIN24_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN24_SYNC1_BYPASS_S  3
/** GPIO_PIN24_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN24_INT_TYPE    0x00000007U
#define GPIO_PIN24_INT_TYPE_M  (GPIO_PIN24_INT_TYPE_V << GPIO_PIN24_INT_TYPE_S)
#define GPIO_PIN24_INT_TYPE_V  0x00000007U
#define GPIO_PIN24_INT_TYPE_S  7
/** GPIO_PIN24_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN24_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_M  (GPIO_PIN24_WAKEUP_ENABLE_V << GPIO_PIN24_WAKEUP_ENABLE_S)
#define GPIO_PIN24_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN24_WAKEUP_ENABLE_S  10
/** GPIO_PIN24_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN24_INT_ENA    0x0000001FU
#define GPIO_PIN24_INT_ENA_M  (GPIO_PIN24_INT_ENA_V << GPIO_PIN24_INT_ENA_S)
#define GPIO_PIN24_INT_ENA_V  0x0000001FU
#define GPIO_PIN24_INT_ENA_S  13

/** GPIO_PIN25_REG register
 *  GPIO25 configuration register
 */
#define GPIO_PIN25_REG (DR_REG_GPIO_BASE + 0x158)
/** GPIO_PIN25_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN25_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN25_SYNC2_BYPASS_M  (GPIO_PIN25_SYNC2_BYPASS_V << GPIO_PIN25_SYNC2_BYPASS_S)
#define GPIO_PIN25_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN25_SYNC2_BYPASS_S  0
/** GPIO_PIN25_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN25_PAD_DRIVER    (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_M  (GPIO_PIN25_PAD_DRIVER_V << GPIO_PIN25_PAD_DRIVER_S)
#define GPIO_PIN25_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN25_PAD_DRIVER_S  2
/** GPIO_PIN25_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN25_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN25_SYNC1_BYPASS_M  (GPIO_PIN25_SYNC1_BYPASS_V << GPIO_PIN25_SYNC1_BYPASS_S)
#define GPIO_PIN25_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN25_SYNC1_BYPASS_S  3
/** GPIO_PIN25_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN25_INT_TYPE    0x00000007U
#define GPIO_PIN25_INT_TYPE_M  (GPIO_PIN25_INT_TYPE_V << GPIO_PIN25_INT_TYPE_S)
#define GPIO_PIN25_INT_TYPE_V  0x00000007U
#define GPIO_PIN25_INT_TYPE_S  7
/** GPIO_PIN25_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN25_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_M  (GPIO_PIN25_WAKEUP_ENABLE_V << GPIO_PIN25_WAKEUP_ENABLE_S)
#define GPIO_PIN25_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN25_WAKEUP_ENABLE_S  10
/** GPIO_PIN25_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN25_INT_ENA    0x0000001FU
#define GPIO_PIN25_INT_ENA_M  (GPIO_PIN25_INT_ENA_V << GPIO_PIN25_INT_ENA_S)
#define GPIO_PIN25_INT_ENA_V  0x0000001FU
#define GPIO_PIN25_INT_ENA_S  13

/** GPIO_PIN26_REG register
 *  GPIO26 configuration register
 */
#define GPIO_PIN26_REG (DR_REG_GPIO_BASE + 0x15c)
/** GPIO_PIN26_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN26_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN26_SYNC2_BYPASS_M  (GPIO_PIN26_SYNC2_BYPASS_V << GPIO_PIN26_SYNC2_BYPASS_S)
#define GPIO_PIN26_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN26_SYNC2_BYPASS_S  0
/** GPIO_PIN26_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN26_PAD_DRIVER    (BIT(2))
#define GPIO_PIN26_PAD_DRIVER_M  (GPIO_PIN26_PAD_DRIVER_V << GPIO_PIN26_PAD_DRIVER_S)
#define GPIO_PIN26_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN26_PAD_DRIVER_S  2
/** GPIO_PIN26_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN26_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN26_SYNC1_BYPASS_M  (GPIO_PIN26_SYNC1_BYPASS_V << GPIO_PIN26_SYNC1_BYPASS_S)
#define GPIO_PIN26_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN26_SYNC1_BYPASS_S  3
/** GPIO_PIN26_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN26_INT_TYPE    0x00000007U
#define GPIO_PIN26_INT_TYPE_M  (GPIO_PIN26_INT_TYPE_V << GPIO_PIN26_INT_TYPE_S)
#define GPIO_PIN26_INT_TYPE_V  0x00000007U
#define GPIO_PIN26_INT_TYPE_S  7
/** GPIO_PIN26_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN26_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN26_WAKEUP_ENABLE_M  (GPIO_PIN26_WAKEUP_ENABLE_V << GPIO_PIN26_WAKEUP_ENABLE_S)
#define GPIO_PIN26_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN26_WAKEUP_ENABLE_S  10
/** GPIO_PIN26_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN26_INT_ENA    0x0000001FU
#define GPIO_PIN26_INT_ENA_M  (GPIO_PIN26_INT_ENA_V << GPIO_PIN26_INT_ENA_S)
#define GPIO_PIN26_INT_ENA_V  0x0000001FU
#define GPIO_PIN26_INT_ENA_S  13

/** GPIO_PIN27_REG register
 *  GPIO27 configuration register
 */
#define GPIO_PIN27_REG (DR_REG_GPIO_BASE + 0x160)
/** GPIO_PIN27_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN27_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN27_SYNC2_BYPASS_M  (GPIO_PIN27_SYNC2_BYPASS_V << GPIO_PIN27_SYNC2_BYPASS_S)
#define GPIO_PIN27_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN27_SYNC2_BYPASS_S  0
/** GPIO_PIN27_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN27_PAD_DRIVER    (BIT(2))
#define GPIO_PIN27_PAD_DRIVER_M  (GPIO_PIN27_PAD_DRIVER_V << GPIO_PIN27_PAD_DRIVER_S)
#define GPIO_PIN27_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN27_PAD_DRIVER_S  2
/** GPIO_PIN27_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN27_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN27_SYNC1_BYPASS_M  (GPIO_PIN27_SYNC1_BYPASS_V << GPIO_PIN27_SYNC1_BYPASS_S)
#define GPIO_PIN27_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN27_SYNC1_BYPASS_S  3
/** GPIO_PIN27_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN27_INT_TYPE    0x00000007U
#define GPIO_PIN27_INT_TYPE_M  (GPIO_PIN27_INT_TYPE_V << GPIO_PIN27_INT_TYPE_S)
#define GPIO_PIN27_INT_TYPE_V  0x00000007U
#define GPIO_PIN27_INT_TYPE_S  7
/** GPIO_PIN27_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN27_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN27_WAKEUP_ENABLE_M  (GPIO_PIN27_WAKEUP_ENABLE_V << GPIO_PIN27_WAKEUP_ENABLE_S)
#define GPIO_PIN27_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN27_WAKEUP_ENABLE_S  10
/** GPIO_PIN27_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN27_INT_ENA    0x0000001FU
#define GPIO_PIN27_INT_ENA_M  (GPIO_PIN27_INT_ENA_V << GPIO_PIN27_INT_ENA_S)
#define GPIO_PIN27_INT_ENA_V  0x0000001FU
#define GPIO_PIN27_INT_ENA_S  13

/** GPIO_PIN28_REG register
 *  GPIO28 configuration register
 */
#define GPIO_PIN28_REG (DR_REG_GPIO_BASE + 0x164)
/** GPIO_PIN28_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN28_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN28_SYNC2_BYPASS_M  (GPIO_PIN28_SYNC2_BYPASS_V << GPIO_PIN28_SYNC2_BYPASS_S)
#define GPIO_PIN28_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN28_SYNC2_BYPASS_S  0
/** GPIO_PIN28_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN28_PAD_DRIVER    (BIT(2))
#define GPIO_PIN28_PAD_DRIVER_M  (GPIO_PIN28_PAD_DRIVER_V << GPIO_PIN28_PAD_DRIVER_S)
#define GPIO_PIN28_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN28_PAD_DRIVER_S  2
/** GPIO_PIN28_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN28_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN28_SYNC1_BYPASS_M  (GPIO_PIN28_SYNC1_BYPASS_V << GPIO_PIN28_SYNC1_BYPASS_S)
#define GPIO_PIN28_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN28_SYNC1_BYPASS_S  3
/** GPIO_PIN28_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN28_INT_TYPE    0x00000007U
#define GPIO_PIN28_INT_TYPE_M  (GPIO_PIN28_INT_TYPE_V << GPIO_PIN28_INT_TYPE_S)
#define GPIO_PIN28_INT_TYPE_V  0x00000007U
#define GPIO_PIN28_INT_TYPE_S  7
/** GPIO_PIN28_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN28_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN28_WAKEUP_ENABLE_M  (GPIO_PIN28_WAKEUP_ENABLE_V << GPIO_PIN28_WAKEUP_ENABLE_S)
#define GPIO_PIN28_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN28_WAKEUP_ENABLE_S  10
/** GPIO_PIN28_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN28_INT_ENA    0x0000001FU
#define GPIO_PIN28_INT_ENA_M  (GPIO_PIN28_INT_ENA_V << GPIO_PIN28_INT_ENA_S)
#define GPIO_PIN28_INT_ENA_V  0x0000001FU
#define GPIO_PIN28_INT_ENA_S  13

/** GPIO_PIN29_REG register
 *  GPIO29 configuration register
 */
#define GPIO_PIN29_REG (DR_REG_GPIO_BASE + 0x168)
/** GPIO_PIN29_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN29_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN29_SYNC2_BYPASS_M  (GPIO_PIN29_SYNC2_BYPASS_V << GPIO_PIN29_SYNC2_BYPASS_S)
#define GPIO_PIN29_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN29_SYNC2_BYPASS_S  0
/** GPIO_PIN29_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN29_PAD_DRIVER    (BIT(2))
#define GPIO_PIN29_PAD_DRIVER_M  (GPIO_PIN29_PAD_DRIVER_V << GPIO_PIN29_PAD_DRIVER_S)
#define GPIO_PIN29_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN29_PAD_DRIVER_S  2
/** GPIO_PIN29_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN29_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN29_SYNC1_BYPASS_M  (GPIO_PIN29_SYNC1_BYPASS_V << GPIO_PIN29_SYNC1_BYPASS_S)
#define GPIO_PIN29_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN29_SYNC1_BYPASS_S  3
/** GPIO_PIN29_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN29_INT_TYPE    0x00000007U
#define GPIO_PIN29_INT_TYPE_M  (GPIO_PIN29_INT_TYPE_V << GPIO_PIN29_INT_TYPE_S)
#define GPIO_PIN29_INT_TYPE_V  0x00000007U
#define GPIO_PIN29_INT_TYPE_S  7
/** GPIO_PIN29_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN29_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN29_WAKEUP_ENABLE_M  (GPIO_PIN29_WAKEUP_ENABLE_V << GPIO_PIN29_WAKEUP_ENABLE_S)
#define GPIO_PIN29_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN29_WAKEUP_ENABLE_S  10
/** GPIO_PIN29_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN29_INT_ENA    0x0000001FU
#define GPIO_PIN29_INT_ENA_M  (GPIO_PIN29_INT_ENA_V << GPIO_PIN29_INT_ENA_S)
#define GPIO_PIN29_INT_ENA_V  0x0000001FU
#define GPIO_PIN29_INT_ENA_S  13

/** GPIO_PIN30_REG register
 *  GPIO30 configuration register
 */
#define GPIO_PIN30_REG (DR_REG_GPIO_BASE + 0x16c)
/** GPIO_PIN30_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN30_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN30_SYNC2_BYPASS_M  (GPIO_PIN30_SYNC2_BYPASS_V << GPIO_PIN30_SYNC2_BYPASS_S)
#define GPIO_PIN30_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN30_SYNC2_BYPASS_S  0
/** GPIO_PIN30_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN30_PAD_DRIVER    (BIT(2))
#define GPIO_PIN30_PAD_DRIVER_M  (GPIO_PIN30_PAD_DRIVER_V << GPIO_PIN30_PAD_DRIVER_S)
#define GPIO_PIN30_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN30_PAD_DRIVER_S  2
/** GPIO_PIN30_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN30_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN30_SYNC1_BYPASS_M  (GPIO_PIN30_SYNC1_BYPASS_V << GPIO_PIN30_SYNC1_BYPASS_S)
#define GPIO_PIN30_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN30_SYNC1_BYPASS_S  3
/** GPIO_PIN30_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN30_INT_TYPE    0x00000007U
#define GPIO_PIN30_INT_TYPE_M  (GPIO_PIN30_INT_TYPE_V << GPIO_PIN30_INT_TYPE_S)
#define GPIO_PIN30_INT_TYPE_V  0x00000007U
#define GPIO_PIN30_INT_TYPE_S  7
/** GPIO_PIN30_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN30_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN30_WAKEUP_ENABLE_M  (GPIO_PIN30_WAKEUP_ENABLE_V << GPIO_PIN30_WAKEUP_ENABLE_S)
#define GPIO_PIN30_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN30_WAKEUP_ENABLE_S  10
/** GPIO_PIN30_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN30_INT_ENA    0x0000001FU
#define GPIO_PIN30_INT_ENA_M  (GPIO_PIN30_INT_ENA_V << GPIO_PIN30_INT_ENA_S)
#define GPIO_PIN30_INT_ENA_V  0x0000001FU
#define GPIO_PIN30_INT_ENA_S  13

/** GPIO_PIN31_REG register
 *  GPIO31 configuration register
 */
#define GPIO_PIN31_REG (DR_REG_GPIO_BASE + 0x170)
/** GPIO_PIN31_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN31_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN31_SYNC2_BYPASS_M  (GPIO_PIN31_SYNC2_BYPASS_V << GPIO_PIN31_SYNC2_BYPASS_S)
#define GPIO_PIN31_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN31_SYNC2_BYPASS_S  0
/** GPIO_PIN31_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN31_PAD_DRIVER    (BIT(2))
#define GPIO_PIN31_PAD_DRIVER_M  (GPIO_PIN31_PAD_DRIVER_V << GPIO_PIN31_PAD_DRIVER_S)
#define GPIO_PIN31_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN31_PAD_DRIVER_S  2
/** GPIO_PIN31_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN31_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN31_SYNC1_BYPASS_M  (GPIO_PIN31_SYNC1_BYPASS_V << GPIO_PIN31_SYNC1_BYPASS_S)
#define GPIO_PIN31_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN31_SYNC1_BYPASS_S  3
/** GPIO_PIN31_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN31_INT_TYPE    0x00000007U
#define GPIO_PIN31_INT_TYPE_M  (GPIO_PIN31_INT_TYPE_V << GPIO_PIN31_INT_TYPE_S)
#define GPIO_PIN31_INT_TYPE_V  0x00000007U
#define GPIO_PIN31_INT_TYPE_S  7
/** GPIO_PIN31_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN31_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN31_WAKEUP_ENABLE_M  (GPIO_PIN31_WAKEUP_ENABLE_V << GPIO_PIN31_WAKEUP_ENABLE_S)
#define GPIO_PIN31_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN31_WAKEUP_ENABLE_S  10
/** GPIO_PIN31_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN31_INT_ENA    0x0000001FU
#define GPIO_PIN31_INT_ENA_M  (GPIO_PIN31_INT_ENA_V << GPIO_PIN31_INT_ENA_S)
#define GPIO_PIN31_INT_ENA_V  0x0000001FU
#define GPIO_PIN31_INT_ENA_S  13

/** GPIO_PIN32_REG register
 *  GPIO32 configuration register
 */
#define GPIO_PIN32_REG (DR_REG_GPIO_BASE + 0x174)
/** GPIO_PIN32_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN32_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN32_SYNC2_BYPASS_M  (GPIO_PIN32_SYNC2_BYPASS_V << GPIO_PIN32_SYNC2_BYPASS_S)
#define GPIO_PIN32_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN32_SYNC2_BYPASS_S  0
/** GPIO_PIN32_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN32_PAD_DRIVER    (BIT(2))
#define GPIO_PIN32_PAD_DRIVER_M  (GPIO_PIN32_PAD_DRIVER_V << GPIO_PIN32_PAD_DRIVER_S)
#define GPIO_PIN32_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN32_PAD_DRIVER_S  2
/** GPIO_PIN32_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN32_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN32_SYNC1_BYPASS_M  (GPIO_PIN32_SYNC1_BYPASS_V << GPIO_PIN32_SYNC1_BYPASS_S)
#define GPIO_PIN32_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN32_SYNC1_BYPASS_S  3
/** GPIO_PIN32_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN32_INT_TYPE    0x00000007U
#define GPIO_PIN32_INT_TYPE_M  (GPIO_PIN32_INT_TYPE_V << GPIO_PIN32_INT_TYPE_S)
#define GPIO_PIN32_INT_TYPE_V  0x00000007U
#define GPIO_PIN32_INT_TYPE_S  7
/** GPIO_PIN32_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN32_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN32_WAKEUP_ENABLE_M  (GPIO_PIN32_WAKEUP_ENABLE_V << GPIO_PIN32_WAKEUP_ENABLE_S)
#define GPIO_PIN32_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN32_WAKEUP_ENABLE_S  10
/** GPIO_PIN32_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN32_INT_ENA    0x0000001FU
#define GPIO_PIN32_INT_ENA_M  (GPIO_PIN32_INT_ENA_V << GPIO_PIN32_INT_ENA_S)
#define GPIO_PIN32_INT_ENA_V  0x0000001FU
#define GPIO_PIN32_INT_ENA_S  13

/** GPIO_PIN33_REG register
 *  GPIO33 configuration register
 */
#define GPIO_PIN33_REG (DR_REG_GPIO_BASE + 0x178)
/** GPIO_PIN33_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN33_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN33_SYNC2_BYPASS_M  (GPIO_PIN33_SYNC2_BYPASS_V << GPIO_PIN33_SYNC2_BYPASS_S)
#define GPIO_PIN33_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN33_SYNC2_BYPASS_S  0
/** GPIO_PIN33_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN33_PAD_DRIVER    (BIT(2))
#define GPIO_PIN33_PAD_DRIVER_M  (GPIO_PIN33_PAD_DRIVER_V << GPIO_PIN33_PAD_DRIVER_S)
#define GPIO_PIN33_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN33_PAD_DRIVER_S  2
/** GPIO_PIN33_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN33_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN33_SYNC1_BYPASS_M  (GPIO_PIN33_SYNC1_BYPASS_V << GPIO_PIN33_SYNC1_BYPASS_S)
#define GPIO_PIN33_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN33_SYNC1_BYPASS_S  3
/** GPIO_PIN33_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN33_INT_TYPE    0x00000007U
#define GPIO_PIN33_INT_TYPE_M  (GPIO_PIN33_INT_TYPE_V << GPIO_PIN33_INT_TYPE_S)
#define GPIO_PIN33_INT_TYPE_V  0x00000007U
#define GPIO_PIN33_INT_TYPE_S  7
/** GPIO_PIN33_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN33_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN33_WAKEUP_ENABLE_M  (GPIO_PIN33_WAKEUP_ENABLE_V << GPIO_PIN33_WAKEUP_ENABLE_S)
#define GPIO_PIN33_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN33_WAKEUP_ENABLE_S  10
/** GPIO_PIN33_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN33_INT_ENA    0x0000001FU
#define GPIO_PIN33_INT_ENA_M  (GPIO_PIN33_INT_ENA_V << GPIO_PIN33_INT_ENA_S)
#define GPIO_PIN33_INT_ENA_V  0x0000001FU
#define GPIO_PIN33_INT_ENA_S  13

/** GPIO_PIN34_REG register
 *  GPIO34 configuration register
 */
#define GPIO_PIN34_REG (DR_REG_GPIO_BASE + 0x17c)
/** GPIO_PIN34_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN34_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN34_SYNC2_BYPASS_M  (GPIO_PIN34_SYNC2_BYPASS_V << GPIO_PIN34_SYNC2_BYPASS_S)
#define GPIO_PIN34_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN34_SYNC2_BYPASS_S  0
/** GPIO_PIN34_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN34_PAD_DRIVER    (BIT(2))
#define GPIO_PIN34_PAD_DRIVER_M  (GPIO_PIN34_PAD_DRIVER_V << GPIO_PIN34_PAD_DRIVER_S)
#define GPIO_PIN34_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN34_PAD_DRIVER_S  2
/** GPIO_PIN34_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN34_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN34_SYNC1_BYPASS_M  (GPIO_PIN34_SYNC1_BYPASS_V << GPIO_PIN34_SYNC1_BYPASS_S)
#define GPIO_PIN34_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN34_SYNC1_BYPASS_S  3
/** GPIO_PIN34_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN34_INT_TYPE    0x00000007U
#define GPIO_PIN34_INT_TYPE_M  (GPIO_PIN34_INT_TYPE_V << GPIO_PIN34_INT_TYPE_S)
#define GPIO_PIN34_INT_TYPE_V  0x00000007U
#define GPIO_PIN34_INT_TYPE_S  7
/** GPIO_PIN34_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN34_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN34_WAKEUP_ENABLE_M  (GPIO_PIN34_WAKEUP_ENABLE_V << GPIO_PIN34_WAKEUP_ENABLE_S)
#define GPIO_PIN34_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN34_WAKEUP_ENABLE_S  10
/** GPIO_PIN34_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN34_INT_ENA    0x0000001FU
#define GPIO_PIN34_INT_ENA_M  (GPIO_PIN34_INT_ENA_V << GPIO_PIN34_INT_ENA_S)
#define GPIO_PIN34_INT_ENA_V  0x0000001FU
#define GPIO_PIN34_INT_ENA_S  13

/** GPIO_PIN35_REG register
 *  GPIO35 configuration register
 */
#define GPIO_PIN35_REG (DR_REG_GPIO_BASE + 0x180)
/** GPIO_PIN35_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN35_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN35_SYNC2_BYPASS_M  (GPIO_PIN35_SYNC2_BYPASS_V << GPIO_PIN35_SYNC2_BYPASS_S)
#define GPIO_PIN35_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN35_SYNC2_BYPASS_S  0
/** GPIO_PIN35_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN35_PAD_DRIVER    (BIT(2))
#define GPIO_PIN35_PAD_DRIVER_M  (GPIO_PIN35_PAD_DRIVER_V << GPIO_PIN35_PAD_DRIVER_S)
#define GPIO_PIN35_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN35_PAD_DRIVER_S  2
/** GPIO_PIN35_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN35_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN35_SYNC1_BYPASS_M  (GPIO_PIN35_SYNC1_BYPASS_V << GPIO_PIN35_SYNC1_BYPASS_S)
#define GPIO_PIN35_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN35_SYNC1_BYPASS_S  3
/** GPIO_PIN35_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN35_INT_TYPE    0x00000007U
#define GPIO_PIN35_INT_TYPE_M  (GPIO_PIN35_INT_TYPE_V << GPIO_PIN35_INT_TYPE_S)
#define GPIO_PIN35_INT_TYPE_V  0x00000007U
#define GPIO_PIN35_INT_TYPE_S  7
/** GPIO_PIN35_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN35_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN35_WAKEUP_ENABLE_M  (GPIO_PIN35_WAKEUP_ENABLE_V << GPIO_PIN35_WAKEUP_ENABLE_S)
#define GPIO_PIN35_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN35_WAKEUP_ENABLE_S  10
/** GPIO_PIN35_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN35_INT_ENA    0x0000001FU
#define GPIO_PIN35_INT_ENA_M  (GPIO_PIN35_INT_ENA_V << GPIO_PIN35_INT_ENA_S)
#define GPIO_PIN35_INT_ENA_V  0x0000001FU
#define GPIO_PIN35_INT_ENA_S  13

/** GPIO_PIN36_REG register
 *  GPIO36 configuration register
 */
#define GPIO_PIN36_REG (DR_REG_GPIO_BASE + 0x184)
/** GPIO_PIN36_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN36_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN36_SYNC2_BYPASS_M  (GPIO_PIN36_SYNC2_BYPASS_V << GPIO_PIN36_SYNC2_BYPASS_S)
#define GPIO_PIN36_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN36_SYNC2_BYPASS_S  0
/** GPIO_PIN36_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN36_PAD_DRIVER    (BIT(2))
#define GPIO_PIN36_PAD_DRIVER_M  (GPIO_PIN36_PAD_DRIVER_V << GPIO_PIN36_PAD_DRIVER_S)
#define GPIO_PIN36_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN36_PAD_DRIVER_S  2
/** GPIO_PIN36_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN36_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN36_SYNC1_BYPASS_M  (GPIO_PIN36_SYNC1_BYPASS_V << GPIO_PIN36_SYNC1_BYPASS_S)
#define GPIO_PIN36_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN36_SYNC1_BYPASS_S  3
/** GPIO_PIN36_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN36_INT_TYPE    0x00000007U
#define GPIO_PIN36_INT_TYPE_M  (GPIO_PIN36_INT_TYPE_V << GPIO_PIN36_INT_TYPE_S)
#define GPIO_PIN36_INT_TYPE_V  0x00000007U
#define GPIO_PIN36_INT_TYPE_S  7
/** GPIO_PIN36_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN36_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN36_WAKEUP_ENABLE_M  (GPIO_PIN36_WAKEUP_ENABLE_V << GPIO_PIN36_WAKEUP_ENABLE_S)
#define GPIO_PIN36_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN36_WAKEUP_ENABLE_S  10
/** GPIO_PIN36_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN36_INT_ENA    0x0000001FU
#define GPIO_PIN36_INT_ENA_M  (GPIO_PIN36_INT_ENA_V << GPIO_PIN36_INT_ENA_S)
#define GPIO_PIN36_INT_ENA_V  0x0000001FU
#define GPIO_PIN36_INT_ENA_S  13

/** GPIO_PIN37_REG register
 *  GPIO37 configuration register
 */
#define GPIO_PIN37_REG (DR_REG_GPIO_BASE + 0x188)
/** GPIO_PIN37_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN37_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN37_SYNC2_BYPASS_M  (GPIO_PIN37_SYNC2_BYPASS_V << GPIO_PIN37_SYNC2_BYPASS_S)
#define GPIO_PIN37_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN37_SYNC2_BYPASS_S  0
/** GPIO_PIN37_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN37_PAD_DRIVER    (BIT(2))
#define GPIO_PIN37_PAD_DRIVER_M  (GPIO_PIN37_PAD_DRIVER_V << GPIO_PIN37_PAD_DRIVER_S)
#define GPIO_PIN37_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN37_PAD_DRIVER_S  2
/** GPIO_PIN37_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN37_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN37_SYNC1_BYPASS_M  (GPIO_PIN37_SYNC1_BYPASS_V << GPIO_PIN37_SYNC1_BYPASS_S)
#define GPIO_PIN37_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN37_SYNC1_BYPASS_S  3
/** GPIO_PIN37_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN37_INT_TYPE    0x00000007U
#define GPIO_PIN37_INT_TYPE_M  (GPIO_PIN37_INT_TYPE_V << GPIO_PIN37_INT_TYPE_S)
#define GPIO_PIN37_INT_TYPE_V  0x00000007U
#define GPIO_PIN37_INT_TYPE_S  7
/** GPIO_PIN37_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN37_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN37_WAKEUP_ENABLE_M  (GPIO_PIN37_WAKEUP_ENABLE_V << GPIO_PIN37_WAKEUP_ENABLE_S)
#define GPIO_PIN37_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN37_WAKEUP_ENABLE_S  10
/** GPIO_PIN37_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN37_INT_ENA    0x0000001FU
#define GPIO_PIN37_INT_ENA_M  (GPIO_PIN37_INT_ENA_V << GPIO_PIN37_INT_ENA_S)
#define GPIO_PIN37_INT_ENA_V  0x0000001FU
#define GPIO_PIN37_INT_ENA_S  13

/** GPIO_PIN38_REG register
 *  GPIO38 configuration register
 */
#define GPIO_PIN38_REG (DR_REG_GPIO_BASE + 0x18c)
/** GPIO_PIN38_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN38_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN38_SYNC2_BYPASS_M  (GPIO_PIN38_SYNC2_BYPASS_V << GPIO_PIN38_SYNC2_BYPASS_S)
#define GPIO_PIN38_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN38_SYNC2_BYPASS_S  0
/** GPIO_PIN38_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN38_PAD_DRIVER    (BIT(2))
#define GPIO_PIN38_PAD_DRIVER_M  (GPIO_PIN38_PAD_DRIVER_V << GPIO_PIN38_PAD_DRIVER_S)
#define GPIO_PIN38_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN38_PAD_DRIVER_S  2
/** GPIO_PIN38_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN38_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN38_SYNC1_BYPASS_M  (GPIO_PIN38_SYNC1_BYPASS_V << GPIO_PIN38_SYNC1_BYPASS_S)
#define GPIO_PIN38_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN38_SYNC1_BYPASS_S  3
/** GPIO_PIN38_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN38_INT_TYPE    0x00000007U
#define GPIO_PIN38_INT_TYPE_M  (GPIO_PIN38_INT_TYPE_V << GPIO_PIN38_INT_TYPE_S)
#define GPIO_PIN38_INT_TYPE_V  0x00000007U
#define GPIO_PIN38_INT_TYPE_S  7
/** GPIO_PIN38_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN38_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN38_WAKEUP_ENABLE_M  (GPIO_PIN38_WAKEUP_ENABLE_V << GPIO_PIN38_WAKEUP_ENABLE_S)
#define GPIO_PIN38_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN38_WAKEUP_ENABLE_S  10
/** GPIO_PIN38_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN38_INT_ENA    0x0000001FU
#define GPIO_PIN38_INT_ENA_M  (GPIO_PIN38_INT_ENA_V << GPIO_PIN38_INT_ENA_S)
#define GPIO_PIN38_INT_ENA_V  0x0000001FU
#define GPIO_PIN38_INT_ENA_S  13

/** GPIO_PIN39_REG register
 *  GPIO39 configuration register
 */
#define GPIO_PIN39_REG (DR_REG_GPIO_BASE + 0x190)
/** GPIO_PIN39_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN39_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN39_SYNC2_BYPASS_M  (GPIO_PIN39_SYNC2_BYPASS_V << GPIO_PIN39_SYNC2_BYPASS_S)
#define GPIO_PIN39_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN39_SYNC2_BYPASS_S  0
/** GPIO_PIN39_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN39_PAD_DRIVER    (BIT(2))
#define GPIO_PIN39_PAD_DRIVER_M  (GPIO_PIN39_PAD_DRIVER_V << GPIO_PIN39_PAD_DRIVER_S)
#define GPIO_PIN39_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN39_PAD_DRIVER_S  2
/** GPIO_PIN39_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN39_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN39_SYNC1_BYPASS_M  (GPIO_PIN39_SYNC1_BYPASS_V << GPIO_PIN39_SYNC1_BYPASS_S)
#define GPIO_PIN39_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN39_SYNC1_BYPASS_S  3
/** GPIO_PIN39_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN39_INT_TYPE    0x00000007U
#define GPIO_PIN39_INT_TYPE_M  (GPIO_PIN39_INT_TYPE_V << GPIO_PIN39_INT_TYPE_S)
#define GPIO_PIN39_INT_TYPE_V  0x00000007U
#define GPIO_PIN39_INT_TYPE_S  7
/** GPIO_PIN39_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN39_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN39_WAKEUP_ENABLE_M  (GPIO_PIN39_WAKEUP_ENABLE_V << GPIO_PIN39_WAKEUP_ENABLE_S)
#define GPIO_PIN39_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN39_WAKEUP_ENABLE_S  10
/** GPIO_PIN39_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN39_INT_ENA    0x0000001FU
#define GPIO_PIN39_INT_ENA_M  (GPIO_PIN39_INT_ENA_V << GPIO_PIN39_INT_ENA_S)
#define GPIO_PIN39_INT_ENA_V  0x0000001FU
#define GPIO_PIN39_INT_ENA_S  13

/** GPIO_PIN40_REG register
 *  GPIO40 configuration register
 */
#define GPIO_PIN40_REG (DR_REG_GPIO_BASE + 0x194)
/** GPIO_PIN40_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN40_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN40_SYNC2_BYPASS_M  (GPIO_PIN40_SYNC2_BYPASS_V << GPIO_PIN40_SYNC2_BYPASS_S)
#define GPIO_PIN40_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN40_SYNC2_BYPASS_S  0
/** GPIO_PIN40_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN40_PAD_DRIVER    (BIT(2))
#define GPIO_PIN40_PAD_DRIVER_M  (GPIO_PIN40_PAD_DRIVER_V << GPIO_PIN40_PAD_DRIVER_S)
#define GPIO_PIN40_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN40_PAD_DRIVER_S  2
/** GPIO_PIN40_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN40_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN40_SYNC1_BYPASS_M  (GPIO_PIN40_SYNC1_BYPASS_V << GPIO_PIN40_SYNC1_BYPASS_S)
#define GPIO_PIN40_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN40_SYNC1_BYPASS_S  3
/** GPIO_PIN40_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN40_INT_TYPE    0x00000007U
#define GPIO_PIN40_INT_TYPE_M  (GPIO_PIN40_INT_TYPE_V << GPIO_PIN40_INT_TYPE_S)
#define GPIO_PIN40_INT_TYPE_V  0x00000007U
#define GPIO_PIN40_INT_TYPE_S  7
/** GPIO_PIN40_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN40_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN40_WAKEUP_ENABLE_M  (GPIO_PIN40_WAKEUP_ENABLE_V << GPIO_PIN40_WAKEUP_ENABLE_S)
#define GPIO_PIN40_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN40_WAKEUP_ENABLE_S  10
/** GPIO_PIN40_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN40_INT_ENA    0x0000001FU
#define GPIO_PIN40_INT_ENA_M  (GPIO_PIN40_INT_ENA_V << GPIO_PIN40_INT_ENA_S)
#define GPIO_PIN40_INT_ENA_V  0x0000001FU
#define GPIO_PIN40_INT_ENA_S  13

/** GPIO_PIN41_REG register
 *  GPIO41 configuration register
 */
#define GPIO_PIN41_REG (DR_REG_GPIO_BASE + 0x198)
/** GPIO_PIN41_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN41_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN41_SYNC2_BYPASS_M  (GPIO_PIN41_SYNC2_BYPASS_V << GPIO_PIN41_SYNC2_BYPASS_S)
#define GPIO_PIN41_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN41_SYNC2_BYPASS_S  0
/** GPIO_PIN41_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN41_PAD_DRIVER    (BIT(2))
#define GPIO_PIN41_PAD_DRIVER_M  (GPIO_PIN41_PAD_DRIVER_V << GPIO_PIN41_PAD_DRIVER_S)
#define GPIO_PIN41_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN41_PAD_DRIVER_S  2
/** GPIO_PIN41_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN41_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN41_SYNC1_BYPASS_M  (GPIO_PIN41_SYNC1_BYPASS_V << GPIO_PIN41_SYNC1_BYPASS_S)
#define GPIO_PIN41_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN41_SYNC1_BYPASS_S  3
/** GPIO_PIN41_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN41_INT_TYPE    0x00000007U
#define GPIO_PIN41_INT_TYPE_M  (GPIO_PIN41_INT_TYPE_V << GPIO_PIN41_INT_TYPE_S)
#define GPIO_PIN41_INT_TYPE_V  0x00000007U
#define GPIO_PIN41_INT_TYPE_S  7
/** GPIO_PIN41_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN41_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN41_WAKEUP_ENABLE_M  (GPIO_PIN41_WAKEUP_ENABLE_V << GPIO_PIN41_WAKEUP_ENABLE_S)
#define GPIO_PIN41_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN41_WAKEUP_ENABLE_S  10
/** GPIO_PIN41_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN41_INT_ENA    0x0000001FU
#define GPIO_PIN41_INT_ENA_M  (GPIO_PIN41_INT_ENA_V << GPIO_PIN41_INT_ENA_S)
#define GPIO_PIN41_INT_ENA_V  0x0000001FU
#define GPIO_PIN41_INT_ENA_S  13

/** GPIO_PIN42_REG register
 *  GPIO42 configuration register
 */
#define GPIO_PIN42_REG (DR_REG_GPIO_BASE + 0x19c)
/** GPIO_PIN42_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN42_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN42_SYNC2_BYPASS_M  (GPIO_PIN42_SYNC2_BYPASS_V << GPIO_PIN42_SYNC2_BYPASS_S)
#define GPIO_PIN42_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN42_SYNC2_BYPASS_S  0
/** GPIO_PIN42_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN42_PAD_DRIVER    (BIT(2))
#define GPIO_PIN42_PAD_DRIVER_M  (GPIO_PIN42_PAD_DRIVER_V << GPIO_PIN42_PAD_DRIVER_S)
#define GPIO_PIN42_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN42_PAD_DRIVER_S  2
/** GPIO_PIN42_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN42_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN42_SYNC1_BYPASS_M  (GPIO_PIN42_SYNC1_BYPASS_V << GPIO_PIN42_SYNC1_BYPASS_S)
#define GPIO_PIN42_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN42_SYNC1_BYPASS_S  3
/** GPIO_PIN42_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN42_INT_TYPE    0x00000007U
#define GPIO_PIN42_INT_TYPE_M  (GPIO_PIN42_INT_TYPE_V << GPIO_PIN42_INT_TYPE_S)
#define GPIO_PIN42_INT_TYPE_V  0x00000007U
#define GPIO_PIN42_INT_TYPE_S  7
/** GPIO_PIN42_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN42_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN42_WAKEUP_ENABLE_M  (GPIO_PIN42_WAKEUP_ENABLE_V << GPIO_PIN42_WAKEUP_ENABLE_S)
#define GPIO_PIN42_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN42_WAKEUP_ENABLE_S  10
/** GPIO_PIN42_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN42_INT_ENA    0x0000001FU
#define GPIO_PIN42_INT_ENA_M  (GPIO_PIN42_INT_ENA_V << GPIO_PIN42_INT_ENA_S)
#define GPIO_PIN42_INT_ENA_V  0x0000001FU
#define GPIO_PIN42_INT_ENA_S  13

/** GPIO_PIN43_REG register
 *  GPIO43 configuration register
 */
#define GPIO_PIN43_REG (DR_REG_GPIO_BASE + 0x1a0)
/** GPIO_PIN43_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN43_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN43_SYNC2_BYPASS_M  (GPIO_PIN43_SYNC2_BYPASS_V << GPIO_PIN43_SYNC2_BYPASS_S)
#define GPIO_PIN43_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN43_SYNC2_BYPASS_S  0
/** GPIO_PIN43_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN43_PAD_DRIVER    (BIT(2))
#define GPIO_PIN43_PAD_DRIVER_M  (GPIO_PIN43_PAD_DRIVER_V << GPIO_PIN43_PAD_DRIVER_S)
#define GPIO_PIN43_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN43_PAD_DRIVER_S  2
/** GPIO_PIN43_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN43_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN43_SYNC1_BYPASS_M  (GPIO_PIN43_SYNC1_BYPASS_V << GPIO_PIN43_SYNC1_BYPASS_S)
#define GPIO_PIN43_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN43_SYNC1_BYPASS_S  3
/** GPIO_PIN43_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN43_INT_TYPE    0x00000007U
#define GPIO_PIN43_INT_TYPE_M  (GPIO_PIN43_INT_TYPE_V << GPIO_PIN43_INT_TYPE_S)
#define GPIO_PIN43_INT_TYPE_V  0x00000007U
#define GPIO_PIN43_INT_TYPE_S  7
/** GPIO_PIN43_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN43_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN43_WAKEUP_ENABLE_M  (GPIO_PIN43_WAKEUP_ENABLE_V << GPIO_PIN43_WAKEUP_ENABLE_S)
#define GPIO_PIN43_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN43_WAKEUP_ENABLE_S  10
/** GPIO_PIN43_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN43_INT_ENA    0x0000001FU
#define GPIO_PIN43_INT_ENA_M  (GPIO_PIN43_INT_ENA_V << GPIO_PIN43_INT_ENA_S)
#define GPIO_PIN43_INT_ENA_V  0x0000001FU
#define GPIO_PIN43_INT_ENA_S  13

/** GPIO_PIN44_REG register
 *  GPIO44 configuration register
 */
#define GPIO_PIN44_REG (DR_REG_GPIO_BASE + 0x1a4)
/** GPIO_PIN44_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN44_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN44_SYNC2_BYPASS_M  (GPIO_PIN44_SYNC2_BYPASS_V << GPIO_PIN44_SYNC2_BYPASS_S)
#define GPIO_PIN44_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN44_SYNC2_BYPASS_S  0
/** GPIO_PIN44_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN44_PAD_DRIVER    (BIT(2))
#define GPIO_PIN44_PAD_DRIVER_M  (GPIO_PIN44_PAD_DRIVER_V << GPIO_PIN44_PAD_DRIVER_S)
#define GPIO_PIN44_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN44_PAD_DRIVER_S  2
/** GPIO_PIN44_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN44_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN44_SYNC1_BYPASS_M  (GPIO_PIN44_SYNC1_BYPASS_V << GPIO_PIN44_SYNC1_BYPASS_S)
#define GPIO_PIN44_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN44_SYNC1_BYPASS_S  3
/** GPIO_PIN44_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN44_INT_TYPE    0x00000007U
#define GPIO_PIN44_INT_TYPE_M  (GPIO_PIN44_INT_TYPE_V << GPIO_PIN44_INT_TYPE_S)
#define GPIO_PIN44_INT_TYPE_V  0x00000007U
#define GPIO_PIN44_INT_TYPE_S  7
/** GPIO_PIN44_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN44_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN44_WAKEUP_ENABLE_M  (GPIO_PIN44_WAKEUP_ENABLE_V << GPIO_PIN44_WAKEUP_ENABLE_S)
#define GPIO_PIN44_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN44_WAKEUP_ENABLE_S  10
/** GPIO_PIN44_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN44_INT_ENA    0x0000001FU
#define GPIO_PIN44_INT_ENA_M  (GPIO_PIN44_INT_ENA_V << GPIO_PIN44_INT_ENA_S)
#define GPIO_PIN44_INT_ENA_V  0x0000001FU
#define GPIO_PIN44_INT_ENA_S  13

/** GPIO_PIN45_REG register
 *  GPIO45 configuration register
 */
#define GPIO_PIN45_REG (DR_REG_GPIO_BASE + 0x1a8)
/** GPIO_PIN45_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN45_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN45_SYNC2_BYPASS_M  (GPIO_PIN45_SYNC2_BYPASS_V << GPIO_PIN45_SYNC2_BYPASS_S)
#define GPIO_PIN45_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN45_SYNC2_BYPASS_S  0
/** GPIO_PIN45_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN45_PAD_DRIVER    (BIT(2))
#define GPIO_PIN45_PAD_DRIVER_M  (GPIO_PIN45_PAD_DRIVER_V << GPIO_PIN45_PAD_DRIVER_S)
#define GPIO_PIN45_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN45_PAD_DRIVER_S  2
/** GPIO_PIN45_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN45_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN45_SYNC1_BYPASS_M  (GPIO_PIN45_SYNC1_BYPASS_V << GPIO_PIN45_SYNC1_BYPASS_S)
#define GPIO_PIN45_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN45_SYNC1_BYPASS_S  3
/** GPIO_PIN45_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN45_INT_TYPE    0x00000007U
#define GPIO_PIN45_INT_TYPE_M  (GPIO_PIN45_INT_TYPE_V << GPIO_PIN45_INT_TYPE_S)
#define GPIO_PIN45_INT_TYPE_V  0x00000007U
#define GPIO_PIN45_INT_TYPE_S  7
/** GPIO_PIN45_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN45_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN45_WAKEUP_ENABLE_M  (GPIO_PIN45_WAKEUP_ENABLE_V << GPIO_PIN45_WAKEUP_ENABLE_S)
#define GPIO_PIN45_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN45_WAKEUP_ENABLE_S  10
/** GPIO_PIN45_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN45_INT_ENA    0x0000001FU
#define GPIO_PIN45_INT_ENA_M  (GPIO_PIN45_INT_ENA_V << GPIO_PIN45_INT_ENA_S)
#define GPIO_PIN45_INT_ENA_V  0x0000001FU
#define GPIO_PIN45_INT_ENA_S  13

/** GPIO_PIN46_REG register
 *  GPIO46 configuration register
 */
#define GPIO_PIN46_REG (DR_REG_GPIO_BASE + 0x1ac)
/** GPIO_PIN46_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN46_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN46_SYNC2_BYPASS_M  (GPIO_PIN46_SYNC2_BYPASS_V << GPIO_PIN46_SYNC2_BYPASS_S)
#define GPIO_PIN46_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN46_SYNC2_BYPASS_S  0
/** GPIO_PIN46_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN46_PAD_DRIVER    (BIT(2))
#define GPIO_PIN46_PAD_DRIVER_M  (GPIO_PIN46_PAD_DRIVER_V << GPIO_PIN46_PAD_DRIVER_S)
#define GPIO_PIN46_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN46_PAD_DRIVER_S  2
/** GPIO_PIN46_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN46_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN46_SYNC1_BYPASS_M  (GPIO_PIN46_SYNC1_BYPASS_V << GPIO_PIN46_SYNC1_BYPASS_S)
#define GPIO_PIN46_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN46_SYNC1_BYPASS_S  3
/** GPIO_PIN46_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN46_INT_TYPE    0x00000007U
#define GPIO_PIN46_INT_TYPE_M  (GPIO_PIN46_INT_TYPE_V << GPIO_PIN46_INT_TYPE_S)
#define GPIO_PIN46_INT_TYPE_V  0x00000007U
#define GPIO_PIN46_INT_TYPE_S  7
/** GPIO_PIN46_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN46_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN46_WAKEUP_ENABLE_M  (GPIO_PIN46_WAKEUP_ENABLE_V << GPIO_PIN46_WAKEUP_ENABLE_S)
#define GPIO_PIN46_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN46_WAKEUP_ENABLE_S  10
/** GPIO_PIN46_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN46_INT_ENA    0x0000001FU
#define GPIO_PIN46_INT_ENA_M  (GPIO_PIN46_INT_ENA_V << GPIO_PIN46_INT_ENA_S)
#define GPIO_PIN46_INT_ENA_V  0x0000001FU
#define GPIO_PIN46_INT_ENA_S  13

/** GPIO_PIN47_REG register
 *  GPIO47 configuration register
 */
#define GPIO_PIN47_REG (DR_REG_GPIO_BASE + 0x1b0)
/** GPIO_PIN47_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN47_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN47_SYNC2_BYPASS_M  (GPIO_PIN47_SYNC2_BYPASS_V << GPIO_PIN47_SYNC2_BYPASS_S)
#define GPIO_PIN47_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN47_SYNC2_BYPASS_S  0
/** GPIO_PIN47_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN47_PAD_DRIVER    (BIT(2))
#define GPIO_PIN47_PAD_DRIVER_M  (GPIO_PIN47_PAD_DRIVER_V << GPIO_PIN47_PAD_DRIVER_S)
#define GPIO_PIN47_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN47_PAD_DRIVER_S  2
/** GPIO_PIN47_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN47_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN47_SYNC1_BYPASS_M  (GPIO_PIN47_SYNC1_BYPASS_V << GPIO_PIN47_SYNC1_BYPASS_S)
#define GPIO_PIN47_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN47_SYNC1_BYPASS_S  3
/** GPIO_PIN47_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN47_INT_TYPE    0x00000007U
#define GPIO_PIN47_INT_TYPE_M  (GPIO_PIN47_INT_TYPE_V << GPIO_PIN47_INT_TYPE_S)
#define GPIO_PIN47_INT_TYPE_V  0x00000007U
#define GPIO_PIN47_INT_TYPE_S  7
/** GPIO_PIN47_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN47_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN47_WAKEUP_ENABLE_M  (GPIO_PIN47_WAKEUP_ENABLE_V << GPIO_PIN47_WAKEUP_ENABLE_S)
#define GPIO_PIN47_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN47_WAKEUP_ENABLE_S  10
/** GPIO_PIN47_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN47_INT_ENA    0x0000001FU
#define GPIO_PIN47_INT_ENA_M  (GPIO_PIN47_INT_ENA_V << GPIO_PIN47_INT_ENA_S)
#define GPIO_PIN47_INT_ENA_V  0x0000001FU
#define GPIO_PIN47_INT_ENA_S  13

/** GPIO_PIN48_REG register
 *  GPIO48 configuration register
 */
#define GPIO_PIN48_REG (DR_REG_GPIO_BASE + 0x1b4)
/** GPIO_PIN48_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN48_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN48_SYNC2_BYPASS_M  (GPIO_PIN48_SYNC2_BYPASS_V << GPIO_PIN48_SYNC2_BYPASS_S)
#define GPIO_PIN48_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN48_SYNC2_BYPASS_S  0
/** GPIO_PIN48_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN48_PAD_DRIVER    (BIT(2))
#define GPIO_PIN48_PAD_DRIVER_M  (GPIO_PIN48_PAD_DRIVER_V << GPIO_PIN48_PAD_DRIVER_S)
#define GPIO_PIN48_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN48_PAD_DRIVER_S  2
/** GPIO_PIN48_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN48_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN48_SYNC1_BYPASS_M  (GPIO_PIN48_SYNC1_BYPASS_V << GPIO_PIN48_SYNC1_BYPASS_S)
#define GPIO_PIN48_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN48_SYNC1_BYPASS_S  3
/** GPIO_PIN48_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN48_INT_TYPE    0x00000007U
#define GPIO_PIN48_INT_TYPE_M  (GPIO_PIN48_INT_TYPE_V << GPIO_PIN48_INT_TYPE_S)
#define GPIO_PIN48_INT_TYPE_V  0x00000007U
#define GPIO_PIN48_INT_TYPE_S  7
/** GPIO_PIN48_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN48_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN48_WAKEUP_ENABLE_M  (GPIO_PIN48_WAKEUP_ENABLE_V << GPIO_PIN48_WAKEUP_ENABLE_S)
#define GPIO_PIN48_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN48_WAKEUP_ENABLE_S  10
/** GPIO_PIN48_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN48_INT_ENA    0x0000001FU
#define GPIO_PIN48_INT_ENA_M  (GPIO_PIN48_INT_ENA_V << GPIO_PIN48_INT_ENA_S)
#define GPIO_PIN48_INT_ENA_V  0x0000001FU
#define GPIO_PIN48_INT_ENA_S  13

/** GPIO_PIN49_REG register
 *  GPIO49 configuration register
 */
#define GPIO_PIN49_REG (DR_REG_GPIO_BASE + 0x1b8)
/** GPIO_PIN49_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN49_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN49_SYNC2_BYPASS_M  (GPIO_PIN49_SYNC2_BYPASS_V << GPIO_PIN49_SYNC2_BYPASS_S)
#define GPIO_PIN49_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN49_SYNC2_BYPASS_S  0
/** GPIO_PIN49_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN49_PAD_DRIVER    (BIT(2))
#define GPIO_PIN49_PAD_DRIVER_M  (GPIO_PIN49_PAD_DRIVER_V << GPIO_PIN49_PAD_DRIVER_S)
#define GPIO_PIN49_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN49_PAD_DRIVER_S  2
/** GPIO_PIN49_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN49_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN49_SYNC1_BYPASS_M  (GPIO_PIN49_SYNC1_BYPASS_V << GPIO_PIN49_SYNC1_BYPASS_S)
#define GPIO_PIN49_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN49_SYNC1_BYPASS_S  3
/** GPIO_PIN49_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN49_INT_TYPE    0x00000007U
#define GPIO_PIN49_INT_TYPE_M  (GPIO_PIN49_INT_TYPE_V << GPIO_PIN49_INT_TYPE_S)
#define GPIO_PIN49_INT_TYPE_V  0x00000007U
#define GPIO_PIN49_INT_TYPE_S  7
/** GPIO_PIN49_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN49_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN49_WAKEUP_ENABLE_M  (GPIO_PIN49_WAKEUP_ENABLE_V << GPIO_PIN49_WAKEUP_ENABLE_S)
#define GPIO_PIN49_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN49_WAKEUP_ENABLE_S  10
/** GPIO_PIN49_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN49_INT_ENA    0x0000001FU
#define GPIO_PIN49_INT_ENA_M  (GPIO_PIN49_INT_ENA_V << GPIO_PIN49_INT_ENA_S)
#define GPIO_PIN49_INT_ENA_V  0x0000001FU
#define GPIO_PIN49_INT_ENA_S  13

/** GPIO_PIN50_REG register
 *  GPIO50 configuration register
 */
#define GPIO_PIN50_REG (DR_REG_GPIO_BASE + 0x1bc)
/** GPIO_PIN50_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN50_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN50_SYNC2_BYPASS_M  (GPIO_PIN50_SYNC2_BYPASS_V << GPIO_PIN50_SYNC2_BYPASS_S)
#define GPIO_PIN50_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN50_SYNC2_BYPASS_S  0
/** GPIO_PIN50_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN50_PAD_DRIVER    (BIT(2))
#define GPIO_PIN50_PAD_DRIVER_M  (GPIO_PIN50_PAD_DRIVER_V << GPIO_PIN50_PAD_DRIVER_S)
#define GPIO_PIN50_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN50_PAD_DRIVER_S  2
/** GPIO_PIN50_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN50_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN50_SYNC1_BYPASS_M  (GPIO_PIN50_SYNC1_BYPASS_V << GPIO_PIN50_SYNC1_BYPASS_S)
#define GPIO_PIN50_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN50_SYNC1_BYPASS_S  3
/** GPIO_PIN50_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN50_INT_TYPE    0x00000007U
#define GPIO_PIN50_INT_TYPE_M  (GPIO_PIN50_INT_TYPE_V << GPIO_PIN50_INT_TYPE_S)
#define GPIO_PIN50_INT_TYPE_V  0x00000007U
#define GPIO_PIN50_INT_TYPE_S  7
/** GPIO_PIN50_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN50_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN50_WAKEUP_ENABLE_M  (GPIO_PIN50_WAKEUP_ENABLE_V << GPIO_PIN50_WAKEUP_ENABLE_S)
#define GPIO_PIN50_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN50_WAKEUP_ENABLE_S  10
/** GPIO_PIN50_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN50_INT_ENA    0x0000001FU
#define GPIO_PIN50_INT_ENA_M  (GPIO_PIN50_INT_ENA_V << GPIO_PIN50_INT_ENA_S)
#define GPIO_PIN50_INT_ENA_V  0x0000001FU
#define GPIO_PIN50_INT_ENA_S  13

/** GPIO_PIN51_REG register
 *  GPIO51 configuration register
 */
#define GPIO_PIN51_REG (DR_REG_GPIO_BASE + 0x1c0)
/** GPIO_PIN51_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN51_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN51_SYNC2_BYPASS_M  (GPIO_PIN51_SYNC2_BYPASS_V << GPIO_PIN51_SYNC2_BYPASS_S)
#define GPIO_PIN51_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN51_SYNC2_BYPASS_S  0
/** GPIO_PIN51_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN51_PAD_DRIVER    (BIT(2))
#define GPIO_PIN51_PAD_DRIVER_M  (GPIO_PIN51_PAD_DRIVER_V << GPIO_PIN51_PAD_DRIVER_S)
#define GPIO_PIN51_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN51_PAD_DRIVER_S  2
/** GPIO_PIN51_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN51_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN51_SYNC1_BYPASS_M  (GPIO_PIN51_SYNC1_BYPASS_V << GPIO_PIN51_SYNC1_BYPASS_S)
#define GPIO_PIN51_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN51_SYNC1_BYPASS_S  3
/** GPIO_PIN51_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN51_INT_TYPE    0x00000007U
#define GPIO_PIN51_INT_TYPE_M  (GPIO_PIN51_INT_TYPE_V << GPIO_PIN51_INT_TYPE_S)
#define GPIO_PIN51_INT_TYPE_V  0x00000007U
#define GPIO_PIN51_INT_TYPE_S  7
/** GPIO_PIN51_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN51_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN51_WAKEUP_ENABLE_M  (GPIO_PIN51_WAKEUP_ENABLE_V << GPIO_PIN51_WAKEUP_ENABLE_S)
#define GPIO_PIN51_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN51_WAKEUP_ENABLE_S  10
/** GPIO_PIN51_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN51_INT_ENA    0x0000001FU
#define GPIO_PIN51_INT_ENA_M  (GPIO_PIN51_INT_ENA_V << GPIO_PIN51_INT_ENA_S)
#define GPIO_PIN51_INT_ENA_V  0x0000001FU
#define GPIO_PIN51_INT_ENA_S  13

/** GPIO_PIN52_REG register
 *  GPIO52 configuration register
 */
#define GPIO_PIN52_REG (DR_REG_GPIO_BASE + 0x1c4)
/** GPIO_PIN52_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN52_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN52_SYNC2_BYPASS_M  (GPIO_PIN52_SYNC2_BYPASS_V << GPIO_PIN52_SYNC2_BYPASS_S)
#define GPIO_PIN52_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN52_SYNC2_BYPASS_S  0
/** GPIO_PIN52_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN52_PAD_DRIVER    (BIT(2))
#define GPIO_PIN52_PAD_DRIVER_M  (GPIO_PIN52_PAD_DRIVER_V << GPIO_PIN52_PAD_DRIVER_S)
#define GPIO_PIN52_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN52_PAD_DRIVER_S  2
/** GPIO_PIN52_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN52_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN52_SYNC1_BYPASS_M  (GPIO_PIN52_SYNC1_BYPASS_V << GPIO_PIN52_SYNC1_BYPASS_S)
#define GPIO_PIN52_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN52_SYNC1_BYPASS_S  3
/** GPIO_PIN52_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN52_INT_TYPE    0x00000007U
#define GPIO_PIN52_INT_TYPE_M  (GPIO_PIN52_INT_TYPE_V << GPIO_PIN52_INT_TYPE_S)
#define GPIO_PIN52_INT_TYPE_V  0x00000007U
#define GPIO_PIN52_INT_TYPE_S  7
/** GPIO_PIN52_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN52_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN52_WAKEUP_ENABLE_M  (GPIO_PIN52_WAKEUP_ENABLE_V << GPIO_PIN52_WAKEUP_ENABLE_S)
#define GPIO_PIN52_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN52_WAKEUP_ENABLE_S  10
/** GPIO_PIN52_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN52_INT_ENA    0x0000001FU
#define GPIO_PIN52_INT_ENA_M  (GPIO_PIN52_INT_ENA_V << GPIO_PIN52_INT_ENA_S)
#define GPIO_PIN52_INT_ENA_V  0x0000001FU
#define GPIO_PIN52_INT_ENA_S  13

/** GPIO_PIN53_REG register
 *  GPIO53 configuration register
 */
#define GPIO_PIN53_REG (DR_REG_GPIO_BASE + 0x1c8)
/** GPIO_PIN53_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN53_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN53_SYNC2_BYPASS_M  (GPIO_PIN53_SYNC2_BYPASS_V << GPIO_PIN53_SYNC2_BYPASS_S)
#define GPIO_PIN53_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN53_SYNC2_BYPASS_S  0
/** GPIO_PIN53_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN53_PAD_DRIVER    (BIT(2))
#define GPIO_PIN53_PAD_DRIVER_M  (GPIO_PIN53_PAD_DRIVER_V << GPIO_PIN53_PAD_DRIVER_S)
#define GPIO_PIN53_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN53_PAD_DRIVER_S  2
/** GPIO_PIN53_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN53_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN53_SYNC1_BYPASS_M  (GPIO_PIN53_SYNC1_BYPASS_V << GPIO_PIN53_SYNC1_BYPASS_S)
#define GPIO_PIN53_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN53_SYNC1_BYPASS_S  3
/** GPIO_PIN53_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN53_INT_TYPE    0x00000007U
#define GPIO_PIN53_INT_TYPE_M  (GPIO_PIN53_INT_TYPE_V << GPIO_PIN53_INT_TYPE_S)
#define GPIO_PIN53_INT_TYPE_V  0x00000007U
#define GPIO_PIN53_INT_TYPE_S  7
/** GPIO_PIN53_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN53_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN53_WAKEUP_ENABLE_M  (GPIO_PIN53_WAKEUP_ENABLE_V << GPIO_PIN53_WAKEUP_ENABLE_S)
#define GPIO_PIN53_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN53_WAKEUP_ENABLE_S  10
/** GPIO_PIN53_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN53_INT_ENA    0x0000001FU
#define GPIO_PIN53_INT_ENA_M  (GPIO_PIN53_INT_ENA_V << GPIO_PIN53_INT_ENA_S)
#define GPIO_PIN53_INT_ENA_V  0x0000001FU
#define GPIO_PIN53_INT_ENA_S  13

/** GPIO_PIN54_REG register
 *  GPIO54 configuration register
 */
#define GPIO_PIN54_REG (DR_REG_GPIO_BASE + 0x1cc)
/** GPIO_PIN54_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN54_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN54_SYNC2_BYPASS_M  (GPIO_PIN54_SYNC2_BYPASS_V << GPIO_PIN54_SYNC2_BYPASS_S)
#define GPIO_PIN54_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN54_SYNC2_BYPASS_S  0
/** GPIO_PIN54_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN54_PAD_DRIVER    (BIT(2))
#define GPIO_PIN54_PAD_DRIVER_M  (GPIO_PIN54_PAD_DRIVER_V << GPIO_PIN54_PAD_DRIVER_S)
#define GPIO_PIN54_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN54_PAD_DRIVER_S  2
/** GPIO_PIN54_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN54_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN54_SYNC1_BYPASS_M  (GPIO_PIN54_SYNC1_BYPASS_V << GPIO_PIN54_SYNC1_BYPASS_S)
#define GPIO_PIN54_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN54_SYNC1_BYPASS_S  3
/** GPIO_PIN54_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN54_INT_TYPE    0x00000007U
#define GPIO_PIN54_INT_TYPE_M  (GPIO_PIN54_INT_TYPE_V << GPIO_PIN54_INT_TYPE_S)
#define GPIO_PIN54_INT_TYPE_V  0x00000007U
#define GPIO_PIN54_INT_TYPE_S  7
/** GPIO_PIN54_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN54_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN54_WAKEUP_ENABLE_M  (GPIO_PIN54_WAKEUP_ENABLE_V << GPIO_PIN54_WAKEUP_ENABLE_S)
#define GPIO_PIN54_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN54_WAKEUP_ENABLE_S  10
/** GPIO_PIN54_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN54_INT_ENA    0x0000001FU
#define GPIO_PIN54_INT_ENA_M  (GPIO_PIN54_INT_ENA_V << GPIO_PIN54_INT_ENA_S)
#define GPIO_PIN54_INT_ENA_V  0x0000001FU
#define GPIO_PIN54_INT_ENA_S  13

/** GPIO_PIN55_REG register
 *  GPIO55 configuration register
 */
#define GPIO_PIN55_REG (DR_REG_GPIO_BASE + 0x1d0)
/** GPIO_PIN55_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN55_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN55_SYNC2_BYPASS_M  (GPIO_PIN55_SYNC2_BYPASS_V << GPIO_PIN55_SYNC2_BYPASS_S)
#define GPIO_PIN55_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN55_SYNC2_BYPASS_S  0
/** GPIO_PIN55_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN55_PAD_DRIVER    (BIT(2))
#define GPIO_PIN55_PAD_DRIVER_M  (GPIO_PIN55_PAD_DRIVER_V << GPIO_PIN55_PAD_DRIVER_S)
#define GPIO_PIN55_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN55_PAD_DRIVER_S  2
/** GPIO_PIN55_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN55_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN55_SYNC1_BYPASS_M  (GPIO_PIN55_SYNC1_BYPASS_V << GPIO_PIN55_SYNC1_BYPASS_S)
#define GPIO_PIN55_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN55_SYNC1_BYPASS_S  3
/** GPIO_PIN55_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN55_INT_TYPE    0x00000007U
#define GPIO_PIN55_INT_TYPE_M  (GPIO_PIN55_INT_TYPE_V << GPIO_PIN55_INT_TYPE_S)
#define GPIO_PIN55_INT_TYPE_V  0x00000007U
#define GPIO_PIN55_INT_TYPE_S  7
/** GPIO_PIN55_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN55_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN55_WAKEUP_ENABLE_M  (GPIO_PIN55_WAKEUP_ENABLE_V << GPIO_PIN55_WAKEUP_ENABLE_S)
#define GPIO_PIN55_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN55_WAKEUP_ENABLE_S  10
/** GPIO_PIN55_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN55_INT_ENA    0x0000001FU
#define GPIO_PIN55_INT_ENA_M  (GPIO_PIN55_INT_ENA_V << GPIO_PIN55_INT_ENA_S)
#define GPIO_PIN55_INT_ENA_V  0x0000001FU
#define GPIO_PIN55_INT_ENA_S  13

/** GPIO_PIN56_REG register
 *  GPIO56 configuration register
 */
#define GPIO_PIN56_REG (DR_REG_GPIO_BASE + 0x1d4)
/** GPIO_PIN56_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN56_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN56_SYNC2_BYPASS_M  (GPIO_PIN56_SYNC2_BYPASS_V << GPIO_PIN56_SYNC2_BYPASS_S)
#define GPIO_PIN56_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN56_SYNC2_BYPASS_S  0
/** GPIO_PIN56_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN56_PAD_DRIVER    (BIT(2))
#define GPIO_PIN56_PAD_DRIVER_M  (GPIO_PIN56_PAD_DRIVER_V << GPIO_PIN56_PAD_DRIVER_S)
#define GPIO_PIN56_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN56_PAD_DRIVER_S  2
/** GPIO_PIN56_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN56_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN56_SYNC1_BYPASS_M  (GPIO_PIN56_SYNC1_BYPASS_V << GPIO_PIN56_SYNC1_BYPASS_S)
#define GPIO_PIN56_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN56_SYNC1_BYPASS_S  3
/** GPIO_PIN56_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN56_INT_TYPE    0x00000007U
#define GPIO_PIN56_INT_TYPE_M  (GPIO_PIN56_INT_TYPE_V << GPIO_PIN56_INT_TYPE_S)
#define GPIO_PIN56_INT_TYPE_V  0x00000007U
#define GPIO_PIN56_INT_TYPE_S  7
/** GPIO_PIN56_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN56_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN56_WAKEUP_ENABLE_M  (GPIO_PIN56_WAKEUP_ENABLE_V << GPIO_PIN56_WAKEUP_ENABLE_S)
#define GPIO_PIN56_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN56_WAKEUP_ENABLE_S  10
/** GPIO_PIN56_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN56_INT_ENA    0x0000001FU
#define GPIO_PIN56_INT_ENA_M  (GPIO_PIN56_INT_ENA_V << GPIO_PIN56_INT_ENA_S)
#define GPIO_PIN56_INT_ENA_V  0x0000001FU
#define GPIO_PIN56_INT_ENA_S  13

/** GPIO_PIN57_REG register
 *  GPIO57 configuration register
 */
#define GPIO_PIN57_REG (DR_REG_GPIO_BASE + 0x1d8)
/** GPIO_PIN57_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN57_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN57_SYNC2_BYPASS_M  (GPIO_PIN57_SYNC2_BYPASS_V << GPIO_PIN57_SYNC2_BYPASS_S)
#define GPIO_PIN57_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN57_SYNC2_BYPASS_S  0
/** GPIO_PIN57_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN57_PAD_DRIVER    (BIT(2))
#define GPIO_PIN57_PAD_DRIVER_M  (GPIO_PIN57_PAD_DRIVER_V << GPIO_PIN57_PAD_DRIVER_S)
#define GPIO_PIN57_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN57_PAD_DRIVER_S  2
/** GPIO_PIN57_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN57_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN57_SYNC1_BYPASS_M  (GPIO_PIN57_SYNC1_BYPASS_V << GPIO_PIN57_SYNC1_BYPASS_S)
#define GPIO_PIN57_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN57_SYNC1_BYPASS_S  3
/** GPIO_PIN57_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN57_INT_TYPE    0x00000007U
#define GPIO_PIN57_INT_TYPE_M  (GPIO_PIN57_INT_TYPE_V << GPIO_PIN57_INT_TYPE_S)
#define GPIO_PIN57_INT_TYPE_V  0x00000007U
#define GPIO_PIN57_INT_TYPE_S  7
/** GPIO_PIN57_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN57_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN57_WAKEUP_ENABLE_M  (GPIO_PIN57_WAKEUP_ENABLE_V << GPIO_PIN57_WAKEUP_ENABLE_S)
#define GPIO_PIN57_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN57_WAKEUP_ENABLE_S  10
/** GPIO_PIN57_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN57_INT_ENA    0x0000001FU
#define GPIO_PIN57_INT_ENA_M  (GPIO_PIN57_INT_ENA_V << GPIO_PIN57_INT_ENA_S)
#define GPIO_PIN57_INT_ENA_V  0x0000001FU
#define GPIO_PIN57_INT_ENA_S  13

/** GPIO_PIN58_REG register
 *  GPIO58 configuration register
 */
#define GPIO_PIN58_REG (DR_REG_GPIO_BASE + 0x1dc)
/** GPIO_PIN58_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN58_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN58_SYNC2_BYPASS_M  (GPIO_PIN58_SYNC2_BYPASS_V << GPIO_PIN58_SYNC2_BYPASS_S)
#define GPIO_PIN58_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN58_SYNC2_BYPASS_S  0
/** GPIO_PIN58_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN58_PAD_DRIVER    (BIT(2))
#define GPIO_PIN58_PAD_DRIVER_M  (GPIO_PIN58_PAD_DRIVER_V << GPIO_PIN58_PAD_DRIVER_S)
#define GPIO_PIN58_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN58_PAD_DRIVER_S  2
/** GPIO_PIN58_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN58_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN58_SYNC1_BYPASS_M  (GPIO_PIN58_SYNC1_BYPASS_V << GPIO_PIN58_SYNC1_BYPASS_S)
#define GPIO_PIN58_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN58_SYNC1_BYPASS_S  3
/** GPIO_PIN58_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN58_INT_TYPE    0x00000007U
#define GPIO_PIN58_INT_TYPE_M  (GPIO_PIN58_INT_TYPE_V << GPIO_PIN58_INT_TYPE_S)
#define GPIO_PIN58_INT_TYPE_V  0x00000007U
#define GPIO_PIN58_INT_TYPE_S  7
/** GPIO_PIN58_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN58_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN58_WAKEUP_ENABLE_M  (GPIO_PIN58_WAKEUP_ENABLE_V << GPIO_PIN58_WAKEUP_ENABLE_S)
#define GPIO_PIN58_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN58_WAKEUP_ENABLE_S  10
/** GPIO_PIN58_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN58_INT_ENA    0x0000001FU
#define GPIO_PIN58_INT_ENA_M  (GPIO_PIN58_INT_ENA_V << GPIO_PIN58_INT_ENA_S)
#define GPIO_PIN58_INT_ENA_V  0x0000001FU
#define GPIO_PIN58_INT_ENA_S  13

/** GPIO_PIN59_REG register
 *  GPIO59 configuration register
 */
#define GPIO_PIN59_REG (DR_REG_GPIO_BASE + 0x1e0)
/** GPIO_PIN59_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN59_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN59_SYNC2_BYPASS_M  (GPIO_PIN59_SYNC2_BYPASS_V << GPIO_PIN59_SYNC2_BYPASS_S)
#define GPIO_PIN59_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN59_SYNC2_BYPASS_S  0
/** GPIO_PIN59_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN59_PAD_DRIVER    (BIT(2))
#define GPIO_PIN59_PAD_DRIVER_M  (GPIO_PIN59_PAD_DRIVER_V << GPIO_PIN59_PAD_DRIVER_S)
#define GPIO_PIN59_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN59_PAD_DRIVER_S  2
/** GPIO_PIN59_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN59_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN59_SYNC1_BYPASS_M  (GPIO_PIN59_SYNC1_BYPASS_V << GPIO_PIN59_SYNC1_BYPASS_S)
#define GPIO_PIN59_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN59_SYNC1_BYPASS_S  3
/** GPIO_PIN59_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN59_INT_TYPE    0x00000007U
#define GPIO_PIN59_INT_TYPE_M  (GPIO_PIN59_INT_TYPE_V << GPIO_PIN59_INT_TYPE_S)
#define GPIO_PIN59_INT_TYPE_V  0x00000007U
#define GPIO_PIN59_INT_TYPE_S  7
/** GPIO_PIN59_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN59_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN59_WAKEUP_ENABLE_M  (GPIO_PIN59_WAKEUP_ENABLE_V << GPIO_PIN59_WAKEUP_ENABLE_S)
#define GPIO_PIN59_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN59_WAKEUP_ENABLE_S  10
/** GPIO_PIN59_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN59_INT_ENA    0x0000001FU
#define GPIO_PIN59_INT_ENA_M  (GPIO_PIN59_INT_ENA_V << GPIO_PIN59_INT_ENA_S)
#define GPIO_PIN59_INT_ENA_V  0x0000001FU
#define GPIO_PIN59_INT_ENA_S  13

/** GPIO_PIN60_REG register
 *  GPIO60 configuration register
 */
#define GPIO_PIN60_REG (DR_REG_GPIO_BASE + 0x1e4)
/** GPIO_PIN60_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN60_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN60_SYNC2_BYPASS_M  (GPIO_PIN60_SYNC2_BYPASS_V << GPIO_PIN60_SYNC2_BYPASS_S)
#define GPIO_PIN60_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN60_SYNC2_BYPASS_S  0
/** GPIO_PIN60_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN60_PAD_DRIVER    (BIT(2))
#define GPIO_PIN60_PAD_DRIVER_M  (GPIO_PIN60_PAD_DRIVER_V << GPIO_PIN60_PAD_DRIVER_S)
#define GPIO_PIN60_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN60_PAD_DRIVER_S  2
/** GPIO_PIN60_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN60_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN60_SYNC1_BYPASS_M  (GPIO_PIN60_SYNC1_BYPASS_V << GPIO_PIN60_SYNC1_BYPASS_S)
#define GPIO_PIN60_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN60_SYNC1_BYPASS_S  3
/** GPIO_PIN60_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN60_INT_TYPE    0x00000007U
#define GPIO_PIN60_INT_TYPE_M  (GPIO_PIN60_INT_TYPE_V << GPIO_PIN60_INT_TYPE_S)
#define GPIO_PIN60_INT_TYPE_V  0x00000007U
#define GPIO_PIN60_INT_TYPE_S  7
/** GPIO_PIN60_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN60_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN60_WAKEUP_ENABLE_M  (GPIO_PIN60_WAKEUP_ENABLE_V << GPIO_PIN60_WAKEUP_ENABLE_S)
#define GPIO_PIN60_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN60_WAKEUP_ENABLE_S  10
/** GPIO_PIN60_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN60_INT_ENA    0x0000001FU
#define GPIO_PIN60_INT_ENA_M  (GPIO_PIN60_INT_ENA_V << GPIO_PIN60_INT_ENA_S)
#define GPIO_PIN60_INT_ENA_V  0x0000001FU
#define GPIO_PIN60_INT_ENA_S  13

/** GPIO_PIN61_REG register
 *  GPIO61 configuration register
 */
#define GPIO_PIN61_REG (DR_REG_GPIO_BASE + 0x1e8)
/** GPIO_PIN61_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN61_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN61_SYNC2_BYPASS_M  (GPIO_PIN61_SYNC2_BYPASS_V << GPIO_PIN61_SYNC2_BYPASS_S)
#define GPIO_PIN61_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN61_SYNC2_BYPASS_S  0
/** GPIO_PIN61_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN61_PAD_DRIVER    (BIT(2))
#define GPIO_PIN61_PAD_DRIVER_M  (GPIO_PIN61_PAD_DRIVER_V << GPIO_PIN61_PAD_DRIVER_S)
#define GPIO_PIN61_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN61_PAD_DRIVER_S  2
/** GPIO_PIN61_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN61_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN61_SYNC1_BYPASS_M  (GPIO_PIN61_SYNC1_BYPASS_V << GPIO_PIN61_SYNC1_BYPASS_S)
#define GPIO_PIN61_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN61_SYNC1_BYPASS_S  3
/** GPIO_PIN61_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN61_INT_TYPE    0x00000007U
#define GPIO_PIN61_INT_TYPE_M  (GPIO_PIN61_INT_TYPE_V << GPIO_PIN61_INT_TYPE_S)
#define GPIO_PIN61_INT_TYPE_V  0x00000007U
#define GPIO_PIN61_INT_TYPE_S  7
/** GPIO_PIN61_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN61_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN61_WAKEUP_ENABLE_M  (GPIO_PIN61_WAKEUP_ENABLE_V << GPIO_PIN61_WAKEUP_ENABLE_S)
#define GPIO_PIN61_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN61_WAKEUP_ENABLE_S  10
/** GPIO_PIN61_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN61_INT_ENA    0x0000001FU
#define GPIO_PIN61_INT_ENA_M  (GPIO_PIN61_INT_ENA_V << GPIO_PIN61_INT_ENA_S)
#define GPIO_PIN61_INT_ENA_V  0x0000001FU
#define GPIO_PIN61_INT_ENA_S  13

/** GPIO_PIN62_REG register
 *  GPIO62 configuration register
 */
#define GPIO_PIN62_REG (DR_REG_GPIO_BASE + 0x1ec)
/** GPIO_PIN62_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN62_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN62_SYNC2_BYPASS_M  (GPIO_PIN62_SYNC2_BYPASS_V << GPIO_PIN62_SYNC2_BYPASS_S)
#define GPIO_PIN62_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN62_SYNC2_BYPASS_S  0
/** GPIO_PIN62_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode.
 *  0: Normal output
 *  1: Open drain output
 */
#define GPIO_PIN62_PAD_DRIVER    (BIT(2))
#define GPIO_PIN62_PAD_DRIVER_M  (GPIO_PIN62_PAD_DRIVER_V << GPIO_PIN62_PAD_DRIVER_S)
#define GPIO_PIN62_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN62_PAD_DRIVER_S  2
/** GPIO_PIN62_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.
 *  0: Not synchronize
 *  1: Synchronize on falling edge
 *  2: Synchronize on rising edge
 *  3: Synchronize on rising edge
 */
#define GPIO_PIN62_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN62_SYNC1_BYPASS_M  (GPIO_PIN62_SYNC1_BYPASS_V << GPIO_PIN62_SYNC1_BYPASS_S)
#define GPIO_PIN62_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN62_SYNC1_BYPASS_S  3
/** GPIO_PIN62_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.
 *  0: GPIO interrupt disabled
 *  1: Rising edge trigger
 *  2: Falling edge trigger
 *  3: Any edge trigger
 *  4: Low level trigger
 *  5: High level trigger
 */
#define GPIO_PIN62_INT_TYPE    0x00000007U
#define GPIO_PIN62_INT_TYPE_M  (GPIO_PIN62_INT_TYPE_V << GPIO_PIN62_INT_TYPE_S)
#define GPIO_PIN62_INT_TYPE_V  0x00000007U
#define GPIO_PIN62_INT_TYPE_S  7
/** GPIO_PIN62_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.
 *  0: Disable
 *  1: Enable
 *  This function only wakes up the CPU from Light-sleep.
 */
#define GPIO_PIN62_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN62_WAKEUP_ENABLE_M  (GPIO_PIN62_WAKEUP_ENABLE_V << GPIO_PIN62_WAKEUP_ENABLE_S)
#define GPIO_PIN62_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN62_WAKEUP_ENABLE_S  10
/** GPIO_PIN62_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
 *  gpio_int_3(need update in different project).
 *
 *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
 *  project):
 *  0: Disable
 *  1: Enable
 *  - bit17: invalid
 */
#define GPIO_PIN62_INT_ENA    0x0000001FU
#define GPIO_PIN62_INT_ENA_M  (GPIO_PIN62_INT_ENA_V << GPIO_PIN62_INT_ENA_S)
#define GPIO_PIN62_INT_ENA_V  0x0000001FU
#define GPIO_PIN62_INT_ENA_S  13

/** GPIO_FUNC0_IN_SEL_CFG_REG register
 *  Configuration register for input signal 0
 */
#define GPIO_FUNC0_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f4)
/** GPIO_FUNC0_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 0.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC0_IN_SEL    0x000000FFU
#define GPIO_FUNC0_IN_SEL_M  (GPIO_FUNC0_IN_SEL_V << GPIO_FUNC0_IN_SEL_S)
#define GPIO_FUNC0_IN_SEL_V  0x000000FFU
#define GPIO_FUNC0_IN_SEL_S  0
/** GPIO_FUNC0_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC0_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC0_IN_INV_SEL_M  (GPIO_FUNC0_IN_INV_SEL_V << GPIO_FUNC0_IN_INV_SEL_S)
#define GPIO_FUNC0_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_IN_INV_SEL_S  8
/** GPIO_SIG0_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG0_IN_SEL    (BIT(9))
#define GPIO_SIG0_IN_SEL_M  (GPIO_SIG0_IN_SEL_V << GPIO_SIG0_IN_SEL_S)
#define GPIO_SIG0_IN_SEL_V  0x00000001U
#define GPIO_SIG0_IN_SEL_S  9

/** GPIO_FUNC1_IN_SEL_CFG_REG register
 *  Configuration register for input signal 1
 */
#define GPIO_FUNC1_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f8)
/** GPIO_FUNC1_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 1.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC1_IN_SEL    0x000000FFU
#define GPIO_FUNC1_IN_SEL_M  (GPIO_FUNC1_IN_SEL_V << GPIO_FUNC1_IN_SEL_S)
#define GPIO_FUNC1_IN_SEL_V  0x000000FFU
#define GPIO_FUNC1_IN_SEL_S  0
/** GPIO_FUNC1_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC1_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC1_IN_INV_SEL_M  (GPIO_FUNC1_IN_INV_SEL_V << GPIO_FUNC1_IN_INV_SEL_S)
#define GPIO_FUNC1_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_IN_INV_SEL_S  8
/** GPIO_SIG1_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG1_IN_SEL    (BIT(9))
#define GPIO_SIG1_IN_SEL_M  (GPIO_SIG1_IN_SEL_V << GPIO_SIG1_IN_SEL_S)
#define GPIO_SIG1_IN_SEL_V  0x00000001U
#define GPIO_SIG1_IN_SEL_S  9

/** GPIO_FUNC2_IN_SEL_CFG_REG register
 *  Configuration register for input signal 2
 */
#define GPIO_FUNC2_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2fc)
/** GPIO_FUNC2_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 2.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC2_IN_SEL    0x000000FFU
#define GPIO_FUNC2_IN_SEL_M  (GPIO_FUNC2_IN_SEL_V << GPIO_FUNC2_IN_SEL_S)
#define GPIO_FUNC2_IN_SEL_V  0x000000FFU
#define GPIO_FUNC2_IN_SEL_S  0
/** GPIO_FUNC2_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC2_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC2_IN_INV_SEL_M  (GPIO_FUNC2_IN_INV_SEL_V << GPIO_FUNC2_IN_INV_SEL_S)
#define GPIO_FUNC2_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_IN_INV_SEL_S  8
/** GPIO_SIG2_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG2_IN_SEL    (BIT(9))
#define GPIO_SIG2_IN_SEL_M  (GPIO_SIG2_IN_SEL_V << GPIO_SIG2_IN_SEL_S)
#define GPIO_SIG2_IN_SEL_V  0x00000001U
#define GPIO_SIG2_IN_SEL_S  9

/** GPIO_FUNC3_IN_SEL_CFG_REG register
 *  Configuration register for input signal 3
 */
#define GPIO_FUNC3_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x300)
/** GPIO_FUNC3_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 3.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC3_IN_SEL    0x000000FFU
#define GPIO_FUNC3_IN_SEL_M  (GPIO_FUNC3_IN_SEL_V << GPIO_FUNC3_IN_SEL_S)
#define GPIO_FUNC3_IN_SEL_V  0x000000FFU
#define GPIO_FUNC3_IN_SEL_S  0
/** GPIO_FUNC3_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC3_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC3_IN_INV_SEL_M  (GPIO_FUNC3_IN_INV_SEL_V << GPIO_FUNC3_IN_INV_SEL_S)
#define GPIO_FUNC3_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_IN_INV_SEL_S  8
/** GPIO_SIG3_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG3_IN_SEL    (BIT(9))
#define GPIO_SIG3_IN_SEL_M  (GPIO_SIG3_IN_SEL_V << GPIO_SIG3_IN_SEL_S)
#define GPIO_SIG3_IN_SEL_V  0x00000001U
#define GPIO_SIG3_IN_SEL_S  9

/** GPIO_FUNC4_IN_SEL_CFG_REG register
 *  Configuration register for input signal 4
 */
#define GPIO_FUNC4_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x304)
/** GPIO_FUNC4_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 4.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC4_IN_SEL    0x000000FFU
#define GPIO_FUNC4_IN_SEL_M  (GPIO_FUNC4_IN_SEL_V << GPIO_FUNC4_IN_SEL_S)
#define GPIO_FUNC4_IN_SEL_V  0x000000FFU
#define GPIO_FUNC4_IN_SEL_S  0
/** GPIO_FUNC4_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC4_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC4_IN_INV_SEL_M  (GPIO_FUNC4_IN_INV_SEL_V << GPIO_FUNC4_IN_INV_SEL_S)
#define GPIO_FUNC4_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_IN_INV_SEL_S  8
/** GPIO_SIG4_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG4_IN_SEL    (BIT(9))
#define GPIO_SIG4_IN_SEL_M  (GPIO_SIG4_IN_SEL_V << GPIO_SIG4_IN_SEL_S)
#define GPIO_SIG4_IN_SEL_V  0x00000001U
#define GPIO_SIG4_IN_SEL_S  9

/** GPIO_FUNC5_IN_SEL_CFG_REG register
 *  Configuration register for input signal 5
 */
#define GPIO_FUNC5_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x308)
/** GPIO_FUNC5_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 5.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC5_IN_SEL    0x000000FFU
#define GPIO_FUNC5_IN_SEL_M  (GPIO_FUNC5_IN_SEL_V << GPIO_FUNC5_IN_SEL_S)
#define GPIO_FUNC5_IN_SEL_V  0x000000FFU
#define GPIO_FUNC5_IN_SEL_S  0
/** GPIO_FUNC5_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC5_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC5_IN_INV_SEL_M  (GPIO_FUNC5_IN_INV_SEL_V << GPIO_FUNC5_IN_INV_SEL_S)
#define GPIO_FUNC5_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_IN_INV_SEL_S  8
/** GPIO_SIG5_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG5_IN_SEL    (BIT(9))
#define GPIO_SIG5_IN_SEL_M  (GPIO_SIG5_IN_SEL_V << GPIO_SIG5_IN_SEL_S)
#define GPIO_SIG5_IN_SEL_V  0x00000001U
#define GPIO_SIG5_IN_SEL_S  9

/** GPIO_FUNC6_IN_SEL_CFG_REG register
 *  Configuration register for input signal 6
 */
#define GPIO_FUNC6_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x30c)
/** GPIO_FUNC6_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 6.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC6_IN_SEL    0x000000FFU
#define GPIO_FUNC6_IN_SEL_M  (GPIO_FUNC6_IN_SEL_V << GPIO_FUNC6_IN_SEL_S)
#define GPIO_FUNC6_IN_SEL_V  0x000000FFU
#define GPIO_FUNC6_IN_SEL_S  0
/** GPIO_FUNC6_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC6_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC6_IN_INV_SEL_M  (GPIO_FUNC6_IN_INV_SEL_V << GPIO_FUNC6_IN_INV_SEL_S)
#define GPIO_FUNC6_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_IN_INV_SEL_S  8
/** GPIO_SIG6_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG6_IN_SEL    (BIT(9))
#define GPIO_SIG6_IN_SEL_M  (GPIO_SIG6_IN_SEL_V << GPIO_SIG6_IN_SEL_S)
#define GPIO_SIG6_IN_SEL_V  0x00000001U
#define GPIO_SIG6_IN_SEL_S  9

/** GPIO_FUNC7_IN_SEL_CFG_REG register
 *  Configuration register for input signal 7
 */
#define GPIO_FUNC7_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x310)
/** GPIO_FUNC7_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 7.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC7_IN_SEL    0x000000FFU
#define GPIO_FUNC7_IN_SEL_M  (GPIO_FUNC7_IN_SEL_V << GPIO_FUNC7_IN_SEL_S)
#define GPIO_FUNC7_IN_SEL_V  0x000000FFU
#define GPIO_FUNC7_IN_SEL_S  0
/** GPIO_FUNC7_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC7_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC7_IN_INV_SEL_M  (GPIO_FUNC7_IN_INV_SEL_V << GPIO_FUNC7_IN_INV_SEL_S)
#define GPIO_FUNC7_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_IN_INV_SEL_S  8
/** GPIO_SIG7_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG7_IN_SEL    (BIT(9))
#define GPIO_SIG7_IN_SEL_M  (GPIO_SIG7_IN_SEL_V << GPIO_SIG7_IN_SEL_S)
#define GPIO_SIG7_IN_SEL_V  0x00000001U
#define GPIO_SIG7_IN_SEL_S  9

/** GPIO_FUNC10_IN_SEL_CFG_REG register
 *  Configuration register for input signal 10
 */
#define GPIO_FUNC10_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x31c)
/** GPIO_FUNC10_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 10.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC10_IN_SEL    0x000000FFU
#define GPIO_FUNC10_IN_SEL_M  (GPIO_FUNC10_IN_SEL_V << GPIO_FUNC10_IN_SEL_S)
#define GPIO_FUNC10_IN_SEL_V  0x000000FFU
#define GPIO_FUNC10_IN_SEL_S  0
/** GPIO_FUNC10_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC10_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC10_IN_INV_SEL_M  (GPIO_FUNC10_IN_INV_SEL_V << GPIO_FUNC10_IN_INV_SEL_S)
#define GPIO_FUNC10_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_IN_INV_SEL_S  8
/** GPIO_SIG10_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG10_IN_SEL    (BIT(9))
#define GPIO_SIG10_IN_SEL_M  (GPIO_SIG10_IN_SEL_V << GPIO_SIG10_IN_SEL_S)
#define GPIO_SIG10_IN_SEL_V  0x00000001U
#define GPIO_SIG10_IN_SEL_S  9

/** GPIO_FUNC11_IN_SEL_CFG_REG register
 *  Configuration register for input signal 11
 */
#define GPIO_FUNC11_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x320)
/** GPIO_FUNC11_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 11.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC11_IN_SEL    0x000000FFU
#define GPIO_FUNC11_IN_SEL_M  (GPIO_FUNC11_IN_SEL_V << GPIO_FUNC11_IN_SEL_S)
#define GPIO_FUNC11_IN_SEL_V  0x000000FFU
#define GPIO_FUNC11_IN_SEL_S  0
/** GPIO_FUNC11_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC11_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC11_IN_INV_SEL_M  (GPIO_FUNC11_IN_INV_SEL_V << GPIO_FUNC11_IN_INV_SEL_S)
#define GPIO_FUNC11_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_IN_INV_SEL_S  8
/** GPIO_SIG11_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG11_IN_SEL    (BIT(9))
#define GPIO_SIG11_IN_SEL_M  (GPIO_SIG11_IN_SEL_V << GPIO_SIG11_IN_SEL_S)
#define GPIO_SIG11_IN_SEL_V  0x00000001U
#define GPIO_SIG11_IN_SEL_S  9

/** GPIO_FUNC12_IN_SEL_CFG_REG register
 *  Configuration register for input signal 12
 */
#define GPIO_FUNC12_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x324)
/** GPIO_FUNC12_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 12.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC12_IN_SEL    0x000000FFU
#define GPIO_FUNC12_IN_SEL_M  (GPIO_FUNC12_IN_SEL_V << GPIO_FUNC12_IN_SEL_S)
#define GPIO_FUNC12_IN_SEL_V  0x000000FFU
#define GPIO_FUNC12_IN_SEL_S  0
/** GPIO_FUNC12_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC12_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC12_IN_INV_SEL_M  (GPIO_FUNC12_IN_INV_SEL_V << GPIO_FUNC12_IN_INV_SEL_S)
#define GPIO_FUNC12_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_IN_INV_SEL_S  8
/** GPIO_SIG12_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG12_IN_SEL    (BIT(9))
#define GPIO_SIG12_IN_SEL_M  (GPIO_SIG12_IN_SEL_V << GPIO_SIG12_IN_SEL_S)
#define GPIO_SIG12_IN_SEL_V  0x00000001U
#define GPIO_SIG12_IN_SEL_S  9

/** GPIO_FUNC13_IN_SEL_CFG_REG register
 *  Configuration register for input signal 13
 */
#define GPIO_FUNC13_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x328)
/** GPIO_FUNC13_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 13.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC13_IN_SEL    0x000000FFU
#define GPIO_FUNC13_IN_SEL_M  (GPIO_FUNC13_IN_SEL_V << GPIO_FUNC13_IN_SEL_S)
#define GPIO_FUNC13_IN_SEL_V  0x000000FFU
#define GPIO_FUNC13_IN_SEL_S  0
/** GPIO_FUNC13_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC13_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC13_IN_INV_SEL_M  (GPIO_FUNC13_IN_INV_SEL_V << GPIO_FUNC13_IN_INV_SEL_S)
#define GPIO_FUNC13_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_IN_INV_SEL_S  8
/** GPIO_SIG13_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG13_IN_SEL    (BIT(9))
#define GPIO_SIG13_IN_SEL_M  (GPIO_SIG13_IN_SEL_V << GPIO_SIG13_IN_SEL_S)
#define GPIO_SIG13_IN_SEL_V  0x00000001U
#define GPIO_SIG13_IN_SEL_S  9

/** GPIO_FUNC14_IN_SEL_CFG_REG register
 *  Configuration register for input signal 14
 */
#define GPIO_FUNC14_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x32c)
/** GPIO_FUNC14_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 14.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC14_IN_SEL    0x000000FFU
#define GPIO_FUNC14_IN_SEL_M  (GPIO_FUNC14_IN_SEL_V << GPIO_FUNC14_IN_SEL_S)
#define GPIO_FUNC14_IN_SEL_V  0x000000FFU
#define GPIO_FUNC14_IN_SEL_S  0
/** GPIO_FUNC14_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC14_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC14_IN_INV_SEL_M  (GPIO_FUNC14_IN_INV_SEL_V << GPIO_FUNC14_IN_INV_SEL_S)
#define GPIO_FUNC14_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_IN_INV_SEL_S  8
/** GPIO_SIG14_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG14_IN_SEL    (BIT(9))
#define GPIO_SIG14_IN_SEL_M  (GPIO_SIG14_IN_SEL_V << GPIO_SIG14_IN_SEL_S)
#define GPIO_SIG14_IN_SEL_V  0x00000001U
#define GPIO_SIG14_IN_SEL_S  9

/** GPIO_FUNC15_IN_SEL_CFG_REG register
 *  Configuration register for input signal 15
 */
#define GPIO_FUNC15_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x330)
/** GPIO_FUNC15_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 15.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC15_IN_SEL    0x000000FFU
#define GPIO_FUNC15_IN_SEL_M  (GPIO_FUNC15_IN_SEL_V << GPIO_FUNC15_IN_SEL_S)
#define GPIO_FUNC15_IN_SEL_V  0x000000FFU
#define GPIO_FUNC15_IN_SEL_S  0
/** GPIO_FUNC15_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC15_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC15_IN_INV_SEL_M  (GPIO_FUNC15_IN_INV_SEL_V << GPIO_FUNC15_IN_INV_SEL_S)
#define GPIO_FUNC15_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_IN_INV_SEL_S  8
/** GPIO_SIG15_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG15_IN_SEL    (BIT(9))
#define GPIO_SIG15_IN_SEL_M  (GPIO_SIG15_IN_SEL_V << GPIO_SIG15_IN_SEL_S)
#define GPIO_SIG15_IN_SEL_V  0x00000001U
#define GPIO_SIG15_IN_SEL_S  9

/** GPIO_FUNC16_IN_SEL_CFG_REG register
 *  Configuration register for input signal 16
 */
#define GPIO_FUNC16_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x334)
/** GPIO_FUNC16_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 16.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC16_IN_SEL    0x000000FFU
#define GPIO_FUNC16_IN_SEL_M  (GPIO_FUNC16_IN_SEL_V << GPIO_FUNC16_IN_SEL_S)
#define GPIO_FUNC16_IN_SEL_V  0x000000FFU
#define GPIO_FUNC16_IN_SEL_S  0
/** GPIO_FUNC16_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC16_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC16_IN_INV_SEL_M  (GPIO_FUNC16_IN_INV_SEL_V << GPIO_FUNC16_IN_INV_SEL_S)
#define GPIO_FUNC16_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_IN_INV_SEL_S  8
/** GPIO_SIG16_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG16_IN_SEL    (BIT(9))
#define GPIO_SIG16_IN_SEL_M  (GPIO_SIG16_IN_SEL_V << GPIO_SIG16_IN_SEL_S)
#define GPIO_SIG16_IN_SEL_V  0x00000001U
#define GPIO_SIG16_IN_SEL_S  9

/** GPIO_FUNC17_IN_SEL_CFG_REG register
 *  Configuration register for input signal 17
 */
#define GPIO_FUNC17_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x338)
/** GPIO_FUNC17_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 17.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC17_IN_SEL    0x000000FFU
#define GPIO_FUNC17_IN_SEL_M  (GPIO_FUNC17_IN_SEL_V << GPIO_FUNC17_IN_SEL_S)
#define GPIO_FUNC17_IN_SEL_V  0x000000FFU
#define GPIO_FUNC17_IN_SEL_S  0
/** GPIO_FUNC17_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC17_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC17_IN_INV_SEL_M  (GPIO_FUNC17_IN_INV_SEL_V << GPIO_FUNC17_IN_INV_SEL_S)
#define GPIO_FUNC17_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_IN_INV_SEL_S  8
/** GPIO_SIG17_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG17_IN_SEL    (BIT(9))
#define GPIO_SIG17_IN_SEL_M  (GPIO_SIG17_IN_SEL_V << GPIO_SIG17_IN_SEL_S)
#define GPIO_SIG17_IN_SEL_V  0x00000001U
#define GPIO_SIG17_IN_SEL_S  9

/** GPIO_FUNC18_IN_SEL_CFG_REG register
 *  Configuration register for input signal 18
 */
#define GPIO_FUNC18_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x33c)
/** GPIO_FUNC18_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 18.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC18_IN_SEL    0x000000FFU
#define GPIO_FUNC18_IN_SEL_M  (GPIO_FUNC18_IN_SEL_V << GPIO_FUNC18_IN_SEL_S)
#define GPIO_FUNC18_IN_SEL_V  0x000000FFU
#define GPIO_FUNC18_IN_SEL_S  0
/** GPIO_FUNC18_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC18_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC18_IN_INV_SEL_M  (GPIO_FUNC18_IN_INV_SEL_V << GPIO_FUNC18_IN_INV_SEL_S)
#define GPIO_FUNC18_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_IN_INV_SEL_S  8
/** GPIO_SIG18_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG18_IN_SEL    (BIT(9))
#define GPIO_SIG18_IN_SEL_M  (GPIO_SIG18_IN_SEL_V << GPIO_SIG18_IN_SEL_S)
#define GPIO_SIG18_IN_SEL_V  0x00000001U
#define GPIO_SIG18_IN_SEL_S  9

/** GPIO_FUNC19_IN_SEL_CFG_REG register
 *  Configuration register for input signal 19
 */
#define GPIO_FUNC19_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x340)
/** GPIO_FUNC19_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 19.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC19_IN_SEL    0x000000FFU
#define GPIO_FUNC19_IN_SEL_M  (GPIO_FUNC19_IN_SEL_V << GPIO_FUNC19_IN_SEL_S)
#define GPIO_FUNC19_IN_SEL_V  0x000000FFU
#define GPIO_FUNC19_IN_SEL_S  0
/** GPIO_FUNC19_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC19_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC19_IN_INV_SEL_M  (GPIO_FUNC19_IN_INV_SEL_V << GPIO_FUNC19_IN_INV_SEL_S)
#define GPIO_FUNC19_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_IN_INV_SEL_S  8
/** GPIO_SIG19_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG19_IN_SEL    (BIT(9))
#define GPIO_SIG19_IN_SEL_M  (GPIO_SIG19_IN_SEL_V << GPIO_SIG19_IN_SEL_S)
#define GPIO_SIG19_IN_SEL_V  0x00000001U
#define GPIO_SIG19_IN_SEL_S  9

/** GPIO_FUNC20_IN_SEL_CFG_REG register
 *  Configuration register for input signal 20
 */
#define GPIO_FUNC20_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x344)
/** GPIO_FUNC20_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 20.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC20_IN_SEL    0x000000FFU
#define GPIO_FUNC20_IN_SEL_M  (GPIO_FUNC20_IN_SEL_V << GPIO_FUNC20_IN_SEL_S)
#define GPIO_FUNC20_IN_SEL_V  0x000000FFU
#define GPIO_FUNC20_IN_SEL_S  0
/** GPIO_FUNC20_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC20_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC20_IN_INV_SEL_M  (GPIO_FUNC20_IN_INV_SEL_V << GPIO_FUNC20_IN_INV_SEL_S)
#define GPIO_FUNC20_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_IN_INV_SEL_S  8
/** GPIO_SIG20_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG20_IN_SEL    (BIT(9))
#define GPIO_SIG20_IN_SEL_M  (GPIO_SIG20_IN_SEL_V << GPIO_SIG20_IN_SEL_S)
#define GPIO_SIG20_IN_SEL_V  0x00000001U
#define GPIO_SIG20_IN_SEL_S  9

/** GPIO_FUNC21_IN_SEL_CFG_REG register
 *  Configuration register for input signal 21
 */
#define GPIO_FUNC21_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x348)
/** GPIO_FUNC21_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 21.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC21_IN_SEL    0x000000FFU
#define GPIO_FUNC21_IN_SEL_M  (GPIO_FUNC21_IN_SEL_V << GPIO_FUNC21_IN_SEL_S)
#define GPIO_FUNC21_IN_SEL_V  0x000000FFU
#define GPIO_FUNC21_IN_SEL_S  0
/** GPIO_FUNC21_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC21_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC21_IN_INV_SEL_M  (GPIO_FUNC21_IN_INV_SEL_V << GPIO_FUNC21_IN_INV_SEL_S)
#define GPIO_FUNC21_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_IN_INV_SEL_S  8
/** GPIO_SIG21_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG21_IN_SEL    (BIT(9))
#define GPIO_SIG21_IN_SEL_M  (GPIO_SIG21_IN_SEL_V << GPIO_SIG21_IN_SEL_S)
#define GPIO_SIG21_IN_SEL_V  0x00000001U
#define GPIO_SIG21_IN_SEL_S  9

/** GPIO_FUNC22_IN_SEL_CFG_REG register
 *  Configuration register for input signal 22
 */
#define GPIO_FUNC22_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x34c)
/** GPIO_FUNC22_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 22.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC22_IN_SEL    0x000000FFU
#define GPIO_FUNC22_IN_SEL_M  (GPIO_FUNC22_IN_SEL_V << GPIO_FUNC22_IN_SEL_S)
#define GPIO_FUNC22_IN_SEL_V  0x000000FFU
#define GPIO_FUNC22_IN_SEL_S  0
/** GPIO_FUNC22_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC22_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC22_IN_INV_SEL_M  (GPIO_FUNC22_IN_INV_SEL_V << GPIO_FUNC22_IN_INV_SEL_S)
#define GPIO_FUNC22_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_IN_INV_SEL_S  8
/** GPIO_SIG22_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG22_IN_SEL    (BIT(9))
#define GPIO_SIG22_IN_SEL_M  (GPIO_SIG22_IN_SEL_V << GPIO_SIG22_IN_SEL_S)
#define GPIO_SIG22_IN_SEL_V  0x00000001U
#define GPIO_SIG22_IN_SEL_S  9

/** GPIO_FUNC23_IN_SEL_CFG_REG register
 *  Configuration register for input signal 23
 */
#define GPIO_FUNC23_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x350)
/** GPIO_FUNC23_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 23.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC23_IN_SEL    0x000000FFU
#define GPIO_FUNC23_IN_SEL_M  (GPIO_FUNC23_IN_SEL_V << GPIO_FUNC23_IN_SEL_S)
#define GPIO_FUNC23_IN_SEL_V  0x000000FFU
#define GPIO_FUNC23_IN_SEL_S  0
/** GPIO_FUNC23_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC23_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC23_IN_INV_SEL_M  (GPIO_FUNC23_IN_INV_SEL_V << GPIO_FUNC23_IN_INV_SEL_S)
#define GPIO_FUNC23_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_IN_INV_SEL_S  8
/** GPIO_SIG23_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG23_IN_SEL    (BIT(9))
#define GPIO_SIG23_IN_SEL_M  (GPIO_SIG23_IN_SEL_V << GPIO_SIG23_IN_SEL_S)
#define GPIO_SIG23_IN_SEL_V  0x00000001U
#define GPIO_SIG23_IN_SEL_S  9

/** GPIO_FUNC24_IN_SEL_CFG_REG register
 *  Configuration register for input signal 24
 */
#define GPIO_FUNC24_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x354)
/** GPIO_FUNC24_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 24.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC24_IN_SEL    0x000000FFU
#define GPIO_FUNC24_IN_SEL_M  (GPIO_FUNC24_IN_SEL_V << GPIO_FUNC24_IN_SEL_S)
#define GPIO_FUNC24_IN_SEL_V  0x000000FFU
#define GPIO_FUNC24_IN_SEL_S  0
/** GPIO_FUNC24_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC24_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC24_IN_INV_SEL_M  (GPIO_FUNC24_IN_INV_SEL_V << GPIO_FUNC24_IN_INV_SEL_S)
#define GPIO_FUNC24_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_IN_INV_SEL_S  8
/** GPIO_SIG24_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG24_IN_SEL    (BIT(9))
#define GPIO_SIG24_IN_SEL_M  (GPIO_SIG24_IN_SEL_V << GPIO_SIG24_IN_SEL_S)
#define GPIO_SIG24_IN_SEL_V  0x00000001U
#define GPIO_SIG24_IN_SEL_S  9

/** GPIO_FUNC25_IN_SEL_CFG_REG register
 *  Configuration register for input signal 25
 */
#define GPIO_FUNC25_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x358)
/** GPIO_FUNC25_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 25.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC25_IN_SEL    0x000000FFU
#define GPIO_FUNC25_IN_SEL_M  (GPIO_FUNC25_IN_SEL_V << GPIO_FUNC25_IN_SEL_S)
#define GPIO_FUNC25_IN_SEL_V  0x000000FFU
#define GPIO_FUNC25_IN_SEL_S  0
/** GPIO_FUNC25_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC25_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC25_IN_INV_SEL_M  (GPIO_FUNC25_IN_INV_SEL_V << GPIO_FUNC25_IN_INV_SEL_S)
#define GPIO_FUNC25_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_IN_INV_SEL_S  8
/** GPIO_SIG25_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG25_IN_SEL    (BIT(9))
#define GPIO_SIG25_IN_SEL_M  (GPIO_SIG25_IN_SEL_V << GPIO_SIG25_IN_SEL_S)
#define GPIO_SIG25_IN_SEL_V  0x00000001U
#define GPIO_SIG25_IN_SEL_S  9

/** GPIO_FUNC26_IN_SEL_CFG_REG register
 *  Configuration register for input signal 26
 */
#define GPIO_FUNC26_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x35c)
/** GPIO_FUNC26_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 26.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC26_IN_SEL    0x000000FFU
#define GPIO_FUNC26_IN_SEL_M  (GPIO_FUNC26_IN_SEL_V << GPIO_FUNC26_IN_SEL_S)
#define GPIO_FUNC26_IN_SEL_V  0x000000FFU
#define GPIO_FUNC26_IN_SEL_S  0
/** GPIO_FUNC26_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC26_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC26_IN_INV_SEL_M  (GPIO_FUNC26_IN_INV_SEL_V << GPIO_FUNC26_IN_INV_SEL_S)
#define GPIO_FUNC26_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_IN_INV_SEL_S  8
/** GPIO_SIG26_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG26_IN_SEL    (BIT(9))
#define GPIO_SIG26_IN_SEL_M  (GPIO_SIG26_IN_SEL_V << GPIO_SIG26_IN_SEL_S)
#define GPIO_SIG26_IN_SEL_V  0x00000001U
#define GPIO_SIG26_IN_SEL_S  9

/** GPIO_FUNC27_IN_SEL_CFG_REG register
 *  Configuration register for input signal 27
 */
#define GPIO_FUNC27_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x360)
/** GPIO_FUNC27_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 27.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC27_IN_SEL    0x000000FFU
#define GPIO_FUNC27_IN_SEL_M  (GPIO_FUNC27_IN_SEL_V << GPIO_FUNC27_IN_SEL_S)
#define GPIO_FUNC27_IN_SEL_V  0x000000FFU
#define GPIO_FUNC27_IN_SEL_S  0
/** GPIO_FUNC27_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC27_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC27_IN_INV_SEL_M  (GPIO_FUNC27_IN_INV_SEL_V << GPIO_FUNC27_IN_INV_SEL_S)
#define GPIO_FUNC27_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_IN_INV_SEL_S  8
/** GPIO_SIG27_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG27_IN_SEL    (BIT(9))
#define GPIO_SIG27_IN_SEL_M  (GPIO_SIG27_IN_SEL_V << GPIO_SIG27_IN_SEL_S)
#define GPIO_SIG27_IN_SEL_V  0x00000001U
#define GPIO_SIG27_IN_SEL_S  9

/** GPIO_FUNC28_IN_SEL_CFG_REG register
 *  Configuration register for input signal 28
 */
#define GPIO_FUNC28_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x364)
/** GPIO_FUNC28_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 28.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC28_IN_SEL    0x000000FFU
#define GPIO_FUNC28_IN_SEL_M  (GPIO_FUNC28_IN_SEL_V << GPIO_FUNC28_IN_SEL_S)
#define GPIO_FUNC28_IN_SEL_V  0x000000FFU
#define GPIO_FUNC28_IN_SEL_S  0
/** GPIO_FUNC28_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC28_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC28_IN_INV_SEL_M  (GPIO_FUNC28_IN_INV_SEL_V << GPIO_FUNC28_IN_INV_SEL_S)
#define GPIO_FUNC28_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_IN_INV_SEL_S  8
/** GPIO_SIG28_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG28_IN_SEL    (BIT(9))
#define GPIO_SIG28_IN_SEL_M  (GPIO_SIG28_IN_SEL_V << GPIO_SIG28_IN_SEL_S)
#define GPIO_SIG28_IN_SEL_V  0x00000001U
#define GPIO_SIG28_IN_SEL_S  9

/** GPIO_FUNC29_IN_SEL_CFG_REG register
 *  Configuration register for input signal 29
 */
#define GPIO_FUNC29_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x368)
/** GPIO_FUNC29_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 29.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC29_IN_SEL    0x000000FFU
#define GPIO_FUNC29_IN_SEL_M  (GPIO_FUNC29_IN_SEL_V << GPIO_FUNC29_IN_SEL_S)
#define GPIO_FUNC29_IN_SEL_V  0x000000FFU
#define GPIO_FUNC29_IN_SEL_S  0
/** GPIO_FUNC29_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC29_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC29_IN_INV_SEL_M  (GPIO_FUNC29_IN_INV_SEL_V << GPIO_FUNC29_IN_INV_SEL_S)
#define GPIO_FUNC29_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_IN_INV_SEL_S  8
/** GPIO_SIG29_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG29_IN_SEL    (BIT(9))
#define GPIO_SIG29_IN_SEL_M  (GPIO_SIG29_IN_SEL_V << GPIO_SIG29_IN_SEL_S)
#define GPIO_SIG29_IN_SEL_V  0x00000001U
#define GPIO_SIG29_IN_SEL_S  9

/** GPIO_FUNC30_IN_SEL_CFG_REG register
 *  Configuration register for input signal 30
 */
#define GPIO_FUNC30_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x36c)
/** GPIO_FUNC30_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 30.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC30_IN_SEL    0x000000FFU
#define GPIO_FUNC30_IN_SEL_M  (GPIO_FUNC30_IN_SEL_V << GPIO_FUNC30_IN_SEL_S)
#define GPIO_FUNC30_IN_SEL_V  0x000000FFU
#define GPIO_FUNC30_IN_SEL_S  0
/** GPIO_FUNC30_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC30_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC30_IN_INV_SEL_M  (GPIO_FUNC30_IN_INV_SEL_V << GPIO_FUNC30_IN_INV_SEL_S)
#define GPIO_FUNC30_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_IN_INV_SEL_S  8
/** GPIO_SIG30_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG30_IN_SEL    (BIT(9))
#define GPIO_SIG30_IN_SEL_M  (GPIO_SIG30_IN_SEL_V << GPIO_SIG30_IN_SEL_S)
#define GPIO_SIG30_IN_SEL_V  0x00000001U
#define GPIO_SIG30_IN_SEL_S  9

/** GPIO_FUNC31_IN_SEL_CFG_REG register
 *  Configuration register for input signal 31
 */
#define GPIO_FUNC31_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x370)
/** GPIO_FUNC31_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 31.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC31_IN_SEL    0x000000FFU
#define GPIO_FUNC31_IN_SEL_M  (GPIO_FUNC31_IN_SEL_V << GPIO_FUNC31_IN_SEL_S)
#define GPIO_FUNC31_IN_SEL_V  0x000000FFU
#define GPIO_FUNC31_IN_SEL_S  0
/** GPIO_FUNC31_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC31_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC31_IN_INV_SEL_M  (GPIO_FUNC31_IN_INV_SEL_V << GPIO_FUNC31_IN_INV_SEL_S)
#define GPIO_FUNC31_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_IN_INV_SEL_S  8
/** GPIO_SIG31_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG31_IN_SEL    (BIT(9))
#define GPIO_SIG31_IN_SEL_M  (GPIO_SIG31_IN_SEL_V << GPIO_SIG31_IN_SEL_S)
#define GPIO_SIG31_IN_SEL_V  0x00000001U
#define GPIO_SIG31_IN_SEL_S  9

/** GPIO_FUNC32_IN_SEL_CFG_REG register
 *  Configuration register for input signal 32
 */
#define GPIO_FUNC32_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x374)
/** GPIO_FUNC32_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 32.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC32_IN_SEL    0x000000FFU
#define GPIO_FUNC32_IN_SEL_M  (GPIO_FUNC32_IN_SEL_V << GPIO_FUNC32_IN_SEL_S)
#define GPIO_FUNC32_IN_SEL_V  0x000000FFU
#define GPIO_FUNC32_IN_SEL_S  0
/** GPIO_FUNC32_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC32_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC32_IN_INV_SEL_M  (GPIO_FUNC32_IN_INV_SEL_V << GPIO_FUNC32_IN_INV_SEL_S)
#define GPIO_FUNC32_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_IN_INV_SEL_S  8
/** GPIO_SIG32_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG32_IN_SEL    (BIT(9))
#define GPIO_SIG32_IN_SEL_M  (GPIO_SIG32_IN_SEL_V << GPIO_SIG32_IN_SEL_S)
#define GPIO_SIG32_IN_SEL_V  0x00000001U
#define GPIO_SIG32_IN_SEL_S  9

/** GPIO_FUNC33_IN_SEL_CFG_REG register
 *  Configuration register for input signal 33
 */
#define GPIO_FUNC33_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x378)
/** GPIO_FUNC33_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 33.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC33_IN_SEL    0x000000FFU
#define GPIO_FUNC33_IN_SEL_M  (GPIO_FUNC33_IN_SEL_V << GPIO_FUNC33_IN_SEL_S)
#define GPIO_FUNC33_IN_SEL_V  0x000000FFU
#define GPIO_FUNC33_IN_SEL_S  0
/** GPIO_FUNC33_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC33_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC33_IN_INV_SEL_M  (GPIO_FUNC33_IN_INV_SEL_V << GPIO_FUNC33_IN_INV_SEL_S)
#define GPIO_FUNC33_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_IN_INV_SEL_S  8
/** GPIO_SIG33_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG33_IN_SEL    (BIT(9))
#define GPIO_SIG33_IN_SEL_M  (GPIO_SIG33_IN_SEL_V << GPIO_SIG33_IN_SEL_S)
#define GPIO_SIG33_IN_SEL_V  0x00000001U
#define GPIO_SIG33_IN_SEL_S  9

/** GPIO_FUNC34_IN_SEL_CFG_REG register
 *  Configuration register for input signal 34
 */
#define GPIO_FUNC34_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x37c)
/** GPIO_FUNC34_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 34.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC34_IN_SEL    0x000000FFU
#define GPIO_FUNC34_IN_SEL_M  (GPIO_FUNC34_IN_SEL_V << GPIO_FUNC34_IN_SEL_S)
#define GPIO_FUNC34_IN_SEL_V  0x000000FFU
#define GPIO_FUNC34_IN_SEL_S  0
/** GPIO_FUNC34_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC34_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC34_IN_INV_SEL_M  (GPIO_FUNC34_IN_INV_SEL_V << GPIO_FUNC34_IN_INV_SEL_S)
#define GPIO_FUNC34_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_IN_INV_SEL_S  8
/** GPIO_SIG34_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG34_IN_SEL    (BIT(9))
#define GPIO_SIG34_IN_SEL_M  (GPIO_SIG34_IN_SEL_V << GPIO_SIG34_IN_SEL_S)
#define GPIO_SIG34_IN_SEL_V  0x00000001U
#define GPIO_SIG34_IN_SEL_S  9

/** GPIO_FUNC35_IN_SEL_CFG_REG register
 *  Configuration register for input signal 35
 */
#define GPIO_FUNC35_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x380)
/** GPIO_FUNC35_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 35.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC35_IN_SEL    0x000000FFU
#define GPIO_FUNC35_IN_SEL_M  (GPIO_FUNC35_IN_SEL_V << GPIO_FUNC35_IN_SEL_S)
#define GPIO_FUNC35_IN_SEL_V  0x000000FFU
#define GPIO_FUNC35_IN_SEL_S  0
/** GPIO_FUNC35_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC35_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC35_IN_INV_SEL_M  (GPIO_FUNC35_IN_INV_SEL_V << GPIO_FUNC35_IN_INV_SEL_S)
#define GPIO_FUNC35_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_IN_INV_SEL_S  8
/** GPIO_SIG35_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG35_IN_SEL    (BIT(9))
#define GPIO_SIG35_IN_SEL_M  (GPIO_SIG35_IN_SEL_V << GPIO_SIG35_IN_SEL_S)
#define GPIO_SIG35_IN_SEL_V  0x00000001U
#define GPIO_SIG35_IN_SEL_S  9

/** GPIO_FUNC36_IN_SEL_CFG_REG register
 *  Configuration register for input signal 36
 */
#define GPIO_FUNC36_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x384)
/** GPIO_FUNC36_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 36.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC36_IN_SEL    0x000000FFU
#define GPIO_FUNC36_IN_SEL_M  (GPIO_FUNC36_IN_SEL_V << GPIO_FUNC36_IN_SEL_S)
#define GPIO_FUNC36_IN_SEL_V  0x000000FFU
#define GPIO_FUNC36_IN_SEL_S  0
/** GPIO_FUNC36_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC36_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC36_IN_INV_SEL_M  (GPIO_FUNC36_IN_INV_SEL_V << GPIO_FUNC36_IN_INV_SEL_S)
#define GPIO_FUNC36_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC36_IN_INV_SEL_S  8
/** GPIO_SIG36_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG36_IN_SEL    (BIT(9))
#define GPIO_SIG36_IN_SEL_M  (GPIO_SIG36_IN_SEL_V << GPIO_SIG36_IN_SEL_S)
#define GPIO_SIG36_IN_SEL_V  0x00000001U
#define GPIO_SIG36_IN_SEL_S  9

/** GPIO_FUNC37_IN_SEL_CFG_REG register
 *  Configuration register for input signal 37
 */
#define GPIO_FUNC37_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x388)
/** GPIO_FUNC37_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 37.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC37_IN_SEL    0x000000FFU
#define GPIO_FUNC37_IN_SEL_M  (GPIO_FUNC37_IN_SEL_V << GPIO_FUNC37_IN_SEL_S)
#define GPIO_FUNC37_IN_SEL_V  0x000000FFU
#define GPIO_FUNC37_IN_SEL_S  0
/** GPIO_FUNC37_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC37_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC37_IN_INV_SEL_M  (GPIO_FUNC37_IN_INV_SEL_V << GPIO_FUNC37_IN_INV_SEL_S)
#define GPIO_FUNC37_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC37_IN_INV_SEL_S  8
/** GPIO_SIG37_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG37_IN_SEL    (BIT(9))
#define GPIO_SIG37_IN_SEL_M  (GPIO_SIG37_IN_SEL_V << GPIO_SIG37_IN_SEL_S)
#define GPIO_SIG37_IN_SEL_V  0x00000001U
#define GPIO_SIG37_IN_SEL_S  9

/** GPIO_FUNC38_IN_SEL_CFG_REG register
 *  Configuration register for input signal 38
 */
#define GPIO_FUNC38_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x38c)
/** GPIO_FUNC38_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 38.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC38_IN_SEL    0x000000FFU
#define GPIO_FUNC38_IN_SEL_M  (GPIO_FUNC38_IN_SEL_V << GPIO_FUNC38_IN_SEL_S)
#define GPIO_FUNC38_IN_SEL_V  0x000000FFU
#define GPIO_FUNC38_IN_SEL_S  0
/** GPIO_FUNC38_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC38_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC38_IN_INV_SEL_M  (GPIO_FUNC38_IN_INV_SEL_V << GPIO_FUNC38_IN_INV_SEL_S)
#define GPIO_FUNC38_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC38_IN_INV_SEL_S  8
/** GPIO_SIG38_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG38_IN_SEL    (BIT(9))
#define GPIO_SIG38_IN_SEL_M  (GPIO_SIG38_IN_SEL_V << GPIO_SIG38_IN_SEL_S)
#define GPIO_SIG38_IN_SEL_V  0x00000001U
#define GPIO_SIG38_IN_SEL_S  9

/** GPIO_FUNC39_IN_SEL_CFG_REG register
 *  Configuration register for input signal 39
 */
#define GPIO_FUNC39_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x390)
/** GPIO_FUNC39_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 39.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC39_IN_SEL    0x000000FFU
#define GPIO_FUNC39_IN_SEL_M  (GPIO_FUNC39_IN_SEL_V << GPIO_FUNC39_IN_SEL_S)
#define GPIO_FUNC39_IN_SEL_V  0x000000FFU
#define GPIO_FUNC39_IN_SEL_S  0
/** GPIO_FUNC39_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC39_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC39_IN_INV_SEL_M  (GPIO_FUNC39_IN_INV_SEL_V << GPIO_FUNC39_IN_INV_SEL_S)
#define GPIO_FUNC39_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC39_IN_INV_SEL_S  8
/** GPIO_SIG39_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG39_IN_SEL    (BIT(9))
#define GPIO_SIG39_IN_SEL_M  (GPIO_SIG39_IN_SEL_V << GPIO_SIG39_IN_SEL_S)
#define GPIO_SIG39_IN_SEL_V  0x00000001U
#define GPIO_SIG39_IN_SEL_S  9

/** GPIO_FUNC40_IN_SEL_CFG_REG register
 *  Configuration register for input signal 40
 */
#define GPIO_FUNC40_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x394)
/** GPIO_FUNC40_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 40.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC40_IN_SEL    0x000000FFU
#define GPIO_FUNC40_IN_SEL_M  (GPIO_FUNC40_IN_SEL_V << GPIO_FUNC40_IN_SEL_S)
#define GPIO_FUNC40_IN_SEL_V  0x000000FFU
#define GPIO_FUNC40_IN_SEL_S  0
/** GPIO_FUNC40_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC40_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC40_IN_INV_SEL_M  (GPIO_FUNC40_IN_INV_SEL_V << GPIO_FUNC40_IN_INV_SEL_S)
#define GPIO_FUNC40_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC40_IN_INV_SEL_S  8
/** GPIO_SIG40_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG40_IN_SEL    (BIT(9))
#define GPIO_SIG40_IN_SEL_M  (GPIO_SIG40_IN_SEL_V << GPIO_SIG40_IN_SEL_S)
#define GPIO_SIG40_IN_SEL_V  0x00000001U
#define GPIO_SIG40_IN_SEL_S  9

/** GPIO_FUNC43_IN_SEL_CFG_REG register
 *  Configuration register for input signal 43
 */
#define GPIO_FUNC43_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a0)
/** GPIO_FUNC43_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 43.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC43_IN_SEL    0x000000FFU
#define GPIO_FUNC43_IN_SEL_M  (GPIO_FUNC43_IN_SEL_V << GPIO_FUNC43_IN_SEL_S)
#define GPIO_FUNC43_IN_SEL_V  0x000000FFU
#define GPIO_FUNC43_IN_SEL_S  0
/** GPIO_FUNC43_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC43_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC43_IN_INV_SEL_M  (GPIO_FUNC43_IN_INV_SEL_V << GPIO_FUNC43_IN_INV_SEL_S)
#define GPIO_FUNC43_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_IN_INV_SEL_S  8
/** GPIO_SIG43_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG43_IN_SEL    (BIT(9))
#define GPIO_SIG43_IN_SEL_M  (GPIO_SIG43_IN_SEL_V << GPIO_SIG43_IN_SEL_S)
#define GPIO_SIG43_IN_SEL_V  0x00000001U
#define GPIO_SIG43_IN_SEL_S  9

/** GPIO_FUNC44_IN_SEL_CFG_REG register
 *  Configuration register for input signal 44
 */
#define GPIO_FUNC44_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a4)
/** GPIO_FUNC44_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 44.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC44_IN_SEL    0x000000FFU
#define GPIO_FUNC44_IN_SEL_M  (GPIO_FUNC44_IN_SEL_V << GPIO_FUNC44_IN_SEL_S)
#define GPIO_FUNC44_IN_SEL_V  0x000000FFU
#define GPIO_FUNC44_IN_SEL_S  0
/** GPIO_FUNC44_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC44_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC44_IN_INV_SEL_M  (GPIO_FUNC44_IN_INV_SEL_V << GPIO_FUNC44_IN_INV_SEL_S)
#define GPIO_FUNC44_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC44_IN_INV_SEL_S  8
/** GPIO_SIG44_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG44_IN_SEL    (BIT(9))
#define GPIO_SIG44_IN_SEL_M  (GPIO_SIG44_IN_SEL_V << GPIO_SIG44_IN_SEL_S)
#define GPIO_SIG44_IN_SEL_V  0x00000001U
#define GPIO_SIG44_IN_SEL_S  9

/** GPIO_FUNC45_IN_SEL_CFG_REG register
 *  Configuration register for input signal 45
 */
#define GPIO_FUNC45_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a8)
/** GPIO_FUNC45_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 45.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC45_IN_SEL    0x000000FFU
#define GPIO_FUNC45_IN_SEL_M  (GPIO_FUNC45_IN_SEL_V << GPIO_FUNC45_IN_SEL_S)
#define GPIO_FUNC45_IN_SEL_V  0x000000FFU
#define GPIO_FUNC45_IN_SEL_S  0
/** GPIO_FUNC45_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC45_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC45_IN_INV_SEL_M  (GPIO_FUNC45_IN_INV_SEL_V << GPIO_FUNC45_IN_INV_SEL_S)
#define GPIO_FUNC45_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC45_IN_INV_SEL_S  8
/** GPIO_SIG45_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG45_IN_SEL    (BIT(9))
#define GPIO_SIG45_IN_SEL_M  (GPIO_SIG45_IN_SEL_V << GPIO_SIG45_IN_SEL_S)
#define GPIO_SIG45_IN_SEL_V  0x00000001U
#define GPIO_SIG45_IN_SEL_S  9

/** GPIO_FUNC47_IN_SEL_CFG_REG register
 *  Configuration register for input signal 47
 */
#define GPIO_FUNC47_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b0)
/** GPIO_FUNC47_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 47.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC47_IN_SEL    0x000000FFU
#define GPIO_FUNC47_IN_SEL_M  (GPIO_FUNC47_IN_SEL_V << GPIO_FUNC47_IN_SEL_S)
#define GPIO_FUNC47_IN_SEL_V  0x000000FFU
#define GPIO_FUNC47_IN_SEL_S  0
/** GPIO_FUNC47_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC47_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC47_IN_INV_SEL_M  (GPIO_FUNC47_IN_INV_SEL_V << GPIO_FUNC47_IN_INV_SEL_S)
#define GPIO_FUNC47_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_IN_INV_SEL_S  8
/** GPIO_SIG47_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG47_IN_SEL    (BIT(9))
#define GPIO_SIG47_IN_SEL_M  (GPIO_SIG47_IN_SEL_V << GPIO_SIG47_IN_SEL_S)
#define GPIO_SIG47_IN_SEL_V  0x00000001U
#define GPIO_SIG47_IN_SEL_S  9

/** GPIO_FUNC48_IN_SEL_CFG_REG register
 *  Configuration register for input signal 48
 */
#define GPIO_FUNC48_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b4)
/** GPIO_FUNC48_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 48.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC48_IN_SEL    0x000000FFU
#define GPIO_FUNC48_IN_SEL_M  (GPIO_FUNC48_IN_SEL_V << GPIO_FUNC48_IN_SEL_S)
#define GPIO_FUNC48_IN_SEL_V  0x000000FFU
#define GPIO_FUNC48_IN_SEL_S  0
/** GPIO_FUNC48_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC48_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC48_IN_INV_SEL_M  (GPIO_FUNC48_IN_INV_SEL_V << GPIO_FUNC48_IN_INV_SEL_S)
#define GPIO_FUNC48_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_IN_INV_SEL_S  8
/** GPIO_SIG48_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG48_IN_SEL    (BIT(9))
#define GPIO_SIG48_IN_SEL_M  (GPIO_SIG48_IN_SEL_V << GPIO_SIG48_IN_SEL_S)
#define GPIO_SIG48_IN_SEL_V  0x00000001U
#define GPIO_SIG48_IN_SEL_S  9

/** GPIO_FUNC49_IN_SEL_CFG_REG register
 *  Configuration register for input signal 49
 */
#define GPIO_FUNC49_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b8)
/** GPIO_FUNC49_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 49.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC49_IN_SEL    0x000000FFU
#define GPIO_FUNC49_IN_SEL_M  (GPIO_FUNC49_IN_SEL_V << GPIO_FUNC49_IN_SEL_S)
#define GPIO_FUNC49_IN_SEL_V  0x000000FFU
#define GPIO_FUNC49_IN_SEL_S  0
/** GPIO_FUNC49_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC49_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC49_IN_INV_SEL_M  (GPIO_FUNC49_IN_INV_SEL_V << GPIO_FUNC49_IN_INV_SEL_S)
#define GPIO_FUNC49_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_IN_INV_SEL_S  8
/** GPIO_SIG49_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG49_IN_SEL    (BIT(9))
#define GPIO_SIG49_IN_SEL_M  (GPIO_SIG49_IN_SEL_V << GPIO_SIG49_IN_SEL_S)
#define GPIO_SIG49_IN_SEL_V  0x00000001U
#define GPIO_SIG49_IN_SEL_S  9

/** GPIO_FUNC50_IN_SEL_CFG_REG register
 *  Configuration register for input signal 50
 */
#define GPIO_FUNC50_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3bc)
/** GPIO_FUNC50_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 50.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC50_IN_SEL    0x000000FFU
#define GPIO_FUNC50_IN_SEL_M  (GPIO_FUNC50_IN_SEL_V << GPIO_FUNC50_IN_SEL_S)
#define GPIO_FUNC50_IN_SEL_V  0x000000FFU
#define GPIO_FUNC50_IN_SEL_S  0
/** GPIO_FUNC50_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC50_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC50_IN_INV_SEL_M  (GPIO_FUNC50_IN_INV_SEL_V << GPIO_FUNC50_IN_INV_SEL_S)
#define GPIO_FUNC50_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_IN_INV_SEL_S  8
/** GPIO_SIG50_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG50_IN_SEL    (BIT(9))
#define GPIO_SIG50_IN_SEL_M  (GPIO_SIG50_IN_SEL_V << GPIO_SIG50_IN_SEL_S)
#define GPIO_SIG50_IN_SEL_V  0x00000001U
#define GPIO_SIG50_IN_SEL_S  9

/** GPIO_FUNC51_IN_SEL_CFG_REG register
 *  Configuration register for input signal 51
 */
#define GPIO_FUNC51_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c0)
/** GPIO_FUNC51_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 51.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC51_IN_SEL    0x000000FFU
#define GPIO_FUNC51_IN_SEL_M  (GPIO_FUNC51_IN_SEL_V << GPIO_FUNC51_IN_SEL_S)
#define GPIO_FUNC51_IN_SEL_V  0x000000FFU
#define GPIO_FUNC51_IN_SEL_S  0
/** GPIO_FUNC51_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC51_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC51_IN_INV_SEL_M  (GPIO_FUNC51_IN_INV_SEL_V << GPIO_FUNC51_IN_INV_SEL_S)
#define GPIO_FUNC51_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_IN_INV_SEL_S  8
/** GPIO_SIG51_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG51_IN_SEL    (BIT(9))
#define GPIO_SIG51_IN_SEL_M  (GPIO_SIG51_IN_SEL_V << GPIO_SIG51_IN_SEL_S)
#define GPIO_SIG51_IN_SEL_V  0x00000001U
#define GPIO_SIG51_IN_SEL_S  9

/** GPIO_FUNC52_IN_SEL_CFG_REG register
 *  Configuration register for input signal 52
 */
#define GPIO_FUNC52_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c4)
/** GPIO_FUNC52_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 52.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC52_IN_SEL    0x000000FFU
#define GPIO_FUNC52_IN_SEL_M  (GPIO_FUNC52_IN_SEL_V << GPIO_FUNC52_IN_SEL_S)
#define GPIO_FUNC52_IN_SEL_V  0x000000FFU
#define GPIO_FUNC52_IN_SEL_S  0
/** GPIO_FUNC52_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC52_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC52_IN_INV_SEL_M  (GPIO_FUNC52_IN_INV_SEL_V << GPIO_FUNC52_IN_INV_SEL_S)
#define GPIO_FUNC52_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_IN_INV_SEL_S  8
/** GPIO_SIG52_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG52_IN_SEL    (BIT(9))
#define GPIO_SIG52_IN_SEL_M  (GPIO_SIG52_IN_SEL_V << GPIO_SIG52_IN_SEL_S)
#define GPIO_SIG52_IN_SEL_V  0x00000001U
#define GPIO_SIG52_IN_SEL_S  9

/** GPIO_FUNC53_IN_SEL_CFG_REG register
 *  Configuration register for input signal 53
 */
#define GPIO_FUNC53_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c8)
/** GPIO_FUNC53_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 53.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC53_IN_SEL    0x000000FFU
#define GPIO_FUNC53_IN_SEL_M  (GPIO_FUNC53_IN_SEL_V << GPIO_FUNC53_IN_SEL_S)
#define GPIO_FUNC53_IN_SEL_V  0x000000FFU
#define GPIO_FUNC53_IN_SEL_S  0
/** GPIO_FUNC53_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC53_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC53_IN_INV_SEL_M  (GPIO_FUNC53_IN_INV_SEL_V << GPIO_FUNC53_IN_INV_SEL_S)
#define GPIO_FUNC53_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_IN_INV_SEL_S  8
/** GPIO_SIG53_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG53_IN_SEL    (BIT(9))
#define GPIO_SIG53_IN_SEL_M  (GPIO_SIG53_IN_SEL_V << GPIO_SIG53_IN_SEL_S)
#define GPIO_SIG53_IN_SEL_V  0x00000001U
#define GPIO_SIG53_IN_SEL_S  9

/** GPIO_FUNC54_IN_SEL_CFG_REG register
 *  Configuration register for input signal 54
 */
#define GPIO_FUNC54_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3cc)
/** GPIO_FUNC54_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 54.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC54_IN_SEL    0x000000FFU
#define GPIO_FUNC54_IN_SEL_M  (GPIO_FUNC54_IN_SEL_V << GPIO_FUNC54_IN_SEL_S)
#define GPIO_FUNC54_IN_SEL_V  0x000000FFU
#define GPIO_FUNC54_IN_SEL_S  0
/** GPIO_FUNC54_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC54_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC54_IN_INV_SEL_M  (GPIO_FUNC54_IN_INV_SEL_V << GPIO_FUNC54_IN_INV_SEL_S)
#define GPIO_FUNC54_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_IN_INV_SEL_S  8
/** GPIO_SIG54_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG54_IN_SEL    (BIT(9))
#define GPIO_SIG54_IN_SEL_M  (GPIO_SIG54_IN_SEL_V << GPIO_SIG54_IN_SEL_S)
#define GPIO_SIG54_IN_SEL_V  0x00000001U
#define GPIO_SIG54_IN_SEL_S  9

/** GPIO_FUNC55_IN_SEL_CFG_REG register
 *  Configuration register for input signal 55
 */
#define GPIO_FUNC55_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d0)
/** GPIO_FUNC55_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 55.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC55_IN_SEL    0x000000FFU
#define GPIO_FUNC55_IN_SEL_M  (GPIO_FUNC55_IN_SEL_V << GPIO_FUNC55_IN_SEL_S)
#define GPIO_FUNC55_IN_SEL_V  0x000000FFU
#define GPIO_FUNC55_IN_SEL_S  0
/** GPIO_FUNC55_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC55_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC55_IN_INV_SEL_M  (GPIO_FUNC55_IN_INV_SEL_V << GPIO_FUNC55_IN_INV_SEL_S)
#define GPIO_FUNC55_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_IN_INV_SEL_S  8
/** GPIO_SIG55_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG55_IN_SEL    (BIT(9))
#define GPIO_SIG55_IN_SEL_M  (GPIO_SIG55_IN_SEL_V << GPIO_SIG55_IN_SEL_S)
#define GPIO_SIG55_IN_SEL_V  0x00000001U
#define GPIO_SIG55_IN_SEL_S  9

/** GPIO_FUNC56_IN_SEL_CFG_REG register
 *  Configuration register for input signal 56
 */
#define GPIO_FUNC56_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d4)
/** GPIO_FUNC56_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 56.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC56_IN_SEL    0x000000FFU
#define GPIO_FUNC56_IN_SEL_M  (GPIO_FUNC56_IN_SEL_V << GPIO_FUNC56_IN_SEL_S)
#define GPIO_FUNC56_IN_SEL_V  0x000000FFU
#define GPIO_FUNC56_IN_SEL_S  0
/** GPIO_FUNC56_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC56_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC56_IN_INV_SEL_M  (GPIO_FUNC56_IN_INV_SEL_V << GPIO_FUNC56_IN_INV_SEL_S)
#define GPIO_FUNC56_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_IN_INV_SEL_S  8
/** GPIO_SIG56_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG56_IN_SEL    (BIT(9))
#define GPIO_SIG56_IN_SEL_M  (GPIO_SIG56_IN_SEL_V << GPIO_SIG56_IN_SEL_S)
#define GPIO_SIG56_IN_SEL_V  0x00000001U
#define GPIO_SIG56_IN_SEL_S  9

/** GPIO_FUNC57_IN_SEL_CFG_REG register
 *  Configuration register for input signal 57
 */
#define GPIO_FUNC57_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d8)
/** GPIO_FUNC57_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 57.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC57_IN_SEL    0x000000FFU
#define GPIO_FUNC57_IN_SEL_M  (GPIO_FUNC57_IN_SEL_V << GPIO_FUNC57_IN_SEL_S)
#define GPIO_FUNC57_IN_SEL_V  0x000000FFU
#define GPIO_FUNC57_IN_SEL_S  0
/** GPIO_FUNC57_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC57_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC57_IN_INV_SEL_M  (GPIO_FUNC57_IN_INV_SEL_V << GPIO_FUNC57_IN_INV_SEL_S)
#define GPIO_FUNC57_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC57_IN_INV_SEL_S  8
/** GPIO_SIG57_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG57_IN_SEL    (BIT(9))
#define GPIO_SIG57_IN_SEL_M  (GPIO_SIG57_IN_SEL_V << GPIO_SIG57_IN_SEL_S)
#define GPIO_SIG57_IN_SEL_V  0x00000001U
#define GPIO_SIG57_IN_SEL_S  9

/** GPIO_FUNC58_IN_SEL_CFG_REG register
 *  Configuration register for input signal 58
 */
#define GPIO_FUNC58_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3dc)
/** GPIO_FUNC58_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 58.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC58_IN_SEL    0x000000FFU
#define GPIO_FUNC58_IN_SEL_M  (GPIO_FUNC58_IN_SEL_V << GPIO_FUNC58_IN_SEL_S)
#define GPIO_FUNC58_IN_SEL_V  0x000000FFU
#define GPIO_FUNC58_IN_SEL_S  0
/** GPIO_FUNC58_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC58_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC58_IN_INV_SEL_M  (GPIO_FUNC58_IN_INV_SEL_V << GPIO_FUNC58_IN_INV_SEL_S)
#define GPIO_FUNC58_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC58_IN_INV_SEL_S  8
/** GPIO_SIG58_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG58_IN_SEL    (BIT(9))
#define GPIO_SIG58_IN_SEL_M  (GPIO_SIG58_IN_SEL_V << GPIO_SIG58_IN_SEL_S)
#define GPIO_SIG58_IN_SEL_V  0x00000001U
#define GPIO_SIG58_IN_SEL_S  9

/** GPIO_FUNC59_IN_SEL_CFG_REG register
 *  Configuration register for input signal 59
 */
#define GPIO_FUNC59_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e0)
/** GPIO_FUNC59_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 59.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC59_IN_SEL    0x000000FFU
#define GPIO_FUNC59_IN_SEL_M  (GPIO_FUNC59_IN_SEL_V << GPIO_FUNC59_IN_SEL_S)
#define GPIO_FUNC59_IN_SEL_V  0x000000FFU
#define GPIO_FUNC59_IN_SEL_S  0
/** GPIO_FUNC59_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC59_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC59_IN_INV_SEL_M  (GPIO_FUNC59_IN_INV_SEL_V << GPIO_FUNC59_IN_INV_SEL_S)
#define GPIO_FUNC59_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC59_IN_INV_SEL_S  8
/** GPIO_SIG59_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG59_IN_SEL    (BIT(9))
#define GPIO_SIG59_IN_SEL_M  (GPIO_SIG59_IN_SEL_V << GPIO_SIG59_IN_SEL_S)
#define GPIO_SIG59_IN_SEL_V  0x00000001U
#define GPIO_SIG59_IN_SEL_S  9

/** GPIO_FUNC60_IN_SEL_CFG_REG register
 *  Configuration register for input signal 60
 */
#define GPIO_FUNC60_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e4)
/** GPIO_FUNC60_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 60.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC60_IN_SEL    0x000000FFU
#define GPIO_FUNC60_IN_SEL_M  (GPIO_FUNC60_IN_SEL_V << GPIO_FUNC60_IN_SEL_S)
#define GPIO_FUNC60_IN_SEL_V  0x000000FFU
#define GPIO_FUNC60_IN_SEL_S  0
/** GPIO_FUNC60_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC60_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC60_IN_INV_SEL_M  (GPIO_FUNC60_IN_INV_SEL_V << GPIO_FUNC60_IN_INV_SEL_S)
#define GPIO_FUNC60_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC60_IN_INV_SEL_S  8
/** GPIO_SIG60_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG60_IN_SEL    (BIT(9))
#define GPIO_SIG60_IN_SEL_M  (GPIO_SIG60_IN_SEL_V << GPIO_SIG60_IN_SEL_S)
#define GPIO_SIG60_IN_SEL_V  0x00000001U
#define GPIO_SIG60_IN_SEL_S  9

/** GPIO_FUNC61_IN_SEL_CFG_REG register
 *  Configuration register for input signal 61
 */
#define GPIO_FUNC61_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e8)
/** GPIO_FUNC61_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 61.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC61_IN_SEL    0x000000FFU
#define GPIO_FUNC61_IN_SEL_M  (GPIO_FUNC61_IN_SEL_V << GPIO_FUNC61_IN_SEL_S)
#define GPIO_FUNC61_IN_SEL_V  0x000000FFU
#define GPIO_FUNC61_IN_SEL_S  0
/** GPIO_FUNC61_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC61_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC61_IN_INV_SEL_M  (GPIO_FUNC61_IN_INV_SEL_V << GPIO_FUNC61_IN_INV_SEL_S)
#define GPIO_FUNC61_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC61_IN_INV_SEL_S  8
/** GPIO_SIG61_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG61_IN_SEL    (BIT(9))
#define GPIO_SIG61_IN_SEL_M  (GPIO_SIG61_IN_SEL_V << GPIO_SIG61_IN_SEL_S)
#define GPIO_SIG61_IN_SEL_V  0x00000001U
#define GPIO_SIG61_IN_SEL_S  9

/** GPIO_FUNC62_IN_SEL_CFG_REG register
 *  Configuration register for input signal 62
 */
#define GPIO_FUNC62_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3ec)
/** GPIO_FUNC62_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 62.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC62_IN_SEL    0x000000FFU
#define GPIO_FUNC62_IN_SEL_M  (GPIO_FUNC62_IN_SEL_V << GPIO_FUNC62_IN_SEL_S)
#define GPIO_FUNC62_IN_SEL_V  0x000000FFU
#define GPIO_FUNC62_IN_SEL_S  0
/** GPIO_FUNC62_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC62_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC62_IN_INV_SEL_M  (GPIO_FUNC62_IN_INV_SEL_V << GPIO_FUNC62_IN_INV_SEL_S)
#define GPIO_FUNC62_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC62_IN_INV_SEL_S  8
/** GPIO_SIG62_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG62_IN_SEL    (BIT(9))
#define GPIO_SIG62_IN_SEL_M  (GPIO_SIG62_IN_SEL_V << GPIO_SIG62_IN_SEL_S)
#define GPIO_SIG62_IN_SEL_V  0x00000001U
#define GPIO_SIG62_IN_SEL_S  9

/** GPIO_FUNC63_IN_SEL_CFG_REG register
 *  Configuration register for input signal 63
 */
#define GPIO_FUNC63_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f0)
/** GPIO_FUNC63_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 63.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC63_IN_SEL    0x000000FFU
#define GPIO_FUNC63_IN_SEL_M  (GPIO_FUNC63_IN_SEL_V << GPIO_FUNC63_IN_SEL_S)
#define GPIO_FUNC63_IN_SEL_V  0x000000FFU
#define GPIO_FUNC63_IN_SEL_S  0
/** GPIO_FUNC63_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC63_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC63_IN_INV_SEL_M  (GPIO_FUNC63_IN_INV_SEL_V << GPIO_FUNC63_IN_INV_SEL_S)
#define GPIO_FUNC63_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC63_IN_INV_SEL_S  8
/** GPIO_SIG63_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG63_IN_SEL    (BIT(9))
#define GPIO_SIG63_IN_SEL_M  (GPIO_SIG63_IN_SEL_V << GPIO_SIG63_IN_SEL_S)
#define GPIO_SIG63_IN_SEL_V  0x00000001U
#define GPIO_SIG63_IN_SEL_S  9

/** GPIO_FUNC64_IN_SEL_CFG_REG register
 *  Configuration register for input signal 64
 */
#define GPIO_FUNC64_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f4)
/** GPIO_FUNC64_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 64.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC64_IN_SEL    0x000000FFU
#define GPIO_FUNC64_IN_SEL_M  (GPIO_FUNC64_IN_SEL_V << GPIO_FUNC64_IN_SEL_S)
#define GPIO_FUNC64_IN_SEL_V  0x000000FFU
#define GPIO_FUNC64_IN_SEL_S  0
/** GPIO_FUNC64_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC64_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC64_IN_INV_SEL_M  (GPIO_FUNC64_IN_INV_SEL_V << GPIO_FUNC64_IN_INV_SEL_S)
#define GPIO_FUNC64_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC64_IN_INV_SEL_S  8
/** GPIO_SIG64_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG64_IN_SEL    (BIT(9))
#define GPIO_SIG64_IN_SEL_M  (GPIO_SIG64_IN_SEL_V << GPIO_SIG64_IN_SEL_S)
#define GPIO_SIG64_IN_SEL_V  0x00000001U
#define GPIO_SIG64_IN_SEL_S  9

/** GPIO_FUNC65_IN_SEL_CFG_REG register
 *  Configuration register for input signal 65
 */
#define GPIO_FUNC65_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f8)
/** GPIO_FUNC65_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 65.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC65_IN_SEL    0x000000FFU
#define GPIO_FUNC65_IN_SEL_M  (GPIO_FUNC65_IN_SEL_V << GPIO_FUNC65_IN_SEL_S)
#define GPIO_FUNC65_IN_SEL_V  0x000000FFU
#define GPIO_FUNC65_IN_SEL_S  0
/** GPIO_FUNC65_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC65_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC65_IN_INV_SEL_M  (GPIO_FUNC65_IN_INV_SEL_V << GPIO_FUNC65_IN_INV_SEL_S)
#define GPIO_FUNC65_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC65_IN_INV_SEL_S  8
/** GPIO_SIG65_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG65_IN_SEL    (BIT(9))
#define GPIO_SIG65_IN_SEL_M  (GPIO_SIG65_IN_SEL_V << GPIO_SIG65_IN_SEL_S)
#define GPIO_SIG65_IN_SEL_V  0x00000001U
#define GPIO_SIG65_IN_SEL_S  9

/** GPIO_FUNC66_IN_SEL_CFG_REG register
 *  Configuration register for input signal 66
 */
#define GPIO_FUNC66_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3fc)
/** GPIO_FUNC66_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 66.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC66_IN_SEL    0x000000FFU
#define GPIO_FUNC66_IN_SEL_M  (GPIO_FUNC66_IN_SEL_V << GPIO_FUNC66_IN_SEL_S)
#define GPIO_FUNC66_IN_SEL_V  0x000000FFU
#define GPIO_FUNC66_IN_SEL_S  0
/** GPIO_FUNC66_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC66_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC66_IN_INV_SEL_M  (GPIO_FUNC66_IN_INV_SEL_V << GPIO_FUNC66_IN_INV_SEL_S)
#define GPIO_FUNC66_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC66_IN_INV_SEL_S  8
/** GPIO_SIG66_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG66_IN_SEL    (BIT(9))
#define GPIO_SIG66_IN_SEL_M  (GPIO_SIG66_IN_SEL_V << GPIO_SIG66_IN_SEL_S)
#define GPIO_SIG66_IN_SEL_V  0x00000001U
#define GPIO_SIG66_IN_SEL_S  9

/** GPIO_FUNC68_IN_SEL_CFG_REG register
 *  Configuration register for input signal 68
 */
#define GPIO_FUNC68_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x404)
/** GPIO_FUNC68_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 68.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC68_IN_SEL    0x000000FFU
#define GPIO_FUNC68_IN_SEL_M  (GPIO_FUNC68_IN_SEL_V << GPIO_FUNC68_IN_SEL_S)
#define GPIO_FUNC68_IN_SEL_V  0x000000FFU
#define GPIO_FUNC68_IN_SEL_S  0
/** GPIO_FUNC68_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC68_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC68_IN_INV_SEL_M  (GPIO_FUNC68_IN_INV_SEL_V << GPIO_FUNC68_IN_INV_SEL_S)
#define GPIO_FUNC68_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC68_IN_INV_SEL_S  8
/** GPIO_SIG68_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG68_IN_SEL    (BIT(9))
#define GPIO_SIG68_IN_SEL_M  (GPIO_SIG68_IN_SEL_V << GPIO_SIG68_IN_SEL_S)
#define GPIO_SIG68_IN_SEL_V  0x00000001U
#define GPIO_SIG68_IN_SEL_S  9

/** GPIO_FUNC69_IN_SEL_CFG_REG register
 *  Configuration register for input signal 69
 */
#define GPIO_FUNC69_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x408)
/** GPIO_FUNC69_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 69.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC69_IN_SEL    0x000000FFU
#define GPIO_FUNC69_IN_SEL_M  (GPIO_FUNC69_IN_SEL_V << GPIO_FUNC69_IN_SEL_S)
#define GPIO_FUNC69_IN_SEL_V  0x000000FFU
#define GPIO_FUNC69_IN_SEL_S  0
/** GPIO_FUNC69_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC69_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC69_IN_INV_SEL_M  (GPIO_FUNC69_IN_INV_SEL_V << GPIO_FUNC69_IN_INV_SEL_S)
#define GPIO_FUNC69_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC69_IN_INV_SEL_S  8
/** GPIO_SIG69_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG69_IN_SEL    (BIT(9))
#define GPIO_SIG69_IN_SEL_M  (GPIO_SIG69_IN_SEL_V << GPIO_SIG69_IN_SEL_S)
#define GPIO_SIG69_IN_SEL_V  0x00000001U
#define GPIO_SIG69_IN_SEL_S  9

/** GPIO_FUNC70_IN_SEL_CFG_REG register
 *  Configuration register for input signal 70
 */
#define GPIO_FUNC70_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x40c)
/** GPIO_FUNC70_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 70.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC70_IN_SEL    0x000000FFU
#define GPIO_FUNC70_IN_SEL_M  (GPIO_FUNC70_IN_SEL_V << GPIO_FUNC70_IN_SEL_S)
#define GPIO_FUNC70_IN_SEL_V  0x000000FFU
#define GPIO_FUNC70_IN_SEL_S  0
/** GPIO_FUNC70_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC70_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC70_IN_INV_SEL_M  (GPIO_FUNC70_IN_INV_SEL_V << GPIO_FUNC70_IN_INV_SEL_S)
#define GPIO_FUNC70_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC70_IN_INV_SEL_S  8
/** GPIO_SIG70_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG70_IN_SEL    (BIT(9))
#define GPIO_SIG70_IN_SEL_M  (GPIO_SIG70_IN_SEL_V << GPIO_SIG70_IN_SEL_S)
#define GPIO_SIG70_IN_SEL_V  0x00000001U
#define GPIO_SIG70_IN_SEL_S  9

/** GPIO_FUNC71_IN_SEL_CFG_REG register
 *  Configuration register for input signal 71
 */
#define GPIO_FUNC71_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x410)
/** GPIO_FUNC71_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 71.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC71_IN_SEL    0x000000FFU
#define GPIO_FUNC71_IN_SEL_M  (GPIO_FUNC71_IN_SEL_V << GPIO_FUNC71_IN_SEL_S)
#define GPIO_FUNC71_IN_SEL_V  0x000000FFU
#define GPIO_FUNC71_IN_SEL_S  0
/** GPIO_FUNC71_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC71_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC71_IN_INV_SEL_M  (GPIO_FUNC71_IN_INV_SEL_V << GPIO_FUNC71_IN_INV_SEL_S)
#define GPIO_FUNC71_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC71_IN_INV_SEL_S  8
/** GPIO_SIG71_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG71_IN_SEL    (BIT(9))
#define GPIO_SIG71_IN_SEL_M  (GPIO_SIG71_IN_SEL_V << GPIO_SIG71_IN_SEL_S)
#define GPIO_SIG71_IN_SEL_V  0x00000001U
#define GPIO_SIG71_IN_SEL_S  9

/** GPIO_FUNC72_IN_SEL_CFG_REG register
 *  Configuration register for input signal 72
 */
#define GPIO_FUNC72_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x414)
/** GPIO_FUNC72_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 72.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC72_IN_SEL    0x000000FFU
#define GPIO_FUNC72_IN_SEL_M  (GPIO_FUNC72_IN_SEL_V << GPIO_FUNC72_IN_SEL_S)
#define GPIO_FUNC72_IN_SEL_V  0x000000FFU
#define GPIO_FUNC72_IN_SEL_S  0
/** GPIO_FUNC72_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC72_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC72_IN_INV_SEL_M  (GPIO_FUNC72_IN_INV_SEL_V << GPIO_FUNC72_IN_INV_SEL_S)
#define GPIO_FUNC72_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC72_IN_INV_SEL_S  8
/** GPIO_SIG72_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG72_IN_SEL    (BIT(9))
#define GPIO_SIG72_IN_SEL_M  (GPIO_SIG72_IN_SEL_V << GPIO_SIG72_IN_SEL_S)
#define GPIO_SIG72_IN_SEL_V  0x00000001U
#define GPIO_SIG72_IN_SEL_S  9

/** GPIO_FUNC73_IN_SEL_CFG_REG register
 *  Configuration register for input signal 73
 */
#define GPIO_FUNC73_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x418)
/** GPIO_FUNC73_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 73.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC73_IN_SEL    0x000000FFU
#define GPIO_FUNC73_IN_SEL_M  (GPIO_FUNC73_IN_SEL_V << GPIO_FUNC73_IN_SEL_S)
#define GPIO_FUNC73_IN_SEL_V  0x000000FFU
#define GPIO_FUNC73_IN_SEL_S  0
/** GPIO_FUNC73_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC73_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC73_IN_INV_SEL_M  (GPIO_FUNC73_IN_INV_SEL_V << GPIO_FUNC73_IN_INV_SEL_S)
#define GPIO_FUNC73_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC73_IN_INV_SEL_S  8
/** GPIO_SIG73_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG73_IN_SEL    (BIT(9))
#define GPIO_SIG73_IN_SEL_M  (GPIO_SIG73_IN_SEL_V << GPIO_SIG73_IN_SEL_S)
#define GPIO_SIG73_IN_SEL_V  0x00000001U
#define GPIO_SIG73_IN_SEL_S  9

/** GPIO_FUNC74_IN_SEL_CFG_REG register
 *  Configuration register for input signal 74
 */
#define GPIO_FUNC74_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x41c)
/** GPIO_FUNC74_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 74.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC74_IN_SEL    0x000000FFU
#define GPIO_FUNC74_IN_SEL_M  (GPIO_FUNC74_IN_SEL_V << GPIO_FUNC74_IN_SEL_S)
#define GPIO_FUNC74_IN_SEL_V  0x000000FFU
#define GPIO_FUNC74_IN_SEL_S  0
/** GPIO_FUNC74_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC74_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC74_IN_INV_SEL_M  (GPIO_FUNC74_IN_INV_SEL_V << GPIO_FUNC74_IN_INV_SEL_S)
#define GPIO_FUNC74_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC74_IN_INV_SEL_S  8
/** GPIO_SIG74_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG74_IN_SEL    (BIT(9))
#define GPIO_SIG74_IN_SEL_M  (GPIO_SIG74_IN_SEL_V << GPIO_SIG74_IN_SEL_S)
#define GPIO_SIG74_IN_SEL_V  0x00000001U
#define GPIO_SIG74_IN_SEL_S  9

/** GPIO_FUNC75_IN_SEL_CFG_REG register
 *  Configuration register for input signal 75
 */
#define GPIO_FUNC75_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x420)
/** GPIO_FUNC75_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 75.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC75_IN_SEL    0x000000FFU
#define GPIO_FUNC75_IN_SEL_M  (GPIO_FUNC75_IN_SEL_V << GPIO_FUNC75_IN_SEL_S)
#define GPIO_FUNC75_IN_SEL_V  0x000000FFU
#define GPIO_FUNC75_IN_SEL_S  0
/** GPIO_FUNC75_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC75_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC75_IN_INV_SEL_M  (GPIO_FUNC75_IN_INV_SEL_V << GPIO_FUNC75_IN_INV_SEL_S)
#define GPIO_FUNC75_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC75_IN_INV_SEL_S  8
/** GPIO_SIG75_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG75_IN_SEL    (BIT(9))
#define GPIO_SIG75_IN_SEL_M  (GPIO_SIG75_IN_SEL_V << GPIO_SIG75_IN_SEL_S)
#define GPIO_SIG75_IN_SEL_V  0x00000001U
#define GPIO_SIG75_IN_SEL_S  9

/** GPIO_FUNC76_IN_SEL_CFG_REG register
 *  Configuration register for input signal 76
 */
#define GPIO_FUNC76_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x424)
/** GPIO_FUNC76_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 76.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC76_IN_SEL    0x000000FFU
#define GPIO_FUNC76_IN_SEL_M  (GPIO_FUNC76_IN_SEL_V << GPIO_FUNC76_IN_SEL_S)
#define GPIO_FUNC76_IN_SEL_V  0x000000FFU
#define GPIO_FUNC76_IN_SEL_S  0
/** GPIO_FUNC76_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC76_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC76_IN_INV_SEL_M  (GPIO_FUNC76_IN_INV_SEL_V << GPIO_FUNC76_IN_INV_SEL_S)
#define GPIO_FUNC76_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC76_IN_INV_SEL_S  8
/** GPIO_SIG76_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG76_IN_SEL    (BIT(9))
#define GPIO_SIG76_IN_SEL_M  (GPIO_SIG76_IN_SEL_V << GPIO_SIG76_IN_SEL_S)
#define GPIO_SIG76_IN_SEL_V  0x00000001U
#define GPIO_SIG76_IN_SEL_S  9

/** GPIO_FUNC77_IN_SEL_CFG_REG register
 *  Configuration register for input signal 77
 */
#define GPIO_FUNC77_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x428)
/** GPIO_FUNC77_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 77.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC77_IN_SEL    0x000000FFU
#define GPIO_FUNC77_IN_SEL_M  (GPIO_FUNC77_IN_SEL_V << GPIO_FUNC77_IN_SEL_S)
#define GPIO_FUNC77_IN_SEL_V  0x000000FFU
#define GPIO_FUNC77_IN_SEL_S  0
/** GPIO_FUNC77_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC77_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC77_IN_INV_SEL_M  (GPIO_FUNC77_IN_INV_SEL_V << GPIO_FUNC77_IN_INV_SEL_S)
#define GPIO_FUNC77_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC77_IN_INV_SEL_S  8
/** GPIO_SIG77_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG77_IN_SEL    (BIT(9))
#define GPIO_SIG77_IN_SEL_M  (GPIO_SIG77_IN_SEL_V << GPIO_SIG77_IN_SEL_S)
#define GPIO_SIG77_IN_SEL_V  0x00000001U
#define GPIO_SIG77_IN_SEL_S  9

/** GPIO_FUNC78_IN_SEL_CFG_REG register
 *  Configuration register for input signal 78
 */
#define GPIO_FUNC78_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x42c)
/** GPIO_FUNC78_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 78.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC78_IN_SEL    0x000000FFU
#define GPIO_FUNC78_IN_SEL_M  (GPIO_FUNC78_IN_SEL_V << GPIO_FUNC78_IN_SEL_S)
#define GPIO_FUNC78_IN_SEL_V  0x000000FFU
#define GPIO_FUNC78_IN_SEL_S  0
/** GPIO_FUNC78_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC78_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC78_IN_INV_SEL_M  (GPIO_FUNC78_IN_INV_SEL_V << GPIO_FUNC78_IN_INV_SEL_S)
#define GPIO_FUNC78_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC78_IN_INV_SEL_S  8
/** GPIO_SIG78_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG78_IN_SEL    (BIT(9))
#define GPIO_SIG78_IN_SEL_M  (GPIO_SIG78_IN_SEL_V << GPIO_SIG78_IN_SEL_S)
#define GPIO_SIG78_IN_SEL_V  0x00000001U
#define GPIO_SIG78_IN_SEL_S  9

/** GPIO_FUNC79_IN_SEL_CFG_REG register
 *  Configuration register for input signal 79
 */
#define GPIO_FUNC79_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x430)
/** GPIO_FUNC79_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 79.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC79_IN_SEL    0x000000FFU
#define GPIO_FUNC79_IN_SEL_M  (GPIO_FUNC79_IN_SEL_V << GPIO_FUNC79_IN_SEL_S)
#define GPIO_FUNC79_IN_SEL_V  0x000000FFU
#define GPIO_FUNC79_IN_SEL_S  0
/** GPIO_FUNC79_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC79_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC79_IN_INV_SEL_M  (GPIO_FUNC79_IN_INV_SEL_V << GPIO_FUNC79_IN_INV_SEL_S)
#define GPIO_FUNC79_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC79_IN_INV_SEL_S  8
/** GPIO_SIG79_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG79_IN_SEL    (BIT(9))
#define GPIO_SIG79_IN_SEL_M  (GPIO_SIG79_IN_SEL_V << GPIO_SIG79_IN_SEL_S)
#define GPIO_SIG79_IN_SEL_V  0x00000001U
#define GPIO_SIG79_IN_SEL_S  9

/** GPIO_FUNC80_IN_SEL_CFG_REG register
 *  Configuration register for input signal 80
 */
#define GPIO_FUNC80_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x434)
/** GPIO_FUNC80_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 80.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC80_IN_SEL    0x000000FFU
#define GPIO_FUNC80_IN_SEL_M  (GPIO_FUNC80_IN_SEL_V << GPIO_FUNC80_IN_SEL_S)
#define GPIO_FUNC80_IN_SEL_V  0x000000FFU
#define GPIO_FUNC80_IN_SEL_S  0
/** GPIO_FUNC80_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC80_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC80_IN_INV_SEL_M  (GPIO_FUNC80_IN_INV_SEL_V << GPIO_FUNC80_IN_INV_SEL_S)
#define GPIO_FUNC80_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC80_IN_INV_SEL_S  8
/** GPIO_SIG80_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG80_IN_SEL    (BIT(9))
#define GPIO_SIG80_IN_SEL_M  (GPIO_SIG80_IN_SEL_V << GPIO_SIG80_IN_SEL_S)
#define GPIO_SIG80_IN_SEL_V  0x00000001U
#define GPIO_SIG80_IN_SEL_S  9

/** GPIO_FUNC81_IN_SEL_CFG_REG register
 *  Configuration register for input signal 81
 */
#define GPIO_FUNC81_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x438)
/** GPIO_FUNC81_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 81.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC81_IN_SEL    0x000000FFU
#define GPIO_FUNC81_IN_SEL_M  (GPIO_FUNC81_IN_SEL_V << GPIO_FUNC81_IN_SEL_S)
#define GPIO_FUNC81_IN_SEL_V  0x000000FFU
#define GPIO_FUNC81_IN_SEL_S  0
/** GPIO_FUNC81_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC81_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC81_IN_INV_SEL_M  (GPIO_FUNC81_IN_INV_SEL_V << GPIO_FUNC81_IN_INV_SEL_S)
#define GPIO_FUNC81_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC81_IN_INV_SEL_S  8
/** GPIO_SIG81_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG81_IN_SEL    (BIT(9))
#define GPIO_SIG81_IN_SEL_M  (GPIO_SIG81_IN_SEL_V << GPIO_SIG81_IN_SEL_S)
#define GPIO_SIG81_IN_SEL_V  0x00000001U
#define GPIO_SIG81_IN_SEL_S  9

/** GPIO_FUNC82_IN_SEL_CFG_REG register
 *  Configuration register for input signal 82
 */
#define GPIO_FUNC82_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x43c)
/** GPIO_FUNC82_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 82.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC82_IN_SEL    0x000000FFU
#define GPIO_FUNC82_IN_SEL_M  (GPIO_FUNC82_IN_SEL_V << GPIO_FUNC82_IN_SEL_S)
#define GPIO_FUNC82_IN_SEL_V  0x000000FFU
#define GPIO_FUNC82_IN_SEL_S  0
/** GPIO_FUNC82_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC82_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC82_IN_INV_SEL_M  (GPIO_FUNC82_IN_INV_SEL_V << GPIO_FUNC82_IN_INV_SEL_S)
#define GPIO_FUNC82_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC82_IN_INV_SEL_S  8
/** GPIO_SIG82_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG82_IN_SEL    (BIT(9))
#define GPIO_SIG82_IN_SEL_M  (GPIO_SIG82_IN_SEL_V << GPIO_SIG82_IN_SEL_S)
#define GPIO_SIG82_IN_SEL_V  0x00000001U
#define GPIO_SIG82_IN_SEL_S  9

/** GPIO_FUNC83_IN_SEL_CFG_REG register
 *  Configuration register for input signal 83
 */
#define GPIO_FUNC83_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x440)
/** GPIO_FUNC83_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 83.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC83_IN_SEL    0x000000FFU
#define GPIO_FUNC83_IN_SEL_M  (GPIO_FUNC83_IN_SEL_V << GPIO_FUNC83_IN_SEL_S)
#define GPIO_FUNC83_IN_SEL_V  0x000000FFU
#define GPIO_FUNC83_IN_SEL_S  0
/** GPIO_FUNC83_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC83_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC83_IN_INV_SEL_M  (GPIO_FUNC83_IN_INV_SEL_V << GPIO_FUNC83_IN_INV_SEL_S)
#define GPIO_FUNC83_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC83_IN_INV_SEL_S  8
/** GPIO_SIG83_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG83_IN_SEL    (BIT(9))
#define GPIO_SIG83_IN_SEL_M  (GPIO_SIG83_IN_SEL_V << GPIO_SIG83_IN_SEL_S)
#define GPIO_SIG83_IN_SEL_V  0x00000001U
#define GPIO_SIG83_IN_SEL_S  9

/** GPIO_FUNC84_IN_SEL_CFG_REG register
 *  Configuration register for input signal 84
 */
#define GPIO_FUNC84_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x444)
/** GPIO_FUNC84_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 84.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC84_IN_SEL    0x000000FFU
#define GPIO_FUNC84_IN_SEL_M  (GPIO_FUNC84_IN_SEL_V << GPIO_FUNC84_IN_SEL_S)
#define GPIO_FUNC84_IN_SEL_V  0x000000FFU
#define GPIO_FUNC84_IN_SEL_S  0
/** GPIO_FUNC84_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC84_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC84_IN_INV_SEL_M  (GPIO_FUNC84_IN_INV_SEL_V << GPIO_FUNC84_IN_INV_SEL_S)
#define GPIO_FUNC84_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC84_IN_INV_SEL_S  8
/** GPIO_SIG84_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG84_IN_SEL    (BIT(9))
#define GPIO_SIG84_IN_SEL_M  (GPIO_SIG84_IN_SEL_V << GPIO_SIG84_IN_SEL_S)
#define GPIO_SIG84_IN_SEL_V  0x00000001U
#define GPIO_SIG84_IN_SEL_S  9

/** GPIO_FUNC85_IN_SEL_CFG_REG register
 *  Configuration register for input signal 85
 */
#define GPIO_FUNC85_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x448)
/** GPIO_FUNC85_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 85.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC85_IN_SEL    0x000000FFU
#define GPIO_FUNC85_IN_SEL_M  (GPIO_FUNC85_IN_SEL_V << GPIO_FUNC85_IN_SEL_S)
#define GPIO_FUNC85_IN_SEL_V  0x000000FFU
#define GPIO_FUNC85_IN_SEL_S  0
/** GPIO_FUNC85_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC85_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC85_IN_INV_SEL_M  (GPIO_FUNC85_IN_INV_SEL_V << GPIO_FUNC85_IN_INV_SEL_S)
#define GPIO_FUNC85_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC85_IN_INV_SEL_S  8
/** GPIO_SIG85_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG85_IN_SEL    (BIT(9))
#define GPIO_SIG85_IN_SEL_M  (GPIO_SIG85_IN_SEL_V << GPIO_SIG85_IN_SEL_S)
#define GPIO_SIG85_IN_SEL_V  0x00000001U
#define GPIO_SIG85_IN_SEL_S  9

/** GPIO_FUNC86_IN_SEL_CFG_REG register
 *  Configuration register for input signal 86
 */
#define GPIO_FUNC86_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x44c)
/** GPIO_FUNC86_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 86.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC86_IN_SEL    0x000000FFU
#define GPIO_FUNC86_IN_SEL_M  (GPIO_FUNC86_IN_SEL_V << GPIO_FUNC86_IN_SEL_S)
#define GPIO_FUNC86_IN_SEL_V  0x000000FFU
#define GPIO_FUNC86_IN_SEL_S  0
/** GPIO_FUNC86_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC86_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC86_IN_INV_SEL_M  (GPIO_FUNC86_IN_INV_SEL_V << GPIO_FUNC86_IN_INV_SEL_S)
#define GPIO_FUNC86_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC86_IN_INV_SEL_S  8
/** GPIO_SIG86_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG86_IN_SEL    (BIT(9))
#define GPIO_SIG86_IN_SEL_M  (GPIO_SIG86_IN_SEL_V << GPIO_SIG86_IN_SEL_S)
#define GPIO_SIG86_IN_SEL_V  0x00000001U
#define GPIO_SIG86_IN_SEL_S  9

/** GPIO_FUNC87_IN_SEL_CFG_REG register
 *  Configuration register for input signal 87
 */
#define GPIO_FUNC87_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x450)
/** GPIO_FUNC87_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 87.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC87_IN_SEL    0x000000FFU
#define GPIO_FUNC87_IN_SEL_M  (GPIO_FUNC87_IN_SEL_V << GPIO_FUNC87_IN_SEL_S)
#define GPIO_FUNC87_IN_SEL_V  0x000000FFU
#define GPIO_FUNC87_IN_SEL_S  0
/** GPIO_FUNC87_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC87_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC87_IN_INV_SEL_M  (GPIO_FUNC87_IN_INV_SEL_V << GPIO_FUNC87_IN_INV_SEL_S)
#define GPIO_FUNC87_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC87_IN_INV_SEL_S  8
/** GPIO_SIG87_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG87_IN_SEL    (BIT(9))
#define GPIO_SIG87_IN_SEL_M  (GPIO_SIG87_IN_SEL_V << GPIO_SIG87_IN_SEL_S)
#define GPIO_SIG87_IN_SEL_V  0x00000001U
#define GPIO_SIG87_IN_SEL_S  9

/** GPIO_FUNC88_IN_SEL_CFG_REG register
 *  Configuration register for input signal 88
 */
#define GPIO_FUNC88_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x454)
/** GPIO_FUNC88_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 88.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC88_IN_SEL    0x000000FFU
#define GPIO_FUNC88_IN_SEL_M  (GPIO_FUNC88_IN_SEL_V << GPIO_FUNC88_IN_SEL_S)
#define GPIO_FUNC88_IN_SEL_V  0x000000FFU
#define GPIO_FUNC88_IN_SEL_S  0
/** GPIO_FUNC88_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC88_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC88_IN_INV_SEL_M  (GPIO_FUNC88_IN_INV_SEL_V << GPIO_FUNC88_IN_INV_SEL_S)
#define GPIO_FUNC88_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC88_IN_INV_SEL_S  8
/** GPIO_SIG88_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG88_IN_SEL    (BIT(9))
#define GPIO_SIG88_IN_SEL_M  (GPIO_SIG88_IN_SEL_V << GPIO_SIG88_IN_SEL_S)
#define GPIO_SIG88_IN_SEL_V  0x00000001U
#define GPIO_SIG88_IN_SEL_S  9

/** GPIO_FUNC89_IN_SEL_CFG_REG register
 *  Configuration register for input signal 89
 */
#define GPIO_FUNC89_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x458)
/** GPIO_FUNC89_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 89.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC89_IN_SEL    0x000000FFU
#define GPIO_FUNC89_IN_SEL_M  (GPIO_FUNC89_IN_SEL_V << GPIO_FUNC89_IN_SEL_S)
#define GPIO_FUNC89_IN_SEL_V  0x000000FFU
#define GPIO_FUNC89_IN_SEL_S  0
/** GPIO_FUNC89_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC89_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC89_IN_INV_SEL_M  (GPIO_FUNC89_IN_INV_SEL_V << GPIO_FUNC89_IN_INV_SEL_S)
#define GPIO_FUNC89_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC89_IN_INV_SEL_S  8
/** GPIO_SIG89_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG89_IN_SEL    (BIT(9))
#define GPIO_SIG89_IN_SEL_M  (GPIO_SIG89_IN_SEL_V << GPIO_SIG89_IN_SEL_S)
#define GPIO_SIG89_IN_SEL_V  0x00000001U
#define GPIO_SIG89_IN_SEL_S  9

/** GPIO_FUNC90_IN_SEL_CFG_REG register
 *  Configuration register for input signal 90
 */
#define GPIO_FUNC90_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x45c)
/** GPIO_FUNC90_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 90.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC90_IN_SEL    0x000000FFU
#define GPIO_FUNC90_IN_SEL_M  (GPIO_FUNC90_IN_SEL_V << GPIO_FUNC90_IN_SEL_S)
#define GPIO_FUNC90_IN_SEL_V  0x000000FFU
#define GPIO_FUNC90_IN_SEL_S  0
/** GPIO_FUNC90_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC90_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC90_IN_INV_SEL_M  (GPIO_FUNC90_IN_INV_SEL_V << GPIO_FUNC90_IN_INV_SEL_S)
#define GPIO_FUNC90_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC90_IN_INV_SEL_S  8
/** GPIO_SIG90_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG90_IN_SEL    (BIT(9))
#define GPIO_SIG90_IN_SEL_M  (GPIO_SIG90_IN_SEL_V << GPIO_SIG90_IN_SEL_S)
#define GPIO_SIG90_IN_SEL_V  0x00000001U
#define GPIO_SIG90_IN_SEL_S  9

/** GPIO_FUNC91_IN_SEL_CFG_REG register
 *  Configuration register for input signal 91
 */
#define GPIO_FUNC91_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x460)
/** GPIO_FUNC91_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 91.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC91_IN_SEL    0x000000FFU
#define GPIO_FUNC91_IN_SEL_M  (GPIO_FUNC91_IN_SEL_V << GPIO_FUNC91_IN_SEL_S)
#define GPIO_FUNC91_IN_SEL_V  0x000000FFU
#define GPIO_FUNC91_IN_SEL_S  0
/** GPIO_FUNC91_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC91_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC91_IN_INV_SEL_M  (GPIO_FUNC91_IN_INV_SEL_V << GPIO_FUNC91_IN_INV_SEL_S)
#define GPIO_FUNC91_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC91_IN_INV_SEL_S  8
/** GPIO_SIG91_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG91_IN_SEL    (BIT(9))
#define GPIO_SIG91_IN_SEL_M  (GPIO_SIG91_IN_SEL_V << GPIO_SIG91_IN_SEL_S)
#define GPIO_SIG91_IN_SEL_V  0x00000001U
#define GPIO_SIG91_IN_SEL_S  9

/** GPIO_FUNC92_IN_SEL_CFG_REG register
 *  Configuration register for input signal 92
 */
#define GPIO_FUNC92_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x464)
/** GPIO_FUNC92_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 92.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC92_IN_SEL    0x000000FFU
#define GPIO_FUNC92_IN_SEL_M  (GPIO_FUNC92_IN_SEL_V << GPIO_FUNC92_IN_SEL_S)
#define GPIO_FUNC92_IN_SEL_V  0x000000FFU
#define GPIO_FUNC92_IN_SEL_S  0
/** GPIO_FUNC92_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC92_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC92_IN_INV_SEL_M  (GPIO_FUNC92_IN_INV_SEL_V << GPIO_FUNC92_IN_INV_SEL_S)
#define GPIO_FUNC92_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC92_IN_INV_SEL_S  8
/** GPIO_SIG92_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG92_IN_SEL    (BIT(9))
#define GPIO_SIG92_IN_SEL_M  (GPIO_SIG92_IN_SEL_V << GPIO_SIG92_IN_SEL_S)
#define GPIO_SIG92_IN_SEL_V  0x00000001U
#define GPIO_SIG92_IN_SEL_S  9

/** GPIO_FUNC93_IN_SEL_CFG_REG register
 *  Configuration register for input signal 93
 */
#define GPIO_FUNC93_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x468)
/** GPIO_FUNC93_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 93.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC93_IN_SEL    0x000000FFU
#define GPIO_FUNC93_IN_SEL_M  (GPIO_FUNC93_IN_SEL_V << GPIO_FUNC93_IN_SEL_S)
#define GPIO_FUNC93_IN_SEL_V  0x000000FFU
#define GPIO_FUNC93_IN_SEL_S  0
/** GPIO_FUNC93_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC93_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC93_IN_INV_SEL_M  (GPIO_FUNC93_IN_INV_SEL_V << GPIO_FUNC93_IN_INV_SEL_S)
#define GPIO_FUNC93_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC93_IN_INV_SEL_S  8
/** GPIO_SIG93_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG93_IN_SEL    (BIT(9))
#define GPIO_SIG93_IN_SEL_M  (GPIO_SIG93_IN_SEL_V << GPIO_SIG93_IN_SEL_S)
#define GPIO_SIG93_IN_SEL_V  0x00000001U
#define GPIO_SIG93_IN_SEL_S  9

/** GPIO_FUNC94_IN_SEL_CFG_REG register
 *  Configuration register for input signal 94
 */
#define GPIO_FUNC94_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x46c)
/** GPIO_FUNC94_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 94.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC94_IN_SEL    0x000000FFU
#define GPIO_FUNC94_IN_SEL_M  (GPIO_FUNC94_IN_SEL_V << GPIO_FUNC94_IN_SEL_S)
#define GPIO_FUNC94_IN_SEL_V  0x000000FFU
#define GPIO_FUNC94_IN_SEL_S  0
/** GPIO_FUNC94_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC94_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC94_IN_INV_SEL_M  (GPIO_FUNC94_IN_INV_SEL_V << GPIO_FUNC94_IN_INV_SEL_S)
#define GPIO_FUNC94_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC94_IN_INV_SEL_S  8
/** GPIO_SIG94_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG94_IN_SEL    (BIT(9))
#define GPIO_SIG94_IN_SEL_M  (GPIO_SIG94_IN_SEL_V << GPIO_SIG94_IN_SEL_S)
#define GPIO_SIG94_IN_SEL_V  0x00000001U
#define GPIO_SIG94_IN_SEL_S  9

/** GPIO_FUNC95_IN_SEL_CFG_REG register
 *  Configuration register for input signal 95
 */
#define GPIO_FUNC95_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x470)
/** GPIO_FUNC95_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 95.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC95_IN_SEL    0x000000FFU
#define GPIO_FUNC95_IN_SEL_M  (GPIO_FUNC95_IN_SEL_V << GPIO_FUNC95_IN_SEL_S)
#define GPIO_FUNC95_IN_SEL_V  0x000000FFU
#define GPIO_FUNC95_IN_SEL_S  0
/** GPIO_FUNC95_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC95_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC95_IN_INV_SEL_M  (GPIO_FUNC95_IN_INV_SEL_V << GPIO_FUNC95_IN_INV_SEL_S)
#define GPIO_FUNC95_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC95_IN_INV_SEL_S  8
/** GPIO_SIG95_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG95_IN_SEL    (BIT(9))
#define GPIO_SIG95_IN_SEL_M  (GPIO_SIG95_IN_SEL_V << GPIO_SIG95_IN_SEL_S)
#define GPIO_SIG95_IN_SEL_V  0x00000001U
#define GPIO_SIG95_IN_SEL_S  9

/** GPIO_FUNC96_IN_SEL_CFG_REG register
 *  Configuration register for input signal 96
 */
#define GPIO_FUNC96_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x474)
/** GPIO_FUNC96_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 96.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC96_IN_SEL    0x000000FFU
#define GPIO_FUNC96_IN_SEL_M  (GPIO_FUNC96_IN_SEL_V << GPIO_FUNC96_IN_SEL_S)
#define GPIO_FUNC96_IN_SEL_V  0x000000FFU
#define GPIO_FUNC96_IN_SEL_S  0
/** GPIO_FUNC96_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC96_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC96_IN_INV_SEL_M  (GPIO_FUNC96_IN_INV_SEL_V << GPIO_FUNC96_IN_INV_SEL_S)
#define GPIO_FUNC96_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC96_IN_INV_SEL_S  8
/** GPIO_SIG96_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG96_IN_SEL    (BIT(9))
#define GPIO_SIG96_IN_SEL_M  (GPIO_SIG96_IN_SEL_V << GPIO_SIG96_IN_SEL_S)
#define GPIO_SIG96_IN_SEL_V  0x00000001U
#define GPIO_SIG96_IN_SEL_S  9

/** GPIO_FUNC97_IN_SEL_CFG_REG register
 *  Configuration register for input signal 97
 */
#define GPIO_FUNC97_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x478)
/** GPIO_FUNC97_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 97.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC97_IN_SEL    0x000000FFU
#define GPIO_FUNC97_IN_SEL_M  (GPIO_FUNC97_IN_SEL_V << GPIO_FUNC97_IN_SEL_S)
#define GPIO_FUNC97_IN_SEL_V  0x000000FFU
#define GPIO_FUNC97_IN_SEL_S  0
/** GPIO_FUNC97_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC97_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC97_IN_INV_SEL_M  (GPIO_FUNC97_IN_INV_SEL_V << GPIO_FUNC97_IN_INV_SEL_S)
#define GPIO_FUNC97_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC97_IN_INV_SEL_S  8
/** GPIO_SIG97_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG97_IN_SEL    (BIT(9))
#define GPIO_SIG97_IN_SEL_M  (GPIO_SIG97_IN_SEL_V << GPIO_SIG97_IN_SEL_S)
#define GPIO_SIG97_IN_SEL_V  0x00000001U
#define GPIO_SIG97_IN_SEL_S  9

/** GPIO_FUNC98_IN_SEL_CFG_REG register
 *  Configuration register for input signal 98
 */
#define GPIO_FUNC98_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x47c)
/** GPIO_FUNC98_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 98.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC98_IN_SEL    0x000000FFU
#define GPIO_FUNC98_IN_SEL_M  (GPIO_FUNC98_IN_SEL_V << GPIO_FUNC98_IN_SEL_S)
#define GPIO_FUNC98_IN_SEL_V  0x000000FFU
#define GPIO_FUNC98_IN_SEL_S  0
/** GPIO_FUNC98_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC98_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC98_IN_INV_SEL_M  (GPIO_FUNC98_IN_INV_SEL_V << GPIO_FUNC98_IN_INV_SEL_S)
#define GPIO_FUNC98_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC98_IN_INV_SEL_S  8
/** GPIO_SIG98_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG98_IN_SEL    (BIT(9))
#define GPIO_SIG98_IN_SEL_M  (GPIO_SIG98_IN_SEL_V << GPIO_SIG98_IN_SEL_S)
#define GPIO_SIG98_IN_SEL_V  0x00000001U
#define GPIO_SIG98_IN_SEL_S  9

/** GPIO_FUNC99_IN_SEL_CFG_REG register
 *  Configuration register for input signal 99
 */
#define GPIO_FUNC99_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x480)
/** GPIO_FUNC99_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 99.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC99_IN_SEL    0x000000FFU
#define GPIO_FUNC99_IN_SEL_M  (GPIO_FUNC99_IN_SEL_V << GPIO_FUNC99_IN_SEL_S)
#define GPIO_FUNC99_IN_SEL_V  0x000000FFU
#define GPIO_FUNC99_IN_SEL_S  0
/** GPIO_FUNC99_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC99_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC99_IN_INV_SEL_M  (GPIO_FUNC99_IN_INV_SEL_V << GPIO_FUNC99_IN_INV_SEL_S)
#define GPIO_FUNC99_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC99_IN_INV_SEL_S  8
/** GPIO_SIG99_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG99_IN_SEL    (BIT(9))
#define GPIO_SIG99_IN_SEL_M  (GPIO_SIG99_IN_SEL_V << GPIO_SIG99_IN_SEL_S)
#define GPIO_SIG99_IN_SEL_V  0x00000001U
#define GPIO_SIG99_IN_SEL_S  9

/** GPIO_FUNC100_IN_SEL_CFG_REG register
 *  Configuration register for input signal 100
 */
#define GPIO_FUNC100_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x484)
/** GPIO_FUNC100_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 100.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC100_IN_SEL    0x000000FFU
#define GPIO_FUNC100_IN_SEL_M  (GPIO_FUNC100_IN_SEL_V << GPIO_FUNC100_IN_SEL_S)
#define GPIO_FUNC100_IN_SEL_V  0x000000FFU
#define GPIO_FUNC100_IN_SEL_S  0
/** GPIO_FUNC100_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC100_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC100_IN_INV_SEL_M  (GPIO_FUNC100_IN_INV_SEL_V << GPIO_FUNC100_IN_INV_SEL_S)
#define GPIO_FUNC100_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC100_IN_INV_SEL_S  8
/** GPIO_SIG100_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG100_IN_SEL    (BIT(9))
#define GPIO_SIG100_IN_SEL_M  (GPIO_SIG100_IN_SEL_V << GPIO_SIG100_IN_SEL_S)
#define GPIO_SIG100_IN_SEL_V  0x00000001U
#define GPIO_SIG100_IN_SEL_S  9

/** GPIO_FUNC101_IN_SEL_CFG_REG register
 *  Configuration register for input signal 101
 */
#define GPIO_FUNC101_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x488)
/** GPIO_FUNC101_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 101.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC101_IN_SEL    0x000000FFU
#define GPIO_FUNC101_IN_SEL_M  (GPIO_FUNC101_IN_SEL_V << GPIO_FUNC101_IN_SEL_S)
#define GPIO_FUNC101_IN_SEL_V  0x000000FFU
#define GPIO_FUNC101_IN_SEL_S  0
/** GPIO_FUNC101_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC101_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC101_IN_INV_SEL_M  (GPIO_FUNC101_IN_INV_SEL_V << GPIO_FUNC101_IN_INV_SEL_S)
#define GPIO_FUNC101_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC101_IN_INV_SEL_S  8
/** GPIO_SIG101_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG101_IN_SEL    (BIT(9))
#define GPIO_SIG101_IN_SEL_M  (GPIO_SIG101_IN_SEL_V << GPIO_SIG101_IN_SEL_S)
#define GPIO_SIG101_IN_SEL_V  0x00000001U
#define GPIO_SIG101_IN_SEL_S  9

/** GPIO_FUNC102_IN_SEL_CFG_REG register
 *  Configuration register for input signal 102
 */
#define GPIO_FUNC102_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x48c)
/** GPIO_FUNC102_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 102.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC102_IN_SEL    0x000000FFU
#define GPIO_FUNC102_IN_SEL_M  (GPIO_FUNC102_IN_SEL_V << GPIO_FUNC102_IN_SEL_S)
#define GPIO_FUNC102_IN_SEL_V  0x000000FFU
#define GPIO_FUNC102_IN_SEL_S  0
/** GPIO_FUNC102_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC102_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC102_IN_INV_SEL_M  (GPIO_FUNC102_IN_INV_SEL_V << GPIO_FUNC102_IN_INV_SEL_S)
#define GPIO_FUNC102_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC102_IN_INV_SEL_S  8
/** GPIO_SIG102_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG102_IN_SEL    (BIT(9))
#define GPIO_SIG102_IN_SEL_M  (GPIO_SIG102_IN_SEL_V << GPIO_SIG102_IN_SEL_S)
#define GPIO_SIG102_IN_SEL_V  0x00000001U
#define GPIO_SIG102_IN_SEL_S  9

/** GPIO_FUNC103_IN_SEL_CFG_REG register
 *  Configuration register for input signal 103
 */
#define GPIO_FUNC103_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x490)
/** GPIO_FUNC103_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 103.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC103_IN_SEL    0x000000FFU
#define GPIO_FUNC103_IN_SEL_M  (GPIO_FUNC103_IN_SEL_V << GPIO_FUNC103_IN_SEL_S)
#define GPIO_FUNC103_IN_SEL_V  0x000000FFU
#define GPIO_FUNC103_IN_SEL_S  0
/** GPIO_FUNC103_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC103_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC103_IN_INV_SEL_M  (GPIO_FUNC103_IN_INV_SEL_V << GPIO_FUNC103_IN_INV_SEL_S)
#define GPIO_FUNC103_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC103_IN_INV_SEL_S  8
/** GPIO_SIG103_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG103_IN_SEL    (BIT(9))
#define GPIO_SIG103_IN_SEL_M  (GPIO_SIG103_IN_SEL_V << GPIO_SIG103_IN_SEL_S)
#define GPIO_SIG103_IN_SEL_V  0x00000001U
#define GPIO_SIG103_IN_SEL_S  9

/** GPIO_FUNC104_IN_SEL_CFG_REG register
 *  Configuration register for input signal 104
 */
#define GPIO_FUNC104_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x494)
/** GPIO_FUNC104_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 104.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC104_IN_SEL    0x000000FFU
#define GPIO_FUNC104_IN_SEL_M  (GPIO_FUNC104_IN_SEL_V << GPIO_FUNC104_IN_SEL_S)
#define GPIO_FUNC104_IN_SEL_V  0x000000FFU
#define GPIO_FUNC104_IN_SEL_S  0
/** GPIO_FUNC104_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC104_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC104_IN_INV_SEL_M  (GPIO_FUNC104_IN_INV_SEL_V << GPIO_FUNC104_IN_INV_SEL_S)
#define GPIO_FUNC104_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC104_IN_INV_SEL_S  8
/** GPIO_SIG104_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG104_IN_SEL    (BIT(9))
#define GPIO_SIG104_IN_SEL_M  (GPIO_SIG104_IN_SEL_V << GPIO_SIG104_IN_SEL_S)
#define GPIO_SIG104_IN_SEL_V  0x00000001U
#define GPIO_SIG104_IN_SEL_S  9

/** GPIO_FUNC105_IN_SEL_CFG_REG register
 *  Configuration register for input signal 105
 */
#define GPIO_FUNC105_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x498)
/** GPIO_FUNC105_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 105.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC105_IN_SEL    0x000000FFU
#define GPIO_FUNC105_IN_SEL_M  (GPIO_FUNC105_IN_SEL_V << GPIO_FUNC105_IN_SEL_S)
#define GPIO_FUNC105_IN_SEL_V  0x000000FFU
#define GPIO_FUNC105_IN_SEL_S  0
/** GPIO_FUNC105_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC105_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC105_IN_INV_SEL_M  (GPIO_FUNC105_IN_INV_SEL_V << GPIO_FUNC105_IN_INV_SEL_S)
#define GPIO_FUNC105_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC105_IN_INV_SEL_S  8
/** GPIO_SIG105_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG105_IN_SEL    (BIT(9))
#define GPIO_SIG105_IN_SEL_M  (GPIO_SIG105_IN_SEL_V << GPIO_SIG105_IN_SEL_S)
#define GPIO_SIG105_IN_SEL_V  0x00000001U
#define GPIO_SIG105_IN_SEL_S  9

/** GPIO_FUNC106_IN_SEL_CFG_REG register
 *  Configuration register for input signal 106
 */
#define GPIO_FUNC106_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x49c)
/** GPIO_FUNC106_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 106.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC106_IN_SEL    0x000000FFU
#define GPIO_FUNC106_IN_SEL_M  (GPIO_FUNC106_IN_SEL_V << GPIO_FUNC106_IN_SEL_S)
#define GPIO_FUNC106_IN_SEL_V  0x000000FFU
#define GPIO_FUNC106_IN_SEL_S  0
/** GPIO_FUNC106_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC106_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC106_IN_INV_SEL_M  (GPIO_FUNC106_IN_INV_SEL_V << GPIO_FUNC106_IN_INV_SEL_S)
#define GPIO_FUNC106_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC106_IN_INV_SEL_S  8
/** GPIO_SIG106_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG106_IN_SEL    (BIT(9))
#define GPIO_SIG106_IN_SEL_M  (GPIO_SIG106_IN_SEL_V << GPIO_SIG106_IN_SEL_S)
#define GPIO_SIG106_IN_SEL_V  0x00000001U
#define GPIO_SIG106_IN_SEL_S  9

/** GPIO_FUNC107_IN_SEL_CFG_REG register
 *  Configuration register for input signal 107
 */
#define GPIO_FUNC107_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4a0)
/** GPIO_FUNC107_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 107.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC107_IN_SEL    0x000000FFU
#define GPIO_FUNC107_IN_SEL_M  (GPIO_FUNC107_IN_SEL_V << GPIO_FUNC107_IN_SEL_S)
#define GPIO_FUNC107_IN_SEL_V  0x000000FFU
#define GPIO_FUNC107_IN_SEL_S  0
/** GPIO_FUNC107_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC107_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC107_IN_INV_SEL_M  (GPIO_FUNC107_IN_INV_SEL_V << GPIO_FUNC107_IN_INV_SEL_S)
#define GPIO_FUNC107_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC107_IN_INV_SEL_S  8
/** GPIO_SIG107_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG107_IN_SEL    (BIT(9))
#define GPIO_SIG107_IN_SEL_M  (GPIO_SIG107_IN_SEL_V << GPIO_SIG107_IN_SEL_S)
#define GPIO_SIG107_IN_SEL_V  0x00000001U
#define GPIO_SIG107_IN_SEL_S  9

/** GPIO_FUNC108_IN_SEL_CFG_REG register
 *  Configuration register for input signal 108
 */
#define GPIO_FUNC108_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4a4)
/** GPIO_FUNC108_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 108.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC108_IN_SEL    0x000000FFU
#define GPIO_FUNC108_IN_SEL_M  (GPIO_FUNC108_IN_SEL_V << GPIO_FUNC108_IN_SEL_S)
#define GPIO_FUNC108_IN_SEL_V  0x000000FFU
#define GPIO_FUNC108_IN_SEL_S  0
/** GPIO_FUNC108_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC108_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC108_IN_INV_SEL_M  (GPIO_FUNC108_IN_INV_SEL_V << GPIO_FUNC108_IN_INV_SEL_S)
#define GPIO_FUNC108_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC108_IN_INV_SEL_S  8
/** GPIO_SIG108_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG108_IN_SEL    (BIT(9))
#define GPIO_SIG108_IN_SEL_M  (GPIO_SIG108_IN_SEL_V << GPIO_SIG108_IN_SEL_S)
#define GPIO_SIG108_IN_SEL_V  0x00000001U
#define GPIO_SIG108_IN_SEL_S  9

/** GPIO_FUNC109_IN_SEL_CFG_REG register
 *  Configuration register for input signal 109
 */
#define GPIO_FUNC109_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4a8)
/** GPIO_FUNC109_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 109.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC109_IN_SEL    0x000000FFU
#define GPIO_FUNC109_IN_SEL_M  (GPIO_FUNC109_IN_SEL_V << GPIO_FUNC109_IN_SEL_S)
#define GPIO_FUNC109_IN_SEL_V  0x000000FFU
#define GPIO_FUNC109_IN_SEL_S  0
/** GPIO_FUNC109_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC109_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC109_IN_INV_SEL_M  (GPIO_FUNC109_IN_INV_SEL_V << GPIO_FUNC109_IN_INV_SEL_S)
#define GPIO_FUNC109_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC109_IN_INV_SEL_S  8
/** GPIO_SIG109_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG109_IN_SEL    (BIT(9))
#define GPIO_SIG109_IN_SEL_M  (GPIO_SIG109_IN_SEL_V << GPIO_SIG109_IN_SEL_S)
#define GPIO_SIG109_IN_SEL_V  0x00000001U
#define GPIO_SIG109_IN_SEL_S  9

/** GPIO_FUNC110_IN_SEL_CFG_REG register
 *  Configuration register for input signal 110
 */
#define GPIO_FUNC110_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4ac)
/** GPIO_FUNC110_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 110.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC110_IN_SEL    0x000000FFU
#define GPIO_FUNC110_IN_SEL_M  (GPIO_FUNC110_IN_SEL_V << GPIO_FUNC110_IN_SEL_S)
#define GPIO_FUNC110_IN_SEL_V  0x000000FFU
#define GPIO_FUNC110_IN_SEL_S  0
/** GPIO_FUNC110_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC110_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC110_IN_INV_SEL_M  (GPIO_FUNC110_IN_INV_SEL_V << GPIO_FUNC110_IN_INV_SEL_S)
#define GPIO_FUNC110_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC110_IN_INV_SEL_S  8
/** GPIO_SIG110_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG110_IN_SEL    (BIT(9))
#define GPIO_SIG110_IN_SEL_M  (GPIO_SIG110_IN_SEL_V << GPIO_SIG110_IN_SEL_S)
#define GPIO_SIG110_IN_SEL_V  0x00000001U
#define GPIO_SIG110_IN_SEL_S  9

/** GPIO_FUNC111_IN_SEL_CFG_REG register
 *  Configuration register for input signal 111
 */
#define GPIO_FUNC111_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b0)
/** GPIO_FUNC111_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 111.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC111_IN_SEL    0x000000FFU
#define GPIO_FUNC111_IN_SEL_M  (GPIO_FUNC111_IN_SEL_V << GPIO_FUNC111_IN_SEL_S)
#define GPIO_FUNC111_IN_SEL_V  0x000000FFU
#define GPIO_FUNC111_IN_SEL_S  0
/** GPIO_FUNC111_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC111_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC111_IN_INV_SEL_M  (GPIO_FUNC111_IN_INV_SEL_V << GPIO_FUNC111_IN_INV_SEL_S)
#define GPIO_FUNC111_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC111_IN_INV_SEL_S  8
/** GPIO_SIG111_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG111_IN_SEL    (BIT(9))
#define GPIO_SIG111_IN_SEL_M  (GPIO_SIG111_IN_SEL_V << GPIO_SIG111_IN_SEL_S)
#define GPIO_SIG111_IN_SEL_V  0x00000001U
#define GPIO_SIG111_IN_SEL_S  9

/** GPIO_FUNC112_IN_SEL_CFG_REG register
 *  Configuration register for input signal 112
 */
#define GPIO_FUNC112_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b4)
/** GPIO_FUNC112_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 112.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC112_IN_SEL    0x000000FFU
#define GPIO_FUNC112_IN_SEL_M  (GPIO_FUNC112_IN_SEL_V << GPIO_FUNC112_IN_SEL_S)
#define GPIO_FUNC112_IN_SEL_V  0x000000FFU
#define GPIO_FUNC112_IN_SEL_S  0
/** GPIO_FUNC112_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC112_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC112_IN_INV_SEL_M  (GPIO_FUNC112_IN_INV_SEL_V << GPIO_FUNC112_IN_INV_SEL_S)
#define GPIO_FUNC112_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC112_IN_INV_SEL_S  8
/** GPIO_SIG112_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG112_IN_SEL    (BIT(9))
#define GPIO_SIG112_IN_SEL_M  (GPIO_SIG112_IN_SEL_V << GPIO_SIG112_IN_SEL_S)
#define GPIO_SIG112_IN_SEL_V  0x00000001U
#define GPIO_SIG112_IN_SEL_S  9

/** GPIO_FUNC113_IN_SEL_CFG_REG register
 *  Configuration register for input signal 113
 */
#define GPIO_FUNC113_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4b8)
/** GPIO_FUNC113_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 113.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC113_IN_SEL    0x000000FFU
#define GPIO_FUNC113_IN_SEL_M  (GPIO_FUNC113_IN_SEL_V << GPIO_FUNC113_IN_SEL_S)
#define GPIO_FUNC113_IN_SEL_V  0x000000FFU
#define GPIO_FUNC113_IN_SEL_S  0
/** GPIO_FUNC113_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC113_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC113_IN_INV_SEL_M  (GPIO_FUNC113_IN_INV_SEL_V << GPIO_FUNC113_IN_INV_SEL_S)
#define GPIO_FUNC113_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC113_IN_INV_SEL_S  8
/** GPIO_SIG113_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG113_IN_SEL    (BIT(9))
#define GPIO_SIG113_IN_SEL_M  (GPIO_SIG113_IN_SEL_V << GPIO_SIG113_IN_SEL_S)
#define GPIO_SIG113_IN_SEL_V  0x00000001U
#define GPIO_SIG113_IN_SEL_S  9

/** GPIO_FUNC114_IN_SEL_CFG_REG register
 *  Configuration register for input signal 114
 */
#define GPIO_FUNC114_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4bc)
/** GPIO_FUNC114_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 114.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC114_IN_SEL    0x000000FFU
#define GPIO_FUNC114_IN_SEL_M  (GPIO_FUNC114_IN_SEL_V << GPIO_FUNC114_IN_SEL_S)
#define GPIO_FUNC114_IN_SEL_V  0x000000FFU
#define GPIO_FUNC114_IN_SEL_S  0
/** GPIO_FUNC114_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC114_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC114_IN_INV_SEL_M  (GPIO_FUNC114_IN_INV_SEL_V << GPIO_FUNC114_IN_INV_SEL_S)
#define GPIO_FUNC114_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC114_IN_INV_SEL_S  8
/** GPIO_SIG114_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG114_IN_SEL    (BIT(9))
#define GPIO_SIG114_IN_SEL_M  (GPIO_SIG114_IN_SEL_V << GPIO_SIG114_IN_SEL_S)
#define GPIO_SIG114_IN_SEL_V  0x00000001U
#define GPIO_SIG114_IN_SEL_S  9

/** GPIO_FUNC115_IN_SEL_CFG_REG register
 *  Configuration register for input signal 115
 */
#define GPIO_FUNC115_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4c0)
/** GPIO_FUNC115_IN_SEL : R/W; bitpos: [7:0]; default: 128;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 115.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC115_IN_SEL    0x000000FFU
#define GPIO_FUNC115_IN_SEL_M  (GPIO_FUNC115_IN_SEL_V << GPIO_FUNC115_IN_SEL_S)
#define GPIO_FUNC115_IN_SEL_V  0x000000FFU
#define GPIO_FUNC115_IN_SEL_S  0
/** GPIO_FUNC115_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC115_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC115_IN_INV_SEL_M  (GPIO_FUNC115_IN_INV_SEL_V << GPIO_FUNC115_IN_INV_SEL_S)
#define GPIO_FUNC115_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC115_IN_INV_SEL_S  8
/** GPIO_SIG115_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG115_IN_SEL    (BIT(9))
#define GPIO_SIG115_IN_SEL_M  (GPIO_SIG115_IN_SEL_V << GPIO_SIG115_IN_SEL_S)
#define GPIO_SIG115_IN_SEL_V  0x00000001U
#define GPIO_SIG115_IN_SEL_S  9

/** GPIO_FUNC118_IN_SEL_CFG_REG register
 *  Configuration register for input signal 118
 */
#define GPIO_FUNC118_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4cc)
/** GPIO_FUNC118_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 118.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC118_IN_SEL    0x000000FFU
#define GPIO_FUNC118_IN_SEL_M  (GPIO_FUNC118_IN_SEL_V << GPIO_FUNC118_IN_SEL_S)
#define GPIO_FUNC118_IN_SEL_V  0x000000FFU
#define GPIO_FUNC118_IN_SEL_S  0
/** GPIO_FUNC118_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC118_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC118_IN_INV_SEL_M  (GPIO_FUNC118_IN_INV_SEL_V << GPIO_FUNC118_IN_INV_SEL_S)
#define GPIO_FUNC118_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC118_IN_INV_SEL_S  8
/** GPIO_SIG118_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG118_IN_SEL    (BIT(9))
#define GPIO_SIG118_IN_SEL_M  (GPIO_SIG118_IN_SEL_V << GPIO_SIG118_IN_SEL_S)
#define GPIO_SIG118_IN_SEL_V  0x00000001U
#define GPIO_SIG118_IN_SEL_S  9

/** GPIO_FUNC119_IN_SEL_CFG_REG register
 *  Configuration register for input signal 119
 */
#define GPIO_FUNC119_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4d0)
/** GPIO_FUNC119_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 119.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC119_IN_SEL    0x000000FFU
#define GPIO_FUNC119_IN_SEL_M  (GPIO_FUNC119_IN_SEL_V << GPIO_FUNC119_IN_SEL_S)
#define GPIO_FUNC119_IN_SEL_V  0x000000FFU
#define GPIO_FUNC119_IN_SEL_S  0
/** GPIO_FUNC119_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC119_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC119_IN_INV_SEL_M  (GPIO_FUNC119_IN_INV_SEL_V << GPIO_FUNC119_IN_INV_SEL_S)
#define GPIO_FUNC119_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC119_IN_INV_SEL_S  8
/** GPIO_SIG119_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG119_IN_SEL    (BIT(9))
#define GPIO_SIG119_IN_SEL_M  (GPIO_SIG119_IN_SEL_V << GPIO_SIG119_IN_SEL_S)
#define GPIO_SIG119_IN_SEL_V  0x00000001U
#define GPIO_SIG119_IN_SEL_S  9

/** GPIO_FUNC120_IN_SEL_CFG_REG register
 *  Configuration register for input signal 120
 */
#define GPIO_FUNC120_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4d4)
/** GPIO_FUNC120_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 120.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC120_IN_SEL    0x000000FFU
#define GPIO_FUNC120_IN_SEL_M  (GPIO_FUNC120_IN_SEL_V << GPIO_FUNC120_IN_SEL_S)
#define GPIO_FUNC120_IN_SEL_V  0x000000FFU
#define GPIO_FUNC120_IN_SEL_S  0
/** GPIO_FUNC120_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC120_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC120_IN_INV_SEL_M  (GPIO_FUNC120_IN_INV_SEL_V << GPIO_FUNC120_IN_INV_SEL_S)
#define GPIO_FUNC120_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC120_IN_INV_SEL_S  8
/** GPIO_SIG120_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG120_IN_SEL    (BIT(9))
#define GPIO_SIG120_IN_SEL_M  (GPIO_SIG120_IN_SEL_V << GPIO_SIG120_IN_SEL_S)
#define GPIO_SIG120_IN_SEL_V  0x00000001U
#define GPIO_SIG120_IN_SEL_S  9

/** GPIO_FUNC121_IN_SEL_CFG_REG register
 *  Configuration register for input signal 121
 */
#define GPIO_FUNC121_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4d8)
/** GPIO_FUNC121_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 121.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC121_IN_SEL    0x000000FFU
#define GPIO_FUNC121_IN_SEL_M  (GPIO_FUNC121_IN_SEL_V << GPIO_FUNC121_IN_SEL_S)
#define GPIO_FUNC121_IN_SEL_V  0x000000FFU
#define GPIO_FUNC121_IN_SEL_S  0
/** GPIO_FUNC121_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC121_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC121_IN_INV_SEL_M  (GPIO_FUNC121_IN_INV_SEL_V << GPIO_FUNC121_IN_INV_SEL_S)
#define GPIO_FUNC121_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC121_IN_INV_SEL_S  8
/** GPIO_SIG121_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG121_IN_SEL    (BIT(9))
#define GPIO_SIG121_IN_SEL_M  (GPIO_SIG121_IN_SEL_V << GPIO_SIG121_IN_SEL_S)
#define GPIO_SIG121_IN_SEL_V  0x00000001U
#define GPIO_SIG121_IN_SEL_S  9

/** GPIO_FUNC122_IN_SEL_CFG_REG register
 *  Configuration register for input signal 122
 */
#define GPIO_FUNC122_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4dc)
/** GPIO_FUNC122_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 122.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC122_IN_SEL    0x000000FFU
#define GPIO_FUNC122_IN_SEL_M  (GPIO_FUNC122_IN_SEL_V << GPIO_FUNC122_IN_SEL_S)
#define GPIO_FUNC122_IN_SEL_V  0x000000FFU
#define GPIO_FUNC122_IN_SEL_S  0
/** GPIO_FUNC122_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC122_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC122_IN_INV_SEL_M  (GPIO_FUNC122_IN_INV_SEL_V << GPIO_FUNC122_IN_INV_SEL_S)
#define GPIO_FUNC122_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC122_IN_INV_SEL_S  8
/** GPIO_SIG122_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG122_IN_SEL    (BIT(9))
#define GPIO_SIG122_IN_SEL_M  (GPIO_SIG122_IN_SEL_V << GPIO_SIG122_IN_SEL_S)
#define GPIO_SIG122_IN_SEL_V  0x00000001U
#define GPIO_SIG122_IN_SEL_S  9

/** GPIO_FUNC123_IN_SEL_CFG_REG register
 *  Configuration register for input signal 123
 */
#define GPIO_FUNC123_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4e0)
/** GPIO_FUNC123_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 123.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC123_IN_SEL    0x000000FFU
#define GPIO_FUNC123_IN_SEL_M  (GPIO_FUNC123_IN_SEL_V << GPIO_FUNC123_IN_SEL_S)
#define GPIO_FUNC123_IN_SEL_V  0x000000FFU
#define GPIO_FUNC123_IN_SEL_S  0
/** GPIO_FUNC123_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC123_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC123_IN_INV_SEL_M  (GPIO_FUNC123_IN_INV_SEL_V << GPIO_FUNC123_IN_INV_SEL_S)
#define GPIO_FUNC123_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC123_IN_INV_SEL_S  8
/** GPIO_SIG123_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG123_IN_SEL    (BIT(9))
#define GPIO_SIG123_IN_SEL_M  (GPIO_SIG123_IN_SEL_V << GPIO_SIG123_IN_SEL_S)
#define GPIO_SIG123_IN_SEL_V  0x00000001U
#define GPIO_SIG123_IN_SEL_S  9

/** GPIO_FUNC124_IN_SEL_CFG_REG register
 *  Configuration register for input signal 124
 */
#define GPIO_FUNC124_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4e4)
/** GPIO_FUNC124_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 124.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC124_IN_SEL    0x000000FFU
#define GPIO_FUNC124_IN_SEL_M  (GPIO_FUNC124_IN_SEL_V << GPIO_FUNC124_IN_SEL_S)
#define GPIO_FUNC124_IN_SEL_V  0x000000FFU
#define GPIO_FUNC124_IN_SEL_S  0
/** GPIO_FUNC124_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC124_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC124_IN_INV_SEL_M  (GPIO_FUNC124_IN_INV_SEL_V << GPIO_FUNC124_IN_INV_SEL_S)
#define GPIO_FUNC124_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC124_IN_INV_SEL_S  8
/** GPIO_SIG124_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG124_IN_SEL    (BIT(9))
#define GPIO_SIG124_IN_SEL_M  (GPIO_SIG124_IN_SEL_V << GPIO_SIG124_IN_SEL_S)
#define GPIO_SIG124_IN_SEL_V  0x00000001U
#define GPIO_SIG124_IN_SEL_S  9

/** GPIO_FUNC125_IN_SEL_CFG_REG register
 *  Configuration register for input signal 125
 */
#define GPIO_FUNC125_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4e8)
/** GPIO_FUNC125_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 125.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC125_IN_SEL    0x000000FFU
#define GPIO_FUNC125_IN_SEL_M  (GPIO_FUNC125_IN_SEL_V << GPIO_FUNC125_IN_SEL_S)
#define GPIO_FUNC125_IN_SEL_V  0x000000FFU
#define GPIO_FUNC125_IN_SEL_S  0
/** GPIO_FUNC125_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC125_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC125_IN_INV_SEL_M  (GPIO_FUNC125_IN_INV_SEL_V << GPIO_FUNC125_IN_INV_SEL_S)
#define GPIO_FUNC125_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC125_IN_INV_SEL_S  8
/** GPIO_SIG125_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG125_IN_SEL    (BIT(9))
#define GPIO_SIG125_IN_SEL_M  (GPIO_SIG125_IN_SEL_V << GPIO_SIG125_IN_SEL_S)
#define GPIO_SIG125_IN_SEL_V  0x00000001U
#define GPIO_SIG125_IN_SEL_S  9

/** GPIO_FUNC126_IN_SEL_CFG_REG register
 *  Configuration register for input signal 126
 */
#define GPIO_FUNC126_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4ec)
/** GPIO_FUNC126_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 126.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC126_IN_SEL    0x000000FFU
#define GPIO_FUNC126_IN_SEL_M  (GPIO_FUNC126_IN_SEL_V << GPIO_FUNC126_IN_SEL_S)
#define GPIO_FUNC126_IN_SEL_V  0x000000FFU
#define GPIO_FUNC126_IN_SEL_S  0
/** GPIO_FUNC126_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC126_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC126_IN_INV_SEL_M  (GPIO_FUNC126_IN_INV_SEL_V << GPIO_FUNC126_IN_INV_SEL_S)
#define GPIO_FUNC126_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC126_IN_INV_SEL_S  8
/** GPIO_SIG126_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG126_IN_SEL    (BIT(9))
#define GPIO_SIG126_IN_SEL_M  (GPIO_SIG126_IN_SEL_V << GPIO_SIG126_IN_SEL_S)
#define GPIO_SIG126_IN_SEL_V  0x00000001U
#define GPIO_SIG126_IN_SEL_S  9

/** GPIO_FUNC127_IN_SEL_CFG_REG register
 *  Configuration register for input signal 127
 */
#define GPIO_FUNC127_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4f0)
/** GPIO_FUNC127_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 127.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC127_IN_SEL    0x000000FFU
#define GPIO_FUNC127_IN_SEL_M  (GPIO_FUNC127_IN_SEL_V << GPIO_FUNC127_IN_SEL_S)
#define GPIO_FUNC127_IN_SEL_V  0x000000FFU
#define GPIO_FUNC127_IN_SEL_S  0
/** GPIO_FUNC127_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC127_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC127_IN_INV_SEL_M  (GPIO_FUNC127_IN_INV_SEL_V << GPIO_FUNC127_IN_INV_SEL_S)
#define GPIO_FUNC127_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC127_IN_INV_SEL_S  8
/** GPIO_SIG127_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG127_IN_SEL    (BIT(9))
#define GPIO_SIG127_IN_SEL_M  (GPIO_SIG127_IN_SEL_V << GPIO_SIG127_IN_SEL_S)
#define GPIO_SIG127_IN_SEL_V  0x00000001U
#define GPIO_SIG127_IN_SEL_S  9

/** GPIO_FUNC128_IN_SEL_CFG_REG register
 *  Configuration register for input signal 128
 */
#define GPIO_FUNC128_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4f4)
/** GPIO_FUNC128_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 128.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC128_IN_SEL    0x000000FFU
#define GPIO_FUNC128_IN_SEL_M  (GPIO_FUNC128_IN_SEL_V << GPIO_FUNC128_IN_SEL_S)
#define GPIO_FUNC128_IN_SEL_V  0x000000FFU
#define GPIO_FUNC128_IN_SEL_S  0
/** GPIO_FUNC128_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC128_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC128_IN_INV_SEL_M  (GPIO_FUNC128_IN_INV_SEL_V << GPIO_FUNC128_IN_INV_SEL_S)
#define GPIO_FUNC128_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC128_IN_INV_SEL_S  8
/** GPIO_SIG128_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG128_IN_SEL    (BIT(9))
#define GPIO_SIG128_IN_SEL_M  (GPIO_SIG128_IN_SEL_V << GPIO_SIG128_IN_SEL_S)
#define GPIO_SIG128_IN_SEL_V  0x00000001U
#define GPIO_SIG128_IN_SEL_S  9

/** GPIO_FUNC129_IN_SEL_CFG_REG register
 *  Configuration register for input signal 129
 */
#define GPIO_FUNC129_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4f8)
/** GPIO_FUNC129_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 129.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC129_IN_SEL    0x000000FFU
#define GPIO_FUNC129_IN_SEL_M  (GPIO_FUNC129_IN_SEL_V << GPIO_FUNC129_IN_SEL_S)
#define GPIO_FUNC129_IN_SEL_V  0x000000FFU
#define GPIO_FUNC129_IN_SEL_S  0
/** GPIO_FUNC129_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC129_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC129_IN_INV_SEL_M  (GPIO_FUNC129_IN_INV_SEL_V << GPIO_FUNC129_IN_INV_SEL_S)
#define GPIO_FUNC129_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC129_IN_INV_SEL_S  8
/** GPIO_SIG129_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG129_IN_SEL    (BIT(9))
#define GPIO_SIG129_IN_SEL_M  (GPIO_SIG129_IN_SEL_V << GPIO_SIG129_IN_SEL_S)
#define GPIO_SIG129_IN_SEL_V  0x00000001U
#define GPIO_SIG129_IN_SEL_S  9

/** GPIO_FUNC130_IN_SEL_CFG_REG register
 *  Configuration register for input signal 130
 */
#define GPIO_FUNC130_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4fc)
/** GPIO_FUNC130_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 130.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC130_IN_SEL    0x000000FFU
#define GPIO_FUNC130_IN_SEL_M  (GPIO_FUNC130_IN_SEL_V << GPIO_FUNC130_IN_SEL_S)
#define GPIO_FUNC130_IN_SEL_V  0x000000FFU
#define GPIO_FUNC130_IN_SEL_S  0
/** GPIO_FUNC130_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC130_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC130_IN_INV_SEL_M  (GPIO_FUNC130_IN_INV_SEL_V << GPIO_FUNC130_IN_INV_SEL_S)
#define GPIO_FUNC130_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC130_IN_INV_SEL_S  8
/** GPIO_SIG130_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG130_IN_SEL    (BIT(9))
#define GPIO_SIG130_IN_SEL_M  (GPIO_SIG130_IN_SEL_V << GPIO_SIG130_IN_SEL_S)
#define GPIO_SIG130_IN_SEL_V  0x00000001U
#define GPIO_SIG130_IN_SEL_S  9

/** GPIO_FUNC131_IN_SEL_CFG_REG register
 *  Configuration register for input signal 131
 */
#define GPIO_FUNC131_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x500)
/** GPIO_FUNC131_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 131.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC131_IN_SEL    0x000000FFU
#define GPIO_FUNC131_IN_SEL_M  (GPIO_FUNC131_IN_SEL_V << GPIO_FUNC131_IN_SEL_S)
#define GPIO_FUNC131_IN_SEL_V  0x000000FFU
#define GPIO_FUNC131_IN_SEL_S  0
/** GPIO_FUNC131_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC131_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC131_IN_INV_SEL_M  (GPIO_FUNC131_IN_INV_SEL_V << GPIO_FUNC131_IN_INV_SEL_S)
#define GPIO_FUNC131_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC131_IN_INV_SEL_S  8
/** GPIO_SIG131_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG131_IN_SEL    (BIT(9))
#define GPIO_SIG131_IN_SEL_M  (GPIO_SIG131_IN_SEL_V << GPIO_SIG131_IN_SEL_S)
#define GPIO_SIG131_IN_SEL_V  0x00000001U
#define GPIO_SIG131_IN_SEL_S  9

/** GPIO_FUNC132_IN_SEL_CFG_REG register
 *  Configuration register for input signal 132
 */
#define GPIO_FUNC132_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x504)
/** GPIO_FUNC132_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 132.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC132_IN_SEL    0x000000FFU
#define GPIO_FUNC132_IN_SEL_M  (GPIO_FUNC132_IN_SEL_V << GPIO_FUNC132_IN_SEL_S)
#define GPIO_FUNC132_IN_SEL_V  0x000000FFU
#define GPIO_FUNC132_IN_SEL_S  0
/** GPIO_FUNC132_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC132_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC132_IN_INV_SEL_M  (GPIO_FUNC132_IN_INV_SEL_V << GPIO_FUNC132_IN_INV_SEL_S)
#define GPIO_FUNC132_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC132_IN_INV_SEL_S  8
/** GPIO_SIG132_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG132_IN_SEL    (BIT(9))
#define GPIO_SIG132_IN_SEL_M  (GPIO_SIG132_IN_SEL_V << GPIO_SIG132_IN_SEL_S)
#define GPIO_SIG132_IN_SEL_V  0x00000001U
#define GPIO_SIG132_IN_SEL_S  9

/** GPIO_FUNC133_IN_SEL_CFG_REG register
 *  Configuration register for input signal 133
 */
#define GPIO_FUNC133_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x508)
/** GPIO_FUNC133_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 133.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC133_IN_SEL    0x000000FFU
#define GPIO_FUNC133_IN_SEL_M  (GPIO_FUNC133_IN_SEL_V << GPIO_FUNC133_IN_SEL_S)
#define GPIO_FUNC133_IN_SEL_V  0x000000FFU
#define GPIO_FUNC133_IN_SEL_S  0
/** GPIO_FUNC133_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC133_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC133_IN_INV_SEL_M  (GPIO_FUNC133_IN_INV_SEL_V << GPIO_FUNC133_IN_INV_SEL_S)
#define GPIO_FUNC133_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC133_IN_INV_SEL_S  8
/** GPIO_SIG133_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG133_IN_SEL    (BIT(9))
#define GPIO_SIG133_IN_SEL_M  (GPIO_SIG133_IN_SEL_V << GPIO_SIG133_IN_SEL_S)
#define GPIO_SIG133_IN_SEL_V  0x00000001U
#define GPIO_SIG133_IN_SEL_S  9

/** GPIO_FUNC134_IN_SEL_CFG_REG register
 *  Configuration register for input signal 134
 */
#define GPIO_FUNC134_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x50c)
/** GPIO_FUNC134_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 134.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC134_IN_SEL    0x000000FFU
#define GPIO_FUNC134_IN_SEL_M  (GPIO_FUNC134_IN_SEL_V << GPIO_FUNC134_IN_SEL_S)
#define GPIO_FUNC134_IN_SEL_V  0x000000FFU
#define GPIO_FUNC134_IN_SEL_S  0
/** GPIO_FUNC134_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC134_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC134_IN_INV_SEL_M  (GPIO_FUNC134_IN_INV_SEL_V << GPIO_FUNC134_IN_INV_SEL_S)
#define GPIO_FUNC134_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC134_IN_INV_SEL_S  8
/** GPIO_SIG134_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG134_IN_SEL    (BIT(9))
#define GPIO_SIG134_IN_SEL_M  (GPIO_SIG134_IN_SEL_V << GPIO_SIG134_IN_SEL_S)
#define GPIO_SIG134_IN_SEL_V  0x00000001U
#define GPIO_SIG134_IN_SEL_S  9

/** GPIO_FUNC135_IN_SEL_CFG_REG register
 *  Configuration register for input signal 135
 */
#define GPIO_FUNC135_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x510)
/** GPIO_FUNC135_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 135.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC135_IN_SEL    0x000000FFU
#define GPIO_FUNC135_IN_SEL_M  (GPIO_FUNC135_IN_SEL_V << GPIO_FUNC135_IN_SEL_S)
#define GPIO_FUNC135_IN_SEL_V  0x000000FFU
#define GPIO_FUNC135_IN_SEL_S  0
/** GPIO_FUNC135_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC135_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC135_IN_INV_SEL_M  (GPIO_FUNC135_IN_INV_SEL_V << GPIO_FUNC135_IN_INV_SEL_S)
#define GPIO_FUNC135_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC135_IN_INV_SEL_S  8
/** GPIO_SIG135_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG135_IN_SEL    (BIT(9))
#define GPIO_SIG135_IN_SEL_M  (GPIO_SIG135_IN_SEL_V << GPIO_SIG135_IN_SEL_S)
#define GPIO_SIG135_IN_SEL_V  0x00000001U
#define GPIO_SIG135_IN_SEL_S  9

/** GPIO_FUNC137_IN_SEL_CFG_REG register
 *  Configuration register for input signal 137
 */
#define GPIO_FUNC137_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x518)
/** GPIO_FUNC137_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 137.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC137_IN_SEL    0x000000FFU
#define GPIO_FUNC137_IN_SEL_M  (GPIO_FUNC137_IN_SEL_V << GPIO_FUNC137_IN_SEL_S)
#define GPIO_FUNC137_IN_SEL_V  0x000000FFU
#define GPIO_FUNC137_IN_SEL_S  0
/** GPIO_FUNC137_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC137_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC137_IN_INV_SEL_M  (GPIO_FUNC137_IN_INV_SEL_V << GPIO_FUNC137_IN_INV_SEL_S)
#define GPIO_FUNC137_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC137_IN_INV_SEL_S  8
/** GPIO_SIG137_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG137_IN_SEL    (BIT(9))
#define GPIO_SIG137_IN_SEL_M  (GPIO_SIG137_IN_SEL_V << GPIO_SIG137_IN_SEL_S)
#define GPIO_SIG137_IN_SEL_V  0x00000001U
#define GPIO_SIG137_IN_SEL_S  9

/** GPIO_FUNC138_IN_SEL_CFG_REG register
 *  Configuration register for input signal 138
 */
#define GPIO_FUNC138_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x51c)
/** GPIO_FUNC138_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 138.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC138_IN_SEL    0x000000FFU
#define GPIO_FUNC138_IN_SEL_M  (GPIO_FUNC138_IN_SEL_V << GPIO_FUNC138_IN_SEL_S)
#define GPIO_FUNC138_IN_SEL_V  0x000000FFU
#define GPIO_FUNC138_IN_SEL_S  0
/** GPIO_FUNC138_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC138_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC138_IN_INV_SEL_M  (GPIO_FUNC138_IN_INV_SEL_V << GPIO_FUNC138_IN_INV_SEL_S)
#define GPIO_FUNC138_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC138_IN_INV_SEL_S  8
/** GPIO_SIG138_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG138_IN_SEL    (BIT(9))
#define GPIO_SIG138_IN_SEL_M  (GPIO_SIG138_IN_SEL_V << GPIO_SIG138_IN_SEL_S)
#define GPIO_SIG138_IN_SEL_V  0x00000001U
#define GPIO_SIG138_IN_SEL_S  9

/** GPIO_FUNC139_IN_SEL_CFG_REG register
 *  Configuration register for input signal 139
 */
#define GPIO_FUNC139_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x520)
/** GPIO_FUNC139_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 139.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC139_IN_SEL    0x000000FFU
#define GPIO_FUNC139_IN_SEL_M  (GPIO_FUNC139_IN_SEL_V << GPIO_FUNC139_IN_SEL_S)
#define GPIO_FUNC139_IN_SEL_V  0x000000FFU
#define GPIO_FUNC139_IN_SEL_S  0
/** GPIO_FUNC139_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC139_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC139_IN_INV_SEL_M  (GPIO_FUNC139_IN_INV_SEL_V << GPIO_FUNC139_IN_INV_SEL_S)
#define GPIO_FUNC139_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC139_IN_INV_SEL_S  8
/** GPIO_SIG139_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG139_IN_SEL    (BIT(9))
#define GPIO_SIG139_IN_SEL_M  (GPIO_SIG139_IN_SEL_V << GPIO_SIG139_IN_SEL_S)
#define GPIO_SIG139_IN_SEL_V  0x00000001U
#define GPIO_SIG139_IN_SEL_S  9

/** GPIO_FUNC140_IN_SEL_CFG_REG register
 *  Configuration register for input signal 140
 */
#define GPIO_FUNC140_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x524)
/** GPIO_FUNC140_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 140.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC140_IN_SEL    0x000000FFU
#define GPIO_FUNC140_IN_SEL_M  (GPIO_FUNC140_IN_SEL_V << GPIO_FUNC140_IN_SEL_S)
#define GPIO_FUNC140_IN_SEL_V  0x000000FFU
#define GPIO_FUNC140_IN_SEL_S  0
/** GPIO_FUNC140_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC140_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC140_IN_INV_SEL_M  (GPIO_FUNC140_IN_INV_SEL_V << GPIO_FUNC140_IN_INV_SEL_S)
#define GPIO_FUNC140_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC140_IN_INV_SEL_S  8
/** GPIO_SIG140_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG140_IN_SEL    (BIT(9))
#define GPIO_SIG140_IN_SEL_M  (GPIO_SIG140_IN_SEL_V << GPIO_SIG140_IN_SEL_S)
#define GPIO_SIG140_IN_SEL_V  0x00000001U
#define GPIO_SIG140_IN_SEL_S  9

/** GPIO_FUNC141_IN_SEL_CFG_REG register
 *  Configuration register for input signal 141
 */
#define GPIO_FUNC141_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x528)
/** GPIO_FUNC141_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 141.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC141_IN_SEL    0x000000FFU
#define GPIO_FUNC141_IN_SEL_M  (GPIO_FUNC141_IN_SEL_V << GPIO_FUNC141_IN_SEL_S)
#define GPIO_FUNC141_IN_SEL_V  0x000000FFU
#define GPIO_FUNC141_IN_SEL_S  0
/** GPIO_FUNC141_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC141_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC141_IN_INV_SEL_M  (GPIO_FUNC141_IN_INV_SEL_V << GPIO_FUNC141_IN_INV_SEL_S)
#define GPIO_FUNC141_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC141_IN_INV_SEL_S  8
/** GPIO_SIG141_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG141_IN_SEL    (BIT(9))
#define GPIO_SIG141_IN_SEL_M  (GPIO_SIG141_IN_SEL_V << GPIO_SIG141_IN_SEL_S)
#define GPIO_SIG141_IN_SEL_V  0x00000001U
#define GPIO_SIG141_IN_SEL_S  9

/** GPIO_FUNC142_IN_SEL_CFG_REG register
 *  Configuration register for input signal 142
 */
#define GPIO_FUNC142_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x52c)
/** GPIO_FUNC142_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 142.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC142_IN_SEL    0x000000FFU
#define GPIO_FUNC142_IN_SEL_M  (GPIO_FUNC142_IN_SEL_V << GPIO_FUNC142_IN_SEL_S)
#define GPIO_FUNC142_IN_SEL_V  0x000000FFU
#define GPIO_FUNC142_IN_SEL_S  0
/** GPIO_FUNC142_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC142_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC142_IN_INV_SEL_M  (GPIO_FUNC142_IN_INV_SEL_V << GPIO_FUNC142_IN_INV_SEL_S)
#define GPIO_FUNC142_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC142_IN_INV_SEL_S  8
/** GPIO_SIG142_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG142_IN_SEL    (BIT(9))
#define GPIO_SIG142_IN_SEL_M  (GPIO_SIG142_IN_SEL_V << GPIO_SIG142_IN_SEL_S)
#define GPIO_SIG142_IN_SEL_V  0x00000001U
#define GPIO_SIG142_IN_SEL_S  9

/** GPIO_FUNC143_IN_SEL_CFG_REG register
 *  Configuration register for input signal 143
 */
#define GPIO_FUNC143_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x530)
/** GPIO_FUNC143_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 143.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC143_IN_SEL    0x000000FFU
#define GPIO_FUNC143_IN_SEL_M  (GPIO_FUNC143_IN_SEL_V << GPIO_FUNC143_IN_SEL_S)
#define GPIO_FUNC143_IN_SEL_V  0x000000FFU
#define GPIO_FUNC143_IN_SEL_S  0
/** GPIO_FUNC143_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC143_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC143_IN_INV_SEL_M  (GPIO_FUNC143_IN_INV_SEL_V << GPIO_FUNC143_IN_INV_SEL_S)
#define GPIO_FUNC143_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC143_IN_INV_SEL_S  8
/** GPIO_SIG143_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG143_IN_SEL    (BIT(9))
#define GPIO_SIG143_IN_SEL_M  (GPIO_SIG143_IN_SEL_V << GPIO_SIG143_IN_SEL_S)
#define GPIO_SIG143_IN_SEL_V  0x00000001U
#define GPIO_SIG143_IN_SEL_S  9

/** GPIO_FUNC144_IN_SEL_CFG_REG register
 *  Configuration register for input signal 144
 */
#define GPIO_FUNC144_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x534)
/** GPIO_FUNC144_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 144.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC144_IN_SEL    0x000000FFU
#define GPIO_FUNC144_IN_SEL_M  (GPIO_FUNC144_IN_SEL_V << GPIO_FUNC144_IN_SEL_S)
#define GPIO_FUNC144_IN_SEL_V  0x000000FFU
#define GPIO_FUNC144_IN_SEL_S  0
/** GPIO_FUNC144_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC144_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC144_IN_INV_SEL_M  (GPIO_FUNC144_IN_INV_SEL_V << GPIO_FUNC144_IN_INV_SEL_S)
#define GPIO_FUNC144_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC144_IN_INV_SEL_S  8
/** GPIO_SIG144_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG144_IN_SEL    (BIT(9))
#define GPIO_SIG144_IN_SEL_M  (GPIO_SIG144_IN_SEL_V << GPIO_SIG144_IN_SEL_S)
#define GPIO_SIG144_IN_SEL_V  0x00000001U
#define GPIO_SIG144_IN_SEL_S  9

/** GPIO_FUNC145_IN_SEL_CFG_REG register
 *  Configuration register for input signal 145
 */
#define GPIO_FUNC145_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x538)
/** GPIO_FUNC145_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 145.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC145_IN_SEL    0x000000FFU
#define GPIO_FUNC145_IN_SEL_M  (GPIO_FUNC145_IN_SEL_V << GPIO_FUNC145_IN_SEL_S)
#define GPIO_FUNC145_IN_SEL_V  0x000000FFU
#define GPIO_FUNC145_IN_SEL_S  0
/** GPIO_FUNC145_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC145_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC145_IN_INV_SEL_M  (GPIO_FUNC145_IN_INV_SEL_V << GPIO_FUNC145_IN_INV_SEL_S)
#define GPIO_FUNC145_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC145_IN_INV_SEL_S  8
/** GPIO_SIG145_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG145_IN_SEL    (BIT(9))
#define GPIO_SIG145_IN_SEL_M  (GPIO_SIG145_IN_SEL_V << GPIO_SIG145_IN_SEL_S)
#define GPIO_SIG145_IN_SEL_V  0x00000001U
#define GPIO_SIG145_IN_SEL_S  9

/** GPIO_FUNC146_IN_SEL_CFG_REG register
 *  Configuration register for input signal 146
 */
#define GPIO_FUNC146_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x53c)
/** GPIO_FUNC146_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 146.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC146_IN_SEL    0x000000FFU
#define GPIO_FUNC146_IN_SEL_M  (GPIO_FUNC146_IN_SEL_V << GPIO_FUNC146_IN_SEL_S)
#define GPIO_FUNC146_IN_SEL_V  0x000000FFU
#define GPIO_FUNC146_IN_SEL_S  0
/** GPIO_FUNC146_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC146_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC146_IN_INV_SEL_M  (GPIO_FUNC146_IN_INV_SEL_V << GPIO_FUNC146_IN_INV_SEL_S)
#define GPIO_FUNC146_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC146_IN_INV_SEL_S  8
/** GPIO_SIG146_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG146_IN_SEL    (BIT(9))
#define GPIO_SIG146_IN_SEL_M  (GPIO_SIG146_IN_SEL_V << GPIO_SIG146_IN_SEL_S)
#define GPIO_SIG146_IN_SEL_V  0x00000001U
#define GPIO_SIG146_IN_SEL_S  9

/** GPIO_FUNC147_IN_SEL_CFG_REG register
 *  Configuration register for input signal 147
 */
#define GPIO_FUNC147_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x540)
/** GPIO_FUNC147_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 147.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC147_IN_SEL    0x000000FFU
#define GPIO_FUNC147_IN_SEL_M  (GPIO_FUNC147_IN_SEL_V << GPIO_FUNC147_IN_SEL_S)
#define GPIO_FUNC147_IN_SEL_V  0x000000FFU
#define GPIO_FUNC147_IN_SEL_S  0
/** GPIO_FUNC147_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC147_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC147_IN_INV_SEL_M  (GPIO_FUNC147_IN_INV_SEL_V << GPIO_FUNC147_IN_INV_SEL_S)
#define GPIO_FUNC147_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC147_IN_INV_SEL_S  8
/** GPIO_SIG147_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG147_IN_SEL    (BIT(9))
#define GPIO_SIG147_IN_SEL_M  (GPIO_SIG147_IN_SEL_V << GPIO_SIG147_IN_SEL_S)
#define GPIO_SIG147_IN_SEL_V  0x00000001U
#define GPIO_SIG147_IN_SEL_S  9

/** GPIO_FUNC148_IN_SEL_CFG_REG register
 *  Configuration register for input signal 148
 */
#define GPIO_FUNC148_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x544)
/** GPIO_FUNC148_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 148.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC148_IN_SEL    0x000000FFU
#define GPIO_FUNC148_IN_SEL_M  (GPIO_FUNC148_IN_SEL_V << GPIO_FUNC148_IN_SEL_S)
#define GPIO_FUNC148_IN_SEL_V  0x000000FFU
#define GPIO_FUNC148_IN_SEL_S  0
/** GPIO_FUNC148_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC148_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC148_IN_INV_SEL_M  (GPIO_FUNC148_IN_INV_SEL_V << GPIO_FUNC148_IN_INV_SEL_S)
#define GPIO_FUNC148_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC148_IN_INV_SEL_S  8
/** GPIO_SIG148_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG148_IN_SEL    (BIT(9))
#define GPIO_SIG148_IN_SEL_M  (GPIO_SIG148_IN_SEL_V << GPIO_SIG148_IN_SEL_S)
#define GPIO_SIG148_IN_SEL_V  0x00000001U
#define GPIO_SIG148_IN_SEL_S  9

/** GPIO_FUNC149_IN_SEL_CFG_REG register
 *  Configuration register for input signal 149
 */
#define GPIO_FUNC149_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x548)
/** GPIO_FUNC149_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 149.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC149_IN_SEL    0x000000FFU
#define GPIO_FUNC149_IN_SEL_M  (GPIO_FUNC149_IN_SEL_V << GPIO_FUNC149_IN_SEL_S)
#define GPIO_FUNC149_IN_SEL_V  0x000000FFU
#define GPIO_FUNC149_IN_SEL_S  0
/** GPIO_FUNC149_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC149_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC149_IN_INV_SEL_M  (GPIO_FUNC149_IN_INV_SEL_V << GPIO_FUNC149_IN_INV_SEL_S)
#define GPIO_FUNC149_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC149_IN_INV_SEL_S  8
/** GPIO_SIG149_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG149_IN_SEL    (BIT(9))
#define GPIO_SIG149_IN_SEL_M  (GPIO_SIG149_IN_SEL_V << GPIO_SIG149_IN_SEL_S)
#define GPIO_SIG149_IN_SEL_V  0x00000001U
#define GPIO_SIG149_IN_SEL_S  9

/** GPIO_FUNC150_IN_SEL_CFG_REG register
 *  Configuration register for input signal 150
 */
#define GPIO_FUNC150_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x54c)
/** GPIO_FUNC150_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 150.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC150_IN_SEL    0x000000FFU
#define GPIO_FUNC150_IN_SEL_M  (GPIO_FUNC150_IN_SEL_V << GPIO_FUNC150_IN_SEL_S)
#define GPIO_FUNC150_IN_SEL_V  0x000000FFU
#define GPIO_FUNC150_IN_SEL_S  0
/** GPIO_FUNC150_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC150_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC150_IN_INV_SEL_M  (GPIO_FUNC150_IN_INV_SEL_V << GPIO_FUNC150_IN_INV_SEL_S)
#define GPIO_FUNC150_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC150_IN_INV_SEL_S  8
/** GPIO_SIG150_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG150_IN_SEL    (BIT(9))
#define GPIO_SIG150_IN_SEL_M  (GPIO_SIG150_IN_SEL_V << GPIO_SIG150_IN_SEL_S)
#define GPIO_SIG150_IN_SEL_V  0x00000001U
#define GPIO_SIG150_IN_SEL_S  9

/** GPIO_FUNC151_IN_SEL_CFG_REG register
 *  Configuration register for input signal 151
 */
#define GPIO_FUNC151_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x550)
/** GPIO_FUNC151_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 151.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC151_IN_SEL    0x000000FFU
#define GPIO_FUNC151_IN_SEL_M  (GPIO_FUNC151_IN_SEL_V << GPIO_FUNC151_IN_SEL_S)
#define GPIO_FUNC151_IN_SEL_V  0x000000FFU
#define GPIO_FUNC151_IN_SEL_S  0
/** GPIO_FUNC151_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC151_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC151_IN_INV_SEL_M  (GPIO_FUNC151_IN_INV_SEL_V << GPIO_FUNC151_IN_INV_SEL_S)
#define GPIO_FUNC151_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC151_IN_INV_SEL_S  8
/** GPIO_SIG151_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG151_IN_SEL    (BIT(9))
#define GPIO_SIG151_IN_SEL_M  (GPIO_SIG151_IN_SEL_V << GPIO_SIG151_IN_SEL_S)
#define GPIO_SIG151_IN_SEL_V  0x00000001U
#define GPIO_SIG151_IN_SEL_S  9

/** GPIO_FUNC152_IN_SEL_CFG_REG register
 *  Configuration register for input signal 152
 */
#define GPIO_FUNC152_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x554)
/** GPIO_FUNC152_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 152.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC152_IN_SEL    0x000000FFU
#define GPIO_FUNC152_IN_SEL_M  (GPIO_FUNC152_IN_SEL_V << GPIO_FUNC152_IN_SEL_S)
#define GPIO_FUNC152_IN_SEL_V  0x000000FFU
#define GPIO_FUNC152_IN_SEL_S  0
/** GPIO_FUNC152_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC152_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC152_IN_INV_SEL_M  (GPIO_FUNC152_IN_INV_SEL_V << GPIO_FUNC152_IN_INV_SEL_S)
#define GPIO_FUNC152_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC152_IN_INV_SEL_S  8
/** GPIO_SIG152_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG152_IN_SEL    (BIT(9))
#define GPIO_SIG152_IN_SEL_M  (GPIO_SIG152_IN_SEL_V << GPIO_SIG152_IN_SEL_S)
#define GPIO_SIG152_IN_SEL_V  0x00000001U
#define GPIO_SIG152_IN_SEL_S  9

/** GPIO_FUNC153_IN_SEL_CFG_REG register
 *  Configuration register for input signal 153
 */
#define GPIO_FUNC153_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x558)
/** GPIO_FUNC153_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 153.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC153_IN_SEL    0x000000FFU
#define GPIO_FUNC153_IN_SEL_M  (GPIO_FUNC153_IN_SEL_V << GPIO_FUNC153_IN_SEL_S)
#define GPIO_FUNC153_IN_SEL_V  0x000000FFU
#define GPIO_FUNC153_IN_SEL_S  0
/** GPIO_FUNC153_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC153_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC153_IN_INV_SEL_M  (GPIO_FUNC153_IN_INV_SEL_V << GPIO_FUNC153_IN_INV_SEL_S)
#define GPIO_FUNC153_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC153_IN_INV_SEL_S  8
/** GPIO_SIG153_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG153_IN_SEL    (BIT(9))
#define GPIO_SIG153_IN_SEL_M  (GPIO_SIG153_IN_SEL_V << GPIO_SIG153_IN_SEL_S)
#define GPIO_SIG153_IN_SEL_V  0x00000001U
#define GPIO_SIG153_IN_SEL_S  9

/** GPIO_FUNC154_IN_SEL_CFG_REG register
 *  Configuration register for input signal 154
 */
#define GPIO_FUNC154_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x55c)
/** GPIO_FUNC154_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 154.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC154_IN_SEL    0x000000FFU
#define GPIO_FUNC154_IN_SEL_M  (GPIO_FUNC154_IN_SEL_V << GPIO_FUNC154_IN_SEL_S)
#define GPIO_FUNC154_IN_SEL_V  0x000000FFU
#define GPIO_FUNC154_IN_SEL_S  0
/** GPIO_FUNC154_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC154_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC154_IN_INV_SEL_M  (GPIO_FUNC154_IN_INV_SEL_V << GPIO_FUNC154_IN_INV_SEL_S)
#define GPIO_FUNC154_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC154_IN_INV_SEL_S  8
/** GPIO_SIG154_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG154_IN_SEL    (BIT(9))
#define GPIO_SIG154_IN_SEL_M  (GPIO_SIG154_IN_SEL_V << GPIO_SIG154_IN_SEL_S)
#define GPIO_SIG154_IN_SEL_V  0x00000001U
#define GPIO_SIG154_IN_SEL_S  9

/** GPIO_FUNC155_IN_SEL_CFG_REG register
 *  Configuration register for input signal 155
 */
#define GPIO_FUNC155_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x560)
/** GPIO_FUNC155_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 155.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC155_IN_SEL    0x000000FFU
#define GPIO_FUNC155_IN_SEL_M  (GPIO_FUNC155_IN_SEL_V << GPIO_FUNC155_IN_SEL_S)
#define GPIO_FUNC155_IN_SEL_V  0x000000FFU
#define GPIO_FUNC155_IN_SEL_S  0
/** GPIO_FUNC155_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC155_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC155_IN_INV_SEL_M  (GPIO_FUNC155_IN_INV_SEL_V << GPIO_FUNC155_IN_INV_SEL_S)
#define GPIO_FUNC155_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC155_IN_INV_SEL_S  8
/** GPIO_SIG155_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG155_IN_SEL    (BIT(9))
#define GPIO_SIG155_IN_SEL_M  (GPIO_SIG155_IN_SEL_V << GPIO_SIG155_IN_SEL_S)
#define GPIO_SIG155_IN_SEL_V  0x00000001U
#define GPIO_SIG155_IN_SEL_S  9

/** GPIO_FUNC156_IN_SEL_CFG_REG register
 *  Configuration register for input signal 156
 */
#define GPIO_FUNC156_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x564)
/** GPIO_FUNC156_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 156.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC156_IN_SEL    0x000000FFU
#define GPIO_FUNC156_IN_SEL_M  (GPIO_FUNC156_IN_SEL_V << GPIO_FUNC156_IN_SEL_S)
#define GPIO_FUNC156_IN_SEL_V  0x000000FFU
#define GPIO_FUNC156_IN_SEL_S  0
/** GPIO_FUNC156_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC156_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC156_IN_INV_SEL_M  (GPIO_FUNC156_IN_INV_SEL_V << GPIO_FUNC156_IN_INV_SEL_S)
#define GPIO_FUNC156_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC156_IN_INV_SEL_S  8
/** GPIO_SIG156_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG156_IN_SEL    (BIT(9))
#define GPIO_SIG156_IN_SEL_M  (GPIO_SIG156_IN_SEL_V << GPIO_SIG156_IN_SEL_S)
#define GPIO_SIG156_IN_SEL_V  0x00000001U
#define GPIO_SIG156_IN_SEL_S  9

/** GPIO_FUNC158_IN_SEL_CFG_REG register
 *  Configuration register for input signal 158
 */
#define GPIO_FUNC158_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x56c)
/** GPIO_FUNC158_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 158.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC158_IN_SEL    0x000000FFU
#define GPIO_FUNC158_IN_SEL_M  (GPIO_FUNC158_IN_SEL_V << GPIO_FUNC158_IN_SEL_S)
#define GPIO_FUNC158_IN_SEL_V  0x000000FFU
#define GPIO_FUNC158_IN_SEL_S  0
/** GPIO_FUNC158_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC158_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC158_IN_INV_SEL_M  (GPIO_FUNC158_IN_INV_SEL_V << GPIO_FUNC158_IN_INV_SEL_S)
#define GPIO_FUNC158_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC158_IN_INV_SEL_S  8
/** GPIO_SIG158_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG158_IN_SEL    (BIT(9))
#define GPIO_SIG158_IN_SEL_M  (GPIO_SIG158_IN_SEL_V << GPIO_SIG158_IN_SEL_S)
#define GPIO_SIG158_IN_SEL_V  0x00000001U
#define GPIO_SIG158_IN_SEL_S  9

/** GPIO_FUNC159_IN_SEL_CFG_REG register
 *  Configuration register for input signal 159
 */
#define GPIO_FUNC159_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x570)
/** GPIO_FUNC159_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 159.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC159_IN_SEL    0x000000FFU
#define GPIO_FUNC159_IN_SEL_M  (GPIO_FUNC159_IN_SEL_V << GPIO_FUNC159_IN_SEL_S)
#define GPIO_FUNC159_IN_SEL_V  0x000000FFU
#define GPIO_FUNC159_IN_SEL_S  0
/** GPIO_FUNC159_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC159_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC159_IN_INV_SEL_M  (GPIO_FUNC159_IN_INV_SEL_V << GPIO_FUNC159_IN_INV_SEL_S)
#define GPIO_FUNC159_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC159_IN_INV_SEL_S  8
/** GPIO_SIG159_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG159_IN_SEL    (BIT(9))
#define GPIO_SIG159_IN_SEL_M  (GPIO_SIG159_IN_SEL_V << GPIO_SIG159_IN_SEL_S)
#define GPIO_SIG159_IN_SEL_V  0x00000001U
#define GPIO_SIG159_IN_SEL_S  9

/** GPIO_FUNC160_IN_SEL_CFG_REG register
 *  Configuration register for input signal 160
 */
#define GPIO_FUNC160_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x574)
/** GPIO_FUNC160_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 160.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC160_IN_SEL    0x000000FFU
#define GPIO_FUNC160_IN_SEL_M  (GPIO_FUNC160_IN_SEL_V << GPIO_FUNC160_IN_SEL_S)
#define GPIO_FUNC160_IN_SEL_V  0x000000FFU
#define GPIO_FUNC160_IN_SEL_S  0
/** GPIO_FUNC160_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC160_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC160_IN_INV_SEL_M  (GPIO_FUNC160_IN_INV_SEL_V << GPIO_FUNC160_IN_INV_SEL_S)
#define GPIO_FUNC160_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC160_IN_INV_SEL_S  8
/** GPIO_SIG160_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG160_IN_SEL    (BIT(9))
#define GPIO_SIG160_IN_SEL_M  (GPIO_SIG160_IN_SEL_V << GPIO_SIG160_IN_SEL_S)
#define GPIO_SIG160_IN_SEL_V  0x00000001U
#define GPIO_SIG160_IN_SEL_S  9

/** GPIO_FUNC161_IN_SEL_CFG_REG register
 *  Configuration register for input signal 161
 */
#define GPIO_FUNC161_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x578)
/** GPIO_FUNC161_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 161.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC161_IN_SEL    0x000000FFU
#define GPIO_FUNC161_IN_SEL_M  (GPIO_FUNC161_IN_SEL_V << GPIO_FUNC161_IN_SEL_S)
#define GPIO_FUNC161_IN_SEL_V  0x000000FFU
#define GPIO_FUNC161_IN_SEL_S  0
/** GPIO_FUNC161_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC161_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC161_IN_INV_SEL_M  (GPIO_FUNC161_IN_INV_SEL_V << GPIO_FUNC161_IN_INV_SEL_S)
#define GPIO_FUNC161_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC161_IN_INV_SEL_S  8
/** GPIO_SIG161_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG161_IN_SEL    (BIT(9))
#define GPIO_SIG161_IN_SEL_M  (GPIO_SIG161_IN_SEL_V << GPIO_SIG161_IN_SEL_S)
#define GPIO_SIG161_IN_SEL_V  0x00000001U
#define GPIO_SIG161_IN_SEL_S  9

/** GPIO_FUNC162_IN_SEL_CFG_REG register
 *  Configuration register for input signal 162
 */
#define GPIO_FUNC162_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x57c)
/** GPIO_FUNC162_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 162.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC162_IN_SEL    0x000000FFU
#define GPIO_FUNC162_IN_SEL_M  (GPIO_FUNC162_IN_SEL_V << GPIO_FUNC162_IN_SEL_S)
#define GPIO_FUNC162_IN_SEL_V  0x000000FFU
#define GPIO_FUNC162_IN_SEL_S  0
/** GPIO_FUNC162_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC162_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC162_IN_INV_SEL_M  (GPIO_FUNC162_IN_INV_SEL_V << GPIO_FUNC162_IN_INV_SEL_S)
#define GPIO_FUNC162_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC162_IN_INV_SEL_S  8
/** GPIO_SIG162_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG162_IN_SEL    (BIT(9))
#define GPIO_SIG162_IN_SEL_M  (GPIO_SIG162_IN_SEL_V << GPIO_SIG162_IN_SEL_S)
#define GPIO_SIG162_IN_SEL_V  0x00000001U
#define GPIO_SIG162_IN_SEL_S  9

/** GPIO_FUNC163_IN_SEL_CFG_REG register
 *  Configuration register for input signal 163
 */
#define GPIO_FUNC163_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x580)
/** GPIO_FUNC163_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 163.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC163_IN_SEL    0x000000FFU
#define GPIO_FUNC163_IN_SEL_M  (GPIO_FUNC163_IN_SEL_V << GPIO_FUNC163_IN_SEL_S)
#define GPIO_FUNC163_IN_SEL_V  0x000000FFU
#define GPIO_FUNC163_IN_SEL_S  0
/** GPIO_FUNC163_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC163_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC163_IN_INV_SEL_M  (GPIO_FUNC163_IN_INV_SEL_V << GPIO_FUNC163_IN_INV_SEL_S)
#define GPIO_FUNC163_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC163_IN_INV_SEL_S  8
/** GPIO_SIG163_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG163_IN_SEL    (BIT(9))
#define GPIO_SIG163_IN_SEL_M  (GPIO_SIG163_IN_SEL_V << GPIO_SIG163_IN_SEL_S)
#define GPIO_SIG163_IN_SEL_V  0x00000001U
#define GPIO_SIG163_IN_SEL_S  9

/** GPIO_FUNC164_IN_SEL_CFG_REG register
 *  Configuration register for input signal 164
 */
#define GPIO_FUNC164_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x584)
/** GPIO_FUNC164_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 164.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC164_IN_SEL    0x000000FFU
#define GPIO_FUNC164_IN_SEL_M  (GPIO_FUNC164_IN_SEL_V << GPIO_FUNC164_IN_SEL_S)
#define GPIO_FUNC164_IN_SEL_V  0x000000FFU
#define GPIO_FUNC164_IN_SEL_S  0
/** GPIO_FUNC164_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC164_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC164_IN_INV_SEL_M  (GPIO_FUNC164_IN_INV_SEL_V << GPIO_FUNC164_IN_INV_SEL_S)
#define GPIO_FUNC164_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC164_IN_INV_SEL_S  8
/** GPIO_SIG164_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG164_IN_SEL    (BIT(9))
#define GPIO_SIG164_IN_SEL_M  (GPIO_SIG164_IN_SEL_V << GPIO_SIG164_IN_SEL_S)
#define GPIO_SIG164_IN_SEL_V  0x00000001U
#define GPIO_SIG164_IN_SEL_S  9

/** GPIO_FUNC165_IN_SEL_CFG_REG register
 *  Configuration register for input signal 165
 */
#define GPIO_FUNC165_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x588)
/** GPIO_FUNC165_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 165.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC165_IN_SEL    0x000000FFU
#define GPIO_FUNC165_IN_SEL_M  (GPIO_FUNC165_IN_SEL_V << GPIO_FUNC165_IN_SEL_S)
#define GPIO_FUNC165_IN_SEL_V  0x000000FFU
#define GPIO_FUNC165_IN_SEL_S  0
/** GPIO_FUNC165_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC165_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC165_IN_INV_SEL_M  (GPIO_FUNC165_IN_INV_SEL_V << GPIO_FUNC165_IN_INV_SEL_S)
#define GPIO_FUNC165_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC165_IN_INV_SEL_S  8
/** GPIO_SIG165_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG165_IN_SEL    (BIT(9))
#define GPIO_SIG165_IN_SEL_M  (GPIO_SIG165_IN_SEL_V << GPIO_SIG165_IN_SEL_S)
#define GPIO_SIG165_IN_SEL_V  0x00000001U
#define GPIO_SIG165_IN_SEL_S  9

/** GPIO_FUNC166_IN_SEL_CFG_REG register
 *  Configuration register for input signal 166
 */
#define GPIO_FUNC166_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x58c)
/** GPIO_FUNC166_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 166.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC166_IN_SEL    0x000000FFU
#define GPIO_FUNC166_IN_SEL_M  (GPIO_FUNC166_IN_SEL_V << GPIO_FUNC166_IN_SEL_S)
#define GPIO_FUNC166_IN_SEL_V  0x000000FFU
#define GPIO_FUNC166_IN_SEL_S  0
/** GPIO_FUNC166_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC166_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC166_IN_INV_SEL_M  (GPIO_FUNC166_IN_INV_SEL_V << GPIO_FUNC166_IN_INV_SEL_S)
#define GPIO_FUNC166_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC166_IN_INV_SEL_S  8
/** GPIO_SIG166_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG166_IN_SEL    (BIT(9))
#define GPIO_SIG166_IN_SEL_M  (GPIO_SIG166_IN_SEL_V << GPIO_SIG166_IN_SEL_S)
#define GPIO_SIG166_IN_SEL_V  0x00000001U
#define GPIO_SIG166_IN_SEL_S  9

/** GPIO_FUNC167_IN_SEL_CFG_REG register
 *  Configuration register for input signal 167
 */
#define GPIO_FUNC167_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x590)
/** GPIO_FUNC167_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 167.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC167_IN_SEL    0x000000FFU
#define GPIO_FUNC167_IN_SEL_M  (GPIO_FUNC167_IN_SEL_V << GPIO_FUNC167_IN_SEL_S)
#define GPIO_FUNC167_IN_SEL_V  0x000000FFU
#define GPIO_FUNC167_IN_SEL_S  0
/** GPIO_FUNC167_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC167_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC167_IN_INV_SEL_M  (GPIO_FUNC167_IN_INV_SEL_V << GPIO_FUNC167_IN_INV_SEL_S)
#define GPIO_FUNC167_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC167_IN_INV_SEL_S  8
/** GPIO_SIG167_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG167_IN_SEL    (BIT(9))
#define GPIO_SIG167_IN_SEL_M  (GPIO_SIG167_IN_SEL_V << GPIO_SIG167_IN_SEL_S)
#define GPIO_SIG167_IN_SEL_V  0x00000001U
#define GPIO_SIG167_IN_SEL_S  9

/** GPIO_FUNC168_IN_SEL_CFG_REG register
 *  Configuration register for input signal 168
 */
#define GPIO_FUNC168_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x594)
/** GPIO_FUNC168_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 168.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC168_IN_SEL    0x000000FFU
#define GPIO_FUNC168_IN_SEL_M  (GPIO_FUNC168_IN_SEL_V << GPIO_FUNC168_IN_SEL_S)
#define GPIO_FUNC168_IN_SEL_V  0x000000FFU
#define GPIO_FUNC168_IN_SEL_S  0
/** GPIO_FUNC168_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC168_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC168_IN_INV_SEL_M  (GPIO_FUNC168_IN_INV_SEL_V << GPIO_FUNC168_IN_INV_SEL_S)
#define GPIO_FUNC168_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC168_IN_INV_SEL_S  8
/** GPIO_SIG168_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG168_IN_SEL    (BIT(9))
#define GPIO_SIG168_IN_SEL_M  (GPIO_SIG168_IN_SEL_V << GPIO_SIG168_IN_SEL_S)
#define GPIO_SIG168_IN_SEL_V  0x00000001U
#define GPIO_SIG168_IN_SEL_S  9

/** GPIO_FUNC169_IN_SEL_CFG_REG register
 *  Configuration register for input signal 169
 */
#define GPIO_FUNC169_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x598)
/** GPIO_FUNC169_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 169.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC169_IN_SEL    0x000000FFU
#define GPIO_FUNC169_IN_SEL_M  (GPIO_FUNC169_IN_SEL_V << GPIO_FUNC169_IN_SEL_S)
#define GPIO_FUNC169_IN_SEL_V  0x000000FFU
#define GPIO_FUNC169_IN_SEL_S  0
/** GPIO_FUNC169_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC169_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC169_IN_INV_SEL_M  (GPIO_FUNC169_IN_INV_SEL_V << GPIO_FUNC169_IN_INV_SEL_S)
#define GPIO_FUNC169_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC169_IN_INV_SEL_S  8
/** GPIO_SIG169_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG169_IN_SEL    (BIT(9))
#define GPIO_SIG169_IN_SEL_M  (GPIO_SIG169_IN_SEL_V << GPIO_SIG169_IN_SEL_S)
#define GPIO_SIG169_IN_SEL_V  0x00000001U
#define GPIO_SIG169_IN_SEL_S  9

/** GPIO_FUNC170_IN_SEL_CFG_REG register
 *  Configuration register for input signal 170
 */
#define GPIO_FUNC170_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x59c)
/** GPIO_FUNC170_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 170.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC170_IN_SEL    0x000000FFU
#define GPIO_FUNC170_IN_SEL_M  (GPIO_FUNC170_IN_SEL_V << GPIO_FUNC170_IN_SEL_S)
#define GPIO_FUNC170_IN_SEL_V  0x000000FFU
#define GPIO_FUNC170_IN_SEL_S  0
/** GPIO_FUNC170_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC170_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC170_IN_INV_SEL_M  (GPIO_FUNC170_IN_INV_SEL_V << GPIO_FUNC170_IN_INV_SEL_S)
#define GPIO_FUNC170_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC170_IN_INV_SEL_S  8
/** GPIO_SIG170_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG170_IN_SEL    (BIT(9))
#define GPIO_SIG170_IN_SEL_M  (GPIO_SIG170_IN_SEL_V << GPIO_SIG170_IN_SEL_S)
#define GPIO_SIG170_IN_SEL_V  0x00000001U
#define GPIO_SIG170_IN_SEL_S  9

/** GPIO_FUNC171_IN_SEL_CFG_REG register
 *  Configuration register for input signal 171
 */
#define GPIO_FUNC171_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a0)
/** GPIO_FUNC171_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 171.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC171_IN_SEL    0x000000FFU
#define GPIO_FUNC171_IN_SEL_M  (GPIO_FUNC171_IN_SEL_V << GPIO_FUNC171_IN_SEL_S)
#define GPIO_FUNC171_IN_SEL_V  0x000000FFU
#define GPIO_FUNC171_IN_SEL_S  0
/** GPIO_FUNC171_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC171_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC171_IN_INV_SEL_M  (GPIO_FUNC171_IN_INV_SEL_V << GPIO_FUNC171_IN_INV_SEL_S)
#define GPIO_FUNC171_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC171_IN_INV_SEL_S  8
/** GPIO_SIG171_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG171_IN_SEL    (BIT(9))
#define GPIO_SIG171_IN_SEL_M  (GPIO_SIG171_IN_SEL_V << GPIO_SIG171_IN_SEL_S)
#define GPIO_SIG171_IN_SEL_V  0x00000001U
#define GPIO_SIG171_IN_SEL_S  9

/** GPIO_FUNC172_IN_SEL_CFG_REG register
 *  Configuration register for input signal 172
 */
#define GPIO_FUNC172_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a4)
/** GPIO_FUNC172_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 172.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC172_IN_SEL    0x000000FFU
#define GPIO_FUNC172_IN_SEL_M  (GPIO_FUNC172_IN_SEL_V << GPIO_FUNC172_IN_SEL_S)
#define GPIO_FUNC172_IN_SEL_V  0x000000FFU
#define GPIO_FUNC172_IN_SEL_S  0
/** GPIO_FUNC172_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC172_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC172_IN_INV_SEL_M  (GPIO_FUNC172_IN_INV_SEL_V << GPIO_FUNC172_IN_INV_SEL_S)
#define GPIO_FUNC172_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC172_IN_INV_SEL_S  8
/** GPIO_SIG172_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG172_IN_SEL    (BIT(9))
#define GPIO_SIG172_IN_SEL_M  (GPIO_SIG172_IN_SEL_V << GPIO_SIG172_IN_SEL_S)
#define GPIO_SIG172_IN_SEL_V  0x00000001U
#define GPIO_SIG172_IN_SEL_S  9

/** GPIO_FUNC173_IN_SEL_CFG_REG register
 *  Configuration register for input signal 173
 */
#define GPIO_FUNC173_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5a8)
/** GPIO_FUNC173_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 173.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC173_IN_SEL    0x000000FFU
#define GPIO_FUNC173_IN_SEL_M  (GPIO_FUNC173_IN_SEL_V << GPIO_FUNC173_IN_SEL_S)
#define GPIO_FUNC173_IN_SEL_V  0x000000FFU
#define GPIO_FUNC173_IN_SEL_S  0
/** GPIO_FUNC173_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC173_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC173_IN_INV_SEL_M  (GPIO_FUNC173_IN_INV_SEL_V << GPIO_FUNC173_IN_INV_SEL_S)
#define GPIO_FUNC173_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC173_IN_INV_SEL_S  8
/** GPIO_SIG173_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG173_IN_SEL    (BIT(9))
#define GPIO_SIG173_IN_SEL_M  (GPIO_SIG173_IN_SEL_V << GPIO_SIG173_IN_SEL_S)
#define GPIO_SIG173_IN_SEL_V  0x00000001U
#define GPIO_SIG173_IN_SEL_S  9

/** GPIO_FUNC174_IN_SEL_CFG_REG register
 *  Configuration register for input signal 174
 */
#define GPIO_FUNC174_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5ac)
/** GPIO_FUNC174_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 174.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC174_IN_SEL    0x000000FFU
#define GPIO_FUNC174_IN_SEL_M  (GPIO_FUNC174_IN_SEL_V << GPIO_FUNC174_IN_SEL_S)
#define GPIO_FUNC174_IN_SEL_V  0x000000FFU
#define GPIO_FUNC174_IN_SEL_S  0
/** GPIO_FUNC174_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC174_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC174_IN_INV_SEL_M  (GPIO_FUNC174_IN_INV_SEL_V << GPIO_FUNC174_IN_INV_SEL_S)
#define GPIO_FUNC174_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC174_IN_INV_SEL_S  8
/** GPIO_SIG174_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG174_IN_SEL    (BIT(9))
#define GPIO_SIG174_IN_SEL_M  (GPIO_SIG174_IN_SEL_V << GPIO_SIG174_IN_SEL_S)
#define GPIO_SIG174_IN_SEL_V  0x00000001U
#define GPIO_SIG174_IN_SEL_S  9

/** GPIO_FUNC175_IN_SEL_CFG_REG register
 *  Configuration register for input signal 175
 */
#define GPIO_FUNC175_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b0)
/** GPIO_FUNC175_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 175.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC175_IN_SEL    0x000000FFU
#define GPIO_FUNC175_IN_SEL_M  (GPIO_FUNC175_IN_SEL_V << GPIO_FUNC175_IN_SEL_S)
#define GPIO_FUNC175_IN_SEL_V  0x000000FFU
#define GPIO_FUNC175_IN_SEL_S  0
/** GPIO_FUNC175_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC175_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC175_IN_INV_SEL_M  (GPIO_FUNC175_IN_INV_SEL_V << GPIO_FUNC175_IN_INV_SEL_S)
#define GPIO_FUNC175_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC175_IN_INV_SEL_S  8
/** GPIO_SIG175_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG175_IN_SEL    (BIT(9))
#define GPIO_SIG175_IN_SEL_M  (GPIO_SIG175_IN_SEL_V << GPIO_SIG175_IN_SEL_S)
#define GPIO_SIG175_IN_SEL_V  0x00000001U
#define GPIO_SIG175_IN_SEL_S  9

/** GPIO_FUNC176_IN_SEL_CFG_REG register
 *  Configuration register for input signal 176
 */
#define GPIO_FUNC176_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b4)
/** GPIO_FUNC176_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 176.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC176_IN_SEL    0x000000FFU
#define GPIO_FUNC176_IN_SEL_M  (GPIO_FUNC176_IN_SEL_V << GPIO_FUNC176_IN_SEL_S)
#define GPIO_FUNC176_IN_SEL_V  0x000000FFU
#define GPIO_FUNC176_IN_SEL_S  0
/** GPIO_FUNC176_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC176_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC176_IN_INV_SEL_M  (GPIO_FUNC176_IN_INV_SEL_V << GPIO_FUNC176_IN_INV_SEL_S)
#define GPIO_FUNC176_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC176_IN_INV_SEL_S  8
/** GPIO_SIG176_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG176_IN_SEL    (BIT(9))
#define GPIO_SIG176_IN_SEL_M  (GPIO_SIG176_IN_SEL_V << GPIO_SIG176_IN_SEL_S)
#define GPIO_SIG176_IN_SEL_V  0x00000001U
#define GPIO_SIG176_IN_SEL_S  9

/** GPIO_FUNC177_IN_SEL_CFG_REG register
 *  Configuration register for input signal 177
 */
#define GPIO_FUNC177_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5b8)
/** GPIO_FUNC177_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 177.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC177_IN_SEL    0x000000FFU
#define GPIO_FUNC177_IN_SEL_M  (GPIO_FUNC177_IN_SEL_V << GPIO_FUNC177_IN_SEL_S)
#define GPIO_FUNC177_IN_SEL_V  0x000000FFU
#define GPIO_FUNC177_IN_SEL_S  0
/** GPIO_FUNC177_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC177_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC177_IN_INV_SEL_M  (GPIO_FUNC177_IN_INV_SEL_V << GPIO_FUNC177_IN_INV_SEL_S)
#define GPIO_FUNC177_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC177_IN_INV_SEL_S  8
/** GPIO_SIG177_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG177_IN_SEL    (BIT(9))
#define GPIO_SIG177_IN_SEL_M  (GPIO_SIG177_IN_SEL_V << GPIO_SIG177_IN_SEL_S)
#define GPIO_SIG177_IN_SEL_V  0x00000001U
#define GPIO_SIG177_IN_SEL_S  9

/** GPIO_FUNC178_IN_SEL_CFG_REG register
 *  Configuration register for input signal 178
 */
#define GPIO_FUNC178_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5bc)
/** GPIO_FUNC178_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 178.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC178_IN_SEL    0x000000FFU
#define GPIO_FUNC178_IN_SEL_M  (GPIO_FUNC178_IN_SEL_V << GPIO_FUNC178_IN_SEL_S)
#define GPIO_FUNC178_IN_SEL_V  0x000000FFU
#define GPIO_FUNC178_IN_SEL_S  0
/** GPIO_FUNC178_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC178_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC178_IN_INV_SEL_M  (GPIO_FUNC178_IN_INV_SEL_V << GPIO_FUNC178_IN_INV_SEL_S)
#define GPIO_FUNC178_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC178_IN_INV_SEL_S  8
/** GPIO_SIG178_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG178_IN_SEL    (BIT(9))
#define GPIO_SIG178_IN_SEL_M  (GPIO_SIG178_IN_SEL_V << GPIO_SIG178_IN_SEL_S)
#define GPIO_SIG178_IN_SEL_V  0x00000001U
#define GPIO_SIG178_IN_SEL_S  9

/** GPIO_FUNC179_IN_SEL_CFG_REG register
 *  Configuration register for input signal 179
 */
#define GPIO_FUNC179_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c0)
/** GPIO_FUNC179_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 179.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC179_IN_SEL    0x000000FFU
#define GPIO_FUNC179_IN_SEL_M  (GPIO_FUNC179_IN_SEL_V << GPIO_FUNC179_IN_SEL_S)
#define GPIO_FUNC179_IN_SEL_V  0x000000FFU
#define GPIO_FUNC179_IN_SEL_S  0
/** GPIO_FUNC179_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC179_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC179_IN_INV_SEL_M  (GPIO_FUNC179_IN_INV_SEL_V << GPIO_FUNC179_IN_INV_SEL_S)
#define GPIO_FUNC179_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC179_IN_INV_SEL_S  8
/** GPIO_SIG179_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG179_IN_SEL    (BIT(9))
#define GPIO_SIG179_IN_SEL_M  (GPIO_SIG179_IN_SEL_V << GPIO_SIG179_IN_SEL_S)
#define GPIO_SIG179_IN_SEL_V  0x00000001U
#define GPIO_SIG179_IN_SEL_S  9

/** GPIO_FUNC180_IN_SEL_CFG_REG register
 *  Configuration register for input signal 180
 */
#define GPIO_FUNC180_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c4)
/** GPIO_FUNC180_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 180.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC180_IN_SEL    0x000000FFU
#define GPIO_FUNC180_IN_SEL_M  (GPIO_FUNC180_IN_SEL_V << GPIO_FUNC180_IN_SEL_S)
#define GPIO_FUNC180_IN_SEL_V  0x000000FFU
#define GPIO_FUNC180_IN_SEL_S  0
/** GPIO_FUNC180_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC180_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC180_IN_INV_SEL_M  (GPIO_FUNC180_IN_INV_SEL_V << GPIO_FUNC180_IN_INV_SEL_S)
#define GPIO_FUNC180_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC180_IN_INV_SEL_S  8
/** GPIO_SIG180_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG180_IN_SEL    (BIT(9))
#define GPIO_SIG180_IN_SEL_M  (GPIO_SIG180_IN_SEL_V << GPIO_SIG180_IN_SEL_S)
#define GPIO_SIG180_IN_SEL_V  0x00000001U
#define GPIO_SIG180_IN_SEL_S  9

/** GPIO_FUNC181_IN_SEL_CFG_REG register
 *  Configuration register for input signal 181
 */
#define GPIO_FUNC181_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5c8)
/** GPIO_FUNC181_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 181.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC181_IN_SEL    0x000000FFU
#define GPIO_FUNC181_IN_SEL_M  (GPIO_FUNC181_IN_SEL_V << GPIO_FUNC181_IN_SEL_S)
#define GPIO_FUNC181_IN_SEL_V  0x000000FFU
#define GPIO_FUNC181_IN_SEL_S  0
/** GPIO_FUNC181_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC181_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC181_IN_INV_SEL_M  (GPIO_FUNC181_IN_INV_SEL_V << GPIO_FUNC181_IN_INV_SEL_S)
#define GPIO_FUNC181_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC181_IN_INV_SEL_S  8
/** GPIO_SIG181_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG181_IN_SEL    (BIT(9))
#define GPIO_SIG181_IN_SEL_M  (GPIO_SIG181_IN_SEL_V << GPIO_SIG181_IN_SEL_S)
#define GPIO_SIG181_IN_SEL_V  0x00000001U
#define GPIO_SIG181_IN_SEL_S  9

/** GPIO_FUNC182_IN_SEL_CFG_REG register
 *  Configuration register for input signal 182
 */
#define GPIO_FUNC182_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5cc)
/** GPIO_FUNC182_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 182.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC182_IN_SEL    0x000000FFU
#define GPIO_FUNC182_IN_SEL_M  (GPIO_FUNC182_IN_SEL_V << GPIO_FUNC182_IN_SEL_S)
#define GPIO_FUNC182_IN_SEL_V  0x000000FFU
#define GPIO_FUNC182_IN_SEL_S  0
/** GPIO_FUNC182_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC182_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC182_IN_INV_SEL_M  (GPIO_FUNC182_IN_INV_SEL_V << GPIO_FUNC182_IN_INV_SEL_S)
#define GPIO_FUNC182_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC182_IN_INV_SEL_S  8
/** GPIO_SIG182_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG182_IN_SEL    (BIT(9))
#define GPIO_SIG182_IN_SEL_M  (GPIO_SIG182_IN_SEL_V << GPIO_SIG182_IN_SEL_S)
#define GPIO_SIG182_IN_SEL_V  0x00000001U
#define GPIO_SIG182_IN_SEL_S  9

/** GPIO_FUNC183_IN_SEL_CFG_REG register
 *  Configuration register for input signal 183
 */
#define GPIO_FUNC183_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5d0)
/** GPIO_FUNC183_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 183.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC183_IN_SEL    0x000000FFU
#define GPIO_FUNC183_IN_SEL_M  (GPIO_FUNC183_IN_SEL_V << GPIO_FUNC183_IN_SEL_S)
#define GPIO_FUNC183_IN_SEL_V  0x000000FFU
#define GPIO_FUNC183_IN_SEL_S  0
/** GPIO_FUNC183_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC183_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC183_IN_INV_SEL_M  (GPIO_FUNC183_IN_INV_SEL_V << GPIO_FUNC183_IN_INV_SEL_S)
#define GPIO_FUNC183_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC183_IN_INV_SEL_S  8
/** GPIO_SIG183_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG183_IN_SEL    (BIT(9))
#define GPIO_SIG183_IN_SEL_M  (GPIO_SIG183_IN_SEL_V << GPIO_SIG183_IN_SEL_S)
#define GPIO_SIG183_IN_SEL_V  0x00000001U
#define GPIO_SIG183_IN_SEL_S  9

/** GPIO_FUNC184_IN_SEL_CFG_REG register
 *  Configuration register for input signal 184
 */
#define GPIO_FUNC184_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5d4)
/** GPIO_FUNC184_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 184.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC184_IN_SEL    0x000000FFU
#define GPIO_FUNC184_IN_SEL_M  (GPIO_FUNC184_IN_SEL_V << GPIO_FUNC184_IN_SEL_S)
#define GPIO_FUNC184_IN_SEL_V  0x000000FFU
#define GPIO_FUNC184_IN_SEL_S  0
/** GPIO_FUNC184_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC184_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC184_IN_INV_SEL_M  (GPIO_FUNC184_IN_INV_SEL_V << GPIO_FUNC184_IN_INV_SEL_S)
#define GPIO_FUNC184_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC184_IN_INV_SEL_S  8
/** GPIO_SIG184_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG184_IN_SEL    (BIT(9))
#define GPIO_SIG184_IN_SEL_M  (GPIO_SIG184_IN_SEL_V << GPIO_SIG184_IN_SEL_S)
#define GPIO_SIG184_IN_SEL_V  0x00000001U
#define GPIO_SIG184_IN_SEL_S  9

/** GPIO_FUNC185_IN_SEL_CFG_REG register
 *  Configuration register for input signal 185
 */
#define GPIO_FUNC185_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5d8)
/** GPIO_FUNC185_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 185.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC185_IN_SEL    0x000000FFU
#define GPIO_FUNC185_IN_SEL_M  (GPIO_FUNC185_IN_SEL_V << GPIO_FUNC185_IN_SEL_S)
#define GPIO_FUNC185_IN_SEL_V  0x000000FFU
#define GPIO_FUNC185_IN_SEL_S  0
/** GPIO_FUNC185_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC185_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC185_IN_INV_SEL_M  (GPIO_FUNC185_IN_INV_SEL_V << GPIO_FUNC185_IN_INV_SEL_S)
#define GPIO_FUNC185_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC185_IN_INV_SEL_S  8
/** GPIO_SIG185_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG185_IN_SEL    (BIT(9))
#define GPIO_SIG185_IN_SEL_M  (GPIO_SIG185_IN_SEL_V << GPIO_SIG185_IN_SEL_S)
#define GPIO_SIG185_IN_SEL_V  0x00000001U
#define GPIO_SIG185_IN_SEL_S  9

/** GPIO_FUNC186_IN_SEL_CFG_REG register
 *  Configuration register for input signal 186
 */
#define GPIO_FUNC186_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5dc)
/** GPIO_FUNC186_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 186.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC186_IN_SEL    0x000000FFU
#define GPIO_FUNC186_IN_SEL_M  (GPIO_FUNC186_IN_SEL_V << GPIO_FUNC186_IN_SEL_S)
#define GPIO_FUNC186_IN_SEL_V  0x000000FFU
#define GPIO_FUNC186_IN_SEL_S  0
/** GPIO_FUNC186_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC186_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC186_IN_INV_SEL_M  (GPIO_FUNC186_IN_INV_SEL_V << GPIO_FUNC186_IN_INV_SEL_S)
#define GPIO_FUNC186_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC186_IN_INV_SEL_S  8
/** GPIO_SIG186_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG186_IN_SEL    (BIT(9))
#define GPIO_SIG186_IN_SEL_M  (GPIO_SIG186_IN_SEL_V << GPIO_SIG186_IN_SEL_S)
#define GPIO_SIG186_IN_SEL_V  0x00000001U
#define GPIO_SIG186_IN_SEL_S  9

/** GPIO_FUNC187_IN_SEL_CFG_REG register
 *  Configuration register for input signal 187
 */
#define GPIO_FUNC187_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5e0)
/** GPIO_FUNC187_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 187.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC187_IN_SEL    0x000000FFU
#define GPIO_FUNC187_IN_SEL_M  (GPIO_FUNC187_IN_SEL_V << GPIO_FUNC187_IN_SEL_S)
#define GPIO_FUNC187_IN_SEL_V  0x000000FFU
#define GPIO_FUNC187_IN_SEL_S  0
/** GPIO_FUNC187_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC187_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC187_IN_INV_SEL_M  (GPIO_FUNC187_IN_INV_SEL_V << GPIO_FUNC187_IN_INV_SEL_S)
#define GPIO_FUNC187_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC187_IN_INV_SEL_S  8
/** GPIO_SIG187_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG187_IN_SEL    (BIT(9))
#define GPIO_SIG187_IN_SEL_M  (GPIO_SIG187_IN_SEL_V << GPIO_SIG187_IN_SEL_S)
#define GPIO_SIG187_IN_SEL_V  0x00000001U
#define GPIO_SIG187_IN_SEL_S  9

/** GPIO_FUNC188_IN_SEL_CFG_REG register
 *  Configuration register for input signal 188
 */
#define GPIO_FUNC188_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5e4)
/** GPIO_FUNC188_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 188.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC188_IN_SEL    0x000000FFU
#define GPIO_FUNC188_IN_SEL_M  (GPIO_FUNC188_IN_SEL_V << GPIO_FUNC188_IN_SEL_S)
#define GPIO_FUNC188_IN_SEL_V  0x000000FFU
#define GPIO_FUNC188_IN_SEL_S  0
/** GPIO_FUNC188_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC188_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC188_IN_INV_SEL_M  (GPIO_FUNC188_IN_INV_SEL_V << GPIO_FUNC188_IN_INV_SEL_S)
#define GPIO_FUNC188_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC188_IN_INV_SEL_S  8
/** GPIO_SIG188_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG188_IN_SEL    (BIT(9))
#define GPIO_SIG188_IN_SEL_M  (GPIO_SIG188_IN_SEL_V << GPIO_SIG188_IN_SEL_S)
#define GPIO_SIG188_IN_SEL_V  0x00000001U
#define GPIO_SIG188_IN_SEL_S  9

/** GPIO_FUNC189_IN_SEL_CFG_REG register
 *  Configuration register for input signal 189
 */
#define GPIO_FUNC189_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5e8)
/** GPIO_FUNC189_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 189.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC189_IN_SEL    0x000000FFU
#define GPIO_FUNC189_IN_SEL_M  (GPIO_FUNC189_IN_SEL_V << GPIO_FUNC189_IN_SEL_S)
#define GPIO_FUNC189_IN_SEL_V  0x000000FFU
#define GPIO_FUNC189_IN_SEL_S  0
/** GPIO_FUNC189_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC189_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC189_IN_INV_SEL_M  (GPIO_FUNC189_IN_INV_SEL_V << GPIO_FUNC189_IN_INV_SEL_S)
#define GPIO_FUNC189_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC189_IN_INV_SEL_S  8
/** GPIO_SIG189_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG189_IN_SEL    (BIT(9))
#define GPIO_SIG189_IN_SEL_M  (GPIO_SIG189_IN_SEL_V << GPIO_SIG189_IN_SEL_S)
#define GPIO_SIG189_IN_SEL_V  0x00000001U
#define GPIO_SIG189_IN_SEL_S  9

/** GPIO_FUNC190_IN_SEL_CFG_REG register
 *  Configuration register for input signal 190
 */
#define GPIO_FUNC190_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5ec)
/** GPIO_FUNC190_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 190.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC190_IN_SEL    0x000000FFU
#define GPIO_FUNC190_IN_SEL_M  (GPIO_FUNC190_IN_SEL_V << GPIO_FUNC190_IN_SEL_S)
#define GPIO_FUNC190_IN_SEL_V  0x000000FFU
#define GPIO_FUNC190_IN_SEL_S  0
/** GPIO_FUNC190_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC190_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC190_IN_INV_SEL_M  (GPIO_FUNC190_IN_INV_SEL_V << GPIO_FUNC190_IN_INV_SEL_S)
#define GPIO_FUNC190_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC190_IN_INV_SEL_S  8
/** GPIO_SIG190_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG190_IN_SEL    (BIT(9))
#define GPIO_SIG190_IN_SEL_M  (GPIO_SIG190_IN_SEL_V << GPIO_SIG190_IN_SEL_S)
#define GPIO_SIG190_IN_SEL_V  0x00000001U
#define GPIO_SIG190_IN_SEL_S  9

/** GPIO_FUNC191_IN_SEL_CFG_REG register
 *  Configuration register for input signal 191
 */
#define GPIO_FUNC191_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5f0)
/** GPIO_FUNC191_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 191.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC191_IN_SEL    0x000000FFU
#define GPIO_FUNC191_IN_SEL_M  (GPIO_FUNC191_IN_SEL_V << GPIO_FUNC191_IN_SEL_S)
#define GPIO_FUNC191_IN_SEL_V  0x000000FFU
#define GPIO_FUNC191_IN_SEL_S  0
/** GPIO_FUNC191_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC191_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC191_IN_INV_SEL_M  (GPIO_FUNC191_IN_INV_SEL_V << GPIO_FUNC191_IN_INV_SEL_S)
#define GPIO_FUNC191_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC191_IN_INV_SEL_S  8
/** GPIO_SIG191_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG191_IN_SEL    (BIT(9))
#define GPIO_SIG191_IN_SEL_M  (GPIO_SIG191_IN_SEL_V << GPIO_SIG191_IN_SEL_S)
#define GPIO_SIG191_IN_SEL_V  0x00000001U
#define GPIO_SIG191_IN_SEL_S  9

/** GPIO_FUNC192_IN_SEL_CFG_REG register
 *  Configuration register for input signal 192
 */
#define GPIO_FUNC192_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5f4)
/** GPIO_FUNC192_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 192.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC192_IN_SEL    0x000000FFU
#define GPIO_FUNC192_IN_SEL_M  (GPIO_FUNC192_IN_SEL_V << GPIO_FUNC192_IN_SEL_S)
#define GPIO_FUNC192_IN_SEL_V  0x000000FFU
#define GPIO_FUNC192_IN_SEL_S  0
/** GPIO_FUNC192_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC192_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC192_IN_INV_SEL_M  (GPIO_FUNC192_IN_INV_SEL_V << GPIO_FUNC192_IN_INV_SEL_S)
#define GPIO_FUNC192_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC192_IN_INV_SEL_S  8
/** GPIO_SIG192_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG192_IN_SEL    (BIT(9))
#define GPIO_SIG192_IN_SEL_M  (GPIO_SIG192_IN_SEL_V << GPIO_SIG192_IN_SEL_S)
#define GPIO_SIG192_IN_SEL_V  0x00000001U
#define GPIO_SIG192_IN_SEL_S  9

/** GPIO_FUNC193_IN_SEL_CFG_REG register
 *  Configuration register for input signal 193
 */
#define GPIO_FUNC193_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5f8)
/** GPIO_FUNC193_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 193.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC193_IN_SEL    0x000000FFU
#define GPIO_FUNC193_IN_SEL_M  (GPIO_FUNC193_IN_SEL_V << GPIO_FUNC193_IN_SEL_S)
#define GPIO_FUNC193_IN_SEL_V  0x000000FFU
#define GPIO_FUNC193_IN_SEL_S  0
/** GPIO_FUNC193_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC193_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC193_IN_INV_SEL_M  (GPIO_FUNC193_IN_INV_SEL_V << GPIO_FUNC193_IN_INV_SEL_S)
#define GPIO_FUNC193_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC193_IN_INV_SEL_S  8
/** GPIO_SIG193_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG193_IN_SEL    (BIT(9))
#define GPIO_SIG193_IN_SEL_M  (GPIO_SIG193_IN_SEL_V << GPIO_SIG193_IN_SEL_S)
#define GPIO_SIG193_IN_SEL_V  0x00000001U
#define GPIO_SIG193_IN_SEL_S  9

/** GPIO_FUNC194_IN_SEL_CFG_REG register
 *  Configuration register for input signal 194
 */
#define GPIO_FUNC194_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x5fc)
/** GPIO_FUNC194_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 194.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC194_IN_SEL    0x000000FFU
#define GPIO_FUNC194_IN_SEL_M  (GPIO_FUNC194_IN_SEL_V << GPIO_FUNC194_IN_SEL_S)
#define GPIO_FUNC194_IN_SEL_V  0x000000FFU
#define GPIO_FUNC194_IN_SEL_S  0
/** GPIO_FUNC194_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC194_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC194_IN_INV_SEL_M  (GPIO_FUNC194_IN_INV_SEL_V << GPIO_FUNC194_IN_INV_SEL_S)
#define GPIO_FUNC194_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC194_IN_INV_SEL_S  8
/** GPIO_SIG194_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG194_IN_SEL    (BIT(9))
#define GPIO_SIG194_IN_SEL_M  (GPIO_SIG194_IN_SEL_V << GPIO_SIG194_IN_SEL_S)
#define GPIO_SIG194_IN_SEL_V  0x00000001U
#define GPIO_SIG194_IN_SEL_S  9

/** GPIO_FUNC195_IN_SEL_CFG_REG register
 *  Configuration register for input signal 195
 */
#define GPIO_FUNC195_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x600)
/** GPIO_FUNC195_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 195.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC195_IN_SEL    0x000000FFU
#define GPIO_FUNC195_IN_SEL_M  (GPIO_FUNC195_IN_SEL_V << GPIO_FUNC195_IN_SEL_S)
#define GPIO_FUNC195_IN_SEL_V  0x000000FFU
#define GPIO_FUNC195_IN_SEL_S  0
/** GPIO_FUNC195_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC195_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC195_IN_INV_SEL_M  (GPIO_FUNC195_IN_INV_SEL_V << GPIO_FUNC195_IN_INV_SEL_S)
#define GPIO_FUNC195_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC195_IN_INV_SEL_S  8
/** GPIO_SIG195_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG195_IN_SEL    (BIT(9))
#define GPIO_SIG195_IN_SEL_M  (GPIO_SIG195_IN_SEL_V << GPIO_SIG195_IN_SEL_S)
#define GPIO_SIG195_IN_SEL_V  0x00000001U
#define GPIO_SIG195_IN_SEL_S  9

/** GPIO_FUNC196_IN_SEL_CFG_REG register
 *  Configuration register for input signal 196
 */
#define GPIO_FUNC196_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x604)
/** GPIO_FUNC196_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 196.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC196_IN_SEL    0x000000FFU
#define GPIO_FUNC196_IN_SEL_M  (GPIO_FUNC196_IN_SEL_V << GPIO_FUNC196_IN_SEL_S)
#define GPIO_FUNC196_IN_SEL_V  0x000000FFU
#define GPIO_FUNC196_IN_SEL_S  0
/** GPIO_FUNC196_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC196_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC196_IN_INV_SEL_M  (GPIO_FUNC196_IN_INV_SEL_V << GPIO_FUNC196_IN_INV_SEL_S)
#define GPIO_FUNC196_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC196_IN_INV_SEL_S  8
/** GPIO_SIG196_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG196_IN_SEL    (BIT(9))
#define GPIO_SIG196_IN_SEL_M  (GPIO_SIG196_IN_SEL_V << GPIO_SIG196_IN_SEL_S)
#define GPIO_SIG196_IN_SEL_V  0x00000001U
#define GPIO_SIG196_IN_SEL_S  9

/** GPIO_FUNC197_IN_SEL_CFG_REG register
 *  Configuration register for input signal 197
 */
#define GPIO_FUNC197_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x608)
/** GPIO_FUNC197_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 197.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC197_IN_SEL    0x000000FFU
#define GPIO_FUNC197_IN_SEL_M  (GPIO_FUNC197_IN_SEL_V << GPIO_FUNC197_IN_SEL_S)
#define GPIO_FUNC197_IN_SEL_V  0x000000FFU
#define GPIO_FUNC197_IN_SEL_S  0
/** GPIO_FUNC197_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC197_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC197_IN_INV_SEL_M  (GPIO_FUNC197_IN_INV_SEL_V << GPIO_FUNC197_IN_INV_SEL_S)
#define GPIO_FUNC197_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC197_IN_INV_SEL_S  8
/** GPIO_SIG197_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG197_IN_SEL    (BIT(9))
#define GPIO_SIG197_IN_SEL_M  (GPIO_SIG197_IN_SEL_V << GPIO_SIG197_IN_SEL_S)
#define GPIO_SIG197_IN_SEL_V  0x00000001U
#define GPIO_SIG197_IN_SEL_S  9

/** GPIO_FUNC198_IN_SEL_CFG_REG register
 *  Configuration register for input signal 198
 */
#define GPIO_FUNC198_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x60c)
/** GPIO_FUNC198_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 198.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC198_IN_SEL    0x000000FFU
#define GPIO_FUNC198_IN_SEL_M  (GPIO_FUNC198_IN_SEL_V << GPIO_FUNC198_IN_SEL_S)
#define GPIO_FUNC198_IN_SEL_V  0x000000FFU
#define GPIO_FUNC198_IN_SEL_S  0
/** GPIO_FUNC198_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC198_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC198_IN_INV_SEL_M  (GPIO_FUNC198_IN_INV_SEL_V << GPIO_FUNC198_IN_INV_SEL_S)
#define GPIO_FUNC198_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC198_IN_INV_SEL_S  8
/** GPIO_SIG198_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG198_IN_SEL    (BIT(9))
#define GPIO_SIG198_IN_SEL_M  (GPIO_SIG198_IN_SEL_V << GPIO_SIG198_IN_SEL_S)
#define GPIO_SIG198_IN_SEL_V  0x00000001U
#define GPIO_SIG198_IN_SEL_S  9

/** GPIO_FUNC199_IN_SEL_CFG_REG register
 *  Configuration register for input signal 199
 */
#define GPIO_FUNC199_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x610)
/** GPIO_FUNC199_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 199.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC199_IN_SEL    0x000000FFU
#define GPIO_FUNC199_IN_SEL_M  (GPIO_FUNC199_IN_SEL_V << GPIO_FUNC199_IN_SEL_S)
#define GPIO_FUNC199_IN_SEL_V  0x000000FFU
#define GPIO_FUNC199_IN_SEL_S  0
/** GPIO_FUNC199_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC199_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC199_IN_INV_SEL_M  (GPIO_FUNC199_IN_INV_SEL_V << GPIO_FUNC199_IN_INV_SEL_S)
#define GPIO_FUNC199_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC199_IN_INV_SEL_S  8
/** GPIO_SIG199_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG199_IN_SEL    (BIT(9))
#define GPIO_SIG199_IN_SEL_M  (GPIO_SIG199_IN_SEL_V << GPIO_SIG199_IN_SEL_S)
#define GPIO_SIG199_IN_SEL_V  0x00000001U
#define GPIO_SIG199_IN_SEL_S  9

/** GPIO_FUNC200_IN_SEL_CFG_REG register
 *  Configuration register for input signal 200
 */
#define GPIO_FUNC200_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x614)
/** GPIO_FUNC200_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 200.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC200_IN_SEL    0x000000FFU
#define GPIO_FUNC200_IN_SEL_M  (GPIO_FUNC200_IN_SEL_V << GPIO_FUNC200_IN_SEL_S)
#define GPIO_FUNC200_IN_SEL_V  0x000000FFU
#define GPIO_FUNC200_IN_SEL_S  0
/** GPIO_FUNC200_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC200_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC200_IN_INV_SEL_M  (GPIO_FUNC200_IN_INV_SEL_V << GPIO_FUNC200_IN_INV_SEL_S)
#define GPIO_FUNC200_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC200_IN_INV_SEL_S  8
/** GPIO_SIG200_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG200_IN_SEL    (BIT(9))
#define GPIO_SIG200_IN_SEL_M  (GPIO_SIG200_IN_SEL_V << GPIO_SIG200_IN_SEL_S)
#define GPIO_SIG200_IN_SEL_V  0x00000001U
#define GPIO_SIG200_IN_SEL_S  9

/** GPIO_FUNC201_IN_SEL_CFG_REG register
 *  Configuration register for input signal 201
 */
#define GPIO_FUNC201_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x618)
/** GPIO_FUNC201_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 201.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC201_IN_SEL    0x000000FFU
#define GPIO_FUNC201_IN_SEL_M  (GPIO_FUNC201_IN_SEL_V << GPIO_FUNC201_IN_SEL_S)
#define GPIO_FUNC201_IN_SEL_V  0x000000FFU
#define GPIO_FUNC201_IN_SEL_S  0
/** GPIO_FUNC201_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC201_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC201_IN_INV_SEL_M  (GPIO_FUNC201_IN_INV_SEL_V << GPIO_FUNC201_IN_INV_SEL_S)
#define GPIO_FUNC201_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC201_IN_INV_SEL_S  8
/** GPIO_SIG201_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG201_IN_SEL    (BIT(9))
#define GPIO_SIG201_IN_SEL_M  (GPIO_SIG201_IN_SEL_V << GPIO_SIG201_IN_SEL_S)
#define GPIO_SIG201_IN_SEL_V  0x00000001U
#define GPIO_SIG201_IN_SEL_S  9

/** GPIO_FUNC202_IN_SEL_CFG_REG register
 *  Configuration register for input signal 202
 */
#define GPIO_FUNC202_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x61c)
/** GPIO_FUNC202_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 202.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC202_IN_SEL    0x000000FFU
#define GPIO_FUNC202_IN_SEL_M  (GPIO_FUNC202_IN_SEL_V << GPIO_FUNC202_IN_SEL_S)
#define GPIO_FUNC202_IN_SEL_V  0x000000FFU
#define GPIO_FUNC202_IN_SEL_S  0
/** GPIO_FUNC202_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC202_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC202_IN_INV_SEL_M  (GPIO_FUNC202_IN_INV_SEL_V << GPIO_FUNC202_IN_INV_SEL_S)
#define GPIO_FUNC202_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC202_IN_INV_SEL_S  8
/** GPIO_SIG202_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG202_IN_SEL    (BIT(9))
#define GPIO_SIG202_IN_SEL_M  (GPIO_SIG202_IN_SEL_V << GPIO_SIG202_IN_SEL_S)
#define GPIO_SIG202_IN_SEL_V  0x00000001U
#define GPIO_SIG202_IN_SEL_S  9

/** GPIO_FUNC203_IN_SEL_CFG_REG register
 *  Configuration register for input signal 203
 */
#define GPIO_FUNC203_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x620)
/** GPIO_FUNC203_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 203.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC203_IN_SEL    0x000000FFU
#define GPIO_FUNC203_IN_SEL_M  (GPIO_FUNC203_IN_SEL_V << GPIO_FUNC203_IN_SEL_S)
#define GPIO_FUNC203_IN_SEL_V  0x000000FFU
#define GPIO_FUNC203_IN_SEL_S  0
/** GPIO_FUNC203_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC203_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC203_IN_INV_SEL_M  (GPIO_FUNC203_IN_INV_SEL_V << GPIO_FUNC203_IN_INV_SEL_S)
#define GPIO_FUNC203_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC203_IN_INV_SEL_S  8
/** GPIO_SIG203_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG203_IN_SEL    (BIT(9))
#define GPIO_SIG203_IN_SEL_M  (GPIO_SIG203_IN_SEL_V << GPIO_SIG203_IN_SEL_S)
#define GPIO_SIG203_IN_SEL_V  0x00000001U
#define GPIO_SIG203_IN_SEL_S  9

/** GPIO_FUNC204_IN_SEL_CFG_REG register
 *  Configuration register for input signal 204
 */
#define GPIO_FUNC204_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x624)
/** GPIO_FUNC204_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 204.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC204_IN_SEL    0x000000FFU
#define GPIO_FUNC204_IN_SEL_M  (GPIO_FUNC204_IN_SEL_V << GPIO_FUNC204_IN_SEL_S)
#define GPIO_FUNC204_IN_SEL_V  0x000000FFU
#define GPIO_FUNC204_IN_SEL_S  0
/** GPIO_FUNC204_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC204_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC204_IN_INV_SEL_M  (GPIO_FUNC204_IN_INV_SEL_V << GPIO_FUNC204_IN_INV_SEL_S)
#define GPIO_FUNC204_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC204_IN_INV_SEL_S  8
/** GPIO_SIG204_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG204_IN_SEL    (BIT(9))
#define GPIO_SIG204_IN_SEL_M  (GPIO_SIG204_IN_SEL_V << GPIO_SIG204_IN_SEL_S)
#define GPIO_SIG204_IN_SEL_V  0x00000001U
#define GPIO_SIG204_IN_SEL_S  9

/** GPIO_FUNC205_IN_SEL_CFG_REG register
 *  Configuration register for input signal 205
 */
#define GPIO_FUNC205_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x628)
/** GPIO_FUNC205_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 205.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC205_IN_SEL    0x000000FFU
#define GPIO_FUNC205_IN_SEL_M  (GPIO_FUNC205_IN_SEL_V << GPIO_FUNC205_IN_SEL_S)
#define GPIO_FUNC205_IN_SEL_V  0x000000FFU
#define GPIO_FUNC205_IN_SEL_S  0
/** GPIO_FUNC205_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC205_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC205_IN_INV_SEL_M  (GPIO_FUNC205_IN_INV_SEL_V << GPIO_FUNC205_IN_INV_SEL_S)
#define GPIO_FUNC205_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC205_IN_INV_SEL_S  8
/** GPIO_SIG205_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG205_IN_SEL    (BIT(9))
#define GPIO_SIG205_IN_SEL_M  (GPIO_SIG205_IN_SEL_V << GPIO_SIG205_IN_SEL_S)
#define GPIO_SIG205_IN_SEL_V  0x00000001U
#define GPIO_SIG205_IN_SEL_S  9

/** GPIO_FUNC206_IN_SEL_CFG_REG register
 *  Configuration register for input signal 206
 */
#define GPIO_FUNC206_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x62c)
/** GPIO_FUNC206_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 206.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC206_IN_SEL    0x000000FFU
#define GPIO_FUNC206_IN_SEL_M  (GPIO_FUNC206_IN_SEL_V << GPIO_FUNC206_IN_SEL_S)
#define GPIO_FUNC206_IN_SEL_V  0x000000FFU
#define GPIO_FUNC206_IN_SEL_S  0
/** GPIO_FUNC206_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC206_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC206_IN_INV_SEL_M  (GPIO_FUNC206_IN_INV_SEL_V << GPIO_FUNC206_IN_INV_SEL_S)
#define GPIO_FUNC206_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC206_IN_INV_SEL_S  8
/** GPIO_SIG206_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG206_IN_SEL    (BIT(9))
#define GPIO_SIG206_IN_SEL_M  (GPIO_SIG206_IN_SEL_V << GPIO_SIG206_IN_SEL_S)
#define GPIO_SIG206_IN_SEL_V  0x00000001U
#define GPIO_SIG206_IN_SEL_S  9

/** GPIO_FUNC207_IN_SEL_CFG_REG register
 *  Configuration register for input signal 207
 */
#define GPIO_FUNC207_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x630)
/** GPIO_FUNC207_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 207.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC207_IN_SEL    0x000000FFU
#define GPIO_FUNC207_IN_SEL_M  (GPIO_FUNC207_IN_SEL_V << GPIO_FUNC207_IN_SEL_S)
#define GPIO_FUNC207_IN_SEL_V  0x000000FFU
#define GPIO_FUNC207_IN_SEL_S  0
/** GPIO_FUNC207_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC207_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC207_IN_INV_SEL_M  (GPIO_FUNC207_IN_INV_SEL_V << GPIO_FUNC207_IN_INV_SEL_S)
#define GPIO_FUNC207_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC207_IN_INV_SEL_S  8
/** GPIO_SIG207_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG207_IN_SEL    (BIT(9))
#define GPIO_SIG207_IN_SEL_M  (GPIO_SIG207_IN_SEL_V << GPIO_SIG207_IN_SEL_S)
#define GPIO_SIG207_IN_SEL_V  0x00000001U
#define GPIO_SIG207_IN_SEL_S  9

/** GPIO_FUNC208_IN_SEL_CFG_REG register
 *  Configuration register for input signal 208
 */
#define GPIO_FUNC208_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x634)
/** GPIO_FUNC208_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 208.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC208_IN_SEL    0x000000FFU
#define GPIO_FUNC208_IN_SEL_M  (GPIO_FUNC208_IN_SEL_V << GPIO_FUNC208_IN_SEL_S)
#define GPIO_FUNC208_IN_SEL_V  0x000000FFU
#define GPIO_FUNC208_IN_SEL_S  0
/** GPIO_FUNC208_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC208_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC208_IN_INV_SEL_M  (GPIO_FUNC208_IN_INV_SEL_V << GPIO_FUNC208_IN_INV_SEL_S)
#define GPIO_FUNC208_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC208_IN_INV_SEL_S  8
/** GPIO_SIG208_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG208_IN_SEL    (BIT(9))
#define GPIO_SIG208_IN_SEL_M  (GPIO_SIG208_IN_SEL_V << GPIO_SIG208_IN_SEL_S)
#define GPIO_SIG208_IN_SEL_V  0x00000001U
#define GPIO_SIG208_IN_SEL_S  9

/** GPIO_FUNC209_IN_SEL_CFG_REG register
 *  Configuration register for input signal 209
 */
#define GPIO_FUNC209_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x638)
/** GPIO_FUNC209_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 209.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC209_IN_SEL    0x000000FFU
#define GPIO_FUNC209_IN_SEL_M  (GPIO_FUNC209_IN_SEL_V << GPIO_FUNC209_IN_SEL_S)
#define GPIO_FUNC209_IN_SEL_V  0x000000FFU
#define GPIO_FUNC209_IN_SEL_S  0
/** GPIO_FUNC209_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC209_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC209_IN_INV_SEL_M  (GPIO_FUNC209_IN_INV_SEL_V << GPIO_FUNC209_IN_INV_SEL_S)
#define GPIO_FUNC209_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC209_IN_INV_SEL_S  8
/** GPIO_SIG209_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG209_IN_SEL    (BIT(9))
#define GPIO_SIG209_IN_SEL_M  (GPIO_SIG209_IN_SEL_V << GPIO_SIG209_IN_SEL_S)
#define GPIO_SIG209_IN_SEL_V  0x00000001U
#define GPIO_SIG209_IN_SEL_S  9

/** GPIO_FUNC210_IN_SEL_CFG_REG register
 *  Configuration register for input signal 210
 */
#define GPIO_FUNC210_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x63c)
/** GPIO_FUNC210_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 210.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC210_IN_SEL    0x000000FFU
#define GPIO_FUNC210_IN_SEL_M  (GPIO_FUNC210_IN_SEL_V << GPIO_FUNC210_IN_SEL_S)
#define GPIO_FUNC210_IN_SEL_V  0x000000FFU
#define GPIO_FUNC210_IN_SEL_S  0
/** GPIO_FUNC210_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC210_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC210_IN_INV_SEL_M  (GPIO_FUNC210_IN_INV_SEL_V << GPIO_FUNC210_IN_INV_SEL_S)
#define GPIO_FUNC210_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC210_IN_INV_SEL_S  8
/** GPIO_SIG210_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG210_IN_SEL    (BIT(9))
#define GPIO_SIG210_IN_SEL_M  (GPIO_SIG210_IN_SEL_V << GPIO_SIG210_IN_SEL_S)
#define GPIO_SIG210_IN_SEL_V  0x00000001U
#define GPIO_SIG210_IN_SEL_S  9

/** GPIO_FUNC211_IN_SEL_CFG_REG register
 *  Configuration register for input signal 211
 */
#define GPIO_FUNC211_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x640)
/** GPIO_FUNC211_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 211.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC211_IN_SEL    0x000000FFU
#define GPIO_FUNC211_IN_SEL_M  (GPIO_FUNC211_IN_SEL_V << GPIO_FUNC211_IN_SEL_S)
#define GPIO_FUNC211_IN_SEL_V  0x000000FFU
#define GPIO_FUNC211_IN_SEL_S  0
/** GPIO_FUNC211_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC211_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC211_IN_INV_SEL_M  (GPIO_FUNC211_IN_INV_SEL_V << GPIO_FUNC211_IN_INV_SEL_S)
#define GPIO_FUNC211_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC211_IN_INV_SEL_S  8
/** GPIO_SIG211_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG211_IN_SEL    (BIT(9))
#define GPIO_SIG211_IN_SEL_M  (GPIO_SIG211_IN_SEL_V << GPIO_SIG211_IN_SEL_S)
#define GPIO_SIG211_IN_SEL_V  0x00000001U
#define GPIO_SIG211_IN_SEL_S  9

/** GPIO_FUNC214_IN_SEL_CFG_REG register
 *  Configuration register for input signal 214
 */
#define GPIO_FUNC214_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x64c)
/** GPIO_FUNC214_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 214.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC214_IN_SEL    0x000000FFU
#define GPIO_FUNC214_IN_SEL_M  (GPIO_FUNC214_IN_SEL_V << GPIO_FUNC214_IN_SEL_S)
#define GPIO_FUNC214_IN_SEL_V  0x000000FFU
#define GPIO_FUNC214_IN_SEL_S  0
/** GPIO_FUNC214_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC214_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC214_IN_INV_SEL_M  (GPIO_FUNC214_IN_INV_SEL_V << GPIO_FUNC214_IN_INV_SEL_S)
#define GPIO_FUNC214_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC214_IN_INV_SEL_S  8
/** GPIO_SIG214_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG214_IN_SEL    (BIT(9))
#define GPIO_SIG214_IN_SEL_M  (GPIO_SIG214_IN_SEL_V << GPIO_SIG214_IN_SEL_S)
#define GPIO_SIG214_IN_SEL_V  0x00000001U
#define GPIO_SIG214_IN_SEL_S  9

/** GPIO_FUNC215_IN_SEL_CFG_REG register
 *  Configuration register for input signal 215
 */
#define GPIO_FUNC215_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x650)
/** GPIO_FUNC215_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 215.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC215_IN_SEL    0x000000FFU
#define GPIO_FUNC215_IN_SEL_M  (GPIO_FUNC215_IN_SEL_V << GPIO_FUNC215_IN_SEL_S)
#define GPIO_FUNC215_IN_SEL_V  0x000000FFU
#define GPIO_FUNC215_IN_SEL_S  0
/** GPIO_FUNC215_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC215_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC215_IN_INV_SEL_M  (GPIO_FUNC215_IN_INV_SEL_V << GPIO_FUNC215_IN_INV_SEL_S)
#define GPIO_FUNC215_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC215_IN_INV_SEL_S  8
/** GPIO_SIG215_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG215_IN_SEL    (BIT(9))
#define GPIO_SIG215_IN_SEL_M  (GPIO_SIG215_IN_SEL_V << GPIO_SIG215_IN_SEL_S)
#define GPIO_SIG215_IN_SEL_V  0x00000001U
#define GPIO_SIG215_IN_SEL_S  9

/** GPIO_FUNC216_IN_SEL_CFG_REG register
 *  Configuration register for input signal 216
 */
#define GPIO_FUNC216_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x654)
/** GPIO_FUNC216_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 216.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC216_IN_SEL    0x000000FFU
#define GPIO_FUNC216_IN_SEL_M  (GPIO_FUNC216_IN_SEL_V << GPIO_FUNC216_IN_SEL_S)
#define GPIO_FUNC216_IN_SEL_V  0x000000FFU
#define GPIO_FUNC216_IN_SEL_S  0
/** GPIO_FUNC216_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC216_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC216_IN_INV_SEL_M  (GPIO_FUNC216_IN_INV_SEL_V << GPIO_FUNC216_IN_INV_SEL_S)
#define GPIO_FUNC216_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC216_IN_INV_SEL_S  8
/** GPIO_SIG216_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG216_IN_SEL    (BIT(9))
#define GPIO_SIG216_IN_SEL_M  (GPIO_SIG216_IN_SEL_V << GPIO_SIG216_IN_SEL_S)
#define GPIO_SIG216_IN_SEL_V  0x00000001U
#define GPIO_SIG216_IN_SEL_S  9

/** GPIO_FUNC217_IN_SEL_CFG_REG register
 *  Configuration register for input signal 217
 */
#define GPIO_FUNC217_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x658)
/** GPIO_FUNC217_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 217.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC217_IN_SEL    0x000000FFU
#define GPIO_FUNC217_IN_SEL_M  (GPIO_FUNC217_IN_SEL_V << GPIO_FUNC217_IN_SEL_S)
#define GPIO_FUNC217_IN_SEL_V  0x000000FFU
#define GPIO_FUNC217_IN_SEL_S  0
/** GPIO_FUNC217_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC217_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC217_IN_INV_SEL_M  (GPIO_FUNC217_IN_INV_SEL_V << GPIO_FUNC217_IN_INV_SEL_S)
#define GPIO_FUNC217_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC217_IN_INV_SEL_S  8
/** GPIO_SIG217_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG217_IN_SEL    (BIT(9))
#define GPIO_SIG217_IN_SEL_M  (GPIO_SIG217_IN_SEL_V << GPIO_SIG217_IN_SEL_S)
#define GPIO_SIG217_IN_SEL_V  0x00000001U
#define GPIO_SIG217_IN_SEL_S  9

/** GPIO_FUNC218_IN_SEL_CFG_REG register
 *  Configuration register for input signal 218
 */
#define GPIO_FUNC218_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x65c)
/** GPIO_FUNC218_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 218.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC218_IN_SEL    0x000000FFU
#define GPIO_FUNC218_IN_SEL_M  (GPIO_FUNC218_IN_SEL_V << GPIO_FUNC218_IN_SEL_S)
#define GPIO_FUNC218_IN_SEL_V  0x000000FFU
#define GPIO_FUNC218_IN_SEL_S  0
/** GPIO_FUNC218_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC218_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC218_IN_INV_SEL_M  (GPIO_FUNC218_IN_INV_SEL_V << GPIO_FUNC218_IN_INV_SEL_S)
#define GPIO_FUNC218_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC218_IN_INV_SEL_S  8
/** GPIO_SIG218_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG218_IN_SEL    (BIT(9))
#define GPIO_SIG218_IN_SEL_M  (GPIO_SIG218_IN_SEL_V << GPIO_SIG218_IN_SEL_S)
#define GPIO_SIG218_IN_SEL_V  0x00000001U
#define GPIO_SIG218_IN_SEL_S  9

/** GPIO_FUNC219_IN_SEL_CFG_REG register
 *  Configuration register for input signal 219
 */
#define GPIO_FUNC219_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x660)
/** GPIO_FUNC219_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 219.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC219_IN_SEL    0x000000FFU
#define GPIO_FUNC219_IN_SEL_M  (GPIO_FUNC219_IN_SEL_V << GPIO_FUNC219_IN_SEL_S)
#define GPIO_FUNC219_IN_SEL_V  0x000000FFU
#define GPIO_FUNC219_IN_SEL_S  0
/** GPIO_FUNC219_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC219_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC219_IN_INV_SEL_M  (GPIO_FUNC219_IN_INV_SEL_V << GPIO_FUNC219_IN_INV_SEL_S)
#define GPIO_FUNC219_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC219_IN_INV_SEL_S  8
/** GPIO_SIG219_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG219_IN_SEL    (BIT(9))
#define GPIO_SIG219_IN_SEL_M  (GPIO_SIG219_IN_SEL_V << GPIO_SIG219_IN_SEL_S)
#define GPIO_SIG219_IN_SEL_V  0x00000001U
#define GPIO_SIG219_IN_SEL_S  9

/** GPIO_FUNC220_IN_SEL_CFG_REG register
 *  Configuration register for input signal 220
 */
#define GPIO_FUNC220_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x664)
/** GPIO_FUNC220_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 220.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC220_IN_SEL    0x000000FFU
#define GPIO_FUNC220_IN_SEL_M  (GPIO_FUNC220_IN_SEL_V << GPIO_FUNC220_IN_SEL_S)
#define GPIO_FUNC220_IN_SEL_V  0x000000FFU
#define GPIO_FUNC220_IN_SEL_S  0
/** GPIO_FUNC220_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC220_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC220_IN_INV_SEL_M  (GPIO_FUNC220_IN_INV_SEL_V << GPIO_FUNC220_IN_INV_SEL_S)
#define GPIO_FUNC220_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC220_IN_INV_SEL_S  8
/** GPIO_SIG220_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG220_IN_SEL    (BIT(9))
#define GPIO_SIG220_IN_SEL_M  (GPIO_SIG220_IN_SEL_V << GPIO_SIG220_IN_SEL_S)
#define GPIO_SIG220_IN_SEL_V  0x00000001U
#define GPIO_SIG220_IN_SEL_S  9

/** GPIO_FUNC221_IN_SEL_CFG_REG register
 *  Configuration register for input signal 221
 */
#define GPIO_FUNC221_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x668)
/** GPIO_FUNC221_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 221.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC221_IN_SEL    0x000000FFU
#define GPIO_FUNC221_IN_SEL_M  (GPIO_FUNC221_IN_SEL_V << GPIO_FUNC221_IN_SEL_S)
#define GPIO_FUNC221_IN_SEL_V  0x000000FFU
#define GPIO_FUNC221_IN_SEL_S  0
/** GPIO_FUNC221_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC221_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC221_IN_INV_SEL_M  (GPIO_FUNC221_IN_INV_SEL_V << GPIO_FUNC221_IN_INV_SEL_S)
#define GPIO_FUNC221_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC221_IN_INV_SEL_S  8
/** GPIO_SIG221_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG221_IN_SEL    (BIT(9))
#define GPIO_SIG221_IN_SEL_M  (GPIO_SIG221_IN_SEL_V << GPIO_SIG221_IN_SEL_S)
#define GPIO_SIG221_IN_SEL_V  0x00000001U
#define GPIO_SIG221_IN_SEL_S  9

/** GPIO_FUNC222_IN_SEL_CFG_REG register
 *  Configuration register for input signal 222
 */
#define GPIO_FUNC222_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x66c)
/** GPIO_FUNC222_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 222.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC222_IN_SEL    0x000000FFU
#define GPIO_FUNC222_IN_SEL_M  (GPIO_FUNC222_IN_SEL_V << GPIO_FUNC222_IN_SEL_S)
#define GPIO_FUNC222_IN_SEL_V  0x000000FFU
#define GPIO_FUNC222_IN_SEL_S  0
/** GPIO_FUNC222_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC222_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC222_IN_INV_SEL_M  (GPIO_FUNC222_IN_INV_SEL_V << GPIO_FUNC222_IN_INV_SEL_S)
#define GPIO_FUNC222_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC222_IN_INV_SEL_S  8
/** GPIO_SIG222_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG222_IN_SEL    (BIT(9))
#define GPIO_SIG222_IN_SEL_M  (GPIO_SIG222_IN_SEL_V << GPIO_SIG222_IN_SEL_S)
#define GPIO_SIG222_IN_SEL_V  0x00000001U
#define GPIO_SIG222_IN_SEL_S  9

/** GPIO_FUNC223_IN_SEL_CFG_REG register
 *  Configuration register for input signal 223
 */
#define GPIO_FUNC223_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x670)
/** GPIO_FUNC223_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 223.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC223_IN_SEL    0x000000FFU
#define GPIO_FUNC223_IN_SEL_M  (GPIO_FUNC223_IN_SEL_V << GPIO_FUNC223_IN_SEL_S)
#define GPIO_FUNC223_IN_SEL_V  0x000000FFU
#define GPIO_FUNC223_IN_SEL_S  0
/** GPIO_FUNC223_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC223_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC223_IN_INV_SEL_M  (GPIO_FUNC223_IN_INV_SEL_V << GPIO_FUNC223_IN_INV_SEL_S)
#define GPIO_FUNC223_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC223_IN_INV_SEL_S  8
/** GPIO_SIG223_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG223_IN_SEL    (BIT(9))
#define GPIO_SIG223_IN_SEL_M  (GPIO_SIG223_IN_SEL_V << GPIO_SIG223_IN_SEL_S)
#define GPIO_SIG223_IN_SEL_V  0x00000001U
#define GPIO_SIG223_IN_SEL_S  9

/** GPIO_FUNC224_IN_SEL_CFG_REG register
 *  Configuration register for input signal 224
 */
#define GPIO_FUNC224_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x674)
/** GPIO_FUNC224_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 224.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC224_IN_SEL    0x000000FFU
#define GPIO_FUNC224_IN_SEL_M  (GPIO_FUNC224_IN_SEL_V << GPIO_FUNC224_IN_SEL_S)
#define GPIO_FUNC224_IN_SEL_V  0x000000FFU
#define GPIO_FUNC224_IN_SEL_S  0
/** GPIO_FUNC224_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC224_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC224_IN_INV_SEL_M  (GPIO_FUNC224_IN_INV_SEL_V << GPIO_FUNC224_IN_INV_SEL_S)
#define GPIO_FUNC224_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC224_IN_INV_SEL_S  8
/** GPIO_SIG224_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG224_IN_SEL    (BIT(9))
#define GPIO_SIG224_IN_SEL_M  (GPIO_SIG224_IN_SEL_V << GPIO_SIG224_IN_SEL_S)
#define GPIO_SIG224_IN_SEL_V  0x00000001U
#define GPIO_SIG224_IN_SEL_S  9

/** GPIO_FUNC225_IN_SEL_CFG_REG register
 *  Configuration register for input signal 225
 */
#define GPIO_FUNC225_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x678)
/** GPIO_FUNC225_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 225.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC225_IN_SEL    0x000000FFU
#define GPIO_FUNC225_IN_SEL_M  (GPIO_FUNC225_IN_SEL_V << GPIO_FUNC225_IN_SEL_S)
#define GPIO_FUNC225_IN_SEL_V  0x000000FFU
#define GPIO_FUNC225_IN_SEL_S  0
/** GPIO_FUNC225_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC225_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC225_IN_INV_SEL_M  (GPIO_FUNC225_IN_INV_SEL_V << GPIO_FUNC225_IN_INV_SEL_S)
#define GPIO_FUNC225_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC225_IN_INV_SEL_S  8
/** GPIO_SIG225_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG225_IN_SEL    (BIT(9))
#define GPIO_SIG225_IN_SEL_M  (GPIO_SIG225_IN_SEL_V << GPIO_SIG225_IN_SEL_S)
#define GPIO_SIG225_IN_SEL_V  0x00000001U
#define GPIO_SIG225_IN_SEL_S  9

/** GPIO_FUNC226_IN_SEL_CFG_REG register
 *  Configuration register for input signal 226
 */
#define GPIO_FUNC226_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x67c)
/** GPIO_FUNC226_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 226.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC226_IN_SEL    0x000000FFU
#define GPIO_FUNC226_IN_SEL_M  (GPIO_FUNC226_IN_SEL_V << GPIO_FUNC226_IN_SEL_S)
#define GPIO_FUNC226_IN_SEL_V  0x000000FFU
#define GPIO_FUNC226_IN_SEL_S  0
/** GPIO_FUNC226_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC226_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC226_IN_INV_SEL_M  (GPIO_FUNC226_IN_INV_SEL_V << GPIO_FUNC226_IN_INV_SEL_S)
#define GPIO_FUNC226_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC226_IN_INV_SEL_S  8
/** GPIO_SIG226_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG226_IN_SEL    (BIT(9))
#define GPIO_SIG226_IN_SEL_M  (GPIO_SIG226_IN_SEL_V << GPIO_SIG226_IN_SEL_S)
#define GPIO_SIG226_IN_SEL_V  0x00000001U
#define GPIO_SIG226_IN_SEL_S  9

/** GPIO_FUNC227_IN_SEL_CFG_REG register
 *  Configuration register for input signal 227
 */
#define GPIO_FUNC227_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x680)
/** GPIO_FUNC227_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 227.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC227_IN_SEL    0x000000FFU
#define GPIO_FUNC227_IN_SEL_M  (GPIO_FUNC227_IN_SEL_V << GPIO_FUNC227_IN_SEL_S)
#define GPIO_FUNC227_IN_SEL_V  0x000000FFU
#define GPIO_FUNC227_IN_SEL_S  0
/** GPIO_FUNC227_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC227_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC227_IN_INV_SEL_M  (GPIO_FUNC227_IN_INV_SEL_V << GPIO_FUNC227_IN_INV_SEL_S)
#define GPIO_FUNC227_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC227_IN_INV_SEL_S  8
/** GPIO_SIG227_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG227_IN_SEL    (BIT(9))
#define GPIO_SIG227_IN_SEL_M  (GPIO_SIG227_IN_SEL_V << GPIO_SIG227_IN_SEL_S)
#define GPIO_SIG227_IN_SEL_V  0x00000001U
#define GPIO_SIG227_IN_SEL_S  9

/** GPIO_FUNC228_IN_SEL_CFG_REG register
 *  Configuration register for input signal 228
 */
#define GPIO_FUNC228_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x684)
/** GPIO_FUNC228_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 228.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC228_IN_SEL    0x000000FFU
#define GPIO_FUNC228_IN_SEL_M  (GPIO_FUNC228_IN_SEL_V << GPIO_FUNC228_IN_SEL_S)
#define GPIO_FUNC228_IN_SEL_V  0x000000FFU
#define GPIO_FUNC228_IN_SEL_S  0
/** GPIO_FUNC228_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC228_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC228_IN_INV_SEL_M  (GPIO_FUNC228_IN_INV_SEL_V << GPIO_FUNC228_IN_INV_SEL_S)
#define GPIO_FUNC228_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC228_IN_INV_SEL_S  8
/** GPIO_SIG228_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG228_IN_SEL    (BIT(9))
#define GPIO_SIG228_IN_SEL_M  (GPIO_SIG228_IN_SEL_V << GPIO_SIG228_IN_SEL_S)
#define GPIO_SIG228_IN_SEL_V  0x00000001U
#define GPIO_SIG228_IN_SEL_S  9

/** GPIO_FUNC229_IN_SEL_CFG_REG register
 *  Configuration register for input signal 229
 */
#define GPIO_FUNC229_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x688)
/** GPIO_FUNC229_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 229.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC229_IN_SEL    0x000000FFU
#define GPIO_FUNC229_IN_SEL_M  (GPIO_FUNC229_IN_SEL_V << GPIO_FUNC229_IN_SEL_S)
#define GPIO_FUNC229_IN_SEL_V  0x000000FFU
#define GPIO_FUNC229_IN_SEL_S  0
/** GPIO_FUNC229_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC229_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC229_IN_INV_SEL_M  (GPIO_FUNC229_IN_INV_SEL_V << GPIO_FUNC229_IN_INV_SEL_S)
#define GPIO_FUNC229_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC229_IN_INV_SEL_S  8
/** GPIO_SIG229_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG229_IN_SEL    (BIT(9))
#define GPIO_SIG229_IN_SEL_M  (GPIO_SIG229_IN_SEL_V << GPIO_SIG229_IN_SEL_S)
#define GPIO_SIG229_IN_SEL_V  0x00000001U
#define GPIO_SIG229_IN_SEL_S  9

/** GPIO_FUNC230_IN_SEL_CFG_REG register
 *  Configuration register for input signal 230
 */
#define GPIO_FUNC230_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x68c)
/** GPIO_FUNC230_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 230.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC230_IN_SEL    0x000000FFU
#define GPIO_FUNC230_IN_SEL_M  (GPIO_FUNC230_IN_SEL_V << GPIO_FUNC230_IN_SEL_S)
#define GPIO_FUNC230_IN_SEL_V  0x000000FFU
#define GPIO_FUNC230_IN_SEL_S  0
/** GPIO_FUNC230_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC230_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC230_IN_INV_SEL_M  (GPIO_FUNC230_IN_INV_SEL_V << GPIO_FUNC230_IN_INV_SEL_S)
#define GPIO_FUNC230_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC230_IN_INV_SEL_S  8
/** GPIO_SIG230_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG230_IN_SEL    (BIT(9))
#define GPIO_SIG230_IN_SEL_M  (GPIO_SIG230_IN_SEL_V << GPIO_SIG230_IN_SEL_S)
#define GPIO_SIG230_IN_SEL_V  0x00000001U
#define GPIO_SIG230_IN_SEL_S  9

/** GPIO_FUNC231_IN_SEL_CFG_REG register
 *  Configuration register for input signal 231
 */
#define GPIO_FUNC231_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x690)
/** GPIO_FUNC231_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 231.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC231_IN_SEL    0x000000FFU
#define GPIO_FUNC231_IN_SEL_M  (GPIO_FUNC231_IN_SEL_V << GPIO_FUNC231_IN_SEL_S)
#define GPIO_FUNC231_IN_SEL_V  0x000000FFU
#define GPIO_FUNC231_IN_SEL_S  0
/** GPIO_FUNC231_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC231_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC231_IN_INV_SEL_M  (GPIO_FUNC231_IN_INV_SEL_V << GPIO_FUNC231_IN_INV_SEL_S)
#define GPIO_FUNC231_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC231_IN_INV_SEL_S  8
/** GPIO_SIG231_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG231_IN_SEL    (BIT(9))
#define GPIO_SIG231_IN_SEL_M  (GPIO_SIG231_IN_SEL_V << GPIO_SIG231_IN_SEL_S)
#define GPIO_SIG231_IN_SEL_V  0x00000001U
#define GPIO_SIG231_IN_SEL_S  9

/** GPIO_FUNC232_IN_SEL_CFG_REG register
 *  Configuration register for input signal 232
 */
#define GPIO_FUNC232_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x694)
/** GPIO_FUNC232_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 232.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC232_IN_SEL    0x000000FFU
#define GPIO_FUNC232_IN_SEL_M  (GPIO_FUNC232_IN_SEL_V << GPIO_FUNC232_IN_SEL_S)
#define GPIO_FUNC232_IN_SEL_V  0x000000FFU
#define GPIO_FUNC232_IN_SEL_S  0
/** GPIO_FUNC232_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC232_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC232_IN_INV_SEL_M  (GPIO_FUNC232_IN_INV_SEL_V << GPIO_FUNC232_IN_INV_SEL_S)
#define GPIO_FUNC232_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC232_IN_INV_SEL_S  8
/** GPIO_SIG232_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG232_IN_SEL    (BIT(9))
#define GPIO_SIG232_IN_SEL_M  (GPIO_SIG232_IN_SEL_V << GPIO_SIG232_IN_SEL_S)
#define GPIO_SIG232_IN_SEL_V  0x00000001U
#define GPIO_SIG232_IN_SEL_S  9

/** GPIO_FUNC233_IN_SEL_CFG_REG register
 *  Configuration register for input signal 233
 */
#define GPIO_FUNC233_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x698)
/** GPIO_FUNC233_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 233.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC233_IN_SEL    0x000000FFU
#define GPIO_FUNC233_IN_SEL_M  (GPIO_FUNC233_IN_SEL_V << GPIO_FUNC233_IN_SEL_S)
#define GPIO_FUNC233_IN_SEL_V  0x000000FFU
#define GPIO_FUNC233_IN_SEL_S  0
/** GPIO_FUNC233_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC233_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC233_IN_INV_SEL_M  (GPIO_FUNC233_IN_INV_SEL_V << GPIO_FUNC233_IN_INV_SEL_S)
#define GPIO_FUNC233_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC233_IN_INV_SEL_S  8
/** GPIO_SIG233_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG233_IN_SEL    (BIT(9))
#define GPIO_SIG233_IN_SEL_M  (GPIO_SIG233_IN_SEL_V << GPIO_SIG233_IN_SEL_S)
#define GPIO_SIG233_IN_SEL_V  0x00000001U
#define GPIO_SIG233_IN_SEL_S  9

/** GPIO_FUNC234_IN_SEL_CFG_REG register
 *  Configuration register for input signal 234
 */
#define GPIO_FUNC234_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x69c)
/** GPIO_FUNC234_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 234.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC234_IN_SEL    0x000000FFU
#define GPIO_FUNC234_IN_SEL_M  (GPIO_FUNC234_IN_SEL_V << GPIO_FUNC234_IN_SEL_S)
#define GPIO_FUNC234_IN_SEL_V  0x000000FFU
#define GPIO_FUNC234_IN_SEL_S  0
/** GPIO_FUNC234_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC234_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC234_IN_INV_SEL_M  (GPIO_FUNC234_IN_INV_SEL_V << GPIO_FUNC234_IN_INV_SEL_S)
#define GPIO_FUNC234_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC234_IN_INV_SEL_S  8
/** GPIO_SIG234_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG234_IN_SEL    (BIT(9))
#define GPIO_SIG234_IN_SEL_M  (GPIO_SIG234_IN_SEL_V << GPIO_SIG234_IN_SEL_S)
#define GPIO_SIG234_IN_SEL_V  0x00000001U
#define GPIO_SIG234_IN_SEL_S  9

/** GPIO_FUNC235_IN_SEL_CFG_REG register
 *  Configuration register for input signal 235
 */
#define GPIO_FUNC235_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6a0)
/** GPIO_FUNC235_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 235.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC235_IN_SEL    0x000000FFU
#define GPIO_FUNC235_IN_SEL_M  (GPIO_FUNC235_IN_SEL_V << GPIO_FUNC235_IN_SEL_S)
#define GPIO_FUNC235_IN_SEL_V  0x000000FFU
#define GPIO_FUNC235_IN_SEL_S  0
/** GPIO_FUNC235_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC235_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC235_IN_INV_SEL_M  (GPIO_FUNC235_IN_INV_SEL_V << GPIO_FUNC235_IN_INV_SEL_S)
#define GPIO_FUNC235_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC235_IN_INV_SEL_S  8
/** GPIO_SIG235_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG235_IN_SEL    (BIT(9))
#define GPIO_SIG235_IN_SEL_M  (GPIO_SIG235_IN_SEL_V << GPIO_SIG235_IN_SEL_S)
#define GPIO_SIG235_IN_SEL_V  0x00000001U
#define GPIO_SIG235_IN_SEL_S  9

/** GPIO_FUNC236_IN_SEL_CFG_REG register
 *  Configuration register for input signal 236
 */
#define GPIO_FUNC236_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6a4)
/** GPIO_FUNC236_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 236.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC236_IN_SEL    0x000000FFU
#define GPIO_FUNC236_IN_SEL_M  (GPIO_FUNC236_IN_SEL_V << GPIO_FUNC236_IN_SEL_S)
#define GPIO_FUNC236_IN_SEL_V  0x000000FFU
#define GPIO_FUNC236_IN_SEL_S  0
/** GPIO_FUNC236_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC236_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC236_IN_INV_SEL_M  (GPIO_FUNC236_IN_INV_SEL_V << GPIO_FUNC236_IN_INV_SEL_S)
#define GPIO_FUNC236_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC236_IN_INV_SEL_S  8
/** GPIO_SIG236_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG236_IN_SEL    (BIT(9))
#define GPIO_SIG236_IN_SEL_M  (GPIO_SIG236_IN_SEL_V << GPIO_SIG236_IN_SEL_S)
#define GPIO_SIG236_IN_SEL_V  0x00000001U
#define GPIO_SIG236_IN_SEL_S  9

/** GPIO_FUNC237_IN_SEL_CFG_REG register
 *  Configuration register for input signal 237
 */
#define GPIO_FUNC237_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6a8)
/** GPIO_FUNC237_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 237.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC237_IN_SEL    0x000000FFU
#define GPIO_FUNC237_IN_SEL_M  (GPIO_FUNC237_IN_SEL_V << GPIO_FUNC237_IN_SEL_S)
#define GPIO_FUNC237_IN_SEL_V  0x000000FFU
#define GPIO_FUNC237_IN_SEL_S  0
/** GPIO_FUNC237_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC237_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC237_IN_INV_SEL_M  (GPIO_FUNC237_IN_INV_SEL_V << GPIO_FUNC237_IN_INV_SEL_S)
#define GPIO_FUNC237_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC237_IN_INV_SEL_S  8
/** GPIO_SIG237_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG237_IN_SEL    (BIT(9))
#define GPIO_SIG237_IN_SEL_M  (GPIO_SIG237_IN_SEL_V << GPIO_SIG237_IN_SEL_S)
#define GPIO_SIG237_IN_SEL_V  0x00000001U
#define GPIO_SIG237_IN_SEL_S  9

/** GPIO_FUNC238_IN_SEL_CFG_REG register
 *  Configuration register for input signal 238
 */
#define GPIO_FUNC238_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6ac)
/** GPIO_FUNC238_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 238.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC238_IN_SEL    0x000000FFU
#define GPIO_FUNC238_IN_SEL_M  (GPIO_FUNC238_IN_SEL_V << GPIO_FUNC238_IN_SEL_S)
#define GPIO_FUNC238_IN_SEL_V  0x000000FFU
#define GPIO_FUNC238_IN_SEL_S  0
/** GPIO_FUNC238_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC238_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC238_IN_INV_SEL_M  (GPIO_FUNC238_IN_INV_SEL_V << GPIO_FUNC238_IN_INV_SEL_S)
#define GPIO_FUNC238_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC238_IN_INV_SEL_S  8
/** GPIO_SIG238_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG238_IN_SEL    (BIT(9))
#define GPIO_SIG238_IN_SEL_M  (GPIO_SIG238_IN_SEL_V << GPIO_SIG238_IN_SEL_S)
#define GPIO_SIG238_IN_SEL_V  0x00000001U
#define GPIO_SIG238_IN_SEL_S  9

/** GPIO_FUNC239_IN_SEL_CFG_REG register
 *  Configuration register for input signal 239
 */
#define GPIO_FUNC239_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6b0)
/** GPIO_FUNC239_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 239.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC239_IN_SEL    0x000000FFU
#define GPIO_FUNC239_IN_SEL_M  (GPIO_FUNC239_IN_SEL_V << GPIO_FUNC239_IN_SEL_S)
#define GPIO_FUNC239_IN_SEL_V  0x000000FFU
#define GPIO_FUNC239_IN_SEL_S  0
/** GPIO_FUNC239_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC239_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC239_IN_INV_SEL_M  (GPIO_FUNC239_IN_INV_SEL_V << GPIO_FUNC239_IN_INV_SEL_S)
#define GPIO_FUNC239_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC239_IN_INV_SEL_S  8
/** GPIO_SIG239_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG239_IN_SEL    (BIT(9))
#define GPIO_SIG239_IN_SEL_M  (GPIO_SIG239_IN_SEL_V << GPIO_SIG239_IN_SEL_S)
#define GPIO_SIG239_IN_SEL_V  0x00000001U
#define GPIO_SIG239_IN_SEL_S  9

/** GPIO_FUNC240_IN_SEL_CFG_REG register
 *  Configuration register for input signal 240
 */
#define GPIO_FUNC240_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6b4)
/** GPIO_FUNC240_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 240.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC240_IN_SEL    0x000000FFU
#define GPIO_FUNC240_IN_SEL_M  (GPIO_FUNC240_IN_SEL_V << GPIO_FUNC240_IN_SEL_S)
#define GPIO_FUNC240_IN_SEL_V  0x000000FFU
#define GPIO_FUNC240_IN_SEL_S  0
/** GPIO_FUNC240_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC240_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC240_IN_INV_SEL_M  (GPIO_FUNC240_IN_INV_SEL_V << GPIO_FUNC240_IN_INV_SEL_S)
#define GPIO_FUNC240_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC240_IN_INV_SEL_S  8
/** GPIO_SIG240_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG240_IN_SEL    (BIT(9))
#define GPIO_SIG240_IN_SEL_M  (GPIO_SIG240_IN_SEL_V << GPIO_SIG240_IN_SEL_S)
#define GPIO_SIG240_IN_SEL_V  0x00000001U
#define GPIO_SIG240_IN_SEL_S  9

/** GPIO_FUNC241_IN_SEL_CFG_REG register
 *  Configuration register for input signal 241
 */
#define GPIO_FUNC241_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6b8)
/** GPIO_FUNC241_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 241.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC241_IN_SEL    0x000000FFU
#define GPIO_FUNC241_IN_SEL_M  (GPIO_FUNC241_IN_SEL_V << GPIO_FUNC241_IN_SEL_S)
#define GPIO_FUNC241_IN_SEL_V  0x000000FFU
#define GPIO_FUNC241_IN_SEL_S  0
/** GPIO_FUNC241_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC241_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC241_IN_INV_SEL_M  (GPIO_FUNC241_IN_INV_SEL_V << GPIO_FUNC241_IN_INV_SEL_S)
#define GPIO_FUNC241_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC241_IN_INV_SEL_S  8
/** GPIO_SIG241_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG241_IN_SEL    (BIT(9))
#define GPIO_SIG241_IN_SEL_M  (GPIO_SIG241_IN_SEL_V << GPIO_SIG241_IN_SEL_S)
#define GPIO_SIG241_IN_SEL_V  0x00000001U
#define GPIO_SIG241_IN_SEL_S  9

/** GPIO_FUNC242_IN_SEL_CFG_REG register
 *  Configuration register for input signal 242
 */
#define GPIO_FUNC242_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6bc)
/** GPIO_FUNC242_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 242.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC242_IN_SEL    0x000000FFU
#define GPIO_FUNC242_IN_SEL_M  (GPIO_FUNC242_IN_SEL_V << GPIO_FUNC242_IN_SEL_S)
#define GPIO_FUNC242_IN_SEL_V  0x000000FFU
#define GPIO_FUNC242_IN_SEL_S  0
/** GPIO_FUNC242_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC242_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC242_IN_INV_SEL_M  (GPIO_FUNC242_IN_INV_SEL_V << GPIO_FUNC242_IN_INV_SEL_S)
#define GPIO_FUNC242_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC242_IN_INV_SEL_S  8
/** GPIO_SIG242_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG242_IN_SEL    (BIT(9))
#define GPIO_SIG242_IN_SEL_M  (GPIO_SIG242_IN_SEL_V << GPIO_SIG242_IN_SEL_S)
#define GPIO_SIG242_IN_SEL_V  0x00000001U
#define GPIO_SIG242_IN_SEL_S  9

/** GPIO_FUNC243_IN_SEL_CFG_REG register
 *  Configuration register for input signal 243
 */
#define GPIO_FUNC243_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6c0)
/** GPIO_FUNC243_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 243.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC243_IN_SEL    0x000000FFU
#define GPIO_FUNC243_IN_SEL_M  (GPIO_FUNC243_IN_SEL_V << GPIO_FUNC243_IN_SEL_S)
#define GPIO_FUNC243_IN_SEL_V  0x000000FFU
#define GPIO_FUNC243_IN_SEL_S  0
/** GPIO_FUNC243_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC243_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC243_IN_INV_SEL_M  (GPIO_FUNC243_IN_INV_SEL_V << GPIO_FUNC243_IN_INV_SEL_S)
#define GPIO_FUNC243_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC243_IN_INV_SEL_S  8
/** GPIO_SIG243_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG243_IN_SEL    (BIT(9))
#define GPIO_SIG243_IN_SEL_M  (GPIO_SIG243_IN_SEL_V << GPIO_SIG243_IN_SEL_S)
#define GPIO_SIG243_IN_SEL_V  0x00000001U
#define GPIO_SIG243_IN_SEL_S  9

/** GPIO_FUNC244_IN_SEL_CFG_REG register
 *  Configuration register for input signal 244
 */
#define GPIO_FUNC244_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6c4)
/** GPIO_FUNC244_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 244.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC244_IN_SEL    0x000000FFU
#define GPIO_FUNC244_IN_SEL_M  (GPIO_FUNC244_IN_SEL_V << GPIO_FUNC244_IN_SEL_S)
#define GPIO_FUNC244_IN_SEL_V  0x000000FFU
#define GPIO_FUNC244_IN_SEL_S  0
/** GPIO_FUNC244_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC244_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC244_IN_INV_SEL_M  (GPIO_FUNC244_IN_INV_SEL_V << GPIO_FUNC244_IN_INV_SEL_S)
#define GPIO_FUNC244_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC244_IN_INV_SEL_S  8
/** GPIO_SIG244_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG244_IN_SEL    (BIT(9))
#define GPIO_SIG244_IN_SEL_M  (GPIO_SIG244_IN_SEL_V << GPIO_SIG244_IN_SEL_S)
#define GPIO_SIG244_IN_SEL_V  0x00000001U
#define GPIO_SIG244_IN_SEL_S  9

/** GPIO_FUNC245_IN_SEL_CFG_REG register
 *  Configuration register for input signal 245
 */
#define GPIO_FUNC245_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6c8)
/** GPIO_FUNC245_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 245.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC245_IN_SEL    0x000000FFU
#define GPIO_FUNC245_IN_SEL_M  (GPIO_FUNC245_IN_SEL_V << GPIO_FUNC245_IN_SEL_S)
#define GPIO_FUNC245_IN_SEL_V  0x000000FFU
#define GPIO_FUNC245_IN_SEL_S  0
/** GPIO_FUNC245_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC245_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC245_IN_INV_SEL_M  (GPIO_FUNC245_IN_INV_SEL_V << GPIO_FUNC245_IN_INV_SEL_S)
#define GPIO_FUNC245_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC245_IN_INV_SEL_S  8
/** GPIO_SIG245_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG245_IN_SEL    (BIT(9))
#define GPIO_SIG245_IN_SEL_M  (GPIO_SIG245_IN_SEL_V << GPIO_SIG245_IN_SEL_S)
#define GPIO_SIG245_IN_SEL_V  0x00000001U
#define GPIO_SIG245_IN_SEL_S  9

/** GPIO_FUNC246_IN_SEL_CFG_REG register
 *  Configuration register for input signal 246
 */
#define GPIO_FUNC246_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6cc)
/** GPIO_FUNC246_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 246.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC246_IN_SEL    0x000000FFU
#define GPIO_FUNC246_IN_SEL_M  (GPIO_FUNC246_IN_SEL_V << GPIO_FUNC246_IN_SEL_S)
#define GPIO_FUNC246_IN_SEL_V  0x000000FFU
#define GPIO_FUNC246_IN_SEL_S  0
/** GPIO_FUNC246_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC246_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC246_IN_INV_SEL_M  (GPIO_FUNC246_IN_INV_SEL_V << GPIO_FUNC246_IN_INV_SEL_S)
#define GPIO_FUNC246_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC246_IN_INV_SEL_S  8
/** GPIO_SIG246_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG246_IN_SEL    (BIT(9))
#define GPIO_SIG246_IN_SEL_M  (GPIO_SIG246_IN_SEL_V << GPIO_SIG246_IN_SEL_S)
#define GPIO_SIG246_IN_SEL_V  0x00000001U
#define GPIO_SIG246_IN_SEL_S  9

/** GPIO_FUNC247_IN_SEL_CFG_REG register
 *  Configuration register for input signal 247
 */
#define GPIO_FUNC247_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6d0)
/** GPIO_FUNC247_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 247.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC247_IN_SEL    0x000000FFU
#define GPIO_FUNC247_IN_SEL_M  (GPIO_FUNC247_IN_SEL_V << GPIO_FUNC247_IN_SEL_S)
#define GPIO_FUNC247_IN_SEL_V  0x000000FFU
#define GPIO_FUNC247_IN_SEL_S  0
/** GPIO_FUNC247_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC247_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC247_IN_INV_SEL_M  (GPIO_FUNC247_IN_INV_SEL_V << GPIO_FUNC247_IN_INV_SEL_S)
#define GPIO_FUNC247_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC247_IN_INV_SEL_S  8
/** GPIO_SIG247_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG247_IN_SEL    (BIT(9))
#define GPIO_SIG247_IN_SEL_M  (GPIO_SIG247_IN_SEL_V << GPIO_SIG247_IN_SEL_S)
#define GPIO_SIG247_IN_SEL_V  0x00000001U
#define GPIO_SIG247_IN_SEL_S  9

/** GPIO_FUNC248_IN_SEL_CFG_REG register
 *  Configuration register for input signal 248
 */
#define GPIO_FUNC248_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6d4)
/** GPIO_FUNC248_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 248.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC248_IN_SEL    0x000000FFU
#define GPIO_FUNC248_IN_SEL_M  (GPIO_FUNC248_IN_SEL_V << GPIO_FUNC248_IN_SEL_S)
#define GPIO_FUNC248_IN_SEL_V  0x000000FFU
#define GPIO_FUNC248_IN_SEL_S  0
/** GPIO_FUNC248_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC248_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC248_IN_INV_SEL_M  (GPIO_FUNC248_IN_INV_SEL_V << GPIO_FUNC248_IN_INV_SEL_S)
#define GPIO_FUNC248_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC248_IN_INV_SEL_S  8
/** GPIO_SIG248_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG248_IN_SEL    (BIT(9))
#define GPIO_SIG248_IN_SEL_M  (GPIO_SIG248_IN_SEL_V << GPIO_SIG248_IN_SEL_S)
#define GPIO_SIG248_IN_SEL_V  0x00000001U
#define GPIO_SIG248_IN_SEL_S  9

/** GPIO_FUNC249_IN_SEL_CFG_REG register
 *  Configuration register for input signal 249
 */
#define GPIO_FUNC249_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6d8)
/** GPIO_FUNC249_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 249.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC249_IN_SEL    0x000000FFU
#define GPIO_FUNC249_IN_SEL_M  (GPIO_FUNC249_IN_SEL_V << GPIO_FUNC249_IN_SEL_S)
#define GPIO_FUNC249_IN_SEL_V  0x000000FFU
#define GPIO_FUNC249_IN_SEL_S  0
/** GPIO_FUNC249_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC249_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC249_IN_INV_SEL_M  (GPIO_FUNC249_IN_INV_SEL_V << GPIO_FUNC249_IN_INV_SEL_S)
#define GPIO_FUNC249_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC249_IN_INV_SEL_S  8
/** GPIO_SIG249_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG249_IN_SEL    (BIT(9))
#define GPIO_SIG249_IN_SEL_M  (GPIO_SIG249_IN_SEL_V << GPIO_SIG249_IN_SEL_S)
#define GPIO_SIG249_IN_SEL_V  0x00000001U
#define GPIO_SIG249_IN_SEL_S  9

/** GPIO_FUNC251_IN_SEL_CFG_REG register
 *  Configuration register for input signal 251
 */
#define GPIO_FUNC251_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6e0)
/** GPIO_FUNC251_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 251.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC251_IN_SEL    0x000000FFU
#define GPIO_FUNC251_IN_SEL_M  (GPIO_FUNC251_IN_SEL_V << GPIO_FUNC251_IN_SEL_S)
#define GPIO_FUNC251_IN_SEL_V  0x000000FFU
#define GPIO_FUNC251_IN_SEL_S  0
/** GPIO_FUNC251_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC251_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC251_IN_INV_SEL_M  (GPIO_FUNC251_IN_INV_SEL_V << GPIO_FUNC251_IN_INV_SEL_S)
#define GPIO_FUNC251_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC251_IN_INV_SEL_S  8
/** GPIO_SIG251_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG251_IN_SEL    (BIT(9))
#define GPIO_SIG251_IN_SEL_M  (GPIO_SIG251_IN_SEL_V << GPIO_SIG251_IN_SEL_S)
#define GPIO_SIG251_IN_SEL_V  0x00000001U
#define GPIO_SIG251_IN_SEL_S  9

/** GPIO_FUNC252_IN_SEL_CFG_REG register
 *  Configuration register for input signal 252
 */
#define GPIO_FUNC252_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6e4)
/** GPIO_FUNC252_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 252.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC252_IN_SEL    0x000000FFU
#define GPIO_FUNC252_IN_SEL_M  (GPIO_FUNC252_IN_SEL_V << GPIO_FUNC252_IN_SEL_S)
#define GPIO_FUNC252_IN_SEL_V  0x000000FFU
#define GPIO_FUNC252_IN_SEL_S  0
/** GPIO_FUNC252_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC252_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC252_IN_INV_SEL_M  (GPIO_FUNC252_IN_INV_SEL_V << GPIO_FUNC252_IN_INV_SEL_S)
#define GPIO_FUNC252_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC252_IN_INV_SEL_S  8
/** GPIO_SIG252_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG252_IN_SEL    (BIT(9))
#define GPIO_SIG252_IN_SEL_M  (GPIO_SIG252_IN_SEL_V << GPIO_SIG252_IN_SEL_S)
#define GPIO_SIG252_IN_SEL_V  0x00000001U
#define GPIO_SIG252_IN_SEL_S  9

/** GPIO_FUNC253_IN_SEL_CFG_REG register
 *  Configuration register for input signal 253
 */
#define GPIO_FUNC253_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6e8)
/** GPIO_FUNC253_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 253.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC253_IN_SEL    0x000000FFU
#define GPIO_FUNC253_IN_SEL_M  (GPIO_FUNC253_IN_SEL_V << GPIO_FUNC253_IN_SEL_S)
#define GPIO_FUNC253_IN_SEL_V  0x000000FFU
#define GPIO_FUNC253_IN_SEL_S  0
/** GPIO_FUNC253_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC253_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC253_IN_INV_SEL_M  (GPIO_FUNC253_IN_INV_SEL_V << GPIO_FUNC253_IN_INV_SEL_S)
#define GPIO_FUNC253_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC253_IN_INV_SEL_S  8
/** GPIO_SIG253_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG253_IN_SEL    (BIT(9))
#define GPIO_SIG253_IN_SEL_M  (GPIO_SIG253_IN_SEL_V << GPIO_SIG253_IN_SEL_S)
#define GPIO_SIG253_IN_SEL_V  0x00000001U
#define GPIO_SIG253_IN_SEL_S  9

/** GPIO_FUNC254_IN_SEL_CFG_REG register
 *  Configuration register for input signal 254
 */
#define GPIO_FUNC254_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6ec)
/** GPIO_FUNC254_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 254.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC254_IN_SEL    0x000000FFU
#define GPIO_FUNC254_IN_SEL_M  (GPIO_FUNC254_IN_SEL_V << GPIO_FUNC254_IN_SEL_S)
#define GPIO_FUNC254_IN_SEL_V  0x000000FFU
#define GPIO_FUNC254_IN_SEL_S  0
/** GPIO_FUNC254_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC254_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC254_IN_INV_SEL_M  (GPIO_FUNC254_IN_INV_SEL_V << GPIO_FUNC254_IN_INV_SEL_S)
#define GPIO_FUNC254_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC254_IN_INV_SEL_S  8
/** GPIO_SIG254_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG254_IN_SEL    (BIT(9))
#define GPIO_SIG254_IN_SEL_M  (GPIO_SIG254_IN_SEL_V << GPIO_SIG254_IN_SEL_S)
#define GPIO_SIG254_IN_SEL_V  0x00000001U
#define GPIO_SIG254_IN_SEL_S  9

/** GPIO_FUNC255_IN_SEL_CFG_REG register
 *  Configuration register for input signal 255
 */
#define GPIO_FUNC255_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x6f0)
/** GPIO_FUNC255_IN_SEL : R/W; bitpos: [7:0]; default: 192;
 *  Configures to select a pin from the 63 GPIO pins to connect the input signal 255.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  61: Select GPIO61
 *  62: Select GPIO62
 *  Or
 *  0x80: A constantly high input
 *  0xc0: A constantly low input
 */
#define GPIO_FUNC255_IN_SEL    0x000000FFU
#define GPIO_FUNC255_IN_SEL_M  (GPIO_FUNC255_IN_SEL_V << GPIO_FUNC255_IN_SEL_S)
#define GPIO_FUNC255_IN_SEL_V  0x000000FFU
#define GPIO_FUNC255_IN_SEL_S  0
/** GPIO_FUNC255_IN_INV_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to invert the input value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC255_IN_INV_SEL    (BIT(8))
#define GPIO_FUNC255_IN_INV_SEL_M  (GPIO_FUNC255_IN_INV_SEL_V << GPIO_FUNC255_IN_INV_SEL_S)
#define GPIO_FUNC255_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC255_IN_INV_SEL_S  8
/** GPIO_SIG255_IN_SEL : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.
 *  1: Route signals via GPIO matrix.
 */
#define GPIO_SIG255_IN_SEL    (BIT(9))
#define GPIO_SIG255_IN_SEL_M  (GPIO_SIG255_IN_SEL_V << GPIO_SIG255_IN_SEL_S)
#define GPIO_SIG255_IN_SEL_V  0x00000001U
#define GPIO_SIG255_IN_SEL_S  9

/** GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO0 output
 */
#define GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf4)
/** GPIO_FUNC0_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO0.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 0 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC0_OUT_SEL    0x000001FFU
#define GPIO_FUNC0_OUT_SEL_M  (GPIO_FUNC0_OUT_SEL_V << GPIO_FUNC0_OUT_SEL_S)
#define GPIO_FUNC0_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC0_OUT_SEL_S  0
/** GPIO_FUNC0_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC0_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC0_OUT_INV_SEL_M  (GPIO_FUNC0_OUT_INV_SEL_V << GPIO_FUNC0_OUT_INV_SEL_S)
#define GPIO_FUNC0_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OUT_INV_SEL_S  9
/** GPIO_FUNC0_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 0 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC0_OE_SEL    (BIT(10))
#define GPIO_FUNC0_OE_SEL_M  (GPIO_FUNC0_OE_SEL_V << GPIO_FUNC0_OE_SEL_S)
#define GPIO_FUNC0_OE_SEL_V  0x00000001U
#define GPIO_FUNC0_OE_SEL_S  10
/** GPIO_FUNC0_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC0_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC0_OE_INV_SEL_M  (GPIO_FUNC0_OE_INV_SEL_V << GPIO_FUNC0_OE_INV_SEL_S)
#define GPIO_FUNC0_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OE_INV_SEL_S  11

/** GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO1 output
 */
#define GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf8)
/** GPIO_FUNC1_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO1.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 1 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC1_OUT_SEL    0x000001FFU
#define GPIO_FUNC1_OUT_SEL_M  (GPIO_FUNC1_OUT_SEL_V << GPIO_FUNC1_OUT_SEL_S)
#define GPIO_FUNC1_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC1_OUT_SEL_S  0
/** GPIO_FUNC1_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC1_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC1_OUT_INV_SEL_M  (GPIO_FUNC1_OUT_INV_SEL_V << GPIO_FUNC1_OUT_INV_SEL_S)
#define GPIO_FUNC1_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OUT_INV_SEL_S  9
/** GPIO_FUNC1_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 1 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC1_OE_SEL    (BIT(10))
#define GPIO_FUNC1_OE_SEL_M  (GPIO_FUNC1_OE_SEL_V << GPIO_FUNC1_OE_SEL_S)
#define GPIO_FUNC1_OE_SEL_V  0x00000001U
#define GPIO_FUNC1_OE_SEL_S  10
/** GPIO_FUNC1_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC1_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC1_OE_INV_SEL_M  (GPIO_FUNC1_OE_INV_SEL_V << GPIO_FUNC1_OE_INV_SEL_S)
#define GPIO_FUNC1_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OE_INV_SEL_S  11

/** GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO2 output
 */
#define GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xafc)
/** GPIO_FUNC2_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO2.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 2 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC2_OUT_SEL    0x000001FFU
#define GPIO_FUNC2_OUT_SEL_M  (GPIO_FUNC2_OUT_SEL_V << GPIO_FUNC2_OUT_SEL_S)
#define GPIO_FUNC2_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC2_OUT_SEL_S  0
/** GPIO_FUNC2_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC2_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC2_OUT_INV_SEL_M  (GPIO_FUNC2_OUT_INV_SEL_V << GPIO_FUNC2_OUT_INV_SEL_S)
#define GPIO_FUNC2_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OUT_INV_SEL_S  9
/** GPIO_FUNC2_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 2 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC2_OE_SEL    (BIT(10))
#define GPIO_FUNC2_OE_SEL_M  (GPIO_FUNC2_OE_SEL_V << GPIO_FUNC2_OE_SEL_S)
#define GPIO_FUNC2_OE_SEL_V  0x00000001U
#define GPIO_FUNC2_OE_SEL_S  10
/** GPIO_FUNC2_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC2_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC2_OE_INV_SEL_M  (GPIO_FUNC2_OE_INV_SEL_V << GPIO_FUNC2_OE_INV_SEL_S)
#define GPIO_FUNC2_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OE_INV_SEL_S  11

/** GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO3 output
 */
#define GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb00)
/** GPIO_FUNC3_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO3.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 3 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC3_OUT_SEL    0x000001FFU
#define GPIO_FUNC3_OUT_SEL_M  (GPIO_FUNC3_OUT_SEL_V << GPIO_FUNC3_OUT_SEL_S)
#define GPIO_FUNC3_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC3_OUT_SEL_S  0
/** GPIO_FUNC3_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC3_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC3_OUT_INV_SEL_M  (GPIO_FUNC3_OUT_INV_SEL_V << GPIO_FUNC3_OUT_INV_SEL_S)
#define GPIO_FUNC3_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OUT_INV_SEL_S  9
/** GPIO_FUNC3_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 3 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC3_OE_SEL    (BIT(10))
#define GPIO_FUNC3_OE_SEL_M  (GPIO_FUNC3_OE_SEL_V << GPIO_FUNC3_OE_SEL_S)
#define GPIO_FUNC3_OE_SEL_V  0x00000001U
#define GPIO_FUNC3_OE_SEL_S  10
/** GPIO_FUNC3_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC3_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC3_OE_INV_SEL_M  (GPIO_FUNC3_OE_INV_SEL_V << GPIO_FUNC3_OE_INV_SEL_S)
#define GPIO_FUNC3_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OE_INV_SEL_S  11

/** GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO4 output
 */
#define GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb04)
/** GPIO_FUNC4_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO4.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 4 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC4_OUT_SEL    0x000001FFU
#define GPIO_FUNC4_OUT_SEL_M  (GPIO_FUNC4_OUT_SEL_V << GPIO_FUNC4_OUT_SEL_S)
#define GPIO_FUNC4_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC4_OUT_SEL_S  0
/** GPIO_FUNC4_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC4_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC4_OUT_INV_SEL_M  (GPIO_FUNC4_OUT_INV_SEL_V << GPIO_FUNC4_OUT_INV_SEL_S)
#define GPIO_FUNC4_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OUT_INV_SEL_S  9
/** GPIO_FUNC4_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 4 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC4_OE_SEL    (BIT(10))
#define GPIO_FUNC4_OE_SEL_M  (GPIO_FUNC4_OE_SEL_V << GPIO_FUNC4_OE_SEL_S)
#define GPIO_FUNC4_OE_SEL_V  0x00000001U
#define GPIO_FUNC4_OE_SEL_S  10
/** GPIO_FUNC4_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC4_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC4_OE_INV_SEL_M  (GPIO_FUNC4_OE_INV_SEL_V << GPIO_FUNC4_OE_INV_SEL_S)
#define GPIO_FUNC4_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OE_INV_SEL_S  11

/** GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO5 output
 */
#define GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb08)
/** GPIO_FUNC5_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO5.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 5 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC5_OUT_SEL    0x000001FFU
#define GPIO_FUNC5_OUT_SEL_M  (GPIO_FUNC5_OUT_SEL_V << GPIO_FUNC5_OUT_SEL_S)
#define GPIO_FUNC5_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC5_OUT_SEL_S  0
/** GPIO_FUNC5_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC5_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC5_OUT_INV_SEL_M  (GPIO_FUNC5_OUT_INV_SEL_V << GPIO_FUNC5_OUT_INV_SEL_S)
#define GPIO_FUNC5_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OUT_INV_SEL_S  9
/** GPIO_FUNC5_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 5 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC5_OE_SEL    (BIT(10))
#define GPIO_FUNC5_OE_SEL_M  (GPIO_FUNC5_OE_SEL_V << GPIO_FUNC5_OE_SEL_S)
#define GPIO_FUNC5_OE_SEL_V  0x00000001U
#define GPIO_FUNC5_OE_SEL_S  10
/** GPIO_FUNC5_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC5_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC5_OE_INV_SEL_M  (GPIO_FUNC5_OE_INV_SEL_V << GPIO_FUNC5_OE_INV_SEL_S)
#define GPIO_FUNC5_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OE_INV_SEL_S  11

/** GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO6 output
 */
#define GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb0c)
/** GPIO_FUNC6_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO6.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 6 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC6_OUT_SEL    0x000001FFU
#define GPIO_FUNC6_OUT_SEL_M  (GPIO_FUNC6_OUT_SEL_V << GPIO_FUNC6_OUT_SEL_S)
#define GPIO_FUNC6_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC6_OUT_SEL_S  0
/** GPIO_FUNC6_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC6_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC6_OUT_INV_SEL_M  (GPIO_FUNC6_OUT_INV_SEL_V << GPIO_FUNC6_OUT_INV_SEL_S)
#define GPIO_FUNC6_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OUT_INV_SEL_S  9
/** GPIO_FUNC6_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 6 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC6_OE_SEL    (BIT(10))
#define GPIO_FUNC6_OE_SEL_M  (GPIO_FUNC6_OE_SEL_V << GPIO_FUNC6_OE_SEL_S)
#define GPIO_FUNC6_OE_SEL_V  0x00000001U
#define GPIO_FUNC6_OE_SEL_S  10
/** GPIO_FUNC6_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC6_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC6_OE_INV_SEL_M  (GPIO_FUNC6_OE_INV_SEL_V << GPIO_FUNC6_OE_INV_SEL_S)
#define GPIO_FUNC6_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OE_INV_SEL_S  11

/** GPIO_FUNC7_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO7 output
 */
#define GPIO_FUNC7_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb10)
/** GPIO_FUNC7_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO7.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 7 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC7_OUT_SEL    0x000001FFU
#define GPIO_FUNC7_OUT_SEL_M  (GPIO_FUNC7_OUT_SEL_V << GPIO_FUNC7_OUT_SEL_S)
#define GPIO_FUNC7_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC7_OUT_SEL_S  0
/** GPIO_FUNC7_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC7_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC7_OUT_INV_SEL_M  (GPIO_FUNC7_OUT_INV_SEL_V << GPIO_FUNC7_OUT_INV_SEL_S)
#define GPIO_FUNC7_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OUT_INV_SEL_S  9
/** GPIO_FUNC7_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 7 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC7_OE_SEL    (BIT(10))
#define GPIO_FUNC7_OE_SEL_M  (GPIO_FUNC7_OE_SEL_V << GPIO_FUNC7_OE_SEL_S)
#define GPIO_FUNC7_OE_SEL_V  0x00000001U
#define GPIO_FUNC7_OE_SEL_S  10
/** GPIO_FUNC7_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC7_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC7_OE_INV_SEL_M  (GPIO_FUNC7_OE_INV_SEL_V << GPIO_FUNC7_OE_INV_SEL_S)
#define GPIO_FUNC7_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OE_INV_SEL_S  11

/** GPIO_FUNC8_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO8 output
 */
#define GPIO_FUNC8_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb14)
/** GPIO_FUNC8_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO8.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 8 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC8_OUT_SEL    0x000001FFU
#define GPIO_FUNC8_OUT_SEL_M  (GPIO_FUNC8_OUT_SEL_V << GPIO_FUNC8_OUT_SEL_S)
#define GPIO_FUNC8_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC8_OUT_SEL_S  0
/** GPIO_FUNC8_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC8_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC8_OUT_INV_SEL_M  (GPIO_FUNC8_OUT_INV_SEL_V << GPIO_FUNC8_OUT_INV_SEL_S)
#define GPIO_FUNC8_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OUT_INV_SEL_S  9
/** GPIO_FUNC8_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 8 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC8_OE_SEL    (BIT(10))
#define GPIO_FUNC8_OE_SEL_M  (GPIO_FUNC8_OE_SEL_V << GPIO_FUNC8_OE_SEL_S)
#define GPIO_FUNC8_OE_SEL_V  0x00000001U
#define GPIO_FUNC8_OE_SEL_S  10
/** GPIO_FUNC8_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC8_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC8_OE_INV_SEL_M  (GPIO_FUNC8_OE_INV_SEL_V << GPIO_FUNC8_OE_INV_SEL_S)
#define GPIO_FUNC8_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OE_INV_SEL_S  11

/** GPIO_FUNC9_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO9 output
 */
#define GPIO_FUNC9_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb18)
/** GPIO_FUNC9_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO9.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 9 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC9_OUT_SEL    0x000001FFU
#define GPIO_FUNC9_OUT_SEL_M  (GPIO_FUNC9_OUT_SEL_V << GPIO_FUNC9_OUT_SEL_S)
#define GPIO_FUNC9_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC9_OUT_SEL_S  0
/** GPIO_FUNC9_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC9_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC9_OUT_INV_SEL_M  (GPIO_FUNC9_OUT_INV_SEL_V << GPIO_FUNC9_OUT_INV_SEL_S)
#define GPIO_FUNC9_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OUT_INV_SEL_S  9
/** GPIO_FUNC9_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 9 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC9_OE_SEL    (BIT(10))
#define GPIO_FUNC9_OE_SEL_M  (GPIO_FUNC9_OE_SEL_V << GPIO_FUNC9_OE_SEL_S)
#define GPIO_FUNC9_OE_SEL_V  0x00000001U
#define GPIO_FUNC9_OE_SEL_S  10
/** GPIO_FUNC9_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC9_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC9_OE_INV_SEL_M  (GPIO_FUNC9_OE_INV_SEL_V << GPIO_FUNC9_OE_INV_SEL_S)
#define GPIO_FUNC9_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OE_INV_SEL_S  11

/** GPIO_FUNC10_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO10 output
 */
#define GPIO_FUNC10_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb1c)
/** GPIO_FUNC10_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO10.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 10 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC10_OUT_SEL    0x000001FFU
#define GPIO_FUNC10_OUT_SEL_M  (GPIO_FUNC10_OUT_SEL_V << GPIO_FUNC10_OUT_SEL_S)
#define GPIO_FUNC10_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC10_OUT_SEL_S  0
/** GPIO_FUNC10_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC10_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC10_OUT_INV_SEL_M  (GPIO_FUNC10_OUT_INV_SEL_V << GPIO_FUNC10_OUT_INV_SEL_S)
#define GPIO_FUNC10_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OUT_INV_SEL_S  9
/** GPIO_FUNC10_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 10 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC10_OE_SEL    (BIT(10))
#define GPIO_FUNC10_OE_SEL_M  (GPIO_FUNC10_OE_SEL_V << GPIO_FUNC10_OE_SEL_S)
#define GPIO_FUNC10_OE_SEL_V  0x00000001U
#define GPIO_FUNC10_OE_SEL_S  10
/** GPIO_FUNC10_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC10_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC10_OE_INV_SEL_M  (GPIO_FUNC10_OE_INV_SEL_V << GPIO_FUNC10_OE_INV_SEL_S)
#define GPIO_FUNC10_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OE_INV_SEL_S  11

/** GPIO_FUNC11_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO11 output
 */
#define GPIO_FUNC11_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb20)
/** GPIO_FUNC11_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO11.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 11 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC11_OUT_SEL    0x000001FFU
#define GPIO_FUNC11_OUT_SEL_M  (GPIO_FUNC11_OUT_SEL_V << GPIO_FUNC11_OUT_SEL_S)
#define GPIO_FUNC11_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC11_OUT_SEL_S  0
/** GPIO_FUNC11_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC11_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC11_OUT_INV_SEL_M  (GPIO_FUNC11_OUT_INV_SEL_V << GPIO_FUNC11_OUT_INV_SEL_S)
#define GPIO_FUNC11_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OUT_INV_SEL_S  9
/** GPIO_FUNC11_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 11 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC11_OE_SEL    (BIT(10))
#define GPIO_FUNC11_OE_SEL_M  (GPIO_FUNC11_OE_SEL_V << GPIO_FUNC11_OE_SEL_S)
#define GPIO_FUNC11_OE_SEL_V  0x00000001U
#define GPIO_FUNC11_OE_SEL_S  10
/** GPIO_FUNC11_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC11_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC11_OE_INV_SEL_M  (GPIO_FUNC11_OE_INV_SEL_V << GPIO_FUNC11_OE_INV_SEL_S)
#define GPIO_FUNC11_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OE_INV_SEL_S  11

/** GPIO_FUNC12_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO12 output
 */
#define GPIO_FUNC12_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb24)
/** GPIO_FUNC12_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO12.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 12 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC12_OUT_SEL    0x000001FFU
#define GPIO_FUNC12_OUT_SEL_M  (GPIO_FUNC12_OUT_SEL_V << GPIO_FUNC12_OUT_SEL_S)
#define GPIO_FUNC12_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC12_OUT_SEL_S  0
/** GPIO_FUNC12_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC12_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC12_OUT_INV_SEL_M  (GPIO_FUNC12_OUT_INV_SEL_V << GPIO_FUNC12_OUT_INV_SEL_S)
#define GPIO_FUNC12_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OUT_INV_SEL_S  9
/** GPIO_FUNC12_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 12 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC12_OE_SEL    (BIT(10))
#define GPIO_FUNC12_OE_SEL_M  (GPIO_FUNC12_OE_SEL_V << GPIO_FUNC12_OE_SEL_S)
#define GPIO_FUNC12_OE_SEL_V  0x00000001U
#define GPIO_FUNC12_OE_SEL_S  10
/** GPIO_FUNC12_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC12_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC12_OE_INV_SEL_M  (GPIO_FUNC12_OE_INV_SEL_V << GPIO_FUNC12_OE_INV_SEL_S)
#define GPIO_FUNC12_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OE_INV_SEL_S  11

/** GPIO_FUNC13_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO13 output
 */
#define GPIO_FUNC13_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb28)
/** GPIO_FUNC13_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO13.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 13 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC13_OUT_SEL    0x000001FFU
#define GPIO_FUNC13_OUT_SEL_M  (GPIO_FUNC13_OUT_SEL_V << GPIO_FUNC13_OUT_SEL_S)
#define GPIO_FUNC13_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC13_OUT_SEL_S  0
/** GPIO_FUNC13_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC13_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC13_OUT_INV_SEL_M  (GPIO_FUNC13_OUT_INV_SEL_V << GPIO_FUNC13_OUT_INV_SEL_S)
#define GPIO_FUNC13_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OUT_INV_SEL_S  9
/** GPIO_FUNC13_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 13 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC13_OE_SEL    (BIT(10))
#define GPIO_FUNC13_OE_SEL_M  (GPIO_FUNC13_OE_SEL_V << GPIO_FUNC13_OE_SEL_S)
#define GPIO_FUNC13_OE_SEL_V  0x00000001U
#define GPIO_FUNC13_OE_SEL_S  10
/** GPIO_FUNC13_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC13_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC13_OE_INV_SEL_M  (GPIO_FUNC13_OE_INV_SEL_V << GPIO_FUNC13_OE_INV_SEL_S)
#define GPIO_FUNC13_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OE_INV_SEL_S  11

/** GPIO_FUNC14_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO14 output
 */
#define GPIO_FUNC14_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb2c)
/** GPIO_FUNC14_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO14.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 14 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC14_OUT_SEL    0x000001FFU
#define GPIO_FUNC14_OUT_SEL_M  (GPIO_FUNC14_OUT_SEL_V << GPIO_FUNC14_OUT_SEL_S)
#define GPIO_FUNC14_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC14_OUT_SEL_S  0
/** GPIO_FUNC14_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC14_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC14_OUT_INV_SEL_M  (GPIO_FUNC14_OUT_INV_SEL_V << GPIO_FUNC14_OUT_INV_SEL_S)
#define GPIO_FUNC14_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OUT_INV_SEL_S  9
/** GPIO_FUNC14_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 14 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC14_OE_SEL    (BIT(10))
#define GPIO_FUNC14_OE_SEL_M  (GPIO_FUNC14_OE_SEL_V << GPIO_FUNC14_OE_SEL_S)
#define GPIO_FUNC14_OE_SEL_V  0x00000001U
#define GPIO_FUNC14_OE_SEL_S  10
/** GPIO_FUNC14_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC14_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC14_OE_INV_SEL_M  (GPIO_FUNC14_OE_INV_SEL_V << GPIO_FUNC14_OE_INV_SEL_S)
#define GPIO_FUNC14_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OE_INV_SEL_S  11

/** GPIO_FUNC15_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO15 output
 */
#define GPIO_FUNC15_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb30)
/** GPIO_FUNC15_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO15.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 15 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC15_OUT_SEL    0x000001FFU
#define GPIO_FUNC15_OUT_SEL_M  (GPIO_FUNC15_OUT_SEL_V << GPIO_FUNC15_OUT_SEL_S)
#define GPIO_FUNC15_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC15_OUT_SEL_S  0
/** GPIO_FUNC15_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC15_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC15_OUT_INV_SEL_M  (GPIO_FUNC15_OUT_INV_SEL_V << GPIO_FUNC15_OUT_INV_SEL_S)
#define GPIO_FUNC15_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OUT_INV_SEL_S  9
/** GPIO_FUNC15_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 15 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC15_OE_SEL    (BIT(10))
#define GPIO_FUNC15_OE_SEL_M  (GPIO_FUNC15_OE_SEL_V << GPIO_FUNC15_OE_SEL_S)
#define GPIO_FUNC15_OE_SEL_V  0x00000001U
#define GPIO_FUNC15_OE_SEL_S  10
/** GPIO_FUNC15_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC15_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC15_OE_INV_SEL_M  (GPIO_FUNC15_OE_INV_SEL_V << GPIO_FUNC15_OE_INV_SEL_S)
#define GPIO_FUNC15_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OE_INV_SEL_S  11

/** GPIO_FUNC16_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO16 output
 */
#define GPIO_FUNC16_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb34)
/** GPIO_FUNC16_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO16.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 16 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC16_OUT_SEL    0x000001FFU
#define GPIO_FUNC16_OUT_SEL_M  (GPIO_FUNC16_OUT_SEL_V << GPIO_FUNC16_OUT_SEL_S)
#define GPIO_FUNC16_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC16_OUT_SEL_S  0
/** GPIO_FUNC16_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC16_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC16_OUT_INV_SEL_M  (GPIO_FUNC16_OUT_INV_SEL_V << GPIO_FUNC16_OUT_INV_SEL_S)
#define GPIO_FUNC16_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OUT_INV_SEL_S  9
/** GPIO_FUNC16_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 16 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC16_OE_SEL    (BIT(10))
#define GPIO_FUNC16_OE_SEL_M  (GPIO_FUNC16_OE_SEL_V << GPIO_FUNC16_OE_SEL_S)
#define GPIO_FUNC16_OE_SEL_V  0x00000001U
#define GPIO_FUNC16_OE_SEL_S  10
/** GPIO_FUNC16_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC16_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC16_OE_INV_SEL_M  (GPIO_FUNC16_OE_INV_SEL_V << GPIO_FUNC16_OE_INV_SEL_S)
#define GPIO_FUNC16_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OE_INV_SEL_S  11

/** GPIO_FUNC17_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO17 output
 */
#define GPIO_FUNC17_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb38)
/** GPIO_FUNC17_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO17.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 17 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC17_OUT_SEL    0x000001FFU
#define GPIO_FUNC17_OUT_SEL_M  (GPIO_FUNC17_OUT_SEL_V << GPIO_FUNC17_OUT_SEL_S)
#define GPIO_FUNC17_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC17_OUT_SEL_S  0
/** GPIO_FUNC17_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC17_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC17_OUT_INV_SEL_M  (GPIO_FUNC17_OUT_INV_SEL_V << GPIO_FUNC17_OUT_INV_SEL_S)
#define GPIO_FUNC17_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OUT_INV_SEL_S  9
/** GPIO_FUNC17_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 17 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC17_OE_SEL    (BIT(10))
#define GPIO_FUNC17_OE_SEL_M  (GPIO_FUNC17_OE_SEL_V << GPIO_FUNC17_OE_SEL_S)
#define GPIO_FUNC17_OE_SEL_V  0x00000001U
#define GPIO_FUNC17_OE_SEL_S  10
/** GPIO_FUNC17_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC17_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC17_OE_INV_SEL_M  (GPIO_FUNC17_OE_INV_SEL_V << GPIO_FUNC17_OE_INV_SEL_S)
#define GPIO_FUNC17_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OE_INV_SEL_S  11

/** GPIO_FUNC18_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO18 output
 */
#define GPIO_FUNC18_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb3c)
/** GPIO_FUNC18_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO18.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 18 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC18_OUT_SEL    0x000001FFU
#define GPIO_FUNC18_OUT_SEL_M  (GPIO_FUNC18_OUT_SEL_V << GPIO_FUNC18_OUT_SEL_S)
#define GPIO_FUNC18_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC18_OUT_SEL_S  0
/** GPIO_FUNC18_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC18_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC18_OUT_INV_SEL_M  (GPIO_FUNC18_OUT_INV_SEL_V << GPIO_FUNC18_OUT_INV_SEL_S)
#define GPIO_FUNC18_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OUT_INV_SEL_S  9
/** GPIO_FUNC18_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 18 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC18_OE_SEL    (BIT(10))
#define GPIO_FUNC18_OE_SEL_M  (GPIO_FUNC18_OE_SEL_V << GPIO_FUNC18_OE_SEL_S)
#define GPIO_FUNC18_OE_SEL_V  0x00000001U
#define GPIO_FUNC18_OE_SEL_S  10
/** GPIO_FUNC18_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC18_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC18_OE_INV_SEL_M  (GPIO_FUNC18_OE_INV_SEL_V << GPIO_FUNC18_OE_INV_SEL_S)
#define GPIO_FUNC18_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OE_INV_SEL_S  11

/** GPIO_FUNC19_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO19 output
 */
#define GPIO_FUNC19_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb40)
/** GPIO_FUNC19_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO19.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 19 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC19_OUT_SEL    0x000001FFU
#define GPIO_FUNC19_OUT_SEL_M  (GPIO_FUNC19_OUT_SEL_V << GPIO_FUNC19_OUT_SEL_S)
#define GPIO_FUNC19_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC19_OUT_SEL_S  0
/** GPIO_FUNC19_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC19_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC19_OUT_INV_SEL_M  (GPIO_FUNC19_OUT_INV_SEL_V << GPIO_FUNC19_OUT_INV_SEL_S)
#define GPIO_FUNC19_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OUT_INV_SEL_S  9
/** GPIO_FUNC19_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 19 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC19_OE_SEL    (BIT(10))
#define GPIO_FUNC19_OE_SEL_M  (GPIO_FUNC19_OE_SEL_V << GPIO_FUNC19_OE_SEL_S)
#define GPIO_FUNC19_OE_SEL_V  0x00000001U
#define GPIO_FUNC19_OE_SEL_S  10
/** GPIO_FUNC19_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC19_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC19_OE_INV_SEL_M  (GPIO_FUNC19_OE_INV_SEL_V << GPIO_FUNC19_OE_INV_SEL_S)
#define GPIO_FUNC19_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OE_INV_SEL_S  11

/** GPIO_FUNC20_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO20 output
 */
#define GPIO_FUNC20_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb44)
/** GPIO_FUNC20_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO20.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 20 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC20_OUT_SEL    0x000001FFU
#define GPIO_FUNC20_OUT_SEL_M  (GPIO_FUNC20_OUT_SEL_V << GPIO_FUNC20_OUT_SEL_S)
#define GPIO_FUNC20_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC20_OUT_SEL_S  0
/** GPIO_FUNC20_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC20_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC20_OUT_INV_SEL_M  (GPIO_FUNC20_OUT_INV_SEL_V << GPIO_FUNC20_OUT_INV_SEL_S)
#define GPIO_FUNC20_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OUT_INV_SEL_S  9
/** GPIO_FUNC20_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 20 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC20_OE_SEL    (BIT(10))
#define GPIO_FUNC20_OE_SEL_M  (GPIO_FUNC20_OE_SEL_V << GPIO_FUNC20_OE_SEL_S)
#define GPIO_FUNC20_OE_SEL_V  0x00000001U
#define GPIO_FUNC20_OE_SEL_S  10
/** GPIO_FUNC20_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC20_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC20_OE_INV_SEL_M  (GPIO_FUNC20_OE_INV_SEL_V << GPIO_FUNC20_OE_INV_SEL_S)
#define GPIO_FUNC20_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OE_INV_SEL_S  11

/** GPIO_FUNC21_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO21 output
 */
#define GPIO_FUNC21_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb48)
/** GPIO_FUNC21_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO21.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 21 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC21_OUT_SEL    0x000001FFU
#define GPIO_FUNC21_OUT_SEL_M  (GPIO_FUNC21_OUT_SEL_V << GPIO_FUNC21_OUT_SEL_S)
#define GPIO_FUNC21_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC21_OUT_SEL_S  0
/** GPIO_FUNC21_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC21_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC21_OUT_INV_SEL_M  (GPIO_FUNC21_OUT_INV_SEL_V << GPIO_FUNC21_OUT_INV_SEL_S)
#define GPIO_FUNC21_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OUT_INV_SEL_S  9
/** GPIO_FUNC21_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 21 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC21_OE_SEL    (BIT(10))
#define GPIO_FUNC21_OE_SEL_M  (GPIO_FUNC21_OE_SEL_V << GPIO_FUNC21_OE_SEL_S)
#define GPIO_FUNC21_OE_SEL_V  0x00000001U
#define GPIO_FUNC21_OE_SEL_S  10
/** GPIO_FUNC21_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC21_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC21_OE_INV_SEL_M  (GPIO_FUNC21_OE_INV_SEL_V << GPIO_FUNC21_OE_INV_SEL_S)
#define GPIO_FUNC21_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OE_INV_SEL_S  11

/** GPIO_FUNC22_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO22 output
 */
#define GPIO_FUNC22_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb4c)
/** GPIO_FUNC22_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO22.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 22 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC22_OUT_SEL    0x000001FFU
#define GPIO_FUNC22_OUT_SEL_M  (GPIO_FUNC22_OUT_SEL_V << GPIO_FUNC22_OUT_SEL_S)
#define GPIO_FUNC22_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC22_OUT_SEL_S  0
/** GPIO_FUNC22_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC22_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC22_OUT_INV_SEL_M  (GPIO_FUNC22_OUT_INV_SEL_V << GPIO_FUNC22_OUT_INV_SEL_S)
#define GPIO_FUNC22_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OUT_INV_SEL_S  9
/** GPIO_FUNC22_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 22 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC22_OE_SEL    (BIT(10))
#define GPIO_FUNC22_OE_SEL_M  (GPIO_FUNC22_OE_SEL_V << GPIO_FUNC22_OE_SEL_S)
#define GPIO_FUNC22_OE_SEL_V  0x00000001U
#define GPIO_FUNC22_OE_SEL_S  10
/** GPIO_FUNC22_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC22_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC22_OE_INV_SEL_M  (GPIO_FUNC22_OE_INV_SEL_V << GPIO_FUNC22_OE_INV_SEL_S)
#define GPIO_FUNC22_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OE_INV_SEL_S  11

/** GPIO_FUNC23_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO23 output
 */
#define GPIO_FUNC23_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb50)
/** GPIO_FUNC23_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO23.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 23 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC23_OUT_SEL    0x000001FFU
#define GPIO_FUNC23_OUT_SEL_M  (GPIO_FUNC23_OUT_SEL_V << GPIO_FUNC23_OUT_SEL_S)
#define GPIO_FUNC23_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC23_OUT_SEL_S  0
/** GPIO_FUNC23_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC23_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC23_OUT_INV_SEL_M  (GPIO_FUNC23_OUT_INV_SEL_V << GPIO_FUNC23_OUT_INV_SEL_S)
#define GPIO_FUNC23_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OUT_INV_SEL_S  9
/** GPIO_FUNC23_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 23 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC23_OE_SEL    (BIT(10))
#define GPIO_FUNC23_OE_SEL_M  (GPIO_FUNC23_OE_SEL_V << GPIO_FUNC23_OE_SEL_S)
#define GPIO_FUNC23_OE_SEL_V  0x00000001U
#define GPIO_FUNC23_OE_SEL_S  10
/** GPIO_FUNC23_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC23_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC23_OE_INV_SEL_M  (GPIO_FUNC23_OE_INV_SEL_V << GPIO_FUNC23_OE_INV_SEL_S)
#define GPIO_FUNC23_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OE_INV_SEL_S  11

/** GPIO_FUNC24_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO24 output
 */
#define GPIO_FUNC24_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb54)
/** GPIO_FUNC24_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO24.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 24 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC24_OUT_SEL    0x000001FFU
#define GPIO_FUNC24_OUT_SEL_M  (GPIO_FUNC24_OUT_SEL_V << GPIO_FUNC24_OUT_SEL_S)
#define GPIO_FUNC24_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC24_OUT_SEL_S  0
/** GPIO_FUNC24_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC24_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC24_OUT_INV_SEL_M  (GPIO_FUNC24_OUT_INV_SEL_V << GPIO_FUNC24_OUT_INV_SEL_S)
#define GPIO_FUNC24_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OUT_INV_SEL_S  9
/** GPIO_FUNC24_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 24 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC24_OE_SEL    (BIT(10))
#define GPIO_FUNC24_OE_SEL_M  (GPIO_FUNC24_OE_SEL_V << GPIO_FUNC24_OE_SEL_S)
#define GPIO_FUNC24_OE_SEL_V  0x00000001U
#define GPIO_FUNC24_OE_SEL_S  10
/** GPIO_FUNC24_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC24_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC24_OE_INV_SEL_M  (GPIO_FUNC24_OE_INV_SEL_V << GPIO_FUNC24_OE_INV_SEL_S)
#define GPIO_FUNC24_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OE_INV_SEL_S  11

/** GPIO_FUNC25_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO25 output
 */
#define GPIO_FUNC25_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb58)
/** GPIO_FUNC25_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO25.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 25 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC25_OUT_SEL    0x000001FFU
#define GPIO_FUNC25_OUT_SEL_M  (GPIO_FUNC25_OUT_SEL_V << GPIO_FUNC25_OUT_SEL_S)
#define GPIO_FUNC25_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC25_OUT_SEL_S  0
/** GPIO_FUNC25_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC25_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC25_OUT_INV_SEL_M  (GPIO_FUNC25_OUT_INV_SEL_V << GPIO_FUNC25_OUT_INV_SEL_S)
#define GPIO_FUNC25_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OUT_INV_SEL_S  9
/** GPIO_FUNC25_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 25 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC25_OE_SEL    (BIT(10))
#define GPIO_FUNC25_OE_SEL_M  (GPIO_FUNC25_OE_SEL_V << GPIO_FUNC25_OE_SEL_S)
#define GPIO_FUNC25_OE_SEL_V  0x00000001U
#define GPIO_FUNC25_OE_SEL_S  10
/** GPIO_FUNC25_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC25_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC25_OE_INV_SEL_M  (GPIO_FUNC25_OE_INV_SEL_V << GPIO_FUNC25_OE_INV_SEL_S)
#define GPIO_FUNC25_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OE_INV_SEL_S  11

/** GPIO_FUNC26_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO26 output
 */
#define GPIO_FUNC26_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb5c)
/** GPIO_FUNC26_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO26.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 26 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC26_OUT_SEL    0x000001FFU
#define GPIO_FUNC26_OUT_SEL_M  (GPIO_FUNC26_OUT_SEL_V << GPIO_FUNC26_OUT_SEL_S)
#define GPIO_FUNC26_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC26_OUT_SEL_S  0
/** GPIO_FUNC26_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC26_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC26_OUT_INV_SEL_M  (GPIO_FUNC26_OUT_INV_SEL_V << GPIO_FUNC26_OUT_INV_SEL_S)
#define GPIO_FUNC26_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OUT_INV_SEL_S  9
/** GPIO_FUNC26_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 26 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC26_OE_SEL    (BIT(10))
#define GPIO_FUNC26_OE_SEL_M  (GPIO_FUNC26_OE_SEL_V << GPIO_FUNC26_OE_SEL_S)
#define GPIO_FUNC26_OE_SEL_V  0x00000001U
#define GPIO_FUNC26_OE_SEL_S  10
/** GPIO_FUNC26_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC26_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC26_OE_INV_SEL_M  (GPIO_FUNC26_OE_INV_SEL_V << GPIO_FUNC26_OE_INV_SEL_S)
#define GPIO_FUNC26_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OE_INV_SEL_S  11

/** GPIO_FUNC27_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO27 output
 */
#define GPIO_FUNC27_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb60)
/** GPIO_FUNC27_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO27.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 27 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC27_OUT_SEL    0x000001FFU
#define GPIO_FUNC27_OUT_SEL_M  (GPIO_FUNC27_OUT_SEL_V << GPIO_FUNC27_OUT_SEL_S)
#define GPIO_FUNC27_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC27_OUT_SEL_S  0
/** GPIO_FUNC27_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC27_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC27_OUT_INV_SEL_M  (GPIO_FUNC27_OUT_INV_SEL_V << GPIO_FUNC27_OUT_INV_SEL_S)
#define GPIO_FUNC27_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OUT_INV_SEL_S  9
/** GPIO_FUNC27_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 27 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC27_OE_SEL    (BIT(10))
#define GPIO_FUNC27_OE_SEL_M  (GPIO_FUNC27_OE_SEL_V << GPIO_FUNC27_OE_SEL_S)
#define GPIO_FUNC27_OE_SEL_V  0x00000001U
#define GPIO_FUNC27_OE_SEL_S  10
/** GPIO_FUNC27_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC27_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC27_OE_INV_SEL_M  (GPIO_FUNC27_OE_INV_SEL_V << GPIO_FUNC27_OE_INV_SEL_S)
#define GPIO_FUNC27_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OE_INV_SEL_S  11

/** GPIO_FUNC28_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO28 output
 */
#define GPIO_FUNC28_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb64)
/** GPIO_FUNC28_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO28.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 28 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC28_OUT_SEL    0x000001FFU
#define GPIO_FUNC28_OUT_SEL_M  (GPIO_FUNC28_OUT_SEL_V << GPIO_FUNC28_OUT_SEL_S)
#define GPIO_FUNC28_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC28_OUT_SEL_S  0
/** GPIO_FUNC28_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC28_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC28_OUT_INV_SEL_M  (GPIO_FUNC28_OUT_INV_SEL_V << GPIO_FUNC28_OUT_INV_SEL_S)
#define GPIO_FUNC28_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OUT_INV_SEL_S  9
/** GPIO_FUNC28_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 28 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC28_OE_SEL    (BIT(10))
#define GPIO_FUNC28_OE_SEL_M  (GPIO_FUNC28_OE_SEL_V << GPIO_FUNC28_OE_SEL_S)
#define GPIO_FUNC28_OE_SEL_V  0x00000001U
#define GPIO_FUNC28_OE_SEL_S  10
/** GPIO_FUNC28_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC28_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC28_OE_INV_SEL_M  (GPIO_FUNC28_OE_INV_SEL_V << GPIO_FUNC28_OE_INV_SEL_S)
#define GPIO_FUNC28_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OE_INV_SEL_S  11

/** GPIO_FUNC29_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO29 output
 */
#define GPIO_FUNC29_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb68)
/** GPIO_FUNC29_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO29.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 29 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC29_OUT_SEL    0x000001FFU
#define GPIO_FUNC29_OUT_SEL_M  (GPIO_FUNC29_OUT_SEL_V << GPIO_FUNC29_OUT_SEL_S)
#define GPIO_FUNC29_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC29_OUT_SEL_S  0
/** GPIO_FUNC29_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC29_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC29_OUT_INV_SEL_M  (GPIO_FUNC29_OUT_INV_SEL_V << GPIO_FUNC29_OUT_INV_SEL_S)
#define GPIO_FUNC29_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OUT_INV_SEL_S  9
/** GPIO_FUNC29_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 29 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC29_OE_SEL    (BIT(10))
#define GPIO_FUNC29_OE_SEL_M  (GPIO_FUNC29_OE_SEL_V << GPIO_FUNC29_OE_SEL_S)
#define GPIO_FUNC29_OE_SEL_V  0x00000001U
#define GPIO_FUNC29_OE_SEL_S  10
/** GPIO_FUNC29_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC29_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC29_OE_INV_SEL_M  (GPIO_FUNC29_OE_INV_SEL_V << GPIO_FUNC29_OE_INV_SEL_S)
#define GPIO_FUNC29_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OE_INV_SEL_S  11

/** GPIO_FUNC30_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO30 output
 */
#define GPIO_FUNC30_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb6c)
/** GPIO_FUNC30_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO30.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 30 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC30_OUT_SEL    0x000001FFU
#define GPIO_FUNC30_OUT_SEL_M  (GPIO_FUNC30_OUT_SEL_V << GPIO_FUNC30_OUT_SEL_S)
#define GPIO_FUNC30_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC30_OUT_SEL_S  0
/** GPIO_FUNC30_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC30_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC30_OUT_INV_SEL_M  (GPIO_FUNC30_OUT_INV_SEL_V << GPIO_FUNC30_OUT_INV_SEL_S)
#define GPIO_FUNC30_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OUT_INV_SEL_S  9
/** GPIO_FUNC30_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 30 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC30_OE_SEL    (BIT(10))
#define GPIO_FUNC30_OE_SEL_M  (GPIO_FUNC30_OE_SEL_V << GPIO_FUNC30_OE_SEL_S)
#define GPIO_FUNC30_OE_SEL_V  0x00000001U
#define GPIO_FUNC30_OE_SEL_S  10
/** GPIO_FUNC30_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC30_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC30_OE_INV_SEL_M  (GPIO_FUNC30_OE_INV_SEL_V << GPIO_FUNC30_OE_INV_SEL_S)
#define GPIO_FUNC30_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OE_INV_SEL_S  11

/** GPIO_FUNC31_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO31 output
 */
#define GPIO_FUNC31_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb70)
/** GPIO_FUNC31_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO31.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 31 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC31_OUT_SEL    0x000001FFU
#define GPIO_FUNC31_OUT_SEL_M  (GPIO_FUNC31_OUT_SEL_V << GPIO_FUNC31_OUT_SEL_S)
#define GPIO_FUNC31_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC31_OUT_SEL_S  0
/** GPIO_FUNC31_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC31_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC31_OUT_INV_SEL_M  (GPIO_FUNC31_OUT_INV_SEL_V << GPIO_FUNC31_OUT_INV_SEL_S)
#define GPIO_FUNC31_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OUT_INV_SEL_S  9
/** GPIO_FUNC31_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 31 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC31_OE_SEL    (BIT(10))
#define GPIO_FUNC31_OE_SEL_M  (GPIO_FUNC31_OE_SEL_V << GPIO_FUNC31_OE_SEL_S)
#define GPIO_FUNC31_OE_SEL_V  0x00000001U
#define GPIO_FUNC31_OE_SEL_S  10
/** GPIO_FUNC31_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC31_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC31_OE_INV_SEL_M  (GPIO_FUNC31_OE_INV_SEL_V << GPIO_FUNC31_OE_INV_SEL_S)
#define GPIO_FUNC31_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OE_INV_SEL_S  11

/** GPIO_FUNC32_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO32 output
 */
#define GPIO_FUNC32_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb74)
/** GPIO_FUNC32_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO32.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 32 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC32_OUT_SEL    0x000001FFU
#define GPIO_FUNC32_OUT_SEL_M  (GPIO_FUNC32_OUT_SEL_V << GPIO_FUNC32_OUT_SEL_S)
#define GPIO_FUNC32_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC32_OUT_SEL_S  0
/** GPIO_FUNC32_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC32_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC32_OUT_INV_SEL_M  (GPIO_FUNC32_OUT_INV_SEL_V << GPIO_FUNC32_OUT_INV_SEL_S)
#define GPIO_FUNC32_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OUT_INV_SEL_S  9
/** GPIO_FUNC32_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 32 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC32_OE_SEL    (BIT(10))
#define GPIO_FUNC32_OE_SEL_M  (GPIO_FUNC32_OE_SEL_V << GPIO_FUNC32_OE_SEL_S)
#define GPIO_FUNC32_OE_SEL_V  0x00000001U
#define GPIO_FUNC32_OE_SEL_S  10
/** GPIO_FUNC32_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC32_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC32_OE_INV_SEL_M  (GPIO_FUNC32_OE_INV_SEL_V << GPIO_FUNC32_OE_INV_SEL_S)
#define GPIO_FUNC32_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OE_INV_SEL_S  11

/** GPIO_FUNC33_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO33 output
 */
#define GPIO_FUNC33_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb78)
/** GPIO_FUNC33_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO33.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 33 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC33_OUT_SEL    0x000001FFU
#define GPIO_FUNC33_OUT_SEL_M  (GPIO_FUNC33_OUT_SEL_V << GPIO_FUNC33_OUT_SEL_S)
#define GPIO_FUNC33_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC33_OUT_SEL_S  0
/** GPIO_FUNC33_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC33_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC33_OUT_INV_SEL_M  (GPIO_FUNC33_OUT_INV_SEL_V << GPIO_FUNC33_OUT_INV_SEL_S)
#define GPIO_FUNC33_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_OUT_INV_SEL_S  9
/** GPIO_FUNC33_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 33 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC33_OE_SEL    (BIT(10))
#define GPIO_FUNC33_OE_SEL_M  (GPIO_FUNC33_OE_SEL_V << GPIO_FUNC33_OE_SEL_S)
#define GPIO_FUNC33_OE_SEL_V  0x00000001U
#define GPIO_FUNC33_OE_SEL_S  10
/** GPIO_FUNC33_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC33_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC33_OE_INV_SEL_M  (GPIO_FUNC33_OE_INV_SEL_V << GPIO_FUNC33_OE_INV_SEL_S)
#define GPIO_FUNC33_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_OE_INV_SEL_S  11

/** GPIO_FUNC34_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO34 output
 */
#define GPIO_FUNC34_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb7c)
/** GPIO_FUNC34_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO34.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 34 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC34_OUT_SEL    0x000001FFU
#define GPIO_FUNC34_OUT_SEL_M  (GPIO_FUNC34_OUT_SEL_V << GPIO_FUNC34_OUT_SEL_S)
#define GPIO_FUNC34_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC34_OUT_SEL_S  0
/** GPIO_FUNC34_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC34_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC34_OUT_INV_SEL_M  (GPIO_FUNC34_OUT_INV_SEL_V << GPIO_FUNC34_OUT_INV_SEL_S)
#define GPIO_FUNC34_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_OUT_INV_SEL_S  9
/** GPIO_FUNC34_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 34 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC34_OE_SEL    (BIT(10))
#define GPIO_FUNC34_OE_SEL_M  (GPIO_FUNC34_OE_SEL_V << GPIO_FUNC34_OE_SEL_S)
#define GPIO_FUNC34_OE_SEL_V  0x00000001U
#define GPIO_FUNC34_OE_SEL_S  10
/** GPIO_FUNC34_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC34_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC34_OE_INV_SEL_M  (GPIO_FUNC34_OE_INV_SEL_V << GPIO_FUNC34_OE_INV_SEL_S)
#define GPIO_FUNC34_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_OE_INV_SEL_S  11

/** GPIO_FUNC35_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO35 output
 */
#define GPIO_FUNC35_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb80)
/** GPIO_FUNC35_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO35.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 35 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC35_OUT_SEL    0x000001FFU
#define GPIO_FUNC35_OUT_SEL_M  (GPIO_FUNC35_OUT_SEL_V << GPIO_FUNC35_OUT_SEL_S)
#define GPIO_FUNC35_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC35_OUT_SEL_S  0
/** GPIO_FUNC35_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC35_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC35_OUT_INV_SEL_M  (GPIO_FUNC35_OUT_INV_SEL_V << GPIO_FUNC35_OUT_INV_SEL_S)
#define GPIO_FUNC35_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_OUT_INV_SEL_S  9
/** GPIO_FUNC35_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 35 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC35_OE_SEL    (BIT(10))
#define GPIO_FUNC35_OE_SEL_M  (GPIO_FUNC35_OE_SEL_V << GPIO_FUNC35_OE_SEL_S)
#define GPIO_FUNC35_OE_SEL_V  0x00000001U
#define GPIO_FUNC35_OE_SEL_S  10
/** GPIO_FUNC35_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC35_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC35_OE_INV_SEL_M  (GPIO_FUNC35_OE_INV_SEL_V << GPIO_FUNC35_OE_INV_SEL_S)
#define GPIO_FUNC35_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_OE_INV_SEL_S  11

/** GPIO_FUNC36_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO36 output
 */
#define GPIO_FUNC36_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb84)
/** GPIO_FUNC36_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO36.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 36 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC36_OUT_SEL    0x000001FFU
#define GPIO_FUNC36_OUT_SEL_M  (GPIO_FUNC36_OUT_SEL_V << GPIO_FUNC36_OUT_SEL_S)
#define GPIO_FUNC36_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC36_OUT_SEL_S  0
/** GPIO_FUNC36_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC36_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC36_OUT_INV_SEL_M  (GPIO_FUNC36_OUT_INV_SEL_V << GPIO_FUNC36_OUT_INV_SEL_S)
#define GPIO_FUNC36_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC36_OUT_INV_SEL_S  9
/** GPIO_FUNC36_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 36 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC36_OE_SEL    (BIT(10))
#define GPIO_FUNC36_OE_SEL_M  (GPIO_FUNC36_OE_SEL_V << GPIO_FUNC36_OE_SEL_S)
#define GPIO_FUNC36_OE_SEL_V  0x00000001U
#define GPIO_FUNC36_OE_SEL_S  10
/** GPIO_FUNC36_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC36_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC36_OE_INV_SEL_M  (GPIO_FUNC36_OE_INV_SEL_V << GPIO_FUNC36_OE_INV_SEL_S)
#define GPIO_FUNC36_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC36_OE_INV_SEL_S  11

/** GPIO_FUNC37_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO37 output
 */
#define GPIO_FUNC37_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb88)
/** GPIO_FUNC37_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO37.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 37 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC37_OUT_SEL    0x000001FFU
#define GPIO_FUNC37_OUT_SEL_M  (GPIO_FUNC37_OUT_SEL_V << GPIO_FUNC37_OUT_SEL_S)
#define GPIO_FUNC37_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC37_OUT_SEL_S  0
/** GPIO_FUNC37_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC37_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC37_OUT_INV_SEL_M  (GPIO_FUNC37_OUT_INV_SEL_V << GPIO_FUNC37_OUT_INV_SEL_S)
#define GPIO_FUNC37_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC37_OUT_INV_SEL_S  9
/** GPIO_FUNC37_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 37 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC37_OE_SEL    (BIT(10))
#define GPIO_FUNC37_OE_SEL_M  (GPIO_FUNC37_OE_SEL_V << GPIO_FUNC37_OE_SEL_S)
#define GPIO_FUNC37_OE_SEL_V  0x00000001U
#define GPIO_FUNC37_OE_SEL_S  10
/** GPIO_FUNC37_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC37_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC37_OE_INV_SEL_M  (GPIO_FUNC37_OE_INV_SEL_V << GPIO_FUNC37_OE_INV_SEL_S)
#define GPIO_FUNC37_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC37_OE_INV_SEL_S  11

/** GPIO_FUNC38_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO38 output
 */
#define GPIO_FUNC38_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb8c)
/** GPIO_FUNC38_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO38.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 38 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC38_OUT_SEL    0x000001FFU
#define GPIO_FUNC38_OUT_SEL_M  (GPIO_FUNC38_OUT_SEL_V << GPIO_FUNC38_OUT_SEL_S)
#define GPIO_FUNC38_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC38_OUT_SEL_S  0
/** GPIO_FUNC38_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC38_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC38_OUT_INV_SEL_M  (GPIO_FUNC38_OUT_INV_SEL_V << GPIO_FUNC38_OUT_INV_SEL_S)
#define GPIO_FUNC38_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC38_OUT_INV_SEL_S  9
/** GPIO_FUNC38_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 38 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC38_OE_SEL    (BIT(10))
#define GPIO_FUNC38_OE_SEL_M  (GPIO_FUNC38_OE_SEL_V << GPIO_FUNC38_OE_SEL_S)
#define GPIO_FUNC38_OE_SEL_V  0x00000001U
#define GPIO_FUNC38_OE_SEL_S  10
/** GPIO_FUNC38_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC38_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC38_OE_INV_SEL_M  (GPIO_FUNC38_OE_INV_SEL_V << GPIO_FUNC38_OE_INV_SEL_S)
#define GPIO_FUNC38_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC38_OE_INV_SEL_S  11

/** GPIO_FUNC39_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO39 output
 */
#define GPIO_FUNC39_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb90)
/** GPIO_FUNC39_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO39.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 39 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC39_OUT_SEL    0x000001FFU
#define GPIO_FUNC39_OUT_SEL_M  (GPIO_FUNC39_OUT_SEL_V << GPIO_FUNC39_OUT_SEL_S)
#define GPIO_FUNC39_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC39_OUT_SEL_S  0
/** GPIO_FUNC39_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC39_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC39_OUT_INV_SEL_M  (GPIO_FUNC39_OUT_INV_SEL_V << GPIO_FUNC39_OUT_INV_SEL_S)
#define GPIO_FUNC39_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC39_OUT_INV_SEL_S  9
/** GPIO_FUNC39_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 39 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC39_OE_SEL    (BIT(10))
#define GPIO_FUNC39_OE_SEL_M  (GPIO_FUNC39_OE_SEL_V << GPIO_FUNC39_OE_SEL_S)
#define GPIO_FUNC39_OE_SEL_V  0x00000001U
#define GPIO_FUNC39_OE_SEL_S  10
/** GPIO_FUNC39_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC39_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC39_OE_INV_SEL_M  (GPIO_FUNC39_OE_INV_SEL_V << GPIO_FUNC39_OE_INV_SEL_S)
#define GPIO_FUNC39_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC39_OE_INV_SEL_S  11

/** GPIO_FUNC40_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO40 output
 */
#define GPIO_FUNC40_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb94)
/** GPIO_FUNC40_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO40.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 40 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC40_OUT_SEL    0x000001FFU
#define GPIO_FUNC40_OUT_SEL_M  (GPIO_FUNC40_OUT_SEL_V << GPIO_FUNC40_OUT_SEL_S)
#define GPIO_FUNC40_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC40_OUT_SEL_S  0
/** GPIO_FUNC40_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC40_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC40_OUT_INV_SEL_M  (GPIO_FUNC40_OUT_INV_SEL_V << GPIO_FUNC40_OUT_INV_SEL_S)
#define GPIO_FUNC40_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC40_OUT_INV_SEL_S  9
/** GPIO_FUNC40_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 40 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC40_OE_SEL    (BIT(10))
#define GPIO_FUNC40_OE_SEL_M  (GPIO_FUNC40_OE_SEL_V << GPIO_FUNC40_OE_SEL_S)
#define GPIO_FUNC40_OE_SEL_V  0x00000001U
#define GPIO_FUNC40_OE_SEL_S  10
/** GPIO_FUNC40_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC40_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC40_OE_INV_SEL_M  (GPIO_FUNC40_OE_INV_SEL_V << GPIO_FUNC40_OE_INV_SEL_S)
#define GPIO_FUNC40_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC40_OE_INV_SEL_S  11

/** GPIO_FUNC41_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO41 output
 */
#define GPIO_FUNC41_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb98)
/** GPIO_FUNC41_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO41.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 41 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC41_OUT_SEL    0x000001FFU
#define GPIO_FUNC41_OUT_SEL_M  (GPIO_FUNC41_OUT_SEL_V << GPIO_FUNC41_OUT_SEL_S)
#define GPIO_FUNC41_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC41_OUT_SEL_S  0
/** GPIO_FUNC41_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC41_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC41_OUT_INV_SEL_M  (GPIO_FUNC41_OUT_INV_SEL_V << GPIO_FUNC41_OUT_INV_SEL_S)
#define GPIO_FUNC41_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_OUT_INV_SEL_S  9
/** GPIO_FUNC41_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 41 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC41_OE_SEL    (BIT(10))
#define GPIO_FUNC41_OE_SEL_M  (GPIO_FUNC41_OE_SEL_V << GPIO_FUNC41_OE_SEL_S)
#define GPIO_FUNC41_OE_SEL_V  0x00000001U
#define GPIO_FUNC41_OE_SEL_S  10
/** GPIO_FUNC41_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC41_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC41_OE_INV_SEL_M  (GPIO_FUNC41_OE_INV_SEL_V << GPIO_FUNC41_OE_INV_SEL_S)
#define GPIO_FUNC41_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_OE_INV_SEL_S  11

/** GPIO_FUNC42_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO42 output
 */
#define GPIO_FUNC42_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb9c)
/** GPIO_FUNC42_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO42.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 42 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC42_OUT_SEL    0x000001FFU
#define GPIO_FUNC42_OUT_SEL_M  (GPIO_FUNC42_OUT_SEL_V << GPIO_FUNC42_OUT_SEL_S)
#define GPIO_FUNC42_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC42_OUT_SEL_S  0
/** GPIO_FUNC42_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC42_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC42_OUT_INV_SEL_M  (GPIO_FUNC42_OUT_INV_SEL_V << GPIO_FUNC42_OUT_INV_SEL_S)
#define GPIO_FUNC42_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_OUT_INV_SEL_S  9
/** GPIO_FUNC42_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 42 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC42_OE_SEL    (BIT(10))
#define GPIO_FUNC42_OE_SEL_M  (GPIO_FUNC42_OE_SEL_V << GPIO_FUNC42_OE_SEL_S)
#define GPIO_FUNC42_OE_SEL_V  0x00000001U
#define GPIO_FUNC42_OE_SEL_S  10
/** GPIO_FUNC42_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC42_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC42_OE_INV_SEL_M  (GPIO_FUNC42_OE_INV_SEL_V << GPIO_FUNC42_OE_INV_SEL_S)
#define GPIO_FUNC42_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_OE_INV_SEL_S  11

/** GPIO_FUNC43_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO43 output
 */
#define GPIO_FUNC43_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xba0)
/** GPIO_FUNC43_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO43.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 43 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC43_OUT_SEL    0x000001FFU
#define GPIO_FUNC43_OUT_SEL_M  (GPIO_FUNC43_OUT_SEL_V << GPIO_FUNC43_OUT_SEL_S)
#define GPIO_FUNC43_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC43_OUT_SEL_S  0
/** GPIO_FUNC43_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC43_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC43_OUT_INV_SEL_M  (GPIO_FUNC43_OUT_INV_SEL_V << GPIO_FUNC43_OUT_INV_SEL_S)
#define GPIO_FUNC43_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_OUT_INV_SEL_S  9
/** GPIO_FUNC43_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 43 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC43_OE_SEL    (BIT(10))
#define GPIO_FUNC43_OE_SEL_M  (GPIO_FUNC43_OE_SEL_V << GPIO_FUNC43_OE_SEL_S)
#define GPIO_FUNC43_OE_SEL_V  0x00000001U
#define GPIO_FUNC43_OE_SEL_S  10
/** GPIO_FUNC43_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC43_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC43_OE_INV_SEL_M  (GPIO_FUNC43_OE_INV_SEL_V << GPIO_FUNC43_OE_INV_SEL_S)
#define GPIO_FUNC43_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_OE_INV_SEL_S  11

/** GPIO_FUNC44_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO44 output
 */
#define GPIO_FUNC44_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xba4)
/** GPIO_FUNC44_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO44.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 44 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC44_OUT_SEL    0x000001FFU
#define GPIO_FUNC44_OUT_SEL_M  (GPIO_FUNC44_OUT_SEL_V << GPIO_FUNC44_OUT_SEL_S)
#define GPIO_FUNC44_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC44_OUT_SEL_S  0
/** GPIO_FUNC44_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC44_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC44_OUT_INV_SEL_M  (GPIO_FUNC44_OUT_INV_SEL_V << GPIO_FUNC44_OUT_INV_SEL_S)
#define GPIO_FUNC44_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC44_OUT_INV_SEL_S  9
/** GPIO_FUNC44_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 44 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC44_OE_SEL    (BIT(10))
#define GPIO_FUNC44_OE_SEL_M  (GPIO_FUNC44_OE_SEL_V << GPIO_FUNC44_OE_SEL_S)
#define GPIO_FUNC44_OE_SEL_V  0x00000001U
#define GPIO_FUNC44_OE_SEL_S  10
/** GPIO_FUNC44_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC44_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC44_OE_INV_SEL_M  (GPIO_FUNC44_OE_INV_SEL_V << GPIO_FUNC44_OE_INV_SEL_S)
#define GPIO_FUNC44_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC44_OE_INV_SEL_S  11

/** GPIO_FUNC45_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO45 output
 */
#define GPIO_FUNC45_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xba8)
/** GPIO_FUNC45_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO45.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 45 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC45_OUT_SEL    0x000001FFU
#define GPIO_FUNC45_OUT_SEL_M  (GPIO_FUNC45_OUT_SEL_V << GPIO_FUNC45_OUT_SEL_S)
#define GPIO_FUNC45_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC45_OUT_SEL_S  0
/** GPIO_FUNC45_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC45_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC45_OUT_INV_SEL_M  (GPIO_FUNC45_OUT_INV_SEL_V << GPIO_FUNC45_OUT_INV_SEL_S)
#define GPIO_FUNC45_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC45_OUT_INV_SEL_S  9
/** GPIO_FUNC45_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 45 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC45_OE_SEL    (BIT(10))
#define GPIO_FUNC45_OE_SEL_M  (GPIO_FUNC45_OE_SEL_V << GPIO_FUNC45_OE_SEL_S)
#define GPIO_FUNC45_OE_SEL_V  0x00000001U
#define GPIO_FUNC45_OE_SEL_S  10
/** GPIO_FUNC45_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC45_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC45_OE_INV_SEL_M  (GPIO_FUNC45_OE_INV_SEL_V << GPIO_FUNC45_OE_INV_SEL_S)
#define GPIO_FUNC45_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC45_OE_INV_SEL_S  11

/** GPIO_FUNC46_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO46 output
 */
#define GPIO_FUNC46_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbac)
/** GPIO_FUNC46_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO46.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 46 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC46_OUT_SEL    0x000001FFU
#define GPIO_FUNC46_OUT_SEL_M  (GPIO_FUNC46_OUT_SEL_V << GPIO_FUNC46_OUT_SEL_S)
#define GPIO_FUNC46_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC46_OUT_SEL_S  0
/** GPIO_FUNC46_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC46_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC46_OUT_INV_SEL_M  (GPIO_FUNC46_OUT_INV_SEL_V << GPIO_FUNC46_OUT_INV_SEL_S)
#define GPIO_FUNC46_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_OUT_INV_SEL_S  9
/** GPIO_FUNC46_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 46 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC46_OE_SEL    (BIT(10))
#define GPIO_FUNC46_OE_SEL_M  (GPIO_FUNC46_OE_SEL_V << GPIO_FUNC46_OE_SEL_S)
#define GPIO_FUNC46_OE_SEL_V  0x00000001U
#define GPIO_FUNC46_OE_SEL_S  10
/** GPIO_FUNC46_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC46_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC46_OE_INV_SEL_M  (GPIO_FUNC46_OE_INV_SEL_V << GPIO_FUNC46_OE_INV_SEL_S)
#define GPIO_FUNC46_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_OE_INV_SEL_S  11

/** GPIO_FUNC47_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO47 output
 */
#define GPIO_FUNC47_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbb0)
/** GPIO_FUNC47_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO47.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 47 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC47_OUT_SEL    0x000001FFU
#define GPIO_FUNC47_OUT_SEL_M  (GPIO_FUNC47_OUT_SEL_V << GPIO_FUNC47_OUT_SEL_S)
#define GPIO_FUNC47_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC47_OUT_SEL_S  0
/** GPIO_FUNC47_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC47_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC47_OUT_INV_SEL_M  (GPIO_FUNC47_OUT_INV_SEL_V << GPIO_FUNC47_OUT_INV_SEL_S)
#define GPIO_FUNC47_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_OUT_INV_SEL_S  9
/** GPIO_FUNC47_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 47 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC47_OE_SEL    (BIT(10))
#define GPIO_FUNC47_OE_SEL_M  (GPIO_FUNC47_OE_SEL_V << GPIO_FUNC47_OE_SEL_S)
#define GPIO_FUNC47_OE_SEL_V  0x00000001U
#define GPIO_FUNC47_OE_SEL_S  10
/** GPIO_FUNC47_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC47_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC47_OE_INV_SEL_M  (GPIO_FUNC47_OE_INV_SEL_V << GPIO_FUNC47_OE_INV_SEL_S)
#define GPIO_FUNC47_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_OE_INV_SEL_S  11

/** GPIO_FUNC48_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO48 output
 */
#define GPIO_FUNC48_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbb4)
/** GPIO_FUNC48_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO48.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 48 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC48_OUT_SEL    0x000001FFU
#define GPIO_FUNC48_OUT_SEL_M  (GPIO_FUNC48_OUT_SEL_V << GPIO_FUNC48_OUT_SEL_S)
#define GPIO_FUNC48_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC48_OUT_SEL_S  0
/** GPIO_FUNC48_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC48_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC48_OUT_INV_SEL_M  (GPIO_FUNC48_OUT_INV_SEL_V << GPIO_FUNC48_OUT_INV_SEL_S)
#define GPIO_FUNC48_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_OUT_INV_SEL_S  9
/** GPIO_FUNC48_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 48 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC48_OE_SEL    (BIT(10))
#define GPIO_FUNC48_OE_SEL_M  (GPIO_FUNC48_OE_SEL_V << GPIO_FUNC48_OE_SEL_S)
#define GPIO_FUNC48_OE_SEL_V  0x00000001U
#define GPIO_FUNC48_OE_SEL_S  10
/** GPIO_FUNC48_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC48_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC48_OE_INV_SEL_M  (GPIO_FUNC48_OE_INV_SEL_V << GPIO_FUNC48_OE_INV_SEL_S)
#define GPIO_FUNC48_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_OE_INV_SEL_S  11

/** GPIO_FUNC49_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO49 output
 */
#define GPIO_FUNC49_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbb8)
/** GPIO_FUNC49_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO49.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 49 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC49_OUT_SEL    0x000001FFU
#define GPIO_FUNC49_OUT_SEL_M  (GPIO_FUNC49_OUT_SEL_V << GPIO_FUNC49_OUT_SEL_S)
#define GPIO_FUNC49_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC49_OUT_SEL_S  0
/** GPIO_FUNC49_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC49_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC49_OUT_INV_SEL_M  (GPIO_FUNC49_OUT_INV_SEL_V << GPIO_FUNC49_OUT_INV_SEL_S)
#define GPIO_FUNC49_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_OUT_INV_SEL_S  9
/** GPIO_FUNC49_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 49 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC49_OE_SEL    (BIT(10))
#define GPIO_FUNC49_OE_SEL_M  (GPIO_FUNC49_OE_SEL_V << GPIO_FUNC49_OE_SEL_S)
#define GPIO_FUNC49_OE_SEL_V  0x00000001U
#define GPIO_FUNC49_OE_SEL_S  10
/** GPIO_FUNC49_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC49_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC49_OE_INV_SEL_M  (GPIO_FUNC49_OE_INV_SEL_V << GPIO_FUNC49_OE_INV_SEL_S)
#define GPIO_FUNC49_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_OE_INV_SEL_S  11

/** GPIO_FUNC50_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO50 output
 */
#define GPIO_FUNC50_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbbc)
/** GPIO_FUNC50_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO50.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 50 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC50_OUT_SEL    0x000001FFU
#define GPIO_FUNC50_OUT_SEL_M  (GPIO_FUNC50_OUT_SEL_V << GPIO_FUNC50_OUT_SEL_S)
#define GPIO_FUNC50_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC50_OUT_SEL_S  0
/** GPIO_FUNC50_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC50_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC50_OUT_INV_SEL_M  (GPIO_FUNC50_OUT_INV_SEL_V << GPIO_FUNC50_OUT_INV_SEL_S)
#define GPIO_FUNC50_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_OUT_INV_SEL_S  9
/** GPIO_FUNC50_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 50 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC50_OE_SEL    (BIT(10))
#define GPIO_FUNC50_OE_SEL_M  (GPIO_FUNC50_OE_SEL_V << GPIO_FUNC50_OE_SEL_S)
#define GPIO_FUNC50_OE_SEL_V  0x00000001U
#define GPIO_FUNC50_OE_SEL_S  10
/** GPIO_FUNC50_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC50_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC50_OE_INV_SEL_M  (GPIO_FUNC50_OE_INV_SEL_V << GPIO_FUNC50_OE_INV_SEL_S)
#define GPIO_FUNC50_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_OE_INV_SEL_S  11

/** GPIO_FUNC51_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO51 output
 */
#define GPIO_FUNC51_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbc0)
/** GPIO_FUNC51_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO51.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 51 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC51_OUT_SEL    0x000001FFU
#define GPIO_FUNC51_OUT_SEL_M  (GPIO_FUNC51_OUT_SEL_V << GPIO_FUNC51_OUT_SEL_S)
#define GPIO_FUNC51_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC51_OUT_SEL_S  0
/** GPIO_FUNC51_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC51_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC51_OUT_INV_SEL_M  (GPIO_FUNC51_OUT_INV_SEL_V << GPIO_FUNC51_OUT_INV_SEL_S)
#define GPIO_FUNC51_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_OUT_INV_SEL_S  9
/** GPIO_FUNC51_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 51 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC51_OE_SEL    (BIT(10))
#define GPIO_FUNC51_OE_SEL_M  (GPIO_FUNC51_OE_SEL_V << GPIO_FUNC51_OE_SEL_S)
#define GPIO_FUNC51_OE_SEL_V  0x00000001U
#define GPIO_FUNC51_OE_SEL_S  10
/** GPIO_FUNC51_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC51_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC51_OE_INV_SEL_M  (GPIO_FUNC51_OE_INV_SEL_V << GPIO_FUNC51_OE_INV_SEL_S)
#define GPIO_FUNC51_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_OE_INV_SEL_S  11

/** GPIO_FUNC52_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO52 output
 */
#define GPIO_FUNC52_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbc4)
/** GPIO_FUNC52_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO52.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 52 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC52_OUT_SEL    0x000001FFU
#define GPIO_FUNC52_OUT_SEL_M  (GPIO_FUNC52_OUT_SEL_V << GPIO_FUNC52_OUT_SEL_S)
#define GPIO_FUNC52_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC52_OUT_SEL_S  0
/** GPIO_FUNC52_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC52_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC52_OUT_INV_SEL_M  (GPIO_FUNC52_OUT_INV_SEL_V << GPIO_FUNC52_OUT_INV_SEL_S)
#define GPIO_FUNC52_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_OUT_INV_SEL_S  9
/** GPIO_FUNC52_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 52 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC52_OE_SEL    (BIT(10))
#define GPIO_FUNC52_OE_SEL_M  (GPIO_FUNC52_OE_SEL_V << GPIO_FUNC52_OE_SEL_S)
#define GPIO_FUNC52_OE_SEL_V  0x00000001U
#define GPIO_FUNC52_OE_SEL_S  10
/** GPIO_FUNC52_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC52_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC52_OE_INV_SEL_M  (GPIO_FUNC52_OE_INV_SEL_V << GPIO_FUNC52_OE_INV_SEL_S)
#define GPIO_FUNC52_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_OE_INV_SEL_S  11

/** GPIO_FUNC53_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO53 output
 */
#define GPIO_FUNC53_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbc8)
/** GPIO_FUNC53_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO53.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 53 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC53_OUT_SEL    0x000001FFU
#define GPIO_FUNC53_OUT_SEL_M  (GPIO_FUNC53_OUT_SEL_V << GPIO_FUNC53_OUT_SEL_S)
#define GPIO_FUNC53_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC53_OUT_SEL_S  0
/** GPIO_FUNC53_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC53_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC53_OUT_INV_SEL_M  (GPIO_FUNC53_OUT_INV_SEL_V << GPIO_FUNC53_OUT_INV_SEL_S)
#define GPIO_FUNC53_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_OUT_INV_SEL_S  9
/** GPIO_FUNC53_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 53 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC53_OE_SEL    (BIT(10))
#define GPIO_FUNC53_OE_SEL_M  (GPIO_FUNC53_OE_SEL_V << GPIO_FUNC53_OE_SEL_S)
#define GPIO_FUNC53_OE_SEL_V  0x00000001U
#define GPIO_FUNC53_OE_SEL_S  10
/** GPIO_FUNC53_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC53_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC53_OE_INV_SEL_M  (GPIO_FUNC53_OE_INV_SEL_V << GPIO_FUNC53_OE_INV_SEL_S)
#define GPIO_FUNC53_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_OE_INV_SEL_S  11

/** GPIO_FUNC54_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO54 output
 */
#define GPIO_FUNC54_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbcc)
/** GPIO_FUNC54_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO54.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 54 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC54_OUT_SEL    0x000001FFU
#define GPIO_FUNC54_OUT_SEL_M  (GPIO_FUNC54_OUT_SEL_V << GPIO_FUNC54_OUT_SEL_S)
#define GPIO_FUNC54_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC54_OUT_SEL_S  0
/** GPIO_FUNC54_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC54_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC54_OUT_INV_SEL_M  (GPIO_FUNC54_OUT_INV_SEL_V << GPIO_FUNC54_OUT_INV_SEL_S)
#define GPIO_FUNC54_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_OUT_INV_SEL_S  9
/** GPIO_FUNC54_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 54 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC54_OE_SEL    (BIT(10))
#define GPIO_FUNC54_OE_SEL_M  (GPIO_FUNC54_OE_SEL_V << GPIO_FUNC54_OE_SEL_S)
#define GPIO_FUNC54_OE_SEL_V  0x00000001U
#define GPIO_FUNC54_OE_SEL_S  10
/** GPIO_FUNC54_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC54_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC54_OE_INV_SEL_M  (GPIO_FUNC54_OE_INV_SEL_V << GPIO_FUNC54_OE_INV_SEL_S)
#define GPIO_FUNC54_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_OE_INV_SEL_S  11

/** GPIO_FUNC55_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO55 output
 */
#define GPIO_FUNC55_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbd0)
/** GPIO_FUNC55_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO55.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 55 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC55_OUT_SEL    0x000001FFU
#define GPIO_FUNC55_OUT_SEL_M  (GPIO_FUNC55_OUT_SEL_V << GPIO_FUNC55_OUT_SEL_S)
#define GPIO_FUNC55_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC55_OUT_SEL_S  0
/** GPIO_FUNC55_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC55_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC55_OUT_INV_SEL_M  (GPIO_FUNC55_OUT_INV_SEL_V << GPIO_FUNC55_OUT_INV_SEL_S)
#define GPIO_FUNC55_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_OUT_INV_SEL_S  9
/** GPIO_FUNC55_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 55 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC55_OE_SEL    (BIT(10))
#define GPIO_FUNC55_OE_SEL_M  (GPIO_FUNC55_OE_SEL_V << GPIO_FUNC55_OE_SEL_S)
#define GPIO_FUNC55_OE_SEL_V  0x00000001U
#define GPIO_FUNC55_OE_SEL_S  10
/** GPIO_FUNC55_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC55_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC55_OE_INV_SEL_M  (GPIO_FUNC55_OE_INV_SEL_V << GPIO_FUNC55_OE_INV_SEL_S)
#define GPIO_FUNC55_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_OE_INV_SEL_S  11

/** GPIO_FUNC56_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO56 output
 */
#define GPIO_FUNC56_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbd4)
/** GPIO_FUNC56_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO56.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 56 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC56_OUT_SEL    0x000001FFU
#define GPIO_FUNC56_OUT_SEL_M  (GPIO_FUNC56_OUT_SEL_V << GPIO_FUNC56_OUT_SEL_S)
#define GPIO_FUNC56_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC56_OUT_SEL_S  0
/** GPIO_FUNC56_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC56_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC56_OUT_INV_SEL_M  (GPIO_FUNC56_OUT_INV_SEL_V << GPIO_FUNC56_OUT_INV_SEL_S)
#define GPIO_FUNC56_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_OUT_INV_SEL_S  9
/** GPIO_FUNC56_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 56 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC56_OE_SEL    (BIT(10))
#define GPIO_FUNC56_OE_SEL_M  (GPIO_FUNC56_OE_SEL_V << GPIO_FUNC56_OE_SEL_S)
#define GPIO_FUNC56_OE_SEL_V  0x00000001U
#define GPIO_FUNC56_OE_SEL_S  10
/** GPIO_FUNC56_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC56_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC56_OE_INV_SEL_M  (GPIO_FUNC56_OE_INV_SEL_V << GPIO_FUNC56_OE_INV_SEL_S)
#define GPIO_FUNC56_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_OE_INV_SEL_S  11

/** GPIO_FUNC57_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO57 output
 */
#define GPIO_FUNC57_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbd8)
/** GPIO_FUNC57_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO57.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 57 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC57_OUT_SEL    0x000001FFU
#define GPIO_FUNC57_OUT_SEL_M  (GPIO_FUNC57_OUT_SEL_V << GPIO_FUNC57_OUT_SEL_S)
#define GPIO_FUNC57_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC57_OUT_SEL_S  0
/** GPIO_FUNC57_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC57_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC57_OUT_INV_SEL_M  (GPIO_FUNC57_OUT_INV_SEL_V << GPIO_FUNC57_OUT_INV_SEL_S)
#define GPIO_FUNC57_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC57_OUT_INV_SEL_S  9
/** GPIO_FUNC57_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 57 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC57_OE_SEL    (BIT(10))
#define GPIO_FUNC57_OE_SEL_M  (GPIO_FUNC57_OE_SEL_V << GPIO_FUNC57_OE_SEL_S)
#define GPIO_FUNC57_OE_SEL_V  0x00000001U
#define GPIO_FUNC57_OE_SEL_S  10
/** GPIO_FUNC57_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC57_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC57_OE_INV_SEL_M  (GPIO_FUNC57_OE_INV_SEL_V << GPIO_FUNC57_OE_INV_SEL_S)
#define GPIO_FUNC57_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC57_OE_INV_SEL_S  11

/** GPIO_FUNC58_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO58 output
 */
#define GPIO_FUNC58_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbdc)
/** GPIO_FUNC58_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO58.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 58 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC58_OUT_SEL    0x000001FFU
#define GPIO_FUNC58_OUT_SEL_M  (GPIO_FUNC58_OUT_SEL_V << GPIO_FUNC58_OUT_SEL_S)
#define GPIO_FUNC58_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC58_OUT_SEL_S  0
/** GPIO_FUNC58_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC58_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC58_OUT_INV_SEL_M  (GPIO_FUNC58_OUT_INV_SEL_V << GPIO_FUNC58_OUT_INV_SEL_S)
#define GPIO_FUNC58_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC58_OUT_INV_SEL_S  9
/** GPIO_FUNC58_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 58 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC58_OE_SEL    (BIT(10))
#define GPIO_FUNC58_OE_SEL_M  (GPIO_FUNC58_OE_SEL_V << GPIO_FUNC58_OE_SEL_S)
#define GPIO_FUNC58_OE_SEL_V  0x00000001U
#define GPIO_FUNC58_OE_SEL_S  10
/** GPIO_FUNC58_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC58_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC58_OE_INV_SEL_M  (GPIO_FUNC58_OE_INV_SEL_V << GPIO_FUNC58_OE_INV_SEL_S)
#define GPIO_FUNC58_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC58_OE_INV_SEL_S  11

/** GPIO_FUNC59_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO59 output
 */
#define GPIO_FUNC59_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbe0)
/** GPIO_FUNC59_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO59.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 59 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC59_OUT_SEL    0x000001FFU
#define GPIO_FUNC59_OUT_SEL_M  (GPIO_FUNC59_OUT_SEL_V << GPIO_FUNC59_OUT_SEL_S)
#define GPIO_FUNC59_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC59_OUT_SEL_S  0
/** GPIO_FUNC59_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC59_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC59_OUT_INV_SEL_M  (GPIO_FUNC59_OUT_INV_SEL_V << GPIO_FUNC59_OUT_INV_SEL_S)
#define GPIO_FUNC59_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC59_OUT_INV_SEL_S  9
/** GPIO_FUNC59_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 59 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC59_OE_SEL    (BIT(10))
#define GPIO_FUNC59_OE_SEL_M  (GPIO_FUNC59_OE_SEL_V << GPIO_FUNC59_OE_SEL_S)
#define GPIO_FUNC59_OE_SEL_V  0x00000001U
#define GPIO_FUNC59_OE_SEL_S  10
/** GPIO_FUNC59_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC59_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC59_OE_INV_SEL_M  (GPIO_FUNC59_OE_INV_SEL_V << GPIO_FUNC59_OE_INV_SEL_S)
#define GPIO_FUNC59_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC59_OE_INV_SEL_S  11

/** GPIO_FUNC60_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO60 output
 */
#define GPIO_FUNC60_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbe4)
/** GPIO_FUNC60_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO60.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 60 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC60_OUT_SEL    0x000001FFU
#define GPIO_FUNC60_OUT_SEL_M  (GPIO_FUNC60_OUT_SEL_V << GPIO_FUNC60_OUT_SEL_S)
#define GPIO_FUNC60_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC60_OUT_SEL_S  0
/** GPIO_FUNC60_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC60_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC60_OUT_INV_SEL_M  (GPIO_FUNC60_OUT_INV_SEL_V << GPIO_FUNC60_OUT_INV_SEL_S)
#define GPIO_FUNC60_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC60_OUT_INV_SEL_S  9
/** GPIO_FUNC60_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 60 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC60_OE_SEL    (BIT(10))
#define GPIO_FUNC60_OE_SEL_M  (GPIO_FUNC60_OE_SEL_V << GPIO_FUNC60_OE_SEL_S)
#define GPIO_FUNC60_OE_SEL_V  0x00000001U
#define GPIO_FUNC60_OE_SEL_S  10
/** GPIO_FUNC60_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC60_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC60_OE_INV_SEL_M  (GPIO_FUNC60_OE_INV_SEL_V << GPIO_FUNC60_OE_INV_SEL_S)
#define GPIO_FUNC60_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC60_OE_INV_SEL_S  11

/** GPIO_FUNC61_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO61 output
 */
#define GPIO_FUNC61_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbe8)
/** GPIO_FUNC61_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO61.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 61 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC61_OUT_SEL    0x000001FFU
#define GPIO_FUNC61_OUT_SEL_M  (GPIO_FUNC61_OUT_SEL_V << GPIO_FUNC61_OUT_SEL_S)
#define GPIO_FUNC61_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC61_OUT_SEL_S  0
/** GPIO_FUNC61_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC61_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC61_OUT_INV_SEL_M  (GPIO_FUNC61_OUT_INV_SEL_V << GPIO_FUNC61_OUT_INV_SEL_S)
#define GPIO_FUNC61_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC61_OUT_INV_SEL_S  9
/** GPIO_FUNC61_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 61 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC61_OE_SEL    (BIT(10))
#define GPIO_FUNC61_OE_SEL_M  (GPIO_FUNC61_OE_SEL_V << GPIO_FUNC61_OE_SEL_S)
#define GPIO_FUNC61_OE_SEL_V  0x00000001U
#define GPIO_FUNC61_OE_SEL_S  10
/** GPIO_FUNC61_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC61_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC61_OE_INV_SEL_M  (GPIO_FUNC61_OE_INV_SEL_V << GPIO_FUNC61_OE_INV_SEL_S)
#define GPIO_FUNC61_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC61_OE_INV_SEL_S  11

/** GPIO_FUNC62_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO62 output
 */
#define GPIO_FUNC62_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xbec)
/** GPIO_FUNC62_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO62.
 *  0: Select signal 0
 *  1: Select signal 1
 *  ......
 *  254: Select signal 254
 *  255: Select signal 255
 *  Or
 *  256: Bit 62 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table .
 *  "
 */
#define GPIO_FUNC62_OUT_SEL    0x000001FFU
#define GPIO_FUNC62_OUT_SEL_M  (GPIO_FUNC62_OUT_SEL_V << GPIO_FUNC62_OUT_SEL_S)
#define GPIO_FUNC62_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC62_OUT_SEL_S  0
/** GPIO_FUNC62_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC62_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC62_OUT_INV_SEL_M  (GPIO_FUNC62_OUT_INV_SEL_V << GPIO_FUNC62_OUT_INV_SEL_S)
#define GPIO_FUNC62_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC62_OUT_INV_SEL_S  9
/** GPIO_FUNC62_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.
 *  0: Use output enable signal from peripheral.
 *  1: Force the output enable signal to be sourced from bit 62 of GPIO_ENABLE_REG.
 */
#define GPIO_FUNC62_OE_SEL    (BIT(10))
#define GPIO_FUNC62_OE_SEL_M  (GPIO_FUNC62_OE_SEL_V << GPIO_FUNC62_OE_SEL_S)
#define GPIO_FUNC62_OE_SEL_V  0x00000001U
#define GPIO_FUNC62_OE_SEL_S  10
/** GPIO_FUNC62_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.
 *  0: Not invert
 *  1: Invert
 */
#define GPIO_FUNC62_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC62_OE_INV_SEL_M  (GPIO_FUNC62_OE_INV_SEL_V << GPIO_FUNC62_OE_INV_SEL_S)
#define GPIO_FUNC62_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC62_OE_INV_SEL_S  11

/** GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define GPIO_CLOCK_GATE_REG (DR_REG_GPIO_BASE + 0xdf8)
/** GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to enable clock gate.
 *  0: Not enable
 *  1: Enable, the clock is free running.
 */
#define GPIO_CLK_EN    (BIT(0))
#define GPIO_CLK_EN_M  (GPIO_CLK_EN_V << GPIO_CLK_EN_S)
#define GPIO_CLK_EN_V  0x00000001U
#define GPIO_CLK_EN_S  0

/** GPIO_DATE_REG register
 *  GPIO version register
 */
#define GPIO_DATE_REG (DR_REG_GPIO_BASE + 0xdfc)
/** GPIO_DATE : R/W; bitpos: [27:0]; default: 38830416;
 *  Version control register.
 */
#define GPIO_DATE    0x0FFFFFFFU
#define GPIO_DATE_M  (GPIO_DATE_V << GPIO_DATE_S)
#define GPIO_DATE_V  0x0FFFFFFFU
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
