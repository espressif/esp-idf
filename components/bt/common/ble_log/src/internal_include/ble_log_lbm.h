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

/* ---------------------------------- */
/*     Log Buffer Manager Defines     */
/* ---------------------------------- */
typedef enum {
    BLE_LOG_LBM_LOCK_NONE,
    BLE_LOG_LBM_LOCK_SPIN,
    BLE_LOG_LBM_LOCK_ATOMIC,
    BLE_LOG_LBM_LOCK_MUTEX,
} ble_log_lbm_lock_t;

typedef struct {
    int trans_idx;
    ble_log_prph_trans_t *trans[BLE_LOG_TRANS_BUF_CNT];
    ble_log_lbm_lock_t lock_type;
    union {
        /* BLE_LOG_LBM_LOCK_NONE */
        void *none;
        /* BLE_LOG_LBM_LOCK_SPIN */
        portMUX_TYPE spin_lock;
        /* BLE_LOG_LBM_LOCK_ATOMIC */
        volatile bool atomic_lock;
        /* BLE_LOG_LBM_LOCK_MUTEX */
        SemaphoreHandle_t mutex;
    };
    uint32_t trans_inflight;
    uint32_t trans_inflight_peak;
} ble_log_lbm_t;

/* --------------------------------------- */
/*     Log Buffer Manager Pool Defines     */
/* --------------------------------------- */
enum {
#if CONFIG_BLE_LOG_LL_ENABLED
    BLE_LOG_LBM_LL_TASK,
    BLE_LOG_LBM_LL_HCI,
#endif /* CONFIG_BLE_LOG_LL_ENABLED */
    BLE_LOG_LBM_LL_MAX,
};

enum {
    BLE_LOG_LBM_SPIN_TASK = 0,
    BLE_LOG_LBM_SPIN_ISR,
    BLE_LOG_LBM_SPIN_MAX,
};

#define BLE_LOG_LBM_ATOMIC_TASK_CNT             CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT
#define BLE_LOG_LBM_ATOMIC_ISR_CNT              CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_ISR_CNT
#define BLE_LOG_LBM_ATOMIC_CNT                  (BLE_LOG_LBM_ATOMIC_TASK_CNT +\
                                                 BLE_LOG_LBM_ATOMIC_ISR_CNT)
#define BLE_LOG_LBM_COMMON_CNT                  (BLE_LOG_LBM_ATOMIC_CNT + BLE_LOG_LBM_SPIN_MAX)
#define BLE_LOG_LBM_CNT                         (BLE_LOG_LBM_COMMON_CNT + BLE_LOG_LBM_LL_MAX)

/* Derived per-buffer size from user-configured total-per-LBM budget */
#define BLE_LOG_TRANS_SIZE                      (CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE / BLE_LOG_TRANS_BUF_CNT)
#define BLE_LOG_TRANS_LL_SIZE                   (CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE / BLE_LOG_TRANS_BUF_CNT)

/* Unified queue depth derivation */
#define BLE_LOG_TRANS_POOL_CNT                  (BLE_LOG_LBM_CNT * BLE_LOG_TRANS_BUF_CNT)
#if BLE_LOG_UART_REDIR_ENABLED
#define BLE_LOG_TRANS_REDIR_CNT                 BLE_LOG_TRANS_BUF_CNT
#else
#define BLE_LOG_TRANS_REDIR_CNT                 (0)
#endif
#define BLE_LOG_TRANS_TOTAL_CNT                 (BLE_LOG_TRANS_POOL_CNT + BLE_LOG_TRANS_REDIR_CNT)

/* ------------------------------------------ */
/*     Log Buffer Manager Context Defines     */
/* ------------------------------------------ */
typedef struct {
    union {
        ble_log_lbm_t lbm_pool[BLE_LOG_LBM_CNT];
        struct {
            union {
                ble_log_lbm_t lbm_common_pool[BLE_LOG_LBM_COMMON_CNT];
                struct {
                    union {
                        ble_log_lbm_t spin_pool[BLE_LOG_LBM_SPIN_MAX];
                        struct {
                            ble_log_lbm_t spin_task;
                            ble_log_lbm_t spin_isr;
                        };
                    };
                    union {
                        ble_log_lbm_t atomic_pool[BLE_LOG_LBM_ATOMIC_CNT];
                        struct {
                            ble_log_lbm_t atomic_pool_task[BLE_LOG_LBM_ATOMIC_TASK_CNT];
                            ble_log_lbm_t atomic_pool_isr[BLE_LOG_LBM_ATOMIC_ISR_CNT];
                        };
                    };
                };
            };
            union {
                ble_log_lbm_t lbm_ll_pool[BLE_LOG_LBM_LL_MAX];
#if CONFIG_BLE_LOG_LL_ENABLED
                struct {
                    ble_log_lbm_t lbm_ll_task;
                    ble_log_lbm_t lbm_ll_hci;
                };
#endif /* CONFIG_BLE_LOG_LL_ENABLED */
            };
        };
    };
} ble_log_lbm_ctx_t;

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
_Static_assert(CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE % BLE_LOG_TRANS_BUF_CNT == 0,
               "Common LBM total buffer size must be a multiple of BLE_LOG_TRANS_BUF_CNT (4)");
#if CONFIG_BLE_LOG_LL_ENABLED
_Static_assert(CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE % BLE_LOG_TRANS_BUF_CNT == 0,
               "LL LBM total buffer size must be a multiple of BLE_LOG_TRANS_BUF_CNT (4)");
#endif
_Static_assert(CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE / BLE_LOG_TRANS_BUF_CNT >= BLE_LOG_FRAME_OVERHEAD,
               "Common LBM per-buffer size too small for a single frame");
_Static_assert((BLE_LOG_TRANS_BUF_CNT & (BLE_LOG_TRANS_BUF_CNT - 1)) == 0,
               "BLE_LOG_TRANS_BUF_CNT must be a power of 2");
_Static_assert(1 + BLE_LOG_LBM_ATOMIC_TASK_CNT <= 16,
               "Common task pool exceeds lbm_id 4-bit index limit (max 15)");
_Static_assert(1 + BLE_LOG_LBM_ATOMIC_ISR_CNT <= 16,
               "Common ISR pool exceeds lbm_id 4-bit index limit (max 15)");
_Static_assert(BLE_LOG_TRANS_BUF_CNT <= 255,
               "BLE_LOG_TRANS_BUF_CNT must fit in uint8_t for ble_log_buf_util_t");

/* --------------------------- */
/*     Internal Interfaces     */
/* --------------------------- */
bool ble_log_lbm_init(void);
void ble_log_lbm_deinit(void);
void ble_log_lbm_enable(bool enable);
void ble_log_write_enh_stat(void);
void ble_log_write_buf_util(void);
#if BLE_LOG_UART_REDIR_ENABLED
void ble_log_lbm_stream_write(ble_log_lbm_t *lbm, ble_log_src_t src_code,
                               const uint8_t *data, size_t len);
void ble_log_lbm_stream_flush(ble_log_lbm_t *lbm, ble_log_src_t src_code);
ble_log_lbm_t *ble_log_prph_get_redir_lbm(void);
#endif

#endif /* __BLE_LOG_LBM_H__ */
