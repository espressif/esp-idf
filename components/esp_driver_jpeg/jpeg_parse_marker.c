/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include <math.h>
#include "jpeg_private.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "private/jpeg_param.h"
#include "hal/jpeg_types.h"
#include "esp_check.h"
#include "sdkconfig.h"

static const char *TAG = "jpeg.decoder";

static uint8_t jpeg_get_char(jpeg_dec_header_info_t *header_info)
{
    uint8_t c = header_info->buffer_offset[0];
    header_info->buffer_offset++;
    header_info->header_size++;
    header_info->buffer_left--;
    return c;
}

uint32_t jpeg_get_bytes(jpeg_dec_header_info_t *header_info, uint8_t num_bytes)
{
    uint32_t result = 0;
    for (int i = 0; i < num_bytes; i++) {
        uint8_t byte = jpeg_get_char(header_info);
        result |= (byte << ((num_bytes - i - 1) * 8));
    }
    return result;
}

esp_err_t jpeg_parse_appn_marker(jpeg_dec_header_info_t *header_info)
{
    uint32_t skip_num = jpeg_get_bytes(header_info, 2);
    header_info->buffer_offset += (skip_num - 2);
    header_info->header_size += (skip_num - 2);
    header_info->buffer_left -= (skip_num - 2);

    return ESP_OK;
}

esp_err_t jpeg_parse_com_marker(jpeg_dec_header_info_t *header_info)
{
    uint32_t skip_num = jpeg_get_bytes(header_info, 2);
    header_info->buffer_offset += (skip_num - 2);
    header_info->header_size += (skip_num - 2);
    header_info->buffer_left -= (skip_num - 2);
    return ESP_OK;
}

esp_err_t jpeg_parse_dqt_marker(jpeg_dec_header_info_t *header_info)
{
    uint32_t n = 0, i = 0, prec = 0;
    uint32_t temp = 0;

    uint32_t length_num = jpeg_get_bytes(header_info, 2);
    length_num -= 2;

    while (length_num) {
        n = jpeg_get_bytes(header_info, 1);
        prec = n >> 4;
        n &= 0x0F;
        length_num -= 1;

        // read quantization entries, in zig-zag order
        for (i = 0; i < 64; i++) {
            temp = jpeg_get_bytes(header_info, 1);
            length_num -= 1;
            if (prec) {
                temp = (temp << 8) + jpeg_get_bytes(header_info, 1);
                length_num -= 1;
            }
            header_info->qt_tbl[n][zigzag_arr[i]] = temp;
        }
        header_info->qt_tbl_num++;
    }

    return ESP_OK;

}

esp_err_t jpeg_parse_sof_marker(jpeg_dec_header_info_t *header_info)
{
    jpeg_get_bytes(header_info, 2);
    if (jpeg_get_bytes(header_info, 1) != 8) {
        ESP_LOGE(TAG, "Sample precision is not 8");
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t height = jpeg_get_bytes(header_info, 2);
    header_info->origin_v = height;
    header_info->process_v = height;

    uint16_t width = jpeg_get_bytes(header_info, 2);
    header_info->origin_h = width;
    header_info->process_h = width;

    if ((width * height % 8) != 0) {
        ESP_LOGE(TAG, "Picture sizes not divisible by 8 are not supported");
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t nf = jpeg_get_bytes(header_info, 1);
    if (nf >= 4 || nf == 0) {
        ESP_LOGE(TAG, "Only frame less or equal than 4 is supported.");
        return ESP_ERR_INVALID_STATE;
    }

    header_info->nf = nf;

    for (int i = 0; i < nf; i++) {
        header_info->ci[i] = jpeg_get_bytes(header_info, 1);
        header_info->hivi[i] = jpeg_get_bytes(header_info, 1);
        header_info->vi[i] = header_info->hivi[i] & 0x0f;
        header_info->hi[i] = (header_info->hivi[i] & 0xf0) >> 4;
        header_info->qtid[i] = jpeg_get_bytes(header_info, 1);
    }

    // Set MCU block pixel according to factor. (For 3 components, we only use Y factor)
    header_info->mcux = header_info->hi[0] * 8;
    header_info->mcuy = header_info->vi[0] * 8;

    // The vertical and horizontal in process must be divided by mcu block.
    if (header_info->origin_v % header_info->mcuy != 0) {
        header_info->process_v = (uint32_t)(ceil(header_info->origin_v / header_info->mcuy) + 1) * header_info->mcuy;
    }
    if (header_info->origin_h % header_info->mcux != 0) {
        header_info->process_h = (uint32_t)(ceil(header_info->origin_h / header_info->mcux) + 1) * header_info->mcux;
    }

    return ESP_OK;
}

esp_err_t jpeg_parse_dht_marker(jpeg_dec_header_info_t *header_info)
{
    // Recording num_left in DHT sector, not including length bytes (2 bytes).
    uint32_t num_left = jpeg_get_bytes(header_info, 2) - 2;
    while (num_left) {
        uint32_t np = 0;

        // Get information of huffman table
        header_info->huffinfo.info = jpeg_get_bytes(header_info, 1);

        for (int i = 0; i < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; i++) {
            header_info->huffbits[header_info->huffinfo.type][header_info->huffinfo.id][i] = jpeg_get_bytes(header_info, 1);
            // Record number of patterns.
            np += header_info->huffbits[header_info->huffinfo.type][header_info->huffinfo.id][i];
        }

        for (int i = 0; i < np; i++) {
            header_info->huffcode[header_info->huffinfo.type][header_info->huffinfo.id][i] = jpeg_get_bytes(header_info, 1);
        }

        num_left -= (1 + JPEG_HUFFMAN_BITS_LEN_TABLE_LEN + np);
    }

    return ESP_OK;
}

esp_err_t jpeg_parse_dri_marker(jpeg_dec_header_info_t *header_info)
{
    uint16_t lr = jpeg_get_bytes(header_info, 2);
    if (lr != 4) {
        ESP_LOGE(TAG, "DRI marker got but stream length is insufficient, the length you got is %" PRIu16, lr);
        return ESP_ERR_INVALID_SIZE;
    }
    header_info->ri = jpeg_get_bytes(header_info, 2);
    header_info->dri_marker = true;
    return ESP_OK;
}

esp_err_t jpeg_parse_sos_marker(jpeg_dec_header_info_t *header_info)
{
    // Got the SOS marker, but need to recover this and feed to 2DDMA.
    header_info->buffer_offset -= 2;
    header_info->header_size -= 2;
    header_info->buffer_left += 2;
    return ESP_OK;
}
