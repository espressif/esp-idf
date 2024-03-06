/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 *  Represents the values of GPIO strapping pins.
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
 *  Configures the output value of GPIO0 ~ 31 output in simple GPIO output mode.\\
 *  0: Low level\\
 *  1: High level\\
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
 *  Configures whether or not to set the output register GPIO_OUT_REG of GPIO0 ~
 *  GPIO31.\\
 *  0: Not set\\
 *  1: The corresponding bit in GPIO_OUT_REG will be set to 1\\
 *  Recommended operation: use this register to set GPIO_OUT_REG. \\
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
 *  GPIO31 output.\\
 *  0: Not clear\\
 *  1: The corresponding bit in GPIO_OUT_REG will be cleared.\\
 *  Recommended operation: use this register to clear GPIO_OUT_REG. \\
 */
#define GPIO_OUT_W1TC    0xFFFFFFFFU
#define GPIO_OUT_W1TC_M  (GPIO_OUT_W1TC_V << GPIO_OUT_W1TC_S)
#define GPIO_OUT_W1TC_V  0xFFFFFFFFU
#define GPIO_OUT_W1TC_S  0

/** GPIO_OUT1_REG register
 *  GPIO output register for GPIO32-32
 */
#define GPIO_OUT1_REG (DR_REG_GPIO_BASE + 0x10)
/** GPIO_OUT1_DATA_ORIG : R/W/SC/WTC; bitpos: [0]; default: 0;
 *  Configures the output value of GPIO32 ~ 32 output in simple GPIO output mode.\\
 *  0: Low level\\
 *  1: High level\\
 */
#define GPIO_OUT1_DATA_ORIG    (BIT(0))
#define GPIO_OUT1_DATA_ORIG_M  (GPIO_OUT1_DATA_ORIG_V << GPIO_OUT1_DATA_ORIG_S)
#define GPIO_OUT1_DATA_ORIG_V  0x00000001U
#define GPIO_OUT1_DATA_ORIG_S  0

/** GPIO_OUT1_W1TS_REG register
 *  GPIO output set register for GPIO32-32
 */
#define GPIO_OUT1_W1TS_REG (DR_REG_GPIO_BASE + 0x14)
/** GPIO_OUT1_W1TS : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to set the output register GPIO_OUT1_REG of GPIO32 ~
 *  GPIO32.\\
 *  0: Not set\\
 *  1: The corresponding bit in GPIO_OUT_REG will be set to 1\\
 *  Recommended operation: use this register to set GPIO_OUT1_REG. \\
 */
#define GPIO_OUT1_W1TS    (BIT(0))
#define GPIO_OUT1_W1TS_M  (GPIO_OUT1_W1TS_V << GPIO_OUT1_W1TS_S)
#define GPIO_OUT1_W1TS_V  0x00000001U
#define GPIO_OUT1_W1TS_S  0

/** GPIO_OUT1_W1TC_REG register
 *  GPIO output clear register for GPIO32-32
 */
#define GPIO_OUT1_W1TC_REG (DR_REG_GPIO_BASE + 0x18)
/** GPIO_OUT1_W1TC : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear the output register GPIO_OUT1_REG of GPIO32 ~
 *  GPIO32 output.\\
 *  0: Not clear\\
 *  1: The corresponding bit in GPIO_OUT_REG will be cleared.\\
 *  Recommended operation: use this register to clear GPIO_OUT1_REG. \\
 */
#define GPIO_OUT1_W1TC    (BIT(0))
#define GPIO_OUT1_W1TC_M  (GPIO_OUT1_W1TC_V << GPIO_OUT1_W1TC_S)
#define GPIO_OUT1_W1TC_V  0x00000001U
#define GPIO_OUT1_W1TC_S  0

/** GPIO_ENABLE_REG register
 *  GPIO output enable register
 */
#define GPIO_ENABLE_REG (DR_REG_GPIO_BASE + 0x34)
/** GPIO_ENABLE_DATA : R/W/WTC; bitpos: [31:0]; default: 0;
 *  Configures whether or not to enable the output of GPIO0 ~ GPIO31.\\
 *  0: Not enable\\
 *  1: Enable\\
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
 *  GPIO0 ~ GPIO31.\\
 *  0: Not set\\
 *  1: The corresponding bit in GPIO_ENABLE_REG will be set to 1\\
 *  Recommended operation: use this register to set GPIO_ENABLE_REG.\\
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
 *  GPIO0 ~ GPIO31. \\
 *  0: Not clear\\
 *  1: The corresponding bit in GPIO_ENABLE_REG will be cleared\\
 *  Recommended operation: use this register to clear GPIO_ENABLE_REG.\\
 */
#define GPIO_ENABLE_W1TC    0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_M  (GPIO_ENABLE_W1TC_V << GPIO_ENABLE_W1TC_S)
#define GPIO_ENABLE_W1TC_V  0xFFFFFFFFU
#define GPIO_ENABLE_W1TC_S  0

/** GPIO_ENABLE1_REG register
 *  GPIO output enable register for GPIO32-32
 */
#define GPIO_ENABLE1_REG (DR_REG_GPIO_BASE + 0x40)
/** GPIO_ENABLE1_DATA : R/W/WTC; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO32 ~ GPIO32.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_ENABLE1_DATA    (BIT(0))
#define GPIO_ENABLE1_DATA_M  (GPIO_ENABLE1_DATA_V << GPIO_ENABLE1_DATA_S)
#define GPIO_ENABLE1_DATA_V  0x00000001U
#define GPIO_ENABLE1_DATA_S  0

/** GPIO_ENABLE1_W1TS_REG register
 *  GPIO output enable set register for GPIO32-32
 */
#define GPIO_ENABLE1_W1TS_REG (DR_REG_GPIO_BASE + 0x44)
/** GPIO_ENABLE1_W1TS : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to set the output enable register GPIO_ENABLE_REG of
 *  GPIO32 ~ GPIO32.\\
 *  0: Not set\\
 *  1: The corresponding bit in GPIO_ENABLE_REG will be set to 1\\
 *  Recommended operation: use this register to set GPIO_ENABLE1_REG.\\
 */
#define GPIO_ENABLE1_W1TS    (BIT(0))
#define GPIO_ENABLE1_W1TS_M  (GPIO_ENABLE1_W1TS_V << GPIO_ENABLE1_W1TS_S)
#define GPIO_ENABLE1_W1TS_V  0x00000001U
#define GPIO_ENABLE1_W1TS_S  0

/** GPIO_ENABLE1_W1TC_REG register
 *  GPIO output enable clear register for GPIO32-32
 */
#define GPIO_ENABLE1_W1TC_REG (DR_REG_GPIO_BASE + 0x48)
/** GPIO_ENABLE1_W1TC : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear the output enable register GPIO_ENABLE_REG of
 *  GPIO32 ~ GPIO32. \\
 *  0: Not clear\\
 *  1: The corresponding bit in GPIO_ENABLE_REG will be cleared\\
 *  Recommended operation: use this register to clear GPIO_ENABLE1_REG.\\
 */
#define GPIO_ENABLE1_W1TC    (BIT(0))
#define GPIO_ENABLE1_W1TC_M  (GPIO_ENABLE1_W1TC_V << GPIO_ENABLE1_W1TC_S)
#define GPIO_ENABLE1_W1TC_V  0x00000001U
#define GPIO_ENABLE1_W1TC_S  0

/** GPIO_IN_REG register
 *  GPIO input register
 */
#define GPIO_IN_REG (DR_REG_GPIO_BASE + 0x64)
/** GPIO_IN_DATA_NEXT : RO; bitpos: [31:0]; default: 0;
 *  Represents the input value of GPIO0 ~ GPIO31. Each bit represents a pin input
 *  value:\\
 *  0: Low level\\
 *  1: High level\\
 */
#define GPIO_IN_DATA_NEXT    0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_M  (GPIO_IN_DATA_NEXT_V << GPIO_IN_DATA_NEXT_S)
#define GPIO_IN_DATA_NEXT_V  0xFFFFFFFFU
#define GPIO_IN_DATA_NEXT_S  0

/** GPIO_IN1_REG register
 *  GPIO input register for GPIO32-32
 */
#define GPIO_IN1_REG (DR_REG_GPIO_BASE + 0x68)
/** GPIO_IN1_DATA_NEXT : RO; bitpos: [0]; default: 0;
 *  Represents the input value of GPIO32 ~ GPIO32. Each bit represents a pin input
 *  value:\\
 *  0: Low level\\
 *  1: High level\\
 */
#define GPIO_IN1_DATA_NEXT    (BIT(0))
#define GPIO_IN1_DATA_NEXT_M  (GPIO_IN1_DATA_NEXT_V << GPIO_IN1_DATA_NEXT_S)
#define GPIO_IN1_DATA_NEXT_V  0x00000001U
#define GPIO_IN1_DATA_NEXT_S  0

/** GPIO_STATUS_REG register
 *  GPIO interrupt status register
 */
#define GPIO_STATUS_REG (DR_REG_GPIO_BASE + 0x74)
/** GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [31:0]; default: 0;
 *  The interrupt status of GPIO0 ~ GPIO31, can be configured by the software.
 *
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
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS_W1TC    0xFFFFFFFFU
#define GPIO_STATUS_W1TC_M  (GPIO_STATUS_W1TC_V << GPIO_STATUS_W1TC_S)
#define GPIO_STATUS_W1TC_V  0xFFFFFFFFU
#define GPIO_STATUS_W1TC_S  0

/** GPIO_STATUS1_REG register
 *  GPIO interrupt status register for GPIO32-32
 */
#define GPIO_STATUS1_REG (DR_REG_GPIO_BASE + 0x80)
/** GPIO_STATUS1_INTERRUPT : R/W/WTC; bitpos: [0]; default: 0;
 *  The interrupt status of GPIO32 ~ GPIO32, can be configured by the software.
 *
 *  - Each bit represents the status of its corresponding GPIO:
 *
 *  - 0: Represents the GPIO does not generate the interrupt configured by
 *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
 *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
 *  or this bit is configured to 1 by the software.
 *
 */
#define GPIO_STATUS1_INTERRUPT    (BIT(0))
#define GPIO_STATUS1_INTERRUPT_M  (GPIO_STATUS1_INTERRUPT_V << GPIO_STATUS1_INTERRUPT_S)
#define GPIO_STATUS1_INTERRUPT_V  0x00000001U
#define GPIO_STATUS1_INTERRUPT_S  0

/** GPIO_STATUS1_W1TS_REG register
 *  GPIO interrupt status set register for GPIO32-32
 */
#define GPIO_STATUS1_W1TS_REG (DR_REG_GPIO_BASE + 0x84)
/** GPIO_STATUS1_W1TS : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to set the interrupt status register
 *  GPIO_STATUS_INTERRUPT of GPIO32 ~ GPIO32.
 *
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be set to 1. \item Recommended operation: use this
 *  register to set GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS1_W1TS    (BIT(0))
#define GPIO_STATUS1_W1TS_M  (GPIO_STATUS1_W1TS_V << GPIO_STATUS1_W1TS_S)
#define GPIO_STATUS1_W1TS_V  0x00000001U
#define GPIO_STATUS1_W1TS_S  0

/** GPIO_STATUS1_W1TC_REG register
 *  GPIO interrupt status clear register for GPIO32-32
 */
#define GPIO_STATUS1_W1TC_REG (DR_REG_GPIO_BASE + 0x88)
/** GPIO_STATUS1_W1TC : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear the interrupt status register
 *  GPIO_STATUS_INTERRUPT of GPIO32 ~ GPIO32.
 *
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS1_W1TC    (BIT(0))
#define GPIO_STATUS1_W1TC_M  (GPIO_STATUS1_W1TC_V << GPIO_STATUS1_W1TC_S)
#define GPIO_STATUS1_W1TC_V  0x00000001U
#define GPIO_STATUS1_W1TC_S  0

/** GPIO_PROCPU_INT_REG register
 *  GPIO_PROCPU_INT interrupt status register for GPIO0-31
 */
#define GPIO_PROCPU_INT_REG (DR_REG_GPIO_BASE + 0xa4)
/** GPIO_PROCPU_INT : RO; bitpos: [31:0]; default: 0;
 *  Represents the CPU interrupt status of GPIO0 ~ GPIO31. Each bit represents:\\
 *  0: Represents CPU interrupt is not enabled, or the GPIO does not generate the
 *  interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the CPU interrupt is enabled.\\
 *  This interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert
 *  (high) enable signal (bit13 of GPIO_PIN$n_REG). \\
 */
#define GPIO_PROCPU_INT    0xFFFFFFFFU
#define GPIO_PROCPU_INT_M  (GPIO_PROCPU_INT_V << GPIO_PROCPU_INT_S)
#define GPIO_PROCPU_INT_V  0xFFFFFFFFU
#define GPIO_PROCPU_INT_S  0

/** GPIO_PROCPU_INT1_REG register
 *  GPIO_PROCPU_INT interrupt status register for GPIO32-32
 */
#define GPIO_PROCPU_INT1_REG (DR_REG_GPIO_BASE + 0xa8)
/** GPIO_PROCPU_INT1 : RO; bitpos: [0]; default: 0;
 *  Represents the CPU interrupt status of GPIO32 ~ GPIO32. Each bit represents:\\
 *  0: Represents CPU interrupt is not enabled, or the GPIO does not generate the
 *  interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the CPU interrupt is enabled.\\
 *  This interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert
 *  (high) enable signal (bit13 of GPIO_PIN$n_REG). \\
 */
#define GPIO_PROCPU_INT1    (BIT(0))
#define GPIO_PROCPU_INT1_M  (GPIO_PROCPU_INT1_V << GPIO_PROCPU_INT1_S)
#define GPIO_PROCPU_INT1_V  0x00000001U
#define GPIO_PROCPU_INT1_S  0

/** GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register
 */
#define GPIO_STATUS_NEXT_REG (DR_REG_GPIO_BASE + 0xb4)
/** GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [31:0]; default: 0;
 *  Represents the interrupt source signal of GPIO0 ~ GPIO31.\\
 *  Each bit represents:\\
 *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.\\
 */
#define GPIO_STATUS_INTERRUPT_NEXT    0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_M  (GPIO_STATUS_INTERRUPT_NEXT_V << GPIO_STATUS_INTERRUPT_NEXT_S)
#define GPIO_STATUS_INTERRUPT_NEXT_V  0xFFFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

/** GPIO_STATUS_NEXT1_REG register
 *  GPIO interrupt source register for GPIO32-32
 */
#define GPIO_STATUS_NEXT1_REG (DR_REG_GPIO_BASE + 0xb8)
/** GPIO_STATUS_INTERRUPT_NEXT1 : RO; bitpos: [0]; default: 0;
 *  Represents the interrupt source signal of GPIO32 ~ GPIO32.\\
 *  Each bit represents:\\
 *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.\\
 */
#define GPIO_STATUS_INTERRUPT_NEXT1    (BIT(0))
#define GPIO_STATUS_INTERRUPT_NEXT1_M  (GPIO_STATUS_INTERRUPT_NEXT1_V << GPIO_STATUS_INTERRUPT_NEXT1_S)
#define GPIO_STATUS_INTERRUPT_NEXT1_V  0x00000001U
#define GPIO_STATUS_INTERRUPT_NEXT1_S  0

