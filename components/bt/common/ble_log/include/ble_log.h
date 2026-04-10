/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_H__
#define __BLE_LOG_H__

/* ------- */
/* BLE Log */
/* ------- */

/* INCLUDE */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* TYPEDEF */
/* CRITICAL:
 * The number of BLE Log source code will directly determine the number of statistic manager
 * memory requirements, keep it as less as possible; it's recommended to use subcode for more
 * log data structure decoding */
typedef enum {
    /* Internal */
    BLE_LOG_SRC_INTERNAL = 0,

    /* Custom */
    BLE_LOG_SRC_CUSTOM,

    /* BLE Stack */
    BLE_LOG_SRC_LL_TASK,
    BLE_LOG_SRC_LL_HCI,
    BLE_LOG_SRC_LL_ISR,
    BLE_LOG_SRC_HOST,
    BLE_LOG_SRC_HCI,
    BLE_LOG_SRC_ENCODE,

    /* UART redirection (PORT 0 only) */
    BLE_LOG_SRC_REDIR,

    BLE_LOG_SRC_MAX,
} ble_log_src_t;

/* HCI Log Direction */
#define BLE_LOG_HCI_DOWNSTREAM  0
#define BLE_LOG_HCI_UPSTREAM    1

/* HCI Log Write Macro
 * Encodes direction in MSB of data[0] (HCI type byte) before writing.
 * Safe because ble_log_write_hex -> ble_log_lbm_write_trans does synchronous memcpy.
 * Parser reads MSB to determine direction; old firmware with MSB=0 defaults to "sent". */
#define ble_log_write_hci(direction, data, len) do {    \
    (data)[0] |= ((direction) << 7);                   \
    ble_log_write_hex(BLE_LOG_SRC_HCI, (data), (len)); \
    (data)[0] &= 0x7F;                                 \
} while (0)

/* INTERFACE */
bool ble_log_init(void);
void ble_log_deinit(void);
bool ble_log_enable(bool enable);
void ble_log_flush(void);
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len);
void ble_log_dump_to_console(void);
#if CONFIG_BLE_LOG_LL_ENABLED
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, uint32_t flag);
#endif /* CONFIG_BLE_LOG_LL_ENABLED */
#if CONFIG_BLE_LOG_TS_ENABLED
bool ble_log_sync_enable(bool enable);
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

#endif /* __BLE_LOG_H__ */
