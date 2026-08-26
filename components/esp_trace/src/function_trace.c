/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Compiler-instrumented function tracing runtime.
 *
 * When a component is built with -finstrument-functions, the compiler inserts a
 * call to __cyg_profile_func_enter/exit at the start and end of every function.
 * These hooks gate the events and forward them to the active trace encoder.
 */

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_trace_port_encoder.h"
#include "esp_trace_function_trace.h"
#include "esp_trace_internal.h"

#define NO_INSTRUMENT __attribute__((no_instrument_function))

static volatile bool s_hook_active;

#if CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START
/* Auto start: function tracing follows the encoder recording state. */
static bool s_app_enabled = true;
#else
/* Manual start: enabled only when the application calls esp_trace_function_trace_start(). */
static bool s_app_enabled;
#endif
/* Encoder recording state. Defaults true for encoders that do not report it. */
static bool s_encoder_recording = true;

/* Per-core re-entry guard so an interrupt cannot run the hooks recursively. */
static volatile bool s_in_hook[SOC_CPU_CORES_NUM];

static esp_trace_encoder_t *s_enc;
static void (*s_fn_enter)(esp_trace_encoder_t *enc, void *func, void *call_site);
static void (*s_fn_exit)(esp_trace_encoder_t *enc, void *func, void *call_site);

static esp_err_t resolve_encoder(void)
{
    if (s_enc) {
        return ESP_OK;
    }

    esp_trace_encoder_t *enc = esp_trace_get_active_encoder();
    if (!enc || !enc->vt) {
        return ESP_ERR_INVALID_STATE;
    }

    const esp_trace_encoder_vtable_t *vt = enc->vt;
    if (!vt->function_enter || !vt->function_exit) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    s_enc = enc;
    s_fn_enter = vt->function_enter;
    s_fn_exit = vt->function_exit;
    return ESP_OK;
}

static void update_hook_state(void)
{
    s_hook_active = s_app_enabled && s_encoder_recording && (s_enc != NULL);
}

void esp_trace_function_trace_notify_recording(bool active)
{
    s_encoder_recording = active;
#if CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START
    if (active) {
        (void)resolve_encoder();
    }
#endif
    update_hook_state();
}

esp_err_t esp_trace_function_trace_start(void)
{
    esp_err_t err = resolve_encoder();
    if (err != ESP_OK) {
        return err;
    }
    s_app_enabled = true;
    update_hook_state();
    return ESP_OK;
}

esp_err_t esp_trace_function_trace_stop(void)
{
    s_app_enabled = false;
    update_hook_state();
    return ESP_OK;
}

static inline NO_INSTRUMENT bool hook_acquire(void)
{
    UBaseType_t irq = portSET_INTERRUPT_MASK_FROM_ISR();
    bool acquired = false;
    int core = esp_cpu_get_core_id();
    if (!s_in_hook[core]) {
        s_in_hook[core] = true;
        acquired = true;
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(irq);
    return acquired;
}

static inline NO_INSTRUMENT void hook_release(void)
{
    s_in_hook[esp_cpu_get_core_id()] = false;
}

NO_INSTRUMENT void __cyg_profile_func_enter(void *func, void *call_site)
{
    if (!s_hook_active) {
        return;
    }
    if (!hook_acquire()) {
        return;
    }
    s_fn_enter(s_enc, func, call_site);
    hook_release();
}

NO_INSTRUMENT void __cyg_profile_func_exit(void *func, void *call_site)
{
    if (!s_hook_active) {
        return;
    }
    if (!hook_acquire()) {
        return;
    }
    s_fn_exit(s_enc, func, call_site);
    hook_release();
}
