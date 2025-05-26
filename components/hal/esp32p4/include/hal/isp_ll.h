/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/hal_utils.h"
#include "hal/isp_types.h"
#include "hal/color_types.h"
#include "soc/isp_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ISP_LL_GET_HW(num)                    (((num) == 0) ? (&ISP) : NULL)

#define ISP_LL_HSIZE_MAX                      1920
#define ISP_LL_VSIZE_MAX                      1080

/*---------------------------------------------------------------
                      Clock
---------------------------------------------------------------*/
#define ISP_LL_TX_MAX_CLK_INT_DIV             0x100


/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
#define ISP_LL_EVENT_DATA_TYPE_ERR            (1<<0)
#define ISP_LL_EVENT_ASYNC_FIFO_OVF           (1<<1)
#define ISP_LL_EVENT_BUF_FULL                 (1<<2)
#define ISP_LL_EVENT_HVNUM_SETTING_ERR        (1<<3)
#define ISP_LL_EVENT_DATA_TYPE_SETTING_ERR    (1<<4)
#define ISP_LL_EVENT_MIPI_HNUM_UNMATCH        (1<<5)
#define ISP_LL_EVENT_DPC_CHECK_DONE           (1<<6)
#define ISP_LL_EVENT_GAMMA_XCOORD_ERR         (1<<7)
#define ISP_LL_EVENT_AE_ENV                   (1<<8)
#define ISP_LL_EVENT_AE_FDONE                 (1<<9)
#define ISP_LL_EVENT_AF_FDONE                 (1<<10)
#define ISP_LL_EVENT_AF_ENV                   (1<<11)
#define ISP_LL_EVENT_AWB_FDONE                (1<<12)
#define ISP_LL_EVENT_HIST_FDONE               (1<<13)
#define ISP_LL_EVENT_FRAME                    (1<<14)
#define ISP_LL_EVENT_BLC_FRAME                (1<<15)
#define ISP_LL_EVENT_LSC_FRAME                (1<<16)
#define ISP_LL_EVENT_DPC_FRAME                (1<<17)
#define ISP_LL_EVENT_BF_FRAME                 (1<<18)
#define ISP_LL_EVENT_DEMOSAIC_FRAME           (1<<19)
#define ISP_LL_EVENT_MEDIAN_FRAME             (1<<20)
#define ISP_LL_EVENT_CCM_FRAME                (1<<21)
#define ISP_LL_EVENT_GAMMA_FRAME              (1<<22)
#define ISP_LL_EVENT_RGB2YUV_FRAME            (1<<23)
#define ISP_LL_EVENT_SHARP_FRAME              (1<<24)
#define ISP_LL_EVENT_COLOR_FRAME              (1<<25)
#define ISP_LL_EVENT_YUV2RGB_FRAME            (1<<26)
#define ISP_LL_EVENT_TAIL_IDI_FRAME           (1<<27)
#define ISP_LL_EVENT_HEADER_IDI_FRAME         (1<<28)

#define ISP_LL_EVENT_ALL_MASK                 (0x1FFFFFFF)
#define ISP_LL_EVENT_AF_MASK                  (ISP_LL_EVENT_AF_FDONE | ISP_LL_EVENT_AF_ENV)
#define ISP_LL_EVENT_AE_MASK                  (ISP_LL_EVENT_AE_FDONE | ISP_LL_EVENT_AE_ENV)
#define ISP_LL_EVENT_AWB_MASK                 (ISP_LL_EVENT_AWB_FDONE)
#define ISP_LL_EVENT_SHARP_MASK               (ISP_LL_EVENT_SHARP_FRAME)
#define ISP_LL_EVENT_HIST_MASK                (ISP_LL_EVENT_HIST_FDONE)
#define ISP_LL_EVENT_COLOR_MASK               (ISP_LL_EVENT_COLOR_FRAME)

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
#define ISP_LL_AF_WINDOW_MAX_RANGE            ((1<<12) - 1)

/*---------------------------------------------------------------
                      AE
---------------------------------------------------------------*/
#define ISP_LL_AE_WINDOW_MAX_RANGE            ((1<<12) - 1)

/*---------------------------------------------------------------
                      AWB
---------------------------------------------------------------*/
#define ISP_LL_AWB_WINDOW_MAX_RANGE    ((1<<12) - 1)
#define ISP_LL_AWB_LUM_MAX_RANGE       ((1<<10) - 1)
#define ISP_LL_AWB_RGB_RATIO_INT_BITS  (2)
#define ISP_LL_AWB_RGB_RATIO_FRAC_BITS (8)

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
#define ISP_LL_BF_DEFAULT_TEMPLATE_VAL        15

/*---------------------------------------------------------------
                      DVP
---------------------------------------------------------------*/
#define ISP_LL_DVP_DATA_TYPE_RAW8     0x2A
#define ISP_LL_DVP_DATA_TYPE_RAW10    0x2B
#define ISP_LL_DVP_DATA_TYPE_RAW12    0x2C

/*---------------------------------------------------------------
                      Color
---------------------------------------------------------------*/
#define ISP_LL_COLOR_CONTRAST_MAX       0xff
#define ISP_LL_COLOR_SATURATION_MAX     0xff
#define ISP_LL_COLOR_HUE_MAX            360
#define ISP_LL_COLOR_BRIGNTNESS_MIN     -128
#define ISP_LL_COLOR_BRIGNTNESS_MAX     127

/*---------------------------------------------------------------
                      LSC
---------------------------------------------------------------*/
#define ISP_LL_LSC_GRID_HEIGHT        32
#define ISP_LL_LSC_GRID_WIDTH         32

/*---------------------------------------------------------------
                      CCM
---------------------------------------------------------------*/
#define ISP_LL_CCM_MATRIX_INT_BITS      (2)
#define ISP_LL_CCM_MATRIX_FRAC_BITS     (10)
#define ISP_LL_CCM_MATRIX_TOT_BITS      (ISP_LL_CCM_MATRIX_INT_BITS + ISP_LL_CCM_MATRIX_FRAC_BITS + 1)  // including one sign bit

typedef union {
    struct {
        uint32_t fraction: ISP_LL_AWB_RGB_RATIO_FRAC_BITS;
        uint32_t integer: ISP_LL_AWB_RGB_RATIO_INT_BITS;
        uint32_t sign: 1;
    };
    uint32_t val;
} isp_ll_ccm_gain_t;

/**
 * @brief Env detector mode
 */
typedef enum {
    ISP_LL_AF_ENV_DETECTOR_MODE_ABS,    ///< Use absolute val for threshold
    ISP_LL_AF_ENV_DETECTOR_MODE_RATIO,  ///< Use ratio val for threshold
} isp_ll_af_env_detector_mode_t;

/**
 * @brief Edge detector mode
 */
typedef enum {
    ISP_LL_AF_EDGE_DETECTOR_MODE_AUTO,      ///< Auto set threshold
    ISP_LL_AF_EDGE_DETECTOR_MODE_MANUAL,    ///< Manual set threshold
} isp_ll_af_edge_detector_mode_t;

typedef union {
    struct {
        uint32_t fraction: ISP_LL_AWB_RGB_RATIO_FRAC_BITS;
        uint32_t integer: ISP_LL_AWB_RGB_RATIO_INT_BITS;
    };
    uint32_t val;
} isp_ll_awb_rgb_ratio_t;

/**
 * @brief ISP LUT
 */
typedef enum {
    ISP_LL_LUT_LSC,    ///< LUT for LSC
    ISP_LL_LUT_DPC,    ///< LUT for DPC
} isp_ll_lut_t;

