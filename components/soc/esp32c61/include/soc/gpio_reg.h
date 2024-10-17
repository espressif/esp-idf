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
 *  - bit0 ~ bit1: invalid
 *  - bit2: GPIO8
 *  - bit3: GPIO9
 *  - bit4: GPIO7
 *  - bit5 ~ bit15: invalid
 */
#define GPIO_STRAPPING    0x0000FFFFU
#define GPIO_STRAPPING_M  (GPIO_STRAPPING_V << GPIO_STRAPPING_S)
#define GPIO_STRAPPING_V  0x0000FFFFU
#define GPIO_STRAPPING_S  0

/** GPIO_OUT_REG register
 *  GPIO output register
 */
#define GPIO_OUT_REG (DR_REG_GPIO_BASE + 0x4)
/** GPIO_OUT_DATA_ORIG : R/W/SC/WTC; bitpos: [28:0]; default: 0;
 *  Configures the output value of GPIO0 ~ 24 output in simple GPIO output mode.\\
 *  0: Low level\\
 *  1: High level\\
 *  The value of bit0 ~ bit24 correspond to the output value of GPIO0 ~ GPIO24
 *  respectively. Bit25 ~ bit31 are invalid.\\
 */
#define GPIO_OUT_DATA_ORIG    0x1FFFFFFFU
#define GPIO_OUT_DATA_ORIG_M  (GPIO_OUT_DATA_ORIG_V << GPIO_OUT_DATA_ORIG_S)
#define GPIO_OUT_DATA_ORIG_V  0x1FFFFFFFU
#define GPIO_OUT_DATA_ORIG_S  0

/** GPIO_OUT_W1TS_REG register
 *  GPIO output set register
 */
#define GPIO_OUT_W1TS_REG (DR_REG_GPIO_BASE + 0x8)
/** GPIO_OUT_W1TS : WT; bitpos: [28:0]; default: 0;
 *  Configures whether or not to set the output register GPIO_OUT_REG of GPIO0 ~
 *  GPIO24.\\
 *  0: Not set\\
 *  1: The corresponding bit in GPIO_OUT_REG will be set to 1\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to set GPIO_OUT_REG. \\
 */
#define GPIO_OUT_W1TS    0x1FFFFFFFU
#define GPIO_OUT_W1TS_M  (GPIO_OUT_W1TS_V << GPIO_OUT_W1TS_S)
#define GPIO_OUT_W1TS_V  0x1FFFFFFFU
#define GPIO_OUT_W1TS_S  0

/** GPIO_OUT_W1TC_REG register
 *  GPIO output clear register
 */
#define GPIO_OUT_W1TC_REG (DR_REG_GPIO_BASE + 0xc)
/** GPIO_OUT_W1TC : WT; bitpos: [28:0]; default: 0;
 *  Configures whether or not to clear the output register GPIO_OUT_REG of GPIO0 ~
 *  GPIO24 output.\\
 *  0: Not clear\\
 *  1: The corresponding bit in GPIO_OUT_REG will be cleared.\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to clear GPIO_OUT_REG. \\
 */
#define GPIO_OUT_W1TC    0x1FFFFFFFU
#define GPIO_OUT_W1TC_M  (GPIO_OUT_W1TC_V << GPIO_OUT_W1TC_S)
#define GPIO_OUT_W1TC_V  0x1FFFFFFFU
#define GPIO_OUT_W1TC_S  0

/** GPIO_ENABLE_REG register
 *  GPIO output enable register
 */
#define GPIO_ENABLE_REG (DR_REG_GPIO_BASE + 0x34)
/** GPIO_ENABLE_DATA : R/W/WTC; bitpos: [28:0]; default: 0;
 *  Configures whether or not to enable the output of GPIO0 ~ GPIO24.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid.\\
 */
#define GPIO_ENABLE_DATA    0x1FFFFFFFU
#define GPIO_ENABLE_DATA_M  (GPIO_ENABLE_DATA_V << GPIO_ENABLE_DATA_S)
#define GPIO_ENABLE_DATA_V  0x1FFFFFFFU
#define GPIO_ENABLE_DATA_S  0

/** GPIO_ENABLE_W1TS_REG register
 *  GPIO output enable set register
 */
#define GPIO_ENABLE_W1TS_REG (DR_REG_GPIO_BASE + 0x38)
/** GPIO_ENABLE_W1TS : WT; bitpos: [28:0]; default: 0;
 *  Configures whether or not to set the output enable register GPIO_ENABLE_REG of
 *  GPIO0 ~ GPIO24.\\
 *  0: Not set\\
 *  1: The corresponding bit in GPIO_ENABLE_REG will be set to 1\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to set GPIO_ENABLE_REG.\\
 */
#define GPIO_ENABLE_W1TS    0x1FFFFFFFU
#define GPIO_ENABLE_W1TS_M  (GPIO_ENABLE_W1TS_V << GPIO_ENABLE_W1TS_S)
#define GPIO_ENABLE_W1TS_V  0x1FFFFFFFU
#define GPIO_ENABLE_W1TS_S  0

/** GPIO_ENABLE_W1TC_REG register
 *  GPIO output enable clear register
 */
#define GPIO_ENABLE_W1TC_REG (DR_REG_GPIO_BASE + 0x3c)
/** GPIO_ENABLE_W1TC : WT; bitpos: [28:0]; default: 0;
 *  Configures whether or not to clear the output enable register GPIO_ENABLE_REG of
 *  GPIO0 ~ GPIO24. \\
 *  0: Not clear\\
 *  1: The corresponding bit in GPIO_ENABLE_REG will be cleared\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to clear GPIO_ENABLE_REG.\\
 */
#define GPIO_ENABLE_W1TC    0x1FFFFFFFU
#define GPIO_ENABLE_W1TC_M  (GPIO_ENABLE_W1TC_V << GPIO_ENABLE_W1TC_S)
#define GPIO_ENABLE_W1TC_V  0x1FFFFFFFU
#define GPIO_ENABLE_W1TC_S  0

/** GPIO_IN_REG register
 *  GPIO input register
 */
#define GPIO_IN_REG (DR_REG_GPIO_BASE + 0x64)
/** GPIO_IN_DATA_NEXT : RO; bitpos: [28:0]; default: 0;
 *  Represents the input value of GPIO0 ~ GPIO24. Each bit represents a pin input
 *  value:\\
 *  0: Low level\\
 *  1: High level\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid.\\
 */
#define GPIO_IN_DATA_NEXT    0x1FFFFFFFU
#define GPIO_IN_DATA_NEXT_M  (GPIO_IN_DATA_NEXT_V << GPIO_IN_DATA_NEXT_S)
#define GPIO_IN_DATA_NEXT_V  0x1FFFFFFFU
#define GPIO_IN_DATA_NEXT_S  0

/** GPIO_STATUS_REG register
 *  GPIO interrupt status register
 */
#define GPIO_STATUS_REG (DR_REG_GPIO_BASE + 0x74)
/** GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [28:0]; default: 0;
 *  The interrupt status of GPIO0 ~ GPIO24, can be configured by the software.
 *
 *  - Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid.
 *  - Each bit represents the status of its corresponding GPIO:
 *
 *  - 0: Represents the GPIO does not generate the interrupt configured by
 *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
 *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
 *  or this bit is configured to 1 by the software.
 *
 */
#define GPIO_STATUS_INTERRUPT    0x1FFFFFFFU
#define GPIO_STATUS_INTERRUPT_M  (GPIO_STATUS_INTERRUPT_V << GPIO_STATUS_INTERRUPT_S)
#define GPIO_STATUS_INTERRUPT_V  0x1FFFFFFFU
#define GPIO_STATUS_INTERRUPT_S  0

/** GPIO_STATUS_W1TS_REG register
 *  GPIO interrupt status set register
 */
#define GPIO_STATUS_W1TS_REG (DR_REG_GPIO_BASE + 0x78)
/** GPIO_STATUS_W1TS : WT; bitpos: [28:0]; default: 0;
 *  Configures whether or not to set the interrupt status register
 *  GPIO_STATUS_INTERRUPT of GPIO0 ~ GPIO24.
 *
 *  - Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be set to 1. \item Recommended operation: use this
 *  register to set GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS_W1TS    0x1FFFFFFFU
#define GPIO_STATUS_W1TS_M  (GPIO_STATUS_W1TS_V << GPIO_STATUS_W1TS_S)
#define GPIO_STATUS_W1TS_V  0x1FFFFFFFU
#define GPIO_STATUS_W1TS_S  0

/** GPIO_STATUS_W1TC_REG register
 *  GPIO interrupt status clear register
 */
