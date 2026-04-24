/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #ifndef __BLE_HOST_INTERNAL_LOG_INDEX_H
 #define __BLE_HOST_INTERNAL_LOG_INDEX_H

 #include <stddef.h>
 #include <stdlib.h>
// Compression function declarations
extern int ble_log_compressed_hex_print
        (uint8_t source, uint32_t log_index, size_t args_size_cnt, ...);
extern int ble_log_compressed_hex_print_buf
        (uint8_t source, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, size_t len);


#define APPL_TRACE_ERROR_1(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(APPL, ERROR))     ble_log_compressed_hex_print(0,1, 1, 0, status);\
;}\
    APPL_TRACE_ERROR(fmt, ##__VA_ARGS__);\
}

#define APPL_TRACE_WARNING_2(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(APPL, WARNING))     ble_log_compressed_hex_print(0,2, 1, 0, status);\
;}\
    APPL_TRACE_WARNING(fmt, ##__VA_ARGS__);\
}

#define APPL_TRACE_API_3(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(APPL, API))     ble_log_compressed_hex_print(0,3, 1, 0, handle);\
;}\
}

#define APPL_TRACE_DEBUG_4(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,4, 2, 0, 0, status, handle);\
;}\
}

#define APPL_TRACE_EVENT_5(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(APPL, EVENT))     ble_log_compressed_hex_print(0,5, 0);\
;}\
}

#define APPL_TRACE_VERBOSE_6(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_VERBOSE && BT_LOG_LEVEL_CHECK(APPL, VERBOSE))     ble_log_compressed_hex_print(0,6, 1, 0, handle);\
;}\
}

#endif // __BLE_HOST_INTERNAL_LOG_INDEX_H