/**
 * @brief ISP pipeline clock control mode
 */
typedef enum {
    ISP_LL_PIPELINE_CLK_CTRL_AUTO,         ///< HW control, off when in frame interval
    ISP_LL_PIPELINE_CLK_CTRL_ALWAYS_ON,    ///< Always on
} isp_ll_pipeline_clk_ctrl_t;


/*---------------------------------------------------------------
                      Clock
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for ISP module
 *
 * @param hw    Hardware instance address
 * @param en    enable / disable
 */
static inline void isp_ll_enable_module_clock(isp_dev_t *hw, bool en)
{
    HP_SYS_CLKRST.peri_clk_ctrl25.reg_isp_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define isp_ll_enable_module_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        isp_ll_enable_module_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the ISP module
 *
 * @param hw    Hardware instance address
 */
static inline void isp_ll_reset_module_clock(isp_dev_t *hw)
{
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_isp = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_isp = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define isp_ll_reset_module_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        isp_ll_reset_module_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Select ISP clock source
 *
 * @param hw       Hardware instance address
 * @param clk_src  clock source, see valid sources in type `soc_periph_isp_clk_src_t`
 */
static inline void isp_ll_select_clk_source(isp_dev_t *hw, soc_periph_isp_clk_src_t clk_src)
{
    uint32_t clk_val = 0;
    switch (clk_src) {
    case ISP_CLK_SRC_XTAL:
        clk_val = 0;
        break;
    case ISP_CLK_SRC_PLL160:
        clk_val = 1;
        break;
    case ISP_CLK_SRC_PLL240:
        clk_val = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }

    HP_SYS_CLKRST.peri_clk_ctrl25.reg_isp_clk_src_sel = clk_val;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define isp_ll_select_clk_source(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        isp_ll_select_clk_source(__VA_ARGS__); \
    } while(0)

/**
 * @brief Set ISP clock div
 *
 * @param hw     Hardware instance address
 * @param div    Clock division with integral and decimal part
 */
static inline void isp_ll_set_clock_div(isp_dev_t *hw, const hal_utils_clk_div_t *clk_div)
{
    HAL_ASSERT(clk_div->integer > 0 && clk_div->integer <= ISP_LL_TX_MAX_CLK_INT_DIV);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl26, reg_isp_clk_div_num, clk_div->integer - 1);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define isp_ll_set_clock_div(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        isp_ll_set_clock_div(__VA_ARGS__); \
    } while(0)

/*---------------------------------------------------------------
                      Misc
---------------------------------------------------------------*/
/**
 * @brief Init ISP
 *
 * @param[in] hw  Hardware instance address
 */
static inline void isp_ll_init(isp_dev_t *hw)
{
    hw->cntl.val = 0;
    hw->int_clr.val = ISP_LL_EVENT_ALL_MASK;
    hw->int_ena.val = ~ISP_LL_EVENT_ALL_MASK;
}

/**
 * @brief Enable / Disable ISP clock
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_clk_enable(isp_dev_t *hw, bool enable)
{
    hw->clk_en.clk_en = enable;
}

/**
 * @brief Enable / Disable ISP
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.isp_en = enable;
}

/**
 * @brief Set input data source
 *
 * @param[in] hw      Hardware instance address
 * @param[in] source  input data source, see `isp_input_data_source_t`
 */
static inline void isp_ll_set_input_data_source(isp_dev_t *hw, isp_input_data_source_t source)
{
    switch (source) {
    case ISP_INPUT_DATA_SOURCE_CSI:
        hw->cntl.isp_in_src = 0;
        hw->cntl.mipi_data_en = 1;
        break;
    case ISP_INPUT_DATA_SOURCE_DVP:
        hw->cntl.isp_in_src = 1;
        hw->cntl.mipi_data_en = 0;
        break;
    case ISP_INPUT_DATA_SOURCE_DWGDMA:
        hw->cntl.isp_in_src = 2;
        hw->cntl.mipi_data_en = 0;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set input data color format
 *
 * @param[in] hw      Hardware instance address
 * @param[in] format  color format, see `color_space_pixel_format_t`
 *
 * @return true for valid format, false for invalid format
 */
static inline bool isp_ll_set_input_data_color_format(isp_dev_t *hw, color_space_pixel_format_t format)
{
    bool valid = false;

    if (format.color_space == COLOR_SPACE_RAW) {
        switch(format.pixel_format) {
        case COLOR_PIXEL_RAW8:
            hw->cntl.isp_data_type = 0;
            valid = true;
            break;
        case COLOR_PIXEL_RAW10:
            hw->cntl.isp_data_type = 1;
            valid = true;
            break;
        case COLOR_PIXEL_RAW12:
            hw->cntl.isp_data_type = 2;
            valid = true;
            break;
        default:
            break;
        }
    }

    return valid;
}

/**
 * @brief Set input data horizontal pixel number
 *
 * @param[in] hw         Hardware instance address
 * @param[in] pixel_num  number of pixels
 */
static inline void isp_ll_set_intput_data_h_pixel_num(isp_dev_t *hw, uint32_t pixel_num)
{
    hw->frame_cfg.hadr_num = pixel_num - 1;
}

/**
 * @brief Set input data vertical row number
 *
 * @param[in] hw       Hardware instance address
 * @param[in] row_num  number of rows
 */
static inline void isp_ll_set_intput_data_v_row_num(isp_dev_t *hw, uint32_t row_num)
{
    hw->frame_cfg.vadr_num = row_num - 1;
}

/**
 * @brief Set output data color format
 *
 * @param[in] hw      Hardware instance address
 * @param[in] format  color format, see `color_space_pixel_format_t`
 *
 * @return true for valid format, false for invalid format
 */
static inline bool isp_ll_set_output_data_color_format(isp_dev_t *hw, color_space_pixel_format_t format)
{
    bool valid = false;

    if (format.color_space == COLOR_SPACE_RAW) {
        switch(format.pixel_format) {
        case COLOR_PIXEL_RAW8:
            hw->cntl.isp_out_type = 0;
            hw->cntl.demosaic_en = 0;
            hw->cntl.rgb2yuv_en = 0;
            hw->cntl.yuv2rgb_en = 0;
            valid = true;
            break;
        default:
            break;
        }
    } else if (format.color_space == COLOR_SPACE_RGB) {
        switch(format.pixel_format) {
        case COLOR_PIXEL_RGB888:
            hw->cntl.isp_out_type = 2;
            hw->cntl.demosaic_en = 1;
            hw->cntl.rgb2yuv_en = 1;
            hw->cntl.yuv2rgb_en = 1;
            valid = true;
            break;
        case COLOR_PIXEL_RGB565:
            hw->cntl.isp_out_type = 4;
            hw->cntl.demosaic_en = 1;
            hw->cntl.rgb2yuv_en = 1;
            hw->cntl.yuv2rgb_en = 1;
            valid = true;
            break;
        default:
            break;
        }
    } else if (format.color_space == COLOR_SPACE_YUV) {
        switch(format.pixel_format) {
        case COLOR_PIXEL_YUV422:
            hw->cntl.isp_out_type = 1;
            hw->cntl.demosaic_en = 1;
            hw->cntl.rgb2yuv_en = 1;
            hw->cntl.yuv2rgb_en = 0;
            valid = true;
            break;
        case COLOR_PIXEL_YUV420:
            hw->cntl.isp_out_type = 3;
            hw->cntl.demosaic_en = 1;
            hw->cntl.rgb2yuv_en = 1;
            hw->cntl.yuv2rgb_en = 0;
            valid = true;
            break;
        default:
            break;
        }
    }

    return valid;
}

/**
 * @brief Set if line start packet exists
 *
 * @param[in] hw  Hardware instance address
 * @param[in] en  Enable / Disable
 */
static inline void isp_ll_enable_line_start_packet_exist(isp_dev_t *hw, bool en)
{
    hw->frame_cfg.hsync_start_exist = en;
}

/**
 * @brief Set if line end packet exists
 *
 * @param[in] hw  Hardware instance address
 * @param[in] en  Enable / Disable
 */
static inline void isp_ll_enable_line_end_packet_exist(isp_dev_t *hw, bool en)
{
    hw->frame_cfg.hsync_end_exist = en;
}

/**
 * @brief Get if demosaic is enabled
 *
 * @param[in] hw  Hardware instance address
 *
 * @return  True: enabled
 */
static inline bool isp_ll_is_demosaic_enabled(isp_dev_t *hw)
{
    return hw->cntl.demosaic_en;
}

/**
 * @brief Get if rgb2yuv is enabled
 *
 * @param[in] hw  Hardware instance address
 *
 * @return  True: enabled
 */
static inline bool isp_ll_is_rgb2yuv_enabled(isp_dev_t *hw)
{
    return hw->cntl.rgb2yuv_en;
}

/**
 * @brief Set bayer mode
 *
 * @param[in] hw           Hardware instance address
 * @param[in] bayer_order  Bayer order
 */
static inline void isp_ll_set_bayer_mode(isp_dev_t *hw, color_raw_element_order_t bayer_order)
{
    hw->frame_cfg.bayer_mode = bayer_order;
}

/**
 * @brief Swap the data endianness order in bytes
 *
 * @param[in] hw           Hardware instance address
 * @param[in] byte_swap_en byte swap enable or not
 */
static inline void isp_ll_set_byte_swap(isp_dev_t *hw, bool byte_swap_en)
{
    hw->cntl.byte_endian_order = byte_swap_en;
}

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
/**
 * @brief Set AF clock control mode
 *
 * @param[in] hw
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_af_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_af_force_on = mode;
}

/**
 * @brief Enable / Disable AF
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_af_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.af_en = enable;
}

/**
 * @brief Enable / Disable auto aupdate AF
 *
 * @param[in] hw
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_af_enable_auto_update(isp_dev_t *hw, bool enable)
{
    hw->af_ctrl0.af_auto_update = enable;
}

/**
 * @brief Manual aupdate AF once
 *
 * @param[in] hw  Hardware instance address
 */
static inline void isp_ll_af_manual_update(isp_dev_t *hw)
{
    //self clear
    hw->af_ctrl0.af_manual_update = 1;
}

/**
 * @brief Set edge thresh mode
 *
 * @param[in] hw    Hardware instance address
 * @param[in] mode  See `isp_ll_af_edge_detector_mode_t`
 */
static inline void isp_ll_af_set_edge_thresh_mode(isp_dev_t *hw, isp_ll_af_edge_detector_mode_t mode)
{
    if (mode == ISP_LL_AF_EDGE_DETECTOR_MODE_AUTO) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->af_threshold, af_threshold, 0);
    }
}

/**
 * @brief Set edge threshold
 *
 * @param[in] hw      Hardware instance address
 * @param[in] thresh  Edge threshold
 */
static inline void isp_ll_af_set_edge_thresh(isp_dev_t *hw, uint32_t thresh)
{
    HAL_ASSERT(thresh != 0);

    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->af_threshold, af_threshold, thresh);
}

/**
 * @brief Set auto edge thresh pixel num
 *
 * @param[in] hw         Hardware instance address
 * @param[in] pixel_num  Pixel numbers
 */
static inline void isp_ll_af_set_auto_edge_thresh_pixel_num(isp_dev_t *hw, uint32_t pixel_num)
{
    HAL_ASSERT(HAL_FORCE_READ_U32_REG_FIELD(hw->af_threshold, af_threshold) == 0);

    hw->af_ctrl1.af_thpixnum = pixel_num;
}

/**
 * @brief Set window range
 *
 * @param[in] hw              Hardware instance address
 * @param[in] window_id       Window ID
 * @param[in] top_left_x      Top left pixel x axis value
 * @param[in] top_left_y      Top left pixel y axis value
 * @param[in] bottom_right_x  Bottom right pixel x axis value
 * @param[in] bottom_right_y  Bottom right pixel y axis value
 */
static inline void isp_ll_af_set_window_range(isp_dev_t *hw, uint32_t window_id, uint32_t top_left_x, uint32_t top_left_y, uint32_t bottom_right_x, uint32_t bottom_right_y)
{
    HAL_ASSERT(top_left_x < ISP_LL_AF_WINDOW_MAX_RANGE &&
               top_left_y < ISP_LL_AF_WINDOW_MAX_RANGE &&
               bottom_right_x < ISP_LL_AF_WINDOW_MAX_RANGE &&
               bottom_right_y < ISP_LL_AF_WINDOW_MAX_RANGE);

    switch (window_id) {
    case 0:
        hw->af_hscale_a.af_lpoint_a = top_left_x;
        hw->af_vscale_a.af_tpoint_a = top_left_y;
        hw->af_hscale_a.af_rpoint_a = bottom_right_x;
        hw->af_vscale_a.af_bpoint_a = bottom_right_y;
        break;
    case 1:
        hw->af_hscale_b.af_lpoint_b = top_left_x;
        hw->af_vscale_b.af_tpoint_b = top_left_y;
        hw->af_hscale_b.af_rpoint_b = bottom_right_x;
        hw->af_vscale_b.af_bpoint_b = bottom_right_y;
        break;
    case 2:
        hw->af_hscale_c.af_lpoint_c = top_left_x;
        hw->af_vscale_c.af_tpoint_c = top_left_y;
        hw->af_hscale_c.af_rpoint_c = bottom_right_x;
        hw->af_vscale_c.af_bpoint_c = bottom_right_y;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Get window sum
 *
 * @param[in] hw         Hardware instance address
 * @param[in] window_id  Window ID
 *
 * @return Window sum
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_af_get_window_sum(isp_dev_t *hw, uint32_t window_id)
{
    switch (window_id) {
    case 0:
        return hw->af_sum_a.af_suma;
    case 1:
        return hw->af_sum_b.af_sumb;
    case 2:
        return hw->af_sum_c.af_sumc;
    default:
        HAL_ASSERT(false);
        return 0;
    }
}

/**
 * @brief Get window lum
 *
 * @param[in] hw         Hardware instance address
 * @param[in] window_id  Window ID
 *
 * @return Window lum
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_af_get_window_lum(isp_dev_t *hw, uint32_t window_id)
{
    switch (window_id) {
    case 0:
        return hw->af_lum_a.af_luma;
    case 1:
        return hw->af_lum_b.af_lumb;
    case 2:
        return hw->af_lum_c.af_lumc;
    default:
        HAL_ASSERT(false);
        return 0;
    }
}

/*---------------------------------------------
                AF Env detector
----------------------------------------------*/
/**
 * @brief Set env detector period
 *
 * @param[in] hw      Hardware instance address
 * @param[in] period  period of the env detector, in frames
 */
static inline void isp_ll_af_env_detector_set_period(isp_dev_t *hw, uint32_t period)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->af_ctrl0, af_env_period, period);
}

/**
 * @brief Set env detector mode
 *
 * @param[in] hw    Hardware instance address
 * @param[in] mode  See `isp_ll_af_env_detector_mode_t`
 */
static inline void isp_ll_af_env_detector_set_mode(isp_dev_t *hw, isp_ll_af_env_detector_mode_t mode)
{
    if (mode == ISP_LL_AF_ENV_DETECTOR_MODE_RATIO) {
        hw->af_env_user_th_sum.af_env_user_threshold_sum = 0x0;
        hw->af_env_user_th_lum.af_env_user_threshold_lum = 0x0;
    }

    //nothing to do to if using abs mode, it'll be enabled after `isp_ll_af_env_detector_set_thresh()`
}

/**
 * @brief Set env detector threshold
 *
 * @param[in] hw          Hardware instance address
 * @param[in] sum_thresh  Threshold for definition
 * @param[in] lum_thresh  Threshold for luminance
 */
static inline void isp_ll_af_env_detector_set_thresh(isp_dev_t *hw, uint32_t sum_thresh, uint32_t lum_thresh)
{
    HAL_ASSERT(sum_thresh != 0 || lum_thresh != 0);

    hw->af_env_user_th_sum.af_env_user_threshold_sum = sum_thresh;
    hw->af_env_user_th_lum.af_env_user_threshold_lum = lum_thresh;
}

/**
 * @brief Set env detector ratio
 *
 * @param[in] hw         Hardware instance address
 * @param[in] ratio_val  Threshold for ratio
 */
static inline void isp_ll_af_env_detector_set_ratio(isp_dev_t *hw, uint32_t ratio_val)
{
    HAL_ASSERT(hw->af_env_user_th_sum.af_env_user_threshold_sum == 0 &&
               hw->af_env_user_th_lum.af_env_user_threshold_lum == 0);

    hw->af_ctrl0.af_env_threshold = ratio_val;
}

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
/**
 * @brief Set BF clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_bf_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_bf_force_on = mode;
}

/**
 * @brief Enable / Disable BF
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_bf_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.bf_en = enable;
}

/**
 * @brief Set ISP BF sigma value
 *
 * @param[in] hw          Hardware instance address
 * @param[in] sigmal_val  sigma value
 */
static inline void isp_ll_bf_set_sigma(isp_dev_t *hw, uint32_t sigma_val)
{
    hw->bf_sigma.sigma = sigma_val;
}

/**
 * @brief Set ISP BF padding mode
 *
 * @param[in] hw            Hardware instance address
 * @param[in] padding_mode  padding mode
 */
static inline void isp_ll_bf_set_padding_mode(isp_dev_t *hw, isp_bf_edge_padding_mode_t padding_mode)
{
    hw->bf_matrix_ctrl.bf_padding_mode = padding_mode;
}

/**
 * @brief Set ISP BF padding data
 *
 * @param[in] hw            Hardware instance address
 * @param[in] padding_data  padding data
 */
static inline void isp_ll_bf_set_padding_data(isp_dev_t *hw, uint32_t padding_data)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->bf_matrix_ctrl, bf_padding_data, padding_data);
}

