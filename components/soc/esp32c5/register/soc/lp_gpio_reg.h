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
 *  LP GPIO output register
 */
#define LP_GPIO_OUT_REG (DR_REG_LP_GPIO_BASE + 0x4)
/** LP_GPIO_OUT_DATA_ORIG : R/W/WTC; bitpos: [7:0]; default: 0;
 *  Configures the output of GPIO0 ~ GPIO7.\\
 *  0: Low level\\
 *  1: High level\\
 *  bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.\\
 */
#define LP_GPIO_OUT_DATA_ORIG    0x000000FFU
#define LP_GPIO_OUT_DATA_ORIG_M  (LP_GPIO_OUT_DATA_ORIG_V << LP_GPIO_OUT_DATA_ORIG_S)
#define LP_GPIO_OUT_DATA_ORIG_V  0x000000FFU
#define LP_GPIO_OUT_DATA_ORIG_S  0

/** LP_GPIO_OUT_W1TS_REG register
 *  LP GPIO output set register
 */
#define LP_GPIO_OUT_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x8)
/** LP_GPIO_OUT_W1TS : WT; bitpos: [7:0]; default: 0;
 *  Configures whether or not to enable the output register LP_IO_OUT_REG of GPIO0 ~
 *  GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the corresponding bit in LP_IO_OUT_REG
 *  will be set to 1.
 *  - Recommended operation: use this register to set LP_IO_OUT_REG.
 */
#define LP_GPIO_OUT_W1TS    0x000000FFU
#define LP_GPIO_OUT_W1TS_M  (LP_GPIO_OUT_W1TS_V << LP_GPIO_OUT_W1TS_S)
#define LP_GPIO_OUT_W1TS_V  0x000000FFU
#define LP_GPIO_OUT_W1TS_S  0

/** LP_GPIO_OUT_W1TC_REG register
 *  LP GPIO output clear register
 */
#define LP_GPIO_OUT_W1TC_REG (DR_REG_LP_GPIO_BASE + 0xc)
/** LP_GPIO_OUT_W1TC : WT; bitpos: [7:0]; default: 0;
 *  Configures whether or not to clear the output register LP_IO_OUT_REG of GPIO0 ~
 *  GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the corresponding bit in LP_IO_OUT_REG
 *  will be cleared.
 *  - Recommended operation: use this register to clear LP_IO_OUT_REG.
 */
#define LP_GPIO_OUT_W1TC    0x000000FFU
#define LP_GPIO_OUT_W1TC_M  (LP_GPIO_OUT_W1TC_V << LP_GPIO_OUT_W1TC_S)
#define LP_GPIO_OUT_W1TC_V  0x000000FFU
#define LP_GPIO_OUT_W1TC_S  0

/** LP_GPIO_ENABLE_REG register
 *  LP GPIO output enable register
 */
#define LP_GPIO_ENABLE_REG (DR_REG_LP_GPIO_BASE + 0x10)
/** LP_GPIO_ENABLE_DATA : R/W/WTC; bitpos: [7:0]; default: 0;
 *  Configures whether or not to enable the output of GPIO0 ~ GPIO7.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.\\
 */
#define LP_GPIO_ENABLE_DATA    0x000000FFU
#define LP_GPIO_ENABLE_DATA_M  (LP_GPIO_ENABLE_DATA_V << LP_GPIO_ENABLE_DATA_S)
#define LP_GPIO_ENABLE_DATA_V  0x000000FFU
#define LP_GPIO_ENABLE_DATA_S  0

/** LP_GPIO_ENABLE_W1TS_REG register
 *  LP GPIO output enable set register
 */
#define LP_GPIO_ENABLE_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x14)
/** LP_GPIO_ENABLE_W1TS : WT; bitpos: [7:0]; default: 0;
 *  Configures whether or not to set the output enable register LP_IO_ENABLE_REG of
 *  GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  LP_IO_ENABLE_REG will be set to 1.
 *  - Recommended operation: use this register to set LP_IO_ENABLE_REG.
 */
#define LP_GPIO_ENABLE_W1TS    0x000000FFU
#define LP_GPIO_ENABLE_W1TS_M  (LP_GPIO_ENABLE_W1TS_V << LP_GPIO_ENABLE_W1TS_S)
#define LP_GPIO_ENABLE_W1TS_V  0x000000FFU
#define LP_GPIO_ENABLE_W1TS_S  0

