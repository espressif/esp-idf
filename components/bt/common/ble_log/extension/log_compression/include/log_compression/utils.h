/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_LOG_COMPRESSION_UTILS_H
#define _BLE_LOG_COMPRESSION_UTILS_H

#include "ble_log.h"
#include <stdio.h>
#include <string.h>

enum {
    BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
    BLE_COMPRESSED_LOG_OUT_SOURCE_MESH,
    BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB,
    BLE_COMPRESSED_LOG_OUT_SOURCE_ISO,
    BLE_COMPRESSED_LOG_OUT_SOURCE_AUDIO_LIB,
};

enum {
    ARG_SIZE_TYPE_U32,     /* argument type with 4 bytes */
    ARG_SIZE_TYPE_STR,     /* argument type with strings */
    ARG_SIZE_TYPE_U64,     /* argument type with 8 bytes */
    ARG_SIZE_TYPE_LZU32,   /* argument type with 4 bytes but with leading zeros */
    ARG_SIZE_TYPE_LZU64,   /* argument type with 8 bytes but with leading zeros */
    ARG_SIZE_TYPE_AZU32,   /* argument type with 4 bytes but all zeros */
    ARG_SIZE_TYPE_AZU64,   /* argument type with 8 bytes but all zeros */
    ARG_SIZE_TYPE_MAX,
};

#define LOG_TYPE_ZERO_ARGS 0
#define LOG_TYPE_HEX_ARGS  1
#define LOG_TYPE_HEX_BUF   2
/* Informational in-band records (protocol v8). Task-id bindings used to
 * be one of these; they are broadcast as INTERNAL frames on the periodic
 * snapshot window now (see ble_log_task_registry.h), keeping the ENCODE
 * stream user records only. */
#define LOG_TYPE_INFO  3

#define LOG_TYPE_INFO_NULL_BUF 1

typedef struct {
    uint8_t *buffer;        /* claim() payload pointer */
    uint16_t idx;           /* bytes written so far */
    uint16_t len;           /* claimed capacity (max_len) */
    uint32_t handle;        /* claim handle for commit/abort */
} ble_cp_log_buffer_mgmt_t;

static inline int ble_log_cp_buffer_safe_check(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint16_t write_len)
{
    if ((pbuf_mgmt->idx + write_len) > pbuf_mgmt->len) {
        printf("Maximum length of buffer(%p) idx %d write_len %d exceed\n", pbuf_mgmt, pbuf_mgmt->idx, write_len);
        return -1;
    }
    return 0;
}

static inline int ble_log_cp_push_u8(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint8_t val)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, 1)) {
        return -1;
    }
    pbuf_mgmt->buffer[pbuf_mgmt->idx] = val;
    pbuf_mgmt->idx++;
    return 0;
}

static inline int ble_log_cp_push_u16(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint16_t val)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, 2)) {
        return -1;
    }
    memcpy(&(pbuf_mgmt->buffer[pbuf_mgmt->idx]), &val, sizeof(val));
    pbuf_mgmt->idx+=2;
    return 0;
}

static inline int ble_log_cp_push_u32(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint32_t val)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, 4)) {
        return -1;
    }
    memcpy(&(pbuf_mgmt->buffer[pbuf_mgmt->idx]), &val, sizeof(val));
    pbuf_mgmt->idx+=4;
    return 0;
}

static inline int ble_log_cp_push_u64(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint64_t val)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, 8)) {
        return -1;
    }
    memcpy(&(pbuf_mgmt->buffer[pbuf_mgmt->idx]), &val, sizeof(val));
    pbuf_mgmt->idx+=8;
    return 0;
}

static inline int ble_log_cp_push_buf(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, const uint8_t *buf, uint16_t len)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, len)) {
        return -1;
    }
    uint8_t *p = (uint8_t *)&(pbuf_mgmt->buffer[pbuf_mgmt->idx]);
    memcpy(p, buf, len);
    pbuf_mgmt->idx+=len;
    return 0;
}

static inline int ble_log_cp_update_half_byte(ble_cp_log_buffer_mgmt_t *pbuf_mgmt,
                                              uint16_t idx, uint8_t new_data, uint8_t high)
{
    if (pbuf_mgmt->idx <= idx) {
        return -1;
    }
    uint8_t old_val = pbuf_mgmt->buffer[idx];
    if (high) {
        pbuf_mgmt->buffer[idx] = (old_val & 0x0f) | (new_data << 4);
    } else {
        pbuf_mgmt->buffer[idx] = (old_val & 0xf0) | (new_data & 0x0f);
    }
    return 0;
}

#endif /* _BLE_LOG_COMPRESSION_UTILS_H */
