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


#define APPL_TRACE_DEBUG_1(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,1, 0);\
;}\
}

#define APPL_TRACE_DEBUG_2(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,2, 0);\
;}\
}

#define APPL_TRACE_ERROR_3(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(APPL, ERROR))     ble_log_compressed_hex_print(0,3, 1, 0, err_code);\
;}\
    APPL_TRACE_ERROR(fmt, ##__VA_ARGS__);\
}

#define APPL_TRACE_DEBUG_4(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,4, 0);\
    ble_log_compressed_hex_print_buf(0, 4, 0, (const uint8_t *)bd_addr, 6);\
;}\
}

#define APPL_TRACE_DEBUG_5(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,5, 1, 0, handle);\
    ble_log_compressed_hex_print_buf(0, 5, 0, (const uint8_t *)addr, 6);\
;}\
}

#endif // __BLE_HOST_INTERNAL_LOG_INDEX_H
