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
 * @brief Configuration parameters for a JPEG decoder image process.
 */
typedef struct {
    jpeg_dec_output_format_t output_format;   /*!< JPEG decoder output format */
    jpeg_dec_rgb_element_order_t rgb_order;   /*!< JPEG decoder output order */
    jpeg_yuv_rgb_conv_std_t conv_std;         /*!< JPEG decoder yuv->rgb standard */
} jpeg_decode_cfg_t;

/**
 * @brief Configuration parameters for the JPEG decoder engine.
 */
typedef struct {
    int intr_priority;                    /*!< JPEG interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    int timeout_ms;                       /*!< JPEG timeout threshold for handling a picture, should larger than valid decode time in ms. For example, for 30fps decode, this value must larger than 34. -1 means wait forever */
    struct {
        uint32_t allow_pd:               1;   /*!< If set, the driver will backup/restore the JPEG registers before/after entering/exist sleep mode.
                                                   By this approach, the system can power off JPEG's power domain.
                                                   This can save power, but at the expense of more RAM being consumed */
    } flags;                              /*!< JPEG engine configuration flags */
} jpeg_decode_engine_cfg_t;

/**
 * @brief Structure for jpeg decode header
 */
typedef struct {
    uint32_t width;          /*!< Number of pixels in the horizontal direction */
    uint32_t height;         /*!< Number of pixels in the vertical direction */
    jpeg_down_sampling_type_t sample_method;     /*!< compressed JPEG picture sampling method */
} jpeg_decode_picture_info_t;

/**
 * @brief JPEG decoder memory allocation config
 */
typedef struct {
    jpeg_dec_buffer_alloc_direction_t buffer_direction;  /*!< Buffer direction for jpeg decoder memory allocation */
} jpeg_decode_memory_alloc_cfg_t;

/**
 * @brief Acquire a JPEG decode engine with the specified configuration.
 *
 * This function acquires a JPEG decode engine with the specified configuration. The configuration
 * parameters are provided through the `dec_eng_cfg` structure, and the resulting JPEG decoder handle
 * is returned through the `ret_decoder` pointer.
 *
 * @param[in] dec_eng_cfg Pointer to the JPEG decode engine configuration.
 * @param[out] ret_decoder Pointer to a variable that will receive the JPEG decoder handle.
 * @return
 *      - ESP_OK: JPEG decoder initialized successfully.
 *      - ESP_ERR_INVALID_ARG: JPEG decoder initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create JPEG decoder failed because of out of memory.
 */
esp_err_t jpeg_new_decoder_engine(const jpeg_decode_engine_cfg_t *dec_eng_cfg, jpeg_decoder_handle_t *ret_decoder);

/**
 * @brief Helper function for getting information about a JPEG image.
 *
 * This function analyzes the provided JPEG image data and retrieves information about the image,
 * such as its width, height. The image data is specified by the `bit_stream` pointer and the `stream_size` parameter.
 * The resulting image information is returned through the `picture_info` structure.
 *
 * @note This function doesn't depend on any jpeg hardware, it helps user to know jpeg information from jpeg header. For example,
 * user can get picture width and height via this function and malloc a reasonable size buffer for jpeg engine process.
 *
 * @param[in] bit_stream Pointer to the buffer containing the JPEG image data.
 * @param[in] stream_size Size of the JPEG image data in bytes. Note that parse beginning partial of picture also works, but the beginning partial should be enough given.
 * @param[out] picture_info Pointer to the structure that will receive the image information.
 * @return
 *      - ESP_OK: JPEG decoder get jpg image header successfully.
 *      - ESP_ERR_INVALID_ARG: JPEG decoder get header info failed because of invalid argument.
 */
esp_err_t jpeg_decoder_get_info(const uint8_t *bit_stream, uint32_t stream_size, jpeg_decode_picture_info_t *picture_info);

/**
 * @brief Process a JPEG image with the specified decoder instance.
 *
 * This function processes the provided JPEG image data using the specified JPEG decoder instance. The input
 * JPEG image data is specified by the `bit_stream` pointer and the `stream_size` parameter. The resulting
 * decoded image data is written to the `decode_outbuf` buffer, and the length of the output image data is
 * returned through the `out_size` pointer.
 *
 * @note 1.Please make sure that the content of `bit_stream` pointer cannot be modified until this function returns.
 *       2.Please note that the output size of image is always the multiple of 16 depends on protocol of JPEG.
 *
 * @param[in] decoder_engine Handle of the JPEG decoder instance to use for processing.
 * @param[in] decode_cfg Config structure of decoder.
 * @param[in] bit_stream Pointer to the buffer containing the input JPEG image data.
 * @param[in] stream_size Size of the input JPEG image data in bytes.
 * @param[in] decode_outbuf Pointer to the buffer that will receive the decoded image data.
 * @param[in] outbuf_size The size of `decode_outbuf`
 * @param[out] out_size Pointer to a variable that will receive the length of the output image data.
 * @return
 *      - ESP_OK: JPEG decoder process successfully.
 *      - ESP_ERR_INVALID_ARG: JPEG decoder process failed because of invalid argument.
 */
esp_err_t jpeg_decoder_process(jpeg_decoder_handle_t decoder_engine, const jpeg_decode_cfg_t *decode_cfg, const uint8_t *bit_stream, uint32_t stream_size, uint8_t *decode_outbuf, uint32_t outbuf_size, uint32_t *out_size);

/**
 * @brief Release resources used by a JPEG decoder instance.
 *
 * This function releases the resources used by the specified JPEG decoder instance. The decoder instance is
 * specified by the `decoder_engine` parameter.
 *
 * @param decoder_engine Handle of the JPEG decoder instance to release resources for.
 * @return
 *      - ESP_OK: Delete JPEG decoder successfully.
 *      - ESP_ERR_INVALID_ARG: Delete JPEG decoder failed because of invalid argument.
 */
esp_err_t jpeg_del_decoder_engine(jpeg_decoder_handle_t decoder_engine);

/**
 * @brief A helper function to allocate memory space for JPEG decoder.
 *
 * @param[in] size The size of memory to allocate.
 * @param[in] mem_cfg Memory configuration for memory allocation
 * @param[out] allocated_size Actual allocated buffer size.
 * @return Pointer to the allocated memory space, or NULL if allocation fails.
 */
void *jpeg_alloc_decoder_mem(size_t size, const jpeg_decode_memory_alloc_cfg_t *mem_cfg, size_t *allocated_size);

#ifdef __cplusplus
}
#endif
