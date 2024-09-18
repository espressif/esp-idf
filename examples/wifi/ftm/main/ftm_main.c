/* Wi-Fi FTM Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "nvs_flash.h"
#include "cmd_system.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_console.h"
#include "esp_mac.h"

typedef struct {
    struct arg_str *ssid;
    struct arg_str *password;
    struct arg_lit *disconnect;
    struct arg_end *end;
} wifi_sta_args_t;

typedef struct {
    struct arg_str *ssid;
    struct arg_str *password;
    struct arg_int *channel;
    struct arg_int *bandwidth;
    struct arg_end *end;
} wifi_ap_args_t;

typedef struct {
    struct arg_str *ssid;
    struct arg_end *end;
} wifi_scan_arg_t;

typedef struct {
    /* FTM Initiator */
    struct arg_lit *initiator;
    struct arg_int *frm_count;
    struct arg_int *burst_period;
    struct arg_str *ssid;
    /* FTM Responder */
    struct arg_lit *responder;
    struct arg_lit *enable;
    struct arg_lit *disable;
    struct arg_int *offset;
    struct arg_end *end;
} wifi_ftm_args_t;

static wifi_sta_args_t sta_args;
static wifi_ap_args_t ap_args;
static wifi_scan_arg_t scan_args;
static wifi_ftm_args_t ftm_args;

wifi_config_t g_ap_config = {
    .ap.max_connection = 4,
    .ap.authmode = WIFI_AUTH_WPA2_PSK,
    .ap.ftm_responder = true
};

#define ETH_ALEN 6
#define MAX_CONNECT_RETRY_ATTEMPTS  5
#define DEFAULT_WAIT_TIME_MS        (10 * 1000)
#define MAX_FTM_BURSTS          8
#define DEFAULT_AP_CHANNEL      1
#define DEFAULT_AP_BANDWIDTH    20

static bool s_reconnect = true;
static int s_retry_num = 0;
static const char *TAG_STA = "ftm_station";
static const char *TAG_AP = "ftm_ap";

static EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int DISCONNECTED_BIT = BIT1;

static EventGroupHandle_t s_ftm_event_group;
static const int FTM_REPORT_BIT = BIT0;
static const int FTM_FAILURE_BIT = BIT1;
static uint8_t s_ftm_report_num_entries;
static uint32_t s_rtt_est, s_dist_est;
static bool s_ap_started;
static uint8_t s_ap_channel;
static uint8_t s_ap_bssid[ETH_ALEN];

const int g_report_lvl =
#ifdef CONFIG_ESP_FTM_REPORT_SHOW_DIAG
    BIT0 |
#endif
#ifdef CONFIG_ESP_FTM_REPORT_SHOW_RTT
    BIT1 |
#endif
#ifdef CONFIG_ESP_FTM_REPORT_SHOW_T1T2T3T4
    BIT2 |
#endif
#ifdef CONFIG_ESP_FTM_REPORT_SHOW_RSSI
    BIT3 |
#endif
0;

