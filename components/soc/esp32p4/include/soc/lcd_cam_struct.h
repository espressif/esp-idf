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

/** Group: lcd configuration registers */
/** Type of lcd_clock register
 *  LCD clock config register.
 */
typedef union {
    struct {
        /** lcd_clkcnt_n : R/W; bitpos: [5:0]; default: 3;
         *  f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1) when reg_clk_equ_sysclk is 0.
         */
        uint32_t lcd_clkcnt_n:6;
        /** lcd_clk_equ_sysclk : R/W; bitpos: [6]; default: 1;
         *  1: f_LCD_PCLK = f_LCD_CLK. 0: f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1).
         */
        uint32_t lcd_clk_equ_sysclk:1;
        /** lcd_ck_idle_edge : R/W; bitpos: [7]; default: 0;
         *  1: LCD_PCLK line is high when idle     0: LCD_PCLK line is low when idle.
         */
        uint32_t lcd_ck_idle_edge:1;
        /** lcd_ck_out_edge : R/W; bitpos: [8]; default: 0;
         *  1: LCD_PCLK line is high in the first half data cycle.     0: LCD_PCLK line is low
         *  in the second half data cycle.
         */
        uint32_t lcd_ck_out_edge:1;
        /** lcd_clkm_div_num : R/W; bitpos: [16:9]; default: 4;
         *  Integral LCD clock divider value
         */
        uint32_t lcd_clkm_div_num:8;
        /** lcd_clkm_div_b : R/W; bitpos: [22:17]; default: 0;
         *  Fractional clock divider numerator value
         */
        uint32_t lcd_clkm_div_b:6;
        /** lcd_clkm_div_a : R/W; bitpos: [28:23]; default: 0;
         *  Fractional clock divider denominator value
         */
        uint32_t lcd_clkm_div_a:6;
        /** lcd_clk_sel : R/W; bitpos: [30:29]; default: 0;
         *  Select LCD module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.
         */
        uint32_t lcd_clk_sel:2;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Set this bit to enable clk gate
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lcdcam_lcd_clock_reg_t;

/** Type of lcd_rgb_yuv register
 *  LCD YUV/RGB converter configuration register.
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lcd_conv_8bits_data_inv : R/W; bitpos: [20]; default: 0;
         *  1:invert every two 8bits input data. 2. disabled.
         */
        uint32_t lcd_conv_8bits_data_inv:1;
        /** lcd_conv_txtorx : R/W; bitpos: [21]; default: 0;
         *  0: txtorx mode off. 1: txtorx mode on.
         */
        uint32_t lcd_conv_txtorx:1;
        /** lcd_conv_yuv2yuv_mode : R/W; bitpos: [23:22]; default: 3;
         *  0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable yuv2yuv mode,
         *  trans_mode must be set to 1.
         */
        uint32_t lcd_conv_yuv2yuv_mode:2;
        /** lcd_conv_yuv_mode : R/W; bitpos: [25:24]; default: 0;
         *  0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode, yuv_mode decides the yuv
         *  mode of Data_in
         */
        uint32_t lcd_conv_yuv_mode:2;
        /** lcd_conv_protocol_mode : R/W; bitpos: [26]; default: 0;
         *  0:BT601. 1:BT709.
         */
        uint32_t lcd_conv_protocol_mode:1;
        /** lcd_conv_data_out_mode : R/W; bitpos: [27]; default: 0;
         *  LIMIT or FULL mode of Data out. 0: limit. 1: full
         */
        uint32_t lcd_conv_data_out_mode:1;
        /** lcd_conv_data_in_mode : R/W; bitpos: [28]; default: 0;
         *  LIMIT or FULL mode of Data in. 0: limit. 1: full
         */
        uint32_t lcd_conv_data_in_mode:1;
        /** lcd_conv_mode_8bits_on : R/W; bitpos: [29]; default: 0;
         *  0: 16bits mode. 1: 8bits mode.
         */
        uint32_t lcd_conv_mode_8bits_on:1;
        /** lcd_conv_trans_mode : R/W; bitpos: [30]; default: 0;
         *  0: YUV to RGB. 1: RGB to YUV.
         */
        uint32_t lcd_conv_trans_mode:1;
        /** lcd_conv_enable : R/W; bitpos: [31]; default: 0;
         *  0: Bypass converter. 1: Enable converter.
         */
        uint32_t lcd_conv_enable:1;
    };
    uint32_t val;
} lcdcam_lcd_rgb_yuv_reg_t;