/** GPIO_PIN0_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN0_REG (DR_REG_GPIO_BASE + 0xc4)
/** GPIO_PIN0_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN0_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN0_SYNC2_BYPASS_M  (GPIO_PIN0_SYNC2_BYPASS_V << GPIO_PIN0_SYNC2_BYPASS_S)
#define GPIO_PIN0_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN0_SYNC2_BYPASS_S  0
/** GPIO_PIN0_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_M  (GPIO_PIN0_PAD_DRIVER_V << GPIO_PIN0_PAD_DRIVER_S)
#define GPIO_PIN0_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN0_PAD_DRIVER_S  2
/** GPIO_PIN0_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN0_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN0_SYNC1_BYPASS_M  (GPIO_PIN0_SYNC1_BYPASS_V << GPIO_PIN0_SYNC1_BYPASS_S)
#define GPIO_PIN0_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN0_SYNC1_BYPASS_S  3
/** GPIO_PIN0_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN0_INT_TYPE    0x00000007U
#define GPIO_PIN0_INT_TYPE_M  (GPIO_PIN0_INT_TYPE_V << GPIO_PIN0_INT_TYPE_S)
#define GPIO_PIN0_INT_TYPE_V  0x00000007U
#define GPIO_PIN0_INT_TYPE_S  7
/** GPIO_PIN0_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_M  (GPIO_PIN0_WAKEUP_ENABLE_V << GPIO_PIN0_WAKEUP_ENABLE_S)
#define GPIO_PIN0_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN0_WAKEUP_ENABLE_S  10
/** GPIO_PIN0_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN0_INT_ENA    0x0000001FU
#define GPIO_PIN0_INT_ENA_M  (GPIO_PIN0_INT_ENA_V << GPIO_PIN0_INT_ENA_S)
#define GPIO_PIN0_INT_ENA_V  0x0000001FU
#define GPIO_PIN0_INT_ENA_S  13

/** GPIO_PIN1_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN1_REG (DR_REG_GPIO_BASE + 0xc8)
/** GPIO_PIN1_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN1_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN1_SYNC2_BYPASS_M  (GPIO_PIN1_SYNC2_BYPASS_V << GPIO_PIN1_SYNC2_BYPASS_S)
#define GPIO_PIN1_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN1_SYNC2_BYPASS_S  0
/** GPIO_PIN1_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_M  (GPIO_PIN1_PAD_DRIVER_V << GPIO_PIN1_PAD_DRIVER_S)
#define GPIO_PIN1_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN1_PAD_DRIVER_S  2
/** GPIO_PIN1_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN1_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN1_SYNC1_BYPASS_M  (GPIO_PIN1_SYNC1_BYPASS_V << GPIO_PIN1_SYNC1_BYPASS_S)
#define GPIO_PIN1_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN1_SYNC1_BYPASS_S  3
/** GPIO_PIN1_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN1_INT_TYPE    0x00000007U
#define GPIO_PIN1_INT_TYPE_M  (GPIO_PIN1_INT_TYPE_V << GPIO_PIN1_INT_TYPE_S)
#define GPIO_PIN1_INT_TYPE_V  0x00000007U
#define GPIO_PIN1_INT_TYPE_S  7
/** GPIO_PIN1_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_M  (GPIO_PIN1_WAKEUP_ENABLE_V << GPIO_PIN1_WAKEUP_ENABLE_S)
#define GPIO_PIN1_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN1_WAKEUP_ENABLE_S  10
/** GPIO_PIN1_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN1_INT_ENA    0x0000001FU
#define GPIO_PIN1_INT_ENA_M  (GPIO_PIN1_INT_ENA_V << GPIO_PIN1_INT_ENA_S)
#define GPIO_PIN1_INT_ENA_V  0x0000001FU
#define GPIO_PIN1_INT_ENA_S  13

/** GPIO_PIN2_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN2_REG (DR_REG_GPIO_BASE + 0xcc)
/** GPIO_PIN2_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN2_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN2_SYNC2_BYPASS_M  (GPIO_PIN2_SYNC2_BYPASS_V << GPIO_PIN2_SYNC2_BYPASS_S)
#define GPIO_PIN2_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN2_SYNC2_BYPASS_S  0
/** GPIO_PIN2_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_M  (GPIO_PIN2_PAD_DRIVER_V << GPIO_PIN2_PAD_DRIVER_S)
#define GPIO_PIN2_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN2_PAD_DRIVER_S  2
/** GPIO_PIN2_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN2_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN2_SYNC1_BYPASS_M  (GPIO_PIN2_SYNC1_BYPASS_V << GPIO_PIN2_SYNC1_BYPASS_S)
#define GPIO_PIN2_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN2_SYNC1_BYPASS_S  3
/** GPIO_PIN2_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN2_INT_TYPE    0x00000007U
#define GPIO_PIN2_INT_TYPE_M  (GPIO_PIN2_INT_TYPE_V << GPIO_PIN2_INT_TYPE_S)
#define GPIO_PIN2_INT_TYPE_V  0x00000007U
#define GPIO_PIN2_INT_TYPE_S  7
/** GPIO_PIN2_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_M  (GPIO_PIN2_WAKEUP_ENABLE_V << GPIO_PIN2_WAKEUP_ENABLE_S)
#define GPIO_PIN2_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN2_WAKEUP_ENABLE_S  10
/** GPIO_PIN2_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN2_INT_ENA    0x0000001FU
#define GPIO_PIN2_INT_ENA_M  (GPIO_PIN2_INT_ENA_V << GPIO_PIN2_INT_ENA_S)
#define GPIO_PIN2_INT_ENA_V  0x0000001FU
#define GPIO_PIN2_INT_ENA_S  13

/** GPIO_PIN3_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN3_REG (DR_REG_GPIO_BASE + 0xd0)
/** GPIO_PIN3_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN3_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN3_SYNC2_BYPASS_M  (GPIO_PIN3_SYNC2_BYPASS_V << GPIO_PIN3_SYNC2_BYPASS_S)
#define GPIO_PIN3_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN3_SYNC2_BYPASS_S  0
/** GPIO_PIN3_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_M  (GPIO_PIN3_PAD_DRIVER_V << GPIO_PIN3_PAD_DRIVER_S)
#define GPIO_PIN3_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN3_PAD_DRIVER_S  2
/** GPIO_PIN3_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN3_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN3_SYNC1_BYPASS_M  (GPIO_PIN3_SYNC1_BYPASS_V << GPIO_PIN3_SYNC1_BYPASS_S)
#define GPIO_PIN3_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN3_SYNC1_BYPASS_S  3
/** GPIO_PIN3_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN3_INT_TYPE    0x00000007U
#define GPIO_PIN3_INT_TYPE_M  (GPIO_PIN3_INT_TYPE_V << GPIO_PIN3_INT_TYPE_S)
#define GPIO_PIN3_INT_TYPE_V  0x00000007U
#define GPIO_PIN3_INT_TYPE_S  7
/** GPIO_PIN3_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_M  (GPIO_PIN3_WAKEUP_ENABLE_V << GPIO_PIN3_WAKEUP_ENABLE_S)
#define GPIO_PIN3_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN3_WAKEUP_ENABLE_S  10
/** GPIO_PIN3_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN3_INT_ENA    0x0000001FU
#define GPIO_PIN3_INT_ENA_M  (GPIO_PIN3_INT_ENA_V << GPIO_PIN3_INT_ENA_S)
#define GPIO_PIN3_INT_ENA_V  0x0000001FU
#define GPIO_PIN3_INT_ENA_S  13

/** GPIO_PIN4_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN4_REG (DR_REG_GPIO_BASE + 0xd4)
/** GPIO_PIN4_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN4_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN4_SYNC2_BYPASS_M  (GPIO_PIN4_SYNC2_BYPASS_V << GPIO_PIN4_SYNC2_BYPASS_S)
#define GPIO_PIN4_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN4_SYNC2_BYPASS_S  0
/** GPIO_PIN4_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_M  (GPIO_PIN4_PAD_DRIVER_V << GPIO_PIN4_PAD_DRIVER_S)
#define GPIO_PIN4_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN4_PAD_DRIVER_S  2
/** GPIO_PIN4_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN4_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN4_SYNC1_BYPASS_M  (GPIO_PIN4_SYNC1_BYPASS_V << GPIO_PIN4_SYNC1_BYPASS_S)
#define GPIO_PIN4_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN4_SYNC1_BYPASS_S  3
/** GPIO_PIN4_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN4_INT_TYPE    0x00000007U
#define GPIO_PIN4_INT_TYPE_M  (GPIO_PIN4_INT_TYPE_V << GPIO_PIN4_INT_TYPE_S)
#define GPIO_PIN4_INT_TYPE_V  0x00000007U
#define GPIO_PIN4_INT_TYPE_S  7
/** GPIO_PIN4_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_M  (GPIO_PIN4_WAKEUP_ENABLE_V << GPIO_PIN4_WAKEUP_ENABLE_S)
#define GPIO_PIN4_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN4_WAKEUP_ENABLE_S  10
/** GPIO_PIN4_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN4_INT_ENA    0x0000001FU
#define GPIO_PIN4_INT_ENA_M  (GPIO_PIN4_INT_ENA_V << GPIO_PIN4_INT_ENA_S)
#define GPIO_PIN4_INT_ENA_V  0x0000001FU
#define GPIO_PIN4_INT_ENA_S  13

/** GPIO_PIN5_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN5_REG (DR_REG_GPIO_BASE + 0xd8)
/** GPIO_PIN5_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN5_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN5_SYNC2_BYPASS_M  (GPIO_PIN5_SYNC2_BYPASS_V << GPIO_PIN5_SYNC2_BYPASS_S)
#define GPIO_PIN5_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN5_SYNC2_BYPASS_S  0
/** GPIO_PIN5_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_M  (GPIO_PIN5_PAD_DRIVER_V << GPIO_PIN5_PAD_DRIVER_S)
#define GPIO_PIN5_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN5_PAD_DRIVER_S  2
/** GPIO_PIN5_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN5_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN5_SYNC1_BYPASS_M  (GPIO_PIN5_SYNC1_BYPASS_V << GPIO_PIN5_SYNC1_BYPASS_S)
#define GPIO_PIN5_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN5_SYNC1_BYPASS_S  3
/** GPIO_PIN5_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN5_INT_TYPE    0x00000007U
#define GPIO_PIN5_INT_TYPE_M  (GPIO_PIN5_INT_TYPE_V << GPIO_PIN5_INT_TYPE_S)
#define GPIO_PIN5_INT_TYPE_V  0x00000007U
#define GPIO_PIN5_INT_TYPE_S  7
/** GPIO_PIN5_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_M  (GPIO_PIN5_WAKEUP_ENABLE_V << GPIO_PIN5_WAKEUP_ENABLE_S)
#define GPIO_PIN5_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN5_WAKEUP_ENABLE_S  10
/** GPIO_PIN5_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN5_INT_ENA    0x0000001FU
#define GPIO_PIN5_INT_ENA_M  (GPIO_PIN5_INT_ENA_V << GPIO_PIN5_INT_ENA_S)
#define GPIO_PIN5_INT_ENA_V  0x0000001FU
#define GPIO_PIN5_INT_ENA_S  13

/** GPIO_PIN6_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN6_REG (DR_REG_GPIO_BASE + 0xdc)
/** GPIO_PIN6_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN6_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN6_SYNC2_BYPASS_M  (GPIO_PIN6_SYNC2_BYPASS_V << GPIO_PIN6_SYNC2_BYPASS_S)
#define GPIO_PIN6_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN6_SYNC2_BYPASS_S  0
/** GPIO_PIN6_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_M  (GPIO_PIN6_PAD_DRIVER_V << GPIO_PIN6_PAD_DRIVER_S)
#define GPIO_PIN6_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN6_PAD_DRIVER_S  2
/** GPIO_PIN6_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN6_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN6_SYNC1_BYPASS_M  (GPIO_PIN6_SYNC1_BYPASS_V << GPIO_PIN6_SYNC1_BYPASS_S)
#define GPIO_PIN6_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN6_SYNC1_BYPASS_S  3
/** GPIO_PIN6_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN6_INT_TYPE    0x00000007U
#define GPIO_PIN6_INT_TYPE_M  (GPIO_PIN6_INT_TYPE_V << GPIO_PIN6_INT_TYPE_S)
#define GPIO_PIN6_INT_TYPE_V  0x00000007U
#define GPIO_PIN6_INT_TYPE_S  7
/** GPIO_PIN6_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_M  (GPIO_PIN6_WAKEUP_ENABLE_V << GPIO_PIN6_WAKEUP_ENABLE_S)
#define GPIO_PIN6_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN6_WAKEUP_ENABLE_S  10
/** GPIO_PIN6_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN6_INT_ENA    0x0000001FU
#define GPIO_PIN6_INT_ENA_M  (GPIO_PIN6_INT_ENA_V << GPIO_PIN6_INT_ENA_S)
#define GPIO_PIN6_INT_ENA_V  0x0000001FU
#define GPIO_PIN6_INT_ENA_S  13

/** GPIO_PIN7_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN7_REG (DR_REG_GPIO_BASE + 0xe0)
/** GPIO_PIN7_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN7_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN7_SYNC2_BYPASS_M  (GPIO_PIN7_SYNC2_BYPASS_V << GPIO_PIN7_SYNC2_BYPASS_S)
#define GPIO_PIN7_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN7_SYNC2_BYPASS_S  0
/** GPIO_PIN7_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN7_PAD_DRIVER    (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_M  (GPIO_PIN7_PAD_DRIVER_V << GPIO_PIN7_PAD_DRIVER_S)
#define GPIO_PIN7_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN7_PAD_DRIVER_S  2
/** GPIO_PIN7_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN7_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN7_SYNC1_BYPASS_M  (GPIO_PIN7_SYNC1_BYPASS_V << GPIO_PIN7_SYNC1_BYPASS_S)
#define GPIO_PIN7_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN7_SYNC1_BYPASS_S  3
/** GPIO_PIN7_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN7_INT_TYPE    0x00000007U
#define GPIO_PIN7_INT_TYPE_M  (GPIO_PIN7_INT_TYPE_V << GPIO_PIN7_INT_TYPE_S)
#define GPIO_PIN7_INT_TYPE_V  0x00000007U
#define GPIO_PIN7_INT_TYPE_S  7
/** GPIO_PIN7_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN7_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_M  (GPIO_PIN7_WAKEUP_ENABLE_V << GPIO_PIN7_WAKEUP_ENABLE_S)
#define GPIO_PIN7_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN7_WAKEUP_ENABLE_S  10
/** GPIO_PIN7_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN7_INT_ENA    0x0000001FU
#define GPIO_PIN7_INT_ENA_M  (GPIO_PIN7_INT_ENA_V << GPIO_PIN7_INT_ENA_S)
#define GPIO_PIN7_INT_ENA_V  0x0000001FU
#define GPIO_PIN7_INT_ENA_S  13

/** GPIO_PIN8_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN8_REG (DR_REG_GPIO_BASE + 0xe4)
/** GPIO_PIN8_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN8_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN8_SYNC2_BYPASS_M  (GPIO_PIN8_SYNC2_BYPASS_V << GPIO_PIN8_SYNC2_BYPASS_S)
#define GPIO_PIN8_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN8_SYNC2_BYPASS_S  0
/** GPIO_PIN8_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN8_PAD_DRIVER    (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_M  (GPIO_PIN8_PAD_DRIVER_V << GPIO_PIN8_PAD_DRIVER_S)
#define GPIO_PIN8_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN8_PAD_DRIVER_S  2
/** GPIO_PIN8_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN8_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN8_SYNC1_BYPASS_M  (GPIO_PIN8_SYNC1_BYPASS_V << GPIO_PIN8_SYNC1_BYPASS_S)
#define GPIO_PIN8_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN8_SYNC1_BYPASS_S  3
/** GPIO_PIN8_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN8_INT_TYPE    0x00000007U
#define GPIO_PIN8_INT_TYPE_M  (GPIO_PIN8_INT_TYPE_V << GPIO_PIN8_INT_TYPE_S)
#define GPIO_PIN8_INT_TYPE_V  0x00000007U
#define GPIO_PIN8_INT_TYPE_S  7
/** GPIO_PIN8_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN8_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_M  (GPIO_PIN8_WAKEUP_ENABLE_V << GPIO_PIN8_WAKEUP_ENABLE_S)
#define GPIO_PIN8_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN8_WAKEUP_ENABLE_S  10
/** GPIO_PIN8_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN8_INT_ENA    0x0000001FU
#define GPIO_PIN8_INT_ENA_M  (GPIO_PIN8_INT_ENA_V << GPIO_PIN8_INT_ENA_S)
#define GPIO_PIN8_INT_ENA_V  0x0000001FU
#define GPIO_PIN8_INT_ENA_S  13

/** GPIO_PIN9_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN9_REG (DR_REG_GPIO_BASE + 0xe8)
/** GPIO_PIN9_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN9_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN9_SYNC2_BYPASS_M  (GPIO_PIN9_SYNC2_BYPASS_V << GPIO_PIN9_SYNC2_BYPASS_S)
#define GPIO_PIN9_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN9_SYNC2_BYPASS_S  0
/** GPIO_PIN9_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN9_PAD_DRIVER    (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_M  (GPIO_PIN9_PAD_DRIVER_V << GPIO_PIN9_PAD_DRIVER_S)
#define GPIO_PIN9_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN9_PAD_DRIVER_S  2
/** GPIO_PIN9_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN9_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN9_SYNC1_BYPASS_M  (GPIO_PIN9_SYNC1_BYPASS_V << GPIO_PIN9_SYNC1_BYPASS_S)
#define GPIO_PIN9_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN9_SYNC1_BYPASS_S  3
/** GPIO_PIN9_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN9_INT_TYPE    0x00000007U
#define GPIO_PIN9_INT_TYPE_M  (GPIO_PIN9_INT_TYPE_V << GPIO_PIN9_INT_TYPE_S)
#define GPIO_PIN9_INT_TYPE_V  0x00000007U
#define GPIO_PIN9_INT_TYPE_S  7
/** GPIO_PIN9_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN9_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_M  (GPIO_PIN9_WAKEUP_ENABLE_V << GPIO_PIN9_WAKEUP_ENABLE_S)
#define GPIO_PIN9_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN9_WAKEUP_ENABLE_S  10
/** GPIO_PIN9_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN9_INT_ENA    0x0000001FU
#define GPIO_PIN9_INT_ENA_M  (GPIO_PIN9_INT_ENA_V << GPIO_PIN9_INT_ENA_S)
#define GPIO_PIN9_INT_ENA_V  0x0000001FU
#define GPIO_PIN9_INT_ENA_S  13

/** GPIO_PIN10_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN10_REG (DR_REG_GPIO_BASE + 0xec)
/** GPIO_PIN10_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN10_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN10_SYNC2_BYPASS_M  (GPIO_PIN10_SYNC2_BYPASS_V << GPIO_PIN10_SYNC2_BYPASS_S)
#define GPIO_PIN10_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN10_SYNC2_BYPASS_S  0
/** GPIO_PIN10_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN10_PAD_DRIVER    (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_M  (GPIO_PIN10_PAD_DRIVER_V << GPIO_PIN10_PAD_DRIVER_S)
#define GPIO_PIN10_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN10_PAD_DRIVER_S  2
/** GPIO_PIN10_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN10_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN10_SYNC1_BYPASS_M  (GPIO_PIN10_SYNC1_BYPASS_V << GPIO_PIN10_SYNC1_BYPASS_S)
#define GPIO_PIN10_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN10_SYNC1_BYPASS_S  3
/** GPIO_PIN10_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN10_INT_TYPE    0x00000007U
#define GPIO_PIN10_INT_TYPE_M  (GPIO_PIN10_INT_TYPE_V << GPIO_PIN10_INT_TYPE_S)
#define GPIO_PIN10_INT_TYPE_V  0x00000007U
#define GPIO_PIN10_INT_TYPE_S  7
/** GPIO_PIN10_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN10_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_M  (GPIO_PIN10_WAKEUP_ENABLE_V << GPIO_PIN10_WAKEUP_ENABLE_S)
#define GPIO_PIN10_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN10_WAKEUP_ENABLE_S  10
/** GPIO_PIN10_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN10_INT_ENA    0x0000001FU
#define GPIO_PIN10_INT_ENA_M  (GPIO_PIN10_INT_ENA_V << GPIO_PIN10_INT_ENA_S)
#define GPIO_PIN10_INT_ENA_V  0x0000001FU
#define GPIO_PIN10_INT_ENA_S  13

/** GPIO_PIN11_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN11_REG (DR_REG_GPIO_BASE + 0xf0)
/** GPIO_PIN11_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN11_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN11_SYNC2_BYPASS_M  (GPIO_PIN11_SYNC2_BYPASS_V << GPIO_PIN11_SYNC2_BYPASS_S)
#define GPIO_PIN11_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN11_SYNC2_BYPASS_S  0
/** GPIO_PIN11_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN11_PAD_DRIVER    (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_M  (GPIO_PIN11_PAD_DRIVER_V << GPIO_PIN11_PAD_DRIVER_S)
#define GPIO_PIN11_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN11_PAD_DRIVER_S  2
/** GPIO_PIN11_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN11_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN11_SYNC1_BYPASS_M  (GPIO_PIN11_SYNC1_BYPASS_V << GPIO_PIN11_SYNC1_BYPASS_S)
#define GPIO_PIN11_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN11_SYNC1_BYPASS_S  3
/** GPIO_PIN11_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN11_INT_TYPE    0x00000007U
#define GPIO_PIN11_INT_TYPE_M  (GPIO_PIN11_INT_TYPE_V << GPIO_PIN11_INT_TYPE_S)
#define GPIO_PIN11_INT_TYPE_V  0x00000007U
#define GPIO_PIN11_INT_TYPE_S  7
/** GPIO_PIN11_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN11_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_M  (GPIO_PIN11_WAKEUP_ENABLE_V << GPIO_PIN11_WAKEUP_ENABLE_S)
#define GPIO_PIN11_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN11_WAKEUP_ENABLE_S  10
/** GPIO_PIN11_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN11_INT_ENA    0x0000001FU
#define GPIO_PIN11_INT_ENA_M  (GPIO_PIN11_INT_ENA_V << GPIO_PIN11_INT_ENA_S)
#define GPIO_PIN11_INT_ENA_V  0x0000001FU
#define GPIO_PIN11_INT_ENA_S  13

/** GPIO_PIN12_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN12_REG (DR_REG_GPIO_BASE + 0xf4)
/** GPIO_PIN12_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN12_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN12_SYNC2_BYPASS_M  (GPIO_PIN12_SYNC2_BYPASS_V << GPIO_PIN12_SYNC2_BYPASS_S)
#define GPIO_PIN12_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN12_SYNC2_BYPASS_S  0
/** GPIO_PIN12_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN12_PAD_DRIVER    (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_M  (GPIO_PIN12_PAD_DRIVER_V << GPIO_PIN12_PAD_DRIVER_S)
#define GPIO_PIN12_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN12_PAD_DRIVER_S  2
/** GPIO_PIN12_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN12_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN12_SYNC1_BYPASS_M  (GPIO_PIN12_SYNC1_BYPASS_V << GPIO_PIN12_SYNC1_BYPASS_S)
#define GPIO_PIN12_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN12_SYNC1_BYPASS_S  3
/** GPIO_PIN12_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN12_INT_TYPE    0x00000007U
#define GPIO_PIN12_INT_TYPE_M  (GPIO_PIN12_INT_TYPE_V << GPIO_PIN12_INT_TYPE_S)
#define GPIO_PIN12_INT_TYPE_V  0x00000007U
#define GPIO_PIN12_INT_TYPE_S  7
/** GPIO_PIN12_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN12_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_M  (GPIO_PIN12_WAKEUP_ENABLE_V << GPIO_PIN12_WAKEUP_ENABLE_S)
#define GPIO_PIN12_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN12_WAKEUP_ENABLE_S  10
/** GPIO_PIN12_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN12_INT_ENA    0x0000001FU
#define GPIO_PIN12_INT_ENA_M  (GPIO_PIN12_INT_ENA_V << GPIO_PIN12_INT_ENA_S)
#define GPIO_PIN12_INT_ENA_V  0x0000001FU
#define GPIO_PIN12_INT_ENA_S  13

/** GPIO_PIN13_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN13_REG (DR_REG_GPIO_BASE + 0xf8)
/** GPIO_PIN13_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN13_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN13_SYNC2_BYPASS_M  (GPIO_PIN13_SYNC2_BYPASS_V << GPIO_PIN13_SYNC2_BYPASS_S)
#define GPIO_PIN13_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN13_SYNC2_BYPASS_S  0
/** GPIO_PIN13_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN13_PAD_DRIVER    (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_M  (GPIO_PIN13_PAD_DRIVER_V << GPIO_PIN13_PAD_DRIVER_S)
#define GPIO_PIN13_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN13_PAD_DRIVER_S  2
/** GPIO_PIN13_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN13_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN13_SYNC1_BYPASS_M  (GPIO_PIN13_SYNC1_BYPASS_V << GPIO_PIN13_SYNC1_BYPASS_S)
#define GPIO_PIN13_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN13_SYNC1_BYPASS_S  3
/** GPIO_PIN13_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN13_INT_TYPE    0x00000007U
#define GPIO_PIN13_INT_TYPE_M  (GPIO_PIN13_INT_TYPE_V << GPIO_PIN13_INT_TYPE_S)
#define GPIO_PIN13_INT_TYPE_V  0x00000007U
#define GPIO_PIN13_INT_TYPE_S  7
/** GPIO_PIN13_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN13_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_M  (GPIO_PIN13_WAKEUP_ENABLE_V << GPIO_PIN13_WAKEUP_ENABLE_S)
#define GPIO_PIN13_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN13_WAKEUP_ENABLE_S  10
/** GPIO_PIN13_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN13_INT_ENA    0x0000001FU
#define GPIO_PIN13_INT_ENA_M  (GPIO_PIN13_INT_ENA_V << GPIO_PIN13_INT_ENA_S)
#define GPIO_PIN13_INT_ENA_V  0x0000001FU
#define GPIO_PIN13_INT_ENA_S  13

/** GPIO_PIN14_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN14_REG (DR_REG_GPIO_BASE + 0xfc)
/** GPIO_PIN14_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN14_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN14_SYNC2_BYPASS_M  (GPIO_PIN14_SYNC2_BYPASS_V << GPIO_PIN14_SYNC2_BYPASS_S)
#define GPIO_PIN14_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN14_SYNC2_BYPASS_S  0
/** GPIO_PIN14_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN14_PAD_DRIVER    (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_M  (GPIO_PIN14_PAD_DRIVER_V << GPIO_PIN14_PAD_DRIVER_S)
#define GPIO_PIN14_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN14_PAD_DRIVER_S  2
/** GPIO_PIN14_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN14_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN14_SYNC1_BYPASS_M  (GPIO_PIN14_SYNC1_BYPASS_V << GPIO_PIN14_SYNC1_BYPASS_S)
#define GPIO_PIN14_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN14_SYNC1_BYPASS_S  3
/** GPIO_PIN14_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN14_INT_TYPE    0x00000007U
#define GPIO_PIN14_INT_TYPE_M  (GPIO_PIN14_INT_TYPE_V << GPIO_PIN14_INT_TYPE_S)
#define GPIO_PIN14_INT_TYPE_V  0x00000007U
#define GPIO_PIN14_INT_TYPE_S  7
/** GPIO_PIN14_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN14_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_M  (GPIO_PIN14_WAKEUP_ENABLE_V << GPIO_PIN14_WAKEUP_ENABLE_S)
#define GPIO_PIN14_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN14_WAKEUP_ENABLE_S  10
/** GPIO_PIN14_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN14_INT_ENA    0x0000001FU
#define GPIO_PIN14_INT_ENA_M  (GPIO_PIN14_INT_ENA_V << GPIO_PIN14_INT_ENA_S)
#define GPIO_PIN14_INT_ENA_V  0x0000001FU
#define GPIO_PIN14_INT_ENA_S  13

/** GPIO_PIN15_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN15_REG (DR_REG_GPIO_BASE + 0x100)
/** GPIO_PIN15_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN15_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN15_SYNC2_BYPASS_M  (GPIO_PIN15_SYNC2_BYPASS_V << GPIO_PIN15_SYNC2_BYPASS_S)
#define GPIO_PIN15_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN15_SYNC2_BYPASS_S  0
/** GPIO_PIN15_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN15_PAD_DRIVER    (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_M  (GPIO_PIN15_PAD_DRIVER_V << GPIO_PIN15_PAD_DRIVER_S)
#define GPIO_PIN15_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN15_PAD_DRIVER_S  2
/** GPIO_PIN15_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN15_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN15_SYNC1_BYPASS_M  (GPIO_PIN15_SYNC1_BYPASS_V << GPIO_PIN15_SYNC1_BYPASS_S)
#define GPIO_PIN15_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN15_SYNC1_BYPASS_S  3
/** GPIO_PIN15_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN15_INT_TYPE    0x00000007U
#define GPIO_PIN15_INT_TYPE_M  (GPIO_PIN15_INT_TYPE_V << GPIO_PIN15_INT_TYPE_S)
#define GPIO_PIN15_INT_TYPE_V  0x00000007U
#define GPIO_PIN15_INT_TYPE_S  7
/** GPIO_PIN15_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN15_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_M  (GPIO_PIN15_WAKEUP_ENABLE_V << GPIO_PIN15_WAKEUP_ENABLE_S)
#define GPIO_PIN15_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN15_WAKEUP_ENABLE_S  10
/** GPIO_PIN15_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN15_INT_ENA    0x0000001FU
#define GPIO_PIN15_INT_ENA_M  (GPIO_PIN15_INT_ENA_V << GPIO_PIN15_INT_ENA_S)
#define GPIO_PIN15_INT_ENA_V  0x0000001FU
#define GPIO_PIN15_INT_ENA_S  13

/** GPIO_PIN16_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN16_REG (DR_REG_GPIO_BASE + 0x104)
/** GPIO_PIN16_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN16_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN16_SYNC2_BYPASS_M  (GPIO_PIN16_SYNC2_BYPASS_V << GPIO_PIN16_SYNC2_BYPASS_S)
#define GPIO_PIN16_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN16_SYNC2_BYPASS_S  0
/** GPIO_PIN16_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN16_PAD_DRIVER    (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_M  (GPIO_PIN16_PAD_DRIVER_V << GPIO_PIN16_PAD_DRIVER_S)
#define GPIO_PIN16_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN16_PAD_DRIVER_S  2
/** GPIO_PIN16_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN16_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN16_SYNC1_BYPASS_M  (GPIO_PIN16_SYNC1_BYPASS_V << GPIO_PIN16_SYNC1_BYPASS_S)
#define GPIO_PIN16_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN16_SYNC1_BYPASS_S  3
/** GPIO_PIN16_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN16_INT_TYPE    0x00000007U
#define GPIO_PIN16_INT_TYPE_M  (GPIO_PIN16_INT_TYPE_V << GPIO_PIN16_INT_TYPE_S)
#define GPIO_PIN16_INT_TYPE_V  0x00000007U
#define GPIO_PIN16_INT_TYPE_S  7
/** GPIO_PIN16_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN16_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_M  (GPIO_PIN16_WAKEUP_ENABLE_V << GPIO_PIN16_WAKEUP_ENABLE_S)
#define GPIO_PIN16_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN16_WAKEUP_ENABLE_S  10
/** GPIO_PIN16_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN16_INT_ENA    0x0000001FU
#define GPIO_PIN16_INT_ENA_M  (GPIO_PIN16_INT_ENA_V << GPIO_PIN16_INT_ENA_S)
#define GPIO_PIN16_INT_ENA_V  0x0000001FU
#define GPIO_PIN16_INT_ENA_S  13

/** GPIO_PIN17_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN17_REG (DR_REG_GPIO_BASE + 0x108)
/** GPIO_PIN17_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN17_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN17_SYNC2_BYPASS_M  (GPIO_PIN17_SYNC2_BYPASS_V << GPIO_PIN17_SYNC2_BYPASS_S)
#define GPIO_PIN17_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN17_SYNC2_BYPASS_S  0
/** GPIO_PIN17_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN17_PAD_DRIVER    (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_M  (GPIO_PIN17_PAD_DRIVER_V << GPIO_PIN17_PAD_DRIVER_S)
#define GPIO_PIN17_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN17_PAD_DRIVER_S  2
/** GPIO_PIN17_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN17_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN17_SYNC1_BYPASS_M  (GPIO_PIN17_SYNC1_BYPASS_V << GPIO_PIN17_SYNC1_BYPASS_S)
#define GPIO_PIN17_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN17_SYNC1_BYPASS_S  3
/** GPIO_PIN17_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN17_INT_TYPE    0x00000007U
#define GPIO_PIN17_INT_TYPE_M  (GPIO_PIN17_INT_TYPE_V << GPIO_PIN17_INT_TYPE_S)
#define GPIO_PIN17_INT_TYPE_V  0x00000007U
#define GPIO_PIN17_INT_TYPE_S  7
/** GPIO_PIN17_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN17_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_M  (GPIO_PIN17_WAKEUP_ENABLE_V << GPIO_PIN17_WAKEUP_ENABLE_S)
#define GPIO_PIN17_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN17_WAKEUP_ENABLE_S  10
/** GPIO_PIN17_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN17_INT_ENA    0x0000001FU
#define GPIO_PIN17_INT_ENA_M  (GPIO_PIN17_INT_ENA_V << GPIO_PIN17_INT_ENA_S)
#define GPIO_PIN17_INT_ENA_V  0x0000001FU
#define GPIO_PIN17_INT_ENA_S  13

/** GPIO_PIN18_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN18_REG (DR_REG_GPIO_BASE + 0x10c)
/** GPIO_PIN18_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN18_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN18_SYNC2_BYPASS_M  (GPIO_PIN18_SYNC2_BYPASS_V << GPIO_PIN18_SYNC2_BYPASS_S)
#define GPIO_PIN18_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN18_SYNC2_BYPASS_S  0
/** GPIO_PIN18_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN18_PAD_DRIVER    (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_M  (GPIO_PIN18_PAD_DRIVER_V << GPIO_PIN18_PAD_DRIVER_S)
#define GPIO_PIN18_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN18_PAD_DRIVER_S  2
/** GPIO_PIN18_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN18_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN18_SYNC1_BYPASS_M  (GPIO_PIN18_SYNC1_BYPASS_V << GPIO_PIN18_SYNC1_BYPASS_S)
#define GPIO_PIN18_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN18_SYNC1_BYPASS_S  3
/** GPIO_PIN18_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN18_INT_TYPE    0x00000007U
#define GPIO_PIN18_INT_TYPE_M  (GPIO_PIN18_INT_TYPE_V << GPIO_PIN18_INT_TYPE_S)
#define GPIO_PIN18_INT_TYPE_V  0x00000007U
#define GPIO_PIN18_INT_TYPE_S  7
/** GPIO_PIN18_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN18_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_M  (GPIO_PIN18_WAKEUP_ENABLE_V << GPIO_PIN18_WAKEUP_ENABLE_S)
#define GPIO_PIN18_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN18_WAKEUP_ENABLE_S  10
/** GPIO_PIN18_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN18_INT_ENA    0x0000001FU
#define GPIO_PIN18_INT_ENA_M  (GPIO_PIN18_INT_ENA_V << GPIO_PIN18_INT_ENA_S)
#define GPIO_PIN18_INT_ENA_V  0x0000001FU
#define GPIO_PIN18_INT_ENA_S  13

/** GPIO_PIN19_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN19_REG (DR_REG_GPIO_BASE + 0x110)
/** GPIO_PIN19_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN19_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN19_SYNC2_BYPASS_M  (GPIO_PIN19_SYNC2_BYPASS_V << GPIO_PIN19_SYNC2_BYPASS_S)
#define GPIO_PIN19_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN19_SYNC2_BYPASS_S  0
/** GPIO_PIN19_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN19_PAD_DRIVER    (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_M  (GPIO_PIN19_PAD_DRIVER_V << GPIO_PIN19_PAD_DRIVER_S)
#define GPIO_PIN19_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN19_PAD_DRIVER_S  2
/** GPIO_PIN19_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN19_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN19_SYNC1_BYPASS_M  (GPIO_PIN19_SYNC1_BYPASS_V << GPIO_PIN19_SYNC1_BYPASS_S)
#define GPIO_PIN19_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN19_SYNC1_BYPASS_S  3
/** GPIO_PIN19_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN19_INT_TYPE    0x00000007U
#define GPIO_PIN19_INT_TYPE_M  (GPIO_PIN19_INT_TYPE_V << GPIO_PIN19_INT_TYPE_S)
#define GPIO_PIN19_INT_TYPE_V  0x00000007U
#define GPIO_PIN19_INT_TYPE_S  7
/** GPIO_PIN19_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN19_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_M  (GPIO_PIN19_WAKEUP_ENABLE_V << GPIO_PIN19_WAKEUP_ENABLE_S)
#define GPIO_PIN19_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN19_WAKEUP_ENABLE_S  10
/** GPIO_PIN19_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN19_INT_ENA    0x0000001FU
#define GPIO_PIN19_INT_ENA_M  (GPIO_PIN19_INT_ENA_V << GPIO_PIN19_INT_ENA_S)
#define GPIO_PIN19_INT_ENA_V  0x0000001FU
#define GPIO_PIN19_INT_ENA_S  13

/** GPIO_PIN20_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN20_REG (DR_REG_GPIO_BASE + 0x114)
/** GPIO_PIN20_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN20_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN20_SYNC2_BYPASS_M  (GPIO_PIN20_SYNC2_BYPASS_V << GPIO_PIN20_SYNC2_BYPASS_S)
#define GPIO_PIN20_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN20_SYNC2_BYPASS_S  0
/** GPIO_PIN20_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN20_PAD_DRIVER    (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_M  (GPIO_PIN20_PAD_DRIVER_V << GPIO_PIN20_PAD_DRIVER_S)
#define GPIO_PIN20_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN20_PAD_DRIVER_S  2
/** GPIO_PIN20_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN20_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN20_SYNC1_BYPASS_M  (GPIO_PIN20_SYNC1_BYPASS_V << GPIO_PIN20_SYNC1_BYPASS_S)
#define GPIO_PIN20_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN20_SYNC1_BYPASS_S  3
/** GPIO_PIN20_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN20_INT_TYPE    0x00000007U
#define GPIO_PIN20_INT_TYPE_M  (GPIO_PIN20_INT_TYPE_V << GPIO_PIN20_INT_TYPE_S)
#define GPIO_PIN20_INT_TYPE_V  0x00000007U
#define GPIO_PIN20_INT_TYPE_S  7
/** GPIO_PIN20_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN20_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_M  (GPIO_PIN20_WAKEUP_ENABLE_V << GPIO_PIN20_WAKEUP_ENABLE_S)
#define GPIO_PIN20_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN20_WAKEUP_ENABLE_S  10
/** GPIO_PIN20_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN20_INT_ENA    0x0000001FU
#define GPIO_PIN20_INT_ENA_M  (GPIO_PIN20_INT_ENA_V << GPIO_PIN20_INT_ENA_S)
#define GPIO_PIN20_INT_ENA_V  0x0000001FU
#define GPIO_PIN20_INT_ENA_S  13

/** GPIO_PIN21_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN21_REG (DR_REG_GPIO_BASE + 0x118)
/** GPIO_PIN21_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN21_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN21_SYNC2_BYPASS_M  (GPIO_PIN21_SYNC2_BYPASS_V << GPIO_PIN21_SYNC2_BYPASS_S)
#define GPIO_PIN21_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN21_SYNC2_BYPASS_S  0
/** GPIO_PIN21_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN21_PAD_DRIVER    (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_M  (GPIO_PIN21_PAD_DRIVER_V << GPIO_PIN21_PAD_DRIVER_S)
#define GPIO_PIN21_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN21_PAD_DRIVER_S  2
/** GPIO_PIN21_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN21_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN21_SYNC1_BYPASS_M  (GPIO_PIN21_SYNC1_BYPASS_V << GPIO_PIN21_SYNC1_BYPASS_S)
#define GPIO_PIN21_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN21_SYNC1_BYPASS_S  3
/** GPIO_PIN21_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN21_INT_TYPE    0x00000007U
#define GPIO_PIN21_INT_TYPE_M  (GPIO_PIN21_INT_TYPE_V << GPIO_PIN21_INT_TYPE_S)
#define GPIO_PIN21_INT_TYPE_V  0x00000007U
#define GPIO_PIN21_INT_TYPE_S  7
/** GPIO_PIN21_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN21_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_M  (GPIO_PIN21_WAKEUP_ENABLE_V << GPIO_PIN21_WAKEUP_ENABLE_S)
#define GPIO_PIN21_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN21_WAKEUP_ENABLE_S  10
/** GPIO_PIN21_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN21_INT_ENA    0x0000001FU
#define GPIO_PIN21_INT_ENA_M  (GPIO_PIN21_INT_ENA_V << GPIO_PIN21_INT_ENA_S)
#define GPIO_PIN21_INT_ENA_V  0x0000001FU
#define GPIO_PIN21_INT_ENA_S  13

/** GPIO_PIN22_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN22_REG (DR_REG_GPIO_BASE + 0x11c)
/** GPIO_PIN22_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN22_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN22_SYNC2_BYPASS_M  (GPIO_PIN22_SYNC2_BYPASS_V << GPIO_PIN22_SYNC2_BYPASS_S)
#define GPIO_PIN22_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN22_SYNC2_BYPASS_S  0
/** GPIO_PIN22_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN22_PAD_DRIVER    (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_M  (GPIO_PIN22_PAD_DRIVER_V << GPIO_PIN22_PAD_DRIVER_S)
#define GPIO_PIN22_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN22_PAD_DRIVER_S  2
/** GPIO_PIN22_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN22_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN22_SYNC1_BYPASS_M  (GPIO_PIN22_SYNC1_BYPASS_V << GPIO_PIN22_SYNC1_BYPASS_S)
#define GPIO_PIN22_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN22_SYNC1_BYPASS_S  3
/** GPIO_PIN22_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN22_INT_TYPE    0x00000007U
#define GPIO_PIN22_INT_TYPE_M  (GPIO_PIN22_INT_TYPE_V << GPIO_PIN22_INT_TYPE_S)
#define GPIO_PIN22_INT_TYPE_V  0x00000007U
#define GPIO_PIN22_INT_TYPE_S  7
/** GPIO_PIN22_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN22_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_M  (GPIO_PIN22_WAKEUP_ENABLE_V << GPIO_PIN22_WAKEUP_ENABLE_S)
#define GPIO_PIN22_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN22_WAKEUP_ENABLE_S  10
/** GPIO_PIN22_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN22_INT_ENA    0x0000001FU
#define GPIO_PIN22_INT_ENA_M  (GPIO_PIN22_INT_ENA_V << GPIO_PIN22_INT_ENA_S)
#define GPIO_PIN22_INT_ENA_V  0x0000001FU
#define GPIO_PIN22_INT_ENA_S  13

/** GPIO_PIN23_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN23_REG (DR_REG_GPIO_BASE + 0x120)
/** GPIO_PIN23_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN23_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN23_SYNC2_BYPASS_M  (GPIO_PIN23_SYNC2_BYPASS_V << GPIO_PIN23_SYNC2_BYPASS_S)
#define GPIO_PIN23_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN23_SYNC2_BYPASS_S  0
/** GPIO_PIN23_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN23_PAD_DRIVER    (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_M  (GPIO_PIN23_PAD_DRIVER_V << GPIO_PIN23_PAD_DRIVER_S)
#define GPIO_PIN23_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN23_PAD_DRIVER_S  2
/** GPIO_PIN23_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN23_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN23_SYNC1_BYPASS_M  (GPIO_PIN23_SYNC1_BYPASS_V << GPIO_PIN23_SYNC1_BYPASS_S)
#define GPIO_PIN23_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN23_SYNC1_BYPASS_S  3
/** GPIO_PIN23_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN23_INT_TYPE    0x00000007U
#define GPIO_PIN23_INT_TYPE_M  (GPIO_PIN23_INT_TYPE_V << GPIO_PIN23_INT_TYPE_S)
#define GPIO_PIN23_INT_TYPE_V  0x00000007U
#define GPIO_PIN23_INT_TYPE_S  7
/** GPIO_PIN23_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN23_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_M  (GPIO_PIN23_WAKEUP_ENABLE_V << GPIO_PIN23_WAKEUP_ENABLE_S)
#define GPIO_PIN23_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN23_WAKEUP_ENABLE_S  10
/** GPIO_PIN23_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN23_INT_ENA    0x0000001FU
#define GPIO_PIN23_INT_ENA_M  (GPIO_PIN23_INT_ENA_V << GPIO_PIN23_INT_ENA_S)
#define GPIO_PIN23_INT_ENA_V  0x0000001FU
#define GPIO_PIN23_INT_ENA_S  13

/** GPIO_PIN24_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN24_REG (DR_REG_GPIO_BASE + 0x124)
/** GPIO_PIN24_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN24_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN24_SYNC2_BYPASS_M  (GPIO_PIN24_SYNC2_BYPASS_V << GPIO_PIN24_SYNC2_BYPASS_S)
#define GPIO_PIN24_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN24_SYNC2_BYPASS_S  0
/** GPIO_PIN24_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN24_PAD_DRIVER    (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_M  (GPIO_PIN24_PAD_DRIVER_V << GPIO_PIN24_PAD_DRIVER_S)
#define GPIO_PIN24_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN24_PAD_DRIVER_S  2
/** GPIO_PIN24_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN24_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN24_SYNC1_BYPASS_M  (GPIO_PIN24_SYNC1_BYPASS_V << GPIO_PIN24_SYNC1_BYPASS_S)
#define GPIO_PIN24_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN24_SYNC1_BYPASS_S  3
/** GPIO_PIN24_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN24_INT_TYPE    0x00000007U
#define GPIO_PIN24_INT_TYPE_M  (GPIO_PIN24_INT_TYPE_V << GPIO_PIN24_INT_TYPE_S)
#define GPIO_PIN24_INT_TYPE_V  0x00000007U
#define GPIO_PIN24_INT_TYPE_S  7
/** GPIO_PIN24_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN24_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_M  (GPIO_PIN24_WAKEUP_ENABLE_V << GPIO_PIN24_WAKEUP_ENABLE_S)
#define GPIO_PIN24_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN24_WAKEUP_ENABLE_S  10
/** GPIO_PIN24_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN24_INT_ENA    0x0000001FU
#define GPIO_PIN24_INT_ENA_M  (GPIO_PIN24_INT_ENA_V << GPIO_PIN24_INT_ENA_S)
#define GPIO_PIN24_INT_ENA_V  0x0000001FU
#define GPIO_PIN24_INT_ENA_S  13

/** GPIO_PIN25_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN25_REG (DR_REG_GPIO_BASE + 0x128)
/** GPIO_PIN25_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN25_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN25_SYNC2_BYPASS_M  (GPIO_PIN25_SYNC2_BYPASS_V << GPIO_PIN25_SYNC2_BYPASS_S)
#define GPIO_PIN25_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN25_SYNC2_BYPASS_S  0
/** GPIO_PIN25_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN25_PAD_DRIVER    (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_M  (GPIO_PIN25_PAD_DRIVER_V << GPIO_PIN25_PAD_DRIVER_S)
#define GPIO_PIN25_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN25_PAD_DRIVER_S  2
/** GPIO_PIN25_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN25_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN25_SYNC1_BYPASS_M  (GPIO_PIN25_SYNC1_BYPASS_V << GPIO_PIN25_SYNC1_BYPASS_S)
#define GPIO_PIN25_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN25_SYNC1_BYPASS_S  3
/** GPIO_PIN25_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN25_INT_TYPE    0x00000007U
#define GPIO_PIN25_INT_TYPE_M  (GPIO_PIN25_INT_TYPE_V << GPIO_PIN25_INT_TYPE_S)
#define GPIO_PIN25_INT_TYPE_V  0x00000007U
#define GPIO_PIN25_INT_TYPE_S  7
/** GPIO_PIN25_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN25_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_M  (GPIO_PIN25_WAKEUP_ENABLE_V << GPIO_PIN25_WAKEUP_ENABLE_S)
#define GPIO_PIN25_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN25_WAKEUP_ENABLE_S  10
/** GPIO_PIN25_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN25_INT_ENA    0x0000001FU
#define GPIO_PIN25_INT_ENA_M  (GPIO_PIN25_INT_ENA_V << GPIO_PIN25_INT_ENA_S)
#define GPIO_PIN25_INT_ENA_V  0x0000001FU
#define GPIO_PIN25_INT_ENA_S  13

/** GPIO_PIN26_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN26_REG (DR_REG_GPIO_BASE + 0x12c)
/** GPIO_PIN26_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN26_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN26_SYNC2_BYPASS_M  (GPIO_PIN26_SYNC2_BYPASS_V << GPIO_PIN26_SYNC2_BYPASS_S)
#define GPIO_PIN26_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN26_SYNC2_BYPASS_S  0
/** GPIO_PIN26_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN26_PAD_DRIVER    (BIT(2))
#define GPIO_PIN26_PAD_DRIVER_M  (GPIO_PIN26_PAD_DRIVER_V << GPIO_PIN26_PAD_DRIVER_S)
#define GPIO_PIN26_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN26_PAD_DRIVER_S  2
/** GPIO_PIN26_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN26_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN26_SYNC1_BYPASS_M  (GPIO_PIN26_SYNC1_BYPASS_V << GPIO_PIN26_SYNC1_BYPASS_S)
#define GPIO_PIN26_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN26_SYNC1_BYPASS_S  3
/** GPIO_PIN26_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN26_INT_TYPE    0x00000007U
#define GPIO_PIN26_INT_TYPE_M  (GPIO_PIN26_INT_TYPE_V << GPIO_PIN26_INT_TYPE_S)
#define GPIO_PIN26_INT_TYPE_V  0x00000007U
#define GPIO_PIN26_INT_TYPE_S  7
/** GPIO_PIN26_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN26_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN26_WAKEUP_ENABLE_M  (GPIO_PIN26_WAKEUP_ENABLE_V << GPIO_PIN26_WAKEUP_ENABLE_S)
#define GPIO_PIN26_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN26_WAKEUP_ENABLE_S  10
/** GPIO_PIN26_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN26_INT_ENA    0x0000001FU
#define GPIO_PIN26_INT_ENA_M  (GPIO_PIN26_INT_ENA_V << GPIO_PIN26_INT_ENA_S)
#define GPIO_PIN26_INT_ENA_V  0x0000001FU
#define GPIO_PIN26_INT_ENA_S  13

/** GPIO_PIN27_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN27_REG (DR_REG_GPIO_BASE + 0x130)
/** GPIO_PIN27_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN27_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN27_SYNC2_BYPASS_M  (GPIO_PIN27_SYNC2_BYPASS_V << GPIO_PIN27_SYNC2_BYPASS_S)
#define GPIO_PIN27_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN27_SYNC2_BYPASS_S  0
/** GPIO_PIN27_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN27_PAD_DRIVER    (BIT(2))
#define GPIO_PIN27_PAD_DRIVER_M  (GPIO_PIN27_PAD_DRIVER_V << GPIO_PIN27_PAD_DRIVER_S)
#define GPIO_PIN27_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN27_PAD_DRIVER_S  2
/** GPIO_PIN27_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN27_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN27_SYNC1_BYPASS_M  (GPIO_PIN27_SYNC1_BYPASS_V << GPIO_PIN27_SYNC1_BYPASS_S)
#define GPIO_PIN27_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN27_SYNC1_BYPASS_S  3
/** GPIO_PIN27_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN27_INT_TYPE    0x00000007U
#define GPIO_PIN27_INT_TYPE_M  (GPIO_PIN27_INT_TYPE_V << GPIO_PIN27_INT_TYPE_S)
#define GPIO_PIN27_INT_TYPE_V  0x00000007U
#define GPIO_PIN27_INT_TYPE_S  7
/** GPIO_PIN27_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN27_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN27_WAKEUP_ENABLE_M  (GPIO_PIN27_WAKEUP_ENABLE_V << GPIO_PIN27_WAKEUP_ENABLE_S)
#define GPIO_PIN27_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN27_WAKEUP_ENABLE_S  10
/** GPIO_PIN27_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN27_INT_ENA    0x0000001FU
#define GPIO_PIN27_INT_ENA_M  (GPIO_PIN27_INT_ENA_V << GPIO_PIN27_INT_ENA_S)
#define GPIO_PIN27_INT_ENA_V  0x0000001FU
#define GPIO_PIN27_INT_ENA_S  13

/** GPIO_PIN28_REG register
 *  GPIO$N configuration register
 */
