/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

// Private includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "ble_log_util.h"
#include "log_compression/utils.h"

#if CONFIG_BLE_COMPRESSED_LOG_ENABLE

#define BUF_NAME(name, idx) name##_buffer##idx
#define BUF_MGMT_NAME(name) name##_log_buffer_mgmt

#define DECL_BUF_OP(name, len, idx) \
    static uint8_t BUF_NAME(name, idx)[len];

#define INIT_MAP_OP(name, _, buffer_idx) \
    {.busy = 0, \
     .idx = 0, \
     .buffer = BUF_NAME(name, buffer_idx), \
     .len = sizeof(BUF_NAME(name, buffer_idx))},

#define DECLARE_BUFFERS(NAME, BUF_LEN, BUF_CNT) \
    FOR_EACH_IDX(DECL_BUF_OP, NAME, BUF_LEN, GEN_INDEX(BUF_CNT));

#define INIT_BUFFER_MGMT(NAME, BUF_CNT) \
    ble_cp_log_buffer_mgmt_t BUF_MGMT_NAME(NAME)[BUF_CNT] = { \
        FOR_EACH_IDX(INIT_MAP_OP, NAME, 0, GEN_INDEX(BUF_CNT)) \
    };

#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
DECLARE_BUFFERS(mesh, CONFIG_BLE_MESH_COMPRESSED_LOG_BUFFER_LEN, LOG_CP_MAX_LOG_BUFFER_USED_SIMU);
INIT_BUFFER_MGMT(mesh, LOG_CP_MAX_LOG_BUFFER_USED_SIMU);
char * mesh_last_task_handle = NULL;
#endif

#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
DECLARE_BUFFERS(host, CONFIG_BLE_HOST_COMPRESSED_LOG_BUFFER_LEN, LOG_CP_MAX_LOG_BUFFER_USED_SIMU);
INIT_BUFFER_MGMT(host, LOG_CP_MAX_LOG_BUFFER_USED_SIMU);
char * host_last_task_handle = NULL;
#endif

/* The maximum number of supported parameters is 64 */
#define LOG_HEADER(log_type, info) ((log_type << 6) | (info & 0x3f))

int ble_compressed_log_cb_get(uint8_t source, ble_cp_log_buffer_mgmt_t **mgmt)
{
    ble_cp_log_buffer_mgmt_t *buffer_mgmt = NULL;
    char ** last_handle = NULL;
    char * cur_handle = pcTaskGetName(NULL);

    switch (source)
    {
#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
    case BLE_COMPRESSED_LOG_OUT_SOURCE_MESH:
    case BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB:
        buffer_mgmt = BUF_MGMT_NAME(mesh);
        last_handle = &mesh_last_task_handle;
        break;
#endif
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
    case BLE_COMPRESSED_LOG_OUT_SOURCE_HOST:
        buffer_mgmt = BUF_MGMT_NAME(host);
        last_handle = &host_last_task_handle;
        break;
#endif
    default:
        assert(0 && "Unsupported log source");
        break;
    }

    for (int i = 0; i < LOG_CP_MAX_LOG_BUFFER_USED_SIMU; i++) {
        if (ble_log_cas_acquire(&(buffer_mgmt[i].busy))) {
            *mgmt = &buffer_mgmt[i];
            ble_log_cp_push_u8(*mgmt, source);
            if (*last_handle == NULL ||
                *last_handle != cur_handle) {
                ble_log_cp_push_u8(*mgmt, LOG_HEADER(LOG_TYPE_INFO, LOG_TYPE_INFO_TASK_SWITCH));
                *last_handle = cur_handle;
            }
            return 0;
        }
    }

    return -1;
}

static inline int ble_compressed_log_buffer_free(ble_cp_log_buffer_mgmt_t *mgmt)
{
#if BLE_LOG_CP_CONTENT_CHECK_ENBALE
    memset(mgmt->buffer, BLE_LOG_CP_CONTENT_CHECK_VAL, mgmt->idx);
#endif
    mgmt->idx = 0;
    ble_log_cas_release(&mgmt->busy);
    return 0;
}

