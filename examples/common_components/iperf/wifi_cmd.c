/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "esp_mac.h"
#include "wifi_cmd.h"

#if CONFIG_SOC_WIFI_HE_SUPPORT

#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_wifi_he.h"
#include "esp_private/esp_wifi_he_private.h"


/*******************************************************
 *                Macros
 *******************************************************/
/*
 * enable/disable rx/tx statistics after wifi started:
 * (1) esp_wifi_enable_rx_statistics(true, true);
 * (2) esp_wifi_enable_tx_statistics(ESP_WIFI_ACI_BE, true);
 */
#define VAR2IPSTR(var)  (uint16_t) var & 0xFF, \
                        (uint16_t) (var >> 8) & 0xFF, \
                        (uint16_t) (var >> 16) & 0xFF, \
                        (uint16_t) (var >> 24) & 0xFF


/*******************************************************
 *                Constants
 *******************************************************/
static const char *TAG = "cmd";

/*******************************************************
 *                Structures
 *******************************************************/
typedef struct {
    struct arg_dbl *read;
    struct arg_dbl *write;
    struct arg_dbl *value;
    struct arg_end *end;
} reg_rw_args_t;

typedef struct {
    struct arg_dbl *disable;
    struct arg_end *end;
} wifi_stbc_args_t;

typedef struct {
    struct arg_dbl *disable;
    struct arg_end *end;
} wifi_bmfmee_args_t;

typedef struct {
    struct arg_dbl *ul_mu_disable;
    struct arg_dbl *ul_mu_data_disable;
    struct arg_dbl *ersu_disable;
    struct arg_dbl *report;
    struct arg_dbl *resounding;
    struct arg_end *end;
} wifi_omctrl_args_t;

typedef struct {
    struct arg_dbl *tf_padding;
    struct arg_end *end;
} wifi_tf_padding_args_t;

typedef struct {
    struct arg_int *enable;
    struct arg_int *txq;
    struct arg_end *end;
} wifi_edca_args_t;

typedef struct {
    struct arg_int *reset_timer;
    struct arg_int *read_timer;
    struct arg_end *end;
} wifi_muedca_args_t;

typedef struct {
    struct arg_dbl *enable;
    struct arg_end *end;
} wifi_cca_ignore_args_t;

typedef struct {
    struct arg_dbl *timeout;
    struct arg_dbl *interval;
    struct arg_int *data_size;
    struct arg_int *count;
    struct arg_int *tos;
    struct arg_str *host;
    struct arg_lit *abort;
    struct arg_end *end;
} wifi_ping_args_t;

typedef struct {
    struct arg_str *ip;
    struct arg_str *gw;
    struct arg_str *netmask;
    struct arg_end *end;
} static_ip_args_t;

typedef struct {
    struct arg_str *proto;
    struct arg_end *end;
} wifi_proto_args_t;

typedef struct {
    struct arg_int *val;
    struct arg_end *end;
} wifi_inactive_time_args_t;

typedef struct {
    struct arg_int *format;
    struct arg_int *rate;
    struct arg_end *end;
} wifi_sounding_rate_t;

typedef struct {
    struct arg_int *mcs;
    struct arg_int *power;
    struct arg_end *end;
} wifi_tx_pwr_t;

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static reg_rw_args_t reg_rw_args;
static wifi_stbc_args_t stbc_args;
static wifi_bmfmee_args_t bmfmee_args;
static wifi_omctrl_args_t omctrl_args;
static wifi_tf_padding_args_t tf_padding_args;
static wifi_edca_args_t edca_args;
static wifi_cca_ignore_args_t cca_args;
static wifi_ping_args_t ping_args;
static static_ip_args_t static_ip_args;
static wifi_proto_args_t proto_args;
static wifi_inactive_time_args_t inactive_time_args;
static wifi_sounding_rate_t wifi_sounding_rate_args;
static wifi_muedca_args_t muedca_args;
static wifi_tx_pwr_t tx_pwr_args;


extern esp_netif_t *netif_ap;
extern esp_netif_t *netif_sta;
extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
static int wifi_cmd_get_mac(int argc, char **argv)
{
    uint8_t mac[6] = { 0, };
    if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
        ESP_LOGW(TAG, "sta mac: " MACSTR "", MAC2STR(mac));
    }
    if (esp_wifi_get_mac(WIFI_IF_AP, mac) == ESP_OK) {
        ESP_LOGW(TAG, "ap  mac: " MACSTR "", MAC2STR(mac));
    }
    return 0;
}

