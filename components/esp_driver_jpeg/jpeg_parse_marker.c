/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
    // Check if there are bytes left to read before decrementing buffer_left
    if (header_info->buffer_left == 0) {
        ESP_LOGE(TAG, "Buffer underflow detected in jpeg_get_char: no more bytes left to read");
        return 0;
    }
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
    // Guarantee the 2-byte length field is in-buffer before reading it.
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "APPn marker truncated: missing length");
    uint16_t skip_num = jpeg_get_bytes(header_info, 2);
    ESP_RETURN_ON_FALSE(skip_num >= 2, ESP_ERR_INVALID_ARG, TAG, "Invalid APPn marker length: %"PRIu16, skip_num);
    uint16_t bytes_to_skip = skip_num - 2;
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= bytes_to_skip, ESP_ERR_INVALID_ARG, TAG, "APPn marker data underflow for buffer_left: %"PRIu32, header_info->buffer_left);
    header_info->buffer_offset += bytes_to_skip;
    header_info->header_size += bytes_to_skip;
    header_info->buffer_left -= bytes_to_skip;

    return ESP_OK;
}

esp_err_t jpeg_parse_com_marker(jpeg_dec_header_info_t *header_info)
{
    // Guarantee the 2-byte length field is in-buffer before reading it.
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "COM marker truncated: missing length");
    uint16_t skip_num = jpeg_get_bytes(header_info, 2);
    ESP_RETURN_ON_FALSE(skip_num >= 2, ESP_ERR_INVALID_ARG, TAG, "Invalid COM marker length: %"PRIu16, skip_num);
    uint32_t bytes_to_skip = skip_num - 2;
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= bytes_to_skip, ESP_ERR_INVALID_ARG, TAG, "COM marker data underflow for buffer_left: %"PRIu32, header_info->buffer_left);
    header_info->buffer_offset += bytes_to_skip;
    header_info->header_size += bytes_to_skip;
    header_info->buffer_left -= bytes_to_skip;
    return ESP_OK;
}

esp_err_t jpeg_parse_dqt_marker(jpeg_dec_header_info_t *header_info)
{
    uint32_t n = 0, i = 0, prec = 0;
    uint32_t temp = 0;

    // Guarantee the 2-byte length field is in-buffer before reading it.
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "DQT marker truncated: missing length");
    uint16_t length_num = jpeg_get_bytes(header_info, 2);
    ESP_RETURN_ON_FALSE(length_num >= 2, ESP_ERR_INVALID_ARG, TAG, "Invalid DQT marker length: %"PRIu16, length_num);
    length_num -= 2;
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= length_num, ESP_ERR_INVALID_ARG, TAG, "DQT marker truncated: buffer_left=%"PRIu32" needed=%"PRIu16, header_info->buffer_left, length_num);

    while (length_num) {
        // Need at least 1 byte for the table identifier (Pq | Tq)
        ESP_RETURN_ON_FALSE(length_num >= 1, ESP_ERR_INVALID_ARG, TAG, "DQT marker length error before reading id");
        n = jpeg_get_bytes(header_info, 1);
        length_num -= 1;
        prec = n >> 4;
        n &= 0x0F;

        ESP_RETURN_ON_FALSE(n < JPEG_COMPONENT_NUMBER_MAX, ESP_ERR_INVALID_ARG, TAG, "DQT marker: invalid quantization table id %"PRIu32, n);
        // Pq must be 0 (8-bit) or 1 (16-bit); other values are reserved and unsupported
        ESP_RETURN_ON_FALSE(prec <= 1, ESP_ERR_INVALID_ARG, TAG, "DQT marker: invalid precision %"PRIu32, prec);

        // Each table needs 64 * (prec+1) bytes; verify upfront before consuming
        const uint32_t entry_bytes = 64 * (prec + 1);
        ESP_RETURN_ON_FALSE(length_num >= entry_bytes, ESP_ERR_INVALID_ARG, TAG, "DQT marker truncated: need %"PRIu32" remaining %"PRIu16, entry_bytes, length_num);

        // Read quantization entries, in zig-zag order
        for (i = 0; i < 64; i++) {
            temp = jpeg_get_bytes(header_info, 1);
            length_num -= 1;
            if (prec) {
                temp = (temp << 8) + jpeg_get_bytes(header_info, 1);
                length_num -= 1;
            }
            header_info->qt_tbl[n][zigzag_arr[i]] = temp;
        }

        // Only count distinct Tq IDs; a repeated ID (in this segment or an
        // earlier DQT segment) rewrites qt_tbl[n] but must not advance
        // qt_tbl_num past the number of populated slots.
        const uint8_t tq_bit = (uint8_t)(1 << n);
        if ((header_info->qt_tbl_seen_mask & tq_bit) == 0) {
            ESP_RETURN_ON_FALSE(header_info->qt_tbl_num < JPEG_COMPONENT_NUMBER_MAX, ESP_ERR_INVALID_ARG, TAG, "DQT marker: too many quantization tables");
            header_info->qt_tbl_seen_mask |= tq_bit;
            header_info->qt_tbl_num++;
        }
    }

    return ESP_OK;

}

