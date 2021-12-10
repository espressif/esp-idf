/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "pcap.h"

static const char *TAG = "pcap";

#define PCAP_MAGIC_BIG_ENDIAN 0xA1B2C3D4    /*!< Big-Endian */
#define PCAP_MAGIC_LITTLE_ENDIAN 0xD4C3B2A1 /*!< Little-Endian */

typedef struct pcap_file_t pcap_file_t;

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
struct pcap_file_t {
    FILE *file;                 /*!< File handle */
    pcap_link_type_t link_type; /*!< Pcap Link Type */
    unsigned int major_version; /*!< Pcap version: major */
    unsigned int minor_version; /*!< Pcap version: minor */
    unsigned int time_zone;     /*!< Pcap timezone code */
    uint32_t endian_magic;      /*!< Magic value related to endian format */
};

esp_err_t pcap_new_session(const pcap_config_t *config, pcap_file_handle_t *ret_pcap)
{
    esp_err_t ret = ESP_OK;
    pcap_file_t *pcap = NULL;
    ESP_GOTO_ON_FALSE(config && ret_pcap, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->fp, ESP_ERR_INVALID_ARG, err, TAG, "pcap file handle can't be NULL");
    pcap = calloc(1, sizeof(pcap_file_t));
    ESP_GOTO_ON_FALSE(pcap, ESP_ERR_NO_MEM, err, TAG, "no mem for pcap file object");
    pcap->file = config->fp;
    pcap->major_version = config->major_version;
    pcap->minor_version = config->minor_version;
    pcap->endian_magic = config->flags.little_endian ? PCAP_MAGIC_LITTLE_ENDIAN : PCAP_MAGIC_BIG_ENDIAN;
    pcap->time_zone = config->time_zone;
    *ret_pcap = pcap;
    return ret;
err:
    if (pcap) {
        free(pcap);
    }
    return ret;
}

