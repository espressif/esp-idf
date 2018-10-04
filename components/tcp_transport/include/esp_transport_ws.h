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

void esp_transport_ws_set_path(esp_transport_handle_t t, const char *path);



#ifdef __cplusplus
}
#endif

#endif /* _ESP_TRANSPORT_WS_H_ */
