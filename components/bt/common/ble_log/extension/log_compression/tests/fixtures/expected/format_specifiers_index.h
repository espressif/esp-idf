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
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,1, 5, 0, 0, 0, 0, 0, a, b, c, d, e);\
;}\
}

#define APPL_TRACE_DEBUG_2(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,2, 1, 2, f_val);\
;}\
}

#define APPL_TRACE_DEBUG_3(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,3, 2, 2, 2, ll_val, llx_val);\
;}\
}

#define APPL_TRACE_DEBUG_4(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,4, 2, 1, 1, str1, str2);\
;}\
}

#define APPL_TRACE_DEBUG_5(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,5, 5, 0, 1, 2, 2, 0, i, s, ll, f, h);\
;}\
}

#define APPL_TRACE_DEBUG_6(fmt, ...) {\
    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL, DEBUG))     ble_log_compressed_hex_print(0,6, 3, 0, 0, 1, val, num, str);\
;}\
}

#endif // __BLE_HOST_INTERNAL_LOG_INDEX_H
