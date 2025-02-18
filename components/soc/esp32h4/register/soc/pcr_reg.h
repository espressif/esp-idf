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

/** PCR_UART0_CONF_REG register
 *  UART0 configuration register
 */
#define PCR_UART0_CONF_REG (DR_REG_PCR_BASE + 0x0)
/** PCR_UART0_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable uart0 apb clock
 */
#define PCR_UART0_CLK_EN    (BIT(0))
#define PCR_UART0_CLK_EN_M  (PCR_UART0_CLK_EN_V << PCR_UART0_CLK_EN_S)
#define PCR_UART0_CLK_EN_V  0x00000001U
#define PCR_UART0_CLK_EN_S  0
/** PCR_UART0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset uart0 module
 */
#define PCR_UART0_RST_EN    (BIT(1))
#define PCR_UART0_RST_EN_M  (PCR_UART0_RST_EN_V << PCR_UART0_RST_EN_S)
#define PCR_UART0_RST_EN_V  0x00000001U
#define PCR_UART0_RST_EN_S  1
/** PCR_UART0_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset uart0 module
 */
#define PCR_UART0_READY    (BIT(2))
#define PCR_UART0_READY_M  (PCR_UART0_READY_V << PCR_UART0_READY_S)
#define PCR_UART0_READY_V  0x00000001U
#define PCR_UART0_READY_S  2

/** PCR_UART0_SCLK_CONF_REG register
 *  UART0_SCLK configuration register
 */
#define PCR_UART0_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x4)
/** PCR_UART0_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the uart0 function clock.
 */
#define PCR_UART0_SCLK_DIV_A    0x0000003FU
#define PCR_UART0_SCLK_DIV_A_M  (PCR_UART0_SCLK_DIV_A_V << PCR_UART0_SCLK_DIV_A_S)
#define PCR_UART0_SCLK_DIV_A_V  0x0000003FU
#define PCR_UART0_SCLK_DIV_A_S  0
/** PCR_UART0_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the uart0 function clock.
 */
#define PCR_UART0_SCLK_DIV_B    0x0000003FU
#define PCR_UART0_SCLK_DIV_B_M  (PCR_UART0_SCLK_DIV_B_V << PCR_UART0_SCLK_DIV_B_S)
#define PCR_UART0_SCLK_DIV_B_V  0x0000003FU
#define PCR_UART0_SCLK_DIV_B_S  6
/** PCR_UART0_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the uart0 function clock.
 */
#define PCR_UART0_SCLK_DIV_NUM    0x000000FFU
#define PCR_UART0_SCLK_DIV_NUM_M  (PCR_UART0_SCLK_DIV_NUM_V << PCR_UART0_SCLK_DIV_NUM_S)
#define PCR_UART0_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_UART0_SCLK_DIV_NUM_S  12
/** PCR_UART0_SCLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of UART0.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_UART0_SCLK_SEL    0x00000003U
#define PCR_UART0_SCLK_SEL_M  (PCR_UART0_SCLK_SEL_V << PCR_UART0_SCLK_SEL_S)
#define PCR_UART0_SCLK_SEL_V  0x00000003U
#define PCR_UART0_SCLK_SEL_S  20
/** PCR_UART0_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable uart0 function clock
 */
#define PCR_UART0_SCLK_EN    (BIT(22))
#define PCR_UART0_SCLK_EN_M  (PCR_UART0_SCLK_EN_V << PCR_UART0_SCLK_EN_S)
#define PCR_UART0_SCLK_EN_V  0x00000001U
#define PCR_UART0_SCLK_EN_S  22

/** PCR_UART0_MEM_LP_CTRL_REG register
 *  UART0 memory power control register
 */
#define PCR_UART0_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x8)
/** PCR_UART0_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures uart0 memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_UART0_MEM_LP_MODE    0x00000003U
#define PCR_UART0_MEM_LP_MODE_M  (PCR_UART0_MEM_LP_MODE_V << PCR_UART0_MEM_LP_MODE_S)
#define PCR_UART0_MEM_LP_MODE_V  0x00000003U
#define PCR_UART0_MEM_LP_MODE_S  0
/** PCR_UART0_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down uart0 memory.
 */
#define PCR_UART0_MEM_LP_EN    (BIT(2))
#define PCR_UART0_MEM_LP_EN_M  (PCR_UART0_MEM_LP_EN_V << PCR_UART0_MEM_LP_EN_S)
#define PCR_UART0_MEM_LP_EN_V  0x00000001U
#define PCR_UART0_MEM_LP_EN_S  2
/** PCR_UART0_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control uart0 memory, disable hardware control.
 */
#define PCR_UART0_MEM_FORCE_CTRL    (BIT(3))
#define PCR_UART0_MEM_FORCE_CTRL_M  (PCR_UART0_MEM_FORCE_CTRL_V << PCR_UART0_MEM_FORCE_CTRL_S)
#define PCR_UART0_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_UART0_MEM_FORCE_CTRL_S  3

/** PCR_UART1_CONF_REG register
 *  UART1 configuration register
 */
#define PCR_UART1_CONF_REG (DR_REG_PCR_BASE + 0xc)
/** PCR_UART1_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable uart1 apb clock
 */
#define PCR_UART1_CLK_EN    (BIT(0))
#define PCR_UART1_CLK_EN_M  (PCR_UART1_CLK_EN_V << PCR_UART1_CLK_EN_S)
#define PCR_UART1_CLK_EN_V  0x00000001U
#define PCR_UART1_CLK_EN_S  0
/** PCR_UART1_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset uart1 module
 */
#define PCR_UART1_RST_EN    (BIT(1))
#define PCR_UART1_RST_EN_M  (PCR_UART1_RST_EN_V << PCR_UART1_RST_EN_S)
#define PCR_UART1_RST_EN_V  0x00000001U
#define PCR_UART1_RST_EN_S  1
/** PCR_UART1_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset uart1 module
 */
#define PCR_UART1_READY    (BIT(2))
#define PCR_UART1_READY_M  (PCR_UART1_READY_V << PCR_UART1_READY_S)
#define PCR_UART1_READY_V  0x00000001U
#define PCR_UART1_READY_S  2

/** PCR_UART1_SCLK_CONF_REG register
 *  UART1_SCLK configuration register
 */
#define PCR_UART1_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x10)
/** PCR_UART1_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the uart1 function clock.
 */
#define PCR_UART1_SCLK_DIV_A    0x0000003FU
#define PCR_UART1_SCLK_DIV_A_M  (PCR_UART1_SCLK_DIV_A_V << PCR_UART1_SCLK_DIV_A_S)
#define PCR_UART1_SCLK_DIV_A_V  0x0000003FU
#define PCR_UART1_SCLK_DIV_A_S  0
/** PCR_UART1_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the uart1 function clock.
 */
#define PCR_UART1_SCLK_DIV_B    0x0000003FU
#define PCR_UART1_SCLK_DIV_B_M  (PCR_UART1_SCLK_DIV_B_V << PCR_UART1_SCLK_DIV_B_S)
#define PCR_UART1_SCLK_DIV_B_V  0x0000003FU
#define PCR_UART1_SCLK_DIV_B_S  6
/** PCR_UART1_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the uart1 function clock.
 */
#define PCR_UART1_SCLK_DIV_NUM    0x000000FFU
#define PCR_UART1_SCLK_DIV_NUM_M  (PCR_UART1_SCLK_DIV_NUM_V << PCR_UART1_SCLK_DIV_NUM_S)
#define PCR_UART1_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_UART1_SCLK_DIV_NUM_S  12
/** PCR_UART1_SCLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of UART1.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_UART1_SCLK_SEL    0x00000003U
#define PCR_UART1_SCLK_SEL_M  (PCR_UART1_SCLK_SEL_V << PCR_UART1_SCLK_SEL_S)
#define PCR_UART1_SCLK_SEL_V  0x00000003U
#define PCR_UART1_SCLK_SEL_S  20
/** PCR_UART1_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable uart0 function clock
 */
#define PCR_UART1_SCLK_EN    (BIT(22))
#define PCR_UART1_SCLK_EN_M  (PCR_UART1_SCLK_EN_V << PCR_UART1_SCLK_EN_S)
#define PCR_UART1_SCLK_EN_V  0x00000001U
#define PCR_UART1_SCLK_EN_S  22

/** PCR_UART1_MEM_LP_CTRL_REG register
 *  UART1 memory power control register
 */
#define PCR_UART1_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x14)
/** PCR_UART1_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures uart1 memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_UART1_MEM_LP_MODE    0x00000003U
#define PCR_UART1_MEM_LP_MODE_M  (PCR_UART1_MEM_LP_MODE_V << PCR_UART1_MEM_LP_MODE_S)
#define PCR_UART1_MEM_LP_MODE_V  0x00000003U
#define PCR_UART1_MEM_LP_MODE_S  0
/** PCR_UART1_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down uart1 memory.
 */
#define PCR_UART1_MEM_LP_EN    (BIT(2))
#define PCR_UART1_MEM_LP_EN_M  (PCR_UART1_MEM_LP_EN_V << PCR_UART1_MEM_LP_EN_S)
#define PCR_UART1_MEM_LP_EN_V  0x00000001U
#define PCR_UART1_MEM_LP_EN_S  2
/** PCR_UART1_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control uart1 memory, disable hardware control.
 */
#define PCR_UART1_MEM_FORCE_CTRL    (BIT(3))
#define PCR_UART1_MEM_FORCE_CTRL_M  (PCR_UART1_MEM_FORCE_CTRL_V << PCR_UART1_MEM_FORCE_CTRL_S)
#define PCR_UART1_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_UART1_MEM_FORCE_CTRL_S  3

/** PCR_MSPI_CONF_REG register
 *  MSPI configuration register
 */
#define PCR_MSPI_CONF_REG (DR_REG_PCR_BASE + 0x18)
/** PCR_MSPI_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable mspi apb clock and mspi pll clock
 */
#define PCR_MSPI_CLK_EN    (BIT(0))
#define PCR_MSPI_CLK_EN_M  (PCR_MSPI_CLK_EN_V << PCR_MSPI_CLK_EN_S)
#define PCR_MSPI_CLK_EN_V  0x00000001U
#define PCR_MSPI_CLK_EN_S  0
/** PCR_MSPI_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset mspi module
 */
#define PCR_MSPI_RST_EN    (BIT(1))
#define PCR_MSPI_RST_EN_M  (PCR_MSPI_RST_EN_V << PCR_MSPI_RST_EN_S)
#define PCR_MSPI_RST_EN_V  0x00000001U
#define PCR_MSPI_RST_EN_S  1
/** PCR_MSPI_PLL_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable mspi pll clock
 */
#define PCR_MSPI_PLL_CLK_EN    (BIT(2))
#define PCR_MSPI_PLL_CLK_EN_M  (PCR_MSPI_PLL_CLK_EN_V << PCR_MSPI_PLL_CLK_EN_S)
#define PCR_MSPI_PLL_CLK_EN_V  0x00000001U
#define PCR_MSPI_PLL_CLK_EN_S  2
/** PCR_MSPI_READY : RO; bitpos: [3]; default: 1;
 *  Query this field after reset mspi module
 */
#define PCR_MSPI_READY    (BIT(3))
#define PCR_MSPI_READY_M  (PCR_MSPI_READY_V << PCR_MSPI_READY_S)
#define PCR_MSPI_READY_V  0x00000001U
#define PCR_MSPI_READY_S  3

/** PCR_MSPI_CLK_CONF_REG register
 *  MSPI_CLK configuration register
 */
#define PCR_MSPI_CLK_CONF_REG (DR_REG_PCR_BASE + 0x1c)
/** PCR_MSPI_FAST_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  Set as one within (0,1,2) to generate div1(default)/div2/div4 of low-speed
 *  clock-source to drive clk_mspi_fast. Only available when the clck-source is a
 *  low-speed clock-source such as XTAL/FOSC.
 */
#define PCR_MSPI_FAST_DIV_NUM    0x000000FFU
#define PCR_MSPI_FAST_DIV_NUM_M  (PCR_MSPI_FAST_DIV_NUM_V << PCR_MSPI_FAST_DIV_NUM_S)
#define PCR_MSPI_FAST_DIV_NUM_V  0x000000FFU
#define PCR_MSPI_FAST_DIV_NUM_S  0
/** PCR_MSPI_FUNC_CLK_SEL : R/W; bitpos: [9:8]; default: 0;
 *  Configures the clock source for MSPI.
 *  0(default): XTAL_CLK
 *  1 RC_FAST_CLK
 *  2: PLL_F480M_CLK
 */
#define PCR_MSPI_FUNC_CLK_SEL    0x00000003U
#define PCR_MSPI_FUNC_CLK_SEL_M  (PCR_MSPI_FUNC_CLK_SEL_V << PCR_MSPI_FUNC_CLK_SEL_S)
#define PCR_MSPI_FUNC_CLK_SEL_V  0x00000003U
#define PCR_MSPI_FUNC_CLK_SEL_S  8
/** PCR_MSPI_FUNC_CLK_EN : R/W; bitpos: [10]; default: 1;
 *  Set 1 to enable mspi func clock
 */
#define PCR_MSPI_FUNC_CLK_EN    (BIT(10))
#define PCR_MSPI_FUNC_CLK_EN_M  (PCR_MSPI_FUNC_CLK_EN_V << PCR_MSPI_FUNC_CLK_EN_S)
#define PCR_MSPI_FUNC_CLK_EN_V  0x00000001U
#define PCR_MSPI_FUNC_CLK_EN_S  10
/** PCR_MSPI_AXI_RST_EN : R/W; bitpos: [11]; default: 0;
 *  Set 1 to reset axi_clock domain of mspi module
 */
#define PCR_MSPI_AXI_RST_EN    (BIT(11))
#define PCR_MSPI_AXI_RST_EN_M  (PCR_MSPI_AXI_RST_EN_V << PCR_MSPI_AXI_RST_EN_S)
#define PCR_MSPI_AXI_RST_EN_V  0x00000001U
#define PCR_MSPI_AXI_RST_EN_S  11

/** PCR_MSPI_MEM_LP_CTRL_REG register
 *  MSPI memory power control register
 */
#define PCR_MSPI_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x20)
/** PCR_MSPI_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures mspi memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_MSPI_MEM_LP_MODE    0x00000003U
#define PCR_MSPI_MEM_LP_MODE_M  (PCR_MSPI_MEM_LP_MODE_V << PCR_MSPI_MEM_LP_MODE_S)
#define PCR_MSPI_MEM_LP_MODE_V  0x00000003U
#define PCR_MSPI_MEM_LP_MODE_S  0
/** PCR_MSPI_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down mspi memory.
 */
#define PCR_MSPI_MEM_LP_EN    (BIT(2))
#define PCR_MSPI_MEM_LP_EN_M  (PCR_MSPI_MEM_LP_EN_V << PCR_MSPI_MEM_LP_EN_S)
#define PCR_MSPI_MEM_LP_EN_V  0x00000001U
#define PCR_MSPI_MEM_LP_EN_S  2
/** PCR_MSPI_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control mspi memory, disable hardware control.
 */
#define PCR_MSPI_MEM_FORCE_CTRL    (BIT(3))
#define PCR_MSPI_MEM_FORCE_CTRL_M  (PCR_MSPI_MEM_FORCE_CTRL_V << PCR_MSPI_MEM_FORCE_CTRL_S)
#define PCR_MSPI_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_MSPI_MEM_FORCE_CTRL_S  3

/** PCR_I2C0_CONF_REG register
 *  I2C configuration register
 */
#define PCR_I2C0_CONF_REG (DR_REG_PCR_BASE + 0x24)
/** PCR_I2C0_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable i2c0 apb clock
 */
#define PCR_I2C0_CLK_EN    (BIT(0))
#define PCR_I2C0_CLK_EN_M  (PCR_I2C0_CLK_EN_V << PCR_I2C0_CLK_EN_S)
#define PCR_I2C0_CLK_EN_V  0x00000001U
#define PCR_I2C0_CLK_EN_S  0
/** PCR_I2C0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset i2c0 module
 */
#define PCR_I2C0_RST_EN    (BIT(1))
#define PCR_I2C0_RST_EN_M  (PCR_I2C0_RST_EN_V << PCR_I2C0_RST_EN_S)
#define PCR_I2C0_RST_EN_V  0x00000001U
#define PCR_I2C0_RST_EN_S  1

/** PCR_I2C0_SCLK_CONF_REG register
 *  I2C_SCLK configuration register
 */
#define PCR_I2C0_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x28)
/** PCR_I2C0_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the i2c0 function clock.
 */
#define PCR_I2C0_SCLK_DIV_A    0x0000003FU
#define PCR_I2C0_SCLK_DIV_A_M  (PCR_I2C0_SCLK_DIV_A_V << PCR_I2C0_SCLK_DIV_A_S)
#define PCR_I2C0_SCLK_DIV_A_V  0x0000003FU
#define PCR_I2C0_SCLK_DIV_A_S  0
/** PCR_I2C0_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the i2c0 function clock.
 */
#define PCR_I2C0_SCLK_DIV_B    0x0000003FU
#define PCR_I2C0_SCLK_DIV_B_M  (PCR_I2C0_SCLK_DIV_B_V << PCR_I2C0_SCLK_DIV_B_S)
#define PCR_I2C0_SCLK_DIV_B_V  0x0000003FU
#define PCR_I2C0_SCLK_DIV_B_S  6
/** PCR_I2C0_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the i2c0 function clock.
 */
#define PCR_I2C0_SCLK_DIV_NUM    0x000000FFU
#define PCR_I2C0_SCLK_DIV_NUM_M  (PCR_I2C0_SCLK_DIV_NUM_V << PCR_I2C0_SCLK_DIV_NUM_S)
#define PCR_I2C0_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_I2C0_SCLK_DIV_NUM_S  12
/** PCR_I2C0_SCLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of I2C.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 */
#define PCR_I2C0_SCLK_SEL    (BIT(20))
#define PCR_I2C0_SCLK_SEL_M  (PCR_I2C0_SCLK_SEL_V << PCR_I2C0_SCLK_SEL_S)
#define PCR_I2C0_SCLK_SEL_V  0x00000001U
#define PCR_I2C0_SCLK_SEL_S  20
/** PCR_I2C0_SCLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable i2c function clock
 */
#define PCR_I2C0_SCLK_EN    (BIT(22))
#define PCR_I2C0_SCLK_EN_M  (PCR_I2C0_SCLK_EN_V << PCR_I2C0_SCLK_EN_S)
#define PCR_I2C0_SCLK_EN_V  0x00000001U
#define PCR_I2C0_SCLK_EN_S  22

/** PCR_TWAI0_CONF_REG register
 *  TWAI0 configuration register
 */
#define PCR_TWAI0_CONF_REG (DR_REG_PCR_BASE + 0x2c)
/** PCR_TWAI0_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable twai0 apb clock
 */
#define PCR_TWAI0_CLK_EN    (BIT(0))
#define PCR_TWAI0_CLK_EN_M  (PCR_TWAI0_CLK_EN_V << PCR_TWAI0_CLK_EN_S)
#define PCR_TWAI0_CLK_EN_V  0x00000001U
#define PCR_TWAI0_CLK_EN_S  0
/** PCR_TWAI0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset twai0 module
 */
#define PCR_TWAI0_RST_EN    (BIT(1))
#define PCR_TWAI0_RST_EN_M  (PCR_TWAI0_RST_EN_V << PCR_TWAI0_RST_EN_S)
#define PCR_TWAI0_RST_EN_V  0x00000001U
#define PCR_TWAI0_RST_EN_S  1
/** PCR_TWAI0_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset twai0 module
 */
#define PCR_TWAI0_READY    (BIT(2))
#define PCR_TWAI0_READY_M  (PCR_TWAI0_READY_V << PCR_TWAI0_READY_S)
#define PCR_TWAI0_READY_V  0x00000001U
#define PCR_TWAI0_READY_S  2

/** PCR_TWAI0_FUNC_CLK_CONF_REG register
 *  TWAI0_FUNC_CLK configuration register
 */
#define PCR_TWAI0_FUNC_CLK_CONF_REG (DR_REG_PCR_BASE + 0x30)
/** PCR_TWAI0_FUNC_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of TWAI0.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F96M_CLK
 */
#define PCR_TWAI0_FUNC_CLK_SEL    0x00000003U
#define PCR_TWAI0_FUNC_CLK_SEL_M  (PCR_TWAI0_FUNC_CLK_SEL_V << PCR_TWAI0_FUNC_CLK_SEL_S)
#define PCR_TWAI0_FUNC_CLK_SEL_V  0x00000003U
#define PCR_TWAI0_FUNC_CLK_SEL_S  20
/** PCR_TWAI0_FUNC_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable twai0 function clock
 */
#define PCR_TWAI0_FUNC_CLK_EN    (BIT(22))
#define PCR_TWAI0_FUNC_CLK_EN_M  (PCR_TWAI0_FUNC_CLK_EN_V << PCR_TWAI0_FUNC_CLK_EN_S)
#define PCR_TWAI0_FUNC_CLK_EN_V  0x00000001U
#define PCR_TWAI0_FUNC_CLK_EN_S  22

/** PCR_UHCI_CONF_REG register
 *  UHCI configuration register
 */
#define PCR_UHCI_CONF_REG (DR_REG_PCR_BASE + 0x34)
/** PCR_UHCI_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable uhci clock
 */
#define PCR_UHCI_CLK_EN    (BIT(0))
#define PCR_UHCI_CLK_EN_M  (PCR_UHCI_CLK_EN_V << PCR_UHCI_CLK_EN_S)
#define PCR_UHCI_CLK_EN_V  0x00000001U
#define PCR_UHCI_CLK_EN_S  0
/** PCR_UHCI_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset uhci module
 */