/**
 * @brief Set ISP BF tail pixen pulse tl
 *
 * @param[in] hw           Hardware instance address
 * @param[in] start_pixel  start pixel value
 */
static inline void isp_ll_bf_set_padding_line_tail_valid_start_pixel(isp_dev_t *hw, uint32_t start_pixel)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->bf_matrix_ctrl, bf_tail_pixen_pulse_tl, start_pixel);
}

/**
 * @brief Set ISP BF tail pixen pulse th
 *
 * @param[in] hw         Hardware instance address
 * @param[in] end_pixel  end pixel value
 */
static inline void isp_ll_bf_set_padding_line_tail_valid_end_pixel(isp_dev_t *hw, uint32_t end_pixel)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->bf_matrix_ctrl, bf_tail_pixen_pulse_th, end_pixel);
}

/**
 * @brief Set ISP BF template
 *
 * @param[in] hw            Hardware instance address
 * @param[in] template_arr  2-d array for the template
 */
static inline void isp_ll_bf_set_template(isp_dev_t *hw, uint8_t template_arr[SOC_ISP_BF_TEMPLATE_X_NUMS][SOC_ISP_BF_TEMPLATE_Y_NUMS])
{
    int cnt = 0;
    for (int i = 0; i < SOC_ISP_BF_TEMPLATE_X_NUMS; i++) {
        for (int j = 0; j < SOC_ISP_BF_TEMPLATE_Y_NUMS; j++) {
            if (i == 2 && j == 2) {
                break;
            }
            hw->bf_gau0.val = (hw->bf_gau0.val &  ~(0xf << (28 - cnt * 4))) | ((template_arr[i][j] & 0xf) << (28 - cnt * 4));
            cnt++;
        }
    }

    hw->bf_gau1.gau_template22 = template_arr[2][2];
}

