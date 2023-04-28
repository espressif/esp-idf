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

/** LCD_CAM_LCD_CLOCK_REG register
 *  LCD clock configuration register
 */
#define LCD_CAM_LCD_CLOCK_REG (DR_REG_LCD_CAM_BASE + 0x0)
/** LCD_CAM_LCD_CLKCNT_N : R/W; bitpos: [5:0]; default: 0;
 *  f<SUB>LCD_PCLK</SUB>
 *  = f<SUB>LCD_CLK</SUB>/(LCD_CAM_LCD_CLKCNT_N + 1) when LCD_CAM_LCD_CLK_EQU_SYSCLK is
 *  0. Note: this field must not be configured to 0.
 */
#define LCD_CAM_LCD_CLKCNT_N    0x0000003FU
#define LCD_CAM_LCD_CLKCNT_N_M  (LCD_CAM_LCD_CLKCNT_N_V << LCD_CAM_LCD_CLKCNT_N_S)
#define LCD_CAM_LCD_CLKCNT_N_V  0x0000003FU
#define LCD_CAM_LCD_CLKCNT_N_S  0
/** LCD_CAM_LCD_CLK_EQU_SYSCLK : R/W; bitpos: [6]; default: 0;
 *  1: f<SUB>LCD_PCLK</SUB>
 *  = f<SUB>LCD_CLK</SUB>. 0: f<SUB>LCD_PCLK</SUB>
 *  = f<SUB>LCD_CLK</SUB>/(LCD_CAM_LCD_CLKCNT_N + 1).
 */
#define LCD_CAM_LCD_CLK_EQU_SYSCLK    (BIT(6))
#define LCD_CAM_LCD_CLK_EQU_SYSCLK_M  (LCD_CAM_LCD_CLK_EQU_SYSCLK_V << LCD_CAM_LCD_CLK_EQU_SYSCLK_S)
#define LCD_CAM_LCD_CLK_EQU_SYSCLK_V  0x00000001U
#define LCD_CAM_LCD_CLK_EQU_SYSCLK_S  6
/** LCD_CAM_LCD_CK_IDLE_EDGE : R/W; bitpos: [7]; default: 0;
 *  1: LCD_PCLK line is high in idle. 0: LCD_PCLK line is low in idle.
 */
#define LCD_CAM_LCD_CK_IDLE_EDGE    (BIT(7))
#define LCD_CAM_LCD_CK_IDLE_EDGE_M  (LCD_CAM_LCD_CK_IDLE_EDGE_V << LCD_CAM_LCD_CK_IDLE_EDGE_S)
#define LCD_CAM_LCD_CK_IDLE_EDGE_V  0x00000001U
#define LCD_CAM_LCD_CK_IDLE_EDGE_S  7
/** LCD_CAM_LCD_CK_OUT_EDGE : R/W; bitpos: [8]; default: 0;
 *  1: LCD_PCLK is high in the first half clock cycle. 0: LCD_PCLK is low in the first
 *  half clock cycle.
 */
#define LCD_CAM_LCD_CK_OUT_EDGE    (BIT(8))
#define LCD_CAM_LCD_CK_OUT_EDGE_M  (LCD_CAM_LCD_CK_OUT_EDGE_V << LCD_CAM_LCD_CK_OUT_EDGE_S)
#define LCD_CAM_LCD_CK_OUT_EDGE_V  0x00000001U
#define LCD_CAM_LCD_CK_OUT_EDGE_S  8
/** LCD_CAM_LCD_CLKM_DIV_NUM : R/W; bitpos: [16:9]; default: 0;
 *  Integral LCD clock divider value.
 */
#define LCD_CAM_LCD_CLKM_DIV_NUM    0x000000FFU
#define LCD_CAM_LCD_CLKM_DIV_NUM_M  (LCD_CAM_LCD_CLKM_DIV_NUM_V << LCD_CAM_LCD_CLKM_DIV_NUM_S)
#define LCD_CAM_LCD_CLKM_DIV_NUM_V  0x000000FFU
#define LCD_CAM_LCD_CLKM_DIV_NUM_S  9
/** LCD_CAM_LCD_CLKM_DIV_B : R/W; bitpos: [22:17]; default: 0;
 *  Fractional clock divider numerator value.
 */
#define LCD_CAM_LCD_CLKM_DIV_B    0x0000003FU
#define LCD_CAM_LCD_CLKM_DIV_B_M  (LCD_CAM_LCD_CLKM_DIV_B_V << LCD_CAM_LCD_CLKM_DIV_B_S)
#define LCD_CAM_LCD_CLKM_DIV_B_V  0x0000003FU
#define LCD_CAM_LCD_CLKM_DIV_B_S  17
/** LCD_CAM_LCD_CLKM_DIV_A : R/W; bitpos: [28:23]; default: 0;
 *  Fractional clock divider denominator value.
 */
#define LCD_CAM_LCD_CLKM_DIV_A    0x0000003FU
#define LCD_CAM_LCD_CLKM_DIV_A_M  (LCD_CAM_LCD_CLKM_DIV_A_V << LCD_CAM_LCD_CLKM_DIV_A_S)
#define LCD_CAM_LCD_CLKM_DIV_A_V  0x0000003FU
#define LCD_CAM_LCD_CLKM_DIV_A_S  23
/** LCD_CAM_LCD_CLK_SEL : R/W; bitpos: [30:29]; default: 0;
 *  Select LCD module source clock. 0: clock source is disabled. 1: XTAL_CLK. 2:
 *  PLL_D2_CLK. 3: PLL_F160M_CLK.
 */
#define LCD_CAM_LCD_CLK_SEL    0x00000003U
#define LCD_CAM_LCD_CLK_SEL_M  (LCD_CAM_LCD_CLK_SEL_V << LCD_CAM_LCD_CLK_SEL_S)
#define LCD_CAM_LCD_CLK_SEL_V  0x00000003U
#define LCD_CAM_LCD_CLK_SEL_S  29
/** LCD_CAM_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Set this bit to force enable the clock for all configuration registers. Clock gate
 *  is not used.
 */
#define LCD_CAM_CLK_EN    (BIT(31))
#define LCD_CAM_CLK_EN_M  (LCD_CAM_CLK_EN_V << LCD_CAM_CLK_EN_S)
#define LCD_CAM_CLK_EN_V  0x00000001U
#define LCD_CAM_CLK_EN_S  31

/** LCD_CAM_CAM_CTRL_REG register
 *  Camera clock configuration register
 */
#define LCD_CAM_CAM_CTRL_REG (DR_REG_LCD_CAM_BASE + 0x4)
/** LCD_CAM_CAM_STOP_EN : R/W; bitpos: [0]; default: 0;
 *  Camera stop enable signal, 1: camera stops when GDMA Rx FIFO is full. 0: Do not
 *  stop.
 */
#define LCD_CAM_CAM_STOP_EN    (BIT(0))
#define LCD_CAM_CAM_STOP_EN_M  (LCD_CAM_CAM_STOP_EN_V << LCD_CAM_CAM_STOP_EN_S)
#define LCD_CAM_CAM_STOP_EN_V  0x00000001U
#define LCD_CAM_CAM_STOP_EN_S  0
/** LCD_CAM_CAM_VSYNC_FILTER_THRES : R/W; bitpos: [3:1]; default: 0;
 *  Filter threshold value for CAM_VSYNC signal.
 */
#define LCD_CAM_CAM_VSYNC_FILTER_THRES    0x00000007U
#define LCD_CAM_CAM_VSYNC_FILTER_THRES_M  (LCD_CAM_CAM_VSYNC_FILTER_THRES_V << LCD_CAM_CAM_VSYNC_FILTER_THRES_S)
#define LCD_CAM_CAM_VSYNC_FILTER_THRES_V  0x00000007U
#define LCD_CAM_CAM_VSYNC_FILTER_THRES_S  1
/** LCD_CAM_CAM_UPDATE : R/W; bitpos: [4]; default: 0;
 *  1: Update camera registers. This bit is cleared by hardware. 0: Do not care.
 */
#define LCD_CAM_CAM_UPDATE    (BIT(4))
#define LCD_CAM_CAM_UPDATE_M  (LCD_CAM_CAM_UPDATE_V << LCD_CAM_CAM_UPDATE_S)
#define LCD_CAM_CAM_UPDATE_V  0x00000001U
#define LCD_CAM_CAM_UPDATE_S  4
/** LCD_CAM_CAM_BYTE_ORDER : R/W; bitpos: [5]; default: 0;
 *  1: Invert data byte order, only valid in 16-bit mode. 0: Do not change.
 */
#define LCD_CAM_CAM_BYTE_ORDER    (BIT(5))
#define LCD_CAM_CAM_BYTE_ORDER_M  (LCD_CAM_CAM_BYTE_ORDER_V << LCD_CAM_CAM_BYTE_ORDER_S)
#define LCD_CAM_CAM_BYTE_ORDER_V  0x00000001U
#define LCD_CAM_CAM_BYTE_ORDER_S  5
/** LCD_CAM_CAM_BIT_ORDER : R/W; bitpos: [6]; default: 0;
 *  1: Change data bit order, change CAM_DATA_in[7:0] to CAM_DATA_in[0:7] in 8-bit
 *  mode, and bits[15:0] to bits[0:15] in 16-bit mode.  0: Do not change.
 */
#define LCD_CAM_CAM_BIT_ORDER    (BIT(6))
#define LCD_CAM_CAM_BIT_ORDER_M  (LCD_CAM_CAM_BIT_ORDER_V << LCD_CAM_CAM_BIT_ORDER_S)
#define LCD_CAM_CAM_BIT_ORDER_V  0x00000001U
#define LCD_CAM_CAM_BIT_ORDER_S  6
/** LCD_CAM_CAM_LINE_INT_EN : R/W; bitpos: [7]; default: 0;
 *  1: Enable to generate LCD_CAM_CAM_HS_INT. 0: Disable.
 */
