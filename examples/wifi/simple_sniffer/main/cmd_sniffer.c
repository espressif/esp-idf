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
#include "esp_app_trace.h"
#include "cmd_sniffer.h"
#include "pcap.h"
#include "sdkconfig.h"

#define SNIFFER_DEFAULT_FILE_NAME "esp-sniffer"
#define SNIFFER_FILE_NAME_MAX_LEN CONFIG_SNIFFER_PCAP_FILE_NAME_MAX_LEN
#define SNIFFER_DEFAULT_CHANNEL (1)
#define SNIFFER_PAYLOAD_FCS_LEN (4)
#define SNIFFER_PROCESS_PACKET_TIMEOUT_MS (100)
#define SNIFFER_PROCESS_APPTRACE_TIMEOUT_US (100)
#define SNIFFER_APPTRACE_RETRY  (10)

static const char *SNIFFER_TAG = "cmd_sniffer";
#define SNIFFER_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                    \
    {                                                                                     \
        if (!(a))                                                                         \
        {                                                                                 \
            ESP_LOGE(SNIFFER_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                                \
        }                                                                                 \
    } while (0)

typedef struct {
    char *filter_name;
    uint32_t filter_val;
} wlan_filter_table_t;

typedef struct {
    bool is_running;
    sniffer_intf_t interf;
    uint32_t channel;
    uint32_t filter;
#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
    char filename[SNIFFER_FILE_NAME_MAX_LEN];
#endif
    pcap_handle_t pcap;
    TaskHandle_t task;
    QueueHandle_t work_queue;
    SemaphoreHandle_t sem_task_over;
} sniffer_runtime_t;

typedef struct {
    void *payload;
    uint32_t length;
    uint32_t seconds;
    uint32_t microseconds;
} sniffer_packet_info_t;

static sniffer_runtime_t snf_rt = {0};
static wlan_filter_table_t wifi_filter_hash_table[SNIFFER_WLAN_FILTER_MAX] = {0};

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
    uint32_t wifi_filter_values[SNIFFER_WLAN_FILTER_MAX] = {WIFI_PROMIS_FILTER_MASK_MGMT, WIFI_PROMIS_FILTER_MASK_DATA,
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
    sniffer_packet_info_t packet_info;
    wifi_promiscuous_pkt_t *sniffer = (wifi_promiscuous_pkt_t *)recv_buf;
    /* prepare packet_info */
    packet_info.seconds = sniffer->rx_ctrl.timestamp / 1000000U;
    packet_info.microseconds = sniffer->rx_ctrl.timestamp % 1000000U;
    packet_info.length = sniffer->rx_ctrl.sig_len;
    /* For now, the sniffer only dumps the length of the MISC type frame */
    if (type != WIFI_PKT_MISC && !sniffer->rx_ctrl.rx_state) {
        packet_info.length -= SNIFFER_PAYLOAD_FCS_LEN;
        void *backup = malloc(packet_info.length);
        if (backup) {
            memcpy(backup, sniffer->payload, packet_info.length);
            packet_info.payload = backup;
            if (snf_rt.work_queue) {
                /* send packet_info */
                if (xQueueSend(snf_rt.work_queue, &packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS)) != pdTRUE) {
                    ESP_LOGE(SNIFFER_TAG, "sniffer work queue full");
                }
            }
        } else {
            ESP_LOGE(SNIFFER_TAG, "No enough memory for promiscuous packet");
        }
    }
}

static void sniffer_task(void *parameters)
{
    sniffer_packet_info_t packet_info;
    sniffer_runtime_t *sniffer = (sniffer_runtime_t *)parameters;

    while (sniffer->is_running) {
        /* receive paclet info from queue */
        if (xQueueReceive(sniffer->work_queue, &packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS)) != pdTRUE) {
            continue;
        }
        if (pcap_capture_packet(sniffer->pcap, packet_info.payload, packet_info.length,
                                packet_info.seconds, packet_info.microseconds) != ESP_OK) {
            ESP_LOGW(SNIFFER_TAG, "save captured packet failed");
        }
        free(packet_info.payload);
    }
    /* notify that sniffer task is over */
    xSemaphoreGive(sniffer->sem_task_over);
    vTaskDelete(NULL);
}

static esp_err_t sniffer_stop(sniffer_runtime_t *sniffer)
{
    SNIFFER_CHECK(sniffer->is_running, "sniffer is already stopped", err);

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        /* Disable wifi promiscuous mode */
        SNIFFER_CHECK(esp_wifi_set_promiscuous(false) == ESP_OK, "stop wifi promiscuous failed", err);
        break;
    default:
        SNIFFER_CHECK(false, "unsupported interface", err);
        break;
    }
    ESP_LOGI(SNIFFER_TAG, "stop WiFi promiscuous ok");

    /* stop sniffer local task */
    sniffer->is_running = false;
    /* wait for task over */
    xSemaphoreTake(sniffer->sem_task_over, portMAX_DELAY);
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
    SNIFFER_CHECK(pcap_deinit(sniffer->pcap) == ESP_OK, "stop pcap session failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
static int trace_writefun(void *cookie, const char *buf, int len)
{
    return esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, buf, len, SNIFFER_PROCESS_APPTRACE_TIMEOUT_US) == ESP_OK ? len : -1;
}

