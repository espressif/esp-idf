/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// TIMESTAMP ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize the trace timestamp source.
 *
 * This function initializes the trace timestamp source based on the configured
 * source. The timestamp source can be the CPU cycle counter, esp_timer, or
 * Timer Group, depending on configuration.
 *
 * @return The timestamp frequency in Hz.
 */
uint32_t esp_trace_timestamp_init(void);

/**
 * @brief Get the current timestamp value from the configured source for ESP trace.
 *
 * This function returns the current timestamp value, which can be sourced from
 * the CPU cycle counter, esp_timer, or Timer Group, depending on configuration.
 *
 * @return The current timestamp value as a 32-bit unsigned integer.
 */
uint32_t esp_trace_timestamp_get(void);

/**
 * @brief Get the current CPU frequency in Hz.
 *
 * This function returns the current CPU frequency as reported by esp_clk_cpu_freq().
 *
 * @return The CPU frequency in Hz.
 */
uint32_t esp_trace_cpu_freq_get(void);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// TIMEOUT /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Timeout structure for trace operations
 *
 * This structure holds data necessary for measuring time intervals.
 * After initialization via esp_trace_tmo_init(), user needs to call esp_trace_tmo_check()
 * periodically to check timeout for expiration.
 */
typedef struct {
    int64_t start;   ///< Time interval start (in us)
    int64_t tmo;     ///< Timeout value (in us)
    int64_t elapsed; ///< Elapsed time (in us)
} esp_trace_tmo_t;

/**
 * @brief Initialize timeout structure
 *
 * @param tmo       Pointer to timeout structure to be initialized
 * @param user_tmo  Timeout value (in us). Use ESP_TRACE_TMO_INFINITE to wait indefinitely
 */
void esp_trace_tmo_init(esp_trace_tmo_t *tmo, uint32_t user_tmo);

/**
 * @brief Check timeout for expiration
 *
 * @param tmo Pointer to timeout structure
 * @return ESP_OK if not expired, ESP_ERR_TIMEOUT if expired
 */
esp_err_t esp_trace_tmo_check(esp_trace_tmo_t *tmo);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LOCKS ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "spinlock.h"

/**
 * @brief Trace component lock structure
 *
 * Similar to esp_apptrace_lock_t but for trace component locking.
 * This structure provides spinlock-based synchronization for trace components
 * (encoders, transports, and other trace components).
 */
typedef struct {
    spinlock_t mux;        ///< Spinlock for synchronization
    unsigned int int_state; ///< Interrupt state for recursive locking
} esp_trace_lock_t;

/**
 * @brief Initialize trace component lock
 * @param lock Pointer to lock structure to be initialized
 */
void esp_trace_lock_init(esp_trace_lock_t *lock);

/**
 * @brief Take trace component lock
 * @param lock Pointer to lock structure
 * @param tmo Timeout in microseconds
 * @return ESP_OK on success, ESP_ERR_TIMEOUT on timeout
 */
esp_err_t esp_trace_lock_take(esp_trace_lock_t *lock, uint32_t tmo_us);

/**
 * @brief Give trace component lock
 * @param lock Pointer to lock structure
 * @return ESP_OK on success
 */
esp_err_t esp_trace_lock_give(esp_trace_lock_t *lock);

#ifdef __cplusplus
}
#endif
