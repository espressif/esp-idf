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

/** LP_GPIO_OUT_REG register
 *  LP_GPIO output register
 */
#define LP_GPIO_OUT_REG (DR_REG_LP_GPIO_BASE + 0x4)
/** LP_GPIO_OUT_DATA_ORIG : R/W/WTC; bitpos: [6:0]; default: 0;
 *  Configures the output value of LP_GPIO0 ~ 6 output in simple LP_GPIO output mode.\\
 *  0: Low level\\
 *  1: High level\\
 *  The value of bit0 ~ bit6 correspond to the output value of LP_GPIO0 ~ LP_GPIO6
 *  respectively. Bit7 ~ bit31 are invalid.\\
 */
#define LP_GPIO_OUT_DATA_ORIG    0x0000007FU
#define LP_GPIO_OUT_DATA_ORIG_M  (LP_GPIO_OUT_DATA_ORIG_V << LP_GPIO_OUT_DATA_ORIG_S)
#define LP_GPIO_OUT_DATA_ORIG_V  0x0000007FU
#define LP_GPIO_OUT_DATA_ORIG_S  0

/** LP_GPIO_OUT_W1TS_REG register
 *  LP_GPIO output set register
 */
#define LP_GPIO_OUT_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x8)
/** LP_GPIO_OUT_W1TS : WT; bitpos: [6:0]; default: 0;
 *  Configures whether or not to set the output register LP_GPIO_OUT_REG of LP_GPIO0 ~
 *  LP_GPIO6.\\
 *  0: Not set\\
 *  1: The corresponding bit in LP_GPIO_OUT_REG will be set to 1\\
 *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to set LP_GPIO_OUT_REG. \\
 */
#define LP_GPIO_OUT_W1TS    0x0000007FU
#define LP_GPIO_OUT_W1TS_M  (LP_GPIO_OUT_W1TS_V << LP_GPIO_OUT_W1TS_S)
#define LP_GPIO_OUT_W1TS_V  0x0000007FU
#define LP_GPIO_OUT_W1TS_S  0

/** LP_GPIO_OUT_W1TC_REG register
 *  LP_GPIO output clear register
 */
#define LP_GPIO_OUT_W1TC_REG (DR_REG_LP_GPIO_BASE + 0xc)
/** LP_GPIO_OUT_W1TC : WT; bitpos: [6:0]; default: 0;
 *  Configures whether or not to clear the output register LP_GPIO_OUT_REG of LP_GPIO0
 *  ~ LP_GPIO6 output.\\
 *  0: Not clear\\
 *  1: The corresponding bit in LP_GPIO_OUT_REG will be cleared.\\
 *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to clear LP_GPIO_OUT_REG. \\
 */
#define LP_GPIO_OUT_W1TC    0x0000007FU
#define LP_GPIO_OUT_W1TC_M  (LP_GPIO_OUT_W1TC_V << LP_GPIO_OUT_W1TC_S)
#define LP_GPIO_OUT_W1TC_V  0x0000007FU
#define LP_GPIO_OUT_W1TC_S  0

/** LP_GPIO_ENABLE_REG register
 *  LP_GPIO output enable register
 */
#define LP_GPIO_ENABLE_REG (DR_REG_LP_GPIO_BASE + 0x10)
/** LP_GPIO_ENABLE_DATA : R/W/WTC; bitpos: [6:0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO0 ~ LP_GPIO6.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.\\
 */
#define LP_GPIO_ENABLE_DATA    0x0000007FU
#define LP_GPIO_ENABLE_DATA_M  (LP_GPIO_ENABLE_DATA_V << LP_GPIO_ENABLE_DATA_S)
#define LP_GPIO_ENABLE_DATA_V  0x0000007FU
#define LP_GPIO_ENABLE_DATA_S  0

/** LP_GPIO_ENABLE_W1TS_REG register
 *  LP_GPIO output enable set register
 */
