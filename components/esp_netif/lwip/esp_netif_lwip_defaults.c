// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_netif.h"
#include "esp_netif_lwip_internal.h"

#include "netif/wlanif.h"
#include "netif/ethernetif.h"

//
// Purpose of this object is to define default network stack configuration
//  of basic types of interfaces using lwip network stack
//

static const struct esp_netif_netstack_config s_eth_netif_config = {
        .init_fn = ethernetif_init,
        .input_fn = ethernetif_input
};
static const struct esp_netif_netstack_config s_wifi_netif_config_ap = {
        .init_fn = wlanif_init_ap,
        .input_fn = wlanif_input
};
static const struct esp_netif_netstack_config s_wifi_netif_config_sta = {
        .init_fn = wlanif_init_sta,
        .input_fn = wlanif_input
};

const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_eth      = &s_eth_netif_config;
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_sta = &s_wifi_netif_config_sta;
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_ap  = &s_wifi_netif_config_ap;