/** LP_GPIO_ENABLE_W1TC_REG register
 *  LP GPIO output enable clear register
 */
#define LP_GPIO_ENABLE_W1TC_REG (DR_REG_LP_GPIO_BASE + 0x18)
/** LP_GPIO_ENABLE_W1TC : WT; bitpos: [7:0]; default: 0;
 *  Configures whether or not to clear the output enable register LP_IO_ENABLE_REG of
 *  GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  LP_IO_ENABLE_REG will be cleared.
 *  - Recommended operation: use this register to clear LP_IO_ENABLE_REG.
 */
#define LP_GPIO_ENABLE_W1TC    0x000000FFU
#define LP_GPIO_ENABLE_W1TC_M  (LP_GPIO_ENABLE_W1TC_V << LP_GPIO_ENABLE_W1TC_S)
#define LP_GPIO_ENABLE_W1TC_V  0x000000FFU
#define LP_GPIO_ENABLE_W1TC_S  0

/** LP_GPIO_IN_REG register
 *  LP GPIO input register
 */
#define LP_GPIO_IN_REG (DR_REG_LP_GPIO_BASE + 0x1c)
/** LP_GPIO_IN_DATA_NEXT : RO; bitpos: [7:0]; default: 0;
 *  Represents the input value of GPIO0 ~ GPIO7.\\
 *  0: Low level input\\
 *  1: High level input\\
 *  bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.\\
 */
#define LP_GPIO_IN_DATA_NEXT    0x000000FFU
#define LP_GPIO_IN_DATA_NEXT_M  (LP_GPIO_IN_DATA_NEXT_V << LP_GPIO_IN_DATA_NEXT_S)
#define LP_GPIO_IN_DATA_NEXT_V  0x000000FFU
#define LP_GPIO_IN_DATA_NEXT_S  0

/** LP_GPIO_STATUS_REG register
 *  LP GPIO interrupt status register
 */
#define LP_GPIO_STATUS_REG (DR_REG_LP_GPIO_BASE + 0x20)
/** LP_GPIO_STATUS_INTERRUPT : R/W/WTC; bitpos: [7:0]; default: 0;
 *  Configures the interrupt status of GPIO0 ~ GPIO7.\\
 *  0: No interrupt\\
 *  1: Interrupt is triggered\\
 *  Bit0 is corresponding to GPIO0, bit1 is corresponding to GPIO1, and etc. This
 *  register is used together LP_IO_PIN$n_INT_TYPE in register LP_IO_PIN$n_REG.\\
 */
#define LP_GPIO_STATUS_INTERRUPT    0x000000FFU
#define LP_GPIO_STATUS_INTERRUPT_M  (LP_GPIO_STATUS_INTERRUPT_V << LP_GPIO_STATUS_INTERRUPT_S)
#define LP_GPIO_STATUS_INTERRUPT_V  0x000000FFU
#define LP_GPIO_STATUS_INTERRUPT_S  0

/** LP_GPIO_STATUS_W1TS_REG register
 *  LP GPIO interrupt status set register
 */
#define LP_GPIO_STATUS_W1TS_REG (DR_REG_LP_GPIO_BASE + 0x24)
/** LP_GPIO_STATUS_W1TS : WT; bitpos: [7:0]; default: 0;
 *  Configures whether or not to set the interrupt status register LP_IO_STATUS_INT of
 *  GPIO0 ~ GPIO7.\\
 *
 *  - Bit0 is corresponding to GPIO0, bit1 is corresponding to GPIO1, and etc.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  LP_IO_STATUS_INT will be set to 1.
 *  - Recommended operation: use this register to set LP_IO_STATUS_INT.
 */
#define LP_GPIO_STATUS_W1TS    0x000000FFU
#define LP_GPIO_STATUS_W1TS_M  (LP_GPIO_STATUS_W1TS_V << LP_GPIO_STATUS_W1TS_S)
#define LP_GPIO_STATUS_W1TS_V  0x000000FFU
#define LP_GPIO_STATUS_W1TS_S  0

