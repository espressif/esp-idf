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
#include "ble_log_lbm_v2.h"
#include "ble_log_util.h"
#include "log_compression/utils.h"

#if CONFIG_BLE_COMPRESSED_LOG_ENABLE

#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
_Static_assert(CONFIG_BLE_MESH_COMPRESSED_LOG_BUFFER_LEN <=
               BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t),
               "Mesh compressed log record exceeds one BLE Log transport");
#endif
#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
_Static_assert(CONFIG_BLE_ISO_COMPRESSED_LOG_BUFFER_LEN <=
               BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t),
               "ISO compressed log record exceeds one BLE Log transport");
#endif
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
_Static_assert(CONFIG_BLE_HOST_COMPRESSED_LOG_BUFFER_LEN <=
               BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t),
               "Host compressed log record exceeds one BLE Log transport");
#endif

#define BLE_CP_TRY_PUSH(expr) do { \
        if ((expr) != 0) { \
            return -1; \
        } \
    } while (0)

#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
char * mesh_last_task_handle = NULL;
static ble_log_atomic_lock_t mesh_source_lock;
#endif

#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
char * iso_last_task_handle = NULL;
static ble_log_atomic_lock_t iso_source_lock;
#endif

#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE && CONFIG_BT_BLUEDROID_ENABLED
char * host_last_task_handle = NULL;
static ble_log_atomic_lock_t host_source_lock;
#endif

#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE && CONFIG_BT_NIMBLE_ENABLED
char * nimble_last_task_handle = NULL;
static ble_log_atomic_lock_t nimble_source_lock;
#endif

#if CONFIG_BLE_LOG_PRPH_TEST
extern void ble_log_test_compression_after_lock_hook(uint8_t source)
__attribute__((weak));
#endif

/* The maximum number of supported parameters is 64 */
#define LOG_HEADER(log_type, info) ((log_type << 6) | (info & 0x3f))

int ble_compressed_log_cb_get(uint8_t source, ble_cp_log_buffer_mgmt_t *mgmt)
{
    char ** last_handle = NULL;
    ble_log_atomic_lock_t *source_lock = NULL;
    uint16_t claim_len = 0;
    char * cur_handle = pcTaskGetName(NULL);

    switch (source)
    {
#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
    case BLE_COMPRESSED_LOG_OUT_SOURCE_MESH:
    case BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB:
        last_handle = &mesh_last_task_handle;
        source_lock = &mesh_source_lock;
        claim_len = CONFIG_BLE_MESH_COMPRESSED_LOG_BUFFER_LEN;
        break;
#endif
#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
    case BLE_COMPRESSED_LOG_OUT_SOURCE_ISO:
    case BLE_COMPRESSED_LOG_OUT_SOURCE_AUDIO_LIB:
        last_handle = &iso_last_task_handle;
        source_lock = &iso_source_lock;
        claim_len = CONFIG_BLE_ISO_COMPRESSED_LOG_BUFFER_LEN;
        break;
#endif
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE && (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)
    case BLE_COMPRESSED_LOG_OUT_SOURCE_HOST:
        claim_len = CONFIG_BLE_HOST_COMPRESSED_LOG_BUFFER_LEN;
#if CONFIG_BT_BLUEDROID_ENABLED
        last_handle = &host_last_task_handle;
        source_lock = &host_source_lock;
#elif CONFIG_BT_NIMBLE_ENABLED
        last_handle = &nimble_last_task_handle;
        source_lock = &nimble_source_lock;
#endif
        break;
#endif
    default:
        assert(0 && "Unsupported log source");
        return -1;
    }

    mgmt->len = claim_len;
    mgmt->idx = 0;
    mgmt->source_lock = source_lock;
    mgmt->last_task_handle = last_handle;
    mgmt->current_task_handle = cur_handle;
    mgmt->task_switched = false;
    mgmt->buffer = ble_log_claim(BLE_LOG_SRC_ENCODE, claim_len, &mgmt->handle);
    if (!mgmt->buffer) {
        return -1;
    }
    if (!BLE_LOG_CAS_ACQUIRE(source_lock)) {
        ble_log_commit(mgmt->handle, 0);
        return -1;
    }
#if CONFIG_BLE_LOG_PRPH_TEST
    if (ble_log_test_compression_after_lock_hook) {
        ble_log_test_compression_after_lock_hook(source);
    }
#endif

    if (ble_log_cp_push_u8(mgmt, source) != 0) {
        ble_log_commit(mgmt->handle, 0);
        BLE_LOG_CAS_RELEASE(source_lock);
        return -1;
    }
    char *previous_handle = __atomic_load_n(last_handle, __ATOMIC_RELAXED);
    if (previous_handle == NULL || previous_handle != cur_handle) {
        if (ble_log_cp_push_u8(
                mgmt, LOG_HEADER(LOG_TYPE_INFO, LOG_TYPE_INFO_TASK_SWITCH)) != 0) {
            ble_log_commit(mgmt->handle, 0);
            BLE_LOG_CAS_RELEASE(source_lock);
            return -1;
        }
        mgmt->task_switched = true;
    }
    return 0;
}

