/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_LBM_V2_H__
#define __BLE_LOG_LBM_V2_H__

/* -------------------------------------------------- */
/*     BLE Log - Unified Transport Pool (v8)          */
/* -------------------------------------------------- */
/* Replaces the legacy multi-LBM design (ble_log_lbm.h) with one shared
 * transport pool. Written as a new module so the legacy implementation
 * stays untouched and reviewable as a pure addition. */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_prph.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* ------------------------- */
/*     Log Frame Defines     */
/* ------------------------- */
typedef struct {
    uint16_t length;
    uint32_t frame_meta;
} __attribute__((packed)) ble_log_frame_head_t;

#define BLE_LOG_FRAME_HEAD_LEN                  (sizeof(ble_log_frame_head_t))
#define BLE_LOG_FRAME_TAIL_LEN                  (sizeof(uint32_t))
#define BLE_LOG_FRAME_OVERHEAD                  (BLE_LOG_FRAME_HEAD_LEN + BLE_LOG_FRAME_TAIL_LEN)
#define BLE_LOG_MAKE_FRAME_META(src, sn) \
    (((src) & 0xffU) | (((sn) & 0x00ffffffU) << 8))

/* ------------------------------------- */
/*     Unified Buffer Pool Defines       */
/* ------------------------------------- */
#define BLE_LOG_POOL_TRANS_CNT                  CONFIG_BLE_LOG_POOL_TRANS_CNT
#define BLE_LOG_POOL_NON_YIELD_RESERVE_CNT      CONFIG_BLE_LOG_POOL_NON_YIELD_RESERVE_CNT
#define BLE_LOG_POOL_SHARED_CNT                 (BLE_LOG_POOL_TRANS_CNT - BLE_LOG_POOL_NON_YIELD_RESERVE_CNT)
#define BLE_LOG_POOL_TRANS_SIZE                 CONFIG_BLE_LOG_POOL_TRANS_SIZE
#define BLE_LOG_TRANS_SIZE                      BLE_LOG_POOL_TRANS_SIZE
#define BLE_LOG_MAX_PAYLOAD_LEN                 (BLE_LOG_POOL_TRANS_SIZE - BLE_LOG_FRAME_OVERHEAD)
#define BLE_LOG_TRANS_INTERNAL_CNT              (1)
/* Dedicated transport of the task-id registry binding broadcast. */
#define BLE_LOG_TRANS_TASK_BINDING_CNT           (1)

#if BLE_LOG_UART_REDIR_ENABLED
#define BLE_LOG_TRANS_REDIR_CNT                 BLE_LOG_TRANS_BUF_CNT
#else
#define BLE_LOG_TRANS_REDIR_CNT                 (0)
#endif

#define BLE_LOG_TRANS_TOTAL_CNT                 \
    (BLE_LOG_POOL_TRANS_CNT + BLE_LOG_TRANS_INTERNAL_CNT + \
     BLE_LOG_TRANS_TASK_BINDING_CNT + BLE_LOG_TRANS_REDIR_CNT)

/* --------------------------------------- */
/*     Protocol v8 Source ID Space         */
/* --------------------------------------- */
/* The frozen public ble_log_src_t values are the on-wire and statistic
 * source IDs: the frame source byte carries the bare enum value. */

/* Statistic slots in the Internal Snapshot: every public source that can
 * produce frames, i.e. CUSTOM through ENCODE. INTERNAL frames carry their
 * own snapshot sequence in the frame header; REDIR is a raw console
 * stream. */
#define BLE_LOG_SRC_CORE_FIRST                  BLE_LOG_SRC_CUSTOM
#define BLE_LOG_SRC_CORE_COUNT                  (BLE_LOG_SRC_ENCODE - BLE_LOG_SRC_CORE_FIRST + 1)

/* ------------------------------------- */
/*     UART Redirection Manager          */
/* ------------------------------------- */
#if BLE_LOG_UART_REDIR_ENABLED
typedef struct {
    ble_log_prph_trans_t *trans[BLE_LOG_TRANS_BUF_CNT];
    SemaphoreHandle_t mutex;
    int trans_idx;
    /* The REDIR console stream keeps its own 24-bit frame sequence: it is
     * not a log attempt, so it never consumes the Global SN; a gap counts
     * a dropped console batch. Zeroed when the manager is created. */
    volatile uint32_t frame_sn;
    volatile uint32_t inflight;
    volatile uint32_t inflight_peak;
} ble_log_redir_t;
#endif

