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
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <esp_types.h>
#include "esp_err.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "esp_ipc.h"
#include "esp_attr.h"
#include "esp_freertos_hooks.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_log.h"
#include "driver/timer.h"

#include "esp_task_wdt.h"

//Assertion macro that exits from critical section and returns 'ret'
#define ASSERT_EXIT_CRIT_RET_ERR(cond, ret)  ({                             \
            if(!(cond)){                                                    \
                portEXIT_CRITICAL(&taskwdt_spinlock);                       \
                return ret;                                                 \
            }                                                               \
})

typedef struct wdt_task_t wdt_task_t;
struct wdt_task_t {
    TaskHandle_t task_handle;
    bool fed;
    wdt_task_t *next;
};

typedef struct task_wdt_config_t task_wdt_config_t;
struct task_wdt_config_t {
    wdt_task_t *list;
    uint32_t timeout;
    bool panic;
    bool idle_enable[portNUM_PROCESSORS];
    TaskHandle_t idle_handles[portNUM_PROCESSORS];
    intr_handle_t intr_handle;
};

static task_wdt_config_t *wdt_config = NULL;
static portMUX_TYPE taskwdt_spinlock = portMUX_INITIALIZER_UNLOCKED;

/*
 * Internal function that checks the list for target task. Returns the list item
 * if found and returns null if not found. Also checks if the all the other
 * tasks have checked in. Called within critical.
 */
static wdt_task_t *check_list(TaskHandle_t handle, bool *checked)
{
    wdt_task_t *target = NULL;
    *checked = true;
    for(wdt_task_t *task = wdt_config->list; task != NULL; task = task->next){
        if(task->task_handle == handle){
            target = task;   //Get pointer to target task list member
        }else{
            if(task->fed == false){     //If a task has yet to check in
                *checked = false;
            }
        }
    }
    return target;
}

/*
 * Resets the wdt and fed flags of each task on the list. Called within critical
 */
static void feed_wdt()
{
    //All tasks have checked in; time to feed the hw watchdog.
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //Reset fed flags on all tasks in list
    for (wdt_task_t *task = wdt_config->list; task != NULL; task = task->next){
        task->fed=false;
    }
}

/*
 * ISR for when wdt expires. Checks for which tasks have not fed. Trigger panic
 * if configured to do so in initialization
 */
static void task_wdt_isr(void *arg)
{
    portENTER_CRITICAL(&taskwdt_spinlock);
    wdt_task_t *wdttask;
    const char *cpu;
    //Feed the watchdog so we don't reset
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //Ack interrupt
    TIMERG0.int_clr_timers.wdt=1;
    //We are taking a spinlock while doing I/O (ets_printf) here. Normally, that is a pretty
    //bad thing, possibly (temporarily) hanging up the 2nd core and stopping FreeRTOS. In this case,
    //something bad already happened and reporting this is considered more important
    //than the badness caused by a spinlock here.
    if (!wdt_config->list) {       //No tasks on list
        portEXIT_CRITICAL(&taskwdt_spinlock);
        return;
    }
    //Watchdog got triggered because at least one task did not report in.
    ets_printf("Task watchdog got triggered. The following tasks did not feed the watchdog in time:\n");
    for (wdttask=wdt_config->list; wdttask!=NULL; wdttask=wdttask->next) {
        if (!wdttask->fed) {
            cpu=xTaskGetAffinity(wdttask->task_handle)==0?DRAM_STR("CPU 0"):DRAM_STR("CPU 1");
            if (xTaskGetAffinity(wdttask->task_handle)==tskNO_AFFINITY) cpu=DRAM_STR("CPU 0/1");
            ets_printf(" - %s (%s)\n", pcTaskGetTaskName(wdttask->task_handle), cpu);
        }
    }
    ets_printf(DRAM_STR("Tasks currently running:\n"));
    for (int x=0; x<portNUM_PROCESSORS; x++) {
        ets_printf("CPU %d: %s\n", x, pcTaskGetTaskName(xTaskGetCurrentTaskHandleForCPU(x)));
    }

    if (wdt_config->panic){
        ets_printf("Aborting.\n");
        portEXIT_CRITICAL(&taskwdt_spinlock);
        abort();
    }

    portEXIT_CRITICAL(&taskwdt_spinlock);
}

/*
 * Idle hook for Idle Task to feed the wdt. Will only call feed if the idle task
 * of that core has been added to wdt task list.
 */
static bool idle_hook(void)
{
    uint32_t core_id = xPortGetCoreID();
    if (wdt_config->idle_enable[core_id]){
        esp_task_wdt_feed();
    }
    return true;
}