#define LCD_CAM_CAM_LINE_INT_EN    (BIT(7))
#define LCD_CAM_CAM_LINE_INT_EN_M  (LCD_CAM_CAM_LINE_INT_EN_V << LCD_CAM_CAM_LINE_INT_EN_S)
#define LCD_CAM_CAM_LINE_INT_EN_V  0x00000001U
#define LCD_CAM_CAM_LINE_INT_EN_S  7
/** LCD_CAM_CAM_VS_EOF_EN : R/W; bitpos: [8]; default: 0;
 *  1: Enable CAM_VSYNC to generate in_suc_eof. 0: in_suc_eof is controlled by
 *  LCD_CAM_CAM_REC_DATA_BYTELEN.
 */
#define LCD_CAM_CAM_VS_EOF_EN    (BIT(8))
#define LCD_CAM_CAM_VS_EOF_EN_M  (LCD_CAM_CAM_VS_EOF_EN_V << LCD_CAM_CAM_VS_EOF_EN_S)
#define LCD_CAM_CAM_VS_EOF_EN_V  0x00000001U
#define LCD_CAM_CAM_VS_EOF_EN_S  8
/** LCD_CAM_CAM_CLKM_DIV_NUM : R/W; bitpos: [16:9]; default: 0;
 *  Integral camera clock divider value.
 */
#define LCD_CAM_CAM_CLKM_DIV_NUM    0x000000FFU
#define LCD_CAM_CAM_CLKM_DIV_NUM_M  (LCD_CAM_CAM_CLKM_DIV_NUM_V << LCD_CAM_CAM_CLKM_DIV_NUM_S)
#define LCD_CAM_CAM_CLKM_DIV_NUM_V  0x000000FFU
#define LCD_CAM_CAM_CLKM_DIV_NUM_S  9
/** LCD_CAM_CAM_CLKM_DIV_B : R/W; bitpos: [22:17]; default: 0;
 *  Fractional clock divider numerator value.
 */
#define LCD_CAM_CAM_CLKM_DIV_B    0x0000003FU
#define LCD_CAM_CAM_CLKM_DIV_B_M  (LCD_CAM_CAM_CLKM_DIV_B_V << LCD_CAM_CAM_CLKM_DIV_B_S)
#define LCD_CAM_CAM_CLKM_DIV_B_V  0x0000003FU
#define LCD_CAM_CAM_CLKM_DIV_B_S  17
/** LCD_CAM_CAM_CLKM_DIV_A : R/W; bitpos: [28:23]; default: 0;
 *  Fractional clock divider denominator value.
 */
#define LCD_CAM_CAM_CLKM_DIV_A    0x0000003FU
#define LCD_CAM_CAM_CLKM_DIV_A_M  (LCD_CAM_CAM_CLKM_DIV_A_V << LCD_CAM_CAM_CLKM_DIV_A_S)
#define LCD_CAM_CAM_CLKM_DIV_A_V  0x0000003FU
#define LCD_CAM_CAM_CLKM_DIV_A_S  23
/** LCD_CAM_CAM_CLK_SEL : R/W; bitpos: [30:29]; default: 0;
 *  Select camera module source clock. 0: Clock source is disabled. 1: XTAL_CLK. 2:
 *  PLL_D2_CLK. 3: PLL_F160M_CLK.
 */
#define LCD_CAM_CAM_CLK_SEL    0x00000003U
#define LCD_CAM_CAM_CLK_SEL_M  (LCD_CAM_CAM_CLK_SEL_V << LCD_CAM_CAM_CLK_SEL_S)
#define LCD_CAM_CAM_CLK_SEL_V  0x00000003U
#define LCD_CAM_CAM_CLK_SEL_S  29

/** LCD_CAM_CAM_CTRL1_REG register
 *  Camera control register
 */
#define LCD_CAM_CAM_CTRL1_REG (DR_REG_LCD_CAM_BASE + 0x8)
/** LCD_CAM_CAM_REC_DATA_BYTELEN : R/W; bitpos: [15:0]; default: 0;
 *  Configure camera received data byte length. When the length of received data
 *  reaches this value + 1, GDMA in_suc_eof_int is triggered.
 */
#define LCD_CAM_CAM_REC_DATA_BYTELEN    0x0000FFFFU
#define LCD_CAM_CAM_REC_DATA_BYTELEN_M  (LCD_CAM_CAM_REC_DATA_BYTELEN_V << LCD_CAM_CAM_REC_DATA_BYTELEN_S)
#define LCD_CAM_CAM_REC_DATA_BYTELEN_V  0x0000FFFFU
#define LCD_CAM_CAM_REC_DATA_BYTELEN_S  0
/** LCD_CAM_CAM_LINE_INT_NUM : R/W; bitpos: [21:16]; default: 0;
 *  Configure line number. When the number of received lines reaches this value + 1,
 *  LCD_CAM_CAM_HS_INT is triggered.
 */
#define LCD_CAM_CAM_LINE_INT_NUM    0x0000003FU
#define LCD_CAM_CAM_LINE_INT_NUM_M  (LCD_CAM_CAM_LINE_INT_NUM_V << LCD_CAM_CAM_LINE_INT_NUM_S)
#define LCD_CAM_CAM_LINE_INT_NUM_V  0x0000003FU
#define LCD_CAM_CAM_LINE_INT_NUM_S  16
/** LCD_CAM_CAM_CLK_INV : R/W; bitpos: [22]; default: 0;
 *  1: Invert the input signal CAM_PCLK. 0: Do not invert.
 */
#define LCD_CAM_CAM_CLK_INV    (BIT(22))
#define LCD_CAM_CAM_CLK_INV_M  (LCD_CAM_CAM_CLK_INV_V << LCD_CAM_CAM_CLK_INV_S)
#define LCD_CAM_CAM_CLK_INV_V  0x00000001U
#define LCD_CAM_CAM_CLK_INV_S  22
/** LCD_CAM_CAM_VSYNC_FILTER_EN : R/W; bitpos: [23]; default: 0;
 *  1: Enable CAM_VSYNC filter function. 0: Bypass.
 */
#define LCD_CAM_CAM_VSYNC_FILTER_EN    (BIT(23))
#define LCD_CAM_CAM_VSYNC_FILTER_EN_M  (LCD_CAM_CAM_VSYNC_FILTER_EN_V << LCD_CAM_CAM_VSYNC_FILTER_EN_S)
#define LCD_CAM_CAM_VSYNC_FILTER_EN_V  0x00000001U
#define LCD_CAM_CAM_VSYNC_FILTER_EN_S  23
/** LCD_CAM_CAM_2BYTE_EN : R/W; bitpos: [24]; default: 0;
 *  1: The width of input data is 16 bits. 0: The width of input data is 8 bits.
 */
#define LCD_CAM_CAM_2BYTE_EN    (BIT(24))
#define LCD_CAM_CAM_2BYTE_EN_M  (LCD_CAM_CAM_2BYTE_EN_V << LCD_CAM_CAM_2BYTE_EN_S)
#define LCD_CAM_CAM_2BYTE_EN_V  0x00000001U
#define LCD_CAM_CAM_2BYTE_EN_S  24
/** LCD_CAM_CAM_DE_INV : R/W; bitpos: [25]; default: 0;
 *  CAM_DE invert enable signal, valid in high level.
 */
#define LCD_CAM_CAM_DE_INV    (BIT(25))
#define LCD_CAM_CAM_DE_INV_M  (LCD_CAM_CAM_DE_INV_V << LCD_CAM_CAM_DE_INV_S)
#define LCD_CAM_CAM_DE_INV_V  0x00000001U
#define LCD_CAM_CAM_DE_INV_S  25
/** LCD_CAM_CAM_HSYNC_INV : R/W; bitpos: [26]; default: 0;
 *  CAM_HSYNC invert enable signal, valid in high level.
 */
#define LCD_CAM_CAM_HSYNC_INV    (BIT(26))
#define LCD_CAM_CAM_HSYNC_INV_M  (LCD_CAM_CAM_HSYNC_INV_V << LCD_CAM_CAM_HSYNC_INV_S)
#define LCD_CAM_CAM_HSYNC_INV_V  0x00000001U
#define LCD_CAM_CAM_HSYNC_INV_S  26
/** LCD_CAM_CAM_VSYNC_INV : R/W; bitpos: [27]; default: 0;
 *  CAM_VSYNC invert enable signal, valid in high level.
 */
#define LCD_CAM_CAM_VSYNC_INV    (BIT(27))
#define LCD_CAM_CAM_VSYNC_INV_M  (LCD_CAM_CAM_VSYNC_INV_V << LCD_CAM_CAM_VSYNC_INV_S)
#define LCD_CAM_CAM_VSYNC_INV_V  0x00000001U
#define LCD_CAM_CAM_VSYNC_INV_S  27
/** LCD_CAM_CAM_VH_DE_MODE_EN : R/W; bitpos: [28]; default: 0;
 *  1: Input control signals are CAM_DE and CAM_HSYNC. CAM_VSYNC is 1. 0: Input control
 *  signals are CAM_DE and CAM_VSYNC. CAM_HSYNC and CAM_DE are all 1 at the the same
 *  time.
 */
#define LCD_CAM_CAM_VH_DE_MODE_EN    (BIT(28))
#define LCD_CAM_CAM_VH_DE_MODE_EN_M  (LCD_CAM_CAM_VH_DE_MODE_EN_V << LCD_CAM_CAM_VH_DE_MODE_EN_S)
#define LCD_CAM_CAM_VH_DE_MODE_EN_V  0x00000001U
#define LCD_CAM_CAM_VH_DE_MODE_EN_S  28
/** LCD_CAM_CAM_START : R/W; bitpos: [29]; default: 0;
 *  Camera module start signal.
 */
