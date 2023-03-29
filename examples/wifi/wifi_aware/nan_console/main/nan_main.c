/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* NAN Console Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "cmd_system.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_nan.h"
#include "esp_mac.h"
#include "esp_console.h"
#include "esp_private/wifi.h"
#include "esp_netif.h"
#include "esp_supplicant_utils.h"

#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

typedef struct {
    /* NAN Discovery parameters */
    struct arg_lit *init;
    struct arg_int *master_pref;
    struct arg_int *op_channel;
    struct arg_int *warmup_time;
    struct arg_lit *deinit;
    struct arg_end *end;
} wifi_nan_args_t;

static wifi_nan_args_t nan_args;

typedef struct {
    /* NAN Publish parameters */
    struct arg_str *name;
    struct arg_int *type;
    struct arg_str *filter;
    struct arg_lit *cancel;
    struct arg_int *id;
    struct arg_end *end;
} wifi_publish_args_t;

static wifi_publish_args_t pub_args;

typedef struct {
    /* NAN Subscribe parameters */
    struct arg_str *name;
    struct arg_int *type;
    struct arg_str *filter;
    struct arg_lit *cancel;
    struct arg_int *id;
    struct arg_end *end;
} wifi_subscribe_args_t;

static wifi_subscribe_args_t sub_args;

typedef struct {
    /* NAN Follow-up parameters */
    struct arg_int *own_id;
    struct arg_int *peer_id;
    struct arg_str *mac_addr;
    struct arg_str *text;
    struct arg_end *end;
} wifi_followup_args_t;

static wifi_followup_args_t fup_args;

typedef struct {
    /* NDP Init/Deinit parameters */
    struct arg_lit *init;
    struct arg_int *peer_pub_id;
    struct arg_str *mac_addr;
    /* NDP Terminate parameters */
    struct arg_lit *terminate;
    struct arg_int *ndp_id;
    struct arg_end *end;
} wifi_ndp_args_t;

static wifi_ndp_args_t ndp_args;

static struct {
    struct arg_str *host;
    struct arg_end *end;
} ping_args;

static const char *TAG = "nan_console";
static esp_netif_t *g_nan_netif;

#define NAN_EXAMPLE_SERVICE_NAME    "ESP_NAN-Service"

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    ESP_LOGI(TAG, "%lu bytes from %s icmp_seq=%u ttl=%u time=%lu ms",
             recv_len, ipaddr_ntoa((ip_addr_t *)&target_addr), seqno, ttl, elapsed_time);
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ESP_LOGI(TAG, "From %s icmp_seq=%d timeout", ipaddr_ntoa((ip_addr_t *)&target_addr), seqno);
}

static void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    ESP_LOGI(TAG, "\n--- %s ping statistics ---", inet6_ntoa(*ip_2_ip6(&target_addr)));
    ESP_LOGI(TAG, "%lu packets transmitted, %lu received, %lu%% packet loss, time %lums",
             transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    esp_ping_delete_session(hdl);
}

static int do_ping_cmd(int argc, char **argv)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.task_stack_size = 4096;
    ip_addr_t target_addr = {0};

    int nerrors = arg_parse(argc, argv, (void **)&ping_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ping_args.end, argv[0]);
        return 1;
    }

    if (!g_nan_netif) {
        ESP_LOGE(TAG, "NAN not started successfully");
        return 1;
    }

    if (ping_args.host->count) {
        /* convert ip6 string to ip6 address */
        ipaddr_aton(ping_args.host->sval[0], &target_addr);
    } else {
        ESP_LOGE(TAG, "No Active datapath for ping");
        return 1;
    }

    config.target_addr = target_addr;
    config.interface = esp_netif_get_netif_impl_index(g_nan_netif);

    /* set callback functions */
    esp_ping_callbacks_t cbs = {
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end,
        .cb_args = NULL
    };
    esp_ping_handle_t ping;
    if (esp_ping_new_session(&config, &cbs, &ping) == ESP_OK) {
        ESP_LOGI(TAG, "Pinging Peer with IPv6 addr %s", ipaddr_ntoa((ip_addr_t *)&target_addr));
        esp_ping_start(ping);
        return 0;
    } else {
        ESP_LOGI(TAG, "Failed to ping Peer with IPv6 addr %s", ipaddr_ntoa((ip_addr_t *)&target_addr));
        return 1;
    }
}