/** LP_GPIO_STATUS_W1TC_REG register
 *  LP GPIO interrupt status clear register
 */
#define LP_GPIO_STATUS_W1TC_REG (DR_REG_LP_GPIO_BASE + 0x28)
/** LP_GPIO_STATUS_W1TC : WT; bitpos: [7:0]; default: 0;
 *  Configures whether or not to clear the interrupt status register LP_IO_STATUS_INT
 *  of GPIO0 ~ GPIO7. \\
 *
 *  - Bit0 is corresponding to GPIO0, bit1 is corresponding to GPIO1, and etc.
 *  - If the value 1 is written to a bit here, the corresponding bit in
 *  LP_IO_STATUS_INT will be cleared
 *  - ecommended operation: use this register to clear LP_IO_STATUS_INT.
 */
#define LP_GPIO_STATUS_W1TC    0x000000FFU
#define LP_GPIO_STATUS_W1TC_M  (LP_GPIO_STATUS_W1TC_V << LP_GPIO_STATUS_W1TC_S)
#define LP_GPIO_STATUS_W1TC_V  0x000000FFU
#define LP_GPIO_STATUS_W1TC_S  0

/** LP_GPIO_STATUS_NEXT_REG register
 *  LP GPIO interrupt source register
 */
#define LP_GPIO_STATUS_NEXT_REG (DR_REG_LP_GPIO_BASE + 0x2c)
/** LP_GPIO_STATUS_INTERRUPT_NEXT : RO; bitpos: [7:0]; default: 0;
 *  Represents the interrupt source status of GPIO0 ~ GPIO7.\\
 *  bit0 ~ bit7 are corresponding to GPIO0 ~ 7. Each bit represents:\\
 *  0: Interrupt source status is invalid.\\
 *  1: Interrupt source status is valid.\\
 *  The interrupt here can be rising-edge triggered, falling-edge triggered, any edge
 *  triggered, or level triggered.\\
 */
#define LP_GPIO_STATUS_INTERRUPT_NEXT    0x000000FFU
#define LP_GPIO_STATUS_INTERRUPT_NEXT_M  (LP_GPIO_STATUS_INTERRUPT_NEXT_V << LP_GPIO_STATUS_INTERRUPT_NEXT_S)
#define LP_GPIO_STATUS_INTERRUPT_NEXT_V  0x000000FFU
#define LP_GPIO_STATUS_INTERRUPT_NEXT_S  0

