/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_timer.h"
#include "esp_clk_tree.h"
#include "esp_cpu.h"
#include "esp_private/esp_clk.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_trace_util.h"
#include "esp_trace_types.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// TIMESTAMP ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Determine which timer to use as timestamp source
#if CONFIG_ESP_TRACE_TS_SOURCE_CCOUNT
#define TS_USE_CCOUNT 1
#elif CONFIG_ESP_TRACE_TS_SOURCE_ESP_TIMER
#define TS_USE_ESP_TIMER 1
#else
#define TS_USE_TIMERGROUP 1
#endif

#if TS_USE_TIMERGROUP
#include "driver/gptimer.h"

// Timer group timer divisor
#define ESP_TRACE_TIMER_DIV       2

// GPTimer handle
static gptimer_handle_t s_trace_gptimer;

#endif // TS_USE_TIMERGROUP

#if TS_USE_ESP_TIMER
// esp_timer provides 1us resolution
#define ESP_TRACE_TIMESTAMP_FREQ  (1000000)
#endif // TS_USE_ESP_TIMER

#if TS_USE_CCOUNT
// CCOUNT is incremented at CPU frequency
#define ESP_TRACE_TIMESTAMP_FREQ  (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000)
#endif // TS_USE_CCOUNT

// System Frequency.
#define ESP_TRACE_CPU_FREQ        (esp_clk_cpu_freq())

uint32_t esp_trace_timestamp_init(void)
{
    /* We only need to initialize something if we use Timer Group.
     * esp_timer and ccount can be used as is.
     */
#if TS_USE_TIMERGROUP
    // get clock source frequency
    uint32_t counter_src_hz = 0;
    ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)GPTIMER_CLK_SRC_DEFAULT,
                                                 ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &counter_src_hz));
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = counter_src_hz / ESP_TRACE_TIMER_DIV,
    };
    // pick any free GPTimer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&config, &s_trace_gptimer));
    /* Start counting */
    gptimer_enable(s_trace_gptimer);
    gptimer_start(s_trace_gptimer);
    return config.resolution_hz;
#else
    return ESP_TRACE_TIMESTAMP_FREQ;
#endif // TS_USE_TIMERGROUP
}

uint32_t esp_trace_timestamp_get(void)
{
#if TS_USE_TIMERGROUP
    uint64_t ts = 0;
    gptimer_get_raw_count(s_trace_gptimer, &ts);
    return (uint32_t)ts; // return lower part of counter value
#elif TS_USE_CCOUNT
    return esp_cpu_get_cycle_count();
#elif TS_USE_ESP_TIMER
    return (uint32_t) esp_timer_get_time(); // return lower part of counter value
#endif
}

uint32_t esp_trace_cpu_freq_get(void)
{
    return esp_clk_cpu_freq();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// TIMEOUT /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void esp_trace_tmo_init(esp_trace_tmo_t *tmo, uint32_t user_tmo)
{
    tmo->start = esp_timer_get_time();
    tmo->tmo = user_tmo == ESP_TRACE_TMO_INFINITE ? (int64_t) -1 : (int64_t)user_tmo;
    tmo->elapsed = 0;
}

esp_err_t esp_trace_tmo_check(esp_trace_tmo_t *tmo)
{
    if (tmo->tmo != (int64_t) -1) {
        tmo->elapsed = esp_timer_get_time() - tmo->start;
        if (tmo->elapsed >= tmo->tmo) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LOCKS ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void esp_trace_lock_init(esp_trace_lock_t *lock)
{
    portMUX_INITIALIZE(&lock->mux);
    lock->int_state = 0;
}

esp_err_t esp_trace_lock_take(esp_trace_lock_t *lock, uint32_t tmo_us)
{
    esp_trace_tmo_t tmo;
    esp_trace_tmo_init(&tmo, tmo_us);

    while (1) {
        // Try to enter critical section (take the spinlock) with 0 timeout
        if (portTRY_ENTER_CRITICAL(&lock->mux, 0) == pdTRUE) {
            return ESP_OK;
        }

        // Failed to enter critical section, check timeout
        if (esp_trace_tmo_check(&tmo) != ESP_OK) {
            return ESP_ERR_TIMEOUT;
        }
    }
}

esp_err_t esp_trace_lock_give(esp_trace_lock_t *lock)
{
    portEXIT_CRITICAL(&lock->mux);
    return ESP_OK;
}