static int wifi_cmd_set_omc(int argc, char **argv)
{
    //TODO ER-SU
    esp_wifi_htc_omc_t omc = { 0, };
    esp_err_t err = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **) &omctrl_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, omctrl_args.end, argv[0]);
        return 1;
    }
    do {
        if (!omctrl_args.ul_mu_disable->count && !omctrl_args.ul_mu_data_disable->count) {
            omc.ul_mu_disable = 1;
            omc.ul_mu_data_disable = 0;
            break;
        }
        /* parse inputs */
        if (omctrl_args.ul_mu_disable->count) {
            omc.ul_mu_disable = omctrl_args.ul_mu_disable->dval[0];
        }
        if (omctrl_args.ul_mu_data_disable->count) {
            omc.ul_mu_data_disable = omctrl_args.ul_mu_data_disable->dval[0];
        }
        if (omctrl_args.ersu_disable->count) {
            omc.er_su_disable = omctrl_args.ersu_disable->dval[0];
        }
        if (omctrl_args.resounding->count) {
            omc.dl_mu_mimo_resounding_recommendation = omctrl_args.resounding->dval[0];
        }
    } while (0);

    if (omctrl_args.report->count && omctrl_args.report->dval[0] == 0) {
        /* not report to ap the om control */
        hal_he_set_ul_mu(omc.ul_mu_disable, omc.ul_mu_data_disable);
        ESP_LOGW(TAG, "(omc)(internal)disable ul mu(%d, data:%d) successfully", omc.ul_mu_disable,
                 omc.ul_mu_data_disable);
    } else {
        err = esp_wifi_set_htc_omc(&omc);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "(omc)disable ul mu(%d, data:%d) failed, err:0x%x", omc.ul_mu_disable, omc.ul_mu_data_disable,
                     err);
        } else {
            ESP_LOGW(TAG, "(omc)disable ul mu(%d, data:%d) successfully", omc.ul_mu_disable, omc.ul_mu_data_disable);
        }
    }
    return 0;
}

static int wifi_cmd_edca_tx(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &edca_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, edca_args.end, argv[0]);
        return 1;
    }

    int txq = 2;
    if (!edca_args.enable->count && !edca_args.txq->count) {
        esp_test_disable_edca_tx(txq);
        ESP_LOGW(TAG, "(tx)disable edca, txq[%d]", txq);
        return 0;
    }
    txq = edca_args.txq->count ? edca_args.txq->ival[0] : txq;
    if (edca_args.enable->ival[0] == 0) {
        esp_test_disable_edca_tx(txq);
        ESP_LOGW(TAG, "(tx)disable edca, txq[%d]", txq);
    } else {
        esp_test_enable_edca_tx(txq);
        ESP_LOGW(TAG, "(tx)enable edca, txq[%d]", txq);
    }
    return 0;
}

static int wifi_cmd_reg_rw(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &reg_rw_args);
    uint32_t addr;
    if (nerrors != 0) {
        arg_print_errors(stderr, reg_rw_args.end, argv[0]);
        return 1;
    }
    if (reg_rw_args.read->count) {
        addr = (uint32_t) reg_rw_args.read->dval[0];
        ESP_LOGW(TAG, "reg read 0x%08lx : 0x%08lx\n", addr, REG_READ(addr));
    } else if (reg_rw_args.write->count && (uint32_t) reg_rw_args.value->count) {
        addr = (uint32_t) reg_rw_args.write->dval[0];
        ESP_LOGW(TAG, "reg write 0x%8lx : 0x%8lx\n", addr, (uint32_t) reg_rw_args.value->dval[0]);
        REG_WRITE(addr, (uint32_t ) reg_rw_args.value->dval[0]);
        ESP_LOGW(TAG, "reg read 0x%08lx : 0x%08lx\n", addr, REG_READ(addr));
    } else {
        printf("Input Error\n");
    }
    return 0;
}

static int wifi_cmd_set_tf_padding(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &tf_padding_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tf_padding_args.end, argv[0]);
        return 1;
    }
    if (tf_padding_args.tf_padding->count) {
        esp_wifi_set_tf_padding_duration((int)tf_padding_args.tf_padding->dval[0]);
        ESP_LOGW(TAG, "(test)set trigger frame mac padding duration:%d", (int)tf_padding_args.tf_padding->dval[0]);
    } else {
        printf("Input Error\n");
    }
    return 0;
}

static int wifi_cmd_tb(int argc, char **argv)
{
    dbg_read_axtb_diag();
    dbg_read_ax_diag(1);
    return 0;
}

static int wifi_cmd_stbc(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &stbc_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, stbc_args.end, argv[0]);
        return 1;
    }
    if (stbc_args.disable->count) {
        esp_wifi_enable_rx_stbc(0);
        ESP_LOGI(TAG, "(cfg)disable he stbc");
    } else {
        esp_wifi_enable_rx_stbc(1);
        ESP_LOGI(TAG, "(cfg)enable he stbc");
    }
    return 0;
}

