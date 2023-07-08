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

/** LCDCAM_LCD_CLOCK_REG register
 *  LCD clock config register.
 */
#define LCDCAM_LCD_CLOCK_REG (DR_REG_LCDCAM_BASE + 0x0)
/** LCDCAM_LCD_CLKCNT_N : R/W; bitpos: [5:0]; default: 3;
 *  f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1) when reg_clk_equ_sysclk is 0.
 */
#define LCDCAM_LCD_CLKCNT_N    0x0000003FU
#define LCDCAM_LCD_CLKCNT_N_M  (LCDCAM_LCD_CLKCNT_N_V << LCDCAM_LCD_CLKCNT_N_S)
#define LCDCAM_LCD_CLKCNT_N_V  0x0000003FU
#define LCDCAM_LCD_CLKCNT_N_S  0
/** LCDCAM_LCD_CLK_EQU_SYSCLK : R/W; bitpos: [6]; default: 1;
 *  1: f_LCD_PCLK = f_LCD_CLK. 0: f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1).
 */
#define LCDCAM_LCD_CLK_EQU_SYSCLK    (BIT(6))
#define LCDCAM_LCD_CLK_EQU_SYSCLK_M  (LCDCAM_LCD_CLK_EQU_SYSCLK_V << LCDCAM_LCD_CLK_EQU_SYSCLK_S)
#define LCDCAM_LCD_CLK_EQU_SYSCLK_V  0x00000001U
#define LCDCAM_LCD_CLK_EQU_SYSCLK_S  6
/** LCDCAM_LCD_CK_IDLE_EDGE : R/W; bitpos: [7]; default: 0;
 *  1: LCD_PCLK line is high when idle     0: LCD_PCLK line is low when idle.
 */
#define LCDCAM_LCD_CK_IDLE_EDGE    (BIT(7))
#define LCDCAM_LCD_CK_IDLE_EDGE_M  (LCDCAM_LCD_CK_IDLE_EDGE_V << LCDCAM_LCD_CK_IDLE_EDGE_S)
#define LCDCAM_LCD_CK_IDLE_EDGE_V  0x00000001U
#define LCDCAM_LCD_CK_IDLE_EDGE_S  7
/** LCDCAM_LCD_CK_OUT_EDGE : R/W; bitpos: [8]; default: 0;
 *  1: LCD_PCLK line is high in the first half data cycle.     0: LCD_PCLK line is low
 *  in the second half data cycle.
 */
#define LCDCAM_LCD_CK_OUT_EDGE    (BIT(8))
#define LCDCAM_LCD_CK_OUT_EDGE_M  (LCDCAM_LCD_CK_OUT_EDGE_V << LCDCAM_LCD_CK_OUT_EDGE_S)
#define LCDCAM_LCD_CK_OUT_EDGE_V  0x00000001U
#define LCDCAM_LCD_CK_OUT_EDGE_S  8
/** LCDCAM_LCD_CLKM_DIV_NUM : R/W; bitpos: [16:9]; default: 4;
 *  Integral LCD clock divider value
 */
#define LCDCAM_LCD_CLKM_DIV_NUM    0x000000FFU
#define LCDCAM_LCD_CLKM_DIV_NUM_M  (LCDCAM_LCD_CLKM_DIV_NUM_V << LCDCAM_LCD_CLKM_DIV_NUM_S)
#define LCDCAM_LCD_CLKM_DIV_NUM_V  0x000000FFU
#define LCDCAM_LCD_CLKM_DIV_NUM_S  9
/** LCDCAM_LCD_CLKM_DIV_B : R/W; bitpos: [22:17]; default: 0;
 *  Fractional clock divider numerator value
 */
#define LCDCAM_LCD_CLKM_DIV_B    0x0000003FU
#define LCDCAM_LCD_CLKM_DIV_B_M  (LCDCAM_LCD_CLKM_DIV_B_V << LCDCAM_LCD_CLKM_DIV_B_S)
#define LCDCAM_LCD_CLKM_DIV_B_V  0x0000003FU
#define LCDCAM_LCD_CLKM_DIV_B_S  17
/** LCDCAM_LCD_CLKM_DIV_A : R/W; bitpos: [28:23]; default: 0;
 *  Fractional clock divider denominator value
 */
#define LCDCAM_LCD_CLKM_DIV_A    0x0000003FU
#define LCDCAM_LCD_CLKM_DIV_A_M  (LCDCAM_LCD_CLKM_DIV_A_V << LCDCAM_LCD_CLKM_DIV_A_S)
#define LCDCAM_LCD_CLKM_DIV_A_V  0x0000003FU
#define LCDCAM_LCD_CLKM_DIV_A_S  23
/** LCDCAM_LCD_CLK_SEL : R/W; bitpos: [30:29]; default: 0;
 *  Select LCD module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.
 */
#define LCDCAM_LCD_CLK_SEL    0x00000003U
#define LCDCAM_LCD_CLK_SEL_M  (LCDCAM_LCD_CLK_SEL_V << LCDCAM_LCD_CLK_SEL_S)
#define LCDCAM_LCD_CLK_SEL_V  0x00000003U
#define LCDCAM_LCD_CLK_SEL_S  29
/** LCDCAM_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Set this bit to enable clk gate
 */
#define LCDCAM_CLK_EN    (BIT(31))
#define LCDCAM_CLK_EN_M  (LCDCAM_CLK_EN_V << LCDCAM_CLK_EN_S)
#define LCDCAM_CLK_EN_V  0x00000001U
#define LCDCAM_CLK_EN_S  31

/** LCDCAM_CAM_CTRL_REG register
 *  CAM config register.
 */
#define LCDCAM_CAM_CTRL_REG (DR_REG_LCDCAM_BASE + 0x4)
/** LCDCAM_CAM_STOP_EN : R/W; bitpos: [0]; default: 0;
 *  Camera stop enable signal, 1: camera stops when DMA Rx FIFO is full. 0: Not stop.
 */
#define LCDCAM_CAM_STOP_EN    (BIT(0))
#define LCDCAM_CAM_STOP_EN_M  (LCDCAM_CAM_STOP_EN_V << LCDCAM_CAM_STOP_EN_S)
#define LCDCAM_CAM_STOP_EN_V  0x00000001U
#define LCDCAM_CAM_STOP_EN_S  0
/** LCDCAM_CAM_VSYNC_FILTER_THRES : R/W; bitpos: [3:1]; default: 0;
 *  Filter threshold value for CAM_VSYNC signal.
 */
#define LCDCAM_CAM_VSYNC_FILTER_THRES    0x00000007U
#define LCDCAM_CAM_VSYNC_FILTER_THRES_M  (LCDCAM_CAM_VSYNC_FILTER_THRES_V << LCDCAM_CAM_VSYNC_FILTER_THRES_S)
#define LCDCAM_CAM_VSYNC_FILTER_THRES_V  0x00000007U
#define LCDCAM_CAM_VSYNC_FILTER_THRES_S  1
/** LCDCAM_CAM_UPDATE_REG : R/W/SC; bitpos: [4]; default: 0;
 *  1: Update Camera registers, will be cleared by hardware. 0 : Not care.
 */
#define LCDCAM_CAM_UPDATE_REG    (BIT(4))
#define LCDCAM_CAM_UPDATE_REG_M  (LCDCAM_CAM_UPDATE_REG_V << LCDCAM_CAM_UPDATE_REG_S)
#define LCDCAM_CAM_UPDATE_REG_V  0x00000001U
#define LCDCAM_CAM_UPDATE_REG_S  4
/** LCDCAM_CAM_BYTE_ORDER : R/W; bitpos: [5]; default: 0;
 *  1: Change data bit order, change CAM_DATA_in[7:0] to CAM_DATA_in[0:7] in one byte
 *  mode, and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.
 */
#define LCDCAM_CAM_BYTE_ORDER    (BIT(5))
#define LCDCAM_CAM_BYTE_ORDER_M  (LCDCAM_CAM_BYTE_ORDER_V << LCDCAM_CAM_BYTE_ORDER_S)
#define LCDCAM_CAM_BYTE_ORDER_V  0x00000001U
#define LCDCAM_CAM_BYTE_ORDER_S  5
/** LCDCAM_CAM_BIT_ORDER : R/W; bitpos: [6]; default: 0;
 *  1: invert data byte order, only valid in 2 byte mode. 0: Not change.
 */
#define LCDCAM_CAM_BIT_ORDER    (BIT(6))
#define LCDCAM_CAM_BIT_ORDER_M  (LCDCAM_CAM_BIT_ORDER_V << LCDCAM_CAM_BIT_ORDER_S)
#define LCDCAM_CAM_BIT_ORDER_V  0x00000001U
#define LCDCAM_CAM_BIT_ORDER_S  6
/** LCDCAM_CAM_LINE_INT_EN : R/W; bitpos: [7]; default: 0;
 *  1: Enable to generate CAM_HS_INT. 0: Disable.
 */
#define LCDCAM_CAM_LINE_INT_EN    (BIT(7))
#define LCDCAM_CAM_LINE_INT_EN_M  (LCDCAM_CAM_LINE_INT_EN_V << LCDCAM_CAM_LINE_INT_EN_S)
#define LCDCAM_CAM_LINE_INT_EN_V  0x00000001U
#define LCDCAM_CAM_LINE_INT_EN_S  7
/** LCDCAM_CAM_VS_EOF_EN : R/W; bitpos: [8]; default: 0;
 *  1: CAM_VSYNC to generate in_suc_eof. 0: in_suc_eof is controlled by
 *  reg_cam_rec_data_cyclelen.
 */
#define LCDCAM_CAM_VS_EOF_EN    (BIT(8))
#define LCDCAM_CAM_VS_EOF_EN_M  (LCDCAM_CAM_VS_EOF_EN_V << LCDCAM_CAM_VS_EOF_EN_S)
#define LCDCAM_CAM_VS_EOF_EN_V  0x00000001U
#define LCDCAM_CAM_VS_EOF_EN_S  8
/** LCDCAM_CAM_CLKM_DIV_NUM : R/W; bitpos: [16:9]; default: 4;
 *  Integral Camera clock divider value
 */
