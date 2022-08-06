/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h> /* For NULL declaration */
#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "soc/lcd_cam_reg.h"
#include "soc/lcd_cam_struct.h"
#include "hal/assert.h"
#include "hal/lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_LL_GET_HW(id) (((id) == 0) ? (&LCD_CAM) : NULL)

// Interrupt event, bit mask
#define LCD_LL_EVENT_VSYNC_END  (1 << 0)
#define LCD_LL_EVENT_TRANS_DONE (1 << 1)

#define LCD_LL_CLK_FRAC_DIV_N_MAX  256 // LCD_CLK = LCD_CLK_S / (N + b/a), the N register is 8 bit-width
#define LCD_LL_CLK_FRAC_DIV_AB_MAX 64  // LCD_CLK = LCD_CLK_S / (N + b/a), the a/b register is 6 bit-width
#define LCD_LL_PCLK_DIV_MAX        64  // LCD_PCLK = LCD_CLK / MO, the MO register is 6 bit-width

#define LCD_LL_COLOR_RANGE_TO_REG(range) (uint8_t[]){0,1}[(range)]
#define LCD_LL_CONV_STD_TO_REG(std)      (uint8_t[]){0,1}[(std)]
#define LCD_LL_YUV_SAMPLE_TO_REG(sample) (uint8_t[]){0,1,2}[(sample)]

static inline void lcd_ll_enable_clock(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_clock.clk_en = en;
}

/**
 * @brief Select clock source for LCD peripheral
 *
 * @param dev LCD register base address
 * @param src Clock source
 */
