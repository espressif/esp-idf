/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ------------------------------- */
/* BLE Log - Log Buffer Management */
/* ------------------------------- */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_lbm.h"
#include "ble_log_rt.h"

#if CONFIG_SOC_ESP_NIMBLE_CONTROLLER
#include "os/os_mbuf.h"
#endif /* CONFIG_SOC_ESP_NIMBLE_CONTROLLER */

/* VARIABLE */
BLE_LOG_STATIC volatile uint32_t lbm_ref_count = 0;
BLE_LOG_STATIC bool lbm_inited = false;
BLE_LOG_STATIC bool lbm_enabled = false;
BLE_LOG_STATIC ble_log_lbm_ctx_t *lbm_ctx = NULL;
BLE_LOG_STATIC ble_log_stat_mgr_t *stat_mgr_ctx[BLE_LOG_SRC_MAX] = {0};

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC ble_log_lbm_t *ble_log_lbm_acquire(void);
BLE_LOG_STATIC void ble_log_lbm_release(ble_log_lbm_t *lbm);
BLE_LOG_STATIC
ble_log_prph_trans_t **ble_log_lbm_get_trans(ble_log_lbm_t *lbm, size_t log_len);
BLE_LOG_STATIC
void ble_log_lbm_write_trans(ble_log_prph_trans_t **trans, ble_log_src_t src_code,
                             const uint8_t *addr, uint16_t len,
                             const uint8_t *addr_append, uint16_t len_append, bool omdata);
#if BLE_LOG_UART_REDIR_ENABLED
BLE_LOG_STATIC
void ble_log_lbm_stream_seal(ble_log_prph_trans_t **trans, ble_log_src_t src_code);
#endif /* BLE_LOG_UART_REDIR_ENABLED */
BLE_LOG_STATIC void ble_log_stat_mgr_update(ble_log_src_t src_code, uint32_t len, bool lost);

/* ------------------------- */
/*     PRIVATE INTERFACE     */
/* ------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_lbm_t *ble_log_lbm_acquire(void)
{
    ble_log_lbm_t *lbm = NULL;
    ble_log_lbm_t *atomic_pool;
    ble_log_lbm_t *spin_lbm;
    int atomic_pool_size;

    if (BLE_LOG_IN_ISR()) {
        atomic_pool = lbm_ctx->atomic_pool_isr;
        spin_lbm = &(lbm_ctx->spin_isr);
        atomic_pool_size = BLE_LOG_LBM_ATOMIC_ISR_CNT;
    } else {
        atomic_pool = lbm_ctx->atomic_pool_task;
        spin_lbm = &(lbm_ctx->spin_task);
        atomic_pool_size = BLE_LOG_LBM_ATOMIC_TASK_CNT;
    }

    /* Try to acquire atomic LBM first */
    for (int i = 0; i < atomic_pool_size; i++) {
        lbm = &atomic_pool[i];
        if (ble_log_cas_acquire(&(lbm->atomic_lock))) {
            return lbm;
        }
    }

    /* Fallback to spinlock LBM */
    lbm = spin_lbm;
    BLE_LOG_ACQUIRE_SPIN_LOCK(&(lbm->spin_lock));
    return lbm;
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_lbm_release(ble_log_lbm_t *lbm)
{
    switch (lbm->lock_type) {
    case BLE_LOG_LBM_LOCK_ATOMIC:
        ble_log_cas_release(&(lbm->atomic_lock));
        break;
    case BLE_LOG_LBM_LOCK_SPIN:
        BLE_LOG_RELEASE_SPIN_LOCK(&lbm->spin_lock);
        break;
    case BLE_LOG_LBM_LOCK_NONE:
    default:
        break;
    }
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_lbm_write_trans(ble_log_prph_trans_t **trans, ble_log_src_t src_code,
                             const uint8_t *addr, uint16_t len,
                             const uint8_t *addr_append, uint16_t len_append, bool omdata)
{
    /* Preparation before writing */
    uint8_t *buf = (*trans)->buf + (*trans)->pos;
    uint16_t payload_len = len + len_append;
    ble_log_stat_mgr_t *stat_mgr = stat_mgr_ctx[src_code];
    uint32_t frame_sn = BLE_LOG_GET_FRAME_SN(&(stat_mgr->frame_sn));
    ble_log_frame_head_t frame_head = {
        .length = payload_len,
        .frame_meta = BLE_LOG_MAKE_FRAME_META(src_code, frame_sn),
    };

    /* Memory operation */
    BLE_LOG_MEMCPY(buf, &frame_head, BLE_LOG_FRAME_HEAD_LEN);
    if (len) {
        BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN, addr, len);
    }
    if (len_append) {
#if CONFIG_SOC_ESP_NIMBLE_CONTROLLER
        if (omdata) {
            os_mbuf_copydata((struct os_mbuf *)addr_append, 0,
                             len_append, buf + BLE_LOG_FRAME_HEAD_LEN + len);
        }
        else
#endif /* CONFIG_SOC_ESP_NIMBLE_CONTROLLER */
        {
            BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN + len, addr_append, len_append);
        }
    }

    /* Data integrity check */
    uint32_t checksum = ble_log_fast_checksum((const uint8_t *)buf, BLE_LOG_FRAME_HEAD_LEN + payload_len);
    BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN + payload_len, &checksum, BLE_LOG_FRAME_TAIL_LEN);

    /* Update peripheral transport */
    (*trans)->pos += payload_len + BLE_LOG_FRAME_OVERHEAD;

    ble_log_stat_mgr_update(src_code, payload_len, false);

    /* Queue trans if full */
    if (BLE_LOG_TRANS_FREE_SPACE((*trans)) <= BLE_LOG_FRAME_OVERHEAD) {
        ble_log_rt_queue_trans(trans);
    }
}