void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static int wifi_cmd_nan_disc(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &nan_args);
    esp_err_t ret;

    if (nerrors != 0) {
        arg_print_errors(stderr, nan_args.end, argv[0]);
        return 1;
    }

    if ((nan_args.init->count == 0) && (nan_args.deinit->count == 0)) {
        ESP_LOGE(TAG, "Invalid NAN Discovery command");
        return 1;
    }

    if (nan_args.init->count) {
        wifi_nan_config_t nan_cfg = WIFI_NAN_CONFIG_DEFAULT();

        if (nan_args.master_pref->count) {
            nan_cfg.master_pref = nan_args.master_pref->ival[0];
        }
        if (nan_args.op_channel->count) {
            nan_cfg.op_channel = nan_args.op_channel->ival[0];
        }
        if (nan_args.warmup_time->count) {
            nan_cfg.warm_up_sec = nan_args.warmup_time->ival[0];
        }

        g_nan_netif = esp_netif_create_default_wifi_nan();
        if ((esp_wifi_nan_start(&nan_cfg)) != ESP_OK) {
            ESP_LOGI(TAG, "Failed to start NAN");
            esp_netif_destroy_default_wifi(g_nan_netif);
            return 1;
        }
        return 0;
    }

    if (nan_args.deinit->count) {
        ret = esp_wifi_nan_stop();

        if (ret != ESP_OK) {
            ESP_LOGI(TAG, "Failed to stop NAN");
            return 1;
        }
        esp_netif_destroy_default_wifi(g_nan_netif);
    }

    return 0;
}

static int wifi_cmd_nan_publish(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &pub_args);
    uint32_t pub_id;
    bool ndp_resp_needed = false;

    if (nerrors != 0) {
        arg_print_errors(stderr, pub_args.end, argv[0]);
        return 1;
    }

    if (pub_args.cancel->count && pub_args.id->count) {
        pub_id = pub_args.id->ival[0];
        if (esp_wifi_nan_cancel_service(pub_id) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to cancel service");
            return 1;
        }
        return 0;
    }

    wifi_nan_publish_cfg_t publish = {
        .service_name = NAN_EXAMPLE_SERVICE_NAME,
        .type = NAN_PUBLISH_UNSOLICITED,
        .single_replied_event = 1,
    };

    if (pub_args.name->count) {
        strlcpy(publish.service_name, pub_args.name->sval[0], ESP_WIFI_MAX_SVC_NAME_LEN);
    }

    if (pub_args.type->count && pub_args.type->ival[0] == 1) {
        publish.type = NAN_PUBLISH_SOLICITED;
    }

    if (pub_args.filter->count) {
        strlcpy(publish.matching_filter, pub_args.filter->sval[0], ESP_WIFI_MAX_SVC_NAME_LEN);
    }

    if (!esp_wifi_nan_publish_service(&publish, ndp_resp_needed)) {
        return 1;
    }

    return 0;
}

static int wifi_cmd_nan_subscribe(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &sub_args);
    uint32_t sub_id;

    if (nerrors != 0) {
        arg_print_errors(stderr, sub_args.end, argv[0]);
        return 1;
    }

    if (sub_args.cancel->count && sub_args.id->count) {
        sub_id = sub_args.id->ival[0];
        if (esp_wifi_nan_cancel_service(sub_id) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to cancel service");
            return 1;
        }
        return 0;
    }

    wifi_nan_subscribe_cfg_t subscribe = {
        .service_name = NAN_EXAMPLE_SERVICE_NAME,
        .type = NAN_SUBSCRIBE_PASSIVE,
        .single_match_event = true,
    };

    if (sub_args.name->count) {
        strlcpy(subscribe.service_name, sub_args.name->sval[0], ESP_WIFI_MAX_SVC_NAME_LEN);
    }

    if (sub_args.type->count && sub_args.type->ival[0] == 1) {
        subscribe.type = NAN_SUBSCRIBE_ACTIVE;
    }

    if (sub_args.filter->count) {
        strlcpy(subscribe.matching_filter, sub_args.filter->sval[0], ESP_WIFI_MAX_SVC_NAME_LEN);
    }

    if (!esp_wifi_nan_subscribe_service(&subscribe)) {
        return 1;
    }

    return 0;
}

