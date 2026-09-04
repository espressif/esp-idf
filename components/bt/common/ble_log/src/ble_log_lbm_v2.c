/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ------------------------------- */
/* BLE Log - Log Buffer Management */
/* ------------------------------- */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_task_registry.h"
#include "ble_log_rt.h"

#include "esp_timer.h"
#include "esp_chip_info.h"

#if CONFIG_BLE_LOG_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER
#if CONFIG_BT_DUAL_MODE_ARCH
#include "ble_mbuf.h"
#define BLE_MBUF_COPY(buf, off, len, dst) ble_mbuf_copydata((struct ble_mbuf *)(buf), off, len, dst)
#else
#include "os/os_mbuf.h"
#define BLE_MBUF_COPY(buf, off, len, dst) os_mbuf_copydata((struct os_mbuf *)(buf), off, len, dst)
#endif // CONFIG_BT_DUAL_MODE_ARCH
#endif /* CONFIG_BLE_LOG_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER */

/* MACRO */
#define BLE_LOG_POOL_MASK(count)                   (0xFFFFFFFFu >> (32 - (count)))
#define BLE_LOG_POOL_ALL_MASK                      BLE_LOG_POOL_MASK(BLE_LOG_POOL_TRANS_CNT)
#define BLE_LOG_POOL_SHARED_MASK                   BLE_LOG_POOL_MASK(BLE_LOG_POOL_SHARED_CNT)
#define BLE_LOG_POOL_RESERVE_MASK                  (BLE_LOG_POOL_ALL_MASK & ~BLE_LOG_POOL_SHARED_MASK)
#define BLE_LOG_POOL_SEM_MAX_COUNT                 (~(UBaseType_t)0)
#define BLE_LOG_WAIT_TIMEOUT_TICKS                 pdMS_TO_TICKS(1000)

/* Single-instruction clock read; a function would add an IRAM call site. */
#define BLE_LOG_TIMESTAMP_NOW()                    ((uint32_t)esp_timer_get_time())

#if CONFIG_BT_CONTROLLER_ENABLED
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
extern const char *btdm_controller_get_compile_version(void);
#define BLE_LOG_CONTROLLER_GET_COMMIT() btdm_controller_get_compile_version()
#elif !CONFIG_BT_DUAL_MODE_ARCH || CONFIG_BT_CTRL_BLE_ENABLE
/* BR/EDR-only dual-mode builds do not link the BLE controller lib */
extern char *ble_controller_get_compile_version(void);
#define BLE_LOG_CONTROLLER_GET_COMMIT() ble_controller_get_compile_version()
#endif
#if CONFIG_BT_DUAL_MODE_ARCH
/* BTDM common lib (dual-mode arch only) */
extern const char *r_btdm_get_compile_version(void);
#define BLE_LOG_BTDM_COMMON_GET_COMMIT() r_btdm_get_compile_version()
#endif
#endif

#if CONFIG_BLE_MESH && CONFIG_BLE_MESH_V11_SUPPORT
/* "Bluetooth Mesh v1.1 commit: <hash>" */
extern const char bt_mesh_v11_commit_str[];
#endif

#if CONFIG_BT_AUDIO && CONFIG_SOC_BLE_AUDIO_SUPPORTED
extern const char *lib_audio_commit_get(void);
#endif

_Static_assert(sizeof(ble_log_version_info_t) == 58,
               "Unexpected BLE Log version info frame size");

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

    /* Round-robin scan cursors. */
    volatile uint32_t open_cursor;
    volatile uint32_t free_cursor;

    /* Peak number of non-free pool buffers. */
    volatile uint32_t inflight_peak;

    /* Number of task writers currently blocked waiting for a buffer. */
    volatile uint32_t waiting_task_count;

    /* Event-only counting semaphore used to wake blocked task writers.
     * Not a lock: it carries no ownership and never protects shared data. */
    SemaphoreHandle_t sem;
} ble_log_pool_t;

/* ------------------------------- */
/*     Claim/Commit Bookkeeping    */
/* ------------------------------- */
/* State that must survive between ble_log_claim() and ble_log_commit(). It
 * lives with the pool, not in ble_log_prph_trans_t: the peripheral transport
 * only needs buf/size/pos and the lifecycle state for DMA handoff. */
typedef struct {
    uint8_t src_code;      /* source of the pending claim */
    uint16_t max_len;      /* maximum committed payload length */
    uint32_t frame_sn;     /* Global SN consumed at claim entry */
    uint32_t generation;   /* stale-handle detection, wraps 24-bit */
} ble_log_pool_claim_t;

/* VARIABLE */
/* Process-lifetime count: a caller rejected by a closed gate may still owe
 * its balancing release when the next LBM epoch starts. Never reset it. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR volatile uint32_t lbm_ref_count = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t lbm_inited = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t lbm_enabled = 0;
BLE_LOG_STATIC volatile bool flush_in_progress = false;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR ble_log_pool_t g_pool;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR ble_log_stat_mgr_t stat_mgr_ctx[BLE_LOG_SRC_MAX];
/* Global SN (the log sources except INTERNAL and REDIR) and the separate
 * Internal Snapshot sequence; see ble_log_lbm_v2.h. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t g_frame_sn;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t g_snapshot_sn;
#define BLE_LOG_GET_GLOBAL_SN()                 BLE_LOG_GET_FRAME_SN(g_frame_sn)
#define BLE_LOG_GET_SNAPSHOT_SN()               BLE_LOG_GET_FRAME_SN(g_snapshot_sn)
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR ble_log_prph_trans_t *internal_trans;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR ble_log_pool_claim_t pool_claim_ctx[BLE_LOG_POOL_TRANS_CNT];
BLE_LOG_STATIC ble_log_internal_snapshot_t internal_snapshot;

#if CONFIG_BLE_LOG_PRPH_TEST
extern void ble_log_test_claim_pre_publish_hook(void) __attribute__((weak));
extern void ble_log_test_enable_before_lifecycle_lock_hook(void) __attribute__((weak));
extern void ble_log_test_disable_before_wake_hook(void) __attribute__((weak));
extern void ble_log_test_claim_locked_hook(void) __attribute__((weak));
extern void ble_log_test_init_snapshot_before_acquire_hook(void) __attribute__((weak));
#endif

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_lbm_disable(void)
{
    BLE_LOG_ATOMIC_STORE_SEQ_CST(lbm_enabled, false);
    /* Pairs with the waiter-side fence so either wake_all observes the waiter
     * or the waiter observes the disabled flag before blocking. */
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
}

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC ble_log_prph_trans_t *ble_log_pool_acquire(size_t log_len,
                                                            bool use_reserve,
                                                            bool wait);
