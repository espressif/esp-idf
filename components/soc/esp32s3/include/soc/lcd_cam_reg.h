// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"

#define LCD_CAM_LCD_CLOCK_REG (DR_REG_LCD_CAM_BASE + 0x000)
/* LCD_CAM_CLK_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set this bit to enable clk gate*/
#define LCD_CAM_CLK_EN (BIT(31))
#define LCD_CAM_CLK_EN_M (BIT(31))
#define LCD_CAM_CLK_EN_V 0x1
#define LCD_CAM_CLK_EN_S 31
/* LCD_CAM_LCD_CLK_SEL : R/W ;bitpos:[30:29] ;default: 2'b0 ; */
/*description: Select LCD module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.*/
#define LCD_CAM_LCD_CLK_SEL 0x00000003
#define LCD_CAM_LCD_CLK_SEL_M ((LCD_CAM_LCD_CLK_SEL_V) << (LCD_CAM_LCD_CLK_SEL_S))
#define LCD_CAM_LCD_CLK_SEL_V 0x3
#define LCD_CAM_LCD_CLK_SEL_S 29
/* LCD_CAM_LCD_CLKM_DIV_A : R/W ;bitpos:[28:23] ;default: 6'h0 ; */
/*description: Fractional clock divider denominator value*/
#define LCD_CAM_LCD_CLKM_DIV_A 0x0000003F
#define LCD_CAM_LCD_CLKM_DIV_A_M ((LCD_CAM_LCD_CLKM_DIV_A_V) << (LCD_CAM_LCD_CLKM_DIV_A_S))
#define LCD_CAM_LCD_CLKM_DIV_A_V 0x3F
#define LCD_CAM_LCD_CLKM_DIV_A_S 23
/* LCD_CAM_LCD_CLKM_DIV_B : R/W ;bitpos:[22:17] ;default: 6'h0 ; */
/*description: Fractional clock divider numerator value*/
#define LCD_CAM_LCD_CLKM_DIV_B 0x0000003F
#define LCD_CAM_LCD_CLKM_DIV_B_M ((LCD_CAM_LCD_CLKM_DIV_B_V) << (LCD_CAM_LCD_CLKM_DIV_B_S))
#define LCD_CAM_LCD_CLKM_DIV_B_V 0x3F
#define LCD_CAM_LCD_CLKM_DIV_B_S 17
/* LCD_CAM_LCD_CLKM_DIV_NUM : R/W ;bitpos:[16:9] ;default: 8'd4 ; */
/*description: Integral LCD clock divider value*/
#define LCD_CAM_LCD_CLKM_DIV_NUM 0x000000FF
#define LCD_CAM_LCD_CLKM_DIV_NUM_M ((LCD_CAM_LCD_CLKM_DIV_NUM_V) << (LCD_CAM_LCD_CLKM_DIV_NUM_S))
#define LCD_CAM_LCD_CLKM_DIV_NUM_V 0xFF
#define LCD_CAM_LCD_CLKM_DIV_NUM_S 9
/* LCD_CAM_LCD_CK_OUT_EDGE : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define LCD_CAM_LCD_CK_OUT_EDGE (BIT(8))
#define LCD_CAM_LCD_CK_OUT_EDGE_M (BIT(8))
#define LCD_CAM_LCD_CK_OUT_EDGE_V 0x1
#define LCD_CAM_LCD_CK_OUT_EDGE_S 8
/* LCD_CAM_LCD_CK_IDLE_EDGE : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: 1: LCD_PCLK line is high when idle     0: LCD_PCLK line is low when idle.*/
#define LCD_CAM_LCD_CK_IDLE_EDGE (BIT(7))
#define LCD_CAM_LCD_CK_IDLE_EDGE_M (BIT(7))
#define LCD_CAM_LCD_CK_IDLE_EDGE_V 0x1
#define LCD_CAM_LCD_CK_IDLE_EDGE_S 7
/* LCD_CAM_LCD_CLK_EQU_SYSCLK : R/W ;bitpos:[6] ;default: 1'h1 ; */
/*description: 1: f_LCD_PCLK = f_LCD_CLK. 0: f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1).*/
#define LCD_CAM_LCD_CLK_EQU_SYSCLK (BIT(6))
#define LCD_CAM_LCD_CLK_EQU_SYSCLK_M (BIT(6))
#define LCD_CAM_LCD_CLK_EQU_SYSCLK_V 0x1
#define LCD_CAM_LCD_CLK_EQU_SYSCLK_S 6
/* LCD_CAM_LCD_CLKCNT_N : R/W ;bitpos:[5:0] ;default: 6'h3 ; */
/*description: f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1) when reg_clk_equ_sysclk is 0.*/
#define LCD_CAM_LCD_CLKCNT_N 0x0000003F
#define LCD_CAM_LCD_CLKCNT_N_M ((LCD_CAM_LCD_CLKCNT_N_V) << (LCD_CAM_LCD_CLKCNT_N_S))
#define LCD_CAM_LCD_CLKCNT_N_V 0x3F
#define LCD_CAM_LCD_CLKCNT_N_S 0

#define LCD_CAM_CAM_CTRL_REG (DR_REG_LCD_CAM_BASE + 0x004)
/* LCD_CAM_CAM_CLK_SEL : R/W ;bitpos:[30:29] ;default: 2'b0 ; */
/*description: Select Camera module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.*/
#define LCD_CAM_CAM_CLK_SEL 0x00000003
#define LCD_CAM_CAM_CLK_SEL_M ((LCD_CAM_CAM_CLK_SEL_V) << (LCD_CAM_CAM_CLK_SEL_S))
#define LCD_CAM_CAM_CLK_SEL_V 0x3
#define LCD_CAM_CAM_CLK_SEL_S 29
/* LCD_CAM_CAM_CLKM_DIV_A : R/W ;bitpos:[28:23] ;default: 6'h0 ; */
/*description: Fractional clock divider denominator value*/
#define LCD_CAM_CAM_CLKM_DIV_A 0x0000003F
#define LCD_CAM_CAM_CLKM_DIV_A_M ((LCD_CAM_CAM_CLKM_DIV_A_V) << (LCD_CAM_CAM_CLKM_DIV_A_S))
#define LCD_CAM_CAM_CLKM_DIV_A_V 0x3F
#define LCD_CAM_CAM_CLKM_DIV_A_S 23
/* LCD_CAM_CAM_CLKM_DIV_B : R/W ;bitpos:[22:17] ;default: 6'h0 ; */
/*description: Fractional clock divider numerator value*/
#define LCD_CAM_CAM_CLKM_DIV_B 0x0000003F
#define LCD_CAM_CAM_CLKM_DIV_B_M ((LCD_CAM_CAM_CLKM_DIV_B_V) << (LCD_CAM_CAM_CLKM_DIV_B_S))
#define LCD_CAM_CAM_CLKM_DIV_B_V 0x3F
#define LCD_CAM_CAM_CLKM_DIV_B_S 17
/* LCD_CAM_CAM_CLKM_DIV_NUM : R/W ;bitpos:[16:9] ;default: 8'd4 ; */
/*description: Integral Camera clock divider value*/
#define LCD_CAM_CAM_CLKM_DIV_NUM 0x000000FF
#define LCD_CAM_CAM_CLKM_DIV_NUM_M ((LCD_CAM_CAM_CLKM_DIV_NUM_V) << (LCD_CAM_CAM_CLKM_DIV_NUM_S))
#define LCD_CAM_CAM_CLKM_DIV_NUM_V 0xFF
#define LCD_CAM_CAM_CLKM_DIV_NUM_S 9
/* LCD_CAM_CAM_VS_EOF_EN : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: 1: CAM_VSYNC to generate in_suc_eof. 0: in_suc_eof is controlled
 by reg_cam_rec_data_cyclelen.*/