static int wifi_cmd_nan_followup(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &fup_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, fup_args.end, argv[0]);
        return 1;
    }

    wifi_nan_followup_params_t fup = {0};
    if (fup_args.own_id->count) {
        fup.inst_id = fup_args.own_id->ival[0];
    }

    if (!fup_args.peer_id->count && !fup_args.mac_addr->count) {
        ESP_LOGE(TAG, "Missing peer's service instance id or peer's MAC.");
        return 1;
    }
    if (fup_args.peer_id->count) {
        fup.peer_inst_id = fup_args.peer_id->ival[0];
    }
    if (fup_args.mac_addr->count &&
            esp_supplicant_str_to_mac((char *)fup_args.mac_addr->sval[0], fup.peer_mac) != ESP_OK) {
        return 1;
    }

    if (fup_args.text->count) {
        strlcpy(fup.svc_info, fup_args.text->sval[0], ESP_WIFI_MAX_SVC_INFO_LEN);
    }

    if (esp_wifi_nan_send_message(&fup) != ESP_OK) {
        return 1;
    }

    return 0;
}

static int wifi_cmd_ndp(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &ndp_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, ndp_args.end, argv[0]);
        return 1;
    }

    if ((ndp_args.init->count == 0) && (ndp_args.terminate->count == 0)) {
        ESP_LOGE(TAG, "Invalid NDP command");
        return 1;
    }

    if (ndp_args.init->count) {
        wifi_nan_datapath_req_t ndp_req = {0};
        ndp_req.confirm_required = true;

        if (!ndp_args.peer_pub_id->count && !ndp_args.mac_addr->count) {
            ESP_LOGE(TAG, "Missing Peer's publish id or peer's MAC");
            return 1;
        }
        if (ndp_args.peer_pub_id->count) {
            ndp_req.pub_id = ndp_args.peer_pub_id->ival[0];
        }
        if (ndp_args.mac_addr->count &&
                esp_supplicant_str_to_mac((char *)ndp_args.mac_addr->sval[0], ndp_req.peer_mac) != ESP_OK) {
            return 1;
        }

        if (!esp_wifi_nan_datapath_req(&ndp_req)) {
            ESP_LOGE(TAG, "Invalid configuration or NDP Request was rejected");
        }
        goto out;
    }

    if (ndp_args.terminate->count) {
        wifi_nan_datapath_end_req_t ndp_end = {0};
        if (ndp_args.ndp_id->count) {
            ndp_end.ndp_id = ndp_args.ndp_id->ival[0];
        } else {
            ESP_LOGE(TAG, "Missing own NDP id, add using '-d' parameter");
            return 1;
        }

        esp_wifi_nan_datapath_end(&ndp_end);
        goto out;
    }

out:
    return 0;
}


