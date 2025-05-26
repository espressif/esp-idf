/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/ppa_types.h"
#include "hal/color_types.h"
#include "soc/ppa_struct.h"
#include "soc/ppa_reg.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/assert.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PPA_LL_GET_HW     &PPA

#define PPA_LL_BLEND0_CLUT_MEM_ADDR_OFFSET  0x400
#define PPA_LL_BLEND1_CLUT_MEM_ADDR_OFFSET  0x800

#define PPA_LL_SRM_SCALING_INT_MAX   (PPA_SR_SCAL_X_INT_V + 1)
#define PPA_LL_SRM_SCALING_FRAG_MAX  (PPA_SR_SCAL_X_FRAG_V + 1)

// TODO: On P4 ECO2, SRM block size needs update
#define PPA_LL_SRM_DEFAULT_BLOCK_SIZE   18 // 18 x 18 block size
#define PPA_LL_SRM_YUV420_BLOCK_SIZE    20 // 20 x 20 block size

/**
 * @brief Enumeration of PPA blending mode
 */
typedef enum {
    PPA_LL_BLEND_TRANS_MODE_BLEND,          /*!< Blending engine performs blend operation */
    PPA_LL_BLEND_TRANS_MODE_FILL,           /*!< Blending engine performs fill operation */
    PPA_LL_BLEND_TRANS_MODE_BYPASS_BLEND,   /*!< Blending engine bypasses blender, background will be output directly. Useful for color space conversion (ARGB). */
} ppa_ll_blend_trans_mode_t;

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Enable or disable the PPA peripheral bus clock
 *
 * @param enable    Set true to enable, false to disable
 */
static inline void ppa_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_ppa_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define ppa_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        ppa_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the PPA module
 */
static inline void ppa_ll_reset_register(void)
{
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_ppa = 1;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_ppa = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define ppa_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        ppa_ll_reset_register(__VA_ARGS__); \
    } while(0)

///////////////////////// Scaling, Rotating, Mirroring (SRM) //////////////////////////////
/**
 * @brief Reset PPA scaling-rotating-mirroring engine
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_srm_reset(ppa_dev_t *dev)
{
    dev->sr_scal_rotate.scal_rotate_rst = 1;
    dev->sr_scal_rotate.scal_rotate_rst = 0;
}

/**
 * @brief Set PPA scaling coefficient in X direction
 *
 * @param dev Peripheral instance address
 * @param x_int The integrated part of scaling coefficient in X direction, 0 - 255
 * @param x_frag The fragment part of scaling coefficient in X direction, 0 - 15. Corresponding fractional value is x_frag/16.
 */
static inline void ppa_ll_srm_set_scaling_x(ppa_dev_t *dev, uint32_t x_int, uint32_t x_frag)
{
    HAL_ASSERT(x_int <= PPA_SR_SCAL_X_INT_V && x_frag <= PPA_SR_SCAL_X_FRAG_V);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->sr_scal_rotate, sr_scal_x_int, x_int);
    dev->sr_scal_rotate.sr_scal_x_frag = x_frag;
}

/**
 * @brief Set PPA scaling coefficient in Y direction
 *
 * @param dev Peripheral instance address
 * @param y_int The integrated part of scaling coefficient in Y direction, 0 - 255
 * @param y_frag The fragment part of scaling coefficient in Y direction, 0 - 15. Corresponding fractional value is y_frag/16.
 */
static inline void ppa_ll_srm_set_scaling_y(ppa_dev_t *dev, uint32_t y_int, uint32_t y_frag)
{
    HAL_ASSERT(y_int <= PPA_SR_SCAL_Y_INT_V && y_frag <= PPA_SR_SCAL_Y_FRAG_V);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->sr_scal_rotate, sr_scal_y_int, y_int);
    dev->sr_scal_rotate.sr_scal_y_frag = y_frag;
}