/** Type of lcd_user register
 *  LCD config register.
 */
typedef union {
    struct {
        /** lcd_dout_cyclelen : R/W; bitpos: [12:0]; default: 1;
         *  The output data cycles minus 1 of LCD module.
         */
        uint32_t lcd_dout_cyclelen:13;
        /** lcd_always_out_en : R/W; bitpos: [13]; default: 0;
         *  LCD always output when LCD is in LCD_DOUT state, unless reg_lcd_start is cleared or
         *  reg_lcd_reset is set.
         */
        uint32_t lcd_always_out_en:1;
        /** lcd_dout_byte_swizzle_mode : R/W; bitpos: [16:14]; default: 0;
         *  0: ABAB->BABA. 1: ABC->ACB. 2: ABC->BAC. 3: ABC->BCA. 4:ABC->CAB. 5:ABC->CBA
         */
        uint32_t lcd_dout_byte_swizzle_mode:3;
        /** lcd_dout_byte_swizzle_enable : R/W; bitpos: [17]; default: 0;
         *  1: enable byte swizzle 0: disable
         */
        uint32_t lcd_dout_byte_swizzle_enable:1;
        /** lcd_dout_bit_order : R/W; bitpos: [18]; default: 0;
         *  1: change bit order in every byte. 0: Not change.
         */
        uint32_t lcd_dout_bit_order:1;
        /** lcd_byte_mode : R/W; bitpos: [20:19]; default: 0;
         *  2: 24bit mode. 1: 16bit mode. 0: 8bit mode
         */
        uint32_t lcd_byte_mode:2;
        /** lcd_update_reg : R/W/SC; bitpos: [21]; default: 0;
         *  1: Update LCD registers, will be cleared by hardware. 0 : Not care.
         */
        uint32_t lcd_update_reg:1;
        /** lcd_bit_order : R/W; bitpos: [22]; default: 0;
         *  1: Change data bit order, change LCD_DATA_out[7:0] to LCD_DATA_out[0:7] in one byte
         *  mode, and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.
         */
        uint32_t lcd_bit_order:1;
        /** lcd_byte_order : R/W; bitpos: [23]; default: 0;
         *  1: invert data byte order, only valid in 2 byte mode. 0: Not change.
         */
        uint32_t lcd_byte_order:1;
        /** lcd_dout : R/W; bitpos: [24]; default: 0;
         *  1: Be able to send data out in LCD sequence when LCD starts. 0: Disable.
         */
        uint32_t lcd_dout:1;
        /** lcd_dummy : R/W; bitpos: [25]; default: 0;
         *  1: Enable DUMMY phase in LCD sequence when LCD starts. 0: Disable.
         */
        uint32_t lcd_dummy:1;
        /** lcd_cmd : R/W; bitpos: [26]; default: 0;
         *  1: Be able to send command in LCD sequence when LCD starts. 0: Disable.
         */
        uint32_t lcd_cmd:1;
        /** lcd_start : R/W/SC; bitpos: [27]; default: 0;
         *  LCD start sending data enable signal, valid in high level.
         */
        uint32_t lcd_start:1;
        /** lcd_reset : WT; bitpos: [28]; default: 0;
         *  The value of  command.
         */
        uint32_t lcd_reset:1;
        /** lcd_dummy_cyclelen : R/W; bitpos: [30:29]; default: 0;
         *  The dummy cycle length minus 1.
         */
        uint32_t lcd_dummy_cyclelen:2;
        /** lcd_cmd_2_cycle_en : R/W; bitpos: [31]; default: 0;
         *  The cycle length of command phase. 1: 2 cycles. 0: 1 cycle.
         */
        uint32_t lcd_cmd_2_cycle_en:1;
    };
    uint32_t val;
} lcdcam_lcd_user_reg_t;

