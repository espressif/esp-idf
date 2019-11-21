// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_modem.h"
#include "esp_modem_netif.h"
#include "esp_log.h"

static const char *TAG = "esp-modem-compat";

static void on_modem_compat_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    int32_t compat_event_id = MODEM_EVENT_UNKNOWN;
    switch (event_id) {
        case ESP_MODEM_EVENT_PPP_START:
            compat_event_id = MODEM_EVENT_PPP_START;
            break;
        case ESP_MODEM_EVENT_PPP_STOP:
            compat_event_id = MODEM_EVENT_PPP_STOP;
            break;
        default:
            break;
    }
    esp_event_post(ESP_MODEM_EVENT, compat_event_id, NULL, 0, 0);
}

static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "IP event! %d", event_id);
    if (event_id == IP_EVENT_PPP_GOT_IP) {
        esp_netif_dns_info_t dns_info;
        ppp_client_ip_info_t ipinfo = {0};
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        esp_netif_t *netif = event->esp_netif;
        ipinfo.ip.addr = event->ip_info.ip.addr;
        ipinfo.gw.addr = event->ip_info.gw.addr;
        ipinfo.netmask.addr = event->ip_info.netmask.addr;
        esp_netif_get_dns_info(netif, 0, &dns_info);
        ipinfo.ns1.addr = dns_info.ip.u_addr.ip4.addr;
        ipinfo.ns2.addr = dns_info.ip.u_addr.ip4.addr;
        esp_event_post(ESP_MODEM_EVENT, MODEM_EVENT_PPP_CONNECT, &ipinfo, sizeof(ipinfo), 0);
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
        esp_event_post(ESP_MODEM_EVENT, MODEM_EVENT_PPP_DISCONNECT, NULL, 0, 0);
    }
}

esp_err_t esp_modem_add_event_handler(modem_dte_t *dte, esp_event_handler_t handler, void *handler_args)
{
    // event loop has to be created when using this API -- create and ignore failure if already created
    esp_event_loop_create_default();
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_MODEM_EVENT, MODEM_EVENT_PPP_START, handler, handler_args));
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_MODEM_EVENT, MODEM_EVENT_PPP_CONNECT, handler, handler_args));
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_MODEM_EVENT, MODEM_EVENT_PPP_DISCONNECT, handler, handler_args));
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_MODEM_EVENT, MODEM_EVENT_PPP_STOP, handler, handler_args));
    return esp_modem_set_event_handler(dte, on_modem_compat_handler, ESP_EVENT_ANY_ID, handler_args);
}

esp_err_t esp_modem_setup_ppp(modem_dte_t *dte)
{
#if CONFIG_LWIP_PPP_PAP_SUPPORT
    esp_netif_auth_type_t auth_type = NETIF_PPP_AUTHTYPE_PAP;
#elif CONFIG_LWIP_PPP_CHAP_SUPPORT
    esp_netif_auth_type_t auth_type = NETIF_PPP_AUTHTYPE_CHAP;
#else
#error "Unsupported AUTH Negotiation"
#endif
    // Init netif object
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *esp_netif = esp_netif_new(&cfg);
    assert(esp_netif);

    // event loop has to be created when using this API -- create and ignore failure if already created
    esp_event_loop_create_default();
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL));
    esp_netif_ppp_set_auth(esp_netif, auth_type, CONFIG_EXAMPLE_MODEM_PPP_AUTH_USERNAME, CONFIG_EXAMPLE_MODEM_PPP_AUTH_PASSWORD);
    void *modem_netif_adapter = esp_modem_netif_setup(dte);
    esp_modem_netif_set_default_handlers(modem_netif_adapter, esp_netif);
    /* attach the modem to the network interface */
    return esp_netif_attach(esp_netif, modem_netif_adapter);
}

esp_err_t esp_modem_exit_ppp(modem_dte_t *dte)
{
    // Note: A minor memory leak is expected when using esp-modem-compat
    return esp_modem_stop_ppp(dte);
}
