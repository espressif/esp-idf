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

#include "esp_timer.h"

#if CONFIG_SOC_ESP_NIMBLE_CONTROLLER
#if CONFIG_BT_DUAL_MODE_ARCH
#include "ble_mbuf.h"
#define BLE_MBUF_COPY(buf, off, len, dst) ble_mbuf_copydata((struct ble_mbuf *)(buf), off, len, dst)
#else
#include "os/os_mbuf.h"
#define BLE_MBUF_COPY(buf, off, len, dst) os_mbuf_copydata((struct os_mbuf *)(buf), off, len, dst)
#endif // CONFIG_BT_DUAL_MODE_ARCH
#endif /* CONFIG_SOC_ESP_NIMBLE_CONTROLLER */

/* MACRO */
#if CONFIG_BLE_LOG_POOL_TASK_WAIT_TIMEOUT_MS > 0
#define BLE_LOG_POOL_TASK_WAIT_TICKS            pdMS_TO_TICKS(CONFIG_BLE_LOG_POOL_TASK_WAIT_TIMEOUT_MS)
#else
#define BLE_LOG_POOL_TASK_WAIT_TICKS            portMAX_DELAY
#endif

#if CONFIG_BLE_LOG_POOL_ISR_OPEN_SCAN_MAX > 0
#define BLE_LOG_POOL_ISR_OPEN_SCAN_MAX          CONFIG_BLE_LOG_POOL_ISR_OPEN_SCAN_MAX
#else
#define BLE_LOG_POOL_ISR_OPEN_SCAN_MAX          BLE_LOG_POOL_TRANS_CNT
#endif

/* ------------------------------- */
/*     Global Pool Context         */
/* ------------------------------- */
typedef struct {
    ble_log_prph_trans_t *trans[BLE_LOG_POOL_TRANS_CNT];

    /* Hint bitmaps: a set bit means "this buffer is likely FREE/OPEN". The
     * real ownership is the per-buffer atomic_lock; bitmaps only accelerate
     * candidate lookup and may be transiently stale. */
    volatile uint32_t free_bitmap;
    volatile uint32_t open_bitmap;

    /* Domain masks (static after init). */
    uint32_t shared_mask;
    uint32_t isr_reserve_mask;

    /* Round-robin scan cursors. */
    volatile uint32_t open_cursor;
    volatile uint32_t free_cursor;

    /* In-flight (SENDING) pool buffers and its peak. */
    volatile uint32_t inflight;
    volatile uint32_t inflight_peak;

    /* Number of task writers currently blocked waiting for a buffer. */
    volatile uint32_t waiting_task_count;

    /* Event-only counting semaphore used to wake blocked task writers.
     * Not a lock: it carries no ownership and never protects shared data. */
    SemaphoreHandle_t sem;

    /* Debug counters. */
    volatile uint32_t send_fail_cnt;        /* wrote a buffer but driver submit failed */
    volatile uint32_t lost_too_large_cnt;   /* frame larger than one transport buffer */
    volatile uint32_t lost_no_buf_task_cnt; /* task could not get a buffer (drop, not block) */
    volatile uint32_t lost_no_buf_isr_cnt;  /* ISR could not get a buffer */
    volatile uint32_t task_wait_cnt;
    volatile uint32_t task_wait_max_us;
    volatile uint32_t task_wait_total_us;

    /* Baseline of aggregated lost counters captured at the last
     * ble_log_pool_stats_reset(), so stats_get() reports per-window deltas. */
    uint32_t lost_frame_base;
    uint32_t lost_bytes_base;
} ble_log_pool_t;

/* VARIABLE */
BLE_LOG_STATIC volatile uint32_t lbm_ref_count = 0;
BLE_LOG_STATIC bool lbm_inited = false;
BLE_LOG_STATIC bool lbm_enabled = false;
BLE_LOG_STATIC ble_log_pool_t g_pool;
BLE_LOG_STATIC ble_log_stat_mgr_t *stat_mgr_ctx[BLE_LOG_SRC_MAX] = {0};

BLE_LOG_STATIC uint32_t write_attempt_bytes = 0;
BLE_LOG_STATIC uint32_t write_attempt_bytes_ll = 0;
BLE_LOG_STATIC ble_log_prph_latency_stat_t prph_owned_latency_stats = {0};
BLE_LOG_STATIC ble_log_prph_latency_stat_t prph_spi_latency_stats = {0};
BLE_LOG_STATIC ble_log_prph_latency_stat_t prph_queue_wait_latency_stats = {0};
BLE_LOG_STATIC ble_log_prph_latency_stat_t prph_dma_latency_stats = {0};

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC ble_log_prph_trans_t *ble_log_pool_acquire(size_t log_len, bool is_isr);
BLE_LOG_STATIC void ble_log_pool_seal_and_send(ble_log_prph_trans_t *trans);
BLE_LOG_STATIC
void ble_log_pool_write_frame(ble_log_prph_trans_t *trans, ble_log_src_t src_code,
                              const uint8_t *addr, uint16_t len,
                              const uint8_t *addr_append, uint16_t len_append, bool omdata);
