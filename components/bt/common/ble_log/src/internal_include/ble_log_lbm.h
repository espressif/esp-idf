/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_LBM_H__
#define __BLE_LOG_LBM_H__

/* --------------------------------------- */
/*     BLE Log - Log Buffer Management     */
/* --------------------------------------- */

/* ---------------- */
/*     Includes     */
/* ---------------- */
#include "ble_log.h"
#include "ble_log_prph.h"

#include "freertos/FreeRTOS.h"

#if defined(CONFIG_BLE_LOG_PRPH_UART_DMA) && (CONFIG_BLE_LOG_PRPH_UART_DMA_PORT == 0)
#define BLE_LOG_UART_REDIR_ENABLED          (1)
#else
#define BLE_LOG_UART_REDIR_ENABLED          (0)
#endif
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
#define BLE_LOG_MAKE_FRAME_META(src_code, sn)   (((src_code) & 0xFF) | ((sn) << 8))

/* ------------------------------------- */
/*     Unified Buffer Pool Defines       */
/* ------------------------------------- */
/* Total transport buffers in the global pool. Bitmaps are 32-bit, so the
 * pool is capped at 32 buffers. */
#define BLE_LOG_POOL_TRANS_CNT                  CONFIG_BLE_LOG_POOL_TRANS_CNT
/* Buffers reserved for ISR context when the shared region is exhausted. */
#define BLE_LOG_POOL_ISR_RESERVE_CNT            CONFIG_BLE_LOG_POOL_ISR_RESERVE_CNT
/* Shared region: usable by both task and ISR context. */
#define BLE_LOG_POOL_SHARED_CNT                 (BLE_LOG_POOL_TRANS_CNT - BLE_LOG_POOL_ISR_RESERVE_CNT)
/* Per-buffer byte size. */
#define BLE_LOG_POOL_TRANS_SIZE                 CONFIG_BLE_LOG_POOL_TRANS_SIZE

/* UART redirection uses its own small buffer set, not part of the pool. */
#if BLE_LOG_UART_REDIR_ENABLED
#define BLE_LOG_TRANS_REDIR_CNT                 BLE_LOG_TRANS_BUF_CNT
#else
#define BLE_LOG_TRANS_REDIR_CNT                 (0)
#endif

/* Depth used to size the runtime queue and the peripheral driver queue: the
 * maximum number of transports that can be in flight simultaneously. */
#define BLE_LOG_TRANS_TOTAL_CNT                 (BLE_LOG_POOL_TRANS_CNT + BLE_LOG_TRANS_REDIR_CNT)

/* ------------------------------------- */
/*     UART Redirection Manager          */
/* ------------------------------------- */
#if BLE_LOG_UART_REDIR_ENABLED
typedef struct {
    ble_log_prph_trans_t *trans[BLE_LOG_TRANS_BUF_CNT];
    SemaphoreHandle_t mutex;
    int trans_idx;
} ble_log_redir_t;
#endif /* BLE_LOG_UART_REDIR_ENABLED */

/* -------------------------------------------- */
/*     Buffer Utilization Reporting Defines     */
/* -------------------------------------------- */
typedef enum {
    BLE_LOG_BUF_UTIL_POOL_COMMON_TASK = 0,
    BLE_LOG_BUF_UTIL_POOL_COMMON_ISR  = 1,
    BLE_LOG_BUF_UTIL_POOL_LL          = 2,
    BLE_LOG_BUF_UTIL_POOL_REDIR       = 3,
} ble_log_buf_util_pool_t;

typedef struct {
    uint8_t int_src_code;
    uint8_t lbm_id;
    uint8_t trans_cnt;
    uint8_t inflight_peak;
} __attribute__((packed)) ble_log_buf_util_t;

#define BLE_LOG_BUF_UTIL_MAKE_ID(pool, idx)     (((pool) << 4) | ((idx) & 0x0F))
#define BLE_LOG_BUF_UTIL_GET_POOL(id)           (((id) >> 4) & 0x0F)
#define BLE_LOG_BUF_UTIL_GET_INDEX(id)          ((id) & 0x0F)

/* ---------------------------------------- */
/*     Enhanced Statistics Data Defines     */
/* ---------------------------------------- */
typedef struct {
    uint8_t int_src_code;
    uint8_t src_code;
    uint32_t written_frame_cnt;
    uint32_t lost_frame_cnt;
    uint32_t written_bytes_cnt;
    uint32_t lost_bytes_cnt;
} __attribute__((packed)) ble_log_enh_stat_t;