BLE_LOG_STATIC void ble_log_pool_seal_and_send(ble_log_prph_trans_t *trans);
BLE_LOG_STATIC void ble_log_pool_seal_open_trans(void);
BLE_LOG_STATIC void ble_log_pool_write_frame(ble_log_prph_trans_t *trans,
                                             uint32_t frame_sn,
                                             ble_log_src_t src_code,
                                             const uint8_t *prefix,
                                             uint16_t prefix_len,
                                             const uint8_t *addr, uint16_t len,
                                             const uint8_t *addr_append,
                                             uint16_t len_append, bool omdata);
BLE_LOG_STATIC void ble_log_pool_finish_frame(ble_log_prph_trans_t *trans,
                                              uint16_t payload_len,
                                              ble_log_stat_mgr_t *stat_mgr);
BLE_LOG_STATIC void ble_log_stat_mgr_mark_lost(ble_log_src_t src_code);
BLE_LOG_STATIC void ble_log_snapshot_stats(ble_log_source_stat_t *snapshots);
BLE_LOG_STATIC bool ble_log_pool_flush_all_trans(void);
BLE_LOG_STATIC void ble_log_pool_wake_all(void);
BLE_LOG_STATIC void ble_log_internal_version_info_init(void);

/* ------------------------- */
/*     BITMAP HELPERS        */
/* ------------------------- */
/* Non-inline: each helper is called from several IRAM sites, and inlining
 * would duplicate its body into every call site (IRAM is the scarce
 * resource, not the call cost). */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void