uint16_t g_scan_ap_num;
wifi_ap_record_t *g_ap_list_buffer;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
	if (event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;

        ESP_LOGI(TAG_STA, "Connected to %s (BSSID: "MACSTR", Channel: %d)", event->ssid,
                 MAC2STR(event->bssid), event->channel);

        memcpy(s_ap_bssid, event->bssid, ETH_ALEN);
        s_ap_channel = event->channel;
        xEventGroupClearBits(s_wifi_event_group, DISCONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_reconnect && ++s_retry_num < MAX_CONNECT_RETRY_ATTEMPTS) {
            ESP_LOGI(TAG_STA, "sta disconnect, retry attempt %d...", s_retry_num);
            esp_wifi_connect();
        } else {
            ESP_LOGI(TAG_STA, "sta disconnected");
        }
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, DISCONNECTED_BIT);
    } else if (event_id == WIFI_EVENT_FTM_REPORT) {
        wifi_event_ftm_report_t *event = (wifi_event_ftm_report_t *) event_data;

        s_rtt_est = event->rtt_est;
        s_dist_est = event->dist_est;
        s_ftm_report_num_entries = event->ftm_report_num_entries;
        if (event->status == FTM_STATUS_SUCCESS) {
            xEventGroupSetBits(s_ftm_event_group, FTM_REPORT_BIT);
        } else if (event->status == FTM_STATUS_USER_TERM) {
            /* Do Nothing */
            ESP_LOGI(TAG_STA, "User terminated FTM procedure");
        } else {
            ESP_LOGI(TAG_STA, "FTM procedure with Peer("MACSTR") failed! (Status - %d)",
                     MAC2STR(event->peer_mac), event->status);
            xEventGroupSetBits(s_ftm_event_group, FTM_FAILURE_BIT);
        }
    } else if (event_id == WIFI_EVENT_AP_START) {
        s_ap_started = true;
    } else if (event_id == WIFI_EVENT_AP_STOP) {
        s_ap_started = false;
    }
}

static void ftm_print_report(void)
{
    int i;
    char *log = NULL;
    wifi_ftm_report_entry_t *ftm_report = NULL;

    if (s_ftm_report_num_entries == 0) {
        /* FTM Failure case */
        return;
    }

    if (!g_report_lvl) {
        /* No need to print, just free the internal FTM report */
        esp_wifi_ftm_get_report(NULL, 0);
        goto exit;
    }

    ftm_report = malloc(sizeof(wifi_ftm_report_entry_t) * s_ftm_report_num_entries);
    if (!ftm_report) {
        ESP_LOGE(TAG_STA, "Failed to alloc buffer for FTM report");
        goto exit;
    }
    bzero(ftm_report, sizeof(wifi_ftm_report_entry_t) * s_ftm_report_num_entries);
    if (ESP_OK != esp_wifi_ftm_get_report(ftm_report, s_ftm_report_num_entries)) {
        ESP_LOGE(TAG_STA, "Could not get FTM report");
        goto exit;
    }

    log = malloc(200);
    if (!log) {
        ESP_LOGE(TAG_STA, "Failed to alloc buffer for FTM report");
        goto exit;
    }

    bzero(log, 200);
    sprintf(log, "%s%s%s%s", g_report_lvl & BIT0 ? " Diag |":"", g_report_lvl & BIT1 ? "   RTT   |":"",
                 g_report_lvl & BIT2 ? "       T1       |       T2       |       T3       |       T4       |":"",
                 g_report_lvl & BIT3 ? "  RSSI  |":"");
    ESP_LOGI(TAG_STA, "FTM Report:");
    ESP_LOGI(TAG_STA, "|%s", log);
    for (i = 0; i < s_ftm_report_num_entries; i++) {
        char *log_ptr = log;

        bzero(log, 200);
        if (g_report_lvl & BIT0) {
            log_ptr += sprintf(log_ptr, "%6d|", ftm_report[i].dlog_token);
        }
        if (g_report_lvl & BIT1) {
            if (ftm_report[i].rtt != UINT32_MAX)
                log_ptr += sprintf(log_ptr, "%7" PRIi32 "  |", ftm_report[i].rtt);
            else
                log_ptr += sprintf(log_ptr, " INVALID |");
        }
        if (g_report_lvl & BIT2) {
            log_ptr += sprintf(log_ptr, "%14llu  |%14llu  |%14llu  |%14llu  |", ftm_report[i].t1,
                                        ftm_report[i].t2, ftm_report[i].t3, ftm_report[i].t4);
        }
        if (g_report_lvl & BIT3) {
            log_ptr += sprintf(log_ptr, "%6d  |", ftm_report[i].rssi);
        }
        ESP_LOGI(TAG_STA, "|%s", log);
    }

exit:
    if (log) {
        free(log);
    }
    if (ftm_report) {
        free(ftm_report);
    }
    s_ftm_report_num_entries = 0;
}

