/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_PRPH_DUMMY_H__
#define __BLE_LOG_PRPH_DUMMY_H__

/* ----------------------------------------------- */
/* BLE Log - Peripheral-specific Transport - Dummy */
/* ----------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph.h"

/* TYPEDEF */
typedef struct {
    uint8_t *trans_buf;
} ble_log_prph_trans_ctx_t;

#endif /* __BLE_LOG_PRPH_DUMMY_H__ */
