// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_attr.h"
#include "esp_freertos_hooks.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_log.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "esp_task_wdt.h"

//Assertion macro where, if 'cond' is false, will exit the critical section and return 'ret'
#define ASSERT_EXIT_CRIT_RETURN(cond, ret)  ({                              \
            if(!(cond)){                                                    \
                portEXIT_CRITICAL(&twdt_spinlock);                          \
                return ret;                                                 \
            }                                                               \
})

//Empty define used in ASSERT_EXIT_CRIT_RETURN macro when returning in void
#define VOID_RETURN

//Structure used for each subscribed task
typedef struct twdt_task_t twdt_task_t;
struct twdt_task_t {
    TaskHandle_t task_handle;
    bool has_reset;
    twdt_task_t *next;
};

//Structure used to hold run time configuration of the TWDT
typedef struct twdt_config_t twdt_config_t;
struct twdt_config_t {
    twdt_task_t *list;      //Linked list of subscribed tasks
    uint32_t timeout;       //Timeout period of TWDT
    bool panic;             //Flag to trigger panic when TWDT times out
    intr_handle_t intr_handle;
};

static twdt_config_t *twdt_config = NULL;
static portMUX_TYPE twdt_spinlock = portMUX_INITIALIZER_UNLOCKED;

/*
 * Idle hook callback for Idle Tasks to reset the TWDT. This callback will only
 * be registered to the Idle Hook of a particular core when the corresponding
 * Idle Task subscribes to the TWDT.
 */
static bool idle_hook_cb(void)
{
    esp_task_wdt_reset();
    return true;
}

/*
 * Internal function that looks for the target task in the TWDT task list.
 * Returns the list item if found and returns null if not found. Also checks if
 * all the other tasks have reset. Should be called within critical.
 */
static twdt_task_t *find_task_in_twdt_list(TaskHandle_t handle, bool *all_reset)
{
    twdt_task_t *target = NULL;
    *all_reset = true;
    for(twdt_task_t *task = twdt_config->list; task != NULL; task = task->next){
        if(task->task_handle == handle){
            target = task;   //Get pointer to target task list member
        }else{
            if(task->has_reset == false){     //If a task has yet to reset
                *all_reset = false;
            }
        }
    }
    return target;
}

/*
 * Resets the hardware timer and has_reset flags of each task on the list.
 * Called within critical
 */
static void reset_hw_timer()
{
    //All tasks have reset; time to reset the hardware timer.
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //Clear all has_reset flags in list
    for (twdt_task_t *task = twdt_config->list; task != NULL; task = task->next){
        task->has_reset=false;
    }
}

/*
 * ISR for when TWDT times out. Checks for which tasks have not reset. Also
 * triggers panic if configured to do so
 */
static void task_wdt_isr(void *arg)
{
    portENTER_CRITICAL(&twdt_spinlock);
    twdt_task_t *twdttask;
    const char *cpu;
    //Reset hardware timer so that 2nd stage timeout is not reached (will trigger system reset)
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //Acknowledge interrupt
    TIMERG0.int_clr_timers.wdt=1;
    //We are taking a spinlock while doing I/O (ets_printf) here. Normally, that is a pretty
    //bad thing, possibly (temporarily) hanging up the 2nd core and stopping FreeRTOS. In this case,
    //something bad already happened and reporting this is considered more important
    //than the badness caused by a spinlock here.

    //Return immediately if no tasks have been added to task list
    ASSERT_EXIT_CRIT_RETURN((twdt_config->list != NULL), VOID_RETURN);

    //Watchdog got triggered because at least one task did not reset in time.
    ets_printf("Task watchdog got triggered. The following tasks did not reset the watchdog in time:\n");
    for (twdttask=twdt_config->list; twdttask!=NULL; twdttask=twdttask->next) {
        if (!twdttask->has_reset) {
            cpu=xTaskGetAffinity(twdttask->task_handle)==0?DRAM_STR("CPU 0"):DRAM_STR("CPU 1");
            if (xTaskGetAffinity(twdttask->task_handle)==tskNO_AFFINITY) cpu=DRAM_STR("CPU 0/1");
            ets_printf(" - %s (%s)\n", pcTaskGetTaskName(twdttask->task_handle), cpu);
        }
    }
    ets_printf(DRAM_STR("Tasks currently running:\n"));
    for (int x=0; x<portNUM_PROCESSORS; x++) {
        ets_printf("CPU %d: %s\n", x, pcTaskGetTaskName(xTaskGetCurrentTaskHandleForCPU(x)));
    }

    if (twdt_config->panic){     //Trigger Panic if configured to do so
        ets_printf("Aborting.\n");
        portEXIT_CRITICAL(&twdt_spinlock);
        abort();
    }

    portEXIT_CRITICAL(&twdt_spinlock);
}

