/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: LCD configuration registers */
/** Type of lcd_clock register
 *  LCD clock configuration register
 */
typedef union {
    struct {
        /** lcd_clkcnt_n : R/W; bitpos: [5:0]; default: 0;
         *  f<SUB>LCD_PCLK</SUB>
         *  = f<SUB>LCD_CLK</SUB>/(LCD_CAM_LCD_CLKCNT_N + 1) when LCD_CAM_LCD_CLK_EQU_SYSCLK is
         *  0. Note: this field must not be configured to 0.
         */
        uint32_t lcd_clkcnt_n: 6;
        /** lcd_clk_equ_sysclk : R/W; bitpos: [6]; default: 0;
         *  1: f<SUB>LCD_PCLK</SUB>
         *  = f<SUB>LCD_CLK</SUB>. 0: f<SUB>LCD_PCLK</SUB>
         *  = f<SUB>LCD_CLK</SUB>/(LCD_CAM_LCD_CLKCNT_N + 1).
         */
        uint32_t lcd_clk_equ_sysclk: 1;
        /** lcd_ck_idle_edge : R/W; bitpos: [7]; default: 0;
         *  1: LCD_PCLK line is high in idle. 0: LCD_PCLK line is low in idle.
         */
        uint32_t lcd_ck_idle_edge: 1;
        /** lcd_ck_out_edge : R/W; bitpos: [8]; default: 0;
         *  1: LCD_PCLK is high in the first half clock cycle. 0: LCD_PCLK is low in the first
         *  half clock cycle.
         */
        uint32_t lcd_ck_out_edge: 1;
        /** lcd_clkm_div_num : R/W; bitpos: [16:9]; default: 0;
         *  Integral LCD clock divider value.
         */
        uint32_t lcd_clkm_div_num: 8;
        /** lcd_clkm_div_b : R/W; bitpos: [22:17]; default: 0;
         *  Fractional clock divider numerator value.
         */
        uint32_t lcd_clkm_div_b: 6;
        /** lcd_clkm_div_a : R/W; bitpos: [28:23]; default: 0;
         *  Fractional clock divider denominator value.
         */
        uint32_t lcd_clkm_div_a: 6;
        /** lcd_clk_sel : R/W; bitpos: [30:29]; default: 0;
         *  Select LCD module source clock. 0: clock source is disabled. 1: XTAL_CLK. 2:
         *  PLL_D2_CLK. 3: PLL_F160M_CLK.
         */
        uint32_t lcd_clk_sel: 2;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Set this bit to force enable the clock for all configuration registers. Clock gate
         *  is not used.
         */
        uint32_t clk_en: 1;
    };
    uint32_t val;
} lcd_cam_lcd_clock_reg_t;

/** Type of lcd_rgb_yuv register
 *  LCD data format conversion register
 */
typedef union {
    struct {
        uint32_t reserved_0: 20;
        /** lcd_conv_8bits_data_inv : R/W; bitpos: [20]; default: 0;
         *  Swap every two 8-bit input data. 1: Enabled. 0: Disabled.
         */
        uint32_t lcd_conv_8bits_data_inv: 1;
        /** lcd_conv_txtorx : R/W; bitpos: [21]; default: 0;
         *  0: txtorx mode off. 1: txtorx mode on.
         */
        uint32_t lcd_conv_txtorx: 1;
        /** lcd_conv_yuv2yuv_mode : R/W; bitpos: [23:22]; default: 0;
         *  In YUV-to-YUV mode, 0: data is converted to YUV422 format. 1: data is converted to
         *  YUV420 format. 2: data is converted to YUV411 format. 3: disabled. To enable
         *  YUV-to-YUV mode, LCD_CAM_LCD_CONV_TRANS_MODE must be set to 1.
         */
        uint32_t lcd_conv_yuv2yuv_mode: 2;
        /** lcd_conv_yuv_mode : R/W; bitpos: [25:24]; default: 0;
         *  In YUV-to-YUV mode and YUV-to-RGB mode, LCD_CAM_LCD_CONV_YUV_MODE decides the YUV
         *  mode of input data. 0: input data is in YUV422 format. 1: input data is in YUV420
         *  format. 2: input data is in YUV411 format. In RGB-to-YUV mode, 0: data is converted
         *  to YUV422 format. 1: data is converted to YUV420 format. 2: data is converted to
         *  YUV411 format.
         */
        uint32_t lcd_conv_yuv_mode: 2;
        /** lcd_conv_protocol_mode : R/W; bitpos: [26]; default: 0;
         *  0: BT601. 1: BT709.
         */
        uint32_t lcd_conv_protocol_mode: 1;
        /** lcd_conv_data_out_mode : R/W; bitpos: [27]; default: 0;
         *  Configure color range for output data. 0: limited color range. 1: full color range.
         */
        uint32_t lcd_conv_data_out_mode: 1;
        /** lcd_conv_data_in_mode : R/W; bitpos: [28]; default: 0;
         *  Configure color range for input data. 0: limited color range. 1: full color range.
         */
        uint32_t lcd_conv_data_in_mode: 1;
        /** lcd_conv_mode_8bits_on : R/W; bitpos: [29]; default: 0;
         *  0: 16-bit mode. 1: 8-bit mode.
         */
        uint32_t lcd_conv_mode_8bits_on: 1;
        /** lcd_conv_trans_mode : R/W; bitpos: [30]; default: 0;
         *  0: converted to RGB format. 1: converted to YUV format.
         */
        uint32_t lcd_conv_trans_mode: 1;
        /** lcd_conv_bypass : R/W; bitpos: [31]; default: 0;
         *  0: Bypass converter. 1: Enable converter.
         */
        uint32_t lcd_conv_bypass: 1;
    };
    uint32_t val;
} lcd_cam_lcd_rgb_yuv_reg_t;