#define LCDCAM_CAM_CLKM_DIV_NUM    0x000000FFU
#define LCDCAM_CAM_CLKM_DIV_NUM_M  (LCDCAM_CAM_CLKM_DIV_NUM_V << LCDCAM_CAM_CLKM_DIV_NUM_S)
#define LCDCAM_CAM_CLKM_DIV_NUM_V  0x000000FFU
#define LCDCAM_CAM_CLKM_DIV_NUM_S  9
/** LCDCAM_CAM_CLKM_DIV_B : R/W; bitpos: [22:17]; default: 0;
 *  Fractional clock divider numerator value
 */
#define LCDCAM_CAM_CLKM_DIV_B    0x0000003FU
#define LCDCAM_CAM_CLKM_DIV_B_M  (LCDCAM_CAM_CLKM_DIV_B_V << LCDCAM_CAM_CLKM_DIV_B_S)
#define LCDCAM_CAM_CLKM_DIV_B_V  0x0000003FU
#define LCDCAM_CAM_CLKM_DIV_B_S  17
/** LCDCAM_CAM_CLKM_DIV_A : R/W; bitpos: [28:23]; default: 0;
 *  Fractional clock divider denominator value
 */
#define LCDCAM_CAM_CLKM_DIV_A    0x0000003FU
#define LCDCAM_CAM_CLKM_DIV_A_M  (LCDCAM_CAM_CLKM_DIV_A_V << LCDCAM_CAM_CLKM_DIV_A_S)
#define LCDCAM_CAM_CLKM_DIV_A_V  0x0000003FU
#define LCDCAM_CAM_CLKM_DIV_A_S  23
/** LCDCAM_CAM_CLK_SEL : R/W; bitpos: [30:29]; default: 0;
 *  Select Camera module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.
 */
#define LCDCAM_CAM_CLK_SEL    0x00000003U
#define LCDCAM_CAM_CLK_SEL_M  (LCDCAM_CAM_CLK_SEL_V << LCDCAM_CAM_CLK_SEL_S)
#define LCDCAM_CAM_CLK_SEL_V  0x00000003U
#define LCDCAM_CAM_CLK_SEL_S  29

/** LCDCAM_CAM_CTRL1_REG register
 *  CAM config register.
 */
#define LCDCAM_CAM_CTRL1_REG (DR_REG_LCDCAM_BASE + 0x8)
/** LCDCAM_CAM_REC_DATA_BYTELEN : R/W; bitpos: [15:0]; default: 0;
 *  Camera receive data byte length minus 1 to set DMA in_suc_eof_int.
 */
#define LCDCAM_CAM_REC_DATA_BYTELEN    0x0000FFFFU
#define LCDCAM_CAM_REC_DATA_BYTELEN_M  (LCDCAM_CAM_REC_DATA_BYTELEN_V << LCDCAM_CAM_REC_DATA_BYTELEN_S)
#define LCDCAM_CAM_REC_DATA_BYTELEN_V  0x0000FFFFU
#define LCDCAM_CAM_REC_DATA_BYTELEN_S  0
/** LCDCAM_CAM_LINE_INT_NUM : R/W; bitpos: [21:16]; default: 0;
 *  The line number minus 1 to generate cam_hs_int.
 */
#define LCDCAM_CAM_LINE_INT_NUM    0x0000003FU
#define LCDCAM_CAM_LINE_INT_NUM_M  (LCDCAM_CAM_LINE_INT_NUM_V << LCDCAM_CAM_LINE_INT_NUM_S)
#define LCDCAM_CAM_LINE_INT_NUM_V  0x0000003FU
#define LCDCAM_CAM_LINE_INT_NUM_S  16
/** LCDCAM_CAM_CLK_INV : R/W; bitpos: [22]; default: 0;
 *  1: Invert  the input signal CAM_PCLK. 0: Not invert.
 */
#define LCDCAM_CAM_CLK_INV    (BIT(22))
#define LCDCAM_CAM_CLK_INV_M  (LCDCAM_CAM_CLK_INV_V << LCDCAM_CAM_CLK_INV_S)
#define LCDCAM_CAM_CLK_INV_V  0x00000001U
#define LCDCAM_CAM_CLK_INV_S  22
/** LCDCAM_CAM_VSYNC_FILTER_EN : R/W; bitpos: [23]; default: 0;
 *  1: Enable CAM_VSYNC filter function. 0: bypass.
 */
#define LCDCAM_CAM_VSYNC_FILTER_EN    (BIT(23))
#define LCDCAM_CAM_VSYNC_FILTER_EN_M  (LCDCAM_CAM_VSYNC_FILTER_EN_V << LCDCAM_CAM_VSYNC_FILTER_EN_S)
#define LCDCAM_CAM_VSYNC_FILTER_EN_V  0x00000001U
#define LCDCAM_CAM_VSYNC_FILTER_EN_S  23
/** LCDCAM_CAM_2BYTE_EN : R/W; bitpos: [24]; default: 0;
 *  1: The bit number of input data is 9~16.  0: The bit number of input data is 0~8.
 */
#define LCDCAM_CAM_2BYTE_EN    (BIT(24))
#define LCDCAM_CAM_2BYTE_EN_M  (LCDCAM_CAM_2BYTE_EN_V << LCDCAM_CAM_2BYTE_EN_S)
#define LCDCAM_CAM_2BYTE_EN_V  0x00000001U
#define LCDCAM_CAM_2BYTE_EN_S  24
/** LCDCAM_CAM_DE_INV : R/W; bitpos: [25]; default: 0;
 *  CAM_DE invert enable signal, valid in high level.
 */
#define LCDCAM_CAM_DE_INV    (BIT(25))
#define LCDCAM_CAM_DE_INV_M  (LCDCAM_CAM_DE_INV_V << LCDCAM_CAM_DE_INV_S)
#define LCDCAM_CAM_DE_INV_V  0x00000001U
#define LCDCAM_CAM_DE_INV_S  25
/** LCDCAM_CAM_HSYNC_INV : R/W; bitpos: [26]; default: 0;
 *  CAM_HSYNC invert enable signal, valid in high level.
 */
#define LCDCAM_CAM_HSYNC_INV    (BIT(26))
#define LCDCAM_CAM_HSYNC_INV_M  (LCDCAM_CAM_HSYNC_INV_V << LCDCAM_CAM_HSYNC_INV_S)
#define LCDCAM_CAM_HSYNC_INV_V  0x00000001U
#define LCDCAM_CAM_HSYNC_INV_S  26
/** LCDCAM_CAM_VSYNC_INV : R/W; bitpos: [27]; default: 0;
 *  CAM_VSYNC invert enable signal, valid in high level.
 */
#define LCDCAM_CAM_VSYNC_INV    (BIT(27))
#define LCDCAM_CAM_VSYNC_INV_M  (LCDCAM_CAM_VSYNC_INV_V << LCDCAM_CAM_VSYNC_INV_S)
#define LCDCAM_CAM_VSYNC_INV_V  0x00000001U
#define LCDCAM_CAM_VSYNC_INV_S  27
/** LCDCAM_CAM_VH_DE_MODE_EN : R/W; bitpos: [28]; default: 0;
 *  1: Input control signals are CAM_DE CAM_HSYNC and CAM_VSYNC. 0: Input control
 *  signals are CAM_DE and CAM_VSYNC.
 */
#define LCDCAM_CAM_VH_DE_MODE_EN    (BIT(28))
#define LCDCAM_CAM_VH_DE_MODE_EN_M  (LCDCAM_CAM_VH_DE_MODE_EN_V << LCDCAM_CAM_VH_DE_MODE_EN_S)
#define LCDCAM_CAM_VH_DE_MODE_EN_V  0x00000001U
#define LCDCAM_CAM_VH_DE_MODE_EN_S  28
/** LCDCAM_CAM_START : R/W/SC; bitpos: [29]; default: 0;
 *  Camera module start signal.
 */
#define LCDCAM_CAM_START    (BIT(29))
#define LCDCAM_CAM_START_M  (LCDCAM_CAM_START_V << LCDCAM_CAM_START_S)
#define LCDCAM_CAM_START_V  0x00000001U
#define LCDCAM_CAM_START_S  29
/** LCDCAM_CAM_RESET : WT; bitpos: [30]; default: 0;
 *  Camera module reset signal.
 */
#define LCDCAM_CAM_RESET    (BIT(30))
#define LCDCAM_CAM_RESET_M  (LCDCAM_CAM_RESET_V << LCDCAM_CAM_RESET_S)
#define LCDCAM_CAM_RESET_V  0x00000001U
#define LCDCAM_CAM_RESET_S  30
/** LCDCAM_CAM_AFIFO_RESET : WT; bitpos: [31]; default: 0;
 *  Camera AFIFO reset signal.
 */
#define LCDCAM_CAM_AFIFO_RESET    (BIT(31))
#define LCDCAM_CAM_AFIFO_RESET_M  (LCDCAM_CAM_AFIFO_RESET_V << LCDCAM_CAM_AFIFO_RESET_S)
#define LCDCAM_CAM_AFIFO_RESET_V  0x00000001U
#define LCDCAM_CAM_AFIFO_RESET_S  31

/** LCDCAM_CAM_RGB_YUV_REG register
 *  CAM YUV/RGB converter configuration register.
 */
#define LCDCAM_CAM_RGB_YUV_REG (DR_REG_LCDCAM_BASE + 0xc)
/** LCDCAM_CAM_CONV_8BITS_DATA_INV : R/W; bitpos: [21]; default: 0;
 *  1:invert every two 8bits input data. 2. disabled.
 */
#define LCDCAM_CAM_CONV_8BITS_DATA_INV    (BIT(21))
#define LCDCAM_CAM_CONV_8BITS_DATA_INV_M  (LCDCAM_CAM_CONV_8BITS_DATA_INV_V << LCDCAM_CAM_CONV_8BITS_DATA_INV_S)
#define LCDCAM_CAM_CONV_8BITS_DATA_INV_V  0x00000001U
#define LCDCAM_CAM_CONV_8BITS_DATA_INV_S  21
/** LCDCAM_CAM_CONV_YUV2YUV_MODE : R/W; bitpos: [23:22]; default: 3;
 *  0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable yuv2yuv mode,
 *  trans_mode must be set to 1.
 */
