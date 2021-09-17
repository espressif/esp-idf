/* cmd_pcap example.
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
#ifdef CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
#include "freertos/timers.h"
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_console.h"
#include "esp_app_trace.h"
#include "esp_check.h"
#include "cmd_sniffer.h"
#include "cmd_pcap.h"
#include "sdkconfig.h"

static const char *CMD_PCAP_TAG = "cmd_pcap";


#define PCAP_FILE_NAME_MAX_LEN CONFIG_SNIFFER_PCAP_FILE_NAME_MAX_LEN
#define PCAP_MEMORY_BUFFER_SIZE CONFIG_SNIFFER_PCAP_MEMORY_SIZE
#define SNIFFER_PROCESS_APPTRACE_TIMEOUT_US (100)
#define SNIFFER_APPTRACE_RETRY              (10)
#define TRACE_TIMER_FLUSH_INT_MS            (1000)

#if CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY
/**
* @brief Pcap memory buffer Type Definition
*
*/
typedef struct {
    char *buffer;
    uint32_t buffer_size;
} pcap_memory_buffer_t;
#endif

typedef struct {
    bool is_opened;
    bool is_writing;
    bool link_type_set;
#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
    char filename[PCAP_FILE_NAME_MAX_LEN];
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_SD
    pcap_file_handle_t pcap_handle;
    pcap_link_type_t link_type;
#if CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY
    pcap_memory_buffer_t pcap_buffer;
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY
#ifdef CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    TimerHandle_t trace_flush_timer; /*!< Timer handle for Trace buffer flush */
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
} pcap_cmd_runtime_t;

static pcap_cmd_runtime_t pcap_cmd_rt = {0};

#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
static int trace_writefun(void *cookie, const char *buf, int len)
{
    return esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, buf, len, SNIFFER_PROCESS_APPTRACE_TIMEOUT_US) ==
           ESP_OK ? len : -1;
}

static int trace_closefun(void *cookie)
{
    return esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, ESP_APPTRACE_TMO_INFINITE) == ESP_OK ? 0 : -1;
}

void pcap_flush_apptrace_timer_cb(TimerHandle_t pxTimer)
{
    esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, pdMS_TO_TICKS(10));
}
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG

static esp_err_t pcap_close(pcap_cmd_runtime_t *pcap)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(pcap->is_opened, ESP_ERR_INVALID_STATE, err, CMD_PCAP_TAG, ".pcap file is already closed");
    ESP_GOTO_ON_ERROR(pcap_del_session(pcap->pcap_handle) != ESP_OK, err, CMD_PCAP_TAG, "stop pcap session failed");
    pcap->is_opened = false;
    pcap->link_type_set = false;
    pcap->pcap_handle = NULL;
#if CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY
    free(pcap->pcap_buffer.buffer);
    pcap->pcap_buffer.buffer_size = 0;
    ESP_LOGI(CMD_PCAP_TAG, "free memory successfully");
#endif
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    if (pcap->trace_flush_timer != NULL) {
        xTimerDelete(pcap->trace_flush_timer, pdMS_TO_TICKS(100));
        pcap->trace_flush_timer = NULL;
    }
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
err:
    return ret;
}

static esp_err_t pcap_open(pcap_cmd_runtime_t *pcap)
{
    esp_err_t ret = ESP_OK;
    /* Create file to write, binary format */
    FILE *fp = NULL;
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    fp = funopen("trace", NULL, trace_writefun, NULL, trace_closefun);
#elif CONFIG_SNIFFER_PCAP_DESTINATION_SD
    fp = fopen(pcap->filename, "wb+");
#elif CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY
    pcap->pcap_buffer.buffer = calloc(PCAP_MEMORY_BUFFER_SIZE, sizeof(char));
    ESP_GOTO_ON_FALSE(pcap->pcap_buffer.buffer, ESP_ERR_NO_MEM, err, CMD_PCAP_TAG, "pcap buffer calloc failed");
    fp = fmemopen(pcap->pcap_buffer.buffer, PCAP_MEMORY_BUFFER_SIZE, "wb+");
#else
#error "pcap file destination hasn't specified"
#endif
    ESP_GOTO_ON_FALSE(fp, ESP_FAIL, err, CMD_PCAP_TAG, "open file failed");
    pcap_config_t pcap_config = {
        .fp = fp,
        .major_version = PCAP_DEFAULT_VERSION_MAJOR,
        .minor_version = PCAP_DEFAULT_VERSION_MINOR,
        .time_zone = PCAP_DEFAULT_TIME_ZONE_GMT,
    };
    ESP_GOTO_ON_ERROR(pcap_new_session(&pcap_config, &pcap_cmd_rt.pcap_handle), err, CMD_PCAP_TAG, "pcap init failed");
    pcap->is_opened = true;
    ESP_LOGI(CMD_PCAP_TAG, "open file successfully");
    return ret;
err:
    if (fp) {
        fclose(fp);
    }
    return ret;
}