#define LP_GPIO_ENABLE_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x14)
/** LP_GPIO_ENABLE_W1TS : WT; bitpos: [6:0]; default: 0;
 *  Configures whether or not to set the output enable register LP_GPIO_ENABLE_REG of
 *  LP_GPIO0 ~ LP_GPIO6.\\
 *  0: Not set\\
 *  1: The corresponding bit in LP_GPIO_ENABLE_REG will be set to 1\\
 *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to set LP_GPIO_ENABLE_REG.\\
 */
#define LP_GPIO_ENABLE_W1TS    0x0000007FU
#define LP_GPIO_ENABLE_W1TS_M  (LP_GPIO_ENABLE_W1TS_V << LP_GPIO_ENABLE_W1TS_S)
#define LP_GPIO_ENABLE_W1TS_V  0x0000007FU
#define LP_GPIO_ENABLE_W1TS_S  0

/** LP_GPIO_ENABLE_W1TC_REG register
 *  LP_GPIO output enable clear register
 */
#define LP_GPIO_ENABLE_W1TC_REG (DR_REG_LP_GPIO_BASE + 0x18)
/** LP_GPIO_ENABLE_W1TC : WT; bitpos: [6:0]; default: 0;
 *  Configures whether or not to clear the output enable register LP_GPIO_ENABLE_REG of
 *  LP_GPIO0 ~ LP_GPIO6. \\
 *  0: Not clear\\
 *  1: The corresponding bit in LP_GPIO_ENABLE_REG will be cleared\\
 *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
 *  Recommended operation: use this register to clear LP_GPIO_ENABLE_REG.\\
 */
#define LP_GPIO_ENABLE_W1TC    0x0000007FU
#define LP_GPIO_ENABLE_W1TC_M  (LP_GPIO_ENABLE_W1TC_V << LP_GPIO_ENABLE_W1TC_S)
#define LP_GPIO_ENABLE_W1TC_V  0x0000007FU
#define LP_GPIO_ENABLE_W1TC_S  0

/** LP_GPIO_IN_REG register
 *  LP_GPIO input register
 */
#define LP_GPIO_IN_REG (DR_REG_LP_GPIO_BASE + 0x1c)
/** LP_GPIO_IN_DATA_NEXT : RO; bitpos: [6:0]; default: 0;
 *  Represents the input value of LP_GPIO0 ~ LP_GPIO6. Each bit represents a pin input
 *  value:\\
 *  0: Low level\\
 *  1: High level\\
 *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.\\
 */
#define LP_GPIO_IN_DATA_NEXT    0x0000007FU
#define LP_GPIO_IN_DATA_NEXT_M  (LP_GPIO_IN_DATA_NEXT_V << LP_GPIO_IN_DATA_NEXT_S)
#define LP_GPIO_IN_DATA_NEXT_V  0x0000007FU
#define LP_GPIO_IN_DATA_NEXT_S  0

/** LP_GPIO_STATUS_REG register
 *  LP_GPIO interrupt status register
 */
#define LP_GPIO_STATUS_REG (DR_REG_LP_GPIO_BASE + 0x20)
/** LP_GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [6:0]; default: 0;
 *  The interrupt status of LP_GPIO0 ~ LP_GPIO6, can be configured by the software.
 *
 *  - Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
 *  - Each bit represents the status of its corresponding LP_GPIO:
 *
 *  - 0: Represents the LP_GPIO does not generate the interrupt configured by
 *  LP_GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
 *  - 1: Represents the LP_GPIO generates the interrupt configured by
 *  LP_GPIO_PIN$n_INT_TYPE, or this bit is configured to 1 by the software.
 *
 */
#define LP_GPIO_STATUS_INTERRUPT    0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_M  (LP_GPIO_STATUS_INTERRUPT_V << LP_GPIO_STATUS_INTERRUPT_S)
#define LP_GPIO_STATUS_INTERRUPT_V  0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_S  0

/** LP_GPIO_STATUS_W1TS_REG register
 *  LP_GPIO interrupt status set register
 */
