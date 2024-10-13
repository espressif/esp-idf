/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/isp_types.h"
#include "hal/hal_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BF configurations
 */
typedef struct {
    isp_bf_edge_padding_mode_t padding_mode;                                 ///< BF edge padding mode
    uint8_t padding_data;                                                    ///< BF edge padding pixel data
    uint8_t bf_template[ISP_BF_TEMPLATE_X_NUMS][ISP_BF_TEMPLATE_Y_NUMS];     ///< BF template data
    uint8_t denoising_level;                                                 ///< BF denoising level, from 2 to 20, the bigger the better denoising performance, but the worse detailed
    uint8_t padding_line_tail_valid_start_pixel;                             ///< BF edge padding line tail valid start pixel
    uint8_t padding_line_tail_valid_end_pixel;                               ///< BF edge padding line tail valid end pixel
} isp_hal_bf_cfg_t;

/**
 * @brief Demosaic configurations
 */
typedef struct {
    isp_demosaic_grad_ratio_t grad_ratio;             /**< Demosaic gradient ratio,
                                                           - gradient_x * grad_ratio < gradient_y, use interpolation results in X direction
                                                           - gradient_y * grad_ratio < gradient_x, use interpolation results in Y direction
                                                           - else use the average results between X and Y
                                                      */
    isp_demosaic_edge_padding_mode_t padding_mode;    ///< Sharpen edge padding mode
    uint8_t padding_data;                             ///< Sharpen edge padding pixel data
    uint8_t padding_line_tail_valid_start_pixel;      ///< Sharpen edge padding line tail valid start pixel
    uint8_t padding_line_tail_valid_end_pixel;        ///< Sharpen edge padding line tail valid end pixel
} isp_hal_demosaic_cfg_t;

/**
 * @brief Sharpen configurations
 */
typedef struct {
    isp_sharpen_h_freq_coeff_t h_freq_coeff;                                                ///< High freq pixel sharpeness coeff
    isp_sharpen_m_freq_coeff m_freq_coeff;                                                  ///< Medium freq pixel sharpeness coeff
    uint8_t h_thresh;                                                                       ///< High threshold, pixel value higher than this threshold will be multiplied by `h_freq_coeff`
    uint8_t l_thresh;                                                                       ///< Low threshold, pixel value higher than this threshold but lower than `h_thresh` will be multiplied by `m_freq_coeff`. Pixel value lower than this threshold will be set to 0
    isp_sharpen_edge_padding_mode_t padding_mode;                                           ///< Sharpen edge padding mode
    uint8_t padding_data;                                                                   ///< Sharpen edge padding pixel data
    uint8_t sharpen_template[ISP_SHARPEN_TEMPLATE_X_NUMS][ISP_SHARPEN_TEMPLATE_Y_NUMS];     ///< Sharpen template data
    uint8_t padding_line_tail_valid_start_pixel;                                            ///< Sharpen edge padding line tail valid start pixel
    uint8_t padding_line_tail_valid_end_pixel;                                              ///< Sharpen edge padding line tail valid end pixel
} isp_hal_sharpen_cfg_t;

/**
 * @brief Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    void *hw;    ///< Beginning address of the ISP registers

    /* BF */
    isp_hal_bf_cfg_t bf_cfg;    ///< BF configurations
} isp_hal_context_t;

/**
 * @brief Init the ISP hal and set the ISP to the default configuration.
 *
 * @note This function should be called first before other hal layer function is called.
 *
 * @param[in] hal     Context of the HAL layer
 * @param[in] isp_id  ISP ID
 */
void isp_hal_init(isp_hal_context_t *hal, int isp_id);


/**
 * @brief Color configurations
 */
typedef struct {
    isp_color_contrast_t color_contrast;          ///< The color contrast value, range 0~1, decimal value should be 0~127
    isp_color_saturation_t color_saturation;      ///< The color saturation value, range 0~1, decimal value should be 0~127
    uint32_t color_hue;                           ///< The color hue angle value, range 0-360
    int color_brightness;                         ///< The color brightness value, range -128~127
} isp_hal_color_cfg_t;


/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
/**
 * @brief Configure AF window
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] window_id  Window ID
 * @param[in] window     Window info, see `isp_window_t`
 */
void isp_hal_af_window_config(isp_hal_context_t *hal, int window_id, const isp_window_t *window);

