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

/** Group: Configuration Registers */
/** Type of blend0_clut_data register
 *  CLUT sram data read/write register in background plane of blender
 */
typedef union {
    struct {
        /** rdwr_word_blend0_clut : R/W; bitpos: [31:0]; default: 0;
         *  Write and read data to/from CLUT RAM in background plane of blender engine through
         *  this field in fifo mode.
         */
        uint32_t rdwr_word_blend0_clut:32;
    };
    uint32_t val;
} ppa_blend0_clut_data_reg_t;

/** Type of blend1_clut_data register
 *  CLUT sram data read/write register in foreground plane of blender
 */
typedef union {
    struct {
        /** rdwr_word_blend1_clut : R/W; bitpos: [31:0]; default: 0;
         *  Write and read data to/from CLUT RAM in foreground plane of blender engine through
         *  this field in fifo mode.
         */
        uint32_t rdwr_word_blend1_clut:32;
    };
    uint32_t val;
} ppa_blend1_clut_data_reg_t;

/** Type of clut_conf register
 *  CLUT configure register
 */
typedef union {
    struct {
        /** apb_fifo_mask : R/W; bitpos: [0]; default: 0;
         *  1'b0: fifo mode to wr/rd clut0/clut1 RAM through register
         *  PPA_SR_CLUT_DATA_REG/PPA_BLEND0_CLUT_DATA_REG/PPA_BLEND1_CLUT_DATA_REG. 1'b1:
         *  memory mode to wr/rd sr/blend0/blend1 clut RAM. The bit 11 and 10 of the waddr
         *  should be 01 to access sr clut and should be 10 to access blend0 clut and should be
         *  11 to access blend 1 clut in memory mode.
         */
        uint32_t apb_fifo_mask:1;
        /** blend0_clut_mem_rst : R/W; bitpos: [1]; default: 0;
         *  Write 1 then write 0 to this bit to reset BLEND0 CLUT.
         */
        uint32_t blend0_clut_mem_rst:1;
        /** blend1_clut_mem_rst : R/W; bitpos: [2]; default: 0;
         *  Write 1 then write 0 to this bit to reset BLEND1 CLUT.
         */
        uint32_t blend1_clut_mem_rst:1;
        /** blend0_clut_mem_rdaddr_rst : R/W; bitpos: [3]; default: 0;
         *  Write 1 then write 0 to reset the read address of BLEND0 CLUT in fifo mode.
         */
        uint32_t blend0_clut_mem_rdaddr_rst:1;
        /** blend1_clut_mem_rdaddr_rst : R/W; bitpos: [4]; default: 0;
         *  Write 1 then write 0 to reset the read address of BLEND1 CLUT in fifo mode.
         */
        uint32_t blend1_clut_mem_rdaddr_rst:1;
        /** blend0_clut_mem_force_pd : R/W; bitpos: [5]; default: 0;
         *  1: force power down BLEND CLUT memory.
         */
        uint32_t blend0_clut_mem_force_pd:1;
        /** blend0_clut_mem_force_pu : R/W; bitpos: [6]; default: 0;
         *  1: force power up BLEND CLUT memory.
         */
        uint32_t blend0_clut_mem_force_pu:1;
        /** blend0_clut_mem_clk_ena : R/W; bitpos: [7]; default: 0;
         *  1: Force clock on for BLEND CLUT memory.
         */
        uint32_t blend0_clut_mem_clk_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ppa_clut_conf_reg_t;

/** Type of sr_color_mode register
 *  Scaling and rotating engine color mode register
 */
typedef union {
    struct {
        /** sr_rx_cm : R/W; bitpos: [3:0]; default: 0;
         *  The source image color mode for Scaling and Rotating engine Rx. 0: ARGB8888. 1:
         *  RGB888. 2: RGB565. 8: YUV420. others: Reserved.
         */
        uint32_t sr_rx_cm:4;
        /** sr_tx_cm : R/W; bitpos: [7:4]; default: 0;
         *  The destination image color mode for Scaling and Rotating engine Tx. 0: ARGB8888.
         *  1: RGB888. 2: RGB565. 8: YUV420. others: Reserved.
         */
        uint32_t sr_tx_cm:4;
        /** yuv_rx_range : R/W; bitpos: [8]; default: 0;
         *  YUV input range when reg_sr_rx_cm is 4'd8. 0: limit range. 1: full range
         */
        uint32_t yuv_rx_range:1;
        /** yuv_tx_range : R/W; bitpos: [9]; default: 0;
         *  YUV output range when reg_sr_tx_cm is 4'd8. 0: limit range. 1: full range
         */
        uint32_t yuv_tx_range:1;
        /** yuv2rgb_protocal : R/W; bitpos: [10]; default: 0;
         *  YUV to RGB protocal when reg_sr_rx_cm is 4'd8. 0: BT601. 1: BT709
         */
        uint32_t yuv2rgb_protocal:1;
        /** rgb2yuv_protocal : R/W; bitpos: [11]; default: 0;
         *  RGB to YUV protocal when reg_sr_tx_cm is 4'd8. 0: BT601. 1: BT709
         */
        uint32_t rgb2yuv_protocal:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} ppa_sr_color_mode_reg_t;

/** Type of blend_color_mode register
 *  blending engine color mode register
 */
typedef union {
    struct {
        /** blend0_rx_cm : R/W; bitpos: [3:0]; default: 0;
         *  The source image color mode for background plane. 0: ARGB8888. 1: RGB888. 2:
         *  RGB565. 3: Reserved. 4: L8. 5: L4.
         */
        uint32_t blend0_rx_cm:4;
        /** blend1_rx_cm : R/W; bitpos: [7:4]; default: 0;
         *  The source image color mode for foreground plane. 0: ARGB8888. 1: RGB888. 2:
         *  RGB565. 3: Reserved. 4: L8. 5: L4. 6: A8. 7: A4.
         */
        uint32_t blend1_rx_cm:4;
        /** blend_tx_cm : R/W; bitpos: [11:8]; default: 0;
         *  The destination image color mode for output of blender. 0: ARGB8888. 1: RGB888. 2:
         *  RGB565. 3: Reserved..
         */
        uint32_t blend_tx_cm:4;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} ppa_blend_color_mode_reg_t;

/** Type of sr_byte_order register
 *  Scaling and rotating engine byte order register
 */
typedef union {
    struct {
        /** sr_rx_byte_swap_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1  the data into Rx channel 0 would be swapped in byte. The Byte0
         *  and Byte1 would be swapped while byte 2 and byte 3 would be swappped.
         */
        uint32_t sr_rx_byte_swap_en:1;
        /** sr_rx_rgb_swap_en : R/W; bitpos: [1]; default: 0;
         *  Set this bit to 1  the data into Rx channel 0 would be swapped in rgb. It means rgb
         *  would be swap to bgr.
         */
        uint32_t sr_rx_rgb_swap_en:1;
        /** sr_macro_bk_ro_bypass : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to bypass the macro block order function. This function is used
         *  to improve efficient accessing external memory.
         */
        uint32_t sr_macro_bk_ro_bypass:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ppa_sr_byte_order_reg_t;

/** Type of blend_byte_order register
 *  Blending engine byte order register
 */
typedef union {
    struct {
        /** blend0_rx_byte_swap_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1  the data into Rx channel 0 would be swapped in byte. The Byte0
         *  and Byte1 would be swapped while byte 2 and byte 3 would be swappped.
         */
        uint32_t blend0_rx_byte_swap_en:1;
        /** blend1_rx_byte_swap_en : R/W; bitpos: [1]; default: 0;
         *  Set this bit to 1  the data into Rx channel 0 would be swapped in byte. The Byte0
         *  and Byte1 would be swapped while byte 2 and byte 3 would be swappped.
         */
        uint32_t blend1_rx_byte_swap_en:1;
        /** blend0_rx_rgb_swap_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1  the data into Rx channel 0 would be swapped in rgb. It means rgb
         *  would be swap to bgr.
         */
        uint32_t blend0_rx_rgb_swap_en:1;
        /** blend1_rx_rgb_swap_en : R/W; bitpos: [3]; default: 0;
         *  Set this bit to 1  the data into Rx channel 0 would be swapped in rgb. It means rgb
         *  would be swap to bgr.
         */
        uint32_t blend1_rx_rgb_swap_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ppa_blend_byte_order_reg_t;

/** Type of blend_trans_mode register
 *  Blending engine mode configure register
 */
typedef union {
    struct {
        /** blend_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable alpha blending.
         */
        uint32_t blend_en:1;
        /** blend_bypass : R/W; bitpos: [1]; default: 0;
         *  Set this bit to bypass blender. Then background date would be output.
         */
        uint32_t blend_bypass:1;
        /** blend_fix_pixel_fill_en : R/W; bitpos: [2]; default: 0;
         *  This bit is used to enable fix pixel filling. When this mode is enable  only Tx
         *  channel is work and the output pixel is configured by PPA_OUT_FIX_PIXEL.
         */
        uint32_t blend_fix_pixel_fill_en:1;
        /** blend_trans_mode_update : WT; bitpos: [3]; default: 0;
         *  Set this bit to update the transfer mode. Only the bit is set  the transfer mode is
         *  valid.
         */
        uint32_t blend_trans_mode_update:1;
        /** blend_rst : R/W; bitpos: [4]; default: 0;
         *  write 1 then write 0 to reset blending engine.
         */
        uint32_t blend_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} ppa_blend_trans_mode_reg_t;

/** Type of sr_fix_alpha register
 *  Scaling and rotating engine alpha override register
 */
typedef union {
    struct {
        /** sr_rx_fix_alpha : R/W; bitpos: [7:0]; default: 128;
         *  The value would replace the alpha value in received pixel for Scaling and Rotating
         *  engine when PPA_SR_RX_ALPHA_CONF_EN is enabled.
         */
        uint32_t sr_rx_fix_alpha:8;
        /** sr_rx_alpha_mod : R/W; bitpos: [9:8]; default: 0;
         *  Alpha mode. 0/3: not replace alpha. 1: replace alpha with PPA_SR_FIX_ALPHA. 2:
         *  Original alpha multiply with PPA_SR_FIX_ALPHA/256.
         */
        uint32_t sr_rx_alpha_mod:2;
        /** sr_rx_alpha_inv : R/W; bitpos: [10]; default: 0;
         *  Set this bit to invert the original alpha value. When RX color mode is
         *  RGB565/RGB88. The original alpha value is 255.
         */
        uint32_t sr_rx_alpha_inv:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} ppa_sr_fix_alpha_reg_t;

/** Type of blend_tx_size register
 *  Fix pixel filling mode image size register
 */
typedef union {
    struct {
        /** blend_hb : R/W; bitpos: [13:0]; default: 0;
         *  The horizontal width of image block that would be filled in fix pixel filling mode.
         *  The unit is pixel
         */
        uint32_t blend_hb:14;
        /** blend_vb : R/W; bitpos: [27:14]; default: 0;
         *  The vertical width of image block that would be filled in fix pixel filling mode.
         *  The unit is pixel
         */
        uint32_t blend_vb:14;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ppa_blend_tx_size_reg_t;

/** Type of blend_fix_alpha register
 *  Blending engine alpha override register
 */
typedef union {
    struct {
        /** blend0_rx_fix_alpha : R/W; bitpos: [7:0]; default: 128;
         *  The value would replace the alpha value in received pixel for background plane of
         *  blender when PPA_BLEND0_RX_ALPHA_CONF_EN is enabled.
         */
        uint32_t blend0_rx_fix_alpha:8;
        /** blend1_rx_fix_alpha : R/W; bitpos: [15:8]; default: 128;
         *  The value would replace the alpha value in received pixel for foreground plane of
         *  blender when PPA_BLEND1_RX_ALPHA_CONF_EN is enabled.
         */
        uint32_t blend1_rx_fix_alpha:8;
        /** blend0_rx_alpha_mod : R/W; bitpos: [17:16]; default: 0;
         *  Alpha mode. 0/3: not replace alpha. 1: replace alpha with PPA_SR_FIX_ALPHA. 2:
         *  Original alpha multiply with PPA_SR_FIX_ALPHA/256.
         */
        uint32_t blend0_rx_alpha_mod:2;
        /** blend1_rx_alpha_mod : R/W; bitpos: [19:18]; default: 0;
         *  Alpha mode. 0/3: not replace alpha. 1: replace alpha with PPA_SR_FIX_ALPHA. 2:
         *  Original alpha multiply with PPA_SR_FIX_ALPHA/256.
         */
        uint32_t blend1_rx_alpha_mod:2;
        /** blend0_rx_alpha_inv : R/W; bitpos: [20]; default: 0;
         *  Set this bit to invert the original alpha value. When RX color mode is
         *  RGB565/RGB88. The original alpha value is 255.
         */
        uint32_t blend0_rx_alpha_inv:1;
        /** blend1_rx_alpha_inv : R/W; bitpos: [21]; default: 0;
         *  Set this bit to invert the original alpha value. When RX color mode is
         *  RGB565/RGB88. The original alpha value is 255.
         */
        uint32_t blend1_rx_alpha_inv:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} ppa_blend_fix_alpha_reg_t;

/** Type of blend_rgb register
 *  RGB color register
 */
typedef union {
    struct {
        /** blend1_rx_b : R/W; bitpos: [7:0]; default: 128;
         *  blue color for A4/A8 mode.
         */
        uint32_t blend1_rx_b:8;
        /** blend1_rx_g : R/W; bitpos: [15:8]; default: 128;
         *  green color for A4/A8 mode.
         */
        uint32_t blend1_rx_g:8;
        /** blend1_rx_r : R/W; bitpos: [23:16]; default: 128;
         *  red color for A4/A8 mode.
         */
        uint32_t blend1_rx_r:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ppa_blend_rgb_reg_t;

/** Type of blend_fix_pixel register
 *  Blending engine fix pixel register
 */
typedef union {
    struct {
        /** blend_tx_fix_pixel : R/W; bitpos: [31:0]; default: 0;
         *  The configure fix pixel in fix pixel filling mode for blender engine.
         */
        uint32_t blend_tx_fix_pixel:32;
    };
    uint32_t val;
} ppa_blend_fix_pixel_reg_t;

/** Type of ck_fg_low register
 *  foreground color key lower threshold
 */
typedef union {
    struct {
        /** colorkey_fg_b_low : R/W; bitpos: [7:0]; default: 255;
         *  color key lower threshold of foreground b channel
         */
        uint32_t colorkey_fg_b_low:8;
        /** colorkey_fg_g_low : R/W; bitpos: [15:8]; default: 255;
         *  color key lower threshold of foreground g channel
         */
        uint32_t colorkey_fg_g_low:8;
        /** colorkey_fg_r_low : R/W; bitpos: [23:16]; default: 255;
         *  color key lower threshold of foreground r channel
         */
        uint32_t colorkey_fg_r_low:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ppa_ck_fg_low_reg_t;

/** Type of ck_fg_high register
 *  foreground color key higher threshold
 */
typedef union {
    struct {
        /** colorkey_fg_b_high : R/W; bitpos: [7:0]; default: 0;
         *  color key higher threshold of foreground b channel
         */
        uint32_t colorkey_fg_b_high:8;
        /** colorkey_fg_g_high : R/W; bitpos: [15:8]; default: 0;
         *  color key higher threshold of foreground g channel
         */
        uint32_t colorkey_fg_g_high:8;
        /** colorkey_fg_r_high : R/W; bitpos: [23:16]; default: 0;
         *  color key higher threshold of foreground r channel
         */
        uint32_t colorkey_fg_r_high:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ppa_ck_fg_high_reg_t;

/** Type of ck_bg_low register
 *  background color key lower threshold
 */
typedef union {
    struct {
        /** colorkey_bg_b_low : R/W; bitpos: [7:0]; default: 255;
         *  color key lower threshold of background b channel
         */
        uint32_t colorkey_bg_b_low:8;
        /** colorkey_bg_g_low : R/W; bitpos: [15:8]; default: 255;
         *  color key lower threshold of background g channel
         */
        uint32_t colorkey_bg_g_low:8;
        /** colorkey_bg_r_low : R/W; bitpos: [23:16]; default: 255;
         *  color key lower threshold of background r channel
         */
        uint32_t colorkey_bg_r_low:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ppa_ck_bg_low_reg_t;

/** Type of ck_bg_high register
 *  background color key higher threshold
 */
typedef union {
    struct {
        /** colorkey_bg_b_high : R/W; bitpos: [7:0]; default: 0;
         *  color key higher threshold of background b channel
         */
        uint32_t colorkey_bg_b_high:8;
        /** colorkey_bg_g_high : R/W; bitpos: [15:8]; default: 0;
         *  color key higher threshold of background g channel
         */
        uint32_t colorkey_bg_g_high:8;
        /** colorkey_bg_r_high : R/W; bitpos: [23:16]; default: 0;
         *  color key higher threshold of background r channel
         */
        uint32_t colorkey_bg_r_high:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ppa_ck_bg_high_reg_t;

/** Type of ck_default register
 *  default value when foreground and background both in color key range
 */
typedef union {
    struct {
        /** colorkey_default_b : R/W; bitpos: [7:0]; default: 0;
         *  default B channle value of color key
         */
        uint32_t colorkey_default_b:8;
        /** colorkey_default_g : R/W; bitpos: [15:8]; default: 0;
         *  default G channle value of color key
         */
        uint32_t colorkey_default_g:8;
        /** colorkey_default_r : R/W; bitpos: [23:16]; default: 0;
         *  default R channle value of color key
         */
        uint32_t colorkey_default_r:8;
        /** colorkey_fg_bg_reverse : R/W; bitpos: [24]; default: 0;
         *  when pixel in bg ck range but not in fg ck range, 0: the result is bg, 1: the
         *  result is fg
         */
        uint32_t colorkey_fg_bg_reverse:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} ppa_ck_default_reg_t;

/** Type of sr_scal_rotate register
 *  Scaling and rotating coefficient register
 */
typedef union {
    struct {
        /** sr_scal_x_int : R/W; bitpos: [7:0]; default: 1;
         *  The integrated part of scaling coefficient in X direction.
         */
        uint32_t sr_scal_x_int:8;
        /** sr_scal_x_frag : R/W; bitpos: [11:8]; default: 0;
         *  The fragment part of scaling coefficient in X direction.
         */
        uint32_t sr_scal_x_frag:4;
        /** sr_scal_y_int : R/W; bitpos: [19:12]; default: 1;
         *  The integrated part of scaling coefficient in Y direction.
         */
        uint32_t sr_scal_y_int:8;
        /** sr_scal_y_frag : R/W; bitpos: [23:20]; default: 0;
         *  The fragment part of scaling coefficient in Y direction.
         */
        uint32_t sr_scal_y_frag:4;
        /** sr_rotate_angle : R/W; bitpos: [25:24]; default: 0;
         *  The rotate angle. 0: 0 degree. 1: 90 degree. 2: 180 degree. 3: 270 degree.
         */
        uint32_t sr_rotate_angle:2;
        /** scal_rotate_rst : R/W; bitpos: [26]; default: 0;
         *  Write 1 then write 0 to this bit to reset scaling and rotating engine.
         */
        uint32_t scal_rotate_rst:1;
        /** scal_rotate_start : WT; bitpos: [27]; default: 0;
         *  Write 1 to enable scaling and rotating engine after parameter is configured.
         */
        uint32_t scal_rotate_start:1;
        /** sr_mirror_x : R/W; bitpos: [28]; default: 0;
         *  Image mirror in X direction. 0: disable, 1: enable
         */
        uint32_t sr_mirror_x:1;
        /** sr_mirror_y : R/W; bitpos: [29]; default: 0;
         *  Image mirror in Y direction. 0: disable, 1: enable
         */
        uint32_t sr_mirror_y:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} ppa_sr_scal_rotate_reg_t;

/** Type of sr_mem_pd register
 *  SR memory power done register
 */
typedef union {
    struct {
        /** sr_mem_clk_ena : R/W; bitpos: [0]; default: 0;
         *  Set this bit to force clock enable of scaling and rotating engine's data memory.
         */
        uint32_t sr_mem_clk_ena:1;
        /** sr_mem_force_pd : R/W; bitpos: [1]; default: 0;
         *  Set this bit to force power down scaling and rotating engine's data memory.
         */
        uint32_t sr_mem_force_pd:1;
        /** sr_mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power up scaling and rotating engine's data memory.
         */
        uint32_t sr_mem_force_pu:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ppa_sr_mem_pd_reg_t;

/** Type of reg_conf register
 *  Register clock enable register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  PPA register clock gate enable signal.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ppa_reg_conf_reg_t;

/** Type of eco_low register
 *  Reserved.
 */
typedef union {
    struct {
        /** rnd_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  Reserved.
         */
        uint32_t rnd_eco_low:32;
    };
    uint32_t val;
} ppa_eco_low_reg_t;

/** Type of eco_high register
 *  Reserved.
 */
typedef union {
    struct {
        /** rnd_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  Reserved.
         */
        uint32_t rnd_eco_high:32;
    };
    uint32_t val;
} ppa_eco_high_reg_t;

/** Type of sram_ctrl register
 *  PPA SRAM Control Register
 */
typedef union {
    struct {
        /** mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  Control signals
         */
        uint32_t mem_aux_ctrl:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} ppa_sram_ctrl_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  Raw status interrupt
 */
typedef union {
    struct {
        /** sr_eof_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when scaling and rotating engine
         *  calculate one frame image.
         */
        uint32_t sr_eof_int_raw:1;
        /** blend_eof_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when blending engine calculate one frame
         *  image.
         */
        uint32_t blend_eof_int_raw:1;
        /** sr_param_cfg_err_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the configured scaling and rotating
         *  coefficient is wrong. User can check the reasons through register
         *  PPA_SR_PARAM_ERR_ST_REG.
         */
        uint32_t sr_param_cfg_err_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ppa_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt
 */
typedef union {
    struct {
        /** sr_eof_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the PPA_SR_EOF_INT interrupt.
         */
        uint32_t sr_eof_int_st:1;
        /** blend_eof_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the PPA_BLEND_EOF_INT interrupt.
         */
        uint32_t blend_eof_int_st:1;
        /** sr_param_cfg_err_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the PPA_SR_RX_YSCAL_ERR_INT interrupt.
         */
        uint32_t sr_param_cfg_err_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ppa_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** sr_eof_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the PPA_SR_EOF_INT interrupt.
         */
        uint32_t sr_eof_int_ena:1;
        /** blend_eof_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the PPA_BLEND_EOF_INT interrupt.
         */
        uint32_t blend_eof_int_ena:1;
        /** sr_param_cfg_err_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the PPA_SR_RX_YSCAL_ERR_INT interrupt.
         */
        uint32_t sr_param_cfg_err_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ppa_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** sr_eof_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the PPA_SR_EOF_INT interrupt.
         */
        uint32_t sr_eof_int_clr:1;
        /** blend_eof_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the PPA_BLEND_EOF_INT interrupt.
         */
        uint32_t blend_eof_int_clr:1;
        /** sr_param_cfg_err_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the PPA_SR_RX_YSCAL_ERR_INT interrupt.
         */
        uint32_t sr_param_cfg_err_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ppa_int_clr_reg_t;


/** Group: Status Registers */
/** Type of clut_cnt register
 *  BLEND CLUT write counter register
 */
typedef union {
    struct {
        /** blend0_clut_cnt : RO; bitpos: [8:0]; default: 0;
         *  The write data counter of BLEND0 CLUT in fifo mode.
         */
        uint32_t blend0_clut_cnt:9;
        /** blend1_clut_cnt : RO; bitpos: [17:9]; default: 0;
         *  The write data counter of BLEND1 CLUT in fifo mode.
         */
        uint32_t blend1_clut_cnt:9;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} ppa_clut_cnt_reg_t;

/** Type of blend_st register
 *  Blending engine status register
 */
typedef union {
    struct {
        /** blend_size_diff_st : RO; bitpos: [0]; default: 0;
         *  1: indicate the size of two image is different.
         */
        uint32_t blend_size_diff_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ppa_blend_st_reg_t;

/** Type of sr_param_err_st register
 *  Scaling and rotating coefficient error register
 */
typedef union {
    struct {
        /** tx_dscr_vb_err_st : RO; bitpos: [0]; default: 0;
         *  The error is that the scaled VB plus the offset of Y coordinate in 2DDMA receive
         *  descriptor is larger than VA in 2DDMA receive descriptor.
         */
        uint32_t tx_dscr_vb_err_st:1;
        /** tx_dscr_hb_err_st : RO; bitpos: [1]; default: 0;
         *  The error is that the scaled HB plus the offset of X coordinate in 2DDMA receive
         *  descriptor is larger than HA in 2DDMA receive descriptor.
         */
        uint32_t tx_dscr_hb_err_st:1;
        /** y_rx_scal_equal_0_err_st : RO; bitpos: [2]; default: 0;
         *  The error is that the PPA_SR_SCAL_Y_INT and PPA_SR_CAL_Y_FRAG both are 0.
         */
        uint32_t y_rx_scal_equal_0_err_st:1;
        /** rx_dscr_vb_err_st : RO; bitpos: [3]; default: 0;
         *  The error is that VB in 2DDMA receive descriptor plus the offset of Y coordinate in
         *  2DDMA transmit descriptor is larger than VA in 2DDMA transmit descriptor
         */
        uint32_t rx_dscr_vb_err_st:1;
        /** ydst_len_too_samll_err_st : RO; bitpos: [4]; default: 0;
         *  The error is that the scaled image width is 0. For example.  when source width is
         *  14. scaled value is 1/16. and no rotate operation. then scaled width would be 0 as
         *  the result would be floored.
         */
        uint32_t ydst_len_too_samll_err_st:1;
        /** ydst_len_too_large_err_st : RO; bitpos: [5]; default: 0;
         *  The error is that the scaled width is larger than (2^13 - 1).
         */
        uint32_t ydst_len_too_large_err_st:1;
        /** x_rx_scal_equal_0_err_st : RO; bitpos: [6]; default: 0;
         *  The error is that the scaled image height is 0.
         */
        uint32_t x_rx_scal_equal_0_err_st:1;
        /** rx_dscr_hb_err_st : RO; bitpos: [7]; default: 0;
         *  The error is that the HB in 2DDMA transmit descriptor plus the offset of X
         *  coordinate in 2DDMA transmit descriptor is larger than HA in 2DDMA transmit
         *  descriptor.
         */
        uint32_t rx_dscr_hb_err_st:1;
        /** xdst_len_too_samll_err_st : RO; bitpos: [8]; default: 0;
         *  The error is that the scaled image height is 0. For example.  when source height is
         *  14. scaled value is 1/16. and no rotate operation. then scaled height would be 0 as
         *  the result would be floored.
         */
        uint32_t xdst_len_too_samll_err_st:1;
        /** xdst_len_too_large_err_st : RO; bitpos: [9]; default: 0;
         *  The error is that the scaled image height is larger than (2^13 - 1).
         */
        uint32_t xdst_len_too_large_err_st:1;
        /** x_yuv420_rx_scale_err_st : RO; bitpos: [10]; default: 0;
         *  The error is that the ha/hb/x param in dma2d descriptor is an odd num when enable
         *  yuv420 rx
         */
        uint32_t x_yuv420_rx_scale_err_st:1;
        /** y_yuv420_rx_scale_err_st : RO; bitpos: [11]; default: 0;
         *  The error is that the va/vb/y param in dma2d descriptor is an odd num when enable
         *  yuv420 rx
         */
        uint32_t y_yuv420_rx_scale_err_st:1;
        /** x_yuv420_tx_scale_err_st : RO; bitpos: [12]; default: 0;
         *  The error is that the ha/hb/x param in dma2d descriptor is an odd num when enable
         *  yuv420 tx
         */
        uint32_t x_yuv420_tx_scale_err_st:1;
        /** y_yuv420_tx_scale_err_st : RO; bitpos: [13]; default: 0;
         *  The error is that the va/vb/y param in dma2d descriptor is an odd num when enable
         *  yuv420 tx
         */
        uint32_t y_yuv420_tx_scale_err_st:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} ppa_sr_param_err_st_reg_t;

/** Type of sr_status register
 *  SR FSM register
 */
typedef union {
    struct {
        /** sr_rx_dscr_sample_state : RO; bitpos: [1:0]; default: 0;
         *  Reserved.
         */
        uint32_t sr_rx_dscr_sample_state:2;
        /** sr_rx_scan_state : RO; bitpos: [3:2]; default: 0;
         *  Reserved.
         */
        uint32_t sr_rx_scan_state:2;
        /** sr_tx_dscr_sample_state : RO; bitpos: [5:4]; default: 0;
         *  Reserved.
         */
        uint32_t sr_tx_dscr_sample_state:2;
        /** sr_tx_scan_state : RO; bitpos: [8:6]; default: 0;
         *  Reserved.
         */
        uint32_t sr_tx_scan_state:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} ppa_sr_status_reg_t;

/** Type of eco_cell_ctrl register
 *  Reserved.
 */
typedef union {
    struct {
        /** rdn_result : RO; bitpos: [0]; default: 0;
         *  Reserved.
         */
        uint32_t rdn_result:1;
        /** rdn_ena : R/W; bitpos: [1]; default: 0;
         *  Reserved.
         */
        uint32_t rdn_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} ppa_eco_cell_ctrl_reg_t;


/** Group: Version Register */
/** Type of date register
 *  PPA Version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36716609;
         *  register version.
         */
        uint32_t date:32;
    };
    uint32_t val;
} ppa_date_reg_t;


typedef struct {
    volatile ppa_blend0_clut_data_reg_t blend0_clut_data;
    volatile ppa_blend1_clut_data_reg_t blend1_clut_data;
    uint32_t reserved_008;
    volatile ppa_clut_conf_reg_t clut_conf;
    volatile ppa_int_raw_reg_t int_raw;
    volatile ppa_int_st_reg_t int_st;
    volatile ppa_int_ena_reg_t int_ena;
    volatile ppa_int_clr_reg_t int_clr;
    volatile ppa_sr_color_mode_reg_t sr_color_mode;
    volatile ppa_blend_color_mode_reg_t blend_color_mode;
    volatile ppa_sr_byte_order_reg_t sr_byte_order;
    volatile ppa_blend_byte_order_reg_t blend_byte_order;
    uint32_t reserved_030;
    volatile ppa_blend_trans_mode_reg_t blend_trans_mode;
    volatile ppa_sr_fix_alpha_reg_t sr_fix_alpha;
    volatile ppa_blend_tx_size_reg_t blend_tx_size;
    volatile ppa_blend_fix_alpha_reg_t blend_fix_alpha;
    uint32_t reserved_044;
    volatile ppa_blend_rgb_reg_t blend_rgb;
    volatile ppa_blend_fix_pixel_reg_t blend_fix_pixel;
    volatile ppa_ck_fg_low_reg_t ck_fg_low;
    volatile ppa_ck_fg_high_reg_t ck_fg_high;
    volatile ppa_ck_bg_low_reg_t ck_bg_low;
    volatile ppa_ck_bg_high_reg_t ck_bg_high;
    volatile ppa_ck_default_reg_t ck_default;
    volatile ppa_sr_scal_rotate_reg_t sr_scal_rotate;
    volatile ppa_sr_mem_pd_reg_t sr_mem_pd;
    volatile ppa_reg_conf_reg_t reg_conf;
    volatile ppa_clut_cnt_reg_t clut_cnt;
    volatile ppa_blend_st_reg_t blend_st;
    volatile ppa_sr_param_err_st_reg_t sr_param_err_st;
    volatile ppa_sr_status_reg_t sr_status;
    volatile ppa_eco_low_reg_t eco_low;
    volatile ppa_eco_high_reg_t eco_high;
    volatile ppa_eco_cell_ctrl_reg_t eco_cell_ctrl;
    volatile ppa_sram_ctrl_reg_t sram_ctrl;
    uint32_t reserved_090[28];
    volatile ppa_date_reg_t date;
} ppa_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(ppa_dev_t) == 0x104, "Invalid size of ppa_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
