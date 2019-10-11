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

#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/cpu.h"
#include "soc/timer_periph.h"
#include "esp_app_trace.h"
#include "esp_private/dbg_stubs.h"
#include "hal/timer_ll.h"

#if CONFIG_ESP32_GCOV_ENABLE

#define ESP_GCOV_DOWN_BUF_SIZE  4200

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_gcov_rtio";

#if GCC_NOT_5_2_0
void __gcov_dump(void);
void __gcov_reset(void);
#else
/* The next code for old GCC */

static void (*s_gcov_exit)(void);
/* Root of a program/shared-object state */
struct gcov_root
{
  void *list;
  unsigned dumped : 1;  /* counts have been dumped.  */
  unsigned run_counted : 1;  /* run has been accounted for.  */
  struct gcov_root *next;
  struct gcov_root *prev;
};

/* Per-dynamic-object gcov state.  */
extern struct gcov_root __gcov_root;

static void esp_gcov_reset_status(void)
{
  __gcov_root.dumped = 0;
  __gcov_root.run_counted = 0;
}
#endif


static int esp_dbg_stub_gcov_dump_do(void)
{
    int ret = ESP_OK;

    ESP_EARLY_LOGV(TAG, "Alloc apptrace down buf %d bytes", ESP_GCOV_DOWN_BUF_SIZE);
    void *down_buf = malloc(ESP_GCOV_DOWN_BUF_SIZE);
    if (down_buf == NULL) {
        ESP_EARLY_LOGE(TAG, "Could not allocate memory for the buffer");
        return ESP_ERR_NO_MEM;
    }
    ESP_EARLY_LOGV(TAG, "Config apptrace down buf");
    esp_apptrace_down_buffer_config(down_buf, ESP_GCOV_DOWN_BUF_SIZE);
    ESP_EARLY_LOGV(TAG, "Dump data...");
#if GCC_NOT_5_2_0
    __gcov_dump();
    // reset dump status to allow incremental data accumulation
    __gcov_reset();
#else
    ESP_EARLY_LOGV(TAG, "Check for dump handler %p", s_gcov_exit);
    if (s_gcov_exit) {
        s_gcov_exit();
        // reset dump status to allow incremental data accumulation
        esp_gcov_reset_status();
    }
#endif
    ESP_EARLY_LOGV(TAG, "Free apptrace down buf");
    free(down_buf);
    ESP_EARLY_LOGV(TAG, "Finish file transfer session");
    ret = esp_apptrace_fstop(ESP_APPTRACE_DEST_TRAX);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send files transfer stop cmd (%d)!", ret);
    }
    return ret;
}

/**
 * @brief Triggers gcov info dump.
 *        This function is to be called by OpenOCD, not by normal user code.
 * TODO: what about interrupted flash access (when cache disabled)???
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
static int esp_dbg_stub_gcov_entry(void)
{
#if GCC_NOT_5_2_0
    return esp_dbg_stub_gcov_dump_do();
#else
    int ret = ESP_OK;
    // disable IRQs on this CPU, other CPU is halted by OpenOCD
    unsigned irq_state = portENTER_CRITICAL_NESTED();
    ret = esp_dbg_stub_gcov_dump_do();
    portEXIT_CRITICAL_NESTED(irq_state);
    return ret;
#endif
}

void esp_gcov_dump(void)
{
    // disable IRQs on this CPU, other CPU is halted by OpenOCD
    unsigned irq_state = portENTER_CRITICAL_NESTED();
#if !CONFIG_FREERTOS_UNICORE
    int other_core = xPortGetCoreID() ? 0 : 1;
    esp_cpu_stall(other_core);
#endif
    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX)) {
        // to avoid complains that task watchdog got triggered for other tasks
        timer_ll_wdt_set_protect(&TIMERG0, false);
        timer_ll_wdt_feed(&TIMERG0);
        timer_ll_wdt_set_protect(&TIMERG0, true);
        // to avoid reboot on INT_WDT
        timer_ll_wdt_set_protect(&TIMERG1, false);
        timer_ll_wdt_feed(&TIMERG1);
        timer_ll_wdt_set_protect(&TIMERG1, true);
    }

    esp_dbg_stub_gcov_dump_do();
#if !CONFIG_FREERTOS_UNICORE
    esp_cpu_unstall(other_core);
#endif
    portEXIT_CRITICAL_NESTED(irq_state);
}

int gcov_rtio_atexit(void (*function)(void) __attribute__ ((unused)))
{
#if GCC_NOT_5_2_0
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
#else
    ESP_EARLY_LOGV(TAG, "%s %p", __FUNCTION__, function);
    s_gcov_exit = function;
#endif
    esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_GCOV, (uint32_t)&esp_dbg_stub_gcov_entry);
    return 0;
}

void *gcov_rtio_fopen(const char *path, const char *mode)
{
    ESP_EARLY_LOGV(TAG, "%s '%s' '%s'", __FUNCTION__, path, mode);
    return esp_apptrace_fopen(ESP_APPTRACE_DEST_TRAX, path, mode);
}

int gcov_rtio_fclose(void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fclose(ESP_APPTRACE_DEST_TRAX, stream);
}

size_t gcov_rtio_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s read %u", __FUNCTION__, size*nmemb);
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