esp_err_t packet_capture(void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds)
{
    return pcap_capture_packet(pcap_cmd_rt.pcap_handle, payload, length, seconds, microseconds);
}

esp_err_t sniff_packet_start(pcap_link_type_t link_type)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    uint32_t retry = 0;
    /* wait until apptrace communication established or timeout */
    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX) && (retry < SNIFFER_APPTRACE_RETRY)) {
        retry++;
        ESP_LOGW(CMD_PCAP_TAG, "waiting for apptrace established");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_GOTO_ON_FALSE(retry < SNIFFER_APPTRACE_RETRY, ESP_ERR_TIMEOUT, err, CMD_PCAP_TAG, "waiting for apptrace established timeout");

    pcap_open(&pcap_cmd_rt);
#endif //CONFIG_SNIFFER_PCAP_DESTINATION_JTAG

    ESP_GOTO_ON_FALSE(pcap_cmd_rt.is_opened, ESP_ERR_INVALID_STATE, err, CMD_PCAP_TAG, "no .pcap file stream is open");
    if (pcap_cmd_rt.link_type_set) {
        ESP_GOTO_ON_FALSE(link_type == pcap_cmd_rt.link_type, ESP_ERR_INVALID_STATE, err, CMD_PCAP_TAG, "link type error");
        ESP_GOTO_ON_FALSE(!pcap_cmd_rt.is_writing, ESP_ERR_INVALID_STATE, err, CMD_PCAP_TAG, "still sniffing");
    } else {
        pcap_cmd_rt.link_type = link_type;
        /* Create file to write, binary format */
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
        /* Ethernet Link layer traffic amount may be much less than on Wi-Fi (no link management msgs.) and trace data is sent to listener
        only after filling trace buffer. Hence the trace buffer might not be filled prior listener's timeout. This condition is resolved by
        flushing the trace buffer periodically. */
        if (link_type == PCAP_LINK_TYPE_ETHERNET) {
            int timer_id = 0xFEED;
            pcap_cmd_rt.trace_flush_timer = xTimerCreate("flush_apptrace_timer",
                                        pdMS_TO_TICKS(TRACE_TIMER_FLUSH_INT_MS),
                                        pdTRUE, (void *) timer_id,
                                        pcap_flush_apptrace_timer_cb);
            ESP_GOTO_ON_FALSE(pcap_cmd_rt.trace_flush_timer, ESP_FAIL, err, CMD_PCAP_TAG, "pcap xTimerCreate failed");
            ESP_GOTO_ON_FALSE(xTimerStart(pcap_cmd_rt.trace_flush_timer, 0), ESP_FAIL, err_timer_start, CMD_PCAP_TAG, "pcap xTimerStart failed");
        }
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
        pcap_write_header(pcap_cmd_rt.pcap_handle, link_type);
        pcap_cmd_rt.link_type_set = true;
    }
    pcap_cmd_rt.is_writing = true;
    return ret;

#ifdef CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
err_timer_start:
    xTimerDelete(pcap_cmd_rt.trace_flush_timer, pdMS_TO_TICKS(100));
    pcap_cmd_rt.trace_flush_timer = NULL;
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
err:
    return ret;
}

esp_err_t sniff_packet_stop(void)
{
    pcap_cmd_rt.is_writing = false;
#if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    pcap_close(&pcap_cmd_rt);
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    return ESP_OK;
}

#if !CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
static struct {
    struct arg_str *file;
    struct arg_lit *open;
    struct arg_lit *close;
    struct arg_lit *summary;
    struct arg_end *end;
} pcap_args;