/*---------------------------------------------------------------
                      CCM
---------------------------------------------------------------*/
/**
 * @brief Set CCM clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_ccm_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_ccm_force_on = mode;
}

/**
 * @brief Enable / Disable CCM
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_ccm_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.ccm_en = enable;
}

/**
 * @brief Set the Color Correction Matrix
 *
 * @param[in] hw      Hardware instance address
 * @param[in] fixed_point_matrix  Color Correction Matrix in fixed-point format
 */
static inline void isp_ll_ccm_set_matrix(isp_dev_t *hw, isp_ll_ccm_gain_t fixed_point_matrix[ISP_CCM_DIMENSION][ISP_CCM_DIMENSION])
{
    hw->ccm_coef0.ccm_rr = fixed_point_matrix[0][0].val;
    hw->ccm_coef0.ccm_rg = fixed_point_matrix[0][1].val;
    hw->ccm_coef1.ccm_rb = fixed_point_matrix[0][2].val;
    hw->ccm_coef1.ccm_gr = fixed_point_matrix[1][0].val;
    hw->ccm_coef3.ccm_gg = fixed_point_matrix[1][1].val;
    hw->ccm_coef3.ccm_gb = fixed_point_matrix[1][2].val;
    hw->ccm_coef4.ccm_br = fixed_point_matrix[2][0].val;
    hw->ccm_coef4.ccm_bg = fixed_point_matrix[2][1].val;
    hw->ccm_coef5.ccm_bb = fixed_point_matrix[2][2].val;
}

/*---------------------------------------------------------------
                      Color
---------------------------------------------------------------*/
/**
 * @brief Set Color clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_color_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_color_force_on = mode;
}

/**
 * @brief Enable / Disable Color
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_color_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.color_en = enable;
}

/**
 * @brief Set color contrast
 *
 * @param[in] hw              Hardware instance address
 * @param[in] color_contrast  Color contrast value
 */
static inline void isp_ll_color_set_contrast(isp_dev_t *hw, isp_color_contrast_t color_contrast)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->color_ctrl, color_contrast, color_contrast.val);
}

/**
 * @brief Set color saturation
 *
 * @param[in] hw                Hardware instance address
 * @param[in] color_saturation  Color saturation value
 */
static inline void isp_ll_color_set_saturation(isp_dev_t *hw, isp_color_saturation_t color_saturation)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->color_ctrl, color_saturation, color_saturation.val);
}

/**
 * @brief Set color hue
 *
 * @param[in] hw              Hardware instance address
 * @param[in] color_hue       Color hue angle
 */
static inline void isp_ll_color_set_hue(isp_dev_t *hw, uint32_t color_hue)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->color_ctrl, color_hue, color_hue);
}

/**
 * @brief Set color brightness
 *
 * @param[in] hw                Hardware instance address
 * @param[in] color_brightness  Color brightness value, signed 2's complement
 */
static inline void isp_ll_color_set_brigntness(isp_dev_t *hw, int8_t color_brightness)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->color_ctrl, color_brightness, color_brightness);
}