#define GPIO_STATUS_W1TC_REG (DR_REG_GPIO_BASE + 0x7c)
/** GPIO_STATUS_W1TC : WT; bitpos: [28:0]; default: 0;
 *  Configures whether or not to clear the interrupt status register
 *  GPIO_STATUS_INTERRUPT of GPIO0 ~ GPIO24.
 *
 *  - Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear GPIO_STATUS_INTERRUPT.
 */
#define GPIO_STATUS_W1TC    0x1FFFFFFFU
#define GPIO_STATUS_W1TC_M  (GPIO_STATUS_W1TC_V << GPIO_STATUS_W1TC_S)
#define GPIO_STATUS_W1TC_V  0x1FFFFFFFU
#define GPIO_STATUS_W1TC_S  0

/** GPIO_PROCPU_INT_REG register
 *  CPU interrupt status register
 */
#define GPIO_PROCPU_INT_REG (DR_REG_GPIO_BASE + 0xa4)
/** GPIO_PROCPU_INT : RO; bitpos: [28:0]; default: 0;
 *  Represents the CPU interrupt status of GPIO0 ~ GPIO24. Each bit represents:\\
 *  0: Represents CPU interrupt is not enabled, or the GPIO does not generate the
 *  interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
 *  after the CPU interrupt is enabled.\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid. This
 *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
 *  enable signal (bit13 of GPIO_PIN$n_REG). \\
 */
#define GPIO_PROCPU_INT    0x1FFFFFFFU
#define GPIO_PROCPU_INT_M  (GPIO_PROCPU_INT_V << GPIO_PROCPU_INT_S)
#define GPIO_PROCPU_INT_V  0x1FFFFFFFU
#define GPIO_PROCPU_INT_S  0

/** GPIO_STATUS_NEXT_REG register
 *  GPIO interrupt source register
 */
#define GPIO_STATUS_NEXT_REG (DR_REG_GPIO_BASE + 0xb4)
/** GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [28:0]; default: 0;
 *  Represents the interrupt source signal of GPIO0 ~ GPIO24.\\
 *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO24. Bit25 ~ bit31 are invalid. Each
 *  bit represents:\\
 *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.\\
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.\\
 */
#define GPIO_STATUS_INTERRUPT_NEXT    0x1FFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_M  (GPIO_STATUS_INTERRUPT_NEXT_V << GPIO_STATUS_INTERRUPT_NEXT_S)
#define GPIO_STATUS_INTERRUPT_NEXT_V  0x1FFFFFFFU
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

/** GPIO_PIN0_REG register
 *  GPIO0 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO1 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO2 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO3 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO4 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO5 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO6 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO7 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO8 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO9 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO10 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO11 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO12 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO13 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO14 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO15 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO16 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO17 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO18 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO19 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO20 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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
 *  GPIO21 configuration register
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
 *  Configures whether or not to enable CPU interrupt.
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

/** GPIO_FUNC0_IN_SEL_CFG_REG register
 *  Configuration register for input signal 0
 */
#define GPIO_FUNC0_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2c4)
/** GPIO_FUNC0_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 0.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC0_IN_SEL    0x0000003FU
#define GPIO_FUNC0_IN_SEL_M  (GPIO_FUNC0_IN_SEL_V << GPIO_FUNC0_IN_SEL_S)
#define GPIO_FUNC0_IN_SEL_V  0x0000003FU
#define GPIO_FUNC0_IN_SEL_S  0
/** GPIO_FUNC0_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC0_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC0_IN_INV_SEL_M  (GPIO_FUNC0_IN_INV_SEL_V << GPIO_FUNC0_IN_INV_SEL_S)
#define GPIO_FUNC0_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC0_IN_INV_SEL_S  6
/** GPIO_SIG0_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG0_IN_SEL    (BIT(7))
#define GPIO_SIG0_IN_SEL_M  (GPIO_SIG0_IN_SEL_V << GPIO_SIG0_IN_SEL_S)
#define GPIO_SIG0_IN_SEL_V  0x00000001U
#define GPIO_SIG0_IN_SEL_S  7

/** GPIO_FUNC6_IN_SEL_CFG_REG register
 *  Configuration register for input signal 6
 */
#define GPIO_FUNC6_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2dc)
/** GPIO_FUNC6_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 6.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC6_IN_SEL    0x0000003FU
#define GPIO_FUNC6_IN_SEL_M  (GPIO_FUNC6_IN_SEL_V << GPIO_FUNC6_IN_SEL_S)
#define GPIO_FUNC6_IN_SEL_V  0x0000003FU
#define GPIO_FUNC6_IN_SEL_S  0
/** GPIO_FUNC6_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC6_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC6_IN_INV_SEL_M  (GPIO_FUNC6_IN_INV_SEL_V << GPIO_FUNC6_IN_INV_SEL_S)
#define GPIO_FUNC6_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC6_IN_INV_SEL_S  6
/** GPIO_SIG6_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG6_IN_SEL    (BIT(7))
#define GPIO_SIG6_IN_SEL_M  (GPIO_SIG6_IN_SEL_V << GPIO_SIG6_IN_SEL_S)
#define GPIO_SIG6_IN_SEL_V  0x00000001U
#define GPIO_SIG6_IN_SEL_S  7

/** GPIO_FUNC7_IN_SEL_CFG_REG register
 *  Configuration register for input signal 7
 */
#define GPIO_FUNC7_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e0)
/** GPIO_FUNC7_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 7.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC7_IN_SEL    0x0000003FU
#define GPIO_FUNC7_IN_SEL_M  (GPIO_FUNC7_IN_SEL_V << GPIO_FUNC7_IN_SEL_S)
#define GPIO_FUNC7_IN_SEL_V  0x0000003FU
#define GPIO_FUNC7_IN_SEL_S  0
/** GPIO_FUNC7_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC7_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC7_IN_INV_SEL_M  (GPIO_FUNC7_IN_INV_SEL_V << GPIO_FUNC7_IN_INV_SEL_S)
#define GPIO_FUNC7_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC7_IN_INV_SEL_S  6
/** GPIO_SIG7_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG7_IN_SEL    (BIT(7))
#define GPIO_SIG7_IN_SEL_M  (GPIO_SIG7_IN_SEL_V << GPIO_SIG7_IN_SEL_S)
#define GPIO_SIG7_IN_SEL_V  0x00000001U
#define GPIO_SIG7_IN_SEL_S  7

/** GPIO_FUNC8_IN_SEL_CFG_REG register
 *  Configuration register for input signal 8
 */
#define GPIO_FUNC8_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e4)
/** GPIO_FUNC8_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 8.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC8_IN_SEL    0x0000003FU
#define GPIO_FUNC8_IN_SEL_M  (GPIO_FUNC8_IN_SEL_V << GPIO_FUNC8_IN_SEL_S)
#define GPIO_FUNC8_IN_SEL_V  0x0000003FU
#define GPIO_FUNC8_IN_SEL_S  0
/** GPIO_FUNC8_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC8_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC8_IN_INV_SEL_M  (GPIO_FUNC8_IN_INV_SEL_V << GPIO_FUNC8_IN_INV_SEL_S)
#define GPIO_FUNC8_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC8_IN_INV_SEL_S  6
/** GPIO_SIG8_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG8_IN_SEL    (BIT(7))
#define GPIO_SIG8_IN_SEL_M  (GPIO_SIG8_IN_SEL_V << GPIO_SIG8_IN_SEL_S)
#define GPIO_SIG8_IN_SEL_V  0x00000001U
#define GPIO_SIG8_IN_SEL_S  7

/** GPIO_FUNC9_IN_SEL_CFG_REG register
 *  Configuration register for input signal 9
 */