/** Type of lcd_misc register
 *  LCD config register.
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** lcd_wire_mode : R/W; bitpos: [5:4]; default: 0;
         *  The wire width of LCD output. 0: 8bit. 1: 16bit 2: 24bit
         */
        uint32_t lcd_wire_mode:2;
        /** lcd_vfk_cyclelen : R/W; bitpos: [11:6]; default: 3;
         *  The setup cycle length minus 1 in LCD non-RGB mode.
         */
        uint32_t lcd_vfk_cyclelen:6;
        /** lcd_vbk_cyclelen : R/W; bitpos: [24:12]; default: 0;
         *  The vertical back blank region cycle length minus 1 in LCD RGB mode, or the hold
         *  time cycle length in LCD non-RGB mode.
         */
        uint32_t lcd_vbk_cyclelen:13;
        /** lcd_next_frame_en : R/W; bitpos: [25]; default: 0;
         *  1: Send the next frame data when the current frame is sent out. 0: LCD stops when
         *  the current frame is sent out.
         */
        uint32_t lcd_next_frame_en:1;
        /** lcd_bk_en : R/W; bitpos: [26]; default: 0;
         *  1: Enable blank region when LCD sends data out. 0: No blank region.
         */
        uint32_t lcd_bk_en:1;
        /** lcd_afifo_reset : WT; bitpos: [27]; default: 0;
         *  LCD AFIFO reset signal.
         */
        uint32_t lcd_afifo_reset:1;
        /** lcd_cd_data_set : R/W; bitpos: [28]; default: 0;
         *  1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DOUT state.  0: LCD_CD =
         *  reg_cd_idle_edge.
         */
        uint32_t lcd_cd_data_set:1;
        /** lcd_cd_dummy_set : R/W; bitpos: [29]; default: 0;
         *  1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DUMMY state.  0: LCD_CD =
         *  reg_cd_idle_edge.
         */
        uint32_t lcd_cd_dummy_set:1;
        /** lcd_cd_cmd_set : R/W; bitpos: [30]; default: 0;
         *  1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_CMD state.  0: LCD_CD =
         *  reg_cd_idle_edge.
         */
        uint32_t lcd_cd_cmd_set:1;
        /** lcd_cd_idle_edge : R/W; bitpos: [31]; default: 0;
         *  The default value of LCD_CD.
         */
        uint32_t lcd_cd_idle_edge:1;
    };
    uint32_t val;
} lcdcam_lcd_misc_reg_t;

/** Type of lcd_ctrl register
 *  LCD config register.
 */
typedef union {
    struct {
        /** lcd_hb_front : R/W; bitpos: [10:0]; default: 0;
         *  It is the horizontal blank front porch of a frame.
         */
        uint32_t lcd_hb_front:11;
        /** lcd_va_height : R/W; bitpos: [20:11]; default: 0;
         *  It is the vertical active height of a frame.
         */
        uint32_t lcd_va_height:10;
        /** lcd_vt_height : R/W; bitpos: [30:21]; default: 0;
         *  It is the vertical total height of a frame.
         */
        uint32_t lcd_vt_height:10;
        /** lcd_rgb_mode_en : R/W; bitpos: [31]; default: 0;
         *  1: Enable LCD RGB mode. 0: Disable LCD RGB mode.
         */
        uint32_t lcd_rgb_mode_en:1;
    };
    uint32_t val;
} lcdcam_lcd_ctrl_reg_t;

/** Type of lcd_ctrl1 register
 *  LCD config register.
 */
