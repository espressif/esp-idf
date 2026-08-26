/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 *  Some legacy APIs of HFP AG, will be removed in the future
 */

#pragma once

#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_hf_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           AG incoming data callback function, the callback is useful in case of
 *                  Voice Over HCI.
 *
 * @param[in]       buf : pointer to incoming data(payload of HCI synchronous data packet), the
 *                  buffer is allocated inside bluetooth protocol stack and will be released after
 *                  invoke of the callback is finished.
 *
 * @param[in]       len : size(in bytes) in buf
 */
typedef void (* esp_hf_incoming_data_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief           AG outgoing data callback function, the callback is useful in case of
 *                  Voice Over HCI. Once audio connection is set up and the application layer has
 *                  prepared data to send, the lower layer will call this function to read data
 *                  and then send. This callback is supposed to be implemented as non-blocking,
 *                  and if data is not enough, return value 0 is supposed.
 *
 * @param[in]       buf : pointer to incoming data(payload of HCI synchronous data packet), the
 *                  buffer is allocated inside bluetooth protocol stack and will be released after
 *                  invoke of the callback is finished.
 *
 * @param[in]       len : size(in bytes) in buf
 *
 * @return          length of data successfully read
 */
typedef uint32_t (* esp_hf_outgoing_data_cb_t) (uint8_t *buf, uint32_t len);

/**
 * @brief           Register AG data output function.
 *                  The callback is only used in the case that Voice Over HCI is enabled.
 *
 * @param[in]       recv: HFP client incoming data callback function
 * @param[in]       send: HFP client outgoing data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_hf_ag_register_data_callback(esp_hf_incoming_data_cb_t recv, esp_hf_outgoing_data_cb_t send);

/**
 * @brief           Trigger the lower-layer to fetch and send audio data.
 *
 *                  This function is only used in the case that Voice Over HCI is enabled.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *                  After this function is called, lower layer will invoke esp_hf_ag_outgoing_data_cb_t to fetch data
 *
 */
void esp_hf_ag_outgoing_data_ready(void);

#ifdef __cplusplus
}
#endif
