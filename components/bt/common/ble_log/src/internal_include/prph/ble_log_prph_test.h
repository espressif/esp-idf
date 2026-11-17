/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_PRPH_TEST_H__
#define __BLE_LOG_PRPH_TEST_H__

/* ---------------------------------------------- */
/* BLE Log - Peripheral-specific Transport - Test */
/* ---------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph.h"

/* TYPEDEF */
typedef struct {
    uint8_t *trans_buf;
    int64_t received_at_us;
} ble_log_prph_trans_ctx_t;

typedef void (*ble_log_prph_test_auto_recycle_hook_t)(void *ctx);

/* Recycles transports immediately and calls hook instead of queueing them.
 * This lets tests model a producer that refills the runtime queue while its
 * dispatch callback is still running. */
void ble_log_prph_test_set_auto_recycle_hook(ble_log_prph_test_auto_recycle_hook_t hook,
                                             void *ctx);

/* Reads and releases one pending transport, returning the bytes copied and
 * optionally the time when the test peripheral received ownership.
 * When bytes_per_second is non-zero the call blocks until the simulated
 * link has transmitted the whole transport at that rate. */
size_t ble_log_prph_test_read(uint8_t *data, size_t len, TickType_t timeout,
                              uint32_t bytes_per_second, int64_t *received_at_us);

#endif /* __BLE_LOG_PRPH_TEST_H__ */
