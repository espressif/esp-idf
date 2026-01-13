/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_LOG_COMPRESSION_UTILS_H
#define _BLE_LOG_COMPRESSION_UTILS_H

#include "ble_log.h"
#include <stdio.h>

#define CONCAT(a, b) a##b
#define _CONCAT(a, b) CONCAT(a, b)

#define _0 0
#define _1 1
#define _2 2
#define _3 3
#define _4 4
#define _5 5
#define _6 6
#define _7 7
#define _8 8
#define _9 9

#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _n, X...) _n
#define COUNT_ARGS(X...) __COUNT_ARGS(, ##X, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define FOR_EACH_IDX(macro, name, len, ...) \
    _CONCAT(_FOR_EACH_, COUNT_ARGS(__VA_ARGS__))(macro, name, len, __VA_ARGS__)

#define _FOR_EACH_0(m, n, l, ...)
#define _FOR_EACH_1(m, n, l, i1, ...) m(n, l, i1)
#define _FOR_EACH_2(m, n, l, i1, ...) m(n, l, i1) _FOR_EACH_1(m, n, l, __VA_ARGS__)
#define _FOR_EACH_3(m, n, l, i1, ...) m(n, l, i1) _FOR_EACH_2(m, n, l, __VA_ARGS__)
#define _FOR_EACH_4(m, n, l, i1, ...) m(n, l, i1) _FOR_EACH_3(m, n, l, __VA_ARGS__)
#define _FOR_EACH_5(m, n, l, i1, ...) m(n, l, i1) _FOR_EACH_4(m, n, l, __VA_ARGS__)

#define _GEN_INDEX_0()
#define _GEN_INDEX_1() _0
#define _GEN_INDEX_2() _0, _1
#define _GEN_INDEX_3() _0, _1, _2
#define _GEN_INDEX_4() _0, _1, _2, _3
#define _GEN_INDEX_5() _0, _1, _2, _3, _4
#define GEN_INDEX(n) _CONCAT(_GEN_INDEX_, n)()


enum {
    BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
    BLE_COMPRESSED_LOG_OUT_SOURCE_MESH,
    BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB,
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

/* The maximum number of buffers used simultaneously */
#define LOG_CP_MAX_LOG_BUFFER_USED_SIMU 3

#define LOG_TYPE_ZERO_ARGS 0
#define LOG_TYPE_HEX_ARGS  1
#define LOG_TYPE_HEX_BUF   2
/* This type of message is used to update log information,
 * such as there is currently a new task log */
#define LOG_TYPE_INFO  3

#define LOG_TYPE_INFO_TASK_ID_UPDATE 0
#define LOG_TYPE_INFO_NULL_BUF 1
#define LOG_TYPE_INFO_TASK_SWITCH 2

typedef struct {
    volatile bool busy;
    uint8_t *buffer;
    uint16_t idx;
    uint16_t len;
} ble_cp_log_buffer_mgmt_t;

#define CONTENT_CHECK(idx, buf, except_val, len)
#define LENGTH_CHECK(idx, pbuffer_mgmt) do ( if(unlikely((idx) > (pbuffer_mgmt->len))) assert(0 && "Maximum log buffer length exceeded");) while(0)

#define BLE_LOG_CP_CONTENT_CHECK_ENBALE 0
#define BLE_LOG_CP_CONTENT_CHECK_VAL 0x00

static inline int ble_log_cp_buffer_safe_check(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint16_t write_len)
{
    if ((pbuf_mgmt->idx + write_len) > pbuf_mgmt->len) {
        printf("Maximum length of buffer(%p) idx %d write_len %d exceed\n", pbuf_mgmt, pbuf_mgmt->idx, write_len);
        return -1;
    }
#if BLE_LOG_CP_CONTENT_CHECK_ENBALE
    for (int i = pbuf_mgmt->idx; i < pbuf_mgmt->idx + write_len; i++) {
        if (pbuf_mgmt->buffer[i] != BLE_LOG_CP_CONTENT_CHECK_VAL) {
            printf("The value(%02x) in the buffer does not match the expected(%02x)\n", pbuf_mgmt->buffer[i], BLE_LOG_CP_CONTENT_CHECK_VAL);
            return -1;
        }
    }
#endif
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
    uint16_t *p = (uint16_t *)&(pbuf_mgmt->buffer[pbuf_mgmt->idx]);
    *p = val;
    pbuf_mgmt->idx+=2;
    return 0;
}

static inline int ble_log_cp_push_u32(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint32_t val)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, 4)) {
        return -1;
    }
    uint32_t *p = (uint32_t *)&(pbuf_mgmt->buffer[pbuf_mgmt->idx]);
    *p = val;
    pbuf_mgmt->idx+=4;
    return 0;
}

static inline int ble_log_cp_push_u64(ble_cp_log_buffer_mgmt_t *pbuf_mgmt, uint64_t val)
{
    if (ble_log_cp_buffer_safe_check(pbuf_mgmt, 8)) {
        return -1;
    }
    uint64_t *p = (uint64_t *)&(pbuf_mgmt->buffer[pbuf_mgmt->idx]);
    *p = val;
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

static inline int ble_log_cp_buffer_print(ble_cp_log_buffer_mgmt_t *pbuf_mgmt)
{
    for (size_t i = 0; i < pbuf_mgmt->idx; i++) {
        printf("%02x ", pbuf_mgmt->buffer[i]);
    }
    printf("\n");
    return 0;
}

static inline int ble_compressed_log_output(uint8_t source, uint8_t *data, uint16_t len)
{
    return ble_log_write_hex(BLE_LOG_SRC_ENCODE, data, len);
}
#endif /* _BLE_LOG_COMPRESSION_UTILS_H */