#define LCD_CAM_CAM_START    (BIT(29))
#define LCD_CAM_CAM_START_M  (LCD_CAM_CAM_START_V << LCD_CAM_CAM_START_S)
#define LCD_CAM_CAM_START_V  0x00000001U
#define LCD_CAM_CAM_START_S  29
/** LCD_CAM_CAM_RESET : WO; bitpos: [30]; default: 0;
 *  Camera module reset signal.
 */
#define LCD_CAM_CAM_RESET    (BIT(30))
#define LCD_CAM_CAM_RESET_M  (LCD_CAM_CAM_RESET_V << LCD_CAM_CAM_RESET_S)
#define LCD_CAM_CAM_RESET_V  0x00000001U
#define LCD_CAM_CAM_RESET_S  30
/** LCD_CAM_CAM_AFIFO_RESET : WO; bitpos: [31]; default: 0;
 *  Camera Async Rx FIFO reset signal.
 */
#define LCD_CAM_CAM_AFIFO_RESET    (BIT(31))
#define LCD_CAM_CAM_AFIFO_RESET_M  (LCD_CAM_CAM_AFIFO_RESET_V << LCD_CAM_CAM_AFIFO_RESET_S)
#define LCD_CAM_CAM_AFIFO_RESET_V  0x00000001U
#define LCD_CAM_CAM_AFIFO_RESET_S  31

/** LCD_CAM_CAM_RGB_YUV_REG register
 *  Camera data format conversion register
 */
#define LCD_CAM_CAM_RGB_YUV_REG (DR_REG_LCD_CAM_BASE + 0xc)
/** LCD_CAM_CAM_CONV_8BITS_DATA_INV : R/W; bitpos: [21]; default: 0;
 *  Swap every two 8-bit input data. 1: Enabled. 0: Disabled.
 */
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV    (BIT(21))
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV_M  (LCD_CAM_CAM_CONV_8BITS_DATA_INV_V << LCD_CAM_CAM_CONV_8BITS_DATA_INV_S)
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV_V  0x00000001U
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV_S  21
/** LCD_CAM_CAM_CONV_YUV2YUV_MODE : R/W; bitpos: [23:22]; default: 0;
 *  In YUV-to-YUV mode, 0: data is converted to YUV422 format. 1: data is converted to
 *  YUV420 format. 2: data is converted to YUV411 format. 3: disabled. To enable
 *  YUV-to-YUV mode, LCD_CAM_CAM_CONV_TRANS_MODE must be set to 1.
 */
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE    0x00000003U
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE_M  (LCD_CAM_CAM_CONV_YUV2YUV_MODE_V << LCD_CAM_CAM_CONV_YUV2YUV_MODE_S)
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE_V  0x00000003U
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE_S  22
/** LCD_CAM_CAM_CONV_YUV_MODE : R/W; bitpos: [25:24]; default: 0;
 *  In YUV-to-YUV mode and YUV-to-RGB mode, LCD_CAM_CAM_CONV_YUV_MODE decides the YUV
 *  mode of input data. 0: input data is in YUV422 format. 1: input data is in YUV420
 *  format. 2: input data is in YUV411 format. In RGB-to-YUV mode, 0: data is converted
 *  to YUV422 format. 1: data is converted to YUV420 format. 2: data is converted to
 *  YUV411 format.
 */
#define LCD_CAM_CAM_CONV_YUV_MODE    0x00000003U
#define LCD_CAM_CAM_CONV_YUV_MODE_M  (LCD_CAM_CAM_CONV_YUV_MODE_V << LCD_CAM_CAM_CONV_YUV_MODE_S)
#define LCD_CAM_CAM_CONV_YUV_MODE_V  0x00000003U
#define LCD_CAM_CAM_CONV_YUV_MODE_S  24
/** LCD_CAM_CAM_CONV_PROTOCOL_MODE : R/W; bitpos: [26]; default: 0;
 *  0: BT601. 1: BT709.
 */
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE    (BIT(26))
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE_M  (LCD_CAM_CAM_CONV_PROTOCOL_MODE_V << LCD_CAM_CAM_CONV_PROTOCOL_MODE_S)
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE_V  0x00000001U
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE_S  26
/** LCD_CAM_CAM_CONV_DATA_OUT_MODE : R/W; bitpos: [27]; default: 0;
 *  Configure color range for output data. 0: limited color range. 1: full color range.
 */
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE    (BIT(27))
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE_M  (LCD_CAM_CAM_CONV_DATA_OUT_MODE_V << LCD_CAM_CAM_CONV_DATA_OUT_MODE_S)
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE_V  0x00000001U
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE_S  27
/** LCD_CAM_CAM_CONV_DATA_IN_MODE : R/W; bitpos: [28]; default: 0;
 *  Configure color range for input data. 0: limited color range. 1: full color range.
 */
#define LCD_CAM_CAM_CONV_DATA_IN_MODE    (BIT(28))
#define LCD_CAM_CAM_CONV_DATA_IN_MODE_M  (LCD_CAM_CAM_CONV_DATA_IN_MODE_V << LCD_CAM_CAM_CONV_DATA_IN_MODE_S)
#define LCD_CAM_CAM_CONV_DATA_IN_MODE_V  0x00000001U
#define LCD_CAM_CAM_CONV_DATA_IN_MODE_S  28
/** LCD_CAM_CAM_CONV_MODE_8BITS_ON : R/W; bitpos: [29]; default: 0;
 *  0: 16-bit mode. 1: 8-bit mode.
 */
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON    (BIT(29))
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON_M  (LCD_CAM_CAM_CONV_MODE_8BITS_ON_V << LCD_CAM_CAM_CONV_MODE_8BITS_ON_S)
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON_V  0x00000001U
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON_S  29
/** LCD_CAM_CAM_CONV_TRANS_MODE : R/W; bitpos: [30]; default: 0;
 *  0: converted to RGB format. 1: converted to YUV format.
 */
#define LCD_CAM_CAM_CONV_TRANS_MODE    (BIT(30))
#define LCD_CAM_CAM_CONV_TRANS_MODE_M  (LCD_CAM_CAM_CONV_TRANS_MODE_V << LCD_CAM_CAM_CONV_TRANS_MODE_S)
#define LCD_CAM_CAM_CONV_TRANS_MODE_V  0x00000001U
#define LCD_CAM_CAM_CONV_TRANS_MODE_S  30
/** LCD_CAM_CAM_CONV_BYPASS : R/W; bitpos: [31]; default: 0;
 *  0: Bypass converter. 1: Enable converter.
 */
#define LCD_CAM_CAM_CONV_BYPASS    (BIT(31))
#define LCD_CAM_CAM_CONV_BYPASS_M  (LCD_CAM_CAM_CONV_BYPASS_V << LCD_CAM_CAM_CONV_BYPASS_S)
#define LCD_CAM_CAM_CONV_BYPASS_V  0x00000001U
#define LCD_CAM_CAM_CONV_BYPASS_S  31

/** LCD_CAM_LCD_RGB_YUV_REG register
 *  LCD data format conversion register
 */
#define LCD_CAM_LCD_RGB_YUV_REG (DR_REG_LCD_CAM_BASE + 0x10)
/** LCD_CAM_LCD_CONV_8BITS_DATA_INV : R/W; bitpos: [20]; default: 0;
 *  Swap every two 8-bit input data. 1: Enabled. 0: Disabled.
 */
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV    (BIT(20))
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV_M  (LCD_CAM_LCD_CONV_8BITS_DATA_INV_V << LCD_CAM_LCD_CONV_8BITS_DATA_INV_S)
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV_V  0x00000001U
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV_S  20
/** LCD_CAM_LCD_CONV_TXTORX : R/W; bitpos: [21]; default: 0;
 *  0: txtorx mode off. 1: txtorx mode on.
 */
#define LCD_CAM_LCD_CONV_TXTORX    (BIT(21))
#define LCD_CAM_LCD_CONV_TXTORX_M  (LCD_CAM_LCD_CONV_TXTORX_V << LCD_CAM_LCD_CONV_TXTORX_S)
#define LCD_CAM_LCD_CONV_TXTORX_V  0x00000001U
#define LCD_CAM_LCD_CONV_TXTORX_S  21
/** LCD_CAM_LCD_CONV_YUV2YUV_MODE : R/W; bitpos: [23:22]; default: 0;
 *  In YUV-to-YUV mode, 0: data is converted to YUV422 format. 1: data is converted to
 *  YUV420 format. 2: data is converted to YUV411 format. 3: disabled. To enable
 *  YUV-to-YUV mode, LCD_CAM_LCD_CONV_TRANS_MODE must be set to 1.
 */
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE    0x00000003U
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE_M  (LCD_CAM_LCD_CONV_YUV2YUV_MODE_V << LCD_CAM_LCD_CONV_YUV2YUV_MODE_S)
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE_V  0x00000003U
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE_S  22
/** LCD_CAM_LCD_CONV_YUV_MODE : R/W; bitpos: [25:24]; default: 0;
 *  In YUV-to-YUV mode and YUV-to-RGB mode, LCD_CAM_LCD_CONV_YUV_MODE decides the YUV
 *  mode of input data. 0: input data is in YUV422 format. 1: input data is in YUV420
 *  format. 2: input data is in YUV411 format. In RGB-to-YUV mode, 0: data is converted
 *  to YUV422 format. 1: data is converted to YUV420 format. 2: data is converted to
 *  YUV411 format.
 */
