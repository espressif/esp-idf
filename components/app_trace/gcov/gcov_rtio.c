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
#include "esp_private/dbg_stubs.h"
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

extern void __gcov_dump(void);
extern void __gcov_reset(void);

static struct syscall_stub_table s_gcov_stub_table;


static int gcov_stub_lock_try_acquire_recursive(_lock_t *lock)
{
    if (*lock && uxSemaphoreGetCount((xSemaphoreHandle)(*lock)) == 0) {
        // we can do nothing here, gcov dump is initiated with some resource locked
        // which is also used by gcov functions
        ESP_EARLY_LOGE(TAG, "Lock 0x%x is busy during GCOV dump! System state can be inconsistent after dump!", lock);
    }
    return pdTRUE;
}

static void gcov_stub_lock_acquire_recursive(_lock_t *lock)
{
    gcov_stub_lock_try_acquire_recursive(lock);
}

static void gcov_stub_lock_release_recursive(_lock_t *lock)
{
}

static int esp_dbg_stub_gcov_dump_do(void)
{
    int ret = ESP_OK;
    FILE* old_stderr = stderr;
    FILE* old_stdout = stdout;
    static struct syscall_stub_table *old_tables[portNUM_PROCESSORS];

    old_tables[0] = syscall_table_ptr_pro;
#if portNUM_PROCESSORS > 1
    old_tables[1] = syscall_table_ptr_app;
#endif
    ESP_EARLY_LOGV(TAG, "Alloc apptrace down buf %d bytes", ESP_GCOV_DOWN_BUF_SIZE);
    void *down_buf = malloc(ESP_GCOV_DOWN_BUF_SIZE);
    if (down_buf == NULL) {
        ESP_EARLY_LOGE(TAG, "Could not allocate memory for the buffer");
        return ESP_ERR_NO_MEM;
    }
    ESP_EARLY_LOGV(TAG, "Config apptrace down buf");
    esp_apptrace_down_buffer_config(down_buf, ESP_GCOV_DOWN_BUF_SIZE);
    ESP_EARLY_LOGV(TAG, "Dump data...");
    // incase of dual-core chip APP and PRO CPUs share the same table, so it is safe to save only PRO's table
    memcpy(&s_gcov_stub_table, syscall_table_ptr_pro, sizeof(s_gcov_stub_table));
    s_gcov_stub_table._lock_acquire_recursive = &gcov_stub_lock_acquire_recursive;
    s_gcov_stub_table._lock_release_recursive = &gcov_stub_lock_release_recursive;
    s_gcov_stub_table._lock_try_acquire_recursive = &gcov_stub_lock_try_acquire_recursive,
    syscall_table_ptr_pro = &s_gcov_stub_table;
#if portNUM_PROCESSORS > 1
    syscall_table_ptr_app = &s_gcov_stub_table;
#endif
    stderr = (FILE*) &__sf_fake_stderr;
    stdout = (FILE*) &__sf_fake_stdout;
    __gcov_dump();
    // reset dump status to allow incremental data accumulation
    __gcov_reset();
    stdout = old_stdout;
    stderr = old_stderr;
    syscall_table_ptr_pro = old_tables[0];
#if portNUM_PROCESSORS > 1
    syscall_table_ptr_app = old_tables[1];
#endif
    ESP_EARLY_LOGV(TAG, "Free apptrace down buf");
    free(down_buf);
    ESP_EARLY_LOGV(TAG, "Finish file transfer session");
    ret = esp_apptrace_fstop(ESP_APPTRACE_DEST_TRAX);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send files transfer stop cmd (%d)!", ret);
    }
    ESP_EARLY_LOGV(TAG, "exit %d", ret);
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
    return esp_dbg_stub_gcov_dump_do();
}

int gcov_rtio_atexit(void (*function)(void) __attribute__ ((unused)))
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_GCOV, (uint32_t)&esp_dbg_stub_gcov_entry);
    return 0;
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
        wdt_hal_context_t twdt = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
        wdt_hal_context_t iwdt = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
        //Feed the Task Watchdog (TG0) to prevent it from timing out
        wdt_hal_write_protect_disable(&twdt);
        wdt_hal_feed(&twdt);
        wdt_hal_write_protect_enable(&twdt);
        //Likewise, feed the Interrupt Watchdog (TG1) to prevent a reboot
        wdt_hal_write_protect_disable(&iwdt);
        wdt_hal_feed(&iwdt);
        wdt_hal_write_protect_enable(&iwdt);
    }

    esp_dbg_stub_gcov_dump_do();
#if !CONFIG_FREERTOS_UNICORE
    esp_cpu_unstall(other_core);
#endif
    portEXIT_CRITICAL_NESTED(irq_state);
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