/* -------------------------------- */
/*     Compact Core Statistics      */
/* -------------------------------- */
typedef struct {
    uint32_t written_frame_cnt;
    uint32_t lost_frame_cnt;
} __attribute__((packed)) ble_log_source_stat_t;

/* Runtime per-source counters. The aligned wrapper keeps the embedded
 * packed counters word-aligned, so atomic updates stay single-instruction
 * (amo/s32c1i) on both toolchains. */
typedef struct {
    ble_log_source_stat_t counters;
} __attribute__((aligned(4))) ble_log_stat_mgr_t;

#define BLE_LOG_GET_FRAME_SN(VAR)               BLE_LOG_ATOMIC_ADD_RELAXED(VAR, 1)

/* One 24-bit Global SN is shared by the log sources except INTERNAL and
 * REDIR: consumed at API entry (before pool contention), it orders all
 * log attempts — including equal-timestamp records from different
 * sources — and every lost or rejected attempt leaves a gap. INTERNAL
 * snapshot frames, the periodic task-binding broadcast, and the REDIR
 * console stream keep their own separate sequences (a gap counts a
 * skipped snapshot, a skipped broadcast window, or a dropped console
 * batch). ble_log_init() resets all of them; its required INIT snapshot
 * starts a new receiver epoch. They stay continuous through FLUSH within
 * that epoch. The per-counter macros live next to their counters in the
 * owning translation units; the 24-bit wire field is enforced where the
 * frame meta is packed. */

/* -------------------------------- */
/*     Internal Snapshot Frame      */
/* -------------------------------- */
/* Clock sample captured at one instant by the runtime periodic tick. */
typedef struct {
    uint8_t int_src_code;
    uint8_t io_level;
    uint32_t lc_ts;
    uint32_t esp_ts;
    uint32_t os_ts;
} __attribute__((packed)) ble_log_ts_info_t;

#define BLE_LOG_SNAPSHOT_REASON_INIT            BIT(0)
#define BLE_LOG_SNAPSHOT_REASON_PERIODIC        BIT(1)
#define BLE_LOG_SNAPSHOT_REASON_FLUSH           BIT(2)
#define BLE_LOG_SNAPSHOT_REASON_TS_VALID        BIT(3)

typedef struct {
    uint8_t int_src_code;
    uint16_t reason_flags;
    ble_log_version_info_t version_info;

    /* Clock samples captured at the same instant, mirroring
     * ble_log_ts_info_t (minus its int_src_code). */
    struct {
        uint8_t io_level;
        uint32_t lc_ts;   /* Link-layer clock; 0 when not available */
        uint32_t esp_ts;  /* esp_timer_get_time() */
        uint32_t os_ts;   /* FreeRTOS tick count in ms */
    } __attribute__((packed)) ts;

    /* Unified pool state at capture time. */
    struct {
        uint8_t trans_cnt;
        uint8_t non_yield_reserve_cnt;
        uint8_t inflight;
        uint8_t inflight_peak;
    } __attribute__((packed)) pool;

    ble_log_source_stat_t stats[BLE_LOG_SRC_CORE_COUNT];
} __attribute__((packed)) ble_log_internal_snapshot_t;

#define BLE_LOG_INTERNAL_FRAME_LEN              \
    (BLE_LOG_FRAME_OVERHEAD + sizeof(uint32_t) + sizeof(ble_log_internal_snapshot_t))
#define BLE_LOG_INTERNAL_TRANS_SIZE             \
    ((BLE_LOG_INTERNAL_FRAME_LEN + 3U) & ~3U)

/* -------------------------- */
/*     Link Layer Defines     */
/* -------------------------- */
#if CONFIG_BLE_LOG_LL_ENABLED
/* Numeric positions are an ABI with prebuilt controller libraries. */
enum {
    BLE_LOG_LL_FLAG_CONTINUE = 0,
    BLE_LOG_LL_FLAG_END = 1,
    BLE_LOG_LL_FLAG_TASK = 2,
    BLE_LOG_LL_FLAG_ISR = 3,
    BLE_LOG_LL_FLAG_HCI = 4,
    BLE_LOG_LL_FLAG_RAW = 5,
    BLE_LOG_LL_FLAG_OMDATA = 6,
    BLE_LOG_LL_FLAG_HCI_UPSTREAM = 7,
};
#endif

/* ------------------------------- */
/*     Compile-Time Guards         */
/* ------------------------------- */
_Static_assert(sizeof(ble_log_frame_head_t) == 6,
               "Unexpected BLE Log frame header size");
_Static_assert(BLE_LOG_POOL_TRANS_CNT >= 2 && BLE_LOG_POOL_TRANS_CNT <= 32,
               "BLE_LOG_POOL_TRANS_CNT must be within [2, 32]");
