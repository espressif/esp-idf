/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "sys/param.h"
#include "esp_log.h"
#include "jpeg_private.h"
#include "private/jpeg_param.h"
#include "private/jpeg_emit_marker.h"
#include "hal/jpeg_defs.h"
#include "esp_private/esp_cache_private.h"

static void emit_byte(jpeg_enc_header_info_t *header_info, uint8_t i)
{
    header_info->header_buf[header_info->header_len] = i;
    header_info->header_len = header_info->header_len + 1;
}

static void emit_word(jpeg_enc_header_info_t *header_info, uint16_t i)
{
    emit_byte(header_info, i >> 8);
    emit_byte(header_info, i & 0xFF);
}

static void emit_marker(jpeg_enc_header_info_t *header_info, uint8_t marker)
{
    emit_byte(header_info, 0xFF);
    emit_byte(header_info, marker);
}

static void emit_dht(jpeg_enc_header_info_t *header_info, uint8_t *bits, uint8_t *val, int index, bool ac_flag)
{
    emit_marker(header_info, JPEG_M_DHT & 0xff);
    int length = 0;

    for (int i = 0; i < 16; i++) {
        length += bits[i];
    }

    emit_word(header_info, length + 2 + 1 + 16);
    emit_byte(header_info, index + (ac_flag << 4));

    for (int i = 0; i < 16; i++) {
        emit_byte(header_info, bits[i]);
    }

    for (int i = 0; i < length; i++) {
        emit_byte(header_info, val[i]);
    }
}

static void compute_quant_table(uint32_t *quant_table, const uint32_t *basic_table, uint32_t quality)
{
    int scaling_factor = 0;
    if (quality < 50) {
        scaling_factor = 5000 / quality;
    } else {
        scaling_factor = 200 - quality * 2;
    }
    for (int i = 0; i < 64; i++) {
        int temp = *basic_table++;
        temp = (temp * scaling_factor + 50L) / 100L;
        *quant_table++ = MIN(MAX(temp, 1), 255);
    }
}

esp_err_t emit_soi_marker(jpeg_enc_header_info_t *header_info)
{
    emit_marker(header_info, JPEG_M_SOI & 0xff);
    return ESP_OK;
}

esp_err_t emit_app0_marker(jpeg_enc_header_info_t *header_info)
{
    emit_marker(header_info, JPEG_M_APP0 & 0xff);
    emit_word(header_info, 2 + 4 + 1 + 2 + 1 + 2 + 2 + 1 + 1);
    // 0x4A46494600 for JIF0
    emit_byte(header_info, 0x4A);
    emit_byte(header_info, 0x46);
    emit_byte(header_info, 0x49);
    emit_byte(header_info, 0x46);
    emit_byte(header_info, 0x00);

    // Major version
    emit_byte(header_info, 1);
    // Minor version
    emit_byte(header_info, 1);
    // Density unit (0: no unit, 1: inch 2: cm)
    emit_byte(header_info, 0);
    // X direction density
    emit_word(header_info, 1);
    // Y direction density
    emit_word(header_info, 1);
    // No thumbnail image
    emit_byte(header_info, 0);
    emit_byte(header_info, 0);
    return ESP_OK;
}

esp_err_t emit_dqt_marker(jpeg_enc_header_info_t *header_info)
{
    compute_quant_table(header_info->m_quantization_tables[0], luminance_quantization_table, header_info->quality);
    compute_quant_table(header_info->m_quantization_tables[1], chrominance_quantization_table, header_info->quality);
    for (int i = 0; i < ((header_info->num_components == 3) ? 2 : 1); i++) {
        emit_marker(header_info, JPEG_M_DQT & 0xff);
        emit_word(header_info, 64 + 1 + 2);
        emit_byte(header_info, (i));

        for (int j = 0; j < 64; j++) {
            emit_byte(header_info, (uint8_t)(header_info->m_quantization_tables[i][zigzag_arr[j]]));
        }
    }

    return ESP_OK;
}