void initialise_wifi(void)
{
    esp_log_level_set("wifi", ESP_LOG_WARN);
    static bool initialized = false;

    if (initialized) {
        return;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    s_ftm_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    initialized = true;
}

esp_err_t wifi_add_mode(wifi_mode_t mode)
{
    wifi_mode_t cur_mode, new_mode = mode;

    if (esp_wifi_get_mode(&cur_mode)) {
        ESP_LOGE(TAG_STA, "Failed to get mode!");
        return ESP_FAIL;
    }

    if (mode == WIFI_MODE_STA) {
        if (cur_mode == WIFI_MODE_STA || cur_mode == WIFI_MODE_APSTA) {
            int bits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, 0, 1, 0);
            if (bits & CONNECTED_BIT) {
                s_reconnect = false;
                xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
                ESP_ERROR_CHECK( esp_wifi_disconnect() );
                xEventGroupWaitBits(s_wifi_event_group, DISCONNECTED_BIT, 0, 1, portMAX_DELAY);
            }
            return ESP_OK;
        } else if (cur_mode == WIFI_MODE_AP) {
            new_mode = WIFI_MODE_APSTA;
        } else {
            new_mode = WIFI_MODE_STA;
        }
    } else if (mode == WIFI_MODE_AP) {
        if (cur_mode == WIFI_MODE_AP || cur_mode == WIFI_MODE_APSTA) {
            /* Do nothing */
            return ESP_OK;
        } else if (cur_mode == WIFI_MODE_STA) {
            new_mode = WIFI_MODE_APSTA;
        } else {
            new_mode = WIFI_MODE_AP;
        }
    }

    ESP_ERROR_CHECK( esp_wifi_set_mode(new_mode) );
    return ESP_OK;
}

static bool wifi_cmd_sta_join(const char *ssid, const char *pass)
{

    if (ESP_OK != wifi_add_mode(WIFI_MODE_STA)) {
        return false;
    }

    wifi_config_t wifi_config = { 0 };
    strlcpy((char *) wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    if (pass) {
        strlcpy((char *) wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));
    }
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_connect() );
    s_reconnect = true;
    s_retry_num = 0;

    xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, 0, 1, 5000 / portTICK_PERIOD_MS);

    return true;
}

static int wifi_cmd_sta(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &sta_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, sta_args.end, argv[0]);
        return 1;
    }
    if (sta_args.disconnect->count) {
        s_reconnect = false;
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        esp_wifi_disconnect();
        xEventGroupWaitBits(s_wifi_event_group, DISCONNECTED_BIT, 0, 1, portMAX_DELAY);
        return 0;
    }

    ESP_LOGI(TAG_STA, "sta connecting to '%s'", sta_args.ssid->sval[0]);
    wifi_cmd_sta_join(sta_args.ssid->sval[0], sta_args.password->sval[0]);
    return 0;
}

static bool wifi_perform_scan(const char *ssid, bool internal)
{
    wifi_scan_config_t scan_config = { 0 };
    scan_config.ssid = (uint8_t *) ssid;
    wifi_mode_t mode;
    uint8_t i;

    ESP_ERROR_CHECK( esp_wifi_get_mode(&mode) );
    if ((mode != WIFI_MODE_STA) && (mode != WIFI_MODE_APSTA)) {
        if (ESP_OK != wifi_add_mode(WIFI_MODE_STA)) {
            return false;
        }
    }
    if (ESP_OK != esp_wifi_scan_start(&scan_config, true)) {
        ESP_LOGI(TAG_STA, "Failed to perform scan");
        return false;
    }

    esp_wifi_scan_get_ap_num(&g_scan_ap_num);
    if (g_scan_ap_num == 0) {
        ESP_LOGI(TAG_STA, "No matching AP found");
        return false;
    }

    if (g_ap_list_buffer) {
        free(g_ap_list_buffer);
    }
    g_ap_list_buffer = malloc(g_scan_ap_num * sizeof(wifi_ap_record_t));
    if (g_ap_list_buffer == NULL) {
        ESP_LOGE(TAG_STA, "Failed to malloc buffer to print scan results");
        esp_wifi_clear_ap_list();
        return false;
    }

    if (esp_wifi_scan_get_ap_records(&g_scan_ap_num, (wifi_ap_record_t *)g_ap_list_buffer) == ESP_OK) {
        if (!internal) {
            for (i = 0; i < g_scan_ap_num; i++) {
                ESP_LOGI(TAG_STA, "[%s][rssi=%d]""%s", g_ap_list_buffer[i].ssid, g_ap_list_buffer[i].rssi,
                         g_ap_list_buffer[i].ftm_responder ? "[FTM Responder]" : "");
            }
        }
    }

    ESP_LOGI(TAG_STA, "sta scan done");

    return true;
}

