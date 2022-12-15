/* cmd_sniffer example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <stdlib.h>
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_console.h"
#include "esp_app_trace.h"
#include "cmd_sniffer.h"
#include "cmd_pcap.h"
#include "esp_check.h"
#include "sdkconfig.h"

#define SNIFFER_DEFAULT_CHANNEL             (1)
#define SNIFFER_PAYLOAD_FCS_LEN             (4)
#define SNIFFER_PROCESS_PACKET_TIMEOUT_MS   (100)
#define SNIFFER_RX_FCS_ERR                  (0X41)
#define SNIFFER_MAX_ETH_INTFS               (3)
#define SNIFFER_DECIMAL_NUM                 (10)


static const char *SNIFFER_TAG = "cmd_sniffer";

typedef struct {
    char *filter_name;
    uint32_t filter_val;
} wlan_filter_table_t;

typedef struct {
    bool is_running;
    sniffer_intf_t interf;
    uint32_t interf_num;
    uint32_t channel;
    uint32_t filter;
    int32_t packets_to_sniff;
    TaskHandle_t task;
    QueueHandle_t work_queue;
    SemaphoreHandle_t sem_task_over;
    esp_eth_handle_t eth_handles[SNIFFER_MAX_ETH_INTFS];
} sniffer_runtime_t;

typedef struct {
    void *payload;
    uint32_t length;
    uint32_t seconds;
    uint32_t microseconds;
} sniffer_packet_info_t;

static sniffer_runtime_t snf_rt = {0};
static wlan_filter_table_t wifi_filter_hash_table[SNIFFER_WLAN_FILTER_MAX] = {0};
static esp_err_t sniffer_stop(sniffer_runtime_t *sniffer);

static uint32_t hash_func(const char *str, uint32_t max_num)
{
    uint32_t ret = 0;
    char *p = (char *)str;
    while (*p) {
        ret += *p;
        p++;
    }
    return ret % max_num;
}

static void create_wifi_filter_hashtable(void)
{
    char *wifi_filter_keys[SNIFFER_WLAN_FILTER_MAX] = {"mgmt", "data", "ctrl", "misc", "mpdu", "ampdu", "fcsfail"};
    uint32_t wifi_filter_values[SNIFFER_WLAN_FILTER_MAX] = {WIFI_PROMIS_FILTER_MASK_MGMT, WIFI_PROMIS_FILTER_MASK_DATA,
                                                            WIFI_PROMIS_FILTER_MASK_CTRL, WIFI_PROMIS_FILTER_MASK_MISC,
                                                            WIFI_PROMIS_FILTER_MASK_DATA_MPDU, WIFI_PROMIS_FILTER_MASK_DATA_AMPDU,
                                                            WIFI_PROMIS_FILTER_MASK_FCSFAIL
                                                           };
    for (int i = 0; i < SNIFFER_WLAN_FILTER_MAX; i++) {
        uint32_t idx = hash_func(wifi_filter_keys[i], SNIFFER_WLAN_FILTER_MAX);
        while (wifi_filter_hash_table[idx].filter_name) {
            idx++;
            if (idx >= SNIFFER_WLAN_FILTER_MAX) {
                idx = 0;
            }
        }
        wifi_filter_hash_table[idx].filter_name = wifi_filter_keys[i];
        wifi_filter_hash_table[idx].filter_val = wifi_filter_values[i];
    }
}

static uint32_t search_wifi_filter_hashtable(const char *key)
{
    uint32_t len = strlen(key);
    uint32_t start_idx = hash_func(key, SNIFFER_WLAN_FILTER_MAX);
    uint32_t idx = start_idx;
    while (strncmp(wifi_filter_hash_table[idx].filter_name, key, len)) {
        idx++;
        if (idx >= SNIFFER_WLAN_FILTER_MAX) {
            idx = 0;
        }
        /* wrong key */
        if (idx == start_idx) {
            return 0;
        }
    }
    return wifi_filter_hash_table[idx].filter_val;
}

static void queue_packet(void *recv_packet, sniffer_packet_info_t *packet_info)
{
    /* Copy a packet from Link Layer driver and queue the copy to be processed by sniffer task */
    void *packet_to_queue = malloc(packet_info->length);
    if (packet_to_queue) {
        memcpy(packet_to_queue, recv_packet, packet_info->length);
        packet_info->payload = packet_to_queue;
        if (snf_rt.work_queue) {
            /* send packet_info */
            if (xQueueSend(snf_rt.work_queue, packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS)) != pdTRUE) {
                ESP_LOGE(SNIFFER_TAG, "sniffer work queue full");
                free(packet_info->payload);
            }
        }
    } else {
        ESP_LOGE(SNIFFER_TAG, "No enough memory for promiscuous packet");
    }
}