/** LP_GPIO_PIN0_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN0_REG (DR_REG_LP_GPIO_BASE + 0x30)
/** LP_GPIO_PIN0_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO0.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN0_PAD_DRIVER_M  (LP_GPIO_PIN0_PAD_DRIVER_V << LP_GPIO_PIN0_PAD_DRIVER_S)
#define LP_GPIO_PIN0_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN0_PAD_DRIVER_S  2
/** LP_GPIO_PIN0_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN0_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN0_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN0_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN0_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO0 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN0_INT_TYPE    0x00000007U
#define LP_GPIO_PIN0_INT_TYPE_M  (LP_GPIO_PIN0_INT_TYPE_V << LP_GPIO_PIN0_INT_TYPE_S)
#define LP_GPIO_PIN0_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN0_INT_TYPE_S  7
/** LP_GPIO_PIN0_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO0 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN0_WAKEUP_ENABLE_M  (LP_GPIO_PIN0_WAKEUP_ENABLE_V << LP_GPIO_PIN0_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN0_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN0_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN1_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN1_REG (DR_REG_LP_GPIO_BASE + 0x34)
/** LP_GPIO_PIN1_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO1.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN1_PAD_DRIVER_M  (LP_GPIO_PIN1_PAD_DRIVER_V << LP_GPIO_PIN1_PAD_DRIVER_S)
#define LP_GPIO_PIN1_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN1_PAD_DRIVER_S  2
/** LP_GPIO_PIN1_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN1_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN1_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN1_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN1_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO1 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN1_INT_TYPE    0x00000007U
#define LP_GPIO_PIN1_INT_TYPE_M  (LP_GPIO_PIN1_INT_TYPE_V << LP_GPIO_PIN1_INT_TYPE_S)
#define LP_GPIO_PIN1_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN1_INT_TYPE_S  7
/** LP_GPIO_PIN1_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO1 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN1_WAKEUP_ENABLE_M  (LP_GPIO_PIN1_WAKEUP_ENABLE_V << LP_GPIO_PIN1_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN1_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN1_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN2_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN2_REG (DR_REG_LP_GPIO_BASE + 0x38)
/** LP_GPIO_PIN2_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO2.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN2_PAD_DRIVER_M  (LP_GPIO_PIN2_PAD_DRIVER_V << LP_GPIO_PIN2_PAD_DRIVER_S)
#define LP_GPIO_PIN2_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN2_PAD_DRIVER_S  2
/** LP_GPIO_PIN2_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN2_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN2_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN2_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN2_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO2 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN2_INT_TYPE    0x00000007U
#define LP_GPIO_PIN2_INT_TYPE_M  (LP_GPIO_PIN2_INT_TYPE_V << LP_GPIO_PIN2_INT_TYPE_S)
#define LP_GPIO_PIN2_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN2_INT_TYPE_S  7
/** LP_GPIO_PIN2_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO2 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN2_WAKEUP_ENABLE_M  (LP_GPIO_PIN2_WAKEUP_ENABLE_V << LP_GPIO_PIN2_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN2_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN2_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN3_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN3_REG (DR_REG_LP_GPIO_BASE + 0x3c)
/** LP_GPIO_PIN3_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO3.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN3_PAD_DRIVER_M  (LP_GPIO_PIN3_PAD_DRIVER_V << LP_GPIO_PIN3_PAD_DRIVER_S)
#define LP_GPIO_PIN3_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN3_PAD_DRIVER_S  2
/** LP_GPIO_PIN3_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN3_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN3_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN3_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN3_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO3 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN3_INT_TYPE    0x00000007U
#define LP_GPIO_PIN3_INT_TYPE_M  (LP_GPIO_PIN3_INT_TYPE_V << LP_GPIO_PIN3_INT_TYPE_S)
#define LP_GPIO_PIN3_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN3_INT_TYPE_S  7
/** LP_GPIO_PIN3_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO3 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN3_WAKEUP_ENABLE_M  (LP_GPIO_PIN3_WAKEUP_ENABLE_V << LP_GPIO_PIN3_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN3_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN3_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN4_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN4_REG (DR_REG_LP_GPIO_BASE + 0x40)
/** LP_GPIO_PIN4_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO4.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN4_PAD_DRIVER_M  (LP_GPIO_PIN4_PAD_DRIVER_V << LP_GPIO_PIN4_PAD_DRIVER_S)
#define LP_GPIO_PIN4_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN4_PAD_DRIVER_S  2
/** LP_GPIO_PIN4_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN4_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN4_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN4_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN4_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO4 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN4_INT_TYPE    0x00000007U
#define LP_GPIO_PIN4_INT_TYPE_M  (LP_GPIO_PIN4_INT_TYPE_V << LP_GPIO_PIN4_INT_TYPE_S)
#define LP_GPIO_PIN4_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN4_INT_TYPE_S  7
/** LP_GPIO_PIN4_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO4 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN4_WAKEUP_ENABLE_M  (LP_GPIO_PIN4_WAKEUP_ENABLE_V << LP_GPIO_PIN4_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN4_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN4_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN5_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN5_REG (DR_REG_LP_GPIO_BASE + 0x44)
/** LP_GPIO_PIN5_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO5.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN5_PAD_DRIVER_M  (LP_GPIO_PIN5_PAD_DRIVER_V << LP_GPIO_PIN5_PAD_DRIVER_S)
#define LP_GPIO_PIN5_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN5_PAD_DRIVER_S  2
/** LP_GPIO_PIN5_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN5_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN5_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN5_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN5_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO5 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN5_INT_TYPE    0x00000007U
#define LP_GPIO_PIN5_INT_TYPE_M  (LP_GPIO_PIN5_INT_TYPE_V << LP_GPIO_PIN5_INT_TYPE_S)
#define LP_GPIO_PIN5_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN5_INT_TYPE_S  7
/** LP_GPIO_PIN5_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO5 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN5_WAKEUP_ENABLE_M  (LP_GPIO_PIN5_WAKEUP_ENABLE_V << LP_GPIO_PIN5_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN5_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN5_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN6_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN6_REG (DR_REG_LP_GPIO_BASE + 0x48)
/** LP_GPIO_PIN6_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures to select the pin dirve mode of GPIO6.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN6_PAD_DRIVER_M  (LP_GPIO_PIN6_PAD_DRIVER_V << LP_GPIO_PIN6_PAD_DRIVER_S)
#define LP_GPIO_PIN6_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN6_PAD_DRIVER_S  2
/** LP_GPIO_PIN6_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
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
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN6_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN6_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN6_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN6_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO6 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN6_INT_TYPE    0x00000007U
#define LP_GPIO_PIN6_INT_TYPE_M  (LP_GPIO_PIN6_INT_TYPE_V << LP_GPIO_PIN6_INT_TYPE_S)
#define LP_GPIO_PIN6_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN6_INT_TYPE_S  7
/** LP_GPIO_PIN6_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO6 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN6_WAKEUP_ENABLE_M  (LP_GPIO_PIN6_WAKEUP_ENABLE_V << LP_GPIO_PIN6_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN6_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN6_WAKEUP_ENABLE_S  10

/** LP_GPIO_PIN7_REG register
 *  LP GPIO0 configuration register
 */
