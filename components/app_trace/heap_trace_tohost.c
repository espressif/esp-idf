/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sdkconfig.h>

#define HEAP_TRACE_SRCFILE /* don't warn on inclusion here */
#include "esp_heap_trace.h"
#undef HEAP_TRACE_SRCFILE
#include "esp_heap_caps.h"
#if CONFIG_APPTRACE_SV_ENABLE
#include "esp_app_trace.h"
#include "esp_sysview_trace.h"
#endif

#define STACK_DEPTH CONFIG_HEAP_TRACING_STACK_DEPTH

#ifdef CONFIG_HEAP_TRACING_TOHOST

#if !CONFIG_APPTRACE_SV_ENABLE
#error None of the heap tracing backends is enabled! You must enable SystemView compatible tracing to use this feature.
#endif

static bool s_tracing;

esp_err_t heap_trace_init_tohost(void)
{
    if (s_tracing) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t heap_trace_start(heap_trace_mode_t mode_param)
{
#if CONFIG_APPTRACE_SV_ENABLE
    esp_err_t ret = esp_sysview_heap_trace_start((uint32_t)-1);
    if (ret != ESP_OK) {
        return ret;
    }
#endif
    s_tracing = true;
    return ESP_OK;
}

esp_err_t heap_trace_stop(void)
{
    esp_err_t ret = ESP_ERR_NOT_SUPPORTED;
#if CONFIG_APPTRACE_SV_ENABLE
    ret = esp_sysview_heap_trace_stop();
#endif
    s_tracing = false;
    return ret;
}

esp_err_t heap_trace_resume(void)
{
    return heap_trace_start(HEAP_TRACE_ALL);
}

size_t heap_trace_get_count(void)
{
    return 0;
}

esp_err_t heap_trace_get(size_t index, heap_trace_record_t *record)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t heap_trace_summary(heap_trace_summary_t *summary)
{
    return ESP_ERR_NOT_SUPPORTED;
}

void heap_trace_dump(void)
{
    return;
}

void heap_trace_dump_caps(__attribute__((unused)) const uint32_t caps)
{
    return;
}

/* Add a new allocation to the heap trace records */
static HEAP_IRAM_ATTR void record_allocation(const heap_trace_record_t *record)
{
    if (!s_tracing) {
        return;
    }
#if CONFIG_APPTRACE_SV_ENABLE
    esp_sysview_heap_trace_alloc(record->address, record->size, record->alloced_by);
#endif
}

/* record a free event in the heap trace log

   For HEAP_TRACE_ALL, this means filling in the freed_by pointer.
   For HEAP_TRACE_LEAKS, this means removing the record from the log.
*/
static HEAP_IRAM_ATTR void record_free(void *p, void **callers)
{
    if (!s_tracing) {
        return;
    }
#if CONFIG_APPTRACE_SV_ENABLE
    esp_sysview_heap_trace_free(p, callers);
#endif
}

#include "heap_trace.inc"

#endif /*CONFIG_HEAP_TRACING_TOHOST*/
