/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <inttypes.h>
#include "cmd_ethernet.h"
#include "esp_netif.h"
#include "esp_console.h"
#include "esp_eth.h"
#include "esp_mac.h"
#include "argtable3/argtable3.h"

/* "ethernet" command */
static struct {
    struct arg_str *control;
    struct arg_end *end;
} eth_control_args;

static int eth_cmd_control(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&eth_control_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, eth_control_args.end, argv[0]);
        return 1;
    }

    if (!strncmp(eth_control_args.control->sval[0], "info", 4)) {
        uint8_t mac_addr[6];
        esp_netif_ip_info_t ip;
        esp_netif_t *esp_netif;
        // we can use esp_netif_next_unsafe since we one time initialize the network and we don't de-init
        esp_netif = esp_netif_next_unsafe(NULL);
        while(esp_netif != NULL) {
            printf("%s:\r\n", esp_netif_get_desc(esp_netif));
            esp_eth_handle_t eth_hndl = esp_netif_get_io_driver(esp_netif);
            esp_eth_ioctl(eth_hndl, ETH_CMD_G_MAC_ADDR, mac_addr);
            printf("  HW ADDR: " MACSTR "\r\n", MAC2STR(mac_addr));
            esp_netif_get_ip_info(esp_netif, &ip);
            printf("  ETHIP: " IPSTR "\r\n", IP2STR(&ip.ip));
            printf("  ETHMASK: " IPSTR "\r\n", IP2STR(&ip.netmask));
            printf("  ETHGW: " IPSTR "\r\n", IP2STR(&ip.gw));
            esp_netif = esp_netif_next_unsafe(esp_netif);
        }
    }
    return 0;
}

void register_ethernet_commands(void)
{
    eth_control_args.control = arg_str1(NULL, NULL, "<info>", "Get info of Ethernet");
    eth_control_args.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "ethernet",
        .help = "Ethernet interface IO control",
        .hint = NULL,
        .func = eth_cmd_control,
        .argtable = &eth_control_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