#define GPIO_PIN28_REG (DR_REG_GPIO_BASE + 0x134)
/** GPIO_PIN28_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN28_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN28_SYNC2_BYPASS_M  (GPIO_PIN28_SYNC2_BYPASS_V << GPIO_PIN28_SYNC2_BYPASS_S)
#define GPIO_PIN28_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN28_SYNC2_BYPASS_S  0
/** GPIO_PIN28_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN28_PAD_DRIVER    (BIT(2))
#define GPIO_PIN28_PAD_DRIVER_M  (GPIO_PIN28_PAD_DRIVER_V << GPIO_PIN28_PAD_DRIVER_S)
#define GPIO_PIN28_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN28_PAD_DRIVER_S  2
/** GPIO_PIN28_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN28_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN28_SYNC1_BYPASS_M  (GPIO_PIN28_SYNC1_BYPASS_V << GPIO_PIN28_SYNC1_BYPASS_S)
#define GPIO_PIN28_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN28_SYNC1_BYPASS_S  3
/** GPIO_PIN28_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN28_INT_TYPE    0x00000007U
#define GPIO_PIN28_INT_TYPE_M  (GPIO_PIN28_INT_TYPE_V << GPIO_PIN28_INT_TYPE_S)
#define GPIO_PIN28_INT_TYPE_V  0x00000007U
#define GPIO_PIN28_INT_TYPE_S  7
/** GPIO_PIN28_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN28_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN28_WAKEUP_ENABLE_M  (GPIO_PIN28_WAKEUP_ENABLE_V << GPIO_PIN28_WAKEUP_ENABLE_S)
#define GPIO_PIN28_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN28_WAKEUP_ENABLE_S  10
/** GPIO_PIN28_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN28_INT_ENA    0x0000001FU
#define GPIO_PIN28_INT_ENA_M  (GPIO_PIN28_INT_ENA_V << GPIO_PIN28_INT_ENA_S)
#define GPIO_PIN28_INT_ENA_V  0x0000001FU
#define GPIO_PIN28_INT_ENA_S  13

/** GPIO_PIN29_REG register
 *  GPIO29 configuration register
 */
#define GPIO_PIN29_REG (DR_REG_GPIO_BASE + 0x138)
/** GPIO_PIN29_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN29_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN29_SYNC2_BYPASS_M  (GPIO_PIN29_SYNC2_BYPASS_V << GPIO_PIN29_SYNC2_BYPASS_S)
#define GPIO_PIN29_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN29_SYNC2_BYPASS_S  0
/** GPIO_PIN29_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN29_PAD_DRIVER    (BIT(2))
#define GPIO_PIN29_PAD_DRIVER_M  (GPIO_PIN29_PAD_DRIVER_V << GPIO_PIN29_PAD_DRIVER_S)
#define GPIO_PIN29_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN29_PAD_DRIVER_S  2
/** GPIO_PIN29_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN29_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN29_SYNC1_BYPASS_M  (GPIO_PIN29_SYNC1_BYPASS_V << GPIO_PIN29_SYNC1_BYPASS_S)
#define GPIO_PIN29_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN29_SYNC1_BYPASS_S  3
/** GPIO_PIN29_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN29_INT_TYPE    0x00000007U
#define GPIO_PIN29_INT_TYPE_M  (GPIO_PIN29_INT_TYPE_V << GPIO_PIN29_INT_TYPE_S)
#define GPIO_PIN29_INT_TYPE_V  0x00000007U
#define GPIO_PIN29_INT_TYPE_S  7
/** GPIO_PIN29_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN29_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN29_WAKEUP_ENABLE_M  (GPIO_PIN29_WAKEUP_ENABLE_V << GPIO_PIN29_WAKEUP_ENABLE_S)
#define GPIO_PIN29_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN29_WAKEUP_ENABLE_S  10
/** GPIO_PIN29_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN29_INT_ENA    0x0000001FU
#define GPIO_PIN29_INT_ENA_M  (GPIO_PIN29_INT_ENA_V << GPIO_PIN29_INT_ENA_S)
#define GPIO_PIN29_INT_ENA_V  0x0000001FU
#define GPIO_PIN29_INT_ENA_S  13

/** GPIO_PIN30_REG register
 *  GPIO30 configuration register
 */
#define GPIO_PIN30_REG (DR_REG_GPIO_BASE + 0x13c)
/** GPIO_PIN30_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN30_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN30_SYNC2_BYPASS_M  (GPIO_PIN30_SYNC2_BYPASS_V << GPIO_PIN30_SYNC2_BYPASS_S)
#define GPIO_PIN30_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN30_SYNC2_BYPASS_S  0
/** GPIO_PIN30_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN30_PAD_DRIVER    (BIT(2))
#define GPIO_PIN30_PAD_DRIVER_M  (GPIO_PIN30_PAD_DRIVER_V << GPIO_PIN30_PAD_DRIVER_S)
#define GPIO_PIN30_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN30_PAD_DRIVER_S  2
/** GPIO_PIN30_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN30_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN30_SYNC1_BYPASS_M  (GPIO_PIN30_SYNC1_BYPASS_V << GPIO_PIN30_SYNC1_BYPASS_S)
#define GPIO_PIN30_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN30_SYNC1_BYPASS_S  3
/** GPIO_PIN30_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN30_INT_TYPE    0x00000007U
#define GPIO_PIN30_INT_TYPE_M  (GPIO_PIN30_INT_TYPE_V << GPIO_PIN30_INT_TYPE_S)
#define GPIO_PIN30_INT_TYPE_V  0x00000007U
#define GPIO_PIN30_INT_TYPE_S  7
/** GPIO_PIN30_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN30_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN30_WAKEUP_ENABLE_M  (GPIO_PIN30_WAKEUP_ENABLE_V << GPIO_PIN30_WAKEUP_ENABLE_S)
#define GPIO_PIN30_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN30_WAKEUP_ENABLE_S  10
/** GPIO_PIN30_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN30_INT_ENA    0x0000001FU
#define GPIO_PIN30_INT_ENA_M  (GPIO_PIN30_INT_ENA_V << GPIO_PIN30_INT_ENA_S)
#define GPIO_PIN30_INT_ENA_V  0x0000001FU
#define GPIO_PIN30_INT_ENA_S  13

/** GPIO_PIN31_REG register
 *  GPIO31 configuration register
 */
#define GPIO_PIN31_REG (DR_REG_GPIO_BASE + 0x140)
/** GPIO_PIN31_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN31_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN31_SYNC2_BYPASS_M  (GPIO_PIN31_SYNC2_BYPASS_V << GPIO_PIN31_SYNC2_BYPASS_S)
#define GPIO_PIN31_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN31_SYNC2_BYPASS_S  0
/** GPIO_PIN31_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN31_PAD_DRIVER    (BIT(2))
#define GPIO_PIN31_PAD_DRIVER_M  (GPIO_PIN31_PAD_DRIVER_V << GPIO_PIN31_PAD_DRIVER_S)
#define GPIO_PIN31_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN31_PAD_DRIVER_S  2
/** GPIO_PIN31_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN31_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN31_SYNC1_BYPASS_M  (GPIO_PIN31_SYNC1_BYPASS_V << GPIO_PIN31_SYNC1_BYPASS_S)
#define GPIO_PIN31_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN31_SYNC1_BYPASS_S  3
/** GPIO_PIN31_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN31_INT_TYPE    0x00000007U
#define GPIO_PIN31_INT_TYPE_M  (GPIO_PIN31_INT_TYPE_V << GPIO_PIN31_INT_TYPE_S)
#define GPIO_PIN31_INT_TYPE_V  0x00000007U
#define GPIO_PIN31_INT_TYPE_S  7
/** GPIO_PIN31_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN31_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN31_WAKEUP_ENABLE_M  (GPIO_PIN31_WAKEUP_ENABLE_V << GPIO_PIN31_WAKEUP_ENABLE_S)
#define GPIO_PIN31_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN31_WAKEUP_ENABLE_S  10
/** GPIO_PIN31_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN31_INT_ENA    0x0000001FU
#define GPIO_PIN31_INT_ENA_M  (GPIO_PIN31_INT_ENA_V << GPIO_PIN31_INT_ENA_S)
#define GPIO_PIN31_INT_ENA_V  0x0000001FU
#define GPIO_PIN31_INT_ENA_S  13

/** GPIO_PIN32_REG register
 *  GPIO32 configuration register
 */
#define GPIO_PIN32_REG (DR_REG_GPIO_BASE + 0x144)
/** GPIO_PIN32_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN32_SYNC2_BYPASS    0x00000003U
#define GPIO_PIN32_SYNC2_BYPASS_M  (GPIO_PIN32_SYNC2_BYPASS_V << GPIO_PIN32_SYNC2_BYPASS_S)
#define GPIO_PIN32_SYNC2_BYPASS_V  0x00000003U
#define GPIO_PIN32_SYNC2_BYPASS_S  0
/** GPIO_PIN32_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define GPIO_PIN32_PAD_DRIVER    (BIT(2))
#define GPIO_PIN32_PAD_DRIVER_M  (GPIO_PIN32_PAD_DRIVER_V << GPIO_PIN32_PAD_DRIVER_S)
#define GPIO_PIN32_PAD_DRIVER_V  0x00000001U
#define GPIO_PIN32_PAD_DRIVER_S  2
/** GPIO_PIN32_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
 *  operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define GPIO_PIN32_SYNC1_BYPASS    0x00000003U
#define GPIO_PIN32_SYNC1_BYPASS_M  (GPIO_PIN32_SYNC1_BYPASS_V << GPIO_PIN32_SYNC1_BYPASS_S)
#define GPIO_PIN32_SYNC1_BYPASS_V  0x00000003U
#define GPIO_PIN32_SYNC1_BYPASS_S  3
/** GPIO_PIN32_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO interrupt type.\\
 *  0: GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define GPIO_PIN32_INT_TYPE    0x00000007U
#define GPIO_PIN32_INT_TYPE_M  (GPIO_PIN32_INT_TYPE_V << GPIO_PIN32_INT_TYPE_S)
#define GPIO_PIN32_INT_TYPE_V  0x00000007U
#define GPIO_PIN32_INT_TYPE_S  7
/** GPIO_PIN32_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define GPIO_PIN32_WAKEUP_ENABLE    (BIT(10))
#define GPIO_PIN32_WAKEUP_ENABLE_M  (GPIO_PIN32_WAKEUP_ENABLE_V << GPIO_PIN32_WAKEUP_ENABLE_S)
#define GPIO_PIN32_WAKEUP_ENABLE_V  0x00000001U
#define GPIO_PIN32_WAKEUP_ENABLE_S  10
/** GPIO_PIN32_INT_ENA : R/W; bitpos: [17:13]; default: 0;
 *  Configures whether or not to enable CPU interrupt or CPU non-maskable interrupt.
 *
 *  - bit13: Configures whether or not to enable CPU interrupt:\\
 *  0: Disable\\
 *  1: Enable\\
 *  - bit14 ~ bit17: invalid
 */
#define GPIO_PIN32_INT_ENA    0x0000001FU
#define GPIO_PIN32_INT_ENA_M  (GPIO_PIN32_INT_ENA_V << GPIO_PIN32_INT_ENA_S)
#define GPIO_PIN32_INT_ENA_V  0x0000001FU
#define GPIO_PIN32_INT_ENA_S  13

/** GPIO_FUNC0_IN_SEL_CFG_REG register
 *  Configuration register for input signal 0
 */
#define GPIO_FUNC0_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c4)
/** GPIO_FUNC0_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 0.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC0_IN_SEL    0x0000007FU
#define GPIO_FUNC0_IN_SEL_M  (GPIO_FUNC0_IN_SEL_V << GPIO_FUNC0_IN_SEL_S)
#define GPIO_FUNC0_IN_SEL_V  0x0000007FU
#define GPIO_FUNC0_IN_SEL_S  0
/** GPIO_FUNC0_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC0_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC0_IN_INV_SEL_M  (GPIO_FUNC0_IN_INV_SEL_V << GPIO_FUNC0_IN_INV_SEL_S)
#define GPIO_FUNC0_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_IN_INV_SEL_S  7
/** GPIO_SIG0_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG0_IN_SEL    (BIT(8))
#define GPIO_SIG0_IN_SEL_M  (GPIO_SIG0_IN_SEL_V << GPIO_SIG0_IN_SEL_S)
#define GPIO_SIG0_IN_SEL_V  0x00000001U
#define GPIO_SIG0_IN_SEL_S  8

/** GPIO_FUNC6_IN_SEL_CFG_REG register
 *  Configuration register for input signal 6
 */
#define GPIO_FUNC6_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2dc)
/** GPIO_FUNC6_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 6.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC6_IN_SEL    0x0000007FU
#define GPIO_FUNC6_IN_SEL_M  (GPIO_FUNC6_IN_SEL_V << GPIO_FUNC6_IN_SEL_S)
#define GPIO_FUNC6_IN_SEL_V  0x0000007FU
#define GPIO_FUNC6_IN_SEL_S  0
/** GPIO_FUNC6_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC6_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC6_IN_INV_SEL_M  (GPIO_FUNC6_IN_INV_SEL_V << GPIO_FUNC6_IN_INV_SEL_S)
#define GPIO_FUNC6_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_IN_INV_SEL_S  7
/** GPIO_SIG6_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG6_IN_SEL    (BIT(8))
#define GPIO_SIG6_IN_SEL_M  (GPIO_SIG6_IN_SEL_V << GPIO_SIG6_IN_SEL_S)
#define GPIO_SIG6_IN_SEL_V  0x00000001U
#define GPIO_SIG6_IN_SEL_S  8

/** GPIO_FUNC7_IN_SEL_CFG_REG register
 *  Configuration register for input signal 7
 */
#define GPIO_FUNC7_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e0)
/** GPIO_FUNC7_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 7.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC7_IN_SEL    0x0000007FU
#define GPIO_FUNC7_IN_SEL_M  (GPIO_FUNC7_IN_SEL_V << GPIO_FUNC7_IN_SEL_S)
#define GPIO_FUNC7_IN_SEL_V  0x0000007FU
#define GPIO_FUNC7_IN_SEL_S  0
/** GPIO_FUNC7_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC7_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC7_IN_INV_SEL_M  (GPIO_FUNC7_IN_INV_SEL_V << GPIO_FUNC7_IN_INV_SEL_S)
#define GPIO_FUNC7_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_IN_INV_SEL_S  7
/** GPIO_SIG7_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG7_IN_SEL    (BIT(8))
#define GPIO_SIG7_IN_SEL_M  (GPIO_SIG7_IN_SEL_V << GPIO_SIG7_IN_SEL_S)
#define GPIO_SIG7_IN_SEL_V  0x00000001U
#define GPIO_SIG7_IN_SEL_S  8

/** GPIO_FUNC8_IN_SEL_CFG_REG register
 *  Configuration register for input signal 8
 */
#define GPIO_FUNC8_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e4)
/** GPIO_FUNC8_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 8.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC8_IN_SEL    0x0000007FU
#define GPIO_FUNC8_IN_SEL_M  (GPIO_FUNC8_IN_SEL_V << GPIO_FUNC8_IN_SEL_S)
#define GPIO_FUNC8_IN_SEL_V  0x0000007FU
#define GPIO_FUNC8_IN_SEL_S  0
/** GPIO_FUNC8_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC8_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC8_IN_INV_SEL_M  (GPIO_FUNC8_IN_INV_SEL_V << GPIO_FUNC8_IN_INV_SEL_S)
#define GPIO_FUNC8_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_IN_INV_SEL_S  7
/** GPIO_SIG8_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG8_IN_SEL    (BIT(8))
#define GPIO_SIG8_IN_SEL_M  (GPIO_SIG8_IN_SEL_V << GPIO_SIG8_IN_SEL_S)
#define GPIO_SIG8_IN_SEL_V  0x00000001U
#define GPIO_SIG8_IN_SEL_S  8

/** GPIO_FUNC9_IN_SEL_CFG_REG register
 *  Configuration register for input signal 9
 */