BLE_LOG_STATIC void ble_log_write_attempt_bytes_add(uint32_t bytes);
BLE_LOG_STATIC void ble_log_write_attempt_bytes_add_ll(uint32_t bytes);
BLE_LOG_STATIC void ble_log_stat_mgr_update(ble_log_src_t src_code, uint32_t len, bool lost);
BLE_LOG_STATIC void ble_log_pool_wake_all(void);
#if BLE_LOG_UART_REDIR_ENABLED
BLE_LOG_STATIC void ble_log_redir_seal(ble_log_prph_trans_t *trans, ble_log_src_t src_code);
#endif /* BLE_LOG_UART_REDIR_ENABLED */

/* ------------------------- */
/*     BITMAP HELPERS        */
/* ------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC BLE_LOG_INLINE
void ble_log_pool_bitmap_set(volatile uint32_t *bitmap, uint8_t id)
{
    __atomic_fetch_or(bitmap, (1u << id), __ATOMIC_RELEASE);
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC BLE_LOG_INLINE
void ble_log_pool_bitmap_clear(volatile uint32_t *bitmap, uint8_t id)
{
    __atomic_fetch_and(bitmap, ~(1u << id), __ATOMIC_RELEASE);
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC BLE_LOG_INLINE
bool ble_log_pool_id_task_usable(uint8_t id)
{
    return (g_pool.shared_mask & (1u << id)) != 0;
}

/* ------------------------- */
/*     WAKE HELPERS          */
/* ------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void ble_log_pool_wake_one(void)
{
    if (!g_pool.sem) {
        return;
    }
    if (BLE_LOG_IN_ISR()) {
        BaseType_t woken = pdFALSE;
        xSemaphoreGiveFromISR(g_pool.sem, &woken);
        if (woken) {
            portYIELD_FROM_ISR(woken);
        }
    } else {
        xSemaphoreGive(g_pool.sem);
    }
}

/* Task context only: release all potentially-blocked task writers. */
BLE_LOG_STATIC void ble_log_pool_wake_all(void)
{
    if (!g_pool.sem) {
        return;
    }
    for (int i = 0; i < BLE_LOG_POOL_TRANS_CNT; i++) {
        /* May fail with pdFALSE when the semaphore is already at max; that is
         * harmless — a token per possible waiter is enough to release them. */
        (void)xSemaphoreGive(g_pool.sem);
    }
}

/* -------------------------------------- */
/*     UNIFIED TRANSPORT RECYCLE          */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR void ble_log_lbm_recycle_trans(ble_log_prph_trans_t *trans)
{
    trans->pos = 0;

    if (trans->id == BLE_LOG_TRANS_ID_NONE) {
        /* Redirection transport: not part of the pool, no bitmap/inflight. */
        __atomic_store_n(&trans->state, BLE_LOG_TRANS_STATE_FREE, __ATOMIC_RELEASE);
        return;
    }

    __atomic_sub_fetch(&g_pool.inflight, 1, __ATOMIC_RELAXED);

    /* CRITICAL store order: publish FREE state before advertising the buffer
     * in free_bitmap. An allocator that sees the hint bit will acquire the
     * lock and re-check state; the release on free_bitmap pairs with the
     * allocator's acquire so it never observes a stale non-FREE state. */
    __atomic_store_n(&trans->state, BLE_LOG_TRANS_STATE_FREE, __ATOMIC_RELEASE);
    ble_log_pool_bitmap_set(&g_pool.free_bitmap, trans->id);

    if (ble_log_pool_id_task_usable(trans->id) &&
        __atomic_load_n(&g_pool.waiting_task_count, __ATOMIC_ACQUIRE) > 0) {
        ble_log_pool_wake_one();
    }
}

void ble_log_lbm_note_send_fail(void)
{
    __atomic_fetch_add(&g_pool.send_fail_cnt, 1, __ATOMIC_RELAXED);
}