#define LCDCAM_CAM_CONV_YUV2YUV_MODE    0x00000003U
#define LCDCAM_CAM_CONV_YUV2YUV_MODE_M  (LCDCAM_CAM_CONV_YUV2YUV_MODE_V << LCDCAM_CAM_CONV_YUV2YUV_MODE_S)
#define LCDCAM_CAM_CONV_YUV2YUV_MODE_V  0x00000003U
#define LCDCAM_CAM_CONV_YUV2YUV_MODE_S  22
/** LCDCAM_CAM_CONV_YUV_MODE : R/W; bitpos: [25:24]; default: 0;
 *  0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode, yuv_mode decides the yuv
 *  mode of Data_in
 */
#define LCDCAM_CAM_CONV_YUV_MODE    0x00000003U
#define LCDCAM_CAM_CONV_YUV_MODE_M  (LCDCAM_CAM_CONV_YUV_MODE_V << LCDCAM_CAM_CONV_YUV_MODE_S)
#define LCDCAM_CAM_CONV_YUV_MODE_V  0x00000003U
#define LCDCAM_CAM_CONV_YUV_MODE_S  24
/** LCDCAM_CAM_CONV_PROTOCOL_MODE : R/W; bitpos: [26]; default: 0;
 *  0:BT601. 1:BT709.
 */
#define LCDCAM_CAM_CONV_PROTOCOL_MODE    (BIT(26))
#define LCDCAM_CAM_CONV_PROTOCOL_MODE_M  (LCDCAM_CAM_CONV_PROTOCOL_MODE_V << LCDCAM_CAM_CONV_PROTOCOL_MODE_S)
#define LCDCAM_CAM_CONV_PROTOCOL_MODE_V  0x00000001U
#define LCDCAM_CAM_CONV_PROTOCOL_MODE_S  26
/** LCDCAM_CAM_CONV_DATA_OUT_MODE : R/W; bitpos: [27]; default: 0;
 *  LIMIT or FULL mode of Data out. 0: limit. 1: full
 */
#define LCDCAM_CAM_CONV_DATA_OUT_MODE    (BIT(27))
#define LCDCAM_CAM_CONV_DATA_OUT_MODE_M  (LCDCAM_CAM_CONV_DATA_OUT_MODE_V << LCDCAM_CAM_CONV_DATA_OUT_MODE_S)
#define LCDCAM_CAM_CONV_DATA_OUT_MODE_V  0x00000001U
#define LCDCAM_CAM_CONV_DATA_OUT_MODE_S  27
/** LCDCAM_CAM_CONV_DATA_IN_MODE : R/W; bitpos: [28]; default: 0;
 *  LIMIT or FULL mode of Data in. 0: limit. 1: full
 */
#define LCDCAM_CAM_CONV_DATA_IN_MODE    (BIT(28))
#define LCDCAM_CAM_CONV_DATA_IN_MODE_M  (LCDCAM_CAM_CONV_DATA_IN_MODE_V << LCDCAM_CAM_CONV_DATA_IN_MODE_S)
#define LCDCAM_CAM_CONV_DATA_IN_MODE_V  0x00000001U
#define LCDCAM_CAM_CONV_DATA_IN_MODE_S  28
/** LCDCAM_CAM_CONV_MODE_8BITS_ON : R/W; bitpos: [29]; default: 0;
 *  0: 16bits mode. 1: 8bits mode.
 */
#define LCDCAM_CAM_CONV_MODE_8BITS_ON    (BIT(29))
#define LCDCAM_CAM_CONV_MODE_8BITS_ON_M  (LCDCAM_CAM_CONV_MODE_8BITS_ON_V << LCDCAM_CAM_CONV_MODE_8BITS_ON_S)
#define LCDCAM_CAM_CONV_MODE_8BITS_ON_V  0x00000001U
#define LCDCAM_CAM_CONV_MODE_8BITS_ON_S  29
/** LCDCAM_CAM_CONV_TRANS_MODE : R/W; bitpos: [30]; default: 0;
 *  0: YUV to RGB. 1: RGB to YUV.
 */
#define LCDCAM_CAM_CONV_TRANS_MODE    (BIT(30))
#define LCDCAM_CAM_CONV_TRANS_MODE_M  (LCDCAM_CAM_CONV_TRANS_MODE_V << LCDCAM_CAM_CONV_TRANS_MODE_S)
#define LCDCAM_CAM_CONV_TRANS_MODE_V  0x00000001U
#define LCDCAM_CAM_CONV_TRANS_MODE_S  30
/** LCDCAM_CAM_CONV_ENABLE : R/W; bitpos: [31]; default: 0;
 *  0: Bypass converter. 1: Enable converter.
 */
#define LCDCAM_CAM_CONV_ENABLE    (BIT(31))
#define LCDCAM_CAM_CONV_ENABLE_M  (LCDCAM_CAM_CONV_ENABLE_V << LCDCAM_CAM_CONV_ENABLE_S)
#define LCDCAM_CAM_CONV_ENABLE_V  0x00000001U
#define LCDCAM_CAM_CONV_ENABLE_S  31

/** LCDCAM_LCD_RGB_YUV_REG register
 *  LCD YUV/RGB converter configuration register.
 */
#define LCDCAM_LCD_RGB_YUV_REG (DR_REG_LCDCAM_BASE + 0x10)
/** LCDCAM_LCD_CONV_8BITS_DATA_INV : R/W; bitpos: [20]; default: 0;
 *  1:invert every two 8bits input data. 2. disabled.
 */
#define LCDCAM_LCD_CONV_8BITS_DATA_INV    (BIT(20))
#define LCDCAM_LCD_CONV_8BITS_DATA_INV_M  (LCDCAM_LCD_CONV_8BITS_DATA_INV_V << LCDCAM_LCD_CONV_8BITS_DATA_INV_S)
#define LCDCAM_LCD_CONV_8BITS_DATA_INV_V  0x00000001U
#define LCDCAM_LCD_CONV_8BITS_DATA_INV_S  20
/** LCDCAM_LCD_CONV_TXTORX : R/W; bitpos: [21]; default: 0;
 *  0: txtorx mode off. 1: txtorx mode on.
 */
#define LCDCAM_LCD_CONV_TXTORX    (BIT(21))
#define LCDCAM_LCD_CONV_TXTORX_M  (LCDCAM_LCD_CONV_TXTORX_V << LCDCAM_LCD_CONV_TXTORX_S)
#define LCDCAM_LCD_CONV_TXTORX_V  0x00000001U
#define LCDCAM_LCD_CONV_TXTORX_S  21
/** LCDCAM_LCD_CONV_YUV2YUV_MODE : R/W; bitpos: [23:22]; default: 3;
 *  0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable yuv2yuv mode,
 *  trans_mode must be set to 1.
 */
#define LCDCAM_LCD_CONV_YUV2YUV_MODE    0x00000003U
#define LCDCAM_LCD_CONV_YUV2YUV_MODE_M  (LCDCAM_LCD_CONV_YUV2YUV_MODE_V << LCDCAM_LCD_CONV_YUV2YUV_MODE_S)
#define LCDCAM_LCD_CONV_YUV2YUV_MODE_V  0x00000003U
#define LCDCAM_LCD_CONV_YUV2YUV_MODE_S  22
/** LCDCAM_LCD_CONV_YUV_MODE : R/W; bitpos: [25:24]; default: 0;
 *  0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode, yuv_mode decides the yuv
 *  mode of Data_in
 */
#define LCDCAM_LCD_CONV_YUV_MODE    0x00000003U
#define LCDCAM_LCD_CONV_YUV_MODE_M  (LCDCAM_LCD_CONV_YUV_MODE_V << LCDCAM_LCD_CONV_YUV_MODE_S)
#define LCDCAM_LCD_CONV_YUV_MODE_V  0x00000003U
#define LCDCAM_LCD_CONV_YUV_MODE_S  24
/** LCDCAM_LCD_CONV_PROTOCOL_MODE : R/W; bitpos: [26]; default: 0;
 *  0:BT601. 1:BT709.
 */
#define LCDCAM_LCD_CONV_PROTOCOL_MODE    (BIT(26))
#define LCDCAM_LCD_CONV_PROTOCOL_MODE_M  (LCDCAM_LCD_CONV_PROTOCOL_MODE_V << LCDCAM_LCD_CONV_PROTOCOL_MODE_S)
#define LCDCAM_LCD_CONV_PROTOCOL_MODE_V  0x00000001U
#define LCDCAM_LCD_CONV_PROTOCOL_MODE_S  26
/** LCDCAM_LCD_CONV_DATA_OUT_MODE : R/W; bitpos: [27]; default: 0;
 *  LIMIT or FULL mode of Data out. 0: limit. 1: full
 */
#define LCDCAM_LCD_CONV_DATA_OUT_MODE    (BIT(27))
#define LCDCAM_LCD_CONV_DATA_OUT_MODE_M  (LCDCAM_LCD_CONV_DATA_OUT_MODE_V << LCDCAM_LCD_CONV_DATA_OUT_MODE_S)
#define LCDCAM_LCD_CONV_DATA_OUT_MODE_V  0x00000001U
#define LCDCAM_LCD_CONV_DATA_OUT_MODE_S  27
/** LCDCAM_LCD_CONV_DATA_IN_MODE : R/W; bitpos: [28]; default: 0;
 *  LIMIT or FULL mode of Data in. 0: limit. 1: full
 */
#define LCDCAM_LCD_CONV_DATA_IN_MODE    (BIT(28))
#define LCDCAM_LCD_CONV_DATA_IN_MODE_M  (LCDCAM_LCD_CONV_DATA_IN_MODE_V << LCDCAM_LCD_CONV_DATA_IN_MODE_S)
#define LCDCAM_LCD_CONV_DATA_IN_MODE_V  0x00000001U
#define LCDCAM_LCD_CONV_DATA_IN_MODE_S  28
/** LCDCAM_LCD_CONV_MODE_8BITS_ON : R/W; bitpos: [29]; default: 0;
 *  0: 16bits mode. 1: 8bits mode.
 */