/*---------------------------------------------------------------
                      DVP Camera
---------------------------------------------------------------*/
/**
 * @brief Set dvp data color format
 *
 * @param[in] hw      Hardware instance address
 * @param[in] format  color format, see `color_space_pixel_format_t`
 *
 * @return true for valid format, false for invalid format
 */
static inline bool isp_ll_dvp_set_data_type(isp_dev_t *hw, color_space_pixel_format_t format)
{
    bool valid = false;

    if (format.color_space == COLOR_SPACE_RAW) {
        switch(format.pixel_format) {
        case COLOR_PIXEL_RAW8:
            hw->cam_conf.cam_data_type = ISP_LL_DVP_DATA_TYPE_RAW8;
            valid = true;
            break;
        case COLOR_PIXEL_RAW10:
            hw->cam_conf.cam_data_type = ISP_LL_DVP_DATA_TYPE_RAW10;
            valid = true;
            break;
        case COLOR_PIXEL_RAW12:
            hw->cam_conf.cam_data_type = ISP_LL_DVP_DATA_TYPE_RAW12;
            valid = true;
            break;
        default:
            break;
        }
    }

    return valid;
}

/**
 * @brief Enable / Disable 2B mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_dvp_enable_2byte_mode(isp_dev_t *hw, bool enable)
{
    if (enable) {
        HAL_ASSERT(hw->cam_conf.cam_data_type == ISP_LL_DVP_DATA_TYPE_RAW8);
        hw->cam_conf.cam_2byte_mode = 1;
    } else {
        hw->cam_conf.cam_2byte_mode = 0;
    }
}

/**
 * @brief Reset DVP CAM module
 *
 * @param[in] hw      Hardware instance address
 */
static inline void isp_ll_dvp_cam_reset(isp_dev_t *hw)
{
    hw->cam_cntl.cam_reset = 1;
    hw->cam_cntl.cam_reset = 0;
}

/**
 * @brief Enable DVP CAM pclk invert
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_cam_enable_pclk_invert(isp_dev_t *hw, bool enable)
{
    hw->cam_cntl.cam_clk_inv = enable;
}

/**
 * @brief Enable DVP CAM de invert
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_cam_enable_de_invert(isp_dev_t *hw, bool enable)
{
    hw->cam_conf.cam_de_inv = enable;
}

/**
 * @brief Enable DVP CAM hsync invert
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_cam_enable_hsync_invert(isp_dev_t *hw, bool enable)
{
    hw->cam_conf.cam_hsync_inv = enable;
}

/**
 * @brief Enable DVP CAM vsync invert
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_cam_enable_vsync_invert(isp_dev_t *hw, bool enable)
{
    hw->cam_conf.cam_vsync_inv = enable;
}

/**
 * @brief Enable DVP CAM
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_cam_enable(isp_dev_t *hw, bool enable)
{
    if (enable) {
        hw->cam_cntl.cam_update_reg = 1;
        hw->cam_cntl.cam_en = 1;
        while (hw->cam_cntl.cam_update_reg);
    } else {
        hw->cam_cntl.cam_en = 0;
    }
}

/*---------------------------------------------------------------
                      AE
---------------------------------------------------------------*/

/**
 * @brief Set AE clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_ae_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_ae_force_on = mode;
}

/**
 * @brief Enable / Disable AE
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_ae_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.ae_en = enable;
}

/**
 * @brief Manual aupdate AF once
 *
 * @param[in] hw  Hardware instance address
 */
static inline void isp_ll_ae_manual_update(isp_dev_t *hw)
{
    hw->ae_ctrl.ae_update = 1;
}

/**
 * @brief Select AE input data source
 *
 * @param[in] hw                Hardware instance address
 * @param[in] sample_point   0: AE input data after demosaic, 1: AE input data after gamma
 */
static inline void isp_ll_ae_set_sample_point(isp_dev_t *hw, isp_ae_sample_point_t sample_point)
{
    hw->ae_ctrl.ae_select = sample_point;
}

/**
 * @brief Set AE window range
 *
 * @param[in] hw        Hardware instance address
 * @param[in] x_start   Top left pixel x axis value
 * @param[in] x_bsize   Block size on x axis
 * @param[in] y_start   Top left pixel y axis value
 * @param[in] y_bsize   Block size on y axis
 */
static inline void isp_ll_ae_set_window_range(isp_dev_t *hw, int x_start, int x_bsize, int y_start, int y_bsize)
{
    hw->ae_bx.ae_x_start = x_start;
    hw->ae_bx.ae_x_bsize = x_bsize;
    hw->ae_by.ae_y_start = y_start;
    hw->ae_by.ae_y_bsize = y_bsize;
}

/**
 * @brief Get block mean luminance
 *
 * @param[in] hw        Hardware instance address
 * @param[in] block_id
 *
 * @return Mean luminance
 */
static inline int isp_ll_ae_get_block_mean_lum(isp_dev_t *hw, int block_id)
{
    HAL_ASSERT(block_id >=0 && block_id < (SOC_ISP_AE_BLOCK_X_NUMS * SOC_ISP_AE_BLOCK_Y_NUMS));
    return hw->ae_block_mean[block_id / 4].ae_b_mean[3 - (block_id % 4)];
}

/**
 * @brief AE set the pixel number of each subwin, and set the reciprocal of each subwin_pixnum, 20bit fraction
 *
 * @param[in] hw            Hardware instance address
 * @param[in] subwin_pixnum Pixel number
 */
static inline void isp_ll_ae_set_subwin_pixnum_recip(isp_dev_t *hw, int subwin_pixnum)
{
    hw->ae_winpixnum.ae_subwin_pixnum = subwin_pixnum;
    int subwin_recip = (1 << 20) / subwin_pixnum;
    hw->ae_win_reciprocal.ae_subwin_recip = subwin_recip;
}

/**
 * @brief Set AE env detector threshold
 *
 * @param[in] hw          Hardware instance address
 * @param[in] low_thresh  Lower lum threshold
 * @param[in] high_thresh Higher lum threshold
 */
static inline void isp_ll_ae_env_detector_set_thresh(isp_dev_t *hw, uint32_t low_thresh, uint32_t high_thresh)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->ae_monitor, ae_monitor_tl, low_thresh);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->ae_monitor, ae_monitor_th, high_thresh);
}

/**
 * @brief Set AE env detector period
 *
 * @param[in] hw      Hardware instance address
 * @param[in] period  period of the AE env detector, in frames
 */
static inline void isp_ll_ae_env_detector_set_period(isp_dev_t *hw, uint32_t period)
{
    hw->ae_monitor.ae_monitor_period = period;
}

/*---------------------------------------------------------------
                      LSC
---------------------------------------------------------------*/
/**
 * @brief Set LSC clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_lsc_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_lsc_force_on = mode;
}

/**
 * @brief Enable / Disable Color
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_lsc_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.lsc_en = enable;
}

/**
 * @brief Set xtable size
 *
 * @param[in] hw          Hardware instance address
 * @param[in] xtablesize  xtablesize
 */
static inline void isp_ll_lsc_set_xtablesize(isp_dev_t *hw, uint8_t xtablesize)
{
    hw->lsc_tablesize.lsc_xtablesize = xtablesize;
}

/*---------------------------------------------------------------
                      LUT
---------------------------------------------------------------*/
/**
 * @brief Select ISP LUT
 *
 * @param[in] hw        Hardware instance address
 * @param[in] is_write  Is write or not
 * @param[in] is_gb_b   Is gb_b or not
 * @param[in] addr      LUT addr
 * @param[in] lut       ISP LUT
 */
