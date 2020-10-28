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

#include <stdint.h>

typedef volatile struct {
    union {
        struct {
            uint32_t lcd_clkcnt_n:       6;            /*f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1) when reg_clk_equ_sysclk is 0.*/
            uint32_t lcd_clk_equ_sysclk: 1;            /*1: f_LCD_PCLK = f_LCD_CLK. 0: f_LCD_PCLK = f_LCD_CLK / (reg_clkcnt_N + 1).*/
            uint32_t lcd_ck_idle_edge:   1;            /*1: LCD_PCLK line is high when idle     0: LCD_PCLK line is low when idle.*/
            uint32_t lcd_ck_out_edge:    1;
            uint32_t lcd_clkm_div_num:   8;            /*Integral LCD clock divider value*/
            uint32_t lcd_clkm_div_b:     6;            /*Fractional clock divider numerator value*/
            uint32_t lcd_clkm_div_a:     6;            /*Fractional clock divider denominator value*/
            uint32_t lcd_clk_sel:        2;            /*Select LCD module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.*/
            uint32_t clk_en:             1;            /*Set this bit to enable clk gate*/
        };
        uint32_t val;
    } lcd_clock;
    union {
        struct {
            uint32_t cam_stop_en:            1;        /*Camera stop enable signal  1: camera stops when DMA Rx FIFO is full. 0: Not stop.*/
            uint32_t cam_vsync_filter_thres: 3;        /*Filter threshold value for CAM_VSYNC signal.*/
            uint32_t cam_update:             1;        /*1: Update Camera registers  will be cleared by hardware. 0 : Not care.*/
            uint32_t cam_byte_order:         1;        /*1: Change data bit order  change CAM_DATA_in[7:0] to CAM_DATA_in[0:7] in one byte mode  and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.*/
            uint32_t cam_bit_order:          1;        /*1: invert data byte order  only valid in 2 byte mode. 0: Not change.*/
            uint32_t cam_line_int_en:        1;        /*1: Enable to generate CAM_HS_INT. 0: Disable.*/
            uint32_t cam_vs_eof_en:          1;        /*1: CAM_VSYNC to generate in_suc_eof. 0: in_suc_eof is controlled by reg_cam_rec_data_cyclelen.*/
            uint32_t cam_clkm_div_num:       8;        /*Integral Camera clock divider value*/
            uint32_t cam_clkm_div_b:         6;        /*Fractional clock divider numerator value*/
            uint32_t cam_clkm_div_a:         6;        /*Fractional clock divider denominator value*/
            uint32_t cam_clk_sel:            2;        /*Select Camera module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: no clock.*/
            uint32_t reserved31:             1;        /*reserved*/
        };
        uint32_t val;
    } cam_ctrl;
    union {
        struct {
            uint32_t cam_rec_data_bytelen: 14;         /*Camera receive data byte length minus 1 to set DMA in_suc_eof_int.*/
            uint32_t cam_line_int_num:     7;          /*The line number minus 1 to generate cam_hs_int.*/
            uint32_t cam_clk_inv:          1;          /*1: Invert  the input signal CAM_PCLK. 0: Not invert.*/
            uint32_t reserved22:           1;
            uint32_t cam_vsync_filter_en:  1;          /*1: Enable CAM_VSYNC filter function. 0: bypass.*/
            uint32_t cam_2byte_en:         1;          /*1: The bit number of input data is 9~16.  0: The bit number of input data is 0~8.*/
            uint32_t cam_de_inv:           1;          /*CAM_DE invert enable signal  valid in high level.*/
            uint32_t cam_hsync_inv:        1;          /*CAM_HSYNC invert enable signal  valid in high level.*/
            uint32_t cam_vsync_inv:        1;          /*CAM_VSYNC invert enable signal  valid in high level.*/
            uint32_t cam_vh_de_mode_en:    1;          /*1: Input control signals are CAM_DE CAM_HSYNC and CAM_VSYNC is 1. 0: Input control signals are CAM_DE and CAM_VSYNC*/
            uint32_t cam_start:            1;          /*Camera module start signal.*/
            uint32_t cam_reset:            1;          /*Camera module reset signal.*/
            uint32_t cam_afifo_reset:      1;          /*Camera AFIFO reset signal.*/
        };
        uint32_t val;
    } cam_ctrl1;
    union {
        struct {
            uint32_t reserved0:              21;       /*reserved*/
            uint32_t cam_conv_8bits_data_inv: 1;       /*1:invert every two 8bits input data. 2. disabled.*/
            uint32_t cam_conv_yuv2yuv_mode:   2;       /*0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable yuv2yuv mode  trans_mode must be set to 1.*/
            uint32_t cam_conv_yuv_mode:       2;       /*0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode  yuv_mode decides the yuv mode of Data_in*/
            uint32_t cam_conv_protocol_mode:  1;       /*0:BT601. 1:BT709.*/
            uint32_t cam_conv_data_out_mode:  1;       /*LIMIT or FULL mode of Data out. 0: limit. 1: full*/
            uint32_t cam_conv_data_in_mode:   1;       /*LIMIT or FULL mode of Data in. 0: limit. 1: full*/
            uint32_t cam_conv_mode_8bits_on:  1;       /*0: 16bits mode. 1: 8bits mode.*/
            uint32_t cam_conv_trans_mode:     1;       /*0: YUV to RGB. 1: RGB to YUV.*/
            uint32_t cam_conv_bypass:         1;       /*0: Bypass converter. 1: Enable converter.*/
        };
        uint32_t val;
    } cam_rgb_yuv;
    union {
        struct {
            uint32_t reserved0:              20;       /*reserved*/
            uint32_t lcd_conv_8bits_data_inv: 1;       /*1:invert every two 8bits input data. 2. disabled.*/
            uint32_t lcd_conv_txtorx:         1;       /*0: txtorx mode off. 1: txtorx mode on.*/
            uint32_t lcd_conv_yuv2yuv_mode:   2;       /*0: to yuv422. 1: to yuv420. 2: to yuv411. 3: disabled.  To enable yuv2yuv mode  trans_mode must be set to 1.*/
            uint32_t lcd_conv_yuv_mode:       2;       /*0: yuv422. 1: yuv420. 2: yuv411. When in yuv2yuv mode  yuv_mode decides the yuv mode of Data_in*/
            uint32_t lcd_conv_protocol_mode:  1;       /*0:BT601. 1:BT709.*/
            uint32_t lcd_conv_data_out_mode:  1;       /*LIMIT or FULL mode of Data out. 0: limit. 1: full*/
            uint32_t lcd_conv_data_in_mode:   1;       /*LIMIT or FULL mode of Data in. 0: limit. 1: full*/
            uint32_t lcd_conv_mode_8bits_on:  1;       /*0: 16bits mode. 1: 8bits mode.*/
            uint32_t lcd_conv_trans_mode:     1;       /*0: YUV to RGB. 1: RGB to YUV.*/
            uint32_t lcd_conv_bypass:         1;       /*0: Bypass converter. 1: Enable converter.*/
        };
        uint32_t val;
    } lcd_rgb_yuv;
    union {
        struct {
            uint32_t lcd_dout_cyclelen: 13;            /*The output data cycles minus 1 of LCD module.*/
            uint32_t lcd_always_out_en:  1;            /*LCD always output when LCD is in LCD_DOUT state  unless reg_lcd_start is cleared or reg_lcd_reset is set.*/
            uint32_t reserved14:         5;            /*reserved*/
            uint32_t lcd_8bits_order:    1;            /*1: invert every two data byte  valid in 1 byte mode. 0: Not change.*/
            uint32_t lcd_update:         1;            /*1: Update LCD registers  will be cleared by hardware. 0 : Not care.*/
            uint32_t lcd_bit_order:      1;            /*1: Change data bit order  change LCD_DATA_out[7:0] to LCD_DATA_out[0:7] in one byte mode  and bits[15:0] to bits[0:15] in two byte mode.  0: Not change.*/
            uint32_t lcd_byte_order:     1;            /*1: invert data byte order  only valid in 2 byte mode. 0: Not change.*/
            uint32_t lcd_2byte_en:       1;            /*1: The bit number of output LCD data is 9~16.  0: The bit number of output LCD data is 0~8.*/
            uint32_t lcd_dout:           1;            /*1: Be able to send data out in LCD sequence when LCD starts. 0: Disable.*/
            uint32_t lcd_dummy:          1;            /*1: Enable DUMMY phase in LCD sequence when LCD starts. 0: Disable.*/
            uint32_t lcd_cmd:            1;            /*1: Be able to send command in LCD sequence when LCD starts. 0: Disable.*/
            uint32_t lcd_start:          1;            /*LCD start sending data enable signal  valid in high level.*/
            uint32_t lcd_reset:          1;            /*The value of  command.*/
            uint32_t lcd_dummy_cyclelen: 2;            /*The dummy cycle length minus 1.*/
            uint32_t lcd_cmd_2_cycle_en: 1;            /*The cycle length of command phase*/
        };
        uint32_t val;
    } lcd_user;
    union {
        struct {
            uint32_t reserved0:               1;       /*reserved*/
            uint32_t lcd_afifo_threshold_num: 5;       /*The awfull threshold number of lcd_afifo.*/
            uint32_t lcd_vfk_cyclelen:        6;       /*The setup cycle length minus 1 in LCD non-RGB mode.*/
            uint32_t lcd_vbk_cyclelen:       13;       /*The vertical back blank region cycle length minus 1 in LCD RGB mode  or the hold time cycle length in LCD non-RGB mode.*/
            uint32_t lcd_next_frame_en:       1;       /*1: Send the next frame data when the current frame is sent out. 0: LCD stops when the current frame is sent out.*/
            uint32_t lcd_bk_en:               1;       /*1: Enable blank region when LCD sends data out. 0: No blank region.*/
            uint32_t lcd_afifo_reset:         1;       /*LCD AFIFO reset signal.*/
            uint32_t lcd_cd_data_set:         1;       /*1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DOUT state.  0: LCD_CD = reg_cd_idle_edge.*/
            uint32_t lcd_cd_dummy_set:        1;       /*1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_DUMMY state.  0: LCD_CD = reg_cd_idle_edge.*/
            uint32_t lcd_cd_cmd_set:          1;       /*1: LCD_CD = !reg_cd_idle_edge when lcd_st[2:0] is in LCD_CMD state.  0: LCD_CD = reg_cd_idle_edge.*/
            uint32_t lcd_cd_idle_edge:        1;       /*The default value of LCD_CD.*/
        };
        uint32_t val;
    } lcd_misc;
    union {
        struct {
            uint32_t lcd_hb_front:   11;               /*It is the horizontal blank front porch of a frame.*/
            uint32_t lcd_va_height:  10;               /*It is the vertical active height of a frame.*/
            uint32_t lcd_vt_height:  10;               /*It is the vertical total height of a frame.*/
            uint32_t lcd_rgb_mode_en: 1;               /*1: Enable reg mode input vsync*/
        };
        uint32_t val;
    } lcd_ctrl;
    union {
        struct {
            uint32_t lcd_vb_front: 8;                  /*It is the vertical blank front porch of a frame.*/
            uint32_t lcd_ha_width: 12;                 /*It is the horizontal active width of a frame.*/
            uint32_t lcd_ht_width: 12;                 /*It is the horizontal total width of a frame.*/
        };
        uint32_t val;
    } lcd_ctrl1;
    union {
        struct {
            uint32_t lcd_vsync_width:    7;            /*It is the position of LCD_VSYNC active pulse in a line.*/
            uint32_t lcd_vsync_idle_pol: 1;            /*It is the idle value of LCD_VSYNC.*/
            uint32_t lcd_de_idle_pol:    1;            /*It is the idle value of LCD_DE.*/
            uint32_t lcd_hs_blank_en:    1;            /*1: The pulse of LCD_HSYNC is out in vertical blanking lines RGB mode. 0: LCD_HSYNC pulse is valid only in active region lines in RGB mode.*/
            uint32_t reserved10:         6;            /*reserved*/
            uint32_t lcd_hsync_width:    7;            /*It is the position of LCD_HSYNC active pulse in a line.*/
            uint32_t lcd_hsync_idle_pol: 1;            /*It is the idle value of LCD_HSYNC.*/
            uint32_t lcd_hsync_position: 8;            /*It is the position of LCD_HSYNC active pulse in a line.*/
        };
        uint32_t val;
    } lcd_ctrl2;
    uint32_t lcd_cmd_val;                              /*The LCD write command value.*/
    uint32_t reserved_2c;
    union {
        struct {
            uint32_t lcd_cd_mode:    2;                /*The output LCD_CD is delayed by module clock LCD_CLK*/
            uint32_t lcd_de_mode:    2;                /*The output LCD_DE is delayed by module clock LCD_CLK*/
            uint32_t lcd_hsync_mode: 2;                /*The output LCD_HSYNC is delayed by module clock LCD_CLK*/
            uint32_t lcd_vsync_mode: 2;                /*The output LCD_VSYNC is delayed by module clock LCD_CLK*/
            uint32_t reserved8:     24;                /*reserved*/
        };
        uint32_t val;
    } lcd_dly_mode;
    uint32_t reserved_34;
    union {
        struct {
            uint32_t dout0_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout1_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout2_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout3_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout4_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout5_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout6_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout7_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout8_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout9_mode:  2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout10_mode: 2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout11_mode: 2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout12_mode: 2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout13_mode: 2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout14_mode: 2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
            uint32_t dout15_mode: 2;                   /*The output data bit $n is delayed by module clock LCD_CLK*/
        };
        uint32_t val;
    } lcd_data_dout_mode;
    uint32_t reserved_3c;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    union {
        struct {
            uint32_t lcd_vsync:              1;        /*The enable bit for LCD frame end interrupt.*/
            uint32_t lcd_trans_done:         1;        /*The enable bit for lcd transfer end interrupt.*/
            uint32_t cam_vsync:              1;        /*The enable bit for Camera frame end interrupt.*/
            uint32_t cam_hs:                 1;        /*The enable bit for Camera line interrupt.*/
            uint32_t reserved4:             28;        /*reserved*/
        };
        uint32_t val;
    } lc_dma_int_ena;
    union {
        struct {
            uint32_t lcd_vsync:              1;        /*The raw bit for LCD frame end interrupt.*/
            uint32_t lcd_trans_done:         1;        /*The raw bit for lcd transfer end interrupt.*/
            uint32_t cam_vsync:              1;        /*The raw bit for Camera frame end interrupt.*/
            uint32_t cam_hs:                 1;        /*The raw bit for Camera line interrupt.*/
            uint32_t reserved4:             28;        /*reserved*/
        };
        uint32_t val;
    } lc_dma_int_raw;
    union {
        struct {
            uint32_t lcd_vsync:             1;         /*The status bit for LCD frame end interrupt.*/
            uint32_t lcd_trans_done:        1;         /*The status bit for lcd transfer end interrupt.*/
            uint32_t cam_vsync:             1;         /*The status bit for Camera frame end interrupt.*/
            uint32_t cam_hs:                1;         /*The status bit for Camera transfer end interrupt.*/
            uint32_t reserved4:            28;         /*reserved*/
        };
        uint32_t val;
    } lc_dma_int_st;
    union {
        struct {
            uint32_t lcd_vsync:              1;        /*The clear bit for LCD frame end interrupt.*/
            uint32_t lcd_trans_done:         1;        /*The clear bit for lcd transfer end interrupt.*/
            uint32_t cam_vsync:              1;        /*The clear bit for Camera frame end interrupt.*/
            uint32_t cam_hs:                 1;        /*The clear bit for Camera line interrupt.*/
            uint32_t reserved4:             28;        /*reserved*/
        };
        uint32_t val;
    } lc_dma_int_clr;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    union {
        struct {
            uint32_t lc_date:   28;                    /*LCD_CAM version control register*/
            uint32_t reserved28: 4;                    /*reserved*/
        };
        uint32_t val;
    } lc_date;
} lcd_cam_dev_t;

extern lcd_cam_dev_t LCD_CAM;

#ifdef __cplusplus
}
#endif
