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
#include "hal/isp_types.h"

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


/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
/**
 * @brief Configure AF window
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] window_id  Window ID
 * @param[in] window     Window info, see `isp_af_window_t`
 */
void isp_hal_af_window_config(const isp_hal_context_t *hal, int window_id, const isp_af_window_t *window);

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
                      BF
---------------------------------------------------------------*/
/**
 * @brief Configure ISP BF registers
 *
 * @param[in] hal        Context of the HAL layer
 * @param[in] config     BF config, set NULL to de-config the ISP BF
 */
void isp_hal_bf_config(isp_hal_context_t *hal, isp_hal_bf_cfg_t *config);

#ifdef __cplusplus
}
#endif