/*
 * Initializes the task watchdog timer by allocating memory for the config data
 * structure, obtaining the idle task handles/registering idle hooks, and
 * setting the hardware timer registers. If reconfiguring, it will just modify
 * wdt_config and reset the hardware timer.
 */
esp_err_t esp_task_wdt_init(uint32_t timeout, bool panic)
{
    portENTER_CRITICAL(&taskwdt_spinlock);
    if(wdt_config == NULL){        //wdt not initialized yet
    	//Allocate memory for wdt_config
        wdt_config = calloc(1, sizeof(task_wdt_config_t));
        ASSERT_EXIT_CRIT_RET_ERR((wdt_config != NULL), ESP_ERR_NO_MEM);

        wdt_config->list = NULL;
        wdt_config->timeout = timeout;
        wdt_config->panic = panic;

        //Get idle task handles
        for(int i = 0; i <  portNUM_PROCESSORS; i++){
            wdt_config->idle_handles[i] = xTaskGetIdleTaskHandleForCPU( i );
            wdt_config->idle_enable[i] = false;
        }

        //Register Idle Hook and Interrupts
        esp_register_freertos_idle_hook(idle_hook);
        ESP_ERROR_CHECK(esp_intr_alloc(ETS_TG0_WDT_LEVEL_INTR_SOURCE, 0, task_wdt_isr, NULL, &wdt_config->intr_handle))

        //Configure hardware timer
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;               //Disable write protection
        TIMERG0.wdt_config0.sys_reset_length=7;                 //3.2uS
        TIMERG0.wdt_config0.cpu_reset_length=7;                 //3.2uS
        TIMERG0.wdt_config0.level_int_en=1;
        TIMERG0.wdt_config0.stg0=TIMG_WDT_STG_SEL_INT;          //1st stage timeout: interrupt
        TIMERG0.wdt_config0.stg1=TIMG_WDT_STG_SEL_RESET_SYSTEM; //2nd stage timeout: reset system
        TIMERG0.wdt_config1.clk_prescale=80*500;                //Prescaler: wdt counts in ticks of 0.5mS
        TIMERG0.wdt_config2=wdt_config->timeout*2;      //Set timeout before interrupt
        TIMERG0.wdt_config3=wdt_config->timeout*4;      //Set timeout before reset
        TIMERG0.wdt_config0.en=1;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;                         //Enable write protection

    }else{      //wdt_config previously initialized
        //Reconfigure task wdt
        wdt_config->panic = panic;
        wdt_config->timeout = timeout;

        //Reconfigure hardware timer
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;   //Disable write protection
        TIMERG0.wdt_config0.en=0;                   //Disable timer
        TIMERG0.wdt_config2=wdt_config->timeout*2;           //Set timeout before interrupt
        TIMERG0.wdt_config3=wdt_config->timeout*4;           //Set timeout before reset
        TIMERG0.wdt_config0.en=1;                   //Renable timer
        TIMERG0.wdt_feed=1;                         //Reset timer
        TIMERG0.wdt_wprotect=0;                     //Enable write protection
    }
    portEXIT_CRITICAL(&taskwdt_spinlock);
    return ESP_OK;
}

esp_err_t esp_task_wdt_deinit()
{
    portENTER_CRITICAL(&taskwdt_spinlock);
    //wdt must already be initialized
    ASSERT_EXIT_CRIT_RET_ERR((wdt_config != NULL), ESP_ERR_INVALID_STATE);
    //Task list must be empty
    ASSERT_EXIT_CRIT_RET_ERR((wdt_config->list == NULL), ESP_ERR_INVALID_STATE);

    //Disable hardware timer
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;   //Disable write protection
    TIMERG0.wdt_config0.en=0;                   //Disable timer
    TIMERG0.wdt_wprotect=0;                     //Enable write protection

    esp_deregister_freertos_idle_hook(idle_hook);   //deregister idle hook
    ESP_ERROR_CHECK(esp_intr_free(wdt_config->intr_handle))  //Unregister interrupt
    free(wdt_config);                      //Free wdt_config
    wdt_config = NULL;
    portEXIT_CRITICAL(&taskwdt_spinlock);
    return ESP_OK;
}