static int wifi_cmd_scan(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &scan_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, scan_args.end, argv[0]);
        return 1;
    }

    ESP_LOGI(TAG_STA, "sta start to scan");
    if ( scan_args.ssid->count == 1 ) {
        wifi_perform_scan(scan_args.ssid->sval[0], false);
    } else {
        wifi_perform_scan(NULL, false);
    }
    return 0;
}

static bool wifi_cmd_ap_set(const char* ssid, const char* pass, uint8_t channel, uint8_t bw)
{
    s_reconnect = false;
    strlcpy((char*) g_ap_config.ap.ssid, ssid, MAX_SSID_LEN);
    if (pass) {
        if (strlen(pass) != 0 && strlen(pass) < 8) {
            s_reconnect = true;
            ESP_LOGE(TAG_AP, "password cannot be less than 8 characters long");
            return false;
        }
        strlcpy((char*) g_ap_config.ap.password, pass, MAX_PASSPHRASE_LEN);
    }
#if !CONFIG_SOC_WIFI_SUPPORT_5G
    if (!(channel >=1 && channel <= 14)) {
        ESP_LOGE(TAG_AP, "Channel cannot be %d!", channel);
        return false;
    }
#endif
    if (bw != 20 && bw != 40) {
        ESP_LOGE(TAG_AP, "Cannot set %d MHz bandwidth!", bw);
        return false;
    }

    if (ESP_OK != wifi_add_mode(WIFI_MODE_AP)) {
        return false;
    }
    wifi_bandwidths_t bws = {0};
    wifi_protocols_t proto = {0};
    if (channel <= 14) {
        if (bw == 40) {
            proto.ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
            proto.ghz_5g = 0;
            esp_wifi_set_protocols(ESP_IF_WIFI_AP, &proto);
            bws.ghz_2g = WIFI_BW_HT40;
            esp_wifi_set_bandwidths(ESP_IF_WIFI_AP, &bws);
        } else {
            bws.ghz_2g = WIFI_BW_HT20;
            esp_wifi_set_bandwidths(ESP_IF_WIFI_AP, &bws);
        }
    } else {
#if CONFIG_SOC_WIFI_SUPPORT_5G
        if (bw == 40) {
            proto.ghz_2g = 0;
            proto.ghz_5g = WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11A;
            esp_wifi_set_protocols(ESP_IF_WIFI_AP, &proto);
            bws.ghz_5g=WIFI_BW_HT40;
            esp_wifi_set_bandwidths(ESP_IF_WIFI_AP, &bws);
        } else {
            proto.ghz_2g = 0;
            proto.ghz_5g = WIFI_PROTOCOL_11AC | WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11AX;
            esp_wifi_set_protocols(ESP_IF_WIFI_AP, &proto);
            bws.ghz_5g = WIFI_BW_HT20;
            esp_wifi_set_bandwidths(ESP_IF_WIFI_AP, &bws);
        }
#endif
    }
    if (strlen(pass) == 0) {
        g_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    g_ap_config.ap.channel = channel;
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &g_ap_config));
    ESP_LOGI(TAG_AP, "Starting SoftAP with FTM Responder support, SSID - %s, Password - %s, Primary Channel - %d, Bandwidth - %dMHz",
                        ap_args.ssid->sval[0], ap_args.password->sval[0], channel, bw);

    return true;
}