ble_log_pool_bitmap_set(volatile uint32_t *bitmap, uint8_t id)
{
    __atomic_fetch_or(bitmap, BIT(id), __ATOMIC_RELEASE);
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
uint32_t ble_log_pool_bitmap_clear(volatile uint32_t *bitmap, uint8_t id)
{
    /* Clearing a hint publishes no data; ownership is already held by lock. */
    return __atomic_fetch_and(bitmap, ~BIT(id), __ATOMIC_RELAXED);
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void
ble_log_pool_update_peak(uint32_t free_bitmap)
{
    uint32_t used = BLE_LOG_POOL_TRANS_CNT -
                    __builtin_popcount(free_bitmap & BLE_LOG_POOL_ALL_MASK);
    ble_log_atomic_update_peak(&g_pool.inflight_peak, used);
}

/* A newly available shared transport satisfies one waiter. The SEQ_CST fence
 * pairs with the waiter-side store/fence/load handshake: either the waiter
 * observes the bitmap publication or this side observes the waiter count. */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void ble_log_pool_notify_waiter(uint8_t id)
{
    /* Reserve transports never satisfy task waiters. */
    if (!(BLE_LOG_POOL_SHARED_MASK & BIT(id))) {
        return;
    }

    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    SemaphoreHandle_t sem = g_pool.sem;
    /* The fence above provides ordering; the count is only a wake predicate. */
    if (!sem ||
        BLE_LOG_ATOMIC_LOAD_RELAXED(g_pool.waiting_task_count) == 0) {
        return;
    }

    if (BLE_LOG_IN_ISR()) {
        BaseType_t woken = pdFALSE;
        xSemaphoreGiveFromISR(sem, &woken);
        if (woken) {
            portYIELD_FROM_ISR(woken);
        }
    } else {
        xSemaphoreGive(sem);
    }
}

/* Publish OPEN and release ownership as one operation so notification can
 * never be moved before the lock release. */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void
ble_log_pool_publish_open_and_unlock(ble_log_prph_trans_t *trans)
{
    BLE_LOG_ATOMIC_STORE_RELAXED(trans->state, BLE_LOG_TRANS_STATE_OPEN);
    /* The following lock release publishes both state and frame data. */
    __atomic_fetch_or(&g_pool.open_bitmap, BIT(trans->id), __ATOMIC_RELAXED);
    BLE_LOG_CAS_RELEASE(&trans->atomic_lock);
    ble_log_pool_notify_waiter(trans->id);
}

/* Task context only: release all potentially-blocked task writers. */
BLE_LOG_STATIC void ble_log_pool_wake_all(void)
{
    if (!g_pool.sem) {
        return;
    }
    uint32_t waiters = BLE_LOG_ATOMIC_LOAD_RELAXED(g_pool.waiting_task_count);
    for (uint32_t i = 0; i < waiters; i++) {
        (void)xSemaphoreGive(g_pool.sem);
    }
}

/* Waiter registration is one atomic RMW with ACQ_REL on both sides of the
 * park: the register must be visible to a concurrent gate close before the
 * task blocks, and the unregister must pair with the wake that released
 * it. The RMW is a CAS loop plus barrier on Xtensa (several instructions),
 * so it lives here once instead of at the four call sites of the IRAM
 * acquire path. delta is +1 or -1. */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void
ble_log_pool_waiter_adjust(int delta)
{
    __atomic_add_fetch(&g_pool.waiting_task_count, (uint32_t)delta,
                       __ATOMIC_ACQ_REL);
}

/* -------------------------------------- */
/*     UNIFIED TRANSPORT RECYCLE          */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR void ble_log_lbm_recycle_trans(ble_log_prph_trans_t *trans)
{
    trans->pos = 0;
    /* A marker set while the transport was SENDING must not leak into its
     * next lifecycle. */
    BLE_LOG_ATOMIC_STORE_RELAXED(trans->pending_seal, false);

    if (trans->owner_kind == BLE_LOG_TRANS_OWNER_INTERNAL) {
        BLE_LOG_ATOMIC_STORE_RELEASE(trans->state, BLE_LOG_TRANS_STATE_FREE);
        return;
    }

    if (trans->owner_kind == BLE_LOG_TRANS_OWNER_REDIR) {
#if BLE_LOG_UART_REDIR_ENABLED
        ble_log_redir_t *redir = ble_log_prph_get_redir_lbm();
        if (redir) {
            __atomic_sub_fetch(&redir->inflight, 1, __ATOMIC_RELAXED);
        }
#endif
        BLE_LOG_ATOMIC_STORE_RELEASE(trans->state, BLE_LOG_TRANS_STATE_FREE);
        return;
    }

    /* Publish FREE state before advertising the pool bitmap hint. */
    BLE_LOG_ATOMIC_STORE_RELEASE(trans->state, BLE_LOG_TRANS_STATE_FREE);
    ble_log_pool_bitmap_set(&g_pool.free_bitmap, trans->id);
    ble_log_pool_notify_waiter(trans->id);
}

/* -------------------------------------- */
/*     SEAL AND SUBMIT (holds lock)       */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR void ble_log_pool_seal_and_send(ble_log_prph_trans_t *trans)
{
    /* Consumes any pending-seal marker: every seal path (capacity, claim,
     * periodic flush, full drain) funnels through here while holding the
     * lock. */
    BLE_LOG_ATOMIC_STORE_RELAXED(trans->pending_seal, false);
    BLE_LOG_ATOMIC_STORE_RELAXED(trans->state, BLE_LOG_TRANS_STATE_SENDING);
    /* A claimed buffer is already absent from free_bitmap. Remove any OPEN
     * hint before releasing the SENDING buffer to the runtime task. */
    ble_log_pool_bitmap_clear(&g_pool.open_bitmap, trans->id);

    /* Release the lock BEFORE submitting. The buffer is now in no bitmap and
     * is SENDING, so no other writer can find it. */
    BLE_LOG_CAS_RELEASE(&trans->atomic_lock);
    ble_log_rt_submit_trans(trans);
}

/* Seals every OPEN transport in the pool. Caller contract: writers are
 * gone (producer gate closed and drained), so each lock is uncontended
 * and the acquire waits are bounded. */
BLE_LOG_STATIC void ble_log_pool_seal_open_trans(void)
{
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        if (!(BLE_LOG_ATOMIC_LOAD_ACQUIRE(g_pool.open_bitmap) & BIT(id))) {
            continue;
        }
        ble_log_prph_trans_t *trans = g_pool.trans[id];
        while (!BLE_LOG_CAS_ACQUIRE(&trans->atomic_lock)) {
        }
        if (BLE_LOG_ATOMIC_LOAD_RELAXED(trans->state) == BLE_LOG_TRANS_STATE_OPEN &&
            trans->pos > 0) {
            ble_log_pool_seal_and_send(trans);   /* releases the lock */
        } else {
            BLE_LOG_CAS_RELEASE(&trans->atomic_lock);
        }
    }
}

/* -------------------------------------- */
/*     AVAILABLE BUFFER CLAIM             */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_try_claim_from(volatile uint32_t *bitmap,
                                                  volatile uint32_t *cursor,
                                                  uint32_t domain_mask,
                                                  ble_log_trans_state_t expected_state,
                                                  uint32_t frame_len)
{
    uint32_t candidates = BLE_LOG_ATOMIC_LOAD_ACQUIRE(*bitmap) & domain_mask;
    if (!candidates) {
        return NULL;
    }

    uint32_t start = BLE_LOG_ATOMIC_LOAD_RELAXED(*cursor);

    for (uint32_t offset = 0; offset < BLE_LOG_POOL_TRANS_CNT; offset++) {
        uint8_t id = (uint8_t)((start + offset) % BLE_LOG_POOL_TRANS_CNT);
        if (!(candidates & BIT(id))) {
            continue;
        }

        ble_log_prph_trans_t *trans = g_pool.trans[id];
        if (!BLE_LOG_CAS_ACQUIRE(&trans->atomic_lock)) {
            continue;
        }
        if (BLE_LOG_ATOMIC_LOAD_RELAXED(trans->state) != expected_state) {
            /* The bitmap is only a hint; another owner may have changed state. */
            BLE_LOG_CAS_RELEASE(&trans->atomic_lock);
            continue;
        }

        if (expected_state == BLE_LOG_TRANS_STATE_OPEN &&
            (BLE_LOG_TRANS_FREE_SPACE(trans) < frame_len ||
             BLE_LOG_ATOMIC_LOAD_ACQUIRE(trans->pending_seal))) {
            /* No room for this frame, or a periodic flush left a
             * pending-seal marker while the transport was busy: send the
             * buffered frames and scan on for another transport. */
            ble_log_pool_seal_and_send(trans);   /* releases the lock */
            continue;
        }

        uint32_t previous = ble_log_pool_bitmap_clear(bitmap, id);
        if (bitmap == &g_pool.free_bitmap) {
            ble_log_pool_update_peak(previous & ~BIT(id));
        }
        /* Keep packing the same OPEN transport instead of scanning the rest
         * of the pool before wrapping back to it on the next write. */
        uint32_t next = expected_state == BLE_LOG_TRANS_STATE_OPEN
                        ? id
                        : (id + 1u) % BLE_LOG_POOL_TRANS_CNT;
        BLE_LOG_ATOMIC_STORE_RELAXED(*cursor, next);
        return trans;   /* returned locked */
    }

    return NULL;
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_try_claim_available(uint32_t frame_len, bool use_reserve)
{
    uint32_t open_domain = use_reserve ? BLE_LOG_POOL_ALL_MASK : BLE_LOG_POOL_SHARED_MASK;

    /* The cursor normally points at the OPEN transport used by the previous
     * write. Claim it directly; fall back to bitmap scanning on contention,
     * stale state, or insufficient space. */
    uint8_t open_id = (uint8_t)BLE_LOG_ATOMIC_LOAD_RELAXED(g_pool.open_cursor);
    if (open_domain & BIT(open_id)) {
        ble_log_prph_trans_t *open_trans = g_pool.trans[open_id];
        if (BLE_LOG_CAS_ACQUIRE(&open_trans->atomic_lock)) {
            if (BLE_LOG_ATOMIC_LOAD_RELAXED(open_trans->state) == BLE_LOG_TRANS_STATE_OPEN) {
                /* A pending-seal marker left by a periodic flush that lost
                 * the lock race sends the buffered frames first; this claim
                 * continues with another transport below. */
                if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(open_trans->pending_seal) &&
                    BLE_LOG_TRANS_FREE_SPACE(open_trans) >= frame_len) {
#if CONFIG_BLE_LOG_PRPH_TEST
                    /* Test point: the transport is locked and still listed
                     * in open_bitmap, mirroring an in-progress writer for a
                     * concurrent flush attempt. */
                    if (ble_log_test_claim_locked_hook) {
                        ble_log_test_claim_locked_hook();
                    }
#endif
                    ble_log_pool_bitmap_clear(&g_pool.open_bitmap, open_id);
                    return open_trans;
                }
                ble_log_pool_seal_and_send(open_trans);   /* releases the lock */
            } else {
                BLE_LOG_CAS_RELEASE(&open_trans->atomic_lock);
            }
        }
    }

    /* Prefer partially-filled buffers to preserve packing efficiency. */
    ble_log_prph_trans_t *trans =
        ble_log_pool_try_claim_from(&g_pool.open_bitmap, &g_pool.open_cursor,
                                    open_domain, BLE_LOG_TRANS_STATE_OPEN,
                                    frame_len);
    if (trans) {
        return trans;
    }

    trans = ble_log_pool_try_claim_from(&g_pool.free_bitmap, &g_pool.free_cursor,
                                        BLE_LOG_POOL_SHARED_MASK, BLE_LOG_TRANS_STATE_FREE,
                                        0);
    if (!trans && use_reserve) {
        trans = ble_log_pool_try_claim_from(&g_pool.free_bitmap, &g_pool.free_cursor,
                                            BLE_LOG_POOL_RESERVE_MASK, BLE_LOG_TRANS_STATE_FREE,
                                            0);
    }
    return trans;
}

/* -------------------------------------- */
/*     ACQUIRE (with task backpressure)   */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
ble_log_prph_trans_t *ble_log_pool_acquire(size_t log_len,
                                           bool use_reserve,
                                           bool wait)
{
    uint32_t frame_len = (uint32_t)log_len + BLE_LOG_FRAME_OVERHEAD;
    if (frame_len > BLE_LOG_POOL_TRANS_SIZE) {
        return NULL;
    }

    for (;;) {
        ble_log_prph_trans_t *trans =
            ble_log_pool_try_claim_available(frame_len, use_reserve);
        if (trans || !wait || !BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_enabled)) {
            return trans;
        }

        ble_log_pool_waiter_adjust(1);
        __atomic_thread_fence(__ATOMIC_SEQ_CST);

        trans = ble_log_pool_try_claim_available(frame_len, use_reserve);
        if (trans) {
            ble_log_pool_waiter_adjust(-1);
            return trans;
        }
        if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_enabled)) {
            ble_log_pool_waiter_adjust(-1);
            return NULL;
        }

        /* A parked LL task keeps its waiter registration but releases the
         * normal writer reference so flush/deinit can close the gate. */
        BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
        xSemaphoreTake(g_pool.sem, portMAX_DELAY);
        BLE_LOG_REF_COUNT_ACQUIRE_SEQ_CST(&lbm_ref_count);
        ble_log_pool_waiter_adjust(-1);

        if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_enabled)) {
            return NULL;
        }
    }
}

