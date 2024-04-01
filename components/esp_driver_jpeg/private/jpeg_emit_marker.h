/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "../jpeg_private.h"
#include "driver/jpeg_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Emit Start of Image (SOI) marker.
 *
 * This function emits the Start of Image (SOI) marker, which indicates the beginning of a JPEG image.
 * It writes the SOI marker to the output buffer specified in the `header_info` structure.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted SOI marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing SOI marker.
 */
esp_err_t emit_soi_marker(jpeg_enc_header_info_t *header_info);

/**
 * @brief Emit Application 0 (APP0) marker.
 *
 * This function emits the Application 0 (APP0) marker, which is used to provide information about the JFIF (JPEG File Interchange Format)
 * or other application-specific data. It writes the APP0 marker and the corresponding data segment to the output buffer specified
 * in the `header_info` structure.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted APP0 marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing APP0 marker.
 */
esp_err_t emit_app0_marker(jpeg_enc_header_info_t *header_info);

/**
 * @brief Emit Define Quantization Table (DQT) marker.
 *
 * This function emits the Define Quantization Table (DQT) marker, which is used to specify the quantization table(s)
 * used for encoding the image data. It writes the DQT marker and the corresponding quantization table(s) to the output buffer
 * specified in the `header_info` structure.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted DQT marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing DQT marker.
 */
esp_err_t emit_dqt_marker(jpeg_enc_header_info_t *header_info);

/**
 * @brief Emit Start of Frame (SOF) marker.
 *
 * This function emits the Start of Frame (SOF) marker, which is used to define the basic parameters of the image frame,
 * such as the image dimensions and the number of color components. It writes the SOF marker and the corresponding
 * frame header data to the output buffer specified in the `header_info` structure.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted SOF marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing SOF marker.
 */
esp_err_t emit_sof_marker(jpeg_enc_header_info_t *header_info);

/**
 * @brief Emit Define Huffman Table (DHT) marker.
 *
 * This function emits the Define Huffman Table (DHT) marker, which is used to specify the Huffman coding tables
 * used for encoding the image data. It writes the DHT marker and the corresponding Huffman coding table(s) to the
 * output buffer specified in the `header_info` structure.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted DHT marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing DHT marker.
 */
esp_err_t emit_dht_marker(jpeg_enc_header_info_t *header_info);

/**
 * @brief Emit Start of Scan (SOS) marker.
 *
 * This function emits the Start of Scan (SOS) marker, which is used to specify the image component layout and
 * the Huffman coding tables used for encoding the image data. It writes the SOS marker and the corresponding scan
 * header data to the output buffer specified in the `header_info` structure.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted SOS marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing SOS marker.
 */
esp_err_t emit_sos_marker(jpeg_enc_header_info_t *header_info);

/**
 * @brief Emit Comment (COM) marker.
 *
 * This function is used for adjust picture header size. Picture body follows alignment rules. So in header emit stage,
 * We add bytes in COM sector to adjust picture header size.
 *
 * @param[in] header_info Pointer to the structure containing JPEG encoding header information.
 *
 * @return
 *     - ESP_OK: Successfully emitted SOS marker.
 *     - ESP_ERR_INVALID_ARG: Invalid argument passed.
 *     - ESP_FAIL: Error occurred while writing SOS marker.
 */
esp_err_t emit_com_marker(jpeg_enc_header_info_t *header_info);

#ifdef __cplusplus
}
#endif