#define LCDCAM_LCD_CONV_MODE_8BITS_ON    (BIT(29))
#define LCDCAM_LCD_CONV_MODE_8BITS_ON_M  (LCDCAM_LCD_CONV_MODE_8BITS_ON_V << LCDCAM_LCD_CONV_MODE_8BITS_ON_S)
#define LCDCAM_LCD_CONV_MODE_8BITS_ON_V  0x00000001U
#define LCDCAM_LCD_CONV_MODE_8BITS_ON_S  29
/** LCDCAM_LCD_CONV_TRANS_MODE : R/W; bitpos: [30]; default: 0;
 *  0: YUV to RGB. 1: RGB to YUV.
 */
#define LCDCAM_LCD_CONV_TRANS_MODE    (BIT(30))
#define LCDCAM_LCD_CONV_TRANS_MODE_M  (LCDCAM_LCD_CONV_TRANS_MODE_V << LCDCAM_LCD_CONV_TRANS_MODE_S)
#define LCDCAM_LCD_CONV_TRANS_MODE_V  0x00000001U
#define LCDCAM_LCD_CONV_TRANS_MODE_S  30
/** LCDCAM_LCD_CONV_ENABLE : R/W; bitpos: [31]; default: 0;
 *  0: Bypass converter. 1: Enable converter.
 */
#define LCDCAM_LCD_CONV_ENABLE    (BIT(31))
#define LCDCAM_LCD_CONV_ENABLE_M  (LCDCAM_LCD_CONV_ENABLE_V << LCDCAM_LCD_CONV_ENABLE_S)
#define LCDCAM_LCD_CONV_ENABLE_V  0x00000001U
#define LCDCAM_LCD_CONV_ENABLE_S  31

/** LCDCAM_LCD_USER_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_USER_REG (DR_REG_LCDCAM_BASE + 0x14)
/** LCDCAM_LCD_DOUT_CYCLELEN : R/W; bitpos: [12:0]; default: 1;
 *  The output data cycles minus 1 of LCD module.
 */
#define LCDCAM_LCD_DOUT_CYCLELEN    0x00001FFFU
#define LCDCAM_LCD_DOUT_CYCLELEN_M  (LCDCAM_LCD_DOUT_CYCLELEN_V << LCDCAM_LCD_DOUT_CYCLELEN_S)
#define LCDCAM_LCD_DOUT_CYCLELEN_V  0x00001FFFU
#define LCDCAM_LCD_DOUT_CYCLELEN_S  0
/** LCDCAM_LCD_ALWAYS_OUT_EN : R/W; bitpos: [13]; default: 0;
 *  LCD always output when LCD is in LCD_DOUT state, unless reg_lcd_start is cleared or
 *  reg_lcd_reset is set.
 */
#define LCDCAM_LCD_ALWAYS_OUT_EN    (BIT(13))
#define LCDCAM_LCD_ALWAYS_OUT_EN_M  (LCDCAM_LCD_ALWAYS_OUT_EN_V << LCDCAM_LCD_ALWAYS_OUT_EN_S)
#define LCDCAM_LCD_ALWAYS_OUT_EN_V  0x00000001U
#define LCDCAM_LCD_ALWAYS_OUT_EN_S  13
/** LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE : R/W; bitpos: [16:14]; default: 0;
 *  0: ABAB->BABA. 1: ABC->ACB. 2: ABC->BAC. 3: ABC->BCA. 4:ABC->CAB. 5:ABC->CBA
 */
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE    0x00000007U
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE_M  (LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE_V << LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE_S)
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE_V  0x00000007U
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_MODE_S  14
/** LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE : R/W; bitpos: [17]; default: 0;
 *  1: enable byte swizzle 0: disable
 */
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE    (BIT(17))
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE_M  (LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE_V << LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE_S)
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE_V  0x00000001U
#define LCDCAM_LCD_DOUT_BYTE_SWIZZLE_ENABLE_S  17
/** LCDCAM_LCD_DOUT_BIT_ORDER : R/W; bitpos: [18]; default: 0;
 *  1: change bit order in every byte. 0: Not change.
 */
#define LCDCAM_LCD_DOUT_BIT_ORDER    (BIT(18))
#define LCDCAM_LCD_DOUT_BIT_ORDER_M  (LCDCAM_LCD_DOUT_BIT_ORDER_V << LCDCAM_LCD_DOUT_BIT_ORDER_S)
#define LCDCAM_LCD_DOUT_BIT_ORDER_V  0x00000001U
#define LCDCAM_LCD_DOUT_BIT_ORDER_S  18
/** LCDCAM_LCD_BYTE_MODE : R/W; bitpos: [20:19]; default: 0;
 *  2: 24bit mode. 1: 16bit mode. 0: 8bit mode
 */
#define LCDCAM_LCD_BYTE_MODE    0x00000003U
#define LCDCAM_LCD_BYTE_MODE_M  (LCDCAM_LCD_BYTE_MODE_V << LCDCAM_LCD_BYTE_MODE_S)
#define LCDCAM_LCD_BYTE_MODE_V  0x00000003U
#define LCDCAM_LCD_BYTE_MODE_S  19
/** LCDCAM_LCD_UPDATE_REG : R/W/SC; bitpos: [21]; default: 0;
 *  1: Update LCD registers, will be cleared by hardware. 0 : Not care.
 */
#define LCDCAM_LCD_UPDATE_REG    (BIT(21))
#define LCDCAM_LCD_UPDATE_REG_M  (LCDCAM_LCD_UPDATE_REG_V << LCDCAM_LCD_UPDATE_REG_S)
#define LCDCAM_LCD_UPDATE_REG_V  0x00000001U
#define LCDCAM_LCD_UPDATE_REG_S  21
/** LCDCAM_LCD_BIT_ORDER : R/W; bitpos: [22]; default: 0;
 *  1: Change data bit order, change LCD_DATA_out[7:0] to LCD_DATA_out[0:7] in one byte
 *  mode, and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.
 */
#define LCDCAM_LCD_BIT_ORDER    (BIT(22))
#define LCDCAM_LCD_BIT_ORDER_M  (LCDCAM_LCD_BIT_ORDER_V << LCDCAM_LCD_BIT_ORDER_S)
#define LCDCAM_LCD_BIT_ORDER_V  0x00000001U
#define LCDCAM_LCD_BIT_ORDER_S  22
/** LCDCAM_LCD_BYTE_ORDER : R/W; bitpos: [23]; default: 0;
 *  1: invert data byte order, only valid in 2 byte mode. 0: Not change.
 */
#define LCDCAM_LCD_BYTE_ORDER    (BIT(23))
#define LCDCAM_LCD_BYTE_ORDER_M  (LCDCAM_LCD_BYTE_ORDER_V << LCDCAM_LCD_BYTE_ORDER_S)
#define LCDCAM_LCD_BYTE_ORDER_V  0x00000001U
#define LCDCAM_LCD_BYTE_ORDER_S  23
/** LCDCAM_LCD_DOUT : R/W; bitpos: [24]; default: 0;
 *  1: Be able to send data out in LCD sequence when LCD starts. 0: Disable.
 */
#define LCDCAM_LCD_DOUT    (BIT(24))
#define LCDCAM_LCD_DOUT_M  (LCDCAM_LCD_DOUT_V << LCDCAM_LCD_DOUT_S)
#define LCDCAM_LCD_DOUT_V  0x00000001U
#define LCDCAM_LCD_DOUT_S  24
/** LCDCAM_LCD_DUMMY : R/W; bitpos: [25]; default: 0;
 *  1: Enable DUMMY phase in LCD sequence when LCD starts. 0: Disable.
 */
#define LCDCAM_LCD_DUMMY    (BIT(25))
#define LCDCAM_LCD_DUMMY_M  (LCDCAM_LCD_DUMMY_V << LCDCAM_LCD_DUMMY_S)
#define LCDCAM_LCD_DUMMY_V  0x00000001U
#define LCDCAM_LCD_DUMMY_S  25
/** LCDCAM_LCD_CMD : R/W; bitpos: [26]; default: 0;
 *  1: Be able to send command in LCD sequence when LCD starts. 0: Disable.
 */
#define LCDCAM_LCD_CMD    (BIT(26))
#define LCDCAM_LCD_CMD_M  (LCDCAM_LCD_CMD_V << LCDCAM_LCD_CMD_S)
#define LCDCAM_LCD_CMD_V  0x00000001U
#define LCDCAM_LCD_CMD_S  26
/** LCDCAM_LCD_START : R/W/SC; bitpos: [27]; default: 0;
 *  LCD start sending data enable signal, valid in high level.
 */
#define LCDCAM_LCD_START    (BIT(27))
#define LCDCAM_LCD_START_M  (LCDCAM_LCD_START_V << LCDCAM_LCD_START_S)
#define LCDCAM_LCD_START_V  0x00000001U
#define LCDCAM_LCD_START_S  27
/** LCDCAM_LCD_RESET : WT; bitpos: [28]; default: 0;
 *  The value of  command.
 */
#define LCDCAM_LCD_RESET    (BIT(28))
#define LCDCAM_LCD_RESET_M  (LCDCAM_LCD_RESET_V << LCDCAM_LCD_RESET_S)
#define LCDCAM_LCD_RESET_V  0x00000001U
#define LCDCAM_LCD_RESET_S  28
/** LCDCAM_LCD_DUMMY_CYCLELEN : R/W; bitpos: [30:29]; default: 0;
 *  The dummy cycle length minus 1.
 */
#define LCDCAM_LCD_DUMMY_CYCLELEN    0x00000003U
#define LCDCAM_LCD_DUMMY_CYCLELEN_M  (LCDCAM_LCD_DUMMY_CYCLELEN_V << LCDCAM_LCD_DUMMY_CYCLELEN_S)
#define LCDCAM_LCD_DUMMY_CYCLELEN_V  0x00000003U
#define LCDCAM_LCD_DUMMY_CYCLELEN_S  29
/** LCDCAM_LCD_CMD_2_CYCLE_EN : R/W; bitpos: [31]; default: 0;
 *  The cycle length of command phase. 1: 2 cycles. 0: 1 cycle.
 */