#if BLE_LOG_UART_REDIR_ENABLED
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_lbm_stream_seal(ble_log_prph_trans_t **trans, ble_log_src_t src_code)
{
    if ((*trans)->pos <= BLE_LOG_FRAME_HEAD_LEN) {
        return;
    }

    uint16_t payload_len = (*trans)->pos - BLE_LOG_FRAME_HEAD_LEN;
    ble_log_stat_mgr_t *stat_mgr = stat_mgr_ctx[src_code];
    uint32_t frame_sn = BLE_LOG_GET_FRAME_SN(&(stat_mgr->frame_sn));
    ble_log_frame_head_t frame_head = {
        .length = payload_len,
        .frame_meta = BLE_LOG_MAKE_FRAME_META(src_code, frame_sn),
    };
    BLE_LOG_MEMCPY((*trans)->buf, &frame_head, BLE_LOG_FRAME_HEAD_LEN);

    uint32_t checksum = ble_log_fast_checksum((*trans)->buf, (*trans)->pos);
    BLE_LOG_MEMCPY((*trans)->buf + (*trans)->pos, &checksum, BLE_LOG_FRAME_TAIL_LEN);
    (*trans)->pos += BLE_LOG_FRAME_TAIL_LEN;

    ble_log_stat_mgr_update(src_code, payload_len, false);

    ble_log_rt_queue_trans(trans);
}
#endif /* BLE_LOG_UART_REDIR_ENABLED */

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_stat_mgr_update(ble_log_src_t src_code, uint32_t len, bool lost)
{
    /* Get statistic manager by source code */
    ble_log_stat_mgr_t *stat_mgr = stat_mgr_ctx[src_code];

    /* Update aligned counters */
    uint32_t bytes_cnt = len + BLE_LOG_FRAME_OVERHEAD;
    if (lost) {
        BLE_LOG_GET_FRAME_SN(&(stat_mgr->frame_sn));  /* consume SN for loss detection */
        stat_mgr->lost_frame_cnt++;
        stat_mgr->lost_bytes_cnt += bytes_cnt;
    } else {
        stat_mgr->written_frame_cnt++;
        stat_mgr->written_bytes_cnt += bytes_cnt;
    }
}

