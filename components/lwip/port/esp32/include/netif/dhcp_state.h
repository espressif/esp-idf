/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _DHCP_STATE_H_
#define _DHCP_STATE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool dhcp_ip_addr_restore(void *netif);

void dhcp_ip_addr_store(void *netif);

void dhcp_ip_addr_erase(void *esp_netif);

#ifdef __cplusplus
}
#endif

#endif /*  _DHCP_STATE_H_ */