#define LCDCAM_LCD_CMD_2_CYCLE_EN    (BIT(31))
#define LCDCAM_LCD_CMD_2_CYCLE_EN_M  (LCDCAM_LCD_CMD_2_CYCLE_EN_V << LCDCAM_LCD_CMD_2_CYCLE_EN_S)
#define LCDCAM_LCD_CMD_2_CYCLE_EN_V  0x00000001U
#define LCDCAM_LCD_CMD_2_CYCLE_EN_S  31

/** LCDCAM_LCD_MISC_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_MISC_REG (DR_REG_LCDCAM_BASE + 0x18)
/** LCDCAM_LCD_WIRE_MODE : R/W; bitpos: [5:4]; default: 0;
 *  The wire width of LCD output. 0: 8bit. 1: 16bit 2: 24bit
 */
#define LCDCAM_LCD_WIRE_MODE    0x00000003U
#define LCDCAM_LCD_WIRE_MODE_M  (LCDCAM_LCD_WIRE_MODE_V << LCDCAM_LCD_WIRE_MODE_S)
#define LCDCAM_LCD_WIRE_MODE_V  0x00000003U
#define LCDCAM_LCD_WIRE_MODE_S  4
/** LCDCAM_LCD_VFK_CYCLELEN : R/W; bitpos: [11:6]; default: 3;
 *  The setup cycle length minus 1 in LCD non-RGB mode.
 */
#define LCDCAM_LCD_VFK_CYCLELEN    0x0000003FU
#define LCDCAM_LCD_VFK_CYCLELEN_M  (LCDCAM_LCD_VFK_CYCLELEN_V << LCDCAM_LCD_VFK_CYCLELEN_S)
#define LCDCAM_LCD_VFK_CYCLELEN_V  0x0000003FU
#define LCDCAM_LCD_VFK_CYCLELEN_S  6
/** LCDCAM_LCD_VBK_CYCLELEN : R/W; bitpos: [24:12]; default: 0;
 *  The vertical back blank region cycle length minus 1 in LCD RGB mode, or the hold
 *  time cycle length in LCD non-RGB mode.
 */
#define LCDCAM_LCD_VBK_CYCLELEN    0x00001FFFU
#define LCDCAM_LCD_VBK_CYCLELEN_M  (LCDCAM_LCD_VBK_CYCLELEN_V << LCDCAM_LCD_VBK_CYCLELEN_S)
#define LCDCAM_LCD_VBK_CYCLELEN_V  0x00001FFFU
#define LCDCAM_LCD_VBK_CYCLELEN_S  12
/** LCDCAM_LCD_NEXT_FRAME_EN : R/W; bitpos: [25]; default: 0;
 *  1: Send the next frame data when the current frame is sent out. 0: LCD stops when
 *  the current frame is sent out.
 */
#define LCDCAM_LCD_NEXT_FRAME_EN    (BIT(25))
#define LCDCAM_LCD_NEXT_FRAME_EN_M  (LCDCAM_LCD_NEXT_FRAME_EN_V << LCDCAM_LCD_NEXT_FRAME_EN_S)
#define LCDCAM_LCD_NEXT_FRAME_EN_V  0x00000001U
#define LCDCAM_LCD_NEXT_FRAME_EN_S  25
/** LCDCAM_LCD_BK_EN : R/W; bitpos: [26]; default: 0;
 *  1: Enable blank region when LCD sends data out. 0: No blank region.
 */
#define LCDCAM_LCD_BK_EN    (BIT(26))
#define LCDCAM_LCD_BK_EN_M  (LCDCAM_LCD_BK_EN_V << LCDCAM_LCD_BK_EN_S)
#define LCDCAM_LCD_BK_EN_V  0x00000001U
#define LCDCAM_LCD_BK_EN_S  26
/** LCDCAM_LCD_AFIFO_RESET : WT; bitpos: [27]; default: 0;
 *  LCD AFIFO reset signal.
 */
#define LCDCAM_LCD_AFIFO_RESET    (BIT(27))
#define LCDCAM_LCD_AFIFO_RESET_M  (LCDCAM_LCD_AFIFO_RESET_V << LCDCAM_LCD_AFIFO_RESET_S)
#define LCDCAM_LCD_AFIFO_RESET_V  0x00000001U
#define LCDCAM_LCD_AFIFO_RESET_S  27
/** LCDCAM_LCD_CD_DATA_SET : R/W; bitpos: [28]; default: 0;
 *  1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DOUT state.  0: LCD_CD =
 *  reg_cd_idle_edge.
 */
#define LCDCAM_LCD_CD_DATA_SET    (BIT(28))
#define LCDCAM_LCD_CD_DATA_SET_M  (LCDCAM_LCD_CD_DATA_SET_V << LCDCAM_LCD_CD_DATA_SET_S)
#define LCDCAM_LCD_CD_DATA_SET_V  0x00000001U
#define LCDCAM_LCD_CD_DATA_SET_S  28
/** LCDCAM_LCD_CD_DUMMY_SET : R/W; bitpos: [29]; default: 0;
 *  1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DUMMY state.  0: LCD_CD =
 *  reg_cd_idle_edge.
 */
#define LCDCAM_LCD_CD_DUMMY_SET    (BIT(29))
#define LCDCAM_LCD_CD_DUMMY_SET_M  (LCDCAM_LCD_CD_DUMMY_SET_V << LCDCAM_LCD_CD_DUMMY_SET_S)
#define LCDCAM_LCD_CD_DUMMY_SET_V  0x00000001U
#define LCDCAM_LCD_CD_DUMMY_SET_S  29
/** LCDCAM_LCD_CD_CMD_SET : R/W; bitpos: [30]; default: 0;
 *  1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_CMD state.  0: LCD_CD =
 *  reg_cd_idle_edge.
 */
#define LCDCAM_LCD_CD_CMD_SET    (BIT(30))
#define LCDCAM_LCD_CD_CMD_SET_M  (LCDCAM_LCD_CD_CMD_SET_V << LCDCAM_LCD_CD_CMD_SET_S)
#define LCDCAM_LCD_CD_CMD_SET_V  0x00000001U
#define LCDCAM_LCD_CD_CMD_SET_S  30
/** LCDCAM_LCD_CD_IDLE_EDGE : R/W; bitpos: [31]; default: 0;
 *  The default value of LCD_CD.
 */
#define LCDCAM_LCD_CD_IDLE_EDGE    (BIT(31))
#define LCDCAM_LCD_CD_IDLE_EDGE_M  (LCDCAM_LCD_CD_IDLE_EDGE_V << LCDCAM_LCD_CD_IDLE_EDGE_S)
#define LCDCAM_LCD_CD_IDLE_EDGE_V  0x00000001U
#define LCDCAM_LCD_CD_IDLE_EDGE_S  31

/** LCDCAM_LCD_CTRL_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_CTRL_REG (DR_REG_LCDCAM_BASE + 0x1c)
/** LCDCAM_LCD_HB_FRONT : R/W; bitpos: [10:0]; default: 0;
 *  It is the horizontal blank front porch of a frame.
 */
#define LCDCAM_LCD_HB_FRONT    0x000007FFU
#define LCDCAM_LCD_HB_FRONT_M  (LCDCAM_LCD_HB_FRONT_V << LCDCAM_LCD_HB_FRONT_S)
#define LCDCAM_LCD_HB_FRONT_V  0x000007FFU
#define LCDCAM_LCD_HB_FRONT_S  0
/** LCDCAM_LCD_VA_HEIGHT : R/W; bitpos: [20:11]; default: 0;
 *  It is the vertical active height of a frame.
 */
#define LCDCAM_LCD_VA_HEIGHT    0x000003FFU
#define LCDCAM_LCD_VA_HEIGHT_M  (LCDCAM_LCD_VA_HEIGHT_V << LCDCAM_LCD_VA_HEIGHT_S)
#define LCDCAM_LCD_VA_HEIGHT_V  0x000003FFU
#define LCDCAM_LCD_VA_HEIGHT_S  11
/** LCDCAM_LCD_VT_HEIGHT : R/W; bitpos: [30:21]; default: 0;
 *  It is the vertical total height of a frame.
 */
#define LCDCAM_LCD_VT_HEIGHT    0x000003FFU
#define LCDCAM_LCD_VT_HEIGHT_M  (LCDCAM_LCD_VT_HEIGHT_V << LCDCAM_LCD_VT_HEIGHT_S)
#define LCDCAM_LCD_VT_HEIGHT_V  0x000003FFU
#define LCDCAM_LCD_VT_HEIGHT_S  21
/** LCDCAM_LCD_RGB_MODE_EN : R/W; bitpos: [31]; default: 0;
 *  1: Enable LCD RGB mode. 0: Disable LCD RGB mode.
 */
#define LCDCAM_LCD_RGB_MODE_EN    (BIT(31))
#define LCDCAM_LCD_RGB_MODE_EN_M  (LCDCAM_LCD_RGB_MODE_EN_V << LCDCAM_LCD_RGB_MODE_EN_S)
#define LCDCAM_LCD_RGB_MODE_EN_V  0x00000001U
#define LCDCAM_LCD_RGB_MODE_EN_S  31

/** LCDCAM_LCD_CTRL1_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_CTRL1_REG (DR_REG_LCDCAM_BASE + 0x20)
/** LCDCAM_LCD_VB_FRONT : R/W; bitpos: [7:0]; default: 0;
 *  It is the vertical blank front porch of a frame.
 */
#define LCDCAM_LCD_VB_FRONT    0x000000FFU
#define LCDCAM_LCD_VB_FRONT_M  (LCDCAM_LCD_VB_FRONT_V << LCDCAM_LCD_VB_FRONT_S)
#define LCDCAM_LCD_VB_FRONT_V  0x000000FFU
#define LCDCAM_LCD_VB_FRONT_S  0
/** LCDCAM_LCD_HA_WIDTH : R/W; bitpos: [19:8]; default: 0;
 *  It is the horizontal active width of a frame.
 */