/** Type of lcd_user register
 *  LCD user configuration register
 */
typedef union {
    struct {
        /** lcd_dout_cyclelen : R/W; bitpos: [12:0]; default: 0;
         *  Configure the cycles for DOUT phase of LCD module. The cycles =  this value + 1.
         */
        uint32_t lcd_dout_cyclelen: 13;
        /** lcd_always_out_en : R/W; bitpos: [13]; default: 0;
         *  LCD continues outputting data when LCD is in DOUT phase, till LCD_CAM_LCD_START is
         *  cleared or LCD_CAM_LCD_RESET is set.
         */
        uint32_t lcd_always_out_en: 1;
        uint32_t reserved_14: 5;
        /** lcd_8bits_order : R/W; bitpos: [19]; default: 0;
         *  1: Swap every two data bytes, valid in 8-bit mode. 0: Do not swap.
         */
        uint32_t lcd_8bits_order: 1;
        /** lcd_update : R/W; bitpos: [20]; default: 0;
         *  1: Update LCD registers. This bit is cleared by hardware. 0: Do not care.
         */
        uint32_t lcd_update: 1;
        /** lcd_bit_order : R/W; bitpos: [21]; default: 0;
         *  1: Change data bit order. Change LCD_DATA_out[7:0] to LCD_DATA_out[0:7] in 8-bit
         *  mode, and bits[15:0] to bits[0:15] in 16-bit mode. 0: Do not change.
         */
        uint32_t lcd_bit_order: 1;
        /** lcd_byte_order : R/W; bitpos: [22]; default: 0;
         *  1: Invert data byte order, only valid in 16-bit mode. 0: Do not invert.
         */
        uint32_t lcd_byte_order: 1;
        /** lcd_2byte_en : R/W; bitpos: [23]; default: 0;
         *  1: The width of output LCD data is 16 bits. 0: The width of output LCD data is 8
         *  bits.
         */
        uint32_t lcd_2byte_en: 1;
        /** lcd_dout : R/W; bitpos: [24]; default: 0;
         *  1: Be able to send data out in LCD sequence when LCD starts. 0: Disable.
         */
        uint32_t lcd_dout: 1;
        /** lcd_dummy : R/W; bitpos: [25]; default: 0;
         *  1: Enable DUMMY phase in LCD sequence when LCD starts. 0: Disable.
         */
        uint32_t lcd_dummy: 1;
        /** lcd_cmd : R/W; bitpos: [26]; default: 0;
         *  1: Be able to send command in LCD sequence when LCD starts. 0: Disable.
         */
        uint32_t lcd_cmd: 1;
        /** lcd_start : R/W; bitpos: [27]; default: 0;
         *  LCD starts sending data enable signal, valid in high level.
         */
        uint32_t lcd_start: 1;
        /** lcd_reset : WO; bitpos: [28]; default: 0;
         *  Reset LCD module.
         */
        uint32_t lcd_reset: 1;
        /** lcd_dummy_cyclelen : R/W; bitpos: [30:29]; default: 0;
         *  Configure DUMMY cycles. DUMMY cycles = this value + 1.
         */
        uint32_t lcd_dummy_cyclelen: 2;
        /** lcd_cmd_2_cycle_en : R/W; bitpos: [31]; default: 0;
         *  The cycle length of command phase. 1: 2 cycles. 0: 1 cycle.
         */
        uint32_t lcd_cmd_2_cycle_en: 1;
    };
    uint32_t val;
} lcd_cam_lcd_user_reg_t;