/* -------------------------- */
/*     INTERNAL INTERFACE     */
/* -------------------------- */
bool ble_log_lbm_init(void)
{
    /* Avoid double init */
    if (lbm_inited) {
        return true;
    }

    /* Initialize LBM context */
    lbm_ctx = (ble_log_lbm_ctx_t *)BLE_LOG_MALLOC(sizeof(ble_log_lbm_ctx_t));
    if (!lbm_ctx) {
        goto exit;
    }
    BLE_LOG_MEMSET(lbm_ctx, 0, sizeof(ble_log_lbm_ctx_t));

    /* Initialize peripheral transport for common LBMs */
    ble_log_lbm_t *lbm;
    for (int i = 0; i < BLE_LOG_LBM_COMMON_CNT; i++) {
        lbm = &(lbm_ctx->lbm_common_pool[i]);
        for (int j = 0; j < BLE_LOG_TRANS_BUF_CNT; j++) {
            if (!ble_log_prph_trans_init(&(lbm->trans[j]),
                                         BLE_LOG_TRANS_SIZE)) {
                goto exit;
            }
            lbm->trans[j]->owner = (void *)lbm;
        }
    }

    /* Initialize lock types for atomic pool */
    for (int i = 0; i < BLE_LOG_LBM_ATOMIC_CNT; i++) {
        lbm_ctx->atomic_pool[i].lock_type = BLE_LOG_LBM_LOCK_ATOMIC;
    }

    /* Initialize lock types for spin pool */
    for (int i = 0; i < BLE_LOG_LBM_SPIN_MAX; i++) {
        lbm_ctx->spin_pool[i].lock_type = BLE_LOG_LBM_LOCK_SPIN;
        portMUX_INITIALIZE(&(lbm_ctx->spin_pool[i].spin_lock));
    }

#if CONFIG_BLE_LOG_LL_ENABLED
    for (int i = 0; i < BLE_LOG_LBM_LL_MAX; i++) {
        lbm = &(lbm_ctx->lbm_ll_pool[i]);
        for (int j = 0; j < BLE_LOG_TRANS_BUF_CNT; j++) {
            if (!ble_log_prph_trans_init(&(lbm->trans[j]),
                                         BLE_LOG_TRANS_LL_SIZE)) {
                goto exit;
            }
            lbm->trans[j]->owner = (void *)lbm;
        }
    }

    /* Initialize lock types for LL pool */
    for (int i = 0; i < BLE_LOG_LBM_LL_MAX; i++) {
        lbm_ctx->lbm_ll_pool[i].lock_type = BLE_LOG_LBM_LOCK_NONE;
    }
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

    /* Initialize statistic manager context */
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        stat_mgr_ctx[i] = (ble_log_stat_mgr_t *)BLE_LOG_MALLOC(sizeof(ble_log_stat_mgr_t));
        if (!stat_mgr_ctx[i]) {
            goto exit;
        }
        BLE_LOG_MEMSET(stat_mgr_ctx[i], 0, sizeof(ble_log_stat_mgr_t));
    }

    /* Initialization done */
    lbm_ref_count = 0;
    lbm_inited = true;
    lbm_enabled = false;
    return true;

exit:
    ble_log_lbm_deinit();
    return false;
}

void ble_log_lbm_deinit(void)
{
    /* Set inited flag to false to prevent new references */
    lbm_inited = false;
    lbm_enabled = false;

    /* Disable module and wait for all references to be released */
    uint32_t time_waited = 0;
    while (lbm_ref_count > 0) {
        vTaskDelay(pdMS_TO_TICKS(1));
        BLE_LOG_ASSERT(time_waited++ < 1000);
    }

    /* Release statistic manager context */
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        if (stat_mgr_ctx[i]) {
            BLE_LOG_FREE(stat_mgr_ctx[i]);
            stat_mgr_ctx[i] = NULL;
        }
    }

    /* Release LBM */
    if (lbm_ctx) {
        /* Release peripheral transport for common pools */
        ble_log_lbm_t *lbm;
        for (int i = 0; i < BLE_LOG_LBM_CNT; i++) {
            lbm = &(lbm_ctx->lbm_pool[i]);
            for (int j = 0; j < BLE_LOG_TRANS_BUF_CNT; j++) {
                ble_log_prph_trans_deinit(&(lbm->trans[j]));
            }
        }

        /* Release LBM context */
        BLE_LOG_FREE(lbm_ctx);
        lbm_ctx = NULL;
    }
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t **ble_log_lbm_get_trans(ble_log_lbm_t *lbm, size_t log_len)
{
    /* Check if available buffer can contain incoming log */
    ble_log_prph_trans_t **trans;
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        trans = &(lbm->trans[lbm->trans_idx]);
        if (!__atomic_load_n(&(*trans)->prph_owned, __ATOMIC_ACQUIRE)) {
            /* Return if there's enough free space in current transport */
            if (BLE_LOG_TRANS_FREE_SPACE((*trans)) >= (log_len + BLE_LOG_FRAME_OVERHEAD)) {
                return trans;
            }

            /* Queue transport if there's insufficient free space */
            if ((*trans)->pos) {
                ble_log_rt_queue_trans(trans);
            }
        }

        /* Current transport unavailable, switch to the other */
        lbm->trans_idx = (lbm->trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
    }

    /* All buffers are unavailable */
    return NULL;
}