#define LP_GPIO_STATUS_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x24)
/** LP_GPIO_STATUS_W1TS : WT; bitpos: [6:0]; default: 0;
 *  Configures whether or not to set the interrupt status register
 *  LP_GPIO_STATUS_INTERRUPT of LP_GPIO0 ~ LP_GPIO6.
 *
 *  - Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  LP_GPIO_STATUS_INTERRUPT will be set to 1. \item Recommended operation: use this
 *  register to set LP_GPIO_STATUS_INTERRUPT.
 */
#define LP_GPIO_STATUS_W1TS    0x0000007FU
#define LP_GPIO_STATUS_W1TS_M  (LP_GPIO_STATUS_W1TS_V << LP_GPIO_STATUS_W1TS_S)
#define LP_GPIO_STATUS_W1TS_V  0x0000007FU
#define LP_GPIO_STATUS_W1TS_S  0

/** LP_GPIO_STATUS_W1TC_REG register
 *  LP_GPIO interrupt status clear register
 */
#define LP_GPIO_STATUS_W1TC_REG (DR_REG_LP_GPIO_BASE + 0x28)
/** LP_GPIO_STATUS_W1TC : WT; bitpos: [6:0]; default: 0;
 *  Configures whether or not to clear the interrupt status register
 *  LP_GPIO_STATUS_INTERRUPT of LP_GPIO0 ~ LP_GPIO6.
 *
 *  - Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  LP_GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
 *  register to clear LP_GPIO_STATUS_INTERRUPT.
 */
#define LP_GPIO_STATUS_W1TC    0x0000007FU
#define LP_GPIO_STATUS_W1TC_M  (LP_GPIO_STATUS_W1TC_V << LP_GPIO_STATUS_W1TC_S)
#define LP_GPIO_STATUS_W1TC_V  0x0000007FU
#define LP_GPIO_STATUS_W1TC_S  0

/** LP_GPIO_STATUS_NEXT_REG register
 *  LP_GPIO interrupt source register
 */
#define LP_GPIO_STATUS_NEXT_REG (DR_REG_LP_GPIO_BASE + 0x2c)
/** LP_GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [6:0]; default: 0;
 *  Represents the interrupt source signal of LP_GPIO0 ~ LP_GPIO6.\\
 *  Bit0 ~ bit24 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
 *  Each bit represents:\\
 *  0: The LP_GPIO does not generate the interrupt configured by
 *  LP_GPIO_PIN$n_INT_TYPE.\\
 *  1: The LP_GPIO generates an interrupt configured by LP_GPIO_PIN$n_INT_TYPE.\\
 *  The interrupt could be rising edge interrupt, falling edge interrupt, level
 *  sensitive interrupt and any edge interrupt.\\
 */
#define LP_GPIO_STATUS_INTERRUPT_NEXT    0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_NEXT_M  (LP_GPIO_STATUS_INTERRUPT_NEXT_V << LP_GPIO_STATUS_INTERRUPT_NEXT_S)
#define LP_GPIO_STATUS_INTERRUPT_NEXT_V  0x0000007FU
#define LP_GPIO_STATUS_INTERRUPT_NEXT_S  0

