/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_etm.h"
#include "esp_async_memcpy.h"
#include "soc/soc_caps.h"

#define DEFAULT_TRANSACTION_QUEUE_LENGTH 4

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MCP_FSM_IDLE_WAIT, /// intermediate state, for state changes from others to IDLE
    MCP_FSM_IDLE,
    MCP_FSM_RUN_WAIT, /// intermediate state, for state changes from others to RUN
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

#ifdef __cplusplus
}
#endif