static int wifi_cmd_ap(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &ap_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, ap_args.end, argv[0]);
        return 1;
    }

    if (!wifi_cmd_ap_set(ap_args.ssid->sval[0], ap_args.password->sval[0],
                                ap_args.channel->count != 0 ? ap_args.channel->ival[0] : DEFAULT_AP_CHANNEL,
                                ap_args.bandwidth->count != 0 ? ap_args.bandwidth->ival[0] : DEFAULT_AP_BANDWIDTH)) {
        ESP_LOGE(TAG_AP, "Failed to start SoftAP!");
        return 1;
    }

    return 0;
}

static int wifi_cmd_query(int argc, char **argv)
{
    wifi_config_t cfg;
    wifi_mode_t mode;

    esp_wifi_get_mode(&mode);
    if (WIFI_MODE_AP == mode) {
        esp_wifi_get_config(WIFI_IF_AP, &cfg);
        ESP_LOGI(TAG_AP, "AP mode, %s %s", cfg.ap.ssid, cfg.ap.password);
    } else if (WIFI_MODE_STA == mode) {
        int bits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, 0, 1, 0);
        if (bits & CONNECTED_BIT) {
            esp_wifi_get_config(WIFI_IF_STA, &cfg);
            ESP_LOGI(TAG_STA, "sta mode, connected %s", cfg.ap.ssid);
        } else {
            ESP_LOGI(TAG_STA, "sta mode, disconnected");
        }
    } else {
        ESP_LOGI(TAG_STA, "NULL mode");
        return 0;
    }

    return 0;
}

wifi_ap_record_t *find_ftm_responder_ap(const char *ssid)
{
    bool retry_scan = false;
    uint8_t i;

    if (!ssid)
        return NULL;

retry:
    if (!g_ap_list_buffer || (g_scan_ap_num == 0)) {
        ESP_LOGI(TAG_STA, "Scanning for %s", ssid);
        if (false == wifi_perform_scan(ssid, true)) {
            return NULL;
        }
    }

    for (i = 0; i < g_scan_ap_num; i++) {
        if (strcmp((const char *)g_ap_list_buffer[i].ssid, ssid) == 0)
            return &g_ap_list_buffer[i];
    }

    if (!retry_scan) {
        retry_scan = true;
        if (g_ap_list_buffer) {
            free(g_ap_list_buffer);
            g_ap_list_buffer = NULL;
        }
        goto retry;
    }

    ESP_LOGI(TAG_STA, "No matching AP found");

    return NULL;
}

