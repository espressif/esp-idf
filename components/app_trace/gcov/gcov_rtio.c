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

#if CONFIG_ESP32_APPTRACE_ENABLE

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_gcov_rtio";

static void (*s_gcov_exit)(void);
static uint8_t s_gcov_down_buf[256];

void esp_gcov_dump()
{
#if CONFIG_FREERTOS_UNICORE == 0
    esp_cpu_stall(!xPortGetCoreID());
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

    if (s_gcov_exit) {
        esp_apptrace_down_buffer_config(s_gcov_down_buf, sizeof(s_gcov_down_buf));
        s_gcov_exit();
    }

    int ret = esp_apptrace_fstop(ESP_APPTRACE_DEST_TRAX);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send files transfer stop cmd (%d)!\n", ret);
    }
}

int gcov_rtio_atexit(void (*function)(void))
{
    s_gcov_exit = function;
    return 0;
}

void *gcov_rtio_fopen(const char *path, const char *mode)
{
    return esp_apptrace_fopen(ESP_APPTRACE_DEST_TRAX, path, mode);
}

int gcov_rtio_fclose(void *stream)
{
    return esp_apptrace_fclose(ESP_APPTRACE_DEST_TRAX, stream);
}

size_t gcov_rtio_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    return esp_apptrace_fwrite(ESP_APPTRACE_DEST_TRAX, ptr, size, nmemb, stream);
}

int gcov_rtio_fseek(void *stream, long offset, int whence)
{
    return esp_apptrace_fseek(ESP_APPTRACE_DEST_TRAX, stream, offset, whence);
}

long gcov_rtio_ftell(void *stream)
{
    return esp_apptrace_ftell(ESP_APPTRACE_DEST_TRAX, stream);
}

#endif