#define LCD_CAM_LCD_CONV_YUV_MODE    0x00000003U
#define LCD_CAM_LCD_CONV_YUV_MODE_M  (LCD_CAM_LCD_CONV_YUV_MODE_V << LCD_CAM_LCD_CONV_YUV_MODE_S)
#define LCD_CAM_LCD_CONV_YUV_MODE_V  0x00000003U
#define LCD_CAM_LCD_CONV_YUV_MODE_S  24
/** LCD_CAM_LCD_CONV_PROTOCOL_MODE : R/W; bitpos: [26]; default: 0;
 *  0: BT601. 1: BT709.
 */
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE    (BIT(26))
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE_M  (LCD_CAM_LCD_CONV_PROTOCOL_MODE_V << LCD_CAM_LCD_CONV_PROTOCOL_MODE_S)
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE_V  0x00000001U
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE_S  26
/** LCD_CAM_LCD_CONV_DATA_OUT_MODE : R/W; bitpos: [27]; default: 0;
 *  Configure color range for output data. 0: limited color range. 1: full color range.
 */
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE    (BIT(27))
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE_M  (LCD_CAM_LCD_CONV_DATA_OUT_MODE_V << LCD_CAM_LCD_CONV_DATA_OUT_MODE_S)
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE_V  0x00000001U
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE_S  27
/** LCD_CAM_LCD_CONV_DATA_IN_MODE : R/W; bitpos: [28]; default: 0;
 *  Configure color range for input data. 0: limited color range. 1: full color range.
 */
#define LCD_CAM_LCD_CONV_DATA_IN_MODE    (BIT(28))
#define LCD_CAM_LCD_CONV_DATA_IN_MODE_M  (LCD_CAM_LCD_CONV_DATA_IN_MODE_V << LCD_CAM_LCD_CONV_DATA_IN_MODE_S)
#define LCD_CAM_LCD_CONV_DATA_IN_MODE_V  0x00000001U
#define LCD_CAM_LCD_CONV_DATA_IN_MODE_S  28
/** LCD_CAM_LCD_CONV_MODE_8BITS_ON : R/W; bitpos: [29]; default: 0;
 *  0: 16-bit mode. 1: 8-bit mode.
 */
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON    (BIT(29))
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON_M  (LCD_CAM_LCD_CONV_MODE_8BITS_ON_V << LCD_CAM_LCD_CONV_MODE_8BITS_ON_S)
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON_V  0x00000001U
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON_S  29
/** LCD_CAM_LCD_CONV_TRANS_MODE : R/W; bitpos: [30]; default: 0;
 *  0: converted to RGB format. 1: converted to YUV format.
 */
#define LCD_CAM_LCD_CONV_TRANS_MODE    (BIT(30))
#define LCD_CAM_LCD_CONV_TRANS_MODE_M  (LCD_CAM_LCD_CONV_TRANS_MODE_V << LCD_CAM_LCD_CONV_TRANS_MODE_S)
#define LCD_CAM_LCD_CONV_TRANS_MODE_V  0x00000001U
#define LCD_CAM_LCD_CONV_TRANS_MODE_S  30
/** LCD_CAM_LCD_CONV_BYPASS : R/W; bitpos: [31]; default: 0;
 *  0: Bypass converter. 1: Enable converter.
 */
#define LCD_CAM_LCD_CONV_BYPASS    (BIT(31))
#define LCD_CAM_LCD_CONV_BYPASS_M  (LCD_CAM_LCD_CONV_BYPASS_V << LCD_CAM_LCD_CONV_BYPASS_S)
#define LCD_CAM_LCD_CONV_BYPASS_V  0x00000001U
#define LCD_CAM_LCD_CONV_BYPASS_S  31

/** LCD_CAM_LCD_USER_REG register
 *  LCD user configuration register
 */
#define LCD_CAM_LCD_USER_REG (DR_REG_LCD_CAM_BASE + 0x14)
/** LCD_CAM_LCD_DOUT_CYCLELEN : R/W; bitpos: [12:0]; default: 0;
 *  Configure the cycles for DOUT phase of LCD module. The cycles =  this value + 1.
 */
#define LCD_CAM_LCD_DOUT_CYCLELEN    0x00001FFFU
#define LCD_CAM_LCD_DOUT_CYCLELEN_M  (LCD_CAM_LCD_DOUT_CYCLELEN_V << LCD_CAM_LCD_DOUT_CYCLELEN_S)
#define LCD_CAM_LCD_DOUT_CYCLELEN_V  0x00001FFFU
#define LCD_CAM_LCD_DOUT_CYCLELEN_S  0
/** LCD_CAM_LCD_ALWAYS_OUT_EN : R/W; bitpos: [13]; default: 0;
 *  LCD continues outputting data when LCD is in DOUT phase, till LCD_CAM_LCD_START is
 *  cleared or LCD_CAM_LCD_RESET is set.
 */
#define LCD_CAM_LCD_ALWAYS_OUT_EN    (BIT(13))
#define LCD_CAM_LCD_ALWAYS_OUT_EN_M  (LCD_CAM_LCD_ALWAYS_OUT_EN_V << LCD_CAM_LCD_ALWAYS_OUT_EN_S)
#define LCD_CAM_LCD_ALWAYS_OUT_EN_V  0x00000001U
#define LCD_CAM_LCD_ALWAYS_OUT_EN_S  13
/** LCD_CAM_LCD_8BITS_ORDER : R/W; bitpos: [19]; default: 0;
 *  1: Swap every two data bytes, valid in 8-bit mode. 0: Do not swap.
 */
#define LCD_CAM_LCD_8BITS_ORDER    (BIT(19))
#define LCD_CAM_LCD_8BITS_ORDER_M  (LCD_CAM_LCD_8BITS_ORDER_V << LCD_CAM_LCD_8BITS_ORDER_S)
#define LCD_CAM_LCD_8BITS_ORDER_V  0x00000001U
#define LCD_CAM_LCD_8BITS_ORDER_S  19
/** LCD_CAM_LCD_UPDATE : R/W; bitpos: [20]; default: 0;
 *  1: Update LCD registers. This bit is cleared by hardware. 0: Do not care.
 */
#define LCD_CAM_LCD_UPDATE    (BIT(20))
#define LCD_CAM_LCD_UPDATE_M  (LCD_CAM_LCD_UPDATE_V << LCD_CAM_LCD_UPDATE_S)
#define LCD_CAM_LCD_UPDATE_V  0x00000001U
#define LCD_CAM_LCD_UPDATE_S  20
/** LCD_CAM_LCD_BIT_ORDER : R/W; bitpos: [21]; default: 0;
 *  1: Change data bit order. Change LCD_DATA_out[7:0] to LCD_DATA_out[0:7] in 8-bit
 *  mode, and bits[15:0] to bits[0:15] in 16-bit mode. 0: Do not change.
 */
#define LCD_CAM_LCD_BIT_ORDER    (BIT(21))
#define LCD_CAM_LCD_BIT_ORDER_M  (LCD_CAM_LCD_BIT_ORDER_V << LCD_CAM_LCD_BIT_ORDER_S)
#define LCD_CAM_LCD_BIT_ORDER_V  0x00000001U
#define LCD_CAM_LCD_BIT_ORDER_S  21
/** LCD_CAM_LCD_BYTE_ORDER : R/W; bitpos: [22]; default: 0;
 *  1: Invert data byte order, only valid in 16-bit mode. 0: Do not invert.
 */
#define LCD_CAM_LCD_BYTE_ORDER    (BIT(22))
#define LCD_CAM_LCD_BYTE_ORDER_M  (LCD_CAM_LCD_BYTE_ORDER_V << LCD_CAM_LCD_BYTE_ORDER_S)
#define LCD_CAM_LCD_BYTE_ORDER_V  0x00000001U
#define LCD_CAM_LCD_BYTE_ORDER_S  22
/** LCD_CAM_LCD_2BYTE_EN : R/W; bitpos: [23]; default: 0;
 *  1: The width of output LCD data is 16 bits. 0: The width of output LCD data is 8
 *  bits.
 */
#define LCD_CAM_LCD_2BYTE_EN    (BIT(23))
#define LCD_CAM_LCD_2BYTE_EN_M  (LCD_CAM_LCD_2BYTE_EN_V << LCD_CAM_LCD_2BYTE_EN_S)
#define LCD_CAM_LCD_2BYTE_EN_V  0x00000001U
#define LCD_CAM_LCD_2BYTE_EN_S  23
/** LCD_CAM_LCD_DOUT : R/W; bitpos: [24]; default: 0;
 *  1: Be able to send data out in LCD sequence when LCD starts. 0: Disable.
 */
#define LCD_CAM_LCD_DOUT    (BIT(24))
#define LCD_CAM_LCD_DOUT_M  (LCD_CAM_LCD_DOUT_V << LCD_CAM_LCD_DOUT_S)
#define LCD_CAM_LCD_DOUT_V  0x00000001U
#define LCD_CAM_LCD_DOUT_S  24
/** LCD_CAM_LCD_DUMMY : R/W; bitpos: [25]; default: 0;
 *  1: Enable DUMMY phase in LCD sequence when LCD starts. 0: Disable.
 */
#define LCD_CAM_LCD_DUMMY    (BIT(25))
#define LCD_CAM_LCD_DUMMY_M  (LCD_CAM_LCD_DUMMY_V << LCD_CAM_LCD_DUMMY_S)
#define LCD_CAM_LCD_DUMMY_V  0x00000001U
#define LCD_CAM_LCD_DUMMY_S  25
/** LCD_CAM_LCD_CMD : R/W; bitpos: [26]; default: 0;
 *  1: Be able to send command in LCD sequence when LCD starts. 0: Disable.
 */
#define LCD_CAM_LCD_CMD    (BIT(26))
#define LCD_CAM_LCD_CMD_M  (LCD_CAM_LCD_CMD_V << LCD_CAM_LCD_CMD_S)
#define LCD_CAM_LCD_CMD_V  0x00000001U
#define LCD_CAM_LCD_CMD_S  26
/** LCD_CAM_LCD_START : R/W; bitpos: [27]; default: 0;
 *  LCD starts sending data enable signal, valid in high level.
 */