static int wifi_cmd_ftm(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &ftm_args);
    wifi_ap_record_t *ap_record;
    uint32_t wait_time_ms = DEFAULT_WAIT_TIME_MS;
    EventBits_t bits;

    wifi_ftm_initiator_cfg_t ftmi_cfg = {
        .frm_count = 32,
        .burst_period = 2,
        .use_get_report_api = true,
    };

    if (nerrors != 0) {
        arg_print_errors(stderr, ftm_args.end, argv[0]);
        return 0;
    }

    if (ftm_args.initiator->count != 0 && ftm_args.responder->count != 0) {
        ESP_LOGE(TAG_STA, "Invalid FTM cmd argument");
        return 0;
    }

    if (ftm_args.responder->count != 0)
        goto ftm_responder;

    bits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, 0, 1, 0);
    if (bits & CONNECTED_BIT && !ftm_args.ssid->count) {
        memcpy(ftmi_cfg.resp_mac, s_ap_bssid, ETH_ALEN);
        ftmi_cfg.channel = s_ap_channel;
    } else if (ftm_args.ssid->count == 1) {
        ap_record = find_ftm_responder_ap(ftm_args.ssid->sval[0]);
        if (ap_record) {
            memcpy(ftmi_cfg.resp_mac, ap_record->bssid, 6);
            ftmi_cfg.channel = ap_record->primary;
        } else {
            return 0;
        }
    } else {
        ESP_LOGE(TAG_STA, "Provide SSID of the AP in disconnected state!");
        return 0;
    }

    if (ftm_args.frm_count->count != 0) {
        uint8_t count = ftm_args.frm_count->ival[0];
        if (count != 0 && count != 8 && count != 16 &&
            count != 24 && count != 32 && count != 64) {
            ESP_LOGE(TAG_STA, "Invalid Frame Count! Valid options are 0/8/16/24/32/64");
            return 0;
        }
        ftmi_cfg.frm_count = count;
    }

    if (ftm_args.burst_period->count != 0) {
        if (ftm_args.burst_period->ival[0] >= 0 &&
                ftm_args.burst_period->ival[0] <= 100) {
            ftmi_cfg.burst_period = ftm_args.burst_period->ival[0];
        } else {
            ESP_LOGE(TAG_STA, "Invalid Burst Period! Valid range is 0-100");
            return 0;
        }
    }

    ESP_LOGI(TAG_STA, "Requesting FTM session with Frm Count - %d, Burst Period - %dmSec (0: No Preference)",
             ftmi_cfg.frm_count, ftmi_cfg.burst_period*100);

    if (ESP_OK != esp_wifi_ftm_initiate_session(&ftmi_cfg)) {
        ESP_LOGE(TAG_STA, "Failed to start FTM session");
        return 0;
    }

    if (ftmi_cfg.burst_period) {
        /* Wait at least double the duration of maximum FTM bursts */
        wait_time_ms = (ftmi_cfg.burst_period * 100) * (MAX_FTM_BURSTS * 2);
    }
    bits = xEventGroupWaitBits(s_ftm_event_group, FTM_REPORT_BIT | FTM_FAILURE_BIT,
                                           pdTRUE, pdFALSE, wait_time_ms / portTICK_PERIOD_MS);
    if (bits & FTM_REPORT_BIT) {
        /* Print detailed data from FTM session */
        ftm_print_report();
        ESP_LOGI(TAG_STA, "Estimated RTT - %" PRId32 " nSec, Estimated Distance - %" PRId32 ".%02" PRId32 " meters",
                          s_rtt_est, s_dist_est / 100, s_dist_est % 100);
    } else if (bits & FTM_FAILURE_BIT) {
        /* FTM Failure case */
        ESP_LOGE(TAG_STA, "FTM procedure failed!");
    } else {
        /* Timeout, end session gracefully */
        ESP_LOGE(TAG_STA, "FTM procedure timed out!");
        esp_wifi_ftm_end_session();
    }

    return 0;

ftm_responder:
    if (ftm_args.offset->count != 0) {
        int16_t offset_cm = ftm_args.offset->ival[0];

        esp_wifi_ftm_resp_set_offset(offset_cm);
    }

    if (ftm_args.enable->count != 0) {
        if (!s_ap_started) {
            ESP_LOGE(TAG_AP, "Start the SoftAP first with 'ap' command");
            return 0;
        }
        g_ap_config.ap.ftm_responder = true;
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &g_ap_config));
        ESP_LOGI(TAG_AP, "Re-starting SoftAP with FTM Responder enabled");

        return 0;
    }

    if (ftm_args.disable->count != 0) {
        if (!s_ap_started) {
            ESP_LOGE(TAG_AP, "Start the SoftAP first with 'ap' command");
            return 0;
        }
        g_ap_config.ap.ftm_responder = false;
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &g_ap_config));
        ESP_LOGI(TAG_AP, "Re-starting SoftAP with FTM Responder disabled");
    }

    return 0;
}