/** Type of lcd_misc register
 *  LCD MISC configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0: 1;
        /** lcd_afifo_threshold_num : R/W; bitpos: [5:1]; default: 17;
         *  Set the threshold for Async Tx FIFO full event.
         */
        uint32_t lcd_afifo_threshold_num: 5;
        /** lcd_vfk_cyclelen : R/W; bitpos: [11:6]; default: 0;
         *  Configure the setup cycles in LCD non-RGB mode. Setup cycles expected = this value
         *  + 1.
         */
        uint32_t lcd_vfk_cyclelen: 6;
        /** lcd_vbk_cyclelen : R/W; bitpos: [24:12]; default: 0;
         *  Configure the hold time cycles in LCD non-RGB mode. Hold cycles expected = this
         *  value + 1. %Configure the cycles for vertical back blank region in LCD RGB mode,
         *  the cycles = this value + 1. Or configure the hold time cycles in LCD non-RGB mode,
         *  the cycles = this value + 1.
         */
        uint32_t lcd_vbk_cyclelen: 13;
        /** lcd_next_frame_en : R/W; bitpos: [25]; default: 0;
         *  1: Send the next frame data when the current frame is sent out. 0: LCD stops when
         *  the current frame is sent out.
         */
        uint32_t lcd_next_frame_en: 1;
        /** lcd_bk_en : R/W; bitpos: [26]; default: 0;
         *  1: Enable blank region when LCD sends data out. 0: No blank region.
         */
        uint32_t lcd_bk_en: 1;
        /** lcd_afifo_reset : WO; bitpos: [27]; default: 0;
         *  Async Tx FIFO reset signal.
         */
        uint32_t lcd_afifo_reset: 1;
        /** lcd_cd_data_set : R/W; bitpos: [28]; default: 0;
         *  1: LCD_CD = !LCD_CAM_LCD_CD_IDLE_EDGE when LCD is in DOUT phase. 0: LCD_CD =
         *  LCD_CAM_LCD_CD_IDLE_EDGE.
         */
        uint32_t lcd_cd_data_set: 1;
        /** lcd_cd_dummy_set : R/W; bitpos: [29]; default: 0;
         *  1: LCD_CD = !LCD_CAM_LCD_CD_IDLE_EDGE when LCD is in DUMMY phase. 0: LCD_CD =
         *  LCD_CAM_LCD_CD_IDLE_EDGE.
         */
        uint32_t lcd_cd_dummy_set: 1;
        /** lcd_cd_cmd_set : R/W; bitpos: [30]; default: 0;
         *  1: LCD_CD = !LCD_CAM_LCD_CD_IDLE_EDGE when LCD is in CMD phase. 0: LCD_CD =
         *  LCD_CAM_LCD_CD_IDLE_EDGE.
         */
        uint32_t lcd_cd_cmd_set: 1;
        /** lcd_cd_idle_edge : R/W; bitpos: [31]; default: 0;
         *  The default value of LCD_CD.
         */
        uint32_t lcd_cd_idle_edge: 1;
    };
    uint32_t val;
} lcd_cam_lcd_misc_reg_t;

/** Type of lcd_ctrl register
 *  LCD signal configuration register
 */
typedef union {
    struct {
        /** lcd_hb_front : R/W; bitpos: [10:0]; default: 0;
         *  It is the horizontal blank front porch of a frame.
         */
        uint32_t lcd_hb_front: 11;
        /** lcd_va_height : R/W; bitpos: [20:11]; default: 0;
         *  It is the vertical active height of a frame.
         */
        uint32_t lcd_va_height: 10;
        /** lcd_vt_height : R/W; bitpos: [30:21]; default: 0;
         *  It is the vertical total height of a frame.
         */
        uint32_t lcd_vt_height: 10;
        /** lcd_rgb_mode_en : R/W; bitpos: [31]; default: 0;
         *  1: Enable RGB mode, and input VSYNC, HSYNC, and DE signals. 0: Disable.
         */
        uint32_t lcd_rgb_mode_en: 1;
    };
    uint32_t val;
} lcd_cam_lcd_ctrl_reg_t;

