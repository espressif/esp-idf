/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 * Tuan PM <tuanpm at live dot com>
 */

#ifndef _ESP_TRANSPORT_WS_H_
#define _ESP_TRANSPORT_WS_H_

#include "esp_transport.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ws_transport_opcodes {
    WS_TRANSPORT_OPCODES_CONT =  0x00,
    WS_TRANSPORT_OPCODES_TEXT =  0x01,
    WS_TRANSPORT_OPCODES_BINARY = 0x02,
    WS_TRANSPORT_OPCODES_CLOSE = 0x08,
    WS_TRANSPORT_OPCODES_PING = 0x09,
    WS_TRANSPORT_OPCODES_PONG = 0x0a,
    WS_TRANSPORT_OPCODES_FIN = 0x80,
    WS_TRANSPORT_OPCODES_NONE = 0x100,   /*!< not a valid opcode to indicate no message previously received
                                          * from the API esp_transport_ws_get_read_opcode() */
} ws_transport_opcodes_t;

/**
 * WS transport configuration structure
 */
typedef struct {
    const char *ws_path;                    /*!< HTTP path to update protocol to websocket */
    const char *sub_protocol;               /*!< WS subprotocol */
    const char *user_agent;                 /*!< WS user agent */
    const char *headers;                    /*!< WS additional headers */
    const char *auth;                       /*!< HTTP authorization header */
    bool        propagate_control_frames;   /*!< If true, control frames are passed to the reader
                                             *   If false, only user frames are propagated, control frames are handled
                                             *   automatically during read operations
                                             */
} esp_transport_ws_config_t;

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

/**
 * @brief               Set websocket user-agent header
 *
 * @param t             websocket transport handle
 * @param sub_protocol  user-agent string
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes
 */
esp_err_t esp_transport_ws_set_user_agent(esp_transport_handle_t t, const char *user_agent);

/**
 * @brief               Set websocket additional headers
 *
 * @param t             websocket transport handle
 * @param sub_protocol  additional header strings each terminated with \r\n
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes
 */
esp_err_t esp_transport_ws_set_headers(esp_transport_handle_t t, const char *headers);

/**
 * @brief               Set websocket authorization headers
 *
 * @param t             websocket transport handle
 * @param sub_protocol  The HTTP authorization header string, set NULL to clear the old value
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes
 */
esp_err_t esp_transport_ws_set_auth(esp_transport_handle_t t, const char *auth);

/**
 * @brief               Set websocket transport parameters
 *
 * @param t             websocket transport handle
 * @param config        pointer to websocket config structure
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes
 */
esp_err_t esp_transport_ws_set_config(esp_transport_handle_t t, const esp_transport_ws_config_t *config);

/**
 * @brief               Sends websocket raw message with custom opcode and payload
 *
 * Note that generic esp_transport_write for ws handle sends
 * binary massages by default if size is > 0 and
 * ping message if message size is set to 0.
 * This API is provided to support explicit messages with arbitrary opcode,
 * should it be PING, PONG or TEXT message with arbitrary data.
 *
 * @param[in]  t           Websocket transport handle
 * @param[in]  opcode      ws operation code
 * @param[in]  buffer      The buffer
 * @param[in]  len         The length
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates block forever)
 *
 * @return
 *  - Number of bytes was written
 *  - (-1) if there are any errors, should check errno
 */
int esp_transport_ws_send_raw(esp_transport_handle_t t, ws_transport_opcodes_t opcode, const char *b, int len, int timeout_ms);

/**
 * @brief               Returns websocket fin flag for last received data
 *
 * @param t             websocket transport handle
 *
 * @return
 *      - Fin flag as a boolean
 */
bool esp_transport_ws_get_fin_flag(esp_transport_handle_t t);

/**
 * @brief               Returns the HTTP status code of the websocket handshake
 *
 * This API should be called after the connection atempt otherwise its result is meaningless
 *
 * @param t             websocket transport handle
 *
 * @return
 *      -Response status code
 *      -1 on failure
 */
int esp_transport_ws_get_upgrade_request_status(esp_transport_handle_t t);

/**
 * @brief               Returns websocket op-code for last received data
 *
 * @param t             websocket transport handle
 *
 * @return
 *      - Received op-code as enum
 */
ws_transport_opcodes_t esp_transport_ws_get_read_opcode(esp_transport_handle_t t);

/**
 * @brief               Returns payload length of the last received data
 *
 * @param t             websocket transport handle
 *
 * @return
 *      - Number of bytes in the payload
 */
int esp_transport_ws_get_read_payload_len(esp_transport_handle_t t);

/**
 * @brief               Polls the active connection for termination
 *
 * This API is typically used by the client to wait for clean connection closure
 * by websocket server
 *
 * @param t             Websocket transport handle
 * @param[in] timeout_ms The timeout milliseconds
 *
 * @return
 *      0 - no activity on read and error socket descriptor within timeout
 *      1 - Success: either connection terminated by FIN or the most common RST err codes
 *      -1 - Failure: Unexpected error code or socket is normally readable
 */
int esp_transport_ws_poll_connection_closed(esp_transport_handle_t t, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_TRANSPORT_WS_H_ */