static int wifi_cmd_su_bmfmee(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &bmfmee_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, bmfmee_args.end, argv[0]);
        return 1;
    }
    if (bmfmee_args.disable->count) {
        esp_wifi_enable_su_bmfmee(0);
        ESP_LOGI(TAG, "(cfg)disable he su bmfmee");
    } else {
        esp_wifi_enable_su_bmfmee(1);
        ESP_LOGI(TAG, "(cfg)enable he su bmfmee");
    }
    return 0;
}

static int wifi_cmd_ignore_cca(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &cca_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, cca_args.end, argv[0]);
        return 1;
    }
    if (cca_args.enable->count) {
        dbg_tb_ignore_cca_enable(1);
    } else {
        dbg_tb_ignore_cca_enable(0);
    }
    return 0;
}

static int wifi_cmd_set_ps_type(int argc, char **argv)
{
    ESP_LOGW(TAG, "set to WIFI_PS_MIN_MODEM");
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MIN_MODEM));
    return 0;
}

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

#ifdef CONFIG_LWIP_IPV6
    printf("%" PRIu32 " bytes from %s icmp_seq=%d ttl=%d time=%" PRIu32 " ms\n",
           recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
#else
    printf("%" PRIu32 " bytes from %s icmp_seq=%d ttl=%d time=%" PRIu32 " ms\n",
           recv_len, inet_ntoa(target_addr.addr), seqno, ttl, elapsed_time);
#endif

}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
#ifdef CONFIG_LWIP_IPV6
    printf("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);
#else
    printf("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.addr), seqno);
#endif
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
    if (IP_IS_V4(&target_addr)) {
        printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    }
#ifdef CONFIG_LWIP_IPV6
    else {
        printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
#endif
    printf("%" PRIu32 " packets transmitted, %" PRIu32 " received, %" PRIu32 "%% packet loss, time %" PRIu32 "ms\n",
           transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    esp_ping_delete_session(hdl);
}

static int do_ping_cmd(int argc, char **argv)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    static esp_ping_handle_t ping;
    int nerrors = arg_parse(argc, argv, (void **)&ping_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ping_args.end, argv[0]);
        return 1;
    }

    if (ping_args.timeout->count > 0) {
        config.timeout_ms = (uint32_t)(ping_args.timeout->dval[0] * 1000);
    }

    if (ping_args.interval->count > 0) {
        config.interval_ms = (uint32_t)(ping_args.interval->dval[0] * 1000);
    }

    if (ping_args.data_size->count > 0) {
        config.data_size = (uint32_t)(ping_args.data_size->ival[0]);
    }

    if (ping_args.count->count > 0) {
        config.count = (uint32_t)(ping_args.count->ival[0]);
    }

    if (ping_args.tos->count > 0) {
        config.tos = (uint32_t)(ping_args.tos->ival[0]);
    }

    if (ping_args.abort->count) {
        esp_ping_stop(ping);
        return 0;
    }

    // parse IP address
    ip_addr_t target_addr;
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));
    /* convert domain name to IP address */
    if (getaddrinfo(ping_args.host->sval[0], NULL, &hint, &res) != 0) {
        printf("ping: unknown host %s\n", ping_args.host->sval[0]);
        return 1;
    }
    if (res->ai_family == AF_INET) {
        struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    }