/*---------------------------------------------------------------
                      AE
---------------------------------------------------------------*/
/**
 * @brief Configure AE window
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] window     Window info, see `isp_window_t`
 */
void isp_hal_ae_window_config(isp_hal_context_t *hal, const isp_window_t *window);

/*---------------------------------------------------------------
                            AWB
---------------------------------------------------------------*/
/**
 * @brief  Set the window of the AWB
 *
 * @param[in] hal   Context of the HAL layer
 * @param[in] win   Pointer to the window of the AWB
 * @return
 *      - true      Set success
 *      - false     Invalid arg
 */
bool isp_hal_awb_set_window_range(isp_hal_context_t *hal, const isp_window_t *win);

/**
 * @brief   Set the luminance range of the white patch
 *
 * @param[in] hal   Context of the HAL layer
 * @param[in] lum_min Minimum luminance
 * @param[in] lum_max Maximum luminance
 * @return
 *      - true      Set success
 *      - false     Invalid arg
 */
bool isp_hal_awb_set_luminance_range(isp_hal_context_t *hal, uint32_t lum_min, uint32_t lum_max);

/**
 * @brief   Set the R/G ratio of the white patch
 *
 * @param[in] hal   Context of the HAL layer
 * @param[in] rg_ratio_range Range of Red to Green ratio
 * @return
 *      - true      Set success
 *      - false     Invalid arg
 */
bool isp_hal_awb_set_rg_ratio_range(isp_hal_context_t *hal, float rg_min, float rg_max);

/**
 * @brief   Set the B/R ratio of the white patch
 *
 * @param[in] hal   Context of the HAL layer
 * @param[in] bg_ratio_range Range of Blue to Green ratio
 * @return
 *      - true      Set success
 *      - false     Invalid arg
 */
bool isp_hal_awb_set_bg_ratio_range(isp_hal_context_t *hal, float bg_min, float bg_max);

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
/**
 * @brief Configure ISP BF
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] config     BF config, set NULL to de-config the ISP BF
 */
void isp_hal_bf_config(isp_hal_context_t *hal, isp_hal_bf_cfg_t *config);

/*---------------------------------------------------------------
                      Color Correction Matrix
---------------------------------------------------------------*/
/**
 * @brief Set Color Correction Matrix
 *
 * @param[in] hal           Context of the HAL layer
 * @param[in] saturation    Whether to enable saturation when float data overflow
 * @param[in] flt_matrix    3x3 RGB correction matrix
 * @return
 *      - true      Set success
 *      - false     Invalid argument
 */
bool isp_hal_ccm_set_matrix(const isp_hal_context_t *hal, bool saturation, const float flt_matrix[ISP_CCM_DIMENSION][ISP_CCM_DIMENSION]);

/*---------------------------------------------------------------
                      Demosaic
---------------------------------------------------------------*/
/**
 * @brief Configure ISP Demosaic
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] config     Demosaic config, set NULL to de-config the ISP Demosaic
 */
void isp_hal_demosaic_config(isp_hal_context_t *hal, isp_hal_demosaic_cfg_t *config);

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
/**
 * @brief Clear ISP HW intr event
 *
 * @param[in] hal   Context of the HAL layer
 * @param[in] mask  HW event mask
 */
uint32_t isp_hal_check_clear_intr_event(const isp_hal_context_t *hal, uint32_t mask);

/*---------------------------------------------------------------
                      Sharpness
---------------------------------------------------------------*/
/**
 * @brief Configure ISP sharpeness
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] config     Sharpness config, set NULL to de-config the ISP sharpness
 */
void isp_hal_sharpen_config(isp_hal_context_t *hal, isp_hal_sharpen_cfg_t *config);

/*---------------------------------------------------------------
                      Histogram
---------------------------------------------------------------*/
/**
 * @brief Configure Histogram window
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] window     Window info, see `isp_window_t`
 */
void isp_hal_hist_window_config(isp_hal_context_t *hal, const isp_window_t *window);

/*---------------------------------------------------------------
                      Color
---------------------------------------------------------------*/

/**
 * @brief   Set the color config
 *
 * @param[in] hal                       Context of the HAL layer
 * @param[in] config                    Color config, set NULL to de-config the ISP color
 */
void isp_hal_color_config(isp_hal_context_t *hal, const isp_hal_color_cfg_t *config);

#ifdef __cplusplus
}
#endif