/**
 * @brief Set PPA rotation angle (in the counterclockwise direction)
 *
 * @param dev Peripheral instance address
 * @param angle One of the values in ppa_srm_rotation_angle_t
 */
static inline void ppa_ll_srm_set_rotation_angle(ppa_dev_t *dev, ppa_srm_rotation_angle_t angle)
{
    uint32_t val = 0;
    switch (angle) {
    case PPA_SRM_ROTATION_ANGLE_0:
        val = 0;
        break;
    case PPA_SRM_ROTATION_ANGLE_90:
        val = 1;
        break;
    case PPA_SRM_ROTATION_ANGLE_180:
        val = 2;
        break;
    case PPA_SRM_ROTATION_ANGLE_270:
        val = 3;
        break;
    default:
        // Unsupported rotation angle
        abort();
    }
    dev->sr_scal_rotate.sr_rotate_angle = val;
}

/**
 * @brief Enable PPA image mirror in X direction
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_srm_enable_mirror_x(ppa_dev_t *dev, bool enable)
{
    dev->sr_scal_rotate.sr_mirror_x = enable;
}

/**
 * @brief Enable PPA image mirror in Y direction
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_srm_enable_mirror_y(ppa_dev_t *dev, bool enable)
{
    dev->sr_scal_rotate.sr_mirror_y = enable;
}

/**
 * @brief Start PPA scaling and rotating engine to perform PPA SRM
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_srm_start(ppa_dev_t *dev)
{
    dev->sr_scal_rotate.scal_rotate_start = 1;
}

/**
 * @brief Set the source image color mode for PPA Scaling-Rotating-Mirroring engine RX
 *
 * @param dev Peripheral instance address
 * @param color_mode One of the values in ppa_srm_color_mode_t
 */
static inline void ppa_ll_srm_set_rx_color_mode(ppa_dev_t *dev, ppa_srm_color_mode_t color_mode)
{
    uint32_t val = 0;
    switch (color_mode) {
    case PPA_SRM_COLOR_MODE_ARGB8888:
        val = 0;
        break;
    case PPA_SRM_COLOR_MODE_RGB888:
        val = 1;
        break;
    case PPA_SRM_COLOR_MODE_RGB565:
        val = 2;
        break;
    case PPA_SRM_COLOR_MODE_YUV420:
        val = 8;
        break;
    default:
        // Unsupported SRM rx color mode
        abort();
    }
    dev->sr_color_mode.sr_rx_cm = val;
}

/**
 * @brief Set the destination image color mode for PPA Scaling-Rotating-Mirroring engine TX
 *
 * @param dev Peripheral instance address
 * @param color_mode One of the values in ppa_srm_color_mode_t
 */
static inline void ppa_ll_srm_set_tx_color_mode(ppa_dev_t *dev, ppa_srm_color_mode_t color_mode)
{
    uint32_t val = 0;
    switch (color_mode) {
    case PPA_SRM_COLOR_MODE_ARGB8888:
        val = 0;
        break;
    case PPA_SRM_COLOR_MODE_RGB888:
        val = 1;
        break;
    case PPA_SRM_COLOR_MODE_RGB565:
        val = 2;
        break;
    case PPA_SRM_COLOR_MODE_YUV420:
        val = 8;
        break;
    default:
        // Unsupported SRM tx color mode
        abort();
    }
    dev->sr_color_mode.sr_tx_cm = val;
}

/**
 * @brief Set YUV to RGB protocol when PPA SRM RX pixel color space is YUV
 *
 * @param dev Peripheral instance address
 * @param std One of the RGB-YUV conversion standards in ppa_color_conv_std_rgb_yuv_t
 */
static inline void ppa_ll_srm_set_rx_yuv2rgb_std(ppa_dev_t *dev, ppa_color_conv_std_rgb_yuv_t std)
{
    switch (std) {
    case PPA_COLOR_CONV_STD_RGB_YUV_BT601:
        dev->sr_color_mode.yuv2rgb_protocol = 0;
        break;
    case PPA_COLOR_CONV_STD_RGB_YUV_BT709:
        dev->sr_color_mode.yuv2rgb_protocol = 1;
        break;
    default:
        // Unsupported RGB-YUV conversion standard
        abort();
    }
}