#ifdef CONFIG_LWIP_IPV6
    else {
        struct in6_addr addr6 = ((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr;
        inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
    }
#endif
    freeaddrinfo(res);
    config.target_addr = target_addr;

    /* set callback functions */
    esp_ping_callbacks_t cbs = {
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end,
        .cb_args = NULL
    };

    esp_ping_new_session(&config, &cbs, &ping);
    esp_ping_start(ping);
    return 0;
}

extern bool pm_is_waked(void);
extern bool pm_is_sleeping(void);
extern bool pm_is_dream(void);
static int wifi_cmd_get_ps_state(int argc, char **argv)
{
    ESP_LOGW(TAG, "ps: awake:%d, sleep:%d, dream:%d", pm_is_waked(), pm_is_sleeping(), pm_is_dream());
    return 0;
}

esp_err_t esp_netif_set_static_ip(esp_netif_t *netif_sta, uint32_t ip, uint32_t gw,
                                  uint32_t netmask)
{
    esp_netif_dhcpc_stop(netif_sta);
    esp_netif_ip_info_t ip_info;
    esp_netif_set_ip4_addr(&ip_info.ip, ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF,
                           (ip >> 24) & 0xFF);
    esp_netif_set_ip4_addr(&ip_info.gw, gw & 0xFF, (gw >> 8) & 0xFF, (gw >> 16) & 0xFF,
                           (gw >> 24) & 0xFF);
    esp_netif_set_ip4_addr(&ip_info.netmask, netmask & 0xFF, (netmask >> 8) & 0xFF,
                           (netmask >> 16) & 0xFF, (netmask >> 24) & 0xFF);
    esp_netif_set_ip_info(netif_sta, &ip_info);
    return ESP_OK;
}

static int wifi_cmd_set_ip(int argc, char **argv)
{
    uint32_t ip = 0, gw = 0, netmask = 0;
    int nerrors = arg_parse(argc, argv, (void **) &static_ip_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, static_ip_args.end, argv[0]);
        return 0;
    }
    if (static_ip_args.ip->count != 0) {
        ip = esp_ip4addr_aton(static_ip_args.ip->sval[0]);
    }
    if (static_ip_args.gw->count != 0) {
        gw = esp_ip4addr_aton(static_ip_args.gw->sval[0]);
    }
    if (static_ip_args.netmask->count != 0) {
        netmask = esp_ip4addr_aton(static_ip_args.netmask->sval[0]);
    }
    if (!ip || !netmask) {
        return 0;
    }
    /* set static IP settings */
    esp_netif_set_static_ip(netif_sta, ip, gw, netmask);

    ESP_LOGD(TAG, "ip:%d.%d.%d.%d, gateway:%d.%d.%d.%d, netmask:%d.%d.%d.%d,",
             VAR2IPSTR(ip), VAR2IPSTR(gw), VAR2IPSTR(netmask));
    return 0;
}

void wifi_get_local_ip(esp_netif_ip_info_t *ip_info)
{
    int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 0, 1, 0);
    esp_netif_t *netif = netif_ap;
    wifi_mode_t mode;

    esp_wifi_get_mode(&mode);
    if (WIFI_MODE_STA == mode) {
        bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 0, 1, 0);
        if (bits & CONNECTED_BIT) {
            netif = netif_sta;
        } else {
            ESP_LOGE(TAG, "sta has no IP");
        }
    }
    esp_netif_get_ip_info(netif, ip_info);
}

static int wifi_cmd_query(int argc, char **argv)
{
    wifi_config_t cfg;
    wifi_bandwidth_t cbw;
    uint8_t mac[6];
    esp_netif_ip_info_t ip_info = { 0, };

    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    wifi_get_local_ip(&ip_info);
    bool is_sta_disconnect = false;
    char temp_ssid[33] = { 0 };
    printf("Wireless info:");
    if (WIFI_MODE_AP == mode) {
        esp_wifi_get_config(WIFI_IF_AP, &cfg);
        esp_wifi_get_bandwidth(WIFI_IF_AP, &cbw);

        printf("\n");
        printf("\tmode: ap\n");
        strncpy(temp_ssid, (char *) cfg.ap.ssid, 32);
        printf("\tssid: %s\n", temp_ssid);
        printf("\tpassword: %s\n", cfg.ap.password);
        printf("\tchannel: %d\n", cfg.ap.channel);
        if (cbw == WIFI_BW_HT20) {
            printf("\tcbw: 20 MHz\n");
        } else if (cbw == WIFI_BW_HT40) {
            printf("\tcbw: 40 MHz\n");
        }

        if (esp_wifi_get_mac(WIFI_IF_AP, mac) == ESP_OK) {
            printf("\tap mac: "MACSTR, MAC2STR(mac));
            printf("\n");
        }
        printf("\tip: %d.%d.%d.%d\n", VAR2IPSTR(ip_info.ip.addr));
        printf("\tnetmask: %d.%d.%d.%d\n", VAR2IPSTR(ip_info.netmask.addr));
        printf("\tgateway: %d.%d.%d.%d\n", VAR2IPSTR(ip_info.gw.addr));
        printf("\n");
    } else if (WIFI_MODE_STA == mode) {
        int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 0, 1, 0);
        if (bits & CONNECTED_BIT) {
            is_sta_disconnect = false;
            esp_wifi_get_config(WIFI_IF_STA, &cfg);
            esp_wifi_get_bandwidth(WIFI_IF_STA, &cbw);
            printf("\n");
            printf("\tmode: station\n");
            printf("\tstatus: connected\n");
            strncpy(temp_ssid, (char *) cfg.sta.ssid, 32);
            printf("\tssid: %s\n", temp_ssid);
            printf("\tbssid: "MACSTR, MAC2STR(cfg.sta.bssid));
            printf("\n");
            printf("\tchannel: %d\n", cfg.sta.channel);
            uint16_t aid;
            esp_wifi_sta_get_aid(&aid);
            printf("\taid: %d\n", aid);
            if (cfg.sta.pmf_cfg.capable) {
                if (cfg.sta.pmf_cfg.required) {
                    printf("\tpmf: required\n");
                } else {
                    printf("\tpmf: optional\n");
                }
            } else {
                printf("\tpmf: disabled\n");
            }
            if (cbw == WIFI_BW_HT20) {
                printf("\tcbw: 20 MHz\n");
            } else if (cbw == WIFI_BW_HT40) {
                printf("\tcbw: 40 MHz\n");
            }

            if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
                printf("\tsta mac: "MACSTR, MAC2STR(mac));
                printf("\n");
            }
            printf("\tip: %d.%d.%d.%d\n", VAR2IPSTR(ip_info.ip.addr));
            printf("\tnetmask: %d.%d.%d.%d\n", VAR2IPSTR(ip_info.netmask.addr));
            printf("\tgateway: %d.%d.%d.%d\n", VAR2IPSTR(ip_info.gw.addr));
            printf("\n");

        } else {
            printf("\n");
            printf("\tmode: disconnected\n");
            is_sta_disconnect = true;
        }
    }
    if (WIFI_MODE_NULL == mode || is_sta_disconnect) {
        printf("\n");
        if (WIFI_MODE_NULL == mode) {
            printf("\tmode: null\n");
        }
        if (esp_wifi_get_mac(WIFI_IF_AP, mac) == ESP_OK) {
            printf("\tap mac: "MACSTR, MAC2STR(mac));
            printf("\n");
        }

        if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
            printf("\tsta mac: "MACSTR, MAC2STR(mac));
            printf("\n");
        }
        return 0;
    }

    return 0;
}