#define LP_GPIO_PIN7_REG (DR_REG_LP_GPIO_BASE + 0x4c)
/** LP_GPIO_PIN7_SYNC2_BYPASS : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
 *  MUX operating clock for the second-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN7_SYNC2_BYPASS    0x00000003U
#define LP_GPIO_PIN7_SYNC2_BYPASS_M  (LP_GPIO_PIN7_SYNC2_BYPASS_V << LP_GPIO_PIN7_SYNC2_BYPASS_S)
#define LP_GPIO_PIN7_SYNC2_BYPASS_V  0x00000003U
#define LP_GPIO_PIN7_SYNC2_BYPASS_S  0
/** LP_GPIO_PIN7_PAD_DRIVER : R/W; bitpos: [2]; default: 0;
 *  Configures to select the pin dirve mode of GPIO7.\\
 *  0: Normal output\\
 *  1: Open drain output\\
 */
#define LP_GPIO_PIN7_PAD_DRIVER    (BIT(2))
#define LP_GPIO_PIN7_PAD_DRIVER_M  (LP_GPIO_PIN7_PAD_DRIVER_V << LP_GPIO_PIN7_PAD_DRIVER_S)
#define LP_GPIO_PIN7_PAD_DRIVER_V  0x00000001U
#define LP_GPIO_PIN7_PAD_DRIVER_S  2
/** LP_GPIO_PIN7_SYNC1_BYPASS : R/W; bitpos: [4:3]; default: 0;
 *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
 *  MUX operating clock for the first-level synchronization.\\
 *  0: Not synchronize\\
 *  1: Synchronize on falling edge\\
 *  2: Synchronize on rising edge\\
 *  3: Synchronize on rising edge\\
 */
#define LP_GPIO_PIN7_SYNC1_BYPASS    0x00000003U
#define LP_GPIO_PIN7_SYNC1_BYPASS_M  (LP_GPIO_PIN7_SYNC1_BYPASS_V << LP_GPIO_PIN7_SYNC1_BYPASS_S)
#define LP_GPIO_PIN7_SYNC1_BYPASS_V  0x00000003U
#define LP_GPIO_PIN7_SYNC1_BYPASS_S  3
/** LP_GPIO_PIN7_EDGE_WAKEUP_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
 *
 *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
 *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
 *  GPIO will be cleared.
 */
#define LP_GPIO_PIN7_EDGE_WAKEUP_CLR    (BIT(5))
#define LP_GPIO_PIN7_EDGE_WAKEUP_CLR_M  (LP_GPIO_PIN7_EDGE_WAKEUP_CLR_V << LP_GPIO_PIN7_EDGE_WAKEUP_CLR_S)
#define LP_GPIO_PIN7_EDGE_WAKEUP_CLR_V  0x00000001U
#define LP_GPIO_PIN7_EDGE_WAKEUP_CLR_S  5
/** LP_GPIO_PIN7_INT_TYPE : R/W; bitpos: [9:7]; default: 0;
 *  Configures GPIO7 interrupt type.\\
 *  0: GPIO interrupt disabled \\
 *  1: Rising edge trigger \\
 *  2: Falling edge trigger \\
 *  3: Any edge trigger \\
 *  4: Low level trigger \\
 *  5: High level trigger \\
 */
