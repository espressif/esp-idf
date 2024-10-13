/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"
#include "hal/isp_types.h"
#include "hal/hal_utils.h"

/**
 * ISP HAL layer
 */
void isp_hal_init(isp_hal_context_t *hal, int isp_id)
{
    //ISP hardware instance
    hal->hw = ISP_LL_GET_HW(isp_id);
    isp_ll_init(hal->hw);
}

/*---------------------------------------------------------------
                      AE
---------------------------------------------------------------*/
void isp_hal_ae_window_config(isp_hal_context_t *hal, const isp_window_t *window)
{
    uint32_t ae_x_start = window->top_left.x;
    uint32_t ae_x_bsize = (window->btm_right.x - window-> top_left.x) / SOC_ISP_AE_BLOCK_X_NUMS;

    uint32_t ae_y_start = window->top_left.y;
    uint32_t ae_y_bsize = (window->btm_right.y - window->top_left.y) / SOC_ISP_AE_BLOCK_Y_NUMS;

    isp_ll_ae_set_window_range(hal->hw, ae_x_start, ae_x_bsize, ae_y_start, ae_y_bsize);

    int ae_subwin_pixnum = ae_x_bsize * ae_y_bsize;
    isp_ll_ae_set_subwin_pixnum_recip(hal->hw, ae_subwin_pixnum);
}

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
void isp_hal_af_window_config(isp_hal_context_t *hal, int window_id, const isp_window_t *window)
{
    isp_ll_af_set_window_range(hal->hw, window_id, window->top_left.x, window->top_left.y, window->btm_right.x, window->btm_right.y);
}

/*---------------------------------------------------------------
                            AWB
---------------------------------------------------------------*/
bool isp_hal_awb_set_window_range(isp_hal_context_t *hal, const isp_window_t *win)
{
    if (win->top_left.x > win->btm_right.x ||
        win->top_left.y > win->btm_right.y ||
        win->btm_right.x > ISP_LL_AWB_WINDOW_MAX_RANGE ||
        win->btm_right.y > ISP_LL_AWB_WINDOW_MAX_RANGE) {
        return false;
    }
    isp_ll_awb_set_window_range(hal->hw, win->top_left.x, win->top_left.y,
                                win->btm_right.x, win->btm_right.y);
    return true;
}

bool isp_hal_awb_set_luminance_range(isp_hal_context_t *hal, uint32_t lum_min, uint32_t lum_max)
{
    if (lum_min > lum_max || lum_max > ISP_LL_AWB_LUM_MAX_RANGE) {
        return false;
    }
    isp_ll_awb_set_luminance_range(hal->hw, lum_min, lum_max);
    return true;
}

bool isp_hal_awb_set_rg_ratio_range(isp_hal_context_t *hal, float rg_min, float rg_max)
{
    // Convert to fixed point
    isp_ll_awb_rgb_ratio_t fp_rg_min = {};
    isp_ll_awb_rgb_ratio_t fp_rg_max = {};
    hal_utils_fixed_point_t fp_cfg = {
        .int_bit = ISP_LL_AWB_RGB_RATIO_INT_BITS,
        .frac_bit = ISP_LL_AWB_RGB_RATIO_FRAC_BITS,
        .saturation = false,
    };
    if (hal_utils_float_to_fixed_point_32b(rg_min, &fp_cfg, &fp_rg_min.val) != 0) {
        return false;
    }
    if (hal_utils_float_to_fixed_point_32b(rg_max, &fp_cfg, &fp_rg_max.val) != 0) {
        return false;
    }

    // Set AWB white patch R/G ratio range
    isp_ll_awb_set_rg_ratio_range(hal->hw, fp_rg_min, fp_rg_max);
    return true;
}