static int wifi_cmd_proto(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&proto_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, proto_args.end, argv[0]);
        return 1;
    }
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if(WIFI_MODE_NULL == mode) {
        ESP_LOGI(TAG, "current wifi mode is null");
        return 1;
    }
    int ifx = (WIFI_MODE_STA == mode) ? 0 : 1;
    if (proto_args.proto->count) {
        if (!strcmp(proto_args.proto->sval[0], "ax")) {
            ESP_ERROR_CHECK(esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX));
            printf("(%s)set to 11ax\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
        } else if (!strcmp(proto_args.proto->sval[0], "bgn")) {
            ESP_ERROR_CHECK(esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N));
            printf("(%s)set to bgn\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
        } else if (!strcmp(proto_args.proto->sval[0], "bg")) {
            ESP_ERROR_CHECK(esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G));
            printf("(%s)set to bg\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
        } else {
            ESP_ERROR_CHECK(esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B));
            printf("(%s)set to b\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
        }
    }
    uint8_t protocol_bitmap = 0;
    ESP_ERROR_CHECK(esp_wifi_get_protocol(ifx, &protocol_bitmap) );
    if (protocol_bitmap & WIFI_PROTOCOL_11AX) {
        printf("(%s)11ax\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
    } else if (protocol_bitmap & WIFI_PROTOCOL_11N) {
        printf("(%s)bgn\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
    } else if (protocol_bitmap & WIFI_PROTOCOL_11G) {
        printf("(%s)bg\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
    } else if (protocol_bitmap & WIFI_PROTOCOL_11B) {
        printf("(%s)b\n", (ifx == WIFI_IF_STA) ? "sta" : "ap");
    }
    return 0;
}

static int wifi_disconnect(int argc, char **argv)
{
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if (WIFI_MODE_AP == mode) {
        // TODO
        ESP_LOGI(TAG, "WIFI_MODE_AP, not support");
    } else if (WIFI_MODE_STA == mode) {
        esp_wifi_disconnect();
        printf("disconnect\n");
    } else {
        ESP_LOGI(TAG, "NULL mode");
    }
    return 0;
}

static int wifi_cmd_inactive_time(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&inactive_time_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, inactive_time_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = ESP_OK;
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if ((mode & WIFI_MODE_AP) && inactive_time_args.val->count) {
        err = esp_wifi_set_inactive_time(ESP_IF_WIFI_AP, inactive_time_args.val->ival[0]);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "set softAP inactive time to %d seconds, err:0x%x\n", inactive_time_args.val->ival[0], err);
        } else {
            ESP_LOGI(TAG, "set softAP inactive time to %d seconds\n", inactive_time_args.val->ival[0]);
        }
    }
    //WIFI_MODE_STA or WIFI_MODE_APSTA
    if ((mode & WIFI_MODE_STA) && inactive_time_args.val->count) {
        err = esp_wifi_set_inactive_time(ESP_IF_WIFI_STA, inactive_time_args.val->ival[0]);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "set STA inactive time to %d seconds, err:0x%x\n", inactive_time_args.val->ival[0], err);
        } else {
            ESP_LOGI(TAG, "set STA inactive time to %d seconds\n", inactive_time_args.val->ival[0]);
        }
    }
    uint16_t secs = 0;
    esp_wifi_get_inactive_time(ESP_IF_WIFI_STA, &secs);
    printf("inactive time: %d seconds\n", secs);
    return 0;
}

