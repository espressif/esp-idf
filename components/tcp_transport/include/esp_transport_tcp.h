// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_TRANSPORT_TCP_H_
#define _ESP_TRANSPORT_TCP_H_

#include "esp_transport.h"
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Set TCP keep-alive configuration
 *
 * @param[in]  t               The transport handle
 * @param[in]  keep_alive_cfg  The keep-alive config
 *
 */
void esp_transport_tcp_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg);

/**
 * @brief      Set name of interface that socket can be binded on
 *             So the data can transport on this interface
 *
 * @param[in]  t        The transport handle
 * @param[in]  if_name  The interface name
 */
void esp_transport_tcp_set_interface_name(esp_transport_handle_t t, struct ifreq *if_name);

/**
 * @brief      Create TCP transport, the transport handle must be release esp_transport_destroy callback
 *
 * @return  the allocated esp_transport_handle_t, or NULL if the handle can not be allocated
 */
esp_transport_handle_t esp_transport_tcp_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _ESP_TRANSPORT_TCP_H_ */