#define LCD_CAM_LCD_START    (BIT(27))
#define LCD_CAM_LCD_START_M  (LCD_CAM_LCD_START_V << LCD_CAM_LCD_START_S)
#define LCD_CAM_LCD_START_V  0x00000001U
#define LCD_CAM_LCD_START_S  27
/** LCD_CAM_LCD_RESET : WO; bitpos: [28]; default: 0;
 *  Reset LCD module.
 */
#define LCD_CAM_LCD_RESET    (BIT(28))
#define LCD_CAM_LCD_RESET_M  (LCD_CAM_LCD_RESET_V << LCD_CAM_LCD_RESET_S)
#define LCD_CAM_LCD_RESET_V  0x00000001U
#define LCD_CAM_LCD_RESET_S  28
/** LCD_CAM_LCD_DUMMY_CYCLELEN : R/W; bitpos: [30:29]; default: 0;
 *  Configure DUMMY cycles. DUMMY cycles = this value + 1.
 */
#define LCD_CAM_LCD_DUMMY_CYCLELEN    0x00000003U
#define LCD_CAM_LCD_DUMMY_CYCLELEN_M  (LCD_CAM_LCD_DUMMY_CYCLELEN_V << LCD_CAM_LCD_DUMMY_CYCLELEN_S)
#define LCD_CAM_LCD_DUMMY_CYCLELEN_V  0x00000003U
#define LCD_CAM_LCD_DUMMY_CYCLELEN_S  29
/** LCD_CAM_LCD_CMD_2_CYCLE_EN : R/W; bitpos: [31]; default: 0;
 *  The cycle length of command phase. 1: 2 cycles. 0: 1 cycle.
 */
#define LCD_CAM_LCD_CMD_2_CYCLE_EN    (BIT(31))
#define LCD_CAM_LCD_CMD_2_CYCLE_EN_M  (LCD_CAM_LCD_CMD_2_CYCLE_EN_V << LCD_CAM_LCD_CMD_2_CYCLE_EN_S)
#define LCD_CAM_LCD_CMD_2_CYCLE_EN_V  0x00000001U
#define LCD_CAM_LCD_CMD_2_CYCLE_EN_S  31

/** LCD_CAM_LCD_MISC_REG register
 *  LCD MISC configuration register
 */
#define LCD_CAM_LCD_MISC_REG (DR_REG_LCD_CAM_BASE + 0x18)
/** LCD_CAM_LCD_AFIFO_THRESHOLD_NUM : R/W; bitpos: [5:1]; default: 17;
 *  Set the threshold for Async Tx FIFO full event.
 */
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM    0x0000001FU
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_M  (LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_V << LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_S)
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_V  0x0000001FU
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_S  1
/** LCD_CAM_LCD_VFK_CYCLELEN : R/W; bitpos: [11:6]; default: 0;
 *  Configure the setup cycles in LCD non-RGB mode. Setup cycles expected = this value
 *  + 1.
 */
#define LCD_CAM_LCD_VFK_CYCLELEN    0x0000003FU
#define LCD_CAM_LCD_VFK_CYCLELEN_M  (LCD_CAM_LCD_VFK_CYCLELEN_V << LCD_CAM_LCD_VFK_CYCLELEN_S)
#define LCD_CAM_LCD_VFK_CYCLELEN_V  0x0000003FU
#define LCD_CAM_LCD_VFK_CYCLELEN_S  6
/** LCD_CAM_LCD_VBK_CYCLELEN : R/W; bitpos: [24:12]; default: 0;
 *  Configure the hold time cycles in LCD non-RGB mode. Hold cycles expected = this
 *  value + 1. %Configure the cycles for vertical back blank region in LCD RGB mode,
 *  the cycles = this value + 1. Or configure the hold time cycles in LCD non-RGB mode,
 *  the cycles = this value + 1.
 */
#define LCD_CAM_LCD_VBK_CYCLELEN    0x00001FFFU
#define LCD_CAM_LCD_VBK_CYCLELEN_M  (LCD_CAM_LCD_VBK_CYCLELEN_V << LCD_CAM_LCD_VBK_CYCLELEN_S)
#define LCD_CAM_LCD_VBK_CYCLELEN_V  0x00001FFFU
#define LCD_CAM_LCD_VBK_CYCLELEN_S  12
/** LCD_CAM_LCD_NEXT_FRAME_EN : R/W; bitpos: [25]; default: 0;
 *  1: Send the next frame data when the current frame is sent out. 0: LCD stops when
 *  the current frame is sent out.
 */
#define LCD_CAM_LCD_NEXT_FRAME_EN    (BIT(25))
#define LCD_CAM_LCD_NEXT_FRAME_EN_M  (LCD_CAM_LCD_NEXT_FRAME_EN_V << LCD_CAM_LCD_NEXT_FRAME_EN_S)
#define LCD_CAM_LCD_NEXT_FRAME_EN_V  0x00000001U
#define LCD_CAM_LCD_NEXT_FRAME_EN_S  25
/** LCD_CAM_LCD_BK_EN : R/W; bitpos: [26]; default: 0;
 *  1: Enable blank region when LCD sends data out. 0: No blank region.
 */
#define LCD_CAM_LCD_BK_EN    (BIT(26))
#define LCD_CAM_LCD_BK_EN_M  (LCD_CAM_LCD_BK_EN_V << LCD_CAM_LCD_BK_EN_S)
#define LCD_CAM_LCD_BK_EN_V  0x00000001U
#define LCD_CAM_LCD_BK_EN_S  26
/** LCD_CAM_LCD_AFIFO_RESET : WO; bitpos: [27]; default: 0;
 *  Async Tx FIFO reset signal.
 */
#define LCD_CAM_LCD_AFIFO_RESET    (BIT(27))
#define LCD_CAM_LCD_AFIFO_RESET_M  (LCD_CAM_LCD_AFIFO_RESET_V << LCD_CAM_LCD_AFIFO_RESET_S)
#define LCD_CAM_LCD_AFIFO_RESET_V  0x00000001U
#define LCD_CAM_LCD_AFIFO_RESET_S  27
/** LCD_CAM_LCD_CD_DATA_SET : R/W; bitpos: [28]; default: 0;
 *  1: LCD_CD = !LCD_CAM_LCD_CD_IDLE_EDGE when LCD is in DOUT phase. 0: LCD_CD =
 *  LCD_CAM_LCD_CD_IDLE_EDGE.
 */
#define LCD_CAM_LCD_CD_DATA_SET    (BIT(28))
#define LCD_CAM_LCD_CD_DATA_SET_M  (LCD_CAM_LCD_CD_DATA_SET_V << LCD_CAM_LCD_CD_DATA_SET_S)
#define LCD_CAM_LCD_CD_DATA_SET_V  0x00000001U
#define LCD_CAM_LCD_CD_DATA_SET_S  28
/** LCD_CAM_LCD_CD_DUMMY_SET : R/W; bitpos: [29]; default: 0;
 *  1: LCD_CD = !LCD_CAM_LCD_CD_IDLE_EDGE when LCD is in DUMMY phase. 0: LCD_CD =
 *  LCD_CAM_LCD_CD_IDLE_EDGE.
 */
#define LCD_CAM_LCD_CD_DUMMY_SET    (BIT(29))
#define LCD_CAM_LCD_CD_DUMMY_SET_M  (LCD_CAM_LCD_CD_DUMMY_SET_V << LCD_CAM_LCD_CD_DUMMY_SET_S)
#define LCD_CAM_LCD_CD_DUMMY_SET_V  0x00000001U
#define LCD_CAM_LCD_CD_DUMMY_SET_S  29
/** LCD_CAM_LCD_CD_CMD_SET : R/W; bitpos: [30]; default: 0;
 *  1: LCD_CD = !LCD_CAM_LCD_CD_IDLE_EDGE when LCD is in CMD phase. 0: LCD_CD =
 *  LCD_CAM_LCD_CD_IDLE_EDGE.
 */
#define LCD_CAM_LCD_CD_CMD_SET    (BIT(30))
#define LCD_CAM_LCD_CD_CMD_SET_M  (LCD_CAM_LCD_CD_CMD_SET_V << LCD_CAM_LCD_CD_CMD_SET_S)
#define LCD_CAM_LCD_CD_CMD_SET_V  0x00000001U
#define LCD_CAM_LCD_CD_CMD_SET_S  30
/** LCD_CAM_LCD_CD_IDLE_EDGE : R/W; bitpos: [31]; default: 0;
 *  The default value of LCD_CD.
 */
#define LCD_CAM_LCD_CD_IDLE_EDGE    (BIT(31))
#define LCD_CAM_LCD_CD_IDLE_EDGE_M  (LCD_CAM_LCD_CD_IDLE_EDGE_V << LCD_CAM_LCD_CD_IDLE_EDGE_S)
#define LCD_CAM_LCD_CD_IDLE_EDGE_V  0x00000001U
#define LCD_CAM_LCD_CD_IDLE_EDGE_S  31

/** LCD_CAM_LCD_CTRL_REG register
 *  LCD signal configuration register
 */
#define LCD_CAM_LCD_CTRL_REG (DR_REG_LCD_CAM_BASE + 0x1c)
/** LCD_CAM_LCD_HB_FRONT : R/W; bitpos: [10:0]; default: 0;
 *  It is the horizontal blank front porch of a frame.
 */
#define LCD_CAM_LCD_HB_FRONT    0x000007FFU
#define LCD_CAM_LCD_HB_FRONT_M  (LCD_CAM_LCD_HB_FRONT_V << LCD_CAM_LCD_HB_FRONT_S)
#define LCD_CAM_LCD_HB_FRONT_V  0x000007FFU
#define LCD_CAM_LCD_HB_FRONT_S  0
/** LCD_CAM_LCD_VA_HEIGHT : R/W; bitpos: [20:11]; default: 0;
 *  It is the vertical active height of a frame.
 */