static int do_pcap_cmd(int argc, char **argv)
{
    int ret = 0;
    int nerrors = arg_parse(argc, argv, (void **)&pcap_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, pcap_args.end, argv[0]);
        return 1;
    }

    /* Check whether or not to close pcap file: "--close" option */
    if (pcap_args.close->count) {
        /* close the pcap file */
        ESP_GOTO_ON_FALSE(!(pcap_cmd_rt.is_writing), ESP_FAIL, err, CMD_PCAP_TAG, "still sniffing, file will not close");
        pcap_close(&pcap_cmd_rt);
        ESP_LOGI(CMD_PCAP_TAG, ".pcap file close done");
        return ret;
    }

#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
    /* set pcap file name: "-f" option */
    int len = snprintf(pcap_cmd_rt.filename, sizeof(pcap_cmd_rt.filename), "%s/%s.pcap", CONFIG_SNIFFER_MOUNT_POINT, pcap_args.file->sval[0]);
    if (len >= sizeof(pcap_cmd_rt.filename)) {
        ESP_LOGW(CMD_PCAP_TAG, "pcap file name too long, try to enlarge memory in menuconfig");
    }

    /* Check if needs to be parsed and shown: "--summary" option */
    if (pcap_args.summary->count) {
        ESP_LOGI(CMD_PCAP_TAG, "%s is to be parsed", pcap_cmd_rt.filename);
        if (pcap_cmd_rt.is_opened) {
            ESP_GOTO_ON_FALSE(!(pcap_cmd_rt.is_writing), ESP_FAIL, err, CMD_PCAP_TAG, "still writing");
            ESP_GOTO_ON_ERROR(pcap_print_summary(pcap_cmd_rt.pcap_handle, stdout), err, CMD_PCAP_TAG, "pcap print summary failed");
        } else {
            FILE *fp;
            fp = fopen(pcap_cmd_rt.filename, "rb");
            ESP_GOTO_ON_FALSE(fp, ESP_FAIL, err, CMD_PCAP_TAG, "open file failed");
            pcap_config_t pcap_config = {
                .fp = fp,
                .major_version = PCAP_DEFAULT_VERSION_MAJOR,
                .minor_version = PCAP_DEFAULT_VERSION_MINOR,
                .time_zone = PCAP_DEFAULT_TIME_ZONE_GMT,
            };
            ESP_GOTO_ON_ERROR(pcap_new_session(&pcap_config, &pcap_cmd_rt.pcap_handle), err, CMD_PCAP_TAG, "pcap init failed");
            ESP_GOTO_ON_ERROR(pcap_print_summary(pcap_cmd_rt.pcap_handle, stdout), err, CMD_PCAP_TAG, "pcap print summary failed");
            ESP_GOTO_ON_ERROR(pcap_del_session(pcap_cmd_rt.pcap_handle), err, CMD_PCAP_TAG, "stop pcap session failed");
        }
    }
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_SD

#if CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY
    /* Check if needs to be parsed and shown: "--summary" option */
    if (pcap_args.summary->count) {
        ESP_LOGI(CMD_PCAP_TAG, "Memory is to be parsed");
        ESP_GOTO_ON_ERROR(pcap_print_summary(pcap_cmd_rt.pcap_handle, stdout), err, CMD_PCAP_TAG, "pcap print summary failed");
    }
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_MEMORY

    if (pcap_args.open->count) {
        pcap_open(&pcap_cmd_rt);
    }
err:
    return ret;
}
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_JTAG

void register_pcap_cmd(void)
{
#if !CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
    pcap_args.summary = arg_lit0(NULL, "summary", "option to parse and show the summary of .pcap file");
    pcap_args.file = arg_str1("f", "file", "<file>",
                              "name of the file storing the packets in pcap format");
    pcap_args.close = arg_lit0(NULL, "close", "close .pcap file");
    pcap_args.open = arg_lit0(NULL, "open", "open .pcap file");
    pcap_args.end = arg_end(1);
    const esp_console_cmd_t pcap_cmd = {
        .command = "pcap",
        .help = "Save and parse pcap file",
        .hint = NULL,
        .func = &do_pcap_cmd,
        .argtable = &pcap_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&pcap_cmd));
#endif // #if CONFIG_SNIFFER_PCAP_DESTINATION_JTAG
}