#define PCR_UHCI_RST_EN    (BIT(1))
#define PCR_UHCI_RST_EN_M  (PCR_UHCI_RST_EN_V << PCR_UHCI_RST_EN_S)
#define PCR_UHCI_RST_EN_V  0x00000001U
#define PCR_UHCI_RST_EN_S  1
/** PCR_UHCI_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset uhci module
 */
#define PCR_UHCI_READY    (BIT(2))
#define PCR_UHCI_READY_M  (PCR_UHCI_READY_V << PCR_UHCI_READY_S)
#define PCR_UHCI_READY_V  0x00000001U
#define PCR_UHCI_READY_S  2

/** PCR_RMT_CONF_REG register
 *  RMT configuration register
 */
#define PCR_RMT_CONF_REG (DR_REG_PCR_BASE + 0x38)
/** PCR_RMT_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable rmt apb clock
 */
#define PCR_RMT_CLK_EN    (BIT(0))
#define PCR_RMT_CLK_EN_M  (PCR_RMT_CLK_EN_V << PCR_RMT_CLK_EN_S)
#define PCR_RMT_CLK_EN_V  0x00000001U
#define PCR_RMT_CLK_EN_S  0
/** PCR_RMT_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset rmt module
 */
#define PCR_RMT_RST_EN    (BIT(1))
#define PCR_RMT_RST_EN_M  (PCR_RMT_RST_EN_V << PCR_RMT_RST_EN_S)
#define PCR_RMT_RST_EN_V  0x00000001U
#define PCR_RMT_RST_EN_S  1

/** PCR_RMT_SCLK_CONF_REG register
 *  RMT_SCLK configuration register
 */
#define PCR_RMT_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x3c)
/** PCR_RMT_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the rmt function clock.
 */
#define PCR_RMT_SCLK_DIV_A    0x0000003FU
#define PCR_RMT_SCLK_DIV_A_M  (PCR_RMT_SCLK_DIV_A_V << PCR_RMT_SCLK_DIV_A_S)
#define PCR_RMT_SCLK_DIV_A_V  0x0000003FU
#define PCR_RMT_SCLK_DIV_A_S  0
/** PCR_RMT_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the rmt function clock.
 */
#define PCR_RMT_SCLK_DIV_B    0x0000003FU
#define PCR_RMT_SCLK_DIV_B_M  (PCR_RMT_SCLK_DIV_B_V << PCR_RMT_SCLK_DIV_B_S)
#define PCR_RMT_SCLK_DIV_B_V  0x0000003FU
#define PCR_RMT_SCLK_DIV_B_S  6
/** PCR_RMT_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 1;
 *  The integral part of the frequency divider factor of the rmt function clock.
 */
#define PCR_RMT_SCLK_DIV_NUM    0x000000FFU
#define PCR_RMT_SCLK_DIV_NUM_M  (PCR_RMT_SCLK_DIV_NUM_V << PCR_RMT_SCLK_DIV_NUM_S)
#define PCR_RMT_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_RMT_SCLK_DIV_NUM_S  12
/** PCR_RMT_SCLK_SEL : R/W; bitpos: [20]; default: 1;
 *  Configures the clock source of RMT.
 *  0: XTAL_CLK
 *  1 (default): RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_RMT_SCLK_SEL    (BIT(20))
#define PCR_RMT_SCLK_SEL_M  (PCR_RMT_SCLK_SEL_V << PCR_RMT_SCLK_SEL_S)
#define PCR_RMT_SCLK_SEL_V  0x00000001U
#define PCR_RMT_SCLK_SEL_S  20
/** PCR_RMT_SCLK_EN : R/W; bitpos: [21]; default: 0;
 *  Set 1 to enable rmt function clock
 */
#define PCR_RMT_SCLK_EN    (BIT(21))
#define PCR_RMT_SCLK_EN_M  (PCR_RMT_SCLK_EN_V << PCR_RMT_SCLK_EN_S)
#define PCR_RMT_SCLK_EN_V  0x00000001U
#define PCR_RMT_SCLK_EN_S  21

/** PCR_RMT_MEM_LP_CTRL_REG register
 *  RMT memory power control register
 */
#define PCR_RMT_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x40)
/** PCR_RMT_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures rmt memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_RMT_MEM_LP_MODE    0x00000003U
#define PCR_RMT_MEM_LP_MODE_M  (PCR_RMT_MEM_LP_MODE_V << PCR_RMT_MEM_LP_MODE_S)
#define PCR_RMT_MEM_LP_MODE_V  0x00000003U
#define PCR_RMT_MEM_LP_MODE_S  0
/** PCR_RMT_MEM_LP_EN : R/W; bitpos: [2]; default: 1;
 *  Set this bit to power down rmt memory.
 */
#define PCR_RMT_MEM_LP_EN    (BIT(2))
#define PCR_RMT_MEM_LP_EN_M  (PCR_RMT_MEM_LP_EN_V << PCR_RMT_MEM_LP_EN_S)
#define PCR_RMT_MEM_LP_EN_V  0x00000001U
#define PCR_RMT_MEM_LP_EN_S  2
/** PCR_RMT_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control rmt memory, disable hardware control.
 */
#define PCR_RMT_MEM_FORCE_CTRL    (BIT(3))
#define PCR_RMT_MEM_FORCE_CTRL_M  (PCR_RMT_MEM_FORCE_CTRL_V << PCR_RMT_MEM_FORCE_CTRL_S)
#define PCR_RMT_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_RMT_MEM_FORCE_CTRL_S  3

/** PCR_LEDC_CONF_REG register
 *  LEDC configuration register
 */
#define PCR_LEDC_CONF_REG (DR_REG_PCR_BASE + 0x44)
/** PCR_LEDC_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable ledc apb clock
 */
#define PCR_LEDC_CLK_EN    (BIT(0))
#define PCR_LEDC_CLK_EN_M  (PCR_LEDC_CLK_EN_V << PCR_LEDC_CLK_EN_S)
#define PCR_LEDC_CLK_EN_V  0x00000001U
#define PCR_LEDC_CLK_EN_S  0
/** PCR_LEDC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset ledc module
 */
#define PCR_LEDC_RST_EN    (BIT(1))
#define PCR_LEDC_RST_EN_M  (PCR_LEDC_RST_EN_V << PCR_LEDC_RST_EN_S)
#define PCR_LEDC_RST_EN_V  0x00000001U
#define PCR_LEDC_RST_EN_S  1
/** PCR_LEDC_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset ledc module
 */
#define PCR_LEDC_READY    (BIT(2))
#define PCR_LEDC_READY_M  (PCR_LEDC_READY_V << PCR_LEDC_READY_S)
#define PCR_LEDC_READY_V  0x00000001U
#define PCR_LEDC_READY_S  2

/** PCR_LEDC_SCLK_CONF_REG register
 *  LEDC_SCLK configuration register
 */
#define PCR_LEDC_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x48)
/** PCR_LEDC_SCLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of LEDC.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_LEDC_SCLK_SEL    0x00000003U
#define PCR_LEDC_SCLK_SEL_M  (PCR_LEDC_SCLK_SEL_V << PCR_LEDC_SCLK_SEL_S)
#define PCR_LEDC_SCLK_SEL_V  0x00000003U
#define PCR_LEDC_SCLK_SEL_S  20
/** PCR_LEDC_SCLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable ledc function clock
 */
#define PCR_LEDC_SCLK_EN    (BIT(22))
#define PCR_LEDC_SCLK_EN_M  (PCR_LEDC_SCLK_EN_V << PCR_LEDC_SCLK_EN_S)
#define PCR_LEDC_SCLK_EN_V  0x00000001U
#define PCR_LEDC_SCLK_EN_S  22

/** PCR_TIMERGROUP0_CONF_REG register
 *  TIMERGROUP0 configuration register
 */
#define PCR_TIMERGROUP0_CONF_REG (DR_REG_PCR_BASE + 0x50)
/** PCR_TG0_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable timer_group0 apb clock
 */
#define PCR_TG0_CLK_EN    (BIT(0))
#define PCR_TG0_CLK_EN_M  (PCR_TG0_CLK_EN_V << PCR_TG0_CLK_EN_S)
#define PCR_TG0_CLK_EN_V  0x00000001U
#define PCR_TG0_CLK_EN_S  0
/** PCR_TG0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset timer_group0 module
 */
#define PCR_TG0_RST_EN    (BIT(1))
#define PCR_TG0_RST_EN_M  (PCR_TG0_RST_EN_V << PCR_TG0_RST_EN_S)
#define PCR_TG0_RST_EN_V  0x00000001U
#define PCR_TG0_RST_EN_S  1
/** PCR_TG0_WDT_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset timer_group0 wdt module
 */
#define PCR_TG0_WDT_READY    (BIT(2))
#define PCR_TG0_WDT_READY_M  (PCR_TG0_WDT_READY_V << PCR_TG0_WDT_READY_S)
#define PCR_TG0_WDT_READY_V  0x00000001U
#define PCR_TG0_WDT_READY_S  2
/** PCR_TG0_TIMER0_READY : RO; bitpos: [3]; default: 1;
 *  Query this field after reset timer_group0 timer0 module
 */
#define PCR_TG0_TIMER0_READY    (BIT(3))
#define PCR_TG0_TIMER0_READY_M  (PCR_TG0_TIMER0_READY_V << PCR_TG0_TIMER0_READY_S)
#define PCR_TG0_TIMER0_READY_V  0x00000001U
#define PCR_TG0_TIMER0_READY_S  3
/** PCR_TG0_TIMER1_READY : RO; bitpos: [4]; default: 1;
 *  Query this field after reset timer_group0 timer1 module
 */
#define PCR_TG0_TIMER1_READY    (BIT(4))
#define PCR_TG0_TIMER1_READY_M  (PCR_TG0_TIMER1_READY_V << PCR_TG0_TIMER1_READY_S)
#define PCR_TG0_TIMER1_READY_V  0x00000001U
#define PCR_TG0_TIMER1_READY_S  4

/** PCR_TIMERGROUP0_TIMER_CLK_CONF_REG register
 *  TIMERGROUP0_TIMER_CLK configuration register
 */
#define PCR_TIMERGROUP0_TIMER_CLK_CONF_REG (DR_REG_PCR_BASE + 0x54)
/** PCR_TG0_TIMER_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of general-purpose timers in Timer Group 0.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_TG0_TIMER_CLK_SEL    0x00000003U
#define PCR_TG0_TIMER_CLK_SEL_M  (PCR_TG0_TIMER_CLK_SEL_V << PCR_TG0_TIMER_CLK_SEL_S)
#define PCR_TG0_TIMER_CLK_SEL_V  0x00000003U
#define PCR_TG0_TIMER_CLK_SEL_S  20
/** PCR_TG0_TIMER_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable timer_group0 timer clock
 */
#define PCR_TG0_TIMER_CLK_EN    (BIT(22))
#define PCR_TG0_TIMER_CLK_EN_M  (PCR_TG0_TIMER_CLK_EN_V << PCR_TG0_TIMER_CLK_EN_S)
#define PCR_TG0_TIMER_CLK_EN_V  0x00000001U
#define PCR_TG0_TIMER_CLK_EN_S  22

/** PCR_TIMERGROUP0_WDT_CLK_CONF_REG register
 *  TIMERGROUP0_WDT_CLK configuration register
 */
#define PCR_TIMERGROUP0_WDT_CLK_CONF_REG (DR_REG_PCR_BASE + 0x58)
/** PCR_TG0_WDT_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of WDT in Timer Group 0.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_TG0_WDT_CLK_SEL    0x00000003U
#define PCR_TG0_WDT_CLK_SEL_M  (PCR_TG0_WDT_CLK_SEL_V << PCR_TG0_WDT_CLK_SEL_S)
#define PCR_TG0_WDT_CLK_SEL_V  0x00000003U
#define PCR_TG0_WDT_CLK_SEL_S  20
/** PCR_TG0_WDT_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable timer_group0 wdt clock
 */
#define PCR_TG0_WDT_CLK_EN    (BIT(22))
#define PCR_TG0_WDT_CLK_EN_M  (PCR_TG0_WDT_CLK_EN_V << PCR_TG0_WDT_CLK_EN_S)
#define PCR_TG0_WDT_CLK_EN_V  0x00000001U
#define PCR_TG0_WDT_CLK_EN_S  22

/** PCR_TIMERGROUP1_CONF_REG register
 *  TIMERGROUP1 configuration register
 */
#define PCR_TIMERGROUP1_CONF_REG (DR_REG_PCR_BASE + 0x5c)
/** PCR_TG1_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable timer_group1 apb clock
 */
#define PCR_TG1_CLK_EN    (BIT(0))
#define PCR_TG1_CLK_EN_M  (PCR_TG1_CLK_EN_V << PCR_TG1_CLK_EN_S)
#define PCR_TG1_CLK_EN_V  0x00000001U
#define PCR_TG1_CLK_EN_S  0
/** PCR_TG1_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset timer_group1 module
 */
#define PCR_TG1_RST_EN    (BIT(1))
#define PCR_TG1_RST_EN_M  (PCR_TG1_RST_EN_V << PCR_TG1_RST_EN_S)
#define PCR_TG1_RST_EN_V  0x00000001U
#define PCR_TG1_RST_EN_S  1
/** PCR_TG1_WDT_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset timer_group1 wdt module
 */
#define PCR_TG1_WDT_READY    (BIT(2))
#define PCR_TG1_WDT_READY_M  (PCR_TG1_WDT_READY_V << PCR_TG1_WDT_READY_S)
#define PCR_TG1_WDT_READY_V  0x00000001U
#define PCR_TG1_WDT_READY_S  2
/** PCR_TG1_TIMER0_READY : RO; bitpos: [3]; default: 1;
 *  Query this field after reset timer_group1 timer0 module
 */
#define PCR_TG1_TIMER0_READY    (BIT(3))
#define PCR_TG1_TIMER0_READY_M  (PCR_TG1_TIMER0_READY_V << PCR_TG1_TIMER0_READY_S)
#define PCR_TG1_TIMER0_READY_V  0x00000001U
#define PCR_TG1_TIMER0_READY_S  3
/** PCR_TG1_TIMER1_READY : RO; bitpos: [4]; default: 1;
 *  Query this field after reset timer_group1 timer1 module
 */
#define PCR_TG1_TIMER1_READY    (BIT(4))
#define PCR_TG1_TIMER1_READY_M  (PCR_TG1_TIMER1_READY_V << PCR_TG1_TIMER1_READY_S)
#define PCR_TG1_TIMER1_READY_V  0x00000001U
#define PCR_TG1_TIMER1_READY_S  4

/** PCR_TIMERGROUP1_TIMER_CLK_CONF_REG register
 *  TIMERGROUP1_TIMER_CLK configuration register
 */
#define PCR_TIMERGROUP1_TIMER_CLK_CONF_REG (DR_REG_PCR_BASE + 0x60)
/** PCR_TG1_TIMER_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of general-purpose timers in Timer Group 1.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_TG1_TIMER_CLK_SEL    0x00000003U
#define PCR_TG1_TIMER_CLK_SEL_M  (PCR_TG1_TIMER_CLK_SEL_V << PCR_TG1_TIMER_CLK_SEL_S)
#define PCR_TG1_TIMER_CLK_SEL_V  0x00000003U
#define PCR_TG1_TIMER_CLK_SEL_S  20
/** PCR_TG1_TIMER_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable timer_group1 timer clock
 */
#define PCR_TG1_TIMER_CLK_EN    (BIT(22))
#define PCR_TG1_TIMER_CLK_EN_M  (PCR_TG1_TIMER_CLK_EN_V << PCR_TG1_TIMER_CLK_EN_S)
#define PCR_TG1_TIMER_CLK_EN_V  0x00000001U
#define PCR_TG1_TIMER_CLK_EN_S  22

/** PCR_TIMERGROUP1_WDT_CLK_CONF_REG register
 *  TIMERGROUP1_WDT_CLK configuration register
 */
#define PCR_TIMERGROUP1_WDT_CLK_CONF_REG (DR_REG_PCR_BASE + 0x64)
/** PCR_TG1_WDT_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of WDT in Timer Group 1.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_TG1_WDT_CLK_SEL    0x00000003U
#define PCR_TG1_WDT_CLK_SEL_M  (PCR_TG1_WDT_CLK_SEL_V << PCR_TG1_WDT_CLK_SEL_S)
#define PCR_TG1_WDT_CLK_SEL_V  0x00000003U
#define PCR_TG1_WDT_CLK_SEL_S  20
/** PCR_TG1_WDT_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable timer_group0 wdt clock
 */
#define PCR_TG1_WDT_CLK_EN    (BIT(22))
#define PCR_TG1_WDT_CLK_EN_M  (PCR_TG1_WDT_CLK_EN_V << PCR_TG1_WDT_CLK_EN_S)
#define PCR_TG1_WDT_CLK_EN_V  0x00000001U
#define PCR_TG1_WDT_CLK_EN_S  22

/** PCR_SYSTIMER_CONF_REG register
 *  SYSTIMER configuration register
 */
#define PCR_SYSTIMER_CONF_REG (DR_REG_PCR_BASE + 0x68)
/** PCR_SYSTIMER_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable systimer apb clock
 */
#define PCR_SYSTIMER_CLK_EN    (BIT(0))
#define PCR_SYSTIMER_CLK_EN_M  (PCR_SYSTIMER_CLK_EN_V << PCR_SYSTIMER_CLK_EN_S)
#define PCR_SYSTIMER_CLK_EN_V  0x00000001U
#define PCR_SYSTIMER_CLK_EN_S  0
/** PCR_SYSTIMER_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset systimer module
 */
#define PCR_SYSTIMER_RST_EN    (BIT(1))
#define PCR_SYSTIMER_RST_EN_M  (PCR_SYSTIMER_RST_EN_V << PCR_SYSTIMER_RST_EN_S)
#define PCR_SYSTIMER_RST_EN_V  0x00000001U
#define PCR_SYSTIMER_RST_EN_S  1
/** PCR_SYSTIMER_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset systimer module
 */
#define PCR_SYSTIMER_READY    (BIT(2))
#define PCR_SYSTIMER_READY_M  (PCR_SYSTIMER_READY_V << PCR_SYSTIMER_READY_S)
#define PCR_SYSTIMER_READY_V  0x00000001U
#define PCR_SYSTIMER_READY_S  2

/** PCR_SYSTIMER_FUNC_CLK_CONF_REG register
 *  SYSTIMER_FUNC_CLK configuration register
 */
#define PCR_SYSTIMER_FUNC_CLK_CONF_REG (DR_REG_PCR_BASE + 0x6c)
/** PCR_SYSTIMER_FUNC_CLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of System Timer.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 */
#define PCR_SYSTIMER_FUNC_CLK_SEL    (BIT(20))
#define PCR_SYSTIMER_FUNC_CLK_SEL_M  (PCR_SYSTIMER_FUNC_CLK_SEL_V << PCR_SYSTIMER_FUNC_CLK_SEL_S)
#define PCR_SYSTIMER_FUNC_CLK_SEL_V  0x00000001U
#define PCR_SYSTIMER_FUNC_CLK_SEL_S  20
/** PCR_SYSTIMER_FUNC_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable systimer function clock
 */
#define PCR_SYSTIMER_FUNC_CLK_EN    (BIT(22))
#define PCR_SYSTIMER_FUNC_CLK_EN_M  (PCR_SYSTIMER_FUNC_CLK_EN_V << PCR_SYSTIMER_FUNC_CLK_EN_S)
#define PCR_SYSTIMER_FUNC_CLK_EN_V  0x00000001U
#define PCR_SYSTIMER_FUNC_CLK_EN_S  22

/** PCR_I2S_CONF_REG register
 *  I2S configuration register
 */
#define PCR_I2S_CONF_REG (DR_REG_PCR_BASE + 0x70)
/** PCR_I2S_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable i2s apb clock
 */
#define PCR_I2S_CLK_EN    (BIT(0))
#define PCR_I2S_CLK_EN_M  (PCR_I2S_CLK_EN_V << PCR_I2S_CLK_EN_S)
#define PCR_I2S_CLK_EN_V  0x00000001U
#define PCR_I2S_CLK_EN_S  0
/** PCR_I2S_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset i2s module
 */
#define PCR_I2S_RST_EN    (BIT(1))
#define PCR_I2S_RST_EN_M  (PCR_I2S_RST_EN_V << PCR_I2S_RST_EN_S)
#define PCR_I2S_RST_EN_V  0x00000001U
#define PCR_I2S_RST_EN_S  1
/** PCR_I2S_RX_READY : RO; bitpos: [2]; default: 1;
 *  Query this field before using i2s rx function, after reset i2s module
 */
#define PCR_I2S_RX_READY    (BIT(2))
#define PCR_I2S_RX_READY_M  (PCR_I2S_RX_READY_V << PCR_I2S_RX_READY_S)
#define PCR_I2S_RX_READY_V  0x00000001U
#define PCR_I2S_RX_READY_S  2
/** PCR_I2S_TX_READY : RO; bitpos: [3]; default: 1;
 *  Query this field before using i2s tx function, after reset i2s module
 */
#define PCR_I2S_TX_READY    (BIT(3))
#define PCR_I2S_TX_READY_M  (PCR_I2S_TX_READY_V << PCR_I2S_TX_READY_S)
#define PCR_I2S_TX_READY_V  0x00000001U
#define PCR_I2S_TX_READY_S  3

/** PCR_I2S_TX_CLKM_CONF_REG register
 *  I2S_TX_CLKM configuration register
 */
#define PCR_I2S_TX_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x74)
/** PCR_I2S_TX_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 2;
 *  Integral I2S TX clock divider value. f_I2S_CLK = f_I2S_CLK_S/(N+b/a). There will be
 *  (a-b) * n-div and b * (n+1)-div.  So the average combination will be:  for b <=
 *  a/2, z * [x * n-div + (n+1)-div] + y * n-div. For b > a/2, z * [n-div + x *
 *  (n+1)-div] + y * (n+1)-div.
 */