/** Type of lcd_ctrl1 register
 *  LCD signal configuration register 1
 */
typedef union {
    struct {
        /** lcd_vb_front : R/W; bitpos: [7:0]; default: 0;
         *  It is the vertical blank front porch of a frame.
         */
        uint32_t lcd_vb_front: 8;
        /** lcd_ha_width : R/W; bitpos: [19:8]; default: 0;
         *  It is the horizontal active width of a frame.
         */
        uint32_t lcd_ha_width: 12;
        /** lcd_ht_width : R/W; bitpos: [31:20]; default: 0;
         *  It is the horizontal total width of a frame.
         */
        uint32_t lcd_ht_width: 12;
    };
    uint32_t val;
} lcd_cam_lcd_ctrl1_reg_t;

/** Type of lcd_ctrl2 register
 *  LCD signal configuration register 2
 */
typedef union {
    struct {
        /** lcd_vsync_width : R/W; bitpos: [6:0]; default: 0;
         *  It is the width of LCD_VSYNC active pulse in a line.
         */
        uint32_t lcd_vsync_width: 7;
        /** lcd_vsync_idle_pol : R/W; bitpos: [7]; default: 0;
         *  It is the idle value of LCD_VSYNC.
         */
        uint32_t lcd_vsync_idle_pol: 1;
        /** lcd_de_idle_pol : R/W; bitpos: [8]; default: 0;
         *  It is the idle value of LCD_DE.
         */
        uint32_t lcd_de_idle_pol: 1;
        /** lcd_hs_blank_en : R/W; bitpos: [9]; default: 0;
         *  1: The pulse of LCD_HSYNC is out in vertical blanking lines in RGB mode. 0:
         *  LCD_HSYNC pulse is valid only in active region lines in RGB mode.
         */
        uint32_t lcd_hs_blank_en: 1;
        uint32_t reserved_10: 6;
        /** lcd_hsync_width : R/W; bitpos: [22:16]; default: 0;
         *  It is the width of LCD_HSYNC active pulse in a line.
         */
        uint32_t lcd_hsync_width: 7;
        /** lcd_hsync_idle_pol : R/W; bitpos: [23]; default: 0;
         *  It is the idle value of LCD_HSYNC.
         */
        uint32_t lcd_hsync_idle_pol: 1;
        /** lcd_hsync_position : R/W; bitpos: [31:24]; default: 0;
         *  It is the position of LCD_HSYNC active pulse in a line.
         */
        uint32_t lcd_hsync_position: 8;
    };
    uint32_t val;
} lcd_cam_lcd_ctrl2_reg_t;

/** Type of lcd_cmd_val register
 *  LCD command value configuration register
 */
typedef union {
    struct {
        /** lcd_cmd_value : R/W; bitpos: [31:0]; default: 0;
         *  The LCD write command value.
         */
        uint32_t lcd_cmd_value: 32;
    };
    uint32_t val;
} lcd_cam_lcd_cmd_val_reg_t;

/** Type of lcd_dly_mode register
 *  LCD signal delay configuration register
 */
typedef union {
    struct {
        /** lcd_cd_mode : R/W; bitpos: [1:0]; default: 0;
         *  The output LCD_CD is delayed by module clock LCD_CLK. 0: output without delay. 1:
         *  delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t lcd_cd_mode: 2;
        /** lcd_de_mode : R/W; bitpos: [3:2]; default: 0;
         *  The output LCD_DE is delayed by module clock LCD_CLK. 0: output without delay. 1:
         *  delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t lcd_de_mode: 2;
        /** lcd_hsync_mode : R/W; bitpos: [5:4]; default: 0;
         *  The output LCD_HSYNC is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t lcd_hsync_mode: 2;
        /** lcd_vsync_mode : R/W; bitpos: [7:6]; default: 0;
         *  The output LCD_VSYNC is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delay by the falling edge of LCD_CLK.
         */
        uint32_t lcd_vsync_mode: 2;
        uint32_t reserved_8: 24;
    };
    uint32_t val;
} lcd_cam_lcd_dly_mode_reg_t;

/** Type of lcd_data_dout_mode register
 *  LCD data delay configuration register
 */