#define GPIO_FUNC9_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e8)
/** GPIO_FUNC9_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 9.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC9_IN_SEL    0x0000007FU
#define GPIO_FUNC9_IN_SEL_M  (GPIO_FUNC9_IN_SEL_V << GPIO_FUNC9_IN_SEL_S)
#define GPIO_FUNC9_IN_SEL_V  0x0000007FU
#define GPIO_FUNC9_IN_SEL_S  0
/** GPIO_FUNC9_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC9_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC9_IN_INV_SEL_M  (GPIO_FUNC9_IN_INV_SEL_V << GPIO_FUNC9_IN_INV_SEL_S)
#define GPIO_FUNC9_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_IN_INV_SEL_S  7
/** GPIO_SIG9_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG9_IN_SEL    (BIT(8))
#define GPIO_SIG9_IN_SEL_M  (GPIO_SIG9_IN_SEL_V << GPIO_SIG9_IN_SEL_S)
#define GPIO_SIG9_IN_SEL_V  0x00000001U
#define GPIO_SIG9_IN_SEL_S  8

/** GPIO_FUNC10_IN_SEL_CFG_REG register
 *  Configuration register for input signal 10
 */
#define GPIO_FUNC10_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2ec)
/** GPIO_FUNC10_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 10.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC10_IN_SEL    0x0000007FU
#define GPIO_FUNC10_IN_SEL_M  (GPIO_FUNC10_IN_SEL_V << GPIO_FUNC10_IN_SEL_S)
#define GPIO_FUNC10_IN_SEL_V  0x0000007FU
#define GPIO_FUNC10_IN_SEL_S  0
/** GPIO_FUNC10_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC10_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC10_IN_INV_SEL_M  (GPIO_FUNC10_IN_INV_SEL_V << GPIO_FUNC10_IN_INV_SEL_S)
#define GPIO_FUNC10_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_IN_INV_SEL_S  7
/** GPIO_SIG10_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG10_IN_SEL    (BIT(8))
#define GPIO_SIG10_IN_SEL_M  (GPIO_SIG10_IN_SEL_V << GPIO_SIG10_IN_SEL_S)
#define GPIO_SIG10_IN_SEL_V  0x00000001U
#define GPIO_SIG10_IN_SEL_S  8

/** GPIO_FUNC11_IN_SEL_CFG_REG register
 *  Configuration register for input signal 11
 */
#define GPIO_FUNC11_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f0)
/** GPIO_FUNC11_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 11.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC11_IN_SEL    0x0000007FU
#define GPIO_FUNC11_IN_SEL_M  (GPIO_FUNC11_IN_SEL_V << GPIO_FUNC11_IN_SEL_S)
#define GPIO_FUNC11_IN_SEL_V  0x0000007FU
#define GPIO_FUNC11_IN_SEL_S  0
/** GPIO_FUNC11_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC11_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC11_IN_INV_SEL_M  (GPIO_FUNC11_IN_INV_SEL_V << GPIO_FUNC11_IN_INV_SEL_S)
#define GPIO_FUNC11_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_IN_INV_SEL_S  7
/** GPIO_SIG11_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG11_IN_SEL    (BIT(8))
#define GPIO_SIG11_IN_SEL_M  (GPIO_SIG11_IN_SEL_V << GPIO_SIG11_IN_SEL_S)
#define GPIO_SIG11_IN_SEL_V  0x00000001U
#define GPIO_SIG11_IN_SEL_S  8

/** GPIO_FUNC12_IN_SEL_CFG_REG register
 *  Configuration register for input signal 12
 */
#define GPIO_FUNC12_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f4)
/** GPIO_FUNC12_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 12.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC12_IN_SEL    0x0000007FU
#define GPIO_FUNC12_IN_SEL_M  (GPIO_FUNC12_IN_SEL_V << GPIO_FUNC12_IN_SEL_S)
#define GPIO_FUNC12_IN_SEL_V  0x0000007FU
#define GPIO_FUNC12_IN_SEL_S  0
/** GPIO_FUNC12_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC12_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC12_IN_INV_SEL_M  (GPIO_FUNC12_IN_INV_SEL_V << GPIO_FUNC12_IN_INV_SEL_S)
#define GPIO_FUNC12_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_IN_INV_SEL_S  7
/** GPIO_SIG12_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG12_IN_SEL    (BIT(8))
#define GPIO_SIG12_IN_SEL_M  (GPIO_SIG12_IN_SEL_V << GPIO_SIG12_IN_SEL_S)
#define GPIO_SIG12_IN_SEL_V  0x00000001U
#define GPIO_SIG12_IN_SEL_S  8

/** GPIO_FUNC13_IN_SEL_CFG_REG register
 *  Configuration register for input signal 13
 */
#define GPIO_FUNC13_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f8)
/** GPIO_FUNC13_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 13.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC13_IN_SEL    0x0000007FU
#define GPIO_FUNC13_IN_SEL_M  (GPIO_FUNC13_IN_SEL_V << GPIO_FUNC13_IN_SEL_S)
#define GPIO_FUNC13_IN_SEL_V  0x0000007FU
#define GPIO_FUNC13_IN_SEL_S  0
/** GPIO_FUNC13_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC13_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC13_IN_INV_SEL_M  (GPIO_FUNC13_IN_INV_SEL_V << GPIO_FUNC13_IN_INV_SEL_S)
#define GPIO_FUNC13_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_IN_INV_SEL_S  7
/** GPIO_SIG13_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG13_IN_SEL    (BIT(8))
#define GPIO_SIG13_IN_SEL_M  (GPIO_SIG13_IN_SEL_V << GPIO_SIG13_IN_SEL_S)
#define GPIO_SIG13_IN_SEL_V  0x00000001U
#define GPIO_SIG13_IN_SEL_S  8

/** GPIO_FUNC14_IN_SEL_CFG_REG register
 *  Configuration register for input signal 14
 */
#define GPIO_FUNC14_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2fc)
/** GPIO_FUNC14_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 14.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC14_IN_SEL    0x0000007FU
#define GPIO_FUNC14_IN_SEL_M  (GPIO_FUNC14_IN_SEL_V << GPIO_FUNC14_IN_SEL_S)
#define GPIO_FUNC14_IN_SEL_V  0x0000007FU
#define GPIO_FUNC14_IN_SEL_S  0
/** GPIO_FUNC14_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC14_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC14_IN_INV_SEL_M  (GPIO_FUNC14_IN_INV_SEL_V << GPIO_FUNC14_IN_INV_SEL_S)
#define GPIO_FUNC14_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_IN_INV_SEL_S  7
/** GPIO_SIG14_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG14_IN_SEL    (BIT(8))
#define GPIO_SIG14_IN_SEL_M  (GPIO_SIG14_IN_SEL_V << GPIO_SIG14_IN_SEL_S)
#define GPIO_SIG14_IN_SEL_V  0x00000001U
#define GPIO_SIG14_IN_SEL_S  8

/** GPIO_FUNC15_IN_SEL_CFG_REG register
 *  Configuration register for input signal 15
 */
#define GPIO_FUNC15_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x300)
/** GPIO_FUNC15_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 15.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC15_IN_SEL    0x0000007FU
#define GPIO_FUNC15_IN_SEL_M  (GPIO_FUNC15_IN_SEL_V << GPIO_FUNC15_IN_SEL_S)
#define GPIO_FUNC15_IN_SEL_V  0x0000007FU
#define GPIO_FUNC15_IN_SEL_S  0
/** GPIO_FUNC15_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC15_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC15_IN_INV_SEL_M  (GPIO_FUNC15_IN_INV_SEL_V << GPIO_FUNC15_IN_INV_SEL_S)
#define GPIO_FUNC15_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_IN_INV_SEL_S  7
/** GPIO_SIG15_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG15_IN_SEL    (BIT(8))
#define GPIO_SIG15_IN_SEL_M  (GPIO_SIG15_IN_SEL_V << GPIO_SIG15_IN_SEL_S)
#define GPIO_SIG15_IN_SEL_V  0x00000001U
#define GPIO_SIG15_IN_SEL_S  8

/** GPIO_FUNC16_IN_SEL_CFG_REG register
 *  Configuration register for input signal 16
 */
#define GPIO_FUNC16_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x304)
/** GPIO_FUNC16_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 16.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC16_IN_SEL    0x0000007FU
#define GPIO_FUNC16_IN_SEL_M  (GPIO_FUNC16_IN_SEL_V << GPIO_FUNC16_IN_SEL_S)
#define GPIO_FUNC16_IN_SEL_V  0x0000007FU
#define GPIO_FUNC16_IN_SEL_S  0
/** GPIO_FUNC16_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC16_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC16_IN_INV_SEL_M  (GPIO_FUNC16_IN_INV_SEL_V << GPIO_FUNC16_IN_INV_SEL_S)
#define GPIO_FUNC16_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_IN_INV_SEL_S  7
/** GPIO_SIG16_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG16_IN_SEL    (BIT(8))
#define GPIO_SIG16_IN_SEL_M  (GPIO_SIG16_IN_SEL_V << GPIO_SIG16_IN_SEL_S)
#define GPIO_SIG16_IN_SEL_V  0x00000001U
#define GPIO_SIG16_IN_SEL_S  8

/** GPIO_FUNC17_IN_SEL_CFG_REG register
 *  Configuration register for input signal 17
 */
#define GPIO_FUNC17_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x308)
/** GPIO_FUNC17_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 17.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC17_IN_SEL    0x0000007FU
#define GPIO_FUNC17_IN_SEL_M  (GPIO_FUNC17_IN_SEL_V << GPIO_FUNC17_IN_SEL_S)
#define GPIO_FUNC17_IN_SEL_V  0x0000007FU
#define GPIO_FUNC17_IN_SEL_S  0
/** GPIO_FUNC17_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC17_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC17_IN_INV_SEL_M  (GPIO_FUNC17_IN_INV_SEL_V << GPIO_FUNC17_IN_INV_SEL_S)
#define GPIO_FUNC17_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_IN_INV_SEL_S  7
/** GPIO_SIG17_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG17_IN_SEL    (BIT(8))
#define GPIO_SIG17_IN_SEL_M  (GPIO_SIG17_IN_SEL_V << GPIO_SIG17_IN_SEL_S)
#define GPIO_SIG17_IN_SEL_V  0x00000001U
#define GPIO_SIG17_IN_SEL_S  8

/** GPIO_FUNC27_IN_SEL_CFG_REG register
 *  Configuration register for input signal 27
 */
#define GPIO_FUNC27_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x330)
/** GPIO_FUNC27_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 27.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC27_IN_SEL    0x0000007FU
#define GPIO_FUNC27_IN_SEL_M  (GPIO_FUNC27_IN_SEL_V << GPIO_FUNC27_IN_SEL_S)
#define GPIO_FUNC27_IN_SEL_V  0x0000007FU
#define GPIO_FUNC27_IN_SEL_S  0
/** GPIO_FUNC27_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC27_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC27_IN_INV_SEL_M  (GPIO_FUNC27_IN_INV_SEL_V << GPIO_FUNC27_IN_INV_SEL_S)
#define GPIO_FUNC27_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_IN_INV_SEL_S  7
/** GPIO_SIG27_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG27_IN_SEL    (BIT(8))
#define GPIO_SIG27_IN_SEL_M  (GPIO_SIG27_IN_SEL_V << GPIO_SIG27_IN_SEL_S)
#define GPIO_SIG27_IN_SEL_V  0x00000001U
#define GPIO_SIG27_IN_SEL_S  8

/** GPIO_FUNC28_IN_SEL_CFG_REG register
 *  Configuration register for input signal 28
 */
#define GPIO_FUNC28_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x334)
/** GPIO_FUNC28_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 28.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC28_IN_SEL    0x0000007FU
#define GPIO_FUNC28_IN_SEL_M  (GPIO_FUNC28_IN_SEL_V << GPIO_FUNC28_IN_SEL_S)
#define GPIO_FUNC28_IN_SEL_V  0x0000007FU
#define GPIO_FUNC28_IN_SEL_S  0
/** GPIO_FUNC28_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC28_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC28_IN_INV_SEL_M  (GPIO_FUNC28_IN_INV_SEL_V << GPIO_FUNC28_IN_INV_SEL_S)
#define GPIO_FUNC28_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_IN_INV_SEL_S  7
/** GPIO_SIG28_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG28_IN_SEL    (BIT(8))
#define GPIO_SIG28_IN_SEL_M  (GPIO_SIG28_IN_SEL_V << GPIO_SIG28_IN_SEL_S)
#define GPIO_SIG28_IN_SEL_V  0x00000001U
#define GPIO_SIG28_IN_SEL_S  8

/** GPIO_FUNC29_IN_SEL_CFG_REG register
 *  Configuration register for input signal 29
 */
#define GPIO_FUNC29_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x338)
/** GPIO_FUNC29_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 29.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC29_IN_SEL    0x0000007FU
#define GPIO_FUNC29_IN_SEL_M  (GPIO_FUNC29_IN_SEL_V << GPIO_FUNC29_IN_SEL_S)
#define GPIO_FUNC29_IN_SEL_V  0x0000007FU
#define GPIO_FUNC29_IN_SEL_S  0
/** GPIO_FUNC29_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC29_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC29_IN_INV_SEL_M  (GPIO_FUNC29_IN_INV_SEL_V << GPIO_FUNC29_IN_INV_SEL_S)
#define GPIO_FUNC29_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_IN_INV_SEL_S  7
/** GPIO_SIG29_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG29_IN_SEL    (BIT(8))
#define GPIO_SIG29_IN_SEL_M  (GPIO_SIG29_IN_SEL_V << GPIO_SIG29_IN_SEL_S)
#define GPIO_SIG29_IN_SEL_V  0x00000001U
#define GPIO_SIG29_IN_SEL_S  8

/** GPIO_FUNC30_IN_SEL_CFG_REG register
 *  Configuration register for input signal 30
 */
#define GPIO_FUNC30_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x33c)
/** GPIO_FUNC30_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 30.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC30_IN_SEL    0x0000007FU
#define GPIO_FUNC30_IN_SEL_M  (GPIO_FUNC30_IN_SEL_V << GPIO_FUNC30_IN_SEL_S)
#define GPIO_FUNC30_IN_SEL_V  0x0000007FU
#define GPIO_FUNC30_IN_SEL_S  0
/** GPIO_FUNC30_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC30_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC30_IN_INV_SEL_M  (GPIO_FUNC30_IN_INV_SEL_V << GPIO_FUNC30_IN_INV_SEL_S)
#define GPIO_FUNC30_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_IN_INV_SEL_S  7
/** GPIO_SIG30_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG30_IN_SEL    (BIT(8))
#define GPIO_SIG30_IN_SEL_M  (GPIO_SIG30_IN_SEL_V << GPIO_SIG30_IN_SEL_S)
#define GPIO_SIG30_IN_SEL_V  0x00000001U
#define GPIO_SIG30_IN_SEL_S  8

/** GPIO_FUNC31_IN_SEL_CFG_REG register
 *  Configuration register for input signal 31
 */
#define GPIO_FUNC31_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x340)
/** GPIO_FUNC31_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 31.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC31_IN_SEL    0x0000007FU
#define GPIO_FUNC31_IN_SEL_M  (GPIO_FUNC31_IN_SEL_V << GPIO_FUNC31_IN_SEL_S)
#define GPIO_FUNC31_IN_SEL_V  0x0000007FU
#define GPIO_FUNC31_IN_SEL_S  0
/** GPIO_FUNC31_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC31_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC31_IN_INV_SEL_M  (GPIO_FUNC31_IN_INV_SEL_V << GPIO_FUNC31_IN_INV_SEL_S)
#define GPIO_FUNC31_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_IN_INV_SEL_S  7
/** GPIO_SIG31_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG31_IN_SEL    (BIT(8))
#define GPIO_SIG31_IN_SEL_M  (GPIO_SIG31_IN_SEL_V << GPIO_SIG31_IN_SEL_S)
#define GPIO_SIG31_IN_SEL_V  0x00000001U
#define GPIO_SIG31_IN_SEL_S  8

/** GPIO_FUNC32_IN_SEL_CFG_REG register
 *  Configuration register for input signal 32
 */
#define GPIO_FUNC32_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x344)
/** GPIO_FUNC32_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 32.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC32_IN_SEL    0x0000007FU
#define GPIO_FUNC32_IN_SEL_M  (GPIO_FUNC32_IN_SEL_V << GPIO_FUNC32_IN_SEL_S)
#define GPIO_FUNC32_IN_SEL_V  0x0000007FU
#define GPIO_FUNC32_IN_SEL_S  0
/** GPIO_FUNC32_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC32_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC32_IN_INV_SEL_M  (GPIO_FUNC32_IN_INV_SEL_V << GPIO_FUNC32_IN_INV_SEL_S)
#define GPIO_FUNC32_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_IN_INV_SEL_S  7
/** GPIO_SIG32_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG32_IN_SEL    (BIT(8))
#define GPIO_SIG32_IN_SEL_M  (GPIO_SIG32_IN_SEL_V << GPIO_SIG32_IN_SEL_S)
#define GPIO_SIG32_IN_SEL_V  0x00000001U
#define GPIO_SIG32_IN_SEL_S  8

/** GPIO_FUNC33_IN_SEL_CFG_REG register
 *  Configuration register for input signal 33
 */
#define GPIO_FUNC33_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x348)
/** GPIO_FUNC33_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 33.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC33_IN_SEL    0x0000007FU
#define GPIO_FUNC33_IN_SEL_M  (GPIO_FUNC33_IN_SEL_V << GPIO_FUNC33_IN_SEL_S)
#define GPIO_FUNC33_IN_SEL_V  0x0000007FU
#define GPIO_FUNC33_IN_SEL_S  0
/** GPIO_FUNC33_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC33_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC33_IN_INV_SEL_M  (GPIO_FUNC33_IN_INV_SEL_V << GPIO_FUNC33_IN_INV_SEL_S)
#define GPIO_FUNC33_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_IN_INV_SEL_S  7
/** GPIO_SIG33_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG33_IN_SEL    (BIT(8))
#define GPIO_SIG33_IN_SEL_M  (GPIO_SIG33_IN_SEL_V << GPIO_SIG33_IN_SEL_S)
#define GPIO_SIG33_IN_SEL_V  0x00000001U
#define GPIO_SIG33_IN_SEL_S  8

/** GPIO_FUNC34_IN_SEL_CFG_REG register
 *  Configuration register for input signal 34
 */
#define GPIO_FUNC34_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x34c)
/** GPIO_FUNC34_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 34.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC34_IN_SEL    0x0000007FU
#define GPIO_FUNC34_IN_SEL_M  (GPIO_FUNC34_IN_SEL_V << GPIO_FUNC34_IN_SEL_S)
#define GPIO_FUNC34_IN_SEL_V  0x0000007FU
#define GPIO_FUNC34_IN_SEL_S  0
/** GPIO_FUNC34_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC34_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC34_IN_INV_SEL_M  (GPIO_FUNC34_IN_INV_SEL_V << GPIO_FUNC34_IN_INV_SEL_S)
#define GPIO_FUNC34_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_IN_INV_SEL_S  7
/** GPIO_SIG34_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG34_IN_SEL    (BIT(8))
#define GPIO_SIG34_IN_SEL_M  (GPIO_SIG34_IN_SEL_V << GPIO_SIG34_IN_SEL_S)
#define GPIO_SIG34_IN_SEL_V  0x00000001U
#define GPIO_SIG34_IN_SEL_S  8

/** GPIO_FUNC35_IN_SEL_CFG_REG register
 *  Configuration register for input signal 35
 */
#define GPIO_FUNC35_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x350)
/** GPIO_FUNC35_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 35.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC35_IN_SEL    0x0000007FU
#define GPIO_FUNC35_IN_SEL_M  (GPIO_FUNC35_IN_SEL_V << GPIO_FUNC35_IN_SEL_S)
#define GPIO_FUNC35_IN_SEL_V  0x0000007FU
#define GPIO_FUNC35_IN_SEL_S  0
/** GPIO_FUNC35_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC35_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC35_IN_INV_SEL_M  (GPIO_FUNC35_IN_INV_SEL_V << GPIO_FUNC35_IN_INV_SEL_S)
#define GPIO_FUNC35_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_IN_INV_SEL_S  7
/** GPIO_SIG35_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG35_IN_SEL    (BIT(8))
#define GPIO_SIG35_IN_SEL_M  (GPIO_SIG35_IN_SEL_V << GPIO_SIG35_IN_SEL_S)
#define GPIO_SIG35_IN_SEL_V  0x00000001U
#define GPIO_SIG35_IN_SEL_S  8

/** GPIO_FUNC41_IN_SEL_CFG_REG register
 *  Configuration register for input signal 41
 */
#define GPIO_FUNC41_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x368)
/** GPIO_FUNC41_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 41.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC41_IN_SEL    0x0000007FU
#define GPIO_FUNC41_IN_SEL_M  (GPIO_FUNC41_IN_SEL_V << GPIO_FUNC41_IN_SEL_S)
#define GPIO_FUNC41_IN_SEL_V  0x0000007FU
#define GPIO_FUNC41_IN_SEL_S  0
/** GPIO_FUNC41_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC41_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC41_IN_INV_SEL_M  (GPIO_FUNC41_IN_INV_SEL_V << GPIO_FUNC41_IN_INV_SEL_S)
#define GPIO_FUNC41_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_IN_INV_SEL_S  7
/** GPIO_SIG41_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG41_IN_SEL    (BIT(8))
#define GPIO_SIG41_IN_SEL_M  (GPIO_SIG41_IN_SEL_V << GPIO_SIG41_IN_SEL_S)
#define GPIO_SIG41_IN_SEL_V  0x00000001U
#define GPIO_SIG41_IN_SEL_S  8

/** GPIO_FUNC42_IN_SEL_CFG_REG register
 *  Configuration register for input signal 42
 */
#define GPIO_FUNC42_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x36c)
/** GPIO_FUNC42_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 42.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC42_IN_SEL    0x0000007FU
#define GPIO_FUNC42_IN_SEL_M  (GPIO_FUNC42_IN_SEL_V << GPIO_FUNC42_IN_SEL_S)
#define GPIO_FUNC42_IN_SEL_V  0x0000007FU
#define GPIO_FUNC42_IN_SEL_S  0
/** GPIO_FUNC42_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC42_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC42_IN_INV_SEL_M  (GPIO_FUNC42_IN_INV_SEL_V << GPIO_FUNC42_IN_INV_SEL_S)
#define GPIO_FUNC42_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_IN_INV_SEL_S  7
/** GPIO_SIG42_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG42_IN_SEL    (BIT(8))
#define GPIO_SIG42_IN_SEL_M  (GPIO_SIG42_IN_SEL_V << GPIO_SIG42_IN_SEL_S)
#define GPIO_SIG42_IN_SEL_V  0x00000001U
#define GPIO_SIG42_IN_SEL_S  8

/** GPIO_FUNC43_IN_SEL_CFG_REG register
 *  Configuration register for input signal 43
 */
#define GPIO_FUNC43_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x370)
/** GPIO_FUNC43_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 43.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC43_IN_SEL    0x0000007FU
#define GPIO_FUNC43_IN_SEL_M  (GPIO_FUNC43_IN_SEL_V << GPIO_FUNC43_IN_SEL_S)
#define GPIO_FUNC43_IN_SEL_V  0x0000007FU
#define GPIO_FUNC43_IN_SEL_S  0
/** GPIO_FUNC43_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC43_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC43_IN_INV_SEL_M  (GPIO_FUNC43_IN_INV_SEL_V << GPIO_FUNC43_IN_INV_SEL_S)
#define GPIO_FUNC43_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_IN_INV_SEL_S  7
/** GPIO_SIG43_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG43_IN_SEL    (BIT(8))
#define GPIO_SIG43_IN_SEL_M  (GPIO_SIG43_IN_SEL_V << GPIO_SIG43_IN_SEL_S)
#define GPIO_SIG43_IN_SEL_V  0x00000001U
#define GPIO_SIG43_IN_SEL_S  8

/** GPIO_FUNC46_IN_SEL_CFG_REG register
 *  Configuration register for input signal 46
 */
#define GPIO_FUNC46_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x37c)
/** GPIO_FUNC46_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 46.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC46_IN_SEL    0x0000007FU
#define GPIO_FUNC46_IN_SEL_M  (GPIO_FUNC46_IN_SEL_V << GPIO_FUNC46_IN_SEL_S)
#define GPIO_FUNC46_IN_SEL_V  0x0000007FU
#define GPIO_FUNC46_IN_SEL_S  0
/** GPIO_FUNC46_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC46_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC46_IN_INV_SEL_M  (GPIO_FUNC46_IN_INV_SEL_V << GPIO_FUNC46_IN_INV_SEL_S)
#define GPIO_FUNC46_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_IN_INV_SEL_S  7
/** GPIO_SIG46_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG46_IN_SEL    (BIT(8))
#define GPIO_SIG46_IN_SEL_M  (GPIO_SIG46_IN_SEL_V << GPIO_SIG46_IN_SEL_S)
#define GPIO_SIG46_IN_SEL_V  0x00000001U
#define GPIO_SIG46_IN_SEL_S  8

/** GPIO_FUNC47_IN_SEL_CFG_REG register
 *  Configuration register for input signal 47
 */
#define GPIO_FUNC47_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x380)
/** GPIO_FUNC47_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 47.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC47_IN_SEL    0x0000007FU
#define GPIO_FUNC47_IN_SEL_M  (GPIO_FUNC47_IN_SEL_V << GPIO_FUNC47_IN_SEL_S)
#define GPIO_FUNC47_IN_SEL_V  0x0000007FU
#define GPIO_FUNC47_IN_SEL_S  0
/** GPIO_FUNC47_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC47_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC47_IN_INV_SEL_M  (GPIO_FUNC47_IN_INV_SEL_V << GPIO_FUNC47_IN_INV_SEL_S)
#define GPIO_FUNC47_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_IN_INV_SEL_S  7
/** GPIO_SIG47_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG47_IN_SEL    (BIT(8))
#define GPIO_SIG47_IN_SEL_M  (GPIO_SIG47_IN_SEL_V << GPIO_SIG47_IN_SEL_S)
#define GPIO_SIG47_IN_SEL_V  0x00000001U
#define GPIO_SIG47_IN_SEL_S  8

/** GPIO_FUNC48_IN_SEL_CFG_REG register
 *  Configuration register for input signal 48
 */
#define GPIO_FUNC48_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x384)
/** GPIO_FUNC48_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 48.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC48_IN_SEL    0x0000007FU
#define GPIO_FUNC48_IN_SEL_M  (GPIO_FUNC48_IN_SEL_V << GPIO_FUNC48_IN_SEL_S)
#define GPIO_FUNC48_IN_SEL_V  0x0000007FU
#define GPIO_FUNC48_IN_SEL_S  0
/** GPIO_FUNC48_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC48_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC48_IN_INV_SEL_M  (GPIO_FUNC48_IN_INV_SEL_V << GPIO_FUNC48_IN_INV_SEL_S)
#define GPIO_FUNC48_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC48_IN_INV_SEL_S  7
/** GPIO_SIG48_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG48_IN_SEL    (BIT(8))
#define GPIO_SIG48_IN_SEL_M  (GPIO_SIG48_IN_SEL_V << GPIO_SIG48_IN_SEL_S)
#define GPIO_SIG48_IN_SEL_V  0x00000001U
#define GPIO_SIG48_IN_SEL_S  8

/** GPIO_FUNC49_IN_SEL_CFG_REG register
 *  Configuration register for input signal 49
 */
#define GPIO_FUNC49_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x388)
/** GPIO_FUNC49_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 49.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC49_IN_SEL    0x0000007FU
#define GPIO_FUNC49_IN_SEL_M  (GPIO_FUNC49_IN_SEL_V << GPIO_FUNC49_IN_SEL_S)
#define GPIO_FUNC49_IN_SEL_V  0x0000007FU
#define GPIO_FUNC49_IN_SEL_S  0
/** GPIO_FUNC49_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC49_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC49_IN_INV_SEL_M  (GPIO_FUNC49_IN_INV_SEL_V << GPIO_FUNC49_IN_INV_SEL_S)
#define GPIO_FUNC49_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC49_IN_INV_SEL_S  7
/** GPIO_SIG49_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG49_IN_SEL    (BIT(8))
#define GPIO_SIG49_IN_SEL_M  (GPIO_SIG49_IN_SEL_V << GPIO_SIG49_IN_SEL_S)
#define GPIO_SIG49_IN_SEL_V  0x00000001U
#define GPIO_SIG49_IN_SEL_S  8

/** GPIO_FUNC50_IN_SEL_CFG_REG register
 *  Configuration register for input signal 50
 */