typedef union {
    struct {
        /** lcd_vb_front : R/W; bitpos: [7:0]; default: 0;
         *  It is the vertical blank front porch of a frame.
         */
        uint32_t lcd_vb_front:8;
        /** lcd_ha_width : R/W; bitpos: [19:8]; default: 0;
         *  It is the horizontal active width of a frame.
         */
        uint32_t lcd_ha_width:12;
        /** lcd_ht_width : R/W; bitpos: [31:20]; default: 0;
         *  It is the horizontal total width of a frame.
         */
        uint32_t lcd_ht_width:12;
    };
    uint32_t val;
} lcdcam_lcd_ctrl1_reg_t;

/** Type of lcd_ctrl2 register
 *  LCD config register.
 */
typedef union {
    struct {
        /** lcd_vsync_width : R/W; bitpos: [6:0]; default: 1;
         *  It is the position of LCD_VSYNC active pulse in a line.
         */
        uint32_t lcd_vsync_width:7;
        /** lcd_vsync_idle_pol : R/W; bitpos: [7]; default: 0;
         *  It is the idle value of LCD_VSYNC.
         */
        uint32_t lcd_vsync_idle_pol:1;
        /** lcd_de_idle_pol : R/W; bitpos: [8]; default: 0;
         *  It is the idle value of LCD_DE.
         */
        uint32_t lcd_de_idle_pol:1;
        /** lcd_hs_blank_en : R/W; bitpos: [9]; default: 0;
         *  1: The pulse of LCD_HSYNC is out in vertical blanking lines RGB mode. 0: LCD_HSYNC
         *  pulse is valid only in active region lines in RGB mode.
         */
        uint32_t lcd_hs_blank_en:1;
        uint32_t reserved_10:6;
        /** lcd_hsync_width : R/W; bitpos: [22:16]; default: 1;
         *  It is the position of LCD_HSYNC active pulse in a line.
         */
        uint32_t lcd_hsync_width:7;
        /** lcd_hsync_idle_pol : R/W; bitpos: [23]; default: 0;
         *  It is the idle value of LCD_HSYNC.
         */
        uint32_t lcd_hsync_idle_pol:1;
        /** lcd_hsync_position : R/W; bitpos: [31:24]; default: 0;
         *  It is the position of LCD_HSYNC active pulse in a line.
         */
        uint32_t lcd_hsync_position:8;
    };
    uint32_t val;
} lcdcam_lcd_ctrl2_reg_t;

/** Type of lcd_first_cmd_val register
 *  LCD config register.
 */
typedef union {
    struct {
        /** lcd_first_cmd_value : R/W; bitpos: [31:0]; default: 0;
         *  The LCD write command value of first cmd cycle.
         */
        uint32_t lcd_first_cmd_value:32;
    };
    uint32_t val;
} lcdcam_lcd_first_cmd_val_reg_t;

/** Type of lcd_latter_cmd_val register
 *  LCD config register.
 */
typedef union {
    struct {
        /** lcd_latter_cmd_value : R/W; bitpos: [31:0]; default: 0;
         *  The LCD write command value of latter cmd cycle.
         */
        uint32_t lcd_latter_cmd_value:32;
    };
    uint32_t val;
} lcdcam_lcd_latter_cmd_val_reg_t;

/** Type of lcd_dly_mode_cfg1 register
 *  LCD config register.
 */