#define LCD_CAM_CAM_VS_EOF_EN (BIT(8))
#define LCD_CAM_CAM_VS_EOF_EN_M (BIT(8))
#define LCD_CAM_CAM_VS_EOF_EN_V 0x1
#define LCD_CAM_CAM_VS_EOF_EN_S 8
/* LCD_CAM_CAM_LINE_INT_EN : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: 1: Enable to generate CAM_HS_INT. 0: Disable.*/
#define LCD_CAM_CAM_LINE_INT_EN (BIT(7))
#define LCD_CAM_CAM_LINE_INT_EN_M (BIT(7))
#define LCD_CAM_CAM_LINE_INT_EN_V 0x1
#define LCD_CAM_CAM_LINE_INT_EN_S 7
/* LCD_CAM_CAM_BIT_ORDER : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: 1: invert data byte order  only valid in 2 byte mode. 0: Not change.*/
#define LCD_CAM_CAM_BIT_ORDER (BIT(6))
#define LCD_CAM_CAM_BIT_ORDER_M (BIT(6))
#define LCD_CAM_CAM_BIT_ORDER_V 0x1
#define LCD_CAM_CAM_BIT_ORDER_S 6
/* LCD_CAM_CAM_BYTE_ORDER : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: 1: Change data bit order  change CAM_DATA_in[7:0] to CAM_DATA_in[0:7]
 in one byte mode  and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.*/
#define LCD_CAM_CAM_BYTE_ORDER (BIT(5))
#define LCD_CAM_CAM_BYTE_ORDER_M (BIT(5))
#define LCD_CAM_CAM_BYTE_ORDER_V 0x1
#define LCD_CAM_CAM_BYTE_ORDER_S 5
/* LCD_CAM_CAM_UPDATE_REG : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: 1: Update Camera registers  will be cleared by hardware. 0 : Not care.*/
#define LCD_CAM_CAM_UPDATE_REG (BIT(4))
#define LCD_CAM_CAM_UPDATE_REG_M (BIT(4))
#define LCD_CAM_CAM_UPDATE_REG_V 0x1
#define LCD_CAM_CAM_UPDATE_REG_S 4
/* LCD_CAM_CAM_VSYNC_FILTER_THRES : R/W ;bitpos:[3:1] ;default: 3'h0 ; */
/*description: Filter threshold value for CAM_VSYNC signal.*/
#define LCD_CAM_CAM_VSYNC_FILTER_THRES 0x00000007
#define LCD_CAM_CAM_VSYNC_FILTER_THRES_M ((LCD_CAM_CAM_VSYNC_FILTER_THRES_V) << (LCD_CAM_CAM_VSYNC_FILTER_THRES_S))
#define LCD_CAM_CAM_VSYNC_FILTER_THRES_V 0x7
#define LCD_CAM_CAM_VSYNC_FILTER_THRES_S 1
/* LCD_CAM_CAM_STOP_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Camera stop enable signal  1: camera stops when DMA Rx FIFO is
 full. 0: Not stop.*/
#define LCD_CAM_CAM_STOP_EN (BIT(0))
#define LCD_CAM_CAM_STOP_EN_M (BIT(0))
#define LCD_CAM_CAM_STOP_EN_V 0x1
#define LCD_CAM_CAM_STOP_EN_S 0

#define LCD_CAM_CAM_CTRL1_REG (DR_REG_LCD_CAM_BASE + 0x008)
/* LCD_CAM_CAM_AFIFO_RESET : WO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Camera AFIFO reset signal.*/
#define LCD_CAM_CAM_AFIFO_RESET (BIT(31))
#define LCD_CAM_CAM_AFIFO_RESET_M (BIT(31))
#define LCD_CAM_CAM_AFIFO_RESET_V 0x1
#define LCD_CAM_CAM_AFIFO_RESET_S 31
/* LCD_CAM_CAM_RESET : WO ;bitpos:[30] ;default: 1'h0 ; */
/*description: Camera module reset signal.*/
#define LCD_CAM_CAM_RESET (BIT(30))
#define LCD_CAM_CAM_RESET_M (BIT(30))
#define LCD_CAM_CAM_RESET_V 0x1
#define LCD_CAM_CAM_RESET_S 30
/* LCD_CAM_CAM_START : R/W ;bitpos:[29] ;default: 1'h0 ; */
/*description: Camera module start signal.*/
#define LCD_CAM_CAM_START (BIT(29))
#define LCD_CAM_CAM_START_M (BIT(29))
#define LCD_CAM_CAM_START_V 0x1
#define LCD_CAM_CAM_START_S 29
/* LCD_CAM_CAM_VH_DE_MODE_EN : R/W ;bitpos:[28] ;default: 1'h0 ; */
/*description: 1: Input control signals are CAM_DE CAM_HSYNC and CAM_VSYNC is
 1. 0: Input control signals are CAM_DE and CAM_VSYNC*/
#define LCD_CAM_CAM_VH_DE_MODE_EN (BIT(28))
#define LCD_CAM_CAM_VH_DE_MODE_EN_M (BIT(28))
#define LCD_CAM_CAM_VH_DE_MODE_EN_V 0x1
#define LCD_CAM_CAM_VH_DE_MODE_EN_S 28
/* LCD_CAM_CAM_VSYNC_INV : R/W ;bitpos:[27] ;default: 1'h0 ; */
/*description: CAM_VSYNC invert enable signal  valid in high level.*/
#define LCD_CAM_CAM_VSYNC_INV (BIT(27))
#define LCD_CAM_CAM_VSYNC_INV_M (BIT(27))
#define LCD_CAM_CAM_VSYNC_INV_V 0x1
#define LCD_CAM_CAM_VSYNC_INV_S 27
/* LCD_CAM_CAM_HSYNC_INV : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: CAM_HSYNC invert enable signal  valid in high level.*/
#define LCD_CAM_CAM_HSYNC_INV (BIT(26))
#define LCD_CAM_CAM_HSYNC_INV_M (BIT(26))
#define LCD_CAM_CAM_HSYNC_INV_V 0x1
#define LCD_CAM_CAM_HSYNC_INV_S 26
/* LCD_CAM_CAM_DE_INV : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: CAM_DE invert enable signal  valid in high level.*/
#define LCD_CAM_CAM_DE_INV (BIT(25))
#define LCD_CAM_CAM_DE_INV_M (BIT(25))
#define LCD_CAM_CAM_DE_INV_V 0x1
#define LCD_CAM_CAM_DE_INV_S 25
/* LCD_CAM_CAM_2BYTE_EN : R/W ;bitpos:[24] ;default: 1'h0 ; */
/*description: 1: The bit number of input data is 9~16.  0: The bit number of
 input data is 0~8.*/