static inline int ble_compressed_log_commit(ble_cp_log_buffer_mgmt_t *mgmt)
{
    ble_log_commit(mgmt->handle, mgmt->idx);
    if (mgmt->task_switched) {
        __atomic_store_n(mgmt->last_task_handle, mgmt->current_task_handle,
                         __ATOMIC_RELAXED);
    }
    BLE_LOG_CAS_RELEASE(mgmt->source_lock);
    return 0;
}

static inline int ble_compressed_log_abort(ble_cp_log_buffer_mgmt_t *mgmt)
{
    ble_log_commit(mgmt->handle, 0);
    BLE_LOG_CAS_RELEASE(mgmt->source_lock);
    return 0;
}

static inline
int ble_log_compressed_hex_print_internal(ble_cp_log_buffer_mgmt_t *mgmt, uint32_t log_index, size_t args_cnt, va_list args)
{
    uint8_t arg_type = 0;
    uint16_t header_size = 1 + 2 + (args_cnt + 1) / 2; // header + log_index + size_info

    if (ble_log_cp_buffer_safe_check(mgmt, header_size)) {
        return -1;
    }

    BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, LOG_HEADER(LOG_TYPE_HEX_ARGS, args_cnt)));
    BLE_CP_TRY_PUSH(ble_log_cp_push_u16(mgmt, log_index));
    uint8_t size_info_idx = mgmt->idx;
    uint8_t *cur = &(mgmt->buffer)[mgmt->idx];
    uint8_t size_info = 0;

    for (size_t i = 0; i < args_cnt; i++) {
        if (i % 2) {
            arg_type = va_arg(args, size_t);
            BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, size_info|arg_type));
            size_info = 0;
            cur++;
        } else {
            arg_type = va_arg(args, size_t);
            if (i == args_cnt - 1) {
                BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, arg_type << 4));
            } else {
                size_info = arg_type << 4;
            }
        }
        if (arg_type >= ARG_SIZE_TYPE_MAX) {
            printf("Found invalid arg type %08lx type %d", log_index, arg_type);
            return -1;
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
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, 3));
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, u32v));
                        BLE_CP_TRY_PUSH(
                            ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_LZU32, !(i%2))
                        );
                        break;
                    } else if (u32v <= 0xffff) {
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, 2));
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u16(mgmt, u32v));
                        BLE_CP_TRY_PUSH(
                            ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_LZU32, !(i%2))
                        );
                        break;
                    } else {
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u32(mgmt, u32v));
                    }
                } else {
                    BLE_CP_TRY_PUSH(ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_AZU32, !(i%2)));
                }
            break;
            case ARG_SIZE_TYPE_U64:
                uint64_t u64v = va_arg(args, uint64_t);
                if (likely(u64v)) {
                    if (unlikely(u64v >> 48)) {
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u64(mgmt, u64v));
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
                        BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, lz));
                        switch (8-lz) {
                            case 5:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u32(mgmt, (uint32_t)u64v));
                                [[fallthrough]];
                            case 1:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, (uint8_t)tmpv));
                            break;
                            case 6:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u32(mgmt, (uint32_t)u64v));
                                [[fallthrough]];
                            case 2:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u16(mgmt, (uint16_t)tmpv));
                            break;
                            case 7:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u32(mgmt, (uint32_t)u64v));
                                [[fallthrough]];
                            case 3:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u8(mgmt, (uint8_t)tmpv));
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u16(mgmt, (uint16_t)(tmpv >> 8)));
                            break;
                            case 4:
                                BLE_CP_TRY_PUSH(ble_log_cp_push_u32(mgmt, (uint32_t)u64v));
                                break;
                            default:
                                assert(0);
                                return -1;
                        }
                        BLE_CP_TRY_PUSH(
                            ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_LZU64, !(i%2))
                        );
                    }
                } else {
                    BLE_CP_TRY_PUSH(ble_log_cp_update_half_byte(mgmt, size_info_idx + i/2, ARG_SIZE_TYPE_AZU64, !(i%2)));
                }
            break;
            case ARG_SIZE_TYPE_STR:
                char *str_p = (char *)va_arg(args, char *);
                if (str_p) {
                    BLE_CP_TRY_PUSH(ble_log_cp_push_buf(mgmt, (const uint8_t *)str_p, strlen(str_p) + 1));
                } else {
                    BLE_CP_TRY_PUSH(ble_log_cp_push_buf(mgmt, (const uint8_t *)"(null str)", sizeof("(null str)")));
                }
            break;
            default:
                printf("Invalid size %d\n", arg_type);
                assert(0);
            return -1;
        }
    }
    return 0;
}