typedef union {
    struct {
        /** dout0_mode : R/W; bitpos: [1:0]; default: 0;
         *  The output data bit 0 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout0_mode: 2;
        /** dout1_mode : R/W; bitpos: [3:2]; default: 0;
         *  The output data bit 1 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout1_mode: 2;
        /** dout2_mode : R/W; bitpos: [5:4]; default: 0;
         *  The output data bit 2 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout2_mode: 2;
        /** dout3_mode : R/W; bitpos: [7:6]; default: 0;
         *  The output data bit 3 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout3_mode: 2;
        /** dout4_mode : R/W; bitpos: [9:8]; default: 0;
         *  The output data bit 4 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout4_mode: 2;
        /** dout5_mode : R/W; bitpos: [11:10]; default: 0;
         *  The output data bit 5 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout5_mode: 2;
        /** dout6_mode : R/W; bitpos: [13:12]; default: 0;
         *  The output data bit 6 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout6_mode: 2;
        /** dout7_mode : R/W; bitpos: [15:14]; default: 0;
         *  The output data bit 7 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout7_mode: 2;
        /** dout8_mode : R/W; bitpos: [17:16]; default: 0;
         *  The output data bit 8 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout8_mode: 2;
        /** dout9_mode : R/W; bitpos: [19:18]; default: 0;
         *  The output data bit 9 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout9_mode: 2;
        /** dout10_mode : R/W; bitpos: [21:20]; default: 0;
         *  The output data bit 10 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout10_mode: 2;
        /** dout11_mode : R/W; bitpos: [23:22]; default: 0;
         *  The output data bit 11 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout11_mode: 2;
        /** dout12_mode : R/W; bitpos: [25:24]; default: 0;
         *  The output data bit 12 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout12_mode: 2;
        /** dout13_mode : R/W; bitpos: [27:26]; default: 0;
         *  The output data bit 13 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout13_mode: 2;
        /** dout14_mode : R/W; bitpos: [29:28]; default: 0;
         *  The output data bit 14 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout14_mode: 2;
        /** dout15_mode : R/W; bitpos: [31:30]; default: 0;
         *  The output data bit 15 is delayed by module clock LCD_CLK. 0: output without delay.
         *  1: delayed by the rising edge of LCD_CLK. 2: delayed by the falling edge of LCD_CLK.
         */
        uint32_t dout15_mode: 2;
    };
    uint32_t val;
} lcd_cam_lcd_data_dout_mode_reg_t;


/** Group: Camera configuration registers */
/** Type of cam_ctrl register
 *  Camera clock configuration register
 */