/** LP_GPIO_PIN0_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN0_REG (DR_REG_LP_GPIO_BASE + 0x30)
/** LP_GPIO_PIN0_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN0_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN0_SYNC2_BYPASS_M  (LP_GPIO_PIN0_SYNC2_BYPASS_V << LP_GPIO_PIN0_SYNC2_BYPASS_S)
#define LP_GPIO_PIN0_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN0_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN0_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN0_PAD_DRIVER_M  (LP_GPIO_PIN0_PAD_DRIVER_V << LP_GPIO_PIN0_PAD_DRIVER_S)
#define LP_GPIO_PIN0_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN0_PAD_DRIVER_S  2
/** LP_GPIO_PIN0_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN0_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN0_SYNC1_BYPASS_M  (LP_GPIO_PIN0_SYNC1_BYPASS_V << LP_GPIO_PIN0_SYNC1_BYPASS_S)
#define LP_GPIO_PIN0_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN0_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN0_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN0_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN0_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN0_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN0_INT_TYPE    0x00000007U
#define LP_GPIO_PIN0_INT_TYPE_M  (LP_GPIO_PIN0_INT_TYPE_V << LP_GPIO_PIN0_INT_TYPE_S)
#define LP_GPIO_PIN0_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN0_INT_TYPE_S  7
/** LP_GPIO_PIN0_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN0_WAKEUP_ENABLE_M  (LP_GPIO_PIN0_WAKEUP_ENABLE_V << LP_GPIO_PIN0_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN0_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN0_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN1_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN1_REG (DR_REG_LP_GPIO_BASE + 0x34)
/** LP_GPIO_PIN1_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN1_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN1_SYNC2_BYPASS_M  (LP_GPIO_PIN1_SYNC2_BYPASS_V << LP_GPIO_PIN1_SYNC2_BYPASS_S)
#define LP_GPIO_PIN1_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN1_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN1_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN1_PAD_DRIVER_M  (LP_GPIO_PIN1_PAD_DRIVER_V << LP_GPIO_PIN1_PAD_DRIVER_S)
#define LP_GPIO_PIN1_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN1_PAD_DRIVER_S  2
/** LP_GPIO_PIN1_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN1_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN1_SYNC1_BYPASS_M  (LP_GPIO_PIN1_SYNC1_BYPASS_V << LP_GPIO_PIN1_SYNC1_BYPASS_S)
#define LP_GPIO_PIN1_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN1_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN1_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN1_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN1_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN1_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN1_INT_TYPE    0x00000007U
#define LP_GPIO_PIN1_INT_TYPE_M  (LP_GPIO_PIN1_INT_TYPE_V << LP_GPIO_PIN1_INT_TYPE_S)
#define LP_GPIO_PIN1_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN1_INT_TYPE_S  7
/** LP_GPIO_PIN1_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN1_WAKEUP_ENABLE_M  (LP_GPIO_PIN1_WAKEUP_ENABLE_V << LP_GPIO_PIN1_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN1_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN1_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN2_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN2_REG (DR_REG_LP_GPIO_BASE + 0x38)
/** LP_GPIO_PIN2_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN2_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN2_SYNC2_BYPASS_M  (LP_GPIO_PIN2_SYNC2_BYPASS_V << LP_GPIO_PIN2_SYNC2_BYPASS_S)
#define LP_GPIO_PIN2_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN2_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN2_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN2_PAD_DRIVER_M  (LP_GPIO_PIN2_PAD_DRIVER_V << LP_GPIO_PIN2_PAD_DRIVER_S)
#define LP_GPIO_PIN2_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN2_PAD_DRIVER_S  2
/** LP_GPIO_PIN2_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN2_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN2_SYNC1_BYPASS_M  (LP_GPIO_PIN2_SYNC1_BYPASS_V << LP_GPIO_PIN2_SYNC1_BYPASS_S)
#define LP_GPIO_PIN2_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN2_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN2_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN2_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN2_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN2_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN2_INT_TYPE    0x00000007U
#define LP_GPIO_PIN2_INT_TYPE_M  (LP_GPIO_PIN2_INT_TYPE_V << LP_GPIO_PIN2_INT_TYPE_S)
#define LP_GPIO_PIN2_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN2_INT_TYPE_S  7
/** LP_GPIO_PIN2_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN2_WAKEUP_ENABLE_M  (LP_GPIO_PIN2_WAKEUP_ENABLE_V << LP_GPIO_PIN2_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN2_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN2_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN3_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN3_REG (DR_REG_LP_GPIO_BASE + 0x3c)
/** LP_GPIO_PIN3_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN3_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN3_SYNC2_BYPASS_M  (LP_GPIO_PIN3_SYNC2_BYPASS_V << LP_GPIO_PIN3_SYNC2_BYPASS_S)
#define LP_GPIO_PIN3_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN3_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN3_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN3_PAD_DRIVER_M  (LP_GPIO_PIN3_PAD_DRIVER_V << LP_GPIO_PIN3_PAD_DRIVER_S)
#define LP_GPIO_PIN3_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN3_PAD_DRIVER_S  2
/** LP_GPIO_PIN3_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN3_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN3_SYNC1_BYPASS_M  (LP_GPIO_PIN3_SYNC1_BYPASS_V << LP_GPIO_PIN3_SYNC1_BYPASS_S)
#define LP_GPIO_PIN3_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN3_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN3_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN3_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN3_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN3_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN3_INT_TYPE    0x00000007U
#define LP_GPIO_PIN3_INT_TYPE_M  (LP_GPIO_PIN3_INT_TYPE_V << LP_GPIO_PIN3_INT_TYPE_S)
#define LP_GPIO_PIN3_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN3_INT_TYPE_S  7
/** LP_GPIO_PIN3_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN3_WAKEUP_ENABLE_M  (LP_GPIO_PIN3_WAKEUP_ENABLE_V << LP_GPIO_PIN3_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN3_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN3_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN4_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN4_REG (DR_REG_LP_GPIO_BASE + 0x40)
/** LP_GPIO_PIN4_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN4_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN4_SYNC2_BYPASS_M  (LP_GPIO_PIN4_SYNC2_BYPASS_V << LP_GPIO_PIN4_SYNC2_BYPASS_S)
#define LP_GPIO_PIN4_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN4_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN4_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN4_PAD_DRIVER_M  (LP_GPIO_PIN4_PAD_DRIVER_V << LP_GPIO_PIN4_PAD_DRIVER_S)
#define LP_GPIO_PIN4_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN4_PAD_DRIVER_S  2
/** LP_GPIO_PIN4_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN4_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN4_SYNC1_BYPASS_M  (LP_GPIO_PIN4_SYNC1_BYPASS_V << LP_GPIO_PIN4_SYNC1_BYPASS_S)
#define LP_GPIO_PIN4_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN4_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN4_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN4_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN4_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN4_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN4_INT_TYPE    0x00000007U
#define LP_GPIO_PIN4_INT_TYPE_M  (LP_GPIO_PIN4_INT_TYPE_V << LP_GPIO_PIN4_INT_TYPE_S)
#define LP_GPIO_PIN4_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN4_INT_TYPE_S  7
/** LP_GPIO_PIN4_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN4_WAKEUP_ENABLE_M  (LP_GPIO_PIN4_WAKEUP_ENABLE_V << LP_GPIO_PIN4_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN4_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN4_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN5_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN5_REG (DR_REG_LP_GPIO_BASE + 0x44)
/** LP_GPIO_PIN5_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN5_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN5_SYNC2_BYPASS_M  (LP_GPIO_PIN5_SYNC2_BYPASS_V << LP_GPIO_PIN5_SYNC2_BYPASS_S)
#define LP_GPIO_PIN5_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN5_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN5_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN5_PAD_DRIVER_M  (LP_GPIO_PIN5_PAD_DRIVER_V << LP_GPIO_PIN5_PAD_DRIVER_S)
#define LP_GPIO_PIN5_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN5_PAD_DRIVER_S  2
/** LP_GPIO_PIN5_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN5_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN5_SYNC1_BYPASS_M  (LP_GPIO_PIN5_SYNC1_BYPASS_V << LP_GPIO_PIN5_SYNC1_BYPASS_S)
#define LP_GPIO_PIN5_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN5_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN5_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN5_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN5_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN5_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN5_INT_TYPE    0x00000007U
#define LP_GPIO_PIN5_INT_TYPE_M  (LP_GPIO_PIN5_INT_TYPE_V << LP_GPIO_PIN5_INT_TYPE_S)
#define LP_GPIO_PIN5_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN5_INT_TYPE_S  7
/** LP_GPIO_PIN5_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN5_WAKEUP_ENABLE_M  (LP_GPIO_PIN5_WAKEUP_ENABLE_V << LP_GPIO_PIN5_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN5_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN5_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN6_REG register
 *  LP_GPIO0 configuration register
 */