#define GPIO_FUNC9_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2e8)
/** GPIO_FUNC9_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 9.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC9_IN_SEL    0x0000003FU
#define GPIO_FUNC9_IN_SEL_M  (GPIO_FUNC9_IN_SEL_V << GPIO_FUNC9_IN_SEL_S)
#define GPIO_FUNC9_IN_SEL_V  0x0000003FU
#define GPIO_FUNC9_IN_SEL_S  0
/** GPIO_FUNC9_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC9_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC9_IN_INV_SEL_M  (GPIO_FUNC9_IN_INV_SEL_V << GPIO_FUNC9_IN_INV_SEL_S)
#define GPIO_FUNC9_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC9_IN_INV_SEL_S  6
/** GPIO_SIG9_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG9_IN_SEL    (BIT(7))
#define GPIO_SIG9_IN_SEL_M  (GPIO_SIG9_IN_SEL_V << GPIO_SIG9_IN_SEL_S)
#define GPIO_SIG9_IN_SEL_V  0x00000001U
#define GPIO_SIG9_IN_SEL_S  7

/** GPIO_FUNC10_IN_SEL_CFG_REG register
 *  Configuration register for input signal 10
 */
#define GPIO_FUNC10_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2ec)
/** GPIO_FUNC10_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 10.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC10_IN_SEL    0x0000003FU
#define GPIO_FUNC10_IN_SEL_M  (GPIO_FUNC10_IN_SEL_V << GPIO_FUNC10_IN_SEL_S)
#define GPIO_FUNC10_IN_SEL_V  0x0000003FU
#define GPIO_FUNC10_IN_SEL_S  0
/** GPIO_FUNC10_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC10_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC10_IN_INV_SEL_M  (GPIO_FUNC10_IN_INV_SEL_V << GPIO_FUNC10_IN_INV_SEL_S)
#define GPIO_FUNC10_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC10_IN_INV_SEL_S  6
/** GPIO_SIG10_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG10_IN_SEL    (BIT(7))
#define GPIO_SIG10_IN_SEL_M  (GPIO_SIG10_IN_SEL_V << GPIO_SIG10_IN_SEL_S)
#define GPIO_SIG10_IN_SEL_V  0x00000001U
#define GPIO_SIG10_IN_SEL_S  7

/** GPIO_FUNC11_IN_SEL_CFG_REG register
 *  Configuration register for input signal 11
 */
#define GPIO_FUNC11_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f0)
/** GPIO_FUNC11_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 11.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC11_IN_SEL    0x0000003FU
#define GPIO_FUNC11_IN_SEL_M  (GPIO_FUNC11_IN_SEL_V << GPIO_FUNC11_IN_SEL_S)
#define GPIO_FUNC11_IN_SEL_V  0x0000003FU
#define GPIO_FUNC11_IN_SEL_S  0
/** GPIO_FUNC11_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC11_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC11_IN_INV_SEL_M  (GPIO_FUNC11_IN_INV_SEL_V << GPIO_FUNC11_IN_INV_SEL_S)
#define GPIO_FUNC11_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC11_IN_INV_SEL_S  6
/** GPIO_SIG11_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG11_IN_SEL    (BIT(7))
#define GPIO_SIG11_IN_SEL_M  (GPIO_SIG11_IN_SEL_V << GPIO_SIG11_IN_SEL_S)
#define GPIO_SIG11_IN_SEL_V  0x00000001U
#define GPIO_SIG11_IN_SEL_S  7

/** GPIO_FUNC12_IN_SEL_CFG_REG register
 *  Configuration register for input signal 12
 */
#define GPIO_FUNC12_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f4)
/** GPIO_FUNC12_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 12.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC12_IN_SEL    0x0000003FU
#define GPIO_FUNC12_IN_SEL_M  (GPIO_FUNC12_IN_SEL_V << GPIO_FUNC12_IN_SEL_S)
#define GPIO_FUNC12_IN_SEL_V  0x0000003FU
#define GPIO_FUNC12_IN_SEL_S  0
/** GPIO_FUNC12_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC12_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC12_IN_INV_SEL_M  (GPIO_FUNC12_IN_INV_SEL_V << GPIO_FUNC12_IN_INV_SEL_S)
#define GPIO_FUNC12_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC12_IN_INV_SEL_S  6
/** GPIO_SIG12_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG12_IN_SEL    (BIT(7))
#define GPIO_SIG12_IN_SEL_M  (GPIO_SIG12_IN_SEL_V << GPIO_SIG12_IN_SEL_S)
#define GPIO_SIG12_IN_SEL_V  0x00000001U
#define GPIO_SIG12_IN_SEL_S  7

/** GPIO_FUNC13_IN_SEL_CFG_REG register
 *  Configuration register for input signal 13
 */
#define GPIO_FUNC13_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2f8)
/** GPIO_FUNC13_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 13.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC13_IN_SEL    0x0000003FU
#define GPIO_FUNC13_IN_SEL_M  (GPIO_FUNC13_IN_SEL_V << GPIO_FUNC13_IN_SEL_S)
#define GPIO_FUNC13_IN_SEL_V  0x0000003FU
#define GPIO_FUNC13_IN_SEL_S  0
/** GPIO_FUNC13_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC13_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC13_IN_INV_SEL_M  (GPIO_FUNC13_IN_INV_SEL_V << GPIO_FUNC13_IN_INV_SEL_S)
#define GPIO_FUNC13_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC13_IN_INV_SEL_S  6
/** GPIO_SIG13_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG13_IN_SEL    (BIT(7))
#define GPIO_SIG13_IN_SEL_M  (GPIO_SIG13_IN_SEL_V << GPIO_SIG13_IN_SEL_S)
#define GPIO_SIG13_IN_SEL_V  0x00000001U
#define GPIO_SIG13_IN_SEL_S  7

/** GPIO_FUNC14_IN_SEL_CFG_REG register
 *  Configuration register for input signal 14
 */
#define GPIO_FUNC14_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x2fc)
/** GPIO_FUNC14_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 14.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC14_IN_SEL    0x0000003FU
#define GPIO_FUNC14_IN_SEL_M  (GPIO_FUNC14_IN_SEL_V << GPIO_FUNC14_IN_SEL_S)
#define GPIO_FUNC14_IN_SEL_V  0x0000003FU
#define GPIO_FUNC14_IN_SEL_S  0
/** GPIO_FUNC14_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC14_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC14_IN_INV_SEL_M  (GPIO_FUNC14_IN_INV_SEL_V << GPIO_FUNC14_IN_INV_SEL_S)
#define GPIO_FUNC14_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC14_IN_INV_SEL_S  6
/** GPIO_SIG14_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG14_IN_SEL    (BIT(7))
#define GPIO_SIG14_IN_SEL_M  (GPIO_SIG14_IN_SEL_V << GPIO_SIG14_IN_SEL_S)
#define GPIO_SIG14_IN_SEL_V  0x00000001U
#define GPIO_SIG14_IN_SEL_S  7

/** GPIO_FUNC15_IN_SEL_CFG_REG register
 *  Configuration register for input signal 15
 */
#define GPIO_FUNC15_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x300)
/** GPIO_FUNC15_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 15.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC15_IN_SEL    0x0000003FU
#define GPIO_FUNC15_IN_SEL_M  (GPIO_FUNC15_IN_SEL_V << GPIO_FUNC15_IN_SEL_S)
#define GPIO_FUNC15_IN_SEL_V  0x0000003FU
#define GPIO_FUNC15_IN_SEL_S  0
/** GPIO_FUNC15_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC15_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC15_IN_INV_SEL_M  (GPIO_FUNC15_IN_INV_SEL_V << GPIO_FUNC15_IN_INV_SEL_S)
#define GPIO_FUNC15_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC15_IN_INV_SEL_S  6
/** GPIO_SIG15_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG15_IN_SEL    (BIT(7))
#define GPIO_SIG15_IN_SEL_M  (GPIO_SIG15_IN_SEL_V << GPIO_SIG15_IN_SEL_S)
#define GPIO_SIG15_IN_SEL_V  0x00000001U
#define GPIO_SIG15_IN_SEL_S  7

/** GPIO_FUNC16_IN_SEL_CFG_REG register
 *  Configuration register for input signal 16
 */
#define GPIO_FUNC16_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x304)
/** GPIO_FUNC16_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 16.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC16_IN_SEL    0x0000003FU
#define GPIO_FUNC16_IN_SEL_M  (GPIO_FUNC16_IN_SEL_V << GPIO_FUNC16_IN_SEL_S)
#define GPIO_FUNC16_IN_SEL_V  0x0000003FU
#define GPIO_FUNC16_IN_SEL_S  0
/** GPIO_FUNC16_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC16_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC16_IN_INV_SEL_M  (GPIO_FUNC16_IN_INV_SEL_V << GPIO_FUNC16_IN_INV_SEL_S)
#define GPIO_FUNC16_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC16_IN_INV_SEL_S  6
/** GPIO_SIG16_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG16_IN_SEL    (BIT(7))
#define GPIO_SIG16_IN_SEL_M  (GPIO_SIG16_IN_SEL_V << GPIO_SIG16_IN_SEL_S)
#define GPIO_SIG16_IN_SEL_V  0x00000001U
#define GPIO_SIG16_IN_SEL_S  7

/** GPIO_FUNC17_IN_SEL_CFG_REG register
 *  Configuration register for input signal 17
 */