/* -------------------------------------- */
/*     WRITE ONE FRAME (holds lock)       */
/* -------------------------------------- */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_pool_write_frame(ble_log_prph_trans_t *trans, uint32_t frame_sn,
                              ble_log_src_t src_code,
                              const uint8_t *prefix, uint16_t prefix_len,
                              const uint8_t *addr, uint16_t len,
                              const uint8_t *addr_append, uint16_t len_append, bool omdata)
{
    uint8_t *buf = trans->buf + trans->pos;
    uint16_t payload_len = prefix_len + len + len_append;
    ble_log_stat_mgr_t *stat_mgr = &stat_mgr_ctx[src_code];
    ble_log_frame_head_t frame_head = {
        .length = payload_len,
        .frame_meta = BLE_LOG_MAKE_FRAME_META(src_code, frame_sn),
    };

    /* Memory operation */
    BLE_LOG_MEMCPY(buf, &frame_head, BLE_LOG_FRAME_HEAD_LEN);
    uint8_t *payload = buf + BLE_LOG_FRAME_HEAD_LEN;
    if (prefix_len) {
        BLE_LOG_MEMCPY(payload, prefix, prefix_len);
        payload += prefix_len;
    }
    if (len) {
        BLE_LOG_MEMCPY(payload, addr, len);
        payload += len;
    }
    if (len_append) {
#if CONFIG_BLE_LOG_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER
        if (omdata) {
            BLE_MBUF_COPY(addr_append, 0, len_append, payload);
        }
        else
#endif /* CONFIG_BLE_LOG_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER */
        {
            BLE_LOG_MEMCPY(payload, addr_append, len_append);
        }
    }
    /* Data integrity check, transport update and completion. */
    ble_log_pool_finish_frame(trans, payload_len, stat_mgr);
}

/* Completes a frame whose head and payload are already in place at
 * trans->pos: writes the checksum tail, advances pos, publishes the
 * written-frame stat, then seals or publishes the transport. */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_pool_finish_frame(ble_log_prph_trans_t *trans, uint16_t payload_len,
                               ble_log_stat_mgr_t *stat_mgr)
{
    uint8_t *buf = trans->buf + trans->pos;
    uint32_t checksum = ble_log_fast_checksum(buf, BLE_LOG_FRAME_HEAD_LEN + payload_len);
    BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN + payload_len, &checksum, BLE_LOG_FRAME_TAIL_LEN);

    trans->pos += payload_len + BLE_LOG_FRAME_OVERHEAD;
    BLE_LOG_ATOMIC_ADD_RELAXED(stat_mgr->counters.written_frame_cnt, 1);

    /* Completion: seal if nearly full, otherwise publish as OPEN. */
    if (BLE_LOG_TRANS_FREE_SPACE(trans) <= BLE_LOG_FRAME_OVERHEAD) {
        ble_log_pool_seal_and_send(trans);      /* releases the lock */
    } else {
        ble_log_pool_publish_open_and_unlock(trans);
    }
}

/* ---------------------------------------------- */
/*     CLAIM / COMMIT INTERFACE (holds lock)      */
/* ---------------------------------------------- */

/* Claim with a caller-chosen wait policy. wait_for_transport=true applies
 * backpressure in a yieldable context (the writer waits for a shared
 * transport instead of dropping); false is a lossy fast path that returns
 * NULL on a busy pool, for callers that must never block (e.g. system
 * periodic output on the shared ESP timer task). Non-yieldable contexts
 * (ISR, scheduler suspended) never wait whatever the policy: they keep
 * their dedicated reserve and fail fast on contention. */
uint8_t *ble_log_claim(ble_log_src_t src_code, size_t max_len,
                       uint32_t *handle, bool wait_for_transport)
{
    if (!handle || src_code != BLE_LOG_SRC_ENCODE || max_len == 0 ||
        max_len > UINT16_MAX || BLE_LOG_IN_ISR()) {
        return NULL;
    }
    *handle = UINT32_MAX;

    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_enabled)) {
        return NULL;
    }

    /* Every claim attempt consumes one Global SN at entry; an abandoned or
     * failed claim therefore leaves a gap. */
    uint32_t frame_sn = BLE_LOG_GET_GLOBAL_SN();

    /* The timestamp is the log occurrence time, before pool contention. */
    uint32_t timestamp = BLE_LOG_TIMESTAMP_NOW();
    bool non_yield = !xPortCanYield() ||
                     xTaskGetSchedulerState() != taskSCHEDULER_RUNNING;
    size_t payload_capacity = sizeof(timestamp) + max_len;
    /* Non-yieldable contexts never wait, whatever the requested policy:
     * blocking there would suspend the only context that can free the
     * transport. wait_for_transport=false is the lossy fast path for
     * yieldable contexts that must not block. */
    ble_log_prph_trans_t *trans = ble_log_pool_acquire(
        payload_capacity, non_yield, wait_for_transport && !non_yield);
    if (!trans) {
        ble_log_stat_mgr_mark_lost(src_code);
        BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
        return NULL;
    }