#define LCD_CAM_LCD_VA_HEIGHT    0x000003FFU
#define LCD_CAM_LCD_VA_HEIGHT_M  (LCD_CAM_LCD_VA_HEIGHT_V << LCD_CAM_LCD_VA_HEIGHT_S)
#define LCD_CAM_LCD_VA_HEIGHT_V  0x000003FFU
#define LCD_CAM_LCD_VA_HEIGHT_S  11
/** LCD_CAM_LCD_VT_HEIGHT : R/W; bitpos: [30:21]; default: 0;
 *  It is the vertical total height of a frame.
 */
#define LCD_CAM_LCD_VT_HEIGHT    0x000003FFU
#define LCD_CAM_LCD_VT_HEIGHT_M  (LCD_CAM_LCD_VT_HEIGHT_V << LCD_CAM_LCD_VT_HEIGHT_S)
#define LCD_CAM_LCD_VT_HEIGHT_V  0x000003FFU
#define LCD_CAM_LCD_VT_HEIGHT_S  21
/** LCD_CAM_LCD_RGB_MODE_EN : R/W; bitpos: [31]; default: 0;
 *  1: Enable RGB mode, and input VSYNC, HSYNC, and DE signals. 0: Disable.
 */
#define LCD_CAM_LCD_RGB_MODE_EN    (BIT(31))
#define LCD_CAM_LCD_RGB_MODE_EN_M  (LCD_CAM_LCD_RGB_MODE_EN_V << LCD_CAM_LCD_RGB_MODE_EN_S)
#define LCD_CAM_LCD_RGB_MODE_EN_V  0x00000001U
#define LCD_CAM_LCD_RGB_MODE_EN_S  31

/** LCD_CAM_LCD_CTRL1_REG register
 *  LCD signal configuration register 1
 */
#define LCD_CAM_LCD_CTRL1_REG (DR_REG_LCD_CAM_BASE + 0x20)
/** LCD_CAM_LCD_VB_FRONT : R/W; bitpos: [7:0]; default: 0;
 *  It is the vertical blank front porch of a frame.
 */
#define LCD_CAM_LCD_VB_FRONT    0x000000FFU
#define LCD_CAM_LCD_VB_FRONT_M  (LCD_CAM_LCD_VB_FRONT_V << LCD_CAM_LCD_VB_FRONT_S)
#define LCD_CAM_LCD_VB_FRONT_V  0x000000FFU
#define LCD_CAM_LCD_VB_FRONT_S  0
/** LCD_CAM_LCD_HA_WIDTH : R/W; bitpos: [19:8]; default: 0;
 *  It is the horizontal active width of a frame.
 */
#define LCD_CAM_LCD_HA_WIDTH    0x00000FFFU
#define LCD_CAM_LCD_HA_WIDTH_M  (LCD_CAM_LCD_HA_WIDTH_V << LCD_CAM_LCD_HA_WIDTH_S)
#define LCD_CAM_LCD_HA_WIDTH_V  0x00000FFFU
#define LCD_CAM_LCD_HA_WIDTH_S  8
/** LCD_CAM_LCD_HT_WIDTH : R/W; bitpos: [31:20]; default: 0;
 *  It is the horizontal total width of a frame.
 */
#define LCD_CAM_LCD_HT_WIDTH    0x00000FFFU
#define LCD_CAM_LCD_HT_WIDTH_M  (LCD_CAM_LCD_HT_WIDTH_V << LCD_CAM_LCD_HT_WIDTH_S)
#define LCD_CAM_LCD_HT_WIDTH_V  0x00000FFFU
#define LCD_CAM_LCD_HT_WIDTH_S  20

/** LCD_CAM_LCD_CTRL2_REG register
 *  LCD signal configuration register 2
 */
#define LCD_CAM_LCD_CTRL2_REG (DR_REG_LCD_CAM_BASE + 0x24)
/** LCD_CAM_LCD_VSYNC_WIDTH : R/W; bitpos: [6:0]; default: 0;
 *  It is the width of LCD_VSYNC active pulse in a line.
 */
#define LCD_CAM_LCD_VSYNC_WIDTH    0x0000007FU
#define LCD_CAM_LCD_VSYNC_WIDTH_M  (LCD_CAM_LCD_VSYNC_WIDTH_V << LCD_CAM_LCD_VSYNC_WIDTH_S)
#define LCD_CAM_LCD_VSYNC_WIDTH_V  0x0000007FU
#define LCD_CAM_LCD_VSYNC_WIDTH_S  0
/** LCD_CAM_LCD_VSYNC_IDLE_POL : R/W; bitpos: [7]; default: 0;
 *  It is the idle value of LCD_VSYNC.
 */
#define LCD_CAM_LCD_VSYNC_IDLE_POL    (BIT(7))
#define LCD_CAM_LCD_VSYNC_IDLE_POL_M  (LCD_CAM_LCD_VSYNC_IDLE_POL_V << LCD_CAM_LCD_VSYNC_IDLE_POL_S)
#define LCD_CAM_LCD_VSYNC_IDLE_POL_V  0x00000001U
#define LCD_CAM_LCD_VSYNC_IDLE_POL_S  7
/** LCD_CAM_LCD_DE_IDLE_POL : R/W; bitpos: [8]; default: 0;
 *  It is the idle value of LCD_DE.
 */
#define LCD_CAM_LCD_DE_IDLE_POL    (BIT(8))
#define LCD_CAM_LCD_DE_IDLE_POL_M  (LCD_CAM_LCD_DE_IDLE_POL_V << LCD_CAM_LCD_DE_IDLE_POL_S)
#define LCD_CAM_LCD_DE_IDLE_POL_V  0x00000001U
#define LCD_CAM_LCD_DE_IDLE_POL_S  8
/** LCD_CAM_LCD_HS_BLANK_EN : R/W; bitpos: [9]; default: 0;
 *  1: The pulse of LCD_HSYNC is out in vertical blanking lines in RGB mode. 0:
 *  LCD_HSYNC pulse is valid only in active region lines in RGB mode.
 */
#define LCD_CAM_LCD_HS_BLANK_EN    (BIT(9))
#define LCD_CAM_LCD_HS_BLANK_EN_M  (LCD_CAM_LCD_HS_BLANK_EN_V << LCD_CAM_LCD_HS_BLANK_EN_S)
#define LCD_CAM_LCD_HS_BLANK_EN_V  0x00000001U
#define LCD_CAM_LCD_HS_BLANK_EN_S  9
/** LCD_CAM_LCD_HSYNC_WIDTH : R/W; bitpos: [22:16]; default: 0;
 *  It is the width of LCD_HSYNC active pulse in a line.
 */
#define LCD_CAM_LCD_HSYNC_WIDTH    0x0000007FU
#define LCD_CAM_LCD_HSYNC_WIDTH_M  (LCD_CAM_LCD_HSYNC_WIDTH_V << LCD_CAM_LCD_HSYNC_WIDTH_S)
#define LCD_CAM_LCD_HSYNC_WIDTH_V  0x0000007FU
#define LCD_CAM_LCD_HSYNC_WIDTH_S  16
/** LCD_CAM_LCD_HSYNC_IDLE_POL : R/W; bitpos: [23]; default: 0;
 *  It is the idle value of LCD_HSYNC.
 */
#define LCD_CAM_LCD_HSYNC_IDLE_POL    (BIT(23))
#define LCD_CAM_LCD_HSYNC_IDLE_POL_M  (LCD_CAM_LCD_HSYNC_IDLE_POL_V << LCD_CAM_LCD_HSYNC_IDLE_POL_S)
#define LCD_CAM_LCD_HSYNC_IDLE_POL_V  0x00000001U
#define LCD_CAM_LCD_HSYNC_IDLE_POL_S  23
/** LCD_CAM_LCD_HSYNC_POSITION : R/W; bitpos: [31:24]; default: 0;
 *  It is the position of LCD_HSYNC active pulse in a line.
 */
#define LCD_CAM_LCD_HSYNC_POSITION    0x000000FFU
#define LCD_CAM_LCD_HSYNC_POSITION_M  (LCD_CAM_LCD_HSYNC_POSITION_V << LCD_CAM_LCD_HSYNC_POSITION_S)
#define LCD_CAM_LCD_HSYNC_POSITION_V  0x000000FFU
#define LCD_CAM_LCD_HSYNC_POSITION_S  24

/** LCD_CAM_LCD_CMD_VAL_REG register
 *  LCD command value configuration register
 */
#define LCD_CAM_LCD_CMD_VAL_REG (DR_REG_LCD_CAM_BASE + 0x28)
/** LCD_CAM_LCD_CMD_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  The LCD write command value.
 */
#define LCD_CAM_LCD_CMD_VALUE    0xFFFFFFFFU
#define LCD_CAM_LCD_CMD_VALUE_M  (LCD_CAM_LCD_CMD_VALUE_V << LCD_CAM_LCD_CMD_VALUE_S)
#define LCD_CAM_LCD_CMD_VALUE_V  0xFFFFFFFFU
#define LCD_CAM_LCD_CMD_VALUE_S  0

/** LCD_CAM_LCD_DLY_MODE_REG register
 *  LCD signal delay configuration register
 */
