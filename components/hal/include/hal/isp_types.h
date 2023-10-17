/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                      Misc
---------------------------------------------------------------*/
#if SOC_ISP_SUPPORTED
typedef soc_periph_isp_clk_src_t    isp_clk_src_t;     ///< Clock source type of ISP
#else
typedef int                         isp_clk_src_t;     ///< Default type
#endif

/**
 * @brief ISP Input Source
 */
typedef enum {
    ISP_INPUT_DATA_SOURCE_CSI,      ///< Input data from CSI
    ISP_INPUT_DATA_SOURCE_CAM,      ///< Input data from CAM
    ISP_INPUT_DATA_SOURCE_DWDMA,    ///< Input data from DW-DMA
} isp_input_data_source_t;

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
/**
 * @brief ISP AF window
 */
typedef struct {
    uint32_t top_left_x;        ///< Top left x axis value
    uint32_t top_left_y;        ///< Top left y axis value
    uint32_t bottom_right_x;    ///< Bottom right x axis value
    uint32_t bottom_right_y;    ///< Bottom right y axis value
} isp_af_window_t;


/**
 * @brief ISP AF result
 */
typedef struct {
#if SOC_ISP_SUPPORTED
    uint32_t definition[SOC_ISP_AF_WINDOW_NUMS];    ///< Definition
    uint32_t luminance[SOC_ISP_AF_WINDOW_NUMS];     ///< Luminance
#endif
} isp_af_result_t;


#ifdef __cplusplus
}
#endif