#define LP_GPIO_PIN6_REG (DR_REG_LP_GPIO_BASE + 0x48)
/** LP_GPIO_PIN6_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN6_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN6_SYNC2_BYPASS_M  (LP_GPIO_PIN6_SYNC2_BYPASS_V << LP_GPIO_PIN6_SYNC2_BYPASS_S)
#define LP_GPIO_PIN6_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN6_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN6_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select pin drive mode. \\
 *  0: Normal output\\
 *  1: Open drain output \\
 */
#define LP_GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN6_PAD_DRIVER_M  (LP_GPIO_PIN6_PAD_DRIVER_V << LP_GPIO_PIN6_PAD_DRIVER_S)
#define LP_GPIO_PIN6_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN6_PAD_DRIVER_S  2
/** LP_GPIO_PIN6_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN6_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN6_SYNC1_BYPASS_M  (LP_GPIO_PIN6_SYNC1_BYPASS_V << LP_GPIO_PIN6_SYNC1_BYPASS_S)
#define LP_GPIO_PIN6_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN6_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN6_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  LP_GPIO wakeup clear register.
 */
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN6_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN6_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN6_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures LP_GPIO interrupt type.\\
 *  0: LP_GPIO interrupt disabled\\
 *  1: Rising edge trigger\\
 *  2: Falling edge trigger\\
 *  3: Any edge trigger\\
 *  4: Low level trigger\\
 *  5: High level trigger\\
 */