void ble_log_write_enh_stat(void)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_enabled) {
        goto deref;
    }

    /* Snapshot all sources under one critical section so the set of
     * counters is mutually consistent, then write outside the lock. */
    ble_log_enh_stat_t snapshots[BLE_LOG_SRC_MAX];
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        snapshots[i].int_src_code = BLE_LOG_INT_SRC_ENH_STAT;
        snapshots[i].src_code     = i;
    }
    BLE_LOG_ENTER_CRITICAL();
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        BLE_LOG_MEMCPY(&snapshots[i].written_frame_cnt,
                       &stat_mgr_ctx[i]->written_frame_cnt,
                       4 * sizeof(uint32_t));
    }
    BLE_LOG_EXIT_CRITICAL();

    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&snapshots[i], sizeof(ble_log_enh_stat_t));
    }

deref:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}

#if BLE_LOG_UART_REDIR_ENABLED
/* ------------------------------------------------- */
/*     STREAM WRITE INTERFACE                        */
/*                                                   */
/* Stream mode appends raw data into a transport     */
/* buffer with deferred frame encapsulation:         */
/*   - Header space is reserved on first write       */
/*   - Data is memcpy'd after the reserved header    */
/*   - Header and checksum are filled in at seal     */
/*                                                   */
/* get_trans(lbm, 0) reuse safety:                   */
/*                                                   */
/* get_trans auto-queues a buffer raw (no seal) when */
/* free_space < log_len + FRAME_OVERHEAD.  With      */
/* log_len = 0, this triggers at free_space < 10.    */
/*                                                   */
/* To prevent unsealed stream data from being sent   */
/* raw, stream_write auto-seals at free_space <= 10. */
/* This guarantees that any unsealed stream buffer   */
/* seen by get_trans always has free_space > 10,     */
/* so get_trans returns it directly without queuing.  */
/* ------------------------------------------------- */
BLE_LOG_IRAM_ATTR
void ble_log_lbm_stream_write(ble_log_lbm_t *lbm, ble_log_src_t src_code,
                               const uint8_t *data, size_t len)
{
    while (len > 0) {
        ble_log_prph_trans_t **trans = ble_log_lbm_get_trans(lbm, 0);
        if (!trans) {
            ble_log_stat_mgr_update(src_code, len, true);
            return;
        }

        if ((*trans)->pos == 0) {
            (*trans)->pos = BLE_LOG_FRAME_HEAD_LEN;
        }

        uint16_t available = BLE_LOG_TRANS_FREE_SPACE((*trans));
        if (available <= BLE_LOG_FRAME_TAIL_LEN) {
            ble_log_lbm_stream_seal(trans, src_code);
            continue;
        }
        available -= BLE_LOG_FRAME_TAIL_LEN;

        size_t to_write = (len < available) ? len : available;
        BLE_LOG_MEMCPY((*trans)->buf + (*trans)->pos, data, to_write);
        (*trans)->pos += to_write;
        data += to_write;
        len -= to_write;

        if (BLE_LOG_TRANS_FREE_SPACE((*trans)) <= BLE_LOG_FRAME_OVERHEAD) {
            ble_log_lbm_stream_seal(trans, src_code);
        }
    }
}

BLE_LOG_IRAM_ATTR
void ble_log_lbm_stream_flush(ble_log_lbm_t *lbm, ble_log_src_t src_code)
{
    int trans_idx = lbm->trans_idx;
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        ble_log_prph_trans_t **trans = &(lbm->trans[trans_idx]);
        if (!__atomic_load_n(&(*trans)->prph_owned, __ATOMIC_ACQUIRE) &&
            (*trans)->pos > BLE_LOG_FRAME_HEAD_LEN) {
            ble_log_lbm_stream_seal(trans, src_code);
        }
        trans_idx = (trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
    }
}
#endif /* BLE_LOG_UART_REDIR_ENABLED */

BLE_LOG_STATIC void ble_log_emit_buf_util(ble_log_lbm_t *lbm, uint8_t lbm_id)
{
    ble_log_buf_util_t util = {
        .int_src_code   = BLE_LOG_INT_SRC_BUF_UTIL,
        .lbm_id         = lbm_id,
        .trans_cnt      = BLE_LOG_TRANS_BUF_CNT,
        .inflight_peak  = (uint8_t)__atomic_load_n(
                              &lbm->trans_inflight_peak, __ATOMIC_RELAXED),
    };
    ble_log_write_hex(BLE_LOG_SRC_INTERNAL,
                      (const uint8_t *)&util, sizeof(ble_log_buf_util_t));
}