#define GPIO_FUNC17_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x308)
/** GPIO_FUNC17_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 17.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC17_IN_SEL    0x0000003FU
#define GPIO_FUNC17_IN_SEL_M  (GPIO_FUNC17_IN_SEL_V << GPIO_FUNC17_IN_SEL_S)
#define GPIO_FUNC17_IN_SEL_V  0x0000003FU
#define GPIO_FUNC17_IN_SEL_S  0
/** GPIO_FUNC17_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC17_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC17_IN_INV_SEL_M  (GPIO_FUNC17_IN_INV_SEL_V << GPIO_FUNC17_IN_INV_SEL_S)
#define GPIO_FUNC17_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC17_IN_INV_SEL_S  6
/** GPIO_SIG17_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG17_IN_SEL    (BIT(7))
#define GPIO_SIG17_IN_SEL_M  (GPIO_SIG17_IN_SEL_V << GPIO_SIG17_IN_SEL_S)
#define GPIO_SIG17_IN_SEL_V  0x00000001U
#define GPIO_SIG17_IN_SEL_S  7

/** GPIO_FUNC27_IN_SEL_CFG_REG register
 *  Configuration register for input signal 27
 */
#define GPIO_FUNC27_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x330)
/** GPIO_FUNC27_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 27.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC27_IN_SEL    0x0000003FU
#define GPIO_FUNC27_IN_SEL_M  (GPIO_FUNC27_IN_SEL_V << GPIO_FUNC27_IN_SEL_S)
#define GPIO_FUNC27_IN_SEL_V  0x0000003FU
#define GPIO_FUNC27_IN_SEL_S  0
/** GPIO_FUNC27_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC27_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC27_IN_INV_SEL_M  (GPIO_FUNC27_IN_INV_SEL_V << GPIO_FUNC27_IN_INV_SEL_S)
#define GPIO_FUNC27_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC27_IN_INV_SEL_S  6
/** GPIO_SIG27_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG27_IN_SEL    (BIT(7))
#define GPIO_SIG27_IN_SEL_M  (GPIO_SIG27_IN_SEL_V << GPIO_SIG27_IN_SEL_S)
#define GPIO_SIG27_IN_SEL_V  0x00000001U
#define GPIO_SIG27_IN_SEL_S  7

/** GPIO_FUNC28_IN_SEL_CFG_REG register
 *  Configuration register for input signal 28
 */
#define GPIO_FUNC28_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x334)
/** GPIO_FUNC28_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 28.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC28_IN_SEL    0x0000003FU
#define GPIO_FUNC28_IN_SEL_M  (GPIO_FUNC28_IN_SEL_V << GPIO_FUNC28_IN_SEL_S)
#define GPIO_FUNC28_IN_SEL_V  0x0000003FU
#define GPIO_FUNC28_IN_SEL_S  0
/** GPIO_FUNC28_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC28_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC28_IN_INV_SEL_M  (GPIO_FUNC28_IN_INV_SEL_V << GPIO_FUNC28_IN_INV_SEL_S)
#define GPIO_FUNC28_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC28_IN_INV_SEL_S  6
/** GPIO_SIG28_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG28_IN_SEL    (BIT(7))
#define GPIO_SIG28_IN_SEL_M  (GPIO_SIG28_IN_SEL_V << GPIO_SIG28_IN_SEL_S)
#define GPIO_SIG28_IN_SEL_V  0x00000001U
#define GPIO_SIG28_IN_SEL_S  7

/** GPIO_FUNC29_IN_SEL_CFG_REG register
 *  Configuration register for input signal 29
 */
#define GPIO_FUNC29_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x338)
/** GPIO_FUNC29_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 29.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC29_IN_SEL    0x0000003FU
#define GPIO_FUNC29_IN_SEL_M  (GPIO_FUNC29_IN_SEL_V << GPIO_FUNC29_IN_SEL_S)
#define GPIO_FUNC29_IN_SEL_V  0x0000003FU
#define GPIO_FUNC29_IN_SEL_S  0
/** GPIO_FUNC29_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC29_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC29_IN_INV_SEL_M  (GPIO_FUNC29_IN_INV_SEL_V << GPIO_FUNC29_IN_INV_SEL_S)
#define GPIO_FUNC29_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC29_IN_INV_SEL_S  6
/** GPIO_SIG29_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG29_IN_SEL    (BIT(7))
#define GPIO_SIG29_IN_SEL_M  (GPIO_SIG29_IN_SEL_V << GPIO_SIG29_IN_SEL_S)
#define GPIO_SIG29_IN_SEL_V  0x00000001U
#define GPIO_SIG29_IN_SEL_S  7

/** GPIO_FUNC30_IN_SEL_CFG_REG register
 *  Configuration register for input signal 30
 */
#define GPIO_FUNC30_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x33c)
/** GPIO_FUNC30_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 30.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC30_IN_SEL    0x0000003FU
#define GPIO_FUNC30_IN_SEL_M  (GPIO_FUNC30_IN_SEL_V << GPIO_FUNC30_IN_SEL_S)
#define GPIO_FUNC30_IN_SEL_V  0x0000003FU
#define GPIO_FUNC30_IN_SEL_S  0
/** GPIO_FUNC30_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC30_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC30_IN_INV_SEL_M  (GPIO_FUNC30_IN_INV_SEL_V << GPIO_FUNC30_IN_INV_SEL_S)
#define GPIO_FUNC30_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC30_IN_INV_SEL_S  6
/** GPIO_SIG30_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG30_IN_SEL    (BIT(7))
#define GPIO_SIG30_IN_SEL_M  (GPIO_SIG30_IN_SEL_V << GPIO_SIG30_IN_SEL_S)
#define GPIO_SIG30_IN_SEL_V  0x00000001U
#define GPIO_SIG30_IN_SEL_S  7

/** GPIO_FUNC31_IN_SEL_CFG_REG register
 *  Configuration register for input signal 31
 */
#define GPIO_FUNC31_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x340)
/** GPIO_FUNC31_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 31.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC31_IN_SEL    0x0000003FU
#define GPIO_FUNC31_IN_SEL_M  (GPIO_FUNC31_IN_SEL_V << GPIO_FUNC31_IN_SEL_S)
#define GPIO_FUNC31_IN_SEL_V  0x0000003FU
#define GPIO_FUNC31_IN_SEL_S  0
/** GPIO_FUNC31_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC31_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC31_IN_INV_SEL_M  (GPIO_FUNC31_IN_INV_SEL_V << GPIO_FUNC31_IN_INV_SEL_S)
#define GPIO_FUNC31_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC31_IN_INV_SEL_S  6
/** GPIO_SIG31_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG31_IN_SEL    (BIT(7))
#define GPIO_SIG31_IN_SEL_M  (GPIO_SIG31_IN_SEL_V << GPIO_SIG31_IN_SEL_S)
#define GPIO_SIG31_IN_SEL_V  0x00000001U
#define GPIO_SIG31_IN_SEL_S  7

/** GPIO_FUNC32_IN_SEL_CFG_REG register
 *  Configuration register for input signal 32
 */
#define GPIO_FUNC32_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x344)
/** GPIO_FUNC32_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 32.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC32_IN_SEL    0x0000003FU
#define GPIO_FUNC32_IN_SEL_M  (GPIO_FUNC32_IN_SEL_V << GPIO_FUNC32_IN_SEL_S)
#define GPIO_FUNC32_IN_SEL_V  0x0000003FU
#define GPIO_FUNC32_IN_SEL_S  0
/** GPIO_FUNC32_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC32_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC32_IN_INV_SEL_M  (GPIO_FUNC32_IN_INV_SEL_V << GPIO_FUNC32_IN_INV_SEL_S)
#define GPIO_FUNC32_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC32_IN_INV_SEL_S  6
/** GPIO_SIG32_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG32_IN_SEL    (BIT(7))
#define GPIO_SIG32_IN_SEL_M  (GPIO_SIG32_IN_SEL_V << GPIO_SIG32_IN_SEL_S)
#define GPIO_SIG32_IN_SEL_V  0x00000001U
#define GPIO_SIG32_IN_SEL_S  7

/** GPIO_FUNC33_IN_SEL_CFG_REG register
 *  Configuration register for input signal 33
 */