static int wifi_cmd_sounding_rate(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&wifi_sounding_rate_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, wifi_sounding_rate_args.end, argv[0]);
        return 1;
    }

    if (wifi_sounding_rate_args.format->count && wifi_sounding_rate_args.rate->count) {
        if (wifi_sounding_rate_args.format->ival[0] == SIG_MODE_LEGACY) {
            if (wifi_sounding_rate_args.rate->ival[0] < WIFI_PHY_RATE_MCS0_LGI &&
                    wifi_sounding_rate_args.rate->ival[0] >= 0) {
                hal_he_set_bf_report_rate(SIG_MODE_LEGACY, wifi_sounding_rate_args.rate->ival[0]);
            } else {
                ESP_LOGW(TAG, "need correct legacy rate(0-%d)", WIFI_PHY_RATE_9M);
            }
        } else {
            if (wifi_sounding_rate_args.rate->ival[0] >= WIFI_PHY_RATE_MCS0_LGI &&
                    wifi_sounding_rate_args.rate->ival[0] <= WIFI_PHY_RATE_MCS9_SGI) {
                hal_he_set_bf_report_rate(wifi_sounding_rate_args.format->ival[0],
                                          wifi_sounding_rate_args.rate->ival[0]);
            } else {
                ESP_LOGW(TAG, "need correct mcs(%d-%d)", WIFI_PHY_RATE_MCS0_LGI, WIFI_PHY_RATE_MCS9_SGI);
            }
        }
    } else {
        ESP_LOGW(TAG, "set rate fail");
    }
    return 0;
}

static int wifi_cmd_muedca(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&muedca_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, muedca_args.end, argv[0]);
        return 1;
    }

    if (muedca_args.reset_timer->count) {
        esp_wifi_sta_reset_muedca_timer(muedca_args.reset_timer->ival[0]);
    }

    uint8_t aci_bitmap = 0;
    if (muedca_args.read_timer->count) {
        aci_bitmap = muedca_args.read_timer->ival[0];
        if (aci_bitmap & BIT(0)) {
            dbg_read_muedca_timer(3);
        }
        if (aci_bitmap & BIT(1)) {
            dbg_read_muedca_timer(2);
        }
        if (aci_bitmap & BIT(2)) {
            dbg_read_muedca_timer(1);
        }
        if (aci_bitmap & BIT(3)) {
            dbg_read_muedca_timer(0);
        }
    }
    return 0;
}

static int wifi_cmd_set_tx_pwr(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&tx_pwr_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tx_pwr_args.end, argv[0]);
        return 1;
    }
    if (tx_pwr_args.mcs->count && tx_pwr_args.power->count) {
        if (tx_pwr_args.mcs->ival[0] <= 9 && tx_pwr_args.mcs->ival[0] >= 0) {
            if (tx_pwr_args.power->ival[0] >= -13 &&
                    tx_pwr_args.power->ival[0] <= 20) {
                esp_test_set_tx_mcs_pwr(tx_pwr_args.mcs->ival[0] + WIFI_PHY_RATE_MCS0_LGI, tx_pwr_args.power->ival[0]);
                ESP_LOGW(TAG, "set MCS%d TX PWR to %d", tx_pwr_args.mcs->ival[0], tx_pwr_args.power->ival[0]);
            } else if (tx_pwr_args.power->ival[0] == 0xff) {
                esp_test_set_tx_mcs_pwr(tx_pwr_args.mcs->ival[0] + WIFI_PHY_RATE_MCS0_LGI, tx_pwr_args.power->ival[0]);
                ESP_LOGW(TAG, "set MCS%d TX PWR to default value", tx_pwr_args.mcs->ival[0]);
            }
        } else {
            ESP_LOGW(TAG, "Set TX power fail, MCS should in range [0,9], power should in range [-13, 30] or set 0xFF for default");
        }
    }
    return 0;
}

static int wifi_read_avgsnr(int argc, char **argv)
{
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if (WIFI_MODE_AP == mode) {
        // TODO
        ESP_LOGI(TAG, "WIFI_MODE_AP, not support");
    } else if (WIFI_MODE_STA == mode || WIFI_MODE_APSTA == mode) {
        printf("%.2f\n", esp_test_get_bfr_avgsnr());
    } else {
        ESP_LOGI(TAG, "NULL mode");
    }
    return 0;
}