esp_err_t emit_sof_marker(jpeg_enc_header_info_t *header_info)
{
    uint8_t comp_h_samp[3] = {0};
    uint8_t comp_v_samp[3] = {0};
    switch (header_info->sub_sample) {
    case JPEG_DOWN_SAMPLING_YUV444: {
        comp_h_samp[0] = 1;
        comp_v_samp[0] = 1;
        comp_h_samp[1] = 1;
        comp_v_samp[1] = 1;
        comp_h_samp[2] = 1;
        comp_v_samp[2] = 1;
        break;
    }
    case JPEG_DOWN_SAMPLING_YUV422: {
        comp_h_samp[0] = 2;
        comp_v_samp[0] = 1;
        comp_h_samp[1] = 1;
        comp_v_samp[1] = 1;
        comp_h_samp[2] = 1;
        comp_v_samp[2] = 1;
        break;
    }
    case JPEG_DOWN_SAMPLING_YUV420: {
        comp_h_samp[0] = 2;
        comp_v_samp[0] = 2;
        comp_h_samp[1] = 1;
        comp_v_samp[1] = 1;
        comp_h_samp[2] = 1;
        comp_v_samp[2] = 1;
        break;
    }
    case JPEG_DOWN_SAMPLING_GRAY: {
        comp_h_samp[0] = 1;
        comp_v_samp[0] = 1;
        comp_h_samp[1] = 1;
        comp_v_samp[1] = 1;
        comp_h_samp[2] = 1;
        comp_v_samp[2] = 1;
        break;
    }
    default:
        break;
    }

    emit_marker(header_info, JPEG_M_SOF0 & 0xff); /* baseline */
    emit_word(header_info, 3 * header_info->num_components + 2 + 5 + 1);
    emit_byte(header_info, 8); /* precision */
    emit_word(header_info, header_info->origin_v);
    emit_word(header_info, header_info->origin_h);
    emit_byte(header_info, header_info->num_components);

    for (int i = 0; i < header_info->num_components; i++) {
        emit_byte(header_info, (i + 1));
        emit_byte(header_info, (comp_h_samp[i] << 4) + comp_v_samp[i]);
        emit_byte(header_info, (i > 0));
    }
    return ESP_OK;
}

esp_err_t emit_dht_marker(jpeg_enc_header_info_t *header_info)
{
    uint8_t m_huff_bits[2][2][JPEG_HUFFMAN_BITS_LEN_TABLE_LEN] = {0};
    uint8_t m_huff_val[2][2][JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {0};
    memcpy(m_huff_bits[0][0], luminance_dc_coefficients, JPEG_HUFFMAN_DC_VALUE_TABLE_LEN);
    memcpy(m_huff_val[0][0], luminance_dc_values, JPEG_HUFFMAN_DC_VALUE_TABLE_LEN);
    memcpy(m_huff_bits[0][1], luminance_ac_coefficients, JPEG_HUFFMAN_DC_VALUE_TABLE_LEN);
    memcpy(m_huff_val[0][1], luminance_ac_values, JPEG_HUFFMAN_AC_VALUE_TABLE_LEN);
    memcpy(m_huff_bits[1][0], chrominance_dc_coefficients, JPEG_HUFFMAN_DC_VALUE_TABLE_LEN);
    memcpy(m_huff_val[1][0], chrominance_dc_values, JPEG_HUFFMAN_DC_VALUE_TABLE_LEN);
    memcpy(m_huff_bits[1][1], chrominance_ac_coefficients, JPEG_HUFFMAN_DC_VALUE_TABLE_LEN);
    memcpy(m_huff_val[1][1], chrominance_ac_values, JPEG_HUFFMAN_AC_VALUE_TABLE_LEN);

    emit_dht(header_info, m_huff_bits[0][0], m_huff_val[0][0], 0, false);
    emit_dht(header_info, m_huff_bits[0][1], m_huff_val[0][1], 0, true);

    if (header_info->num_components == 3) {
        emit_dht(header_info, m_huff_bits[1][0], m_huff_val[1][0], 1, false);
        emit_dht(header_info, m_huff_bits[1][1], m_huff_val[1][1], 1, true);
    }

    return ESP_OK;
}

esp_err_t emit_sos_marker(jpeg_enc_header_info_t *header_info)
{
    emit_marker(header_info, JPEG_M_SOS & 0xff);
    emit_word(header_info, 2 * header_info->num_components + 2 + 1 + 3);
    emit_byte(header_info, header_info->num_components);

    for (int i = 0; i < header_info->num_components; i++) {
        emit_byte(header_info, i + 1);

        if (i == 0) {
            emit_byte(header_info, (0 << 4) + 0);
        } else {
            emit_byte(header_info, (1 << 4) + 1);
        }
    }

    emit_byte(header_info, 0); /* spectral selection */
    emit_byte(header_info, 63);
    emit_byte(header_info, 0);
    return ESP_OK;
}

esp_err_t emit_com_marker(jpeg_enc_header_info_t *header_info)
{
    // Calculate how many bytes should be compensate to make it byte aligned.
    size_t cache_align = 0;
    esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_align);
    // compensate_size = aligned_size - SOS marker size(2 * header_info->num_components + 2 + 1 + 3 + 2) - COM marker size(4).
    int compensate_size = ((header_info->header_len / cache_align + 1) * cache_align) - header_info->header_len - (2 * header_info->num_components + 2 + 1 + 3 + 2) - 4;
    if (compensate_size < 0) {
        compensate_size += cache_align;
    }
    emit_marker(header_info, JPEG_M_COM & 0xff);
    emit_word(header_info, compensate_size);
    for (int i = 0; i < compensate_size; i++) {
        emit_byte(header_info, 0);
    }

    return ESP_OK;
}
