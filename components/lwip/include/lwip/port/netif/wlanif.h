/*
 *  Copyright (c) 2010-2011 	Espressif System
 *   
*/

#ifndef _WLAN_LWIP_IF_H_
#define _WLAN_LWIP_IF_H_

#include "esp_wifi.h"

#include "esp_wifi_internal.h"

#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

err_t wlanif_init(struct netif *netif);

void wlanif_input(struct netif *netif, void *buffer, u16_t len, void* eb);

wifi_interface_t wifi_get_interface(void *dev);

void netif_reg_addr_change_cb(void* cb);

#ifdef __cplusplus
}
#endif

#endif /*  _WLAN_LWIP_IF_H_ */