void register_wifi_cmd(void)
{
    /* mac */
    const esp_console_cmd_t maccmd = {
        .command = "mac",
        .help = "get mac",
        .hint = NULL,
        .func = &wifi_cmd_get_mac,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&maccmd));
    /* disable edca */
    edca_args.enable = arg_int0("e", "enable", "[enable]", "enable edca tx");
    edca_args.txq = arg_int0("q", "txq", "[txq]", "enable edca txq");
    edca_args.end = arg_end(1);
    const esp_console_cmd_t edca_cmd = {
        .command = "edca",
        .help = "enable/disable edca",
        .hint = NULL,
        .func = &wifi_cmd_edca_tx,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&edca_cmd));
    /* read/write hw registers */
    reg_rw_args.read = arg_dbl0("r", NULL, "<read_addr>", "read register address");
    reg_rw_args.write = arg_dbl0("w", NULL, "<write_addr>", "write register address");
    reg_rw_args.value = arg_dbl0("v", NULL, "<value>", "write value");
    reg_rw_args.end = arg_end(2);
    const esp_console_cmd_t reg_rw_cmd = {
        .command = "reg",
        .help = "r/w hw register",
        .hint = NULL,
        .func = &wifi_cmd_reg_rw,
        .argtable = &reg_rw_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reg_rw_cmd));
    /* om control */
    omctrl_args.ul_mu_disable = arg_dbl0("u", "ulmu", "[ulmu]", "disable ul mu");
    omctrl_args.ul_mu_data_disable = arg_dbl0("d", "uldata", "[uldata]", "disable ul mu data");
    omctrl_args.ersu_disable = arg_dbl0("e", "ersu", "[ersu]", "disable ersu");
    omctrl_args.report = arg_dbl0("r", "report", "[report]", "report om control to ap");
    omctrl_args.resounding = arg_dbl0("s", "resounding", "[resounding]", "DL MU-MIMO resound Recoummendation");
    omctrl_args.end = arg_end(1);
    const esp_console_cmd_t omctrl_cmd = {
        .command = "omc",
        .help = "om control",
        .hint = NULL,
        .func = &wifi_cmd_set_omc,
        .argtable = &omctrl_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&omctrl_cmd));
    /* stbc */
    stbc_args.disable = arg_dbl0("d", "disable", "[disable]", "disable stbc");
    stbc_args.end = arg_end(1);
    const esp_console_cmd_t stbc_cmd = {
        .command = "stbc",
        .help = "configure stbc",
        .hint = NULL,
        .func = &wifi_cmd_stbc,
        .argtable = &stbc_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&stbc_cmd));
    /* su bmfmee */
    bmfmee_args.disable = arg_dbl0("d", "disable", "[disable]", "disable bmfmee");
    bmfmee_args.end = arg_end(1);
    const esp_console_cmd_t bmfmee_cmd = {
        .command = "bmfmee",
        .help = "configure su bmfmee",
        .hint = NULL,
        .func = &wifi_cmd_su_bmfmee,
        .argtable = &bmfmee_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&bmfmee_cmd));
    /* set trigger frame mac padding duration */
    tf_padding_args.tf_padding = arg_dbl0("p", "padding", "[padding]", "set trigger frame mac padding duration");
    tf_padding_args.end = arg_end(1);
    const esp_console_cmd_t tf_padding_cmd = {
        .command = "tf",
        .help = "set padding",
        .hint = NULL,
        .func = &wifi_cmd_set_tf_padding,
        .argtable = &tf_padding_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&tf_padding_cmd));
    /* ignore cca */
    cca_args.enable = arg_dbl0("e", "enable", "[enable]", "enable ignore cca");
    cca_args.end = arg_end(1);
    const esp_console_cmd_t cca_cmd = {
        .command = "cca",
        .help = "ignore cca",
        .hint = NULL,
        .func = &wifi_cmd_ignore_cca,
        .argtable = &cca_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cca_cmd));
    /* dump tx tb ppdu */
    const esp_console_cmd_t tb_cmd = {
        .command = "tb",
        .help = "dump tx tb ppdu",
        .hint = NULL,
        .func = &wifi_cmd_tb,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&tb_cmd));
    /* set ps type */
    const esp_console_cmd_t ps_cmd = {
        .command = "ps",
        .help = "set ps type",
        .hint = NULL,
        .func = &wifi_cmd_set_ps_type,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&ps_cmd));
    /* ping test */
    ping_args.timeout = arg_dbl0("W", "timeout", "<t>", "Time to wait for a response, in seconds");
    ping_args.interval = arg_dbl0("i", "interval", "<t>", "Wait interval seconds between sending each packet");
    ping_args.data_size = arg_int0("s", "size", "<n>", "Specify the number of data bytes to be sent");
    ping_args.count = arg_int0("c", "count", "<n>", "Stop after sending count packets");
    ping_args.tos = arg_int0("Q", "tos", "<n>", "Set Type of Service related bits in IP datagrams");
    ping_args.host = arg_str0(NULL, NULL, "[host]", "Host address");
    ping_args.abort = arg_lit0("a", "abort", "abort");
    ping_args.end = arg_end(1);
    const esp_console_cmd_t ping_cmd = {
        .command = "ping",
        .help = "send ICMP ECHO_REQUEST to network hosts",
        .hint = NULL,
        .func = &do_ping_cmd,
        .argtable = &ping_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&ping_cmd));
    /* get ps state */
    const esp_console_cmd_t pss_cmd = {
        .command = "pss",
        .help = "get ps state",
        .hint = NULL,
        .func = &wifi_cmd_get_ps_state,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&pss_cmd));
    /* ip */
    static_ip_args.ip = arg_str0("i", "ip", "<ip>", "ip address");
    static_ip_args.gw = arg_str0("g", "gateway", "<gw>", "gateway address");
    static_ip_args.netmask = arg_str0("n", "netmask", "<netmask>", "netmask addess");
    static_ip_args.end = arg_end(1);
    const esp_console_cmd_t static_ip_cmd = {
        .command = "ip",
        .help = "ip settings",
        .hint = NULL,
        .func = &wifi_cmd_set_ip,
        .argtable = &static_ip_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&static_ip_cmd));
    /* query */
    const esp_console_cmd_t query_cmd = {
        .command = "query",
        .help = "query WiFi info",
        .hint = NULL,
        .func = &wifi_cmd_query,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&query_cmd));
    /* proto */
    proto_args.proto = arg_str0(NULL, NULL, "<proto>", "proto [ax,bgn,bg,b]");
    proto_args.end = arg_end(1);
    const esp_console_cmd_t proto_cmd = {
        .command = "proto",
        .help = "set wifi protocol",
        .hint = NULL,
        .func = &wifi_cmd_proto,
        .argtable = &proto_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&proto_cmd));
    /* disconnect */
    const esp_console_cmd_t disconnect_cmd = {
        .command = "disconnect",
        .help = "disconnect",
        .hint = NULL,
        .func = &wifi_disconnect,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&disconnect_cmd));
    /* inactive time */
    inactive_time_args.val = arg_int0("t", "time", "time", "set inactive time, in seconds");
    inactive_time_args.end = arg_end(1);
    const esp_console_cmd_t inactive_cmd = {
        .command = "inactive",
        .help = "inactive time, unit: seconds",
        .hint = NULL,
        .func = &wifi_cmd_inactive_time,
        .argtable = &inactive_time_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&inactive_cmd));
    /* set beamforming report rate */
    wifi_sounding_rate_args.format = arg_int0("f", "format", "format", "set format");
    wifi_sounding_rate_args.rate = arg_int0("r", "rate", "rate", "set rate");
    wifi_sounding_rate_args.end = arg_end(1);
    const esp_console_cmd_t sounding_rate_cmd = {
        .command = "sounding",
        .help = "set beamforming report rate",
        .hint = NULL,
        .func = &wifi_cmd_sounding_rate,
        .argtable = &wifi_sounding_rate_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&sounding_rate_cmd));
    /* muedca */
    muedca_args.reset_timer = arg_int0("r", NULL, "reset timer", "reset muedca timer");
    muedca_args.read_timer = arg_int0("d", NULL, "read timer", "read muedca timer");
    muedca_args.end = arg_end(1);
    const esp_console_cmd_t reg_muedca_cmd = {
        .command = "muedca",
        .help = "Reset/Read muedca timer",
        .hint = NULL,
        .func = &wifi_cmd_muedca,
        .argtable = &muedca_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reg_muedca_cmd));
    /* tx_pwr */
    tx_pwr_args.mcs = arg_int0("m", NULL, "[0, 9]", "force tx power on MCSX");
    tx_pwr_args.power = arg_int0("p", NULL, "[-13, 20]", "set max power, set 0xFF for default");
    tx_pwr_args.end = arg_end(1);
    const esp_console_cmd_t reg_tx_pwr_cmd = {
        .command = "txpwr",
        .help = "force tx power on MCSX",
        .hint = NULL,
        .func = &wifi_cmd_set_tx_pwr,
        .argtable = &tx_pwr_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reg_tx_pwr_cmd));
    /* avgSNR */
    const esp_console_cmd_t avgsnr_cmd = {
        .command = "avgsnr",
        .help = "show avgSnr in beamforming memory",
        .hint = NULL,
        .func = &wifi_read_avgsnr,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&avgsnr_cmd));
}
#else

void register_wifi_cmd(void)
{
    ;
}

#endif /* CONFIG_SOC_WIFI_HE_SUPPORT */