#define LCDCAM_LCD_HA_WIDTH    0x00000FFFU
#define LCDCAM_LCD_HA_WIDTH_M  (LCDCAM_LCD_HA_WIDTH_V << LCDCAM_LCD_HA_WIDTH_S)
#define LCDCAM_LCD_HA_WIDTH_V  0x00000FFFU
#define LCDCAM_LCD_HA_WIDTH_S  8
/** LCDCAM_LCD_HT_WIDTH : R/W; bitpos: [31:20]; default: 0;
 *  It is the horizontal total width of a frame.
 */
#define LCDCAM_LCD_HT_WIDTH    0x00000FFFU
#define LCDCAM_LCD_HT_WIDTH_M  (LCDCAM_LCD_HT_WIDTH_V << LCDCAM_LCD_HT_WIDTH_S)
#define LCDCAM_LCD_HT_WIDTH_V  0x00000FFFU
#define LCDCAM_LCD_HT_WIDTH_S  20

/** LCDCAM_LCD_CTRL2_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_CTRL2_REG (DR_REG_LCDCAM_BASE + 0x24)
/** LCDCAM_LCD_VSYNC_WIDTH : R/W; bitpos: [6:0]; default: 1;
 *  It is the position of LCD_VSYNC active pulse in a line.
 */
#define LCDCAM_LCD_VSYNC_WIDTH    0x0000007FU
#define LCDCAM_LCD_VSYNC_WIDTH_M  (LCDCAM_LCD_VSYNC_WIDTH_V << LCDCAM_LCD_VSYNC_WIDTH_S)
#define LCDCAM_LCD_VSYNC_WIDTH_V  0x0000007FU
#define LCDCAM_LCD_VSYNC_WIDTH_S  0
/** LCDCAM_LCD_VSYNC_IDLE_POL : R/W; bitpos: [7]; default: 0;
 *  It is the idle value of LCD_VSYNC.
 */
#define LCDCAM_LCD_VSYNC_IDLE_POL    (BIT(7))
#define LCDCAM_LCD_VSYNC_IDLE_POL_M  (LCDCAM_LCD_VSYNC_IDLE_POL_V << LCDCAM_LCD_VSYNC_IDLE_POL_S)
#define LCDCAM_LCD_VSYNC_IDLE_POL_V  0x00000001U
#define LCDCAM_LCD_VSYNC_IDLE_POL_S  7
/** LCDCAM_LCD_DE_IDLE_POL : R/W; bitpos: [8]; default: 0;
 *  It is the idle value of LCD_DE.
 */
#define LCDCAM_LCD_DE_IDLE_POL    (BIT(8))
#define LCDCAM_LCD_DE_IDLE_POL_M  (LCDCAM_LCD_DE_IDLE_POL_V << LCDCAM_LCD_DE_IDLE_POL_S)
#define LCDCAM_LCD_DE_IDLE_POL_V  0x00000001U
#define LCDCAM_LCD_DE_IDLE_POL_S  8
/** LCDCAM_LCD_HS_BLANK_EN : R/W; bitpos: [9]; default: 0;
 *  1: The pulse of LCD_HSYNC is out in vertical blanking lines RGB mode. 0: LCD_HSYNC
 *  pulse is valid only in active region lines in RGB mode.
 */
#define LCDCAM_LCD_HS_BLANK_EN    (BIT(9))
#define LCDCAM_LCD_HS_BLANK_EN_M  (LCDCAM_LCD_HS_BLANK_EN_V << LCDCAM_LCD_HS_BLANK_EN_S)
#define LCDCAM_LCD_HS_BLANK_EN_V  0x00000001U
#define LCDCAM_LCD_HS_BLANK_EN_S  9
/** LCDCAM_LCD_HSYNC_WIDTH : R/W; bitpos: [22:16]; default: 1;
 *  It is the position of LCD_HSYNC active pulse in a line.
 */
#define LCDCAM_LCD_HSYNC_WIDTH    0x0000007FU
#define LCDCAM_LCD_HSYNC_WIDTH_M  (LCDCAM_LCD_HSYNC_WIDTH_V << LCDCAM_LCD_HSYNC_WIDTH_S)
#define LCDCAM_LCD_HSYNC_WIDTH_V  0x0000007FU
#define LCDCAM_LCD_HSYNC_WIDTH_S  16
/** LCDCAM_LCD_HSYNC_IDLE_POL : R/W; bitpos: [23]; default: 0;
 *  It is the idle value of LCD_HSYNC.
 */
#define LCDCAM_LCD_HSYNC_IDLE_POL    (BIT(23))
#define LCDCAM_LCD_HSYNC_IDLE_POL_M  (LCDCAM_LCD_HSYNC_IDLE_POL_V << LCDCAM_LCD_HSYNC_IDLE_POL_S)
#define LCDCAM_LCD_HSYNC_IDLE_POL_V  0x00000001U
#define LCDCAM_LCD_HSYNC_IDLE_POL_S  23
/** LCDCAM_LCD_HSYNC_POSITION : R/W; bitpos: [31:24]; default: 0;
 *  It is the position of LCD_HSYNC active pulse in a line.
 */
#define LCDCAM_LCD_HSYNC_POSITION    0x000000FFU
#define LCDCAM_LCD_HSYNC_POSITION_M  (LCDCAM_LCD_HSYNC_POSITION_V << LCDCAM_LCD_HSYNC_POSITION_S)
#define LCDCAM_LCD_HSYNC_POSITION_V  0x000000FFU
#define LCDCAM_LCD_HSYNC_POSITION_S  24

/** LCDCAM_LCD_FIRST_CMD_VAL_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_FIRST_CMD_VAL_REG (DR_REG_LCDCAM_BASE + 0x28)
/** LCDCAM_LCD_FIRST_CMD_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  The LCD write command value of first cmd cycle.
 */
#define LCDCAM_LCD_FIRST_CMD_VALUE    0xFFFFFFFFU
#define LCDCAM_LCD_FIRST_CMD_VALUE_M  (LCDCAM_LCD_FIRST_CMD_VALUE_V << LCDCAM_LCD_FIRST_CMD_VALUE_S)
#define LCDCAM_LCD_FIRST_CMD_VALUE_V  0xFFFFFFFFU
#define LCDCAM_LCD_FIRST_CMD_VALUE_S  0

/** LCDCAM_LCD_LATTER_CMD_VAL_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_LATTER_CMD_VAL_REG (DR_REG_LCDCAM_BASE + 0x2c)
/** LCDCAM_LCD_LATTER_CMD_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  The LCD write command value of latter cmd cycle.
 */
#define LCDCAM_LCD_LATTER_CMD_VALUE    0xFFFFFFFFU
#define LCDCAM_LCD_LATTER_CMD_VALUE_M  (LCDCAM_LCD_LATTER_CMD_VALUE_V << LCDCAM_LCD_LATTER_CMD_VALUE_S)
#define LCDCAM_LCD_LATTER_CMD_VALUE_V  0xFFFFFFFFU
#define LCDCAM_LCD_LATTER_CMD_VALUE_S  0

/** LCDCAM_LCD_DLY_MODE_CFG1_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_DLY_MODE_CFG1_REG (DR_REG_LCDCAM_BASE + 0x30)
/** LCDCAM_DOUT16_MODE : R/W; bitpos: [1:0]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT16_MODE    0x00000003U
#define LCDCAM_DOUT16_MODE_M  (LCDCAM_DOUT16_MODE_V << LCDCAM_DOUT16_MODE_S)
#define LCDCAM_DOUT16_MODE_V  0x00000003U
#define LCDCAM_DOUT16_MODE_S  0
/** LCDCAM_DOUT17_MODE : R/W; bitpos: [3:2]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT17_MODE    0x00000003U
#define LCDCAM_DOUT17_MODE_M  (LCDCAM_DOUT17_MODE_V << LCDCAM_DOUT17_MODE_S)
#define LCDCAM_DOUT17_MODE_V  0x00000003U
#define LCDCAM_DOUT17_MODE_S  2
/** LCDCAM_DOUT18_MODE : R/W; bitpos: [5:4]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT18_MODE    0x00000003U
#define LCDCAM_DOUT18_MODE_M  (LCDCAM_DOUT18_MODE_V << LCDCAM_DOUT18_MODE_S)
#define LCDCAM_DOUT18_MODE_V  0x00000003U
#define LCDCAM_DOUT18_MODE_S  4
/** LCDCAM_DOUT19_MODE : R/W; bitpos: [7:6]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT19_MODE    0x00000003U
#define LCDCAM_DOUT19_MODE_M  (LCDCAM_DOUT19_MODE_V << LCDCAM_DOUT19_MODE_S)
#define LCDCAM_DOUT19_MODE_V  0x00000003U
#define LCDCAM_DOUT19_MODE_S  6
/** LCDCAM_DOUT20_MODE : R/W; bitpos: [9:8]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT20_MODE    0x00000003U
#define LCDCAM_DOUT20_MODE_M  (LCDCAM_DOUT20_MODE_V << LCDCAM_DOUT20_MODE_S)
#define LCDCAM_DOUT20_MODE_V  0x00000003U
#define LCDCAM_DOUT20_MODE_S  8
/** LCDCAM_DOUT21_MODE : R/W; bitpos: [11:10]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT21_MODE    0x00000003U
#define LCDCAM_DOUT21_MODE_M  (LCDCAM_DOUT21_MODE_V << LCDCAM_DOUT21_MODE_S)
#define LCDCAM_DOUT21_MODE_V  0x00000003U
#define LCDCAM_DOUT21_MODE_S  10
/** LCDCAM_DOUT22_MODE : R/W; bitpos: [13:12]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT22_MODE    0x00000003U
#define LCDCAM_DOUT22_MODE_M  (LCDCAM_DOUT22_MODE_V << LCDCAM_DOUT22_MODE_S)
#define LCDCAM_DOUT22_MODE_V  0x00000003U
#define LCDCAM_DOUT22_MODE_S  12
/** LCDCAM_DOUT23_MODE : R/W; bitpos: [15:14]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT23_MODE    0x00000003U
#define LCDCAM_DOUT23_MODE_M  (LCDCAM_DOUT23_MODE_V << LCDCAM_DOUT23_MODE_S)
#define LCDCAM_DOUT23_MODE_V  0x00000003U
#define LCDCAM_DOUT23_MODE_S  14
/** LCDCAM_LCD_CD_MODE : R/W; bitpos: [17:16]; default: 0;
 *  The output LCD_CD is delayed by module clock LCD_CLK. 0: output without delayed. 1:
 *  delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
 */