#define GPIO_FUNC50_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x38c)
/** GPIO_FUNC50_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 50.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC50_IN_SEL    0x0000007FU
#define GPIO_FUNC50_IN_SEL_M  (GPIO_FUNC50_IN_SEL_V << GPIO_FUNC50_IN_SEL_S)
#define GPIO_FUNC50_IN_SEL_V  0x0000007FU
#define GPIO_FUNC50_IN_SEL_S  0
/** GPIO_FUNC50_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC50_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC50_IN_INV_SEL_M  (GPIO_FUNC50_IN_INV_SEL_V << GPIO_FUNC50_IN_INV_SEL_S)
#define GPIO_FUNC50_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC50_IN_INV_SEL_S  7
/** GPIO_SIG50_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG50_IN_SEL    (BIT(8))
#define GPIO_SIG50_IN_SEL_M  (GPIO_SIG50_IN_SEL_V << GPIO_SIG50_IN_SEL_S)
#define GPIO_SIG50_IN_SEL_V  0x00000001U
#define GPIO_SIG50_IN_SEL_S  8

/** GPIO_FUNC51_IN_SEL_CFG_REG register
 *  Configuration register for input signal 51
 */
#define GPIO_FUNC51_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x390)
/** GPIO_FUNC51_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 51.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC51_IN_SEL    0x0000007FU
#define GPIO_FUNC51_IN_SEL_M  (GPIO_FUNC51_IN_SEL_V << GPIO_FUNC51_IN_SEL_S)
#define GPIO_FUNC51_IN_SEL_V  0x0000007FU
#define GPIO_FUNC51_IN_SEL_S  0
/** GPIO_FUNC51_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC51_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC51_IN_INV_SEL_M  (GPIO_FUNC51_IN_INV_SEL_V << GPIO_FUNC51_IN_INV_SEL_S)
#define GPIO_FUNC51_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC51_IN_INV_SEL_S  7
/** GPIO_SIG51_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG51_IN_SEL    (BIT(8))
#define GPIO_SIG51_IN_SEL_M  (GPIO_SIG51_IN_SEL_V << GPIO_SIG51_IN_SEL_S)
#define GPIO_SIG51_IN_SEL_V  0x00000001U
#define GPIO_SIG51_IN_SEL_S  8

/** GPIO_FUNC52_IN_SEL_CFG_REG register
 *  Configuration register for input signal 52
 */
#define GPIO_FUNC52_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x394)
/** GPIO_FUNC52_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 52.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC52_IN_SEL    0x0000007FU
#define GPIO_FUNC52_IN_SEL_M  (GPIO_FUNC52_IN_SEL_V << GPIO_FUNC52_IN_SEL_S)
#define GPIO_FUNC52_IN_SEL_V  0x0000007FU
#define GPIO_FUNC52_IN_SEL_S  0
/** GPIO_FUNC52_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC52_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC52_IN_INV_SEL_M  (GPIO_FUNC52_IN_INV_SEL_V << GPIO_FUNC52_IN_INV_SEL_S)
#define GPIO_FUNC52_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC52_IN_INV_SEL_S  7
/** GPIO_SIG52_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG52_IN_SEL    (BIT(8))
#define GPIO_SIG52_IN_SEL_M  (GPIO_SIG52_IN_SEL_V << GPIO_SIG52_IN_SEL_S)
#define GPIO_SIG52_IN_SEL_V  0x00000001U
#define GPIO_SIG52_IN_SEL_S  8

/** GPIO_FUNC53_IN_SEL_CFG_REG register
 *  Configuration register for input signal 53
 */
#define GPIO_FUNC53_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x398)
/** GPIO_FUNC53_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 53.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC53_IN_SEL    0x0000007FU
#define GPIO_FUNC53_IN_SEL_M  (GPIO_FUNC53_IN_SEL_V << GPIO_FUNC53_IN_SEL_S)
#define GPIO_FUNC53_IN_SEL_V  0x0000007FU
#define GPIO_FUNC53_IN_SEL_S  0
/** GPIO_FUNC53_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC53_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC53_IN_INV_SEL_M  (GPIO_FUNC53_IN_INV_SEL_V << GPIO_FUNC53_IN_INV_SEL_S)
#define GPIO_FUNC53_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC53_IN_INV_SEL_S  7
/** GPIO_SIG53_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG53_IN_SEL    (BIT(8))
#define GPIO_SIG53_IN_SEL_M  (GPIO_SIG53_IN_SEL_V << GPIO_SIG53_IN_SEL_S)
#define GPIO_SIG53_IN_SEL_V  0x00000001U
#define GPIO_SIG53_IN_SEL_S  8

/** GPIO_FUNC54_IN_SEL_CFG_REG register
 *  Configuration register for input signal 54
 */
#define GPIO_FUNC54_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x39c)
/** GPIO_FUNC54_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 54.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC54_IN_SEL    0x0000007FU
#define GPIO_FUNC54_IN_SEL_M  (GPIO_FUNC54_IN_SEL_V << GPIO_FUNC54_IN_SEL_S)
#define GPIO_FUNC54_IN_SEL_V  0x0000007FU
#define GPIO_FUNC54_IN_SEL_S  0
/** GPIO_FUNC54_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC54_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC54_IN_INV_SEL_M  (GPIO_FUNC54_IN_INV_SEL_V << GPIO_FUNC54_IN_INV_SEL_S)
#define GPIO_FUNC54_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC54_IN_INV_SEL_S  7
/** GPIO_SIG54_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG54_IN_SEL    (BIT(8))
#define GPIO_SIG54_IN_SEL_M  (GPIO_SIG54_IN_SEL_V << GPIO_SIG54_IN_SEL_S)
#define GPIO_SIG54_IN_SEL_V  0x00000001U
#define GPIO_SIG54_IN_SEL_S  8

/** GPIO_FUNC55_IN_SEL_CFG_REG register
 *  Configuration register for input signal 55
 */
#define GPIO_FUNC55_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a0)
/** GPIO_FUNC55_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 55.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC55_IN_SEL    0x0000007FU
#define GPIO_FUNC55_IN_SEL_M  (GPIO_FUNC55_IN_SEL_V << GPIO_FUNC55_IN_SEL_S)
#define GPIO_FUNC55_IN_SEL_V  0x0000007FU
#define GPIO_FUNC55_IN_SEL_S  0
/** GPIO_FUNC55_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC55_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC55_IN_INV_SEL_M  (GPIO_FUNC55_IN_INV_SEL_V << GPIO_FUNC55_IN_INV_SEL_S)
#define GPIO_FUNC55_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC55_IN_INV_SEL_S  7
/** GPIO_SIG55_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG55_IN_SEL    (BIT(8))
#define GPIO_SIG55_IN_SEL_M  (GPIO_SIG55_IN_SEL_V << GPIO_SIG55_IN_SEL_S)
#define GPIO_SIG55_IN_SEL_V  0x00000001U
#define GPIO_SIG55_IN_SEL_S  8

/** GPIO_FUNC56_IN_SEL_CFG_REG register
 *  Configuration register for input signal 56
 */
#define GPIO_FUNC56_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a4)
/** GPIO_FUNC56_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 56.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC56_IN_SEL    0x0000007FU
#define GPIO_FUNC56_IN_SEL_M  (GPIO_FUNC56_IN_SEL_V << GPIO_FUNC56_IN_SEL_S)
#define GPIO_FUNC56_IN_SEL_V  0x0000007FU
#define GPIO_FUNC56_IN_SEL_S  0
/** GPIO_FUNC56_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC56_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC56_IN_INV_SEL_M  (GPIO_FUNC56_IN_INV_SEL_V << GPIO_FUNC56_IN_INV_SEL_S)
#define GPIO_FUNC56_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC56_IN_INV_SEL_S  7
/** GPIO_SIG56_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG56_IN_SEL    (BIT(8))
#define GPIO_SIG56_IN_SEL_M  (GPIO_SIG56_IN_SEL_V << GPIO_SIG56_IN_SEL_S)
#define GPIO_SIG56_IN_SEL_V  0x00000001U
#define GPIO_SIG56_IN_SEL_S  8

/** GPIO_FUNC57_IN_SEL_CFG_REG register
 *  Configuration register for input signal 57
 */
#define GPIO_FUNC57_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3a8)
/** GPIO_FUNC57_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 57.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC57_IN_SEL    0x0000007FU
#define GPIO_FUNC57_IN_SEL_M  (GPIO_FUNC57_IN_SEL_V << GPIO_FUNC57_IN_SEL_S)
#define GPIO_FUNC57_IN_SEL_V  0x0000007FU
#define GPIO_FUNC57_IN_SEL_S  0
/** GPIO_FUNC57_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC57_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC57_IN_INV_SEL_M  (GPIO_FUNC57_IN_INV_SEL_V << GPIO_FUNC57_IN_INV_SEL_S)
#define GPIO_FUNC57_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC57_IN_INV_SEL_S  7
/** GPIO_SIG57_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG57_IN_SEL    (BIT(8))
#define GPIO_SIG57_IN_SEL_M  (GPIO_SIG57_IN_SEL_V << GPIO_SIG57_IN_SEL_S)
#define GPIO_SIG57_IN_SEL_V  0x00000001U
#define GPIO_SIG57_IN_SEL_S  8

/** GPIO_FUNC58_IN_SEL_CFG_REG register
 *  Configuration register for input signal 58
 */
#define GPIO_FUNC58_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3ac)
/** GPIO_FUNC58_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 58.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC58_IN_SEL    0x0000007FU
#define GPIO_FUNC58_IN_SEL_M  (GPIO_FUNC58_IN_SEL_V << GPIO_FUNC58_IN_SEL_S)
#define GPIO_FUNC58_IN_SEL_V  0x0000007FU
#define GPIO_FUNC58_IN_SEL_S  0
/** GPIO_FUNC58_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC58_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC58_IN_INV_SEL_M  (GPIO_FUNC58_IN_INV_SEL_V << GPIO_FUNC58_IN_INV_SEL_S)
#define GPIO_FUNC58_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC58_IN_INV_SEL_S  7
/** GPIO_SIG58_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG58_IN_SEL    (BIT(8))
#define GPIO_SIG58_IN_SEL_M  (GPIO_SIG58_IN_SEL_V << GPIO_SIG58_IN_SEL_S)
#define GPIO_SIG58_IN_SEL_V  0x00000001U
#define GPIO_SIG58_IN_SEL_S  8

/** GPIO_FUNC59_IN_SEL_CFG_REG register
 *  Configuration register for input signal 59
 */
#define GPIO_FUNC59_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b0)
/** GPIO_FUNC59_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 59.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC59_IN_SEL    0x0000007FU
#define GPIO_FUNC59_IN_SEL_M  (GPIO_FUNC59_IN_SEL_V << GPIO_FUNC59_IN_SEL_S)
#define GPIO_FUNC59_IN_SEL_V  0x0000007FU
#define GPIO_FUNC59_IN_SEL_S  0
/** GPIO_FUNC59_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC59_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC59_IN_INV_SEL_M  (GPIO_FUNC59_IN_INV_SEL_V << GPIO_FUNC59_IN_INV_SEL_S)
#define GPIO_FUNC59_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC59_IN_INV_SEL_S  7
/** GPIO_SIG59_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG59_IN_SEL    (BIT(8))
#define GPIO_SIG59_IN_SEL_M  (GPIO_SIG59_IN_SEL_V << GPIO_SIG59_IN_SEL_S)
#define GPIO_SIG59_IN_SEL_V  0x00000001U
#define GPIO_SIG59_IN_SEL_S  8

/** GPIO_FUNC60_IN_SEL_CFG_REG register
 *  Configuration register for input signal 60
 */
#define GPIO_FUNC60_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b4)
/** GPIO_FUNC60_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 60.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC60_IN_SEL    0x0000007FU
#define GPIO_FUNC60_IN_SEL_M  (GPIO_FUNC60_IN_SEL_V << GPIO_FUNC60_IN_SEL_S)
#define GPIO_FUNC60_IN_SEL_V  0x0000007FU
#define GPIO_FUNC60_IN_SEL_S  0
/** GPIO_FUNC60_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC60_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC60_IN_INV_SEL_M  (GPIO_FUNC60_IN_INV_SEL_V << GPIO_FUNC60_IN_INV_SEL_S)
#define GPIO_FUNC60_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC60_IN_INV_SEL_S  7
/** GPIO_SIG60_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG60_IN_SEL    (BIT(8))
#define GPIO_SIG60_IN_SEL_M  (GPIO_SIG60_IN_SEL_V << GPIO_SIG60_IN_SEL_S)
#define GPIO_SIG60_IN_SEL_V  0x00000001U
#define GPIO_SIG60_IN_SEL_S  8

/** GPIO_FUNC61_IN_SEL_CFG_REG register
 *  Configuration register for input signal 61
 */
#define GPIO_FUNC61_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3b8)
/** GPIO_FUNC61_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 61.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC61_IN_SEL    0x0000007FU
#define GPIO_FUNC61_IN_SEL_M  (GPIO_FUNC61_IN_SEL_V << GPIO_FUNC61_IN_SEL_S)
#define GPIO_FUNC61_IN_SEL_V  0x0000007FU
#define GPIO_FUNC61_IN_SEL_S  0
/** GPIO_FUNC61_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC61_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC61_IN_INV_SEL_M  (GPIO_FUNC61_IN_INV_SEL_V << GPIO_FUNC61_IN_INV_SEL_S)
#define GPIO_FUNC61_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC61_IN_INV_SEL_S  7
/** GPIO_SIG61_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG61_IN_SEL    (BIT(8))
#define GPIO_SIG61_IN_SEL_M  (GPIO_SIG61_IN_SEL_V << GPIO_SIG61_IN_SEL_S)
#define GPIO_SIG61_IN_SEL_V  0x00000001U
#define GPIO_SIG61_IN_SEL_S  8

/** GPIO_FUNC62_IN_SEL_CFG_REG register
 *  Configuration register for input signal 62
 */
#define GPIO_FUNC62_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3bc)
/** GPIO_FUNC62_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 62.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC62_IN_SEL    0x0000007FU
#define GPIO_FUNC62_IN_SEL_M  (GPIO_FUNC62_IN_SEL_V << GPIO_FUNC62_IN_SEL_S)
#define GPIO_FUNC62_IN_SEL_V  0x0000007FU
#define GPIO_FUNC62_IN_SEL_S  0
/** GPIO_FUNC62_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC62_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC62_IN_INV_SEL_M  (GPIO_FUNC62_IN_INV_SEL_V << GPIO_FUNC62_IN_INV_SEL_S)
#define GPIO_FUNC62_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC62_IN_INV_SEL_S  7
/** GPIO_SIG62_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG62_IN_SEL    (BIT(8))
#define GPIO_SIG62_IN_SEL_M  (GPIO_SIG62_IN_SEL_V << GPIO_SIG62_IN_SEL_S)
#define GPIO_SIG62_IN_SEL_V  0x00000001U
#define GPIO_SIG62_IN_SEL_S  8

/** GPIO_FUNC63_IN_SEL_CFG_REG register
 *  Configuration register for input signal 63
 */
#define GPIO_FUNC63_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c0)
/** GPIO_FUNC63_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 63.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC63_IN_SEL    0x0000007FU
#define GPIO_FUNC63_IN_SEL_M  (GPIO_FUNC63_IN_SEL_V << GPIO_FUNC63_IN_SEL_S)
#define GPIO_FUNC63_IN_SEL_V  0x0000007FU
#define GPIO_FUNC63_IN_SEL_S  0
/** GPIO_FUNC63_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC63_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC63_IN_INV_SEL_M  (GPIO_FUNC63_IN_INV_SEL_V << GPIO_FUNC63_IN_INV_SEL_S)
#define GPIO_FUNC63_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC63_IN_INV_SEL_S  7
/** GPIO_SIG63_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG63_IN_SEL    (BIT(8))
#define GPIO_SIG63_IN_SEL_M  (GPIO_SIG63_IN_SEL_V << GPIO_SIG63_IN_SEL_S)
#define GPIO_SIG63_IN_SEL_V  0x00000001U
#define GPIO_SIG63_IN_SEL_S  8

/** GPIO_FUNC64_IN_SEL_CFG_REG register
 *  Configuration register for input signal 64
 */
#define GPIO_FUNC64_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c4)
/** GPIO_FUNC64_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 64.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC64_IN_SEL    0x0000007FU
#define GPIO_FUNC64_IN_SEL_M  (GPIO_FUNC64_IN_SEL_V << GPIO_FUNC64_IN_SEL_S)
#define GPIO_FUNC64_IN_SEL_V  0x0000007FU
#define GPIO_FUNC64_IN_SEL_S  0
/** GPIO_FUNC64_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC64_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC64_IN_INV_SEL_M  (GPIO_FUNC64_IN_INV_SEL_V << GPIO_FUNC64_IN_INV_SEL_S)
#define GPIO_FUNC64_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC64_IN_INV_SEL_S  7
/** GPIO_SIG64_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG64_IN_SEL    (BIT(8))
#define GPIO_SIG64_IN_SEL_M  (GPIO_SIG64_IN_SEL_V << GPIO_SIG64_IN_SEL_S)
#define GPIO_SIG64_IN_SEL_V  0x00000001U
#define GPIO_SIG64_IN_SEL_S  8

/** GPIO_FUNC65_IN_SEL_CFG_REG register
 *  Configuration register for input signal 65
 */
#define GPIO_FUNC65_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c8)
/** GPIO_FUNC65_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 65.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC65_IN_SEL    0x0000007FU
#define GPIO_FUNC65_IN_SEL_M  (GPIO_FUNC65_IN_SEL_V << GPIO_FUNC65_IN_SEL_S)
#define GPIO_FUNC65_IN_SEL_V  0x0000007FU
#define GPIO_FUNC65_IN_SEL_S  0
/** GPIO_FUNC65_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC65_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC65_IN_INV_SEL_M  (GPIO_FUNC65_IN_INV_SEL_V << GPIO_FUNC65_IN_INV_SEL_S)
#define GPIO_FUNC65_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC65_IN_INV_SEL_S  7
/** GPIO_SIG65_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG65_IN_SEL    (BIT(8))
#define GPIO_SIG65_IN_SEL_M  (GPIO_SIG65_IN_SEL_V << GPIO_SIG65_IN_SEL_S)
#define GPIO_SIG65_IN_SEL_V  0x00000001U
#define GPIO_SIG65_IN_SEL_S  8

/** GPIO_FUNC66_IN_SEL_CFG_REG register
 *  Configuration register for input signal 66
 */
#define GPIO_FUNC66_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3cc)
/** GPIO_FUNC66_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 66.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC66_IN_SEL    0x0000007FU
#define GPIO_FUNC66_IN_SEL_M  (GPIO_FUNC66_IN_SEL_V << GPIO_FUNC66_IN_SEL_S)
#define GPIO_FUNC66_IN_SEL_V  0x0000007FU
#define GPIO_FUNC66_IN_SEL_S  0
/** GPIO_FUNC66_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC66_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC66_IN_INV_SEL_M  (GPIO_FUNC66_IN_INV_SEL_V << GPIO_FUNC66_IN_INV_SEL_S)
#define GPIO_FUNC66_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC66_IN_INV_SEL_S  7
/** GPIO_SIG66_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG66_IN_SEL    (BIT(8))
#define GPIO_SIG66_IN_SEL_M  (GPIO_SIG66_IN_SEL_V << GPIO_SIG66_IN_SEL_S)
#define GPIO_SIG66_IN_SEL_V  0x00000001U
#define GPIO_SIG66_IN_SEL_S  8

/** GPIO_FUNC70_IN_SEL_CFG_REG register
 *  Configuration register for input signal 70
 */
#define GPIO_FUNC70_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3dc)
/** GPIO_FUNC70_IN_SEL : R/W; bitpos: [6:0]; default: 64;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 70.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC70_IN_SEL    0x0000007FU
#define GPIO_FUNC70_IN_SEL_M  (GPIO_FUNC70_IN_SEL_V << GPIO_FUNC70_IN_SEL_S)
#define GPIO_FUNC70_IN_SEL_V  0x0000007FU
#define GPIO_FUNC70_IN_SEL_S  0
/** GPIO_FUNC70_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC70_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC70_IN_INV_SEL_M  (GPIO_FUNC70_IN_INV_SEL_V << GPIO_FUNC70_IN_INV_SEL_S)
#define GPIO_FUNC70_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC70_IN_INV_SEL_S  7
/** GPIO_SIG70_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG70_IN_SEL    (BIT(8))
#define GPIO_SIG70_IN_SEL_M  (GPIO_SIG70_IN_SEL_V << GPIO_SIG70_IN_SEL_S)
#define GPIO_SIG70_IN_SEL_V  0x00000001U
#define GPIO_SIG70_IN_SEL_S  8

/** GPIO_FUNC74_IN_SEL_CFG_REG register
 *  Configuration register for input signal 74
 */
#define GPIO_FUNC74_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3ec)
/** GPIO_FUNC74_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 74.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC74_IN_SEL    0x0000007FU
#define GPIO_FUNC74_IN_SEL_M  (GPIO_FUNC74_IN_SEL_V << GPIO_FUNC74_IN_SEL_S)
#define GPIO_FUNC74_IN_SEL_V  0x0000007FU
#define GPIO_FUNC74_IN_SEL_S  0
/** GPIO_FUNC74_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC74_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC74_IN_INV_SEL_M  (GPIO_FUNC74_IN_INV_SEL_V << GPIO_FUNC74_IN_INV_SEL_S)
#define GPIO_FUNC74_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC74_IN_INV_SEL_S  7
/** GPIO_SIG74_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG74_IN_SEL    (BIT(8))
#define GPIO_SIG74_IN_SEL_M  (GPIO_SIG74_IN_SEL_V << GPIO_SIG74_IN_SEL_S)
#define GPIO_SIG74_IN_SEL_V  0x00000001U
#define GPIO_SIG74_IN_SEL_S  8

/** GPIO_FUNC75_IN_SEL_CFG_REG register
 *  Configuration register for input signal 75
 */
#define GPIO_FUNC75_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f0)
/** GPIO_FUNC75_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 75.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC75_IN_SEL    0x0000007FU
#define GPIO_FUNC75_IN_SEL_M  (GPIO_FUNC75_IN_SEL_V << GPIO_FUNC75_IN_SEL_S)
#define GPIO_FUNC75_IN_SEL_V  0x0000007FU
#define GPIO_FUNC75_IN_SEL_S  0
/** GPIO_FUNC75_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC75_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC75_IN_INV_SEL_M  (GPIO_FUNC75_IN_INV_SEL_V << GPIO_FUNC75_IN_INV_SEL_S)
#define GPIO_FUNC75_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC75_IN_INV_SEL_S  7
/** GPIO_SIG75_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG75_IN_SEL    (BIT(8))
#define GPIO_SIG75_IN_SEL_M  (GPIO_SIG75_IN_SEL_V << GPIO_SIG75_IN_SEL_S)
#define GPIO_SIG75_IN_SEL_V  0x00000001U
#define GPIO_SIG75_IN_SEL_S  8

/** GPIO_FUNC76_IN_SEL_CFG_REG register
 *  Configuration register for input signal 76
 */
#define GPIO_FUNC76_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f4)
/** GPIO_FUNC76_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 76.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC76_IN_SEL    0x0000007FU
#define GPIO_FUNC76_IN_SEL_M  (GPIO_FUNC76_IN_SEL_V << GPIO_FUNC76_IN_SEL_S)
#define GPIO_FUNC76_IN_SEL_V  0x0000007FU
#define GPIO_FUNC76_IN_SEL_S  0
/** GPIO_FUNC76_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC76_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC76_IN_INV_SEL_M  (GPIO_FUNC76_IN_INV_SEL_V << GPIO_FUNC76_IN_INV_SEL_S)
#define GPIO_FUNC76_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC76_IN_INV_SEL_S  7
/** GPIO_SIG76_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG76_IN_SEL    (BIT(8))
#define GPIO_SIG76_IN_SEL_M  (GPIO_SIG76_IN_SEL_V << GPIO_SIG76_IN_SEL_S)
#define GPIO_SIG76_IN_SEL_V  0x00000001U
#define GPIO_SIG76_IN_SEL_S  8

/** GPIO_FUNC77_IN_SEL_CFG_REG register
 *  Configuration register for input signal 77
 */
#define GPIO_FUNC77_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3f8)
/** GPIO_FUNC77_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 77.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC77_IN_SEL    0x0000007FU
#define GPIO_FUNC77_IN_SEL_M  (GPIO_FUNC77_IN_SEL_V << GPIO_FUNC77_IN_SEL_S)
#define GPIO_FUNC77_IN_SEL_V  0x0000007FU
#define GPIO_FUNC77_IN_SEL_S  0
/** GPIO_FUNC77_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC77_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC77_IN_INV_SEL_M  (GPIO_FUNC77_IN_INV_SEL_V << GPIO_FUNC77_IN_INV_SEL_S)
#define GPIO_FUNC77_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC77_IN_INV_SEL_S  7
/** GPIO_SIG77_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG77_IN_SEL    (BIT(8))
#define GPIO_SIG77_IN_SEL_M  (GPIO_SIG77_IN_SEL_V << GPIO_SIG77_IN_SEL_S)
#define GPIO_SIG77_IN_SEL_V  0x00000001U
#define GPIO_SIG77_IN_SEL_S  8

/** GPIO_FUNC78_IN_SEL_CFG_REG register
 *  Configuration register for input signal 78
 */
#define GPIO_FUNC78_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3fc)
/** GPIO_FUNC78_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 78.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC78_IN_SEL    0x0000007FU
#define GPIO_FUNC78_IN_SEL_M  (GPIO_FUNC78_IN_SEL_V << GPIO_FUNC78_IN_SEL_S)
#define GPIO_FUNC78_IN_SEL_V  0x0000007FU
#define GPIO_FUNC78_IN_SEL_S  0
/** GPIO_FUNC78_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC78_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC78_IN_INV_SEL_M  (GPIO_FUNC78_IN_INV_SEL_V << GPIO_FUNC78_IN_INV_SEL_S)
#define GPIO_FUNC78_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC78_IN_INV_SEL_S  7
/** GPIO_SIG78_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG78_IN_SEL    (BIT(8))
#define GPIO_SIG78_IN_SEL_M  (GPIO_SIG78_IN_SEL_V << GPIO_SIG78_IN_SEL_S)
#define GPIO_SIG78_IN_SEL_V  0x00000001U
#define GPIO_SIG78_IN_SEL_S  8

/** GPIO_FUNC79_IN_SEL_CFG_REG register
 *  Configuration register for input signal 79
 */
#define GPIO_FUNC79_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x400)
/** GPIO_FUNC79_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 79.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC79_IN_SEL    0x0000007FU
#define GPIO_FUNC79_IN_SEL_M  (GPIO_FUNC79_IN_SEL_V << GPIO_FUNC79_IN_SEL_S)
#define GPIO_FUNC79_IN_SEL_V  0x0000007FU
#define GPIO_FUNC79_IN_SEL_S  0
/** GPIO_FUNC79_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC79_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC79_IN_INV_SEL_M  (GPIO_FUNC79_IN_INV_SEL_V << GPIO_FUNC79_IN_INV_SEL_S)
#define GPIO_FUNC79_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC79_IN_INV_SEL_S  7
/** GPIO_SIG79_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG79_IN_SEL    (BIT(8))
#define GPIO_SIG79_IN_SEL_M  (GPIO_SIG79_IN_SEL_V << GPIO_SIG79_IN_SEL_S)
#define GPIO_SIG79_IN_SEL_V  0x00000001U
#define GPIO_SIG79_IN_SEL_S  8

/** GPIO_FUNC80_IN_SEL_CFG_REG register
 *  Configuration register for input signal 80
 */