static inline void lcd_ll_select_clk_src(lcd_cam_dev_t *dev, lcd_clock_source_t src)
{
    switch (src) {
    case LCD_CLK_SRC_PLL160M:
        dev->lcd_clock.lcd_clk_sel = 3;
        break;
    case LCD_CLK_SRC_PLL240M:
        dev->lcd_clock.lcd_clk_sel = 2;
        break;
    case LCD_CLK_SRC_XTAL:
        dev->lcd_clock.lcd_clk_sel = 1;
        break;
    default:
        // disable LCD clock source
        dev->lcd_clock.lcd_clk_sel = 0;
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Set clock coefficient of LCD peripheral
 *
 * @param dev LCD register base address
 * @param div_num Integer part of the divider
 * @param div_a denominator of the divider
 * @param div_b numerator of the divider
 */
static inline void lcd_ll_set_group_clock_coeff(lcd_cam_dev_t *dev, int div_num, int div_a, int div_b)
{
    // lcd_clk = module_clock_src / (div_num + div_b / div_a)
    HAL_ASSERT(div_num >= 2 && div_num <= LCD_LL_CLK_FRAC_DIV_N_MAX);
    // dic_num == 0 means LCD_LL_CLK_FRAC_DIV_N_MAX divider in hardware
    if (div_num >= LCD_LL_CLK_FRAC_DIV_N_MAX) {
        div_num = 0;
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lcd_clock, lcd_clkm_div_num, div_num);
    dev->lcd_clock.lcd_clkm_div_a = div_a;
    dev->lcd_clock.lcd_clkm_div_b = div_b;
}


/**
 * @brief Set the PCLK clock level state when there's no transaction undergoing
 *
 * @param dev LCD register base address
 * @param level 1 is high level, 0 is low level
 */
__attribute__((always_inline))
static inline void lcd_ll_set_clock_idle_level(lcd_cam_dev_t *dev, bool level)
{
    dev->lcd_clock.lcd_ck_idle_edge = level;
}

__attribute__((always_inline))
static inline void lcd_ll_set_pixel_clock_edge(lcd_cam_dev_t *dev, bool active_on_neg)
{
    dev->lcd_clock.lcd_ck_out_edge = active_on_neg;
}

__attribute__((always_inline))
static inline void lcd_ll_set_pixel_clock_prescale(lcd_cam_dev_t *dev, uint32_t prescale)
{
    HAL_ASSERT(prescale <= LCD_LL_PCLK_DIV_MAX);
    // Formula: pixel_clk = lcd_clk / (1 + clkcnt_n)
    // clkcnt_n can't be zero
    uint32_t scale = 1;
    if (prescale == 1) {
        dev->lcd_clock.lcd_clk_equ_sysclk = 1;
    } else {
        dev->lcd_clock.lcd_clk_equ_sysclk = 0;
        scale = prescale - 1;
    }
    dev->lcd_clock.lcd_clkcnt_n = scale;
}

static inline void lcd_ll_enable_rgb_yuv_convert(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_rgb_yuv.lcd_conv_bypass = en;
}

/**
 * @brief Set convert data line width
 *
 * @param dev LCD register base address
 * @param width data line width (8 or 16)
 */
static inline void lcd_ll_set_convert_data_width(lcd_cam_dev_t *dev, uint32_t width)
{
    HAL_ASSERT(width == 8 || width == 16);
    dev->lcd_rgb_yuv.lcd_conv_mode_8bits_on = (width == 8) ? 1 : 0;
}

/**
 * @brief Set the color range of input data
 *
 * @param dev LCD register base address
 * @param range Color range
 */
static inline void lcd_ll_set_input_color_range(lcd_cam_dev_t *dev, lcd_color_range_t range)
{
    dev->lcd_rgb_yuv.lcd_conv_data_in_mode = LCD_LL_COLOR_RANGE_TO_REG(range);
}

/**
 * @brief Set the color range of output data
 *
 * @param dev LCD register base address
 * @param range Color range
 */
static inline void lcd_ll_set_output_color_range(lcd_cam_dev_t *dev, lcd_color_range_t range)
{
    dev->lcd_rgb_yuv.lcd_conv_data_out_mode = LCD_LL_COLOR_RANGE_TO_REG(range);
}

/**
 * @brief Set YUV conversion standard
 *
 * @param dev LCD register base address
 * @param std YUV conversion standard
 */
static inline void lcd_ll_set_yuv_convert_std(lcd_cam_dev_t *dev, lcd_yuv_conv_std_t std)
{
    dev->lcd_rgb_yuv.lcd_conv_protocol_mode = LCD_LL_CONV_STD_TO_REG(std);
}

/**
 * @brief Set the converter mode: RGB565 to YUV
 *
 * @param dev LCD register base address
 * @param yuv_sample YUV sample mode
 */
static inline void lcd_ll_set_convert_mode_rgb_to_yuv(lcd_cam_dev_t *dev, lcd_yuv_sample_t yuv_sample)
{
    dev->lcd_rgb_yuv.lcd_conv_trans_mode = 1;
    dev->lcd_rgb_yuv.lcd_conv_yuv_mode = LCD_LL_YUV_SAMPLE_TO_REG(yuv_sample);
    dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 3;
}

/**
 * @brief Set the converter mode: YUV to RGB565
 *
 * @param dev LCD register base address
 * @param yuv_sample YUV sample mode
 */
static inline void lcd_ll_set_convert_mode_yuv_to_rgb(lcd_cam_dev_t *dev, lcd_yuv_sample_t yuv_sample)
{
    dev->lcd_rgb_yuv.lcd_conv_trans_mode = 0;
    dev->lcd_rgb_yuv.lcd_conv_yuv_mode = LCD_LL_YUV_SAMPLE_TO_REG(yuv_sample);
    dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 3;
}

/**
 * @brief Set the converter mode: YUV to YUV
 *
 * @param dev LCD register base address
 * @param src_sample Source YUV sample mode
 * @param dst_sample Destination YUV sample mode
 */
static inline void lcd_ll_set_convert_mode_yuv_to_yuv(lcd_cam_dev_t *dev, lcd_yuv_sample_t src_sample, lcd_yuv_sample_t dst_sample)
{
    HAL_ASSERT(src_sample != dst_sample);
    dev->lcd_rgb_yuv.lcd_conv_trans_mode = 1;
    dev->lcd_rgb_yuv.lcd_conv_yuv_mode = LCD_LL_YUV_SAMPLE_TO_REG(src_sample);
    dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = LCD_LL_YUV_SAMPLE_TO_REG(dst_sample);
}

__attribute__((always_inline))
static inline void lcd_ll_set_phase_cycles(lcd_cam_dev_t *dev, uint32_t cmd_cycles, uint32_t dummy_cycles, uint32_t data_cycles)
{
    HAL_ASSERT(cmd_cycles <= 2);
    dev->lcd_user.lcd_cmd = (cmd_cycles > 0);
    dev->lcd_user.lcd_dummy = (dummy_cycles > 0);
    dev->lcd_user.lcd_dout = (data_cycles > 0);
    dev->lcd_user.lcd_cmd_2_cycle_en = cmd_cycles > 1;
    dev->lcd_user.lcd_dummy_cyclelen = dummy_cycles - 1;
    dev->lcd_user.lcd_dout_cyclelen = data_cycles - 1;
}

static inline void lcd_ll_set_blank_cycles(lcd_cam_dev_t *dev, uint32_t fk_cycles, uint32_t bk_cycles)
{
    dev->lcd_misc.lcd_bk_en = (fk_cycles || bk_cycles);
    dev->lcd_misc.lcd_vfk_cyclelen = fk_cycles - 1;
    dev->lcd_misc.lcd_vbk_cyclelen = bk_cycles - 1;
}

static inline void lcd_ll_set_data_width(lcd_cam_dev_t *dev, uint32_t width)
{
    HAL_ASSERT(width == 8 || width == 16);
    dev->lcd_user.lcd_2byte_en = (width == 16);
}

static inline void lcd_ll_enable_output_always_on(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_always_out_en = en;
}

__attribute__((always_inline))
static inline void lcd_ll_start(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_update = 1; // update parameters before start transaction
    dev->lcd_user.lcd_start = 1;
}

__attribute__((always_inline))
static inline void lcd_ll_stop(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_start = 0;
    dev->lcd_user.lcd_update = 1; // self clear
}

static inline void lcd_ll_reset(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_reset = 1; // self clear
}

__attribute__((always_inline))
static inline void lcd_ll_reverse_bit_order(lcd_cam_dev_t *dev, bool en)
{
    // whether to change LCD_DATA_out[N:0] to LCD_DATA_out[0:N]
    dev->lcd_user.lcd_bit_order = en;
}

__attribute__((always_inline))
static inline void lcd_ll_swap_byte_order(lcd_cam_dev_t *dev, uint32_t width, bool en)
{
    HAL_ASSERT(width == 8 || width == 16);
    if (width == 8) {
        // {B0}{B1}{B2}{B3} => {B1}{B0}{B3}{B2}
        dev->lcd_user.lcd_8bits_order = en;
        dev->lcd_user.lcd_byte_order = 0;
    } else if (width == 16) {
        // {B1,B0},{B3,B2} => {B0,B1}{B2,B3}
        dev->lcd_user.lcd_byte_order = en;
        dev->lcd_user.lcd_8bits_order = 0;
    }
}

__attribute__((always_inline))
static inline void lcd_ll_fifo_reset(lcd_cam_dev_t *dev)
{
    dev->lcd_misc.lcd_afifo_reset = 1; // self clear
}

__attribute__((always_inline))
static inline void lcd_ll_set_dc_level(lcd_cam_dev_t *dev, bool idle_phase, bool cmd_phase, bool dummy_phase, bool data_phase)
{
    dev->lcd_misc.lcd_cd_idle_edge = idle_phase;
    dev->lcd_misc.lcd_cd_cmd_set = (cmd_phase != idle_phase);
    dev->lcd_misc.lcd_cd_dummy_set = (dummy_phase != idle_phase);
    dev->lcd_misc.lcd_cd_data_set = (data_phase != idle_phase);
}

static inline void lcd_ll_set_dc_delay_ticks(lcd_cam_dev_t *dev, uint32_t delay)
{
    dev->lcd_dly_mode.lcd_cd_mode = delay;
}

__attribute__((always_inline))
static inline void lcd_ll_set_command(lcd_cam_dev_t *dev, uint32_t data_width, uint32_t command)
{
    HAL_ASSERT(data_width == 8 || data_width == 16);
    // if command phase has two cycles, in the first cycle, command[15:0] is sent out via lcd_data_out[15:0]
    // in the second cycle, command[31:16] is sent out via lcd_data_out[15:0]
    if (data_width == 8) {
        command = (command & 0xFF) | (command & 0xFF00) << 8;
    }
    dev->lcd_cmd_val.lcd_cmd_value = command;
}

static inline void lcd_ll_enable_rgb_mode(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_ctrl.lcd_rgb_mode_en = en;
}

static inline void lcd_ll_enable_auto_next_frame(lcd_cam_dev_t *dev, bool en)
{
    // in RGB mode, enabling "next frame" means LCD controller keeps sending frame data
    dev->lcd_misc.lcd_next_frame_en = en;
}

static inline void lcd_ll_enable_output_hsync_in_porch_region(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_ctrl2.lcd_hs_blank_en = en;
}

static inline void lcd_ll_set_hsync_position(lcd_cam_dev_t *dev, uint32_t offset_in_line)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lcd_ctrl2, lcd_hsync_position, offset_in_line);
}

