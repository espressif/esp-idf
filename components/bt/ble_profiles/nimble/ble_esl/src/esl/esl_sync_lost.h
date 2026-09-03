/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Pure classification helpers for PERIODIC_SYNC_LOST (host-testable).
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
    uint32_t sync_generation;
    uint16_t retiring_sync_handle;
    uint32_t retiring_sync_generation;
    bool retiring_local_terminate;
} esl_sync_lost_ctx_t;

static inline uint32_t esl_sync_lost_generation_hint(const esl_sync_lost_ctx_t *ctx,
                                                     uint16_t evt_sync_handle)
{
    if (ctx == NULL) {
        return 0;
    }
    if (evt_sync_handle == ctx->current_sync_handle &&
            ctx->current_sync_handle != ESL_SYNC_HANDLE_NONE) {
        return ctx->sync_generation;
    }
    if (evt_sync_handle == ctx->retiring_sync_handle &&
            ctx->retiring_sync_handle != ESL_SYNC_HANDLE_NONE) {
        return ctx->retiring_sync_generation;
    }
    return 0;
}

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
        if (esl_sync_lost_generation_hint(ctx, evt_sync_handle) == ctx->sync_generation) {
            return ESL_SYNC_LOST_CURRENT_NATURAL;
        }
    }

    return ESL_SYNC_LOST_STALE;
}

#ifdef __cplusplus
}
#endif