esp_err_t pcap_del_session(pcap_file_handle_t pcap)
{
    ESP_RETURN_ON_FALSE(pcap, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    if (pcap->file) {
        fclose(pcap->file);
        pcap->file = NULL;
    }
    free(pcap);
    return ESP_OK;
}

esp_err_t pcap_write_header(pcap_file_handle_t pcap, pcap_link_type_t link_type)
{
    ESP_RETURN_ON_FALSE(pcap, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    /* Write Pcap File header */
    pcap_file_header_t header = {
        .magic = pcap->endian_magic,
        .major = pcap->major_version,
        .minor = pcap->minor_version,
        .zone = pcap->time_zone,
        .sigfigs = 0,
        .snaplen = 0x40000,
        .link_type = link_type,
    };
    size_t real_write = fwrite(&header, sizeof(header), 1, pcap->file);
    ESP_RETURN_ON_FALSE(real_write == 1, ESP_FAIL, TAG, "write pcap file header failed");
    /* Save the link type to pcap file object */
    pcap->link_type = link_type;
    /* Flush content in the buffer into device */
    fflush(pcap->file);
    return ESP_OK;
}

esp_err_t pcap_capture_packet(pcap_file_handle_t pcap, void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds)
{
    ESP_RETURN_ON_FALSE(pcap && payload, ESP_ERR_INVALID_ARG, TAG, "invalid argumnet");
    size_t real_write = 0;
    pcap_packet_header_t header = {
        .seconds = seconds,
        .microseconds = microseconds,
        .capture_length = length,
        .packet_length = length
    };
    real_write = fwrite(&header, sizeof(header), 1, pcap->file);
    ESP_RETURN_ON_FALSE(real_write == 1, ESP_FAIL, TAG, "write packet header failed");
    real_write = fwrite(payload, sizeof(uint8_t), length, pcap->file);
    ESP_RETURN_ON_FALSE(real_write == length, ESP_FAIL, TAG, "write packet payload failed");
    /* Flush content in the buffer into device */
    fflush(pcap->file);
    return ESP_OK;
}

esp_err_t pcap_print_summary(pcap_file_handle_t pcap, FILE *print_file)
{
    esp_err_t ret = ESP_OK;
    long size = 0;
    char *packet_payload = NULL;
    ESP_RETURN_ON_FALSE(pcap && print_file, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // get file size
    fseek(pcap->file, 0L, SEEK_END);
    size = ftell(pcap->file);
    fseek(pcap->file, 0L, SEEK_SET);
    // file empty is allowed, so return ESP_OK
    ESP_RETURN_ON_FALSE(size, ESP_OK, TAG, "pcap file is empty");
    // packet index (by bytes)
    uint32_t index = 0;
    pcap_file_header_t file_header;
    size_t real_read = fread(&file_header, sizeof(pcap_file_header_t), 1, pcap->file);
    ESP_RETURN_ON_FALSE(real_read == 1, ESP_FAIL, TAG, "read pcap file header failed");
    index += sizeof(pcap_file_header_t);
    //print pcap header information
    fprintf(print_file, "------------------------------------------------------------------------\n");
    fprintf(print_file, "Pcap packet Head:\n");
    fprintf(print_file, "------------------------------------------------------------------------\n");
    fprintf(print_file, "Magic Number: %x\n", file_header.magic);
    fprintf(print_file, "Major Version: %d\n", file_header.major);
    fprintf(print_file, "Minor Version: %d\n", file_header.minor);
    fprintf(print_file, "SnapLen: %d\n", file_header.snaplen);
    fprintf(print_file, "LinkType: %d\n", file_header.link_type);
    fprintf(print_file, "------------------------------------------------------------------------\n");
    uint32_t packet_num = 0;
    pcap_packet_header_t packet_header;
    while (index < size) {
        real_read = fread(&packet_header, sizeof(pcap_packet_header_t), 1, pcap->file);
        ESP_GOTO_ON_FALSE(real_read == 1, ESP_FAIL, err, TAG, "read pcap packet header failed");
        // print packet header information
        fprintf(print_file, "Packet %d:\n", packet_num);
        fprintf(print_file, "Timestamp (Seconds): %d\n", packet_header.seconds);
        fprintf(print_file, "Timestamp (Microseconds): %d\n", packet_header.microseconds);
        fprintf(print_file, "Capture Length: %d\n", packet_header.capture_length);
        fprintf(print_file, "Packet Length: %d\n", packet_header.packet_length);
        size_t payload_length = packet_header.capture_length;
        packet_payload = malloc(payload_length);
        ESP_GOTO_ON_FALSE(packet_payload, ESP_ERR_NO_MEM, err, TAG, "no mem to save packet payload");
        real_read = fread(packet_payload, payload_length, 1, pcap->file);
        ESP_GOTO_ON_FALSE(real_read == 1, ESP_FAIL, err, TAG, "read payload error");
        // print packet information
        if (file_header.link_type == PCAP_LINK_TYPE_802_11) {
            // Frame Control Field is coded as LSB first
            fprintf(print_file, "Frame Type: %2x\n", (packet_payload[0] >> 2) & 0x03);
            fprintf(print_file, "Frame Subtype: %2x\n", (packet_payload[0] >> 4) & 0x0F);
            fprintf(print_file, "Destination: ");
            for (int j = 0; j < 5; j++) {
                fprintf(print_file, "%2x ", packet_payload[4 + j]);
            }
            fprintf(print_file, "%2x\n", packet_payload[9]);
            fprintf(print_file, "Source: ");
            for (int j = 0; j < 5; j++) {
                fprintf(print_file, "%2x ", packet_payload[10 + j]);
            }
            fprintf(print_file, "%2x\n", packet_payload[15]);
            fprintf(print_file, "------------------------------------------------------------------------\n");
        } else if (file_header.link_type == PCAP_LINK_TYPE_ETHERNET){
            fprintf(print_file, "Destination: ");
            for (int j = 0; j < 5; j++) {
                fprintf(print_file, "%2x ", packet_payload[j]);
            }
            fprintf(print_file, "%2x\n", packet_payload[5]);
            fprintf(print_file, "Source: ");
            for (int j = 0; j < 5; j++) {
                fprintf(print_file, "%2x ", packet_payload[6 + j]);
            }
            fprintf(print_file, "%2x\n", packet_payload[11]);
            fprintf(print_file, "Type: 0x%x\n", packet_payload[13] | (packet_payload[12] << 8));
            fprintf(print_file, "------------------------------------------------------------------------\n");
        }
        else {
            fprintf(print_file, "Unknown link type:%d\n", file_header.link_type);
            fprintf(print_file, "------------------------------------------------------------------------\n");
        }
        free(packet_payload);
        packet_payload = NULL;
        index += packet_header.capture_length + sizeof(pcap_packet_header_t);
        packet_num ++;
    }
    fprintf(print_file, "Pcap packet Number: %d\n", packet_num);
    fprintf(print_file, "------------------------------------------------------------------------\n");
    return ret;
err:
    if (packet_payload) {
        free(packet_payload);
    }
    return ret;
}