#define LCD_CAM_CAM_2BYTE_EN (BIT(24))
#define LCD_CAM_CAM_2BYTE_EN_M (BIT(24))
#define LCD_CAM_CAM_2BYTE_EN_V 0x1
#define LCD_CAM_CAM_2BYTE_EN_S 24
/* LCD_CAM_CAM_VSYNC_FILTER_EN : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: 1: Enable CAM_VSYNC filter function. 0: bypass.*/
#define LCD_CAM_CAM_VSYNC_FILTER_EN (BIT(23))
#define LCD_CAM_CAM_VSYNC_FILTER_EN_M (BIT(23))
#define LCD_CAM_CAM_VSYNC_FILTER_EN_V 0x1
#define LCD_CAM_CAM_VSYNC_FILTER_EN_S 23
/* LCD_CAM_CAM_CLK_INV : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: 1: Invert  the input signal CAM_PCLK. 0: Not invert.*/
#define LCD_CAM_CAM_CLK_INV (BIT(21))
#define LCD_CAM_CAM_CLK_INV_M (BIT(21))
#define LCD_CAM_CAM_CLK_INV_V 0x1
#define LCD_CAM_CAM_CLK_INV_S 21
/* LCD_CAM_CAM_LINE_INT_NUM : R/W ;bitpos:[20:14] ;default: 7'h0 ; */
/*description: The line number minus 1 to generate cam_hs_int.*/
#define LCD_CAM_CAM_LINE_INT_NUM 0x0000007F
#define LCD_CAM_CAM_LINE_INT_NUM_M ((LCD_CAM_CAM_LINE_INT_NUM_V) << (LCD_CAM_CAM_LINE_INT_NUM_S))
#define LCD_CAM_CAM_LINE_INT_NUM_V 0x7F
#define LCD_CAM_CAM_LINE_INT_NUM_S 14
/* LCD_CAM_CAM_REC_DATA_BYTELEN : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: Camera receive data byte length minus 1 to set DMA in_suc_eof_int.*/
#define LCD_CAM_CAM_REC_DATA_BYTELEN 0x00003FFF
#define LCD_CAM_CAM_REC_DATA_BYTELEN_M ((LCD_CAM_CAM_REC_DATA_BYTELEN_V) << (LCD_CAM_CAM_REC_DATA_BYTELEN_S))
#define LCD_CAM_CAM_REC_DATA_BYTELEN_V 0x3FFF
#define LCD_CAM_CAM_REC_DATA_BYTELEN_S 0

#define LCD_CAM_CAM_RGB_YUV_REG (DR_REG_LCD_CAM_BASE + 0x00C)
/* LCD_CAM_CAM_CONV_BYPASS : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 0: Bypass converter. 1: Enable converter.*/
#define LCD_CAM_CAM_CONV_BYPASS (BIT(31))
#define LCD_CAM_CAM_CONV_BYPASS_M (BIT(31))
#define LCD_CAM_CAM_CONV_BYPASS_V 0x1
#define LCD_CAM_CAM_CONV_BYPASS_S 31
/* LCD_CAM_CAM_CONV_TRANS_MODE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: YUV to RGB. 1: RGB to YUV.*/
#define LCD_CAM_CAM_CONV_TRANS_MODE (BIT(30))
#define LCD_CAM_CAM_CONV_TRANS_MODE_M (BIT(30))
#define LCD_CAM_CAM_CONV_TRANS_MODE_V 0x1
#define LCD_CAM_CAM_CONV_TRANS_MODE_S 30
/* LCD_CAM_CAM_CONV_MODE_8BITS_ON : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 0: 16bits mode. 1: 8bits mode.*/
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON (BIT(29))
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON_M (BIT(29))
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON_V 0x1
#define LCD_CAM_CAM_CONV_MODE_8BITS_ON_S 29
/* LCD_CAM_CAM_CONV_DATA_IN_MODE : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: LIMIT or FULL mode of Data in. 0: limit. 1: full*/
#define LCD_CAM_CAM_CONV_DATA_IN_MODE (BIT(28))
#define LCD_CAM_CAM_CONV_DATA_IN_MODE_M (BIT(28))
#define LCD_CAM_CAM_CONV_DATA_IN_MODE_V 0x1
#define LCD_CAM_CAM_CONV_DATA_IN_MODE_S 28
/* LCD_CAM_CAM_CONV_DATA_OUT_MODE : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: LIMIT or FULL mode of Data out. 0: limit. 1: full*/
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE (BIT(27))
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE_M (BIT(27))
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE_V 0x1
#define LCD_CAM_CAM_CONV_DATA_OUT_MODE_S 27
/* LCD_CAM_CAM_CONV_PROTOCOL_MODE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 0:BT601. 1:BT709.*/
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE (BIT(26))
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE_M (BIT(26))
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE_V 0x1
#define LCD_CAM_CAM_CONV_PROTOCOL_MODE_S 26
/* LCD_CAM_CAM_CONV_YUV_MODE : R/W ;bitpos:[25:24] ;default: 2'b0 ; */
/*description: 0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode  yuv_mode
 decides the yuv mode of Data_in*/
#define LCD_CAM_CAM_CONV_YUV_MODE 0x00000003
#define LCD_CAM_CAM_CONV_YUV_MODE_M ((LCD_CAM_CAM_CONV_YUV_MODE_V) << (LCD_CAM_CAM_CONV_YUV_MODE_S))
#define LCD_CAM_CAM_CONV_YUV_MODE_V 0x3
#define LCD_CAM_CAM_CONV_YUV_MODE_S 24
/* LCD_CAM_CAM_CONV_YUV2YUV_MODE : R/W ;bitpos:[23:22] ;default: 2'd3 ; */
/*description: 0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable
 yuv2yuv mode  trans_mode must be set to 1.*/
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE 0x00000003
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE_M ((LCD_CAM_CAM_CONV_YUV2YUV_MODE_V) << (LCD_CAM_CAM_CONV_YUV2YUV_MODE_S))
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE_V 0x3
#define LCD_CAM_CAM_CONV_YUV2YUV_MODE_S 22
/* LCD_CAM_CAM_CONV_8BITS_DATA_INV : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: 1:invert every two 8bits input data. 2. disabled.*/
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV (BIT(21))
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV_M (BIT(21))
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV_V 0x1
#define LCD_CAM_CAM_CONV_8BITS_DATA_INV_S 21

#define LCD_CAM_LCD_RGB_YUV_REG (DR_REG_LCD_CAM_BASE + 0x010)
/* LCD_CAM_LCD_CONV_BYPASS : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 0: Bypass converter. 1: Enable converter.*/
#define LCD_CAM_LCD_CONV_BYPASS (BIT(31))
#define LCD_CAM_LCD_CONV_BYPASS_M (BIT(31))
#define LCD_CAM_LCD_CONV_BYPASS_V 0x1
#define LCD_CAM_LCD_CONV_BYPASS_S 31
/* LCD_CAM_LCD_CONV_TRANS_MODE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: YUV to RGB. 1: RGB to YUV.*/
#define LCD_CAM_LCD_CONV_TRANS_MODE (BIT(30))
#define LCD_CAM_LCD_CONV_TRANS_MODE_M (BIT(30))
#define LCD_CAM_LCD_CONV_TRANS_MODE_V 0x1
#define LCD_CAM_LCD_CONV_TRANS_MODE_S 30
/* LCD_CAM_LCD_CONV_MODE_8BITS_ON : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 0: 16bits mode. 1: 8bits mode.*/
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON (BIT(29))
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON_M (BIT(29))
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON_V 0x1
#define LCD_CAM_LCD_CONV_MODE_8BITS_ON_S 29
/* LCD_CAM_LCD_CONV_DATA_IN_MODE : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: LIMIT or FULL mode of Data in. 0: limit. 1: full*/
#define LCD_CAM_LCD_CONV_DATA_IN_MODE (BIT(28))
#define LCD_CAM_LCD_CONV_DATA_IN_MODE_M (BIT(28))
#define LCD_CAM_LCD_CONV_DATA_IN_MODE_V 0x1
#define LCD_CAM_LCD_CONV_DATA_IN_MODE_S 28
/* LCD_CAM_LCD_CONV_DATA_OUT_MODE : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: LIMIT or FULL mode of Data out. 0: limit. 1: full*/
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE (BIT(27))
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE_M (BIT(27))
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE_V 0x1
#define LCD_CAM_LCD_CONV_DATA_OUT_MODE_S 27
/* LCD_CAM_LCD_CONV_PROTOCOL_MODE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 0:BT601. 1:BT709.*/
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE (BIT(26))
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE_M (BIT(26))
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE_V 0x1
#define LCD_CAM_LCD_CONV_PROTOCOL_MODE_S 26
/* LCD_CAM_LCD_CONV_YUV_MODE : R/W ;bitpos:[25:24] ;default: 2'b0 ; */
/*description: 0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode  yuv_mode
 decides the yuv mode of Data_in*/
