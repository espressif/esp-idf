// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>
#include "esp_err.h"
#include "esp_intr.h"
#include "esp_attr.h"
#include "esp_freertos_hooks.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_log.h"

#include "esp_task_wdt.h"

#if CONFIG_TASK_WDT

static const char* TAG = "task_wdt";

typedef struct wdt_task_t wdt_task_t;
struct wdt_task_t {
    TaskHandle_t task_handle;
    bool fed_watchdog;
    wdt_task_t *next;
};

static wdt_task_t *wdt_task_list=NULL;

static void IRAM_ATTR task_wdt_isr(void *arg) {
    wdt_task_t *wdttask;
    const char *cpu;
    //Feed the watchdog so we do not reset
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //Ack interrupt
    TIMERG0.int_clr_timers.wdt=1;
    //Watchdog got triggered because at least one task did not report in.
    ets_printf("Task watchdog got triggered. The following tasks did not feed the watchdog in time:\n");
    for (wdttask=wdt_task_list; wdttask!=NULL; wdttask=wdttask->next) {
        if (!wdttask->fed_watchdog) {
            cpu=xTaskGetAffinity(wdttask->task_handle)==0?"CPU 0":"CPU 1";
            if (xTaskGetAffinity(wdttask->task_handle)==tskNO_AFFINITY) cpu="CPU 0/1";
            printf(" - %s (%s)\n", pcTaskGetTaskName(wdttask->task_handle), cpu);
        }
    }
    ets_printf("Tasks currently running:\n");
    for (int x=0; x<portNUM_PROCESSORS; x++) {
        ets_printf("CPU %d: %s\n", x, pcTaskGetTaskName(xTaskGetCurrentTaskHandleForCPU(x)));
    }

#if CONFIG_TASK_WDT_PANIC
    ets_printf("Aborting.\n");
    abort();
#endif
}


void esp_task_wdt_feed() {
    wdt_task_t *wdttask=wdt_task_list;
    bool found_task=false, do_feed_wdt=true;
    TaskHandle_t handle=xTaskGetCurrentTaskHandle();
    //Walk the linked list of wdt tasks to find this one, as well as see if we need to feed
    //the real watchdog timer.
    for (wdttask=wdt_task_list; wdttask!=NULL; wdttask=wdttask->next) {
        //See if we are at the current task.
        if (wdttask->task_handle == handle) {
            wdttask->fed_watchdog=true;
            found_task=true;
        }
        //If even one task in the list doesn't have the do_feed_wdt var set, we do not feed the watchdog.
        if (!wdttask->fed_watchdog) do_feed_wdt=false;
    }
    
    if (!found_task) {
        //This is the first time the task calls the task_wdt_feed function. Create a new entry for it in
        //the linked list.
        wdt_task_t *newtask=malloc(sizeof(wdt_task_t));
        memset(newtask, 0, sizeof(wdt_task_t));
        newtask->task_handle=handle;
        newtask->fed_watchdog=true;
        if (wdt_task_list == NULL) {
            wdt_task_list=newtask;
        } else {
            for (wdttask=wdt_task_list; wdttask->next!=NULL; wdttask=wdttask->next) ;
            wdttask->next=newtask;
        }
    }
    if (do_feed_wdt) {
        //All tasks have checked in; time to feed the hw watchdog.
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
        //Reset fed_watchdog status
        for (wdttask=wdt_task_list; wdttask->next!=NULL; wdttask=wdttask->next) wdttask->fed_watchdog=false;
    }
}

void esp_task_wdt_delete() {
    TaskHandle_t handle=xTaskGetCurrentTaskHandle();
    wdt_task_t *wdttask=wdt_task_list;
    //Wdt task list can't be empty
    if (!wdt_task_list) {
        ESP_LOGE(TAG, "task_wdt_delete: No tasks in list?");
        return;
    }
    if (handle==wdt_task_list) {
        //Current task is first on list.
        wdt_task_list=wdt_task_list->next;
        free(wdttask);
    } else {
        //Find current task in list
        while (wdttask->next!=NULL && wdttask->next->task_handle!=handle) wdttask=wdttask->next;
        if (!wdttask->next) {
            ESP_LOGE(TAG, "task_wdt_delete: Task never called task_wdt_feed!");
            return;
        }
        wdt_task_t *freeme=wdttask->next;
        wdttask->next=wdttask->next->next;
        free(freeme);
    }
}


#if CONFIG_TASK_WDT_CHECK_IDLE_TASK
static bool idle_hook(void) {
#if !CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1
    if (xPortGetCoreID()!=0) return true;
#endif
    esp_task_wdt_feed();
    return true;
}
#endif


void esp_task_wdt_init() {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_config0.sys_reset_length=7;                 //3.2uS
    TIMERG0.wdt_config0.cpu_reset_length=7;                 //3.2uS
    TIMERG0.wdt_config0.level_int_en=1;
    TIMERG0.wdt_config0.stg0=TIMG_WDT_STG_SEL_INT;          //1st stage timeout: interrupt
    TIMERG0.wdt_config0.stg1=TIMG_WDT_STG_SEL_RESET_SYSTEM; //2nd stage timeout: reset system
    TIMERG0.wdt_config1.clk_prescale=80*500;                //Prescaler: wdt counts in ticks of 0.5mS
    TIMERG0.wdt_config2=CONFIG_TASK_WDT_TIMEOUT_S*2000;     //Set timeout before interrupt
    TIMERG0.wdt_config3=CONFIG_TASK_WDT_TIMEOUT_S*4000;     //Set timeout before reset
    TIMERG0.wdt_config0.en=1;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
#if CONFIG_TASK_WDT_CHECK_IDLE_TASK
    esp_register_freertos_idle_hook(idle_hook);
#endif
    ESP_INTR_DISABLE(ETS_T0_WDT_INUM);
    intr_matrix_set(xPortGetCoreID(), ETS_TG0_WDT_LEVEL_INTR_SOURCE, ETS_T0_WDT_INUM);
    xt_set_interrupt_handler(ETS_T0_WDT_INUM, task_wdt_isr, NULL);
    TIMERG0.int_clr_timers.wdt=1;
    TIMERG0.int_ena.wdt=1;
    ESP_INTR_ENABLE(ETS_T0_WDT_INUM);
}


#endif