#define GPIO_FUNC33_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x348)
/** GPIO_FUNC33_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 33.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC33_IN_SEL    0x0000003FU
#define GPIO_FUNC33_IN_SEL_M  (GPIO_FUNC33_IN_SEL_V << GPIO_FUNC33_IN_SEL_S)
#define GPIO_FUNC33_IN_SEL_V  0x0000003FU
#define GPIO_FUNC33_IN_SEL_S  0
/** GPIO_FUNC33_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC33_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC33_IN_INV_SEL_M  (GPIO_FUNC33_IN_INV_SEL_V << GPIO_FUNC33_IN_INV_SEL_S)
#define GPIO_FUNC33_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC33_IN_INV_SEL_S  6
/** GPIO_SIG33_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG33_IN_SEL    (BIT(7))
#define GPIO_SIG33_IN_SEL_M  (GPIO_SIG33_IN_SEL_V << GPIO_SIG33_IN_SEL_S)
#define GPIO_SIG33_IN_SEL_V  0x00000001U
#define GPIO_SIG33_IN_SEL_S  7

/** GPIO_FUNC34_IN_SEL_CFG_REG register
 *  Configuration register for input signal 34
 */
#define GPIO_FUNC34_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x34c)
/** GPIO_FUNC34_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 34.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC34_IN_SEL    0x0000003FU
#define GPIO_FUNC34_IN_SEL_M  (GPIO_FUNC34_IN_SEL_V << GPIO_FUNC34_IN_SEL_S)
#define GPIO_FUNC34_IN_SEL_V  0x0000003FU
#define GPIO_FUNC34_IN_SEL_S  0
/** GPIO_FUNC34_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC34_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC34_IN_INV_SEL_M  (GPIO_FUNC34_IN_INV_SEL_V << GPIO_FUNC34_IN_INV_SEL_S)
#define GPIO_FUNC34_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC34_IN_INV_SEL_S  6
/** GPIO_SIG34_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG34_IN_SEL    (BIT(7))
#define GPIO_SIG34_IN_SEL_M  (GPIO_SIG34_IN_SEL_V << GPIO_SIG34_IN_SEL_S)
#define GPIO_SIG34_IN_SEL_V  0x00000001U
#define GPIO_SIG34_IN_SEL_S  7

/** GPIO_FUNC35_IN_SEL_CFG_REG register
 *  Configuration register for input signal 35
 */
#define GPIO_FUNC35_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x350)
/** GPIO_FUNC35_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 35.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC35_IN_SEL    0x0000003FU
#define GPIO_FUNC35_IN_SEL_M  (GPIO_FUNC35_IN_SEL_V << GPIO_FUNC35_IN_SEL_S)
#define GPIO_FUNC35_IN_SEL_V  0x0000003FU
#define GPIO_FUNC35_IN_SEL_S  0
/** GPIO_FUNC35_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC35_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC35_IN_INV_SEL_M  (GPIO_FUNC35_IN_INV_SEL_V << GPIO_FUNC35_IN_INV_SEL_S)
#define GPIO_FUNC35_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC35_IN_INV_SEL_S  6
/** GPIO_SIG35_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG35_IN_SEL    (BIT(7))
#define GPIO_SIG35_IN_SEL_M  (GPIO_SIG35_IN_SEL_V << GPIO_SIG35_IN_SEL_S)
#define GPIO_SIG35_IN_SEL_V  0x00000001U
#define GPIO_SIG35_IN_SEL_S  7

/** GPIO_FUNC41_IN_SEL_CFG_REG register
 *  Configuration register for input signal 41
 */
#define GPIO_FUNC41_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x368)
/** GPIO_FUNC41_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 41.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC41_IN_SEL    0x0000003FU
#define GPIO_FUNC41_IN_SEL_M  (GPIO_FUNC41_IN_SEL_V << GPIO_FUNC41_IN_SEL_S)
#define GPIO_FUNC41_IN_SEL_V  0x0000003FU
#define GPIO_FUNC41_IN_SEL_S  0
/** GPIO_FUNC41_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC41_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC41_IN_INV_SEL_M  (GPIO_FUNC41_IN_INV_SEL_V << GPIO_FUNC41_IN_INV_SEL_S)
#define GPIO_FUNC41_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC41_IN_INV_SEL_S  6
/** GPIO_SIG41_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG41_IN_SEL    (BIT(7))
#define GPIO_SIG41_IN_SEL_M  (GPIO_SIG41_IN_SEL_V << GPIO_SIG41_IN_SEL_S)
#define GPIO_SIG41_IN_SEL_V  0x00000001U
#define GPIO_SIG41_IN_SEL_S  7

/** GPIO_FUNC42_IN_SEL_CFG_REG register
 *  Configuration register for input signal 42
 */
#define GPIO_FUNC42_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x36c)
/** GPIO_FUNC42_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 42.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC42_IN_SEL    0x0000003FU
#define GPIO_FUNC42_IN_SEL_M  (GPIO_FUNC42_IN_SEL_V << GPIO_FUNC42_IN_SEL_S)
#define GPIO_FUNC42_IN_SEL_V  0x0000003FU
#define GPIO_FUNC42_IN_SEL_S  0
/** GPIO_FUNC42_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC42_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC42_IN_INV_SEL_M  (GPIO_FUNC42_IN_INV_SEL_V << GPIO_FUNC42_IN_INV_SEL_S)
#define GPIO_FUNC42_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC42_IN_INV_SEL_S  6
/** GPIO_SIG42_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG42_IN_SEL    (BIT(7))
#define GPIO_SIG42_IN_SEL_M  (GPIO_SIG42_IN_SEL_V << GPIO_SIG42_IN_SEL_S)
#define GPIO_SIG42_IN_SEL_V  0x00000001U
#define GPIO_SIG42_IN_SEL_S  7

/** GPIO_FUNC43_IN_SEL_CFG_REG register
 *  Configuration register for input signal 43
 */
#define GPIO_FUNC43_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x370)
/** GPIO_FUNC43_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 43.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC43_IN_SEL    0x0000003FU
#define GPIO_FUNC43_IN_SEL_M  (GPIO_FUNC43_IN_SEL_V << GPIO_FUNC43_IN_SEL_S)
#define GPIO_FUNC43_IN_SEL_V  0x0000003FU
#define GPIO_FUNC43_IN_SEL_S  0
/** GPIO_FUNC43_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC43_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC43_IN_INV_SEL_M  (GPIO_FUNC43_IN_INV_SEL_V << GPIO_FUNC43_IN_INV_SEL_S)
#define GPIO_FUNC43_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC43_IN_INV_SEL_S  6
/** GPIO_SIG43_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG43_IN_SEL    (BIT(7))
#define GPIO_SIG43_IN_SEL_M  (GPIO_SIG43_IN_SEL_V << GPIO_SIG43_IN_SEL_S)
#define GPIO_SIG43_IN_SEL_V  0x00000001U
#define GPIO_SIG43_IN_SEL_S  7

/** GPIO_FUNC46_IN_SEL_CFG_REG register
 *  Configuration register for input signal 46
 */
#define GPIO_FUNC46_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x37c)
/** GPIO_FUNC46_IN_SEL : R/W; bitpos: [5:0]; default: 32;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 46.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC46_IN_SEL    0x0000003FU
#define GPIO_FUNC46_IN_SEL_M  (GPIO_FUNC46_IN_SEL_V << GPIO_FUNC46_IN_SEL_S)
#define GPIO_FUNC46_IN_SEL_V  0x0000003FU
#define GPIO_FUNC46_IN_SEL_S  0
/** GPIO_FUNC46_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC46_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC46_IN_INV_SEL_M  (GPIO_FUNC46_IN_INV_SEL_V << GPIO_FUNC46_IN_INV_SEL_S)
#define GPIO_FUNC46_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC46_IN_INV_SEL_S  6
/** GPIO_SIG46_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG46_IN_SEL    (BIT(7))
#define GPIO_SIG46_IN_SEL_M  (GPIO_SIG46_IN_SEL_V << GPIO_SIG46_IN_SEL_S)
#define GPIO_SIG46_IN_SEL_V  0x00000001U
#define GPIO_SIG46_IN_SEL_S  7

/** GPIO_FUNC47_IN_SEL_CFG_REG register
 *  Configuration register for input signal 47
 */
