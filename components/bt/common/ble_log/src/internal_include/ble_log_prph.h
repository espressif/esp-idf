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

/* INCLUDE */
#include "ble_log_util.h"

/* TYPEDEF */
/* Transport buffer lifecycle state. Ownership of a transport is decided
 * solely by its per-buffer atomic_lock; state only expresses the lifecycle.
 * See the unified buffer pool design for the ownership/hint model. */
typedef enum {
    BLE_LOG_TRANS_STATE_FREE = 0,   /* empty, allocatable */
    BLE_LOG_TRANS_STATE_OPEN,       /* holds frame(s), may still be appended */
    BLE_LOG_TRANS_STATE_SENDING,    /* sealed and handed to the peripheral link */
} ble_log_trans_state_t;

/* Sentinel id for transports that do not belong to the global pool
 * (e.g. the UART redirection buffers). */
#define BLE_LOG_TRANS_ID_NONE                   (0xFF)

typedef struct {
    volatile bool atomic_lock;              /* per-buffer lock, the only ownership source */
    volatile uint8_t state;                 /* ble_log_trans_state_t */
    uint8_t id;                             /* pool index, or BLE_LOG_TRANS_ID_NONE */
    uint8_t *buf;
    uint16_t size;
    uint16_t pos;

    /* Peripheral implementation specific context */
    void *ctx;
} ble_log_prph_trans_t;

#define BLE_LOG_TRANS_FREE_SPACE(trans)         ((trans)->size - (trans)->pos)
#define BLE_LOG_TRANS_BUF_CNT                   (4)

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt);
void ble_log_prph_deinit(void);
bool ble_log_prph_trans_init(ble_log_prph_trans_t **trans, size_t trans_size);
void ble_log_prph_trans_deinit(ble_log_prph_trans_t **trans);
void ble_log_prph_send_trans(ble_log_prph_trans_t *trans);
void ble_log_prph_reset_util_counters(void);

#endif /* __BLE_LOG_PRPH_H__ */
