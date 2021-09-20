// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

// This module implements runtime file I/O API for GCOV.

#include <string.h>
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/cpu.h"
#include "soc/timer_periph.h"
#include "esp_app_trace.h"
#include "esp_freertos_hooks.h"
#include "esp_private/dbg_stubs.h"
#include "esp_ipc.h"
#include "hal/wdt_hal.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/libc_stubs.h"
#endif

#if CONFIG_APPTRACE_GCOV_ENABLE

#define ESP_GCOV_DOWN_BUF_SIZE  4200

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_gcov_rtio";
static volatile bool s_create_gcov_task = false;
static volatile bool s_gcov_task_running = false;

extern void __gcov_dump(void);
extern void __gcov_reset(void);

void gcov_dump_task(void *pvParameter)
{
    int dump_result = 0;
    bool *running = (bool *)pvParameter;

    ESP_EARLY_LOGV(TAG, "%s stack use in %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));

    ESP_EARLY_LOGV(TAG, "Alloc apptrace down buf %d bytes", ESP_GCOV_DOWN_BUF_SIZE);
    void *down_buf = malloc(ESP_GCOV_DOWN_BUF_SIZE);
    if (down_buf == NULL) {
        ESP_EARLY_LOGE(TAG, "Could not allocate memory for the buffer");
        dump_result = ESP_ERR_NO_MEM;
        goto gcov_exit;
    }
    ESP_EARLY_LOGV(TAG, "Config apptrace down buf");
    esp_apptrace_down_buffer_config(down_buf, ESP_GCOV_DOWN_BUF_SIZE);
    /* we are directing the std outputs to the fake ones in order to reduce stack usage */
    FILE *old_stderr = stderr;
    FILE *old_stdout = stdout;
    stderr = (FILE *) &__sf_fake_stderr;
    stdout = (FILE *) &__sf_fake_stdout;
    ESP_EARLY_LOGV(TAG, "Dump data...");
    __gcov_dump();
    // reset dump status to allow incremental data accumulation
    __gcov_reset();
    free(down_buf);
    stderr = old_stderr;
    stdout = old_stdout;
    ESP_EARLY_LOGV(TAG, "Finish file transfer session");
    dump_result = esp_apptrace_fstop(ESP_APPTRACE_DEST_TRAX);
    if (dump_result != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send files transfer stop cmd (%d)!", dump_result);
    }

gcov_exit:
    ESP_EARLY_LOGV(TAG, "dump_result %d", dump_result);
    if (running) {
        *running = false;
    }

    ESP_EARLY_LOGV(TAG, "%s stack use out %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));

    vTaskDelete(NULL);
}

void gcov_create_task(void *arg)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    xTaskCreatePinnedToCore(&gcov_dump_task, "gcov_dump_task", 2048, (void *)&s_gcov_task_running, configMAX_PRIORITIES - 1, NULL, 0);
}

void gcov_create_task_tick_hook(void)
{
    extern esp_err_t esp_ipc_start_gcov_from_isr(uint32_t cpu_id, esp_ipc_func_t func, void* arg);
    if (s_create_gcov_task) {
        if (esp_ipc_start_gcov_from_isr(xPortGetCoreID(), &gcov_create_task, NULL) == ESP_OK) {
            s_create_gcov_task = false;
        }
    }
}

/**
 * @brief Triggers gcov info dump task
 *        This function is to be called by OpenOCD, not by normal user code.
 * TODO: what about interrupted flash access (when cache disabled)
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
static int esp_dbg_stub_gcov_entry(void)
{
    /* we are in isr context here */
    s_create_gcov_task = true;
    return ESP_OK;
}

int gcov_rtio_atexit(void (*function)(void) __attribute__ ((unused)))
{
    uint32_t capabilities = 0;
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_GCOV, (uint32_t)&esp_dbg_stub_gcov_entry);
    if (esp_dbg_stub_entry_get(ESP_DBG_STUB_ENTRY_CAPABILITIES, &capabilities) == ESP_OK) {
        esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_CAPABILITIES, capabilities | ESP_DBG_STUB_CAP_GCOV_TASK);
    }
    esp_register_freertos_tick_hook(gcov_create_task_tick_hook);
    return ESP_OK;
}

void esp_gcov_dump(void)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);

    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX)) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* We are not in isr context here. Waiting for the completion is safe */
    s_gcov_task_running = true;
    s_create_gcov_task = true;
    while (s_gcov_task_running) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void *gcov_rtio_fopen(const char *path, const char *mode)
{
    ESP_EARLY_LOGV(TAG, "%s '%s' '%s'", __FUNCTION__, path, mode);
    void *f = esp_apptrace_fopen(ESP_APPTRACE_DEST_TRAX, path, mode);
    ESP_EARLY_LOGV(TAG, "%s ret %p", __FUNCTION__, f);
    return f;
}

int gcov_rtio_fclose(void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fclose(ESP_APPTRACE_DEST_TRAX, stream);
}

size_t gcov_rtio_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s read %u", __FUNCTION__, size * nmemb);
    size_t sz = esp_apptrace_fread(ESP_APPTRACE_DEST_TRAX, ptr, size, nmemb, stream);
    ESP_EARLY_LOGV(TAG, "%s actually read %u", __FUNCTION__, sz);
    return sz;
}

size_t gcov_rtio_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fwrite(ESP_APPTRACE_DEST_TRAX, ptr, size, nmemb, stream);
}

int gcov_rtio_fseek(void *stream, long offset, int whence)
{
    int ret = esp_apptrace_fseek(ESP_APPTRACE_DEST_TRAX, stream, offset, whence);
    ESP_EARLY_LOGV(TAG, "%s(%p %ld %d) = %d", __FUNCTION__, stream, offset, whence, ret);
    return ret;
}

long gcov_rtio_ftell(void *stream)
{
    long ret = esp_apptrace_ftell(ESP_APPTRACE_DEST_TRAX, stream);
    ESP_EARLY_LOGV(TAG, "%s(%p) = %ld", __FUNCTION__, stream, ret);
    return ret;
}
#endif