#define LCD_CAM_LCD_CONV_YUV_MODE 0x00000003
#define LCD_CAM_LCD_CONV_YUV_MODE_M ((LCD_CAM_LCD_CONV_YUV_MODE_V) << (LCD_CAM_LCD_CONV_YUV_MODE_S))
#define LCD_CAM_LCD_CONV_YUV_MODE_V 0x3
#define LCD_CAM_LCD_CONV_YUV_MODE_S 24
/* LCD_CAM_LCD_CONV_YUV2YUV_MODE : R/W ;bitpos:[23:22] ;default: 2'd3 ; */
/*description: 0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable
 yuv2yuv mode  trans_mode must be set to 1.*/
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE 0x00000003
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE_M ((LCD_CAM_LCD_CONV_YUV2YUV_MODE_V) << (LCD_CAM_LCD_CONV_YUV2YUV_MODE_S))
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE_V 0x3
#define LCD_CAM_LCD_CONV_YUV2YUV_MODE_S 22
/* LCD_CAM_LCD_CONV_TXTORX : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: 0: txtorx mode off. 1: txtorx mode on.*/
#define LCD_CAM_LCD_CONV_TXTORX (BIT(21))
#define LCD_CAM_LCD_CONV_TXTORX_M (BIT(21))
#define LCD_CAM_LCD_CONV_TXTORX_V 0x1
#define LCD_CAM_LCD_CONV_TXTORX_S 21
/* LCD_CAM_LCD_CONV_8BITS_DATA_INV : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: 1:invert every two 8bits input data. 2. disabled.*/
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV (BIT(20))
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV_M (BIT(20))
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV_V 0x1
#define LCD_CAM_LCD_CONV_8BITS_DATA_INV_S 20

#define LCD_CAM_LCD_USER_REG (DR_REG_LCD_CAM_BASE + 0x014)
/* LCD_CAM_LCD_CMD_2_CYCLE_EN : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: The cycle length of command phase*/
#define LCD_CAM_LCD_CMD_2_CYCLE_EN (BIT(31))
#define LCD_CAM_LCD_CMD_2_CYCLE_EN_M (BIT(31))
#define LCD_CAM_LCD_CMD_2_CYCLE_EN_V 0x1
#define LCD_CAM_LCD_CMD_2_CYCLE_EN_S 31
/* LCD_CAM_LCD_DUMMY_CYCLELEN : R/W ;bitpos:[30:29] ;default: 2'b0 ; */
/*description: The dummy cycle length minus 1.*/
#define LCD_CAM_LCD_DUMMY_CYCLELEN 0x00000003
#define LCD_CAM_LCD_DUMMY_CYCLELEN_M ((LCD_CAM_LCD_DUMMY_CYCLELEN_V) << (LCD_CAM_LCD_DUMMY_CYCLELEN_S))
#define LCD_CAM_LCD_DUMMY_CYCLELEN_V 0x3
#define LCD_CAM_LCD_DUMMY_CYCLELEN_S 29
/* LCD_CAM_LCD_RESET : WO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The value of  command.*/
#define LCD_CAM_LCD_RESET (BIT(28))
#define LCD_CAM_LCD_RESET_M (BIT(28))
#define LCD_CAM_LCD_RESET_V 0x1
#define LCD_CAM_LCD_RESET_S 28
/* LCD_CAM_LCD_START : R/W ;bitpos:[27] ;default: 1'h0 ; */
/*description: LCD start sending data enable signal  valid in high level.*/
#define LCD_CAM_LCD_START (BIT(27))
#define LCD_CAM_LCD_START_M (BIT(27))
#define LCD_CAM_LCD_START_V 0x1
#define LCD_CAM_LCD_START_S 27
/* LCD_CAM_LCD_CMD : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: 1: Be able to send command in LCD sequence when LCD starts. 0: Disable.*/
#define LCD_CAM_LCD_CMD (BIT(26))
#define LCD_CAM_LCD_CMD_M (BIT(26))
#define LCD_CAM_LCD_CMD_V 0x1
#define LCD_CAM_LCD_CMD_S 26
/* LCD_CAM_LCD_DUMMY : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: 1: Enable DUMMY phase in LCD sequence when LCD starts. 0: Disable.*/
#define LCD_CAM_LCD_DUMMY (BIT(25))
#define LCD_CAM_LCD_DUMMY_M (BIT(25))
#define LCD_CAM_LCD_DUMMY_V 0x1
#define LCD_CAM_LCD_DUMMY_S 25
/* LCD_CAM_LCD_DOUT : R/W ;bitpos:[24] ;default: 1'h0 ; */
/*description: 1: Be able to send data out in LCD sequence when LCD starts. 0: Disable.*/
#define LCD_CAM_LCD_DOUT (BIT(24))
#define LCD_CAM_LCD_DOUT_M (BIT(24))
#define LCD_CAM_LCD_DOUT_V 0x1
#define LCD_CAM_LCD_DOUT_S 24
/* LCD_CAM_LCD_2BYTE_EN : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: 1: The bit number of output LCD data is 9~16.  0: The bit number
 of output LCD data is 0~8.*/
#define LCD_CAM_LCD_2BYTE_EN (BIT(23))
#define LCD_CAM_LCD_2BYTE_EN_M (BIT(23))
#define LCD_CAM_LCD_2BYTE_EN_V 0x1
#define LCD_CAM_LCD_2BYTE_EN_S 23
/* LCD_CAM_LCD_BYTE_ORDER : R/W ;bitpos:[22] ;default: 1'h0 ; */
/*description: 1: invert data byte order  only valid in 2 byte mode. 0: Not change.*/
#define LCD_CAM_LCD_BYTE_ORDER (BIT(22))
#define LCD_CAM_LCD_BYTE_ORDER_M (BIT(22))
#define LCD_CAM_LCD_BYTE_ORDER_V 0x1
#define LCD_CAM_LCD_BYTE_ORDER_S 22
/* LCD_CAM_LCD_BIT_ORDER : R/W ;bitpos:[21] ;default: 1'h0 ; */
/*description: 1: Change data bit order  change LCD_DATA_out[7:0] to LCD_DATA_out[0:7]
 in one byte mode  and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.*/
#define LCD_CAM_LCD_BIT_ORDER (BIT(21))
#define LCD_CAM_LCD_BIT_ORDER_M (BIT(21))
#define LCD_CAM_LCD_BIT_ORDER_V 0x1
#define LCD_CAM_LCD_BIT_ORDER_S 21
/* LCD_CAM_LCD_UPDATE_REG : R/W ;bitpos:[20] ;default: 1'h0 ; */
/*description: 1: Update LCD registers  will be cleared by hardware. 0 : Not care.*/
#define LCD_CAM_LCD_UPDATE_REG (BIT(20))
#define LCD_CAM_LCD_UPDATE_REG_M (BIT(20))
#define LCD_CAM_LCD_UPDATE_REG_V 0x1
#define LCD_CAM_LCD_UPDATE_REG_S 20
/* LCD_CAM_LCD_8BITS_ORDER : R/W ;bitpos:[19] ;default: 1'h0 ; */
/*description: 1: invert every two data byte  valid in 1 byte mode. 0: Not change.*/
#define LCD_CAM_LCD_8BITS_ORDER (BIT(19))
#define LCD_CAM_LCD_8BITS_ORDER_M (BIT(19))
#define LCD_CAM_LCD_8BITS_ORDER_V 0x1
#define LCD_CAM_LCD_8BITS_ORDER_S 19
/* LCD_CAM_LCD_ALWAYS_OUT_EN : R/W ;bitpos:[13] ;default: 1'h0 ; */
/*description: LCD always output when LCD is in LCD_DOUT state  unless reg_lcd_start
 is cleared or reg_lcd_reset is set.*/