#if CONFIG_BLE_LOG_PRPH_TEST
    if (ble_log_test_claim_pre_publish_hook) {
        ble_log_test_claim_pre_publish_hook();
    }
#endif
    ble_log_pool_claim_t *claim = &pool_claim_ctx[trans->id];
    claim->src_code = (uint8_t)src_code;
    claim->max_len = (uint16_t)max_len;
    claim->frame_sn = frame_sn;
    claim->generation = (claim->generation + 1) & 0x00ffffffU;

    uint8_t *payload = trans->buf + trans->pos + BLE_LOG_FRAME_HEAD_LEN;
    BLE_LOG_MEMCPY(payload, &timestamp, sizeof(timestamp));

    /* Publish only after all claim metadata and payload prefix are complete.
     * An acquire load in commit then rejects stale generations without ever
     * observing a partially initialized claim. */
    BLE_LOG_ATOMIC_STORE_RELEASE(trans->state, BLE_LOG_TRANS_STATE_CLAIMED);
    *handle = (claim->generation << 8) | trans->id;
    return payload + sizeof(timestamp);
}

void ble_log_commit(uint32_t handle, size_t actual_len)
{
    uint8_t id = handle & 0xffU;
    uint32_t generation = handle >> 8;
    if (id >= BLE_LOG_POOL_TRANS_CNT) {
        return;
    }
    ble_log_prph_trans_t *trans = g_pool.trans[id];
    ble_log_pool_claim_t *claim = &pool_claim_ctx[id];
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(trans->state) !=
            BLE_LOG_TRANS_STATE_CLAIMED ||
        claim->generation != generation) {
        return;
    }

    ble_log_src_t src_code = (ble_log_src_t)claim->src_code;
    if (actual_len == 0 || actual_len > claim->max_len) {
        ble_log_stat_mgr_mark_lost(src_code);
        if (trans->pos == 0) {
            /* Returning straight to FREE: recycle covers pos, pending_seal,
             * state, the free-bitmap hint and the waiter wake. The transport
             * is in no bitmap while CLAIMED, so releasing the lock first
             * keeps it invisible to scanners until recycle publishes it. */
            BLE_LOG_CAS_RELEASE(&trans->atomic_lock);
            ble_log_lbm_recycle_trans(trans);
        } else {
            ble_log_pool_publish_open_and_unlock(trans);
        }
        BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
        return;
    }

    uint16_t payload_len = (uint16_t)(sizeof(uint32_t) + actual_len);
    ble_log_stat_mgr_t *stat_mgr = &stat_mgr_ctx[src_code];
    ble_log_frame_head_t frame_head = {
        .length = payload_len,
        .frame_meta = BLE_LOG_MAKE_FRAME_META(src_code, claim->frame_sn),
    };
    BLE_LOG_MEMCPY(trans->buf + trans->pos, &frame_head, BLE_LOG_FRAME_HEAD_LEN);
    ble_log_pool_finish_frame(trans, payload_len, stat_mgr);

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}


BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_stat_mgr_mark_lost(ble_log_src_t src_code)
{
    /* The Global SN was already consumed at API entry; only the per-source
     * loss counter is attributed here. */
    ble_log_stat_mgr_t *stat_mgr = &stat_mgr_ctx[src_code];
    BLE_LOG_ATOMIC_ADD_RELAXED(stat_mgr->counters.lost_frame_cnt, 1);
}

/* -------------------------- */
/*     INTERNAL INTERFACE     */
/* -------------------------- */
bool ble_log_lbm_init(void)
{
    /* Avoid double init */
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_inited)) {
        return true;
    }

    BLE_LOG_MEMSET(&g_pool, 0, sizeof(g_pool));

    /* The semaphore carries wake events rather than buffer ownership, so its
     * capacity must accommodate every possible blocked writer. Counting
     * semaphores have a zero-sized queue item, so this does not allocate
     * storage proportional to the maximum count. */
    g_pool.sem = xSemaphoreCreateCounting(BLE_LOG_POOL_SEM_MAX_COUNT, 0);
    if (!g_pool.sem) {
        goto exit;
    }

    /* Allocate the transport buffers of the global pool. trans_init zeroes
     * the storage, so only the non-zero identity fields need setting. */
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        if (!ble_log_prph_trans_init(&(g_pool.trans[id]), BLE_LOG_POOL_TRANS_SIZE)) {
            goto exit;
        }
        g_pool.trans[id]->id = (uint8_t)id;
    }

    if (!ble_log_prph_trans_init(&internal_trans, BLE_LOG_INTERNAL_TRANS_SIZE)) {
        goto exit;
    }
    internal_trans->id = BLE_LOG_TRANS_ID_NONE;
    internal_trans->owner_kind = BLE_LOG_TRANS_OWNER_INTERNAL;

    g_pool.free_bitmap = BLE_LOG_POOL_ALL_MASK;
    g_pool.open_bitmap = 0;
    BLE_LOG_MEMSET(stat_mgr_ctx, 0, sizeof(stat_mgr_ctx));
    g_frame_sn = 0;
    g_snapshot_sn = 0;
    /* Task registry: fresh epoch (registry, sequence and its dedicated
     * transport) for the new receiver epoch. */
    if (!ble_log_task_registry_init()) {
        goto exit;
    }
    BLE_LOG_MEMSET(&internal_snapshot, 0, sizeof(internal_snapshot));
    internal_snapshot.int_src_code = BLE_LOG_INT_SRC_SNAPSHOT;
    internal_snapshot.pool.trans_cnt = BLE_LOG_POOL_TRANS_CNT;
    internal_snapshot.pool.non_yield_reserve_cnt = BLE_LOG_POOL_NON_YIELD_RESERVE_CNT;
    ble_log_internal_version_info_init();

    BLE_LOG_ATOMIC_STORE_RELEASE(lbm_enabled, false);
    BLE_LOG_ATOMIC_STORE_RELEASE(lbm_inited, true);
    return true;

exit:
    ble_log_lbm_deinit();
    return false;
}