/**
 * @brief Set RGB to YUV protocol when PPA SRM TX pixel color space is YUV
 *
 * @param dev Peripheral instance address
 * @param std One of the RGB-YUV conversion standards in ppa_color_conv_std_rgb_yuv_t
 */
static inline void ppa_ll_srm_set_tx_rgb2yuv_std(ppa_dev_t *dev, ppa_color_conv_std_rgb_yuv_t std)
{
    switch (std) {
    case PPA_COLOR_CONV_STD_RGB_YUV_BT601:
        dev->sr_color_mode.rgb2yuv_protocol = 0;
        break;
    case PPA_COLOR_CONV_STD_RGB_YUV_BT709:
        dev->sr_color_mode.rgb2yuv_protocol = 1;
        break;
    default:
        // Unsupported RGB-YUV conversion standard
        abort();
    }
}

/**
 * @brief Set PPA SRM YUV input range
 *
 * @param dev Peripheral instance address
 * @param range One of color range options in ppa_color_range_t
 */
static inline void ppa_ll_srm_set_rx_yuv_range(ppa_dev_t *dev, ppa_color_range_t range)
{
    switch (range) {
    case PPA_COLOR_RANGE_LIMIT:
        dev->sr_color_mode.yuv_rx_range = 0;
        break;
    case PPA_COLOR_RANGE_FULL:
        dev->sr_color_mode.yuv_rx_range = 1;
        break;
    default:
        // Unsupported color range
        abort();
    }
}

/**
 * @brief Set PPA SRM YUV output range
 *
 * @param dev Peripheral instance address
 * @param range One of color range options in ppa_color_range_t
 */
static inline void ppa_ll_srm_set_tx_yuv_range(ppa_dev_t *dev, ppa_color_range_t range)
{
    switch (range) {
    case PPA_COLOR_RANGE_LIMIT:
        dev->sr_color_mode.yuv_tx_range = 0;
        break;
    case PPA_COLOR_RANGE_FULL:
        dev->sr_color_mode.yuv_tx_range = 1;
        break;
    default:
        // Unsupported color range
        abort();
    }
}

/**
 * @brief Enable PPA SRM input data swap in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR)
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_srm_enable_rx_rgb_swap(ppa_dev_t *dev, bool enable)
{
    dev->sr_byte_order.sr_rx_rgb_swap_en = enable;
}

/**
 * @brief Enable PPA SRM input data swap in byte (The Byte0 and Byte1 would be swapped while byte 2 and byte 3 would be swappped)
 *
 * Only supported when input color mode is ARGB8888 or RGB565.
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_srm_enable_rx_byte_swap(ppa_dev_t *dev, bool enable)
{
    dev->sr_byte_order.sr_rx_byte_swap_en = enable;
}

/**
 * @brief Configure PPA SRM alpha value update mode
 *
 * @param dev Peripheral instance address
 * @param mode Alpha value update mode, one of the values in ppa_alpha_update_mode_t
 * @param val When PPA_ALPHA_FIX_VALUE mode is selected, val is the alpha value to be replaced with (output_alpha = val)
 *            When PPA_ALPHA_SCALE mode is selected, val/256 is the multiplier to the input alpha value (output_alpha = input_alpha * val / 256)
 *            When other modes are selected, this field is not used
 */