#define LCD_CAM_LCD_ALWAYS_OUT_EN (BIT(13))
#define LCD_CAM_LCD_ALWAYS_OUT_EN_M (BIT(13))
#define LCD_CAM_LCD_ALWAYS_OUT_EN_V 0x1
#define LCD_CAM_LCD_ALWAYS_OUT_EN_S 13
/* LCD_CAM_LCD_DOUT_CYCLELEN : R/W ;bitpos:[12:0] ;default: 13'h1 ; */
/*description: The output data cycles minus 1 of LCD module.*/
#define LCD_CAM_LCD_DOUT_CYCLELEN 0x00001FFF
#define LCD_CAM_LCD_DOUT_CYCLELEN_M ((LCD_CAM_LCD_DOUT_CYCLELEN_V) << (LCD_CAM_LCD_DOUT_CYCLELEN_S))
#define LCD_CAM_LCD_DOUT_CYCLELEN_V 0x1FFF
#define LCD_CAM_LCD_DOUT_CYCLELEN_S 0

#define LCD_CAM_LCD_MISC_REG (DR_REG_LCD_CAM_BASE + 0x018)
/* LCD_CAM_LCD_CD_IDLE_EDGE : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: The default value of LCD_CD.*/
#define LCD_CAM_LCD_CD_IDLE_EDGE (BIT(31))
#define LCD_CAM_LCD_CD_IDLE_EDGE_M (BIT(31))
#define LCD_CAM_LCD_CD_IDLE_EDGE_V 0x1
#define LCD_CAM_LCD_CD_IDLE_EDGE_S 31
/* LCD_CAM_LCD_CD_CMD_SET : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_CMD
 state.  0: LCD_CD = reg_cd_idle_edge.*/
#define LCD_CAM_LCD_CD_CMD_SET (BIT(30))
#define LCD_CAM_LCD_CD_CMD_SET_M (BIT(30))
#define LCD_CAM_LCD_CD_CMD_SET_V 0x1
#define LCD_CAM_LCD_CD_CMD_SET_S 30
/* LCD_CAM_LCD_CD_DUMMY_SET : R/W ;bitpos:[29] ;default: 1'h0 ; */
/*description: 1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DUMMY
 state.  0: LCD_CD = reg_cd_idle_edge.*/
#define LCD_CAM_LCD_CD_DUMMY_SET (BIT(29))
#define LCD_CAM_LCD_CD_DUMMY_SET_M (BIT(29))
#define LCD_CAM_LCD_CD_DUMMY_SET_V 0x1
#define LCD_CAM_LCD_CD_DUMMY_SET_S 29
/* LCD_CAM_LCD_CD_DATA_SET : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: 1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DOUT
 state.  0: LCD_CD = reg_cd_idle_edge.*/
#define LCD_CAM_LCD_CD_DATA_SET (BIT(28))
#define LCD_CAM_LCD_CD_DATA_SET_M (BIT(28))
#define LCD_CAM_LCD_CD_DATA_SET_V 0x1
#define LCD_CAM_LCD_CD_DATA_SET_S 28
/* LCD_CAM_LCD_AFIFO_RESET : WO ;bitpos:[27] ;default: 1'b0 ; */
/*description: LCD AFIFO reset signal.*/
#define LCD_CAM_LCD_AFIFO_RESET (BIT(27))
#define LCD_CAM_LCD_AFIFO_RESET_M (BIT(27))
#define LCD_CAM_LCD_AFIFO_RESET_V 0x1
#define LCD_CAM_LCD_AFIFO_RESET_S 27
/* LCD_CAM_LCD_BK_EN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: Enable blank region when LCD sends data out. 0: No blank region.*/
#define LCD_CAM_LCD_BK_EN (BIT(26))
#define LCD_CAM_LCD_BK_EN_M (BIT(26))
#define LCD_CAM_LCD_BK_EN_V 0x1
#define LCD_CAM_LCD_BK_EN_S 26
/* LCD_CAM_LCD_NEXT_FRAME_EN : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: Send the next frame data when the current frame is sent out.
 0: LCD stops when the current frame is sent out.*/
#define LCD_CAM_LCD_NEXT_FRAME_EN (BIT(25))
#define LCD_CAM_LCD_NEXT_FRAME_EN_M (BIT(25))
#define LCD_CAM_LCD_NEXT_FRAME_EN_V 0x1
#define LCD_CAM_LCD_NEXT_FRAME_EN_S 25
/* LCD_CAM_LCD_VBK_CYCLELEN : R/W ;bitpos:[24:12] ;default: 13'h0 ; */
/*description: The vertical back blank region cycle length minus 1 in LCD RGB
 mode  or the hold time cycle length in LCD non-RGB mode.*/
#define LCD_CAM_LCD_VBK_CYCLELEN 0x00001FFF
#define LCD_CAM_LCD_VBK_CYCLELEN_M ((LCD_CAM_LCD_VBK_CYCLELEN_V) << (LCD_CAM_LCD_VBK_CYCLELEN_S))
#define LCD_CAM_LCD_VBK_CYCLELEN_V 0x1FFF
#define LCD_CAM_LCD_VBK_CYCLELEN_S 12
/* LCD_CAM_LCD_VFK_CYCLELEN : R/W ;bitpos:[11:6] ;default: 6'h3 ; */
/*description: The setup cycle length minus 1 in LCD non-RGB mode.*/
#define LCD_CAM_LCD_VFK_CYCLELEN 0x0000003F
#define LCD_CAM_LCD_VFK_CYCLELEN_M ((LCD_CAM_LCD_VFK_CYCLELEN_V) << (LCD_CAM_LCD_VFK_CYCLELEN_S))
#define LCD_CAM_LCD_VFK_CYCLELEN_V 0x3F
#define LCD_CAM_LCD_VFK_CYCLELEN_S 6
/* LCD_CAM_LCD_AFIFO_THRESHOLD_NUM : R/W ;bitpos:[5:1] ;default: 5'd11 ; */
/*description: The awfull threshold number of lcd_afifo.*/
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM 0x0000001F
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_M ((LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_V) << (LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_S))
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_V 0x1F
#define LCD_CAM_LCD_AFIFO_THRESHOLD_NUM_S 1