void ble_log_lbm_begin_deinit(void)
{
    /* Serialize both producer-gate closes with enable and flush restore. */
    BLE_LOG_ENTER_CRITICAL();
    ble_log_lbm_disable();
    BLE_LOG_ATOMIC_STORE_SEQ_CST(lbm_inited, false);
    BLE_LOG_EXIT_CRITICAL();

    /* Close the task-registry gate with the LBM gate: its broadcast is
     * system output, so only teardown stops it. The publish path never
     * blocks, so this drain is immediate. */
    ble_log_task_registry_begin_deinit();

    /* Wake any blocked task writers and wait until BOTH the reference count
     * and the waiting-task count drain to zero. Blocked writers hold no
     * reference while parked, so waiting on ref_count alone could free the
     * pool while a woken task still touches it (use-after-free). */
    TickType_t ticks_waited = 0;
    while ((BLE_LOG_ATOMIC_LOAD_SEQ_CST(lbm_ref_count) > 0) ||
           (BLE_LOG_ATOMIC_LOAD_ACQUIRE(g_pool.waiting_task_count) > 0)) {
        ble_log_pool_wake_all();
        vTaskDelay(1);
        BLE_LOG_ASSERT(ticks_waited++ < BLE_LOG_WAIT_TIMEOUT_TICKS);
    }
}

__attribute__((noinline)) BLE_LOG_STATIC
void ble_log_snapshot_stats(ble_log_source_stat_t *snapshots)
{
    /* The counter writers (claim/commit/write_hex_ll, some in IRAM/ISR
     * context) use relaxed atomics and never take the spinlock, so an
     * exclusive section here would exclude nobody. The relaxed loads
     * are the whole protection. */
    for (int i = 0; i < BLE_LOG_SRC_CORE_COUNT; i++) {
        ble_log_stat_mgr_t *stat_mgr =
            &stat_mgr_ctx[BLE_LOG_SRC_CORE_FIRST + i];
        snapshots[i].written_frame_cnt =
            BLE_LOG_ATOMIC_LOAD_RELAXED(stat_mgr->counters.written_frame_cnt);
        snapshots[i].lost_frame_cnt =
            BLE_LOG_ATOMIC_LOAD_RELAXED(stat_mgr->counters.lost_frame_cnt);
    }
}

void ble_log_lbm_deinit(void)
{
    ble_log_lbm_begin_deinit();

    ble_log_task_registry_deinit();
    ble_log_prph_trans_deinit(&internal_trans);
    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        ble_log_prph_trans_deinit(&(g_pool.trans[id]));
    }

    if (g_pool.sem) {
        vSemaphoreDelete(g_pool.sem);
        g_pool.sem = NULL;
    }
}

bool ble_log_lbm_is_enabled(void)
{
    return BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_enabled);
}

/* Copies a NUL-terminated commit string into a fixed-width zero-padded field */
BLE_LOG_STATIC void ble_log_commit_copy(uint8_t *dst, const char *src, size_t len)
{
    BLE_LOG_MEMCPY(dst, src, strnlen(src, len));
}

/* Fills the version-info record carried by internal snapshots, in place:
 * no intermediate copy. ble_log_lbm_init zeroes internal_snapshot first. */
BLE_LOG_STATIC void ble_log_internal_version_info_init(void)
{
    ble_log_version_info_t *vi = &internal_snapshot.version_info;
    vi->int_src_code = BLE_LOG_INT_SRC_VERSION_INFO;
    vi->version = BLE_LOG_VERSION;
#ifdef BLE_LOG_IDF_COMMIT
    BLE_LOG_MEMCPY(vi->idf_commit, BLE_LOG_IDF_COMMIT,
                   BLE_LOG_IDF_COMMIT_LEN);
#endif
#if CONFIG_BT_CONTROLLER_ENABLED && defined(BLE_LOG_CONTROLLER_GET_COMMIT)
    ble_log_commit_copy(vi->controller_commit,
                        BLE_LOG_CONTROLLER_GET_COMMIT(), BLE_LOG_LIB_COMMIT_LEN);
#endif
#if CONFIG_BT_CONTROLLER_ENABLED && defined(BLE_LOG_BTDM_COMMON_GET_COMMIT)
    ble_log_commit_copy(vi->btdm_common_commit,
                        BLE_LOG_BTDM_COMMON_GET_COMMIT(), BLE_LOG_LIB_COMMIT_LEN);
#endif
#if CONFIG_BLE_MESH && CONFIG_BLE_MESH_V11_SUPPORT
    const char *mesh_commit = strrchr(bt_mesh_v11_commit_str, ' ');
    if (mesh_commit) {
        ble_log_commit_copy(vi->mesh_commit, mesh_commit + 1,
                            BLE_LOG_LIB_COMMIT_LEN);
    }
#endif
#if CONFIG_BT_AUDIO && CONFIG_SOC_BLE_AUDIO_SUPPORTED
    ble_log_commit_copy(vi->audio_commit, lib_audio_commit_get(),
                        BLE_LOG_LIB_COMMIT_LEN);
#endif
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    vi->chip_model = (uint16_t)chip_info.model;
    vi->chip_revision = chip_info.revision;
}