static inline void ppa_ll_srm_configure_rx_alpha(ppa_dev_t *dev, ppa_alpha_update_mode_t mode, uint32_t val)
{
    switch (mode) {
    case PPA_ALPHA_NO_CHANGE:
        dev->sr_fix_alpha.sr_rx_alpha_mod = 0;
        dev->sr_fix_alpha.sr_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_FIX_VALUE:
        dev->sr_fix_alpha.sr_rx_alpha_mod = 1;
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->sr_fix_alpha, sr_rx_fix_alpha, val);
        dev->sr_fix_alpha.sr_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_SCALE:
        dev->sr_fix_alpha.sr_rx_alpha_mod = 2;
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->sr_fix_alpha, sr_rx_fix_alpha, val);
        dev->sr_fix_alpha.sr_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_INVERT:
        dev->sr_fix_alpha.sr_rx_alpha_mod = 0;
        dev->sr_fix_alpha.sr_rx_alpha_inv = 1;
        break;
    default:
        // Unsupported alpha update mode
        abort();
    }
}

//////////////////////////////////// Blending ////////////////////////////////////////
/*
 * Alpha Blending Calculation:
 * 1. A_bg, A_fg first convert to A_bg', A_fg' according to blend_fix_alpha register configuration
 * 2. Then do blending
 *    A_out = A_bg' + A_fg' - A_bg' * A_fg'
 *    C_out_r/g/b = (C_bg_r/g/b * A_bg' * (1 - A_fg') + C_fg_r/g/b * A_fg') / A_out
 */

/**
 * @brief Reset PPA blending engine
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_blend_reset(ppa_dev_t *dev)
{
    dev->blend_trans_mode.blend_rst = 1;
    dev->blend_trans_mode.blend_rst = 0;
}

/**
 * @brief Start PPA blending engine to perform PPA Blending
 *
 * @param dev Peripheral instance address
 * @param trans_mode Blending engine transfer mode, one of the values in ppa_ll_blend_trans_mode_t
 */
static inline void ppa_ll_blend_start(ppa_dev_t *dev, ppa_ll_blend_trans_mode_t trans_mode)
{
    switch (trans_mode) {
    case PPA_LL_BLEND_TRANS_MODE_BLEND:
        dev->blend_trans_mode.blend_bypass = 0;
        dev->blend_trans_mode.blend_fix_pixel_fill_en = 0;
        break;
    case PPA_LL_BLEND_TRANS_MODE_FILL:
        dev->blend_trans_mode.blend_bypass = 0;
        dev->blend_trans_mode.blend_fix_pixel_fill_en = 1;
        break;
    case PPA_LL_BLEND_TRANS_MODE_BYPASS_BLEND:
        dev->blend_trans_mode.blend_bypass = 1;
        dev->blend_trans_mode.blend_fix_pixel_fill_en = 0;
        break;
    default:
        // Unsupported blending trans mode
        abort();
    }
    dev->blend_trans_mode.blend_en = 1;
    dev->blend_trans_mode.blend_trans_mode_update = 1;
}

/**
 * @brief Set the source image color mode for background for PPA blending engine RX
 *
 * @param dev Peripheral instance address
 * @param color_mode One of the values in ppa_blend_color_mode_t (some color modes may not be applicable to rx bg)
 */
static inline void ppa_ll_blend_set_rx_bg_color_mode(ppa_dev_t *dev, ppa_blend_color_mode_t color_mode)
{
    uint32_t val = 0;
    switch (color_mode) {
    case PPA_BLEND_COLOR_MODE_ARGB8888:
        val = 0;
        break;
    case PPA_BLEND_COLOR_MODE_RGB888:
        val = 1;
        break;
    case PPA_BLEND_COLOR_MODE_RGB565:
        val = 2;
        break;
    // case PPA_BLEND_COLOR_MODE_L8:
    //     val = 4;
    //     break;
    // case PPA_BLEND_COLOR_MODE_L4:
    //     val = 5;
    //     break;
    default:
        // Unsupported blending rx background color mode
        abort();
    }
    dev->blend_color_mode.blend0_rx_cm = val;
}

/**
 * @brief Set the source image color mode for foreground for PPA blending engine RX
 *
 * @param dev Peripheral instance address
 * @param color_mode One of the values in ppa_blend_color_mode_t
 */