#define LCD_CAM_LCD_CTRL_REG (DR_REG_LCD_CAM_BASE + 0x01C)
/* LCD_CAM_LCD_RGB_MODE_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: Enable reg mode input vsync*/
#define LCD_CAM_LCD_RGB_MODE_EN (BIT(31))
#define LCD_CAM_LCD_RGB_MODE_EN_M (BIT(31))
#define LCD_CAM_LCD_RGB_MODE_EN_V 0x1
#define LCD_CAM_LCD_RGB_MODE_EN_S 31
/* LCD_CAM_LCD_VT_HEIGHT : R/W ;bitpos:[30:21] ;default: 10'd0 ; */
/*description: It is the vertical total height of a frame.*/
#define LCD_CAM_LCD_VT_HEIGHT 0x000003FF
#define LCD_CAM_LCD_VT_HEIGHT_M ((LCD_CAM_LCD_VT_HEIGHT_V) << (LCD_CAM_LCD_VT_HEIGHT_S))
#define LCD_CAM_LCD_VT_HEIGHT_V 0x3FF
#define LCD_CAM_LCD_VT_HEIGHT_S 21
/* LCD_CAM_LCD_VA_HEIGHT : R/W ;bitpos:[20:11] ;default: 10'd0 ; */
/*description: It is the vertical active height of a frame.*/
#define LCD_CAM_LCD_VA_HEIGHT 0x000003FF
#define LCD_CAM_LCD_VA_HEIGHT_M ((LCD_CAM_LCD_VA_HEIGHT_V) << (LCD_CAM_LCD_VA_HEIGHT_S))
#define LCD_CAM_LCD_VA_HEIGHT_V 0x3FF
#define LCD_CAM_LCD_VA_HEIGHT_S 11
/* LCD_CAM_LCD_HB_FRONT : R/W ;bitpos:[10:0] ;default: 11'd0 ; */
/*description: It is the horizontal blank front porch of a frame.*/
#define LCD_CAM_LCD_HB_FRONT 0x000007FF
#define LCD_CAM_LCD_HB_FRONT_M ((LCD_CAM_LCD_HB_FRONT_V) << (LCD_CAM_LCD_HB_FRONT_S))
#define LCD_CAM_LCD_HB_FRONT_V 0x7FF
#define LCD_CAM_LCD_HB_FRONT_S 0

#define LCD_CAM_LCD_CTRL1_REG (DR_REG_LCD_CAM_BASE + 0x020)
/* LCD_CAM_LCD_HT_WIDTH : R/W ;bitpos:[31:20] ;default: 12'd0 ; */
/*description: It is the horizontal total width of a frame.*/
#define LCD_CAM_LCD_HT_WIDTH 0x00000FFF
#define LCD_CAM_LCD_HT_WIDTH_M ((LCD_CAM_LCD_HT_WIDTH_V) << (LCD_CAM_LCD_HT_WIDTH_S))
#define LCD_CAM_LCD_HT_WIDTH_V 0xFFF
#define LCD_CAM_LCD_HT_WIDTH_S 20
/* LCD_CAM_LCD_HA_WIDTH : R/W ;bitpos:[19:8] ;default: 12'd0 ; */
/*description: It is the horizontal active width of a frame.*/
#define LCD_CAM_LCD_HA_WIDTH 0x00000FFF
#define LCD_CAM_LCD_HA_WIDTH_M ((LCD_CAM_LCD_HA_WIDTH_V) << (LCD_CAM_LCD_HA_WIDTH_S))
#define LCD_CAM_LCD_HA_WIDTH_V 0xFFF
#define LCD_CAM_LCD_HA_WIDTH_S 8
/* LCD_CAM_LCD_VB_FRONT : R/W ;bitpos:[7:0] ;default: 8'd0 ; */
/*description: It is the vertical blank front porch of a frame.*/
#define LCD_CAM_LCD_VB_FRONT 0x000000FF
#define LCD_CAM_LCD_VB_FRONT_M ((LCD_CAM_LCD_VB_FRONT_V) << (LCD_CAM_LCD_VB_FRONT_S))
#define LCD_CAM_LCD_VB_FRONT_V 0xFF
#define LCD_CAM_LCD_VB_FRONT_S 0

#define LCD_CAM_LCD_CTRL2_REG (DR_REG_LCD_CAM_BASE + 0x024)
/* LCD_CAM_LCD_HSYNC_POSITION : R/W ;bitpos:[31:24] ;default: 8'd0 ; */
/*description: It is the position of LCD_HSYNC active pulse in a line.*/
#define LCD_CAM_LCD_HSYNC_POSITION 0x000000FF
#define LCD_CAM_LCD_HSYNC_POSITION_M ((LCD_CAM_LCD_HSYNC_POSITION_V) << (LCD_CAM_LCD_HSYNC_POSITION_S))
#define LCD_CAM_LCD_HSYNC_POSITION_V 0xFF
#define LCD_CAM_LCD_HSYNC_POSITION_S 24
/* LCD_CAM_LCD_HSYNC_IDLE_POL : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: It is the idle value of LCD_HSYNC.*/
#define LCD_CAM_LCD_HSYNC_IDLE_POL (BIT(23))
#define LCD_CAM_LCD_HSYNC_IDLE_POL_M (BIT(23))
#define LCD_CAM_LCD_HSYNC_IDLE_POL_V 0x1
#define LCD_CAM_LCD_HSYNC_IDLE_POL_S 23
/* LCD_CAM_LCD_HSYNC_WIDTH : R/W ;bitpos:[22:16] ;default: 7'd1 ; */
/*description: It is the position of LCD_HSYNC active pulse in a line.*/
#define LCD_CAM_LCD_HSYNC_WIDTH 0x0000007F
#define LCD_CAM_LCD_HSYNC_WIDTH_M ((LCD_CAM_LCD_HSYNC_WIDTH_V) << (LCD_CAM_LCD_HSYNC_WIDTH_S))
#define LCD_CAM_LCD_HSYNC_WIDTH_V 0x7F
#define LCD_CAM_LCD_HSYNC_WIDTH_S 16
/* LCD_CAM_LCD_HS_BLANK_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: The pulse of LCD_HSYNC is out in vertical blanking lines RGB
 mode. 0: LCD_HSYNC pulse is valid only in active region lines in RGB mode.*/
#define LCD_CAM_LCD_HS_BLANK_EN (BIT(9))
#define LCD_CAM_LCD_HS_BLANK_EN_M (BIT(9))
#define LCD_CAM_LCD_HS_BLANK_EN_V 0x1
#define LCD_CAM_LCD_HS_BLANK_EN_S 9
/* LCD_CAM_LCD_DE_IDLE_POL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: It is the idle value of LCD_DE.*/
#define LCD_CAM_LCD_DE_IDLE_POL (BIT(8))
#define LCD_CAM_LCD_DE_IDLE_POL_M (BIT(8))
#define LCD_CAM_LCD_DE_IDLE_POL_V 0x1
#define LCD_CAM_LCD_DE_IDLE_POL_S 8
/* LCD_CAM_LCD_VSYNC_IDLE_POL : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: It is the idle value of LCD_VSYNC.*/
#define LCD_CAM_LCD_VSYNC_IDLE_POL (BIT(7))
#define LCD_CAM_LCD_VSYNC_IDLE_POL_M (BIT(7))
#define LCD_CAM_LCD_VSYNC_IDLE_POL_V 0x1
#define LCD_CAM_LCD_VSYNC_IDLE_POL_S 7
/* LCD_CAM_LCD_VSYNC_WIDTH : R/W ;bitpos:[6:0] ;default: 7'd1 ; */
/*description: It is the position of LCD_VSYNC active pulse in a line.*/
#define LCD_CAM_LCD_VSYNC_WIDTH 0x0000007F
#define LCD_CAM_LCD_VSYNC_WIDTH_M ((LCD_CAM_LCD_VSYNC_WIDTH_V) << (LCD_CAM_LCD_VSYNC_WIDTH_S))
#define LCD_CAM_LCD_VSYNC_WIDTH_V 0x7F
#define LCD_CAM_LCD_VSYNC_WIDTH_S 0

