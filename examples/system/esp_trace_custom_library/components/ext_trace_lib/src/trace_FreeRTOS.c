/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Implementations of the trace*() hooks declared in trace_FreeRTOS.h.
 *
 * Each hook encodes a single human-readable line that can be observed directly
 * in any serial monitor — no decoder is needed:
 *
 *   [+    123 us] TASK_IN      Task 1
 *   [+   1000 us] ISR_IN       irq=5
 *
 * The leading number is the time elapsed since the previous traced event.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace_FreeRTOS.h"
#include "esp_trace.h"
#include "esp_trace_port_encoder.h"
#include "esp_trace_util.h"

static esp_trace_handle_t s_esp_trace_handle = NULL;
static esp_trace_encoder_t *s_enc = NULL;
static uint32_t s_ts_freq_hz = 1000000;   /* default assume 1 MHz */
static uint32_t s_last_ts = 0;
static volatile bool s_enabled = false;

void init_trace_lib(esp_trace_encoder_t *enc)
{
    s_esp_trace_handle = esp_trace_get_active_handle();
    s_enc = enc;

    uint32_t freq = esp_trace_timestamp_init();
    if (freq != 0) {
        s_ts_freq_hz = freq;
    }
    s_last_ts = esp_trace_timestamp_get();
}

void trace_lib_start(void)
{
    if (!s_esp_trace_handle) {
        return;
    }

    s_last_ts = esp_trace_timestamp_get();
    s_enabled = true;
}

void trace_lib_stop(void)
{
    s_enabled = false;
}

/* Encode one trace line and write it through. */
static void encode(const char *type, const char *detail)
{
    if (!s_enabled || !s_esp_trace_handle || !s_enc) {
        return;
    }

    unsigned int int_state = s_enc->vt->take_lock(s_enc, ESP_TRACE_TMO_INFINITE);

    uint32_t now   = esp_trace_timestamp_get();
    uint32_t delta = now - s_last_ts;     /* uint32 modular subtraction handles wrap */
    s_last_ts = now;

    uint32_t delta_us = (s_ts_freq_hz == 1000000)
                        ? delta
                        : (uint32_t)((uint64_t)delta * 1000000ULL / s_ts_freq_hz);

    char line[96];
    int n = snprintf(line, sizeof(line), "[+%7lu us] %-12s %s\n",
                     (unsigned long)delta_us, type, detail ? detail : "");
    if (n > 0) {
        if (n >= (int)sizeof(line)) {
            n = (int)sizeof(line) - 1;
        }
        esp_trace_write(s_esp_trace_handle, line, (size_t)n, 0);
    }

    s_enc->vt->give_lock(s_enc, int_state);
}

void trace_lib_task_switched_in(void)
{
    TaskHandle_t h = xTaskGetCurrentTaskHandle();
    encode("TASK_IN", h ? pcTaskGetName(h) : "?");
}

void trace_lib_task_create(void *pxNewTCB)
{
    if (!pxNewTCB) {
        encode("TASK_CREATE", "(null)");
        return;
    }
    encode("TASK_CREATE", pcTaskGetName((TaskHandle_t)pxNewTCB));
}

void trace_lib_isr_enter(uint32_t irq)
{
    char d[24];
    snprintf(d, sizeof(d), "irq=%lu", (unsigned long)irq);
    encode("ISR_IN", d);
}

void trace_lib_isr_exit(void)
{
    encode("ISR_OUT", "");
}

void trace_lib_isr_exit_to_scheduler(void)
{
    encode("ISR_YIELD", "");
}

void trace_lib_queue_send(void *pxQueue)
{
    char d[24];
    snprintf(d, sizeof(d), "q=%p", pxQueue);
    encode("Q_SEND", d);
}

void trace_lib_queue_receive(void *pxQueue)
{
    char d[24];
    snprintf(d, sizeof(d), "q=%p", pxQueue);
    encode("Q_RECEIVE", d);
}

void trace_lib_queue_create(void *pxNewQueue)
{
    char d[24];
    snprintf(d, sizeof(d), "q=%p", pxNewQueue);
    encode("Q_CREATE", d);
}