#define GPIO_FUNC47_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x380)
/** GPIO_FUNC47_IN_SEL : R/W; bitpos: [5:0]; default: 32;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 47.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC47_IN_SEL    0x0000003FU
#define GPIO_FUNC47_IN_SEL_M  (GPIO_FUNC47_IN_SEL_V << GPIO_FUNC47_IN_SEL_S)
#define GPIO_FUNC47_IN_SEL_V  0x0000003FU
#define GPIO_FUNC47_IN_SEL_S  0
/** GPIO_FUNC47_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC47_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC47_IN_INV_SEL_M  (GPIO_FUNC47_IN_INV_SEL_V << GPIO_FUNC47_IN_INV_SEL_S)
#define GPIO_FUNC47_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC47_IN_INV_SEL_S  6
/** GPIO_SIG47_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG47_IN_SEL    (BIT(7))
#define GPIO_SIG47_IN_SEL_M  (GPIO_SIG47_IN_SEL_V << GPIO_SIG47_IN_SEL_S)
#define GPIO_SIG47_IN_SEL_V  0x00000001U
#define GPIO_SIG47_IN_SEL_S  7

/** GPIO_FUNC64_IN_SEL_CFG_REG register
 *  Configuration register for input signal 64
 */
#define GPIO_FUNC64_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c4)
/** GPIO_FUNC64_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 64.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC64_IN_SEL    0x0000003FU
#define GPIO_FUNC64_IN_SEL_M  (GPIO_FUNC64_IN_SEL_V << GPIO_FUNC64_IN_SEL_S)
#define GPIO_FUNC64_IN_SEL_V  0x0000003FU
#define GPIO_FUNC64_IN_SEL_S  0
/** GPIO_FUNC64_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC64_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC64_IN_INV_SEL_M  (GPIO_FUNC64_IN_INV_SEL_V << GPIO_FUNC64_IN_INV_SEL_S)
#define GPIO_FUNC64_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC64_IN_INV_SEL_S  6
/** GPIO_SIG64_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG64_IN_SEL    (BIT(7))
#define GPIO_SIG64_IN_SEL_M  (GPIO_SIG64_IN_SEL_V << GPIO_SIG64_IN_SEL_S)
#define GPIO_SIG64_IN_SEL_V  0x00000001U
#define GPIO_SIG64_IN_SEL_S  7

/** GPIO_FUNC65_IN_SEL_CFG_REG register
 *  Configuration register for input signal 65
 */
#define GPIO_FUNC65_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3c8)
/** GPIO_FUNC65_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 65.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC65_IN_SEL    0x0000003FU
#define GPIO_FUNC65_IN_SEL_M  (GPIO_FUNC65_IN_SEL_V << GPIO_FUNC65_IN_SEL_S)
#define GPIO_FUNC65_IN_SEL_V  0x0000003FU
#define GPIO_FUNC65_IN_SEL_S  0
/** GPIO_FUNC65_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC65_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC65_IN_INV_SEL_M  (GPIO_FUNC65_IN_INV_SEL_V << GPIO_FUNC65_IN_INV_SEL_S)
#define GPIO_FUNC65_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC65_IN_INV_SEL_S  6
/** GPIO_SIG65_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG65_IN_SEL    (BIT(7))
#define GPIO_SIG65_IN_SEL_M  (GPIO_SIG65_IN_SEL_V << GPIO_SIG65_IN_SEL_S)
#define GPIO_SIG65_IN_SEL_V  0x00000001U
#define GPIO_SIG65_IN_SEL_S  7

/** GPIO_FUNC66_IN_SEL_CFG_REG register
 *  Configuration register for input signal 66
 */
#define GPIO_FUNC66_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3cc)
/** GPIO_FUNC66_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 66.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC66_IN_SEL    0x0000003FU
#define GPIO_FUNC66_IN_SEL_M  (GPIO_FUNC66_IN_SEL_V << GPIO_FUNC66_IN_SEL_S)
#define GPIO_FUNC66_IN_SEL_V  0x0000003FU
#define GPIO_FUNC66_IN_SEL_S  0
/** GPIO_FUNC66_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC66_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC66_IN_INV_SEL_M  (GPIO_FUNC66_IN_INV_SEL_V << GPIO_FUNC66_IN_INV_SEL_S)
#define GPIO_FUNC66_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC66_IN_INV_SEL_S  6
/** GPIO_SIG66_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG66_IN_SEL    (BIT(7))
#define GPIO_SIG66_IN_SEL_M  (GPIO_SIG66_IN_SEL_V << GPIO_SIG66_IN_SEL_S)
#define GPIO_SIG66_IN_SEL_V  0x00000001U
#define GPIO_SIG66_IN_SEL_S  7

/** GPIO_FUNC67_IN_SEL_CFG_REG register
 *  Configuration register for input signal 67
 */
#define GPIO_FUNC67_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d0)
/** GPIO_FUNC67_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 67.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC67_IN_SEL    0x0000003FU
#define GPIO_FUNC67_IN_SEL_M  (GPIO_FUNC67_IN_SEL_V << GPIO_FUNC67_IN_SEL_S)
#define GPIO_FUNC67_IN_SEL_V  0x0000003FU
#define GPIO_FUNC67_IN_SEL_S  0
/** GPIO_FUNC67_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC67_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC67_IN_INV_SEL_M  (GPIO_FUNC67_IN_INV_SEL_V << GPIO_FUNC67_IN_INV_SEL_S)
#define GPIO_FUNC67_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC67_IN_INV_SEL_S  6
/** GPIO_SIG67_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG67_IN_SEL    (BIT(7))
#define GPIO_SIG67_IN_SEL_M  (GPIO_SIG67_IN_SEL_V << GPIO_SIG67_IN_SEL_S)
#define GPIO_SIG67_IN_SEL_V  0x00000001U
#define GPIO_SIG67_IN_SEL_S  7

/** GPIO_FUNC68_IN_SEL_CFG_REG register
 *  Configuration register for input signal 68
 */
#define GPIO_FUNC68_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d4)
/** GPIO_FUNC68_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 68.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC68_IN_SEL    0x0000003FU
#define GPIO_FUNC68_IN_SEL_M  (GPIO_FUNC68_IN_SEL_V << GPIO_FUNC68_IN_SEL_S)
#define GPIO_FUNC68_IN_SEL_V  0x0000003FU
#define GPIO_FUNC68_IN_SEL_S  0
/** GPIO_FUNC68_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC68_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC68_IN_INV_SEL_M  (GPIO_FUNC68_IN_INV_SEL_V << GPIO_FUNC68_IN_INV_SEL_S)
#define GPIO_FUNC68_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC68_IN_INV_SEL_S  6
/** GPIO_SIG68_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG68_IN_SEL    (BIT(7))
#define GPIO_SIG68_IN_SEL_M  (GPIO_SIG68_IN_SEL_V << GPIO_SIG68_IN_SEL_S)
#define GPIO_SIG68_IN_SEL_V  0x00000001U
#define GPIO_SIG68_IN_SEL_S  7

/** GPIO_FUNC69_IN_SEL_CFG_REG register
 *  Configuration register for input signal 69
 */
#define GPIO_FUNC69_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3d8)
/** GPIO_FUNC69_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 69.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC69_IN_SEL    0x0000003FU
#define GPIO_FUNC69_IN_SEL_M  (GPIO_FUNC69_IN_SEL_V << GPIO_FUNC69_IN_SEL_S)
#define GPIO_FUNC69_IN_SEL_V  0x0000003FU
#define GPIO_FUNC69_IN_SEL_S  0
/** GPIO_FUNC69_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC69_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC69_IN_INV_SEL_M  (GPIO_FUNC69_IN_INV_SEL_V << GPIO_FUNC69_IN_INV_SEL_S)
#define GPIO_FUNC69_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC69_IN_INV_SEL_S  6
/** GPIO_SIG69_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG69_IN_SEL    (BIT(7))
#define GPIO_SIG69_IN_SEL_M  (GPIO_SIG69_IN_SEL_V << GPIO_SIG69_IN_SEL_S)
#define GPIO_SIG69_IN_SEL_V  0x00000001U
#define GPIO_SIG69_IN_SEL_S  7

/** GPIO_FUNC72_IN_SEL_CFG_REG register
 *  Configuration register for input signal 72
 */