#define PCR_I2S_TX_CLKM_DIV_NUM    0x000000FFU
#define PCR_I2S_TX_CLKM_DIV_NUM_M  (PCR_I2S_TX_CLKM_DIV_NUM_V << PCR_I2S_TX_CLKM_DIV_NUM_S)
#define PCR_I2S_TX_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_I2S_TX_CLKM_DIV_NUM_S  12
/** PCR_I2S_TX_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of I2S TX.
 *  0 (default): XTAL_CLK
 *  1: PLL_F240M_CLK
 *  2: PLL_F160M_CLK
 *  3: I2S_MCLK_in
 */
#define PCR_I2S_TX_CLKM_SEL    0x00000003U
#define PCR_I2S_TX_CLKM_SEL_M  (PCR_I2S_TX_CLKM_SEL_V << PCR_I2S_TX_CLKM_SEL_S)
#define PCR_I2S_TX_CLKM_SEL_V  0x00000003U
#define PCR_I2S_TX_CLKM_SEL_S  20
/** PCR_I2S_TX_CLKM_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable i2s_tx function clock
 */
#define PCR_I2S_TX_CLKM_EN    (BIT(22))
#define PCR_I2S_TX_CLKM_EN_M  (PCR_I2S_TX_CLKM_EN_V << PCR_I2S_TX_CLKM_EN_S)
#define PCR_I2S_TX_CLKM_EN_V  0x00000001U
#define PCR_I2S_TX_CLKM_EN_S  22

/** PCR_I2S_TX_CLKM_DIV_CONF_REG register
 *  I2S_TX_CLKM_DIV configuration register
 */
#define PCR_I2S_TX_CLKM_DIV_CONF_REG (DR_REG_PCR_BASE + 0x78)
/** PCR_I2S_TX_CLKM_DIV_Z : R/W; bitpos: [8:0]; default: 0;
 *  For b <= a/2, the value of I2S_TX_CLKM_DIV_Z is b. For b > a/2, the value of
 *  I2S_TX_CLKM_DIV_Z is (a-b).
 */
#define PCR_I2S_TX_CLKM_DIV_Z    0x000001FFU
#define PCR_I2S_TX_CLKM_DIV_Z_M  (PCR_I2S_TX_CLKM_DIV_Z_V << PCR_I2S_TX_CLKM_DIV_Z_S)
#define PCR_I2S_TX_CLKM_DIV_Z_V  0x000001FFU
#define PCR_I2S_TX_CLKM_DIV_Z_S  0
/** PCR_I2S_TX_CLKM_DIV_Y : R/W; bitpos: [17:9]; default: 1;
 *  For b <= a/2, the value of I2S_TX_CLKM_DIV_Y is (a%b) . For b > a/2, the value of
 *  I2S_TX_CLKM_DIV_Y is (a%(a-b)).
 */
#define PCR_I2S_TX_CLKM_DIV_Y    0x000001FFU
#define PCR_I2S_TX_CLKM_DIV_Y_M  (PCR_I2S_TX_CLKM_DIV_Y_V << PCR_I2S_TX_CLKM_DIV_Y_S)
#define PCR_I2S_TX_CLKM_DIV_Y_V  0x000001FFU
#define PCR_I2S_TX_CLKM_DIV_Y_S  9
/** PCR_I2S_TX_CLKM_DIV_X : R/W; bitpos: [26:18]; default: 0;
 *  For b <= a/2, the value of I2S_TX_CLKM_DIV_X is (a/b) - 1. For b > a/2, the value
 *  of I2S_TX_CLKM_DIV_X is (a/(a-b)) - 1.
 */
#define PCR_I2S_TX_CLKM_DIV_X    0x000001FFU
#define PCR_I2S_TX_CLKM_DIV_X_M  (PCR_I2S_TX_CLKM_DIV_X_V << PCR_I2S_TX_CLKM_DIV_X_S)
#define PCR_I2S_TX_CLKM_DIV_X_V  0x000001FFU
#define PCR_I2S_TX_CLKM_DIV_X_S  18
/** PCR_I2S_TX_CLKM_DIV_YN1 : R/W; bitpos: [27]; default: 0;
 *  For b <= a/2, the value of I2S_TX_CLKM_DIV_YN1 is 0 . For b > a/2, the value of
 *  I2S_TX_CLKM_DIV_YN1 is 1.
 */
#define PCR_I2S_TX_CLKM_DIV_YN1    (BIT(27))
#define PCR_I2S_TX_CLKM_DIV_YN1_M  (PCR_I2S_TX_CLKM_DIV_YN1_V << PCR_I2S_TX_CLKM_DIV_YN1_S)
#define PCR_I2S_TX_CLKM_DIV_YN1_V  0x00000001U
#define PCR_I2S_TX_CLKM_DIV_YN1_S  27

/** PCR_I2S_RX_CLKM_CONF_REG register
 *  I2S_RX_CLKM configuration register
 */
#define PCR_I2S_RX_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x7c)
/** PCR_I2S_RX_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 2;
 *  Integral I2S clock divider value
 */
#define PCR_I2S_RX_CLKM_DIV_NUM    0x000000FFU
#define PCR_I2S_RX_CLKM_DIV_NUM_M  (PCR_I2S_RX_CLKM_DIV_NUM_V << PCR_I2S_RX_CLKM_DIV_NUM_S)
#define PCR_I2S_RX_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_I2S_RX_CLKM_DIV_NUM_S  12
/** PCR_I2S_RX_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of I2S RX.
 *  0 (default): XTAL_CLK
 *  1: PLL_F240M_CLK
 *  2: PLL_F160M_CLK
 *  3: I2S_MCLK_in
 */
#define PCR_I2S_RX_CLKM_SEL    0x00000003U
#define PCR_I2S_RX_CLKM_SEL_M  (PCR_I2S_RX_CLKM_SEL_V << PCR_I2S_RX_CLKM_SEL_S)
#define PCR_I2S_RX_CLKM_SEL_V  0x00000003U
#define PCR_I2S_RX_CLKM_SEL_S  20
/** PCR_I2S_RX_CLKM_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable i2s_rx function clock
 */
#define PCR_I2S_RX_CLKM_EN    (BIT(22))
#define PCR_I2S_RX_CLKM_EN_M  (PCR_I2S_RX_CLKM_EN_V << PCR_I2S_RX_CLKM_EN_S)
#define PCR_I2S_RX_CLKM_EN_V  0x00000001U
#define PCR_I2S_RX_CLKM_EN_S  22
/** PCR_I2S_MCLK_SEL : R/W; bitpos: [23]; default: 0;
 *  Configures to select master clock.
 *  0 (default): I2S_TX_CLK
 *  1: I2S_RX_CLK
 */
#define PCR_I2S_MCLK_SEL    (BIT(23))
#define PCR_I2S_MCLK_SEL_M  (PCR_I2S_MCLK_SEL_V << PCR_I2S_MCLK_SEL_S)
#define PCR_I2S_MCLK_SEL_V  0x00000001U
#define PCR_I2S_MCLK_SEL_S  23

/** PCR_I2S_RX_CLKM_DIV_CONF_REG register
 *  I2S_RX_CLKM_DIV configuration register
 */
#define PCR_I2S_RX_CLKM_DIV_CONF_REG (DR_REG_PCR_BASE + 0x80)
/** PCR_I2S_RX_CLKM_DIV_Z : R/W; bitpos: [8:0]; default: 0;
 *  For b <= a/2, the value of I2S_RX_CLKM_DIV_Z is b. For b > a/2, the value of
 *  I2S_RX_CLKM_DIV_Z is (a-b).
 */
#define PCR_I2S_RX_CLKM_DIV_Z    0x000001FFU
#define PCR_I2S_RX_CLKM_DIV_Z_M  (PCR_I2S_RX_CLKM_DIV_Z_V << PCR_I2S_RX_CLKM_DIV_Z_S)
#define PCR_I2S_RX_CLKM_DIV_Z_V  0x000001FFU
#define PCR_I2S_RX_CLKM_DIV_Z_S  0
/** PCR_I2S_RX_CLKM_DIV_Y : R/W; bitpos: [17:9]; default: 1;
 *  For b <= a/2, the value of I2S_RX_CLKM_DIV_Y is (a%b) . For b > a/2, the value of
 *  I2S_RX_CLKM_DIV_Y is (a%(a-b)).
 */
#define PCR_I2S_RX_CLKM_DIV_Y    0x000001FFU
#define PCR_I2S_RX_CLKM_DIV_Y_M  (PCR_I2S_RX_CLKM_DIV_Y_V << PCR_I2S_RX_CLKM_DIV_Y_S)
#define PCR_I2S_RX_CLKM_DIV_Y_V  0x000001FFU
#define PCR_I2S_RX_CLKM_DIV_Y_S  9
/** PCR_I2S_RX_CLKM_DIV_X : R/W; bitpos: [26:18]; default: 0;
 *  For b <= a/2, the value of I2S_RX_CLKM_DIV_X is (a/b) - 1. For b > a/2, the value
 *  of I2S_RX_CLKM_DIV_X is (a/(a-b)) - 1.
 */
#define PCR_I2S_RX_CLKM_DIV_X    0x000001FFU
#define PCR_I2S_RX_CLKM_DIV_X_M  (PCR_I2S_RX_CLKM_DIV_X_V << PCR_I2S_RX_CLKM_DIV_X_S)
#define PCR_I2S_RX_CLKM_DIV_X_V  0x000001FFU
#define PCR_I2S_RX_CLKM_DIV_X_S  18
/** PCR_I2S_RX_CLKM_DIV_YN1 : R/W; bitpos: [27]; default: 0;
 *  For b <= a/2, the value of I2S_RX_CLKM_DIV_YN1 is 0 . For b > a/2, the value of
 *  I2S_RX_CLKM_DIV_YN1 is 1.
 */
#define PCR_I2S_RX_CLKM_DIV_YN1    (BIT(27))
#define PCR_I2S_RX_CLKM_DIV_YN1_M  (PCR_I2S_RX_CLKM_DIV_YN1_V << PCR_I2S_RX_CLKM_DIV_YN1_S)
#define PCR_I2S_RX_CLKM_DIV_YN1_V  0x00000001U
#define PCR_I2S_RX_CLKM_DIV_YN1_S  27

/** PCR_SARADC_CONF_REG register
 *  SARADC configuration register
 */
#define PCR_SARADC_CONF_REG (DR_REG_PCR_BASE + 0x84)
/** PCR_SARADC_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  no use
 */
#define PCR_SARADC_CLK_EN    (BIT(0))
#define PCR_SARADC_CLK_EN_M  (PCR_SARADC_CLK_EN_V << PCR_SARADC_CLK_EN_S)
#define PCR_SARADC_CLK_EN_V  0x00000001U
#define PCR_SARADC_CLK_EN_S  0
/** PCR_SARADC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset function_register of saradc module
 */
#define PCR_SARADC_RST_EN    (BIT(1))
#define PCR_SARADC_RST_EN_M  (PCR_SARADC_RST_EN_V << PCR_SARADC_RST_EN_S)
#define PCR_SARADC_RST_EN_V  0x00000001U
#define PCR_SARADC_RST_EN_S  1
/** PCR_SARADC_REG_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to enable saradc apb clock
 */
#define PCR_SARADC_REG_CLK_EN    (BIT(2))
#define PCR_SARADC_REG_CLK_EN_M  (PCR_SARADC_REG_CLK_EN_V << PCR_SARADC_REG_CLK_EN_S)
#define PCR_SARADC_REG_CLK_EN_V  0x00000001U
#define PCR_SARADC_REG_CLK_EN_S  2
/** PCR_SARADC_REG_RST_EN : R/W; bitpos: [3]; default: 0;
 *  Set 1 to reset apb_register of saradc module
 */
#define PCR_SARADC_REG_RST_EN    (BIT(3))
#define PCR_SARADC_REG_RST_EN_M  (PCR_SARADC_REG_RST_EN_V << PCR_SARADC_REG_RST_EN_S)
#define PCR_SARADC_REG_RST_EN_V  0x00000001U
#define PCR_SARADC_REG_RST_EN_S  3

/** PCR_SARADC_CLKM_CONF_REG register
 *  SARADC_CLKM configuration register
 */
#define PCR_SARADC_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x88)
/** PCR_SARADC_CLKM_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the saradc function clock.
 */
#define PCR_SARADC_CLKM_DIV_A    0x0000003FU
#define PCR_SARADC_CLKM_DIV_A_M  (PCR_SARADC_CLKM_DIV_A_V << PCR_SARADC_CLKM_DIV_A_S)
#define PCR_SARADC_CLKM_DIV_A_V  0x0000003FU
#define PCR_SARADC_CLKM_DIV_A_S  0
/** PCR_SARADC_CLKM_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the saradc function clock.
 */
#define PCR_SARADC_CLKM_DIV_B    0x0000003FU
#define PCR_SARADC_CLKM_DIV_B_M  (PCR_SARADC_CLKM_DIV_B_V << PCR_SARADC_CLKM_DIV_B_S)
#define PCR_SARADC_CLKM_DIV_B_V  0x0000003FU
#define PCR_SARADC_CLKM_DIV_B_S  6
/** PCR_SARADC_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the saradc function clock.
 */
#define PCR_SARADC_CLKM_DIV_NUM    0x000000FFU
#define PCR_SARADC_CLKM_DIV_NUM_M  (PCR_SARADC_CLKM_DIV_NUM_V << PCR_SARADC_CLKM_DIV_NUM_S)
#define PCR_SARADC_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_SARADC_CLKM_DIV_NUM_S  12
/** PCR_SARADC_CLKM_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of SAR ADC.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 */
#define PCR_SARADC_CLKM_SEL    (BIT(20))
#define PCR_SARADC_CLKM_SEL_M  (PCR_SARADC_CLKM_SEL_V << PCR_SARADC_CLKM_SEL_S)
#define PCR_SARADC_CLKM_SEL_V  0x00000001U
#define PCR_SARADC_CLKM_SEL_S  20
/** PCR_SARADC_CLKM_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable saradc function clock
 */
#define PCR_SARADC_CLKM_EN    (BIT(22))
#define PCR_SARADC_CLKM_EN_M  (PCR_SARADC_CLKM_EN_V << PCR_SARADC_CLKM_EN_S)
#define PCR_SARADC_CLKM_EN_V  0x00000001U
#define PCR_SARADC_CLKM_EN_S  22

/** PCR_TSENS_CLK_CONF_REG register
 *  TSENS_CLK configuration register
 */
#define PCR_TSENS_CLK_CONF_REG (DR_REG_PCR_BASE + 0x8c)
/** PCR_TSENS_CLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of the temperature sensor.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 */
#define PCR_TSENS_CLK_SEL    (BIT(20))
#define PCR_TSENS_CLK_SEL_M  (PCR_TSENS_CLK_SEL_V << PCR_TSENS_CLK_SEL_S)
#define PCR_TSENS_CLK_SEL_V  0x00000001U
#define PCR_TSENS_CLK_SEL_S  20
/** PCR_TSENS_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable tsens clock
 */
#define PCR_TSENS_CLK_EN    (BIT(22))
#define PCR_TSENS_CLK_EN_M  (PCR_TSENS_CLK_EN_V << PCR_TSENS_CLK_EN_S)
#define PCR_TSENS_CLK_EN_V  0x00000001U
#define PCR_TSENS_CLK_EN_S  22
/** PCR_TSENS_RST_EN : R/W; bitpos: [23]; default: 0;
 *  Set 1 to reset tsens module
 */
#define PCR_TSENS_RST_EN    (BIT(23))
#define PCR_TSENS_RST_EN_M  (PCR_TSENS_RST_EN_V << PCR_TSENS_RST_EN_S)
#define PCR_TSENS_RST_EN_V  0x00000001U
#define PCR_TSENS_RST_EN_S  23

/** PCR_USB_DEVICE_CONF_REG register
 *  USB_DEVICE configuration register
 */
#define PCR_USB_DEVICE_CONF_REG (DR_REG_PCR_BASE + 0x90)
/** PCR_USB_DEVICE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable usb_device clock
 */
#define PCR_USB_DEVICE_CLK_EN    (BIT(0))
#define PCR_USB_DEVICE_CLK_EN_M  (PCR_USB_DEVICE_CLK_EN_V << PCR_USB_DEVICE_CLK_EN_S)
#define PCR_USB_DEVICE_CLK_EN_V  0x00000001U
#define PCR_USB_DEVICE_CLK_EN_S  0
/** PCR_USB_DEVICE_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset usb_device module
 */
#define PCR_USB_DEVICE_RST_EN    (BIT(1))
#define PCR_USB_DEVICE_RST_EN_M  (PCR_USB_DEVICE_RST_EN_V << PCR_USB_DEVICE_RST_EN_S)
#define PCR_USB_DEVICE_RST_EN_V  0x00000001U
#define PCR_USB_DEVICE_RST_EN_S  1
/** PCR_USB_DEVICE_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset usb_device module
 */
#define PCR_USB_DEVICE_READY    (BIT(2))
#define PCR_USB_DEVICE_READY_M  (PCR_USB_DEVICE_READY_V << PCR_USB_DEVICE_READY_S)
#define PCR_USB_DEVICE_READY_V  0x00000001U
#define PCR_USB_DEVICE_READY_S  2

/** PCR_USB_DEVICE_MEM_LP_CTRL_REG register
 *  USB_DEVICE memory power control register
 */
#define PCR_USB_DEVICE_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x94)
/** PCR_USB_DEVICE_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures usb_device memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_USB_DEVICE_MEM_LP_MODE    0x00000003U
#define PCR_USB_DEVICE_MEM_LP_MODE_M  (PCR_USB_DEVICE_MEM_LP_MODE_V << PCR_USB_DEVICE_MEM_LP_MODE_S)
#define PCR_USB_DEVICE_MEM_LP_MODE_V  0x00000003U
#define PCR_USB_DEVICE_MEM_LP_MODE_S  0
/** PCR_USB_DEVICE_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down usb_device memory.
 */
#define PCR_USB_DEVICE_MEM_LP_EN    (BIT(2))
#define PCR_USB_DEVICE_MEM_LP_EN_M  (PCR_USB_DEVICE_MEM_LP_EN_V << PCR_USB_DEVICE_MEM_LP_EN_S)
#define PCR_USB_DEVICE_MEM_LP_EN_V  0x00000001U
#define PCR_USB_DEVICE_MEM_LP_EN_S  2
/** PCR_USB_DEVICE_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control usb_device memory, disable hardware control.
 */
#define PCR_USB_DEVICE_MEM_FORCE_CTRL    (BIT(3))
#define PCR_USB_DEVICE_MEM_FORCE_CTRL_M  (PCR_USB_DEVICE_MEM_FORCE_CTRL_V << PCR_USB_DEVICE_MEM_FORCE_CTRL_S)
#define PCR_USB_DEVICE_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_USB_DEVICE_MEM_FORCE_CTRL_S  3

/** PCR_INTMTX_CONF_REG register
 *  INTMTX configuration register
 */
#define PCR_INTMTX_CONF_REG (DR_REG_PCR_BASE + 0x98)
/** PCR_INTMTX_CORE0_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable core0 intmtx clock
 */
#define PCR_INTMTX_CORE0_CLK_EN    (BIT(0))
#define PCR_INTMTX_CORE0_CLK_EN_M  (PCR_INTMTX_CORE0_CLK_EN_V << PCR_INTMTX_CORE0_CLK_EN_S)
#define PCR_INTMTX_CORE0_CLK_EN_V  0x00000001U
#define PCR_INTMTX_CORE0_CLK_EN_S  0
/** PCR_INTMTX_CORE0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset core0 intmtx module
 */
#define PCR_INTMTX_CORE0_RST_EN    (BIT(1))
#define PCR_INTMTX_CORE0_RST_EN_M  (PCR_INTMTX_CORE0_RST_EN_V << PCR_INTMTX_CORE0_RST_EN_S)
#define PCR_INTMTX_CORE0_RST_EN_V  0x00000001U
#define PCR_INTMTX_CORE0_RST_EN_S  1
/** PCR_INTMTX_CORE1_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to enable core1 intmtx clock
 */
#define PCR_INTMTX_CORE1_CLK_EN    (BIT(2))
#define PCR_INTMTX_CORE1_CLK_EN_M  (PCR_INTMTX_CORE1_CLK_EN_V << PCR_INTMTX_CORE1_CLK_EN_S)
#define PCR_INTMTX_CORE1_CLK_EN_V  0x00000001U
#define PCR_INTMTX_CORE1_CLK_EN_S  2
/** PCR_INTMTX_CORE1_RST_EN : R/W; bitpos: [3]; default: 1;
 *  Set 1 to reset core1 intmtx module
 */
#define PCR_INTMTX_CORE1_RST_EN    (BIT(3))
#define PCR_INTMTX_CORE1_RST_EN_M  (PCR_INTMTX_CORE1_RST_EN_V << PCR_INTMTX_CORE1_RST_EN_S)
#define PCR_INTMTX_CORE1_RST_EN_V  0x00000001U
#define PCR_INTMTX_CORE1_RST_EN_S  3
/** PCR_INTMTX_CORE0_READY : RO; bitpos: [4]; default: 1;
 *  Query this field after reset intmtx module
 */
#define PCR_INTMTX_CORE0_READY    (BIT(4))
#define PCR_INTMTX_CORE0_READY_M  (PCR_INTMTX_CORE0_READY_V << PCR_INTMTX_CORE0_READY_S)
#define PCR_INTMTX_CORE0_READY_V  0x00000001U
#define PCR_INTMTX_CORE0_READY_S  4
/** PCR_INTMTX_CORE1_READY : RO; bitpos: [5]; default: 1;
 *  Query this field after reset intmtx module
 */