_Static_assert(BLE_LOG_POOL_NON_YIELD_RESERVE_CNT > 0 &&
               BLE_LOG_POOL_NON_YIELD_RESERVE_CNT < BLE_LOG_POOL_TRANS_CNT,
               "Non-yield reserve must leave at least one shared transport");
_Static_assert(BLE_LOG_POOL_TRANS_SIZE >= BLE_LOG_FRAME_OVERHEAD + sizeof(uint32_t),
               "BLE_LOG_POOL_TRANS_SIZE is too small for a timestamped frame");
_Static_assert(BLE_LOG_POOL_TRANS_SIZE <= UINT16_MAX,
               "BLE_LOG_POOL_TRANS_SIZE exceeds the transport size field");
_Static_assert(BLE_LOG_POOL_TRANS_SIZE <= 10240,
               "BLE_LOG_POOL_TRANS_SIZE exceeds the peripheral transfer limit");
#if CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA || CONFIG_BLE_LOG_PRPH_SPI_MASTER_HD
_Static_assert((BLE_LOG_POOL_TRANS_SIZE & 3U) == 0,
               "SPI BLE Log transport size must be four-byte aligned");
#endif
_Static_assert(sizeof(ble_log_version_info_t) == 58,
               "Unexpected BLE Log version information size");
_Static_assert(sizeof(ble_log_source_stat_t) == 8,
               "Unexpected BLE Log source statistic size");
_Static_assert(offsetof(ble_log_stat_mgr_t, counters) == 0 &&
               sizeof(ble_log_stat_mgr_t) == 8,
               "stat manager layout must keep the counters word-aligned");
_Static_assert(sizeof(ble_log_internal_snapshot_t) == 134,
               "Unexpected BLE Log Internal Snapshot size");
_Static_assert(BLE_LOG_INTERNAL_FRAME_LEN == 148,
               "Unexpected BLE Log Internal frame size");
_Static_assert(BLE_LOG_INTERNAL_TRANS_SIZE >= BLE_LOG_INTERNAL_FRAME_LEN,
               "Internal transport is too small");
_Static_assert((BLE_LOG_TRANS_BUF_CNT & (BLE_LOG_TRANS_BUF_CNT - 1)) == 0,
               "BLE_LOG_TRANS_BUF_CNT must be a power of two");

/* --------------------------- */
/*     Internal Interfaces     */
/* --------------------------- */
bool ble_log_lbm_init(void);
void ble_log_lbm_begin_deinit(void);
void ble_log_lbm_deinit(void);
bool ble_log_lbm_is_enabled(void);
/* Best-effort system flush, gated by LBM lifetime rather than producers. */
void ble_log_lbm_flush_open_trans(void);
/* Deinit drain: seal every OPEN transport and dispatch it to the runtime
 * queue. Contract: called after ble_log_lbm_begin_deinit() (producer gate
 * closed, writers drained) and before ble_log_rt_deinit(); every transport
 * lock is then uncontended. The peripheral deinit wait completes the
 * delivery of the dispatched buffers. */
void ble_log_lbm_drain_open_trans(void);
void ble_log_lbm_recycle_trans(ble_log_prph_trans_t *trans);
/* System output: gated by the LBM lifetime, not ble_log_enable(). A false
 * wait_for_transport makes a busy dedicated transport a lossy fast path. */
bool ble_log_internal_snapshot(uint16_t reason_flags,
                               const ble_log_ts_info_t *ts_info,
                               bool wait_for_transport);

/* The task-id registry (protocol v8 attribution) lives in its own module:
 * ble_log_task_registry.c/h, like the UART redirection writer. */

/* Claim/commit: the public ble_log_src_t is accepted for API stability, but
 * only BLE_LOG_SRC_ENCODE is supported; its frames are stamped with the
 * ENCODE source ID on the wire. wait_for_transport=false turns a busy pool
 * into a lossy fast path (NULL, as in a non-yieldable context) instead of
 * waiting, for output that must not block its caller (e.g. the shared ESP
 * timer task); true waits in yieldable contexts, never in non-yieldable
 * ones. */
uint8_t *ble_log_claim(ble_log_src_t src_code, size_t max_len,
                       uint32_t *handle, bool wait_for_transport);
void ble_log_commit(uint32_t handle, size_t actual_len);

#if BLE_LOG_UART_REDIR_ENABLED
ble_log_redir_t *ble_log_prph_get_redir_lbm(void);
#endif

#endif /* __BLE_LOG_LBM_V2_H__ */