static inline void ppa_ll_blend_set_rx_fg_color_mode(ppa_dev_t *dev, ppa_blend_color_mode_t color_mode)
{
    uint32_t val = 0;
    switch (color_mode) {
    case PPA_BLEND_COLOR_MODE_ARGB8888:
        val = 0;
        break;
    case PPA_BLEND_COLOR_MODE_RGB888:
        val = 1;
        break;
    case PPA_BLEND_COLOR_MODE_RGB565:
        val = 2;
        break;
    // case PPA_BLEND_COLOR_MODE_L8:
    //     val = 4;
    //     break;
    // case PPA_BLEND_COLOR_MODE_L4:
    //     val = 5;
    //     break;
    case PPA_BLEND_COLOR_MODE_A8:
        val = 6;
        break;
    case PPA_BLEND_COLOR_MODE_A4:
        val = 7;
        break;
    default:
        // Unsupported blending rx foreground color mode
        abort();
    }
    dev->blend_color_mode.blend1_rx_cm = val;
}

/**
 * @brief Set the destination image color mode for PPA blending engine TX
 *
 * @param dev Peripheral instance address
 * @param color_mode One of the values in ppa_blend_color_mode_t (some color modes may not be applicable to tx)
 */
static inline void ppa_ll_blend_set_tx_color_mode(ppa_dev_t *dev, ppa_blend_color_mode_t color_mode)
{
    uint32_t val = 0;
    switch (color_mode) {
    case PPA_BLEND_COLOR_MODE_ARGB8888:
        val = 0;
        break;
    case PPA_BLEND_COLOR_MODE_RGB888:
        val = 1;
        break;
    case PPA_BLEND_COLOR_MODE_RGB565:
        val = 2;
        break;
    default:
        // Unsupported blending tx color mode
        abort();
    }
    dev->blend_color_mode.blend_tx_cm = val;
}

/**
 * @brief Enable PPA blending input background data wrap in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR)
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_blend_enable_rx_bg_rgb_swap(ppa_dev_t *dev, bool enable)
{
    dev->blend_byte_order.blend0_rx_rgb_swap_en = enable;
}

/**
 * @brief Enable PPA blending input foreground data wrap in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR)
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_blend_enable_rx_fg_rgb_swap(ppa_dev_t *dev, bool enable)
{
    dev->blend_byte_order.blend1_rx_rgb_swap_en = enable;
}

/**
 * @brief Enable PPA blending input background data swap in byte (The Byte0 and Byte1 would be swapped while byte 2 and byte 3 would be swappped)
 *
 * Only supported when input color mode is ARGB8888 or RGB565.
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_blend_enable_rx_bg_byte_swap(ppa_dev_t *dev, bool enable)
{
    dev->blend_byte_order.blend0_rx_byte_swap_en = enable;
}

/**
 * @brief Enable PPA blending input foreground data swap in byte (The Byte0 and Byte1 would be swapped while byte 2 and byte 3 would be swappped)
 *
 * Only supported when input color mode is ARGB8888 or RGB565.
 *
 * @param dev Peripheral instance address
 * @param enable True to enable; False to disable
 */
static inline void ppa_ll_blend_enable_rx_fg_byte_swap(ppa_dev_t *dev, bool enable)
{
    dev->blend_byte_order.blend1_rx_byte_swap_en = enable;
}

/**
 * @brief Configure PPA blending input background alpha value update mode
 *
 * @param dev Peripheral instance address
 * @param mode Alpha value update mode, one of the values in ppa_alpha_update_mode_t
 * @param val When PPA_ALPHA_FIX_VALUE mode is selected, val is the alpha value to be replaced with (output_alpha = val)
 *            When PPA_ALPHA_SCALE mode is selected, val/256 is the multiplier to the input alpha value (output_alpha = input_alpha * val / 256)
 *            When other modes are selected, this field is not used
 */