static int trace_closefun(void *cookie)
{
    return esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, ESP_APPTRACE_TMO_INFINITE) == ESP_OK ? 0 : -1;
}
#endif

static esp_err_t sniffer_start(sniffer_runtime_t *sniffer)
{
    pcap_config_t pcap_config;
    wifi_promiscuous_filter_t wifi_filter;

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        pcap_config.link_type = PCAP_LINK_TYPE_802_11;
        break;
    default:
        SNIFFER_CHECK(false, "unsupported interface", err);
        break;
    }

    /* Create file to write, binary format */
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    pcap_config.fp = funopen("trace", NULL, trace_writefun, NULL, trace_closefun);
#elif CONFIG_SNIFFER_PCAP_DESTINATION_SD
    pcap_config.fp = fopen(sniffer->filename, "wb");
#else
#error "pcap file destination hasn't specified"
#endif
    SNIFFER_CHECK(pcap_config.fp, "open file failed", err);
    ESP_LOGI(SNIFFER_TAG, "open file successfully");

    /* init a pcap session */
    SNIFFER_CHECK(pcap_init(&pcap_config, &sniffer->pcap) == ESP_OK, "init pcap session failed", err);

    sniffer->is_running = true;
    sniffer->work_queue = xQueueCreate(CONFIG_SNIFFER_WORK_QUEUE_LEN, sizeof(sniffer_packet_info_t));
    SNIFFER_CHECK(sniffer->work_queue, "create work queue failed", err_queue);
    sniffer->sem_task_over = xSemaphoreCreateBinary();
    SNIFFER_CHECK(sniffer->sem_task_over, "create sem failed", err_sem);
    SNIFFER_CHECK(xTaskCreate(sniffer_task, "snifferT", CONFIG_SNIFFER_TASK_STACK_SIZE,
                              sniffer, CONFIG_SNIFFER_TASK_PRIORITY, &sniffer->task) == pdTRUE,
                  "create task failed", err_task);

    switch (sniffer->interf) {
    case SNIFFER_INTF_WLAN:
        /* Start WiFi Promicuous Mode */
        wifi_filter.filter_mask = sniffer->filter;
        esp_wifi_set_promiscuous_filter(&wifi_filter);
        esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
        SNIFFER_CHECK(esp_wifi_set_promiscuous(true) == ESP_OK, "start wifi promiscuous failed", err_start);
        esp_wifi_set_channel(sniffer->channel, WIFI_SECOND_CHAN_NONE);
        ESP_LOGI(SNIFFER_TAG, "start WiFi promiscuous ok");
        break;
    default:
        break;
    }
    return ESP_OK;
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
    pcap_deinit(sniffer->pcap);
err:
    return ESP_FAIL;
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
    snf_rt.interf = SNIFFER_INTF_WLAN;
    if (sniffer_args.interface->count) {
        if (!strncmp(sniffer_args.interface->sval[0], "wlan", 4)) {
            snf_rt.interf = SNIFFER_INTF_WLAN;
        } else {
            ESP_LOGE(SNIFFER_TAG, "unsupported interface %s", sniffer_args.interface->sval[0]);
            return 1;
        }
    }

    /* Check channel: "-c" option */
    snf_rt.channel = SNIFFER_DEFAULT_CHANNEL;
    if (sniffer_args.channel->count) {
        snf_rt.channel = sniffer_args.channel->ival[0];
    }

#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
    /* set pcap file name: "-f" option */
    int len = snprintf(snf_rt.filename, sizeof(snf_rt.filename), "%s/%s.pcap", CONFIG_SNIFFER_MOUNT_POINT, SNIFFER_DEFAULT_FILE_NAME);
    if (sniffer_args.file->count) {
        len = snprintf(snf_rt.filename, sizeof(snf_rt.filename), "%s/%s.pcap", CONFIG_SNIFFER_MOUNT_POINT, sniffer_args.file->sval[0]);
    }
    if (len >= sizeof(snf_rt.filename)) {
        ESP_LOGW(SNIFFER_TAG, "pcap file name too long, try to enlarge memory in menuconfig");
    }
#endif
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    uint32_t retry = 0;
    /* wait until apptrace communication established or timeout */
    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX) && (retry < SNIFFER_APPTRACE_RETRY)) {
        retry++;
        ESP_LOGW(SNIFFER_TAG, "waiting for apptrace established");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (retry >= SNIFFER_APPTRACE_RETRY) {
        ESP_LOGE(SNIFFER_TAG, "waiting for apptrace established timeout");
        return 1;
    }
#endif

    /* Check filter setting: "-F" option */
    switch (snf_rt.interf) {
    case SNIFFER_INTF_WLAN:
        if (sniffer_args.filter->count) {
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
    default:
        break;
    }

    /* start sniffer */
    sniffer_start(&snf_rt);

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
