// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "pcap.h"

static const char *PCAP_TAG = "pcap";
#define PCAP_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(PCAP_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

/**
 * @brief Pcap File Header
 *
 */
typedef struct {
    uint32_t magic;     /*!< Magic Number */
    uint16_t major;     /*!< Major Version */
    uint16_t minor;     /*!< Minor Version */
    uint32_t zone;      /*!< Time Zone Offset */
    uint32_t sigfigs;   /*!< Timestamp Accuracy */
    uint32_t snaplen;   /*!< Max Length to Capture */
    uint32_t link_type; /*!< Link Layer Type */
} pcap_file_header_t;

/**
 * @brief Pcap Packet Header
 *
 */
typedef struct {
    uint32_t seconds;        /*!< Number of seconds since January 1st, 1970, 00:00:00 GMT */
    uint32_t microseconds;   /*!< Number of microseconds when the packet was captured (offset from seconds) */
    uint32_t capture_length; /*!< Number of bytes of captured data, no longer than packet_length */
    uint32_t packet_length;  /*!< Actual length of current packet */
} pcap_packet_header_t;

/**
 * @brief Pcap Runtime Handle
 *
 */
typedef struct {
    FILE *file; /*!< File handle */
} pcap_runtime_t;

esp_err_t pcap_capture_packet(pcap_handle_t handle, void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds)
{
    PCAP_CHECK(handle, "pcap handle is NULL", err);
    pcap_runtime_t *pcap_rt = (pcap_runtime_t *)handle;
    PCAP_CHECK(pcap_rt->file, "pcap file is NULL", err);
    size_t real_write = 0;
    pcap_packet_header_t header = {
        .seconds = seconds,
        .microseconds = microseconds,
        .capture_length = length,
        .packet_length = length
    };
    real_write = fwrite(&header, sizeof(header), 1, pcap_rt->file);
    PCAP_CHECK(real_write == 1, "write packet header error", err);
    real_write = fwrite(payload, sizeof(uint8_t), length, pcap_rt->file);
    PCAP_CHECK(real_write == length, "write packet payload error", err);
    /* Flush content in the buffer into device */
    fflush(pcap_rt->file);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t pcap_deinit(pcap_handle_t handle)
{
    PCAP_CHECK(handle, "pcap handle is NULL", err);
    pcap_runtime_t *pcap_rt = (pcap_runtime_t *)handle;
    PCAP_CHECK(pcap_rt->file, "pcap file is NULL", err);
    PCAP_CHECK(fclose(pcap_rt->file) == 0, "close pcap file failed", err);
    pcap_rt->file = NULL;
    free(pcap_rt);
    ESP_LOGD(PCAP_TAG, "pcap deinit OK");
    return ESP_OK;
err:
    ESP_LOGW(PCAP_TAG, "pcap deinit failed");
    return ESP_FAIL;
}

esp_err_t pcap_init(pcap_config_t *config, pcap_handle_t *handle)
{
    PCAP_CHECK(config, "config is NULL", err);
    PCAP_CHECK(handle, "pcap handle is NULL", err);
    pcap_runtime_t *pcap_rt = calloc(sizeof(pcap_runtime_t), 1);
    PCAP_CHECK(pcap_rt, "calloc pcap runtime failed", err);
    pcap_rt->file = config->fp;
    /* Write Pcap File header */
    pcap_file_header_t header = {
        .magic = PCAP_MAGIC_BIG_ENDIAN,
        .major = PCAP_VERSION_MAJOR,
        .minor = PCAP_VERSION_MINOR,
        .zone = PCAP_TIME_ZONE_GMT,
        .sigfigs = 0,
        .snaplen = 0x40000,
        .link_type = config->link_type
    };
    size_t real_write = fwrite(&header, sizeof(header), 1, pcap_rt->file);
    PCAP_CHECK(real_write == 1, "write pcap file header failed", err_write);
    /* Flush content in the buffer into device */
    fflush(pcap_rt->file);
    *handle = (pcap_handle_t)pcap_rt;
    ESP_LOGD(PCAP_TAG, "pcap init OK");
    return ESP_OK;
    /* Error Handling */
err_write:
    fclose(pcap_rt->file);
    pcap_rt->file = NULL;
    free(pcap_rt);
err:
    ESP_LOGW(PCAP_TAG, "pcap init failed");
    return ESP_FAIL;
}
