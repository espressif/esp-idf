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
/* CRITICAL: this enum is a public ABI and must not be reordered or renamed.
 * Its values are also the on-wire source IDs of protocol v7 frames. */
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

/* Encodes HCI direction in payload byte 0 bit 7 for the synchronous copy,
 * then restores the complete original HCI type byte. The caller guarantees a
 * non-NULL buffer with len > 0. */
#define ble_log_write_hci(direction, data, len) do {                  \
    uint8_t *const ble_log_hci_data__ = (data);                       \
    const uint8_t ble_log_hci_type__ = ble_log_hci_data__[0];         \
    ble_log_hci_data__[0] = (ble_log_hci_type__ & 0x7fU) |            \
                            ((direction) ? 0x80U : 0U);               \
    (void)ble_log_write_hex(BLE_LOG_SRC_HCI, ble_log_hci_data__,      \
                            (len));                                   \
    ble_log_hci_data__[0] = ble_log_hci_type__;                       \
} while (0)

/* INTERFACE */
bool ble_log_init(void);
void ble_log_deinit(void);
bool ble_log_enable(bool enable);
/* Blocking; call only from a caller-owned task, not an ISR or system callback. */
void ble_log_flush(void);
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len);
uint8_t *ble_log_claim(ble_log_src_t src_code, size_t max_len, uint32_t *handle);
void ble_log_commit(uint32_t handle, size_t actual_len);
void ble_log_dump_to_console(void);
#if CONFIG_BLE_LOG_LL_ENABLED
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, uint32_t flag);
#endif /* CONFIG_BLE_LOG_LL_ENABLED */
bool ble_log_sync_enable(bool enable);

#endif /* __BLE_LOG_H__ */
