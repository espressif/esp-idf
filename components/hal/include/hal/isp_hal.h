/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    void *hw;    ///< Beginning address of the ISP registers
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

/**
 * @brief Get AF oneshot result
 *
 * @param[in]  hal      Context of the HAL layer
 * @param[out] out_res  AF result
 */
void isp_hal_af_get_oneshot_result(const isp_hal_context_t *hal, isp_af_result_t *out_res);

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


#ifdef __cplusplus
}
#endif