static inline
int ble_log_compressed_hex_print_internal(ble_cp_log_buffer_mgmt_t *mgmt, uint32_t log_index, size_t args_cnt, va_list args)
{
    uint8_t arg_type = 0;

    ble_log_cp_push_u8(mgmt, LOG_HEADER(LOG_TYPE_HEX_ARGS, args_cnt));
    ble_log_cp_push_u16(mgmt, log_index);
    uint8_t size_info_idx = mgmt->idx;
    uint8_t *cur = &(mgmt->buffer)[mgmt->idx];
    uint8_t size_info = 0;

    for (size_t i = 0; i < args_cnt; i++) {
        if (i % 2) {
            arg_type = va_arg(args, size_t);
            ble_log_cp_push_u8(mgmt, size_info|arg_type);
            size_info = 0;
            cur++;
        } else {
            arg_type = va_arg(args, size_t);
            if (i == args_cnt - 1) {
                ble_log_cp_push_u8(mgmt, arg_type);
            } else {
                size_info = arg_type << 4;
            }
        }
        if (arg_type >= ARG_SIZE_TYPE_MAX) {
            printf("Found invalid arg type %08lx type %d", log_index, arg_type);
            return 0;
        }
    }

    cur = &(mgmt->buffer)[size_info_idx];

    for (size_t i = 0; i < args_cnt; i++) {
        if (i % 2) {
            arg_type = (*cur) & 0x0f;
            cur++;
        } else {
            arg_type = (*cur) >> 4;
        }
        switch(arg_type) {
            case ARG_SIZE_TYPE_U32:
                uint32_t u32v = va_arg(args, size_t);
                if (likely(u32v)) {
                    if (u32v <= 0xff) {
                        ble_log_cp_push_u8(mgmt, 3);
                        ble_log_cp_push_u8(mgmt, u32v);
                        ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_LZU32, !(i%2));
                        break;
                    } else if (u32v <= 0xffff) {
                        ble_log_cp_push_u8(mgmt, 2);
                        ble_log_cp_push_u16(mgmt, u32v);
                        ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_LZU32, !(i%2));
                        break;
                    } else {
                        ble_log_cp_push_u32(mgmt, u32v);
                    }
                } else {
                    ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_AZU32, !(i%2));
                }
            break;
            case ARG_SIZE_TYPE_U64:
                uint64_t u64v = va_arg(args, uint64_t);
                if (likely(u64v)) {
                    if (unlikely(u64v >> 48)) {
                        ble_log_cp_push_u64(mgmt, u64v);
                    } else {
                        uint32_t tmpv = 0;
                        uint8_t lz = 0;
                        if (likely(u64v <= UINT32_MAX)) {
                            tmpv = (uint32_t)u64v;
                            lz = 4;
                        } else {
                            tmpv = u64v >> 32;
                        }
                        lz += __builtin_clz(tmpv) / 8;
                        ble_log_cp_push_u8(mgmt, lz);
                        switch (8-lz) {
                            case 5:
                                ble_log_cp_push_u32(mgmt, (uint32_t)u64v);
                                [[fallthrough]];
                            case 1:
                                ble_log_cp_push_u8(mgmt, (uint8_t)tmpv);
                            break;
                            case 6:
                                ble_log_cp_push_u32(mgmt, (uint32_t)u64v);
                                [[fallthrough]];
                            case 2:
                                ble_log_cp_push_u16(mgmt, (uint16_t)tmpv);
                            break;
                            case 7:
                                ble_log_cp_push_u32(mgmt, (uint32_t)u64v);
                                [[fallthrough]];
                            case 3:
                                ble_log_cp_push_u8(mgmt, (uint8_t)tmpv);
                                ble_log_cp_push_u16(mgmt, (uint16_t)(tmpv >> 8));
                            break;
                            default:
                                assert(0);
                                break;
                        }
                        ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_LZU64, !(i%2));
                    }
                } else {
                    ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_AZU64, !(i%2));
                }
            break;
            case ARG_SIZE_TYPE_STR:
                char *str_p = (char *)va_arg(args, char *);
                ble_log_cp_push_buf(mgmt, (const uint8_t *)str_p, strlen(str_p) + 1);
            break;
            default:
                printf("Invalid size %d\n", arg_type);
                assert(0);
            break;
        }
    }
    return 0;
}

int ble_log_compressed_hex_printv(uint8_t source, uint32_t log_index, size_t args_cnt, va_list args)
{
    ble_cp_log_buffer_mgmt_t *mgmt = NULL;

    if (ble_compressed_log_cb_get(source, &mgmt)) {
        return 0;
    }

    ble_log_compressed_hex_print_internal(mgmt, log_index, args_cnt, args);
    ble_compressed_log_output(source, mgmt->buffer, mgmt->idx);
    ble_compressed_log_buffer_free(mgmt);
    return 0;
}

int ble_log_compressed_hex_print(uint8_t source, uint32_t log_index, size_t args_cnt, ...)
{
    ble_cp_log_buffer_mgmt_t *mgmt = NULL;

    if (ble_compressed_log_cb_get(source, &mgmt)) {
        return 0;
    }

    if (args_cnt == 0) {
        ble_log_cp_push_u8(mgmt, LOG_HEADER(LOG_TYPE_HEX_ARGS, 0));
        ble_log_cp_push_u16(mgmt, log_index);
    } else {
        va_list args;
        va_start(args, args_cnt);
        ble_log_compressed_hex_print_internal(mgmt, log_index, args_cnt, args);
        va_end(args);
    }

    ble_compressed_log_output(source, mgmt->buffer, mgmt->idx);
    ble_compressed_log_buffer_free(mgmt);
    return 0;
}

int ble_log_compressed_hex_print_buf(uint8_t source, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, size_t len)
{
    ble_cp_log_buffer_mgmt_t *mgmt = NULL;

    if (ble_compressed_log_cb_get(source, &mgmt)) {
        return 0;
    }

    if (buf == NULL && len != 0) {
        ble_log_cp_push_u8(mgmt, LOG_HEADER(LOG_TYPE_INFO, LOG_TYPE_INFO_NULL_BUF));
        ble_log_cp_push_u16(mgmt, log_index);
        ble_compressed_log_output(source, mgmt->buffer, mgmt->idx);
        ble_compressed_log_buffer_free(mgmt);
        return 0;
    }

    ble_log_cp_push_u8(mgmt, LOG_HEADER(LOG_TYPE_HEX_BUF, buf_idx));
    ble_log_cp_push_u16(mgmt, log_index);
    ble_log_cp_push_buf(mgmt, buf, len);
    ble_compressed_log_output(source, mgmt->buffer, mgmt->idx);
    ble_compressed_log_buffer_free(mgmt);
    return 0;
}
#endif /* CONFIG_BLE_COMPRESSED_LOG_ENABLE */