static inline void isp_ll_lut_set_cmd(isp_dev_t *hw, bool is_write, bool is_gb_b, uint32_t addr, isp_ll_lut_t lut)
{
    uint32_t val = 0;
    val |= is_write ? (1 << 16) : 0;
    val |= is_gb_b ? 0 : (1 << 10);
    val |= addr & ((1 << 10) - 1);
    val |= lut << 12;
    hw->lut_cmd.val = val;
}

/**
 * @brief Set lut gb and b gain
 *
 * @param[in] hw        Hardware instance address
 * @param[in] gb_gain   gb gain
 * @param[in] b_gain    b gain
 */
static inline void isp_ll_lut_set_wdata_gb_b(isp_dev_t *hw, isp_lsc_gain_t gb_gain, isp_lsc_gain_t b_gain)
{
    hw->lut_wdata.lut_wdata = (gb_gain.val & 0x3ff) << 10 | (b_gain.val & 0x3ff);
}

/**
 * @brief Set lut r and gr gain
 *
 * @param[in] hw        Hardware instance address
 * @param[in] r_gain   r gain
 * @param[in] gr_gain    gr gain
 */
static inline void isp_ll_lut_set_wdata_r_gr(isp_dev_t *hw, isp_lsc_gain_t r_gain, isp_lsc_gain_t gr_gain)
{
    hw->lut_wdata.lut_wdata = (r_gain.val & 0x3ff) << 10 | (gr_gain.val & 0x3ff);
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
/**
 * @brief Enable / Disable interrupt
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mask    INTR mask
 * @param[in] enable  Enable / disable
 */
__attribute__((always_inline))
static inline void isp_ll_enable_intr(isp_dev_t *hw, uint32_t mask, bool enable)
{
    if (enable) {
        hw->int_ena.val |= mask;
    } else {
        hw->int_ena.val &= ~mask;
    }
}

/**
 * @brief Get interrupt status
 *
 * @param[in] hw  Hardware instance address
 *
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_get_intr_status(isp_dev_t *hw)
{
    return hw->int_st.val;
}

/**
 * @brief Get interrupt status reg address
 *
 * @param[in] hw  Hardware instance address
 *
 * @return Interrupt status reg address
 */
static inline uint32_t isp_ll_get_intr_status_reg_addr(isp_dev_t *hw)
{
    return (uint32_t)&(hw->int_st);
}

/**
 * @brief Get interrupt raw
 *
 * @param[in] hw  Hardware instance address
 *
 * @return Interrupt raw
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_get_intr_raw(isp_dev_t *hw)
{
    return hw->int_raw.val;
}

/**
 * @brief Clear interrupt
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mask    INTR mask
 */
__attribute__((always_inline))
static inline void isp_ll_clear_intr(isp_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

/*---------------------------------------------------------------
                      AWB
---------------------------------------------------------------*/
/**
 * @brief Set AWB clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_awb_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_awb_force_on = mode;
}

/**
 * @brief Enable AWB statistics
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
__attribute__((always_inline))
static inline void isp_ll_awb_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.awb_en = enable;
}

/**
 * @brief Set AWB sample point
 *
 * @param[in] hw      Hardware instance address
 * @param[in] point   Sample point
 *                      - 0: Before CCM
 *                      - 1: After CCM
 */
static inline void isp_ll_awb_set_sample_point(isp_dev_t *hw, isp_awb_sample_point_t point)
{
    hw->awb_mode.awb_sample = point;
}

/**
 * @brief Set AWB algorithm mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable algorithm mode 1
 */
static inline void isp_ll_awb_enable_algorithm_mode(isp_dev_t *hw, bool enable)
{
    hw->awb_mode.awb_mode = enable;
}

/**
 * @brief Set AWB window range
 *
 * @param[in] hw              Hardware instance address
 * @param[in] top_left_x      Top left pixel x axis value
 * @param[in] top_left_y      Top left pixel y axis value
 * @param[in] bottom_right_x  Bottom right pixel x axis value
 * @param[in] bottom_right_y  Bottom right pixel y axis value
 */
static inline void isp_ll_awb_set_window_range(isp_dev_t *hw, uint32_t top_left_x, uint32_t top_left_y, uint32_t bottom_right_x, uint32_t bottom_right_y)
{
    hw->awb_hscale.awb_lpoint = top_left_x;
    hw->awb_vscale.awb_tpoint = top_left_y;
    hw->awb_hscale.awb_rpoint = bottom_right_x;
    hw->awb_vscale.awb_bpoint = bottom_right_y;
}

/**
 * @brief Set AWB luminance range
 *
 * @param[in] hw              Hardware instance address
 * @param[in] min             Minimum luminance
 * @param[in] max             Maximum luminance
 */
static inline void isp_ll_awb_set_luminance_range(isp_dev_t *hw, uint32_t min, uint32_t max)
{
    hw->awb_th_lum.awb_min_lum = min;
    hw->awb_th_lum.awb_max_lum = max;
}

/**
 * @brief Set AWB R/G ratio range
 *
 * @param[in] hw              Hardware instance address
 * @param[in] min             Minimum R/G ratio in fixed-point data type
 * @param[in] max             Maximum R/G ratio in fixed-point data type
 */
static inline void isp_ll_awb_set_rg_ratio_range(isp_dev_t *hw, isp_ll_awb_rgb_ratio_t min, isp_ll_awb_rgb_ratio_t max)
{
    hw->awb_th_rg.awb_min_rg = min.val;
    hw->awb_th_rg.awb_max_rg = max.val;
}

/**
 * @brief Set AWB B/G ratio range
 *
 * @param[in] hw              Hardware instance address
 * @param[in] min             Minimum B/G ratio in fixed-point data type
 * @param[in] max             Maximum B/G ratio in fixed-point data type
 */
static inline void isp_ll_awb_set_bg_ratio_range(isp_dev_t *hw, isp_ll_awb_rgb_ratio_t min, isp_ll_awb_rgb_ratio_t max)
{
    hw->awb_th_bg.awb_min_bg = min.val;
    hw->awb_th_bg.awb_max_bg = max.val;
}

/**
 * @brief Get AWB white patch count
 *
 * @param[in] hw              Hardware instance address
 * @return
 *      - white patch count
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_awb_get_white_patch_cnt(isp_dev_t *hw)
{
    return hw->awb0_white_cnt.awb0_white_cnt;
}

/**
 * @brief Get AWB accumulated R value of white patches
 *
 * @param[in] hw              Hardware instance address
 * @return
 *      - Accumulated R value of white patches
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_awb_get_accumulated_r_value(isp_dev_t *hw)
{
    return hw->awb0_acc_r.awb0_acc_r;
}

/**
 * @brief Get AWB accumulated G value of white patches
 *
 * @param[in] hw              Hardware instance address
 * @return
 *      - Accumulated G value of white patches
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_awb_get_accumulated_g_value(isp_dev_t *hw)
{
    return hw->awb0_acc_g.awb0_acc_g;
}

/**
 * @brief Get AWB accumulated B value of white patches
 *
 * @param[in] hw              Hardware instance address
 * @return
 *      - Accumulated B value of white patches
 */
__attribute__((always_inline))
static inline uint32_t isp_ll_awb_get_accumulated_b_value(isp_dev_t *hw)
{
    return hw->awb0_acc_b.awb0_acc_b;
}

/*---------------------------------------------------------------
                      Demosaic
---------------------------------------------------------------*/
/**
 * @brief Set demosaic clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_demosaic_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_demosaic_force_on = mode;
}

/**
 * @brief Enable / Disable demosaic
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_demosaic_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.demosaic_en = enable;
}

/**
 * @brief Set demosaic low thresh
 *
 * @param[in] hw      Hardware instance address
 * @param[in] thresh  Thresh
 */
__attribute__((always_inline))
static inline void isp_ll_demosaic_set_grad_ratio(isp_dev_t *hw, isp_demosaic_grad_ratio_t grad_ratio)
{
    hw->demosaic_grad_ratio.demosaic_grad_ratio = grad_ratio.val;
}

/**
 * @brief Set ISP demosaic padding mode
 *
 * @param[in] hw            Hardware instance address
 * @param[in] padding_mode  padding mode
 */
__attribute__((always_inline))
static inline void isp_ll_demosaic_set_padding_mode(isp_dev_t *hw, isp_demosaic_edge_padding_mode_t padding_mode)
{
    hw->demosaic_matrix_ctrl.demosaic_padding_mode = padding_mode;
}

/**
 * @brief Set ISP demosaic padding data
 *
 * @param[in] hw            Hardware instance address
 * @param[in] padding_data  padding data
 */
__attribute__((always_inline))
static inline void isp_ll_demosaic_set_padding_data(isp_dev_t *hw, uint32_t padding_data)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->demosaic_matrix_ctrl, demosaic_padding_data, padding_data);
}

