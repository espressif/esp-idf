/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Net-suite test code

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "stdinout.h"
#include "lwip/err.h"
#include "lwip/debug.h"
#include "lwip/tcp.h"

/* these test data are used to populate the ARP cache so the IPs are known */
static char arp1[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x01,
    0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a, 0x00, 0x00, 0x01
};

/* Test data (ICMP packet) for verification of tcp ip test netif
    00-00-00-00-00-01-00-00-00-00-00-02-08-00-45-00-00-1c-00-00-00-00-ff-01-a7-de-0a-00-00-02-0a-00-00-01-08-00-f7-fd-00-01-00-01
*/

/* creating test pcb */
static struct tcp_pcb *test_pcb;

err_t test_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    return ERR_OK;
}

void test_error(void *arg, err_t err)
{
    printf("Error CB from pcb %d\n", err);
}

err_t test_poll(void *arg, struct tcp_pcb *tpcb)
{
    return ERR_OK;
}

err_t test_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    tcp_setprio(newpcb, TCP_PRIO_MIN);
    tcp_arg(newpcb, NULL);
    tcp_recv(newpcb, test_recv);
    tcp_err(newpcb, test_error);
    tcp_poll(newpcb, test_poll, 0);

    return ERR_OK;
}

void test_tcp_init(void)
{
    test_pcb = tcp_new();
    if (test_pcb != NULL) {
        err_t err;
        /* Binding this test_pcb to 4242 to accept connections on this port
         * - this has to be configured as DUT endpoint
         * - all network traffic from and to network stack is tracked in nettestif
         */
        err = tcp_bind(test_pcb, IP_ADDR_ANY, 4242);
        if (err == ERR_OK) {
            test_pcb = tcp_listen(test_pcb);
            tcp_accept(test_pcb, test_accept);
        } else  {
            printf("cannot bind test_pcb\n");
            abort();
        }
    } else  {
        printf("cannot create test_pcb\n");
        abort();
    }
}


void app_main(void)
{
    char packet[128];

    // Netif configs
    //
    esp_netif_ip_info_t ip_info;
    uint8_t mac[] =  { 0,0,0,0,0,1};
    esp_netif_inherent_config_t netif_common_config = {
            .flags = ESP_NETIF_FLAG_AUTOUP,
            .ip_info = (esp_netif_ip_info_t*)&ip_info,
            .if_key = "TEST",
            .if_desc = "net_test_if"
    };
    esp_netif_set_ip4_addr(&ip_info.ip, 10, 0 , 0, 1);
    esp_netif_set_ip4_addr(&ip_info.gw, 10, 0 , 0, 1);
    esp_netif_set_ip4_addr(&ip_info.netmask, 255, 255 , 255, 0);

    esp_netif_config_t config = {
        .base = &netif_common_config,                 // use specific behaviour configuration
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA, // use default WIFI-like network stack configuration
    };

    // Netif creation and configuration
    //
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_t* netif = esp_netif_new(&config);
    assert(netif);
    esp_netif_attach(netif, netsuite_io_new());

    // Start the netif in a manual way, no need for events
    //
    esp_netif_set_mac(netif, mac);
    esp_netif_action_start(netif, NULL, 0, NULL);

    // initializes TCP endpoint on DUT per https://github.com/intel/net-test-suites#21-endpoints
    test_tcp_init();
    // Inject ARP packet to let the network stack know about IP/MAC of the counterpart
    esp_netif_receive(netif, arp1, sizeof(arp1), NULL);

    /* Now read from stdin and pass the data to test netif */
    while (1) {
        /* read one packet from the I/O object */
        ssize_t len = netsuite_io_get_packet(packet, sizeof(packet));
        if (len > 0) {
            /* input the packet to esp-netif */
            esp_netif_receive(netif, packet, len, NULL);
        }
    }

}
