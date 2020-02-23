/* perfmon (performance monitor) example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"

#include "perfmon.h"

static const char* TAG = "example";

static void exec_test_function(void *params)
{
    for (int i = 0 ; i < 100 ; i++) {
        __asm__ __volatile__("   nop");
    }
}

// Table with dedicated performance counters
static uint32_t pm_check_table[] = {
    XTPERF_CNT_CYCLES, XTPERF_MASK_CYCLES, // total cycles
    XTPERF_CNT_INSN, XTPERF_MASK_INSN_ALL, // total instructions
    XTPERF_CNT_D_LOAD_U1, XTPERF_MASK_D_LOAD_LOCAL_MEM, // Mem read
    XTPERF_CNT_D_STORE_U1, XTPERF_MASK_D_STORE_LOCAL_MEM, // Mem write
    XTPERF_CNT_BUBBLES, XTPERF_MASK_BUBBLES_ALL &(~XTPERF_MASK_BUBBLES_R_HOLD_REG_DEP),  // wait for other reasons
    XTPERF_CNT_BUBBLES, XTPERF_MASK_BUBBLES_R_HOLD_REG_DEP,           // Wait for register dependency
    XTPERF_CNT_OVERFLOW, XTPERF_MASK_OVERFLOW,               // Last test cycle
};

#define TOTAL_CALL_AMOUNT 200
#define PERFMON_TRACELEVEL -1 // -1 - will ignore trace level

void app_main(void)
{
    ESP_LOGI(TAG, "Start");
    ESP_LOGI(TAG, "Start test with printing all available statistic");
    xtensa_perfmon_config_t pm_config = {};
    pm_config.counters_size = sizeof(xtensa_perfmon_select_mask_all) / sizeof(uint32_t) / 2;
    pm_config.select_mask = xtensa_perfmon_select_mask_all;
    pm_config.repeat_count = TOTAL_CALL_AMOUNT;
    pm_config.max_deviation = 1;
    pm_config.call_function = exec_test_function;
    pm_config.callback = xtensa_perfmon_view_cb;
    pm_config.callback_params = stdout;
    pm_config.tracelevel = PERFMON_TRACELEVEL;
    xtensa_perfmon_exec(&pm_config);

    ESP_LOGI(TAG, "Start test with user defined statistic");
    pm_config.counters_size = sizeof(pm_check_table) / sizeof(uint32_t) / 2;
    pm_config.select_mask = pm_check_table;
    pm_config.repeat_count = TOTAL_CALL_AMOUNT;
    pm_config.max_deviation = 1;
    pm_config.call_function = exec_test_function;
    pm_config.callback = xtensa_perfmon_view_cb;
    pm_config.callback_params = stdout;
    pm_config.tracelevel = PERFMON_TRACELEVEL;

    xtensa_perfmon_exec(&pm_config);

    ESP_LOGI(TAG, "The End");
}
