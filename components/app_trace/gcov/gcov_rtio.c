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
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_app_trace.h"
#include "esp_dbg_stubs.h"

#if CONFIG_ESP32_GCOV_ENABLE

#define ESP_GCOV_DOWN_BUF_SIZE  4200

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_gcov_rtio";

static void (*s_gcov_exit)(void);

/* TODO: remove code extracted from GCC when IDF toolchain will be updated */
/*=============== GCC CODE START ====================*/
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
/*=============== GCC CODE END ====================*/

static int esp_dbg_stub_gcov_dump_do(void)
{
    int ret = ESP_OK;

    ESP_EARLY_LOGV(TAG, "Check for dump handler %p", s_gcov_exit);
    if (s_gcov_exit) {
        ESP_EARLY_LOGV(TAG, "Alloc apptrace down buf %d bytes", ESP_GCOV_DOWN_BUF_SIZE);
        void *down_buf = malloc(ESP_GCOV_DOWN_BUF_SIZE);
        if (down_buf == NULL) {
            ESP_EARLY_LOGE(TAG, "Failed to send files transfer stop cmd (%d)!", ret);
            return ESP_ERR_NO_MEM;
        }
        ESP_EARLY_LOGV(TAG, "Config apptrace down buf");
        esp_apptrace_down_buffer_config(down_buf, ESP_GCOV_DOWN_BUF_SIZE);
        ESP_EARLY_LOGV(TAG, "Dump data... %p", s_gcov_exit);
        s_gcov_exit();
        ESP_EARLY_LOGV(TAG, "Free apptrace down buf");
        free(down_buf);
    }
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
    int ret = ESP_OK;

    // disable IRQs on this CPU, other CPU is halted by OpenOCD
    unsigned irq_state = portENTER_CRITICAL_NESTED();
    ret = esp_dbg_stub_gcov_dump_do();
    // reset dump status to allow incremental data accumulation
    esp_gcov_reset_status();
    portEXIT_CRITICAL_NESTED(irq_state);
    return ret;
}

void esp_gcov_dump()
{
#if CONFIG_FREERTOS_UNICORE == 0
    int other_core = xPortGetCoreID() ? 0 : 1;
    esp_cpu_stall(other_core);
#endif

    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX)) {
        // to avoid complains that task watchdog got triggered for other tasks
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
        // to avoid reboot on INT_WDT
        TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG1.wdt_feed=1;
        TIMERG1.wdt_wprotect=0;
    }

    esp_dbg_stub_gcov_dump_do();
    // reset dump status to allow incremental data accumulation
    esp_gcov_reset_status();
#if CONFIG_FREERTOS_UNICORE == 0
    esp_cpu_unstall(other_core);
#endif
}

int gcov_rtio_atexit(void (*function)(void))
{
    ESP_EARLY_LOGV(TAG, "%s %p", __FUNCTION__, function);
    s_gcov_exit = function;
    esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_GCOV, (uint32_t)&esp_dbg_stub_gcov_entry);
    return 0;
}

void *gcov_rtio_fopen(const char *path, const char *mode)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fopen(ESP_APPTRACE_DEST_TRAX, path, mode);
}

int gcov_rtio_fclose(void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fclose(ESP_APPTRACE_DEST_TRAX, stream);
}

size_t gcov_rtio_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fread(ESP_APPTRACE_DEST_TRAX, ptr, size, nmemb, stream);
}

size_t gcov_rtio_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return esp_apptrace_fwrite(ESP_APPTRACE_DEST_TRAX, ptr, size, nmemb, stream);
}

int gcov_rtio_fseek(void *stream, long offset, int whence)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    int ret = esp_apptrace_fseek(ESP_APPTRACE_DEST_TRAX, stream, offset, whence);
    ESP_EARLY_LOGV(TAG, "%s EXIT", __FUNCTION__);
    return ret;
}

long gcov_rtio_ftell(void *stream)
{
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    long ret = esp_apptrace_ftell(ESP_APPTRACE_DEST_TRAX, stream);
    ESP_EARLY_LOGV(TAG, "%s", __FUNCTION__);
    return ret;
}
#endif