static inline void ppa_ll_blend_configure_rx_bg_alpha(ppa_dev_t *dev, ppa_alpha_update_mode_t mode, uint32_t val)
{
    switch (mode) {
    case PPA_ALPHA_NO_CHANGE:
        dev->blend_fix_alpha.blend0_rx_alpha_mod = 0;
        dev->blend_fix_alpha.blend0_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_FIX_VALUE:
        dev->blend_fix_alpha.blend0_rx_alpha_mod = 1;
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_fix_alpha, blend0_rx_fix_alpha, val);
        dev->blend_fix_alpha.blend0_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_SCALE:
        dev->blend_fix_alpha.blend0_rx_alpha_mod = 2;
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_fix_alpha, blend0_rx_fix_alpha, val);
        dev->blend_fix_alpha.blend0_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_INVERT:
        dev->blend_fix_alpha.blend0_rx_alpha_mod = 0;
        dev->blend_fix_alpha.blend0_rx_alpha_inv = 1;
        break;
    default:
        // Unsupported alpha update mode
        abort();
    }
}

/**
 * @brief Configure PPA blending input foreground alpha value update mode
 *
 * @param dev Peripheral instance address
 * @param mode Alpha value update mode, one of the values in ppa_alpha_update_mode_t
 * @param val When PPA_ALPHA_FIX_VALUE mode is selected, val is the alpha value to be replaced with (output_alpha = val)
 *            When PPA_ALPHA_SCALE mode is selected, val/256 is the multiplier to the input alpha value (output_alpha = input_alpha * val / 256)
 *            When other modes are selected, this field is not used
 */
static inline void ppa_ll_blend_configure_rx_fg_alpha(ppa_dev_t *dev, ppa_alpha_update_mode_t mode, uint32_t val)
{
    switch (mode) {
    case PPA_ALPHA_NO_CHANGE:
        dev->blend_fix_alpha.blend1_rx_alpha_mod = 0;
        dev->blend_fix_alpha.blend1_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_FIX_VALUE:
        dev->blend_fix_alpha.blend1_rx_alpha_mod = 1;
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_fix_alpha, blend1_rx_fix_alpha, val);
        dev->blend_fix_alpha.blend1_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_SCALE:
        dev->blend_fix_alpha.blend1_rx_alpha_mod = 2;
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_fix_alpha, blend1_rx_fix_alpha, val);
        dev->blend_fix_alpha.blend1_rx_alpha_inv = 0;
        break;
    case PPA_ALPHA_INVERT:
        dev->blend_fix_alpha.blend1_rx_alpha_mod = 0;
        dev->blend_fix_alpha.blend1_rx_alpha_inv = 1;
        break;
    default:
        // Unsupported alpha update mode
        abort();
    }
}

/**
 * @brief Configure PPA blending pixel filling image block
 *
 * @param dev Peripheral instance address
 * @param data The fix data to be filled to the image block pixels in ARGB8888 format
 * @param hb The horizontal width of image block that would be filled in fix pixel filling mode. The unit is pixel.
 * @param vb The vertical height of image block that would be filled in fix pixel filling mode. The unit is pixel.
 */
static inline void ppa_ll_blend_configure_filling_block(ppa_dev_t *dev, color_pixel_argb8888_data_t *data, uint32_t hb, uint32_t vb)
{
    HAL_ASSERT(hb <= PPA_BLEND_HB_V && vb <= PPA_BLEND_VB_V);
    dev->blend_fix_pixel.blend_tx_fix_pixel = data->val;
    dev->blend_tx_size.blend_hb = hb;
    dev->blend_tx_size.blend_vb = vb;
}

/**
 * @brief Configure PPA blending input foreground fixed RGB color for A4/A8 color mode
 *
 * @param dev Peripheral instance address
 * @param rgb RGB color for A4/A8 mode in RGB888 format
 */
static inline void ppa_ll_blend_set_rx_fg_fix_rgb(ppa_dev_t *dev, color_pixel_rgb888_data_t *rgb)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_rgb, blend1_rx_b, rgb->b);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_rgb, blend1_rx_g, rgb->g);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->blend_rgb, blend1_rx_r, rgb->r);
}

