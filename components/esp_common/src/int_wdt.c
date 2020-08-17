// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "sdkconfig.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_freertos_hooks.h"
#include "soc/timer_periph.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "esp_int_wdt.h"
#include "esp_private/system_internal.h"
#include "hal/timer_types.h"
#include "hal/wdt_hal.h"

#if CONFIG_ESP_INT_WDT

#define WDT_INT_NUM ETS_T1_WDT_INUM

#define IWDT_INSTANCE           WDT_MWDT1
#define IWDT_PRESCALER          MWDT1_TICK_PRESCALER   //Tick period of 500us if WDT source clock is 80MHz
#define IWDT_TICKS_PER_US       MWDT1_TICKS_PER_US
#define IWDT_INITIAL_TIMEOUT_S  5
static wdt_hal_context_t iwdt_context;

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
/*
 * This parameter is indicates the response time of Interrupt watchdog to
 * identify the live lock.
 */
#define IWDT_LIVELOCK_TIMEOUT_MS    (20)

extern uint32_t _l4_intr_livelock_counter, _l4_intr_livelock_max;
#endif

//Take care: the tick hook can also be called before esp_int_wdt_init() is called.
#if CONFIG_ESP_INT_WDT_CHECK_CPU1
//Not static; the ISR assembly checks this.
bool int_wdt_app_cpu_ticked = false;

static void IRAM_ATTR tick_hook(void) {
    if (xPortGetCoreID()!=0) {
        int_wdt_app_cpu_ticked = true;
    } else {
        //Only feed wdt if app cpu also ticked.
        if (int_wdt_app_cpu_ticked) {
            //Todo: Check if there's a way to avoid reconfiguring the stages on each feed.
            wdt_hal_write_protect_disable(&iwdt_context);
            //Reconfigure stage timeouts
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
            _l4_intr_livelock_counter = 0;
            wdt_hal_config_stage(&iwdt_context, WDT_STAGE0,
                    CONFIG_ESP_INT_WDT_TIMEOUT_MS*1000/IWDT_TICKS_PER_US/(_l4_intr_livelock_max+1), WDT_STAGE_ACTION_INT);                            //Set timeout before interrupt
#else
            wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, CONFIG_ESP_INT_WDT_TIMEOUT_MS*1000/IWDT_TICKS_PER_US, WDT_STAGE_ACTION_INT);              //Set timeout before interrupt
#endif
            wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, 2*CONFIG_ESP_INT_WDT_TIMEOUT_MS*1000/IWDT_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM);   //Set timeout before reset
            wdt_hal_feed(&iwdt_context);
            wdt_hal_write_protect_enable(&iwdt_context);
            int_wdt_app_cpu_ticked = false;
        }
    }
}
#else
static void IRAM_ATTR tick_hook(void) {
#if CONFIG_IDF_TARGET_ESP32
    if (xPortGetCoreID()!=0) {
        return;
    }
#endif
    //Todo: Check if there's a way to avoid reconfiguring the stages on each feed.
    wdt_hal_write_protect_disable(&iwdt_context);
    //Reconfigure stage timeouts
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, CONFIG_ESP_INT_WDT_TIMEOUT_MS*1000/IWDT_TICKS_PER_US, WDT_STAGE_ACTION_INT);              //Set timeout before interrupt
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, 2*CONFIG_ESP_INT_WDT_TIMEOUT_MS*1000/IWDT_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM);   //Set timeout before reset
    wdt_hal_feed(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);
}
#endif


void esp_int_wdt_init(void) {
    periph_module_enable(PERIPH_TIMG1_MODULE);
    //The timer configs initially are set to 5 seconds, to make sure the CPU can start up. The tick hook sets
    //it to their actual value.
    wdt_hal_init(&iwdt_context, IWDT_INSTANCE, IWDT_PRESCALER, true);
    wdt_hal_write_protect_disable(&iwdt_context);
    //The timer configs initially are set to 5 seconds, to make sure the CPU can start up. The tick hook sets
    //it to their actual value.
    //1st stage timeout: interrupt
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, IWDT_INITIAL_TIMEOUT_S * 1000000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_INT);
    //2nd stage timeout: reset system
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, IWDT_INITIAL_TIMEOUT_S * 1000000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM);
    //Enable WDT
    wdt_hal_enable(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);
}

void esp_int_wdt_cpu_init(void)
{
    assert((CONFIG_ESP_INT_WDT_TIMEOUT_MS >= (portTICK_PERIOD_MS<<1)) && "Interrupt watchdog timeout needs to meet twice the RTOS tick period!");
    esp_register_freertos_tick_hook_for_cpu(tick_hook, xPortGetCoreID());
    ESP_INTR_DISABLE(WDT_INT_NUM);
    intr_matrix_set(xPortGetCoreID(), ETS_TG1_WDT_LEVEL_INTR_SOURCE, WDT_INT_NUM);
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    /*
     * This is a workaround for issue 3.15 in "ESP32 ECO and workarounds for
     * Bugs" document.
     */
    _l4_intr_livelock_counter = 0;
    if (soc_has_cache_lock_bug()) {
        assert((portTICK_PERIOD_MS<<1) <= IWDT_LIVELOCK_TIMEOUT_MS);
        assert(CONFIG_ESP_INT_WDT_TIMEOUT_MS >= (IWDT_LIVELOCK_TIMEOUT_MS*3));
        _l4_intr_livelock_max = CONFIG_ESP_INT_WDT_TIMEOUT_MS/IWDT_LIVELOCK_TIMEOUT_MS - 1;
    }
#endif
    //We do not register a handler for the interrupt because it is interrupt level 4 which
    //is not servicable from C. Instead, xtensa_vectors.S has a call to the panic handler for
    //this interrupt.
    ESP_INTR_ENABLE(WDT_INT_NUM);
}

#endif
