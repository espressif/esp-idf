// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <sdkconfig.h>

#define HEAP_TRACE_SRCFILE /* don't warn on inclusion here */
#include "esp_heap_trace.h"
#undef HEAP_TRACE_SRCFILE

#if CONFIG_SYSVIEW_ENABLE
#include "esp_app_trace.h"
#include "esp_sysview_trace.h"
#endif

#define STACK_DEPTH CONFIG_HEAP_TRACING_STACK_DEPTH

#ifdef CONFIG_HEAP_TRACING_TOHOST

#if !CONFIG_SYSVIEW_ENABLE
#error None of the heap tracing backends is enabled! You must enable SystemView compatible tracing to use this feature.
#endif

static bool s_tracing;

esp_err_t heap_trace_init_tohost()
{
    if (s_tracing) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t heap_trace_start(heap_trace_mode_t mode_param)
{
#if CONFIG_SYSVIEW_ENABLE
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
#if CONFIG_SYSVIEW_ENABLE
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

void heap_trace_dump(void)
{
    return;
}

/* Add a new allocation to the heap trace records */
static IRAM_ATTR void record_allocation(const heap_trace_record_t *record)
{
    if (!s_tracing) {
        return;
    }
#if CONFIG_SYSVIEW_ENABLE
    esp_sysview_heap_trace_alloc(record->address, record->size, record->alloced_by);
#endif
}

/* record a free event in the heap trace log

   For HEAP_TRACE_ALL, this means filling in the freed_by pointer.
   For HEAP_TRACE_LEAKS, this means removing the record from the log.
*/
static IRAM_ATTR void record_free(void *p, void **callers)
{
    if (!s_tracing) {
        return;
    }
#if CONFIG_SYSVIEW_ENABLE
    esp_sysview_heap_trace_free(p, callers);
#endif
}

#include "heap_trace.inc"

#endif /*CONFIG_HEAP_TRACING_TOHOST*/

