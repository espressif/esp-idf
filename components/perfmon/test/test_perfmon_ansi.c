// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "unity.h"
#include "esp_log.h"
#include <stdlib.h>

#include "perfmon.h"

// These includes required only for the tests
#include "xtensa-debug-module.h"
#include "eri.h"

static const char *TAG = "perfmon";


TEST_CASE("Perfomance counter dump", "[perfmon]")
{

    xtensa_perfmon_dump();
    xtensa_perfmon_stop();
    xtensa_perfmon_dump();
    xtensa_perfmon_init(0, 0, 0xffff, 0, 6);
    xtensa_perfmon_dump();
    xtensa_perfmon_reset(0);
    xtensa_perfmon_start();
    int pm_data[10];
    for (int i = 0 ; i < 10 ; i++) {
        if (i == 4) {
            xtensa_perfmon_reset(0);
            xtensa_perfmon_start();
        }
        if (i == 6) {
            xtensa_perfmon_stop();
        }
        if (i == 8) {
            xtensa_perfmon_start();
        }
        pm_data[i] = eri_read(ERI_PERFMON_PM0);
    }
    for (int i = 0 ; i < 10 ; i++) {
        ESP_LOGI(TAG, "pm_data[%i]= %08x", i, pm_data[i]);
    }
    if (pm_data[4] > pm_data[3]) {
        ESP_LOGE(TAG, "The functions xtensa_perfmon_reset and xtensa_perfmon_start are not working correct.");
        ESP_LOGW(TAG, "pm_data[3]= %i, must be > pm_data[4]= %i", pm_data[3], pm_data[4]);
        TEST_ESP_OK(ESP_FAIL);
    }
    if ( pm_data[6] !=  pm_data[7]) {
        ESP_LOGE(TAG, "The xtensa_perfmon_stop functions is not working correct.");
        ESP_LOGW(TAG, "pm_data[6]= %i, must be == pm_data[7]= %i", pm_data[6], pm_data[7]);
        TEST_ESP_OK(ESP_FAIL);
    }
    if ( pm_data[7] ==  pm_data[8]) {
        ESP_LOGE(TAG, "The xtensa_perfmon_start functions is not working correct.");
        ESP_LOGW(TAG, "pm_data[7]= %i, must be < pm_data[8]= %i", pm_data[7], pm_data[8]);
        TEST_ESP_OK(ESP_FAIL);
    }

    xtensa_perfmon_stop();
}

static void test_call(void* params)
{
    for (int i = 0 ; i < 1000 ; i++) {
        __asm__ __volatile__("   nop");
    }
}

static bool callback_called = false;
static int callback_call_count = 0;
static void test_callback(void *params, uint32_t select, uint32_t mask, uint32_t value)
{
    ESP_LOGI("test", "test_callback select = %i,  mask = %i, value = %i", select, mask, value);
    callback_called = true;
    callback_call_count++;
}

TEST_CASE("Performacnce test callback", "[perfmon]")
{
    ESP_LOGI(TAG, "Initialize performance structure");
    xtensa_perfmon_config_t pm_config = {};
    pm_config.counters_size = sizeof(xtensa_perfmon_select_mask_all) / sizeof(uint32_t) / 2;
    pm_config.select_mask = xtensa_perfmon_select_mask_all;
    pm_config.repeat_count = 200;
    pm_config.max_deviation = 1;
    pm_config.call_function = test_call;
    pm_config.callback = test_callback;
    pm_config.callback_params = stdout;
    pm_config.tracelevel = -1; // Trace all events
    callback_called = false;
    callback_call_count = 0;
    xtensa_perfmon_exec(&pm_config);

    ESP_LOGI(TAG, "Callback count = %i", callback_call_count);
    if (callback_call_count != pm_config.counters_size) {
        ESP_LOGE(TAG, "The callback count is not correct.");
        ESP_LOGW(TAG, "callback_call_count= %i, must be == pm_config.counters_size= %i", callback_call_count, pm_config.counters_size);
        TEST_ESP_OK(ESP_FAIL);
    }
    if (ESP_OK != xtensa_perfmon_overflow(0))
    {
        ESP_LOGE(TAG, "Perfmon 0 overflow detected!");
        TEST_ESP_OK(ESP_FAIL);
    }
    if (ESP_OK != xtensa_perfmon_overflow(1))
    {
        ESP_LOGE(TAG, "Perfmon 1 overflow detected!");
        TEST_ESP_OK(ESP_FAIL);
    }
    if (false == callback_called) {
        TEST_ESP_OK(ESP_FAIL);
    }
}