/* -------------------------------------- */
/*     SEAL AND SUBMIT (holds lock)       */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void ble_log_pool_seal_and_send(ble_log_prph_trans_t *trans)
{
    __atomic_store_n(&trans->state, BLE_LOG_TRANS_STATE_SENDING, __ATOMIC_RELAXED);
    /* Defensive: a SENDING buffer must not linger in any hint bitmap. */
    ble_log_pool_bitmap_clear(&g_pool.open_bitmap, trans->id);

    uint32_t infl = __atomic_add_fetch(&g_pool.inflight, 1, __ATOMIC_RELAXED);
    uint32_t peak = __atomic_load_n(&g_pool.inflight_peak, __ATOMIC_RELAXED);
    while (infl > peak) {
        if (__atomic_compare_exchange_n(&g_pool.inflight_peak, &peak, infl,
                                        true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            break;
        }
    }

    /* Release the lock BEFORE submitting. The buffer is now in no bitmap and
     * is SENDING, so no other writer can find it; submitting outside the lock
     * avoids racing tx_done against the lock owner. */
    ble_log_cas_release(&trans->atomic_lock);
    ble_log_rt_queue_trans(trans);
}

/* -------------------------------------- */
/*     OPEN BUFFER LOOKUP                 */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_try_find_open(uint32_t domain_mask, uint32_t frame_len, bool is_isr)
{
    uint32_t bm = __atomic_load_n(&g_pool.open_bitmap, __ATOMIC_ACQUIRE) & domain_mask;
    if (!bm) {
        return NULL;
    }

    uint32_t start = __atomic_load_n(&g_pool.open_cursor, __ATOMIC_RELAXED);
    int scan_limit = is_isr ? BLE_LOG_POOL_ISR_OPEN_SCAN_MAX : BLE_LOG_POOL_TRANS_CNT;
    int scanned = 0;

    for (int i = 0; i < BLE_LOG_POOL_TRANS_CNT; i++) {
        uint8_t id = (uint8_t)((start + i) % BLE_LOG_POOL_TRANS_CNT);
        if (!(bm & (1u << id))) {
            continue;
        }
        if (scanned++ >= scan_limit) {
            break;
        }

        ble_log_prph_trans_t *trans = g_pool.trans[id];
        /* Ownership: hint bit alone is not enough, take the lock. */
        if (!ble_log_cas_acquire(&trans->atomic_lock)) {
            continue;
        }
        if (__atomic_load_n(&trans->state, __ATOMIC_RELAXED) != BLE_LOG_TRANS_STATE_OPEN) {
            /* Stale hint: leave the bitmap alone, owner will maintain it. */
            ble_log_cas_release(&trans->atomic_lock);
            continue;
        }

        if (BLE_LOG_TRANS_FREE_SPACE(trans) >= frame_len) {
            ble_log_pool_bitmap_clear(&g_pool.open_bitmap, id);
            __atomic_store_n(&g_pool.open_cursor,
                             (id + 1u) % BLE_LOG_POOL_TRANS_CNT, __ATOMIC_RELAXED);
            return trans;   /* returned locked */
        }

        if (trans->pos > 0) {
            ble_log_pool_bitmap_clear(&g_pool.open_bitmap, id);
            ble_log_pool_seal_and_send(trans);   /* releases the lock */
            continue;
        }

        /* Abnormal: OPEN yet empty. Revert to FREE defensively. */
        __atomic_store_n(&trans->state, BLE_LOG_TRANS_STATE_FREE, __ATOMIC_RELEASE);
        ble_log_pool_bitmap_set(&g_pool.free_bitmap, id);
        ble_log_pool_bitmap_clear(&g_pool.open_bitmap, id);
        ble_log_cas_release(&trans->atomic_lock);
    }

    return NULL;
}

/* -------------------------------------- */
/*     FREE BUFFER ALLOCATION             */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_alloc_from_mask(uint32_t mask)
{
    uint32_t bm = __atomic_load_n(&g_pool.free_bitmap, __ATOMIC_ACQUIRE) & mask;
    if (!bm) {
        return NULL;
    }

    uint32_t start = __atomic_load_n(&g_pool.free_cursor, __ATOMIC_RELAXED);
    for (int i = 0; i < BLE_LOG_POOL_TRANS_CNT; i++) {
        uint8_t id = (uint8_t)((start + i) % BLE_LOG_POOL_TRANS_CNT);
        if (!(bm & (1u << id))) {
            continue;
        }

        ble_log_prph_trans_t *trans = g_pool.trans[id];
        if (!ble_log_cas_acquire(&trans->atomic_lock)) {
            /* Contended hint: do not touch the bitmap. */
            continue;
        }
        if (__atomic_load_n(&trans->state, __ATOMIC_RELAXED) != BLE_LOG_TRANS_STATE_FREE) {
            /* Stale hint: do not touch the bitmap, owner maintains it. */
            ble_log_cas_release(&trans->atomic_lock);
            continue;
        }

        /* Confirmed owner: only now clear the hint bit. */
        ble_log_pool_bitmap_clear(&g_pool.free_bitmap, id);
        trans->pos = 0;
        __atomic_store_n(&g_pool.free_cursor,
                         (id + 1u) % BLE_LOG_POOL_TRANS_CNT, __ATOMIC_RELAXED);
        return trans;   /* returned locked */
    }

    return NULL;
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_try_alloc_free(bool is_isr)
{
    ble_log_prph_trans_t *trans = ble_log_pool_alloc_from_mask(g_pool.shared_mask);
    if (trans) {
        return trans;
    }
    if (is_isr) {
        trans = ble_log_pool_alloc_from_mask(g_pool.isr_reserve_mask);
        if (trans) {
            return trans;
        }
    }
    return NULL;
}

/* -------------------------------------- */
/*     ACQUIRE (with task backpressure)   */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_acquire(size_t log_len, bool is_isr)
{
    uint32_t frame_len = (uint32_t)log_len + BLE_LOG_FRAME_OVERHEAD;
    if (frame_len > BLE_LOG_POOL_TRANS_SIZE) {
        /* Frame can never fit a single transport; drop it. */
        __atomic_fetch_add(&g_pool.lost_too_large_cnt, 1, __ATOMIC_RELAXED);
        return NULL;
    }

    uint32_t open_mask = is_isr ? (g_pool.shared_mask | g_pool.isr_reserve_mask)
                                : g_pool.shared_mask;

    /* Task writers may block for a buffer; ISR / scheduler-suspended / the
     * runtime task itself must never block (would stall recycling). */
    bool can_block = !is_isr &&
                     (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) &&
                     !ble_log_rt_in_self_task();

    ble_log_prph_trans_t *trans;
    for (;;) {
        trans = ble_log_pool_try_find_open(open_mask, frame_len, is_isr);
        if (trans) {
            return trans;
        }
        trans = ble_log_pool_try_alloc_free(is_isr);
        if (trans) {
            return trans;
        }

        if (!can_block || !lbm_enabled) {
            __atomic_fetch_add(is_isr ? &g_pool.lost_no_buf_isr_cnt
                                      : &g_pool.lost_no_buf_task_cnt,
                               1, __ATOMIC_RELAXED);
            return NULL;
        }

        __atomic_add_fetch(&g_pool.waiting_task_count, 1, __ATOMIC_ACQ_REL);

        /* Second scan closes the window between the first failed scan and the
         * waiting_task_count increment (a tx_done may have recycled a buffer
         * without waking anyone). */
        trans = ble_log_pool_try_find_open(open_mask, frame_len, is_isr);
        if (!trans) {
            trans = ble_log_pool_try_alloc_free(is_isr);
        }
        if (trans) {
            __atomic_sub_fetch(&g_pool.waiting_task_count, 1, __ATOMIC_ACQ_REL);
            return trans;
        }

        /* Block without holding a module reference so flush/deinit can drain
         * ref_count. Re-acquire the reference before touching pool state. */
        uint64_t wait_start_us = esp_timer_get_time();
        BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
        xSemaphoreTake(g_pool.sem, BLE_LOG_POOL_TASK_WAIT_TICKS);
        BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
        __atomic_sub_fetch(&g_pool.waiting_task_count, 1, __ATOMIC_ACQ_REL);

        uint64_t waited_us = esp_timer_get_time() - wait_start_us;
        uint32_t w = waited_us > UINT32_MAX ? UINT32_MAX : (uint32_t)waited_us;
        __atomic_fetch_add(&g_pool.task_wait_cnt, 1, __ATOMIC_RELAXED);
        __atomic_fetch_add(&g_pool.task_wait_total_us, w, __ATOMIC_RELAXED);
        uint32_t wmax = __atomic_load_n(&g_pool.task_wait_max_us, __ATOMIC_RELAXED);
        while (w > wmax) {
            if (__atomic_compare_exchange_n(&g_pool.task_wait_max_us, &wmax, w,
                                            true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
                break;
            }
        }

        if (!lbm_enabled) {
            /* Pool is closing (disable/flush/deinit woke us). Give up.
             * Only task context reaches here (can_block implies !is_isr). */
            __atomic_fetch_add(&g_pool.lost_no_buf_task_cnt, 1, __ATOMIC_RELAXED);
            return NULL;
        }
        /* Otherwise loop and retry the scan. */
    }
}

/* -------------------------------------- */
/*     WRITE ONE FRAME (holds lock)       */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_pool_write_frame(ble_log_prph_trans_t *trans, ble_log_src_t src_code,
                              const uint8_t *addr, uint16_t len,
                              const uint8_t *addr_append, uint16_t len_append, bool omdata)
{
    uint8_t *buf = trans->buf + trans->pos;
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
            BLE_MBUF_COPY(addr_append, 0, len_append, buf + BLE_LOG_FRAME_HEAD_LEN + len);
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

    /* Update transport */
    trans->pos += payload_len + BLE_LOG_FRAME_OVERHEAD;
    ble_log_stat_mgr_update(src_code, payload_len, false);

    /* Completion: seal if nearly full, otherwise publish as OPEN. */
    if (BLE_LOG_TRANS_FREE_SPACE(trans) <= BLE_LOG_FRAME_OVERHEAD) {
        ble_log_pool_seal_and_send(trans);      /* releases the lock */
    } else {
        __atomic_store_n(&trans->state, BLE_LOG_TRANS_STATE_OPEN, __ATOMIC_RELAXED);
        ble_log_pool_bitmap_set(&g_pool.open_bitmap, trans->id);
        ble_log_cas_release(&trans->atomic_lock);
    }
}

#if BLE_LOG_UART_REDIR_ENABLED
/* ------------------------------------------------- */
/*     STREAM WRITE INTERFACE (UART redirection)     */
/*                                                   */
/* Stream mode appends raw data into a transport     */
/* buffer with deferred frame encapsulation.         */
/* Redirection transports are single-writer under    */
/* redir->mutex; their only concurrent mutation is   */
/* the UART tx-done recycle (state SENDING -> FREE),  */
/* so state access uses atomics.                     */
/* ------------------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_redir_get_trans(ble_log_redir_t *redir, ble_log_src_t src_code, size_t log_len)
{
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        ble_log_prph_trans_t *trans = redir->trans[redir->trans_idx];
        if (__atomic_load_n(&trans->state, __ATOMIC_ACQUIRE) != BLE_LOG_TRANS_STATE_SENDING) {
            if (BLE_LOG_TRANS_FREE_SPACE(trans) >= (log_len + BLE_LOG_FRAME_OVERHEAD)) {
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

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_redir_seal(ble_log_prph_trans_t *trans, ble_log_src_t src_code)
{
    if (trans->pos <= BLE_LOG_FRAME_HEAD_LEN) {
        return;
    }

    uint16_t payload_len = trans->pos - BLE_LOG_FRAME_HEAD_LEN;
    ble_log_stat_mgr_t *stat_mgr = stat_mgr_ctx[src_code];
    uint32_t frame_sn = BLE_LOG_GET_FRAME_SN(&(stat_mgr->frame_sn));
    ble_log_frame_head_t frame_head = {
        .length = payload_len,
        .frame_meta = BLE_LOG_MAKE_FRAME_META(src_code, frame_sn),
    };
    BLE_LOG_MEMCPY(trans->buf, &frame_head, BLE_LOG_FRAME_HEAD_LEN);

    uint32_t checksum = ble_log_fast_checksum(trans->buf, trans->pos);
    BLE_LOG_MEMCPY(trans->buf + trans->pos, &checksum, BLE_LOG_FRAME_TAIL_LEN);
    trans->pos += BLE_LOG_FRAME_TAIL_LEN;

    ble_log_stat_mgr_update(src_code, payload_len, false);

    __atomic_store_n(&trans->state, BLE_LOG_TRANS_STATE_SENDING, __ATOMIC_RELAXED);
    ble_log_rt_queue_trans(trans);
}

BLE_LOG_IRAM_ATTR
void ble_log_lbm_stream_write(ble_log_redir_t *redir, ble_log_src_t src_code,
                               const uint8_t *data, size_t len)
{
    while (len > 0) {
        ble_log_prph_trans_t *trans = ble_log_redir_get_trans(redir, src_code, 0);
        if (!trans) {
            ble_log_stat_mgr_update(src_code, len, true);
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

BLE_LOG_IRAM_ATTR
void ble_log_lbm_stream_flush(ble_log_redir_t *redir, ble_log_src_t src_code)
{
    int trans_idx = redir->trans_idx;
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        ble_log_prph_trans_t *trans = redir->trans[trans_idx];
        if (__atomic_load_n(&trans->state, __ATOMIC_ACQUIRE) != BLE_LOG_TRANS_STATE_SENDING &&
            trans->pos > BLE_LOG_FRAME_HEAD_LEN) {
            ble_log_redir_seal(trans, src_code);
        }
        trans_idx = (trans_idx + 1) & (BLE_LOG_TRANS_BUF_CNT - 1);
    }
}
#endif /* BLE_LOG_UART_REDIR_ENABLED */

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_stat_mgr_update(ble_log_src_t src_code, uint32_t len, bool lost)
{
    ble_log_stat_mgr_t *stat_mgr = stat_mgr_ctx[src_code];

    uint32_t bytes_cnt = len + BLE_LOG_FRAME_OVERHEAD;
    if (lost) {
        BLE_LOG_GET_FRAME_SN(&(stat_mgr->frame_sn));  /* consume SN for loss detection */
        __atomic_fetch_add(&stat_mgr->lost_frame_cnt, 1, __ATOMIC_RELAXED);
        __atomic_fetch_add(&stat_mgr->lost_bytes_cnt, bytes_cnt, __ATOMIC_RELAXED);
    } else {
        __atomic_fetch_add(&stat_mgr->written_frame_cnt, 1, __ATOMIC_RELAXED);
        __atomic_fetch_add(&stat_mgr->written_bytes_cnt, bytes_cnt, __ATOMIC_RELAXED);
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

    BLE_LOG_MEMSET(&g_pool, 0, sizeof(g_pool));

    /* Counting semaphore used only to wake blocked task writers. Max equals
     * the number of task-usable (shared) buffers; initial count is 0. */
    g_pool.sem = xSemaphoreCreateCounting(BLE_LOG_POOL_SHARED_CNT, 0);
    if (!g_pool.sem) {
        goto exit;
    }

    /* Allocate the transport buffers of the global pool. */
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        if (!ble_log_prph_trans_init(&(g_pool.trans[id]), BLE_LOG_POOL_TRANS_SIZE)) {
            goto exit;
        }
        g_pool.trans[id]->id = (uint8_t)id;
        g_pool.trans[id]->state = BLE_LOG_TRANS_STATE_FREE;
        g_pool.trans[id]->atomic_lock = false;
    }

    /* Domain masks: low SHARED_CNT bits are shared, the rest is ISR reserve. */
    uint32_t all_mask = (BLE_LOG_POOL_TRANS_CNT >= 32)
                        ? 0xFFFFFFFFu : ((1u << BLE_LOG_POOL_TRANS_CNT) - 1u);
    uint32_t shared_mask = (BLE_LOG_POOL_SHARED_CNT >= 32)
                           ? 0xFFFFFFFFu : ((1u << BLE_LOG_POOL_SHARED_CNT) - 1u);
    g_pool.shared_mask = shared_mask;
    g_pool.isr_reserve_mask = all_mask & ~shared_mask;
    g_pool.free_bitmap = all_mask;    /* every buffer starts FREE */
    g_pool.open_bitmap = 0;

    /* Initialize statistic manager context */
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        stat_mgr_ctx[i] = (ble_log_stat_mgr_t *)BLE_LOG_MALLOC(sizeof(ble_log_stat_mgr_t));
        if (!stat_mgr_ctx[i]) {
            goto exit;
        }
        BLE_LOG_MEMSET(stat_mgr_ctx[i], 0, sizeof(ble_log_stat_mgr_t));
    }

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

    /* Wake any blocked task writers and wait until BOTH the reference count
     * and the waiting-task count drain to zero. Blocked writers hold no
     * reference while parked, so waiting on ref_count alone could free the
     * pool while a woken task still touches it (use-after-free). */
    uint32_t time_waited = 0;
    while ((__atomic_load_n(&lbm_ref_count, __ATOMIC_ACQUIRE) > 0) ||
           (__atomic_load_n(&g_pool.waiting_task_count, __ATOMIC_ACQUIRE) > 0)) {
        ble_log_pool_wake_all();
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

    /* Release transport buffers */
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        ble_log_prph_trans_deinit(&(g_pool.trans[id]));
    }

    if (g_pool.sem) {
        vSemaphoreDelete(g_pool.sem);
        g_pool.sem = NULL;
    }
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

void ble_log_write_buf_util(void)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_enabled) {
        goto deref;
    }

    /* Single record representing the whole unified pool. */
    ble_log_buf_util_t util = {
        .int_src_code   = BLE_LOG_INT_SRC_BUF_UTIL,
        .lbm_id         = BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_COMMON_TASK, 0),
        .trans_cnt      = BLE_LOG_POOL_TRANS_CNT,
        .inflight_peak  = (uint8_t)__atomic_load_n(&g_pool.inflight_peak, __ATOMIC_RELAXED),
    };
    ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&util, sizeof(ble_log_buf_util_t));