void register_wifi(void)
{
    sta_args.ssid = arg_str0(NULL, NULL, "<ssid>", "SSID of AP");
    sta_args.password = arg_str0(NULL, NULL, "<pass>", "password of AP");
    sta_args.disconnect = arg_lit0("d", "disconnect", "Disconnect from the connected AP");
    sta_args.end = arg_end(2);

    const esp_console_cmd_t sta_cmd = {
        .command = "sta",
        .help = "WiFi is station mode, join specified soft-AP",
        .hint = NULL,
        .func = &wifi_cmd_sta,
        .argtable = &sta_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&sta_cmd) );

    ap_args.ssid = arg_str0(NULL, NULL, "<ssid>", "SSID of AP");
    ap_args.password = arg_str0(NULL, NULL, "<pass>", "password of AP");
    ap_args.channel = arg_int0("c", "channel", "<1-11>", "Primary channel of AP");
    ap_args.bandwidth = arg_int0("b", "bandwidth", "<20/40>", "Channel bandwidth");
    ap_args.end = arg_end(2);

    const esp_console_cmd_t ap_cmd = {
        .command = "ap",
        .help = "AP mode, configure ssid and password",
        .hint = NULL,
        .func = &wifi_cmd_ap,
        .argtable = &ap_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&ap_cmd) );

    scan_args.ssid = arg_str0(NULL, NULL, "<ssid>", "SSID of AP want to be scanned");
    scan_args.end = arg_end(1);

    const esp_console_cmd_t scan_cmd = {
        .command = "scan",
        .help = "WiFi is station mode, start scan ap",
        .hint = NULL,
        .func = &wifi_cmd_scan,
        .argtable = &scan_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&scan_cmd) );

    const esp_console_cmd_t query_cmd = {
        .command = "query",
        .help = "query WiFi info",
        .hint = NULL,
        .func = &wifi_cmd_query,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&query_cmd) );

    /* FTM Initiator commands */
    ftm_args.initiator = arg_lit0("I", "ftm_initiator", "FTM Initiator mode");
    ftm_args.ssid = arg_str0("s", "ssid", "SSID", "SSID of AP");
    ftm_args.frm_count = arg_int0("c", "frm_count", "<0/8/16/24/32/64>", "FTM frames to be exchanged (0: No preference)");
    ftm_args.burst_period = arg_int0("p", "burst_period", "<0-100 (x 100 mSec)>", "Periodicity of FTM bursts in 100's of milliseconds (0: No preference)");
    /* FTM Responder commands */
    ftm_args.responder = arg_lit0("R", "ftm_responder", "FTM Responder mode");
    ftm_args.enable = arg_lit0("e", "enable", "Restart SoftAP with FTM enabled");
    ftm_args.disable = arg_lit0("d", "disable", "Restart SoftAP with FTM disabled");
    ftm_args.offset = arg_int0("o", "offset", "Offset in cm", "T1 offset in cm for FTM Responder");
    ftm_args.end = arg_end(1);

    const esp_console_cmd_t ftm_cmd = {
        .command = "ftm",
        .help = "FTM command",
        .hint = NULL,
        .func = &wifi_cmd_ftm,
        .argtable = &ftm_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&ftm_cmd) );
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

    repl_config.prompt = "ftm>";

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
#else
#error Unsupported console type
#endif

    /* Register commands */
    register_system();
    register_wifi();

    printf("\n ===============================================================\n");
    printf(" |                  Steps to test FTM Initiator                 |\n");
    printf(" |                                                              |\n");
    printf(" |  1. Use 'scan' command to scan AP's. In results, check AP's  |\n");
    printf(" |     with tag '[FTM Responder]' for supported AP's            |\n");
    printf(" |  2. Optionally connect to the AP with 'sta <SSID> <password>'|\n");
    printf(" |  3. Initiate FTM with 'ftm -I -s <SSID>'                     |\n");
    printf(" |______________________________________________________________|\n");
    printf(" |                  Steps to test FTM Responder                 |\n");
    printf(" |                                                              |\n");
    printf(" |  1. Start SoftAP with command 'ap <SSID> <password>'         |\n");
    printf(" |  2. Optionally add '-c <ch>' to set channel, '-b 40' for 40M |\n");
    printf(" |  3. Use 'help' for detailed information for all parameters   |\n");
    printf(" ================================================================\n\n");

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
