/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include "hal/jpeg_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jpeg_dev_t *jpeg_soc_handle_t; // JPEG SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    jpeg_soc_handle_t dev; // JPEG SOC layer handle (i.e. register base address)
} jpeg_hal_context_t;

/**
 * @brief Initialize the JPEG codec HAL driver
 *
 * @param hal: JPEG codec HAL context
 */
void jpeg_hal_init(jpeg_hal_context_t *hal);

/**
 * @brief Deinitialize the JPEG codec HAL driver
 *
 * @param hal: JPEG codec HAL context
 */
void jpeg_hal_deinit(jpeg_hal_context_t *hal);

/**
 * @brief Function pointer typedef for configuring DHT tables in JPEG decoding.
 *
 * This function pointer typedef represents a callback function that can be used
 * to configure the DHT (Huffman) tables in the JPEG decoding process. It takes
 * as input a JPEG HAL context, as well as pointers to arrays representing the
 * minimum codes, bit lengths, and Huffman codes for the DHT tables.
 *
 * @param hal       The JPEG HAL context.
 * @param huffbits  Pointer to the array of bit lengths for the DHT tables.
 * @param huffcode  Pointer to the array of Huffman codes for the DHT tables.
 */
typedef void (*jpeg_config_dht_table_t)(jpeg_hal_context_t *hal, uint8_t *huffbits, uint8_t *huffcode, uint32_t *tmp_huff);

/**
 * @brief Array of function pointers for configuring DHT tables in JPEG decoding.
 *
 * This two-dimensional array represents a collection of function pointers that
 * can be used to configure the DHT (Huffman) tables in the JPEG decoding process.
 */
extern jpeg_config_dht_table_t dht_func[DHT_TC_NUM][DHT_TH_NUM];

/**
 * Typedef for a function pointer to configure frame information in JPEG.
 *
 * This function is used to configure frame information in JPEG hardware.
 *
 * @param hw                The JPEG SOC handle.
 * @param identifier        The identifier of the frame.
 * @param horizontal_factor The horizontal factor for chroma subsampling.
 * @param vertical_factor   The vertical factor for chroma subsampling.
 * @param qun_table_id      The quantization table ID.
 */
typedef void (*jpeg_config_frame_info_t)(jpeg_soc_handle_t hw, uint8_t identifier, uint8_t horizontal_factor, uint8_t vertical_factor, uint8_t qun_table_id);

/**
 * Array of function pointers to configure frame information in JPEG.
 *
 * This array holds function pointers to configure frame information in JPEG hardware.
 * Each element corresponds to a specific component number.
 */
extern jpeg_config_frame_info_t sof_func[JPEG_COMPONENT_NUMBER_MAX];

/**
 * Typedef for a function pointer to configure quantization coefficients in JPEG.
 *
 * This function is used to configure quantization coefficients in JPEG hardware.
 *
 * @param hw                    The JPEG SOC handle.
 * @param quantization_table    Pointer to the quantization table.
 */
typedef void (*jpeg_config_quantization_coefficient_t)(jpeg_soc_handle_t hw, uint32_t *quantization_table);

/**
 * Array of function pointers to configure quantization coefficients in JPEG.
 *
 * This array holds function pointers to configure quantization coefficients in JPEG hardware.
 * Each element corresponds to a specific component number.
 */
extern jpeg_config_quantization_coefficient_t dqt_func[JPEG_COMPONENT_NUMBER_MAX];

/**
 * Set the quantization coefficients for luminance and chrominance in the JPEG hardware accelerator context.
 *
 * @param hal Pointer to the JPEG hardware accelerator context.
 * @param lqnr Pointer to an array of luminance quantization coefficients.
 * @param cqnr Pointer to an array of chrominance quantization coefficients.
 */
void jpeg_hal_set_quantization_coefficient(jpeg_hal_context_t *hal, uint32_t *lqnr, uint32_t *cqnr);

#ifdef __cplusplus
}
#endif
