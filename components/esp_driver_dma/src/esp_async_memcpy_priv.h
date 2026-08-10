/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "esp_err.h"
#include "esp_etm.h"
#include "esp_cache.h"
#include "esp_async_memcpy.h"
#include "soc/soc_caps.h"

#define DEFAULT_TRANSACTION_QUEUE_LENGTH 4

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MCP_FSM_WAIT, /// intermediate state, for state changes between IDLE and RUN
    MCP_FSM_IDLE,
    MCP_FSM_RUN,
} async_memcpy_fsm_t;

typedef struct async_memcpy_context_t async_memcpy_context_t;

struct async_memcpy_context_t {
    /// @brief Start a new async memcpy transaction
    esp_err_t (*memcpy)(async_memcpy_context_t *ctx, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args);
#if SOC_ETM_SUPPORTED
    /// @brief Create ETM event handle of specific event type
    esp_err_t (*new_etm_event)(async_memcpy_context_t *ctx, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event);
#endif // SOC_ETM_SUPPORTED
    /// @brief Delete async memcpy driver context
    esp_err_t (*del)(async_memcpy_context_t *ctx);
};

/// @brief Cache aligned split of an async memcpy transaction.
/// @note The DMA engine only handles the cache aligned body. The unaligned head and tail (if any)
///       are copied by the CPU right before the DMA transfer starts (see
///       `async_memcpy_do_cache_ops_and_cpu_copy`). The head/tail and the body never share a cache
///       line, so the CPU and DMA can work concurrently without cache coherency conflicts. This
///       avoids the need for a stash buffer and the merge step in the ISR.
typedef struct {
    void *src;          ///< Source buffer of the whole transaction (for the deferred CPU copy and cache ops)
    void *dst;          ///< Destination buffer of the whole transaction (for the deferred CPU copy and cache ops)
    size_t head_len;    ///< Bytes of the unaligned head (copied by CPU)
    size_t body_len;    ///< Bytes of the cache aligned body (copied by DMA)
    size_t tail_len;    ///< Bytes of the unaligned tail (copied by CPU)
    void *body_src;     ///< Source address of the body
    void *body_dst;     ///< Destination address of the body
    size_t cache_line_size; ///< Cache line size used for the split (0 if the memory is not cached)
} async_memcpy_split_t;

/// @brief Split an async memcpy transaction into a cache aligned body and (optional) head/tail.
/// @note This function only computes the split. It does NOT touch any memory or cache, the caller
///       must invoke `async_memcpy_do_cache_ops_and_cpu_copy` (right before the DMA transfer starts)
///       to actually copy the head/tail and do the required cache maintenance.
/// @note The top level esp_async_memcpy guarantees n >= 2 * cache_line_size, so a non-zero body is
///       always available. This is asserted here defensively.
static inline esp_err_t async_memcpy_split_cache_aligned(void *dst, void *src, size_t n, async_memcpy_split_t *split)
{
    split->src = src;
    split->dst = dst;
    split->head_len = 0;
    split->tail_len = 0;
    split->body_len = n;
    split->body_src = src;
    split->body_dst = dst;
    split->cache_line_size = esp_cache_get_line_size_by_addr(dst);

    if (split->cache_line_size) {
        size_t head_overflow = (uintptr_t)dst % split->cache_line_size;
        split->head_len = head_overflow ? (split->cache_line_size - head_overflow) : 0;
        // the top level esp_async_memcpy guarantees n >= 2 * cache_line_size, so a non-zero body is always available
        if (split->head_len >= n) {
            return ESP_ERR_INVALID_SIZE;
        }
        split->tail_len = (n - split->head_len) % split->cache_line_size;
        split->body_len = n - split->head_len - split->tail_len;
        if (split->body_len == 0) {
            return ESP_ERR_INVALID_SIZE;
        }
        split->body_src = (uint8_t *)src + split->head_len;
        split->body_dst = (uint8_t *)dst + split->head_len;
    }

    return ESP_OK;
}

/// @brief Do the CPU copy of the unaligned head/tail and the cache maintenance for a split.
/// @note This must be called from `try_start_pending_transaction`, right before the DMA transfer
///       actually kicks off. Because the async memcpy allows different requests to use the same or
///       overlapping destination buffers, doing this at submit time would race with a previous
///       (still running) DMA transfer that writes to the same destination. By deferring it to the
///       moment the transfer starts (which, with a single channel FSM, happens only after the
///       previous transfer has completed), the CPU copies and cache operations are serialized with
///       the DMA engine.
/// @note The head/tail cache lines are disjoint from the body's, so the CPU copies don't interfere
///       with the body transferred by the DMA.
static inline esp_err_t async_memcpy_do_cache_ops_and_cpu_copy(const async_memcpy_split_t *split)
{
    size_t n = split->head_len + split->body_len + split->tail_len;

    // CPU copies the unaligned head and tail. These cache lines are disjoint from the body's.
    if (split->head_len) {
        memcpy(split->dst, split->src, split->head_len);
    }
    if (split->tail_len) {
        memcpy((uint8_t *)split->body_dst + split->body_len, (uint8_t *)split->body_src + split->body_len, split->tail_len);
    }

    // write back the source data if it's behind the cache, so that the DMA can read the latest data
    if (esp_cache_get_line_size_by_addr(split->src) > 0) {
        esp_cache_msync(split->src, n, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }
    // invalidate the destination body if it's behind the cache, so that the CPU can see the data written
    // by the DMA engine later (the head/tail cache lines are kept intact, they hold the CPU data)
    if (split->cache_line_size) {
        esp_cache_msync(split->body_dst, split->body_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    }

    return ESP_OK;
}

#ifdef __cplusplus
}
#endif