/*
 * Initializes the TWDT by allocating memory for the config data
 * structure, obtaining the idle task handles/registering idle hooks, and
 * setting the hardware timer registers. If reconfiguring, it will just modify
 * wdt_config and reset the hardware timer.
 */
esp_err_t esp_task_wdt_init(uint32_t timeout, bool panic)
{
    portENTER_CRITICAL(&twdt_spinlock);
    if(twdt_config == NULL){        //TWDT not initialized yet
        //Allocate memory for wdt_config
        twdt_config = calloc(1, sizeof(twdt_config_t));
        ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), ESP_ERR_NO_MEM);

        twdt_config->list = NULL;
        twdt_config->timeout = timeout;
        twdt_config->panic = panic;

        //Register Interrupt and ISR
        ESP_ERROR_CHECK(esp_intr_alloc(ETS_TG0_WDT_LEVEL_INTR_SOURCE, 0, task_wdt_isr, NULL, &twdt_config->intr_handle))

        //Configure hardware timer
        periph_module_enable(PERIPH_TIMG0_MODULE);
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;               //Disable write protection
        TIMERG0.wdt_config0.sys_reset_length=7;                 //3.2uS
        TIMERG0.wdt_config0.cpu_reset_length=7;                 //3.2uS
        TIMERG0.wdt_config0.level_int_en=1;
        TIMERG0.wdt_config0.stg0=TIMG_WDT_STG_SEL_INT;          //1st stage timeout: interrupt
        TIMERG0.wdt_config0.stg1=TIMG_WDT_STG_SEL_RESET_SYSTEM; //2nd stage timeout: reset system
        TIMERG0.wdt_config1.clk_prescale=80*500;                //Prescaler: wdt counts in ticks of 0.5mS
        TIMERG0.wdt_config2=twdt_config->timeout*2000;      //Set timeout before interrupt
        TIMERG0.wdt_config3=twdt_config->timeout*4000;      //Set timeout before reset
        TIMERG0.wdt_config0.en=1;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;                         //Enable write protection

    }else{      //twdt_config previously initialized
        //Reconfigure task wdt
        twdt_config->panic = panic;
        twdt_config->timeout = timeout;

        //Reconfigure hardware timer
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;   //Disable write protection
        TIMERG0.wdt_config0.en=0;                   //Disable timer
        TIMERG0.wdt_config2=twdt_config->timeout*2000;           //Set timeout before interrupt
        TIMERG0.wdt_config3=twdt_config->timeout*4000;           //Set timeout before reset
        TIMERG0.wdt_config0.en=1;                   //Renable timer
        TIMERG0.wdt_feed=1;                         //Reset timer
        TIMERG0.wdt_wprotect=0;                     //Enable write protection
    }
    portEXIT_CRITICAL(&twdt_spinlock);
    return ESP_OK;
}

esp_err_t esp_task_wdt_deinit()
{
    portENTER_CRITICAL(&twdt_spinlock);
    //TWDT must already be initialized
    ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), ESP_ERR_NOT_FOUND);
    //Task list must be empty
    ASSERT_EXIT_CRIT_RETURN((twdt_config->list == NULL), ESP_ERR_INVALID_STATE);

    //Disable hardware timer
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;   //Disable write protection
    TIMERG0.wdt_config0.en=0;                   //Disable timer
    TIMERG0.wdt_wprotect=0;                     //Enable write protection

    ESP_ERROR_CHECK(esp_intr_free(twdt_config->intr_handle))  //Unregister interrupt
    free(twdt_config);                      //Free twdt_config
    twdt_config = NULL;
    portEXIT_CRITICAL(&twdt_spinlock);
    return ESP_OK;
}

esp_err_t esp_task_wdt_add(TaskHandle_t handle)
{
    portENTER_CRITICAL(&twdt_spinlock);
    //TWDT must already be initialized
    ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), ESP_ERR_INVALID_STATE);

    twdt_task_t *target_task;
    bool all_reset;
    if (handle == NULL){    //Get handle of current task if none is provided
        handle = xTaskGetCurrentTaskHandle();
    }
    //Check if tasks exists in task list, and if all other tasks have reset
    target_task = find_task_in_twdt_list(handle, &all_reset);
    //task cannot be already subscribed
    ASSERT_EXIT_CRIT_RETURN((target_task == NULL), ESP_ERR_INVALID_ARG);

    //Add target task to TWDT task list
    target_task = calloc(1,sizeof(twdt_task_t));
    ASSERT_EXIT_CRIT_RETURN((target_task != NULL), ESP_ERR_NO_MEM);
    target_task->task_handle = handle;
    target_task->has_reset = true;
    target_task->next = NULL;
    if (twdt_config->list == NULL) {    //Adding to empty list
        twdt_config->list = target_task;
    } else {    //Adding to tail of list
        twdt_task_t *task;
        for (task = twdt_config->list; task->next != NULL; task = task->next){
            ;   //point task to current tail of TWDT task list
        }
        task->next = target_task;
    }

    //If idle task, register the idle hook callback to appropriate core
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        if(handle == xTaskGetIdleTaskHandleForCPU(i)){
            ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_hook_cb, i))
            break;
        }
    }

    if(all_reset){     //Reset hardware timer if all other tasks in list have reset in
        reset_hw_timer();
    }

    portEXIT_CRITICAL(&twdt_spinlock);       //Nested critical if Legacy
    return ESP_OK;
}

