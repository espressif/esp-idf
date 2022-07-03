/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LWIP_ESP_DHCP_STATE_H
#define LWIP_ESP_DHCP_STATE_H

#include <stdbool.h>
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

bool dhcp_ip_addr_restore(struct netif *netif);

void dhcp_ip_addr_store(struct netif *netif);

void dhcp_ip_addr_erase(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /*  LWIP_ESP_DHCP_STATE_H */