void register_nan(void)
{
    /* NAN Init/Deinit parameters */
    nan_args.init = arg_lit0("S", "start", "NAN Start");
    nan_args.deinit = arg_lit0("T", "stop", "NAN Stop");
    nan_args.master_pref = arg_int0("p", "mast_pref", "<1-254>", "NAN Master Preference");
    nan_args.op_channel = arg_int0("c", "op_chan", "<1-11>", "NAN Operating Channe");
    nan_args.warmup_time = arg_int0("w", "warmup", "<5-120>", "NAN Warmup Time in Sec");
    nan_args.end = arg_end(1);

    const esp_console_cmd_t nan_cmd = {
        .command = "nan",
        .help = "NAN Discovery command",
        .hint = NULL,
        .func = &wifi_cmd_nan_disc,
        .argtable = &nan_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&nan_cmd) );

    /* NAN Publish parameters */
    pub_args.name = arg_str0("n", "name", "<name>", "Name for the service");
    pub_args.type = arg_int0("t", "type", "<0/1>", "0 - Unsolicited(Default), 1 - Solicited");
    pub_args.filter = arg_str0("f", "filter", "<filter>", "Comma separated Matching Filter");
    /* NAN Publish cancel parameters */
    pub_args.cancel = arg_lit0("C", "cancel", "Cancel a service");
    pub_args.id = arg_int0("i", "id", "<0-255>", "Publish service id");
    pub_args.end = arg_end(1);

    const esp_console_cmd_t pub_cmd = {
        .command = "publish",
        .help = "NAN Publish Service command",
        .hint = NULL,
        .func = &wifi_cmd_nan_publish,
        .argtable = &pub_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&pub_cmd) );

    /* NAN Subscribe parameters */
    sub_args.name = arg_str0("n", "name", "<name>", "Name for the service");
    sub_args.type = arg_int0("t", "type", "<0/1>", "0 - Passive(Default), 1 - Active");
    sub_args.filter = arg_str0("f", "filter", "<filter>", "Comma separated Matching Filter");
    /* NAN Subscribe cancel parameters */
    sub_args.cancel = arg_lit0("C", "cancel", "Cancel a service");
    sub_args.id = arg_int0("i", "id", "<0-255>", "Subscribe service id");
    sub_args.end = arg_end(1);

    const esp_console_cmd_t sub_cmd = {
        .command = "subscribe",
        .help = "NAN Subscribe to Service command",
        .hint = NULL,
        .func = &wifi_cmd_nan_subscribe,
        .argtable = &sub_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&sub_cmd) );

    /* NAN Follow-up parameters */
    fup_args.own_id = arg_int0("i", "own_id", "<0-255>", "Own service id");
    fup_args.peer_id = arg_int0("p", "peer_id", "<0-255>", "Peer's service id");
    fup_args.mac_addr = arg_str0("m", "mac", "<mac>", "Peer's MAC Address");
    fup_args.text = arg_str1("t", "text", "<info>", "Text to be shared as Service Info");
    fup_args.end = arg_end(1);

    const esp_console_cmd_t fup_cmd = {
        .command = "send",
        .help = "NAN Follow-up command",
        .hint = NULL,
        .func = &wifi_cmd_nan_followup,
        .argtable = &fup_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&fup_cmd) );

    /* NDP Init/Deinit parameters */
    ndp_args.init = arg_lit0("I", "initiate", "NDP Initiate");
    ndp_args.peer_pub_id = arg_int0("p", "peer_pub_id", "<1-254>", "Peer's Publish Id");
    ndp_args.mac_addr = arg_str0("m", "mac", "<mac>", "Peer's MAC Address");
    /* NDP Terminate parameters */
    ndp_args.terminate = arg_lit0("T", "terminate", "NDP Terminate");
    ndp_args.ndp_id = arg_int0("d", "ndp_id", "<1-254>", "NDP ID");
    ndp_args.end = arg_end(1);

    const esp_console_cmd_t ndp_cmd = {
        .command = "ndp",
        .help = "NDP command",
        .hint = NULL,
        .func = &wifi_cmd_ndp,
        .argtable = &ndp_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&ndp_cmd) );

    ping_args.host = arg_str1(NULL, NULL, "<host>", "Host address");
    ping_args.end = arg_end(1);

    const esp_console_cmd_t ping_cmd = {
        .command = "ping",
        .help = "send ICMP ECHO_REQUEST to network hosts",
        .hint = NULL,
        .func = &do_ping_cmd,
        .argtable = &ping_args
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&ping_cmd));
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    initialise_wifi();

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    repl_config.prompt = "nan>";
    // init console REPL environment
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    /* Register commands */
    register_system();
    register_nan();

    printf("\n ==================================================================\n");
    printf(" |       Basic Steps to test NAN Discovery & Datapath               |\n");
    printf(" |                                                                  |\n");
    printf(" |  Below are short commands that use defaults, use 'help'          |\n");
    printf(" |  to see detailed command parameters available                    |\n");
    printf(" |                                                                  |\n");
    printf(" |  # NAN Discovery -                                               |\n");
    printf(" |  1. To start NAN issue 'nan -S', to stop it use 'nan -T'         |\n");
    printf(" |  2. Publish OR Subscribe a service with name 'TEST' -            |\n");
    printf(" |      > publish -n TEST                                           |\n");
    printf(" |      > subscribe -n TEST                                         |\n");
    printf(" |  4. After service match, send a text to Peer with service id 5 - |\n");
    printf(" |      > send -p 5 -t Hello                                        |\n");
    printf(" |  5. Cancel an ongoing service with id 5 -                        |\n");
    printf(" |      > publish -C -i 5                                           |\n");
    printf(" |                                                                  |\n");
    printf(" |  # NAN Datapath -                                                |\n");
    printf(" |  1. Subscriber can initiate datapath using 'ndp -I -p [pub_id]'  |\n");
    printf(" |  2. After NDP setup, use 'ping [Peer's IPv6]' to test datapath   |\n");
    printf(" |  3. Terminate the NDP using 'ndp -T'                             |\n");
    printf(" |                                                                  |\n");
    printf(" ====================================================================\n\n");

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
