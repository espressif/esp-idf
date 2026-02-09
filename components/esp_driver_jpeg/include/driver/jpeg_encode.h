/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "jpeg_types.h"
#include "hal/jpeg_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief JPEG encoder configure structure
 */
typedef struct {
    uint32_t height;                        /*!< Number of pixels in the horizontal direction */
    uint32_t width;                         /*!< Number of pixels in the vertical direction */
    jpeg_enc_input_format_t src_type;       /*!< Source type of raw image to be encoded, see `jpeg_enc_src_type_t` */
    jpeg_down_sampling_type_t sub_sample;   /*!< JPEG subsampling method */
    uint32_t image_quality;                 /*!< JPEG compressing quality, value from 1-100, higher value means higher quality */
} jpeg_encode_cfg_t;

/**
 * @brief Configuration parameters for the JPEG encode engine.
 */
typedef struct {
    int intr_priority;                    /*!< JPEG interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    int timeout_ms;                       /*!< JPEG timeout threshold for handling a picture, should larger than valid encode time in ms. For example, for 30fps encode, this value must larger than 34. -1 means wait forever */
    struct {
        uint32_t allow_pd:               1;   /*!< If set, the driver will backup/restore the JPEG registers before/after entering/exist sleep mode.
                                                   By this approach, the system can power off JPEG's power domain.
                                                   This can save power, but at the expense of more RAM being consumed */
    } flags;                              /*!< JPEG engine configuration flags */
} jpeg_encode_engine_cfg_t;

/**
 * @brief JPEG encoder memory allocation config
 */
typedef struct {
    jpeg_enc_buffer_alloc_direction_t buffer_direction;  /*!< Buffer direction for jpeg encoder memory allocation */
} jpeg_encode_memory_alloc_cfg_t;

/**
 * @brief Allocate JPEG encoder
 *
 * @param[in] enc_eng_cfg config for jpeg encoder
 * @param[out] ret_encoder handle for jpeg encoder
 * @return
 *      - ESP_OK: JPEG encoder initialized successfully.
 *      - ESP_ERR_INVALID_ARG: JPEG encoder initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create JPEG encoder failed because of out of memory.
 */
esp_err_t jpeg_new_encoder_engine(const jpeg_encode_engine_cfg_t *enc_eng_cfg, jpeg_encoder_handle_t *ret_encoder);

/**
 * @brief Process encoding of JPEG data using the specified encoder engine.
 *
 * This function processes the encoding of JPEG data using the provided encoder engine
 * and configuration. It takes an input buffer containing the raw image data, performs
 * encoding based on the configuration settings, and outputs the compressed bitstream.
 *
 * @param[in] encoder_engine Handle to the JPEG encoder engine to be used for encoding.
 * @param[in] encode_cfg Pointer to the configuration structure for the JPEG encoding process.
 * @param[in] encode_inbuf Pointer to the input buffer containing the raw image data.
 * @param[in] inbuf_size Size of the input buffer in bytes.
 * @param[in] encode_outbuf Pointer to the output buffer where the compressed bitstream will be stored.
 * @param[in] outbuf_size The size of output buffer.
 * @param[out] out_size Pointer to a variable where the size of the output bitstream will be stored.
 *
 * @return
 *      - ESP_OK: JPEG encoder process successfully.
 *      - ESP_ERR_INVALID_ARG: JPEG encoder process failed because of invalid argument.
 *      - ESP_ERR_TIMEOUT: JPEG encoder process timeout.
 */
esp_err_t jpeg_encoder_process(jpeg_encoder_handle_t encoder_engine, const jpeg_encode_cfg_t *encode_cfg, const uint8_t *encode_inbuf, uint32_t inbuf_size, uint8_t *encode_outbuf, uint32_t outbuf_size, uint32_t *out_size);

/**
 * @brief Release resources used by a JPEG encoder instance.
 *
 * This function releases the resources used by the specified JPEG encoder instance. The encoder instance is
 * specified by the `encoder_engine` parameter.
 *
 * @param[in] encoder_engine Handle of the JPEG encoder instance to release resources for.
 * @return
 *      - ESP_OK: Delete JPEG encoder successfully.
 *      - ESP_ERR_INVALID_ARG: Delete JPEG encoder failed because of invalid argument.
 */
esp_err_t jpeg_del_encoder_engine(jpeg_encoder_handle_t encoder_engine);

/**
 * @brief A helper function to allocate memory space for JPEG encoder.
 *
 * @param[in] size The size of memory to allocate.
 * @param[in] mem_cfg Memory configuration for memory allocation
 * @param[out] allocated_size Actual allocated buffer size.
 * @return Pointer to the allocated memory space, or NULL if allocation fails.
 */
void *jpeg_alloc_encoder_mem(size_t size, const jpeg_encode_memory_alloc_cfg_t *mem_cfg, size_t *allocated_size);

#ifdef __cplusplus
}
#endif
