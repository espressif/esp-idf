/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/jpeg_types.h"
#include "../jpeg_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves a specified number of bytes from the JPEG decoder.
 *
 * @param header_info The handle to the JPEG information.
 * @param num_bytes   The number of bytes to retrieve from the decoder.
 *
 * @note num_bytes should not larger than 4, because the return value is uint32.
 *
 * @return The retrieved bytes as a 32-bit unsigned integer.
 */
uint32_t jpeg_get_bytes(jpeg_dec_header_info_t *header_info, uint8_t num_bytes);

/**
 * @brief Parses with the APPn (application specific) marker in a JPEG file.
 *
 * This function is called when the decoder encounters an APPn marker in the
 * input stream. The function handles any application-specific data contained
 * within the marker and performs necessary processing or actions based on the
 * specific application requirements.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_appn_marker(jpeg_dec_header_info_t *header_info);

/**
 * @brief Parses with the COM (comment) marker in a JPEG file.
 *
 * This function is called when the decoder encounters a COM marker in the input stream.
 * The function handles any comment data contained within the marker and performs any
 * necessary processing or actions based on the comment information.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_com_marker(jpeg_dec_header_info_t *header_info);

/**
 * @brief Parses with the DQT (quantization table) marker in a JPEG file.
 *
 * This function is called when the decoder encounters a DQT marker in the input stream.
 * The function handles the quantization table data contained within the marker and
 * performs any necessary processing or actions based on the quantization table information.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_dqt_marker(jpeg_dec_header_info_t *header_info);

/**
 * @brief Parses with the SOF (Start of Frame) marker in a JPEG file.
 *
 * This function only used for verify there is an SOF marker, the content of frame
 * would be handle in hardware.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_sof_marker(jpeg_dec_header_info_t *header_info);

/**
 * @brief Parses with the DHT (Huffman table) marker in a JPEG file.
 *
 * This function is called when the decoder encounters a DHT marker in the input stream.
 * The function handles the Huffman table data contained within the marker and performs
 * any necessary processing or actions based on the Huffman table information.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_dht_marker(jpeg_dec_header_info_t *header_info);

/**
 * @brief Parses with the SOS (Start of Scan) marker in a JPEG file.
 *
 * This function is called when the decoder encounters a SOS marker in the input stream.
 * The function handles the scan header data contained within the marker and performs
 * any necessary processing or actions based on the scan information.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_sos_marker(jpeg_dec_header_info_t *header_info);

/**
 * @brief Parses with the DRI (Define Restart Interval) marker in a JPEG file.
 *
 * This function is called when the decoder encounters a DRI marker in the input stream.
 * The function handles the restart interval data contained within the marker and performs
 * any necessary processing or actions based on the restart interval information.
 *
 * @param[in] header_info Pointer to the JPEG picture information.
 *
 * @return    ESP_OK on success, or an appropriate error code if an error occurred.
 */
esp_err_t jpeg_parse_dri_marker(jpeg_dec_header_info_t *header_info);

#ifdef __cplusplus
}
#endif