bool isp_hal_awb_set_bg_ratio_range(isp_hal_context_t *hal, float bg_min, float bg_max)
{
    // Convert to fixed point
    isp_ll_awb_rgb_ratio_t fp_bg_min = {};
    isp_ll_awb_rgb_ratio_t fp_bg_max = {};
    hal_utils_fixed_point_t fp_cfg = {
        .int_bit = ISP_LL_AWB_RGB_RATIO_INT_BITS,
        .frac_bit = ISP_LL_AWB_RGB_RATIO_FRAC_BITS,
        .saturation = false,
    };
    if (hal_utils_float_to_fixed_point_32b(bg_min, &fp_cfg, &fp_bg_min.val) != 0) {
        return false;
    }
    if (hal_utils_float_to_fixed_point_32b(bg_max, &fp_cfg, &fp_bg_max.val) != 0) {
        return false;
    }

    // Set AWB white patch B/G ratio range
    isp_ll_awb_set_bg_ratio_range(hal->hw, fp_bg_min, fp_bg_max);
    return true;
}

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
void isp_hal_bf_config(isp_hal_context_t *hal, isp_hal_bf_cfg_t *config)
{
    if (config) {
        isp_ll_bf_set_sigma(hal->hw, config->denoising_level);
        isp_ll_bf_set_padding_mode(hal->hw, config->padding_mode);
        isp_ll_bf_set_padding_data(hal->hw, config->padding_data);
        isp_ll_bf_set_padding_line_tail_valid_start_pixel(hal->hw, config->padding_line_tail_valid_start_pixel);
        isp_ll_bf_set_padding_line_tail_valid_end_pixel(hal->hw, config->padding_line_tail_valid_end_pixel);
        isp_ll_bf_set_template(hal->hw, config->bf_template);
    } else {
        isp_ll_bf_set_sigma(hal->hw, 0);
        isp_ll_bf_set_padding_mode(hal->hw, 0);
        isp_ll_bf_set_padding_data(hal->hw, 0);
        isp_ll_bf_set_padding_line_tail_valid_start_pixel(hal->hw, 0);
        isp_ll_bf_set_padding_line_tail_valid_end_pixel(hal->hw, 0);
        uint8_t default_template[SOC_ISP_BF_TEMPLATE_X_NUMS][SOC_ISP_BF_TEMPLATE_Y_NUMS] = {};
        memset(default_template, 0, sizeof(default_template));
        isp_ll_bf_set_template(hal->hw, default_template);
    }
}

/*---------------------------------------------------------------
                      Color Correction Matrix
---------------------------------------------------------------*/
bool isp_hal_ccm_set_matrix(const isp_hal_context_t *hal, bool saturation, const float flt_matrix[ISP_CCM_DIMENSION][ISP_CCM_DIMENSION])
{
    isp_ll_ccm_gain_t fp_matrix[ISP_CCM_DIMENSION][ISP_CCM_DIMENSION] = {};
    hal_utils_fixed_point_t fp_cfg = {
        .int_bit = ISP_LL_CCM_MATRIX_INT_BITS,
        .frac_bit = ISP_LL_CCM_MATRIX_FRAC_BITS,
        .saturation = saturation,
    };
    int err_level = saturation ? -1 : 0;
    /* Transfer the float type to fixed point */
    for (int i = 0; i < ISP_CCM_DIMENSION; i++) {
        for (int j = 0; j < ISP_CCM_DIMENSION; j++) {
            if (hal_utils_float_to_fixed_point_32b(flt_matrix[i][j], &fp_cfg, &fp_matrix[i][j].val) < err_level) {
                return false;
            }
        }
    }
    isp_ll_ccm_set_matrix(hal->hw, fp_matrix);
    return true;
}

/*---------------------------------------------------------------
                      Demosaic
---------------------------------------------------------------*/
void isp_hal_demosaic_config(isp_hal_context_t *hal, isp_hal_demosaic_cfg_t *config)
{
    if (config) {
        isp_ll_demosaic_set_grad_ratio(hal->hw, config->grad_ratio);
        isp_ll_demosaic_set_padding_mode(hal->hw, config->padding_mode);
        isp_ll_demosaic_set_padding_data(hal->hw, config->padding_data);
        isp_ll_demosaic_set_padding_line_tail_valid_start_pixel(hal->hw, config->padding_line_tail_valid_start_pixel);
        isp_ll_demosaic_set_padding_line_tail_valid_end_pixel(hal->hw, config->padding_line_tail_valid_end_pixel);
    } else {
        isp_demosaic_grad_ratio_t grad_ratio = {};
        isp_ll_demosaic_set_grad_ratio(hal->hw, grad_ratio);
        isp_ll_demosaic_set_padding_mode(hal->hw, 0);
        isp_ll_demosaic_set_padding_data(hal->hw, 0);
        isp_ll_demosaic_set_padding_line_tail_valid_start_pixel(hal->hw, 0);
        isp_ll_demosaic_set_padding_line_tail_valid_end_pixel(hal->hw, 0);
    }
}