void ble_log_write_buf_util(void)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_enabled) {
        goto deref;
    }

    ble_log_emit_buf_util(&lbm_ctx->spin_task,
        BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_COMMON_TASK, 0));
    for (int i = 0; i < BLE_LOG_LBM_ATOMIC_TASK_CNT; i++) {
        ble_log_emit_buf_util(&lbm_ctx->atomic_pool_task[i],
            BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_COMMON_TASK, 1 + i));
    }

    ble_log_emit_buf_util(&lbm_ctx->spin_isr,
        BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_COMMON_ISR, 0));
    for (int i = 0; i < BLE_LOG_LBM_ATOMIC_ISR_CNT; i++) {
        ble_log_emit_buf_util(&lbm_ctx->atomic_pool_isr[i],
            BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_COMMON_ISR, 1 + i));
    }

#if CONFIG_BLE_LOG_LL_ENABLED
    ble_log_emit_buf_util(&lbm_ctx->lbm_ll_task,
        BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_LL, 0));
    ble_log_emit_buf_util(&lbm_ctx->lbm_ll_hci,
        BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_LL, 1));
#endif

#if BLE_LOG_UART_REDIR_ENABLED
    ble_log_lbm_t *redir_lbm = ble_log_prph_get_redir_lbm();
    if (redir_lbm) {
        ble_log_emit_buf_util(redir_lbm,
            BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_REDIR, 0));
    }
#endif

deref:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}

/* ------------------------ */
/*     PUBLIC INTERFACE     */
/* ------------------------ */
bool ble_log_enable(bool enable)
{
    if (!lbm_inited) {
        return false;
    }
    lbm_enabled = enable;
    return true;
}

void ble_log_flush(void)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_inited) {
        goto deref;
    }

    /* Write enhanced statistics before module disable */
    ble_log_write_enh_stat();
    ble_log_write_buf_util();

    /* Write BLE Log flush log */
    ble_log_info_t ble_log_info = {
        .int_src_code = BLE_LOG_INT_SRC_FLUSH,
        .version = BLE_LOG_VERSION,
    };
    ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&ble_log_info, sizeof(ble_log_info_t));

    /* Disable module and wait for all other references to release */
    bool lbm_enabled_copy = lbm_enabled;
    lbm_enabled = false;
    uint32_t time_waited = 0;
    while (lbm_ref_count > 1) {
        vTaskDelay(pdMS_TO_TICKS(1));
        BLE_LOG_ASSERT(time_waited++ < 1000);
    }

    /* Queue transports with logs */
    ble_log_lbm_t *lbm;
    ble_log_prph_trans_t **trans;

    /* Flush pools */
    for (int i = 0; i < BLE_LOG_LBM_CNT; i++) {
        lbm = &(lbm_ctx->lbm_pool[i]);
        int trans_idx = lbm->trans_idx;
        for (int j = 0; j < BLE_LOG_TRANS_BUF_CNT; j++) {
            trans = &(lbm->trans[trans_idx]);
            if (!__atomic_load_n(&(*trans)->prph_owned, __ATOMIC_ACQUIRE) &&
                (*trans)->pos) {
                ble_log_rt_queue_trans(trans);
            }
            trans_idx = (trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
        }
    }

    /* Wait for transportation to finish */
    time_waited = 0;
    bool in_progress;
    do {
        in_progress = false;
        for (int i = 0; i < BLE_LOG_LBM_CNT; i++) {
            lbm = &(lbm_ctx->lbm_pool[i]);
            for (int j = 0; j < BLE_LOG_TRANS_BUF_CNT; j++) {
                trans = &(lbm->trans[j]);
                in_progress |= __atomic_load_n(&(*trans)->prph_owned, __ATOMIC_ACQUIRE);
            }
        }
        if (in_progress) {
            vTaskDelay(pdMS_TO_TICKS(1));
            BLE_LOG_ASSERT(time_waited++ < 1000);
        }
    } while (in_progress);

    /* Reset statistics manager after all operations complete */
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        BLE_LOG_MEMSET(stat_mgr_ctx[i], 0, sizeof(ble_log_stat_mgr_t));
    }

    for (int i = 0; i < BLE_LOG_LBM_CNT; i++) {
        lbm = &(lbm_ctx->lbm_pool[i]);
        __atomic_store_n(&lbm->trans_inflight, 0, __ATOMIC_RELAXED);
        __atomic_store_n(&lbm->trans_inflight_peak, 0, __ATOMIC_RELAXED);
    }
    ble_log_prph_reset_util_counters();

    /* Resume enable status */
    lbm_enabled = lbm_enabled_copy;