esp_err_t esp_task_wdt_add(TaskHandle_t handle)
{
    portENTER_CRITICAL(&taskwdt_spinlock);      //Nested critical in Legacy (called from feed)
#ifdef CONFIG_TASK_WDT_LEGACY_BEHAVIOR
    wdt_task_t *target_task;
#else
    //Return error if wdt has not been initialized
    ASSERT_EXIT_CRIT_RET_ERR((wdt_config != NULL), ESP_ERR_INVALID_STATE);

    wdt_task_t *target_task;
    bool all_fed;
    if (handle == NULL){    //Get handle of current task if none is provided
        handle = xTaskGetCurrentTaskHandle();
    }

    //Check if tasks exists in task list, and if all other tasks have checked in
    target_task = check_list(handle, &all_fed);
    //Return error if task has already been added
    ASSERT_EXIT_CRIT_RET_ERR((target_task == NULL), ESP_ERR_INVALID_ARG);
#endif      //CONFIG_TASK_WDT_LEGACY_BEHAVIOR
    //Add task to wdt list
    target_task = calloc(1,sizeof(wdt_task_t));
    //Return error if calloc failed
    ASSERT_EXIT_CRIT_RET_ERR((target_task != NULL), ESP_ERR_NO_MEM);

    target_task->task_handle = handle;
    target_task->fed = true;
    target_task->next = NULL;
    if (wdt_config->list == NULL) {    //Adding to empty list
        wdt_config->list = target_task;
    } else {    //Adding to tail of list
        wdt_task_t *task;
        for (task = wdt_config->list; task->next != NULL; task = task->next){
            ;   //point task to current tail of wdt task list
        }
        task->next = target_task;
    }

    //If idle task, set idle_enable flag
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        if(handle == wdt_config->idle_handles[i]){
            wdt_config->idle_enable[i] = true;
            break;
        }
    }

#ifndef CONFIG_TASK_WDT_LEGACY_BEHAVIOR
    if(all_fed){     //Reset if all other tasks in list have checked in
        feed_wdt();
    }
#endif
    portEXIT_CRITICAL(&taskwdt_spinlock);		//Nested critical if Legacy
    return ESP_OK;
}

esp_err_t esp_task_wdt_feed()
{
    portENTER_CRITICAL(&taskwdt_spinlock);
    //Return error if wdt has not been initialized
    ASSERT_EXIT_CRIT_RET_ERR((wdt_config != NULL), ESP_ERR_INVALID_STATE);

    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    wdt_task_t *target_task;
    bool all_fed;

    //Check if tasks exists in task list, and if all other tasks have checked in
    target_task = check_list(handle, &all_fed);

    if(target_task == NULL){
#ifdef CONFIG_TASK_WDT_LEGACY_BEHAVIOR
        //Add task to wdt task list if it doesn't exist, return error if failed to add
        ASSERT_EXIT_CRIT_RET_ERR((esp_task_wdt_add(handle) == ESP_OK), ESP_ERR_NO_MEM);
#else
        portEXIT_CRITICAL(&taskwdt_spinlock);
        return ESP_ERR_INVALID_STATE;			//Return error if task does not exist
#endif      //CONFIG_TASK_WDT_LEGACY_BEHAVIOR
    }else{
        target_task->fed = true;   //Feed the task
    }

    if(all_fed){     //Reset if all other tasks in list have checked in
        feed_wdt();
    }

    portEXIT_CRITICAL(&taskwdt_spinlock);
    return ESP_OK;
}

#ifdef CONFIG_TASK_WDT_LEGACY_BEHAVIOR
esp_err_t esp_task_wdt_delete()
{
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
#else
esp_err_t esp_task_wdt_delete(TaskHandle_t handle)
{
    if(handle == NULL){
        handle = xTaskGetCurrentTaskHandle();
    }
#endif      //CONFIG_TASK_WDT_LEGACY_BEHAVIOR
    portENTER_CRITICAL(&taskwdt_spinlock);
    //Return error if wdt has not been initialized
    ASSERT_EXIT_CRIT_RET_ERR((wdt_config != NULL), ESP_ERR_INVALID_STATE);
    wdt_task_t *target_task;
    bool all_fed;

    target_task = check_list(handle, &all_fed);

    //Task doesn't exist on list. Return error
    ASSERT_EXIT_CRIT_RET_ERR((target_task != NULL), ESP_ERR_INVALID_ARG);

    if(target_task == wdt_config->list){     //target_task is head of list. Delete
        wdt_config->list = target_task->next;
        free(target_task);
    }else{                                      //target_task not head of list. Delete
        wdt_task_t *prev;
        for (prev = wdt_config->list; prev->next != target_task; prev = prev->next){
            ;   //point prev to task preceding target_task
        }
        prev->next = target_task->next;
        free(target_task);
    }

    //If idle task, disable idle_enable flag
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        if(handle == wdt_config->idle_handles[i]){
            wdt_config->idle_enable[i] = false;
        }
    }

    if(all_fed){     //Reset timer if all remaining tasks have checked in
        feed_wdt();
    }

    portEXIT_CRITICAL(&taskwdt_spinlock);
    return ESP_OK;
}

