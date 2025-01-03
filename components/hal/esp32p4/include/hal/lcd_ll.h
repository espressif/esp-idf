/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/hp_sys_clkrst_struct.h"

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
#define LCD_LL_FIFO_DEPTH          8   // Async FIFO depth

/**
 * @brief LCD data byte swizzle mode
 */
typedef enum {
    LCD_LL_SWIZZLE_AB2BA,   /*!< AB -> BA */
    LCD_LL_SWIZZLE_ABC2ACB, /*!< ABC -> ACB */
    LCD_LL_SWIZZLE_ABC2BAC, /*!< ABC -> BAC */
    LCD_LL_SWIZZLE_ABC2BCA, /*!< ABC -> BCA */
    LCD_LL_SWIZZLE_ABC2CAB, /*!< ABC -> CAB */
    LCD_LL_SWIZZLE_ABC2CBA, /*!< ABC -> CBA */
} lcd_ll_swizzle_mode_t;

/**
 * @brief Enable the bus clock for LCD module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void lcd_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl3.reg_lcdcam_apb_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define lcd_ll_enable_bus_clock(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; lcd_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the LCD module
 *
 * @param group_id Group ID
 */
static inline void _lcd_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_lcdcam = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_lcdcam = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define lcd_ll_reset_register(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _lcd_ll_reset_register(__VA_ARGS__)

/**
 * @brief Enable clock gating
 *
 * @param dev LCD register base address
 * @param en True to enable, False to disable
 */
static inline void lcd_ll_enable_clock(lcd_cam_dev_t *dev, bool en)
{
    HP_SYS_CLKRST.peri_clk_ctrl19.reg_lcd_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lcd_ll_enable_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; lcd_ll_enable_clock(__VA_ARGS__)

/**
 * @brief Select clock source for LCD peripheral
 *
 * @param dev LCD register base address
 * @param src Clock source
 */
static inline void lcd_ll_select_clk_src(lcd_cam_dev_t *dev, lcd_clock_source_t src)
{
    switch (src) {
    case LCD_CLK_SRC_XTAL:
        HP_SYS_CLKRST.peri_clk_ctrl19.reg_lcd_clk_src_sel = 0;
        break;
    case LCD_CLK_SRC_PLL160M:
        HP_SYS_CLKRST.peri_clk_ctrl19.reg_lcd_clk_src_sel = 1;
        break;
    case LCD_CLK_SRC_APLL:
        HP_SYS_CLKRST.peri_clk_ctrl19.reg_lcd_clk_src_sel = 2;
        break;
    default:
        // disable the clock
        HP_SYS_CLKRST.peri_clk_ctrl19.reg_lcd_clk_src_sel = 3;
        break;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lcd_ll_select_clk_src(...) (void)__DECLARE_RCC_ATOMIC_ENV; lcd_ll_select_clk_src(__VA_ARGS__)

/**
 * @brief Set clock coefficient of LCD peripheral
 *
 * @param dev LCD register base address
 * @param div_num Integer part of the divider
 * @param div_a denominator of the divider
 * @param div_b numerator of the divider
 */
__attribute__((always_inline))
static inline void lcd_ll_set_group_clock_coeff(lcd_cam_dev_t *dev, int div_num, int div_a, int div_b)
{
    // lcd_clk = module_clock_src / (div_num + div_b / div_a)
    HAL_ASSERT(div_num > 0 && div_num <= LCD_LL_CLK_FRAC_DIV_N_MAX);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl110, reg_lcd_clk_div_num, div_num - 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl110, reg_lcd_clk_div_denominator, div_a);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl110, reg_lcd_clk_div_numerator, div_b);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lcd_ll_set_group_clock_coeff(...) (void)__DECLARE_RCC_ATOMIC_ENV; lcd_ll_set_group_clock_coeff(__VA_ARGS__)

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

/**
 * @brief Set the PCLK sample edge
 *
 * @param dev LCD register base address
 * @param active_on_neg True: sample on negedge, False: sample on posedge
 */
__attribute__((always_inline))
static inline void lcd_ll_set_pixel_clock_edge(lcd_cam_dev_t *dev, bool active_on_neg)
{
    dev->lcd_clock.lcd_ck_out_edge = active_on_neg;
}

/**
 * @brief Set PCLK prescale
 *
 * @param dev LCD register base address
 * @param prescale Prescale value, PCLK = LCD_CLK / prescale
 */
__attribute__((always_inline))
static inline void lcd_ll_set_pixel_clock_prescale(lcd_cam_dev_t *dev, uint32_t prescale)
{
    HAL_ASSERT(prescale > 0 && prescale <= LCD_LL_PCLK_DIV_MAX);
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

/**
 * @brief Enable YUV-RGB converter
 *
 * @param dev LCD register base address
 * @param en True to enable converter, False to disable converter
 */
static inline void lcd_ll_enable_rgb_yuv_convert(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_rgb_yuv.lcd_conv_enable = en;
}

/**
 * @brief Set convert data line width
 *
 * @param dev LCD register base address
 * @param width data line width
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
    if (range == LCD_COLOR_RANGE_LIMIT) {
        dev->lcd_rgb_yuv.lcd_conv_data_in_mode = 0;
    } else if (range == LCD_COLOR_RANGE_FULL) {
        dev->lcd_rgb_yuv.lcd_conv_data_in_mode = 1;
    }
}

/**
 * @brief Set the color range of output data
 *
 * @param dev LCD register base address
 * @param range Color range
 */
static inline void lcd_ll_set_output_color_range(lcd_cam_dev_t *dev, lcd_color_range_t range)
{
    if (range == LCD_COLOR_RANGE_LIMIT) {
        dev->lcd_rgb_yuv.lcd_conv_data_out_mode = 0;
    } else if (range == LCD_COLOR_RANGE_FULL) {
        dev->lcd_rgb_yuv.lcd_conv_data_out_mode = 1;
    }
}

/**
 * @brief Set YUV conversion standard
 *
 * @param dev LCD register base address
 * @param std YUV conversion standard
 */
static inline void lcd_ll_set_yuv_convert_std(lcd_cam_dev_t *dev, lcd_yuv_conv_std_t std)
{
    if (std == LCD_YUV_CONV_STD_BT601) {
        dev->lcd_rgb_yuv.lcd_conv_protocol_mode = 0;
    } else if (std == LCD_YUV_CONV_STD_BT709) {
        dev->lcd_rgb_yuv.lcd_conv_protocol_mode = 1;
    }
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
    dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 3;
    switch (yuv_sample) {
    case LCD_YUV_SAMPLE_422:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 0;
        break;
    case LCD_YUV_SAMPLE_420:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 1;
        break;
    case LCD_YUV_SAMPLE_411:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 2;
        break;
    default:
        abort();
    }
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
    dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 3;
    switch (yuv_sample) {
    case LCD_YUV_SAMPLE_422:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 0;
        break;
    case LCD_YUV_SAMPLE_420:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 1;
        break;
    case LCD_YUV_SAMPLE_411:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 2;
        break;
    default:
        abort();
    }
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
    switch (src_sample) {
    case LCD_YUV_SAMPLE_422:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 0;
        break;
    case LCD_YUV_SAMPLE_420:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 1;
        break;
    case LCD_YUV_SAMPLE_411:
        dev->lcd_rgb_yuv.lcd_conv_yuv_mode = 2;
        break;
    default:
        abort();
    }
    switch (dst_sample) {
    case LCD_YUV_SAMPLE_422:
        dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 0;
        break;
    case LCD_YUV_SAMPLE_420:
        dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 1;
        break;
    case LCD_YUV_SAMPLE_411:
        dev->lcd_rgb_yuv.lcd_conv_yuv2yuv_mode = 2;
        break;
    default:
        abort();
    }
}

/**
 * @brief Set clock cycles of each transaction phases
 *
 * @param dev LCD register base address
 * @param cmd_cycles Clock cycles of CMD phase
 * @param dummy_cycles Clock cycles of DUMMY phase
 * @param data_cycles Clock cycles of DATA phase
 */
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

/**
 * @brief Set clock cycles of blank phases
 *
 * @param dev LCD register base address
 * @param fk_cycles Clock cycles of front blank
 * @param bk_cycles Clock cycles of back blank
 */
static inline void lcd_ll_set_blank_cycles(lcd_cam_dev_t *dev, uint32_t fk_cycles, uint32_t bk_cycles)
{
    dev->lcd_misc.lcd_bk_en = (fk_cycles || bk_cycles);
    dev->lcd_misc.lcd_vfk_cyclelen = fk_cycles - 1;
    dev->lcd_misc.lcd_vbk_cyclelen = bk_cycles - 1;
}

/**
 * @brief Set data read stride, i.e., number of bytes the LCD reads from the DMA in each step
 *
 * @param dev LCD register base address
 * @param stride data stride size, in bits
 */
static inline void lcd_ll_set_dma_read_stride(lcd_cam_dev_t *dev, uint32_t stride)
{
    switch (stride) {
    case 8:
        dev->lcd_user.lcd_byte_mode = 0;
        break;
    case 16:
        dev->lcd_user.lcd_byte_mode = 1;
        break;
    case 24:
        dev->lcd_user.lcd_byte_mode = 2;
        break;
    case 32:
        dev->lcd_user.lcd_byte_mode = 3;
        break;
    default:
        abort();
    }
}

/**
 * @brief Set the wire width of LCD output
 *
 * @param dev LCD register base address
 * @param width LCD output wire width
 */
static inline void lcd_ll_set_data_wire_width(lcd_cam_dev_t *dev, uint32_t width)
{
    switch (width) {
    case 8:
        dev->lcd_misc.lcd_wire_mode = 0;
        break;
    case 16:
        dev->lcd_misc.lcd_wire_mode = 1;
        break;
    case 24:
        dev->lcd_misc.lcd_wire_mode = 2;
        break;
    default:
        abort();
    }
}

/**
 * @brief Whether to continue the data phase when the DMA has content to send
 *
 * @param dev LCD register base address
 * @param en True: The number of data cycles will be controller by DMA buffer size, instead of lcd_dout_cyclelen
 *           False: The number of data cycles will be controlled by lcd_dout_cyclelen
 */
static inline void lcd_ll_enable_output_always_on(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_always_out_en = en;
}

/**
 * @brief Start the LCD transaction
 *
 * @param dev LCD register base address
 */
__attribute__((always_inline))
static inline void lcd_ll_start(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_update_reg = 1; // update parameters before start transaction
    dev->lcd_user.lcd_start = 1;
}

/**
 * @brief Stop the LCD transaction
 *
 * @param dev LCD register base address
 */
__attribute__((always_inline))
static inline void lcd_ll_stop(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_start = 0;
    dev->lcd_user.lcd_update_reg = 1; // self clear
}

/**
 * @brief Reset LCD TX controller and RGB/YUV converter
 *
 * @param dev LCD register base address
 */
static inline void lcd_ll_reset(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_reset = 1; // self clear
}

/**
 * @brief Whether to reverse the data bit order
 *
 * @note It acts before the YUV-RGB converter
 *
 * @param dev LCD register base address
 * @param en True to reverse, False to not reverse
 */
__attribute__((always_inline))
static inline void lcd_ll_reverse_dma_data_bit_order(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_bit_order = en;
}

/**
 * @brief Whether to reverse the output data bit order
 *
 * @note It acts after the YUV-RGB converter
 *
 * @param dev LCD register base address
 * @param en True to reverse, False to not reverse
 */
static inline void lcd_ll_reverse_wire_bit_order(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_dout_bit_order = en;
}

/**
 * @brief Whether to swap adjacent two bytes
 *
 * @note This acts before the YUV-RGB converter, mainly to change the data endian.
 *       e.g. {B1,B0},{B3,B2} => {B0,B1}{B2,B3}.
 *       Only valid when `lcd_ll_set_dma_read_stride` set the DMA read stride >= 16 bits
 *
 * @param dev LCD register base address
 * @param en True to swap the byte order, False to not swap
 */
__attribute__((always_inline))
static inline void lcd_ll_swap_dma_data_byte_order(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_byte_order = en;
}

/**
 * @brief Enable the byte swizzle
 *
 * @note The swizzle module acts after the YUV-RGB converter, used to reorder the data bytes before the data output line
 *
 * @param dev LCD register base address
 * @param en True to enable, False to disable
 */
__attribute__((always_inline))
static inline void lcd_ll_enable_swizzle(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_dout_byte_swizzle_enable = en;
}

/**
 * @brief Set data byte swizzle mode
 *
 * @param dev LCD register base address
 * @param mode Swizzle mode
 */
static inline void lcd_ll_set_swizzle_mode(lcd_cam_dev_t *dev, lcd_ll_swizzle_mode_t mode)
{
    dev->lcd_user.lcd_dout_byte_swizzle_mode = mode;
}

/**
 * @brief Reset Async TX FIFO
 *
 * @param dev LCD register base address
 */
__attribute__((always_inline))
static inline void lcd_ll_fifo_reset(lcd_cam_dev_t *dev)
{
    dev->lcd_misc.lcd_afifo_reset = 1; // self clear
}

/**
 * @brief Set the level state of DC line, on different transaction phases
 *
 * @param dev LCD register base address
 * @param idle_phase Level state of DC line on IDLE phase
 * @param cmd_phase Level state of DC line on CMD phase
 * @param dummy_phase Level state of DC line on DUMMY phase
 * @param data_phase Level state of DC line on DATA phase
 */
__attribute__((always_inline))
static inline void lcd_ll_set_dc_level(lcd_cam_dev_t *dev, bool idle_phase, bool cmd_phase, bool dummy_phase, bool data_phase)
{
    dev->lcd_misc.lcd_cd_idle_edge = idle_phase;
    dev->lcd_misc.lcd_cd_cmd_set = (cmd_phase != idle_phase);
    dev->lcd_misc.lcd_cd_dummy_set = (dummy_phase != idle_phase);
    dev->lcd_misc.lcd_cd_data_set = (data_phase != idle_phase);
}

/**
 * @brief Set cycle of delay for DC line
 *
 * @param dev LCD register base address
 * @param delay Ticks of delay
 */
static inline void lcd_ll_set_dc_delay_ticks(lcd_cam_dev_t *dev, uint32_t delay)
{
    dev->lcd_dly_mode_cfg1.lcd_cd_mode = delay;
}

/**
 * @brief Set the LCD command (the data at CMD phase)
 *
 * @param dev LCD register base address
 * @param data_width Data line width
 * @param command command value
 */
__attribute__((always_inline))
static inline void lcd_ll_set_command(lcd_cam_dev_t *dev, uint32_t data_width, uint32_t command)
{
    // i80 interface only supports 8-bit or 16-bit data width
    HAL_ASSERT(data_width == 8 || data_width == 16);
    // if command phase has two cycles, in the first cycle we use lcd_first_cmd_val
    // in the second cycle, we use lcd_latter_cmd_val
    if (data_width == 8) {
        dev->lcd_first_cmd_val.val = command & 0xFF;
        dev->lcd_latter_cmd_val.val = (command >> 8) & 0xFF;
    } else if (data_width == 16) {
        dev->lcd_first_cmd_val.val = command;
    }
}

/**
 * @brief Whether to enable RGB interface
 *
 * @param dev LCD register base address
 * @param en True to enable RGB interface, False to disable RGB interface
 */
static inline void lcd_ll_enable_rgb_mode(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_ctrl.lcd_rgb_mode_en = en;
}

/**
 * @brief Whether to send the next frame automatically
 *
 * @param dev LCD register base address
 * @param en True to enable, False to disable
 */
static inline void lcd_ll_enable_auto_next_frame(lcd_cam_dev_t *dev, bool en)
{
    // in RGB mode, enabling "next frame" means LCD controller keeps sending frame data
    dev->lcd_misc.lcd_next_frame_en = en;
}

/**
 * @brief Whether to output HSYNC signal in porch resion
 *
 * @param dev LCD register base address
 * @param en True to enable, False to disable
 */
static inline void lcd_ll_enable_output_hsync_in_porch_region(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_ctrl2.lcd_hs_blank_en = en;
}

/**
 * @brief Set HSYNC signal offset in the line
 *
 * @param dev LCD register base address
 * @param offset_in_line Offset value
 */
static inline void lcd_ll_set_hsync_position(lcd_cam_dev_t *dev, uint32_t offset_in_line)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lcd_ctrl2, lcd_hsync_position, offset_in_line);
}

/**
 * @brief Set RGB LCD horizontal timing
 *
 * @param dev LCD register base address
 * @param hsw Horizontal sync width
 * @param hbp Horizontal back porch
 * @param active_width Horizontal active width
 * @param hfp Horizontal front porch
 */
static inline void lcd_ll_set_horizontal_timing(lcd_cam_dev_t *dev, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    dev->lcd_ctrl2.lcd_hsync_width = hsw - 1;
    dev->lcd_ctrl.lcd_hb_front = hbp + hsw - 1;
    dev->lcd_ctrl1.lcd_ha_width = active_width - 1;
    dev->lcd_ctrl1.lcd_ht_width = hsw + hbp + active_width + hfp - 1;
}

/**
 * @brief Set RGB vertical timing
 *
 * @param dev LCD register base address
 * @param vsw Vertical sync width
 * @param vbp Vertical back porch
 * @param active_height Vertical active height
 * @param vfp Vertical front porch
 */
static inline void lcd_ll_set_vertical_timing(lcd_cam_dev_t *dev, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    dev->lcd_ctrl2.lcd_vsync_width = vsw - 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lcd_ctrl1, lcd_vb_front, vbp + vsw - 1);
    dev->lcd_ctrl.lcd_va_height = active_height - 1;
    dev->lcd_ctrl.lcd_vt_height = vsw + vbp + active_height + vfp - 1;
}

/**
 * @brief Set level state for hsync, vsync, de at IDLE phase
 *
 * @param dev LCD register base address
 * @param hsync_idle_level HSYNC level on IDLE phase
 * @param vsync_idle_level VSYNC level on IDLE phase
 * @param de_idle_level DE level on IDLE phase
 */
static inline void lcd_ll_set_idle_level(lcd_cam_dev_t *dev, bool hsync_idle_level, bool vsync_idle_level, bool de_idle_level)
{
    dev->lcd_ctrl2.lcd_hsync_idle_pol = hsync_idle_level;
    dev->lcd_ctrl2.lcd_vsync_idle_pol = vsync_idle_level;
    dev->lcd_ctrl2.lcd_de_idle_pol = de_idle_level;
}

/**
 * @brief Set extra delay for HSYNC, VSYNC, and DE signals
 *
 * @param dev LCD register base address
 * @param hsync_delay HSYNC delay
 * @param vsync_delay VSYNC delay
 * @param de_delay DE delay
 */
static inline void lcd_ll_set_delay_ticks(lcd_cam_dev_t *dev, uint32_t hsync_delay, uint32_t vsync_delay, uint32_t de_delay)
{
    dev->lcd_dly_mode_cfg1.lcd_hsync_mode = hsync_delay;
    dev->lcd_dly_mode_cfg1.lcd_vsync_mode = vsync_delay;
    dev->lcd_dly_mode_cfg1.lcd_de_mode = de_delay;
}

/**
 * @brief Enable/disable interrupt by mask
 *
 * @param dev LCD register base address
 * @param mask Interrupt mask
 * @param en True to enable interrupt, False to disable interrupt
 */
static inline void lcd_ll_enable_interrupt(lcd_cam_dev_t *dev, uint32_t mask, bool en)
{
    if (en) {
        dev->lc_dma_int_ena.val |= mask & 0x03;
    } else {
        dev->lc_dma_int_ena.val &= ~(mask & 0x03);
    }
}

/**
 * @brief Get interrupt status value
 *
 * @param dev LCD register base address
 * @return Interrupt status value
 */
__attribute__((always_inline))
static inline uint32_t lcd_ll_get_interrupt_status(lcd_cam_dev_t *dev)
{
    return dev->lc_dma_int_st.val & 0x03;
}

/**
 * @brief Clear interrupt status by mask
 *
 * @param dev LCD register base address
 * @param mask Interrupt status mask
 */
__attribute__((always_inline))
static inline void lcd_ll_clear_interrupt_status(lcd_cam_dev_t *dev, uint32_t mask)
{
    dev->lc_dma_int_clr.val = mask & 0x03;
}

/**
 * @brief Get address of interrupt status register address
 *
 * @param dev LCD register base address
 * @return Interrupt status register address
 */
static inline volatile void *lcd_ll_get_interrupt_status_reg(lcd_cam_dev_t *dev)
{
    return &dev->lc_dma_int_st;
}

#ifdef __cplusplus
}
#endif