int ble_log_compressed_hex_printv(uint8_t source, uint32_t log_index, size_t args_cnt, va_list args)
{
    ble_cp_log_buffer_mgmt_t mgmt;

    if (ble_compressed_log_cb_get(source, &mgmt)) {
        return 0;
    }

    if (ble_log_compressed_hex_print_internal(&mgmt, log_index, args_cnt, args) != 0) {
        ble_compressed_log_abort(&mgmt);
        return 0;
    }
    ble_compressed_log_commit(&mgmt);
    return 0;
}

int ble_log_compressed_hex_print(uint8_t source, uint32_t log_index, size_t args_cnt, ...)
{
    ble_cp_log_buffer_mgmt_t mgmt;

    if (ble_compressed_log_cb_get(source, &mgmt)) {
        return 0;
    }

    if (args_cnt == 0) {
        if (ble_log_cp_push_u8(&mgmt, LOG_HEADER(LOG_TYPE_HEX_ARGS, 0)) != 0 ||
            ble_log_cp_push_u16(&mgmt, log_index) != 0) {
            ble_compressed_log_abort(&mgmt);
            return 0;
        }
    } else {
        va_list args;
        va_start(args, args_cnt);
        if (ble_log_compressed_hex_print_internal(&mgmt, log_index, args_cnt, args) != 0) {
            va_end(args);
            ble_compressed_log_abort(&mgmt);
            return 0;
        }
        va_end(args);
    }

    ble_compressed_log_commit(&mgmt);
    return 0;
}

int ble_log_compressed_hex_print_buf(uint8_t source, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, size_t len)
{
    ble_cp_log_buffer_mgmt_t mgmt;

    if (ble_compressed_log_cb_get(source, &mgmt)) {
        return 0;
    }

    if (buf == NULL) {
        ble_log_cp_push_u8(&mgmt, LOG_HEADER(LOG_TYPE_INFO, LOG_TYPE_INFO_NULL_BUF));
        ble_log_cp_push_u16(&mgmt, log_index);
        ble_compressed_log_commit(&mgmt);
        return 0;
    }

    if (ble_log_cp_push_u8(&mgmt, LOG_HEADER(LOG_TYPE_HEX_BUF, buf_idx)) != 0 ||
        ble_log_cp_push_u16(&mgmt, log_index) != 0 ||
        ble_log_cp_push_u16(&mgmt, (uint16_t)len) != 0 ||
        ble_log_cp_push_buf(&mgmt, buf, (uint16_t)len) != 0) {
        ble_compressed_log_abort(&mgmt);
        return 0;
    }
    ble_compressed_log_commit(&mgmt);
    return 0;
}
#endif /* CONFIG_BLE_COMPRESSED_LOG_ENABLE */
