/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 * Tuan PM <tuanpm at live dot com>
 */

#ifndef _ESP_TRANSPORT_WS_H_
#define _ESP_TRANSPORT_WS_H_

#include "esp_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Create web socket transport
 *
 * @return
 *  - transport
 *  - NULL
 */
esp_transport_handle_t esp_transport_ws_init(esp_transport_handle_t parent_handle);

/**
 * @brief       Set HTTP path to update protocol to websocket
 *
 * @param t     websocket transport handle
 * @param path  The HTTP Path
 */
void esp_transport_ws_set_path(esp_transport_handle_t t, const char *path);

/**
 * @brief               Set websocket sub protocol header
 *
 * @param t             websocket transport handle
 * @param sub_protocol  Sub protocol string
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes
 */
esp_err_t esp_transport_ws_set_subprotocol(esp_transport_handle_t t, const char *sub_protocol);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_TRANSPORT_WS_H_ */
