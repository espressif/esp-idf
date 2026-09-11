/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Pure classification helpers for PERIODIC_SYNC_LOST (host-testable).
 *
 * SYNC_LOST carries only a handle. Handle reuse cannot be detected here, so
 * classification uses current vs retiring handles plus the local-terminate
 * flag — not a generation counter.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESL_SYNC_HANDLE_NONE
#define ESL_SYNC_HANDLE_NONE  0xffffu
#endif

typedef enum {
    ESL_SYNC_LOST_CURRENT_NATURAL = 0,
    ESL_SYNC_LOST_RETIRING_LOCAL,
    ESL_SYNC_LOST_STALE,
} esl_sync_lost_class_t;

typedef struct {
    uint16_t current_sync_handle;
    uint16_t retiring_sync_handle;
    bool retiring_local_terminate;
} esl_sync_lost_ctx_t;

static inline esl_sync_lost_class_t esl_classify_sync_lost(const esl_sync_lost_ctx_t *ctx,
                                                           uint16_t evt_sync_handle)
{
    if (ctx == NULL) {
        return ESL_SYNC_LOST_STALE;
    }

    if (evt_sync_handle == ctx->retiring_sync_handle &&
            ctx->retiring_sync_handle != ESL_SYNC_HANDLE_NONE &&
            ctx->retiring_local_terminate) {
        return ESL_SYNC_LOST_RETIRING_LOCAL;
    }

    if (evt_sync_handle == ctx->current_sync_handle &&
            ctx->current_sync_handle != ESL_SYNC_HANDLE_NONE) {
        return ESL_SYNC_LOST_CURRENT_NATURAL;
    }

    return ESL_SYNC_LOST_STALE;
}

#ifdef __cplusplus
}
#endif