#define GPIO_FUNC72_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e4)
/** GPIO_FUNC72_IN_SEL : R/W; bitpos: [5:0]; default: 32;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 72.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC72_IN_SEL    0x0000003FU
#define GPIO_FUNC72_IN_SEL_M  (GPIO_FUNC72_IN_SEL_V << GPIO_FUNC72_IN_SEL_S)
#define GPIO_FUNC72_IN_SEL_V  0x0000003FU
#define GPIO_FUNC72_IN_SEL_S  0
/** GPIO_FUNC72_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC72_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC72_IN_INV_SEL_M  (GPIO_FUNC72_IN_INV_SEL_V << GPIO_FUNC72_IN_INV_SEL_S)
#define GPIO_FUNC72_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC72_IN_INV_SEL_S  6
/** GPIO_SIG72_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG72_IN_SEL    (BIT(7))
#define GPIO_SIG72_IN_SEL_M  (GPIO_SIG72_IN_SEL_V << GPIO_SIG72_IN_SEL_S)
#define GPIO_SIG72_IN_SEL_V  0x00000001U
#define GPIO_SIG72_IN_SEL_S  7

/** GPIO_FUNC73_IN_SEL_CFG_REG register
 *  Configuration register for input signal 73
 */
#define GPIO_FUNC73_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3e8)
/** GPIO_FUNC73_IN_SEL : R/W; bitpos: [5:0]; default: 32;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 73.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC73_IN_SEL    0x0000003FU
#define GPIO_FUNC73_IN_SEL_M  (GPIO_FUNC73_IN_SEL_V << GPIO_FUNC73_IN_SEL_S)
#define GPIO_FUNC73_IN_SEL_V  0x0000003FU
#define GPIO_FUNC73_IN_SEL_S  0
/** GPIO_FUNC73_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC73_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC73_IN_INV_SEL_M  (GPIO_FUNC73_IN_INV_SEL_V << GPIO_FUNC73_IN_INV_SEL_S)
#define GPIO_FUNC73_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC73_IN_INV_SEL_S  6
/** GPIO_SIG73_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG73_IN_SEL    (BIT(7))
#define GPIO_SIG73_IN_SEL_M  (GPIO_SIG73_IN_SEL_V << GPIO_SIG73_IN_SEL_S)
#define GPIO_SIG73_IN_SEL_V  0x00000001U
#define GPIO_SIG73_IN_SEL_S  7

/** GPIO_FUNC74_IN_SEL_CFG_REG register
 *  Configuration register for input signal 74
 */
#define GPIO_FUNC74_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x3ec)
/** GPIO_FUNC74_IN_SEL : R/W; bitpos: [5:0]; default: 32;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 74.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC74_IN_SEL    0x0000003FU
#define GPIO_FUNC74_IN_SEL_M  (GPIO_FUNC74_IN_SEL_V << GPIO_FUNC74_IN_SEL_S)
#define GPIO_FUNC74_IN_SEL_V  0x0000003FU
#define GPIO_FUNC74_IN_SEL_S  0
/** GPIO_FUNC74_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC74_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC74_IN_INV_SEL_M  (GPIO_FUNC74_IN_INV_SEL_V << GPIO_FUNC74_IN_INV_SEL_S)
#define GPIO_FUNC74_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC74_IN_INV_SEL_S  6
/** GPIO_SIG74_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG74_IN_SEL    (BIT(7))
#define GPIO_SIG74_IN_SEL_M  (GPIO_SIG74_IN_SEL_V << GPIO_SIG74_IN_SEL_S)
#define GPIO_SIG74_IN_SEL_V  0x00000001U
#define GPIO_SIG74_IN_SEL_S  7

/** GPIO_FUNC82_IN_SEL_CFG_REG register
 *  Configuration register for input signal 82
 */
#define GPIO_FUNC82_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x40c)
/** GPIO_FUNC82_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 82.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC82_IN_SEL    0x0000003FU
#define GPIO_FUNC82_IN_SEL_M  (GPIO_FUNC82_IN_SEL_V << GPIO_FUNC82_IN_SEL_S)
#define GPIO_FUNC82_IN_SEL_V  0x0000003FU
#define GPIO_FUNC82_IN_SEL_S  0
/** GPIO_FUNC82_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC82_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC82_IN_INV_SEL_M  (GPIO_FUNC82_IN_INV_SEL_V << GPIO_FUNC82_IN_INV_SEL_S)
#define GPIO_FUNC82_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC82_IN_INV_SEL_S  6
/** GPIO_SIG82_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG82_IN_SEL    (BIT(7))
#define GPIO_SIG82_IN_SEL_M  (GPIO_SIG82_IN_SEL_V << GPIO_SIG82_IN_SEL_S)
#define GPIO_SIG82_IN_SEL_V  0x00000001U
#define GPIO_SIG82_IN_SEL_S  7

/** GPIO_FUNC83_IN_SEL_CFG_REG register
 *  Configuration register for input signal 83
 */
#define GPIO_FUNC83_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x410)
/** GPIO_FUNC83_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 83.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC83_IN_SEL    0x0000003FU
#define GPIO_FUNC83_IN_SEL_M  (GPIO_FUNC83_IN_SEL_V << GPIO_FUNC83_IN_SEL_S)
#define GPIO_FUNC83_IN_SEL_V  0x0000003FU
#define GPIO_FUNC83_IN_SEL_S  0
/** GPIO_FUNC83_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC83_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC83_IN_INV_SEL_M  (GPIO_FUNC83_IN_INV_SEL_V << GPIO_FUNC83_IN_INV_SEL_S)
#define GPIO_FUNC83_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC83_IN_INV_SEL_S  6
/** GPIO_SIG83_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG83_IN_SEL    (BIT(7))
#define GPIO_SIG83_IN_SEL_M  (GPIO_SIG83_IN_SEL_V << GPIO_SIG83_IN_SEL_S)
#define GPIO_SIG83_IN_SEL_V  0x00000001U
#define GPIO_SIG83_IN_SEL_S  7

/** GPIO_FUNC97_IN_SEL_CFG_REG register
 *  Configuration register for input signal 97
 */
#define GPIO_FUNC97_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x448)
/** GPIO_FUNC97_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 97.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC97_IN_SEL    0x0000003FU
#define GPIO_FUNC97_IN_SEL_M  (GPIO_FUNC97_IN_SEL_V << GPIO_FUNC97_IN_SEL_S)
#define GPIO_FUNC97_IN_SEL_V  0x0000003FU
#define GPIO_FUNC97_IN_SEL_S  0
/** GPIO_FUNC97_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC97_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC97_IN_INV_SEL_M  (GPIO_FUNC97_IN_INV_SEL_V << GPIO_FUNC97_IN_INV_SEL_S)
#define GPIO_FUNC97_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC97_IN_INV_SEL_S  6
/** GPIO_SIG97_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG97_IN_SEL    (BIT(7))
#define GPIO_SIG97_IN_SEL_M  (GPIO_SIG97_IN_SEL_V << GPIO_SIG97_IN_SEL_S)
#define GPIO_SIG97_IN_SEL_V  0x00000001U
#define GPIO_SIG97_IN_SEL_S  7

/** GPIO_FUNC98_IN_SEL_CFG_REG register
 *  Configuration register for input signal 98
 */
#define GPIO_FUNC98_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x44c)
/** GPIO_FUNC98_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 98.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC98_IN_SEL    0x0000003FU
#define GPIO_FUNC98_IN_SEL_M  (GPIO_FUNC98_IN_SEL_V << GPIO_FUNC98_IN_SEL_S)
#define GPIO_FUNC98_IN_SEL_V  0x0000003FU
#define GPIO_FUNC98_IN_SEL_S  0
/** GPIO_FUNC98_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC98_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC98_IN_INV_SEL_M  (GPIO_FUNC98_IN_INV_SEL_V << GPIO_FUNC98_IN_INV_SEL_S)
#define GPIO_FUNC98_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC98_IN_INV_SEL_S  6
/** GPIO_SIG98_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG98_IN_SEL    (BIT(7))
#define GPIO_SIG98_IN_SEL_M  (GPIO_SIG98_IN_SEL_V << GPIO_SIG98_IN_SEL_S)
#define GPIO_SIG98_IN_SEL_V  0x00000001U
#define GPIO_SIG98_IN_SEL_S  7

/** GPIO_FUNC99_IN_SEL_CFG_REG register
 *  Configuration register for input signal 99
 */
