/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_eth_spec.h"
#include "bridge_console_cmd.h"

typedef struct {
    struct arg_str *addr;
    struct arg_int *port;
    struct arg_lit *drop;
    struct arg_lit *flood;
    struct arg_lit *cpu;
    struct arg_end *end;
} br_add_fdb_args_t;

typedef struct {
    struct arg_str *addr;
    struct arg_end *end;
} br_remove_fdb_args_t;

static const char *TAG = "br_config_console";

static br_add_fdb_args_t s_add_args;
static br_remove_fdb_args_t s_remove_args;
static esp_netif_t *s_br_netif;
static uint8_t s_br_port_cnt;

static esp_err_t str2mac(const char *str, uint8_t *mac_addr)
{
    unsigned int mac_tmp[ETH_ADDR_LEN];
    if (ETH_ADDR_LEN != sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x%*c",
                    &mac_tmp[0], &mac_tmp[1], &mac_tmp[2],
                    &mac_tmp[3], &mac_tmp[4], &mac_tmp[5])) {
        return ESP_ERR_INVALID_MAC;
    }
    for (int i = 0; i < ETH_ADDR_LEN; i++) {
        mac_addr[i] = (uint8_t)mac_tmp[i];
    }
    return ESP_OK;
}

static int cmd_br_fdb_add(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &s_add_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, s_add_args.end, argv[0]);
        return 1;
    }

    int exp_argc = 2;
    if (s_add_args.port->count > 0) {
        exp_argc += 2 * s_add_args.port->count;
        if (s_add_args.cpu->count > 0) {
            exp_argc++;
        }
    } else {
        exp_argc++;
    }
    ESP_RETURN_ON_FALSE(argc == exp_argc, 1, TAG, "Invalid number or combination of arguments");

    uint64_t port_mask = ESP_NETIF_BR_DROP;
    for (int i = 0; i < s_add_args.port->count; i++) {
        ESP_RETURN_ON_FALSE(s_add_args.port->ival[i] > 0 && s_add_args.port->ival[i] <= s_br_port_cnt, 1, TAG, "Invalid port number");
        port_mask |= 1 << ((uint64_t)(s_add_args.port->ival[i]) - 1);
    }
    if (s_add_args.drop->count > 0) {
        port_mask = ESP_NETIF_BR_DROP;
    }
    if (s_add_args.flood->count > 0) {
        port_mask = ESP_NETIF_BR_FLOOD;
    }
    if (s_add_args.cpu->count > 0) {
        port_mask |= ESP_NETIF_BR_FDW_CPU;
    }
    uint8_t mac_addr[ETH_ADDR_LEN];
    ESP_RETURN_ON_FALSE(str2mac(s_add_args.addr->sval[0], mac_addr) == ESP_OK,
                        1, TAG, "Ivalid MAC address format (expected xx:xx:xx:xx:xx:xx)");
    ESP_RETURN_ON_FALSE(esp_netif_bridge_fdb_add(s_br_netif, mac_addr, port_mask) == ESP_OK,
                        1, TAG, "Adding FDB entry failed");

    ESP_LOG_BUFFER_HEX_LEVEL(TAG, mac_addr, ETH_ADDR_LEN, ESP_LOG_DEBUG);
    ESP_LOGD(TAG, "portmask 0x%" PRIu64, port_mask);
    ESP_LOGI(TAG, "Bridge Config OK!");

    return 0;
}

static int cmd_br_fdb_remove(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &s_remove_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, s_remove_args.end, argv[0]);
        return 1;
    }

    uint8_t mac_addr[ETH_ADDR_LEN];
    ESP_RETURN_ON_FALSE(str2mac(s_remove_args.addr->sval[0], mac_addr) == ESP_OK,
                        1, TAG, "Ivalid MAC address format (expected xx:xx:xx:xx:xx:xx)");
    ESP_RETURN_ON_FALSE(esp_netif_bridge_fdb_remove(s_br_netif, mac_addr) == ESP_OK,
                        1, TAG, "Removing FDB entry failed");

    ESP_LOG_BUFFER_HEX_LEVEL(TAG, mac_addr, ETH_ADDR_LEN, ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Bridge Config OK!");
    return 0;
}

void example_register_br_config_commands(esp_netif_t *br_netif, uint8_t br_port_cnt)
{
    ESP_LOGI(TAG, "Registering Bridge config commands.");

    s_add_args.addr = arg_str1(NULL, "addr", "<MAC address>", "MAC address to be added in expected xx:xx:xx:xx:xx:xx format");
    s_add_args.port = arg_intn("p", "port", "<port_num>", 0, br_port_cnt + 1, "Forward to Port Number");
    s_add_args.drop = arg_lit0("d", "drop", "Drop");
    s_add_args.flood = arg_lit0("f", "flood", "Flood to all ports");
    s_add_args.cpu = arg_lit0("c", "cpu", "Forward to CPU");
    s_add_args.end = arg_end(2);
    const esp_console_cmd_t br_add_fdb_cmd = {
        .command = "add",
        .help = "Add Forwarding Database entry",
        .hint = NULL,
        .func = &cmd_br_fdb_add,
        .argtable = &s_add_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&br_add_fdb_cmd));

    s_remove_args.addr = arg_str1(NULL, "addr", "<MAC address>", "MAC address to be removed in expected xx:xx:xx:xx:xx:xx format");
    s_remove_args.end = arg_end(1);
    const esp_console_cmd_t br_remove_fdb_cmd = {
        .command = "remove",
        .help = "Remove Forwarding Database entry",
        .hint = NULL,
        .func = &cmd_br_fdb_remove,
        .argtable = &s_remove_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&br_remove_fdb_cmd));

    s_br_netif = br_netif;
    s_br_port_cnt = br_port_cnt;
}