typedef union {
    struct {
        /** dout16_mode : R/W; bitpos: [1:0]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout16_mode:2;
        /** dout17_mode : R/W; bitpos: [3:2]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout17_mode:2;
        /** dout18_mode : R/W; bitpos: [5:4]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout18_mode:2;
        /** dout19_mode : R/W; bitpos: [7:6]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout19_mode:2;
        /** dout20_mode : R/W; bitpos: [9:8]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout20_mode:2;
        /** dout21_mode : R/W; bitpos: [11:10]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout21_mode:2;
        /** dout22_mode : R/W; bitpos: [13:12]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout22_mode:2;
        /** dout23_mode : R/W; bitpos: [15:14]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout23_mode:2;
        /** lcd_cd_mode : R/W; bitpos: [17:16]; default: 0;
         *  The output LCD_CD is delayed by module clock LCD_CLK. 0: output without delayed. 1:
         *  delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
         */
        uint32_t lcd_cd_mode:2;
        /** lcd_de_mode : R/W; bitpos: [19:18]; default: 0;
         *  The output LCD_DE is delayed by module clock LCD_CLK. 0: output without delayed. 1:
         *  delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
         */
        uint32_t lcd_de_mode:2;
        /** lcd_hsync_mode : R/W; bitpos: [21:20]; default: 0;
         *  The output LCD_HSYNC is delayed by module clock LCD_CLK. 0: output without delayed.
         *  1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
         */
        uint32_t lcd_hsync_mode:2;
        /** lcd_vsync_mode : R/W; bitpos: [23:22]; default: 0;
         *  The output LCD_VSYNC is delayed by module clock LCD_CLK. 0: output without delayed.
         *  1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of LCD_CLK.
         */
        uint32_t lcd_vsync_mode:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} lcdcam_lcd_dly_mode_cfg1_reg_t;

/** Type of lcd_dly_mode_cfg2 register
 *  LCD config register.
 */
typedef union {
    struct {
        /** dout0_mode : R/W; bitpos: [1:0]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout0_mode:2;
        /** dout1_mode : R/W; bitpos: [3:2]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout1_mode:2;
        /** dout2_mode : R/W; bitpos: [5:4]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout2_mode:2;
        /** dout3_mode : R/W; bitpos: [7:6]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout3_mode:2;
        /** dout4_mode : R/W; bitpos: [9:8]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout4_mode:2;
        /** dout5_mode : R/W; bitpos: [11:10]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout5_mode:2;
        /** dout6_mode : R/W; bitpos: [13:12]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout6_mode:2;
        /** dout7_mode : R/W; bitpos: [15:14]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout7_mode:2;
        /** dout8_mode : R/W; bitpos: [17:16]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout8_mode:2;
        /** dout9_mode : R/W; bitpos: [19:18]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout9_mode:2;
        /** dout10_mode : R/W; bitpos: [21:20]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout10_mode:2;
        /** dout11_mode : R/W; bitpos: [23:22]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout11_mode:2;
        /** dout12_mode : R/W; bitpos: [25:24]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout12_mode:2;
        /** dout13_mode : R/W; bitpos: [27:26]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout13_mode:2;
        /** dout14_mode : R/W; bitpos: [29:28]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout14_mode:2;
        /** dout15_mode : R/W; bitpos: [31:30]; default: 0;
         *  The output data bit $n is delayed by module clock LCD_CLK. 0: output without
         *  delayed. 1: delay by the positive edge of LCD_CLK. 2: delay by the negative edge of
         *  LCD_CLK.
         */
        uint32_t dout15_mode:2;
    };
    uint32_t val;
} lcdcam_lcd_dly_mode_cfg2_reg_t;


/** Group: cam configuration registers */
/** Type of cam_ctrl register
 *  CAM config register.
 */
typedef union {
    struct {
        /** cam_stop_en : R/W; bitpos: [0]; default: 0;
         *  Camera stop enable signal, 1: camera stops when DMA Rx FIFO is full. 0: Not stop.
         */
        uint32_t cam_stop_en:1;
        /** cam_vsync_filter_thres : R/W; bitpos: [3:1]; default: 0;
         *  Filter threshold value for CAM_VSYNC signal.
         */
        uint32_t cam_vsync_filter_thres:3;
        /** cam_update_reg : R/W/SC; bitpos: [4]; default: 0;
         *  1: Update Camera registers, will be cleared by hardware. 0 : Not care.
         */
        uint32_t cam_update_reg:1;
        /** cam_byte_order : R/W; bitpos: [5]; default: 0;
         *  1: Change data bit order, change CAM_DATA_in[7:0] to CAM_DATA_in[0:7] in one byte
         *  mode, and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.
         */
        uint32_t cam_byte_order:1;
        /** cam_bit_order : R/W; bitpos: [6]; default: 0;
         *  1: invert data byte order, only valid in 2 byte mode. 0: Not change.
         */
        uint32_t cam_bit_order:1;
        /** cam_line_int_en : R/W; bitpos: [7]; default: 0;
         *  1: Enable to generate CAM_HS_INT. 0: Disable.
         */
        uint32_t cam_line_int_en:1;
        /** cam_vs_eof_en : R/W; bitpos: [8]; default: 0;
         *  1: CAM_VSYNC to generate in_suc_eof. 0: in_suc_eof is controlled by
         *  reg_cam_rec_data_cyclelen.
         */
        uint32_t cam_vs_eof_en:1;
        /** cam_clkm_div_num : R/W; bitpos: [16:9]; default: 4;
         *  Integral Camera clock divider value
         */
        uint32_t cam_clkm_div_num:8;
        /** cam_clkm_div_b : R/W; bitpos: [22:17]; default: 0;
         *  Fractional clock divider numerator value
         */
        uint32_t cam_clkm_div_b:6;
        /** cam_clkm_div_a : R/W; bitpos: [28:23]; default: 0;
         *  Fractional clock divider denominator value
         */
        uint32_t cam_clkm_div_a:6;
        /** cam_clk_sel : R/W; bitpos: [30:29]; default: 0;
         *  Select Camera module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.
         */
        uint32_t cam_clk_sel:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lcdcam_cam_ctrl_reg_t;

/** Type of cam_ctrl1 register
 *  CAM config register.
 */
typedef union {
    struct {
        /** cam_rec_data_bytelen : R/W; bitpos: [15:0]; default: 0;
         *  Camera receive data byte length minus 1 to set DMA in_suc_eof_int.
         */
        uint32_t cam_rec_data_bytelen:16;
        /** cam_line_int_num : R/W; bitpos: [21:16]; default: 0;
         *  The line number minus 1 to generate cam_hs_int.
         */
        uint32_t cam_line_int_num:6;
        /** cam_clk_inv : R/W; bitpos: [22]; default: 0;
         *  1: Invert  the input signal CAM_PCLK. 0: Not invert.
         */
        uint32_t cam_clk_inv:1;
        /** cam_vsync_filter_en : R/W; bitpos: [23]; default: 0;
         *  1: Enable CAM_VSYNC filter function. 0: bypass.
         */
        uint32_t cam_vsync_filter_en:1;
        /** cam_2byte_en : R/W; bitpos: [24]; default: 0;
         *  1: The bit number of input data is 9~16.  0: The bit number of input data is 0~8.
         */
        uint32_t cam_2byte_en:1;
        /** cam_de_inv : R/W; bitpos: [25]; default: 0;
         *  CAM_DE invert enable signal, valid in high level.
         */
        uint32_t cam_de_inv:1;
        /** cam_hsync_inv : R/W; bitpos: [26]; default: 0;
         *  CAM_HSYNC invert enable signal, valid in high level.
         */
        uint32_t cam_hsync_inv:1;
        /** cam_vsync_inv : R/W; bitpos: [27]; default: 0;
         *  CAM_VSYNC invert enable signal, valid in high level.
         */
        uint32_t cam_vsync_inv:1;
        /** cam_vh_de_mode_en : R/W; bitpos: [28]; default: 0;
         *  1: Input control signals are CAM_DE CAM_HSYNC and CAM_VSYNC. 0: Input control
         *  signals are CAM_DE and CAM_VSYNC.
         */
        uint32_t cam_vh_de_mode_en:1;
        /** cam_start : R/W/SC; bitpos: [29]; default: 0;
         *  Camera module start signal.
         */
        uint32_t cam_start:1;
        /** cam_reset : WT; bitpos: [30]; default: 0;
         *  Camera module reset signal.
         */
        uint32_t cam_reset:1;
        /** cam_afifo_reset : WT; bitpos: [31]; default: 0;
         *  Camera AFIFO reset signal.
         */
        uint32_t cam_afifo_reset:1;
    };
    uint32_t val;
} lcdcam_cam_ctrl1_reg_t;

/** Type of cam_rgb_yuv register
 *  CAM YUV/RGB converter configuration register.
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** cam_conv_8bits_data_inv : R/W; bitpos: [21]; default: 0;
         *  1:invert every two 8bits input data. 2. disabled.
         */
        uint32_t cam_conv_8bits_data_inv:1;
        /** cam_conv_yuv2yuv_mode : R/W; bitpos: [23:22]; default: 3;
         *  0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable yuv2yuv mode,
         *  trans_mode must be set to 1.
         */
        uint32_t cam_conv_yuv2yuv_mode:2;
        /** cam_conv_yuv_mode : R/W; bitpos: [25:24]; default: 0;
         *  0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode, yuv_mode decides the yuv
         *  mode of Data_in
         */
        uint32_t cam_conv_yuv_mode:2;
        /** cam_conv_protocol_mode : R/W; bitpos: [26]; default: 0;
         *  0:BT601. 1:BT709.
         */
        uint32_t cam_conv_protocol_mode:1;
        /** cam_conv_data_out_mode : R/W; bitpos: [27]; default: 0;
         *  LIMIT or FULL mode of Data out. 0: limit. 1: full
         */
        uint32_t cam_conv_data_out_mode:1;
        /** cam_conv_data_in_mode : R/W; bitpos: [28]; default: 0;
         *  LIMIT or FULL mode of Data in. 0: limit. 1: full
         */
        uint32_t cam_conv_data_in_mode:1;
        /** cam_conv_mode_8bits_on : R/W; bitpos: [29]; default: 0;
         *  0: 16bits mode. 1: 8bits mode.
         */
        uint32_t cam_conv_mode_8bits_on:1;
        /** cam_conv_trans_mode : R/W; bitpos: [30]; default: 0;
         *  0: YUV to RGB. 1: RGB to YUV.
         */
        uint32_t cam_conv_trans_mode:1;
        /** cam_conv_enable : R/W; bitpos: [31]; default: 0;
         *  0: Bypass converter. 1: Enable converter.
         */
        uint32_t cam_conv_enable:1;
    };
    uint32_t val;
} lcdcam_cam_rgb_yuv_reg_t;


/** Group: Interrupt registers */
/** Type of lc_dma_int_ena register
 *  LCDCAM interrupt enable register.
 */
typedef union {
    struct {
        /** lcd_vsync_int_ena : R/W; bitpos: [0]; default: 0;
         *  The enable bit for LCD frame end interrupt.
         */
        uint32_t lcd_vsync_int_ena:1;
        /** lcd_trans_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  The enable bit for lcd transfer end interrupt.
         */
        uint32_t lcd_trans_done_int_ena:1;
        /** cam_vsync_int_ena : R/W; bitpos: [2]; default: 0;
         *  The enable bit for Camera frame end interrupt.
         */
        uint32_t cam_vsync_int_ena:1;
        /** cam_hs_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable bit for Camera line interrupt.
         */
        uint32_t cam_hs_int_ena:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lcdcam_lc_dma_int_ena_reg_t;

/** Type of lc_dma_int_raw register
 *  LCDCAM interrupt raw register, valid in level.
 */
typedef union {
    struct {
        /** lcd_vsync_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw bit for LCD frame end interrupt.
         */
        uint32_t lcd_vsync_int_raw:1;
        /** lcd_trans_done_int_raw : RO/WTC/SS; bitpos: [1]; default: 0;
         *  The raw bit for lcd transfer end interrupt.
         */
        uint32_t lcd_trans_done_int_raw:1;
        /** cam_vsync_int_raw : RO/WTC/SS; bitpos: [2]; default: 0;
         *  The raw bit for Camera frame end interrupt.
         */
        uint32_t cam_vsync_int_raw:1;
        /** cam_hs_int_raw : RO/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit for Camera line interrupt.
         */
        uint32_t cam_hs_int_raw:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lcdcam_lc_dma_int_raw_reg_t;

/** Type of lc_dma_int_st register
 *  LCDCAM interrupt status register.
 */
typedef union {
    struct {
        /** lcd_vsync_int_st : RO; bitpos: [0]; default: 0;
         *  The status bit for LCD frame end interrupt.
         */
        uint32_t lcd_vsync_int_st:1;
        /** lcd_trans_done_int_st : RO; bitpos: [1]; default: 0;
         *  The status bit for lcd transfer end interrupt.
         */
        uint32_t lcd_trans_done_int_st:1;
        /** cam_vsync_int_st : RO; bitpos: [2]; default: 0;
         *  The status bit for Camera frame end interrupt.
         */
        uint32_t cam_vsync_int_st:1;
        /** cam_hs_int_st : RO; bitpos: [3]; default: 0;
         *  The status bit for Camera transfer end interrupt.
         */
        uint32_t cam_hs_int_st:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lcdcam_lc_dma_int_st_reg_t;

/** Type of lc_dma_int_clr register
 *  LCDCAM interrupt clear register.
 */
typedef union {
    struct {
        /** lcd_vsync_int_clr : WT; bitpos: [0]; default: 0;
         *  The clear bit for LCD frame end interrupt.
         */
        uint32_t lcd_vsync_int_clr:1;
        /** lcd_trans_done_int_clr : WT; bitpos: [1]; default: 0;
         *  The clear bit for lcd transfer end interrupt.
         */
        uint32_t lcd_trans_done_int_clr:1;
        /** cam_vsync_int_clr : WT; bitpos: [2]; default: 0;
         *  The clear bit for Camera frame end interrupt.
         */
        uint32_t cam_vsync_int_clr:1;
        /** cam_hs_int_clr : WT; bitpos: [3]; default: 0;
         *  The clear bit for Camera line interrupt.
         */
        uint32_t cam_hs_int_clr:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lcdcam_lc_dma_int_clr_reg_t;


/** Group: Version register */
/** Type of lc_reg_date register
 *  Version register
 */
typedef union {
    struct {
        /** lc_date : R/W; bitpos: [27:0]; default: 36712592;
         *  LCD_CAM version control register
         */
        uint32_t lc_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lcdcam_lc_reg_date_reg_t;


typedef struct lcdcam_dev_t {
    volatile lcdcam_lcd_clock_reg_t lcd_clock;
    volatile lcdcam_cam_ctrl_reg_t cam_ctrl;
    volatile lcdcam_cam_ctrl1_reg_t cam_ctrl1;
    volatile lcdcam_cam_rgb_yuv_reg_t cam_rgb_yuv;
    volatile lcdcam_lcd_rgb_yuv_reg_t lcd_rgb_yuv;
    volatile lcdcam_lcd_user_reg_t lcd_user;
    volatile lcdcam_lcd_misc_reg_t lcd_misc;
    volatile lcdcam_lcd_ctrl_reg_t lcd_ctrl;
    volatile lcdcam_lcd_ctrl1_reg_t lcd_ctrl1;
    volatile lcdcam_lcd_ctrl2_reg_t lcd_ctrl2;
    volatile lcdcam_lcd_first_cmd_val_reg_t lcd_first_cmd_val;
    volatile lcdcam_lcd_latter_cmd_val_reg_t lcd_latter_cmd_val;
    volatile lcdcam_lcd_dly_mode_cfg1_reg_t lcd_dly_mode_cfg1;
    uint32_t reserved_034;
    volatile lcdcam_lcd_dly_mode_cfg2_reg_t lcd_dly_mode_cfg2;
    uint32_t reserved_03c[10];
    volatile lcdcam_lc_dma_int_ena_reg_t lc_dma_int_ena;
    volatile lcdcam_lc_dma_int_raw_reg_t lc_dma_int_raw;
    volatile lcdcam_lc_dma_int_st_reg_t lc_dma_int_st;
    volatile lcdcam_lc_dma_int_clr_reg_t lc_dma_int_clr;
    uint32_t reserved_074[34];
    volatile lcdcam_lc_reg_date_reg_t lc_reg_date;
} lcdcam_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lcdcam_dev_t) == 0x100, "Invalid size of lcdcam_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