/**
 * @brief Set ISP demosaic tail start pulse pixel
 *
 * @param[in] hw           Hardware instance address
 * @param[in] start_pixel  start pixel value
 */
__attribute__((always_inline))
static inline void isp_ll_demosaic_set_padding_line_tail_valid_start_pixel(isp_dev_t *hw, uint32_t start_pixel)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->demosaic_matrix_ctrl, demosaic_tail_pixen_pulse_tl, start_pixel);
}

/**
 * @brief Set ISP demosaic tail pulse end pixel
 *
 * @param[in] hw         Hardware instance address
 * @param[in] end_pixel  end pixel value
 */
__attribute__((always_inline))
static inline void isp_ll_demosaic_set_padding_line_tail_valid_end_pixel(isp_dev_t *hw, uint32_t end_pixel)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->demosaic_matrix_ctrl, demosaic_tail_pixen_pulse_th, end_pixel);
}

/*---------------------------------------------------------------
                      Sharpen
---------------------------------------------------------------*/
/**
 * @brief Set sharpen clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_sharp_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_sharp_force_on = mode;
}

/**
 * @brief Enable / Disable sharpen
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_sharp_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.sharp_en = enable;
}

/**
 * @brief Set sharpen low thresh
 *
 * @param[in] hw      Hardware instance address
 * @param[in] thresh  Thresh
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_low_thresh(isp_dev_t *hw, uint8_t thresh)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_ctrl0, sharp_threshold_low, thresh);
}

/**
 * @brief Set sharpen high thresh
 *
 * @param[in] hw      Hardware instance address
 * @param[in] thresh  Thresh
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_high_thresh(isp_dev_t *hw, uint8_t thresh)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_ctrl0, sharp_threshold_high, thresh);
}

/**
 * @brief Set sharpen medium pixel coeff
 *
 * @param[in] hw      Hardware instance address
 * @param[in] coeff  coeff
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_medium_freq_coeff(isp_dev_t *hw, isp_sharpen_m_freq_coeff coeff)
{
    //val between `sharp_amount_low` and `sharp_threshold_high` will be multiplied by `sharp_amount_low`
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_ctrl0, sharp_amount_low, coeff.val);
}

/**
 * @brief Set sharpen high freq pixel coeff
 *
 * @param[in] hw      Hardware instance address
 * @param[in] coeff  coeff
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_high_freq_coeff(isp_dev_t *hw, isp_sharpen_h_freq_coeff_t coeff)
{
    //val higher than `sharp_threshold_high` will be multiplied by `sharp_amount_high`
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_ctrl0, sharp_amount_high, coeff.val);
}

/**
 * @brief Set ISP sharpen padding mode
 *
 * @param[in] hw            Hardware instance address
 * @param[in] padding_mode  padding mode
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_padding_mode(isp_dev_t *hw, isp_sharpen_edge_padding_mode_t padding_mode)
{
    hw->sharp_matrix_ctrl.sharp_padding_mode = padding_mode;
}

/**
 * @brief Set ISP sharpen padding data
 *
 * @param[in] hw            Hardware instance address
 * @param[in] padding_data  padding data
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_padding_data(isp_dev_t *hw, uint32_t padding_data)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_matrix_ctrl, sharp_padding_data, padding_data);
}

/**
 * @brief Set ISP sharpen tail start pulse pixel
 *
 * @param[in] hw           Hardware instance address
 * @param[in] start_pixel  start pixel value
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_padding_line_tail_valid_start_pixel(isp_dev_t *hw, uint32_t start_pixel)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_matrix_ctrl, sharp_tail_pixen_pulse_tl, start_pixel);
}

/**
 * @brief Set ISP sharpen tail pulse end pixel
 *
 * @param[in] hw         Hardware instance address
 * @param[in] end_pixel  end pixel value
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_padding_line_tail_valid_end_pixel(isp_dev_t *hw, uint32_t end_pixel)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sharp_matrix_ctrl, sharp_tail_pixen_pulse_th, end_pixel);
}

/**
 * @brief Set ISP sharpen template
 *
 * @param[in] hw            Hardware instance address
 * @param[in] template_arr  2-d array for the template
 */
__attribute__((always_inline))
static inline void isp_ll_sharp_set_template(isp_dev_t *hw, uint8_t template_arr[SOC_ISP_SHARPEN_TEMPLATE_X_NUMS][SOC_ISP_SHARPEN_TEMPLATE_Y_NUMS])
{
    for (int i = 0; i < SOC_ISP_SHARPEN_TEMPLATE_X_NUMS; i++) {
        for (int j = 0; j < SOC_ISP_SHARPEN_TEMPLATE_Y_NUMS; j++) {
            if (j == 0) {
                hw->sharp_filter[i].sharp_filter_coe0 = template_arr[i][j];
            } else if (j == 1) {
                hw->sharp_filter[i].sharp_filter_coe1 = template_arr[i][j];
            } else if (j == 2) {
                hw->sharp_filter[i].sharp_filter_coe2 = template_arr[i][j];
            } else {
                HAL_ASSERT(false);
            }
        }
    }
}

/**
 * @brief Get ISP sharpen high freq image pixel max value
 *
 * @param[in] hw            Hardware instance address
 *
 * @return Max value
 */
__attribute__((always_inline))
static inline uint8_t isp_ll_sharp_get_high_freq_pixel_max(isp_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->sharp_ctrl1, sharp_gradient_max);
}

/*---------------------------------------------------------------
                      RGB/YUV
---------------------------------------------------------------*/
/**
 * @brief Set rgb2yuv clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_rgb2yuv_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_rgb2yuv_force_on = mode;
}

/**
 * @brief Set yuv2rgb clock control mode
 *
 * @param[in] hw      Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
 */
static inline void isp_ll_yuv2rgb_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_yuv2rgb_force_on = mode;
}

/**
 * @brief Set YUV range
 *
 * @param[in] hw      Hardware instance address
 * @param[in] range   see `isp_color_range_t`
 */
static inline void isp_ll_yuv_set_range(isp_dev_t *hw, isp_color_range_t range)
{
    switch (range) {
    case ISP_COLOR_RANGE_LIMIT:
        hw->yuv_format.yuv_range = 1;
        break;
    case ISP_COLOR_RANGE_FULL:
        hw->yuv_format.yuv_range = 0;
        break;
    default:
        // Unsupported color range
        abort();
    }
}