static void wifi_sniffer_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type)
{
    sniffer_packet_info_t packet_info;
    wifi_promiscuous_pkt_t *sniffer = (wifi_promiscuous_pkt_t *)recv_buf;
    /* prepare packet_info */
    packet_info.seconds = sniffer->rx_ctrl.timestamp / 1000000U;
    packet_info.microseconds = sniffer->rx_ctrl.timestamp % 1000000U;
    packet_info.length = sniffer->rx_ctrl.sig_len;

    /* For now, the sniffer only dumps the length of the MISC type frame */
    if (type != WIFI_PKT_MISC && !sniffer->rx_ctrl.rx_state) {
        packet_info.length -= SNIFFER_PAYLOAD_FCS_LEN;
        queue_packet(sniffer->payload, &packet_info);
    }
}

static esp_err_t eth_sniffer_cb(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
    sniffer_packet_info_t packet_info;
    struct timeval tv_now;

    // ESP32 Ethernet MAC provides hardware time stamping for incoming frames in its Linked List Descriptors (see TMR, section 10.8.2).
    // However, this information is not currently accessible via Ethernet driver => do at least software time stamping
    gettimeofday(&tv_now, NULL);

    packet_info.seconds = tv_now.tv_sec;
    packet_info.microseconds = tv_now.tv_usec;
    packet_info.length = length;

    queue_packet(buffer, &packet_info);

    free(buffer);

    return ESP_OK;
}

static void sniffer_task(void *parameters)
{
    sniffer_packet_info_t packet_info;
    sniffer_runtime_t *sniffer = (sniffer_runtime_t *)parameters;

    while (sniffer->is_running) {
        if (sniffer->packets_to_sniff == 0) {
            sniffer_stop(sniffer);
            break;
        }
        /* receive packet info from queue */
        if (xQueueReceive(sniffer->work_queue, &packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS)) != pdTRUE) {
            continue;
        }
        if (packet_capture(packet_info.payload, packet_info.length, packet_info.seconds,
                           packet_info.microseconds) != ESP_OK) {
            ESP_LOGW(SNIFFER_TAG, "save captured packet failed");
        }
        free(packet_info.payload);
        if (sniffer->packets_to_sniff > 0) {
            sniffer->packets_to_sniff--;
        }

    }
    /* notify that sniffer task is over */
    if (sniffer->packets_to_sniff != 0) {
        xSemaphoreGive(sniffer->sem_task_over);
    }
    vTaskDelete(NULL);
}

static esp_err_t sniffer_stop(sniffer_runtime_t *sniffer)
{
    bool eth_set_promiscuous;
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_FALSE(sniffer->is_running, ESP_ERR_INVALID_STATE, err, SNIFFER_TAG, "sniffer is already stopped");

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        /* Disable wifi promiscuous mode */
        ESP_GOTO_ON_ERROR(esp_wifi_set_promiscuous(false), err, SNIFFER_TAG, "stop wifi promiscuous failed");
        break;
    case SNIFFER_INTF_ETH:
        /* Disable Ethernet Promiscuous Mode */
        eth_set_promiscuous = false;
        ESP_GOTO_ON_ERROR(esp_eth_ioctl(sniffer->eth_handles[sniffer->interf_num], ETH_CMD_S_PROMISCUOUS, &eth_set_promiscuous),
                          err, SNIFFER_TAG, "stop Ethernet promiscuous failed");
        esp_eth_update_input_path(sniffer->eth_handles[sniffer->interf_num], NULL, NULL);
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, SNIFFER_TAG, "unsupported interface");
        break;
    }
    ESP_LOGI(SNIFFER_TAG, "stop promiscuous ok");

    /* stop sniffer local task */
    sniffer->is_running = false;
    /* wait for task over */
    if (sniffer->packets_to_sniff != 0) {
        xSemaphoreTake(sniffer->sem_task_over, portMAX_DELAY);
    }

    vSemaphoreDelete(sniffer->sem_task_over);
    sniffer->sem_task_over = NULL;
    /* make sure to free all resources in the left items */
    UBaseType_t left_items = uxQueueMessagesWaiting(sniffer->work_queue);

    sniffer_packet_info_t packet_info;
    while (left_items--) {
        xQueueReceive(sniffer->work_queue, &packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS));
        free(packet_info.payload);
    }
    vQueueDelete(sniffer->work_queue);
    sniffer->work_queue = NULL;

    /* stop pcap session */
    sniff_packet_stop();