#define LCD_CAM_LCD_CMD_VAL_REG (DR_REG_LCD_CAM_BASE + 0x028)
/* LCD_CAM_LCD_CMD_VALUE : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The LCD write command value.*/
#define LCD_CAM_LCD_CMD_VALUE 0xFFFFFFFF
#define LCD_CAM_LCD_CMD_VALUE_M ((LCD_CAM_LCD_CMD_VALUE_V) << (LCD_CAM_LCD_CMD_VALUE_S))
#define LCD_CAM_LCD_CMD_VALUE_V 0xFFFFFFFF
#define LCD_CAM_LCD_CMD_VALUE_S 0

#define LCD_CAM_LCD_DLY_MODE_REG (DR_REG_LCD_CAM_BASE + 0x030)
/* LCD_CAM_LCD_VSYNC_MODE : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: The output LCD_VSYNC is delayed by module clock LCD_CLK*/
#define LCD_CAM_LCD_VSYNC_MODE 0x00000003
#define LCD_CAM_LCD_VSYNC_MODE_M ((LCD_CAM_LCD_VSYNC_MODE_V) << (LCD_CAM_LCD_VSYNC_MODE_S))
#define LCD_CAM_LCD_VSYNC_MODE_V 0x3
#define LCD_CAM_LCD_VSYNC_MODE_S 6
/* LCD_CAM_LCD_HSYNC_MODE : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: The output LCD_HSYNC is delayed by module clock LCD_CLK*/
#define LCD_CAM_LCD_HSYNC_MODE 0x00000003
#define LCD_CAM_LCD_HSYNC_MODE_M ((LCD_CAM_LCD_HSYNC_MODE_V) << (LCD_CAM_LCD_HSYNC_MODE_S))
#define LCD_CAM_LCD_HSYNC_MODE_V 0x3
#define LCD_CAM_LCD_HSYNC_MODE_S 4
/* LCD_CAM_LCD_DE_MODE : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: The output LCD_DE is delayed by module clock LCD_CLK*/
#define LCD_CAM_LCD_DE_MODE 0x00000003
#define LCD_CAM_LCD_DE_MODE_M ((LCD_CAM_LCD_DE_MODE_V) << (LCD_CAM_LCD_DE_MODE_S))
#define LCD_CAM_LCD_DE_MODE_V 0x3
#define LCD_CAM_LCD_DE_MODE_S 2
/* LCD_CAM_LCD_CD_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: The output LCD_CD is delayed by module clock LCD_CLK*/
#define LCD_CAM_LCD_CD_MODE 0x00000003
#define LCD_CAM_LCD_CD_MODE_M ((LCD_CAM_LCD_CD_MODE_V) << (LCD_CAM_LCD_CD_MODE_S))
#define LCD_CAM_LCD_CD_MODE_V 0x3
#define LCD_CAM_LCD_CD_MODE_S 0

#define LCD_CAM_LCD_DATA_DOUT_MODE_REG (DR_REG_LCD_CAM_BASE + 0x038)
/* LCD_CAM_DOUT15_MODE : R/W ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT15_MODE 0x00000003
#define LCD_CAM_DOUT15_MODE_M ((LCD_CAM_DOUT15_MODE_V) << (LCD_CAM_DOUT15_MODE_S))
#define LCD_CAM_DOUT15_MODE_V 0x3
#define LCD_CAM_DOUT15_MODE_S 30
/* LCD_CAM_DOUT14_MODE : R/W ;bitpos:[29:28] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT14_MODE 0x00000003
#define LCD_CAM_DOUT14_MODE_M ((LCD_CAM_DOUT14_MODE_V) << (LCD_CAM_DOUT14_MODE_S))
#define LCD_CAM_DOUT14_MODE_V 0x3
#define LCD_CAM_DOUT14_MODE_S 28
/* LCD_CAM_DOUT13_MODE : R/W ;bitpos:[27:26] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT13_MODE 0x00000003
#define LCD_CAM_DOUT13_MODE_M ((LCD_CAM_DOUT13_MODE_V) << (LCD_CAM_DOUT13_MODE_S))
#define LCD_CAM_DOUT13_MODE_V 0x3
#define LCD_CAM_DOUT13_MODE_S 26
/* LCD_CAM_DOUT12_MODE : R/W ;bitpos:[25:24] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT12_MODE 0x00000003
#define LCD_CAM_DOUT12_MODE_M ((LCD_CAM_DOUT12_MODE_V) << (LCD_CAM_DOUT12_MODE_S))
#define LCD_CAM_DOUT12_MODE_V 0x3
#define LCD_CAM_DOUT12_MODE_S 24
/* LCD_CAM_DOUT11_MODE : R/W ;bitpos:[23:22] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT11_MODE 0x00000003
#define LCD_CAM_DOUT11_MODE_M ((LCD_CAM_DOUT11_MODE_V) << (LCD_CAM_DOUT11_MODE_S))
#define LCD_CAM_DOUT11_MODE_V 0x3
#define LCD_CAM_DOUT11_MODE_S 22
/* LCD_CAM_DOUT10_MODE : R/W ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT10_MODE 0x00000003
#define LCD_CAM_DOUT10_MODE_M ((LCD_CAM_DOUT10_MODE_V) << (LCD_CAM_DOUT10_MODE_S))
#define LCD_CAM_DOUT10_MODE_V 0x3
#define LCD_CAM_DOUT10_MODE_S 20
/* LCD_CAM_DOUT9_MODE : R/W ;bitpos:[19:18] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT9_MODE 0x00000003
#define LCD_CAM_DOUT9_MODE_M ((LCD_CAM_DOUT9_MODE_V) << (LCD_CAM_DOUT9_MODE_S))
#define LCD_CAM_DOUT9_MODE_V 0x3
#define LCD_CAM_DOUT9_MODE_S 18
/* LCD_CAM_DOUT8_MODE : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT8_MODE 0x00000003
#define LCD_CAM_DOUT8_MODE_M ((LCD_CAM_DOUT8_MODE_V) << (LCD_CAM_DOUT8_MODE_S))
#define LCD_CAM_DOUT8_MODE_V 0x3
#define LCD_CAM_DOUT8_MODE_S 16
/* LCD_CAM_DOUT7_MODE : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT7_MODE 0x00000003
#define LCD_CAM_DOUT7_MODE_M ((LCD_CAM_DOUT7_MODE_V) << (LCD_CAM_DOUT7_MODE_S))
#define LCD_CAM_DOUT7_MODE_V 0x3
#define LCD_CAM_DOUT7_MODE_S 14
/* LCD_CAM_DOUT6_MODE : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT6_MODE 0x00000003
#define LCD_CAM_DOUT6_MODE_M ((LCD_CAM_DOUT6_MODE_V) << (LCD_CAM_DOUT6_MODE_S))
#define LCD_CAM_DOUT6_MODE_V 0x3
#define LCD_CAM_DOUT6_MODE_S 12
/* LCD_CAM_DOUT5_MODE : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT5_MODE 0x00000003
#define LCD_CAM_DOUT5_MODE_M ((LCD_CAM_DOUT5_MODE_V) << (LCD_CAM_DOUT5_MODE_S))
#define LCD_CAM_DOUT5_MODE_V 0x3
#define LCD_CAM_DOUT5_MODE_S 10
/* LCD_CAM_DOUT4_MODE : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT4_MODE 0x00000003
#define LCD_CAM_DOUT4_MODE_M ((LCD_CAM_DOUT4_MODE_V) << (LCD_CAM_DOUT4_MODE_S))
#define LCD_CAM_DOUT4_MODE_V 0x3
#define LCD_CAM_DOUT4_MODE_S 8
/* LCD_CAM_DOUT3_MODE : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT3_MODE 0x00000003
#define LCD_CAM_DOUT3_MODE_M ((LCD_CAM_DOUT3_MODE_V) << (LCD_CAM_DOUT3_MODE_S))
#define LCD_CAM_DOUT3_MODE_V 0x3
#define LCD_CAM_DOUT3_MODE_S 6
/* LCD_CAM_DOUT2_MODE : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT2_MODE 0x00000003
#define LCD_CAM_DOUT2_MODE_M ((LCD_CAM_DOUT2_MODE_V) << (LCD_CAM_DOUT2_MODE_S))
#define LCD_CAM_DOUT2_MODE_V 0x3
#define LCD_CAM_DOUT2_MODE_S 4
/* LCD_CAM_DOUT1_MODE : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT1_MODE 0x00000003
#define LCD_CAM_DOUT1_MODE_M ((LCD_CAM_DOUT1_MODE_V) << (LCD_CAM_DOUT1_MODE_S))
#define LCD_CAM_DOUT1_MODE_V 0x3
#define LCD_CAM_DOUT1_MODE_S 2
/* LCD_CAM_DOUT0_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: The output data bit $n is delayed by module clock LCD_CLK*/
#define LCD_CAM_DOUT0_MODE 0x00000003
#define LCD_CAM_DOUT0_MODE_M ((LCD_CAM_DOUT0_MODE_V) << (LCD_CAM_DOUT0_MODE_S))
#define LCD_CAM_DOUT0_MODE_V 0x3
#define LCD_CAM_DOUT0_MODE_S 0