#define GPIO_FUNC99_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x450)
/** GPIO_FUNC99_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 99.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC99_IN_SEL    0x0000003FU
#define GPIO_FUNC99_IN_SEL_M  (GPIO_FUNC99_IN_SEL_V << GPIO_FUNC99_IN_SEL_S)
#define GPIO_FUNC99_IN_SEL_V  0x0000003FU
#define GPIO_FUNC99_IN_SEL_S  0
/** GPIO_FUNC99_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC99_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC99_IN_INV_SEL_M  (GPIO_FUNC99_IN_INV_SEL_V << GPIO_FUNC99_IN_INV_SEL_S)
#define GPIO_FUNC99_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC99_IN_INV_SEL_S  6
/** GPIO_SIG99_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG99_IN_SEL    (BIT(7))
#define GPIO_SIG99_IN_SEL_M  (GPIO_SIG99_IN_SEL_V << GPIO_SIG99_IN_SEL_S)
#define GPIO_SIG99_IN_SEL_V  0x00000001U
#define GPIO_SIG99_IN_SEL_S  7

/** GPIO_FUNC100_IN_SEL_CFG_REG register
 *  Configuration register for input signal 100
 */
#define GPIO_FUNC100_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x454)
/** GPIO_FUNC100_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 100.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC100_IN_SEL    0x0000003FU
#define GPIO_FUNC100_IN_SEL_M  (GPIO_FUNC100_IN_SEL_V << GPIO_FUNC100_IN_SEL_S)
#define GPIO_FUNC100_IN_SEL_V  0x0000003FU
#define GPIO_FUNC100_IN_SEL_S  0
/** GPIO_FUNC100_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC100_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC100_IN_INV_SEL_M  (GPIO_FUNC100_IN_INV_SEL_V << GPIO_FUNC100_IN_INV_SEL_S)
#define GPIO_FUNC100_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC100_IN_INV_SEL_S  6
/** GPIO_SIG100_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG100_IN_SEL    (BIT(7))
#define GPIO_SIG100_IN_SEL_M  (GPIO_SIG100_IN_SEL_V << GPIO_SIG100_IN_SEL_S)
#define GPIO_SIG100_IN_SEL_V  0x00000001U
#define GPIO_SIG100_IN_SEL_S  7

/** GPIO_FUNC118_IN_SEL_CFG_REG register
 *  Configuration register for input signal 118
 */
#define GPIO_FUNC118_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x49c)
/** GPIO_FUNC118_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 118.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC118_IN_SEL    0x0000003FU
#define GPIO_FUNC118_IN_SEL_M  (GPIO_FUNC118_IN_SEL_V << GPIO_FUNC118_IN_SEL_S)
#define GPIO_FUNC118_IN_SEL_V  0x0000003FU
#define GPIO_FUNC118_IN_SEL_S  0
/** GPIO_FUNC118_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC118_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC118_IN_INV_SEL_M  (GPIO_FUNC118_IN_INV_SEL_V << GPIO_FUNC118_IN_INV_SEL_S)
#define GPIO_FUNC118_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC118_IN_INV_SEL_S  6
/** GPIO_SIG118_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG118_IN_SEL    (BIT(7))
#define GPIO_SIG118_IN_SEL_M  (GPIO_SIG118_IN_SEL_V << GPIO_SIG118_IN_SEL_S)
#define GPIO_SIG118_IN_SEL_V  0x00000001U
#define GPIO_SIG118_IN_SEL_S  7

/** GPIO_FUNC119_IN_SEL_CFG_REG register
 *  Configuration register for input signal 119
 */
#define GPIO_FUNC119_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4a0)
/** GPIO_FUNC119_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 119.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC119_IN_SEL    0x0000003FU
#define GPIO_FUNC119_IN_SEL_M  (GPIO_FUNC119_IN_SEL_V << GPIO_FUNC119_IN_SEL_S)
#define GPIO_FUNC119_IN_SEL_V  0x0000003FU
#define GPIO_FUNC119_IN_SEL_S  0
/** GPIO_FUNC119_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC119_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC119_IN_INV_SEL_M  (GPIO_FUNC119_IN_INV_SEL_V << GPIO_FUNC119_IN_INV_SEL_S)
#define GPIO_FUNC119_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC119_IN_INV_SEL_S  6
/** GPIO_SIG119_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG119_IN_SEL    (BIT(7))
#define GPIO_SIG119_IN_SEL_M  (GPIO_SIG119_IN_SEL_V << GPIO_SIG119_IN_SEL_S)
#define GPIO_SIG119_IN_SEL_V  0x00000001U
#define GPIO_SIG119_IN_SEL_S  7

/** GPIO_FUNC120_IN_SEL_CFG_REG register
 *  Configuration register for input signal 120
 */
#define GPIO_FUNC120_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4a4)
/** GPIO_FUNC120_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 120.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC120_IN_SEL    0x0000003FU
#define GPIO_FUNC120_IN_SEL_M  (GPIO_FUNC120_IN_SEL_V << GPIO_FUNC120_IN_SEL_S)
#define GPIO_FUNC120_IN_SEL_V  0x0000003FU
#define GPIO_FUNC120_IN_SEL_S  0
/** GPIO_FUNC120_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC120_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC120_IN_INV_SEL_M  (GPIO_FUNC120_IN_INV_SEL_V << GPIO_FUNC120_IN_INV_SEL_S)
#define GPIO_FUNC120_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC120_IN_INV_SEL_S  6
/** GPIO_SIG120_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG120_IN_SEL    (BIT(7))
#define GPIO_SIG120_IN_SEL_M  (GPIO_SIG120_IN_SEL_V << GPIO_SIG120_IN_SEL_S)
#define GPIO_SIG120_IN_SEL_V  0x00000001U
#define GPIO_SIG120_IN_SEL_S  7

/** GPIO_FUNC121_IN_SEL_CFG_REG register
 *  Configuration register for input signal 121
 */
#define GPIO_FUNC121_IN_SEL_CFG_REG (DR_REG_GPIO_BASE + 0x4a8)
/** GPIO_FUNC121_IN_SEL : R/W; bitpos: [5:0]; default: 48;
 *  Configures to select a pin from the 25 GPIO pins to connect the input signal 121.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  23: Select GPIO23\\
 *  24: Select GPIO24\\
 *  Or\\
 *  0x20: A constantly high input\\
 *  0x30: A constantly low input\\
 */
#define GPIO_FUNC121_IN_SEL    0x0000003FU
#define GPIO_FUNC121_IN_SEL_M  (GPIO_FUNC121_IN_SEL_V << GPIO_FUNC121_IN_SEL_S)
#define GPIO_FUNC121_IN_SEL_V  0x0000003FU
#define GPIO_FUNC121_IN_SEL_S  0
/** GPIO_FUNC121_IN_INV_SEL : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to invert the input value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define GPIO_FUNC121_IN_INV_SEL    (BIT(6))
#define GPIO_FUNC121_IN_INV_SEL_M  (GPIO_FUNC121_IN_INV_SEL_V << GPIO_FUNC121_IN_INV_SEL_S)
#define GPIO_FUNC121_IN_INV_SEL_V  0x00000001U
#define GPIO_FUNC121_IN_INV_SEL_S  6
/** GPIO_SIG121_IN_SEL : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to route signals via GPIO matrix.\\
 *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
 *  IO MUX.\\
 *  1: Route signals via GPIO matrix.\\
 */
#define GPIO_SIG121_IN_SEL    (BIT(7))
#define GPIO_SIG121_IN_SEL_M  (GPIO_SIG121_IN_SEL_V << GPIO_SIG121_IN_SEL_S)
#define GPIO_SIG121_IN_SEL_V  0x00000001U
#define GPIO_SIG121_IN_SEL_S  7

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
/** GPIO_DATE : R/W; bitpos: [27:0]; default: 37753392;
 *  Version control register. \\
 */
#define GPIO_DATE    0x0FFFFFFFU
#define GPIO_DATE_M  (GPIO_DATE_V << GPIO_DATE_S)
#define GPIO_DATE_V  0x0FFFFFFFU
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
