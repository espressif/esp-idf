/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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


#define ISP_LL_GET_HW(num)            (((num) == 0) ? (&ISP) : NULL)

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
#define ISP_LL_EVENT_AE_MONITOR               (1<<8)
#define ISP_LL_EVENT_AE_FRAME_DONE            (1<<9)
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

#define ISP_LL_EVENT_ALL_MASK   (0x1FFFFFFF)
#define ISP_LL_EVENT_AF_MASK    (ISP_LL_EVENT_AF_FDONE | ISP_LL_EVENT_AF_ENV)

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
#define ISP_LL_AF_WINDOW_MAX_RANGE    ((1<<12) - 1)


/**
 * @brief Env monitor mode
 */
typedef enum {
    ISP_LL_AF_ENV_MONITOR_MODE_ABS,    ///< Use absolute val for threshold
    ISP_LL_AF_ENV_MONITOR_MODE_RATIO,  ///< Use ratio val for threshold
} isp_ll_af_env_monitor_mode_t;

/**
 * @brief Edge monitor mode
 */
typedef enum {
    ISP_LL_AF_EDGE_MONITOR_MODE_AUTO,      ///< Auto set threshold
    ISP_LL_AF_EDGE_MONITOR_MODE_MANUAL,    ///< Manual set threshold
} isp_ll_af_edge_monitor_mode_t;


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
#define isp_ll_enable_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; isp_ll_enable_module_clock(__VA_ARGS__)

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
#define isp_ll_reset_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; isp_ll_reset_module_clock(__VA_ARGS__)

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
#define isp_ll_select_clk_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; isp_ll_select_clk_source(__VA_ARGS__)

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
#define isp_ll_set_clock_div(...) (void)__DECLARE_RCC_ATOMIC_ENV; isp_ll_set_clock_div(__VA_ARGS__)

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

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
/**
 * @brief Enable / Disable AF clock
 *
 * @param[in] hw
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_af_clk_enable(isp_dev_t *hw, bool enable)
{
    hw->clk_en.clk_af_force_on = enable;
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
 * @param[in] mode  See `isp_ll_af_edge_monitor_mode_t`
 */
static inline void isp_ll_af_set_edge_thresh_mode(isp_dev_t *hw, isp_ll_af_edge_monitor_mode_t mode)
{
    if (mode == ISP_LL_AF_EDGE_MONITOR_MODE_AUTO) {
        hw->af_threshold.af_threshold = 0;
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

    hw->af_threshold.af_threshold = thresh;
}

/**
 * @brief Set auto edge thresh pixel num
 *
 * @param[in] hw         Hardware instance address
 * @param[in] pixel_num  Pixel numbers
 */
static inline void isp_ll_af_set_auto_edge_thresh_pixel_num(isp_dev_t *hw, uint32_t pixel_num)
{
    HAL_ASSERT(hw->af_threshold.af_threshold == 0);

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
                AF Env Monitor
----------------------------------------------*/
/**
 * @brief Set env monitor period
 *
 * @param[in] hw      Hardware instance address
 * @param[in] period  period of the env monitor, in frames
 */
static inline void isp_ll_af_env_monitor_set_period(isp_dev_t *hw, uint32_t period)
{
    hw->af_ctrl0.af_env_period = period;
}

/**
 * @brief Set env monitor mode
 *
 * @param[in] hw    Hardware instance address
 * @param[in] mode  See `isp_ll_af_env_monitor_mode_t`
 */
static inline void isp_ll_af_env_monitor_set_mode(isp_dev_t *hw, isp_ll_af_env_monitor_mode_t mode)
{
    if (mode == ISP_LL_AF_ENV_MONITOR_MODE_RATIO) {
        hw->af_env_user_th_sum.af_env_user_threshold_sum = 0x0;
        hw->af_env_user_th_lum.af_env_user_threshold_lum = 0x0;
    }

    //nothing to do to if using abs mode, it'll be enabled after `isp_ll_af_env_monitor_set_thresh()`
}

/**
 * @brief Set env monitor threshold
 *
 * @param[in] hw          Hardware instance address
 * @param[in] sum_thresh  Threshold for definition
 * @param[in] lum_thresh  Threshold for luminance
 */
static inline void isp_ll_af_env_monitor_set_thresh(isp_dev_t *hw, uint32_t sum_thresh, uint32_t lum_thresh)
{
    HAL_ASSERT(sum_thresh != 0 || lum_thresh != 0);

    hw->af_env_user_th_sum.af_env_user_threshold_sum = sum_thresh;
    hw->af_env_user_th_lum.af_env_user_threshold_lum = lum_thresh;
}

/**
 * @brief Set env monitor ratio
 *
 * @param[in] hw         Hardware instance address
 * @param[in] ratio_val  Threshold for ratio
 */
static inline void isp_ll_af_env_monitor_set_ratio(isp_dev_t *hw, uint32_t ratio_val)
{
    HAL_ASSERT(hw->af_env_user_th_sum.af_env_user_threshold_sum == 0 &&
               hw->af_env_user_th_lum.af_env_user_threshold_lum == 0);

    hw->af_ctrl0.af_env_threshold = ratio_val;
}

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
/**
 * @brief Enable / Disable BF clock
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_bf_clk_enable(isp_dev_t *hw, bool enable)
{
    hw->clk_en.clk_bf_force_on = enable;
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

/*---------------------------------------------------------------
                      CCM
---------------------------------------------------------------*/
/**
 * @brief Enable / Disable CCM clock
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_ccm_clk_enable(isp_dev_t *hw, bool enable)
{
    hw->clk_en.clk_ccm_force_on = enable;
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

/*---------------------------------------------------------------
                      Color
---------------------------------------------------------------*/
/**
 * @brief Enable / Disable Color clock
 *
 * @param[in] hw      Hardware instance address
 * @param[in] enable  Enable / Disable
 */
static inline void isp_ll_color_clk_enable(isp_dev_t *hw, bool enable)
{
    hw->clk_en.clk_color_force_on = enable;
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

#ifdef __cplusplus
}
#endif
