/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 *  Some legacy APIs of A2DP, will be removed in the future
 */

#pragma once

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           [Deprecated] A2DP sink data callback function
 *
 * @param[in]       buf : pointer to the data received from A2DP source device and is PCM format decoded from SBC decoder;
 *                  buf references to a static memory block and can be overwritten by upcoming data
 *
 * @param[in]       len : size(in bytes) in buf
 */
typedef void (* esp_a2d_sink_data_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief           [Deprecated] A2DP source data read callback function
 *
 * @param[in]       buf : buffer to be filled with PCM data stream from higher layer
 *
 * @param[in]       len : size(in bytes) of data block to be copied to buf. -1 is an indication to user
 *                  that data buffer shall be flushed
 *
 * @return          size of bytes read successfully, if the argument len is -1, this value is ignored.
 *
 */
typedef int32_t (* esp_a2d_source_data_cb_t)(uint8_t *buf, int32_t len);

/**
 * @brief           [Deprecated] Register A2DP sink data output function; For now the output is PCM data stream decoded
 *                  from SBC format. This function should be called only after esp_bluedroid_enable()
 *                  completes successfully, used only by A2DP sink. The callback is invoked in the context
 *                  of A2DP sink task whose stack size is configurable through menuconfig.
 *
 * @param[in]       callback: A2DP sink data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_a2d_sink_register_data_callback(esp_a2d_sink_data_cb_t callback);

/**
 * @brief           [Deprecated] Register A2DP source data input function. For now, the input should be PCM data stream.
 *                  This function should be called only after esp_bluedroid_enable() completes
 *                  successfully. The callback is invoked in the context of A2DP source task whose
 *                  stack size is configurable through menuconfig.
 *
 * @param[in]       callback: A2DP source data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_a2d_source_register_data_callback(esp_a2d_source_data_cb_t callback);

#ifdef __cplusplus
}
#endif
