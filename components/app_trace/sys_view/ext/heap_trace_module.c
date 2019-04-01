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
#include <stdint.h>
#include <sdkconfig.h>
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"
#include "esp_app_trace.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
const static char *TAG = "sysview_heap_trace";

#ifdef CONFIG_HEAP_TRACING_STACK_DEPTH
#define CALLSTACK_SIZE  CONFIG_HEAP_TRACING_STACK_DEPTH
#else
#define CALLSTACK_SIZE  0
#endif

static SEGGER_SYSVIEW_MODULE s_esp_sysview_heap_module = {
    .sModule = "ESP32 SystemView Heap Tracing Module",
    .NumEvents = 2,
};

static bool s_mod_registered;


esp_err_t esp_sysview_heap_trace_start(uint32_t tmo)
{
    uint32_t tmo_ticks = tmo/(1000*portTICK_PERIOD_MS);

    ESP_EARLY_LOGV(TAG, "%s", __func__);
    do {
        if (tmo != (uint32_t)-1) {
            // Currently timeout implementation is simple and has granularity of 1 OS tick,
            // so just count down the number of times to call vTaskDelay
            if (tmo_ticks-- == 0) {
                return ESP_ERR_TIMEOUT;
            }
        }
        vTaskDelay(1);
    } while(!SEGGER_SYSVIEW_Started());

    SEGGER_SYSVIEW_RegisterModule(&s_esp_sysview_heap_module);
    s_mod_registered = true;
    return ESP_OK;
}

esp_err_t esp_sysview_heap_trace_stop(void)
{
    ESP_EARLY_LOGV(TAG, "%s", __func__);
    SEGGER_RTT_ESP32_Flush(0, ESP_APPTRACE_TMO_INFINITE);
    return ESP_OK;
}

void esp_sysview_heap_trace_alloc(const void *addr, uint32_t size, const void *callers)
{
    U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + (2+CALLSTACK_SIZE)*SEGGER_SYSVIEW_QUANTA_U32];
    U8* pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    U32 *calls = (U32 *)callers;

    if (!s_mod_registered) {
        return;
    }
    ESP_EARLY_LOGV(TAG, "%s %p %lu", __func__, addr, size);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, (U32)addr);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, size);
    for (int i = 0; i < CALLSTACK_SIZE; i++) {
        pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, calls[i]);
    }
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, s_esp_sysview_heap_module.EventOffset + 0);
}

void esp_sysview_heap_trace_free(const void *addr, const void *callers)
{
    U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + (1+CALLSTACK_SIZE)*SEGGER_SYSVIEW_QUANTA_U32];
    U8* pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    U32 *calls = (U32 *)callers;

    if (!s_mod_registered) {
        return;
    }
    ESP_EARLY_LOGV(TAG, "%s %p", __func__, addr);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, (U32)addr);
    for (int i = 0; i < CALLSTACK_SIZE; i++) {
        pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, calls[i]);
    }
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, s_esp_sysview_heap_module.EventOffset + 1);
}
