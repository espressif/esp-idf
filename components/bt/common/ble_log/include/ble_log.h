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
 * Its values are the base on-wire source IDs of protocol v8 frames. */
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
/* Controls public producers only. Disabling does not stop buffered-data flushing,
 * periodic snapshots, task-binding broadcasts, or configured external sync output.
 * Clock samples and diagnostic statistics remain available until deinit. */
bool ble_log_enable(bool enable);
/* Blocking lifecycle operation in a caller-owned, yieldable task; serialize
 * with init/deinit/enable/flush. Do not call from ISR, critical sections,
 * a suspended scheduler or shared system callbacks (including ESP Timer).
 * The caller must commit its own outstanding claims before pausing.
 * Closes admission and waits up to 1 second for writers to exit, without waiting
 * for transport output. An uninitialized module is a no-op.
 * Drain timeout asserts with admission closed; with assertions disabled,
 * keeps waiting rather than allowing deletion of unfinished writers.
 * Once paused, controller disable keeps admission closed until log deinit,
 * including when the controller's own disable operation fails. */
void ble_log_pause(void);
/* Blocking; the only production caller in the IDF build is the BLE Log
 * shutdown handler. Normal logging uses periodic draining, not this function.
 * Shutdown handlers run sequentially, so flush and controller pause do not overlap.
 * Tests may call directly from a caller-owned task. Serialize with BLE Log
 * init/deinit/enable/pause and the complete controller lifecycle operation;
 * concurrent lifecycle calls, ISR and shared system callback calls are unsupported. */
void ble_log_flush(void);
/* Waits for a shared transport in ordinary yieldable tasks. The shared ESP
 * Timer task, ISR and critical-section callers fail fast when none is available. */
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len);
/* Same backpressure as ble_log_write_hex(): ordinary yieldable tasks wait
 * when wait_for_transport is true; false opts out. The shared ESP Timer
 * task and non-yieldable contexts fail fast regardless of this flag. */
uint8_t *ble_log_claim(ble_log_src_t src_code, size_t max_len,
                       uint32_t *handle, bool wait_for_transport);
void ble_log_commit(uint32_t handle, size_t actual_len);
void ble_log_dump_to_console(void);
#if CONFIG_BLE_LOG_LL_ENABLED
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, uint32_t flag);
#endif /* CONFIG_BLE_LOG_LL_ENABLED */
/* Task-context only. Controls the optional TS sync IO toggle, which starts
 * disabled and low; this is a lifecycle-checked no-op when the toggle is not
 * built. Periodic Internal Snapshots remain active in either state. */
bool ble_log_ts_sync_io_toggle_enable(bool enable);
/* Backward-compatible name for ble_log_ts_sync_io_toggle_enable(). */
bool ble_log_sync_enable(bool enable);

#endif /* __BLE_LOG_H__ */