/*
 * Color-key Calculation:
 * 1) When FG in ck range, but BG not in ck range, then outputs BG pixel value.
 * 2) When BG in ck range, but FG not in ck range, then depends on FG_BG_REVERSE value: if FG_BG_REVERSE = 0, outputs BG pixel value; otherwise, outputs FG pixel value.
 * 3) When FG and BG both in their ck ranges, then outputs ck default color.
 * 4) When FG and BG both not in their ck ranges, then it goes through normal Alpha Blending calculation.
 */

/**
 * @brief Configure PPA blending input background color-key threshold
 *
 * @param dev Peripheral instance address
 * @param rgb_thres_low Color-key lower threshold of background in RGB888 format
 * @param rgb_thres_high Color-key higher threshold of background in RGB888 format
 */
static inline void ppa_ll_blend_configure_rx_bg_ck_range(ppa_dev_t *dev, color_pixel_rgb888_data_t *rgb_thres_low, color_pixel_rgb888_data_t *rgb_thres_high)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_bg_low, colorkey_bg_b_low, rgb_thres_low->b);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_bg_low, colorkey_bg_g_low, rgb_thres_low->g);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_bg_low, colorkey_bg_r_low, rgb_thres_low->r);

    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_bg_high, colorkey_bg_b_high, rgb_thres_high->b);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_bg_high, colorkey_bg_g_high, rgb_thres_high->g);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_bg_high, colorkey_bg_r_high, rgb_thres_high->r);
}

/**
 * @brief Configure PPA blending input foreground color-key threshold
 *
 * @param dev Peripheral instance address
 * @param rgb_thres_low Color-key lower threshold of foreground in RGB888 format
 * @param rgb_thres_high Color-key higher threshold of foreground in RGB888 format
 */
static inline void ppa_ll_blend_configure_rx_fg_ck_range(ppa_dev_t *dev, color_pixel_rgb888_data_t *rgb_thres_low, color_pixel_rgb888_data_t *rgb_thres_high)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_fg_low, colorkey_fg_b_low, rgb_thres_low->b);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_fg_low, colorkey_fg_g_low, rgb_thres_low->g);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_fg_low, colorkey_fg_r_low, rgb_thres_low->r);

    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_fg_high, colorkey_fg_b_high, rgb_thres_high->b);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_fg_high, colorkey_fg_g_high, rgb_thres_high->g);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_fg_high, colorkey_fg_r_high, rgb_thres_high->r);
}

/**
 * @brief Configure PPA blending default RGB value when foreground and background both in color-key range
 *
 * @param dev Peripheral instance address
 * @param rgb Default RGB value in RGB888 format
 */
static inline void ppa_ll_blend_set_ck_default_rgb(ppa_dev_t *dev, color_pixel_rgb888_data_t *rgb)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_default, colorkey_default_b, rgb->b);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_default, colorkey_default_g, rgb->g);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ck_default, colorkey_default_r, rgb->r);
}

/**
 * @brief Configure whether to reverse the output when PPA blending pixel in background color-key range but not in foreground color-key range
 *
 * @param dev Peripheral instance address
 * @param enable True then output the foreground pixel value; False then output the background pixel value
 */
static inline void ppa_ll_blend_enable_ck_fg_bg_reverse(ppa_dev_t *dev, bool enable)
{
    dev->ck_default.colorkey_fg_bg_reverse = enable;
}

/////////////////////////// Color Look-Up Table (CLUT) ///////////////////////////////
/**
 * @brief Configure PPA CLUT mem access mode
 *
 * @param dev Peripheral instance address
 * @param fifo_mode True to access through fifo mode; False to access through memory mode
 */
static inline void ppa_ll_configure_clut_access_mode(ppa_dev_t *dev, bool fifo_mode)
{
    dev->clut_conf.apb_fifo_mask = !fifo_mode;
}

