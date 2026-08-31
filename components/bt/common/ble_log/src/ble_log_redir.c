/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ------------------------------------------------- */
/*     BLE Log - UART Redirection Stream Writer      */
/*                                                   */
/* Stream mode appends raw data into a transport     */
/* buffer with deferred frame encapsulation.         */
/* Redirection transports are single-writer under    */
/* redir->mutex; their only concurrent mutation is   */
/* the UART tx-done recycle (state SENDING -> FREE),  */
/* so state access uses atomics.                     */
/* ------------------------------------------------- */

/* INCLUDE */
#include "ble_log_redir.h"
#include "ble_log_rt.h"

#if BLE_LOG_UART_REDIR_ENABLED
BLE_LOG_STATIC void ble_log_redir_seal(ble_log_prph_trans_t *trans, ble_log_src_t src_code);

BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_redir_get_trans(ble_log_redir_t *redir,
                                               ble_log_src_t src_code)
{
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        ble_log_prph_trans_t *trans = redir->trans[redir->trans_idx];
        if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(trans->state) != BLE_LOG_TRANS_STATE_SENDING) {
            if (BLE_LOG_TRANS_FREE_SPACE(trans) >= BLE_LOG_FRAME_OVERHEAD) {
                return trans;
            }
            if (trans->pos > BLE_LOG_FRAME_HEAD_LEN) {
                ble_log_redir_seal(trans, src_code);
            }
        }
        redir->trans_idx = (redir->trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
    }
    return NULL;
}

BLE_LOG_STATIC
void ble_log_redir_seal(ble_log_prph_trans_t *trans, ble_log_src_t src_code)
{
    if (trans->pos <= BLE_LOG_FRAME_HEAD_LEN) {
        return;
    }

    uint16_t payload_len = trans->pos - BLE_LOG_FRAME_HEAD_LEN;
    ble_log_redir_t *redir = ble_log_prph_get_redir_lbm();
    BLE_LOG_ASSERT(redir);
    /* REDIR keeps its own stream sequence: a raw console stream is not a
     * log attempt, so it never eats Global SNs or fakes loss gaps. The
     * stream has no core-stat slot. */
    uint32_t frame_sn = BLE_LOG_GET_FRAME_SN(redir->frame_sn);
    ble_log_frame_head_t frame_head = {
        .length = payload_len,
        .frame_meta = BLE_LOG_MAKE_FRAME_META(src_code, frame_sn),
    };
    BLE_LOG_MEMCPY(trans->buf, &frame_head, BLE_LOG_FRAME_HEAD_LEN);

    uint32_t checksum = ble_log_fast_checksum(trans->buf, trans->pos);
    BLE_LOG_MEMCPY(trans->buf + trans->pos, &checksum, BLE_LOG_FRAME_TAIL_LEN);
    trans->pos += BLE_LOG_FRAME_TAIL_LEN;

    uint32_t infl = __atomic_add_fetch(&redir->inflight, 1, __ATOMIC_RELAXED);
    uint32_t peak = BLE_LOG_ATOMIC_LOAD_RELAXED(redir->inflight_peak);
    while (infl > peak &&
           !__atomic_compare_exchange_n(&redir->inflight_peak, &peak, infl, true,
                                        __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
    }

    BLE_LOG_ATOMIC_STORE_RELAXED(trans->state, BLE_LOG_TRANS_STATE_SENDING);
    ble_log_rt_submit_trans(trans);
}

void ble_log_lbm_stream_write(ble_log_redir_t *redir, ble_log_src_t src_code,
                              const uint8_t *data, size_t len)
{
    while (len > 0) {
        ble_log_prph_trans_t *trans = ble_log_redir_get_trans(redir, src_code);
        if (!trans) {
            /* Burn one stream SN so the dropped console batch leaves a
             * gap in the REDIR sequence. */
            (void)BLE_LOG_GET_FRAME_SN(redir->frame_sn);
            return;
        }

        if (trans->pos == 0) {
            trans->pos = BLE_LOG_FRAME_HEAD_LEN;
        }

        uint16_t available = BLE_LOG_TRANS_FREE_SPACE(trans);
        if (available <= BLE_LOG_FRAME_TAIL_LEN) {
            ble_log_redir_seal(trans, src_code);
            continue;
        }
        available -= BLE_LOG_FRAME_TAIL_LEN;

        size_t to_write = (len < available) ? len : available;
        BLE_LOG_MEMCPY(trans->buf + trans->pos, data, to_write);
        trans->pos += to_write;
        data += to_write;
        len -= to_write;

        if (BLE_LOG_TRANS_FREE_SPACE(trans) <= BLE_LOG_FRAME_OVERHEAD) {
            ble_log_redir_seal(trans, src_code);
        }
    }
}

void ble_log_lbm_stream_flush(ble_log_redir_t *redir, ble_log_src_t src_code)
{
    int trans_idx = redir->trans_idx;
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        ble_log_prph_trans_t *trans = redir->trans[trans_idx];
        if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(trans->state) != BLE_LOG_TRANS_STATE_SENDING &&
            trans->pos > BLE_LOG_FRAME_HEAD_LEN) {
            ble_log_redir_seal(trans, src_code);
        }
        trans_idx = (trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
    }
}
#endif /* BLE_LOG_UART_REDIR_ENABLED */