/**
 * @brief Set YUV standard
 *
 * @param[in] hw      Hardware instance address
 * @param[in] std     see `isp_yuv_conv_std_t`
 */
static inline void isp_ll_yuv_set_std(isp_dev_t *hw, isp_yuv_conv_std_t std)
{
    hw->yuv_format.yuv_mode = std;
}

/*---------------------------------------------------------------
                      Gamma Correction
---------------------------------------------------------------*/
/**
 * @brief Enable / Disable gamma
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_gamma_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.gamma_en = enable;
}

/**
 * @brief Set gamma correction curve for one of the R/G/B components
 *
 * @param[in] hw      Hardware instance address
 * @param[in] channel One of the R/G/B components, color_component_t
 * @param[in] pts     Pointer to the structure that contains the information of the gamma curve
 */
__attribute__((always_inline))
static inline void isp_ll_gamma_set_correction_curve(isp_dev_t *hw, color_component_t channel, const isp_gamma_curve_points_t *pts)
{
    int ch_index = -1;
    switch (channel) {
    case COLOR_COMPONENT_R:
        ch_index = 0;
        break;
    case COLOR_COMPONENT_G:
        ch_index = 1;
        break;
    case COLOR_COMPONENT_B:
        ch_index = 2;
        break;
    default:
        abort();
    }

    uint32_t x_prev = 0;
    uint32_t gamma_x1 = 0, gamma_x2 = 0, gamma_y1 = 0, gamma_y2 = 0, gamma_y3 = 0, gamma_y4 = 0;
    for (int i = 0; i < ISP_GAMMA_CURVE_POINTS_NUM; i++) {
        uint32_t x_delta = (i == (ISP_GAMMA_CURVE_POINTS_NUM - 1) ? 256 : pts->pt[i].x) - x_prev;
        uint32_t power = __builtin_ctz(x_delta);
        HAL_ASSERT((x_delta & (x_delta - 1)) == 0 && power < 8);
        if (i < 4) {
            gamma_x1 |= (power << (21 - i * 3));
            gamma_y1 |= (pts->pt[i].y << (24 - i * 8));
        } else if (i < 8) {
            gamma_x1 |= (power << (21 - i * 3));
            gamma_y2 |= (pts->pt[i].y << (24 - (i - 4) * 8));
        } else if (i < 12) {
            gamma_x2 |= (power << (21 - (i - 8) * 3));
            gamma_y3 |= (pts->pt[i].y << (24 - (i - 8) * 8));
        } else {
            gamma_x2 |= (power << (21 - (i - 8) * 3));
            gamma_y4 |= (pts->pt[i].y << (24 - (i - 12) * 8));
        }
        x_prev = pts->pt[i].x;
    }
    hw->gamma_rgb_x[ch_index].gamma_x1.val = gamma_x1;
    hw->gamma_rgb_x[ch_index].gamma_x2.val = gamma_x2;
    hw->gamma_rgb_y[ch_index].gamma_y1.val = gamma_y1;
    hw->gamma_rgb_y[ch_index].gamma_y2.val = gamma_y2;
    hw->gamma_rgb_y[ch_index].gamma_y3.val = gamma_y3;
    hw->gamma_rgb_y[ch_index].gamma_y4.val = gamma_y4;

    hw->gamma_ctrl.gamma_update = 1;
    while (hw->gamma_ctrl.gamma_update);
}

/*---------------------------------------------------------------
                      HIST
---------------------------------------------------------------*/
/**
 * @brief Set histogram clock control mode
 *
 * @param[in] hw Hardware instance address
 * @param[in] mode    'isp_ll_pipeline_clk_ctrl_t`
*/
static inline void isp_ll_hist_set_clk_ctrl_mode(isp_dev_t *hw, isp_ll_pipeline_clk_ctrl_t mode)
{
    hw->clk_en.clk_hist_force_on = mode;
}

/**
 * @brief Set histogram subwindow weight
 *
 * @param[in] hw Hardware instance address
 * @param[in] window_weight array for window weight
*/
static inline void isp_ll_hist_set_subwindow_weight(isp_dev_t *hw, const isp_hist_weight_t hist_window_weight[SOC_ISP_HIST_BLOCK_X_NUMS * SOC_ISP_HIST_BLOCK_Y_NUMS])
{
    for (int i = 0; i < SOC_ISP_HIST_BLOCK_X_NUMS * SOC_ISP_HIST_BLOCK_Y_NUMS; i++) {
        // On ESP32P4, hist_weight [7,0] are decimal
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hist_weight[i / 4], hist_weight_b[3 - (i % 4)], hist_window_weight[i].decimal);
    }
}

/**
 * @brief Set histogram segment threshold
 *
 * @param[in] hw Hardware instance address
 * @param[in] segment_threshold array for segment threshold
*/
static inline void isp_ll_hist_set_segment_threshold(isp_dev_t *hw, const uint32_t segment_threshold[SOC_ISP_HIST_INTERVAL_NUMS])
{
    for (int i = 0; i < SOC_ISP_HIST_INTERVAL_NUMS; i++) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hist_seg[i / 4], hist_seg_b[3 - (i % 4)], segment_threshold[i]);
    }
}

/**
 * @brief Set histogram window range
 *
 * @param[in] hw        Hardware instance address
 * @param[in] x_start   Top left pixel x axis value
 * @param[in] x_bsize   Block size on x axis
 * @param[in] y_start   Top left pixel y axis value
 * @param[in] y_bsize   Block size on y axis
 */
static inline void isp_ll_hist_set_window_range(isp_dev_t *hw, int x_start, int x_bsize, int y_start, int y_bsize)
{
    hw->hist_offs.hist_x_offs = x_start;
    hw->hist_offs.hist_y_offs = y_start;
    hw->hist_size.hist_x_size = x_bsize;
    hw->hist_size.hist_y_size = y_bsize;
}

/**
 * @brief Enable / Disable histogram statistic
 *
 * @param[in] hw Hardware instance address
 * @param[in] enable enable/disable
*/
static inline void isp_ll_hist_enable(isp_dev_t *hw, bool enable)
{
    hw->cntl.hist_en = enable;
}

/**
 * @brief Get histogram value
 *
 * @param[in] hw Hardware instance address
 * @param[out] histogram_value pointer to histogram result
*/
__attribute__((always_inline))
static inline void isp_ll_hist_get_histogram_value(isp_dev_t *hw, uint32_t *histogram_value)
{
    for (int i = 0; i < SOC_ISP_HIST_SEGMENT_NUMS; i++) {
        histogram_value[i] = hw->hist_binn[i].hist_bin_n;
    }
}

/**
 * @brief Set histogram sampling mode
 *
 * @param[in] hw Hardware instance address
 * @param[in] hist_mode histogram mode
*/
static inline void isp_ll_hist_set_mode(isp_dev_t *hw, isp_hist_sampling_mode_t hist_mode)
{
    hw->hist_mode.hist_mode = hist_mode;
}

/**
 * @brief Set histogram RGB coefficients, only effect when hist_mode is ISP_HIST_SAMPLING_RGB
 *
 * @param[in] hw Hardware instance address
 * @param[in] rgb_coeff RGB coefficients
*/
static inline void isp_ll_hist_set_rgb_coefficient(isp_dev_t *hw, const isp_hist_rgb_coefficient_t *rgb_coeff)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hist_coeff, hist_coeff_r, rgb_coeff->coeff_r.decimal);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hist_coeff, hist_coeff_g, rgb_coeff->coeff_g.decimal);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hist_coeff, hist_coeff_b, rgb_coeff->coeff_b.decimal);
}

#ifdef __cplusplus
}
#endif