#define LCDCAM_LCD_CD_MODE    0x00000003U
#define LCDCAM_LCD_CD_MODE_M  (LCDCAM_LCD_CD_MODE_V << LCDCAM_LCD_CD_MODE_S)
#define LCDCAM_LCD_CD_MODE_V  0x00000003U
#define LCDCAM_LCD_CD_MODE_S  16
/** LCDCAM_LCD_DE_MODE : R/W; bitpos: [19:18]; default: 0;
 *  The output LCD_DE is delayed by module clock LCD_CLK. 0: output without delayed. 1:
 *  delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
 */
#define LCDCAM_LCD_DE_MODE    0x00000003U
#define LCDCAM_LCD_DE_MODE_M  (LCDCAM_LCD_DE_MODE_V << LCDCAM_LCD_DE_MODE_S)
#define LCDCAM_LCD_DE_MODE_V  0x00000003U
#define LCDCAM_LCD_DE_MODE_S  18
/** LCDCAM_LCD_HSYNC_MODE : R/W; bitpos: [21:20]; default: 0;
 *  The output LCD_HSYNC is delayed by module clock LCD_CLK. 0: output without delayed.
 *  1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
 */
#define LCDCAM_LCD_HSYNC_MODE    0x00000003U
#define LCDCAM_LCD_HSYNC_MODE_M  (LCDCAM_LCD_HSYNC_MODE_V << LCDCAM_LCD_HSYNC_MODE_S)
#define LCDCAM_LCD_HSYNC_MODE_V  0x00000003U
#define LCDCAM_LCD_HSYNC_MODE_S  20
/** LCDCAM_LCD_VSYNC_MODE : R/W; bitpos: [23:22]; default: 0;
 *  The output LCD_VSYNC is delayed by module clock LCD_CLK. 0: output without delayed.
 *  1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
 */
#define LCDCAM_LCD_VSYNC_MODE    0x00000003U
#define LCDCAM_LCD_VSYNC_MODE_M  (LCDCAM_LCD_VSYNC_MODE_V << LCDCAM_LCD_VSYNC_MODE_S)
#define LCDCAM_LCD_VSYNC_MODE_V  0x00000003U
#define LCDCAM_LCD_VSYNC_MODE_S  22

/** LCDCAM_LCD_DLY_MODE_CFG2_REG register
 *  LCD config register.
 */
#define LCDCAM_LCD_DLY_MODE_CFG2_REG (DR_REG_LCDCAM_BASE + 0x38)
/** LCDCAM_DOUT0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT0_MODE    0x00000003U
#define LCDCAM_DOUT0_MODE_M  (LCDCAM_DOUT0_MODE_V << LCDCAM_DOUT0_MODE_S)
#define LCDCAM_DOUT0_MODE_V  0x00000003U
#define LCDCAM_DOUT0_MODE_S  0
/** LCDCAM_DOUT1_MODE : R/W; bitpos: [3:2]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT1_MODE    0x00000003U
#define LCDCAM_DOUT1_MODE_M  (LCDCAM_DOUT1_MODE_V << LCDCAM_DOUT1_MODE_S)
#define LCDCAM_DOUT1_MODE_V  0x00000003U
#define LCDCAM_DOUT1_MODE_S  2
/** LCDCAM_DOUT2_MODE : R/W; bitpos: [5:4]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT2_MODE    0x00000003U
#define LCDCAM_DOUT2_MODE_M  (LCDCAM_DOUT2_MODE_V << LCDCAM_DOUT2_MODE_S)
#define LCDCAM_DOUT2_MODE_V  0x00000003U
#define LCDCAM_DOUT2_MODE_S  4
/** LCDCAM_DOUT3_MODE : R/W; bitpos: [7:6]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT3_MODE    0x00000003U
#define LCDCAM_DOUT3_MODE_M  (LCDCAM_DOUT3_MODE_V << LCDCAM_DOUT3_MODE_S)
#define LCDCAM_DOUT3_MODE_V  0x00000003U
#define LCDCAM_DOUT3_MODE_S  6
/** LCDCAM_DOUT4_MODE : R/W; bitpos: [9:8]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT4_MODE    0x00000003U
#define LCDCAM_DOUT4_MODE_M  (LCDCAM_DOUT4_MODE_V << LCDCAM_DOUT4_MODE_S)
#define LCDCAM_DOUT4_MODE_V  0x00000003U
#define LCDCAM_DOUT4_MODE_S  8
/** LCDCAM_DOUT5_MODE : R/W; bitpos: [11:10]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT5_MODE    0x00000003U
#define LCDCAM_DOUT5_MODE_M  (LCDCAM_DOUT5_MODE_V << LCDCAM_DOUT5_MODE_S)
#define LCDCAM_DOUT5_MODE_V  0x00000003U
#define LCDCAM_DOUT5_MODE_S  10
/** LCDCAM_DOUT6_MODE : R/W; bitpos: [13:12]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT6_MODE    0x00000003U
#define LCDCAM_DOUT6_MODE_M  (LCDCAM_DOUT6_MODE_V << LCDCAM_DOUT6_MODE_S)
#define LCDCAM_DOUT6_MODE_V  0x00000003U
#define LCDCAM_DOUT6_MODE_S  12
/** LCDCAM_DOUT7_MODE : R/W; bitpos: [15:14]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT7_MODE    0x00000003U
#define LCDCAM_DOUT7_MODE_M  (LCDCAM_DOUT7_MODE_V << LCDCAM_DOUT7_MODE_S)
#define LCDCAM_DOUT7_MODE_V  0x00000003U
#define LCDCAM_DOUT7_MODE_S  14
/** LCDCAM_DOUT8_MODE : R/W; bitpos: [17:16]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT8_MODE    0x00000003U
#define LCDCAM_DOUT8_MODE_M  (LCDCAM_DOUT8_MODE_V << LCDCAM_DOUT8_MODE_S)
#define LCDCAM_DOUT8_MODE_V  0x00000003U
#define LCDCAM_DOUT8_MODE_S  16
/** LCDCAM_DOUT9_MODE : R/W; bitpos: [19:18]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT9_MODE    0x00000003U
#define LCDCAM_DOUT9_MODE_M  (LCDCAM_DOUT9_MODE_V << LCDCAM_DOUT9_MODE_S)
#define LCDCAM_DOUT9_MODE_V  0x00000003U
#define LCDCAM_DOUT9_MODE_S  18
/** LCDCAM_DOUT10_MODE : R/W; bitpos: [21:20]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT10_MODE    0x00000003U
#define LCDCAM_DOUT10_MODE_M  (LCDCAM_DOUT10_MODE_V << LCDCAM_DOUT10_MODE_S)
#define LCDCAM_DOUT10_MODE_V  0x00000003U
#define LCDCAM_DOUT10_MODE_S  20
/** LCDCAM_DOUT11_MODE : R/W; bitpos: [23:22]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT11_MODE    0x00000003U
#define LCDCAM_DOUT11_MODE_M  (LCDCAM_DOUT11_MODE_V << LCDCAM_DOUT11_MODE_S)
#define LCDCAM_DOUT11_MODE_V  0x00000003U
#define LCDCAM_DOUT11_MODE_S  22
/** LCDCAM_DOUT12_MODE : R/W; bitpos: [25:24]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT12_MODE    0x00000003U
#define LCDCAM_DOUT12_MODE_M  (LCDCAM_DOUT12_MODE_V << LCDCAM_DOUT12_MODE_S)
#define LCDCAM_DOUT12_MODE_V  0x00000003U
#define LCDCAM_DOUT12_MODE_S  24
/** LCDCAM_DOUT13_MODE : R/W; bitpos: [27:26]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT13_MODE    0x00000003U
#define LCDCAM_DOUT13_MODE_M  (LCDCAM_DOUT13_MODE_V << LCDCAM_DOUT13_MODE_S)
#define LCDCAM_DOUT13_MODE_V  0x00000003U
#define LCDCAM_DOUT13_MODE_S  26
/** LCDCAM_DOUT14_MODE : R/W; bitpos: [29:28]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT14_MODE    0x00000003U
#define LCDCAM_DOUT14_MODE_M  (LCDCAM_DOUT14_MODE_V << LCDCAM_DOUT14_MODE_S)
#define LCDCAM_DOUT14_MODE_V  0x00000003U
#define LCDCAM_DOUT14_MODE_S  28
/** LCDCAM_DOUT15_MODE : R/W; bitpos: [31:30]; default: 0;
 *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
 *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
 *  LCD_CLK.
 */
#define LCDCAM_DOUT15_MODE    0x00000003U
#define LCDCAM_DOUT15_MODE_M  (LCDCAM_DOUT15_MODE_V << LCDCAM_DOUT15_MODE_S)
#define LCDCAM_DOUT15_MODE_V  0x00000003U
#define LCDCAM_DOUT15_MODE_S  30

/** LCDCAM_LC_DMA_INT_ENA_REG register
 *  LCDCAM interrupt enable register.
 */
#define LCDCAM_LC_DMA_INT_ENA_REG (DR_REG_LCDCAM_BASE + 0x64)
/** LCDCAM_LCD_VSYNC_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The enable bit for LCD frame end interrupt.
 */
#define LCDCAM_LCD_VSYNC_INT_ENA    (BIT(0))
#define LCDCAM_LCD_VSYNC_INT_ENA_M  (LCDCAM_LCD_VSYNC_INT_ENA_V << LCDCAM_LCD_VSYNC_INT_ENA_S)
#define LCDCAM_LCD_VSYNC_INT_ENA_V  0x00000001U
#define LCDCAM_LCD_VSYNC_INT_ENA_S  0
/** LCDCAM_LCD_TRANS_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The enable bit for lcd transfer end interrupt.
 */