#define PCR_INTMTX_CORE1_READY    (BIT(5))
#define PCR_INTMTX_CORE1_READY_M  (PCR_INTMTX_CORE1_READY_V << PCR_INTMTX_CORE1_READY_S)
#define PCR_INTMTX_CORE1_READY_V  0x00000001U
#define PCR_INTMTX_CORE1_READY_S  5

/** PCR_PCNT_CONF_REG register
 *  PCNT configuration register
 */
#define PCR_PCNT_CONF_REG (DR_REG_PCR_BASE + 0x9c)
/** PCR_PCNT_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable pcnt clock
 */
#define PCR_PCNT_CLK_EN    (BIT(0))
#define PCR_PCNT_CLK_EN_M  (PCR_PCNT_CLK_EN_V << PCR_PCNT_CLK_EN_S)
#define PCR_PCNT_CLK_EN_V  0x00000001U
#define PCR_PCNT_CLK_EN_S  0
/** PCR_PCNT_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset pcnt module
 */
#define PCR_PCNT_RST_EN    (BIT(1))
#define PCR_PCNT_RST_EN_M  (PCR_PCNT_RST_EN_V << PCR_PCNT_RST_EN_S)
#define PCR_PCNT_RST_EN_V  0x00000001U
#define PCR_PCNT_RST_EN_S  1
/** PCR_PCNT_REG_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to enable pcnt reg clock
 */
#define PCR_PCNT_REG_CLK_EN    (BIT(2))
#define PCR_PCNT_REG_CLK_EN_M  (PCR_PCNT_REG_CLK_EN_V << PCR_PCNT_REG_CLK_EN_S)
#define PCR_PCNT_REG_CLK_EN_V  0x00000001U
#define PCR_PCNT_REG_CLK_EN_S  2
/** PCR_PCNT_REG_RST_EN : R/W; bitpos: [3]; default: 0;
 *  Set 1 to reset pcnt reg module
 */
#define PCR_PCNT_REG_RST_EN    (BIT(3))
#define PCR_PCNT_REG_RST_EN_M  (PCR_PCNT_REG_RST_EN_V << PCR_PCNT_REG_RST_EN_S)
#define PCR_PCNT_REG_RST_EN_V  0x00000001U
#define PCR_PCNT_REG_RST_EN_S  3
/** PCR_PCNT_READY : RO; bitpos: [4]; default: 1;
 *  Query this field after reset pcnt module
 */
#define PCR_PCNT_READY    (BIT(4))
#define PCR_PCNT_READY_M  (PCR_PCNT_READY_V << PCR_PCNT_READY_S)
#define PCR_PCNT_READY_V  0x00000001U
#define PCR_PCNT_READY_S  4
/** PCR_PCNT_CLK_SEL : R/W; bitpos: [6:5]; default: 0;
 *  Configures the clock source of the pcnt.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F32M_CLK
 */
#define PCR_PCNT_CLK_SEL    0x00000003U
#define PCR_PCNT_CLK_SEL_M  (PCR_PCNT_CLK_SEL_V << PCR_PCNT_CLK_SEL_S)
#define PCR_PCNT_CLK_SEL_V  0x00000003U
#define PCR_PCNT_CLK_SEL_S  5
/** PCR_PCNT_CLK_DIV_NUM : R/W; bitpos: [10:7]; default: 0;
 *  The integral part of the frequency divider factor of the pcnt function clock.
 */
#define PCR_PCNT_CLK_DIV_NUM    0x0000000FU
#define PCR_PCNT_CLK_DIV_NUM_M  (PCR_PCNT_CLK_DIV_NUM_V << PCR_PCNT_CLK_DIV_NUM_S)
#define PCR_PCNT_CLK_DIV_NUM_V  0x0000000FU
#define PCR_PCNT_CLK_DIV_NUM_S  7

/** PCR_ETM_CONF_REG register
 *  ETM configuration register
 */
#define PCR_ETM_CONF_REG (DR_REG_PCR_BASE + 0xa0)
/** PCR_ETM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable etm clock
 */
#define PCR_ETM_CLK_EN    (BIT(0))
#define PCR_ETM_CLK_EN_M  (PCR_ETM_CLK_EN_V << PCR_ETM_CLK_EN_S)
#define PCR_ETM_CLK_EN_V  0x00000001U
#define PCR_ETM_CLK_EN_S  0
/** PCR_ETM_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset etm module
 */
#define PCR_ETM_RST_EN    (BIT(1))
#define PCR_ETM_RST_EN_M  (PCR_ETM_RST_EN_V << PCR_ETM_RST_EN_S)
#define PCR_ETM_RST_EN_V  0x00000001U
#define PCR_ETM_RST_EN_S  1
/** PCR_ETM_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset etm module
 */
#define PCR_ETM_READY    (BIT(2))
#define PCR_ETM_READY_M  (PCR_ETM_READY_V << PCR_ETM_READY_S)
#define PCR_ETM_READY_V  0x00000001U
#define PCR_ETM_READY_S  2

/** PCR_PWM0_CONF_REG register
 *  PWM0 configuration register
 */
#define PCR_PWM0_CONF_REG (DR_REG_PCR_BASE + 0xa4)
/** PCR_PWM0_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable pwm0 clock
 */
#define PCR_PWM0_CLK_EN    (BIT(0))
#define PCR_PWM0_CLK_EN_M  (PCR_PWM0_CLK_EN_V << PCR_PWM0_CLK_EN_S)
#define PCR_PWM0_CLK_EN_V  0x00000001U
#define PCR_PWM0_CLK_EN_S  0
/** PCR_PWM0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset pwm0 module
 */
#define PCR_PWM0_RST_EN    (BIT(1))
#define PCR_PWM0_RST_EN_M  (PCR_PWM0_RST_EN_V << PCR_PWM0_RST_EN_S)
#define PCR_PWM0_RST_EN_V  0x00000001U
#define PCR_PWM0_RST_EN_S  1
/** PCR_PWM0_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset pwm0 module
 */
#define PCR_PWM0_READY    (BIT(2))
#define PCR_PWM0_READY_M  (PCR_PWM0_READY_V << PCR_PWM0_READY_S)
#define PCR_PWM0_READY_V  0x00000001U
#define PCR_PWM0_READY_S  2

/** PCR_PWM0_CLK_CONF_REG register
 *  PWM0_CLK configuration register
 */
#define PCR_PWM0_CLK_CONF_REG (DR_REG_PCR_BASE + 0xa8)
/** PCR_PWM0_DIV_NUM : R/W; bitpos: [19:12]; default: 4;
 *  The integral part of the frequency divider factor of the pwm0 function clock.
 */
#define PCR_PWM0_DIV_NUM    0x000000FFU
#define PCR_PWM0_DIV_NUM_M  (PCR_PWM0_DIV_NUM_V << PCR_PWM0_DIV_NUM_S)
#define PCR_PWM0_DIV_NUM_V  0x000000FFU
#define PCR_PWM0_DIV_NUM_S  12
/** PCR_PWM0_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of MCPWM0.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F160M_CLK
 */
#define PCR_PWM0_CLKM_SEL    0x00000003U
#define PCR_PWM0_CLKM_SEL_M  (PCR_PWM0_CLKM_SEL_V << PCR_PWM0_CLKM_SEL_S)
#define PCR_PWM0_CLKM_SEL_V  0x00000003U
#define PCR_PWM0_CLKM_SEL_S  20
/** PCR_PWM0_CLKM_EN : R/W; bitpos: [22]; default: 0;
 *  set this field as 1 to activate pwm0 clkm.
 */
#define PCR_PWM0_CLKM_EN    (BIT(22))
#define PCR_PWM0_CLKM_EN_M  (PCR_PWM0_CLKM_EN_V << PCR_PWM0_CLKM_EN_S)
#define PCR_PWM0_CLKM_EN_V  0x00000001U
#define PCR_PWM0_CLKM_EN_S  22

/** PCR_PARL_IO_CONF_REG register
 *  PARL_IO configuration register
 */
#define PCR_PARL_IO_CONF_REG (DR_REG_PCR_BASE + 0xac)
/** PCR_PARL_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable parl apb clock
 */
#define PCR_PARL_CLK_EN    (BIT(0))
#define PCR_PARL_CLK_EN_M  (PCR_PARL_CLK_EN_V << PCR_PARL_CLK_EN_S)
#define PCR_PARL_CLK_EN_V  0x00000001U
#define PCR_PARL_CLK_EN_S  0
/** PCR_PARL_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset parl apb reg
 */
#define PCR_PARL_RST_EN    (BIT(1))
#define PCR_PARL_RST_EN_M  (PCR_PARL_RST_EN_V << PCR_PARL_RST_EN_S)
#define PCR_PARL_RST_EN_V  0x00000001U
#define PCR_PARL_RST_EN_S  1
/** PCR_PARL_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset parl module
 */
#define PCR_PARL_READY    (BIT(2))
#define PCR_PARL_READY_M  (PCR_PARL_READY_V << PCR_PARL_READY_S)
#define PCR_PARL_READY_V  0x00000001U
#define PCR_PARL_READY_S  2

/** PCR_PARL_CLK_RX_CONF_REG register
 *  PARL_CLK_RX configuration register
 */
#define PCR_PARL_CLK_RX_CONF_REG (DR_REG_PCR_BASE + 0xb0)
/** PCR_PARL_CLK_RX_DIV_NUM : R/W; bitpos: [15:0]; default: 0;
 *  The integral part of the frequency divider factor of the parl rx clock.
 */
#define PCR_PARL_CLK_RX_DIV_NUM    0x0000FFFFU
#define PCR_PARL_CLK_RX_DIV_NUM_M  (PCR_PARL_CLK_RX_DIV_NUM_V << PCR_PARL_CLK_RX_DIV_NUM_S)
#define PCR_PARL_CLK_RX_DIV_NUM_V  0x0000FFFFU
#define PCR_PARL_CLK_RX_DIV_NUM_S  0
/** PCR_PARL_CLK_RX_SEL : R/W; bitpos: [17:16]; default: 0;
 *  Configures the clock source of Paraller IO RX
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F240M_CLK
 *  3: Use the clock from chip pin
 */
#define PCR_PARL_CLK_RX_SEL    0x00000003U
#define PCR_PARL_CLK_RX_SEL_M  (PCR_PARL_CLK_RX_SEL_V << PCR_PARL_CLK_RX_SEL_S)
#define PCR_PARL_CLK_RX_SEL_V  0x00000003U
#define PCR_PARL_CLK_RX_SEL_S  16
/** PCR_PARL_CLK_RX_EN : R/W; bitpos: [18]; default: 0;
 *  Set 1 to enable parl rx clock
 */
#define PCR_PARL_CLK_RX_EN    (BIT(18))
#define PCR_PARL_CLK_RX_EN_M  (PCR_PARL_CLK_RX_EN_V << PCR_PARL_CLK_RX_EN_S)
#define PCR_PARL_CLK_RX_EN_V  0x00000001U
#define PCR_PARL_CLK_RX_EN_S  18
/** PCR_PARL_RX_RST_EN : R/W; bitpos: [19]; default: 0;
 *  Set 1 to reset parl rx module
 */
#define PCR_PARL_RX_RST_EN    (BIT(19))
#define PCR_PARL_RX_RST_EN_M  (PCR_PARL_RX_RST_EN_V << PCR_PARL_RX_RST_EN_S)
#define PCR_PARL_RX_RST_EN_V  0x00000001U
#define PCR_PARL_RX_RST_EN_S  19

/** PCR_PARL_CLK_TX_CONF_REG register
 *  PARL_CLK_TX configuration register
 */
#define PCR_PARL_CLK_TX_CONF_REG (DR_REG_PCR_BASE + 0xb4)
/** PCR_PARL_CLK_TX_DIV_NUM : R/W; bitpos: [15:0]; default: 0;
 *  The integral part of the frequency divider factor of the parl tx clock.
 */
#define PCR_PARL_CLK_TX_DIV_NUM    0x0000FFFFU
#define PCR_PARL_CLK_TX_DIV_NUM_M  (PCR_PARL_CLK_TX_DIV_NUM_V << PCR_PARL_CLK_TX_DIV_NUM_S)
#define PCR_PARL_CLK_TX_DIV_NUM_V  0x0000FFFFU
#define PCR_PARL_CLK_TX_DIV_NUM_S  0
/** PCR_PARL_CLK_TX_SEL : R/W; bitpos: [17:16]; default: 0;
 *  Configures the clock source of Paraller IO RX
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F240M_CLK
 *  3: Use the clock from chip pin
 */
#define PCR_PARL_CLK_TX_SEL    0x00000003U
#define PCR_PARL_CLK_TX_SEL_M  (PCR_PARL_CLK_TX_SEL_V << PCR_PARL_CLK_TX_SEL_S)
#define PCR_PARL_CLK_TX_SEL_V  0x00000003U
#define PCR_PARL_CLK_TX_SEL_S  16
/** PCR_PARL_CLK_TX_EN : R/W; bitpos: [18]; default: 0;
 *  Set 1 to enable parl tx clock
 */
#define PCR_PARL_CLK_TX_EN    (BIT(18))
#define PCR_PARL_CLK_TX_EN_M  (PCR_PARL_CLK_TX_EN_V << PCR_PARL_CLK_TX_EN_S)
#define PCR_PARL_CLK_TX_EN_V  0x00000001U
#define PCR_PARL_CLK_TX_EN_S  18
/** PCR_PARL_TX_RST_EN : R/W; bitpos: [19]; default: 0;
 *  Set 1 to reset parl tx module
 */
#define PCR_PARL_TX_RST_EN    (BIT(19))
#define PCR_PARL_TX_RST_EN_M  (PCR_PARL_TX_RST_EN_V << PCR_PARL_TX_RST_EN_S)
#define PCR_PARL_TX_RST_EN_V  0x00000001U
#define PCR_PARL_TX_RST_EN_S  19

/** PCR_PVT_MONITOR_CONF_REG register
 *  PVT_MONITOR configuration register
 */
#define PCR_PVT_MONITOR_CONF_REG (DR_REG_PCR_BASE + 0xb8)
/** PCR_PVT_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable apb clock of pvt module
 */
#define PCR_PVT_MONITOR_CLK_EN    (BIT(0))
#define PCR_PVT_MONITOR_CLK_EN_M  (PCR_PVT_MONITOR_CLK_EN_V << PCR_PVT_MONITOR_CLK_EN_S)
#define PCR_PVT_MONITOR_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_CLK_EN_S  0
/** PCR_PVT_MONITOR_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset all pvt monitor module
 */
#define PCR_PVT_MONITOR_RST_EN    (BIT(1))
#define PCR_PVT_MONITOR_RST_EN_M  (PCR_PVT_MONITOR_RST_EN_V << PCR_PVT_MONITOR_RST_EN_S)
#define PCR_PVT_MONITOR_RST_EN_V  0x00000001U
#define PCR_PVT_MONITOR_RST_EN_S  1
/** PCR_PVT_MONITOR_SITE1_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable function clock of modem pvt module
 */
#define PCR_PVT_MONITOR_SITE1_CLK_EN    (BIT(2))
#define PCR_PVT_MONITOR_SITE1_CLK_EN_M  (PCR_PVT_MONITOR_SITE1_CLK_EN_V << PCR_PVT_MONITOR_SITE1_CLK_EN_S)
#define PCR_PVT_MONITOR_SITE1_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_SITE1_CLK_EN_S  2
/** PCR_PVT_MONITOR_SITE2_CLK_EN : R/W; bitpos: [3]; default: 1;
 *  Set 1 to enable function clock of cpu pvt module
 */
#define PCR_PVT_MONITOR_SITE2_CLK_EN    (BIT(3))
#define PCR_PVT_MONITOR_SITE2_CLK_EN_M  (PCR_PVT_MONITOR_SITE2_CLK_EN_V << PCR_PVT_MONITOR_SITE2_CLK_EN_S)
#define PCR_PVT_MONITOR_SITE2_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_SITE2_CLK_EN_S  3
/** PCR_PVT_MONITOR_SITE3_CLK_EN : R/W; bitpos: [4]; default: 1;
 *  Set 1 to enable function clock of hp_peri pvt module
 */
#define PCR_PVT_MONITOR_SITE3_CLK_EN    (BIT(4))
#define PCR_PVT_MONITOR_SITE3_CLK_EN_M  (PCR_PVT_MONITOR_SITE3_CLK_EN_V << PCR_PVT_MONITOR_SITE3_CLK_EN_S)
#define PCR_PVT_MONITOR_SITE3_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_SITE3_CLK_EN_S  4

/** PCR_PVT_MONITOR_FUNC_CLK_CONF_REG register
 *  PVT_MONITOR function clock configuration register
 */
#define PCR_PVT_MONITOR_FUNC_CLK_CONF_REG (DR_REG_PCR_BASE + 0xbc)
/** PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM : R/W; bitpos: [3:0]; default: 0;
 *  The integral part of the frequency divider factor of the pvt_monitor function clock.
 */
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM    0x0000000FU
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_M  (PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_V << PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S)
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_V  0x0000000FU
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S  0
/** PCR_PVT_MONITOR_FUNC_CLK_SEL : R/W; bitpos: [20]; default: 1;
 *  Configures the clock source of PVT MONITOR.
 *  0: XTAL_CLK
 *  1(default): PLL_F160M_CLK
 */
#define PCR_PVT_MONITOR_FUNC_CLK_SEL    (BIT(20))
#define PCR_PVT_MONITOR_FUNC_CLK_SEL_M  (PCR_PVT_MONITOR_FUNC_CLK_SEL_V << PCR_PVT_MONITOR_FUNC_CLK_SEL_S)
#define PCR_PVT_MONITOR_FUNC_CLK_SEL_V  0x00000001U
#define PCR_PVT_MONITOR_FUNC_CLK_SEL_S  20
/** PCR_PVT_MONITOR_FUNC_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable source clock of pvt sitex
 */
#define PCR_PVT_MONITOR_FUNC_CLK_EN    (BIT(22))
#define PCR_PVT_MONITOR_FUNC_CLK_EN_M  (PCR_PVT_MONITOR_FUNC_CLK_EN_V << PCR_PVT_MONITOR_FUNC_CLK_EN_S)
#define PCR_PVT_MONITOR_FUNC_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_FUNC_CLK_EN_S  22

/** PCR_GDMA_CONF_REG register
 *  GDMA configuration register
 */
#define PCR_GDMA_CONF_REG (DR_REG_PCR_BASE + 0xc0)
/** PCR_GDMA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable gdma clock
 */
#define PCR_GDMA_CLK_EN    (BIT(0))
#define PCR_GDMA_CLK_EN_M  (PCR_GDMA_CLK_EN_V << PCR_GDMA_CLK_EN_S)
#define PCR_GDMA_CLK_EN_V  0x00000001U
#define PCR_GDMA_CLK_EN_S  0
/** PCR_GDMA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset gdma module
 */
#define PCR_GDMA_RST_EN    (BIT(1))
#define PCR_GDMA_RST_EN_M  (PCR_GDMA_RST_EN_V << PCR_GDMA_RST_EN_S)
#define PCR_GDMA_RST_EN_V  0x00000001U
#define PCR_GDMA_RST_EN_S  1

/** PCR_SPI2_CONF_REG register
 *  SPI2 configuration register
 */
#define PCR_SPI2_CONF_REG (DR_REG_PCR_BASE + 0xc4)
/** PCR_SPI2_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable spi2 apb clock
 */
#define PCR_SPI2_CLK_EN    (BIT(0))
#define PCR_SPI2_CLK_EN_M  (PCR_SPI2_CLK_EN_V << PCR_SPI2_CLK_EN_S)
#define PCR_SPI2_CLK_EN_V  0x00000001U
#define PCR_SPI2_CLK_EN_S  0
/** PCR_SPI2_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset spi2 module
 */
#define PCR_SPI2_RST_EN    (BIT(1))
#define PCR_SPI2_RST_EN_M  (PCR_SPI2_RST_EN_V << PCR_SPI2_RST_EN_S)
#define PCR_SPI2_RST_EN_V  0x00000001U
#define PCR_SPI2_RST_EN_S  1
/** PCR_SPI2_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset spi2 module
 */
#define PCR_SPI2_READY    (BIT(2))
#define PCR_SPI2_READY_M  (PCR_SPI2_READY_V << PCR_SPI2_READY_S)
#define PCR_SPI2_READY_V  0x00000001U
#define PCR_SPI2_READY_S  2

/** PCR_SPI2_CLKM_CONF_REG register
 *  SPI2_CLKM configuration register
 */
#define PCR_SPI2_CLKM_CONF_REG (DR_REG_PCR_BASE + 0xc8)
/** PCR_SPI2_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the spi2_mst clock.
 */
#define PCR_SPI2_CLKM_DIV_NUM    0x000000FFU
#define PCR_SPI2_CLKM_DIV_NUM_M  (PCR_SPI2_CLKM_DIV_NUM_V << PCR_SPI2_CLKM_DIV_NUM_S)
#define PCR_SPI2_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_SPI2_CLKM_DIV_NUM_S  12
/** PCR_SPI2_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of SPI2.
 *  0 (default): XTAL_CLK
 *  1: PLL_F160M_CLK
 *  2: RC_FAST_CLK
 *  3: PLL_F120M_CLK
 */
#define PCR_SPI2_CLKM_SEL    0x00000003U
#define PCR_SPI2_CLKM_SEL_M  (PCR_SPI2_CLKM_SEL_V << PCR_SPI2_CLKM_SEL_S)
#define PCR_SPI2_CLKM_SEL_V  0x00000003U
#define PCR_SPI2_CLKM_SEL_S  20
/** PCR_SPI2_CLKM_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable spi2 function clock
 */
#define PCR_SPI2_CLKM_EN    (BIT(22))
#define PCR_SPI2_CLKM_EN_M  (PCR_SPI2_CLKM_EN_V << PCR_SPI2_CLKM_EN_S)
#define PCR_SPI2_CLKM_EN_V  0x00000001U
#define PCR_SPI2_CLKM_EN_S  22

