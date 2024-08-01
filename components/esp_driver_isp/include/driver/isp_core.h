/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/isp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ISP configurations
 */
typedef struct {
    isp_clk_src_t clk_src;                        ///< Clock source
    uint32_t clk_hz;                              ///< Clock frequency in Hz, suggest twice higher than cam sensor speed
    isp_input_data_source_t input_data_source;    ///< Input data source
    isp_color_t input_data_color_type;            ///< Input color type
    isp_color_t output_data_color_type;           ///< Output color type
    bool has_line_start_packet;                   ///< Enable line start packet
    bool has_line_end_packet;                     ///< Enable line end packet
    uint32_t h_res;                               ///< Input horizontal resolution, i.e. the number of pixels in a line
    uint32_t v_res;                               ///< Input vertical resolution, i.e. the number of lines in a frame
    int intr_priority;                            ///< The interrupt priority, range 0~3
} esp_isp_processor_cfg_t;

/**
 * @brief New an ISP processor
 *
 * @param[in]  proc_config  Pointer to ISP config. Refer to ``esp_isp_processor_cfg_t``.
 * @param[out] ret_proc     Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NOT_SUPPORTED Not supported mode
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t esp_isp_new_processor(const esp_isp_processor_cfg_t *proc_config, isp_proc_handle_t *ret_proc);

/**
 * @brief Delete an ISP processor
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_del_processor(isp_proc_handle_t proc);

/**
 * @brief Enable an ISP processor
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_enable(isp_proc_handle_t proc);

/**
 * @brief Disable an ISP processor
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_disable(isp_proc_handle_t proc);

#ifdef __cplusplus
}
#endif