esp_err_t jpeg_parse_sof_marker(jpeg_dec_header_info_t *header_info)
{
    // SOF segment layout (excluding the 0xFFCx marker bytes):
    //   Lf(2) + P(1) + Y(2) + X(2) + Nf(1) + Nf * [Ci(1) + Hi|Vi(1) + Tqi(1)]
    // Guarantee the 2-byte length field is in-buffer before reading it.
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "SOF marker truncated: missing length");
    uint16_t lf = jpeg_get_bytes(header_info, 2);
    ESP_RETURN_ON_FALSE(lf >= 8, ESP_ERR_INVALID_ARG, TAG, "Invalid SOF marker length: %"PRIu16, lf);
    uint16_t remaining = lf - 2;
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= remaining, ESP_ERR_INVALID_ARG, TAG, "SOF marker truncated: buffer_left=%"PRIu32" need=%"PRIu16, header_info->buffer_left, remaining);

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

    // Reject zero dimensions to avoid division-by-zero / degenerate buffers downstream
    ESP_RETURN_ON_FALSE(width != 0 && height != 0, ESP_ERR_INVALID_ARG, TAG, "Invalid picture size %ux%u", (unsigned)width, (unsigned)height);

    if ((width * height % 8) != 0) {
        ESP_LOGE(TAG, "Picture sizes not divisible by 8 are not supported");
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t nf = jpeg_get_bytes(header_info, 1);
    // Hardware supports 1..3 components (gray / YUV). nf must fit JPEG_COMPONENT_NUMBER_MAX-sized arrays.
    if (nf == 0 || nf >= JPEG_COMPONENT_NUMBER_MAX) {
        ESP_LOGE(TAG, "Only frame less than %d (and non-zero) is supported, got %u", JPEG_COMPONENT_NUMBER_MAX, nf);
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_FALSE(lf >= 8 + 3 * nf, ESP_ERR_INVALID_ARG, TAG, "SOF length %"PRIu16" too small for %u components", lf, nf);

    header_info->nf = nf;

    for (int i = 0; i < nf; i++) {
        header_info->ci[i] = jpeg_get_bytes(header_info, 1);
        header_info->hivi[i] = jpeg_get_bytes(header_info, 1);
        header_info->vi[i] = header_info->hivi[i] & 0x0f;
        header_info->hi[i] = (header_info->hivi[i] & 0xf0) >> 4;
        header_info->qtid[i] = jpeg_get_bytes(header_info, 1);
        // qtid selects the quantization table that the hardware will use; bound it.
        ESP_RETURN_ON_FALSE(header_info->qtid[i] < JPEG_COMPONENT_NUMBER_MAX, ESP_ERR_INVALID_ARG, TAG, "SOF: invalid Tq[%d]=%u", i, header_info->qtid[i]);
    }

    // Guard against zero sampling factors before we use them as a divisor below.
    ESP_RETURN_ON_FALSE(header_info->hi[0] != 0 && header_info->vi[0] != 0, ESP_ERR_INVALID_ARG, TAG, "SOF: zero sampling factor H=%u V=%u", header_info->hi[0], header_info->vi[0]);

    // Set MCU block pixel according to factor. (For 3 components, we only use Y factor)
    header_info->mcux = header_info->hi[0] * 8;
    header_info->mcuy = header_info->vi[0] * 8;

    // The vertical and horizontal in process must be divided by mcu block.
    if (header_info->origin_v % header_info->mcuy != 0) {
        header_info->process_v = ((header_info->origin_v / header_info->mcuy) + 1) * header_info->mcuy;
    }
    if (header_info->origin_h % header_info->mcux != 0) {
        header_info->process_h = ((header_info->origin_h / header_info->mcux) + 1) * header_info->mcux;
    }

    return ESP_OK;
}

esp_err_t jpeg_parse_dht_marker(jpeg_dec_header_info_t *header_info)
{
    // Recording num_left in DHT sector, not including length bytes (2 bytes).
    // Guarantee the 2-byte length field is in-buffer before reading it.
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "DHT marker truncated: missing length");
    uint16_t raw_length = jpeg_get_bytes(header_info, 2);
    // Check for integer underflow before subtraction
    ESP_RETURN_ON_FALSE(raw_length >= 2, ESP_ERR_INVALID_ARG, TAG, "Invalid DHT marker length: %"PRIu16, raw_length);
    uint16_t num_left = raw_length - 2;
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= num_left, ESP_ERR_INVALID_ARG, TAG, "DHT marker truncated: buffer_left=%"PRIu32" needed=%"PRIu16, header_info->buffer_left, num_left);

    while (num_left) {
        uint32_t np = 0;
        uint8_t tc = 0;
        uint8_t th = 0;

        // Need 1 byte (Tc | Th) + 16 bytes (Li) at minimum before reading values
        ESP_RETURN_ON_FALSE(num_left >= (1 + JPEG_HUFFMAN_BITS_LEN_TABLE_LEN), ESP_ERR_INVALID_ARG, TAG, "DHT marker truncated: num_left=%"PRIu16, num_left);

        // Get information of huffman table
        header_info->huffinfo.info = jpeg_get_bytes(header_info, 1);
        tc = header_info->huffinfo.type;
        th = header_info->huffinfo.id;

        ESP_RETURN_ON_FALSE(tc < DHT_TC_NUM, ESP_ERR_INVALID_ARG, TAG, "DHT marker: invalid table class Tc=%u", tc);
        ESP_RETURN_ON_FALSE(th < DHT_TH_NUM, ESP_ERR_INVALID_ARG, TAG, "DHT marker: invalid table id Th=%u", th);

        for (int i = 0; i < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; i++) {
            header_info->huffbits[tc][th][i] = jpeg_get_bytes(header_info, 1);
            // Record number of patterns.
            np += header_info->huffbits[tc][th][i];
        }
        ESP_RETURN_ON_FALSE(np <= JPEG_HUFFMAN_AC_VALUE_TABLE_LEN, ESP_ERR_INVALID_ARG, TAG, "DHT marker: huffcode count %"PRIu32" exceeds table size", np);
        ESP_RETURN_ON_FALSE(num_left >= (1u + JPEG_HUFFMAN_BITS_LEN_TABLE_LEN + np), ESP_ERR_INVALID_ARG, TAG, "DHT marker truncated for huffcode: num_left=%"PRIu16" need=%"PRIu32, num_left, (uint32_t)(1u + JPEG_HUFFMAN_BITS_LEN_TABLE_LEN + np));

        for (uint32_t i = 0; i < np; i++) {
            header_info->huffcode[tc][th][i] = jpeg_get_bytes(header_info, 1);
        }

        num_left -= (1 + JPEG_HUFFMAN_BITS_LEN_TABLE_LEN + np);
    }

    // Record, that Huffman table present in JPEG header
    header_info->dht_marker = true;
    return ESP_OK;
}