/** PCR_AES_CONF_REG register
 *  AES configuration register
 */
#define PCR_AES_CONF_REG (DR_REG_PCR_BASE + 0xcc)
/** PCR_AES_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable aes clock
 */
#define PCR_AES_CLK_EN    (BIT(0))
#define PCR_AES_CLK_EN_M  (PCR_AES_CLK_EN_V << PCR_AES_CLK_EN_S)
#define PCR_AES_CLK_EN_V  0x00000001U
#define PCR_AES_CLK_EN_S  0
/** PCR_AES_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset aes module
 */
#define PCR_AES_RST_EN    (BIT(1))
#define PCR_AES_RST_EN_M  (PCR_AES_RST_EN_V << PCR_AES_RST_EN_S)
#define PCR_AES_RST_EN_V  0x00000001U
#define PCR_AES_RST_EN_S  1
/** PCR_AES_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset aes module
 */
#define PCR_AES_READY    (BIT(2))
#define PCR_AES_READY_M  (PCR_AES_READY_V << PCR_AES_READY_S)
#define PCR_AES_READY_V  0x00000001U
#define PCR_AES_READY_S  2

/** PCR_SHA_CONF_REG register
 *  SHA configuration register
 */
#define PCR_SHA_CONF_REG (DR_REG_PCR_BASE + 0xd0)
/** PCR_SHA_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable sha clock
 */
#define PCR_SHA_CLK_EN    (BIT(0))
#define PCR_SHA_CLK_EN_M  (PCR_SHA_CLK_EN_V << PCR_SHA_CLK_EN_S)
#define PCR_SHA_CLK_EN_V  0x00000001U
#define PCR_SHA_CLK_EN_S  0
/** PCR_SHA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset sha module
 */
#define PCR_SHA_RST_EN    (BIT(1))
#define PCR_SHA_RST_EN_M  (PCR_SHA_RST_EN_V << PCR_SHA_RST_EN_S)
#define PCR_SHA_RST_EN_V  0x00000001U
#define PCR_SHA_RST_EN_S  1
/** PCR_SHA_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset sha module
 */
#define PCR_SHA_READY    (BIT(2))
#define PCR_SHA_READY_M  (PCR_SHA_READY_V << PCR_SHA_READY_S)
#define PCR_SHA_READY_V  0x00000001U
#define PCR_SHA_READY_S  2

/** PCR_ECC_CONF_REG register
 *  ECC configuration register
 */
#define PCR_ECC_CONF_REG (DR_REG_PCR_BASE + 0xdc)
/** PCR_ECC_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable ecc clock
 */
#define PCR_ECC_CLK_EN    (BIT(0))
#define PCR_ECC_CLK_EN_M  (PCR_ECC_CLK_EN_V << PCR_ECC_CLK_EN_S)
#define PCR_ECC_CLK_EN_V  0x00000001U
#define PCR_ECC_CLK_EN_S  0
/** PCR_ECC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset ecc module
 */
#define PCR_ECC_RST_EN    (BIT(1))
#define PCR_ECC_RST_EN_M  (PCR_ECC_RST_EN_V << PCR_ECC_RST_EN_S)
#define PCR_ECC_RST_EN_V  0x00000001U
#define PCR_ECC_RST_EN_S  1
/** PCR_ECC_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset ecc module
 */
#define PCR_ECC_READY    (BIT(2))
#define PCR_ECC_READY_M  (PCR_ECC_READY_V << PCR_ECC_READY_S)
#define PCR_ECC_READY_V  0x00000001U
#define PCR_ECC_READY_S  2

/** PCR_ECC_MEM_LP_CTRL_REG register
 *  HP CORE0 & HP CORE1 memory power control register
 */
#define PCR_ECC_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0xe0)
/** PCR_ECC_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 2;
 *  Configures ecc memory low power mode in low power stage.
 *  0: deep sleep
 *  1: light sleep
 *  2(default): shut down
 *  3: disable low power stage
 */
#define PCR_ECC_MEM_LP_MODE    0x00000003U
#define PCR_ECC_MEM_LP_MODE_M  (PCR_ECC_MEM_LP_MODE_V << PCR_ECC_MEM_LP_MODE_S)
#define PCR_ECC_MEM_LP_MODE_V  0x00000003U
#define PCR_ECC_MEM_LP_MODE_S  0
/** PCR_ECC_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down ecc memory.
 */
#define PCR_ECC_MEM_LP_EN    (BIT(2))
#define PCR_ECC_MEM_LP_EN_M  (PCR_ECC_MEM_LP_EN_V << PCR_ECC_MEM_LP_EN_S)
#define PCR_ECC_MEM_LP_EN_V  0x00000001U
#define PCR_ECC_MEM_LP_EN_S  2
/** PCR_ECC_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control ecc memory, disable hardware control.
 */
#define PCR_ECC_MEM_FORCE_CTRL    (BIT(3))
#define PCR_ECC_MEM_FORCE_CTRL_M  (PCR_ECC_MEM_FORCE_CTRL_V << PCR_ECC_MEM_FORCE_CTRL_S)
#define PCR_ECC_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_ECC_MEM_FORCE_CTRL_S  3

/** PCR_HMAC_CONF_REG register
 *  HMAC configuration register
 */
#define PCR_HMAC_CONF_REG (DR_REG_PCR_BASE + 0xe8)
/** PCR_HMAC_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable hmac clock
 */
#define PCR_HMAC_CLK_EN    (BIT(0))
#define PCR_HMAC_CLK_EN_M  (PCR_HMAC_CLK_EN_V << PCR_HMAC_CLK_EN_S)
#define PCR_HMAC_CLK_EN_V  0x00000001U
#define PCR_HMAC_CLK_EN_S  0
/** PCR_HMAC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset hmac module
 */
#define PCR_HMAC_RST_EN    (BIT(1))
#define PCR_HMAC_RST_EN_M  (PCR_HMAC_RST_EN_V << PCR_HMAC_RST_EN_S)
#define PCR_HMAC_RST_EN_V  0x00000001U
#define PCR_HMAC_RST_EN_S  1
/** PCR_HMAC_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset hmac module
 */
#define PCR_HMAC_READY    (BIT(2))
#define PCR_HMAC_READY_M  (PCR_HMAC_READY_V << PCR_HMAC_READY_S)
#define PCR_HMAC_READY_V  0x00000001U
#define PCR_HMAC_READY_S  2

/** PCR_ECDSA_CONF_REG register
 *  ECDSA configuration register
 */
#define PCR_ECDSA_CONF_REG (DR_REG_PCR_BASE + 0xec)
/** PCR_ECDSA_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable ecdsa clock
 */
#define PCR_ECDSA_CLK_EN    (BIT(0))
#define PCR_ECDSA_CLK_EN_M  (PCR_ECDSA_CLK_EN_V << PCR_ECDSA_CLK_EN_S)
#define PCR_ECDSA_CLK_EN_V  0x00000001U
#define PCR_ECDSA_CLK_EN_S  0
/** PCR_ECDSA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset ecdsa module
 */
#define PCR_ECDSA_RST_EN    (BIT(1))
#define PCR_ECDSA_RST_EN_M  (PCR_ECDSA_RST_EN_V << PCR_ECDSA_RST_EN_S)
#define PCR_ECDSA_RST_EN_V  0x00000001U
#define PCR_ECDSA_RST_EN_S  1
/** PCR_ECDSA_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset ecdsa module
 */
#define PCR_ECDSA_READY    (BIT(2))
#define PCR_ECDSA_READY_M  (PCR_ECDSA_READY_V << PCR_ECDSA_READY_S)
#define PCR_ECDSA_READY_V  0x00000001U
#define PCR_ECDSA_READY_S  2

/** PCR_IOMUX_CONF_REG register
 *  IOMUX configuration register
 */
#define PCR_IOMUX_CONF_REG (DR_REG_PCR_BASE + 0xf0)
/** PCR_IOMUX_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable iomux apb clock
 */
#define PCR_IOMUX_CLK_EN    (BIT(0))
#define PCR_IOMUX_CLK_EN_M  (PCR_IOMUX_CLK_EN_V << PCR_IOMUX_CLK_EN_S)
#define PCR_IOMUX_CLK_EN_V  0x00000001U
#define PCR_IOMUX_CLK_EN_S  0
/** PCR_IOMUX_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset iomux module
 */
#define PCR_IOMUX_RST_EN    (BIT(1))
#define PCR_IOMUX_RST_EN_M  (PCR_IOMUX_RST_EN_V << PCR_IOMUX_RST_EN_S)
#define PCR_IOMUX_RST_EN_V  0x00000001U
#define PCR_IOMUX_RST_EN_S  1

/** PCR_IOMUX_CLK_CONF_REG register
 *  IOMUX_CLK configuration register
 */
#define PCR_IOMUX_CLK_CONF_REG (DR_REG_PCR_BASE + 0xf4)
/** PCR_IOMUX_FUNC_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of IO MUX.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_IOMUX_FUNC_CLK_SEL    0x00000003U
#define PCR_IOMUX_FUNC_CLK_SEL_M  (PCR_IOMUX_FUNC_CLK_SEL_V << PCR_IOMUX_FUNC_CLK_SEL_S)
#define PCR_IOMUX_FUNC_CLK_SEL_V  0x00000003U
#define PCR_IOMUX_FUNC_CLK_SEL_S  20
/** PCR_IOMUX_FUNC_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable iomux function clock
 */
#define PCR_IOMUX_FUNC_CLK_EN    (BIT(22))
#define PCR_IOMUX_FUNC_CLK_EN_M  (PCR_IOMUX_FUNC_CLK_EN_V << PCR_IOMUX_FUNC_CLK_EN_S)
#define PCR_IOMUX_FUNC_CLK_EN_V  0x00000001U
#define PCR_IOMUX_FUNC_CLK_EN_S  22

/** PCR_REGDMA_CONF_REG register
 *  REGDMA configuration register
 */
#define PCR_REGDMA_CONF_REG (DR_REG_PCR_BASE + 0xf8)
/** PCR_REGDMA_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable regdma clock
 */
#define PCR_REGDMA_CLK_EN    (BIT(0))
#define PCR_REGDMA_CLK_EN_M  (PCR_REGDMA_CLK_EN_V << PCR_REGDMA_CLK_EN_S)
#define PCR_REGDMA_CLK_EN_V  0x00000001U
#define PCR_REGDMA_CLK_EN_S  0
/** PCR_REGDMA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset regdma module
 */
#define PCR_REGDMA_RST_EN    (BIT(1))
#define PCR_REGDMA_RST_EN_M  (PCR_REGDMA_RST_EN_V << PCR_REGDMA_RST_EN_S)
#define PCR_REGDMA_RST_EN_V  0x00000001U
#define PCR_REGDMA_RST_EN_S  1
/** PCR_REGDMA_MAC_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable modem control regdma clock relate logic clock
 */
#define PCR_REGDMA_MAC_CLK_EN    (BIT(2))
#define PCR_REGDMA_MAC_CLK_EN_M  (PCR_REGDMA_MAC_CLK_EN_V << PCR_REGDMA_MAC_CLK_EN_S)
#define PCR_REGDMA_MAC_CLK_EN_V  0x00000001U
#define PCR_REGDMA_MAC_CLK_EN_S  2

/** PCR_TRACE_CONF_REG register
 *  TRACE configuration register
 */
#define PCR_TRACE_CONF_REG (DR_REG_PCR_BASE + 0xfc)
/** PCR_TRACE_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable core0 trace clock
 */
#define PCR_TRACE_CLK_EN    (BIT(0))
#define PCR_TRACE_CLK_EN_M  (PCR_TRACE_CLK_EN_V << PCR_TRACE_CLK_EN_S)
#define PCR_TRACE_CLK_EN_V  0x00000001U
#define PCR_TRACE_CLK_EN_S  0
/** PCR_TRACE_RST_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to reset core0 trace module
 */
#define PCR_TRACE_RST_EN    (BIT(1))
#define PCR_TRACE_RST_EN_M  (PCR_TRACE_RST_EN_V << PCR_TRACE_RST_EN_S)
#define PCR_TRACE_RST_EN_V  0x00000001U
#define PCR_TRACE_RST_EN_S  1
/** PCR_TRACE1_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to enable core1 trace clock
 */
#define PCR_TRACE1_CLK_EN    (BIT(2))
#define PCR_TRACE1_CLK_EN_M  (PCR_TRACE1_CLK_EN_V << PCR_TRACE1_CLK_EN_S)
#define PCR_TRACE1_CLK_EN_V  0x00000001U
#define PCR_TRACE1_CLK_EN_S  2
/** PCR_TRACE1_RST_EN : R/W; bitpos: [3]; default: 1;
 *  Set 1 to reset core1 trace module
 */
#define PCR_TRACE1_RST_EN    (BIT(3))
#define PCR_TRACE1_RST_EN_M  (PCR_TRACE1_RST_EN_V << PCR_TRACE1_RST_EN_S)
#define PCR_TRACE1_RST_EN_V  0x00000001U
#define PCR_TRACE1_RST_EN_S  3

/** PCR_ASSIST_CONF_REG register
 *  ASSIST configuration register
 */
#define PCR_ASSIST_CONF_REG (DR_REG_PCR_BASE + 0x100)
/** PCR_ASSIST_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable core0 assist clock
 */
#define PCR_ASSIST_CLK_EN    (BIT(0))
#define PCR_ASSIST_CLK_EN_M  (PCR_ASSIST_CLK_EN_V << PCR_ASSIST_CLK_EN_S)
#define PCR_ASSIST_CLK_EN_V  0x00000001U
#define PCR_ASSIST_CLK_EN_S  0
/** PCR_ASSIST_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset core0 assist module
 */
#define PCR_ASSIST_RST_EN    (BIT(1))
#define PCR_ASSIST_RST_EN_M  (PCR_ASSIST_RST_EN_V << PCR_ASSIST_RST_EN_S)
#define PCR_ASSIST_RST_EN_V  0x00000001U
#define PCR_ASSIST_RST_EN_S  1
/** PCR_ASSIST1_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to enable core1 assist clock
 */
#define PCR_ASSIST1_CLK_EN    (BIT(2))
#define PCR_ASSIST1_CLK_EN_M  (PCR_ASSIST1_CLK_EN_V << PCR_ASSIST1_CLK_EN_S)
#define PCR_ASSIST1_CLK_EN_V  0x00000001U
#define PCR_ASSIST1_CLK_EN_S  2
/** PCR_ASSIST1_RST_EN : R/W; bitpos: [3]; default: 1;
 *  Set 1 to reset core1 assist module
 */
#define PCR_ASSIST1_RST_EN    (BIT(3))
#define PCR_ASSIST1_RST_EN_M  (PCR_ASSIST1_RST_EN_V << PCR_ASSIST1_RST_EN_S)
#define PCR_ASSIST1_RST_EN_V  0x00000001U
#define PCR_ASSIST1_RST_EN_S  3

/** PCR_CACHE_CONF_REG register
 *  CACHE configuration register
 */
#define PCR_CACHE_CONF_REG (DR_REG_PCR_BASE + 0x104)
/** PCR_ICACHE0_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable icache0 clock
 */
#define PCR_ICACHE0_CLK_EN    (BIT(0))
#define PCR_ICACHE0_CLK_EN_M  (PCR_ICACHE0_CLK_EN_V << PCR_ICACHE0_CLK_EN_S)
#define PCR_ICACHE0_CLK_EN_V  0x00000001U
#define PCR_ICACHE0_CLK_EN_S  0
/** PCR_ICACHE1_CLK_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to enable icache1 clock
 */
#define PCR_ICACHE1_CLK_EN    (BIT(1))
#define PCR_ICACHE1_CLK_EN_M  (PCR_ICACHE1_CLK_EN_V << PCR_ICACHE1_CLK_EN_S)
#define PCR_ICACHE1_CLK_EN_V  0x00000001U
#define PCR_ICACHE1_CLK_EN_S  1
/** PCR_DCACHE_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable dcache clock
 */
#define PCR_DCACHE_CLK_EN    (BIT(2))
#define PCR_DCACHE_CLK_EN_M  (PCR_DCACHE_CLK_EN_V << PCR_DCACHE_CLK_EN_S)
#define PCR_DCACHE_CLK_EN_V  0x00000001U
#define PCR_DCACHE_CLK_EN_S  2
/** PCR_ICACHE0_RST_EN : R/W; bitpos: [3]; default: 0;
 *  Set 1 to reset icache0 module
 */
#define PCR_ICACHE0_RST_EN    (BIT(3))
#define PCR_ICACHE0_RST_EN_M  (PCR_ICACHE0_RST_EN_V << PCR_ICACHE0_RST_EN_S)
#define PCR_ICACHE0_RST_EN_V  0x00000001U
#define PCR_ICACHE0_RST_EN_S  3
/** PCR_ICACHE1_RST_EN : R/W; bitpos: [4]; default: 0;
 *  Set 1 to reset icache1 module
 */
#define PCR_ICACHE1_RST_EN    (BIT(4))
#define PCR_ICACHE1_RST_EN_M  (PCR_ICACHE1_RST_EN_V << PCR_ICACHE1_RST_EN_S)
#define PCR_ICACHE1_RST_EN_V  0x00000001U
#define PCR_ICACHE1_RST_EN_S  4
/** PCR_DCACHE_RST_EN : R/W; bitpos: [5]; default: 0;
 *  Set 1 to reset dcache module
 */
#define PCR_DCACHE_RST_EN    (BIT(5))
#define PCR_DCACHE_RST_EN_M  (PCR_DCACHE_RST_EN_V << PCR_DCACHE_RST_EN_S)
#define PCR_DCACHE_RST_EN_V  0x00000001U
#define PCR_DCACHE_RST_EN_S  5
/** PCR_CACHE_RST_EN : R/W; bitpos: [8]; default: 0;
 *  Set 1 to reset total cache module
 */
#define PCR_CACHE_RST_EN    (BIT(8))
#define PCR_CACHE_RST_EN_M  (PCR_CACHE_RST_EN_V << PCR_CACHE_RST_EN_S)
#define PCR_CACHE_RST_EN_V  0x00000001U
#define PCR_CACHE_RST_EN_S  8

/** PCR_CACHE_MEM_LP_CTRL_REG register
 *  HP CORE0 & HP CORE1 memory power control register
 */
#define PCR_CACHE_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x108)
/** PCR_L1_CACHE_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures l1_cache memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_L1_CACHE_MEM_LP_MODE    0x00000003U
#define PCR_L1_CACHE_MEM_LP_MODE_M  (PCR_L1_CACHE_MEM_LP_MODE_V << PCR_L1_CACHE_MEM_LP_MODE_S)
#define PCR_L1_CACHE_MEM_LP_MODE_V  0x00000003U
#define PCR_L1_CACHE_MEM_LP_MODE_S  0
/** PCR_L1_CACHE_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down l1_cache memory.
 */
#define PCR_L1_CACHE_MEM_LP_EN    (BIT(2))
#define PCR_L1_CACHE_MEM_LP_EN_M  (PCR_L1_CACHE_MEM_LP_EN_V << PCR_L1_CACHE_MEM_LP_EN_S)
#define PCR_L1_CACHE_MEM_LP_EN_V  0x00000001U
#define PCR_L1_CACHE_MEM_LP_EN_S  2
/** PCR_L1_CACHE_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control l1_cache memory, disable hardware control.
 */
#define PCR_L1_CACHE_MEM_FORCE_CTRL    (BIT(3))
#define PCR_L1_CACHE_MEM_FORCE_CTRL_M  (PCR_L1_CACHE_MEM_FORCE_CTRL_V << PCR_L1_CACHE_MEM_FORCE_CTRL_S)
#define PCR_L1_CACHE_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_L1_CACHE_MEM_FORCE_CTRL_S  3

/** PCR_MODEM_CONF_REG register
 *  MODEM_APB configuration register
 */
#define PCR_MODEM_CONF_REG (DR_REG_PCR_BASE + 0x10c)
/** PCR_MODEM_CLK_SEL : R/W; bitpos: [0]; default: 0;
 *  xxxx
 */
#define PCR_MODEM_CLK_SEL    (BIT(0))
#define PCR_MODEM_CLK_SEL_M  (PCR_MODEM_CLK_SEL_V << PCR_MODEM_CLK_SEL_S)
#define PCR_MODEM_CLK_SEL_V  0x00000001U
#define PCR_MODEM_CLK_SEL_S  0
/** PCR_MODEM_CLK_EN : R/W; bitpos: [1]; default: 1;
 *  xxxx
 */
#define PCR_MODEM_CLK_EN    (BIT(1))
#define PCR_MODEM_CLK_EN_M  (PCR_MODEM_CLK_EN_V << PCR_MODEM_CLK_EN_S)
#define PCR_MODEM_CLK_EN_V  0x00000001U
#define PCR_MODEM_CLK_EN_S  1
/** PCR_MODEM_RST_EN : R/W; bitpos: [2]; default: 0;
 *  Set this file as 1 to reset modem-subsystem.
 */
#define PCR_MODEM_RST_EN    (BIT(2))
#define PCR_MODEM_RST_EN_M  (PCR_MODEM_RST_EN_V << PCR_MODEM_RST_EN_S)
#define PCR_MODEM_RST_EN_V  0x00000001U
#define PCR_MODEM_RST_EN_S  2

/** PCR_TIMEOUT_CONF_REG register
 *  TIMEOUT configuration register
 */
#define PCR_TIMEOUT_CONF_REG (DR_REG_PCR_BASE + 0x110)
/** PCR_CPU_TIMEOUT_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset cpu_peri timeout module
 */
#define PCR_CPU_TIMEOUT_RST_EN    (BIT(1))
#define PCR_CPU_TIMEOUT_RST_EN_M  (PCR_CPU_TIMEOUT_RST_EN_V << PCR_CPU_TIMEOUT_RST_EN_S)
#define PCR_CPU_TIMEOUT_RST_EN_V  0x00000001U
#define PCR_CPU_TIMEOUT_RST_EN_S  1
/** PCR_HP_TIMEOUT_RST_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to reset hp_peri timeout module and hp_modem timeout module
 */