#define LCD_CAM_LCD_DLY_MODE_REG (DR_REG_LCD_CAM_BASE + 0x30)
/** LCD_CAM_LCD_CD_MODE : R/W; bitpos: [1:0]; default: 0;
 *  The output LCD_CD is delayed by module clock LCD_CLK. 0: output without delay. 1:
 *  delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_LCD_CD_MODE    0x00000003U
#define LCD_CAM_LCD_CD_MODE_M  (LCD_CAM_LCD_CD_MODE_V << LCD_CAM_LCD_CD_MODE_S)
#define LCD_CAM_LCD_CD_MODE_V  0x00000003U
#define LCD_CAM_LCD_CD_MODE_S  0
/** LCD_CAM_LCD_DE_MODE : R/W; bitpos: [3:2]; default: 0;
 *  The output LCD_DE is delayed by module clock LCD_CLK. 0: output without delay. 1:
 *  delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_LCD_DE_MODE    0x00000003U
#define LCD_CAM_LCD_DE_MODE_M  (LCD_CAM_LCD_DE_MODE_V << LCD_CAM_LCD_DE_MODE_S)
#define LCD_CAM_LCD_DE_MODE_V  0x00000003U
#define LCD_CAM_LCD_DE_MODE_S  2
/** LCD_CAM_LCD_HSYNC_MODE : R/W; bitpos: [5:4]; default: 0;
 *  The output LCD_HSYNC is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_LCD_HSYNC_MODE    0x00000003U
#define LCD_CAM_LCD_HSYNC_MODE_M  (LCD_CAM_LCD_HSYNC_MODE_V << LCD_CAM_LCD_HSYNC_MODE_S)
#define LCD_CAM_LCD_HSYNC_MODE_V  0x00000003U
#define LCD_CAM_LCD_HSYNC_MODE_S  4
/** LCD_CAM_LCD_VSYNC_MODE : R/W; bitpos: [7:6]; default: 0;
 *  The output LCD_VSYNC is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delay by the falling edge of LCD_CLK.
 */
#define LCD_CAM_LCD_VSYNC_MODE    0x00000003U
#define LCD_CAM_LCD_VSYNC_MODE_M  (LCD_CAM_LCD_VSYNC_MODE_V << LCD_CAM_LCD_VSYNC_MODE_S)
#define LCD_CAM_LCD_VSYNC_MODE_V  0x00000003U
#define LCD_CAM_LCD_VSYNC_MODE_S  6

/** LCD_CAM_LCD_DATA_DOUT_MODE_REG register
 *  LCD data delay configuration register
 */
#define LCD_CAM_LCD_DATA_DOUT_MODE_REG (DR_REG_LCD_CAM_BASE + 0x38)
/** LCD_CAM_DOUT0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  The output data bit 0 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT0_MODE    0x00000003U
#define LCD_CAM_DOUT0_MODE_M  (LCD_CAM_DOUT0_MODE_V << LCD_CAM_DOUT0_MODE_S)
#define LCD_CAM_DOUT0_MODE_V  0x00000003U
#define LCD_CAM_DOUT0_MODE_S  0
/** LCD_CAM_DOUT1_MODE : R/W; bitpos: [3:2]; default: 0;
 *  The output data bit 1 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT1_MODE    0x00000003U
#define LCD_CAM_DOUT1_MODE_M  (LCD_CAM_DOUT1_MODE_V << LCD_CAM_DOUT1_MODE_S)
#define LCD_CAM_DOUT1_MODE_V  0x00000003U
#define LCD_CAM_DOUT1_MODE_S  2
/** LCD_CAM_DOUT2_MODE : R/W; bitpos: [5:4]; default: 0;
 *  The output data bit 2 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT2_MODE    0x00000003U
#define LCD_CAM_DOUT2_MODE_M  (LCD_CAM_DOUT2_MODE_V << LCD_CAM_DOUT2_MODE_S)
#define LCD_CAM_DOUT2_MODE_V  0x00000003U
#define LCD_CAM_DOUT2_MODE_S  4
/** LCD_CAM_DOUT3_MODE : R/W; bitpos: [7:6]; default: 0;
 *  The output data bit 3 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT3_MODE    0x00000003U
#define LCD_CAM_DOUT3_MODE_M  (LCD_CAM_DOUT3_MODE_V << LCD_CAM_DOUT3_MODE_S)
#define LCD_CAM_DOUT3_MODE_V  0x00000003U
#define LCD_CAM_DOUT3_MODE_S  6
/** LCD_CAM_DOUT4_MODE : R/W; bitpos: [9:8]; default: 0;
 *  The output data bit 4 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT4_MODE    0x00000003U
#define LCD_CAM_DOUT4_MODE_M  (LCD_CAM_DOUT4_MODE_V << LCD_CAM_DOUT4_MODE_S)
#define LCD_CAM_DOUT4_MODE_V  0x00000003U
#define LCD_CAM_DOUT4_MODE_S  8
/** LCD_CAM_DOUT5_MODE : R/W; bitpos: [11:10]; default: 0;
 *  The output data bit 5 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT5_MODE    0x00000003U
#define LCD_CAM_DOUT5_MODE_M  (LCD_CAM_DOUT5_MODE_V << LCD_CAM_DOUT5_MODE_S)
#define LCD_CAM_DOUT5_MODE_V  0x00000003U
#define LCD_CAM_DOUT5_MODE_S  10
/** LCD_CAM_DOUT6_MODE : R/W; bitpos: [13:12]; default: 0;
 *  The output data bit 6 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT6_MODE    0x00000003U
#define LCD_CAM_DOUT6_MODE_M  (LCD_CAM_DOUT6_MODE_V << LCD_CAM_DOUT6_MODE_S)
#define LCD_CAM_DOUT6_MODE_V  0x00000003U
#define LCD_CAM_DOUT6_MODE_S  12
/** LCD_CAM_DOUT7_MODE : R/W; bitpos: [15:14]; default: 0;
 *  The output data bit 7 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT7_MODE    0x00000003U
#define LCD_CAM_DOUT7_MODE_M  (LCD_CAM_DOUT7_MODE_V << LCD_CAM_DOUT7_MODE_S)
#define LCD_CAM_DOUT7_MODE_V  0x00000003U
#define LCD_CAM_DOUT7_MODE_S  14
/** LCD_CAM_DOUT8_MODE : R/W; bitpos: [17:16]; default: 0;
 *  The output data bit 8 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT8_MODE    0x00000003U
#define LCD_CAM_DOUT8_MODE_M  (LCD_CAM_DOUT8_MODE_V << LCD_CAM_DOUT8_MODE_S)
#define LCD_CAM_DOUT8_MODE_V  0x00000003U
#define LCD_CAM_DOUT8_MODE_S  16
/** LCD_CAM_DOUT9_MODE : R/W; bitpos: [19:18]; default: 0;
 *  The output data bit 9 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT9_MODE    0x00000003U
#define LCD_CAM_DOUT9_MODE_M  (LCD_CAM_DOUT9_MODE_V << LCD_CAM_DOUT9_MODE_S)
#define LCD_CAM_DOUT9_MODE_V  0x00000003U
#define LCD_CAM_DOUT9_MODE_S  18
/** LCD_CAM_DOUT10_MODE : R/W; bitpos: [21:20]; default: 0;
 *  The output data bit 10 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT10_MODE    0x00000003U
#define LCD_CAM_DOUT10_MODE_M  (LCD_CAM_DOUT10_MODE_V << LCD_CAM_DOUT10_MODE_S)
#define LCD_CAM_DOUT10_MODE_V  0x00000003U
#define LCD_CAM_DOUT10_MODE_S  20
/** LCD_CAM_DOUT11_MODE : R/W; bitpos: [23:22]; default: 0;
 *  The output data bit 11 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT11_MODE    0x00000003U
#define LCD_CAM_DOUT11_MODE_M  (LCD_CAM_DOUT11_MODE_V << LCD_CAM_DOUT11_MODE_S)
#define LCD_CAM_DOUT11_MODE_V  0x00000003U
#define LCD_CAM_DOUT11_MODE_S  22
/** LCD_CAM_DOUT12_MODE : R/W; bitpos: [25:24]; default: 0;
 *  The output data bit 12 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT12_MODE    0x00000003U
#define LCD_CAM_DOUT12_MODE_M  (LCD_CAM_DOUT12_MODE_V << LCD_CAM_DOUT12_MODE_S)
#define LCD_CAM_DOUT12_MODE_V  0x00000003U
#define LCD_CAM_DOUT12_MODE_S  24
/** LCD_CAM_DOUT13_MODE : R/W; bitpos: [27:26]; default: 0;
 *  The output data bit 13 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT13_MODE    0x00000003U
#define LCD_CAM_DOUT13_MODE_M  (LCD_CAM_DOUT13_MODE_V << LCD_CAM_DOUT13_MODE_S)
#define LCD_CAM_DOUT13_MODE_V  0x00000003U
#define LCD_CAM_DOUT13_MODE_S  26
/** LCD_CAM_DOUT14_MODE : R/W; bitpos: [29:28]; default: 0;
 *  The output data bit 14 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT14_MODE    0x00000003U
#define LCD_CAM_DOUT14_MODE_M  (LCD_CAM_DOUT14_MODE_V << LCD_CAM_DOUT14_MODE_S)
#define LCD_CAM_DOUT14_MODE_V  0x00000003U
#define LCD_CAM_DOUT14_MODE_S  28
/** LCD_CAM_DOUT15_MODE : R/W; bitpos: [31:30]; default: 0;
 *  The output data bit 15 is delayed by module clock LCD_CLK. 0: output without delay.
 *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
 */
#define LCD_CAM_DOUT15_MODE    0x00000003U
#define LCD_CAM_DOUT15_MODE_M  (LCD_CAM_DOUT15_MODE_V << LCD_CAM_DOUT15_MODE_S)
#define LCD_CAM_DOUT15_MODE_V  0x00000003U
#define LCD_CAM_DOUT15_MODE_S  30

/** LCD_CAM_LC_DMA_INT_ENA_REG register
 *  LCD_CAM GDMA interrupt enable register
 */
#define LCD_CAM_LC_DMA_INT_ENA_REG (DR_REG_LCD_CAM_BASE + 0x64)
/** LCD_CAM_LCD_VSYNC_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The enable bit for LCD_CAM_LCD_VSYNC_INT interrupt.
 */