#define GPIO_FUNC80_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x404)
/** GPIO_FUNC80_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 80.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC80_IN_SEL    0x0000007FU
#define GPIO_FUNC80_IN_SEL_M  (GPIO_FUNC80_IN_SEL_V << GPIO_FUNC80_IN_SEL_S)
#define GPIO_FUNC80_IN_SEL_V  0x0000007FU
#define GPIO_FUNC80_IN_SEL_S  0
/** GPIO_FUNC80_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC80_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC80_IN_INV_SEL_M  (GPIO_FUNC80_IN_INV_SEL_V << GPIO_FUNC80_IN_INV_SEL_S)
#define GPIO_FUNC80_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC80_IN_INV_SEL_S  7
/** GPIO_SIG80_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG80_IN_SEL    (BIT(8))
#define GPIO_SIG80_IN_SEL_M  (GPIO_SIG80_IN_SEL_V << GPIO_SIG80_IN_SEL_S)
#define GPIO_SIG80_IN_SEL_V  0x00000001U
#define GPIO_SIG80_IN_SEL_S  8

/** GPIO_FUNC81_IN_SEL_CFG_REG register
 *  Configuration register for input signal 81
 */
#define GPIO_FUNC81_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x408)
/** GPIO_FUNC81_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 81.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC81_IN_SEL    0x0000007FU
#define GPIO_FUNC81_IN_SEL_M  (GPIO_FUNC81_IN_SEL_V << GPIO_FUNC81_IN_SEL_S)
#define GPIO_FUNC81_IN_SEL_V  0x0000007FU
#define GPIO_FUNC81_IN_SEL_S  0
/** GPIO_FUNC81_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC81_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC81_IN_INV_SEL_M  (GPIO_FUNC81_IN_INV_SEL_V << GPIO_FUNC81_IN_INV_SEL_S)
#define GPIO_FUNC81_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC81_IN_INV_SEL_S  7
/** GPIO_SIG81_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG81_IN_SEL    (BIT(8))
#define GPIO_SIG81_IN_SEL_M  (GPIO_SIG81_IN_SEL_V << GPIO_SIG81_IN_SEL_S)
#define GPIO_SIG81_IN_SEL_V  0x00000001U
#define GPIO_SIG81_IN_SEL_S  8

/** GPIO_FUNC82_IN_SEL_CFG_REG register
 *  Configuration register for input signal 82
 */
#define GPIO_FUNC82_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x40c)
/** GPIO_FUNC82_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 82.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC82_IN_SEL    0x0000007FU
#define GPIO_FUNC82_IN_SEL_M  (GPIO_FUNC82_IN_SEL_V << GPIO_FUNC82_IN_SEL_S)
#define GPIO_FUNC82_IN_SEL_V  0x0000007FU
#define GPIO_FUNC82_IN_SEL_S  0
/** GPIO_FUNC82_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC82_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC82_IN_INV_SEL_M  (GPIO_FUNC82_IN_INV_SEL_V << GPIO_FUNC82_IN_INV_SEL_S)
#define GPIO_FUNC82_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC82_IN_INV_SEL_S  7
/** GPIO_SIG82_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG82_IN_SEL    (BIT(8))
#define GPIO_SIG82_IN_SEL_M  (GPIO_SIG82_IN_SEL_V << GPIO_SIG82_IN_SEL_S)
#define GPIO_SIG82_IN_SEL_V  0x00000001U
#define GPIO_SIG82_IN_SEL_S  8

/** GPIO_FUNC83_IN_SEL_CFG_REG register
 *  Configuration register for input signal 83
 */
#define GPIO_FUNC83_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x410)
/** GPIO_FUNC83_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 83.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC83_IN_SEL    0x0000007FU
#define GPIO_FUNC83_IN_SEL_M  (GPIO_FUNC83_IN_SEL_V << GPIO_FUNC83_IN_SEL_S)
#define GPIO_FUNC83_IN_SEL_V  0x0000007FU
#define GPIO_FUNC83_IN_SEL_S  0
/** GPIO_FUNC83_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC83_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC83_IN_INV_SEL_M  (GPIO_FUNC83_IN_INV_SEL_V << GPIO_FUNC83_IN_INV_SEL_S)
#define GPIO_FUNC83_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC83_IN_INV_SEL_S  7
/** GPIO_SIG83_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG83_IN_SEL    (BIT(8))
#define GPIO_SIG83_IN_SEL_M  (GPIO_SIG83_IN_SEL_V << GPIO_SIG83_IN_SEL_S)
#define GPIO_SIG83_IN_SEL_V  0x00000001U
#define GPIO_SIG83_IN_SEL_S  8

/** GPIO_FUNC84_IN_SEL_CFG_REG register
 *  Configuration register for input signal 84
 */
#define GPIO_FUNC84_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x414)
/** GPIO_FUNC84_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 84.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC84_IN_SEL    0x0000007FU
#define GPIO_FUNC84_IN_SEL_M  (GPIO_FUNC84_IN_SEL_V << GPIO_FUNC84_IN_SEL_S)
#define GPIO_FUNC84_IN_SEL_V  0x0000007FU
#define GPIO_FUNC84_IN_SEL_S  0
/** GPIO_FUNC84_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC84_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC84_IN_INV_SEL_M  (GPIO_FUNC84_IN_INV_SEL_V << GPIO_FUNC84_IN_INV_SEL_S)
#define GPIO_FUNC84_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC84_IN_INV_SEL_S  7
/** GPIO_SIG84_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG84_IN_SEL    (BIT(8))
#define GPIO_SIG84_IN_SEL_M  (GPIO_SIG84_IN_SEL_V << GPIO_SIG84_IN_SEL_S)
#define GPIO_SIG84_IN_SEL_V  0x00000001U
#define GPIO_SIG84_IN_SEL_S  8

/** GPIO_FUNC85_IN_SEL_CFG_REG register
 *  Configuration register for input signal 85
 */
#define GPIO_FUNC85_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x418)
/** GPIO_FUNC85_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 85.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC85_IN_SEL    0x0000007FU
#define GPIO_FUNC85_IN_SEL_M  (GPIO_FUNC85_IN_SEL_V << GPIO_FUNC85_IN_SEL_S)
#define GPIO_FUNC85_IN_SEL_V  0x0000007FU
#define GPIO_FUNC85_IN_SEL_S  0
/** GPIO_FUNC85_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC85_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC85_IN_INV_SEL_M  (GPIO_FUNC85_IN_INV_SEL_V << GPIO_FUNC85_IN_INV_SEL_S)
#define GPIO_FUNC85_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC85_IN_INV_SEL_S  7
/** GPIO_SIG85_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG85_IN_SEL    (BIT(8))
#define GPIO_SIG85_IN_SEL_M  (GPIO_SIG85_IN_SEL_V << GPIO_SIG85_IN_SEL_S)
#define GPIO_SIG85_IN_SEL_V  0x00000001U
#define GPIO_SIG85_IN_SEL_S  8

/** GPIO_FUNC86_IN_SEL_CFG_REG register
 *  Configuration register for input signal 86
 */
#define GPIO_FUNC86_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x41c)
/** GPIO_FUNC86_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 86.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC86_IN_SEL    0x0000007FU
#define GPIO_FUNC86_IN_SEL_M  (GPIO_FUNC86_IN_SEL_V << GPIO_FUNC86_IN_SEL_S)
#define GPIO_FUNC86_IN_SEL_V  0x0000007FU
#define GPIO_FUNC86_IN_SEL_S  0
/** GPIO_FUNC86_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC86_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC86_IN_INV_SEL_M  (GPIO_FUNC86_IN_INV_SEL_V << GPIO_FUNC86_IN_INV_SEL_S)
#define GPIO_FUNC86_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC86_IN_INV_SEL_S  7
/** GPIO_SIG86_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG86_IN_SEL    (BIT(8))
#define GPIO_SIG86_IN_SEL_M  (GPIO_SIG86_IN_SEL_V << GPIO_SIG86_IN_SEL_S)
#define GPIO_SIG86_IN_SEL_V  0x00000001U
#define GPIO_SIG86_IN_SEL_S  8

/** GPIO_FUNC87_IN_SEL_CFG_REG register
 *  Configuration register for input signal 87
 */
#define GPIO_FUNC87_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x420)
/** GPIO_FUNC87_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 87.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC87_IN_SEL    0x0000007FU
#define GPIO_FUNC87_IN_SEL_M  (GPIO_FUNC87_IN_SEL_V << GPIO_FUNC87_IN_SEL_S)
#define GPIO_FUNC87_IN_SEL_V  0x0000007FU
#define GPIO_FUNC87_IN_SEL_S  0
/** GPIO_FUNC87_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC87_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC87_IN_INV_SEL_M  (GPIO_FUNC87_IN_INV_SEL_V << GPIO_FUNC87_IN_INV_SEL_S)
#define GPIO_FUNC87_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC87_IN_INV_SEL_S  7
/** GPIO_SIG87_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG87_IN_SEL    (BIT(8))
#define GPIO_SIG87_IN_SEL_M  (GPIO_SIG87_IN_SEL_V << GPIO_SIG87_IN_SEL_S)
#define GPIO_SIG87_IN_SEL_V  0x00000001U
#define GPIO_SIG87_IN_SEL_S  8

/** GPIO_FUNC88_IN_SEL_CFG_REG register
 *  Configuration register for input signal 88
 */
#define GPIO_FUNC88_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x424)
/** GPIO_FUNC88_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 88.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC88_IN_SEL    0x0000007FU
#define GPIO_FUNC88_IN_SEL_M  (GPIO_FUNC88_IN_SEL_V << GPIO_FUNC88_IN_SEL_S)
#define GPIO_FUNC88_IN_SEL_V  0x0000007FU
#define GPIO_FUNC88_IN_SEL_S  0
/** GPIO_FUNC88_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC88_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC88_IN_INV_SEL_M  (GPIO_FUNC88_IN_INV_SEL_V << GPIO_FUNC88_IN_INV_SEL_S)
#define GPIO_FUNC88_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC88_IN_INV_SEL_S  7
/** GPIO_SIG88_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG88_IN_SEL    (BIT(8))
#define GPIO_SIG88_IN_SEL_M  (GPIO_SIG88_IN_SEL_V << GPIO_SIG88_IN_SEL_S)
#define GPIO_SIG88_IN_SEL_V  0x00000001U
#define GPIO_SIG88_IN_SEL_S  8

/** GPIO_FUNC89_IN_SEL_CFG_REG register
 *  Configuration register for input signal 89
 */
#define GPIO_FUNC89_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x428)
/** GPIO_FUNC89_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 89.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC89_IN_SEL    0x0000007FU
#define GPIO_FUNC89_IN_SEL_M  (GPIO_FUNC89_IN_SEL_V << GPIO_FUNC89_IN_SEL_S)
#define GPIO_FUNC89_IN_SEL_V  0x0000007FU
#define GPIO_FUNC89_IN_SEL_S  0
/** GPIO_FUNC89_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC89_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC89_IN_INV_SEL_M  (GPIO_FUNC89_IN_INV_SEL_V << GPIO_FUNC89_IN_INV_SEL_S)
#define GPIO_FUNC89_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC89_IN_INV_SEL_S  7
/** GPIO_SIG89_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG89_IN_SEL    (BIT(8))
#define GPIO_SIG89_IN_SEL_M  (GPIO_SIG89_IN_SEL_V << GPIO_SIG89_IN_SEL_S)
#define GPIO_SIG89_IN_SEL_V  0x00000001U
#define GPIO_SIG89_IN_SEL_S  8

/** GPIO_FUNC90_IN_SEL_CFG_REG register
 *  Configuration register for input signal 90
 */
#define GPIO_FUNC90_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x42c)
/** GPIO_FUNC90_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 90.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC90_IN_SEL    0x0000007FU
#define GPIO_FUNC90_IN_SEL_M  (GPIO_FUNC90_IN_SEL_V << GPIO_FUNC90_IN_SEL_S)
#define GPIO_FUNC90_IN_SEL_V  0x0000007FU
#define GPIO_FUNC90_IN_SEL_S  0
/** GPIO_FUNC90_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC90_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC90_IN_INV_SEL_M  (GPIO_FUNC90_IN_INV_SEL_V << GPIO_FUNC90_IN_INV_SEL_S)
#define GPIO_FUNC90_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC90_IN_INV_SEL_S  7
/** GPIO_SIG90_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG90_IN_SEL    (BIT(8))
#define GPIO_SIG90_IN_SEL_M  (GPIO_SIG90_IN_SEL_V << GPIO_SIG90_IN_SEL_S)
#define GPIO_SIG90_IN_SEL_V  0x00000001U
#define GPIO_SIG90_IN_SEL_S  8

/** GPIO_FUNC91_IN_SEL_CFG_REG register
 *  Configuration register for input signal 91
 */
#define GPIO_FUNC91_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x430)
/** GPIO_FUNC91_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 91.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC91_IN_SEL    0x0000007FU
#define GPIO_FUNC91_IN_SEL_M  (GPIO_FUNC91_IN_SEL_V << GPIO_FUNC91_IN_SEL_S)
#define GPIO_FUNC91_IN_SEL_V  0x0000007FU
#define GPIO_FUNC91_IN_SEL_S  0
/** GPIO_FUNC91_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC91_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC91_IN_INV_SEL_M  (GPIO_FUNC91_IN_INV_SEL_V << GPIO_FUNC91_IN_INV_SEL_S)
#define GPIO_FUNC91_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC91_IN_INV_SEL_S  7
/** GPIO_SIG91_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG91_IN_SEL    (BIT(8))
#define GPIO_SIG91_IN_SEL_M  (GPIO_SIG91_IN_SEL_V << GPIO_SIG91_IN_SEL_S)
#define GPIO_SIG91_IN_SEL_V  0x00000001U
#define GPIO_SIG91_IN_SEL_S  8

/** GPIO_FUNC92_IN_SEL_CFG_REG register
 *  Configuration register for input signal 92
 */
#define GPIO_FUNC92_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x434)
/** GPIO_FUNC92_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 92.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC92_IN_SEL    0x0000007FU
#define GPIO_FUNC92_IN_SEL_M  (GPIO_FUNC92_IN_SEL_V << GPIO_FUNC92_IN_SEL_S)
#define GPIO_FUNC92_IN_SEL_V  0x0000007FU
#define GPIO_FUNC92_IN_SEL_S  0
/** GPIO_FUNC92_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC92_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC92_IN_INV_SEL_M  (GPIO_FUNC92_IN_INV_SEL_V << GPIO_FUNC92_IN_INV_SEL_S)
#define GPIO_FUNC92_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC92_IN_INV_SEL_S  7
/** GPIO_SIG92_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG92_IN_SEL    (BIT(8))
#define GPIO_SIG92_IN_SEL_M  (GPIO_SIG92_IN_SEL_V << GPIO_SIG92_IN_SEL_S)
#define GPIO_SIG92_IN_SEL_V  0x00000001U
#define GPIO_SIG92_IN_SEL_S  8

/** GPIO_FUNC97_IN_SEL_CFG_REG register
 *  Configuration register for input signal 97
 */
#define GPIO_FUNC97_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x448)
/** GPIO_FUNC97_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 97.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC97_IN_SEL    0x0000007FU
#define GPIO_FUNC97_IN_SEL_M  (GPIO_FUNC97_IN_SEL_V << GPIO_FUNC97_IN_SEL_S)
#define GPIO_FUNC97_IN_SEL_V  0x0000007FU
#define GPIO_FUNC97_IN_SEL_S  0
/** GPIO_FUNC97_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC97_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC97_IN_INV_SEL_M  (GPIO_FUNC97_IN_INV_SEL_V << GPIO_FUNC97_IN_INV_SEL_S)
#define GPIO_FUNC97_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC97_IN_INV_SEL_S  7
/** GPIO_SIG97_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG97_IN_SEL    (BIT(8))
#define GPIO_SIG97_IN_SEL_M  (GPIO_SIG97_IN_SEL_V << GPIO_SIG97_IN_SEL_S)
#define GPIO_SIG97_IN_SEL_V  0x00000001U
#define GPIO_SIG97_IN_SEL_S  8

/** GPIO_FUNC98_IN_SEL_CFG_REG register
 *  Configuration register for input signal 98
 */
#define GPIO_FUNC98_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x44c)
/** GPIO_FUNC98_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 98.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC98_IN_SEL    0x0000007FU
#define GPIO_FUNC98_IN_SEL_M  (GPIO_FUNC98_IN_SEL_V << GPIO_FUNC98_IN_SEL_S)
#define GPIO_FUNC98_IN_SEL_V  0x0000007FU
#define GPIO_FUNC98_IN_SEL_S  0
/** GPIO_FUNC98_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC98_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC98_IN_INV_SEL_M  (GPIO_FUNC98_IN_INV_SEL_V << GPIO_FUNC98_IN_INV_SEL_S)
#define GPIO_FUNC98_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC98_IN_INV_SEL_S  7
/** GPIO_SIG98_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG98_IN_SEL    (BIT(8))
#define GPIO_SIG98_IN_SEL_M  (GPIO_SIG98_IN_SEL_V << GPIO_SIG98_IN_SEL_S)
#define GPIO_SIG98_IN_SEL_V  0x00000001U
#define GPIO_SIG98_IN_SEL_S  8

/** GPIO_FUNC99_IN_SEL_CFG_REG register
 *  Configuration register for input signal 99
 */
#define GPIO_FUNC99_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x450)
/** GPIO_FUNC99_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 99.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC99_IN_SEL    0x0000007FU
#define GPIO_FUNC99_IN_SEL_M  (GPIO_FUNC99_IN_SEL_V << GPIO_FUNC99_IN_SEL_S)
#define GPIO_FUNC99_IN_SEL_V  0x0000007FU
#define GPIO_FUNC99_IN_SEL_S  0
/** GPIO_FUNC99_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC99_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC99_IN_INV_SEL_M  (GPIO_FUNC99_IN_INV_SEL_V << GPIO_FUNC99_IN_INV_SEL_S)
#define GPIO_FUNC99_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC99_IN_INV_SEL_S  7
/** GPIO_SIG99_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG99_IN_SEL    (BIT(8))
#define GPIO_SIG99_IN_SEL_M  (GPIO_SIG99_IN_SEL_V << GPIO_SIG99_IN_SEL_S)
#define GPIO_SIG99_IN_SEL_V  0x00000001U
#define GPIO_SIG99_IN_SEL_S  8

/** GPIO_FUNC100_IN_SEL_CFG_REG register
 *  Configuration register for input signal 100
 */
#define GPIO_FUNC100_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x454)
/** GPIO_FUNC100_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 100.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC100_IN_SEL    0x0000007FU
#define GPIO_FUNC100_IN_SEL_M  (GPIO_FUNC100_IN_SEL_V << GPIO_FUNC100_IN_SEL_S)
#define GPIO_FUNC100_IN_SEL_V  0x0000007FU
#define GPIO_FUNC100_IN_SEL_S  0
/** GPIO_FUNC100_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC100_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC100_IN_INV_SEL_M  (GPIO_FUNC100_IN_INV_SEL_V << GPIO_FUNC100_IN_INV_SEL_S)
#define GPIO_FUNC100_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC100_IN_INV_SEL_S  7
/** GPIO_SIG100_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG100_IN_SEL    (BIT(8))
#define GPIO_SIG100_IN_SEL_M  (GPIO_SIG100_IN_SEL_V << GPIO_SIG100_IN_SEL_S)
#define GPIO_SIG100_IN_SEL_V  0x00000001U
#define GPIO_SIG100_IN_SEL_S  8

/** GPIO_FUNC101_IN_SEL_CFG_REG register
 *  Configuration register for input signal 101
 */
#define GPIO_FUNC101_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x458)
/** GPIO_FUNC101_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 101.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC101_IN_SEL    0x0000007FU
#define GPIO_FUNC101_IN_SEL_M  (GPIO_FUNC101_IN_SEL_V << GPIO_FUNC101_IN_SEL_S)
#define GPIO_FUNC101_IN_SEL_V  0x0000007FU
#define GPIO_FUNC101_IN_SEL_S  0
/** GPIO_FUNC101_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC101_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC101_IN_INV_SEL_M  (GPIO_FUNC101_IN_INV_SEL_V << GPIO_FUNC101_IN_INV_SEL_S)
#define GPIO_FUNC101_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC101_IN_INV_SEL_S  7
/** GPIO_SIG101_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG101_IN_SEL    (BIT(8))
#define GPIO_SIG101_IN_SEL_M  (GPIO_SIG101_IN_SEL_V << GPIO_SIG101_IN_SEL_S)
#define GPIO_SIG101_IN_SEL_V  0x00000001U
#define GPIO_SIG101_IN_SEL_S  8

/** GPIO_FUNC102_IN_SEL_CFG_REG register
 *  Configuration register for input signal 102
 */
#define GPIO_FUNC102_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x45c)
/** GPIO_FUNC102_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 102.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC102_IN_SEL    0x0000007FU
#define GPIO_FUNC102_IN_SEL_M  (GPIO_FUNC102_IN_SEL_V << GPIO_FUNC102_IN_SEL_S)
#define GPIO_FUNC102_IN_SEL_V  0x0000007FU
#define GPIO_FUNC102_IN_SEL_S  0
/** GPIO_FUNC102_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC102_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC102_IN_INV_SEL_M  (GPIO_FUNC102_IN_INV_SEL_V << GPIO_FUNC102_IN_INV_SEL_S)
#define GPIO_FUNC102_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC102_IN_INV_SEL_S  7
/** GPIO_SIG102_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG102_IN_SEL    (BIT(8))
#define GPIO_SIG102_IN_SEL_M  (GPIO_SIG102_IN_SEL_V << GPIO_SIG102_IN_SEL_S)
#define GPIO_SIG102_IN_SEL_V  0x00000001U
#define GPIO_SIG102_IN_SEL_S  8

/** GPIO_FUNC103_IN_SEL_CFG_REG register
 *  Configuration register for input signal 103
 */
#define GPIO_FUNC103_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x460)
/** GPIO_FUNC103_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 103.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC103_IN_SEL    0x0000007FU
#define GPIO_FUNC103_IN_SEL_M  (GPIO_FUNC103_IN_SEL_V << GPIO_FUNC103_IN_SEL_S)
#define GPIO_FUNC103_IN_SEL_V  0x0000007FU
#define GPIO_FUNC103_IN_SEL_S  0
/** GPIO_FUNC103_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC103_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC103_IN_INV_SEL_M  (GPIO_FUNC103_IN_INV_SEL_V << GPIO_FUNC103_IN_INV_SEL_S)
#define GPIO_FUNC103_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC103_IN_INV_SEL_S  7
/** GPIO_SIG103_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG103_IN_SEL    (BIT(8))
#define GPIO_SIG103_IN_SEL_M  (GPIO_SIG103_IN_SEL_V << GPIO_SIG103_IN_SEL_S)
#define GPIO_SIG103_IN_SEL_V  0x00000001U
#define GPIO_SIG103_IN_SEL_S  8

/** GPIO_FUNC104_IN_SEL_CFG_REG register
 *  Configuration register for input signal 104
 */
#define GPIO_FUNC104_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x464)
/** GPIO_FUNC104_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 104.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC104_IN_SEL    0x0000007FU
#define GPIO_FUNC104_IN_SEL_M  (GPIO_FUNC104_IN_SEL_V << GPIO_FUNC104_IN_SEL_S)
#define GPIO_FUNC104_IN_SEL_V  0x0000007FU
#define GPIO_FUNC104_IN_SEL_S  0
/** GPIO_FUNC104_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC104_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC104_IN_INV_SEL_M  (GPIO_FUNC104_IN_INV_SEL_V << GPIO_FUNC104_IN_INV_SEL_S)
#define GPIO_FUNC104_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC104_IN_INV_SEL_S  7
/** GPIO_SIG104_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG104_IN_SEL    (BIT(8))
#define GPIO_SIG104_IN_SEL_M  (GPIO_SIG104_IN_SEL_V << GPIO_SIG104_IN_SEL_S)
#define GPIO_SIG104_IN_SEL_V  0x00000001U
#define GPIO_SIG104_IN_SEL_S  8

/** GPIO_FUNC105_IN_SEL_CFG_REG register
 *  Configuration register for input signal 105
 */
#define GPIO_FUNC105_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x468)
/** GPIO_FUNC105_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 105.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC105_IN_SEL    0x0000007FU
#define GPIO_FUNC105_IN_SEL_M  (GPIO_FUNC105_IN_SEL_V << GPIO_FUNC105_IN_SEL_S)
#define GPIO_FUNC105_IN_SEL_V  0x0000007FU
#define GPIO_FUNC105_IN_SEL_S  0
/** GPIO_FUNC105_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC105_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC105_IN_INV_SEL_M  (GPIO_FUNC105_IN_INV_SEL_V << GPIO_FUNC105_IN_INV_SEL_S)
#define GPIO_FUNC105_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC105_IN_INV_SEL_S  7
/** GPIO_SIG105_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG105_IN_SEL    (BIT(8))
#define GPIO_SIG105_IN_SEL_M  (GPIO_SIG105_IN_SEL_V << GPIO_SIG105_IN_SEL_S)
#define GPIO_SIG105_IN_SEL_V  0x00000001U
#define GPIO_SIG105_IN_SEL_S  8

/** GPIO_FUNC106_IN_SEL_CFG_REG register
 *  Configuration register for input signal 106
 */
#define GPIO_FUNC106_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x46c)
/** GPIO_FUNC106_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 106.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC106_IN_SEL    0x0000007FU
#define GPIO_FUNC106_IN_SEL_M  (GPIO_FUNC106_IN_SEL_V << GPIO_FUNC106_IN_SEL_S)
#define GPIO_FUNC106_IN_SEL_V  0x0000007FU
#define GPIO_FUNC106_IN_SEL_S  0
/** GPIO_FUNC106_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC106_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC106_IN_INV_SEL_M  (GPIO_FUNC106_IN_INV_SEL_V << GPIO_FUNC106_IN_INV_SEL_S)
#define GPIO_FUNC106_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC106_IN_INV_SEL_S  7
/** GPIO_SIG106_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG106_IN_SEL    (BIT(8))
#define GPIO_SIG106_IN_SEL_M  (GPIO_SIG106_IN_SEL_V << GPIO_SIG106_IN_SEL_S)
#define GPIO_SIG106_IN_SEL_V  0x00000001U
#define GPIO_SIG106_IN_SEL_S  8

/** GPIO_FUNC107_IN_SEL_CFG_REG register
 *  Configuration register for input signal 107
 */
#define GPIO_FUNC107_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x470)
/** GPIO_FUNC107_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 107.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC107_IN_SEL    0x0000007FU
#define GPIO_FUNC107_IN_SEL_M  (GPIO_FUNC107_IN_SEL_V << GPIO_FUNC107_IN_SEL_S)
#define GPIO_FUNC107_IN_SEL_V  0x0000007FU
#define GPIO_FUNC107_IN_SEL_S  0
/** GPIO_FUNC107_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC107_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC107_IN_INV_SEL_M  (GPIO_FUNC107_IN_INV_SEL_V << GPIO_FUNC107_IN_INV_SEL_S)
#define GPIO_FUNC107_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC107_IN_INV_SEL_S  7
/** GPIO_SIG107_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG107_IN_SEL    (BIT(8))
#define GPIO_SIG107_IN_SEL_M  (GPIO_SIG107_IN_SEL_V << GPIO_SIG107_IN_SEL_S)
#define GPIO_SIG107_IN_SEL_V  0x00000001U
#define GPIO_SIG107_IN_SEL_S  8

/** GPIO_FUNC108_IN_SEL_CFG_REG register
 *  Configuration register for input signal 108
 */
#define GPIO_FUNC108_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x474)
/** GPIO_FUNC108_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 108.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC108_IN_SEL    0x0000007FU
#define GPIO_FUNC108_IN_SEL_M  (GPIO_FUNC108_IN_SEL_V << GPIO_FUNC108_IN_SEL_S)
#define GPIO_FUNC108_IN_SEL_V  0x0000007FU
#define GPIO_FUNC108_IN_SEL_S  0
/** GPIO_FUNC108_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC108_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC108_IN_INV_SEL_M  (GPIO_FUNC108_IN_INV_SEL_V << GPIO_FUNC108_IN_INV_SEL_S)
#define GPIO_FUNC108_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC108_IN_INV_SEL_S  7
/** GPIO_SIG108_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG108_IN_SEL    (BIT(8))
#define GPIO_SIG108_IN_SEL_M  (GPIO_SIG108_IN_SEL_V << GPIO_SIG108_IN_SEL_S)
#define GPIO_SIG108_IN_SEL_V  0x00000001U
#define GPIO_SIG108_IN_SEL_S  8

/** GPIO_FUNC109_IN_SEL_CFG_REG register
 *  Configuration register for input signal 109
 */