esp_err_t jpeg_parse_dri_marker(jpeg_dec_header_info_t *header_info)
{
    // Guarantee the 2-byte length field is in-buffer before reading it.
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "DRI marker truncated: missing length");
    uint16_t lr = jpeg_get_bytes(header_info, 2);
    if (lr != 4) {
        ESP_LOGE(TAG, "DRI marker got but stream length is insufficient, the length you got is %" PRIu16, lr);
        return ESP_ERR_INVALID_SIZE;
    }
    ESP_RETURN_ON_FALSE(header_info->buffer_left >= 2, ESP_ERR_INVALID_ARG, TAG, "DRI marker truncated, buffer_left=%"PRIu32, header_info->buffer_left);
    header_info->ri = jpeg_get_bytes(header_info, 2);
    return ESP_OK;
}

esp_err_t jpeg_parse_sos_marker(jpeg_dec_header_info_t *header_info)
{
    // Got the SOS marker, but need to recover this and feed to 2DDMA.
    ESP_RETURN_ON_FALSE(header_info->header_size >= 2, ESP_ERR_INVALID_ARG, TAG, "SOS marker header_size underflow: %"PRIu32, header_info->header_size);
    header_info->buffer_offset -= 2;
    header_info->header_size -= 2;
    header_info->buffer_left += 2;
    return ESP_OK;
}

esp_err_t jpeg_parse_inv_marker(jpeg_dec_header_info_t *header_info)
{
    // Got invalid 0xFFFF, (followed by a valid marker type)
    // Go one byte back, to skip the first 0xFF
    ESP_RETURN_ON_FALSE(header_info->header_size >= 1, ESP_ERR_INVALID_ARG, TAG, "INV marker header_size underflow: %"PRIu32, header_info->header_size);
    header_info->buffer_offset--;
    header_info->header_size--;
    header_info->buffer_left++;
    return ESP_OK;
}
