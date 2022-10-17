/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROV_WIFI_CTRL_H_
#define _PROV_WIFI_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal handlers for receiving and responding to protocomm
 *          requests from client
 *
 * This is to be passed as priv_data for protocomm request handler
 * (refer to `wifi_ctrl_handler()`) when calling `protocomm_add_endpoint()`.
 */
typedef struct wifi_ctrl_handlers {
    /**
     * Handler function called when ctrl reset command is received
     */
    esp_err_t (*ctrl_reset)(void);

    /**
     * Handler function called when ctrl reprov command is received
     */
    esp_err_t (*ctrl_reprov)(void);

} wifi_ctrl_handlers_t;

/**
 * @brief   Handler for sending on demand Wi-Fi ctrl results
 *
 * This is to be registered as the `prov-ctrl` endpoint handler
 * (protocomm `protocomm_req_handler_t`) using `protocomm_add_endpoint()`
 */
esp_err_t wifi_ctrl_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                 uint8_t **outbuf, ssize_t *outlen, void *priv_data);

#ifdef __cplusplus
}
#endif

#endif