#define GPIO_FUNC109_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x478)
/** GPIO_FUNC109_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 109.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC109_IN_SEL    0x0000007FU
#define GPIO_FUNC109_IN_SEL_M  (GPIO_FUNC109_IN_SEL_V << GPIO_FUNC109_IN_SEL_S)
#define GPIO_FUNC109_IN_SEL_V  0x0000007FU
#define GPIO_FUNC109_IN_SEL_S  0
/** GPIO_FUNC109_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC109_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC109_IN_INV_SEL_M  (GPIO_FUNC109_IN_INV_SEL_V << GPIO_FUNC109_IN_INV_SEL_S)
#define GPIO_FUNC109_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC109_IN_INV_SEL_S  7
/** GPIO_SIG109_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG109_IN_SEL    (BIT(8))
#define GPIO_SIG109_IN_SEL_M  (GPIO_SIG109_IN_SEL_V << GPIO_SIG109_IN_SEL_S)
#define GPIO_SIG109_IN_SEL_V  0x00000001U
#define GPIO_SIG109_IN_SEL_S  8

/** GPIO_FUNC110_IN_SEL_CFG_REG register
 *  Configuration register for input signal 110
 */
#define GPIO_FUNC110_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x47c)
/** GPIO_FUNC110_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 110.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC110_IN_SEL    0x0000007FU
#define GPIO_FUNC110_IN_SEL_M  (GPIO_FUNC110_IN_SEL_V << GPIO_FUNC110_IN_SEL_S)
#define GPIO_FUNC110_IN_SEL_V  0x0000007FU
#define GPIO_FUNC110_IN_SEL_S  0
/** GPIO_FUNC110_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC110_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC110_IN_INV_SEL_M  (GPIO_FUNC110_IN_INV_SEL_V << GPIO_FUNC110_IN_INV_SEL_S)
#define GPIO_FUNC110_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC110_IN_INV_SEL_S  7
/** GPIO_SIG110_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG110_IN_SEL    (BIT(8))
#define GPIO_SIG110_IN_SEL_M  (GPIO_SIG110_IN_SEL_V << GPIO_SIG110_IN_SEL_S)
#define GPIO_SIG110_IN_SEL_V  0x00000001U
#define GPIO_SIG110_IN_SEL_S  8

/** GPIO_FUNC111_IN_SEL_CFG_REG register
 *  Configuration register for input signal 111
 */
#define GPIO_FUNC111_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x480)
/** GPIO_FUNC111_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 111.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC111_IN_SEL    0x0000007FU
#define GPIO_FUNC111_IN_SEL_M  (GPIO_FUNC111_IN_SEL_V << GPIO_FUNC111_IN_SEL_S)
#define GPIO_FUNC111_IN_SEL_V  0x0000007FU
#define GPIO_FUNC111_IN_SEL_S  0
/** GPIO_FUNC111_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC111_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC111_IN_INV_SEL_M  (GPIO_FUNC111_IN_INV_SEL_V << GPIO_FUNC111_IN_INV_SEL_S)
#define GPIO_FUNC111_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC111_IN_INV_SEL_S  7
/** GPIO_SIG111_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG111_IN_SEL    (BIT(8))
#define GPIO_SIG111_IN_SEL_M  (GPIO_SIG111_IN_SEL_V << GPIO_SIG111_IN_SEL_S)
#define GPIO_SIG111_IN_SEL_V  0x00000001U
#define GPIO_SIG111_IN_SEL_S  8

/** GPIO_FUNC112_IN_SEL_CFG_REG register
 *  Configuration register for input signal 112
 */
#define GPIO_FUNC112_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x484)
/** GPIO_FUNC112_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 112.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC112_IN_SEL    0x0000007FU
#define GPIO_FUNC112_IN_SEL_M  (GPIO_FUNC112_IN_SEL_V << GPIO_FUNC112_IN_SEL_S)
#define GPIO_FUNC112_IN_SEL_V  0x0000007FU
#define GPIO_FUNC112_IN_SEL_S  0
/** GPIO_FUNC112_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC112_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC112_IN_INV_SEL_M  (GPIO_FUNC112_IN_INV_SEL_V << GPIO_FUNC112_IN_INV_SEL_S)
#define GPIO_FUNC112_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC112_IN_INV_SEL_S  7
/** GPIO_SIG112_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG112_IN_SEL    (BIT(8))
#define GPIO_SIG112_IN_SEL_M  (GPIO_SIG112_IN_SEL_V << GPIO_SIG112_IN_SEL_S)
#define GPIO_SIG112_IN_SEL_V  0x00000001U
#define GPIO_SIG112_IN_SEL_S  8

/** GPIO_FUNC113_IN_SEL_CFG_REG register
 *  Configuration register for input signal 113
 */
#define GPIO_FUNC113_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x488)
/** GPIO_FUNC113_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 113.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC113_IN_SEL    0x0000007FU
#define GPIO_FUNC113_IN_SEL_M  (GPIO_FUNC113_IN_SEL_V << GPIO_FUNC113_IN_SEL_S)
#define GPIO_FUNC113_IN_SEL_V  0x0000007FU
#define GPIO_FUNC113_IN_SEL_S  0
/** GPIO_FUNC113_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC113_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC113_IN_INV_SEL_M  (GPIO_FUNC113_IN_INV_SEL_V << GPIO_FUNC113_IN_INV_SEL_S)
#define GPIO_FUNC113_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC113_IN_INV_SEL_S  7
/** GPIO_SIG113_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG113_IN_SEL    (BIT(8))
#define GPIO_SIG113_IN_SEL_M  (GPIO_SIG113_IN_SEL_V << GPIO_SIG113_IN_SEL_S)
#define GPIO_SIG113_IN_SEL_V  0x00000001U
#define GPIO_SIG113_IN_SEL_S  8

/** GPIO_FUNC114_IN_SEL_CFG_REG register
 *  Configuration register for input signal 114
 */
#define GPIO_FUNC114_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x48c)
/** GPIO_FUNC114_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 114.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC114_IN_SEL    0x0000007FU
#define GPIO_FUNC114_IN_SEL_M  (GPIO_FUNC114_IN_SEL_V << GPIO_FUNC114_IN_SEL_S)
#define GPIO_FUNC114_IN_SEL_V  0x0000007FU
#define GPIO_FUNC114_IN_SEL_S  0
/** GPIO_FUNC114_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC114_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC114_IN_INV_SEL_M  (GPIO_FUNC114_IN_INV_SEL_V << GPIO_FUNC114_IN_INV_SEL_S)
#define GPIO_FUNC114_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC114_IN_INV_SEL_S  7
/** GPIO_SIG114_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG114_IN_SEL    (BIT(8))
#define GPIO_SIG114_IN_SEL_M  (GPIO_SIG114_IN_SEL_V << GPIO_SIG114_IN_SEL_S)
#define GPIO_SIG114_IN_SEL_V  0x00000001U
#define GPIO_SIG114_IN_SEL_S  8

/** GPIO_FUNC115_IN_SEL_CFG_REG register
 *  Configuration register for input signal 115
 */
#define GPIO_FUNC115_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x490)
/** GPIO_FUNC115_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 115.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC115_IN_SEL    0x0000007FU
#define GPIO_FUNC115_IN_SEL_M  (GPIO_FUNC115_IN_SEL_V << GPIO_FUNC115_IN_SEL_S)
#define GPIO_FUNC115_IN_SEL_V  0x0000007FU
#define GPIO_FUNC115_IN_SEL_S  0
/** GPIO_FUNC115_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC115_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC115_IN_INV_SEL_M  (GPIO_FUNC115_IN_INV_SEL_V << GPIO_FUNC115_IN_INV_SEL_S)
#define GPIO_FUNC115_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC115_IN_INV_SEL_S  7
/** GPIO_SIG115_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG115_IN_SEL    (BIT(8))
#define GPIO_SIG115_IN_SEL_M  (GPIO_SIG115_IN_SEL_V << GPIO_SIG115_IN_SEL_S)
#define GPIO_SIG115_IN_SEL_V  0x00000001U
#define GPIO_SIG115_IN_SEL_S  8

/** GPIO_FUNC116_IN_SEL_CFG_REG register
 *  Configuration register for input signal 116
 */
#define GPIO_FUNC116_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x494)
/** GPIO_FUNC116_IN_SEL : R/W; bitpos: [6:0]; default: 96;
 *  Configures to select a pin from the 29 GPIO pins to connect the input signal 116.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  Or\\
 *  0x40: A constantly high input\\
 *  0x50: A constantly low input\\
 */
#define GPIO_FUNC116_IN_SEL    0x0000007FU
#define GPIO_FUNC116_IN_SEL_M  (GPIO_FUNC116_IN_SEL_V << GPIO_FUNC116_IN_SEL_S)
#define GPIO_FUNC116_IN_SEL_V  0x0000007FU
#define GPIO_FUNC116_IN_SEL_S  0
/** GPIO_FUNC116_IN_INV_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC116_IN_INV_SEL    (BIT(7))
#define GPIO_FUNC116_IN_INV_SEL_M  (GPIO_FUNC116_IN_INV_SEL_V << GPIO_FUNC116_IN_INV_SEL_S)
#define GPIO_FUNC116_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC116_IN_INV_SEL_S  7
/** GPIO_SIG116_IN_SEL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG116_IN_SEL    (BIT(8))
#define GPIO_SIG116_IN_SEL_M  (GPIO_SIG116_IN_SEL_V << GPIO_SIG116_IN_SEL_S)
#define GPIO_SIG116_IN_SEL_V  0x00000001U
#define GPIO_SIG116_IN_SEL_S  8

/** GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO0 output
 */
#define GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xac4)
/** GPIO_FUNC0_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO0.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 0 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC0_OUT_SEL    0x000001FFU
#define GPIO_FUNC0_OUT_SEL_M  (GPIO_FUNC0_OUT_SEL_V << GPIO_FUNC0_OUT_SEL_S)
#define GPIO_FUNC0_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC0_OUT_SEL_S  0
/** GPIO_FUNC0_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC0_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC0_OUT_INV_SEL_M  (GPIO_FUNC0_OUT_INV_SEL_V << GPIO_FUNC0_OUT_INV_SEL_S)
#define GPIO_FUNC0_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OUT_INV_SEL_S  9
/** GPIO_FUNC0_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 0 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC0_OE_SEL    (BIT(10))
#define GPIO_FUNC0_OE_SEL_M  (GPIO_FUNC0_OE_SEL_V << GPIO_FUNC0_OE_SEL_S)
#define GPIO_FUNC0_OE_SEL_V  0x00000001U
#define GPIO_FUNC0_OE_SEL_S  10
/** GPIO_FUNC0_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC0_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC0_OE_INV_SEL_M  (GPIO_FUNC0_OE_INV_SEL_V << GPIO_FUNC0_OE_INV_SEL_S)
#define GPIO_FUNC0_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_OE_INV_SEL_S  11

/** GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO1 output
 */
#define GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xac8)
/** GPIO_FUNC1_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO1.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 1 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC1_OUT_SEL    0x000001FFU
#define GPIO_FUNC1_OUT_SEL_M  (GPIO_FUNC1_OUT_SEL_V << GPIO_FUNC1_OUT_SEL_S)
#define GPIO_FUNC1_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC1_OUT_SEL_S  0
/** GPIO_FUNC1_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC1_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC1_OUT_INV_SEL_M  (GPIO_FUNC1_OUT_INV_SEL_V << GPIO_FUNC1_OUT_INV_SEL_S)
#define GPIO_FUNC1_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OUT_INV_SEL_S  9
/** GPIO_FUNC1_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 1 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC1_OE_SEL    (BIT(10))
#define GPIO_FUNC1_OE_SEL_M  (GPIO_FUNC1_OE_SEL_V << GPIO_FUNC1_OE_SEL_S)
#define GPIO_FUNC1_OE_SEL_V  0x00000001U
#define GPIO_FUNC1_OE_SEL_S  10
/** GPIO_FUNC1_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC1_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC1_OE_INV_SEL_M  (GPIO_FUNC1_OE_INV_SEL_V << GPIO_FUNC1_OE_INV_SEL_S)
#define GPIO_FUNC1_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC1_OE_INV_SEL_S  11

/** GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO2 output
 */
#define GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xacc)
/** GPIO_FUNC2_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO2.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 2 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC2_OUT_SEL    0x000001FFU
#define GPIO_FUNC2_OUT_SEL_M  (GPIO_FUNC2_OUT_SEL_V << GPIO_FUNC2_OUT_SEL_S)
#define GPIO_FUNC2_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC2_OUT_SEL_S  0
/** GPIO_FUNC2_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC2_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC2_OUT_INV_SEL_M  (GPIO_FUNC2_OUT_INV_SEL_V << GPIO_FUNC2_OUT_INV_SEL_S)
#define GPIO_FUNC2_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OUT_INV_SEL_S  9
/** GPIO_FUNC2_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 2 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC2_OE_SEL    (BIT(10))
#define GPIO_FUNC2_OE_SEL_M  (GPIO_FUNC2_OE_SEL_V << GPIO_FUNC2_OE_SEL_S)
#define GPIO_FUNC2_OE_SEL_V  0x00000001U
#define GPIO_FUNC2_OE_SEL_S  10
/** GPIO_FUNC2_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC2_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC2_OE_INV_SEL_M  (GPIO_FUNC2_OE_INV_SEL_V << GPIO_FUNC2_OE_INV_SEL_S)
#define GPIO_FUNC2_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC2_OE_INV_SEL_S  11

/** GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO3 output
 */
#define GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xad0)
/** GPIO_FUNC3_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO3.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 3 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC3_OUT_SEL    0x000001FFU
#define GPIO_FUNC3_OUT_SEL_M  (GPIO_FUNC3_OUT_SEL_V << GPIO_FUNC3_OUT_SEL_S)
#define GPIO_FUNC3_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC3_OUT_SEL_S  0
/** GPIO_FUNC3_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC3_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC3_OUT_INV_SEL_M  (GPIO_FUNC3_OUT_INV_SEL_V << GPIO_FUNC3_OUT_INV_SEL_S)
#define GPIO_FUNC3_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OUT_INV_SEL_S  9
/** GPIO_FUNC3_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 3 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC3_OE_SEL    (BIT(10))
#define GPIO_FUNC3_OE_SEL_M  (GPIO_FUNC3_OE_SEL_V << GPIO_FUNC3_OE_SEL_S)
#define GPIO_FUNC3_OE_SEL_V  0x00000001U
#define GPIO_FUNC3_OE_SEL_S  10
/** GPIO_FUNC3_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC3_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC3_OE_INV_SEL_M  (GPIO_FUNC3_OE_INV_SEL_V << GPIO_FUNC3_OE_INV_SEL_S)
#define GPIO_FUNC3_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC3_OE_INV_SEL_S  11

/** GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO4 output
 */
#define GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xad4)
/** GPIO_FUNC4_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO4.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 4 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC4_OUT_SEL    0x000001FFU
#define GPIO_FUNC4_OUT_SEL_M  (GPIO_FUNC4_OUT_SEL_V << GPIO_FUNC4_OUT_SEL_S)
#define GPIO_FUNC4_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC4_OUT_SEL_S  0
/** GPIO_FUNC4_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC4_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC4_OUT_INV_SEL_M  (GPIO_FUNC4_OUT_INV_SEL_V << GPIO_FUNC4_OUT_INV_SEL_S)
#define GPIO_FUNC4_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OUT_INV_SEL_S  9
/** GPIO_FUNC4_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 4 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC4_OE_SEL    (BIT(10))
#define GPIO_FUNC4_OE_SEL_M  (GPIO_FUNC4_OE_SEL_V << GPIO_FUNC4_OE_SEL_S)
#define GPIO_FUNC4_OE_SEL_V  0x00000001U
#define GPIO_FUNC4_OE_SEL_S  10
/** GPIO_FUNC4_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC4_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC4_OE_INV_SEL_M  (GPIO_FUNC4_OE_INV_SEL_V << GPIO_FUNC4_OE_INV_SEL_S)
#define GPIO_FUNC4_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC4_OE_INV_SEL_S  11

/** GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO5 output
 */
#define GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xad8)
/** GPIO_FUNC5_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO5.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 5 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC5_OUT_SEL    0x000001FFU
#define GPIO_FUNC5_OUT_SEL_M  (GPIO_FUNC5_OUT_SEL_V << GPIO_FUNC5_OUT_SEL_S)
#define GPIO_FUNC5_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC5_OUT_SEL_S  0
/** GPIO_FUNC5_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC5_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC5_OUT_INV_SEL_M  (GPIO_FUNC5_OUT_INV_SEL_V << GPIO_FUNC5_OUT_INV_SEL_S)
#define GPIO_FUNC5_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OUT_INV_SEL_S  9
/** GPIO_FUNC5_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 5 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC5_OE_SEL    (BIT(10))
#define GPIO_FUNC5_OE_SEL_M  (GPIO_FUNC5_OE_SEL_V << GPIO_FUNC5_OE_SEL_S)
#define GPIO_FUNC5_OE_SEL_V  0x00000001U
#define GPIO_FUNC5_OE_SEL_S  10
/** GPIO_FUNC5_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC5_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC5_OE_INV_SEL_M  (GPIO_FUNC5_OE_INV_SEL_V << GPIO_FUNC5_OE_INV_SEL_S)
#define GPIO_FUNC5_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC5_OE_INV_SEL_S  11

/** GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO6 output
 */
#define GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xadc)
/** GPIO_FUNC6_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO6.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 6 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC6_OUT_SEL    0x000001FFU
#define GPIO_FUNC6_OUT_SEL_M  (GPIO_FUNC6_OUT_SEL_V << GPIO_FUNC6_OUT_SEL_S)
#define GPIO_FUNC6_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC6_OUT_SEL_S  0
/** GPIO_FUNC6_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC6_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC6_OUT_INV_SEL_M  (GPIO_FUNC6_OUT_INV_SEL_V << GPIO_FUNC6_OUT_INV_SEL_S)
#define GPIO_FUNC6_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OUT_INV_SEL_S  9
/** GPIO_FUNC6_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 6 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC6_OE_SEL    (BIT(10))
#define GPIO_FUNC6_OE_SEL_M  (GPIO_FUNC6_OE_SEL_V << GPIO_FUNC6_OE_SEL_S)
#define GPIO_FUNC6_OE_SEL_V  0x00000001U
#define GPIO_FUNC6_OE_SEL_S  10
/** GPIO_FUNC6_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC6_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC6_OE_INV_SEL_M  (GPIO_FUNC6_OE_INV_SEL_V << GPIO_FUNC6_OE_INV_SEL_S)
#define GPIO_FUNC6_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_OE_INV_SEL_S  11

/** GPIO_FUNC7_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO7 output
 */
#define GPIO_FUNC7_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xae0)
/** GPIO_FUNC7_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO7.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 7 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC7_OUT_SEL    0x000001FFU
#define GPIO_FUNC7_OUT_SEL_M  (GPIO_FUNC7_OUT_SEL_V << GPIO_FUNC7_OUT_SEL_S)
#define GPIO_FUNC7_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC7_OUT_SEL_S  0
/** GPIO_FUNC7_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC7_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC7_OUT_INV_SEL_M  (GPIO_FUNC7_OUT_INV_SEL_V << GPIO_FUNC7_OUT_INV_SEL_S)
#define GPIO_FUNC7_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OUT_INV_SEL_S  9
/** GPIO_FUNC7_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 7 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC7_OE_SEL    (BIT(10))
#define GPIO_FUNC7_OE_SEL_M  (GPIO_FUNC7_OE_SEL_V << GPIO_FUNC7_OE_SEL_S)
#define GPIO_FUNC7_OE_SEL_V  0x00000001U
#define GPIO_FUNC7_OE_SEL_S  10
/** GPIO_FUNC7_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC7_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC7_OE_INV_SEL_M  (GPIO_FUNC7_OE_INV_SEL_V << GPIO_FUNC7_OE_INV_SEL_S)
#define GPIO_FUNC7_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_OE_INV_SEL_S  11

/** GPIO_FUNC8_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO8 output
 */
#define GPIO_FUNC8_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xae4)
/** GPIO_FUNC8_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO8.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 8 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC8_OUT_SEL    0x000001FFU
#define GPIO_FUNC8_OUT_SEL_M  (GPIO_FUNC8_OUT_SEL_V << GPIO_FUNC8_OUT_SEL_S)
#define GPIO_FUNC8_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC8_OUT_SEL_S  0
/** GPIO_FUNC8_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC8_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC8_OUT_INV_SEL_M  (GPIO_FUNC8_OUT_INV_SEL_V << GPIO_FUNC8_OUT_INV_SEL_S)
#define GPIO_FUNC8_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OUT_INV_SEL_S  9
/** GPIO_FUNC8_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 8 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC8_OE_SEL    (BIT(10))
#define GPIO_FUNC8_OE_SEL_M  (GPIO_FUNC8_OE_SEL_V << GPIO_FUNC8_OE_SEL_S)
#define GPIO_FUNC8_OE_SEL_V  0x00000001U
#define GPIO_FUNC8_OE_SEL_S  10
/** GPIO_FUNC8_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC8_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC8_OE_INV_SEL_M  (GPIO_FUNC8_OE_INV_SEL_V << GPIO_FUNC8_OE_INV_SEL_S)
#define GPIO_FUNC8_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_OE_INV_SEL_S  11

/** GPIO_FUNC9_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO9 output
 */
#define GPIO_FUNC9_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xae8)
/** GPIO_FUNC9_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO9.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 9 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
 *  output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC9_OUT_SEL    0x000001FFU
#define GPIO_FUNC9_OUT_SEL_M  (GPIO_FUNC9_OUT_SEL_V << GPIO_FUNC9_OUT_SEL_S)
#define GPIO_FUNC9_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC9_OUT_SEL_S  0
/** GPIO_FUNC9_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC9_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC9_OUT_INV_SEL_M  (GPIO_FUNC9_OUT_INV_SEL_V << GPIO_FUNC9_OUT_INV_SEL_S)
#define GPIO_FUNC9_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OUT_INV_SEL_S  9
/** GPIO_FUNC9_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 9 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC9_OE_SEL    (BIT(10))
#define GPIO_FUNC9_OE_SEL_M  (GPIO_FUNC9_OE_SEL_V << GPIO_FUNC9_OE_SEL_S)
#define GPIO_FUNC9_OE_SEL_V  0x00000001U
#define GPIO_FUNC9_OE_SEL_S  10
/** GPIO_FUNC9_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC9_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC9_OE_INV_SEL_M  (GPIO_FUNC9_OE_INV_SEL_V << GPIO_FUNC9_OE_INV_SEL_S)
#define GPIO_FUNC9_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_OE_INV_SEL_S  11

/** GPIO_FUNC10_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO10 output
 */
#define GPIO_FUNC10_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaec)
/** GPIO_FUNC10_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO10.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 10 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC10_OUT_SEL    0x000001FFU
#define GPIO_FUNC10_OUT_SEL_M  (GPIO_FUNC10_OUT_SEL_V << GPIO_FUNC10_OUT_SEL_S)
#define GPIO_FUNC10_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC10_OUT_SEL_S  0
/** GPIO_FUNC10_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC10_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC10_OUT_INV_SEL_M  (GPIO_FUNC10_OUT_INV_SEL_V << GPIO_FUNC10_OUT_INV_SEL_S)
#define GPIO_FUNC10_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OUT_INV_SEL_S  9
/** GPIO_FUNC10_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 10 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC10_OE_SEL    (BIT(10))
#define GPIO_FUNC10_OE_SEL_M  (GPIO_FUNC10_OE_SEL_V << GPIO_FUNC10_OE_SEL_S)
#define GPIO_FUNC10_OE_SEL_V  0x00000001U
#define GPIO_FUNC10_OE_SEL_S  10
/** GPIO_FUNC10_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC10_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC10_OE_INV_SEL_M  (GPIO_FUNC10_OE_INV_SEL_V << GPIO_FUNC10_OE_INV_SEL_S)
#define GPIO_FUNC10_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_OE_INV_SEL_S  11

/** GPIO_FUNC11_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO11 output
 */
#define GPIO_FUNC11_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf0)
/** GPIO_FUNC11_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO11.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 11 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC11_OUT_SEL    0x000001FFU
#define GPIO_FUNC11_OUT_SEL_M  (GPIO_FUNC11_OUT_SEL_V << GPIO_FUNC11_OUT_SEL_S)
#define GPIO_FUNC11_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC11_OUT_SEL_S  0
/** GPIO_FUNC11_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC11_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC11_OUT_INV_SEL_M  (GPIO_FUNC11_OUT_INV_SEL_V << GPIO_FUNC11_OUT_INV_SEL_S)
#define GPIO_FUNC11_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OUT_INV_SEL_S  9
/** GPIO_FUNC11_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 11 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC11_OE_SEL    (BIT(10))
#define GPIO_FUNC11_OE_SEL_M  (GPIO_FUNC11_OE_SEL_V << GPIO_FUNC11_OE_SEL_S)
#define GPIO_FUNC11_OE_SEL_V  0x00000001U
#define GPIO_FUNC11_OE_SEL_S  10
/** GPIO_FUNC11_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC11_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC11_OE_INV_SEL_M  (GPIO_FUNC11_OE_INV_SEL_V << GPIO_FUNC11_OE_INV_SEL_S)
#define GPIO_FUNC11_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_OE_INV_SEL_S  11

/** GPIO_FUNC12_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO12 output
 */
#define GPIO_FUNC12_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf4)
/** GPIO_FUNC12_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO12.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 12 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC12_OUT_SEL    0x000001FFU
#define GPIO_FUNC12_OUT_SEL_M  (GPIO_FUNC12_OUT_SEL_V << GPIO_FUNC12_OUT_SEL_S)
#define GPIO_FUNC12_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC12_OUT_SEL_S  0
/** GPIO_FUNC12_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC12_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC12_OUT_INV_SEL_M  (GPIO_FUNC12_OUT_INV_SEL_V << GPIO_FUNC12_OUT_INV_SEL_S)
#define GPIO_FUNC12_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OUT_INV_SEL_S  9
/** GPIO_FUNC12_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 12 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC12_OE_SEL    (BIT(10))
#define GPIO_FUNC12_OE_SEL_M  (GPIO_FUNC12_OE_SEL_V << GPIO_FUNC12_OE_SEL_S)
#define GPIO_FUNC12_OE_SEL_V  0x00000001U
#define GPIO_FUNC12_OE_SEL_S  10
/** GPIO_FUNC12_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC12_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC12_OE_INV_SEL_M  (GPIO_FUNC12_OE_INV_SEL_V << GPIO_FUNC12_OE_INV_SEL_S)
#define GPIO_FUNC12_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_OE_INV_SEL_S  11

/** GPIO_FUNC13_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO13 output
 */
#define GPIO_FUNC13_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xaf8)
/** GPIO_FUNC13_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO13.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 13 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC13_OUT_SEL    0x000001FFU
#define GPIO_FUNC13_OUT_SEL_M  (GPIO_FUNC13_OUT_SEL_V << GPIO_FUNC13_OUT_SEL_S)
#define GPIO_FUNC13_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC13_OUT_SEL_S  0
/** GPIO_FUNC13_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC13_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC13_OUT_INV_SEL_M  (GPIO_FUNC13_OUT_INV_SEL_V << GPIO_FUNC13_OUT_INV_SEL_S)
#define GPIO_FUNC13_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OUT_INV_SEL_S  9
/** GPIO_FUNC13_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 13 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC13_OE_SEL    (BIT(10))
#define GPIO_FUNC13_OE_SEL_M  (GPIO_FUNC13_OE_SEL_V << GPIO_FUNC13_OE_SEL_S)
#define GPIO_FUNC13_OE_SEL_V  0x00000001U
#define GPIO_FUNC13_OE_SEL_S  10
/** GPIO_FUNC13_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC13_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC13_OE_INV_SEL_M  (GPIO_FUNC13_OE_INV_SEL_V << GPIO_FUNC13_OE_INV_SEL_S)
#define GPIO_FUNC13_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_OE_INV_SEL_S  11

/** GPIO_FUNC14_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO14 output
 */