/* -------------------------------- */
/*     Final Statistics Defines     */
/* -------------------------------- */
typedef struct {
    uint8_t src_code;
    uint32_t written_frame_cnt;
    uint32_t lost_frame_cnt;
    uint32_t written_bytes_cnt;
    uint32_t lost_bytes_cnt;
} __attribute__((packed)) ble_log_final_stat_entry_t;

typedef struct {
    uint8_t int_src_code;
    uint8_t src_cnt;
    ble_log_final_stat_entry_t entries[BLE_LOG_SRC_MAX];
} __attribute__((packed)) ble_log_final_stat_t;

#define BLE_LOG_FINAL_STAT_LEN                 sizeof(ble_log_final_stat_t)

/* -------------------------------------- */
/*     Log Statistics Manager Context     */
/* -------------------------------------- */
typedef struct {
    uint32_t frame_sn;
    /* Aligned live counters — updated by ble_log_stat_mgr_update(),
     * snapshot by ble_log_write_enh_stat(). Natural 4-byte alignment
     * ensures each load/store compiles to a single l32i/s32i on
     * Xtensa/RISC-V, so individual field access is atomic without locks.
     * The packed ble_log_enh_stat_t wire format is built on the stack
     * only when serializing to UART. */
    uint32_t written_frame_cnt;
    uint32_t lost_frame_cnt;
    uint32_t written_bytes_cnt;
    uint32_t lost_bytes_cnt;
} ble_log_stat_mgr_t;

#define BLE_LOG_GET_FRAME_SN(VAR)               __atomic_fetch_add(VAR, 1, __ATOMIC_RELAXED)

/* -------------------------- */
/*     Link Layer Defines     */
/* -------------------------- */
#if CONFIG_BLE_LOG_LL_ENABLED
enum {
    BLE_LOG_LL_FLAG_CONTINUE = 0,
    BLE_LOG_LL_FLAG_END,
    BLE_LOG_LL_FLAG_TASK,
    BLE_LOG_LL_FLAG_ISR,
    BLE_LOG_LL_FLAG_HCI,
    BLE_LOG_LL_FLAG_RAW,
    BLE_LOG_LL_FLAG_OMDATA,
    BLE_LOG_LL_FLAG_HCI_UPSTREAM,
};
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

/* ------------------------------- */
/*     Compile-Time Guards         */
/* ------------------------------- */
_Static_assert(BLE_LOG_POOL_TRANS_CNT >= 2 && BLE_LOG_POOL_TRANS_CNT <= 32,
               "BLE_LOG_POOL_TRANS_CNT must be within [2, 32] (32-bit bitmap)");
_Static_assert(BLE_LOG_POOL_ISR_RESERVE_CNT >= 0 &&
               BLE_LOG_POOL_ISR_RESERVE_CNT < BLE_LOG_POOL_TRANS_CNT,
               "BLE_LOG_POOL_ISR_RESERVE_CNT must leave at least one shared buffer");
_Static_assert(BLE_LOG_POOL_TRANS_SIZE >= BLE_LOG_FRAME_OVERHEAD,
               "BLE_LOG_POOL_TRANS_SIZE too small for a single frame");
_Static_assert((BLE_LOG_TRANS_BUF_CNT & (BLE_LOG_TRANS_BUF_CNT - 1)) == 0,
               "BLE_LOG_TRANS_BUF_CNT must be a power of 2");
_Static_assert(BLE_LOG_TRANS_BUF_CNT <= 255,
               "BLE_LOG_TRANS_BUF_CNT must fit in uint8_t for ble_log_buf_util_t");

/* --------------------------- */
/*     Internal Interfaces     */
/* --------------------------- */
bool ble_log_lbm_init(void);
void ble_log_lbm_deinit(void);
void ble_log_write_enh_stat(void);
void ble_log_write_buf_util(void);

/* Unified transport recycle: return a transport to the global pool (or reset
 * a redirection transport). Safe to call from ISR / driver callback context.
 * Used by every peripheral tx-done / send-failure path and by the runtime
 * queue drain, so that buffer recycling behaviour is identical everywhere. */
void ble_log_lbm_recycle_trans(ble_log_prph_trans_t *trans);

#if BLE_LOG_UART_REDIR_ENABLED
void ble_log_lbm_stream_write(ble_log_redir_t *redir, ble_log_src_t src_code,
                               const uint8_t *data, size_t len);
void ble_log_lbm_stream_flush(ble_log_redir_t *redir, ble_log_src_t src_code);
ble_log_redir_t *ble_log_prph_get_redir_lbm(void);
#endif

#endif /* __BLE_LOG_LBM_H__ */