#define LCD_CAM_LCD_VSYNC_INT_ENA    (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_ENA_M  (LCD_CAM_LCD_VSYNC_INT_ENA_V << LCD_CAM_LCD_VSYNC_INT_ENA_S)
#define LCD_CAM_LCD_VSYNC_INT_ENA_V  0x00000001U
#define LCD_CAM_LCD_VSYNC_INT_ENA_S  0
/** LCD_CAM_LCD_TRANS_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The enable bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
 */
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA    (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA_M  (LCD_CAM_LCD_TRANS_DONE_INT_ENA_V << LCD_CAM_LCD_TRANS_DONE_INT_ENA_S)
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA_V  0x00000001U
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA_S  1
/** LCD_CAM_CAM_VSYNC_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The enable bit for LCD_CAM_CAM_VSYNC_INT interrupt.
 */
#define LCD_CAM_CAM_VSYNC_INT_ENA    (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_ENA_M  (LCD_CAM_CAM_VSYNC_INT_ENA_V << LCD_CAM_CAM_VSYNC_INT_ENA_S)
#define LCD_CAM_CAM_VSYNC_INT_ENA_V  0x00000001U
#define LCD_CAM_CAM_VSYNC_INT_ENA_S  2
/** LCD_CAM_CAM_HS_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The enable bit for LCD_CAM_CAM_HS_INT interrupt.
 */
#define LCD_CAM_CAM_HS_INT_ENA    (BIT(3))
#define LCD_CAM_CAM_HS_INT_ENA_M  (LCD_CAM_CAM_HS_INT_ENA_V << LCD_CAM_CAM_HS_INT_ENA_S)
#define LCD_CAM_CAM_HS_INT_ENA_V  0x00000001U
#define LCD_CAM_CAM_HS_INT_ENA_S  3

/** LCD_CAM_LC_DMA_INT_RAW_REG register
 *  LCD_CAM GDMA raw interrupt status register
 */
#define LCD_CAM_LC_DMA_INT_RAW_REG (DR_REG_LCD_CAM_BASE + 0x68)
/** LCD_CAM_LCD_VSYNC_INT_RAW : RO; bitpos: [0]; default: 0;
 *  The raw bit for LCD_CAM_LCD_VSYNC_INT interrupt.
 */
#define LCD_CAM_LCD_VSYNC_INT_RAW    (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_RAW_M  (LCD_CAM_LCD_VSYNC_INT_RAW_V << LCD_CAM_LCD_VSYNC_INT_RAW_S)
#define LCD_CAM_LCD_VSYNC_INT_RAW_V  0x00000001U
#define LCD_CAM_LCD_VSYNC_INT_RAW_S  0
/** LCD_CAM_LCD_TRANS_DONE_INT_RAW : RO; bitpos: [1]; default: 0;
 *  The raw bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
 */
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW    (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW_M  (LCD_CAM_LCD_TRANS_DONE_INT_RAW_V << LCD_CAM_LCD_TRANS_DONE_INT_RAW_S)
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW_V  0x00000001U
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW_S  1
/** LCD_CAM_CAM_VSYNC_INT_RAW : RO; bitpos: [2]; default: 0;
 *  The raw bit for LCD_CAM_CAM_VSYNC_INT interrupt.
 */
#define LCD_CAM_CAM_VSYNC_INT_RAW    (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_RAW_M  (LCD_CAM_CAM_VSYNC_INT_RAW_V << LCD_CAM_CAM_VSYNC_INT_RAW_S)
#define LCD_CAM_CAM_VSYNC_INT_RAW_V  0x00000001U
#define LCD_CAM_CAM_VSYNC_INT_RAW_S  2
/** LCD_CAM_CAM_HS_INT_RAW : RO; bitpos: [3]; default: 0;
 *  The raw bit for LCD_CAM_CAM_HS_INT interrupt.
 */
#define LCD_CAM_CAM_HS_INT_RAW    (BIT(3))
#define LCD_CAM_CAM_HS_INT_RAW_M  (LCD_CAM_CAM_HS_INT_RAW_V << LCD_CAM_CAM_HS_INT_RAW_S)
#define LCD_CAM_CAM_HS_INT_RAW_V  0x00000001U
#define LCD_CAM_CAM_HS_INT_RAW_S  3

/** LCD_CAM_LC_DMA_INT_ST_REG register
 *  LCD_CAM GDMA masked interrupt status register
 */
#define LCD_CAM_LC_DMA_INT_ST_REG (DR_REG_LCD_CAM_BASE + 0x6c)
/** LCD_CAM_LCD_VSYNC_INT_ST : RO; bitpos: [0]; default: 0;
 *  The status bit for LCD_CAM_LCD_VSYNC_INT interrupt.
 */
#define LCD_CAM_LCD_VSYNC_INT_ST    (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_ST_M  (LCD_CAM_LCD_VSYNC_INT_ST_V << LCD_CAM_LCD_VSYNC_INT_ST_S)
#define LCD_CAM_LCD_VSYNC_INT_ST_V  0x00000001U
#define LCD_CAM_LCD_VSYNC_INT_ST_S  0
/** LCD_CAM_LCD_TRANS_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The status bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
 */
#define LCD_CAM_LCD_TRANS_DONE_INT_ST    (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_ST_M  (LCD_CAM_LCD_TRANS_DONE_INT_ST_V << LCD_CAM_LCD_TRANS_DONE_INT_ST_S)
#define LCD_CAM_LCD_TRANS_DONE_INT_ST_V  0x00000001U
#define LCD_CAM_LCD_TRANS_DONE_INT_ST_S  1
/** LCD_CAM_CAM_VSYNC_INT_ST : RO; bitpos: [2]; default: 0;
 *  The status bit for LCD_CAM_CAM_VSYNC_INT interrupt.
 */
#define LCD_CAM_CAM_VSYNC_INT_ST    (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_ST_M  (LCD_CAM_CAM_VSYNC_INT_ST_V << LCD_CAM_CAM_VSYNC_INT_ST_S)
#define LCD_CAM_CAM_VSYNC_INT_ST_V  0x00000001U
#define LCD_CAM_CAM_VSYNC_INT_ST_S  2
/** LCD_CAM_CAM_HS_INT_ST : RO; bitpos: [3]; default: 0;
 *  The status bit for LCD_CAM_CAM_HS_INT interrupt.
 */
#define LCD_CAM_CAM_HS_INT_ST    (BIT(3))
#define LCD_CAM_CAM_HS_INT_ST_M  (LCD_CAM_CAM_HS_INT_ST_V << LCD_CAM_CAM_HS_INT_ST_S)
#define LCD_CAM_CAM_HS_INT_ST_V  0x00000001U
#define LCD_CAM_CAM_HS_INT_ST_S  3

/** LCD_CAM_LC_DMA_INT_CLR_REG register
 *  LCD_CAM GDMA interrupt clear register
 */
#define LCD_CAM_LC_DMA_INT_CLR_REG (DR_REG_LCD_CAM_BASE + 0x70)
/** LCD_CAM_LCD_VSYNC_INT_CLR : WO; bitpos: [0]; default: 0;
 *  The clear bit for LCD_CAM_LCD_VSYNC_INT interrupt.
 */
#define LCD_CAM_LCD_VSYNC_INT_CLR    (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_CLR_M  (LCD_CAM_LCD_VSYNC_INT_CLR_V << LCD_CAM_LCD_VSYNC_INT_CLR_S)
#define LCD_CAM_LCD_VSYNC_INT_CLR_V  0x00000001U
#define LCD_CAM_LCD_VSYNC_INT_CLR_S  0
/** LCD_CAM_LCD_TRANS_DONE_INT_CLR : WO; bitpos: [1]; default: 0;
 *  The clear bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
 */
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR    (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR_M  (LCD_CAM_LCD_TRANS_DONE_INT_CLR_V << LCD_CAM_LCD_TRANS_DONE_INT_CLR_S)
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR_V  0x00000001U
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR_S  1
/** LCD_CAM_CAM_VSYNC_INT_CLR : WO; bitpos: [2]; default: 0;
 *  The clear bit for LCD_CAM_CAM_VSYNC_INT interrupt.
 */
#define LCD_CAM_CAM_VSYNC_INT_CLR    (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_CLR_M  (LCD_CAM_CAM_VSYNC_INT_CLR_V << LCD_CAM_CAM_VSYNC_INT_CLR_S)
#define LCD_CAM_CAM_VSYNC_INT_CLR_V  0x00000001U
#define LCD_CAM_CAM_VSYNC_INT_CLR_S  2
/** LCD_CAM_CAM_HS_INT_CLR : WO; bitpos: [3]; default: 0;
 *  The clear bit for LCD_CAM_CAM_HS_INT interrupt.
 */
#define LCD_CAM_CAM_HS_INT_CLR    (BIT(3))
#define LCD_CAM_CAM_HS_INT_CLR_M  (LCD_CAM_CAM_HS_INT_CLR_V << LCD_CAM_CAM_HS_INT_CLR_S)
#define LCD_CAM_CAM_HS_INT_CLR_V  0x00000001U
#define LCD_CAM_CAM_HS_INT_CLR_S  3

/** LCD_CAM_LC_REG_DATE_REG register
 *  Version control register
 */
#define LCD_CAM_LC_REG_DATE_REG (DR_REG_LCD_CAM_BASE + 0xfc)
/** LCD_CAM_LC_DATE : R/W; bitpos: [27:0]; default: 33566752;
 *  Version control register
 */
#define LCD_CAM_LC_DATE    0x0FFFFFFFU
#define LCD_CAM_LC_DATE_M  (LCD_CAM_LC_DATE_V << LCD_CAM_LC_DATE_S)
#define LCD_CAM_LC_DATE_V  0x0FFFFFFFU
#define LCD_CAM_LC_DATE_S  0

#ifdef __cplusplus
}
#endif
