/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 * Tuan PM <tuanpm at live dot com>
 */

#ifndef _TRANSPORT_WS_H_
#define _TRANSPORT_WS_H_

#include "esp_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127
#define MAX_WEBSOCKET_HEADER_SIZE 10
#define WS_RESPONSE_OK    101

/**
 * @brief      Create TCP transport
 *
 * @return
 *  - transport
 *  - NULL
 */
transport_handle_t transport_ws_init(transport_handle_t parent_handle);

void transport_ws_set_path(transport_handle_t t, const char *path);



#ifdef __cplusplus
}
#endif

#endif