#if BLE_LOG_UART_REDIR_ENABLED
    ble_log_redir_t *redir = ble_log_prph_get_redir_lbm();
    if (redir) {
        ble_log_buf_util_t redir_util = {
            .int_src_code   = BLE_LOG_INT_SRC_BUF_UTIL,
            .lbm_id         = BLE_LOG_BUF_UTIL_MAKE_ID(BLE_LOG_BUF_UTIL_POOL_REDIR, 0),
            .trans_cnt      = BLE_LOG_TRANS_BUF_CNT,
            .inflight_peak  = 0,
        };
        ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&redir_util, sizeof(ble_log_buf_util_t));
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
    if (!enable) {
        /* Release any blocked task writers so they can observe the disable. */
        ble_log_pool_wake_all();
    }
    return true;
}

void ble_log_flush(void)
{
    /* Prevent concurrent flush — two concurrent callers would deadlock on
     * the ref_count spin-wait (both hold a ref, both wait for ref_count <= 1).
     * Second caller returns immediately instead of deadlocking. */
    static volatile bool flush_in_progress = false;
    if (__atomic_test_and_set(&flush_in_progress, __ATOMIC_ACQUIRE)) {
        return;
    }

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

    /* Disable module and wait for all other references AND blocked writers to
     * release. Blocked task writers are woken so they exit the wait loop. */
    bool lbm_enabled_copy = lbm_enabled;
    lbm_enabled = false;
    uint32_t time_waited = 0;
    while ((__atomic_load_n(&lbm_ref_count, __ATOMIC_ACQUIRE) > 1) ||
           (__atomic_load_n(&g_pool.waiting_task_count, __ATOMIC_ACQUIRE) > 0)) {
        ble_log_pool_wake_all();
        vTaskDelay(pdMS_TO_TICKS(1));
        BLE_LOG_ASSERT(time_waited++ < 1000);
    }

    /* Seal every OPEN buffer. Do NOT skip a locked buffer (that would drop its
     * data); new writes are already stopped, so retry-acquire converges. */
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        ble_log_prph_trans_t *trans = g_pool.trans[id];
        while (!ble_log_cas_acquire(&trans->atomic_lock)) {
            /* A writer holds the lock only for a single-frame memcpy. */
        }
        if (__atomic_load_n(&trans->state, __ATOMIC_RELAXED) == BLE_LOG_TRANS_STATE_OPEN &&
            trans->pos > 0) {
            ble_log_pool_bitmap_clear(&g_pool.open_bitmap, id);
            ble_log_pool_seal_and_send(trans);   /* releases the lock */
        } else {
            ble_log_cas_release(&trans->atomic_lock);
        }
    }

    /* Wait for all in-flight transports to complete. */
    time_waited = 0;
    while (__atomic_load_n(&g_pool.inflight, __ATOMIC_ACQUIRE) != 0) {
        vTaskDelay(pdMS_TO_TICKS(1));
        BLE_LOG_ASSERT(time_waited++ < 1000);
    }

    /* Reset statistics manager after all operations complete */
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        BLE_LOG_MEMSET(stat_mgr_ctx[i], 0, sizeof(ble_log_stat_mgr_t));
    }
    __atomic_store_n(&g_pool.inflight_peak, 0, __ATOMIC_RELAXED);
    ble_log_prph_reset_util_counters();

    /* Resume enable status */
    lbm_enabled = lbm_enabled_copy;