err:
    return ret;
}

static esp_err_t sniffer_start(sniffer_runtime_t *sniffer)
{
    esp_err_t ret = ESP_OK;
    pcap_link_type_t link_type;
    wifi_promiscuous_filter_t wifi_filter;
    bool eth_set_promiscuous;

    ESP_GOTO_ON_FALSE(!(sniffer->is_running), ESP_ERR_INVALID_STATE, err, SNIFFER_TAG, "sniffer is already running");

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        link_type = PCAP_LINK_TYPE_802_11;
        break;
    case SNIFFER_INTF_ETH:
        link_type = PCAP_LINK_TYPE_ETHERNET;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, SNIFFER_TAG, "unsupported interface");
        break;
    }

    /* init a pcap session */
    ESP_GOTO_ON_ERROR(sniff_packet_start(link_type), err, SNIFFER_TAG, "init pcap session failed");

    sniffer->is_running = true;
    sniffer->work_queue = xQueueCreate(CONFIG_SNIFFER_WORK_QUEUE_LEN, sizeof(sniffer_packet_info_t));
    ESP_GOTO_ON_FALSE(sniffer->work_queue, ESP_FAIL, err_queue, SNIFFER_TAG, "create work queue failed");
    sniffer->sem_task_over = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(sniffer->sem_task_over, ESP_FAIL, err_sem, SNIFFER_TAG, "create work queue failed");
    ESP_GOTO_ON_FALSE(xTaskCreate(sniffer_task, "snifferT", CONFIG_SNIFFER_TASK_STACK_SIZE,
                                  sniffer, CONFIG_SNIFFER_TASK_PRIORITY, &sniffer->task), ESP_FAIL,
                      err_task, SNIFFER_TAG, "create task failed");

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        /* Start WiFi Promiscuous Mode */
        wifi_filter.filter_mask = sniffer->filter;
        esp_wifi_set_promiscuous_filter(&wifi_filter);
        esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
        ESP_GOTO_ON_ERROR(esp_wifi_set_promiscuous(true), err_start, SNIFFER_TAG, "create work queue failed");
        esp_wifi_set_channel(sniffer->channel, WIFI_SECOND_CHAN_NONE);
        ESP_LOGI(SNIFFER_TAG, "start WiFi promiscuous ok");
        break;
    case SNIFFER_INTF_ETH:
        /* Start Ethernet Promiscuous Mode */
        eth_set_promiscuous = true;
        ESP_GOTO_ON_ERROR(esp_eth_ioctl(sniffer->eth_handles[sniffer->interf_num], ETH_CMD_S_PROMISCUOUS, &eth_set_promiscuous),
                          err_start, SNIFFER_TAG, "start Ethernet promiscuous failed");
        esp_eth_update_input_path(sniffer->eth_handles[sniffer->interf_num], eth_sniffer_cb, NULL);
        ESP_LOGI(SNIFFER_TAG, "start Ethernet promiscuous ok");
        break;
    default:
        break;
    }
    return ret;
err_start:
    vTaskDelete(sniffer->task);
    sniffer->task = NULL;
err_task:
    vSemaphoreDelete(sniffer->sem_task_over);
    sniffer->sem_task_over = NULL;
err_sem:
    vQueueDelete(sniffer->work_queue);
    sniffer->work_queue = NULL;
err_queue:
    sniffer->is_running = false;
err:
    return ret;
}

static struct {
    struct arg_str *interface;
    struct arg_str *filter;
    struct arg_int *channel;
    struct arg_lit *stop;
    struct arg_int *number;
    struct arg_end *end;
} sniffer_args;

esp_err_t sniffer_reg_eth_intf(esp_eth_handle_t eth_handle)
{
    esp_err_t ret = ESP_OK;
    int32_t i = 0;
    while ((snf_rt.eth_handles[i] != NULL) && (i < SNIFFER_MAX_ETH_INTFS)) {
        i++;
    }
    ESP_GOTO_ON_FALSE(i < SNIFFER_MAX_ETH_INTFS, ESP_FAIL, err, SNIFFER_TAG, "maximum num. of eth interfaces registered");
    snf_rt.eth_handles[i] = eth_handle;

err:
    return ret;
}

