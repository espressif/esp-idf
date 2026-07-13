/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_H__
#define __BLE_LOG_H__

/* ------- */
/* BLE Log */
/* ------- */

/* INCLUDE */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* TYPEDEF */
/* CRITICAL:
 * The number of BLE Log source code will directly determine the number of statistic manager
 * memory requirements, keep it as less as possible; it's recommended to use subcode for more
 * log data structure decoding */
typedef enum {
    /* Internal */
    BLE_LOG_SRC_INTERNAL = 0,

    /* Custom */
    BLE_LOG_SRC_CUSTOM,

    /* BLE Stack */
    BLE_LOG_SRC_LL_TASK,
    BLE_LOG_SRC_LL_HCI,
    BLE_LOG_SRC_LL_ISR,
    BLE_LOG_SRC_HOST,
    BLE_LOG_SRC_HCI,
    BLE_LOG_SRC_ENCODE,

    /* UART redirection (PORT 0 only) */
    BLE_LOG_SRC_REDIR,

    BLE_LOG_SRC_MAX,
} ble_log_src_t;

typedef struct {
    uint32_t count;
    uint32_t min_us;
    uint32_t max_us;
    uint32_t last_us;
    uint64_t total_us;
} ble_log_prph_latency_stat_t;

typedef struct {
    ble_log_prph_latency_stat_t owned;
    ble_log_prph_latency_stat_t spi;
    ble_log_prph_latency_stat_t queue_wait;
    ble_log_prph_latency_stat_t dma;
} ble_log_prph_latency_stats_t;

/* Unified buffer pool diagnostics. */
typedef struct {
    uint32_t task_wait_count;     /* task writes that blocked waiting for a buffer */
    uint32_t task_wait_max_us;    /* longest single backpressure wait */
    uint32_t task_wait_total_us;  /* cumulative backpressure wait time */
    uint32_t inflight_peak;       /* peak simultaneously in-flight pool buffers */
    uint32_t send_fail_count;     /* transports dropped because the driver queue was full (== lost_send_fail) */
    uint32_t lost_frame_cnt;      /* frames dropped since last reset (all sources) */
    uint32_t lost_bytes_cnt;      /* bytes dropped since last reset (all sources) */

    /* Breakdown of drop reasons since last reset. The first three sum to the
     * "no buffer to write into" losses (also counted in lost_frame_cnt);
     * send_fail_count is separate (buffer written but driver submit failed). */
    uint32_t lost_frame_too_large; /* single frame larger than one transport buffer */
    uint32_t lost_no_buffer_task;  /* task context could not obtain a buffer */
    uint32_t lost_no_buffer_isr;   /* ISR context could not obtain a buffer */
} ble_log_pool_stats_t;

/* HCI Log Direction */
#define BLE_LOG_HCI_DOWNSTREAM  0
#define BLE_LOG_HCI_UPSTREAM    1

/* HCI Log Write Macro
 * Encodes direction in MSB of data[0] (HCI type byte) before writing.
 * Safe because ble_log_write_hex -> ble_log_lbm_write_trans does synchronous memcpy.
 * Parser reads MSB to determine direction; old firmware with MSB=0 defaults to "sent". */
#define ble_log_write_hci(direction, data, len) do {    \
    (data)[0] |= ((direction) << 7);                   \
    ble_log_write_hex(BLE_LOG_SRC_HCI, (data), (len)); \
    (data)[0] &= 0x7F;                                 \
} while (0)

/* INTERFACE */
bool ble_log_init(void);
void ble_log_deinit(void);
bool ble_log_enable(bool enable);
void ble_log_flush(void);
bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len);
void ble_log_dump_to_console(void);
#if CONFIG_BLE_LOG_LL_ENABLED
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, uint32_t flag);
#endif /* CONFIG_BLE_LOG_LL_ENABLED */
#if CONFIG_BLE_LOG_TS_ENABLED
bool ble_log_sync_enable(bool enable);
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

void ble_log_write_attempt_bytes_get(uint32_t *bytes, uint32_t *bytes_ll);
void ble_log_write_attempt_bytes_reset(void);
void ble_log_prph_latency_stats_get(ble_log_prph_latency_stats_t *stats);
void ble_log_prph_latency_stats_reset(void);
void ble_log_pool_stats_get(ble_log_pool_stats_t *stats);
void ble_log_pool_stats_reset(void);

#endif /* __BLE_LOG_H__ */
