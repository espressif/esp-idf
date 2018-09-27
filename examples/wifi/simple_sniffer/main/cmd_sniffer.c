/* cmd_sniffer example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
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
#include "cmd_sniffer.h"
#include "pcap.h"
#include "sdkconfig.h"

#define SNIFFER_DEFAULT_FILE_NAME "sniffer"
#define SNIFFER_DEFAULT_CHANNEL 1

static const char *TAG = "cmd_sniffer";

static bool sniffer_running = false;
static pcap_config_t pcap_config;
static QueueHandle_t sniffer_work_queue = NULL;
static SemaphoreHandle_t sem_task_over = NULL;

static wlan_filter_table_t wifi_filter_hash_table[SNIFFER_WLAN_FILTER_MAX] = {0};
static char packet_filepath[PCAP_FILE_NAME_MAX_LEN];

typedef struct {
    void *payload;
    uint32_t length;
    uint32_t seconds;
    uint32_t microseconds;
} sniffer_packet_into_t;

static esp_err_t create_packet_file(void)
{
    uint32_t file_no = 0;
    char filename[PCAP_FILE_NAME_MAX_LEN];
    do {
        snprintf(filename, PCAP_FILE_NAME_MAX_LEN, "%s%d.pcap", packet_filepath, file_no);
        file_no++;
    } while (0 == access(filename, F_OK));
    /* Create file to write, binary format */
    pcap_config.fp = fopen(filename, "wb");
    if (!pcap_config.fp) {
        ESP_LOGE(TAG, "Create file %s failed", filename);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Store packets to file: %s", filename);

    return ESP_OK;
}

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

static void create_wifi_filter_hashtable()
{
    char *wifi_filter_keys[SNIFFER_WLAN_FILTER_MAX] = {"mgmt", "data", "ctrl", "misc", "mpdu", "ampdu"};
    uint32_t wifi_filter_values[SNIFFER_WLAN_FILTER_MAX] =  {WIFI_PROMIS_FILTER_MASK_MGMT, WIFI_PROMIS_FILTER_MASK_DATA,
                                                             WIFI_PROMIS_FILTER_MASK_CTRL, WIFI_PROMIS_FILTER_MASK_MISC,
                                                             WIFI_PROMIS_FILTER_MASK_DATA_MPDU, WIFI_PROMIS_FILTER_MASK_DATA_AMPDU
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

static void wifi_sniffer_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type)
{
    if (sniffer_running) {
        sniffer_packet_into_t packet_info;
        wifi_promiscuous_pkt_t *sniffer = (wifi_promiscuous_pkt_t *)recv_buf;
        /* prepare packet_info */
        packet_info.seconds = sniffer->rx_ctrl.timestamp / 1000000U;
        packet_info.microseconds = sniffer->rx_ctrl.timestamp % 1000000U;
        packet_info.length = sniffer->rx_ctrl.sig_len;
        wifi_promiscuous_pkt_t *backup = malloc(sniffer->rx_ctrl.sig_len);
        if (backup) {
            memcpy(backup, sniffer->payload, sniffer->rx_ctrl.sig_len);
            packet_info.payload = backup;
            if (sniffer_work_queue) {
                /* send packet_info */
                if (xQueueSend(sniffer_work_queue, &packet_info, 100 / portTICK_PERIOD_MS) != pdTRUE) {
                    ESP_LOGE(TAG, "sniffer work queue full");
                }
            }
        } else {
            ESP_LOGE(TAG, "No enough memory for promiscuous packet");
        }
    }
}

static void sniffer_task(void *parameters)
{
    static uint32_t count = 0;
    sniffer_packet_into_t packet_info;
    BaseType_t ret = 0;

    while (sniffer_running) {
        /* receive paclet info from queue */
        ret = xQueueReceive(sniffer_work_queue, &packet_info, 100 / portTICK_PERIOD_MS);
        if (ret != pdTRUE) {
            continue;
        }
        if (pcap_capture_packet(packet_info.payload, packet_info.length,
                                packet_info.seconds, packet_info.microseconds) == ESP_OK) {
            count++;
            /* truncate, create another file */
            if (count >= CONFIG_PCAP_FILE_MAX_PACKETS) {
                pcap_close();
                if (create_packet_file() != ESP_OK || pcap_new(&pcap_config) != ESP_OK) {
                    sniffer_running = false;
                } else {
                    count = 0;
                }
            }
        }
        free(packet_info.payload);
    }
    /* notify that sniffer task is over */
    xSemaphoreGive(sem_task_over);
    vTaskDelete(NULL);
}

static esp_err_t snifer_stop(sniffer_config_t *sniffer)
{
    /* Do interface specific work here */
    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        /* Disable wifi promiscuous mode */
        esp_wifi_set_promiscuous(false);
        break;
    default:
        break;
    }
    /* stop sniffer local task */
    sniffer_running = false;
    /* wait for task over */
    xSemaphoreTake(sem_task_over, portMAX_DELAY);
    vSemaphoreDelete(sem_task_over);
    sem_task_over = NULL;
    /* make sure to free all resources in the left items */
    UBaseType_t left_items = uxQueueMessagesWaiting(sniffer_work_queue);
    sniffer_packet_into_t packet_info;
    while (left_items--) {
        xQueueReceive(sniffer_work_queue, &packet_info, 100 / portTICK_PERIOD_MS);
        free(packet_info.payload);
    }
    /* delete queue */
    vQueueDelete(sniffer_work_queue);
    sniffer_work_queue = NULL;
    /* Close the pcap file */
    pcap_close();

    ESP_LOGI(TAG, "Sniffer Stopped");
    return ESP_OK;
}

