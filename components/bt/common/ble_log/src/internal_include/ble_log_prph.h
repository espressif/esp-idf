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
typedef struct {
    bool prph_owned;
    uint8_t *buf;
    uint16_t size;
    uint16_t pos;

    /* Peripheral implementation specific context */
    void *ctx;

    /* Opaque back-reference to owning LBM, set once at init */
    void *owner;
} ble_log_prph_trans_t;

#define BLE_LOG_TRANS_FREE_SPACE(trans)         (trans->size - trans->pos)
#define BLE_LOG_TRANS_BUF_CNT                   (4)

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt);
void ble_log_prph_deinit(void);
bool ble_log_prph_trans_init(ble_log_prph_trans_t **trans, size_t trans_size);
void ble_log_prph_trans_deinit(ble_log_prph_trans_t **trans);
void ble_log_prph_send_trans(ble_log_prph_trans_t *trans);
void ble_log_prph_reset_util_counters(void);

#endif /* __BLE_LOG_PRPH_H__ */