/**
 * @brief Force clock and power on for PPA blending BLEND CLUT mem
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_blend_enable_clut_mem(ppa_dev_t *dev)
{
    dev->clut_conf.blend_clut_mem_clk_ena = 1;
    dev->clut_conf.blend_clut_mem_force_pu = 1; // force clut mem remain power when memory domain PD
}

/**
 * @brief Force clock and power off for PPA blending BLEND CLUT mem
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_blend_disable_clut_mem(ppa_dev_t *dev)
{
    dev->clut_conf.blend_clut_mem_clk_ena = 0;
    dev->clut_conf.blend_clut_mem_force_pu = 0;
}

/**
 * @brief Reset PPA blending BLEND0 CLUT mem and read address
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_blend_reset_rx_bg_clut_mem(ppa_dev_t *dev)
{
    dev->clut_conf.blend0_clut_mem_rst = 1;
    dev->clut_conf.blend0_clut_mem_rst = 0;
    dev->clut_conf.blend0_clut_mem_rdaddr_rst = 1;
    dev->clut_conf.blend0_clut_mem_rdaddr_rst = 0;
}

/**
 * @brief Reset PPA blending BLEND1 CLUT mem and read address
 *
 * @param dev Peripheral instance address
 */
static inline void ppa_ll_blend_reset_rx_fg_clut_mem(ppa_dev_t *dev)
{
    dev->clut_conf.blend1_clut_mem_rst = 1;
    dev->clut_conf.blend1_clut_mem_rst = 0;
    dev->clut_conf.blend1_clut_mem_rdaddr_rst = 1;
    dev->clut_conf.blend1_clut_mem_rdaddr_rst = 0;
}

/**
 * @brief Write PPA blending BLEND0 CLUT mem entry through FIFO mode
 *
 * @param dev Peripheral instance address
 * @param data The data to be written into BLEND0 CLUT entry in ARGB8888 format
 */
static inline void ppa_ll_blend_wr_rx_bg_clut_data_by_fifo(ppa_dev_t *dev, uint32_t data)
{
    dev->blend0_clut_data.rdwr_word_blend0_clut = data;
}

/**
 * @brief Write PPA blending BLEND1 CLUT mem entry through FIFO mode
 *
 * @param dev Peripheral instance address
 * @param data The data to be written into BLEND1 CLUT entry in ARGB8888 format
 */
static inline void ppa_ll_blend_wr_rx_fg_clut_data_by_fifo(ppa_dev_t *dev, uint32_t data)
{
    dev->blend1_clut_data.rdwr_word_blend1_clut = data;
}

/**
 * @brief Write PPA blending BLEND0 CLUT mem entry through memory mode
 *
 * @param dev Peripheral instance address
 * @param idx Entry index to the CLUT mem
 * @param data The data to be written into BLEND0 CLUT entry in ARGB8888 format
 */
static inline void ppa_ll_blend_wr_rx_bg_clut_data_by_mem(ppa_dev_t *dev, uint32_t idx, uint32_t data)
{
    volatile uint32_t *blend0_clut_mem = (uint32_t *)((uint32_t)dev + PPA_LL_BLEND0_CLUT_MEM_ADDR_OFFSET);
    blend0_clut_mem[idx] = data;
}

/**
 * @brief Write PPA blending BLEND1 CLUT mem entry through memory mode
 *
 * @param dev Peripheral instance address
 * @param idx Entry index to the CLUT mem
 * @param data The data to be written into BLEND1 CLUT entry in ARGB8888 format
 */
static inline void ppa_ll_blend_wr_rx_fg_clut_data_by_mem(ppa_dev_t *dev, uint32_t idx, uint32_t data)
{
    volatile uint32_t *blend1_clut_mem = (uint32_t *)((uint32_t)dev + PPA_LL_BLEND1_CLUT_MEM_ADDR_OFFSET);
    blend1_clut_mem[idx] = data;
}

#ifdef __cplusplus
}
#endif