deref:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}

BLE_LOG_IRAM_ATTR
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_enabled) {
        goto exit;
    }

    /* Get transport */
    size_t payload_len = len + sizeof(uint32_t);
    ble_log_lbm_t *lbm = ble_log_lbm_acquire();
    ble_log_prph_trans_t **trans = ble_log_lbm_get_trans(lbm, payload_len);
    if (!trans) {
        ble_log_lbm_release(lbm);
        goto failed;
    }

    /* Write transport */
    uint32_t os_ts = pdTICKS_TO_MS(BLE_LOG_IN_ISR()?
                                   xTaskGetTickCountFromISR():
                                   xTaskGetTickCount());
    ble_log_lbm_write_trans(trans, src_code, (const uint8_t *)&os_ts,
                            sizeof(uint32_t), addr, len, false);

    /* Release */
    ble_log_lbm_release(lbm);
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return true;

failed:
    if (lbm_inited) {
        ble_log_stat_mgr_update(src_code, payload_len, true);
    }
exit:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return false;
}

#if CONFIG_BLE_LOG_LL_ENABLED
BLE_LOG_IRAM_ATTR
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, uint32_t flag)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_enabled) {
        goto exit;
    }

    /* Source code shall be determined before LBM determination */
    ble_log_src_t src_code = BLE_LOG_SRC_MAX;
    bool use_ll_task = false;
    if (flag & BIT(BLE_LOG_LL_FLAG_ISR)) {
        src_code = BLE_LOG_SRC_LL_ISR;
    } else if (flag & BIT(BLE_LOG_LL_FLAG_HCI)) {
        src_code = BLE_LOG_SRC_LL_HCI;
    } else if (flag & BIT(BLE_LOG_LL_FLAG_HCI_UPSTREAM)) {
        src_code = BLE_LOG_SRC_HCI;
    } else {
        src_code = BLE_LOG_SRC_LL_TASK;
        use_ll_task = true;
    }
    bool omdata = flag & BIT(BLE_LOG_LL_FLAG_OMDATA);

    /* Determine LBM by flag */
    ble_log_lbm_t *lbm;
    if (BLE_LOG_IN_ISR()) {
        /* Reuse common LBM acquire logic */
        lbm = ble_log_lbm_acquire();

        /* os_mbuf_copydata is in flash and not safe to call from ISR */
        omdata = false;
    } else {
        if (use_ll_task) {
            lbm = &(lbm_ctx->lbm_ll_task);
        } else {
            lbm = &(lbm_ctx->lbm_ll_hci);
#if CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT_ENABLED
            if (len_append > CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT) {
                len_append = CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT;
            }
#endif /* CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT_ENABLED */
        }
    }

    /* Get transport */
    size_t payload_len = len + len_append;
    ble_log_prph_trans_t **trans = ble_log_lbm_get_trans(lbm, payload_len);
    if (!trans) {
        ble_log_lbm_release(lbm);
        goto failed;
    }

    /* Write transport */
    ble_log_lbm_write_trans(trans, src_code, addr, len, addr_append, len_append, omdata);

    ble_log_lbm_release(lbm);
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return;

failed:
    if (lbm_inited) {
        ble_log_stat_mgr_update(src_code, payload_len, true);
    }
exit:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return;
}
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

void ble_log_dump_to_console(void)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_inited) {
        goto deref;
    }

    int trans_idx;
    ble_log_lbm_t *lbm;
    ble_log_prph_trans_t *trans;
    BLE_LOG_ENTER_CRITICAL();
    BLE_LOG_CONSOLE("[BLE_LOG_DUMP_START:\n");
    for (int i = 0; i < BLE_LOG_LBM_CNT; i++) {
        lbm = &(lbm_ctx->lbm_pool[i]);
        trans_idx = lbm->trans_idx;
        for (int j = 0; j < BLE_LOG_TRANS_BUF_CNT; j++) {
            trans = lbm->trans[trans_idx];
            BLE_LOG_FEED_WDT();

            for (int k = 0; k < trans->size; k++) {
                BLE_LOG_CONSOLE("%02x ", trans->buf[k]);
                if (!(k & 0xFF)) {
                    BLE_LOG_FEED_WDT();
                }
            }
            trans_idx = (trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
        }
    }
    BLE_LOG_CONSOLE("\n:BLE_LOG_DUMP_END]\n\n");
    BLE_LOG_EXIT_CRITICAL();

deref:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return;
}
