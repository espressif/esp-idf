/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ----------------------------------------------- */
/* BLE Log - Peripheral-specific Transport - Dummy */
/* ----------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph_dummy.h"

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt)
{
    (void)trans_cnt;
    return true;
}

void ble_log_prph_deinit(void)
{
}

bool ble_log_prph_trans_init(ble_log_prph_trans_t **trans, size_t trans_size)
{
    /* Validate inputs */
    if (!trans || !trans_size) {
        return false;
    }

    /* Initialize peripheral transport data */
    *trans = (ble_log_prph_trans_t *)BLE_LOG_MALLOC(sizeof(ble_log_prph_trans_t));
    if (!(*trans)) {
        goto exit;
    }
    BLE_LOG_MEMSET(*trans, 0, sizeof(ble_log_prph_trans_t));
    (*trans)->size = trans_size;

    /* Initialize peripheral-specific transport context */
    ble_log_prph_trans_ctx_t *dummy_trans_ctx = (ble_log_prph_trans_ctx_t *)BLE_LOG_MALLOC(sizeof(ble_log_prph_trans_ctx_t));
    if (!dummy_trans_ctx) {
        goto exit;
    }
    BLE_LOG_MEMSET(dummy_trans_ctx, 0, sizeof(ble_log_prph_trans_ctx_t));
    (*trans)->ctx = (void *)dummy_trans_ctx;

    /* Initialize log buffer */
    (*trans)->buf = (uint8_t *)BLE_LOG_MALLOC(trans_size);
    if (!(*trans)->buf) {
        goto exit;
    }
    BLE_LOG_MEMSET((*trans)->buf, 0, trans_size);
    dummy_trans_ctx->trans_buf = (*trans)->buf;
    return true;

exit:
    ble_log_prph_trans_deinit(trans);
    return false;
}

void ble_log_prph_trans_deinit(ble_log_prph_trans_t **trans)
{
    /* Validate inputs */
    if (!trans || !(*trans)) {
        return;
    }

    /* Release log buffer */
    if ((*trans)->buf) {
        BLE_LOG_FREE((*trans)->buf);
    }

    /* Release peripheral-specific transport context */
    if ((*trans)->ctx) {
        BLE_LOG_FREE((*trans)->ctx);
    }

    /* Release peripheral transport data */
    BLE_LOG_FREE(*trans);
    *trans = NULL;
}

void ble_log_prph_send_trans(ble_log_prph_trans_t *trans)
{
    (void)trans;
}