#define LP_GPIO_PIN6_INT_TYPE    0x00000007U
#define LP_GPIO_PIN6_INT_TYPE_M  (LP_GPIO_PIN6_INT_TYPE_V << LP_GPIO_PIN6_INT_TYPE_S)
#define LP_GPIO_PIN6_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN6_INT_TYPE_S  7
/** LP_GPIO_PIN6_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LP_GPIO wake-up function.\\
 *  0: Disable\\
 *  1: Enable\\
 *  This function only wakes up the CPU from Light-sleep. \\
 */
#define LP_GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN6_WAKEUP_ENABLE_M  (LP_GPIO_PIN6_WAKEUP_ENABLE_V << LP_GPIO_PIN6_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN6_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN6_WAKEUP_ENABLE_S  10

/** LP_GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO0 output
 */
#define LP_GPIO_FUNC0_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2b0)
/** LP_GPIO_FUNC0_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC0_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC0_OUT_INV_SEL_M  (LP_GPIO_FUNC0_OUT_INV_SEL_V << LP_GPIO_FUNC0_OUT_INV_SEL_S)
#define LP_GPIO_FUNC0_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC0_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC0_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC0_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC0_OE_INV_SEL_M  (LP_GPIO_FUNC0_OE_INV_SEL_V << LP_GPIO_FUNC0_OE_INV_SEL_S)
#define LP_GPIO_FUNC0_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC0_OE_INV_SEL_S  2

/** LP_GPIO_FUNC1_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO1 output
 */
#define LP_GPIO_FUNC1_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2b4)
/** LP_GPIO_FUNC1_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC1_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC1_OUT_INV_SEL_M  (LP_GPIO_FUNC1_OUT_INV_SEL_V << LP_GPIO_FUNC1_OUT_INV_SEL_S)
#define LP_GPIO_FUNC1_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC1_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC1_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC1_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC1_OE_INV_SEL_M  (LP_GPIO_FUNC1_OE_INV_SEL_V << LP_GPIO_FUNC1_OE_INV_SEL_S)
#define LP_GPIO_FUNC1_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC1_OE_INV_SEL_S  2

/** LP_GPIO_FUNC2_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO2 output
 */
#define LP_GPIO_FUNC2_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2b8)
/** LP_GPIO_FUNC2_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC2_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC2_OUT_INV_SEL_M  (LP_GPIO_FUNC2_OUT_INV_SEL_V << LP_GPIO_FUNC2_OUT_INV_SEL_S)
#define LP_GPIO_FUNC2_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC2_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC2_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC2_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC2_OE_INV_SEL_M  (LP_GPIO_FUNC2_OE_INV_SEL_V << LP_GPIO_FUNC2_OE_INV_SEL_S)
#define LP_GPIO_FUNC2_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC2_OE_INV_SEL_S  2

/** LP_GPIO_FUNC3_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO3 output
 */