static inline void lcd_ll_set_horizontal_timing(lcd_cam_dev_t *dev, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    dev->lcd_ctrl2.lcd_hsync_width = hsw - 1;
    dev->lcd_ctrl.lcd_hb_front = hbp + hsw - 1;
    dev->lcd_ctrl1.lcd_ha_width = active_width - 1;
    dev->lcd_ctrl1.lcd_ht_width = hsw + hbp + active_width + hfp - 1;
}

static inline void lcd_ll_set_vertical_timing(lcd_cam_dev_t *dev, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    dev->lcd_ctrl2.lcd_vsync_width = vsw - 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lcd_ctrl1, lcd_vb_front, vbp + vsw - 1);
    dev->lcd_ctrl.lcd_va_height = active_height - 1;
    dev->lcd_ctrl.lcd_vt_height = vsw + vbp + active_height + vfp - 1;
}

static inline void lcd_ll_set_idle_level(lcd_cam_dev_t *dev, bool hsync_idle_level, bool vsync_idle_level, bool de_idle_level)
{
    dev->lcd_ctrl2.lcd_hsync_idle_pol = hsync_idle_level;
    dev->lcd_ctrl2.lcd_vsync_idle_pol = vsync_idle_level;
    dev->lcd_ctrl2.lcd_de_idle_pol = de_idle_level;
}

