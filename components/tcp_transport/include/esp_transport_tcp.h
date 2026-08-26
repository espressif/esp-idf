/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_TRANSPORT_TCP_H_
#define _ESP_TRANSPORT_TCP_H_

#include "esp_transport.h"
#include <sys/socket.h>
#include <net/if.h>

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