typedef union {
    struct {
        /** cam_stop_en : R/W; bitpos: [0]; default: 0;
         *  Camera stop enable signal, 1: camera stops when GDMA Rx FIFO is full. 0: Do not
         *  stop.
         */
        uint32_t cam_stop_en: 1;
        /** cam_vsync_filter_thres : R/W; bitpos: [3:1]; default: 0;
         *  Filter threshold value for CAM_VSYNC signal.
         */
        uint32_t cam_vsync_filter_thres: 3;
        /** cam_update : R/W; bitpos: [4]; default: 0;
         *  1: Update camera registers. This bit is cleared by hardware. 0: Do not care.
         */
        uint32_t cam_update: 1;
        /** cam_byte_order : R/W; bitpos: [5]; default: 0;
         *  1: Invert data byte order, only valid in 16-bit mode. 0: Do not change.
         */
        uint32_t cam_byte_order: 1;
        /** cam_bit_order : R/W; bitpos: [6]; default: 0;
         *  1: Change data bit order, change CAM_DATA_in[7:0] to CAM_DATA_in[0:7] in 8-bit
         *  mode, and bits[15:0] to bits[0:15] in 16-bit mode.  0: Do not change.
         */
        uint32_t cam_bit_order: 1;
        /** cam_line_int_en : R/W; bitpos: [7]; default: 0;
         *  1: Enable to generate LCD_CAM_CAM_HS_INT. 0: Disable.
         */
        uint32_t cam_line_int_en: 1;
        /** cam_vs_eof_en : R/W; bitpos: [8]; default: 0;
         *  1: Enable CAM_VSYNC to generate in_suc_eof. 0: in_suc_eof is controlled by
         *  LCD_CAM_CAM_REC_DATA_BYTELEN.
         */
        uint32_t cam_vs_eof_en: 1;
        /** cam_clkm_div_num : R/W; bitpos: [16:9]; default: 0;
         *  Integral camera clock divider value.
         */
        uint32_t cam_clkm_div_num: 8;
        /** cam_clkm_div_b : R/W; bitpos: [22:17]; default: 0;
         *  Fractional clock divider numerator value.
         */
        uint32_t cam_clkm_div_b: 6;
        /** cam_clkm_div_a : R/W; bitpos: [28:23]; default: 0;
         *  Fractional clock divider denominator value.
         */
        uint32_t cam_clkm_div_a: 6;
        /** cam_clk_sel : R/W; bitpos: [30:29]; default: 0;
         *  Select camera module source clock. 0: Clock source is disabled. 1: XTAL_CLK. 2:
         *  PLL_D2_CLK. 3: PLL_F160M_CLK.
         */
        uint32_t cam_clk_sel: 2;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} lcd_cam_cam_ctrl_reg_t;

/** Type of cam_ctrl1 register
 *  Camera control register
 */
typedef union {
    struct {
        /** cam_rec_data_bytelen : R/W; bitpos: [15:0]; default: 0;
         *  Configure camera received data byte length. When the length of received data
         *  reaches this value + 1, GDMA in_suc_eof_int is triggered.
         */
        uint32_t cam_rec_data_bytelen: 16;
        /** cam_line_int_num : R/W; bitpos: [21:16]; default: 0;
         *  Configure line number. When the number of received lines reaches this value + 1,
         *  LCD_CAM_CAM_HS_INT is triggered.
         */
        uint32_t cam_line_int_num: 6;
        /** cam_clk_inv : R/W; bitpos: [22]; default: 0;
         *  1: Invert the input signal CAM_PCLK. 0: Do not invert.
         */
        uint32_t cam_clk_inv: 1;
        /** cam_vsync_filter_en : R/W; bitpos: [23]; default: 0;
         *  1: Enable CAM_VSYNC filter function. 0: Bypass.
         */
        uint32_t cam_vsync_filter_en: 1;
        /** cam_2byte_en : R/W; bitpos: [24]; default: 0;
         *  1: The width of input data is 16 bits. 0: The width of input data is 8 bits.
         */
        uint32_t cam_2byte_en: 1;
        /** cam_de_inv : R/W; bitpos: [25]; default: 0;
         *  CAM_DE invert enable signal, valid in high level.
         */
        uint32_t cam_de_inv: 1;
        /** cam_hsync_inv : R/W; bitpos: [26]; default: 0;
         *  CAM_HSYNC invert enable signal, valid in high level.
         */
        uint32_t cam_hsync_inv: 1;
        /** cam_vsync_inv : R/W; bitpos: [27]; default: 0;
         *  CAM_VSYNC invert enable signal, valid in high level.
         */
        uint32_t cam_vsync_inv: 1;
        /** cam_vh_de_mode_en : R/W; bitpos: [28]; default: 0;
         *  1: Input control signals are CAM_DE and CAM_HSYNC. CAM_VSYNC is 1. 0: Input control
         *  signals are CAM_DE and CAM_VSYNC. CAM_HSYNC and CAM_DE are all 1 at the the same
         *  time.
         */
        uint32_t cam_vh_de_mode_en: 1;
        /** cam_start : R/W; bitpos: [29]; default: 0;
         *  Camera module start signal.
         */
        uint32_t cam_start: 1;
        /** cam_reset : WO; bitpos: [30]; default: 0;
         *  Camera module reset signal.
         */
        uint32_t cam_reset: 1;
        /** cam_afifo_reset : WO; bitpos: [31]; default: 0;
         *  Camera Async Rx FIFO reset signal.
         */
        uint32_t cam_afifo_reset: 1;
    };
    uint32_t val;
} lcd_cam_cam_ctrl1_reg_t;

/** Type of cam_rgb_yuv register
 *  Camera data format conversion register
 */
typedef union {
    struct {
        uint32_t reserved_0: 21;
        /** cam_conv_8bits_data_inv : R/W; bitpos: [21]; default: 0;
         *  Swap every two 8-bit input data. 1: Enabled. 0: Disabled.
         */
        uint32_t cam_conv_8bits_data_inv: 1;
        /** cam_conv_yuv2yuv_mode : R/W; bitpos: [23:22]; default: 0;
         *  In YUV-to-YUV mode, 0: data is converted to YUV422 format. 1: data is converted to
         *  YUV420 format. 2: data is converted to YUV411 format. 3: disabled. To enable
         *  YUV-to-YUV mode, LCD_CAM_CAM_CONV_TRANS_MODE must be set to 1.
         */
        uint32_t cam_conv_yuv2yuv_mode: 2;
        /** cam_conv_yuv_mode : R/W; bitpos: [25:24]; default: 0;
         *  In YUV-to-YUV mode and YUV-to-RGB mode, LCD_CAM_CAM_CONV_YUV_MODE decides the YUV
         *  mode of input data. 0: input data is in YUV422 format. 1: input data is in YUV420
         *  format. 2: input data is in YUV411 format. In RGB-to-YUV mode, 0: data is converted
         *  to YUV422 format. 1: data is converted to YUV420 format. 2: data is converted to
         *  YUV411 format.
         */
        uint32_t cam_conv_yuv_mode: 2;
        /** cam_conv_protocol_mode : R/W; bitpos: [26]; default: 0;
         *  0: BT601. 1: BT709.
         */
        uint32_t cam_conv_protocol_mode: 1;
        /** cam_conv_data_out_mode : R/W; bitpos: [27]; default: 0;
         *  Configure color range for output data. 0: limited color range. 1: full color range.
         */
        uint32_t cam_conv_data_out_mode: 1;
        /** cam_conv_data_in_mode : R/W; bitpos: [28]; default: 0;
         *  Configure color range for input data. 0: limited color range. 1: full color range.
         */
        uint32_t cam_conv_data_in_mode: 1;
        /** cam_conv_mode_8bits_on : R/W; bitpos: [29]; default: 0;
         *  0: 16-bit mode. 1: 8-bit mode.
         */
        uint32_t cam_conv_mode_8bits_on: 1;
        /** cam_conv_trans_mode : R/W; bitpos: [30]; default: 0;
         *  0: converted to RGB format. 1: converted to YUV format.
         */
        uint32_t cam_conv_trans_mode: 1;
        /** cam_conv_bypass : R/W; bitpos: [31]; default: 0;
         *  0: Bypass converter. 1: Enable converter.
         */
        uint32_t cam_conv_bypass: 1;
    };
    uint32_t val;
} lcd_cam_cam_rgb_yuv_reg_t;


/** Group: Interrupt registers */
/** Type of lc_dma_int_ena register
 *  LCD_CAM GDMA interrupt enable register
 */
typedef union {
    struct {
        /** lcd_vsync_int_ena : R/W; bitpos: [0]; default: 0;
         *  The enable bit for LCD_CAM_LCD_VSYNC_INT interrupt.
         */
        uint32_t lcd_vsync_int_ena: 1;
        /** lcd_trans_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  The enable bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
         */
        uint32_t lcd_trans_done_int_ena: 1;
        /** cam_vsync_int_ena : R/W; bitpos: [2]; default: 0;
         *  The enable bit for LCD_CAM_CAM_VSYNC_INT interrupt.
         */
        uint32_t cam_vsync_int_ena: 1;
        /** cam_hs_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable bit for LCD_CAM_CAM_HS_INT interrupt.
         */
        uint32_t cam_hs_int_ena: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} lcd_cam_lc_dma_int_ena_reg_t;

/** Type of lc_dma_int_raw register
 *  LCD_CAM GDMA raw interrupt status register
 */
typedef union {
    struct {
        /** lcd_vsync_int_raw : RO; bitpos: [0]; default: 0;
         *  The raw bit for LCD_CAM_LCD_VSYNC_INT interrupt.
         */
        uint32_t lcd_vsync_int_raw: 1;
        /** lcd_trans_done_int_raw : RO; bitpos: [1]; default: 0;
         *  The raw bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
         */
        uint32_t lcd_trans_done_int_raw: 1;
        /** cam_vsync_int_raw : RO; bitpos: [2]; default: 0;
         *  The raw bit for LCD_CAM_CAM_VSYNC_INT interrupt.
         */
        uint32_t cam_vsync_int_raw: 1;
        /** cam_hs_int_raw : RO; bitpos: [3]; default: 0;
         *  The raw bit for LCD_CAM_CAM_HS_INT interrupt.
         */
        uint32_t cam_hs_int_raw: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} lcd_cam_lc_dma_int_raw_reg_t;

/** Type of lc_dma_int_st register
 *  LCD_CAM GDMA masked interrupt status register
 */
typedef union {
    struct {
        /** lcd_vsync_int_st : RO; bitpos: [0]; default: 0;
         *  The status bit for LCD_CAM_LCD_VSYNC_INT interrupt.
         */
        uint32_t lcd_vsync_int_st: 1;
        /** lcd_trans_done_int_st : RO; bitpos: [1]; default: 0;
         *  The status bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
         */
        uint32_t lcd_trans_done_int_st: 1;
        /** cam_vsync_int_st : RO; bitpos: [2]; default: 0;
         *  The status bit for LCD_CAM_CAM_VSYNC_INT interrupt.
         */
        uint32_t cam_vsync_int_st: 1;
        /** cam_hs_int_st : RO; bitpos: [3]; default: 0;
         *  The status bit for LCD_CAM_CAM_HS_INT interrupt.
         */
        uint32_t cam_hs_int_st: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} lcd_cam_lc_dma_int_st_reg_t;

/** Type of lc_dma_int_clr register
 *  LCD_CAM GDMA interrupt clear register
 */
typedef union {
    struct {
        /** lcd_vsync_int_clr : WO; bitpos: [0]; default: 0;
         *  The clear bit for LCD_CAM_LCD_VSYNC_INT interrupt.
         */
        uint32_t lcd_vsync_int_clr: 1;
        /** lcd_trans_done_int_clr : WO; bitpos: [1]; default: 0;
         *  The clear bit for LCD_CAM_LCD_TRANS_DONE_INT interrupt.
         */
        uint32_t lcd_trans_done_int_clr: 1;
        /** cam_vsync_int_clr : WO; bitpos: [2]; default: 0;
         *  The clear bit for LCD_CAM_CAM_VSYNC_INT interrupt.
         */
        uint32_t cam_vsync_int_clr: 1;
        /** cam_hs_int_clr : WO; bitpos: [3]; default: 0;
         *  The clear bit for LCD_CAM_CAM_HS_INT interrupt.
         */
        uint32_t cam_hs_int_clr: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} lcd_cam_lc_dma_int_clr_reg_t;


/** Group: Version register */
/** Type of lc_reg_date register
 *  Version control register
 */
typedef union {
    struct {
        /** lc_date : R/W; bitpos: [27:0]; default: 33566752;
         *  Version control register
         */
        uint32_t lc_date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} lcd_cam_lc_reg_date_reg_t;


typedef struct lcd_cam_dev_t {
    volatile lcd_cam_lcd_clock_reg_t lcd_clock;
    volatile lcd_cam_cam_ctrl_reg_t cam_ctrl;
    volatile lcd_cam_cam_ctrl1_reg_t cam_ctrl1;
    volatile lcd_cam_cam_rgb_yuv_reg_t cam_rgb_yuv;
    volatile lcd_cam_lcd_rgb_yuv_reg_t lcd_rgb_yuv;
    volatile lcd_cam_lcd_user_reg_t lcd_user;
    volatile lcd_cam_lcd_misc_reg_t lcd_misc;
    volatile lcd_cam_lcd_ctrl_reg_t lcd_ctrl;
    volatile lcd_cam_lcd_ctrl1_reg_t lcd_ctrl1;
    volatile lcd_cam_lcd_ctrl2_reg_t lcd_ctrl2;
    volatile lcd_cam_lcd_cmd_val_reg_t lcd_cmd_val;
    uint32_t reserved_02c;
    volatile lcd_cam_lcd_dly_mode_reg_t lcd_dly_mode;
    uint32_t reserved_034;
    volatile lcd_cam_lcd_data_dout_mode_reg_t lcd_data_dout_mode;
    uint32_t reserved_03c[10];
    volatile lcd_cam_lc_dma_int_ena_reg_t lc_dma_int_ena;
    volatile lcd_cam_lc_dma_int_raw_reg_t lc_dma_int_raw;
    volatile lcd_cam_lc_dma_int_st_reg_t lc_dma_int_st;
    volatile lcd_cam_lc_dma_int_clr_reg_t lc_dma_int_clr;
    uint32_t reserved_074[34];
    volatile lcd_cam_lc_reg_date_reg_t lc_reg_date;
} lcd_cam_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(lcd_cam_dev_t) == 0x100, "Invalid size of lcd_cam_dev_t structure");
#endif

extern lcd_cam_dev_t LCD_CAM;

#ifdef __cplusplus
}
#endif