static inline void lcd_ll_set_delay_ticks(lcd_cam_dev_t *dev, uint32_t hsync_delay, uint32_t vsync_delay, uint32_t de_delay)
{
    dev->lcd_dly_mode.lcd_hsync_mode = hsync_delay;
    dev->lcd_dly_mode.lcd_vsync_mode = vsync_delay;
    dev->lcd_dly_mode.lcd_de_mode = de_delay;
}

static inline void lcd_ll_set_data_delay_ticks(lcd_cam_dev_t *dev, uint32_t delay)
{
    uint32_t reg_val = 0;
    for (int i = 0; i < 16; i++) {
        reg_val |= (delay & 0x03) << (2 * i);
    }
    dev->lcd_data_dout_mode.val = reg_val;
}

static inline void lcd_ll_enable_interrupt(lcd_cam_dev_t *dev, uint32_t mask, bool en)
{
    if (en) {
        dev->lc_dma_int_ena.val |= mask & 0x03;
    } else {
        dev->lc_dma_int_ena.val &= ~(mask & 0x03);
    }
}

__attribute__((always_inline))
static inline uint32_t lcd_ll_get_interrupt_status(lcd_cam_dev_t *dev)
{
    return dev->lc_dma_int_st.val & 0x03;
}

__attribute__((always_inline))
static inline void lcd_ll_clear_interrupt_status(lcd_cam_dev_t *dev, uint32_t mask)
{
    dev->lc_dma_int_clr.val = mask & 0x03;
}

static inline volatile void *lcd_ll_get_interrupt_status_reg(lcd_cam_dev_t *dev)
{
    return &dev->lc_dma_int_st;
}

#ifdef __cplusplus
}
#endif