esp_err_t esp_task_wdt_reset()
{
    portENTER_CRITICAL(&twdt_spinlock);
    //TWDT must already be initialized
    ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), ESP_ERR_INVALID_STATE);

    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    twdt_task_t *target_task;
    bool all_reset;

    //Check if task exists in task list, and if all other tasks have reset
    target_task = find_task_in_twdt_list(handle, &all_reset);
    //Return error if trying to reset task that is not on the task list
    ASSERT_EXIT_CRIT_RETURN((target_task != NULL), ESP_ERR_NOT_FOUND);

    target_task->has_reset = true;    //Reset the task if it's on the task list
    if(all_reset){     //Reset if all other tasks in list have reset in
        reset_hw_timer();
    }

    portEXIT_CRITICAL(&twdt_spinlock);
    return ESP_OK;
}

esp_err_t esp_task_wdt_delete(TaskHandle_t handle)
{
    if(handle == NULL){
        handle = xTaskGetCurrentTaskHandle();
    }
    portENTER_CRITICAL(&twdt_spinlock);
    //Return error if twdt has not been initialized
    ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), ESP_ERR_NOT_FOUND);

    twdt_task_t *target_task;
    bool all_reset;
    target_task = find_task_in_twdt_list(handle, &all_reset);
    //Task doesn't exist on list. Return error
    ASSERT_EXIT_CRIT_RETURN((target_task != NULL), ESP_ERR_INVALID_ARG);

    if(target_task == twdt_config->list){     //target_task is head of list. Delete
        twdt_config->list = target_task->next;
        free(target_task);
    }else{                                      //target_task not head of list. Delete
        twdt_task_t *prev;
        for (prev = twdt_config->list; prev->next != target_task; prev = prev->next){
            ;   //point prev to task preceding target_task
        }
        prev->next = target_task->next;
        free(target_task);
    }

    //If idle task, deregister idle hook callback form appropriate core
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        if(handle == xTaskGetIdleTaskHandleForCPU(i)){
            esp_deregister_freertos_idle_hook_for_cpu(idle_hook_cb, i);
            break;
        }
    }

    if(all_reset){     //Reset hardware timer if all remaining tasks have reset
        reset_hw_timer();
    }

    portEXIT_CRITICAL(&twdt_spinlock);
    return ESP_OK;
}

esp_err_t esp_task_wdt_status(TaskHandle_t handle)
{
    if(handle == NULL){
        handle = xTaskGetCurrentTaskHandle();
    }

    portENTER_CRITICAL(&twdt_spinlock);
    //Return if TWDT is not initialized
    ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), ESP_ERR_INVALID_STATE);

    twdt_task_t *task;
    for(task = twdt_config->list; task!=NULL; task=task->next){
        //Return ESP_OK if task is found
        ASSERT_EXIT_CRIT_RETURN((task->task_handle != handle), ESP_OK);
    }

    //Task could not be found
    portEXIT_CRITICAL(&twdt_spinlock);
    return ESP_ERR_NOT_FOUND;
}

void esp_task_wdt_feed()
{
    portENTER_CRITICAL(&twdt_spinlock);
    //Return immediately if TWDT has not been initialized
    ASSERT_EXIT_CRIT_RETURN((twdt_config != NULL), VOID_RETURN);

    //Check if task is on list
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    bool all_reset;
    twdt_task_t *target_task = find_task_in_twdt_list(handle, &all_reset);

    //reset the task if it's on the list, then return
    if(target_task != NULL){
        target_task->has_reset = true;
        if(all_reset){
            reset_hw_timer();       //Reset hardware timer if all other tasks have reset
        }
        portEXIT_CRITICAL(&twdt_spinlock);
        return;
    }

    //Add task if it's has not on list
    target_task = calloc(1, sizeof(twdt_task_t));
    ASSERT_EXIT_CRIT_RETURN((target_task != NULL), VOID_RETURN);   //If calloc failed
    target_task->task_handle = handle;
    target_task->has_reset = true;
    target_task->next = NULL;

    if (twdt_config->list == NULL) {    //Adding to empty list
        twdt_config->list = target_task;
    } else {    //Adding to tail of list
        twdt_task_t *task;
        for (task = twdt_config->list; task->next != NULL; task = task->next){
            ;   //point task to current tail of wdt task list
        }
        task->next = target_task;
    }

    portEXIT_CRITICAL(&twdt_spinlock);
}


