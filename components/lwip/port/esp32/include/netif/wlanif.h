/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _WLAN_LWIP_IF_H_
#define _WLAN_LWIP_IF_H_

#include "esp_wifi.h"

#include "lwip/err.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif


err_t wlanif_init_ap(struct netif *netif);
err_t wlanif_init_sta(struct netif *netif);
err_t set_wifi_netif(int wifi_inx, void* netif);
esp_err_t wifi_rxcb_sta(void *buffer, uint16_t len, void *l2_buff);
esp_err_t wifi_rxcb_ap(void *buffer, uint16_t len, void *l2_buff);


void wlanif_input(void *netif, void *buffer, size_t len, void* eb);
err_t wlanif_init(struct netif *netif);

wifi_interface_t wifi_get_interface(void *dev);

void netif_reg_addr_change_cb(void* cb);

#ifdef __cplusplus
}
#endif

#endif /*  _WLAN_LWIP_IF_H_ */
