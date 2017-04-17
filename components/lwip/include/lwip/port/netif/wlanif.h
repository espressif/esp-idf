// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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


#ifndef _WLAN_LWIP_IF_H_
#define _WLAN_LWIP_IF_H_

#include "esp_wifi.h"

#include "esp_wifi_internal.h"

#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

err_t wlanif_init_ap(struct netif *netif);
err_t wlanif_init_sta(struct netif *netif);

void wlanif_input(struct netif *netif, void *buffer, u16_t len, void* eb);

wifi_interface_t wifi_get_interface(void *dev);

void netif_reg_addr_change_cb(void* cb);

#ifdef __cplusplus
}
#endif

#endif /*  _WLAN_LWIP_IF_H_ */