deref:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    __atomic_clear(&flush_in_progress, __ATOMIC_RELEASE);
}

BLE_LOG_IRAM_ATTR
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len)
{
    BLE_LOG_REF_COUNT_ACQUIRE(&lbm_ref_count);
    if (!lbm_enabled) {
        goto exit;
    }

    bool is_isr = BLE_LOG_IN_ISR();

    /* Payload is [os_ts][user data] */
    size_t payload_len = len + sizeof(uint32_t);
    ble_log_write_attempt_bytes_add(payload_len);

    ble_log_prph_trans_t *trans = ble_log_pool_acquire(payload_len, is_isr);
    if (!trans) {
        goto failed;
    }

    uint32_t os_ts = pdTICKS_TO_MS(is_isr ? xTaskGetTickCountFromISR() : xTaskGetTickCount());
    ble_log_pool_write_frame(trans, src_code, (const uint8_t *)&os_ts,
                             sizeof(uint32_t), addr, len, false);

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return true;

failed:
    if (lbm_inited) {
        ble_log_stat_mgr_update(src_code, len + sizeof(uint32_t), true);
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

    /* Determine source code from flags */
    ble_log_src_t src_code;
    if (flag & BIT(BLE_LOG_LL_FLAG_ISR)) {
        src_code = BLE_LOG_SRC_LL_ISR;
    } else if (flag & BIT(BLE_LOG_LL_FLAG_HCI)) {
        src_code = BLE_LOG_SRC_LL_HCI;
    } else if (flag & BIT(BLE_LOG_LL_FLAG_HCI_UPSTREAM)) {
        src_code = BLE_LOG_SRC_HCI;
    } else {
        src_code = BLE_LOG_SRC_LL_TASK;
    }
    bool omdata = flag & BIT(BLE_LOG_LL_FLAG_OMDATA);
    bool is_isr = BLE_LOG_IN_ISR();

    if (is_isr) {
        /* os_mbuf_copydata is in flash and not safe to call from ISR */
        omdata = false;
    }
#if CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT_ENABLED
    else if (src_code == BLE_LOG_SRC_LL_HCI) {
        if (len_append > CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT) {
            len_append = CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT;
        }
    }
#endif /* CONFIG_BLE_LOG_LL_HCI_LOG_PAYLOAD_LEN_LIMIT_ENABLED */

    size_t payload_len = len + len_append;
    ble_log_write_attempt_bytes_add_ll(payload_len);

    ble_log_prph_trans_t *trans = ble_log_pool_acquire(payload_len, is_isr);
    if (!trans) {
        goto failed;
    }

    ble_log_pool_write_frame(trans, src_code, addr, len, addr_append, len_append, omdata);

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return;

failed:
    if (lbm_inited) {
        ble_log_stat_mgr_update(src_code, len + len_append, true);
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

    BLE_LOG_ENTER_CRITICAL();
    BLE_LOG_CONSOLE("[BLE_LOG_DUMP_START:\n");
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        ble_log_prph_trans_t *trans = g_pool.trans[id];
        BLE_LOG_FEED_WDT();
        for (int k = 0; k < trans->size; k++) {
            BLE_LOG_CONSOLE("%02x ", trans->buf[k]);
            if (!(k & 0xFF)) {
                BLE_LOG_FEED_WDT();
            }
        }
    }
    BLE_LOG_CONSOLE("\n:BLE_LOG_DUMP_END]\n\n");
    BLE_LOG_EXIT_CRITICAL();

deref:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return;
}

void ble_log_write_attempt_bytes_reset(void)
{
    __atomic_store_n(&write_attempt_bytes, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&write_attempt_bytes_ll, 0, __ATOMIC_RELAXED);
}

void ble_log_write_attempt_bytes_get(uint32_t *bytes, uint32_t *bytes_ll)
{
    *bytes = __atomic_load_n(&write_attempt_bytes, __ATOMIC_RELAXED);
    *bytes_ll = __atomic_load_n(&write_attempt_bytes_ll, __ATOMIC_RELAXED);
}

BLE_LOG_STATIC void ble_log_write_attempt_bytes_add(uint32_t bytes)
{
    __atomic_fetch_add(&write_attempt_bytes, bytes, __ATOMIC_RELAXED);
}

BLE_LOG_STATIC void ble_log_write_attempt_bytes_add_ll(uint32_t bytes)
{
    __atomic_fetch_add(&write_attempt_bytes_ll, bytes, __ATOMIC_RELAXED);
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC uint32_t ble_log_latency_us_saturate(uint64_t latency_us)
{
    return latency_us > UINT32_MAX ? UINT32_MAX : (uint32_t)latency_us;
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void ble_log_latency_stat_update(ble_log_prph_latency_stat_t *stat,
                                                                  uint64_t latency_us)
{
    uint32_t latency_us_32 = ble_log_latency_us_saturate(latency_us);

    if (stat->count == 0 || latency_us_32 < stat->min_us) {
        stat->min_us = latency_us_32;
    }
    if (latency_us_32 > stat->max_us) {
        stat->max_us = latency_us_32;
    }
    stat->last_us = latency_us_32;
    stat->total_us += latency_us_32;
    stat->count += 1;
}

BLE_LOG_IRAM_ATTR void ble_log_prph_latency_stats_update(uint64_t owned_latency_us, bool owned_valid,
                                                         uint64_t spi_latency_us, bool spi_valid,
                                                         uint64_t queue_wait_us, bool queue_wait_valid,
                                                         uint64_t dma_time_us, bool dma_time_valid)
{
    if (!owned_valid && !spi_valid && !queue_wait_valid && !dma_time_valid) {
        return;
    }

    BLE_LOG_ENTER_CRITICAL();
    if (owned_valid) {
        ble_log_latency_stat_update(&prph_owned_latency_stats, owned_latency_us);
    }
    if (spi_valid) {
        ble_log_latency_stat_update(&prph_spi_latency_stats, spi_latency_us);
    }
    if (queue_wait_valid) {
        ble_log_latency_stat_update(&prph_queue_wait_latency_stats, queue_wait_us);
    }
    if (dma_time_valid) {
        ble_log_latency_stat_update(&prph_dma_latency_stats, dma_time_us);
    }
    BLE_LOG_EXIT_CRITICAL();
}

void ble_log_prph_latency_stats_reset(void)
{
    BLE_LOG_ENTER_CRITICAL();
    BLE_LOG_MEMSET(&prph_owned_latency_stats, 0, sizeof(prph_owned_latency_stats));
    BLE_LOG_MEMSET(&prph_spi_latency_stats, 0, sizeof(prph_spi_latency_stats));
    BLE_LOG_MEMSET(&prph_queue_wait_latency_stats, 0, sizeof(prph_queue_wait_latency_stats));
    BLE_LOG_MEMSET(&prph_dma_latency_stats, 0, sizeof(prph_dma_latency_stats));
    BLE_LOG_EXIT_CRITICAL();
}

void ble_log_prph_latency_stats_get(ble_log_prph_latency_stats_t *stats)
{
    if (stats == NULL) {
        return;
    }

    BLE_LOG_ENTER_CRITICAL();
    stats->owned = prph_owned_latency_stats;
    stats->spi = prph_spi_latency_stats;
    stats->queue_wait = prph_queue_wait_latency_stats;
    stats->dma = prph_dma_latency_stats;
    BLE_LOG_EXIT_CRITICAL();
}

/* Aggregate the per-source lost counters. Caller holds the module lock. */
BLE_LOG_STATIC void ble_log_pool_sum_lost(uint32_t *frames, uint32_t *bytes)
{
    uint32_t f = 0;
    uint32_t b = 0;
    if (lbm_inited) {
        for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
            if (stat_mgr_ctx[i]) {
                f += stat_mgr_ctx[i]->lost_frame_cnt;
                b += stat_mgr_ctx[i]->lost_bytes_cnt;
            }
        }
    }
    *frames = f;
    *bytes = b;
}

void ble_log_pool_stats_get(ble_log_pool_stats_t *stats)
{
    if (stats == NULL) {
        return;
    }

    BLE_LOG_MEMSET(stats, 0, sizeof(*stats));

    BLE_LOG_ENTER_CRITICAL();
    stats->task_wait_count    = g_pool.task_wait_cnt;
    stats->task_wait_max_us   = g_pool.task_wait_max_us;
    stats->task_wait_total_us = g_pool.task_wait_total_us;
    stats->inflight_peak      = g_pool.inflight_peak;
    stats->send_fail_count    = g_pool.send_fail_cnt;
    stats->lost_frame_too_large = g_pool.lost_too_large_cnt;
    stats->lost_no_buffer_task  = g_pool.lost_no_buf_task_cnt;
    stats->lost_no_buffer_isr   = g_pool.lost_no_buf_isr_cnt;

    /* Lost counters are reported as a per-window delta relative to the
     * baseline snapshotted at the last reset. If a ble_log_flush() zeroed the
     * underlying counters mid-window (current < baseline), fall back to the
     * current value instead of underflowing. */
    uint32_t cur_frames;
    uint32_t cur_bytes;
    ble_log_pool_sum_lost(&cur_frames, &cur_bytes);
    stats->lost_frame_cnt = (cur_frames >= g_pool.lost_frame_base)
                            ? (cur_frames - g_pool.lost_frame_base) : cur_frames;
    stats->lost_bytes_cnt = (cur_bytes >= g_pool.lost_bytes_base)
                            ? (cur_bytes - g_pool.lost_bytes_base) : cur_bytes;
    BLE_LOG_EXIT_CRITICAL();
}

void ble_log_pool_stats_reset(void)
{
    /* Reset the pool measurement counters (not on the wire) and snapshot the
     * current aggregated lost counters as the per-window baseline. */
    BLE_LOG_ENTER_CRITICAL();
    g_pool.task_wait_cnt = 0;
    g_pool.task_wait_max_us = 0;
    g_pool.task_wait_total_us = 0;
    g_pool.send_fail_cnt = 0;
    g_pool.lost_too_large_cnt = 0;
    g_pool.lost_no_buf_task_cnt = 0;
    g_pool.lost_no_buf_isr_cnt = 0;
    g_pool.inflight_peak = 0;
    ble_log_pool_sum_lost(&g_pool.lost_frame_base, &g_pool.lost_bytes_base);
    BLE_LOG_EXIT_CRITICAL();
}
