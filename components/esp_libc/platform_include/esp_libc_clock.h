/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <time.h>
#include <sys/timex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Operations for a clock source
 *
 * Callbacks return 0 on success, -1 on error (with errno set).
 */
typedef struct {
    int (*gettime)(struct timespec *tp, void *ctx);       /*!< Get current time (required) */
    int (*settime)(const struct timespec *tp, void *ctx); /*!< Set current time (optional, may be NULL) */
    int (*adjtime)(struct timex *buf, void *ctx);         /*!< Adjust clock via struct timex (optional, may be NULL) */
    int (*getres)(struct timespec *res, void *ctx);       /*!< Get clock resolution (required) */
} esp_libc_clock_ops_t;

/**
 * @brief Link-time clock descriptor (placed in .esp_libc_clock_desc)
 */
typedef struct {
    clockid_t clk_id;
    esp_libc_clock_ops_t ops;
    void *ctx;
} esp_libc_clock_desc_t;

/**
 * @brief Register a clock at link time
 *
 * Place this macro at file scope after defining the clock operation functions.
 * The linker collects all descriptors into a contiguous array; clock_gettime(),
 * clock_settime(), clock_getres(), and clock_adjtime() dispatch by clock ID.
 *
 * @note The entity registering the clock is responsible for ensuring the
 * clock operations functions are thread safe.
 *
 * @param name_token  Unique C identifier token for the static descriptor symbol
 * @param clk_id_val  Clock ID (e.g. CLOCK_REALTIME or a custom clockid_t)
 * @param ops_symbol  lvalue of type esp_libc_clock_ops_t (e.g. s_my_ops)
 * @param ctx_val     Context pointer passed to callbacks (may be NULL)
 */
#define ESP_LIBC_CLOCK_REGISTER(name_token, clk_id_val, ops_symbol, ctx_val)   \
    static const esp_libc_clock_desc_t _esp_libc_clock_desc_##name_token       \
    __attribute__((used, section(".esp_libc_clock_desc"))) = {                 \
        .clk_id = (clk_id_val), .ops = (ops_symbol), .ctx = (ctx_val) }

#ifdef __cplusplus
}
#endif