bool ble_log_internal_snapshot(uint16_t reason_flags,
                               const ble_log_ts_info_t *ts_info,
                               bool wait_for_transport)
{
#if CONFIG_BLE_LOG_PRPH_TEST
    if ((reason_flags & BLE_LOG_SNAPSHOT_REASON_INIT) &&
            ble_log_test_init_snapshot_before_acquire_hook) {
        ble_log_test_init_snapshot_before_acquire_hook();
    }
#endif
    /* Internal Snapshots are system output and outlive the public producer
     * gate. Only LBM teardown stops new snapshots. */
    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_inited)) {
        return false;
    }
    if (!internal_trans) {
        goto failed;
    }

    /* The caller sampled the clocks (with or without a sync IO toggle)
     * before any dedicated-buffer drain or wait; the frame timestamp is
     * that sample's esp_ts. */
    uint32_t timestamp = ts_info->esp_ts;
    TickType_t start_tick = xTaskGetTickCount();
    for (;;) {
        if (BLE_LOG_CAS_ACQUIRE(&internal_trans->atomic_lock)) {
            if (BLE_LOG_ATOMIC_LOAD_RELAXED(internal_trans->state) ==
                BLE_LOG_TRANS_STATE_FREE) {
                break;
            }
            BLE_LOG_CAS_RELEASE(&internal_trans->atomic_lock);
        }
        if (!wait_for_transport ||
            (xTaskGetTickCount() - start_tick) >= BLE_LOG_WAIT_TIMEOUT_TICKS) {
            goto lost;
        }
        (void)ble_log_rt_drain();
        vTaskDelay(1);
    }

    internal_snapshot.reason_flags = reason_flags;
    internal_snapshot.ts.io_level = ts_info->io_level;
    internal_snapshot.ts.lc_ts = ts_info->lc_ts;
    internal_snapshot.ts.esp_ts = timestamp;
    internal_snapshot.ts.os_ts = ts_info->os_ts;
    uint32_t free_bitmap = BLE_LOG_ATOMIC_LOAD_ACQUIRE(g_pool.free_bitmap) &
                           BLE_LOG_POOL_ALL_MASK;
    internal_snapshot.pool.inflight =
        (uint8_t)(BLE_LOG_POOL_TRANS_CNT - __builtin_popcount(free_bitmap));
    internal_snapshot.pool.inflight_peak =
        (uint8_t)BLE_LOG_ATOMIC_LOAD_RELAXED(g_pool.inflight_peak);
    ble_log_snapshot_stats(internal_snapshot.stats);

    uint32_t frame_sn = BLE_LOG_GET_SNAPSHOT_SN();
    ble_log_frame_head_t frame_head = {
        .length = sizeof(timestamp) + sizeof(internal_snapshot),
        .frame_meta = BLE_LOG_MAKE_FRAME_META(BLE_LOG_SRC_INTERNAL, frame_sn),
    };
    uint8_t *buf = internal_trans->buf;
    BLE_LOG_MEMCPY(buf, &frame_head, sizeof(frame_head));
    BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN, &timestamp, sizeof(timestamp));
    BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN + sizeof(timestamp),
                   &internal_snapshot, sizeof(internal_snapshot));
    uint32_t checksum = ble_log_fast_checksum(
        buf, BLE_LOG_FRAME_HEAD_LEN + frame_head.length);
    BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN + frame_head.length,
                   &checksum, sizeof(checksum));

    internal_trans->pos = BLE_LOG_INTERNAL_FRAME_LEN;
    BLE_LOG_ATOMIC_STORE_RELAXED(internal_trans->state,
                                 BLE_LOG_TRANS_STATE_SENDING);
    BLE_LOG_CAS_RELEASE(&internal_trans->atomic_lock);
    ble_log_rt_submit_trans(internal_trans);
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return true;

lost:
    /* A skipped snapshot burns one snapshot SN: the gap in the snapshot
     * sequence is the loss signal. */
    (void)BLE_LOG_GET_SNAPSHOT_SN();
failed:
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return false;
}

BLE_LOG_STATIC bool ble_log_pool_flush_all_trans(void)
{
    /* New writes are disabled, so every held lock will be released after its
     * current frame copy. Seal every remaining OPEN transport. */
    ble_log_pool_seal_open_trans();
    TickType_t start_tick = xTaskGetTickCount();
    while ((BLE_LOG_ATOMIC_LOAD_ACQUIRE(g_pool.free_bitmap) &
            BLE_LOG_POOL_ALL_MASK) != BLE_LOG_POOL_ALL_MASK) {
        (void)ble_log_rt_drain();
        if ((xTaskGetTickCount() - start_tick) >= BLE_LOG_WAIT_TIMEOUT_TICKS) {
            BLE_LOG_CONSOLE("@EW: Timed out waiting for BLE Log transports\n");
            return false;
        }
        vTaskDelay(1);
    }

    return true;
}

void ble_log_lbm_flush_open_trans(void)
{
    /* Periodic OPEN flush is system output, independent from the public
     * producer gate. LBM teardown remains the lifetime boundary. */
    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_inited)) {
        return;
    }

    for (int id = 0; id < BLE_LOG_POOL_TRANS_CNT; id++) {
        if (!(BLE_LOG_ATOMIC_LOAD_ACQUIRE(g_pool.open_bitmap) & BIT(id))) {
            continue;
        }
        ble_log_prph_trans_t *trans = g_pool.trans[id];
        if (!BLE_LOG_CAS_ACQUIRE(&trans->atomic_lock)) {
            /* A writer holds the buffer: leave a pending-seal marker for
             * the next claim instead of waiting (the flusher never
             * competes for a lock). The marker is serviced by the next
             * writer on this transport, or by the next periodic pass once
             * the lock is uncontended. */
            BLE_LOG_ATOMIC_STORE_RELEASE(trans->pending_seal, true);
            continue;
        }
        if (BLE_LOG_ATOMIC_LOAD_RELAXED(trans->state) == BLE_LOG_TRANS_STATE_OPEN &&
            trans->pos > 0) {
            ble_log_pool_seal_and_send(trans);
        } else {
            BLE_LOG_CAS_RELEASE(&trans->atomic_lock);
        }
    }

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}

void ble_log_lbm_drain_open_trans(void)
{
    /* Contract (see header): called after ble_log_lbm_begin_deinit() and
     * before ble_log_rt_deinit(). The producer gate is closed and writers
     * have drained, so no lock is held for longer than one frame copy. */
    ble_log_pool_seal_open_trans();
    /* Hand the sealed buffers to the peripheral before the runtime queue
     * is destroyed; the peripheral deinit wait completes the delivery. */
    (void)ble_log_rt_drain();
}

/* ------------------------ */
/*     PUBLIC INTERFACE     */
/* ------------------------ */
bool ble_log_enable(bool enable)
{
#if CONFIG_BLE_LOG_PRPH_TEST
    if (ble_log_test_enable_before_lifecycle_lock_hook) {
        ble_log_test_enable_before_lifecycle_lock_hook();
    }
#endif
    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_inited)) {
        return false;
    }

    BLE_LOG_ENTER_CRITICAL();
    /* Recheck under the same lifecycle lock used by deinit and flush restore,
     * so a racing enable cannot reopen a closed producer gate. */
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_inited) || flush_in_progress) {
        BLE_LOG_EXIT_CRITICAL();
        BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
        return false;
    }
    if (enable) {
        BLE_LOG_ATOMIC_STORE_RELEASE(lbm_enabled, true);
    } else {
        ble_log_lbm_disable();
    }
    BLE_LOG_EXIT_CRITICAL();

    if (!enable) {
#if CONFIG_BLE_LOG_PRPH_TEST
        if (ble_log_test_disable_before_wake_hook) {
            ble_log_test_disable_before_wake_hook();
        }
#endif
        /* Keep the lifetime reference until every waiter wake is complete. */
        ble_log_pool_wake_all();
    }
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return true;
}

void ble_log_flush(void)
{
    BLE_LOG_ENTER_CRITICAL();
    if (flush_in_progress) {
        BLE_LOG_EXIT_CRITICAL();
        return;
    }
    flush_in_progress = true;
    BLE_LOG_EXIT_CRITICAL();

    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_inited)) {
        goto done;
    }

    bool lbm_enabled_copy = BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_enabled);
    ble_log_lbm_disable();
    TickType_t start_tick = xTaskGetTickCount();
    while ((BLE_LOG_ATOMIC_LOAD_SEQ_CST(lbm_ref_count) > 1) ||
           (BLE_LOG_ATOMIC_LOAD_ACQUIRE(g_pool.waiting_task_count) > 0)) {
        ble_log_pool_wake_all();
        if ((xTaskGetTickCount() - start_tick) >= BLE_LOG_WAIT_TIMEOUT_TICKS) {
            BLE_LOG_CONSOLE("@EW: Timed out waiting for BLE Log writers\n");
            goto restore;
        }
        vTaskDelay(1);
    }

