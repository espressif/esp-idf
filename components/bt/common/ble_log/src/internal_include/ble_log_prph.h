/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_PRPH_H__
#define __BLE_LOG_PRPH_H__

/* ------------------------------ */
/* BLE Log - Peripheral Interface */
/* ------------------------------ */

#include "ble_log_util.h"

#if defined(CONFIG_BLE_LOG_PRPH_UART_DMA) && (CONFIG_BLE_LOG_PRPH_UART_DMA_PORT == 0)
#define BLE_LOG_UART_REDIR_ENABLED              (1)
#else
#define BLE_LOG_UART_REDIR_ENABLED              (0)
#endif

typedef enum {
    BLE_LOG_TRANS_STATE_FREE = 0,
    BLE_LOG_TRANS_STATE_OPEN,
    BLE_LOG_TRANS_STATE_CLAIMED,
    BLE_LOG_TRANS_STATE_SENDING,
} ble_log_trans_state_t;

typedef enum {
    BLE_LOG_TRANS_OWNER_POOL = 0,
    BLE_LOG_TRANS_OWNER_INTERNAL,
    BLE_LOG_TRANS_OWNER_REDIR,
} ble_log_trans_owner_t;

#define BLE_LOG_TRANS_ID_NONE                   (0xff)

typedef struct {
    /* Transport lifecycle ownership, shared with the runtime dispatch and
     * the peripheral tx-done recycle. Claim/commit bookkeeping deliberately
     * lives in the pool (ble_log_lbm_v2.c), not here. */
    volatile ble_log_atomic_lock_t atomic_lock;
    volatile uint8_t state;
    uint8_t id;
    uint8_t owner_kind;

    /* Lazy flush marker, pool-owned: set by the periodic flusher (without
     * holding atomic_lock) when the transport was busy at flush time; the
     * next claim that takes the lock seals the buffered frames first.
     * Cleared by seal_and_send and on recycle, but a delayed flusher may
     * store true afterwards. Such a stale hint permits an extra partial
     * seal in the next lifecycle, never sending a writer-owned buffer. */
    volatile uint8_t pending_seal;

    uint8_t *buf;
    uint16_t size;
    uint16_t pos;

    /* Peripheral implementation specific context. */
    void *ctx;
} ble_log_prph_trans_t;

#define BLE_LOG_TRANS_FREE_SPACE(trans)         ((trans)->size - (trans)->pos)
#define BLE_LOG_TRANS_BUF_CNT                   (4)

bool ble_log_prph_init(size_t trans_cnt);
void ble_log_prph_deinit(void);
/* Allocates a transport whose storage is zeroed (every field except size
 * starts at its zero value: state FREE, owner POOL, lock and pending_seal
 * clear). Callers only need to set the non-zero identity fields. */
bool ble_log_prph_trans_init(ble_log_prph_trans_t **trans, size_t trans_size);
void ble_log_prph_trans_deinit(ble_log_prph_trans_t **trans);
void ble_log_prph_send_trans(ble_log_prph_trans_t *trans);
#if BLE_LOG_UART_REDIR_ENABLED
bool ble_log_prph_flush(void);
void ble_log_prph_reset_util_counters(void);
#endif

#endif /* __BLE_LOG_PRPH_H__ */