static esp_err_t sniffer_start(sniffer_config_t *sniffer)
{
    wifi_promiscuous_filter_t wifi_filter;
    /* set sniffer running status before it starts to run */
    sniffer_running = true;
    sniffer_work_queue = xQueueCreate(CONFIG_SNIFFER_WORK_QUEUE_LENGTH, sizeof(sniffer_packet_into_t));
    sem_task_over = xSemaphoreCreateBinary();
    /* sniffer task going to run*/
    xTaskCreate(sniffer_task, "sniffer", CONFIG_SNIFFER_TASK_STACK_SIZE, NULL, CONFIG_SNIFFER_TASK_PRIORITY, NULL);

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        /* Set Promicuous Mode */
        wifi_filter.filter_mask = sniffer->filter;
        esp_wifi_set_promiscuous_filter(&wifi_filter);
        esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
        ESP_LOGI(TAG, "Start WiFi Promicuous Mode");
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
        /* Specify the channel */
        esp_wifi_set_channel(sniffer->channel, WIFI_SECOND_CHAN_NONE);
        /* Create a new pcap object */
        pcap_config.link_type = PCAP_LINK_TYPE_802_11;
        pcap_new(&pcap_config);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static struct {
    struct arg_str *file;
    struct arg_str *interface;
    struct arg_str *filter;
    struct arg_int *channel;
    struct arg_lit *stop;
    struct arg_end *end;
} sniffer_args;

static int do_sniffer_cmd(int argc, char **argv)
{
    sniffer_config_t sniffer;

    int nerrors = arg_parse(argc, argv, (void **)&sniffer_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, sniffer_args.end, argv[0]);
        return 0;
    }

    memset(&sniffer, 0, sizeof(sniffer));

    /* Check interface: "-i" option */
    if (sniffer_args.interface->count) {
        if (!strncmp(sniffer_args.interface->sval[0], "wlan", 4)) {
            sniffer.interf = SNIFFER_INTF_WLAN;
        } else {
            ESP_LOGE(TAG, "Do not support interface %s", sniffer_args.interface->sval[0]);
            return 1;
        }
    } else {
        sniffer.interf = SNIFFER_INTF_WLAN;
    }
    /* Check whether or not to stop sniffer: "--stop" option */
    if (sniffer_args.stop->count) {
        /* stop sniffer */
        snifer_stop(&sniffer);
        return 0;
    }
    /* Check channel: "-c" option */
    sniffer.channel = 0;
    if (sniffer_args.channel->count) {
        sniffer.channel = sniffer_args.channel->ival[0];
    } else {
        sniffer.channel = SNIFFER_DEFAULT_CHANNEL;
    }

    /* set pcap file name: "-f" option */
    if (sniffer_args.file->count) {
        snprintf(packet_filepath, PCAP_FILE_NAME_MAX_LEN, "%s/%s",
                 CONFIG_SNIFFER_MOUNT_POINT, sniffer_args.file->sval[0]);
    } else {
        snprintf(packet_filepath, PCAP_FILE_NAME_MAX_LEN, "%s/%s",
                 CONFIG_SNIFFER_MOUNT_POINT, SNIFFER_DEFAULT_FILE_NAME);
    }
    /* Determin file name */
    if (create_packet_file() != ESP_OK) {
        return 1;
    }

    /* Check filter setting: "-F" option */
    switch (sniffer.interf) {
    case SNIFFER_INTF_WLAN:
        if (sniffer_args.filter->count) {
            for (int i = 0; i < sniffer_args.filter->count; i++) {
                sniffer.filter += search_wifi_filter_hashtable(sniffer_args.filter->sval[i]);
            }
            /* When filter conditions are all wrong */
            if (sniffer.filter == 0) {
                sniffer.filter = WIFI_PROMIS_FILTER_MASK_ALL;
            }
        } else {
            sniffer.filter = WIFI_PROMIS_FILTER_MASK_ALL;
        }
        break;
    default:
        break;
    }

    /* start sniffer */
    sniffer_start(&sniffer);

    return 0;
}

void register_sniffer()
{
    sniffer_args.file = arg_str0("f", "file", "<file>",
                                 "name of the file storing the packets in pcap format");
    sniffer_args.interface = arg_str0("i", "interface", "<wlan>",
                                      "which interface to capture packet");
    sniffer_args.filter = arg_strn("F", "filter", "<mgmt|data|ctrl|misc|mpdu|ampdu>", 0, 6, "filter parameters");
    sniffer_args.channel = arg_int0("c", "channel", "<channel>", "communication channel to use");
    sniffer_args.stop = arg_lit0(NULL, "stop", "stop running sniffer");
    sniffer_args.end = arg_end(1);
    const esp_console_cmd_t iperf_cmd = {
        .command = "sniffer",
        .help = "Capture specific packet and store in pcap format",
        .hint = NULL,
        .func = &do_sniffer_cmd,
        .argtable = &sniffer_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&iperf_cmd));

    create_wifi_filter_hashtable();
}