#if BLE_LOG_UART_REDIR_ENABLED
    if (!ble_log_prph_flush()) {
        goto restore;
    }
#endif

    if (!ble_log_pool_flush_all_trans()) {
        goto restore;
    }

    /* One-shot FLUSH snapshot: sample after the pool drained so the clocks
     * reflect the flush completion; no sync IO toggle (periodic flow only). */
    ble_log_ts_info_t ts_info;
    ble_log_rt_ts_sample(&ts_info, false);
    if (!ble_log_internal_snapshot(BLE_LOG_SNAPSHOT_REASON_FLUSH, &ts_info, true)) {
        goto restore;
    }

    start_tick = xTaskGetTickCount();
    while (BLE_LOG_ATOMIC_LOAD_ACQUIRE(internal_trans->state) !=
           BLE_LOG_TRANS_STATE_FREE) {
        (void)ble_log_rt_drain();
        if ((xTaskGetTickCount() - start_tick) >= BLE_LOG_WAIT_TIMEOUT_TICKS) {
            BLE_LOG_CONSOLE("@EW: Timed out waiting for BLE Log Internal Snapshot\n");
            goto restore;
        }
        vTaskDelay(1);
    }

    /* FLUSH is not a segment boundary: reset interval counters while the
     * Global SN and the snapshot sequence stay continuous. Snapshot loss is
     * lifecycle-cumulative in the snapshot sequence and is preserved. */
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        BLE_LOG_ATOMIC_STORE_RELAXED(stat_mgr_ctx[i].counters.written_frame_cnt, 0);
        BLE_LOG_ATOMIC_STORE_RELAXED(stat_mgr_ctx[i].counters.lost_frame_cnt, 0);
    }
    BLE_LOG_ATOMIC_STORE_RELAXED(g_pool.inflight_peak, 0);
#if BLE_LOG_UART_REDIR_ENABLED
    ble_log_prph_reset_util_counters();
#endif

restore:
    BLE_LOG_ENTER_CRITICAL();
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_inited)) {
        BLE_LOG_ATOMIC_STORE_RELEASE(lbm_enabled, lbm_enabled_copy);
    }
    BLE_LOG_EXIT_CRITICAL();

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
done:
    BLE_LOG_ENTER_CRITICAL();
    flush_in_progress = false;
    BLE_LOG_EXIT_CRITICAL();
}

BLE_LOG_IRAM_ATTR
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len)
{
    /* Source IDs are the frozen public enum values; garbage values are
     * rejected. A non-NULL buffer with len > 0 is the caller's contract. */
    if (src_code >= BLE_LOG_SRC_MAX || (!addr && len)) {
        return false;
    }

    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_enabled)) {
        return false;
    }
    /* One Global SN per attempt, consumed before the length and pool checks
     * so rejected or lost attempts leave a gap. */
    uint32_t frame_sn = BLE_LOG_GET_GLOBAL_SN();

    size_t max_log_len = BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t);
    if (len > max_log_len) {
        goto failed;
    }

    bool is_isr = BLE_LOG_IN_ISR();
    bool can_yield = !is_isr && xPortCanYield() &&
                     xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
    uint32_t timestamp = BLE_LOG_TIMESTAMP_NOW();
    size_t payload_len = sizeof(timestamp) + len;
    ble_log_prph_trans_t *trans =
        ble_log_pool_acquire(payload_len, !can_yield, can_yield);
    if (!trans) {
        goto failed;
    }

    ble_log_pool_write_frame(trans, frame_sn, src_code,
                             (const uint8_t *)&timestamp, sizeof(timestamp),
                             addr, (uint16_t)len, NULL, 0, false);

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return true;

failed:
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_inited)) {
        ble_log_stat_mgr_mark_lost(src_code);
    }
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return false;
}

#if CONFIG_BLE_LOG_LL_ENABLED
BLE_LOG_IRAM_ATTR
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, uint32_t flag)
{
#if !CONFIG_BLE_LOG_HCI_LOG_ENABLED || CONFIG_BT_BLUEDROID_ENABLED || \
    CONFIG_BT_NIMBLE_LEGACY_VHCI_ENABLE
    /* Bluedroid and legacy NimBLE already capture HCI on the Host side. */
    if (flag & (BIT(BLE_LOG_LL_FLAG_HCI) | BIT(BLE_LOG_LL_FLAG_HCI_UPSTREAM))) {
        return;
    }
#endif
    if ((!addr && len) || (!addr_append && len_append)) {
        return;
    }

    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_enabled)) {
        return;
    }

    /* One Global SN per attempt; the entry order it assigns also breaks
     * ties between equal-timestamp records from different sources. */
    uint32_t frame_sn = BLE_LOG_GET_GLOBAL_SN();

    /* Preserve the controller flag-to-source mapping and ISR precedence. */
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

    bool is_isr = BLE_LOG_IN_ISR();
    bool can_yield = !is_isr && xPortCanYield() &&
                     xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
    bool non_yield = !can_yield || (flag & BIT(BLE_LOG_LL_FLAG_ISR));
    bool omdata = flag & BIT(BLE_LOG_LL_FLAG_OMDATA);
    if (is_isr) {
        /* os_mbuf_copydata is in flash and cannot run from an ISR. */
        omdata = false;
    }

    if (len > BLE_LOG_MAX_PAYLOAD_LEN ||
        len_append > BLE_LOG_MAX_PAYLOAD_LEN - len) {
        goto failed;
    }

    size_t payload_len = len + len_append;
    ble_log_prph_trans_t *trans =
        ble_log_pool_acquire(payload_len, non_yield, can_yield && !non_yield);
    if (!trans) {
        goto failed;
    }

    ble_log_pool_write_frame(trans, frame_sn, src_code,
                             NULL, 0, addr, (uint16_t)len,
                             addr_append, (uint16_t)len_append, omdata);

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    return;

failed:
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(lbm_inited)) {
        ble_log_stat_mgr_mark_lost(src_code);
    }
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

void ble_log_dump_to_console(void)
{
    if (!ble_log_ref_count_try_acquire(&lbm_ref_count, &lbm_inited)) {
        return;
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

    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
}