#define PCR_HP_TIMEOUT_RST_EN    (BIT(2))
#define PCR_HP_TIMEOUT_RST_EN_M  (PCR_HP_TIMEOUT_RST_EN_V << PCR_HP_TIMEOUT_RST_EN_S)
#define PCR_HP_TIMEOUT_RST_EN_V  0x00000001U
#define PCR_HP_TIMEOUT_RST_EN_S  2

/** PCR_SYSCLK_CONF_REG register
 *  SYSCLK configuration register
 */
#define PCR_SYSCLK_CONF_REG (DR_REG_PCR_BASE + 0x114)
/** PCR_LS_DIV_NUM : HRO; bitpos: [7:0]; default: 0;
 *  clk_hproot is div1 of low-speed clock-source if clck-source is a low-speed
 *  clock-source such as XTAL/FOSC.
 */
#define PCR_LS_DIV_NUM    0x000000FFU
#define PCR_LS_DIV_NUM_M  (PCR_LS_DIV_NUM_V << PCR_LS_DIV_NUM_S)
#define PCR_LS_DIV_NUM_V  0x000000FFU
#define PCR_LS_DIV_NUM_S  0
/** PCR_HS_DIV_NUM : HRO; bitpos: [15:8]; default: 2;
 *  clk_hproot is div3 of SPLL if the clock-source is high-speed clock SPLL.
 */
#define PCR_HS_DIV_NUM    0x000000FFU
#define PCR_HS_DIV_NUM_M  (PCR_HS_DIV_NUM_V << PCR_HS_DIV_NUM_S)
#define PCR_HS_DIV_NUM_V  0x000000FFU
#define PCR_HS_DIV_NUM_S  8
/** PCR_SOC_CLK_SEL : R/W; bitpos: [17:16]; default: 0;
 *  Configures to select the clock source of HP_ROOT_CLK.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F160M_CLK
 *  2: PLL_F240M_CLK
 */
#define PCR_SOC_CLK_SEL    0x00000003U
#define PCR_SOC_CLK_SEL_M  (PCR_SOC_CLK_SEL_V << PCR_SOC_CLK_SEL_S)
#define PCR_SOC_CLK_SEL_V  0x00000003U
#define PCR_SOC_CLK_SEL_S  16
/** PCR_CLK_XTAL_FREQ : HRO; bitpos: [30:24]; default: 32;
 *  This field indicates the frequency(MHz) of XTAL.
 */
#define PCR_CLK_XTAL_FREQ    0x0000007FU
#define PCR_CLK_XTAL_FREQ_M  (PCR_CLK_XTAL_FREQ_V << PCR_CLK_XTAL_FREQ_S)
#define PCR_CLK_XTAL_FREQ_V  0x0000007FU
#define PCR_CLK_XTAL_FREQ_S  24
/** PCR_CPU_DBGMD_CLK_EN : R/W; bitpos: [31]; default: 1;
 *  This field indicates if cpu debug mode clock is enable. 0: disable, 1:
 *  enable(default).
 */
#define PCR_CPU_DBGMD_CLK_EN    (BIT(31))
#define PCR_CPU_DBGMD_CLK_EN_M  (PCR_CPU_DBGMD_CLK_EN_V << PCR_CPU_DBGMD_CLK_EN_S)
#define PCR_CPU_DBGMD_CLK_EN_V  0x00000001U
#define PCR_CPU_DBGMD_CLK_EN_S  31

/** PCR_CPU_WAITI_CONF_REG register
 *  CPU_WAITI configuration register
 */
#define PCR_CPU_WAITI_CONF_REG (DR_REG_PCR_BASE + 0x118)
/** PCR_CPU1_WAIT_MODE_FORCE_ON : R/W; bitpos: [3]; default: 1;
 *  Set 1 to force cpu1_waiti_clk enable.
 */
#define PCR_CPU1_WAIT_MODE_FORCE_ON    (BIT(3))
#define PCR_CPU1_WAIT_MODE_FORCE_ON_M  (PCR_CPU1_WAIT_MODE_FORCE_ON_V << PCR_CPU1_WAIT_MODE_FORCE_ON_S)
#define PCR_CPU1_WAIT_MODE_FORCE_ON_V  0x00000001U
#define PCR_CPU1_WAIT_MODE_FORCE_ON_S  3
/** PCR_CPU0_WAIT_MODE_FORCE_ON : R/W; bitpos: [4]; default: 1;
 *  Set 1 to force cpu0_waiti_clk enable.
 */
#define PCR_CPU0_WAIT_MODE_FORCE_ON    (BIT(4))
#define PCR_CPU0_WAIT_MODE_FORCE_ON_M  (PCR_CPU0_WAIT_MODE_FORCE_ON_V << PCR_CPU0_WAIT_MODE_FORCE_ON_S)
#define PCR_CPU0_WAIT_MODE_FORCE_ON_V  0x00000001U
#define PCR_CPU0_WAIT_MODE_FORCE_ON_S  4
/** PCR_CPU_WAITI_DELAY_NUM : R/W; bitpos: [8:5]; default: 0;
 *  This field used to set delay cycle when cpu enter waiti mode, after delay waiti_clk
 *  will close
 */
#define PCR_CPU_WAITI_DELAY_NUM    0x0000000FU
#define PCR_CPU_WAITI_DELAY_NUM_M  (PCR_CPU_WAITI_DELAY_NUM_V << PCR_CPU_WAITI_DELAY_NUM_S)
#define PCR_CPU_WAITI_DELAY_NUM_V  0x0000000FU
#define PCR_CPU_WAITI_DELAY_NUM_S  5
/** PCR_CPU_WFI_DECREASE_EN : R/W; bitpos: [9]; default: 0;
 *  Set 1 to enable cpu freq decrease to ahb freq when cpu enter waiti mode
 */
#define PCR_CPU_WFI_DECREASE_EN    (BIT(9))
#define PCR_CPU_WFI_DECREASE_EN_M  (PCR_CPU_WFI_DECREASE_EN_V << PCR_CPU_WFI_DECREASE_EN_S)
#define PCR_CPU_WFI_DECREASE_EN_V  0x00000001U
#define PCR_CPU_WFI_DECREASE_EN_S  9

/** PCR_CPU_FREQ_CONF_REG register
 *  CPU_FREQ configuration register
 */
#define PCR_CPU_FREQ_CONF_REG (DR_REG_PCR_BASE + 0x11c)
/** PCR_CPU_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  Set this field to generate clk_cpu derived by clk_hproot. The clk_cpu is
 *  div1(default)/div2/div4 of clk_hproot. This field is only available for low-speed
 *  clock-source such as XTAL/FOSC, and should be used together with PCR_AHB_DIV_NUM.
 */
#define PCR_CPU_DIV_NUM    0x000000FFU
#define PCR_CPU_DIV_NUM_M  (PCR_CPU_DIV_NUM_V << PCR_CPU_DIV_NUM_S)
#define PCR_CPU_DIV_NUM_V  0x000000FFU
#define PCR_CPU_DIV_NUM_S  0

/** PCR_AHB_FREQ_CONF_REG register
 *  AHB_FREQ configuration register
 */
#define PCR_AHB_FREQ_CONF_REG (DR_REG_PCR_BASE + 0x120)
/** PCR_AHB_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  Set this field to generate clk_ahb derived by clk_hproot. The clk_ahb is
 *  div1(default)/div2/div4/div8 of clk_hproot. This field is only available for
 *  low-speed clock-source such as XTAL/FOSC, and should be used together with
 *  PCR_CPU_DIV_NUM.
 */
#define PCR_AHB_DIV_NUM    0x000000FFU
#define PCR_AHB_DIV_NUM_M  (PCR_AHB_DIV_NUM_V << PCR_AHB_DIV_NUM_S)
#define PCR_AHB_DIV_NUM_V  0x000000FFU
#define PCR_AHB_DIV_NUM_S  0

/** PCR_APB_FREQ_CONF_REG register
 *  APB_FREQ configuration register
 */
#define PCR_APB_FREQ_CONF_REG (DR_REG_PCR_BASE + 0x124)
/** PCR_APB_DECREASE_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  If this field's value is grater than PCR_APB_DIV_NUM, the clk_apb will be
 *  automatically down to clk_apb_decrease only when no access is on apb-bus, and will
 *  recover to the previous frequency when a new access appears on apb-bus. Set as one
 *  within (0,1,3) to set clk_apb_decrease as div1/div2/div4(default) of clk_ahb. Note
 *  that enable this function will reduce performance. Users can set this field as zero
 *  to disable the auto-decrease-apb-freq function. By default, this function is
 *  disable.
 */
#define PCR_APB_DECREASE_DIV_NUM    0x000000FFU
#define PCR_APB_DECREASE_DIV_NUM_M  (PCR_APB_DECREASE_DIV_NUM_V << PCR_APB_DECREASE_DIV_NUM_S)
#define PCR_APB_DECREASE_DIV_NUM_V  0x000000FFU
#define PCR_APB_DECREASE_DIV_NUM_S  0
/** PCR_APB_DIV_NUM : R/W; bitpos: [15:8]; default: 0;
 *  Set as one within (0,1,3) to generate clk_apb derived by clk_ahb. The clk_apb is
 *  div1(default)/div2/div4 of clk_ahb.
 */
#define PCR_APB_DIV_NUM    0x000000FFU
#define PCR_APB_DIV_NUM_M  (PCR_APB_DIV_NUM_V << PCR_APB_DIV_NUM_S)
#define PCR_APB_DIV_NUM_V  0x000000FFU
#define PCR_APB_DIV_NUM_S  8

/** PCR_SYSCLK_FREQ_QUERY_0_REG register
 *  SYSCLK frequency query 0 register
 */
#define PCR_SYSCLK_FREQ_QUERY_0_REG (DR_REG_PCR_BASE + 0x128)
/** PCR_FOSC_FREQ : HRO; bitpos: [7:0]; default: 8;
 *  This field indicates the frequency(MHz) of FOSC.
 */
#define PCR_FOSC_FREQ    0x000000FFU
#define PCR_FOSC_FREQ_M  (PCR_FOSC_FREQ_V << PCR_FOSC_FREQ_S)
#define PCR_FOSC_FREQ_V  0x000000FFU
#define PCR_FOSC_FREQ_S  0
/** PCR_PLL_FREQ : HRO; bitpos: [17:8]; default: 96;
 *  This field indicates the frequency(MHz) of SPLL.
 */
#define PCR_PLL_FREQ    0x000003FFU
#define PCR_PLL_FREQ_M  (PCR_PLL_FREQ_V << PCR_PLL_FREQ_S)
#define PCR_PLL_FREQ_V  0x000003FFU
#define PCR_PLL_FREQ_S  8

/** PCR_PLL_DIV_CLK_EN_REG register
 *  SPLL DIV clock-gating configuration register
 */
#define PCR_PLL_DIV_CLK_EN_REG (DR_REG_PCR_BASE + 0x12c)
/** PCR_PLL_96M_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  This field is used to open 96 MHz clock  derived from SPLL 96M. 0: close, 1:
 *  open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_96M_CLK_EN    (BIT(0))
#define PCR_PLL_96M_CLK_EN_M  (PCR_PLL_96M_CLK_EN_V << PCR_PLL_96M_CLK_EN_S)
#define PCR_PLL_96M_CLK_EN_V  0x00000001U
#define PCR_PLL_96M_CLK_EN_S  0
/** PCR_PLL_64M_CLK_EN : R/W; bitpos: [1]; default: 1;
 *  This field is used to open 64 MHz clock derived from SPLL 64M. 0: close, 1:
 *  open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_64M_CLK_EN    (BIT(1))
#define PCR_PLL_64M_CLK_EN_M  (PCR_PLL_64M_CLK_EN_V << PCR_PLL_64M_CLK_EN_S)
#define PCR_PLL_64M_CLK_EN_V  0x00000001U
#define PCR_PLL_64M_CLK_EN_S  1
/** PCR_PLL_48M_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  This field is used to open 48 MHz clock (div2 of SPLL) derived from SPLL 96M. 0:
 *  close, 1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_48M_CLK_EN    (BIT(2))
#define PCR_PLL_48M_CLK_EN_M  (PCR_PLL_48M_CLK_EN_V << PCR_PLL_48M_CLK_EN_S)
#define PCR_PLL_48M_CLK_EN_V  0x00000001U
#define PCR_PLL_48M_CLK_EN_S  2
/** PCR_PLL_32M_CLK_EN : R/W; bitpos: [3]; default: 1;
 *  This field is used to open 32 MHz clock (div2  of SPLL) derived from SPLL 64M. 0:
 *  close, 1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_32M_CLK_EN    (BIT(3))
#define PCR_PLL_32M_CLK_EN_M  (PCR_PLL_32M_CLK_EN_V << PCR_PLL_32M_CLK_EN_S)
#define PCR_PLL_32M_CLK_EN_V  0x00000001U
#define PCR_PLL_32M_CLK_EN_S  3
/** PCR_PLL_16M_CLK_EN : R/W; bitpos: [4]; default: 1;
 *  This field is used to open 16 MHz clock (div6 of SPLL) derived from SPLL 96M. 0:
 *  close, 1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_16M_CLK_EN    (BIT(4))
#define PCR_PLL_16M_CLK_EN_M  (PCR_PLL_16M_CLK_EN_V << PCR_PLL_16M_CLK_EN_S)
#define PCR_PLL_16M_CLK_EN_V  0x00000001U
#define PCR_PLL_16M_CLK_EN_S  4
/** PCR_PLL_8M_CLK_EN : R/W; bitpos: [5]; default: 1;
 *  This field is used to open   8 MHz clock (div12 of SPLL) derived from SPLL 96M. 0:
 *  close, 1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_8M_CLK_EN    (BIT(5))
#define PCR_PLL_8M_CLK_EN_M  (PCR_PLL_8M_CLK_EN_V << PCR_PLL_8M_CLK_EN_S)
#define PCR_PLL_8M_CLK_EN_V  0x00000001U
#define PCR_PLL_8M_CLK_EN_S  5

/** PCR_CTRL_CLK_OUT_EN_REG register
 *  CLK_OUT_EN configuration register
 */
#define PCR_CTRL_CLK_OUT_EN_REG (DR_REG_PCR_BASE + 0x130)
/** PCR_CLK8_OEN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable 8m clock
 */
#define PCR_CLK8_OEN    (BIT(0))
#define PCR_CLK8_OEN_M  (PCR_CLK8_OEN_V << PCR_CLK8_OEN_S)
#define PCR_CLK8_OEN_V  0x00000001U
#define PCR_CLK8_OEN_S  0
/** PCR_CLK16_OEN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to enable 16m clock
 */
#define PCR_CLK16_OEN    (BIT(1))
#define PCR_CLK16_OEN_M  (PCR_CLK16_OEN_V << PCR_CLK16_OEN_S)
#define PCR_CLK16_OEN_V  0x00000001U
#define PCR_CLK16_OEN_S  1
/** PCR_CLK32_OEN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable 32m clock
 */
#define PCR_CLK32_OEN    (BIT(2))
#define PCR_CLK32_OEN_M  (PCR_CLK32_OEN_V << PCR_CLK32_OEN_S)
#define PCR_CLK32_OEN_V  0x00000001U
#define PCR_CLK32_OEN_S  2
/** PCR_CLK_ADC_INF_OEN : R/W; bitpos: [3]; default: 1;
 *  Reserved
 */
#define PCR_CLK_ADC_INF_OEN    (BIT(3))
#define PCR_CLK_ADC_INF_OEN_M  (PCR_CLK_ADC_INF_OEN_V << PCR_CLK_ADC_INF_OEN_S)
#define PCR_CLK_ADC_INF_OEN_V  0x00000001U
#define PCR_CLK_ADC_INF_OEN_S  3
/** PCR_CLK_DFM_INF_OEN : R/W; bitpos: [4]; default: 1;
 *  Reserved
 */
#define PCR_CLK_DFM_INF_OEN    (BIT(4))
#define PCR_CLK_DFM_INF_OEN_M  (PCR_CLK_DFM_INF_OEN_V << PCR_CLK_DFM_INF_OEN_S)
#define PCR_CLK_DFM_INF_OEN_V  0x00000001U
#define PCR_CLK_DFM_INF_OEN_S  4
/** PCR_CLK_SDM_MOD_OEN : R/W; bitpos: [5]; default: 1;
 *  Reserved
 */
#define PCR_CLK_SDM_MOD_OEN    (BIT(5))
#define PCR_CLK_SDM_MOD_OEN_M  (PCR_CLK_SDM_MOD_OEN_V << PCR_CLK_SDM_MOD_OEN_S)
#define PCR_CLK_SDM_MOD_OEN_V  0x00000001U
#define PCR_CLK_SDM_MOD_OEN_S  5
/** PCR_CLK_XTAL_OEN : R/W; bitpos: [6]; default: 1;
 *  Set 1 to enable xtal clock
 */
#define PCR_CLK_XTAL_OEN    (BIT(6))
#define PCR_CLK_XTAL_OEN_M  (PCR_CLK_XTAL_OEN_V << PCR_CLK_XTAL_OEN_S)
#define PCR_CLK_XTAL_OEN_V  0x00000001U
#define PCR_CLK_XTAL_OEN_S  6

/** PCR_TIMG_CALI_CLK_CONF_REG register
 *  timergrout calibrate clock configuration register
 */
#define PCR_TIMG_CALI_CLK_CONF_REG (DR_REG_PCR_BASE + 0x134)
/** PCR_TIMG_CALI_CLK_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures the 32KHz clock for TIMER_GROUP.
 *  0 (default): RC32K_CLK
 *  1: XTAL32K_CLK
 *  2: OSC_SLOW_CLK
 *  3: RC_SLOW_CLK
 *  4: TIMG_SECURE_CLK
 */
#define PCR_TIMG_CALI_CLK_SEL    0x00000007U
#define PCR_TIMG_CALI_CLK_SEL_M  (PCR_TIMG_CALI_CLK_SEL_V << PCR_TIMG_CALI_CLK_SEL_S)
#define PCR_TIMG_CALI_CLK_SEL_V  0x00000007U
#define PCR_TIMG_CALI_CLK_SEL_S  0
/** PCR_TIMG_SECURE_CLK_SEL : R/W; bitpos: [7:4]; default: 7;
 *  Configures the clock source for the TIMG_SECURE_CLK.
 *  0 (default):CPU_CLK
 *  1: AHB_CLK
 *  2: APB_CLK
 *  3: sec function clock
 *  4: mspi function clock
 *  5: iomux function clock
 *  6: parl io rx function clock
 *  7: parl io tx function clock
 *  8: spi2 function clock
 *  9: spi3 function clock
 */
#define PCR_TIMG_SECURE_CLK_SEL    0x0000000FU
#define PCR_TIMG_SECURE_CLK_SEL_M  (PCR_TIMG_SECURE_CLK_SEL_V << PCR_TIMG_SECURE_CLK_SEL_S)
#define PCR_TIMG_SECURE_CLK_SEL_V  0x0000000FU
#define PCR_TIMG_SECURE_CLK_SEL_S  4
/** PCR_TIMG_SECURE_CLK_DIV_NUM : R/W; bitpos: [15:8]; default: 7;
 *  When PCR_TIMG_CALI_CLK_SEL set as 4, This field PCR_TIMG_SECURE_CLK_DIV_NUM is used
 *  to set the divider number of TIMG_SECURE_CLK.
 */
#define PCR_TIMG_SECURE_CLK_DIV_NUM    0x000000FFU
#define PCR_TIMG_SECURE_CLK_DIV_NUM_M  (PCR_TIMG_SECURE_CLK_DIV_NUM_V << PCR_TIMG_SECURE_CLK_DIV_NUM_S)
#define PCR_TIMG_SECURE_CLK_DIV_NUM_V  0x000000FFU
#define PCR_TIMG_SECURE_CLK_DIV_NUM_S  8

/** PCR_ROM_MEM_LP_CTRL_REG register
 *  rom power control register
 */
#define PCR_ROM_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x138)
/** PCR_INTERNAL_ROM_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures rom low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_INTERNAL_ROM_MEM_LP_MODE    0x00000003U
#define PCR_INTERNAL_ROM_MEM_LP_MODE_M  (PCR_INTERNAL_ROM_MEM_LP_MODE_V << PCR_INTERNAL_ROM_MEM_LP_MODE_S)
#define PCR_INTERNAL_ROM_MEM_LP_MODE_V  0x00000003U
#define PCR_INTERNAL_ROM_MEM_LP_MODE_S  0
/** PCR_INTERNAL_ROM_MEM_LP_EN : R/W; bitpos: [3:2]; default: 0;
 *  Set this bit to power down rom.
 */
#define PCR_INTERNAL_ROM_MEM_LP_EN    0x00000003U
#define PCR_INTERNAL_ROM_MEM_LP_EN_M  (PCR_INTERNAL_ROM_MEM_LP_EN_V << PCR_INTERNAL_ROM_MEM_LP_EN_S)
#define PCR_INTERNAL_ROM_MEM_LP_EN_V  0x00000003U
#define PCR_INTERNAL_ROM_MEM_LP_EN_S  2
/** PCR_INTERNAL_ROM_MEM_FORCE_CTRL : R/W; bitpos: [5:4]; default: 0;
 *  Set this bit to force software control rom, disable hardware control.
 */
#define PCR_INTERNAL_ROM_MEM_FORCE_CTRL    0x00000003U
#define PCR_INTERNAL_ROM_MEM_FORCE_CTRL_M  (PCR_INTERNAL_ROM_MEM_FORCE_CTRL_V << PCR_INTERNAL_ROM_MEM_FORCE_CTRL_S)
#define PCR_INTERNAL_ROM_MEM_FORCE_CTRL_V  0x00000003U
#define PCR_INTERNAL_ROM_MEM_FORCE_CTRL_S  4

/** PCR_SRAM_POWER_CONF_1_REG register
 *  HP SRAM/ROM configuration register
 */
#define PCR_SRAM_POWER_CONF_1_REG (DR_REG_PCR_BASE + 0x13c)
/** PCR_SRAM_CLKGATE_FORCE_ON : R/W; bitpos: [26:20]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the SRAM. 0: A
 *  gate-clock will be used when accessing the SRAM.
 */
#define PCR_SRAM_CLKGATE_FORCE_ON    0x0000007FU
#define PCR_SRAM_CLKGATE_FORCE_ON_M  (PCR_SRAM_CLKGATE_FORCE_ON_V << PCR_SRAM_CLKGATE_FORCE_ON_S)
#define PCR_SRAM_CLKGATE_FORCE_ON_V  0x0000007FU
#define PCR_SRAM_CLKGATE_FORCE_ON_S  20

/** PCR_SEC_CONF_REG register
 *  Clock source configuration register for External Memory Encryption and Decryption
 */
#define PCR_SEC_CONF_REG (DR_REG_PCR_BASE + 0x140)
/** PCR_SEC_CLK_SEL : R/W; bitpos: [1:0]; default: 0;
 *  Configures the clock source for the External Memory Encryption and Decryption
 *  module.
 *  0(default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F64M_CLK
 *  3: PLL_F96M_CLK
 */
#define PCR_SEC_CLK_SEL    0x00000003U
#define PCR_SEC_CLK_SEL_M  (PCR_SEC_CLK_SEL_V << PCR_SEC_CLK_SEL_S)
#define PCR_SEC_CLK_SEL_V  0x00000003U
#define PCR_SEC_CLK_SEL_S  0
/** PCR_SEC_RST_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to reset sec module
 */
#define PCR_SEC_RST_EN    (BIT(2))
#define PCR_SEC_RST_EN_M  (PCR_SEC_RST_EN_V << PCR_SEC_RST_EN_S)
#define PCR_SEC_RST_EN_V  0x00000001U
#define PCR_SEC_RST_EN_S  2
/** PCR_SEC_DIV_NUMINATOR : R/W; bitpos: [6:3]; default: 0;
 *  The  denominator of the frequency divider factor of the sec function clock.
 */
#define PCR_SEC_DIV_NUMINATOR    0x0000000FU
#define PCR_SEC_DIV_NUMINATOR_M  (PCR_SEC_DIV_NUMINATOR_V << PCR_SEC_DIV_NUMINATOR_S)
#define PCR_SEC_DIV_NUMINATOR_V  0x0000000FU
#define PCR_SEC_DIV_NUMINATOR_S  3
/** PCR_SEC_DIV_DENOMINATOR : R/W; bitpos: [10:7]; default: 0;
 *  The numerator of the frequency divider factor of the sec function clock.
 */
#define PCR_SEC_DIV_DENOMINATOR    0x0000000FU
#define PCR_SEC_DIV_DENOMINATOR_M  (PCR_SEC_DIV_DENOMINATOR_V << PCR_SEC_DIV_DENOMINATOR_S)
#define PCR_SEC_DIV_DENOMINATOR_V  0x0000000FU
#define PCR_SEC_DIV_DENOMINATOR_S  7
/** PCR_SEC_DIV_NUM : R/W; bitpos: [14:11]; default: 0;
 *  The integral part of the frequency divider factor of the sec function clock.
 */
#define PCR_SEC_DIV_NUM    0x0000000FU
#define PCR_SEC_DIV_NUM_M  (PCR_SEC_DIV_NUM_V << PCR_SEC_DIV_NUM_S)
#define PCR_SEC_DIV_NUM_V  0x0000000FU
#define PCR_SEC_DIV_NUM_S  11

/** PCR_BUS_CLK_UPDATE_REG register
 *  Configuration register for applying updated high-performance system clock sources
 */
#define PCR_BUS_CLK_UPDATE_REG (DR_REG_PCR_BASE + 0x148)
/** PCR_BUS_CLOCK_UPDATE : R/W/WTC; bitpos: [0]; default: 0;
 *  Configures whether or not to update configurations for CPU_CLK division, AHB_CLK
 *  division and HP_ROOT_CLK clock source selection.
 *  0: Not update configurations
 *  1: Update configurations
 *  This bit is automatically cleared when configurations have been updated.
 */
#define PCR_BUS_CLOCK_UPDATE    (BIT(0))
#define PCR_BUS_CLOCK_UPDATE_M  (PCR_BUS_CLOCK_UPDATE_V << PCR_BUS_CLOCK_UPDATE_S)
#define PCR_BUS_CLOCK_UPDATE_V  0x00000001U
#define PCR_BUS_CLOCK_UPDATE_S  0

/** PCR_SAR_CLK_DIV_REG register
 *  SAR ADC clock divisor configuration register
 */
#define PCR_SAR_CLK_DIV_REG (DR_REG_PCR_BASE + 0x14c)
/** PCR_SAR2_CLK_DIV_NUM : R/W; bitpos: [7:0]; default: 15;
 *  Configures the divisor for SAR ADC 2  clock to generate ADC analog control signals.
 */
#define PCR_SAR2_CLK_DIV_NUM    0x000000FFU
#define PCR_SAR2_CLK_DIV_NUM_M  (PCR_SAR2_CLK_DIV_NUM_V << PCR_SAR2_CLK_DIV_NUM_S)
#define PCR_SAR2_CLK_DIV_NUM_V  0x000000FFU
#define PCR_SAR2_CLK_DIV_NUM_S  0
/** PCR_SAR1_CLK_DIV_NUM : R/W; bitpos: [15:8]; default: 15;
 *  Configures the divisor for SAR ADC 1  clock to generate ADC analog control signals.
 */
#define PCR_SAR1_CLK_DIV_NUM    0x000000FFU
#define PCR_SAR1_CLK_DIV_NUM_M  (PCR_SAR1_CLK_DIV_NUM_V << PCR_SAR1_CLK_DIV_NUM_S)
#define PCR_SAR1_CLK_DIV_NUM_V  0x000000FFU
#define PCR_SAR1_CLK_DIV_NUM_S  8

/** PCR_PWDET_SAR_CLK_CONF_REG register
 *  xxxx
 */
#define PCR_PWDET_SAR_CLK_CONF_REG (DR_REG_PCR_BASE + 0x150)
/** PCR_PWDET_SAR_READER_EN : R/W; bitpos: [8]; default: 1;
 *  xxxx
 */
#define PCR_PWDET_SAR_READER_EN    (BIT(8))
#define PCR_PWDET_SAR_READER_EN_M  (PCR_PWDET_SAR_READER_EN_V << PCR_PWDET_SAR_READER_EN_S)
#define PCR_PWDET_SAR_READER_EN_V  0x00000001U
#define PCR_PWDET_SAR_READER_EN_S  8

/** PCR_TIMERGROUP_WDT_CONF_REG register
 *  TIMERGROUP_WDT configuration register
 */
#define PCR_TIMERGROUP_WDT_CONF_REG (DR_REG_PCR_BASE + 0x154)
/** PCR_TG0_WDT_RST_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to reset timer_group0 wdt module
 */
#define PCR_TG0_WDT_RST_EN    (BIT(0))
#define PCR_TG0_WDT_RST_EN_M  (PCR_TG0_WDT_RST_EN_V << PCR_TG0_WDT_RST_EN_S)
#define PCR_TG0_WDT_RST_EN_V  0x00000001U
#define PCR_TG0_WDT_RST_EN_S  0
/** PCR_TG1_WDT_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset timer_group1 wdt module
 */
#define PCR_TG1_WDT_RST_EN    (BIT(1))
#define PCR_TG1_WDT_RST_EN_M  (PCR_TG1_WDT_RST_EN_V << PCR_TG1_WDT_RST_EN_S)
#define PCR_TG1_WDT_RST_EN_V  0x00000001U
#define PCR_TG1_WDT_RST_EN_S  1

/** PCR_TIMERGROUP_XTAL_CONF_REG register
 *  TIMERGROUP1 configuration register
 */
#define PCR_TIMERGROUP_XTAL_CONF_REG (DR_REG_PCR_BASE + 0x160)
/** PCR_TG0_XTAL_RST_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to reset timer_group0 xtal clock domain
 */
#define PCR_TG0_XTAL_RST_EN    (BIT(0))
#define PCR_TG0_XTAL_RST_EN_M  (PCR_TG0_XTAL_RST_EN_V << PCR_TG0_XTAL_RST_EN_S)
#define PCR_TG0_XTAL_RST_EN_V  0x00000001U
#define PCR_TG0_XTAL_RST_EN_S  0
/** PCR_TG1_XTAL_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset timer_group1 xtal clock domain
 */
#define PCR_TG1_XTAL_RST_EN    (BIT(1))
#define PCR_TG1_XTAL_RST_EN_M  (PCR_TG1_XTAL_RST_EN_V << PCR_TG1_XTAL_RST_EN_S)
#define PCR_TG1_XTAL_RST_EN_V  0x00000001U
#define PCR_TG1_XTAL_RST_EN_S  1
/** PCR_TG0_XTAL_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable tg0 xtal clock
 */
#define PCR_TG0_XTAL_CLK_EN    (BIT(2))
#define PCR_TG0_XTAL_CLK_EN_M  (PCR_TG0_XTAL_CLK_EN_V << PCR_TG0_XTAL_CLK_EN_S)
#define PCR_TG0_XTAL_CLK_EN_V  0x00000001U
#define PCR_TG0_XTAL_CLK_EN_S  2

/** PCR_KM_CONF_REG register
 *  Key Manager configuration register
 */
#define PCR_KM_CONF_REG (DR_REG_PCR_BASE + 0x164)
/** PCR_KM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable km clock
 */
#define PCR_KM_CLK_EN    (BIT(0))
#define PCR_KM_CLK_EN_M  (PCR_KM_CLK_EN_V << PCR_KM_CLK_EN_S)
#define PCR_KM_CLK_EN_V  0x00000001U
#define PCR_KM_CLK_EN_S  0
/** PCR_KM_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset km module
 */
#define PCR_KM_RST_EN    (BIT(1))
#define PCR_KM_RST_EN_M  (PCR_KM_RST_EN_V << PCR_KM_RST_EN_S)
#define PCR_KM_RST_EN_V  0x00000001U
#define PCR_KM_RST_EN_S  1
/** PCR_KM_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset km module
 */
#define PCR_KM_READY    (BIT(2))
#define PCR_KM_READY_M  (PCR_KM_READY_V << PCR_KM_READY_S)
#define PCR_KM_READY_V  0x00000001U
#define PCR_KM_READY_S  2

/** PCR_KM_MEM_LP_CTRL_REG register
 *  KM memory power control register
 */
#define PCR_KM_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x168)
/** PCR_KM_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 2;
 *  Configures km memory low power mode in low power stage.
 *  0: deep sleep
 *  1: light sleep
 *  2(default): shut down
 *  3: disable low power stage
 */
#define PCR_KM_MEM_LP_MODE    0x00000003U
#define PCR_KM_MEM_LP_MODE_M  (PCR_KM_MEM_LP_MODE_V << PCR_KM_MEM_LP_MODE_S)
#define PCR_KM_MEM_LP_MODE_V  0x00000003U
#define PCR_KM_MEM_LP_MODE_S  0
/** PCR_KM_MEM_LP_EN : R/W; bitpos: [2]; default: 1;
 *  Set this bit to power down km memory.
 */
#define PCR_KM_MEM_LP_EN    (BIT(2))
#define PCR_KM_MEM_LP_EN_M  (PCR_KM_MEM_LP_EN_V << PCR_KM_MEM_LP_EN_S)
#define PCR_KM_MEM_LP_EN_V  0x00000001U
#define PCR_KM_MEM_LP_EN_S  2
/** PCR_KM_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control km memory, disable hardware control.
 */
#define PCR_KM_MEM_FORCE_CTRL    (BIT(3))
#define PCR_KM_MEM_FORCE_CTRL_M  (PCR_KM_MEM_FORCE_CTRL_V << PCR_KM_MEM_FORCE_CTRL_S)
#define PCR_KM_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_KM_MEM_FORCE_CTRL_S  3

/** PCR_TCM_MEM_MONITOR_CONF_REG register
 *  TCM_MEM_MONITOR configuration register
 */
#define PCR_TCM_MEM_MONITOR_CONF_REG (DR_REG_PCR_BASE + 0x16c)
/** PCR_TCM_MEM_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable tcm_mem_monitor clock
 */
#define PCR_TCM_MEM_MONITOR_CLK_EN    (BIT(0))
#define PCR_TCM_MEM_MONITOR_CLK_EN_M  (PCR_TCM_MEM_MONITOR_CLK_EN_V << PCR_TCM_MEM_MONITOR_CLK_EN_S)
#define PCR_TCM_MEM_MONITOR_CLK_EN_V  0x00000001U
#define PCR_TCM_MEM_MONITOR_CLK_EN_S  0
/** PCR_TCM_MEM_MONITOR_RST_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to reset tcm_mem_monitor module
 */
#define PCR_TCM_MEM_MONITOR_RST_EN    (BIT(1))
#define PCR_TCM_MEM_MONITOR_RST_EN_M  (PCR_TCM_MEM_MONITOR_RST_EN_V << PCR_TCM_MEM_MONITOR_RST_EN_S)
#define PCR_TCM_MEM_MONITOR_RST_EN_V  0x00000001U
#define PCR_TCM_MEM_MONITOR_RST_EN_S  1
/** PCR_TCM_MEM_MONITOR_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset tcm_mem_monitor module
 */
#define PCR_TCM_MEM_MONITOR_READY    (BIT(2))
#define PCR_TCM_MEM_MONITOR_READY_M  (PCR_TCM_MEM_MONITOR_READY_V << PCR_TCM_MEM_MONITOR_READY_S)
#define PCR_TCM_MEM_MONITOR_READY_V  0x00000001U
#define PCR_TCM_MEM_MONITOR_READY_S  2

/** PCR_PSRAM_MEM_MONITOR_CONF_REG register
 *  PSRAM_MEM_MONITOR configuration register
 */
#define PCR_PSRAM_MEM_MONITOR_CONF_REG (DR_REG_PCR_BASE + 0x170)
/** PCR_PSRAM_MEM_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable psram_mem_monitor clock
 */
#define PCR_PSRAM_MEM_MONITOR_CLK_EN    (BIT(0))
#define PCR_PSRAM_MEM_MONITOR_CLK_EN_M  (PCR_PSRAM_MEM_MONITOR_CLK_EN_V << PCR_PSRAM_MEM_MONITOR_CLK_EN_S)
#define PCR_PSRAM_MEM_MONITOR_CLK_EN_V  0x00000001U
#define PCR_PSRAM_MEM_MONITOR_CLK_EN_S  0
/** PCR_PSRAM_MEM_MONITOR_RST_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to reset psram_mem_monitor module
 */
#define PCR_PSRAM_MEM_MONITOR_RST_EN    (BIT(1))
#define PCR_PSRAM_MEM_MONITOR_RST_EN_M  (PCR_PSRAM_MEM_MONITOR_RST_EN_V << PCR_PSRAM_MEM_MONITOR_RST_EN_S)
#define PCR_PSRAM_MEM_MONITOR_RST_EN_V  0x00000001U
#define PCR_PSRAM_MEM_MONITOR_RST_EN_S  1
/** PCR_PSRAM_MEM_MONITOR_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset psram_mem_monitor module
 */
#define PCR_PSRAM_MEM_MONITOR_READY    (BIT(2))
#define PCR_PSRAM_MEM_MONITOR_READY_M  (PCR_PSRAM_MEM_MONITOR_READY_V << PCR_PSRAM_MEM_MONITOR_READY_S)
#define PCR_PSRAM_MEM_MONITOR_READY_V  0x00000001U
#define PCR_PSRAM_MEM_MONITOR_READY_S  2

/** PCR_RESET_EVENT_BYPASS_REG register
 *  reset event bypass backdoor configuration register
 */
#define PCR_RESET_EVENT_BYPASS_REG (DR_REG_PCR_BASE + 0x174)
/** PCR_RESET_EVENT_BYPASS_TEE_APM : R/W; bitpos: [0]; default: 0;
 *  This field is used to control reset event relationship for tee_apm. 1: tee_apm will
 *  only be reset by power-reset. some reset event will be bypass. 0:tee_apm will not
 *  only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_TEE_APM    (BIT(0))
#define PCR_RESET_EVENT_BYPASS_TEE_APM_M  (PCR_RESET_EVENT_BYPASS_TEE_APM_V << PCR_RESET_EVENT_BYPASS_TEE_APM_S)
#define PCR_RESET_EVENT_BYPASS_TEE_APM_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_TEE_APM_S  0
/** PCR_RESET_EVENT_BYPASS_PCR : R/W; bitpos: [1]; default: 0;
 *  This field is used to control reset event relationship for pcr. 1: pcr will only be
 *  reset by power-reset. some reset event will be bypass. 0:pcr will not only be reset
 *  by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_PCR    (BIT(1))
#define PCR_RESET_EVENT_BYPASS_PCR_M  (PCR_RESET_EVENT_BYPASS_PCR_V << PCR_RESET_EVENT_BYPASS_PCR_S)
#define PCR_RESET_EVENT_BYPASS_PCR_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_PCR_S  1
/** PCR_RESET_EVENT_BYPASS_HPSYSREG : R/W; bitpos: [2]; default: 0;
 *  This field is used to control reset event relationship for hpsysreg. 1: hpsysreg
 *  will only be reset by power-reset. some reset event will be bypass. 0:hpsysreg will
 *  not only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_HPSYSREG    (BIT(2))
#define PCR_RESET_EVENT_BYPASS_HPSYSREG_M  (PCR_RESET_EVENT_BYPASS_HPSYSREG_V << PCR_RESET_EVENT_BYPASS_HPSYSREG_S)
#define PCR_RESET_EVENT_BYPASS_HPSYSREG_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_HPSYSREG_S  2
/** PCR_RESET_EVENT_BYPASS_IOMUX : R/W; bitpos: [3]; default: 0;
 *  This field is used to control reset event relationship for iomux. 1: iomux will
 *  only be reset by power-reset. some reset event will be bypass. 0:iomux will not
 *  only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_IOMUX    (BIT(3))
#define PCR_RESET_EVENT_BYPASS_IOMUX_M  (PCR_RESET_EVENT_BYPASS_IOMUX_V << PCR_RESET_EVENT_BYPASS_IOMUX_S)
#define PCR_RESET_EVENT_BYPASS_IOMUX_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_IOMUX_S  3
/** PCR_RESET_EVENT_BYPASS_INTMTX0 : R/W; bitpos: [4]; default: 0;
 *  This field is used to control reset event relationship for intmtx0. 1: intmtx0 will
 *  only be reset by power-reset. some reset event will be bypass. 0:intmtx0 will not
 *  only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_INTMTX0    (BIT(4))
#define PCR_RESET_EVENT_BYPASS_INTMTX0_M  (PCR_RESET_EVENT_BYPASS_INTMTX0_V << PCR_RESET_EVENT_BYPASS_INTMTX0_S)
#define PCR_RESET_EVENT_BYPASS_INTMTX0_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_INTMTX0_S  4
/** PCR_RESET_EVENT_BYPASS_INTMTX1 : R/W; bitpos: [5]; default: 0;
 *  This field is used to control reset event relationship for intmtx1. 1: intmtx1 will
 *  only be reset by power-reset. some reset event will be bypass. 0:intmtx1 will not
 *  only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_INTMTX1    (BIT(5))
#define PCR_RESET_EVENT_BYPASS_INTMTX1_M  (PCR_RESET_EVENT_BYPASS_INTMTX1_V << PCR_RESET_EVENT_BYPASS_INTMTX1_S)
#define PCR_RESET_EVENT_BYPASS_INTMTX1_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_INTMTX1_S  5
/** PCR_RESET_EVENT_BYPASS_MODEM : R/W; bitpos: [6]; default: 0;
 *  This field is used to control reset event relationship for modem. 1: modem will
 *  only be reset by power-reset. some reset event will be bypass. 0:modem will not
 *  only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_MODEM    (BIT(6))
#define PCR_RESET_EVENT_BYPASS_MODEM_M  (PCR_RESET_EVENT_BYPASS_MODEM_V << PCR_RESET_EVENT_BYPASS_MODEM_S)
#define PCR_RESET_EVENT_BYPASS_MODEM_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_MODEM_S  6
/** PCR_RESET_EVENT_BYPASS_BUS_MODEM : R/W; bitpos: [7]; default: 0;
 *  This field is used to control reset event relationship for bus_modem. 1: bus_modem
 *  will only be reset by power-reset. some reset event will be bypass. 0:bus_modem
 *  will not only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_BUS_MODEM    (BIT(7))
#define PCR_RESET_EVENT_BYPASS_BUS_MODEM_M  (PCR_RESET_EVENT_BYPASS_BUS_MODEM_V << PCR_RESET_EVENT_BYPASS_BUS_MODEM_S)
#define PCR_RESET_EVENT_BYPASS_BUS_MODEM_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_BUS_MODEM_S  7

/** PCR_HPCORE_MEM_LP_CTRL_REG register
 *  HP CORE0 & HP CORE1 memory power control register
 */
#define PCR_HPCORE_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x178)
/** PCR_HPCORE_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_HPCORE_MEM_LP_MODE    0x00000003U
#define PCR_HPCORE_MEM_LP_MODE_M  (PCR_HPCORE_MEM_LP_MODE_V << PCR_HPCORE_MEM_LP_MODE_S)
#define PCR_HPCORE_MEM_LP_MODE_V  0x00000003U
#define PCR_HPCORE_MEM_LP_MODE_S  0
/** PCR_HPCORE_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down HP CORE0 & HP CORE1 memory.
 */
#define PCR_HPCORE_MEM_LP_EN    (BIT(2))
#define PCR_HPCORE_MEM_LP_EN_M  (PCR_HPCORE_MEM_LP_EN_V << PCR_HPCORE_MEM_LP_EN_S)
#define PCR_HPCORE_MEM_LP_EN_V  0x00000001U
#define PCR_HPCORE_MEM_LP_EN_S  2
/** PCR_HPCORE_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control HP CORE0 & HP CORE1 memory, disable hardware
 *  control.
 */
#define PCR_HPCORE_MEM_FORCE_CTRL    (BIT(3))
#define PCR_HPCORE_MEM_FORCE_CTRL_M  (PCR_HPCORE_MEM_FORCE_CTRL_V << PCR_HPCORE_MEM_FORCE_CTRL_S)
#define PCR_HPCORE_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_HPCORE_MEM_FORCE_CTRL_S  3

/** PCR_USB_OTG11_CONF_REG register
 *  USB_OTG configuration register
 */
#define PCR_USB_OTG11_CONF_REG (DR_REG_PCR_BASE + 0x17c)
/** PCR_USB_OTG11_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable usb_otg11 clock
 */
#define PCR_USB_OTG11_CLK_EN    (BIT(0))
#define PCR_USB_OTG11_CLK_EN_M  (PCR_USB_OTG11_CLK_EN_V << PCR_USB_OTG11_CLK_EN_S)
#define PCR_USB_OTG11_CLK_EN_V  0x00000001U
#define PCR_USB_OTG11_CLK_EN_S  0
/** PCR_USB_OTG11_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset usb_otg11 module
 */
#define PCR_USB_OTG11_RST_EN    (BIT(1))
#define PCR_USB_OTG11_RST_EN_M  (PCR_USB_OTG11_RST_EN_V << PCR_USB_OTG11_RST_EN_S)
#define PCR_USB_OTG11_RST_EN_V  0x00000001U
#define PCR_USB_OTG11_RST_EN_S  1
/** PCR_USB_OTG11_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset usb_otg11 module
 */
#define PCR_USB_OTG11_READY    (BIT(2))
#define PCR_USB_OTG11_READY_M  (PCR_USB_OTG11_READY_V << PCR_USB_OTG11_READY_S)
#define PCR_USB_OTG11_READY_V  0x00000001U
#define PCR_USB_OTG11_READY_S  2

/** PCR_USB_OTG11_MEM_LP_CTRL_REG register
 *  USB_OTG11 memory power control register
 */
#define PCR_USB_OTG11_MEM_LP_CTRL_REG (DR_REG_PCR_BASE + 0x180)
/** PCR_USB_OTG11_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures usb_otg11 memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define PCR_USB_OTG11_MEM_LP_MODE    0x00000003U
#define PCR_USB_OTG11_MEM_LP_MODE_M  (PCR_USB_OTG11_MEM_LP_MODE_V << PCR_USB_OTG11_MEM_LP_MODE_S)
#define PCR_USB_OTG11_MEM_LP_MODE_V  0x00000003U
#define PCR_USB_OTG11_MEM_LP_MODE_S  0
/** PCR_USB_OTG11_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down usb_otg11 memory.
 */
#define PCR_USB_OTG11_MEM_LP_EN    (BIT(2))
#define PCR_USB_OTG11_MEM_LP_EN_M  (PCR_USB_OTG11_MEM_LP_EN_V << PCR_USB_OTG11_MEM_LP_EN_S)
#define PCR_USB_OTG11_MEM_LP_EN_V  0x00000001U
#define PCR_USB_OTG11_MEM_LP_EN_S  2
/** PCR_USB_OTG11_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control usb_otg11 memory, disable hardware control.
 */
#define PCR_USB_OTG11_MEM_FORCE_CTRL    (BIT(3))
#define PCR_USB_OTG11_MEM_FORCE_CTRL_M  (PCR_USB_OTG11_MEM_FORCE_CTRL_V << PCR_USB_OTG11_MEM_FORCE_CTRL_S)
#define PCR_USB_OTG11_MEM_FORCE_CTRL_V  0x00000001U
#define PCR_USB_OTG11_MEM_FORCE_CTRL_S  3

/** PCR_CORE1_CONF_REG register
 *  USB_OTG configuration register
 */
#define PCR_CORE1_CONF_REG (DR_REG_PCR_BASE + 0x188)
/** PCR_CORE1_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable core1 clock
 */
#define PCR_CORE1_CLK_EN    (BIT(0))
#define PCR_CORE1_CLK_EN_M  (PCR_CORE1_CLK_EN_V << PCR_CORE1_CLK_EN_S)
#define PCR_CORE1_CLK_EN_V  0x00000001U
#define PCR_CORE1_CLK_EN_S  0
/** PCR_CORE1_RST_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to reset core1 module
 */
#define PCR_CORE1_RST_EN    (BIT(1))
#define PCR_CORE1_RST_EN_M  (PCR_CORE1_RST_EN_V << PCR_CORE1_RST_EN_S)
#define PCR_CORE1_RST_EN_V  0x00000001U
#define PCR_CORE1_RST_EN_S  1

/** PCR_I2C1_CONF_REG register
 *  I2C configuration register
 */
#define PCR_I2C1_CONF_REG (DR_REG_PCR_BASE + 0x18c)
/** PCR_I2C1_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable i2c1 apb clock
 */
#define PCR_I2C1_CLK_EN    (BIT(0))
#define PCR_I2C1_CLK_EN_M  (PCR_I2C1_CLK_EN_V << PCR_I2C1_CLK_EN_S)
#define PCR_I2C1_CLK_EN_V  0x00000001U
#define PCR_I2C1_CLK_EN_S  0
/** PCR_I2C1_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset i2c1 module
 */
#define PCR_I2C1_RST_EN    (BIT(1))
#define PCR_I2C1_RST_EN_M  (PCR_I2C1_RST_EN_V << PCR_I2C1_RST_EN_S)
#define PCR_I2C1_RST_EN_V  0x00000001U
#define PCR_I2C1_RST_EN_S  1

/** PCR_I2C1_SCLK_CONF_REG register
 *  I2C_SCLK configuration register
 */
#define PCR_I2C1_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x190)
/** PCR_I2C1_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the i2c1 function clock.
 */
#define PCR_I2C1_SCLK_DIV_A    0x0000003FU
#define PCR_I2C1_SCLK_DIV_A_M  (PCR_I2C1_SCLK_DIV_A_V << PCR_I2C1_SCLK_DIV_A_S)
#define PCR_I2C1_SCLK_DIV_A_V  0x0000003FU
#define PCR_I2C1_SCLK_DIV_A_S  0
/** PCR_I2C1_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the i2c1 function clock.
 */
#define PCR_I2C1_SCLK_DIV_B    0x0000003FU
#define PCR_I2C1_SCLK_DIV_B_M  (PCR_I2C1_SCLK_DIV_B_V << PCR_I2C1_SCLK_DIV_B_S)
#define PCR_I2C1_SCLK_DIV_B_V  0x0000003FU
#define PCR_I2C1_SCLK_DIV_B_S  6
/** PCR_I2C1_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the i2c1 function clock.
 */
#define PCR_I2C1_SCLK_DIV_NUM    0x000000FFU
#define PCR_I2C1_SCLK_DIV_NUM_M  (PCR_I2C1_SCLK_DIV_NUM_V << PCR_I2C1_SCLK_DIV_NUM_S)
#define PCR_I2C1_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_I2C1_SCLK_DIV_NUM_S  12
/** PCR_I2C1_SCLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of I2C.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 */
#define PCR_I2C1_SCLK_SEL    (BIT(20))
#define PCR_I2C1_SCLK_SEL_M  (PCR_I2C1_SCLK_SEL_V << PCR_I2C1_SCLK_SEL_S)
#define PCR_I2C1_SCLK_SEL_V  0x00000001U
#define PCR_I2C1_SCLK_SEL_S  20
/** PCR_I2C1_SCLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable i2c function clock
 */
#define PCR_I2C1_SCLK_EN    (BIT(22))
#define PCR_I2C1_SCLK_EN_M  (PCR_I2C1_SCLK_EN_V << PCR_I2C1_SCLK_EN_S)
#define PCR_I2C1_SCLK_EN_V  0x00000001U
#define PCR_I2C1_SCLK_EN_S  22

/** PCR_PWM1_CONF_REG register
 *  PWM1 configuration register
 */
#define PCR_PWM1_CONF_REG (DR_REG_PCR_BASE + 0x194)
/** PCR_PWM1_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable pwm1 clock
 */
#define PCR_PWM1_CLK_EN    (BIT(0))
#define PCR_PWM1_CLK_EN_M  (PCR_PWM1_CLK_EN_V << PCR_PWM1_CLK_EN_S)
#define PCR_PWM1_CLK_EN_V  0x00000001U
#define PCR_PWM1_CLK_EN_S  0
/** PCR_PWM1_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset pwm1 module
 */
#define PCR_PWM1_RST_EN    (BIT(1))
#define PCR_PWM1_RST_EN_M  (PCR_PWM1_RST_EN_V << PCR_PWM1_RST_EN_S)
#define PCR_PWM1_RST_EN_V  0x00000001U
#define PCR_PWM1_RST_EN_S  1
/** PCR_PWM1_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset pwm1 module
 */
#define PCR_PWM1_READY    (BIT(2))
#define PCR_PWM1_READY_M  (PCR_PWM1_READY_V << PCR_PWM1_READY_S)
#define PCR_PWM1_READY_V  0x00000001U
#define PCR_PWM1_READY_S  2

/** PCR_PWM1_CLK_CONF_REG register
 *  PWM1_CLK configuration register
 */
#define PCR_PWM1_CLK_CONF_REG (DR_REG_PCR_BASE + 0x198)
/** PCR_PWM1_DIV_NUM : R/W; bitpos: [19:12]; default: 4;
 *  The integral part of the frequency divider factor of the pwm1 function clock.
 */
#define PCR_PWM1_DIV_NUM    0x000000FFU
#define PCR_PWM1_DIV_NUM_M  (PCR_PWM1_DIV_NUM_V << PCR_PWM1_DIV_NUM_S)
#define PCR_PWM1_DIV_NUM_V  0x000000FFU
#define PCR_PWM1_DIV_NUM_S  12
/** PCR_PWM1_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of MCPWM1.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F160M_CLK
 */
#define PCR_PWM1_CLKM_SEL    0x00000003U
#define PCR_PWM1_CLKM_SEL_M  (PCR_PWM1_CLKM_SEL_V << PCR_PWM1_CLKM_SEL_S)
#define PCR_PWM1_CLKM_SEL_V  0x00000003U
#define PCR_PWM1_CLKM_SEL_S  20
/** PCR_PWM1_CLKM_EN : R/W; bitpos: [22]; default: 0;
 *  set this field as 1 to activate pwm1 clkm.
 */
#define PCR_PWM1_CLKM_EN    (BIT(22))
#define PCR_PWM1_CLKM_EN_M  (PCR_PWM1_CLKM_EN_V << PCR_PWM1_CLKM_EN_S)
#define PCR_PWM1_CLKM_EN_V  0x00000001U
#define PCR_PWM1_CLKM_EN_S  22

/** PCR_SPI3_CONF_REG register
 *  SPI3 configuration register
 */
#define PCR_SPI3_CONF_REG (DR_REG_PCR_BASE + 0x19c)
/** PCR_SPI3_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable spi3 apb clock
 */
#define PCR_SPI3_CLK_EN    (BIT(0))
#define PCR_SPI3_CLK_EN_M  (PCR_SPI3_CLK_EN_V << PCR_SPI3_CLK_EN_S)
#define PCR_SPI3_CLK_EN_V  0x00000001U
#define PCR_SPI3_CLK_EN_S  0
/** PCR_SPI3_RST_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to reset spi3 module
 */
#define PCR_SPI3_RST_EN    (BIT(1))
#define PCR_SPI3_RST_EN_M  (PCR_SPI3_RST_EN_V << PCR_SPI3_RST_EN_S)
#define PCR_SPI3_RST_EN_V  0x00000001U
#define PCR_SPI3_RST_EN_S  1
/** PCR_SPI3_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset spi3 module
 */
#define PCR_SPI3_READY    (BIT(2))
#define PCR_SPI3_READY_M  (PCR_SPI3_READY_V << PCR_SPI3_READY_S)
#define PCR_SPI3_READY_V  0x00000001U
#define PCR_SPI3_READY_S  2

/** PCR_SPI3_CLKM_CONF_REG register
 *  SPI3_CLKM configuration register
 */
#define PCR_SPI3_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x1a0)
/** PCR_SPI3_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the spi3_mst clock.
 */
#define PCR_SPI3_CLKM_DIV_NUM    0x000000FFU
#define PCR_SPI3_CLKM_DIV_NUM_M  (PCR_SPI3_CLKM_DIV_NUM_V << PCR_SPI3_CLKM_DIV_NUM_S)
#define PCR_SPI3_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_SPI3_CLKM_DIV_NUM_S  12
/** PCR_SPI3_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of SPI3.
 *  0 (default): XTAL_CLK
 *  1: PLL_F160M_CLK
 *  2: RC_FAST_CLK
 *  3: PLL_F120M_CLK
 */
#define PCR_SPI3_CLKM_SEL    0x00000003U
#define PCR_SPI3_CLKM_SEL_M  (PCR_SPI3_CLKM_SEL_V << PCR_SPI3_CLKM_SEL_S)
#define PCR_SPI3_CLKM_SEL_V  0x00000003U
#define PCR_SPI3_CLKM_SEL_S  20
/** PCR_SPI3_CLKM_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable spi3 function clock
 */
#define PCR_SPI3_CLKM_EN    (BIT(22))
#define PCR_SPI3_CLKM_EN_M  (PCR_SPI3_CLKM_EN_V << PCR_SPI3_CLKM_EN_S)
#define PCR_SPI3_CLKM_EN_V  0x00000001U
#define PCR_SPI3_CLKM_EN_S  22

/** PCR_ASRC_FUNC_CLK_CONF_REG register
 *  AUDIO_SAMPLE_RATE_FUNC_CLK configuration register
 */
#define PCR_ASRC_FUNC_CLK_CONF_REG (DR_REG_PCR_BASE + 0x1a8)
/** PCR_ASRC_APB_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable audio_sample rate converter apb clock
 */
#define PCR_ASRC_APB_CLK_EN    (BIT(22))
#define PCR_ASRC_APB_CLK_EN_M  (PCR_ASRC_APB_CLK_EN_V << PCR_ASRC_APB_CLK_EN_S)
#define PCR_ASRC_APB_CLK_EN_V  0x00000001U
#define PCR_ASRC_APB_CLK_EN_S  22
/** PCR_ASRC_FUNC_CLK_EN : R/W; bitpos: [23]; default: 1;
 *  Set 1 to enable audio_sample rate converter function clock
 */
#define PCR_ASRC_FUNC_CLK_EN    (BIT(23))
#define PCR_ASRC_FUNC_CLK_EN_M  (PCR_ASRC_FUNC_CLK_EN_V << PCR_ASRC_FUNC_CLK_EN_S)
#define PCR_ASRC_FUNC_CLK_EN_V  0x00000001U
#define PCR_ASRC_FUNC_CLK_EN_S  23
/** PCR_ASRC_RST_EN : R/W; bitpos: [24]; default: 0;
 *  Set 1 to reset audio_sample_rate_converter module
 */
#define PCR_ASRC_RST_EN    (BIT(24))
#define PCR_ASRC_RST_EN_M  (PCR_ASRC_RST_EN_V << PCR_ASRC_RST_EN_S)
#define PCR_ASRC_RST_EN_V  0x00000001U
#define PCR_ASRC_RST_EN_S  24

/** PCR_BUS_CLOCK_GATE_BYPASS_REG register
 *  bus clock gating bypass configuration register
 */
#define PCR_BUS_CLOCK_GATE_BYPASS_REG (DR_REG_PCR_BASE + 0x1ac)
/** PCR_AHB_CLK_GATING_BYPASS : R/W; bitpos: [0]; default: 1;
 *  Set 1 to bypass the clock gating for ahb bus
 */
#define PCR_AHB_CLK_GATING_BYPASS    (BIT(0))
#define PCR_AHB_CLK_GATING_BYPASS_M  (PCR_AHB_CLK_GATING_BYPASS_V << PCR_AHB_CLK_GATING_BYPASS_S)
#define PCR_AHB_CLK_GATING_BYPASS_V  0x00000001U
#define PCR_AHB_CLK_GATING_BYPASS_S  0
/** PCR_APB_CLK_GATING_BYPASS : R/W; bitpos: [1]; default: 1;
 *  Set 1 to bypass the clock gating for apb bus
 */
#define PCR_APB_CLK_GATING_BYPASS    (BIT(1))
#define PCR_APB_CLK_GATING_BYPASS_M  (PCR_APB_CLK_GATING_BYPASS_V << PCR_APB_CLK_GATING_BYPASS_S)
#define PCR_APB_CLK_GATING_BYPASS_V  0x00000001U
#define PCR_APB_CLK_GATING_BYPASS_S  1
/** PCR_AXI_CLK_GATING_BYPASS : R/W; bitpos: [2]; default: 1;
 *  Set 1 to bypass the clock gating for axi bus
 */
#define PCR_AXI_CLK_GATING_BYPASS    (BIT(2))
#define PCR_AXI_CLK_GATING_BYPASS_M  (PCR_AXI_CLK_GATING_BYPASS_V << PCR_AXI_CLK_GATING_BYPASS_S)
#define PCR_AXI_CLK_GATING_BYPASS_V  0x00000001U
#define PCR_AXI_CLK_GATING_BYPASS_S  2
/** PCR_MEM_CLK_GATING_BYPASS : R/W; bitpos: [3]; default: 1;
 *  Set 1 to bypass the clock gating for mem bus
 */
#define PCR_MEM_CLK_GATING_BYPASS    (BIT(3))
#define PCR_MEM_CLK_GATING_BYPASS_M  (PCR_MEM_CLK_GATING_BYPASS_V << PCR_MEM_CLK_GATING_BYPASS_S)
#define PCR_MEM_CLK_GATING_BYPASS_V  0x00000001U
#define PCR_MEM_CLK_GATING_BYPASS_S  3

/** PCR_ZERO_DET_CONF_REG register
 *  ZERO_DET configuration register
 */
#define PCR_ZERO_DET_CONF_REG (DR_REG_PCR_BASE + 0x1b0)
/** PCR_ZERO_DET_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable zero_det apb clock
 */
#define PCR_ZERO_DET_CLK_EN    (BIT(0))
#define PCR_ZERO_DET_CLK_EN_M  (PCR_ZERO_DET_CLK_EN_V << PCR_ZERO_DET_CLK_EN_S)
#define PCR_ZERO_DET_CLK_EN_V  0x00000001U
#define PCR_ZERO_DET_CLK_EN_S  0
/** PCR_ZERO_DET_RST_EN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to reset zero_det module
 */
#define PCR_ZERO_DET_RST_EN    (BIT(1))
#define PCR_ZERO_DET_RST_EN_M  (PCR_ZERO_DET_RST_EN_V << PCR_ZERO_DET_RST_EN_S)
#define PCR_ZERO_DET_RST_EN_V  0x00000001U
#define PCR_ZERO_DET_RST_EN_S  1

/** PCR_ZERO_DET_CLK_CONF_REG register
 *  ZERO_DET_CLK configuration register
 */
#define PCR_ZERO_DET_CLK_CONF_REG (DR_REG_PCR_BASE + 0x1b4)
/** PCR_ZERO_DET_FUNC_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of ZERO DETECT.
 *  0 (default): XTAL_CLK
 *  1: RC_FAST_CLK
 *  2: PLL_F80M_CLK
 */
#define PCR_ZERO_DET_FUNC_CLK_SEL    0x00000003U
#define PCR_ZERO_DET_FUNC_CLK_SEL_M  (PCR_ZERO_DET_FUNC_CLK_SEL_V << PCR_ZERO_DET_FUNC_CLK_SEL_S)
#define PCR_ZERO_DET_FUNC_CLK_SEL_V  0x00000003U
#define PCR_ZERO_DET_FUNC_CLK_SEL_S  20
/** PCR_ZERO_DET_FUNC_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable zero_det function clock
 */
#define PCR_ZERO_DET_FUNC_CLK_EN    (BIT(22))
#define PCR_ZERO_DET_FUNC_CLK_EN_M  (PCR_ZERO_DET_FUNC_CLK_EN_V << PCR_ZERO_DET_FUNC_CLK_EN_S)
#define PCR_ZERO_DET_FUNC_CLK_EN_V  0x00000001U
#define PCR_ZERO_DET_FUNC_CLK_EN_S  22

/** PCR_RST_EN_REG register
 *  PCR clock gating configure register
 */
#define PCR_RST_EN_REG (DR_REG_PCR_BASE + 0x1b8)
/** PCR_PCR_RST_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to reset pcr module
 */
#define PCR_PCR_RST_EN    (BIT(0))
#define PCR_PCR_RST_EN_M  (PCR_PCR_RST_EN_V << PCR_PCR_RST_EN_S)
#define PCR_PCR_RST_EN_V  0x00000001U
#define PCR_PCR_RST_EN_S  0

/** PCR_DATE_REG register
 *  Date register.
 */
#define PCR_DATE_REG (DR_REG_PCR_BASE + 0xffc)
/** PCR_DATE : R/W; bitpos: [27:0]; default: 37823024;
 *  PCR version information.
 */
#define PCR_DATE    0x0FFFFFFFU
#define PCR_DATE_M  (PCR_DATE_V << PCR_DATE_S)
#define PCR_DATE_V  0x0FFFFFFFU
#define PCR_DATE_S  0

#ifdef __cplusplus
}
#endif