#define LCDCAM_LCD_TRANS_DONE_INT_ENA    (BIT(1))
#define LCDCAM_LCD_TRANS_DONE_INT_ENA_M  (LCDCAM_LCD_TRANS_DONE_INT_ENA_V << LCDCAM_LCD_TRANS_DONE_INT_ENA_S)
#define LCDCAM_LCD_TRANS_DONE_INT_ENA_V  0x00000001U
#define LCDCAM_LCD_TRANS_DONE_INT_ENA_S  1
/** LCDCAM_CAM_VSYNC_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The enable bit for Camera frame end interrupt.
 */
#define LCDCAM_CAM_VSYNC_INT_ENA    (BIT(2))
#define LCDCAM_CAM_VSYNC_INT_ENA_M  (LCDCAM_CAM_VSYNC_INT_ENA_V << LCDCAM_CAM_VSYNC_INT_ENA_S)
#define LCDCAM_CAM_VSYNC_INT_ENA_V  0x00000001U
#define LCDCAM_CAM_VSYNC_INT_ENA_S  2
/** LCDCAM_CAM_HS_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The enable bit for Camera line interrupt.
 */
#define LCDCAM_CAM_HS_INT_ENA    (BIT(3))
#define LCDCAM_CAM_HS_INT_ENA_M  (LCDCAM_CAM_HS_INT_ENA_V << LCDCAM_CAM_HS_INT_ENA_S)
#define LCDCAM_CAM_HS_INT_ENA_V  0x00000001U
#define LCDCAM_CAM_HS_INT_ENA_S  3

/** LCDCAM_LC_DMA_INT_RAW_REG register
 *  LCDCAM interrupt raw register, valid in level.
 */
#define LCDCAM_LC_DMA_INT_RAW_REG (DR_REG_LCDCAM_BASE + 0x68)
/** LCDCAM_LCD_VSYNC_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw bit for LCD frame end interrupt.
 */
#define LCDCAM_LCD_VSYNC_INT_RAW    (BIT(0))
#define LCDCAM_LCD_VSYNC_INT_RAW_M  (LCDCAM_LCD_VSYNC_INT_RAW_V << LCDCAM_LCD_VSYNC_INT_RAW_S)
#define LCDCAM_LCD_VSYNC_INT_RAW_V  0x00000001U
#define LCDCAM_LCD_VSYNC_INT_RAW_S  0
/** LCDCAM_LCD_TRANS_DONE_INT_RAW : RO/WTC/SS; bitpos: [1]; default: 0;
 *  The raw bit for lcd transfer end interrupt.
 */
#define LCDCAM_LCD_TRANS_DONE_INT_RAW    (BIT(1))
#define LCDCAM_LCD_TRANS_DONE_INT_RAW_M  (LCDCAM_LCD_TRANS_DONE_INT_RAW_V << LCDCAM_LCD_TRANS_DONE_INT_RAW_S)
#define LCDCAM_LCD_TRANS_DONE_INT_RAW_V  0x00000001U
#define LCDCAM_LCD_TRANS_DONE_INT_RAW_S  1
/** LCDCAM_CAM_VSYNC_INT_RAW : RO/WTC/SS; bitpos: [2]; default: 0;
 *  The raw bit for Camera frame end interrupt.
 */
#define LCDCAM_CAM_VSYNC_INT_RAW    (BIT(2))
#define LCDCAM_CAM_VSYNC_INT_RAW_M  (LCDCAM_CAM_VSYNC_INT_RAW_V << LCDCAM_CAM_VSYNC_INT_RAW_S)
#define LCDCAM_CAM_VSYNC_INT_RAW_V  0x00000001U
#define LCDCAM_CAM_VSYNC_INT_RAW_S  2
/** LCDCAM_CAM_HS_INT_RAW : RO/WTC/SS; bitpos: [3]; default: 0;
 *  The raw bit for Camera line interrupt.
 */
#define LCDCAM_CAM_HS_INT_RAW    (BIT(3))
#define LCDCAM_CAM_HS_INT_RAW_M  (LCDCAM_CAM_HS_INT_RAW_V << LCDCAM_CAM_HS_INT_RAW_S)
#define LCDCAM_CAM_HS_INT_RAW_V  0x00000001U
#define LCDCAM_CAM_HS_INT_RAW_S  3

/** LCDCAM_LC_DMA_INT_ST_REG register
 *  LCDCAM interrupt status register.
 */
#define LCDCAM_LC_DMA_INT_ST_REG (DR_REG_LCDCAM_BASE + 0x6c)
/** LCDCAM_LCD_VSYNC_INT_ST : RO; bitpos: [0]; default: 0;
 *  The status bit for LCD frame end interrupt.
 */
#define LCDCAM_LCD_VSYNC_INT_ST    (BIT(0))
#define LCDCAM_LCD_VSYNC_INT_ST_M  (LCDCAM_LCD_VSYNC_INT_ST_V << LCDCAM_LCD_VSYNC_INT_ST_S)
#define LCDCAM_LCD_VSYNC_INT_ST_V  0x00000001U
#define LCDCAM_LCD_VSYNC_INT_ST_S  0
/** LCDCAM_LCD_TRANS_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The status bit for lcd transfer end interrupt.
 */
#define LCDCAM_LCD_TRANS_DONE_INT_ST    (BIT(1))
#define LCDCAM_LCD_TRANS_DONE_INT_ST_M  (LCDCAM_LCD_TRANS_DONE_INT_ST_V << LCDCAM_LCD_TRANS_DONE_INT_ST_S)
#define LCDCAM_LCD_TRANS_DONE_INT_ST_V  0x00000001U
#define LCDCAM_LCD_TRANS_DONE_INT_ST_S  1
/** LCDCAM_CAM_VSYNC_INT_ST : RO; bitpos: [2]; default: 0;
 *  The status bit for Camera frame end interrupt.
 */
#define LCDCAM_CAM_VSYNC_INT_ST    (BIT(2))
#define LCDCAM_CAM_VSYNC_INT_ST_M  (LCDCAM_CAM_VSYNC_INT_ST_V << LCDCAM_CAM_VSYNC_INT_ST_S)
#define LCDCAM_CAM_VSYNC_INT_ST_V  0x00000001U
#define LCDCAM_CAM_VSYNC_INT_ST_S  2
/** LCDCAM_CAM_HS_INT_ST : RO; bitpos: [3]; default: 0;
 *  The status bit for Camera transfer end interrupt.
 */
#define LCDCAM_CAM_HS_INT_ST    (BIT(3))
#define LCDCAM_CAM_HS_INT_ST_M  (LCDCAM_CAM_HS_INT_ST_V << LCDCAM_CAM_HS_INT_ST_S)
#define LCDCAM_CAM_HS_INT_ST_V  0x00000001U
#define LCDCAM_CAM_HS_INT_ST_S  3

/** LCDCAM_LC_DMA_INT_CLR_REG register
 *  LCDCAM interrupt clear register.
 */
#define LCDCAM_LC_DMA_INT_CLR_REG (DR_REG_LCDCAM_BASE + 0x70)
/** LCDCAM_LCD_VSYNC_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The clear bit for LCD frame end interrupt.
 */
#define LCDCAM_LCD_VSYNC_INT_CLR    (BIT(0))
#define LCDCAM_LCD_VSYNC_INT_CLR_M  (LCDCAM_LCD_VSYNC_INT_CLR_V << LCDCAM_LCD_VSYNC_INT_CLR_S)
#define LCDCAM_LCD_VSYNC_INT_CLR_V  0x00000001U
#define LCDCAM_LCD_VSYNC_INT_CLR_S  0
/** LCDCAM_LCD_TRANS_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The clear bit for lcd transfer end interrupt.
 */
#define LCDCAM_LCD_TRANS_DONE_INT_CLR    (BIT(1))
#define LCDCAM_LCD_TRANS_DONE_INT_CLR_M  (LCDCAM_LCD_TRANS_DONE_INT_CLR_V << LCDCAM_LCD_TRANS_DONE_INT_CLR_S)
#define LCDCAM_LCD_TRANS_DONE_INT_CLR_V  0x00000001U
#define LCDCAM_LCD_TRANS_DONE_INT_CLR_S  1
/** LCDCAM_CAM_VSYNC_INT_CLR : WT; bitpos: [2]; default: 0;
 *  The clear bit for Camera frame end interrupt.
 */
#define LCDCAM_CAM_VSYNC_INT_CLR    (BIT(2))
#define LCDCAM_CAM_VSYNC_INT_CLR_M  (LCDCAM_CAM_VSYNC_INT_CLR_V << LCDCAM_CAM_VSYNC_INT_CLR_S)
#define LCDCAM_CAM_VSYNC_INT_CLR_V  0x00000001U
#define LCDCAM_CAM_VSYNC_INT_CLR_S  2
/** LCDCAM_CAM_HS_INT_CLR : WT; bitpos: [3]; default: 0;
 *  The clear bit for Camera line interrupt.
 */
#define LCDCAM_CAM_HS_INT_CLR    (BIT(3))
#define LCDCAM_CAM_HS_INT_CLR_M  (LCDCAM_CAM_HS_INT_CLR_V << LCDCAM_CAM_HS_INT_CLR_S)
#define LCDCAM_CAM_HS_INT_CLR_V  0x00000001U
#define LCDCAM_CAM_HS_INT_CLR_S  3

/** LCDCAM_LC_REG_DATE_REG register
 *  Version register
 */
#define LCDCAM_LC_REG_DATE_REG (DR_REG_LCDCAM_BASE + 0xfc)
/** LCDCAM_LC_DATE : R/W; bitpos: [27:0]; default: 36712592;
 *  LCD_CAM version control register
 */
#define LCDCAM_LC_DATE    0x0FFFFFFFU
#define LCDCAM_LC_DATE_M  (LCDCAM_LC_DATE_V << LCDCAM_LC_DATE_S)
#define LCDCAM_LC_DATE_V  0x0FFFFFFFU
#define LCDCAM_LC_DATE_S  0

#ifdef __cplusplus
}
#endif