#define LCD_CAM_LC_DMA_INT_ENA_REG (DR_REG_LCD_CAM_BASE + 0x064)
/* LCD_CAM_CAM_HS_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The enable bit for Camera line interrupt.*/
#define LCD_CAM_CAM_HS_INT_ENA (BIT(3))
#define LCD_CAM_CAM_HS_INT_ENA_M (BIT(3))
#define LCD_CAM_CAM_HS_INT_ENA_V 0x1
#define LCD_CAM_CAM_HS_INT_ENA_S 3
/* LCD_CAM_CAM_VSYNC_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The enable bit for Camera frame end interrupt.*/
#define LCD_CAM_CAM_VSYNC_INT_ENA (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_ENA_M (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_ENA_V 0x1
#define LCD_CAM_CAM_VSYNC_INT_ENA_S 2
/* LCD_CAM_LCD_TRANS_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable bit for lcd transfer end interrupt.*/
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA_M (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA_V 0x1
#define LCD_CAM_LCD_TRANS_DONE_INT_ENA_S 1
/* LCD_CAM_LCD_VSYNC_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable bit for LCD frame end interrupt.*/
#define LCD_CAM_LCD_VSYNC_INT_ENA (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_ENA_M (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_ENA_V 0x1
#define LCD_CAM_LCD_VSYNC_INT_ENA_S 0

#define LCD_CAM_LC_DMA_INT_RAW_REG (DR_REG_LCD_CAM_BASE + 0x068)
/* LCD_CAM_CAM_HS_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw bit for Camera line interrupt.*/
#define LCD_CAM_CAM_HS_INT_RAW (BIT(3))
#define LCD_CAM_CAM_HS_INT_RAW_M (BIT(3))
#define LCD_CAM_CAM_HS_INT_RAW_V 0x1
#define LCD_CAM_CAM_HS_INT_RAW_S 3
/* LCD_CAM_CAM_VSYNC_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw bit for Camera frame end interrupt.*/
#define LCD_CAM_CAM_VSYNC_INT_RAW (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_RAW_M (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_RAW_V 0x1
#define LCD_CAM_CAM_VSYNC_INT_RAW_S 2
/* LCD_CAM_LCD_TRANS_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit for lcd transfer end interrupt.*/
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW_M (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW_V 0x1
#define LCD_CAM_LCD_TRANS_DONE_INT_RAW_S 1
/* LCD_CAM_LCD_VSYNC_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit for LCD frame end interrupt.*/
#define LCD_CAM_LCD_VSYNC_INT_RAW (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_RAW_M (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_RAW_V 0x1
#define LCD_CAM_LCD_VSYNC_INT_RAW_S 0

#define LCD_CAM_LC_DMA_INT_ST_REG (DR_REG_LCD_CAM_BASE + 0x06C)
/* LCD_CAM_CAM_HS_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The status bit for Camera transfer end interrupt.*/
#define LCD_CAM_CAM_HS_INT_ST (BIT(3))
#define LCD_CAM_CAM_HS_INT_ST_M (BIT(3))
#define LCD_CAM_CAM_HS_INT_ST_V 0x1
#define LCD_CAM_CAM_HS_INT_ST_S 3
/* LCD_CAM_CAM_VSYNC_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The status bit for Camera frame end interrupt.*/
#define LCD_CAM_CAM_VSYNC_INT_ST (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_ST_M (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_ST_V 0x1
#define LCD_CAM_CAM_VSYNC_INT_ST_S 2
/* LCD_CAM_LCD_TRANS_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status bit for lcd transfer end interrupt.*/
#define LCD_CAM_LCD_TRANS_DONE_INT_ST (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_ST_M (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_ST_V 0x1
#define LCD_CAM_LCD_TRANS_DONE_INT_ST_S 1
/* LCD_CAM_LCD_VSYNC_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status bit for LCD frame end interrupt.*/
#define LCD_CAM_LCD_VSYNC_INT_ST (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_ST_M (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_ST_V 0x1
#define LCD_CAM_LCD_VSYNC_INT_ST_S 0

#define LCD_CAM_LC_DMA_INT_CLR_REG (DR_REG_LCD_CAM_BASE + 0x070)
/* LCD_CAM_CAM_HS_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The clear bit for Camera line interrupt.*/
#define LCD_CAM_CAM_HS_INT_CLR (BIT(3))
#define LCD_CAM_CAM_HS_INT_CLR_M (BIT(3))
#define LCD_CAM_CAM_HS_INT_CLR_V 0x1
#define LCD_CAM_CAM_HS_INT_CLR_S 3
/* LCD_CAM_CAM_VSYNC_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The clear bit for Camera frame end interrupt.*/
#define LCD_CAM_CAM_VSYNC_INT_CLR (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_CLR_M (BIT(2))
#define LCD_CAM_CAM_VSYNC_INT_CLR_V 0x1
#define LCD_CAM_CAM_VSYNC_INT_CLR_S 2
/* LCD_CAM_LCD_TRANS_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear bit for lcd transfer end interrupt.*/
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR_M (BIT(1))
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR_V 0x1
#define LCD_CAM_LCD_TRANS_DONE_INT_CLR_S 1
/* LCD_CAM_LCD_VSYNC_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear bit for LCD frame end interrupt.*/
#define LCD_CAM_LCD_VSYNC_INT_CLR (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_CLR_M (BIT(0))
#define LCD_CAM_LCD_VSYNC_INT_CLR_V 0x1
#define LCD_CAM_LCD_VSYNC_INT_CLR_S 0

#define LCD_CAM_LC_DATE_REG (DR_REG_LCD_CAM_BASE + 0x0FC)
/* LCD_CAM_LC_DATE : R/W ;bitpos:[27:0] ;default: 28'h2003020 ; */
/*description: LCD_CAM version control register*/
#define LCD_CAM_LC_DATE 0x0FFFFFFF
#define LCD_CAM_LC_DATE_M ((LCD_CAM_LC_DATE_V) << (LCD_CAM_LC_DATE_S))
#define LCD_CAM_LC_DATE_V 0xFFFFFFF
#define LCD_CAM_LC_DATE_S 0

#ifdef __cplusplus
}
#endif