/*---------------------------------------------------------------
                      Color
---------------------------------------------------------------*/
void isp_hal_color_config(isp_hal_context_t *hal, const isp_hal_color_cfg_t *config)
{
    if (config) {
        isp_ll_color_set_contrast(hal->hw, config->color_contrast);
        isp_ll_color_set_saturation(hal->hw, config->color_saturation);
        isp_ll_color_set_hue(hal->hw, (config->color_hue * 256) / 360);
        isp_ll_color_set_brigntness(hal->hw, (int8_t)config->color_brightness);
    } else {
        isp_color_contrast_t color_contrast_default = {
            .integer = 1,
            .decimal = 0,
        };
        isp_color_saturation_t color_saturation_default = {
            .integer = 1,
            .decimal = 0,
        };
        isp_ll_color_set_contrast(hal->hw, color_contrast_default);
        isp_ll_color_set_saturation(hal->hw, color_saturation_default);
        isp_ll_color_set_hue(hal->hw, 0);
        isp_ll_color_set_brigntness(hal->hw, 0);
    }
}

/*---------------------------------------------------------------
                      Histogram
---------------------------------------------------------------*/
void isp_hal_hist_window_config(isp_hal_context_t *hal, const isp_window_t *window)
{
    uint32_t hist_x_start = window->top_left.x;
    uint32_t hist_x_bsize = (window->btm_right.x - window-> top_left.x) / SOC_ISP_HIST_BLOCK_X_NUMS;

    uint32_t hist_y_start = window->top_left.y;
    uint32_t hist_y_bsize = (window->btm_right.y - window->top_left.y) / SOC_ISP_HIST_BLOCK_Y_NUMS;

    isp_ll_hist_set_window_range(hal->hw, hist_x_start, hist_x_bsize, hist_y_start, hist_y_bsize);

}

/*---------------------------------------------------------------
                      Sharpness
---------------------------------------------------------------*/
void isp_hal_sharpen_config(isp_hal_context_t *hal, isp_hal_sharpen_cfg_t *config)
{
    if (config) {
        isp_ll_sharp_set_low_thresh(hal->hw, config->l_thresh);
        isp_ll_sharp_set_high_thresh(hal->hw, config->h_thresh);
        isp_ll_sharp_set_medium_freq_coeff(hal->hw, config->m_freq_coeff);
        isp_ll_sharp_set_high_freq_coeff(hal->hw, config->h_freq_coeff);
        isp_ll_sharp_set_padding_mode(hal->hw, config->padding_mode);
        isp_ll_sharp_set_padding_data(hal->hw, config->padding_data);
        isp_ll_sharp_set_padding_line_tail_valid_start_pixel(hal->hw, config->padding_line_tail_valid_start_pixel);
        isp_ll_sharp_set_padding_line_tail_valid_end_pixel(hal->hw, config->padding_line_tail_valid_end_pixel);
        isp_ll_sharp_set_template(hal->hw, config->sharpen_template);
    } else {
        isp_ll_sharp_set_low_thresh(hal->hw, 0);
        isp_ll_sharp_set_high_thresh(hal->hw, 0);
        isp_sharpen_m_freq_coeff m_freq = {};
        isp_sharpen_h_freq_coeff_t h_freq = {};
        isp_ll_sharp_set_medium_freq_coeff(hal->hw, m_freq);
        isp_ll_sharp_set_high_freq_coeff(hal->hw, h_freq);
        isp_ll_sharp_set_padding_mode(hal->hw, 0);
        isp_ll_sharp_set_padding_data(hal->hw, 0);
        isp_ll_sharp_set_padding_line_tail_valid_start_pixel(hal->hw, 0);
        isp_ll_sharp_set_padding_line_tail_valid_end_pixel(hal->hw, 0);
        uint8_t default_template[SOC_ISP_SHARPEN_TEMPLATE_X_NUMS][SOC_ISP_SHARPEN_TEMPLATE_Y_NUMS] = {};
        memset(default_template, 0, sizeof(default_template));
        isp_ll_sharp_set_template(hal->hw, default_template);
    }
}

/*---------------------------------------------------------------
                      INTR, put in iram
---------------------------------------------------------------*/
uint32_t isp_hal_check_clear_intr_event(const isp_hal_context_t *hal, uint32_t mask)
{
    uint32_t triggered_events = isp_ll_get_intr_status(hal->hw) & mask;

    if (triggered_events) {
        isp_ll_clear_intr(hal->hw, triggered_events);
    }

    return triggered_events;
}