#define LP_GPIO_PIN7_INT_TYPE    0x00000007U
#define LP_GPIO_PIN7_INT_TYPE_M  (LP_GPIO_PIN7_INT_TYPE_V << LP_GPIO_PIN7_INT_TYPE_S)
#define LP_GPIO_PIN7_INT_TYPE_V  0x00000007U
#define LP_GPIO_PIN7_INT_TYPE_S  7
/** LP_GPIO_PIN7_WAKEUP_ENABLE : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable GPIO7 wake-up function.\\
 *  0: Not enable\\
 *  1: Enable\\
 *  This function is disabled when PD_LP_PERI is powered off.\\
 */
#define LP_GPIO_PIN7_WAKEUP_ENABLE    (BIT(10))
#define LP_GPIO_PIN7_WAKEUP_ENABLE_M  (LP_GPIO_PIN7_WAKEUP_ENABLE_V << LP_GPIO_PIN7_WAKEUP_ENABLE_S)
#define LP_GPIO_PIN7_WAKEUP_ENABLE_V  0x00000001U
#define LP_GPIO_PIN7_WAKEUP_ENABLE_S  10

/** LP_GPIO_FUNC0_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO0 output
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
 *  Configuration register for GPIO0 output
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
 *  Configuration register for GPIO0 output
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
 *  Configuration register for GPIO0 output
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
 *  Configuration register for GPIO0 output
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
 *  Configuration register for GPIO0 output
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
 *  Configuration register for GPIO0 output
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

/** LP_GPIO_FUNC7_OUT_SEL_CFG_REG register
 *  Configuration register for GPIO0 output
 */
#define LP_GPIO_FUNC7_OUT_SEL_CFG_REG (DR_REG_LP_GPIO_BASE + 0x2cc)
/** LP_GPIO_FUNC7_OUT_INV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to invert the output value.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC7_OUT_INV_SEL    (BIT(0))
#define LP_GPIO_FUNC7_OUT_INV_SEL_M  (LP_GPIO_FUNC7_OUT_INV_SEL_V << LP_GPIO_FUNC7_OUT_INV_SEL_S)
#define LP_GPIO_FUNC7_OUT_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC7_OUT_INV_SEL_S  0
/** LP_GPIO_FUNC7_OE_INV_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to invert the output enable signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_GPIO_FUNC7_OE_INV_SEL    (BIT(2))
#define LP_GPIO_FUNC7_OE_INV_SEL_M  (LP_GPIO_FUNC7_OE_INV_SEL_V << LP_GPIO_FUNC7_OE_INV_SEL_S)
#define LP_GPIO_FUNC7_OE_INV_SEL_V  0x00000001U
#define LP_GPIO_FUNC7_OE_INV_SEL_S  2

/** LP_GPIO_CLOCK_GATE_REG register
 *  GPIO clock gate register
 */
#define LP_GPIO_CLOCK_GATE_REG (DR_REG_LP_GPIO_BASE + 0x3f8)
/** LP_GPIO_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  set this bit to enable GPIO clock gate.\\
 */
#define LP_GPIO_CLK_EN    (BIT(0))
#define LP_GPIO_CLK_EN_M  (LP_GPIO_CLK_EN_V << LP_GPIO_CLK_EN_S)
#define LP_GPIO_CLK_EN_V  0x00000001U
#define LP_GPIO_CLK_EN_S  0

/** LP_GPIO_DATE_REG register
 *  GPIO version register
 */
#define LP_GPIO_DATE_REG (DR_REG_LP_GPIO_BASE + 0x3fc)
/** LP_GPIO_DATE : R/W; bitpos: [27:0]; default: 36773904;
 *  version register.\\
 */
#define LP_GPIO_DATE    0x0FFFFFFFU
#define LP_GPIO_DATE_M  (LP_GPIO_DATE_V << LP_GPIO_DATE_S)
#define LP_GPIO_DATE_V  0x0FFFFFFFU
#define LP_GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif
