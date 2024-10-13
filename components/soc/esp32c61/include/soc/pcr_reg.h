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
 *  Set 0 to reset uart0 module
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
 *  Configures the clock source of UART0.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
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

/** PCR_UART0_PD_CTRL_REG register
 *  UART0 power control register
 */
#define PCR_UART0_PD_CTRL_REG (DR_REG_PCR_BASE + 0x8)
/** PCR_UART0_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power down UART0 memory.
 */
#define PCR_UART0_MEM_FORCE_PU    (BIT(1))
#define PCR_UART0_MEM_FORCE_PU_M  (PCR_UART0_MEM_FORCE_PU_V << PCR_UART0_MEM_FORCE_PU_S)
#define PCR_UART0_MEM_FORCE_PU_V  0x00000001U
#define PCR_UART0_MEM_FORCE_PU_S  1
/** PCR_UART0_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power up UART0 memory.
 */
#define PCR_UART0_MEM_FORCE_PD    (BIT(2))
#define PCR_UART0_MEM_FORCE_PD_M  (PCR_UART0_MEM_FORCE_PD_V << PCR_UART0_MEM_FORCE_PD_S)
#define PCR_UART0_MEM_FORCE_PD_V  0x00000001U
#define PCR_UART0_MEM_FORCE_PD_S  2

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
 *  Set 0 to reset uart1 module
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
 *  Configures the clock source of UART1.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
 */
#define PCR_UART1_SCLK_SEL    0x00000003U
#define PCR_UART1_SCLK_SEL_M  (PCR_UART1_SCLK_SEL_V << PCR_UART1_SCLK_SEL_S)
#define PCR_UART1_SCLK_SEL_V  0x00000003U
#define PCR_UART1_SCLK_SEL_S  20
/** PCR_UART1_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable uart1 function clock
 */
#define PCR_UART1_SCLK_EN    (BIT(22))
#define PCR_UART1_SCLK_EN_M  (PCR_UART1_SCLK_EN_V << PCR_UART1_SCLK_EN_S)
#define PCR_UART1_SCLK_EN_V  0x00000001U
#define PCR_UART1_SCLK_EN_S  22

/** PCR_UART1_PD_CTRL_REG register
 *  UART1 power control register
 */
#define PCR_UART1_PD_CTRL_REG (DR_REG_PCR_BASE + 0x14)
/** PCR_UART1_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power down UART1 memory.
 */
#define PCR_UART1_MEM_FORCE_PU    (BIT(1))
#define PCR_UART1_MEM_FORCE_PU_M  (PCR_UART1_MEM_FORCE_PU_V << PCR_UART1_MEM_FORCE_PU_S)
#define PCR_UART1_MEM_FORCE_PU_V  0x00000001U
#define PCR_UART1_MEM_FORCE_PU_S  1
/** PCR_UART1_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power up UART1 memory.
 */
#define PCR_UART1_MEM_FORCE_PD    (BIT(2))
#define PCR_UART1_MEM_FORCE_PD_M  (PCR_UART1_MEM_FORCE_PD_V << PCR_UART1_MEM_FORCE_PD_S)
#define PCR_UART1_MEM_FORCE_PD_V  0x00000001U
#define PCR_UART1_MEM_FORCE_PD_S  2

/** PCR_UART2_CONF_REG register
 *  UART2 configuration register
 */
#define PCR_UART2_CONF_REG (DR_REG_PCR_BASE + 0x18)
/** PCR_UART2_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable uart2 apb clock
 */
#define PCR_UART2_CLK_EN    (BIT(0))
#define PCR_UART2_CLK_EN_M  (PCR_UART2_CLK_EN_V << PCR_UART2_CLK_EN_S)
#define PCR_UART2_CLK_EN_V  0x00000001U
#define PCR_UART2_CLK_EN_S  0
/** PCR_UART2_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset uart2 module
 */
#define PCR_UART2_RST_EN    (BIT(1))
#define PCR_UART2_RST_EN_M  (PCR_UART2_RST_EN_V << PCR_UART2_RST_EN_S)
#define PCR_UART2_RST_EN_V  0x00000001U
#define PCR_UART2_RST_EN_S  1
/** PCR_UART2_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset uart2 module
 */
#define PCR_UART2_READY    (BIT(2))
#define PCR_UART2_READY_M  (PCR_UART2_READY_V << PCR_UART2_READY_S)
#define PCR_UART2_READY_V  0x00000001U
#define PCR_UART2_READY_S  2

/** PCR_UART2_SCLK_CONF_REG register
 *  UART2_SCLK configuration register
 */
#define PCR_UART2_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x1c)
/** PCR_UART2_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the uart2 function clock.
 */
#define PCR_UART2_SCLK_DIV_A    0x0000003FU
#define PCR_UART2_SCLK_DIV_A_M  (PCR_UART2_SCLK_DIV_A_V << PCR_UART2_SCLK_DIV_A_S)
#define PCR_UART2_SCLK_DIV_A_V  0x0000003FU
#define PCR_UART2_SCLK_DIV_A_S  0
/** PCR_UART2_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the uart2 function clock.
 */
#define PCR_UART2_SCLK_DIV_B    0x0000003FU
#define PCR_UART2_SCLK_DIV_B_M  (PCR_UART2_SCLK_DIV_B_V << PCR_UART2_SCLK_DIV_B_S)
#define PCR_UART2_SCLK_DIV_B_V  0x0000003FU
#define PCR_UART2_SCLK_DIV_B_S  6
/** PCR_UART2_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the uart2 function clock.
 */
#define PCR_UART2_SCLK_DIV_NUM    0x000000FFU
#define PCR_UART2_SCLK_DIV_NUM_M  (PCR_UART2_SCLK_DIV_NUM_V << PCR_UART2_SCLK_DIV_NUM_S)
#define PCR_UART2_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_UART2_SCLK_DIV_NUM_S  12
/** PCR_UART2_SCLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of UART2.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
 */
#define PCR_UART2_SCLK_SEL    0x00000003U
#define PCR_UART2_SCLK_SEL_M  (PCR_UART2_SCLK_SEL_V << PCR_UART2_SCLK_SEL_S)
#define PCR_UART2_SCLK_SEL_V  0x00000003U
#define PCR_UART2_SCLK_SEL_S  20
/** PCR_UART2_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable uart2 function clock
 */
#define PCR_UART2_SCLK_EN    (BIT(22))
#define PCR_UART2_SCLK_EN_M  (PCR_UART2_SCLK_EN_V << PCR_UART2_SCLK_EN_S)
#define PCR_UART2_SCLK_EN_V  0x00000001U
#define PCR_UART2_SCLK_EN_S  22

/** PCR_UART2_PD_CTRL_REG register
 *  UART2 power control register
 */
#define PCR_UART2_PD_CTRL_REG (DR_REG_PCR_BASE + 0x20)
/** PCR_UART2_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power down UART2 memory.
 */
#define PCR_UART2_MEM_FORCE_PU    (BIT(1))
#define PCR_UART2_MEM_FORCE_PU_M  (PCR_UART2_MEM_FORCE_PU_V << PCR_UART2_MEM_FORCE_PU_S)
#define PCR_UART2_MEM_FORCE_PU_V  0x00000001U
#define PCR_UART2_MEM_FORCE_PU_S  1
/** PCR_UART2_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power up UART2 memory.
 */
#define PCR_UART2_MEM_FORCE_PD    (BIT(2))
#define PCR_UART2_MEM_FORCE_PD_M  (PCR_UART2_MEM_FORCE_PD_V << PCR_UART2_MEM_FORCE_PD_S)
#define PCR_UART2_MEM_FORCE_PD_V  0x00000001U
#define PCR_UART2_MEM_FORCE_PD_S  2

/** PCR_MSPI_CONF_REG register
 *  MSPI configuration register
 */
#define PCR_MSPI_CONF_REG (DR_REG_PCR_BASE + 0x24)
/** PCR_MSPI_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable mspi apb clock and mspi pll clock
 */
#define PCR_MSPI_CLK_EN    (BIT(0))
#define PCR_MSPI_CLK_EN_M  (PCR_MSPI_CLK_EN_V << PCR_MSPI_CLK_EN_S)
#define PCR_MSPI_CLK_EN_V  0x00000001U
#define PCR_MSPI_CLK_EN_S  0
/** PCR_MSPI_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset mspi module
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
#define PCR_MSPI_CLK_CONF_REG (DR_REG_PCR_BASE + 0x28)
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
 *  Configures the clock source for MSPI.\\
 *  0(default): XTAL_CLK\\
 *  1 RC_FAST_CLK\\
 *  2: PLL_F480M_CLK\\
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
 *  Set 0 to reset axi_clock domain of mspi module
 */
#define PCR_MSPI_AXI_RST_EN    (BIT(11))
#define PCR_MSPI_AXI_RST_EN_M  (PCR_MSPI_AXI_RST_EN_V << PCR_MSPI_AXI_RST_EN_S)
#define PCR_MSPI_AXI_RST_EN_V  0x00000001U
#define PCR_MSPI_AXI_RST_EN_S  11

/** PCR_I2C_CONF_REG register
 *  I2C configuration register
 */
#define PCR_I2C_CONF_REG (DR_REG_PCR_BASE + 0x2c)
/** PCR_I2C_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable i2c apb clock
 */
#define PCR_I2C_CLK_EN    (BIT(0))
#define PCR_I2C_CLK_EN_M  (PCR_I2C_CLK_EN_V << PCR_I2C_CLK_EN_S)
#define PCR_I2C_CLK_EN_V  0x00000001U
#define PCR_I2C_CLK_EN_S  0
/** PCR_I2C_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset i2c module
 */
#define PCR_I2C_RST_EN    (BIT(1))
#define PCR_I2C_RST_EN_M  (PCR_I2C_RST_EN_V << PCR_I2C_RST_EN_S)
#define PCR_I2C_RST_EN_V  0x00000001U
#define PCR_I2C_RST_EN_S  1

/** PCR_I2C_SCLK_CONF_REG register
 *  I2C_SCLK configuration register
 */
#define PCR_I2C_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x30)
/** PCR_I2C_SCLK_DIV_A : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor of the i2c function clock.
 */
#define PCR_I2C_SCLK_DIV_A    0x0000003FU
#define PCR_I2C_SCLK_DIV_A_M  (PCR_I2C_SCLK_DIV_A_V << PCR_I2C_SCLK_DIV_A_S)
#define PCR_I2C_SCLK_DIV_A_V  0x0000003FU
#define PCR_I2C_SCLK_DIV_A_S  0
/** PCR_I2C_SCLK_DIV_B : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor of the i2c function clock.
 */
#define PCR_I2C_SCLK_DIV_B    0x0000003FU
#define PCR_I2C_SCLK_DIV_B_M  (PCR_I2C_SCLK_DIV_B_V << PCR_I2C_SCLK_DIV_B_S)
#define PCR_I2C_SCLK_DIV_B_V  0x0000003FU
#define PCR_I2C_SCLK_DIV_B_S  6
/** PCR_I2C_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the i2c function clock.
 */
#define PCR_I2C_SCLK_DIV_NUM    0x000000FFU
#define PCR_I2C_SCLK_DIV_NUM_M  (PCR_I2C_SCLK_DIV_NUM_V << PCR_I2C_SCLK_DIV_NUM_S)
#define PCR_I2C_SCLK_DIV_NUM_V  0x000000FFU
#define PCR_I2C_SCLK_DIV_NUM_S  12
/** PCR_I2C_SCLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of I2C.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 */
#define PCR_I2C_SCLK_SEL    (BIT(20))
#define PCR_I2C_SCLK_SEL_M  (PCR_I2C_SCLK_SEL_V << PCR_I2C_SCLK_SEL_S)
#define PCR_I2C_SCLK_SEL_V  0x00000001U
#define PCR_I2C_SCLK_SEL_S  20
/** PCR_I2C_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable i2c function clock
 */
#define PCR_I2C_SCLK_EN    (BIT(22))
#define PCR_I2C_SCLK_EN_M  (PCR_I2C_SCLK_EN_V << PCR_I2C_SCLK_EN_S)
#define PCR_I2C_SCLK_EN_V  0x00000001U
#define PCR_I2C_SCLK_EN_S  22

/** PCR_LEDC_CONF_REG register
 *  LEDC configuration register
 */
#define PCR_LEDC_CONF_REG (DR_REG_PCR_BASE + 0x34)
/** PCR_LEDC_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable ledc apb clock
 */
#define PCR_LEDC_CLK_EN    (BIT(0))
#define PCR_LEDC_CLK_EN_M  (PCR_LEDC_CLK_EN_V << PCR_LEDC_CLK_EN_S)
#define PCR_LEDC_CLK_EN_V  0x00000001U
#define PCR_LEDC_CLK_EN_S  0
/** PCR_LEDC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset ledc module
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
#define PCR_LEDC_SCLK_CONF_REG (DR_REG_PCR_BASE + 0x38)
/** PCR_LEDC_SCLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of LEDC.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
 */
#define PCR_LEDC_SCLK_SEL    0x00000003U
#define PCR_LEDC_SCLK_SEL_M  (PCR_LEDC_SCLK_SEL_V << PCR_LEDC_SCLK_SEL_S)
#define PCR_LEDC_SCLK_SEL_V  0x00000003U
#define PCR_LEDC_SCLK_SEL_S  20
/** PCR_LEDC_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable ledc function clock
 */
#define PCR_LEDC_SCLK_EN    (BIT(22))
#define PCR_LEDC_SCLK_EN_M  (PCR_LEDC_SCLK_EN_V << PCR_LEDC_SCLK_EN_S)
#define PCR_LEDC_SCLK_EN_V  0x00000001U
#define PCR_LEDC_SCLK_EN_S  22

/** PCR_LEDC_PD_CTRL_REG register
 *  LEDC power control register
 */
#define PCR_LEDC_PD_CTRL_REG (DR_REG_PCR_BASE + 0x3c)
/** PCR_LEDC_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power down LEDC memory.
 */
#define PCR_LEDC_MEM_FORCE_PU    (BIT(1))
#define PCR_LEDC_MEM_FORCE_PU_M  (PCR_LEDC_MEM_FORCE_PU_V << PCR_LEDC_MEM_FORCE_PU_S)
#define PCR_LEDC_MEM_FORCE_PU_V  0x00000001U
#define PCR_LEDC_MEM_FORCE_PU_S  1
/** PCR_LEDC_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power up LEDC memory.
 */
#define PCR_LEDC_MEM_FORCE_PD    (BIT(2))
#define PCR_LEDC_MEM_FORCE_PD_M  (PCR_LEDC_MEM_FORCE_PD_V << PCR_LEDC_MEM_FORCE_PD_S)
#define PCR_LEDC_MEM_FORCE_PD_V  0x00000001U
#define PCR_LEDC_MEM_FORCE_PD_S  2

/** PCR_TIMERGROUP0_CONF_REG register
 *  TIMERGROUP0 configuration register
 */
#define PCR_TIMERGROUP0_CONF_REG (DR_REG_PCR_BASE + 0x40)
/** PCR_TG0_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable timer_group0 apb clock
 */
#define PCR_TG0_CLK_EN    (BIT(0))
#define PCR_TG0_CLK_EN_M  (PCR_TG0_CLK_EN_V << PCR_TG0_CLK_EN_S)
#define PCR_TG0_CLK_EN_V  0x00000001U
#define PCR_TG0_CLK_EN_S  0
/** PCR_TG0_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset timer_group0 module
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
 *  reserved
 */
#define PCR_TG0_TIMER1_READY    (BIT(4))
#define PCR_TG0_TIMER1_READY_M  (PCR_TG0_TIMER1_READY_V << PCR_TG0_TIMER1_READY_S)
#define PCR_TG0_TIMER1_READY_V  0x00000001U
#define PCR_TG0_TIMER1_READY_S  4

/** PCR_TIMERGROUP0_TIMER_CLK_CONF_REG register
 *  TIMERGROUP0_TIMER_CLK configuration register
 */
#define PCR_TIMERGROUP0_TIMER_CLK_CONF_REG (DR_REG_PCR_BASE + 0x44)
/** PCR_TG0_TIMER_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of general-purpose timers in Timer Group 0.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
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
#define PCR_TIMERGROUP0_WDT_CLK_CONF_REG (DR_REG_PCR_BASE + 0x48)
/** PCR_TG0_WDT_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of WDT in Timer Group 0.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
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
#define PCR_TIMERGROUP1_CONF_REG (DR_REG_PCR_BASE + 0x4c)
/** PCR_TG1_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable timer_group1 apb clock
 */
#define PCR_TG1_CLK_EN    (BIT(0))
#define PCR_TG1_CLK_EN_M  (PCR_TG1_CLK_EN_V << PCR_TG1_CLK_EN_S)
#define PCR_TG1_CLK_EN_V  0x00000001U
#define PCR_TG1_CLK_EN_S  0
/** PCR_TG1_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset timer_group1 module
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
 *  reserved
 */
#define PCR_TG1_TIMER1_READY    (BIT(4))
#define PCR_TG1_TIMER1_READY_M  (PCR_TG1_TIMER1_READY_V << PCR_TG1_TIMER1_READY_S)
#define PCR_TG1_TIMER1_READY_V  0x00000001U
#define PCR_TG1_TIMER1_READY_S  4

/** PCR_TIMERGROUP1_TIMER_CLK_CONF_REG register
 *  TIMERGROUP1_TIMER_CLK configuration register
 */
#define PCR_TIMERGROUP1_TIMER_CLK_CONF_REG (DR_REG_PCR_BASE + 0x50)
/** PCR_TG1_TIMER_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of general-purpose timers in Timer Group 1.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
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
#define PCR_TIMERGROUP1_WDT_CLK_CONF_REG (DR_REG_PCR_BASE + 0x54)
/** PCR_TG1_WDT_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of WDT in Timer Group 1.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
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
#define PCR_SYSTIMER_CONF_REG (DR_REG_PCR_BASE + 0x58)
/** PCR_SYSTIMER_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable systimer apb clock
 */
#define PCR_SYSTIMER_CLK_EN    (BIT(0))
#define PCR_SYSTIMER_CLK_EN_M  (PCR_SYSTIMER_CLK_EN_V << PCR_SYSTIMER_CLK_EN_S)
#define PCR_SYSTIMER_CLK_EN_V  0x00000001U
#define PCR_SYSTIMER_CLK_EN_S  0
/** PCR_SYSTIMER_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset systimer module
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
#define PCR_SYSTIMER_FUNC_CLK_CONF_REG (DR_REG_PCR_BASE + 0x5c)
/** PCR_SYSTIMER_FUNC_CLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of System Timer.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
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
#define PCR_I2S_CONF_REG (DR_REG_PCR_BASE + 0x60)
/** PCR_I2S_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable i2s apb clock
 */
#define PCR_I2S_CLK_EN    (BIT(0))
#define PCR_I2S_CLK_EN_M  (PCR_I2S_CLK_EN_V << PCR_I2S_CLK_EN_S)
#define PCR_I2S_CLK_EN_V  0x00000001U
#define PCR_I2S_CLK_EN_S  0
/** PCR_I2S_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset i2s module
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
#define PCR_I2S_TX_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x64)
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
 *  Configures the clock source of I2S TX.\\
 *  0 (default): XTAL_CLK\\
 *  1: PLL_F120M_CLK\\
 *  2: PLL_F160M_CLK\\
 *  3: I2S_MCLK_in\\
 */
#define PCR_I2S_TX_CLKM_SEL    0x00000003U
#define PCR_I2S_TX_CLKM_SEL_M  (PCR_I2S_TX_CLKM_SEL_V << PCR_I2S_TX_CLKM_SEL_S)
#define PCR_I2S_TX_CLKM_SEL_V  0x00000003U
#define PCR_I2S_TX_CLKM_SEL_S  20
/** PCR_I2S_TX_CLKM_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable i2s_tx function clock
 */
#define PCR_I2S_TX_CLKM_EN    (BIT(22))
#define PCR_I2S_TX_CLKM_EN_M  (PCR_I2S_TX_CLKM_EN_V << PCR_I2S_TX_CLKM_EN_S)
#define PCR_I2S_TX_CLKM_EN_V  0x00000001U
#define PCR_I2S_TX_CLKM_EN_S  22

/** PCR_I2S_TX_CLKM_DIV_CONF_REG register
 *  I2S_TX_CLKM_DIV configuration register
 */
#define PCR_I2S_TX_CLKM_DIV_CONF_REG (DR_REG_PCR_BASE + 0x68)
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
#define PCR_I2S_RX_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x6c)
/** PCR_I2S_RX_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 2;
 *  Integral I2S clock divider value
 */
#define PCR_I2S_RX_CLKM_DIV_NUM    0x000000FFU
#define PCR_I2S_RX_CLKM_DIV_NUM_M  (PCR_I2S_RX_CLKM_DIV_NUM_V << PCR_I2S_RX_CLKM_DIV_NUM_S)
#define PCR_I2S_RX_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_I2S_RX_CLKM_DIV_NUM_S  12
/** PCR_I2S_RX_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of I2S RX.\\
 *  0 (default): XTAL_CLK\\
 *  1: PLL_F120M_CLK\\
 *  2: PLL_F160M_CLK\\
 *  3: I2S_MCLK_in\\
 */
#define PCR_I2S_RX_CLKM_SEL    0x00000003U
#define PCR_I2S_RX_CLKM_SEL_M  (PCR_I2S_RX_CLKM_SEL_V << PCR_I2S_RX_CLKM_SEL_S)
#define PCR_I2S_RX_CLKM_SEL_V  0x00000003U
#define PCR_I2S_RX_CLKM_SEL_S  20
/** PCR_I2S_RX_CLKM_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable i2s_rx function clock
 */
#define PCR_I2S_RX_CLKM_EN    (BIT(22))
#define PCR_I2S_RX_CLKM_EN_M  (PCR_I2S_RX_CLKM_EN_V << PCR_I2S_RX_CLKM_EN_S)
#define PCR_I2S_RX_CLKM_EN_V  0x00000001U
#define PCR_I2S_RX_CLKM_EN_S  22
/** PCR_I2S_MCLK_SEL : R/W; bitpos: [23]; default: 0;
 *  Configures to select master clock.\\
 *  0 (default): I2S_TX_CLK\\
 *  1: I2S_RX_CLK\\
 */
#define PCR_I2S_MCLK_SEL    (BIT(23))
#define PCR_I2S_MCLK_SEL_M  (PCR_I2S_MCLK_SEL_V << PCR_I2S_MCLK_SEL_S)
#define PCR_I2S_MCLK_SEL_V  0x00000001U
#define PCR_I2S_MCLK_SEL_S  23

/** PCR_I2S_RX_CLKM_DIV_CONF_REG register
 *  I2S_RX_CLKM_DIV configuration register
 */
#define PCR_I2S_RX_CLKM_DIV_CONF_REG (DR_REG_PCR_BASE + 0x70)
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
#define PCR_SARADC_CONF_REG (DR_REG_PCR_BASE + 0x74)
/** PCR_SARADC_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  no use
 */
#define PCR_SARADC_CLK_EN    (BIT(0))
#define PCR_SARADC_CLK_EN_M  (PCR_SARADC_CLK_EN_V << PCR_SARADC_CLK_EN_S)
#define PCR_SARADC_CLK_EN_V  0x00000001U
#define PCR_SARADC_CLK_EN_S  0
/** PCR_SARADC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset function_register of saradc module
 */
#define PCR_SARADC_RST_EN    (BIT(1))
#define PCR_SARADC_RST_EN_M  (PCR_SARADC_RST_EN_V << PCR_SARADC_RST_EN_S)
#define PCR_SARADC_RST_EN_V  0x00000001U
#define PCR_SARADC_RST_EN_S  1
/** PCR_SARADC_REG_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable saradc apb clock
 */
#define PCR_SARADC_REG_CLK_EN    (BIT(2))
#define PCR_SARADC_REG_CLK_EN_M  (PCR_SARADC_REG_CLK_EN_V << PCR_SARADC_REG_CLK_EN_S)
#define PCR_SARADC_REG_CLK_EN_V  0x00000001U
#define PCR_SARADC_REG_CLK_EN_S  2
/** PCR_SARADC_REG_RST_EN : R/W; bitpos: [3]; default: 0;
 *  Set 0 to reset apb_register of saradc module
 */
#define PCR_SARADC_REG_RST_EN    (BIT(3))
#define PCR_SARADC_REG_RST_EN_M  (PCR_SARADC_REG_RST_EN_V << PCR_SARADC_REG_RST_EN_S)
#define PCR_SARADC_REG_RST_EN_V  0x00000001U
#define PCR_SARADC_REG_RST_EN_S  3

/** PCR_SARADC_CLKM_CONF_REG register
 *  SARADC_CLKM configuration register
 */
#define PCR_SARADC_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x78)
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
/** PCR_SARADC_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 4;
 *  The integral part of the frequency divider factor of the saradc function clock.
 */
#define PCR_SARADC_CLKM_DIV_NUM    0x000000FFU
#define PCR_SARADC_CLKM_DIV_NUM_M  (PCR_SARADC_CLKM_DIV_NUM_V << PCR_SARADC_CLKM_DIV_NUM_S)
#define PCR_SARADC_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_SARADC_CLKM_DIV_NUM_S  12
/** PCR_SARADC_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of SAR ADC.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
 */
#define PCR_SARADC_CLKM_SEL    0x00000003U
#define PCR_SARADC_CLKM_SEL_M  (PCR_SARADC_CLKM_SEL_V << PCR_SARADC_CLKM_SEL_S)
#define PCR_SARADC_CLKM_SEL_V  0x00000003U
#define PCR_SARADC_CLKM_SEL_S  20
/** PCR_SARADC_CLKM_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable saradc function clock
 */
#define PCR_SARADC_CLKM_EN    (BIT(22))
#define PCR_SARADC_CLKM_EN_M  (PCR_SARADC_CLKM_EN_V << PCR_SARADC_CLKM_EN_S)
#define PCR_SARADC_CLKM_EN_V  0x00000001U
#define PCR_SARADC_CLKM_EN_S  22

/** PCR_TSENS_CLK_CONF_REG register
 *  TSENS_CLK configuration register
 */
#define PCR_TSENS_CLK_CONF_REG (DR_REG_PCR_BASE + 0x7c)
/** PCR_TSENS_CLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of the temperature sensor.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 */
#define PCR_TSENS_CLK_SEL    (BIT(20))
#define PCR_TSENS_CLK_SEL_M  (PCR_TSENS_CLK_SEL_V << PCR_TSENS_CLK_SEL_S)
#define PCR_TSENS_CLK_SEL_V  0x00000001U
#define PCR_TSENS_CLK_SEL_S  20
/** PCR_TSENS_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable tsens clock
 */
#define PCR_TSENS_CLK_EN    (BIT(22))
#define PCR_TSENS_CLK_EN_M  (PCR_TSENS_CLK_EN_V << PCR_TSENS_CLK_EN_S)
#define PCR_TSENS_CLK_EN_V  0x00000001U
#define PCR_TSENS_CLK_EN_S  22
/** PCR_TSENS_RST_EN : R/W; bitpos: [23]; default: 0;
 *  Set 0 to reset tsens module
 */
#define PCR_TSENS_RST_EN    (BIT(23))
#define PCR_TSENS_RST_EN_M  (PCR_TSENS_RST_EN_V << PCR_TSENS_RST_EN_S)
#define PCR_TSENS_RST_EN_V  0x00000001U
#define PCR_TSENS_RST_EN_S  23

/** PCR_USB_DEVICE_CONF_REG register
 *  USB_DEVICE configuration register
 */
#define PCR_USB_DEVICE_CONF_REG (DR_REG_PCR_BASE + 0x80)
/** PCR_USB_DEVICE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable usb_device clock
 */
#define PCR_USB_DEVICE_CLK_EN    (BIT(0))
#define PCR_USB_DEVICE_CLK_EN_M  (PCR_USB_DEVICE_CLK_EN_V << PCR_USB_DEVICE_CLK_EN_S)
#define PCR_USB_DEVICE_CLK_EN_V  0x00000001U
#define PCR_USB_DEVICE_CLK_EN_S  0
/** PCR_USB_DEVICE_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset usb_device module
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

/** PCR_INTMTX_CONF_REG register
 *  INTMTX configuration register
 */
#define PCR_INTMTX_CONF_REG (DR_REG_PCR_BASE + 0x84)
/** PCR_INTMTX_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable intmtx clock
 */
#define PCR_INTMTX_CLK_EN    (BIT(0))
#define PCR_INTMTX_CLK_EN_M  (PCR_INTMTX_CLK_EN_V << PCR_INTMTX_CLK_EN_S)
#define PCR_INTMTX_CLK_EN_V  0x00000001U
#define PCR_INTMTX_CLK_EN_S  0
/** PCR_INTMTX_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset intmtx module
 */
#define PCR_INTMTX_RST_EN    (BIT(1))
#define PCR_INTMTX_RST_EN_M  (PCR_INTMTX_RST_EN_V << PCR_INTMTX_RST_EN_S)
#define PCR_INTMTX_RST_EN_V  0x00000001U
#define PCR_INTMTX_RST_EN_S  1
/** PCR_INTMTX_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset intmtx module
 */
#define PCR_INTMTX_READY    (BIT(2))
#define PCR_INTMTX_READY_M  (PCR_INTMTX_READY_V << PCR_INTMTX_READY_S)
#define PCR_INTMTX_READY_V  0x00000001U
#define PCR_INTMTX_READY_S  2

/** PCR_PVT_MONITOR_CONF_REG register
 *  PVT_MONITOR configuration register
 */
#define PCR_PVT_MONITOR_CONF_REG (DR_REG_PCR_BASE + 0x88)
/** PCR_PVT_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 1;
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
/** PCR_PVT_MONITOR_SITE2_CLK_EN : R/W; bitpos: [3]; default: 1;
 *  Set 1 to enable function clock of cpu pvt module
 */
#define PCR_PVT_MONITOR_SITE2_CLK_EN    (BIT(3))
#define PCR_PVT_MONITOR_SITE2_CLK_EN_M  (PCR_PVT_MONITOR_SITE2_CLK_EN_V << PCR_PVT_MONITOR_SITE2_CLK_EN_S)
#define PCR_PVT_MONITOR_SITE2_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_SITE2_CLK_EN_S  3
/** PCR_PVT_MONITOR_SITE2_RST_EN : R/W; bitpos: [4]; default: 0;
 *  Set 1 to reset cpu pvt monitor site
 */
#define PCR_PVT_MONITOR_SITE2_RST_EN    (BIT(4))
#define PCR_PVT_MONITOR_SITE2_RST_EN_M  (PCR_PVT_MONITOR_SITE2_RST_EN_V << PCR_PVT_MONITOR_SITE2_RST_EN_S)
#define PCR_PVT_MONITOR_SITE2_RST_EN_V  0x00000001U
#define PCR_PVT_MONITOR_SITE2_RST_EN_S  4
/** PCR_PVT_MONITOR_SITE3_CLK_EN : R/W; bitpos: [5]; default: 1;
 *  Set 1 to enable function clock of modem pvt module
 */
#define PCR_PVT_MONITOR_SITE3_CLK_EN    (BIT(5))
#define PCR_PVT_MONITOR_SITE3_CLK_EN_M  (PCR_PVT_MONITOR_SITE3_CLK_EN_V << PCR_PVT_MONITOR_SITE3_CLK_EN_S)
#define PCR_PVT_MONITOR_SITE3_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_SITE3_CLK_EN_S  5

/** PCR_PVT_MONITOR_FUNC_CLK_CONF_REG register
 *  PVT_MONITOR function clock configuration register
 */
#define PCR_PVT_MONITOR_FUNC_CLK_CONF_REG (DR_REG_PCR_BASE + 0x8c)
/** PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM : R/W; bitpos: [3:0]; default: 0;
 *  The integral part of the frequency divider factor of the pvt_monitor function clock.
 */
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM    0x0000000FU
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_M  (PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_V << PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S)
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_V  0x0000000FU
#define PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S  0
/** PCR_PVT_MONITOR_FUNC_CLK_SEL : R/W; bitpos: [20]; default: 0;
 *  Configures the clock source of PVT MONITOR.\\
 *  0 (default): XTAL_CLK\\
 *  1: PLL_F160M_CLK\\
 */
#define PCR_PVT_MONITOR_FUNC_CLK_SEL    (BIT(20))
#define PCR_PVT_MONITOR_FUNC_CLK_SEL_M  (PCR_PVT_MONITOR_FUNC_CLK_SEL_V << PCR_PVT_MONITOR_FUNC_CLK_SEL_S)
#define PCR_PVT_MONITOR_FUNC_CLK_SEL_V  0x00000001U
#define PCR_PVT_MONITOR_FUNC_CLK_SEL_S  20
/** PCR_PVT_MONITOR_FUNC_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set 1 to enable source clock of pvt sitex
 */
#define PCR_PVT_MONITOR_FUNC_CLK_EN    (BIT(22))
#define PCR_PVT_MONITOR_FUNC_CLK_EN_M  (PCR_PVT_MONITOR_FUNC_CLK_EN_V << PCR_PVT_MONITOR_FUNC_CLK_EN_S)
#define PCR_PVT_MONITOR_FUNC_CLK_EN_V  0x00000001U
#define PCR_PVT_MONITOR_FUNC_CLK_EN_S  22

/** PCR_GDMA_CONF_REG register
 *  GDMA configuration register
 */
#define PCR_GDMA_CONF_REG (DR_REG_PCR_BASE + 0x90)
/** PCR_GDMA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable gdma clock
 */
#define PCR_GDMA_CLK_EN    (BIT(0))
#define PCR_GDMA_CLK_EN_M  (PCR_GDMA_CLK_EN_V << PCR_GDMA_CLK_EN_S)
#define PCR_GDMA_CLK_EN_V  0x00000001U
#define PCR_GDMA_CLK_EN_S  0
/** PCR_GDMA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset gdma module
 */
#define PCR_GDMA_RST_EN    (BIT(1))
#define PCR_GDMA_RST_EN_M  (PCR_GDMA_RST_EN_V << PCR_GDMA_RST_EN_S)
#define PCR_GDMA_RST_EN_V  0x00000001U
#define PCR_GDMA_RST_EN_S  1

/** PCR_SPI2_CONF_REG register
 *  SPI2 configuration register
 */
#define PCR_SPI2_CONF_REG (DR_REG_PCR_BASE + 0x94)
/** PCR_SPI2_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable spi2 apb clock
 */
#define PCR_SPI2_CLK_EN    (BIT(0))
#define PCR_SPI2_CLK_EN_M  (PCR_SPI2_CLK_EN_V << PCR_SPI2_CLK_EN_S)
#define PCR_SPI2_CLK_EN_V  0x00000001U
#define PCR_SPI2_CLK_EN_S  0
/** PCR_SPI2_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset spi2 module
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
#define PCR_SPI2_CLKM_CONF_REG (DR_REG_PCR_BASE + 0x98)
/** PCR_SPI2_CLKM_DIV_NUM : R/W; bitpos: [19:12]; default: 0;
 *  The integral part of the frequency divider factor of the spi2_mst clock.
 */
#define PCR_SPI2_CLKM_DIV_NUM    0x000000FFU
#define PCR_SPI2_CLKM_DIV_NUM_M  (PCR_SPI2_CLKM_DIV_NUM_V << PCR_SPI2_CLKM_DIV_NUM_S)
#define PCR_SPI2_CLKM_DIV_NUM_V  0x000000FFU
#define PCR_SPI2_CLKM_DIV_NUM_S  12
/** PCR_SPI2_CLKM_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of SPI2.\\
 *  0 (default): XTAL_CLK\\
 *  1: PLL_F160M_CLK\\
 *  2: RC_FAST_CLK\\
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
#define PCR_AES_CONF_REG (DR_REG_PCR_BASE + 0x9c)
/** PCR_AES_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable aes clock
 */
#define PCR_AES_CLK_EN    (BIT(0))
#define PCR_AES_CLK_EN_M  (PCR_AES_CLK_EN_V << PCR_AES_CLK_EN_S)
#define PCR_AES_CLK_EN_V  0x00000001U
#define PCR_AES_CLK_EN_S  0
/** PCR_AES_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset aes module
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
#define PCR_SHA_CONF_REG (DR_REG_PCR_BASE + 0xa0)
/** PCR_SHA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable sha clock
 */
#define PCR_SHA_CLK_EN    (BIT(0))
#define PCR_SHA_CLK_EN_M  (PCR_SHA_CLK_EN_V << PCR_SHA_CLK_EN_S)
#define PCR_SHA_CLK_EN_V  0x00000001U
#define PCR_SHA_CLK_EN_S  0
/** PCR_SHA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset sha module
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

/** PCR_RSA_CONF_REG register
 *  RSA configuration register
 */
#define PCR_RSA_CONF_REG (DR_REG_PCR_BASE + 0xa4)
/** PCR_RSA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable rsa clock
 */
#define PCR_RSA_CLK_EN    (BIT(0))
#define PCR_RSA_CLK_EN_M  (PCR_RSA_CLK_EN_V << PCR_RSA_CLK_EN_S)
#define PCR_RSA_CLK_EN_V  0x00000001U
#define PCR_RSA_CLK_EN_S  0
/** PCR_RSA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset rsa module
 */
#define PCR_RSA_RST_EN    (BIT(1))
#define PCR_RSA_RST_EN_M  (PCR_RSA_RST_EN_V << PCR_RSA_RST_EN_S)
#define PCR_RSA_RST_EN_V  0x00000001U
#define PCR_RSA_RST_EN_S  1
/** PCR_RSA_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset rsa module
 */
#define PCR_RSA_READY    (BIT(2))
#define PCR_RSA_READY_M  (PCR_RSA_READY_V << PCR_RSA_READY_S)
#define PCR_RSA_READY_V  0x00000001U
#define PCR_RSA_READY_S  2

/** PCR_RSA_PD_CTRL_REG register
 *  RSA power control register
 */
#define PCR_RSA_PD_CTRL_REG (DR_REG_PCR_BASE + 0xa8)
/** PCR_RSA_MEM_PD : R/W; bitpos: [0]; default: 0;
 *  Set this bit to power down rsa internal memory.
 */
#define PCR_RSA_MEM_PD    (BIT(0))
#define PCR_RSA_MEM_PD_M  (PCR_RSA_MEM_PD_V << PCR_RSA_MEM_PD_S)
#define PCR_RSA_MEM_PD_V  0x00000001U
#define PCR_RSA_MEM_PD_S  0
/** PCR_RSA_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power up rsa internal memory
 */
#define PCR_RSA_MEM_FORCE_PU    (BIT(1))
#define PCR_RSA_MEM_FORCE_PU_M  (PCR_RSA_MEM_FORCE_PU_V << PCR_RSA_MEM_FORCE_PU_S)
#define PCR_RSA_MEM_FORCE_PU_V  0x00000001U
#define PCR_RSA_MEM_FORCE_PU_S  1
/** PCR_RSA_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power down rsa internal memory.
 */
#define PCR_RSA_MEM_FORCE_PD    (BIT(2))
#define PCR_RSA_MEM_FORCE_PD_M  (PCR_RSA_MEM_FORCE_PD_V << PCR_RSA_MEM_FORCE_PD_S)
#define PCR_RSA_MEM_FORCE_PD_V  0x00000001U
#define PCR_RSA_MEM_FORCE_PD_S  2

/** PCR_ECC_CONF_REG register
 *  ECC configuration register
 */
#define PCR_ECC_CONF_REG (DR_REG_PCR_BASE + 0xac)
/** PCR_ECC_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable ecc clock
 */
#define PCR_ECC_CLK_EN    (BIT(0))
#define PCR_ECC_CLK_EN_M  (PCR_ECC_CLK_EN_V << PCR_ECC_CLK_EN_S)
#define PCR_ECC_CLK_EN_V  0x00000001U
#define PCR_ECC_CLK_EN_S  0
/** PCR_ECC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset ecc module
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

/** PCR_ECC_PD_CTRL_REG register
 *  ECC power control register
 */
#define PCR_ECC_PD_CTRL_REG (DR_REG_PCR_BASE + 0xb0)
/** PCR_ECC_MEM_PD : R/W; bitpos: [0]; default: 0;
 *  Set this bit to power down ecc internal memory.
 */
#define PCR_ECC_MEM_PD    (BIT(0))
#define PCR_ECC_MEM_PD_M  (PCR_ECC_MEM_PD_V << PCR_ECC_MEM_PD_S)
#define PCR_ECC_MEM_PD_V  0x00000001U
#define PCR_ECC_MEM_PD_S  0
/** PCR_ECC_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power up ecc internal memory
 */
#define PCR_ECC_MEM_FORCE_PU    (BIT(1))
#define PCR_ECC_MEM_FORCE_PU_M  (PCR_ECC_MEM_FORCE_PU_V << PCR_ECC_MEM_FORCE_PU_S)
#define PCR_ECC_MEM_FORCE_PU_V  0x00000001U
#define PCR_ECC_MEM_FORCE_PU_S  1
/** PCR_ECC_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power down ecc internal memory.
 */
#define PCR_ECC_MEM_FORCE_PD    (BIT(2))
#define PCR_ECC_MEM_FORCE_PD_M  (PCR_ECC_MEM_FORCE_PD_V << PCR_ECC_MEM_FORCE_PD_S)
#define PCR_ECC_MEM_FORCE_PD_V  0x00000001U
#define PCR_ECC_MEM_FORCE_PD_S  2

/** PCR_DS_CONF_REG register
 *  DS configuration register
 */
#define PCR_DS_CONF_REG (DR_REG_PCR_BASE + 0xb4)
/** PCR_DS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable ds clock
 */
#define PCR_DS_CLK_EN    (BIT(0))
#define PCR_DS_CLK_EN_M  (PCR_DS_CLK_EN_V << PCR_DS_CLK_EN_S)
#define PCR_DS_CLK_EN_V  0x00000001U
#define PCR_DS_CLK_EN_S  0
/** PCR_DS_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset ds module
 */
#define PCR_DS_RST_EN    (BIT(1))
#define PCR_DS_RST_EN_M  (PCR_DS_RST_EN_V << PCR_DS_RST_EN_S)
#define PCR_DS_RST_EN_V  0x00000001U
#define PCR_DS_RST_EN_S  1
/** PCR_DS_READY : RO; bitpos: [2]; default: 1;
 *  Query this field after reset ds module
 */
#define PCR_DS_READY    (BIT(2))
#define PCR_DS_READY_M  (PCR_DS_READY_V << PCR_DS_READY_S)
#define PCR_DS_READY_V  0x00000001U
#define PCR_DS_READY_S  2

/** PCR_HMAC_CONF_REG register
 *  HMAC configuration register
 */
#define PCR_HMAC_CONF_REG (DR_REG_PCR_BASE + 0xb8)
/** PCR_HMAC_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable hmac clock
 */
#define PCR_HMAC_CLK_EN    (BIT(0))
#define PCR_HMAC_CLK_EN_M  (PCR_HMAC_CLK_EN_V << PCR_HMAC_CLK_EN_S)
#define PCR_HMAC_CLK_EN_V  0x00000001U
#define PCR_HMAC_CLK_EN_S  0
/** PCR_HMAC_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset hmac module
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
#define PCR_ECDSA_CONF_REG (DR_REG_PCR_BASE + 0xbc)
/** PCR_ECDSA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable ecdsa clock
 */
#define PCR_ECDSA_CLK_EN    (BIT(0))
#define PCR_ECDSA_CLK_EN_M  (PCR_ECDSA_CLK_EN_V << PCR_ECDSA_CLK_EN_S)
#define PCR_ECDSA_CLK_EN_V  0x00000001U
#define PCR_ECDSA_CLK_EN_S  0
/** PCR_ECDSA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset ecdsa module
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
#define PCR_IOMUX_CONF_REG (DR_REG_PCR_BASE + 0xc0)
/** PCR_IOMUX_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable iomux apb clock
 */
#define PCR_IOMUX_CLK_EN    (BIT(0))
#define PCR_IOMUX_CLK_EN_M  (PCR_IOMUX_CLK_EN_V << PCR_IOMUX_CLK_EN_S)
#define PCR_IOMUX_CLK_EN_V  0x00000001U
#define PCR_IOMUX_CLK_EN_S  0
/** PCR_IOMUX_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset iomux module
 */
#define PCR_IOMUX_RST_EN    (BIT(1))
#define PCR_IOMUX_RST_EN_M  (PCR_IOMUX_RST_EN_V << PCR_IOMUX_RST_EN_S)
#define PCR_IOMUX_RST_EN_V  0x00000001U
#define PCR_IOMUX_RST_EN_S  1

/** PCR_IOMUX_CLK_CONF_REG register
 *  IOMUX_CLK configuration register
 */
#define PCR_IOMUX_CLK_CONF_REG (DR_REG_PCR_BASE + 0xc4)
/** PCR_IOMUX_FUNC_CLK_SEL : R/W; bitpos: [21:20]; default: 0;
 *  Configures the clock source of IO MUX.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F80M_CLK\\
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

/** PCR_TCM_MEM_MONITOR_CONF_REG register
 *  TCM_MEM_MONITOR configuration register
 */
#define PCR_TCM_MEM_MONITOR_CONF_REG (DR_REG_PCR_BASE + 0xc8)
/** PCR_TCM_MEM_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable tcm_mem_monitor clock
 */
#define PCR_TCM_MEM_MONITOR_CLK_EN    (BIT(0))
#define PCR_TCM_MEM_MONITOR_CLK_EN_M  (PCR_TCM_MEM_MONITOR_CLK_EN_V << PCR_TCM_MEM_MONITOR_CLK_EN_S)
#define PCR_TCM_MEM_MONITOR_CLK_EN_V  0x00000001U
#define PCR_TCM_MEM_MONITOR_CLK_EN_S  0
/** PCR_TCM_MEM_MONITOR_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset tcm_mem_monitor module
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
#define PCR_PSRAM_MEM_MONITOR_CONF_REG (DR_REG_PCR_BASE + 0xcc)
/** PCR_PSRAM_MEM_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable psram_mem_monitor clock
 */
#define PCR_PSRAM_MEM_MONITOR_CLK_EN    (BIT(0))
#define PCR_PSRAM_MEM_MONITOR_CLK_EN_M  (PCR_PSRAM_MEM_MONITOR_CLK_EN_V << PCR_PSRAM_MEM_MONITOR_CLK_EN_S)
#define PCR_PSRAM_MEM_MONITOR_CLK_EN_V  0x00000001U
#define PCR_PSRAM_MEM_MONITOR_CLK_EN_S  0
/** PCR_PSRAM_MEM_MONITOR_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset psram_mem_monitor module
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

/** PCR_TRACE_CONF_REG register
 *  TRACE configuration register
 */
#define PCR_TRACE_CONF_REG (DR_REG_PCR_BASE + 0xd0)
/** PCR_TRACE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable trace clock
 */
#define PCR_TRACE_CLK_EN    (BIT(0))
#define PCR_TRACE_CLK_EN_M  (PCR_TRACE_CLK_EN_V << PCR_TRACE_CLK_EN_S)
#define PCR_TRACE_CLK_EN_V  0x00000001U
#define PCR_TRACE_CLK_EN_S  0
/** PCR_TRACE_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset trace module
 */
#define PCR_TRACE_RST_EN    (BIT(1))
#define PCR_TRACE_RST_EN_M  (PCR_TRACE_RST_EN_V << PCR_TRACE_RST_EN_S)
#define PCR_TRACE_RST_EN_V  0x00000001U
#define PCR_TRACE_RST_EN_S  1

/** PCR_ASSIST_CONF_REG register
 *  ASSIST configuration register
 */
#define PCR_ASSIST_CONF_REG (DR_REG_PCR_BASE + 0xd4)
/** PCR_ASSIST_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable assist clock
 */
#define PCR_ASSIST_CLK_EN    (BIT(0))
#define PCR_ASSIST_CLK_EN_M  (PCR_ASSIST_CLK_EN_V << PCR_ASSIST_CLK_EN_S)
#define PCR_ASSIST_CLK_EN_V  0x00000001U
#define PCR_ASSIST_CLK_EN_S  0
/** PCR_ASSIST_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset assist module
 */
#define PCR_ASSIST_RST_EN    (BIT(1))
#define PCR_ASSIST_RST_EN_M  (PCR_ASSIST_RST_EN_V << PCR_ASSIST_RST_EN_S)
#define PCR_ASSIST_RST_EN_V  0x00000001U
#define PCR_ASSIST_RST_EN_S  1

/** PCR_CACHE_CONF_REG register
 *  CACHE configuration register
 */
#define PCR_CACHE_CONF_REG (DR_REG_PCR_BASE + 0xd8)
/** PCR_CACHE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable cache clock
 */
#define PCR_CACHE_CLK_EN    (BIT(0))
#define PCR_CACHE_CLK_EN_M  (PCR_CACHE_CLK_EN_V << PCR_CACHE_CLK_EN_S)
#define PCR_CACHE_CLK_EN_V  0x00000001U
#define PCR_CACHE_CLK_EN_S  0
/** PCR_CACHE_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset cache module
 */
#define PCR_CACHE_RST_EN    (BIT(1))
#define PCR_CACHE_RST_EN_M  (PCR_CACHE_RST_EN_V << PCR_CACHE_RST_EN_S)
#define PCR_CACHE_RST_EN_V  0x00000001U
#define PCR_CACHE_RST_EN_S  1

/** PCR_CACHE_PD_CTRL_REG register
 *  CACHE power control register
 */
#define PCR_CACHE_PD_CTRL_REG (DR_REG_PCR_BASE + 0xdc)
/** PCR_CACHE_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force power down CACHE memory.
 */
#define PCR_CACHE_MEM_FORCE_PU    (BIT(1))
#define PCR_CACHE_MEM_FORCE_PU_M  (PCR_CACHE_MEM_FORCE_PU_V << PCR_CACHE_MEM_FORCE_PU_S)
#define PCR_CACHE_MEM_FORCE_PU_V  0x00000001U
#define PCR_CACHE_MEM_FORCE_PU_S  1
/** PCR_CACHE_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force power up CACHE memory.
 */
#define PCR_CACHE_MEM_FORCE_PD    (BIT(2))
#define PCR_CACHE_MEM_FORCE_PD_M  (PCR_CACHE_MEM_FORCE_PD_V << PCR_CACHE_MEM_FORCE_PD_S)
#define PCR_CACHE_MEM_FORCE_PD_V  0x00000001U
#define PCR_CACHE_MEM_FORCE_PD_S  2

/** PCR_MODEM_CONF_REG register
 *  MODEM_APB configuration register
 */
#define PCR_MODEM_CONF_REG (DR_REG_PCR_BASE + 0xe0)
/** PCR_MODEM_APB_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  This field indicates if modem_apb clock is enable. 0: disable, 1: enable(default).
 */
#define PCR_MODEM_APB_CLK_EN    (BIT(0))
#define PCR_MODEM_APB_CLK_EN_M  (PCR_MODEM_APB_CLK_EN_V << PCR_MODEM_APB_CLK_EN_S)
#define PCR_MODEM_APB_CLK_EN_V  0x00000001U
#define PCR_MODEM_APB_CLK_EN_S  0
/** PCR_MODEM_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set this file as 1 to reset modem-subsystem.
 */
#define PCR_MODEM_RST_EN    (BIT(1))
#define PCR_MODEM_RST_EN_M  (PCR_MODEM_RST_EN_V << PCR_MODEM_RST_EN_S)
#define PCR_MODEM_RST_EN_V  0x00000001U
#define PCR_MODEM_RST_EN_S  1
/** PCR_MODEM_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  This field indicates if modem source clock is enable. 0: disable, 1:
 *  enable(default).
 */
#define PCR_MODEM_CLK_EN    (BIT(2))
#define PCR_MODEM_CLK_EN_M  (PCR_MODEM_CLK_EN_V << PCR_MODEM_CLK_EN_S)
#define PCR_MODEM_CLK_EN_V  0x00000001U
#define PCR_MODEM_CLK_EN_S  2

/** PCR_TIMEOUT_CONF_REG register
 *  TIMEOUT configuration register
 */
#define PCR_TIMEOUT_CONF_REG (DR_REG_PCR_BASE + 0xe4)
/** PCR_CPU_TIMEOUT_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset cpu_peri timeout module
 */
#define PCR_CPU_TIMEOUT_RST_EN    (BIT(1))
#define PCR_CPU_TIMEOUT_RST_EN_M  (PCR_CPU_TIMEOUT_RST_EN_V << PCR_CPU_TIMEOUT_RST_EN_S)
#define PCR_CPU_TIMEOUT_RST_EN_V  0x00000001U
#define PCR_CPU_TIMEOUT_RST_EN_S  1
/** PCR_HP_TIMEOUT_RST_EN : R/W; bitpos: [2]; default: 0;
 *  Set 0 to reset hp_peri timeout module and hp_modem timeout module
 */
#define PCR_HP_TIMEOUT_RST_EN    (BIT(2))
#define PCR_HP_TIMEOUT_RST_EN_M  (PCR_HP_TIMEOUT_RST_EN_V << PCR_HP_TIMEOUT_RST_EN_S)
#define PCR_HP_TIMEOUT_RST_EN_V  0x00000001U
#define PCR_HP_TIMEOUT_RST_EN_S  2

/** PCR_SYSCLK_CONF_REG register
 *  SYSCLK configuration register
 */
#define PCR_SYSCLK_CONF_REG (DR_REG_PCR_BASE + 0xe8)
/** PCR_SOC_CLK_SEL : R/W; bitpos: [17:16]; default: 0;
 *  Configures to select the clock source of HP_ROOT_CLK.\\
 *  0 (default): XTAL_CLK\\
 *  1: RC_FAST_CLK\\
 *  2: PLL_F160M_CLK\\
 */
#define PCR_SOC_CLK_SEL    0x00000003U
#define PCR_SOC_CLK_SEL_M  (PCR_SOC_CLK_SEL_V << PCR_SOC_CLK_SEL_S)
#define PCR_SOC_CLK_SEL_V  0x00000003U
#define PCR_SOC_CLK_SEL_S  16
/** PCR_CLK_XTAL_FREQ : RO; bitpos: [30:24]; default: 40;
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
#define PCR_CPU_WAITI_CONF_REG (DR_REG_PCR_BASE + 0xec)
/** PCR_CPUPERIOD_SEL : HRO; bitpos: [1:0]; default: 1;
 *  Reserved. This filed has been replaced by PCR_CPU_DIV_NUM
 */
#define PCR_CPUPERIOD_SEL    0x00000003U
#define PCR_CPUPERIOD_SEL_M  (PCR_CPUPERIOD_SEL_V << PCR_CPUPERIOD_SEL_S)
#define PCR_CPUPERIOD_SEL_V  0x00000003U
#define PCR_CPUPERIOD_SEL_S  0
/** PCR_PLL_FREQ_SEL : HRO; bitpos: [2]; default: 1;
 *  Reserved. This filed has been replaced by PCR_CPU_DIV_NUM
 */
#define PCR_PLL_FREQ_SEL    (BIT(2))
#define PCR_PLL_FREQ_SEL_M  (PCR_PLL_FREQ_SEL_V << PCR_PLL_FREQ_SEL_S)
#define PCR_PLL_FREQ_SEL_V  0x00000001U
#define PCR_PLL_FREQ_SEL_S  2
/** PCR_CPU_WAIT_MODE_FORCE_ON : R/W; bitpos: [3]; default: 1;
 *  Set 1 to force cpu_waiti_clk enable.
 */
#define PCR_CPU_WAIT_MODE_FORCE_ON    (BIT(3))
#define PCR_CPU_WAIT_MODE_FORCE_ON_M  (PCR_CPU_WAIT_MODE_FORCE_ON_V << PCR_CPU_WAIT_MODE_FORCE_ON_S)
#define PCR_CPU_WAIT_MODE_FORCE_ON_V  0x00000001U
#define PCR_CPU_WAIT_MODE_FORCE_ON_S  3
/** PCR_CPU_WAITI_DELAY_NUM : R/W; bitpos: [7:4]; default: 0;
 *  This field used to set delay cycle when cpu enter waiti mode, after delay waiti_clk
 *  will close
 */
#define PCR_CPU_WAITI_DELAY_NUM    0x0000000FU
#define PCR_CPU_WAITI_DELAY_NUM_M  (PCR_CPU_WAITI_DELAY_NUM_V << PCR_CPU_WAITI_DELAY_NUM_S)
#define PCR_CPU_WAITI_DELAY_NUM_V  0x0000000FU
#define PCR_CPU_WAITI_DELAY_NUM_S  4

/** PCR_CPU_FREQ_CONF_REG register
 *  CPU_FREQ configuration register
 */
#define PCR_CPU_FREQ_CONF_REG (DR_REG_PCR_BASE + 0xf0)
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
#define PCR_AHB_FREQ_CONF_REG (DR_REG_PCR_BASE + 0xf4)
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
#define PCR_APB_FREQ_CONF_REG (DR_REG_PCR_BASE + 0xf8)
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
#define PCR_SYSCLK_FREQ_QUERY_0_REG (DR_REG_PCR_BASE + 0xfc)
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
#define PCR_PLL_DIV_CLK_EN_REG (DR_REG_PCR_BASE + 0x100)
/** PCR_PLL_160M_CLK_EN : R/W; bitpos: [1]; default: 1;
 *  This field is used to open 160 MHz clock (div3 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_160M_CLK_EN    (BIT(1))
#define PCR_PLL_160M_CLK_EN_M  (PCR_PLL_160M_CLK_EN_V << PCR_PLL_160M_CLK_EN_S)
#define PCR_PLL_160M_CLK_EN_V  0x00000001U
#define PCR_PLL_160M_CLK_EN_S  1
/** PCR_PLL_120M_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  This field is used to open 120 MHz clock (div4 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_120M_CLK_EN    (BIT(2))
#define PCR_PLL_120M_CLK_EN_M  (PCR_PLL_120M_CLK_EN_V << PCR_PLL_120M_CLK_EN_S)
#define PCR_PLL_120M_CLK_EN_V  0x00000001U
#define PCR_PLL_120M_CLK_EN_S  2
/** PCR_PLL_80M_CLK_EN : R/W; bitpos: [3]; default: 1;
 *  This field is used to open 80 MHz clock (div6  of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_80M_CLK_EN    (BIT(3))
#define PCR_PLL_80M_CLK_EN_M  (PCR_PLL_80M_CLK_EN_V << PCR_PLL_80M_CLK_EN_S)
#define PCR_PLL_80M_CLK_EN_V  0x00000001U
#define PCR_PLL_80M_CLK_EN_S  3
/** PCR_PLL_60M_CLK_EN : R/W; bitpos: [4]; default: 1;
 *  This field is used to open 60 MHz clock (div8 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_60M_CLK_EN    (BIT(4))
#define PCR_PLL_60M_CLK_EN_M  (PCR_PLL_60M_CLK_EN_V << PCR_PLL_60M_CLK_EN_S)
#define PCR_PLL_60M_CLK_EN_V  0x00000001U
#define PCR_PLL_60M_CLK_EN_S  4
/** PCR_PLL_48M_CLK_EN : R/W; bitpos: [5]; default: 1;
 *  This field is used to open 48 MHz clock (div10 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_48M_CLK_EN    (BIT(5))
#define PCR_PLL_48M_CLK_EN_M  (PCR_PLL_48M_CLK_EN_V << PCR_PLL_48M_CLK_EN_S)
#define PCR_PLL_48M_CLK_EN_V  0x00000001U
#define PCR_PLL_48M_CLK_EN_S  5
/** PCR_PLL_40M_CLK_EN : R/W; bitpos: [6]; default: 1;
 *  This field is used to open 40 MHz clock (div12 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_40M_CLK_EN    (BIT(6))
#define PCR_PLL_40M_CLK_EN_M  (PCR_PLL_40M_CLK_EN_V << PCR_PLL_40M_CLK_EN_S)
#define PCR_PLL_40M_CLK_EN_V  0x00000001U
#define PCR_PLL_40M_CLK_EN_S  6
/** PCR_PLL_20M_CLK_EN : R/W; bitpos: [7]; default: 1;
 *  This field is used to open 20 MHz clock (div24 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_20M_CLK_EN    (BIT(7))
#define PCR_PLL_20M_CLK_EN_M  (PCR_PLL_20M_CLK_EN_V << PCR_PLL_20M_CLK_EN_S)
#define PCR_PLL_20M_CLK_EN_V  0x00000001U
#define PCR_PLL_20M_CLK_EN_S  7
/** PCR_PLL_12M_CLK_EN : HRO; bitpos: [8]; default: 1;
 *  This field is used to open 12 MHz clock (div40 of SPLL) derived from SPLL. 0: close,
 *  1: open(default). Only available when high-speed clock-source SPLL is active.
 */
#define PCR_PLL_12M_CLK_EN    (BIT(8))
#define PCR_PLL_12M_CLK_EN_M  (PCR_PLL_12M_CLK_EN_V << PCR_PLL_12M_CLK_EN_S)
#define PCR_PLL_12M_CLK_EN_V  0x00000001U
#define PCR_PLL_12M_CLK_EN_S  8

/** PCR_CTRL_CLK_OUT_EN_REG register
 *  CLK_OUT_EN configuration register
 */
#define PCR_CTRL_CLK_OUT_EN_REG (DR_REG_PCR_BASE + 0x104)
/** PCR_CLK20_OEN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable 20m clock
 */
#define PCR_CLK20_OEN    (BIT(0))
#define PCR_CLK20_OEN_M  (PCR_CLK20_OEN_V << PCR_CLK20_OEN_S)
#define PCR_CLK20_OEN_V  0x00000001U
#define PCR_CLK20_OEN_S  0
/** PCR_CLK22_OEN : R/W; bitpos: [1]; default: 1;
 *  Set 1 to enable 22m clock
 */
#define PCR_CLK22_OEN    (BIT(1))
#define PCR_CLK22_OEN_M  (PCR_CLK22_OEN_V << PCR_CLK22_OEN_S)
#define PCR_CLK22_OEN_V  0x00000001U
#define PCR_CLK22_OEN_S  1
/** PCR_CLK44_OEN : R/W; bitpos: [2]; default: 1;
 *  Set 1 to enable 44m clock
 */
#define PCR_CLK44_OEN    (BIT(2))
#define PCR_CLK44_OEN_M  (PCR_CLK44_OEN_V << PCR_CLK44_OEN_S)
#define PCR_CLK44_OEN_V  0x00000001U
#define PCR_CLK44_OEN_S  2
/** PCR_CLK_BB_OEN : R/W; bitpos: [3]; default: 1;
 *  Set 1 to enable bb clock
 */
#define PCR_CLK_BB_OEN    (BIT(3))
#define PCR_CLK_BB_OEN_M  (PCR_CLK_BB_OEN_V << PCR_CLK_BB_OEN_S)
#define PCR_CLK_BB_OEN_V  0x00000001U
#define PCR_CLK_BB_OEN_S  3
/** PCR_CLK80_OEN : R/W; bitpos: [4]; default: 1;
 *  Set 1 to enable 80m clock
 */
#define PCR_CLK80_OEN    (BIT(4))
#define PCR_CLK80_OEN_M  (PCR_CLK80_OEN_V << PCR_CLK80_OEN_S)
#define PCR_CLK80_OEN_V  0x00000001U
#define PCR_CLK80_OEN_S  4
/** PCR_CLK160_OEN : R/W; bitpos: [5]; default: 1;
 *  Set 1 to enable 160m clock
 */
#define PCR_CLK160_OEN    (BIT(5))
#define PCR_CLK160_OEN_M  (PCR_CLK160_OEN_V << PCR_CLK160_OEN_S)
#define PCR_CLK160_OEN_V  0x00000001U
#define PCR_CLK160_OEN_S  5
/** PCR_CLK_320M_OEN : R/W; bitpos: [6]; default: 1;
 *  Set 1 to enable 320m clock
 */
#define PCR_CLK_320M_OEN    (BIT(6))
#define PCR_CLK_320M_OEN_M  (PCR_CLK_320M_OEN_V << PCR_CLK_320M_OEN_S)
#define PCR_CLK_320M_OEN_V  0x00000001U
#define PCR_CLK_320M_OEN_S  6
/** PCR_CLK_ADC_INF_OEN : R/W; bitpos: [7]; default: 1;
 *  Reserved
 */
#define PCR_CLK_ADC_INF_OEN    (BIT(7))
#define PCR_CLK_ADC_INF_OEN_M  (PCR_CLK_ADC_INF_OEN_V << PCR_CLK_ADC_INF_OEN_S)
#define PCR_CLK_ADC_INF_OEN_V  0x00000001U
#define PCR_CLK_ADC_INF_OEN_S  7
/** PCR_CLK_DAC_CPU_OEN : R/W; bitpos: [8]; default: 1;
 *  Reserved
 */
#define PCR_CLK_DAC_CPU_OEN    (BIT(8))
#define PCR_CLK_DAC_CPU_OEN_M  (PCR_CLK_DAC_CPU_OEN_V << PCR_CLK_DAC_CPU_OEN_S)
#define PCR_CLK_DAC_CPU_OEN_V  0x00000001U
#define PCR_CLK_DAC_CPU_OEN_S  8
/** PCR_CLK_XTAL_OEN : R/W; bitpos: [9]; default: 1;
 *  Set 1 to enable xtal clock
 */
#define PCR_CLK_XTAL_OEN    (BIT(9))
#define PCR_CLK_XTAL_OEN_M  (PCR_CLK_XTAL_OEN_V << PCR_CLK_XTAL_OEN_S)
#define PCR_CLK_XTAL_OEN_V  0x00000001U
#define PCR_CLK_XTAL_OEN_S  9

/** PCR_CTRL_32K_CONF_REG register
 *  32KHz clock configuration register
 */
#define PCR_CTRL_32K_CONF_REG (DR_REG_PCR_BASE + 0x10c)
/** PCR_32K_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures the 32KHz clock for TIMER_GROUP.\\
 *  0 (default): RC32K_CLK\\
 *  1: XTAL32K_CLK\\
 *  2: OSC_SLOW_CLK\\
 *  3: RC_SLOW_CLK\\
 *  4: RC_FAST_CLK\\
 */
#define PCR_32K_SEL    0x00000007U
#define PCR_32K_SEL_M  (PCR_32K_SEL_V << PCR_32K_SEL_S)
#define PCR_32K_SEL_V  0x00000007U
#define PCR_32K_SEL_S  0
/** PCR_FOSC_TICK_NUM : R/W; bitpos: [15:8]; default: 7;
 *  When PCR_32K_SEL set as 4, This field PCR_FOSC_TICK_NUM is used to set the divider
 *  number of fosc.
 */
#define PCR_FOSC_TICK_NUM    0x000000FFU
#define PCR_FOSC_TICK_NUM_M  (PCR_FOSC_TICK_NUM_V << PCR_FOSC_TICK_NUM_S)
#define PCR_FOSC_TICK_NUM_V  0x000000FFU
#define PCR_FOSC_TICK_NUM_S  8

/** PCR_SRAM_POWER_CONF_0_REG register
 *  HP SRAM/ROM configuration register
 */
#define PCR_SRAM_POWER_CONF_0_REG (DR_REG_PCR_BASE + 0x110)
/** PCR_ROM_FORCE_PU : R/W; bitpos: [1:0]; default: 3;
 *  Set this bit to force power up ROM
 */
#define PCR_ROM_FORCE_PU    0x00000003U
#define PCR_ROM_FORCE_PU_M  (PCR_ROM_FORCE_PU_V << PCR_ROM_FORCE_PU_S)
#define PCR_ROM_FORCE_PU_V  0x00000003U
#define PCR_ROM_FORCE_PU_S  0
/** PCR_ROM_FORCE_PD : R/W; bitpos: [3:2]; default: 0;
 *  Set this bit to force power down ROM.
 */
#define PCR_ROM_FORCE_PD    0x00000003U
#define PCR_ROM_FORCE_PD_M  (PCR_ROM_FORCE_PD_V << PCR_ROM_FORCE_PD_S)
#define PCR_ROM_FORCE_PD_V  0x00000003U
#define PCR_ROM_FORCE_PD_S  2
/** PCR_ROM_CLKGATE_FORCE_ON : R/W; bitpos: [5:4]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the ROM. 0: A
 *  gate-clock will be used when accessing the ROM.
 */
#define PCR_ROM_CLKGATE_FORCE_ON    0x00000003U
#define PCR_ROM_CLKGATE_FORCE_ON_M  (PCR_ROM_CLKGATE_FORCE_ON_V << PCR_ROM_CLKGATE_FORCE_ON_S)
#define PCR_ROM_CLKGATE_FORCE_ON_V  0x00000003U
#define PCR_ROM_CLKGATE_FORCE_ON_S  4

/** PCR_SRAM_POWER_CONF_1_REG register
 *  HP SRAM/ROM configuration register
 */
#define PCR_SRAM_POWER_CONF_1_REG (DR_REG_PCR_BASE + 0x114)
/** PCR_SRAM_FORCE_PU : R/W; bitpos: [4:0]; default: 31;
 *  Set this bit to force power up SRAM
 */
#define PCR_SRAM_FORCE_PU    0x0000001FU
#define PCR_SRAM_FORCE_PU_M  (PCR_SRAM_FORCE_PU_V << PCR_SRAM_FORCE_PU_S)
#define PCR_SRAM_FORCE_PU_V  0x0000001FU
#define PCR_SRAM_FORCE_PU_S  0
/** PCR_SRAM_FORCE_PD : R/W; bitpos: [14:10]; default: 0;
 *  Set this bit to force power down SRAM.
 */
#define PCR_SRAM_FORCE_PD    0x0000001FU
#define PCR_SRAM_FORCE_PD_M  (PCR_SRAM_FORCE_PD_V << PCR_SRAM_FORCE_PD_S)
#define PCR_SRAM_FORCE_PD_V  0x0000001FU
#define PCR_SRAM_FORCE_PD_S  10
/** PCR_SRAM_CLKGATE_FORCE_ON : R/W; bitpos: [24:20]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the SRAM. 0: A
 *  gate-clock will be used when accessing the SRAM.
 */
#define PCR_SRAM_CLKGATE_FORCE_ON    0x0000001FU
#define PCR_SRAM_CLKGATE_FORCE_ON_M  (PCR_SRAM_CLKGATE_FORCE_ON_V << PCR_SRAM_CLKGATE_FORCE_ON_S)
#define PCR_SRAM_CLKGATE_FORCE_ON_V  0x0000001FU
#define PCR_SRAM_CLKGATE_FORCE_ON_S  20

/** PCR_SEC_CONF_REG register
 *  Clock source configuration register for External Memory Encryption and Decryption
 */
#define PCR_SEC_CONF_REG (DR_REG_PCR_BASE + 0x118)
/** PCR_SEC_CLK_SEL : R/W; bitpos: [1:0]; default: 0;
 *  Configures the clock source for the External Memory Encryption and Decryption
 *  module.\\
 *  0(default): XTAL_CLK\\
 *  1 RC_FAST_CLK\\
 *  2: PLL_F480M_CLK\\
 */
#define PCR_SEC_CLK_SEL    0x00000003U
#define PCR_SEC_CLK_SEL_M  (PCR_SEC_CLK_SEL_V << PCR_SEC_CLK_SEL_S)
#define PCR_SEC_CLK_SEL_V  0x00000003U
#define PCR_SEC_CLK_SEL_S  0
/** PCR_SEC_RST_EN : R/W; bitpos: [2]; default: 0;
 *  Set 0 to reset sec module
 */
#define PCR_SEC_RST_EN    (BIT(2))
#define PCR_SEC_RST_EN_M  (PCR_SEC_RST_EN_V << PCR_SEC_RST_EN_S)
#define PCR_SEC_RST_EN_V  0x00000001U
#define PCR_SEC_RST_EN_S  2

/** PCR_ADC_DAC_INV_PHASE_CONF_REG register
 *  xxxx
 */
#define PCR_ADC_DAC_INV_PHASE_CONF_REG (DR_REG_PCR_BASE + 0x11c)
/** PCR_CLK_RX_ADC_INV_PHASE_ENA : R/W; bitpos: [0]; default: 0;
 *  xxxx
 */
#define PCR_CLK_RX_ADC_INV_PHASE_ENA    (BIT(0))
#define PCR_CLK_RX_ADC_INV_PHASE_ENA_M  (PCR_CLK_RX_ADC_INV_PHASE_ENA_V << PCR_CLK_RX_ADC_INV_PHASE_ENA_S)
#define PCR_CLK_RX_ADC_INV_PHASE_ENA_V  0x00000001U
#define PCR_CLK_RX_ADC_INV_PHASE_ENA_S  0
/** PCR_CLK_TX_DAC_INV_PHASE_ENA : R/W; bitpos: [1]; default: 0;
 *  xxxx
 */
#define PCR_CLK_TX_DAC_INV_PHASE_ENA    (BIT(1))
#define PCR_CLK_TX_DAC_INV_PHASE_ENA_M  (PCR_CLK_TX_DAC_INV_PHASE_ENA_V << PCR_CLK_TX_DAC_INV_PHASE_ENA_S)
#define PCR_CLK_TX_DAC_INV_PHASE_ENA_V  0x00000001U
#define PCR_CLK_TX_DAC_INV_PHASE_ENA_S  1

/** PCR_BUS_CLK_UPDATE_REG register
 *  Configuration register for applying updated high-performance system clock sources
 */
#define PCR_BUS_CLK_UPDATE_REG (DR_REG_PCR_BASE + 0x120)
/** PCR_BUS_CLOCK_UPDATE : R/W/WTC; bitpos: [0]; default: 0;
 *  Configures whether or not to update configurations for CPU_CLK division, AHB_CLK
 *  division and HP_ROOT_CLK clock source selection.\\
 *  0: Not update configurations\\
 *  1: Update configurations\\
 *  This bit is automatically cleared when configurations have been updated.\\
 */
#define PCR_BUS_CLOCK_UPDATE    (BIT(0))
#define PCR_BUS_CLOCK_UPDATE_M  (PCR_BUS_CLOCK_UPDATE_V << PCR_BUS_CLOCK_UPDATE_S)
#define PCR_BUS_CLOCK_UPDATE_V  0x00000001U
#define PCR_BUS_CLOCK_UPDATE_S  0

/** PCR_SAR_CLK_DIV_REG register
 *  SAR ADC clock divisor configuration register
 */
#define PCR_SAR_CLK_DIV_REG (DR_REG_PCR_BASE + 0x124)
/** PCR_SAR2_CLK_DIV_NUM : R/W; bitpos: [7:0]; default: 4;
 *  Configures the divisor for SAR ADC 2  clock to generate ADC analog control
 *  signals.\\
 */
#define PCR_SAR2_CLK_DIV_NUM    0x000000FFU
#define PCR_SAR2_CLK_DIV_NUM_M  (PCR_SAR2_CLK_DIV_NUM_V << PCR_SAR2_CLK_DIV_NUM_S)
#define PCR_SAR2_CLK_DIV_NUM_V  0x000000FFU
#define PCR_SAR2_CLK_DIV_NUM_S  0
/** PCR_SAR1_CLK_DIV_NUM : R/W; bitpos: [15:8]; default: 4;
 *  Configures the divisor for SAR ADC 1  clock to generate ADC analog control
 *  signals.\\
 */
#define PCR_SAR1_CLK_DIV_NUM    0x000000FFU
#define PCR_SAR1_CLK_DIV_NUM_M  (PCR_SAR1_CLK_DIV_NUM_V << PCR_SAR1_CLK_DIV_NUM_S)
#define PCR_SAR1_CLK_DIV_NUM_V  0x000000FFU
#define PCR_SAR1_CLK_DIV_NUM_S  8

/** PCR_PWDET_SAR_CLK_CONF_REG register
 *  xxxx
 */
#define PCR_PWDET_SAR_CLK_CONF_REG (DR_REG_PCR_BASE + 0x128)
/** PCR_PWDET_SAR_CLK_DIV_NUM : R/W; bitpos: [7:0]; default: 7;
 *  xxxx
 */
#define PCR_PWDET_SAR_CLK_DIV_NUM    0x000000FFU
#define PCR_PWDET_SAR_CLK_DIV_NUM_M  (PCR_PWDET_SAR_CLK_DIV_NUM_V << PCR_PWDET_SAR_CLK_DIV_NUM_S)
#define PCR_PWDET_SAR_CLK_DIV_NUM_V  0x000000FFU
#define PCR_PWDET_SAR_CLK_DIV_NUM_S  0
/** PCR_PWDET_SAR_CLK_EN : R/W; bitpos: [8]; default: 1;
 *  xxxx
 */
#define PCR_PWDET_SAR_CLK_EN    (BIT(8))
#define PCR_PWDET_SAR_CLK_EN_M  (PCR_PWDET_SAR_CLK_EN_V << PCR_PWDET_SAR_CLK_EN_S)
#define PCR_PWDET_SAR_CLK_EN_V  0x00000001U
#define PCR_PWDET_SAR_CLK_EN_S  8

/** PCR_TIMERGROUP_WDT_CONF_REG register
 *  TIMERGROUP_WDT configuration register
 */
#define PCR_TIMERGROUP_WDT_CONF_REG (DR_REG_PCR_BASE + 0x12c)
/** PCR_TG0_WDT_RST_EN : R/W; bitpos: [0]; default: 0;
 *  Set 0 to reset timer_group0 wdt module
 */
#define PCR_TG0_WDT_RST_EN    (BIT(0))
#define PCR_TG0_WDT_RST_EN_M  (PCR_TG0_WDT_RST_EN_V << PCR_TG0_WDT_RST_EN_S)
#define PCR_TG0_WDT_RST_EN_V  0x00000001U
#define PCR_TG0_WDT_RST_EN_S  0
/** PCR_TG1_WDT_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset timer_group1 wdt module
 */
#define PCR_TG1_WDT_RST_EN    (BIT(1))
#define PCR_TG1_WDT_RST_EN_M  (PCR_TG1_WDT_RST_EN_V << PCR_TG1_WDT_RST_EN_S)
#define PCR_TG1_WDT_RST_EN_V  0x00000001U
#define PCR_TG1_WDT_RST_EN_S  1

/** PCR_TIMERGROUP_XTAL_CONF_REG register
 *  TIMERGROUP1 configuration register
 */
#define PCR_TIMERGROUP_XTAL_CONF_REG (DR_REG_PCR_BASE + 0x130)
/** PCR_TG0_XTAL_RST_EN : R/W; bitpos: [0]; default: 0;
 *  Set 0 to reset timer_group0 xtal clock domain
 */
#define PCR_TG0_XTAL_RST_EN    (BIT(0))
#define PCR_TG0_XTAL_RST_EN_M  (PCR_TG0_XTAL_RST_EN_V << PCR_TG0_XTAL_RST_EN_S)
#define PCR_TG0_XTAL_RST_EN_V  0x00000001U
#define PCR_TG0_XTAL_RST_EN_S  0
/** PCR_TG1_XTAL_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset timer_group1 xtal clock domain
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

/** PCR_RESET_EVENT_BYPASS_REG register
 *  reset event bypass backdoor configuration register
 */
#define PCR_RESET_EVENT_BYPASS_REG (DR_REG_PCR_BASE + 0x134)
/** PCR_RESET_EVENT_BYPASS_APM : R/W; bitpos: [0]; default: 0;
 *  This field is used to control reset event relationship for
 *  tee_reg/apm_reg/hp_system_reg. 1: tee_reg/apm_reg/hp_system_reg will only be reset
 *  by power-reset. some reset event will be bypass. 0: tee_reg/apm_reg/hp_system_reg
 *  will not only be reset by power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS_APM    (BIT(0))
#define PCR_RESET_EVENT_BYPASS_APM_M  (PCR_RESET_EVENT_BYPASS_APM_V << PCR_RESET_EVENT_BYPASS_APM_S)
#define PCR_RESET_EVENT_BYPASS_APM_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_APM_S  0
/** PCR_RESET_EVENT_BYPASS : R/W; bitpos: [1]; default: 1;
 *  This field is used to control reset event relationship for system-bus. 1: system
 *  bus (including arbiter/router) will only be reset by power-reset. some reset event
 *  will be bypass. 0: system bus (including arbiter/router) will not only be reset by
 *  power-reset, but also some reset event.
 */
#define PCR_RESET_EVENT_BYPASS    (BIT(1))
#define PCR_RESET_EVENT_BYPASS_M  (PCR_RESET_EVENT_BYPASS_V << PCR_RESET_EVENT_BYPASS_S)
#define PCR_RESET_EVENT_BYPASS_V  0x00000001U
#define PCR_RESET_EVENT_BYPASS_S  1

/** PCR_REGDMA_CONF_REG register
 *  REGDMA configuration register
 */
#define PCR_REGDMA_CONF_REG (DR_REG_PCR_BASE + 0x138)
/** PCR_REGDMA_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to enable regdma clock
 */
#define PCR_REGDMA_CLK_EN    (BIT(0))
#define PCR_REGDMA_CLK_EN_M  (PCR_REGDMA_CLK_EN_V << PCR_REGDMA_CLK_EN_S)
#define PCR_REGDMA_CLK_EN_V  0x00000001U
#define PCR_REGDMA_CLK_EN_S  0
/** PCR_REGDMA_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset regdma module
 */
#define PCR_REGDMA_RST_EN    (BIT(1))
#define PCR_REGDMA_RST_EN_M  (PCR_REGDMA_RST_EN_V << PCR_REGDMA_RST_EN_S)
#define PCR_REGDMA_RST_EN_V  0x00000001U
#define PCR_REGDMA_RST_EN_S  1

/** PCR_ETM_CONF_REG register
 *  ETM configuration register
 */
#define PCR_ETM_CONF_REG (DR_REG_PCR_BASE + 0x13c)
/** PCR_ETM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Set 1 to enable etm clock
 */
#define PCR_ETM_CLK_EN    (BIT(0))
#define PCR_ETM_CLK_EN_M  (PCR_ETM_CLK_EN_V << PCR_ETM_CLK_EN_S)
#define PCR_ETM_CLK_EN_V  0x00000001U
#define PCR_ETM_CLK_EN_S  0
/** PCR_ETM_RST_EN : R/W; bitpos: [1]; default: 0;
 *  Set 0 to reset etm module
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

/** PCR_FPGA_DEBUG_REG register
 *  fpga debug register
 */
#define PCR_FPGA_DEBUG_REG (DR_REG_PCR_BASE + 0xff4)
/** PCR_FPGA_DEBUG : R/W; bitpos: [31:0]; default: 4294967295;
 *  Only used in fpga debug.
 */
#define PCR_FPGA_DEBUG    0xFFFFFFFFU
#define PCR_FPGA_DEBUG_M  (PCR_FPGA_DEBUG_V << PCR_FPGA_DEBUG_S)
#define PCR_FPGA_DEBUG_V  0xFFFFFFFFU
#define PCR_FPGA_DEBUG_S  0

/** PCR_CLOCK_GATE_REG register
 *  PCR clock gating configure register
 */
#define PCR_CLOCK_GATE_REG (DR_REG_PCR_BASE + 0xff8)
/** PCR_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to force on clock gating.
 */
#define PCR_CLK_EN    (BIT(0))
#define PCR_CLK_EN_M  (PCR_CLK_EN_V << PCR_CLK_EN_S)
#define PCR_CLK_EN_V  0x00000001U
#define PCR_CLK_EN_S  0

/** PCR_DATE_REG register
 *  Date register.
 */
#define PCR_DATE_REG (DR_REG_PCR_BASE + 0xffc)
/** PCR_DATE : R/W; bitpos: [27:0]; default: 37753376;
 *  PCR version information.
 */
#define PCR_DATE    0x0FFFFFFFU
#define PCR_DATE_M  (PCR_DATE_V << PCR_DATE_S)
#define PCR_DATE_V  0x0FFFFFFFU
#define PCR_DATE_S  0

#ifdef __cplusplus
}
#endif