#define GPIO_FUNC14_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xafc)
/** GPIO_FUNC14_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO14.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 14 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC14_OUT_SEL    0x000001FFU
#define GPIO_FUNC14_OUT_SEL_M  (GPIO_FUNC14_OUT_SEL_V << GPIO_FUNC14_OUT_SEL_S)
#define GPIO_FUNC14_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC14_OUT_SEL_S  0
/** GPIO_FUNC14_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC14_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC14_OUT_INV_SEL_M  (GPIO_FUNC14_OUT_INV_SEL_V << GPIO_FUNC14_OUT_INV_SEL_S)
#define GPIO_FUNC14_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OUT_INV_SEL_S  9
/** GPIO_FUNC14_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 14 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC14_OE_SEL    (BIT(10))
#define GPIO_FUNC14_OE_SEL_M  (GPIO_FUNC14_OE_SEL_V << GPIO_FUNC14_OE_SEL_S)
#define GPIO_FUNC14_OE_SEL_V  0x00000001U
#define GPIO_FUNC14_OE_SEL_S  10
/** GPIO_FUNC14_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC14_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC14_OE_INV_SEL_M  (GPIO_FUNC14_OE_INV_SEL_V << GPIO_FUNC14_OE_INV_SEL_S)
#define GPIO_FUNC14_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_OE_INV_SEL_S  11

/** GPIO_FUNC15_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO15 output
 */
#define GPIO_FUNC15_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb00)
/** GPIO_FUNC15_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO15.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 15 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC15_OUT_SEL    0x000001FFU
#define GPIO_FUNC15_OUT_SEL_M  (GPIO_FUNC15_OUT_SEL_V << GPIO_FUNC15_OUT_SEL_S)
#define GPIO_FUNC15_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC15_OUT_SEL_S  0
/** GPIO_FUNC15_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC15_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC15_OUT_INV_SEL_M  (GPIO_FUNC15_OUT_INV_SEL_V << GPIO_FUNC15_OUT_INV_SEL_S)
#define GPIO_FUNC15_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OUT_INV_SEL_S  9
/** GPIO_FUNC15_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 15 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC15_OE_SEL    (BIT(10))
#define GPIO_FUNC15_OE_SEL_M  (GPIO_FUNC15_OE_SEL_V << GPIO_FUNC15_OE_SEL_S)
#define GPIO_FUNC15_OE_SEL_V  0x00000001U
#define GPIO_FUNC15_OE_SEL_S  10
/** GPIO_FUNC15_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC15_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC15_OE_INV_SEL_M  (GPIO_FUNC15_OE_INV_SEL_V << GPIO_FUNC15_OE_INV_SEL_S)
#define GPIO_FUNC15_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_OE_INV_SEL_S  11

/** GPIO_FUNC16_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO16 output
 */
#define GPIO_FUNC16_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb04)
/** GPIO_FUNC16_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO16.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 16 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC16_OUT_SEL    0x000001FFU
#define GPIO_FUNC16_OUT_SEL_M  (GPIO_FUNC16_OUT_SEL_V << GPIO_FUNC16_OUT_SEL_S)
#define GPIO_FUNC16_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC16_OUT_SEL_S  0
/** GPIO_FUNC16_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC16_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC16_OUT_INV_SEL_M  (GPIO_FUNC16_OUT_INV_SEL_V << GPIO_FUNC16_OUT_INV_SEL_S)
#define GPIO_FUNC16_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OUT_INV_SEL_S  9
/** GPIO_FUNC16_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 16 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC16_OE_SEL    (BIT(10))
#define GPIO_FUNC16_OE_SEL_M  (GPIO_FUNC16_OE_SEL_V << GPIO_FUNC16_OE_SEL_S)
#define GPIO_FUNC16_OE_SEL_V  0x00000001U
#define GPIO_FUNC16_OE_SEL_S  10
/** GPIO_FUNC16_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC16_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC16_OE_INV_SEL_M  (GPIO_FUNC16_OE_INV_SEL_V << GPIO_FUNC16_OE_INV_SEL_S)
#define GPIO_FUNC16_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_OE_INV_SEL_S  11

/** GPIO_FUNC17_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO17 output
 */
#define GPIO_FUNC17_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb08)
/** GPIO_FUNC17_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO17.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 17 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC17_OUT_SEL    0x000001FFU
#define GPIO_FUNC17_OUT_SEL_M  (GPIO_FUNC17_OUT_SEL_V << GPIO_FUNC17_OUT_SEL_S)
#define GPIO_FUNC17_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC17_OUT_SEL_S  0
/** GPIO_FUNC17_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC17_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC17_OUT_INV_SEL_M  (GPIO_FUNC17_OUT_INV_SEL_V << GPIO_FUNC17_OUT_INV_SEL_S)
#define GPIO_FUNC17_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OUT_INV_SEL_S  9
/** GPIO_FUNC17_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 17 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC17_OE_SEL    (BIT(10))
#define GPIO_FUNC17_OE_SEL_M  (GPIO_FUNC17_OE_SEL_V << GPIO_FUNC17_OE_SEL_S)
#define GPIO_FUNC17_OE_SEL_V  0x00000001U
#define GPIO_FUNC17_OE_SEL_S  10
/** GPIO_FUNC17_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC17_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC17_OE_INV_SEL_M  (GPIO_FUNC17_OE_INV_SEL_V << GPIO_FUNC17_OE_INV_SEL_S)
#define GPIO_FUNC17_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_OE_INV_SEL_S  11

/** GPIO_FUNC18_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO18 output
 */
#define GPIO_FUNC18_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb0c)
/** GPIO_FUNC18_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO18.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 18 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC18_OUT_SEL    0x000001FFU
#define GPIO_FUNC18_OUT_SEL_M  (GPIO_FUNC18_OUT_SEL_V << GPIO_FUNC18_OUT_SEL_S)
#define GPIO_FUNC18_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC18_OUT_SEL_S  0
/** GPIO_FUNC18_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC18_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC18_OUT_INV_SEL_M  (GPIO_FUNC18_OUT_INV_SEL_V << GPIO_FUNC18_OUT_INV_SEL_S)
#define GPIO_FUNC18_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OUT_INV_SEL_S  9
/** GPIO_FUNC18_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 18 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC18_OE_SEL    (BIT(10))
#define GPIO_FUNC18_OE_SEL_M  (GPIO_FUNC18_OE_SEL_V << GPIO_FUNC18_OE_SEL_S)
#define GPIO_FUNC18_OE_SEL_V  0x00000001U
#define GPIO_FUNC18_OE_SEL_S  10
/** GPIO_FUNC18_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC18_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC18_OE_INV_SEL_M  (GPIO_FUNC18_OE_INV_SEL_V << GPIO_FUNC18_OE_INV_SEL_S)
#define GPIO_FUNC18_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC18_OE_INV_SEL_S  11

/** GPIO_FUNC19_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO19 output
 */
#define GPIO_FUNC19_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb10)
/** GPIO_FUNC19_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO19.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 19 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC19_OUT_SEL    0x000001FFU
#define GPIO_FUNC19_OUT_SEL_M  (GPIO_FUNC19_OUT_SEL_V << GPIO_FUNC19_OUT_SEL_S)
#define GPIO_FUNC19_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC19_OUT_SEL_S  0
/** GPIO_FUNC19_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC19_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC19_OUT_INV_SEL_M  (GPIO_FUNC19_OUT_INV_SEL_V << GPIO_FUNC19_OUT_INV_SEL_S)
#define GPIO_FUNC19_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OUT_INV_SEL_S  9
/** GPIO_FUNC19_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 19 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC19_OE_SEL    (BIT(10))
#define GPIO_FUNC19_OE_SEL_M  (GPIO_FUNC19_OE_SEL_V << GPIO_FUNC19_OE_SEL_S)
#define GPIO_FUNC19_OE_SEL_V  0x00000001U
#define GPIO_FUNC19_OE_SEL_S  10
/** GPIO_FUNC19_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC19_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC19_OE_INV_SEL_M  (GPIO_FUNC19_OE_INV_SEL_V << GPIO_FUNC19_OE_INV_SEL_S)
#define GPIO_FUNC19_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC19_OE_INV_SEL_S  11

/** GPIO_FUNC20_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO20 output
 */
#define GPIO_FUNC20_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb14)
/** GPIO_FUNC20_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO20.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 20 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC20_OUT_SEL    0x000001FFU
#define GPIO_FUNC20_OUT_SEL_M  (GPIO_FUNC20_OUT_SEL_V << GPIO_FUNC20_OUT_SEL_S)
#define GPIO_FUNC20_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC20_OUT_SEL_S  0
/** GPIO_FUNC20_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC20_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC20_OUT_INV_SEL_M  (GPIO_FUNC20_OUT_INV_SEL_V << GPIO_FUNC20_OUT_INV_SEL_S)
#define GPIO_FUNC20_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OUT_INV_SEL_S  9
/** GPIO_FUNC20_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 20 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC20_OE_SEL    (BIT(10))
#define GPIO_FUNC20_OE_SEL_M  (GPIO_FUNC20_OE_SEL_V << GPIO_FUNC20_OE_SEL_S)
#define GPIO_FUNC20_OE_SEL_V  0x00000001U
#define GPIO_FUNC20_OE_SEL_S  10
/** GPIO_FUNC20_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC20_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC20_OE_INV_SEL_M  (GPIO_FUNC20_OE_INV_SEL_V << GPIO_FUNC20_OE_INV_SEL_S)
#define GPIO_FUNC20_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC20_OE_INV_SEL_S  11

/** GPIO_FUNC21_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO21 output
 */
#define GPIO_FUNC21_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb18)
/** GPIO_FUNC21_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO21.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 21 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC21_OUT_SEL    0x000001FFU
#define GPIO_FUNC21_OUT_SEL_M  (GPIO_FUNC21_OUT_SEL_V << GPIO_FUNC21_OUT_SEL_S)
#define GPIO_FUNC21_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC21_OUT_SEL_S  0
/** GPIO_FUNC21_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC21_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC21_OUT_INV_SEL_M  (GPIO_FUNC21_OUT_INV_SEL_V << GPIO_FUNC21_OUT_INV_SEL_S)
#define GPIO_FUNC21_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OUT_INV_SEL_S  9
/** GPIO_FUNC21_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 21 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC21_OE_SEL    (BIT(10))
#define GPIO_FUNC21_OE_SEL_M  (GPIO_FUNC21_OE_SEL_V << GPIO_FUNC21_OE_SEL_S)
#define GPIO_FUNC21_OE_SEL_V  0x00000001U
#define GPIO_FUNC21_OE_SEL_S  10
/** GPIO_FUNC21_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC21_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC21_OE_INV_SEL_M  (GPIO_FUNC21_OE_INV_SEL_V << GPIO_FUNC21_OE_INV_SEL_S)
#define GPIO_FUNC21_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC21_OE_INV_SEL_S  11

/** GPIO_FUNC22_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO22 output
 */
#define GPIO_FUNC22_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb1c)
/** GPIO_FUNC22_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO22.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 22 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC22_OUT_SEL    0x000001FFU
#define GPIO_FUNC22_OUT_SEL_M  (GPIO_FUNC22_OUT_SEL_V << GPIO_FUNC22_OUT_SEL_S)
#define GPIO_FUNC22_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC22_OUT_SEL_S  0
/** GPIO_FUNC22_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC22_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC22_OUT_INV_SEL_M  (GPIO_FUNC22_OUT_INV_SEL_V << GPIO_FUNC22_OUT_INV_SEL_S)
#define GPIO_FUNC22_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OUT_INV_SEL_S  9
/** GPIO_FUNC22_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 22 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC22_OE_SEL    (BIT(10))
#define GPIO_FUNC22_OE_SEL_M  (GPIO_FUNC22_OE_SEL_V << GPIO_FUNC22_OE_SEL_S)
#define GPIO_FUNC22_OE_SEL_V  0x00000001U
#define GPIO_FUNC22_OE_SEL_S  10
/** GPIO_FUNC22_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC22_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC22_OE_INV_SEL_M  (GPIO_FUNC22_OE_INV_SEL_V << GPIO_FUNC22_OE_INV_SEL_S)
#define GPIO_FUNC22_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC22_OE_INV_SEL_S  11

/** GPIO_FUNC23_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO23 output
 */
#define GPIO_FUNC23_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb20)
/** GPIO_FUNC23_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO23.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 23 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC23_OUT_SEL    0x000001FFU
#define GPIO_FUNC23_OUT_SEL_M  (GPIO_FUNC23_OUT_SEL_V << GPIO_FUNC23_OUT_SEL_S)
#define GPIO_FUNC23_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC23_OUT_SEL_S  0
/** GPIO_FUNC23_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC23_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC23_OUT_INV_SEL_M  (GPIO_FUNC23_OUT_INV_SEL_V << GPIO_FUNC23_OUT_INV_SEL_S)
#define GPIO_FUNC23_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OUT_INV_SEL_S  9
/** GPIO_FUNC23_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 23 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC23_OE_SEL    (BIT(10))
#define GPIO_FUNC23_OE_SEL_M  (GPIO_FUNC23_OE_SEL_V << GPIO_FUNC23_OE_SEL_S)
#define GPIO_FUNC23_OE_SEL_V  0x00000001U
#define GPIO_FUNC23_OE_SEL_S  10
/** GPIO_FUNC23_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC23_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC23_OE_INV_SEL_M  (GPIO_FUNC23_OE_INV_SEL_V << GPIO_FUNC23_OE_INV_SEL_S)
#define GPIO_FUNC23_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC23_OE_INV_SEL_S  11

/** GPIO_FUNC24_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO24 output
 */
#define GPIO_FUNC24_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb24)
/** GPIO_FUNC24_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO24.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 24 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC24_OUT_SEL    0x000001FFU
#define GPIO_FUNC24_OUT_SEL_M  (GPIO_FUNC24_OUT_SEL_V << GPIO_FUNC24_OUT_SEL_S)
#define GPIO_FUNC24_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC24_OUT_SEL_S  0
/** GPIO_FUNC24_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC24_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC24_OUT_INV_SEL_M  (GPIO_FUNC24_OUT_INV_SEL_V << GPIO_FUNC24_OUT_INV_SEL_S)
#define GPIO_FUNC24_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OUT_INV_SEL_S  9
/** GPIO_FUNC24_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 24 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC24_OE_SEL    (BIT(10))
#define GPIO_FUNC24_OE_SEL_M  (GPIO_FUNC24_OE_SEL_V << GPIO_FUNC24_OE_SEL_S)
#define GPIO_FUNC24_OE_SEL_V  0x00000001U
#define GPIO_FUNC24_OE_SEL_S  10
/** GPIO_FUNC24_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC24_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC24_OE_INV_SEL_M  (GPIO_FUNC24_OE_INV_SEL_V << GPIO_FUNC24_OE_INV_SEL_S)
#define GPIO_FUNC24_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC24_OE_INV_SEL_S  11

/** GPIO_FUNC25_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO25 output
 */
#define GPIO_FUNC25_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb28)
/** GPIO_FUNC25_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO25.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 25 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC25_OUT_SEL    0x000001FFU
#define GPIO_FUNC25_OUT_SEL_M  (GPIO_FUNC25_OUT_SEL_V << GPIO_FUNC25_OUT_SEL_S)
#define GPIO_FUNC25_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC25_OUT_SEL_S  0
/** GPIO_FUNC25_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC25_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC25_OUT_INV_SEL_M  (GPIO_FUNC25_OUT_INV_SEL_V << GPIO_FUNC25_OUT_INV_SEL_S)
#define GPIO_FUNC25_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OUT_INV_SEL_S  9
/** GPIO_FUNC25_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 25 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC25_OE_SEL    (BIT(10))
#define GPIO_FUNC25_OE_SEL_M  (GPIO_FUNC25_OE_SEL_V << GPIO_FUNC25_OE_SEL_S)
#define GPIO_FUNC25_OE_SEL_V  0x00000001U
#define GPIO_FUNC25_OE_SEL_S  10
/** GPIO_FUNC25_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC25_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC25_OE_INV_SEL_M  (GPIO_FUNC25_OE_INV_SEL_V << GPIO_FUNC25_OE_INV_SEL_S)
#define GPIO_FUNC25_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC25_OE_INV_SEL_S  11

/** GPIO_FUNC26_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO26 output
 */
#define GPIO_FUNC26_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb2c)
/** GPIO_FUNC26_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO26.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 26 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC26_OUT_SEL    0x000001FFU
#define GPIO_FUNC26_OUT_SEL_M  (GPIO_FUNC26_OUT_SEL_V << GPIO_FUNC26_OUT_SEL_S)
#define GPIO_FUNC26_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC26_OUT_SEL_S  0
/** GPIO_FUNC26_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC26_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC26_OUT_INV_SEL_M  (GPIO_FUNC26_OUT_INV_SEL_V << GPIO_FUNC26_OUT_INV_SEL_S)
#define GPIO_FUNC26_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OUT_INV_SEL_S  9
/** GPIO_FUNC26_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 26 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC26_OE_SEL    (BIT(10))
#define GPIO_FUNC26_OE_SEL_M  (GPIO_FUNC26_OE_SEL_V << GPIO_FUNC26_OE_SEL_S)
#define GPIO_FUNC26_OE_SEL_V  0x00000001U
#define GPIO_FUNC26_OE_SEL_S  10
/** GPIO_FUNC26_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC26_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC26_OE_INV_SEL_M  (GPIO_FUNC26_OE_INV_SEL_V << GPIO_FUNC26_OE_INV_SEL_S)
#define GPIO_FUNC26_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC26_OE_INV_SEL_S  11

/** GPIO_FUNC27_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO27 output
 */
#define GPIO_FUNC27_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb30)
/** GPIO_FUNC27_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO27.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 27 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC27_OUT_SEL    0x000001FFU
#define GPIO_FUNC27_OUT_SEL_M  (GPIO_FUNC27_OUT_SEL_V << GPIO_FUNC27_OUT_SEL_S)
#define GPIO_FUNC27_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC27_OUT_SEL_S  0
/** GPIO_FUNC27_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC27_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC27_OUT_INV_SEL_M  (GPIO_FUNC27_OUT_INV_SEL_V << GPIO_FUNC27_OUT_INV_SEL_S)
#define GPIO_FUNC27_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OUT_INV_SEL_S  9
/** GPIO_FUNC27_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 27 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC27_OE_SEL    (BIT(10))
#define GPIO_FUNC27_OE_SEL_M  (GPIO_FUNC27_OE_SEL_V << GPIO_FUNC27_OE_SEL_S)
#define GPIO_FUNC27_OE_SEL_V  0x00000001U
#define GPIO_FUNC27_OE_SEL_S  10
/** GPIO_FUNC27_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC27_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC27_OE_INV_SEL_M  (GPIO_FUNC27_OE_INV_SEL_V << GPIO_FUNC27_OE_INV_SEL_S)
#define GPIO_FUNC27_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_OE_INV_SEL_S  11

/** GPIO_FUNC28_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO28 output
 */
#define GPIO_FUNC28_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb34)
/** GPIO_FUNC28_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO28.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit 28 of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC28_OUT_SEL    0x000001FFU
#define GPIO_FUNC28_OUT_SEL_M  (GPIO_FUNC28_OUT_SEL_V << GPIO_FUNC28_OUT_SEL_S)
#define GPIO_FUNC28_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC28_OUT_SEL_S  0
/** GPIO_FUNC28_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC28_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC28_OUT_INV_SEL_M  (GPIO_FUNC28_OUT_INV_SEL_V << GPIO_FUNC28_OUT_INV_SEL_S)
#define GPIO_FUNC28_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OUT_INV_SEL_S  9
/** GPIO_FUNC28_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit 28 of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC28_OE_SEL    (BIT(10))
#define GPIO_FUNC28_OE_SEL_M  (GPIO_FUNC28_OE_SEL_V << GPIO_FUNC28_OE_SEL_S)
#define GPIO_FUNC28_OE_SEL_V  0x00000001U
#define GPIO_FUNC28_OE_SEL_S  10
/** GPIO_FUNC28_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC28_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC28_OE_INV_SEL_M  (GPIO_FUNC28_OE_INV_SEL_V << GPIO_FUNC28_OE_INV_SEL_S)
#define GPIO_FUNC28_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_OE_INV_SEL_S  11

/** GPIO_FUNC29_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO$n output
 */
#define GPIO_FUNC29_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb38)
/** GPIO_FUNC29_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO$n.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit $n of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC29_OUT_SEL    0x000001FFU
#define GPIO_FUNC29_OUT_SEL_M  (GPIO_FUNC29_OUT_SEL_V << GPIO_FUNC29_OUT_SEL_S)
#define GPIO_FUNC29_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC29_OUT_SEL_S  0
/** GPIO_FUNC29_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC29_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC29_OUT_INV_SEL_M  (GPIO_FUNC29_OUT_INV_SEL_V << GPIO_FUNC29_OUT_INV_SEL_S)
#define GPIO_FUNC29_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OUT_INV_SEL_S  9
/** GPIO_FUNC29_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit $n of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC29_OE_SEL    (BIT(10))
#define GPIO_FUNC29_OE_SEL_M  (GPIO_FUNC29_OE_SEL_V << GPIO_FUNC29_OE_SEL_S)
#define GPIO_FUNC29_OE_SEL_V  0x00000001U
#define GPIO_FUNC29_OE_SEL_S  10
/** GPIO_FUNC29_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC29_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC29_OE_INV_SEL_M  (GPIO_FUNC29_OE_INV_SEL_V << GPIO_FUNC29_OE_INV_SEL_S)
#define GPIO_FUNC29_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_OE_INV_SEL_S  11

/** GPIO_FUNC30_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO$n output
 */
#define GPIO_FUNC30_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb3c)
/** GPIO_FUNC30_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO$n.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit $n of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC30_OUT_SEL    0x000001FFU
#define GPIO_FUNC30_OUT_SEL_M  (GPIO_FUNC30_OUT_SEL_V << GPIO_FUNC30_OUT_SEL_S)
#define GPIO_FUNC30_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC30_OUT_SEL_S  0
/** GPIO_FUNC30_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC30_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC30_OUT_INV_SEL_M  (GPIO_FUNC30_OUT_INV_SEL_V << GPIO_FUNC30_OUT_INV_SEL_S)
#define GPIO_FUNC30_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OUT_INV_SEL_S  9
/** GPIO_FUNC30_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit $n of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC30_OE_SEL    (BIT(10))
#define GPIO_FUNC30_OE_SEL_M  (GPIO_FUNC30_OE_SEL_V << GPIO_FUNC30_OE_SEL_S)
#define GPIO_FUNC30_OE_SEL_V  0x00000001U
#define GPIO_FUNC30_OE_SEL_S  10
/** GPIO_FUNC30_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC30_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC30_OE_INV_SEL_M  (GPIO_FUNC30_OE_INV_SEL_V << GPIO_FUNC30_OE_INV_SEL_S)
#define GPIO_FUNC30_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_OE_INV_SEL_S  11

/** GPIO_FUNC31_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO$n output
 */
#define GPIO_FUNC31_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb40)
/** GPIO_FUNC31_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO$n.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit $n of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC31_OUT_SEL    0x000001FFU
#define GPIO_FUNC31_OUT_SEL_M  (GPIO_FUNC31_OUT_SEL_V << GPIO_FUNC31_OUT_SEL_S)
#define GPIO_FUNC31_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC31_OUT_SEL_S  0
/** GPIO_FUNC31_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC31_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC31_OUT_INV_SEL_M  (GPIO_FUNC31_OUT_INV_SEL_V << GPIO_FUNC31_OUT_INV_SEL_S)
#define GPIO_FUNC31_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OUT_INV_SEL_S  9
/** GPIO_FUNC31_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit $n of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC31_OE_SEL    (BIT(10))
#define GPIO_FUNC31_OE_SEL_M  (GPIO_FUNC31_OE_SEL_V << GPIO_FUNC31_OE_SEL_S)
#define GPIO_FUNC31_OE_SEL_V  0x00000001U
#define GPIO_FUNC31_OE_SEL_S  10
/** GPIO_FUNC31_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC31_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC31_OE_INV_SEL_M  (GPIO_FUNC31_OE_INV_SEL_V << GPIO_FUNC31_OE_INV_SEL_S)
#define GPIO_FUNC31_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_OE_INV_SEL_S  11

/** GPIO_FUNC32_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO$n output
 */
#define GPIO_FUNC32_OUT_SEL_CFG_REG (DR_REG_GPIO_BASE + 0xb44)
/** GPIO_FUNC32_OUT_SEL : R/W/SC; bitpos: [8:0]; default: 256;
 *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
 *  output from GPIO$n.\\
 *  0: Select signal 0\\
 *  1: Select signal 1\\
 *  ......\\
 *  254: Select signal 254\\
 *  255: Select signal 255\\
 *  Or\\
 *  256: Bit $n of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value
 *  and output enable.
 *
 *  For the detailed signal list, see Table <a
 *  href=tab:iomuxgpio-periph-signals-via-gpio-matrix">link</a>.
 *  "
 */
#define GPIO_FUNC32_OUT_SEL    0x000001FFU
#define GPIO_FUNC32_OUT_SEL_M  (GPIO_FUNC32_OUT_SEL_V << GPIO_FUNC32_OUT_SEL_S)
#define GPIO_FUNC32_OUT_SEL_V  0x000001FFU
#define GPIO_FUNC32_OUT_SEL_S  0
/** GPIO_FUNC32_OUT_INV_SEL : R/W/SC; bitpos: [9]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC32_OUT_INV_SEL    (BIT(9))
#define GPIO_FUNC32_OUT_INV_SEL_M  (GPIO_FUNC32_OUT_INV_SEL_V << GPIO_FUNC32_OUT_INV_SEL_S)
#define GPIO_FUNC32_OUT_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OUT_INV_SEL_S  9
/** GPIO_FUNC32_OE_SEL : R/W; bitpos: [10]; default: 0;
 *  Configures to select the source of output enable signal.\\
 *  0: Use output enable signal from peripheral.\\
 *  1: Force the output enable signal to be sourced from bit $n of GPIO_ENABLE_REG. \\
 */
#define GPIO_FUNC32_OE_SEL    (BIT(10))
#define GPIO_FUNC32_OE_SEL_M  (GPIO_FUNC32_OE_SEL_V << GPIO_FUNC32_OE_SEL_S)
#define GPIO_FUNC32_OE_SEL_V  0x00000001U
#define GPIO_FUNC32_OE_SEL_S  10
/** GPIO_FUNC32_OE_INV_SEL : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC32_OE_INV_SEL    (BIT(11))
#define GPIO_FUNC32_OE_INV_SEL_M  (GPIO_FUNC32_OE_INV_SEL_V << GPIO_FUNC32_OE_INV_SEL_S)
#define GPIO_FUNC32_OE_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_OE_INV_SEL_S  11

/** GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define GPIO_CLOCK_GATE_REG (DR_REG_GPIO_BASE + 0xdf8)
/** GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to enable clock gate.\\
 *  0: Not enable\\
 *  1: Enable, the clock is free running. \\
 */
#define GPIO_CLK_EN    (BIT(0))
#define GPIO_CLK_EN_M  (GPIO_CLK_EN_V << GPIO_CLK_EN_S)
#define GPIO_CLK_EN_V  0x00000001U
#define GPIO_CLK_EN_S  0

/** GPIO_DATE_REG register
 *  GPIO version register
 */
#define GPIO_DATE_REG (DR_REG_GPIO_BASE + 0xdfc)
/** GPIO_DATE : R/W; bitpos: [27:0]; default: 36774208;
 *  Version control register. \\
 */
#define GPIO_DATE    0x0FFFFFFFU
#define GPIO_DATE_M  (GPIO_DATE_V << GPIO_DATE_S)
#define GPIO_DATE_V  0x0FFFFFFFU
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