static int do_sniffer_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&sniffer_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, sniffer_args.end, argv[0]);
        return 0;
    }

    /* Check whether or not to stop sniffer: "--stop" option */
    if (sniffer_args.stop->count) {
        /* stop sniffer */
        sniffer_stop(&snf_rt);
        return 0;
    }

    /* Check interface: "-i" option */
    if (sniffer_args.interface->count) {
        if (!strncmp(sniffer_args.interface->sval[0], "wlan", 4)) {
            snf_rt.interf = SNIFFER_INTF_WLAN;
        } else if (!strncmp(sniffer_args.interface->sval[0], "eth", 3)
                   && strlen(sniffer_args.interface->sval[0]) >= 4) {
            char *end_ptr = NULL;
            const char *eth_num_str_start = sniffer_args.interface->sval[0] + 3;
            int32_t eth_intf_num = strtol(eth_num_str_start, &end_ptr, SNIFFER_DECIMAL_NUM);

            if ((eth_intf_num >= 0) && (eth_intf_num < SNIFFER_MAX_ETH_INTFS)
                    && (eth_num_str_start != end_ptr) && (snf_rt.eth_handles[eth_intf_num] != NULL)) {
                snf_rt.interf = SNIFFER_INTF_ETH;
                snf_rt.interf_num = eth_intf_num;
            } else {
                ESP_LOGE(SNIFFER_TAG, "interface %s not found", sniffer_args.interface->sval[0]);
                return 1;
            }
        } else {
            ESP_LOGE(SNIFFER_TAG, "interface %s not found", sniffer_args.interface->sval[0]);
            return 1;
        }
    } else {
        snf_rt.interf = SNIFFER_INTF_WLAN;
        ESP_LOGW(SNIFFER_TAG, "sniffing interface set to wlan by default");
    }

    /* Check channel: "-c" option */
    switch (snf_rt.interf) {
    case SNIFFER_INTF_WLAN:
        snf_rt.channel = SNIFFER_DEFAULT_CHANNEL;
        if (sniffer_args.channel->count) {
            snf_rt.channel = sniffer_args.channel->ival[0];
        }
        break;
    case SNIFFER_INTF_ETH:
        if (sniffer_args.channel->count) {
            ESP_LOGW(SNIFFER_TAG, "'channel' option is not available for Ethernet");
        }
        break;
    default:
        break;
    }

    /* Check filter setting: "-F" option */
    switch (snf_rt.interf) {
    case SNIFFER_INTF_WLAN:
        if (sniffer_args.filter->count) {
            snf_rt.filter = 0;
            for (int i = 0; i < sniffer_args.filter->count; i++) {
                snf_rt.filter += search_wifi_filter_hashtable(sniffer_args.filter->sval[i]);
            }
            /* When filter conditions are all wrong */
            if (snf_rt.filter == 0) {
                snf_rt.filter = WIFI_PROMIS_FILTER_MASK_ALL;
            }
        } else {
            snf_rt.filter = WIFI_PROMIS_FILTER_MASK_ALL;
        }
        break;
    case SNIFFER_INTF_ETH:
        if (sniffer_args.filter->count) {
            ESP_LOGW(SNIFFER_TAG, "'filter' option is not available for Ethernet");
        }
    default:
        break;
    }

    /* Check the number of captured packages: "-n" option */
    snf_rt.packets_to_sniff = -1;
    if (sniffer_args.number->count) {
        snf_rt.packets_to_sniff = sniffer_args.number->ival[0];
        ESP_LOGI(SNIFFER_TAG, "%" PRIi32 " packages will be captured", snf_rt.packets_to_sniff);
    }

    /* start sniffer */
    sniffer_start(&snf_rt);
    return 0;
}

void register_sniffer_cmd(void)
{
    sniffer_args.number = arg_int0("n", "number", "<num>",
                                   "the number of the packets to be captured");
    sniffer_args.interface = arg_str0("i", "interface", "<wlan|eth0|eth1|...>",
                                      "which interface to capture packet");
    sniffer_args.filter = arg_strn("F", "filter", "<mgmt|data|ctrl|misc|mpdu|ampdu|fcsfail>", 0, 7, "filter parameters");
    sniffer_args.channel = arg_int0("c", "channel", "<channel>", "communication channel to use");
    sniffer_args.stop = arg_lit0(NULL, "stop", "stop running sniffer");
    sniffer_args.end = arg_end(1);
    const esp_console_cmd_t sniffer_cmd = {
        .command = "sniffer",
        .help = "Capture specific packet and store in pcap format",
        .hint = NULL,
        .func = &do_sniffer_cmd,
        .argtable = &sniffer_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&sniffer_cmd));

    create_wifi_filter_hashtable();
}