static void exec_callback(void *params)
{
    for (int i = 0 ; i < 100 ; i++) {
        __asm__ __volatile__("   nop");
    }
}

static const uint32_t test_dsp_table[] = {
    XTPERF_CNT_CYCLES, XTPERF_MASK_CYCLES, // total cycles
    XTPERF_CNT_INSN, XTPERF_MASK_INSN_ALL, // total instructions
    XTPERF_CNT_D_LOAD_U1, XTPERF_MASK_D_LOAD_LOCAL_MEM, // Mem read
    XTPERF_CNT_D_STORE_U1, XTPERF_MASK_D_STORE_LOCAL_MEM, // Mem write
    XTPERF_CNT_BUBBLES, XTPERF_MASK_BUBBLES_ALL &(~XTPERF_MASK_BUBBLES_R_HOLD_REG_DEP),  // wait for other reasons
    XTPERF_CNT_BUBBLES, XTPERF_MASK_BUBBLES_R_HOLD_REG_DEP,           // Wait for register dependency
    XTPERF_CNT_OVERFLOW, XTPERF_MASK_OVERFLOW,               // Last test cycle
};

TEST_CASE("Performance test for Empty callback", "[perfmon]")
{
    for (int i = 5 ; i < 10 ; i++) {
        exec_callback(NULL);
        ESP_LOGD(TAG, "Empty call passed.");
    }

    ESP_LOGI(TAG, "Start first test");
    xtensa_perfmon_config_t pm_config = {};
    pm_config.counters_size = sizeof(xtensa_perfmon_select_mask_all) / sizeof(uint32_t) / 2;
    pm_config.select_mask = xtensa_perfmon_select_mask_all;
    pm_config.repeat_count = 200;
    pm_config.max_deviation = 1;
    pm_config.call_function = exec_callback;
    pm_config.callback = xtensa_perfmon_view_cb;
    pm_config.callback_params = stdout;
    pm_config.tracelevel = -1;

    xtensa_perfmon_exec(&pm_config);
    callback_call_count = 0;
    ESP_LOGI(TAG, "Start second test");
    pm_config.counters_size = sizeof(test_dsp_table) / sizeof(uint32_t) / 2;
    pm_config.select_mask = test_dsp_table;
    pm_config.repeat_count = 200;
    pm_config.max_deviation = 1;
    pm_config.call_function = exec_callback;
    pm_config.callback = xtensa_perfmon_view_cb;
    pm_config.callback_params = stdout;
    pm_config.tracelevel = -1;

    xtensa_perfmon_exec(&pm_config);
    callback_call_count = 0;
    ESP_LOGI(TAG, "Start third test");
    pm_config.counters_size = sizeof(test_dsp_table) / sizeof(uint32_t) / 2;
    pm_config.select_mask = test_dsp_table;
    pm_config.repeat_count = 200;
    pm_config.max_deviation = 1;
    pm_config.call_function = exec_callback;
    pm_config.callback = test_callback;
    pm_config.callback_params = stdout;
    pm_config.tracelevel = -1;

    xtensa_perfmon_exec(&pm_config);
    if (callback_call_count != pm_config.counters_size) {
        TEST_ESP_OK(ESP_FAIL);
    }
    ESP_LOGI(TAG, "All tests passed.");
}