#define LP_GPIO_FUNC3_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2bc)
/** LP_GPIO_FUNC3_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC3_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC3_OUT_INV_SEL_M  (LP_GPIO_FUNC3_OUT_INV_SEL_V << LP_GPIO_FUNC3_OUT_INV_SEL_S)
#define LP_GPIO_FUNC3_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC3_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC3_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC3_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC3_OE_INV_SEL_M  (LP_GPIO_FUNC3_OE_INV_SEL_V << LP_GPIO_FUNC3_OE_INV_SEL_S)
#define LP_GPIO_FUNC3_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC3_OE_INV_SEL_S  2

/** LP_GPIO_FUNC4_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO4 output
 */
#define LP_GPIO_FUNC4_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2c0)
/** LP_GPIO_FUNC4_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC4_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC4_OUT_INV_SEL_M  (LP_GPIO_FUNC4_OUT_INV_SEL_V << LP_GPIO_FUNC4_OUT_INV_SEL_S)
#define LP_GPIO_FUNC4_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC4_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC4_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC4_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC4_OE_INV_SEL_M  (LP_GPIO_FUNC4_OE_INV_SEL_V << LP_GPIO_FUNC4_OE_INV_SEL_S)
#define LP_GPIO_FUNC4_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC4_OE_INV_SEL_S  2

/** LP_GPIO_FUNC5_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO5 output
 */
#define LP_GPIO_FUNC5_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2c4)
/** LP_GPIO_FUNC5_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC5_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC5_OUT_INV_SEL_M  (LP_GPIO_FUNC5_OUT_INV_SEL_V << LP_GPIO_FUNC5_OUT_INV_SEL_S)
#define LP_GPIO_FUNC5_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC5_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC5_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC5_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC5_OE_INV_SEL_M  (LP_GPIO_FUNC5_OE_INV_SEL_V << LP_GPIO_FUNC5_OE_INV_SEL_S)
#define LP_GPIO_FUNC5_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC5_OE_INV_SEL_S  2

/** LP_GPIO_FUNC6_OUT_SEL_CFG_REG register
 *  Configuration register for LP_GPIO6 output
 */
#define LP_GPIO_FUNC6_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2c8)
/** LP_GPIO_FUNC6_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC6_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC6_OUT_INV_SEL_M  (LP_GPIO_FUNC6_OUT_INV_SEL_V << LP_GPIO_FUNC6_OUT_INV_SEL_S)
#define LP_GPIO_FUNC6_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC6_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC6_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC6_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC6_OE_INV_SEL_M  (LP_GPIO_FUNC6_OE_INV_SEL_V << LP_GPIO_FUNC6_OE_INV_SEL_S)
#define LP_GPIO_FUNC6_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC6_OE_INV_SEL_S  2

/** LP_GPIO_CLOCK_GATE_REG register
 *  LP_GPIO clock gate register
 */
#define LP_GPIO_CLOCK_GATE_REG (DR_REG_LP_GPIO_BASE + 0x3f8)
/** LP_GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to enable clock gate.\\
 *  0: Not enable\\
 *  1: Enable, the clock is free running. \\
 */
#define LP_GPIO_CLK_EN    (BIT(0))
#define LP_GPIO_CLK_EN_M  (LP_GPIO_CLK_EN_V << LP_GPIO_CLK_EN_S)
#define LP_GPIO_CLK_EN_V  0x00000001U
#define LP_GPIO_CLK_EN_S  0

/** LP_GPIO_DATE_REG register
 *  LP_GPIO version register
 */
#define LP_GPIO_DATE_REG (DR_REG_LP_GPIO_BASE + 0x3fc)
/** LP_GPIO_DATE : R/W; bitpos: [27:0]; default: 36766272;
 *  Version control register. \\
 */
#define LP_GPIO_DATE    0x0FFFFFFFU
#define LP_GPIO_DATE_M  (LP_GPIO_DATE_V << LP_GPIO_DATE_S)
#define LP_GPIO_DATE_V  0x0FFFFFFFU
#define LP_GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
