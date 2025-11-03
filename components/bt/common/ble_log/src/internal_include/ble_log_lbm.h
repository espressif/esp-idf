/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#define BLE_LOG_MAKE_FRAME_META(src_code, sn)   ((src_code & 0xFF) | (sn << 8))

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
    ble_log_prph_trans_t *trans[BLE_LOG_TRANS_PING_PONG_BUF_CNT];
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
#define BLE_LOG_TRANS_CNT                       (BLE_LOG_LBM_CNT * BLE_LOG_TRANS_PING_PONG_BUF_CNT)

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

/* ---------------------------------------- */
/*     Enhanced Statistics Data Defines     */
/* ---------------------------------------- */
#if CONFIG_BLE_LOG_ENH_STAT_ENABLED
typedef struct {
    uint8_t int_src_code;
    uint8_t src_code;
    uint32_t written_frame_cnt;
    uint32_t lost_frame_cnt;
    uint32_t written_bytes_cnt;
    uint32_t lost_bytes_cnt;
} __attribute__((packed)) ble_log_enh_stat_t;
#endif /* CONFIG_BLE_LOG_ENH_STAT_ENABLED */

/* -------------------------------------- */
/*     Log Statistics Manager Context     */
/* -------------------------------------- */
typedef struct {
    uint32_t frame_sn;
#if CONFIG_BLE_LOG_ENH_STAT_ENABLED
    ble_log_enh_stat_t enh_stat;
#endif /* CONFIG_BLE_LOG_ENH_STAT_ENABLED */
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

/* --------------------------- */
/*     Internal Interfaces     */
/* --------------------------- */
bool ble_log_lbm_init(void);
void ble_log_lbm_deinit(void);
ble_log_prph_trans_t **ble_log_lbm_get_trans(ble_log_lbm_t *lbm, size_t log_len);
void ble_log_lbm_enable(bool enable);
#if CONFIG_BLE_LOG_ENH_STAT_ENABLED
void ble_log_write_enh_stat(void);
#endif /* CONFIG_BLE_LOG_ENH_STAT_ENABLED */

#endif /* __BLE_LOG_LBM_H__ */
