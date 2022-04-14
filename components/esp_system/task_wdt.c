/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <sys/queue.h>
#include <esp_types.h>
#include "esp_err.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_debug_helpers.h"
#include "esp_freertos_hooks.h"
#include "soc/timer_periph.h"
#include "esp_log.h"
#include "esp_private/periph_ctrl.h"
#include "esp_task_wdt.h"
#include "esp_private/system_internal.h"
#include "esp_private/crosscore_int.h"
#include "hal/timer_types.h"
#include "hal/wdt_hal.h"

// --------------------------------------------------- Definitions -----------------------------------------------------

// ----------------------- Macros --------------------------

// HAL related variables and constants
#define TWDT_INSTANCE           WDT_MWDT0
#define TWDT_TICKS_PER_US       MWDT0_TICKS_PER_US
#define TWDT_PRESCALER          MWDT0_TICK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz

// ---------------------- Typedefs -------------------------

// Structure used for each subscribed task
typedef struct twdt_entry twdt_entry_t;
struct twdt_entry {
    SLIST_ENTRY(twdt_entry) slist_entry;
    TaskHandle_t task_handle;
    bool has_reset;
};

// Structure used to hold run time configuration of the TWDT
typedef struct twdt_obj twdt_obj_t;
struct twdt_obj {
    wdt_hal_context_t hal;
    SLIST_HEAD(entry_list_head, twdt_entry) entries_slist;
    bool panic; // Flag to trigger panic when TWDT times out
    intr_handle_t intr_handle;
};

// ----------------------- Objects -------------------------

static const char *TAG = "task_wdt";
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
static twdt_obj_t *p_twdt_obj = NULL;

// ----------------------------------------------------- Private -------------------------------------------------------

/**
 * @brief Find an entry from its task handle, and checks if all other entries have been reset
 *
 * @param[in] handle Task handle
 * @param[out] all_reset Whether all entries have been reset
 * @return Entry, or NULL if not found
 */
static twdt_entry_t *find_entry_from_handle_and_check_all_reset(TaskHandle_t handle, bool *all_reset)
{
    twdt_entry_t *target = NULL;
    bool found_non_reset = false;

    twdt_entry_t *entry;
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        if (entry->task_handle == handle) {
            target = entry;
        } else if (entry->has_reset == false) {
            found_non_reset = true;
        }
    }

    *all_reset = !found_non_reset;
    return target;
}

/**
 * @brief Reset hardware timer and entry flags
 */
static void reset_hw_timer(void)
{
    // All tasks have reset; time to reset the hardware timer.
    wdt_hal_write_protect_disable(&p_twdt_obj->hal);
    wdt_hal_feed(&p_twdt_obj->hal);
    wdt_hal_write_protect_enable(&p_twdt_obj->hal);
    //C lear the has_reset flag in each entry
    twdt_entry_t *entry;
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        entry->has_reset = false;
    }
}

/**
 * @brief Idle hook callback
 *
 * Idle hook callback for Idle Tasks to reset the TWDT. This callback will only be registered to the Idle Hook of a
 * particular core when the corresponding Idle Task subscribes to the TWDT.
 *
 * @return Always returns true
 */
static bool idle_hook_cb(void)
{
    esp_task_wdt_reset();
    return true;
}

/**
 * @brief User ISR callback placeholder
 *
 * This function is called by task_wdt_isr function (ISR for when TWDT times out). It can be redefined in user code to
 * handle twdt events.
 *
 * @note It has the same limitations as the interrupt function. Do not use ESP_LOGI functions inside.
 */
void __attribute__((weak)) esp_task_wdt_isr_user_handler(void)
{

}

/**
 * @brief TWDT timeout ISR function
 *
 * Tee ISR checks which entries have not been reset, prints some debugging information, and triggers a panic if
 * configured to do so.
 *
 * @param arg ISR argument
 */
static void task_wdt_isr(void *arg)
{
    portENTER_CRITICAL_ISR(&spinlock);
    // Reset hardware timer so that 2nd stage timeout is not reached (will trigger system reset)
    wdt_hal_write_protect_disable(&p_twdt_obj->hal);
    wdt_hal_handle_intr(&p_twdt_obj->hal);  // Feeds WDT and clears acknowledges interrupt
    wdt_hal_write_protect_enable(&p_twdt_obj->hal);
    // If there are no entries, there's nothing to do.
    if (SLIST_EMPTY(&p_twdt_obj->entries_slist)) {
        portEXIT_CRITICAL_ISR(&spinlock);
        return;
    }
    // Find what entries triggered the TWDT timeout (i.e., which entries have not been reset)
    /*
    Note: We are currently in a critical section, thus under normal circumstances, logging should not be allowed.
    However, TWDT timeouts count as fatal errors, thus reporting the fatal error is considered more important than
    minimizing interrupt latency. Thus we allow logging in critical sections in this narrow case.
    */
    ESP_EARLY_LOGE(TAG, "Task watchdog got triggered. The following tasks did not reset the watchdog in time:");
    twdt_entry_t *entry;
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        if (!entry->has_reset) {
            BaseType_t task_affinity = xTaskGetAffinity(entry->task_handle);
            const char *cpu;
            if (task_affinity == 0) {
                cpu = DRAM_STR("CPU 0");
            } else if (task_affinity == 1) {
                cpu = DRAM_STR("CPU 1");
            } else {
                cpu = DRAM_STR("CPU 0/1");
            }
            ESP_EARLY_LOGE(TAG, " - %s (%s)", pcTaskGetName(entry->task_handle), cpu);
        }
    }
    ESP_EARLY_LOGE(TAG, "%s", DRAM_STR("Tasks currently running:"));
    for (int x = 0; x < portNUM_PROCESSORS; x++) {
        ESP_EARLY_LOGE(TAG, "CPU %d: %s", x, pcTaskGetName(xTaskGetCurrentTaskHandleForCPU(x)));
    }
    portEXIT_CRITICAL_ISR(&spinlock);

    // Run user ISR handler
    esp_task_wdt_isr_user_handler();
    // Trigger configured timeout behavior (e.g., panic or print backtrace)
    if (p_twdt_obj->panic) {
        ESP_EARLY_LOGE(TAG, "Aborting.");
        esp_reset_reason_set_hint(ESP_RST_TASK_WDT);
        abort();
    } else {    // Print
#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2 && !CONFIG_IDF_TARGET_ESP32C2 // TODO: ESP32-C3 IDF-2986
        int current_core = xPortGetCoreID();
        // Print backtrace of current core
        ESP_EARLY_LOGE(TAG, "Print CPU %d (current core) backtrace", current_core);
        esp_backtrace_print(100);
#if !CONFIG_FREERTOS_UNICORE
        // Print backtrace of other core
        ESP_EARLY_LOGE(TAG, "Print CPU %d backtrace", !current_core);
        esp_crosscore_int_send_print_backtrace(!current_core);
#endif
#endif
    }

}

// ----------------------------------------------------- Public --------------------------------------------------------

esp_err_t esp_task_wdt_init(uint32_t timeout, bool panic)
{
    esp_err_t ret;

    twdt_obj_t *obj = NULL;
    if (p_twdt_obj == NULL) {
        // Allocate and initialize TWDT driver object
        obj = calloc(1, sizeof(twdt_obj_t));
        ESP_GOTO_ON_FALSE((obj != NULL), ESP_ERR_NO_MEM, err, TAG, "insufficient memory");
        SLIST_INIT(&obj->entries_slist);
        obj->panic = panic;
        ESP_ERROR_CHECK(esp_intr_alloc(ETS_TG0_WDT_LEVEL_INTR_SOURCE, 0, task_wdt_isr, NULL, &obj->intr_handle));
        portENTER_CRITICAL(&spinlock);
        // Configure hardware timer
        periph_module_enable(PERIPH_TIMG0_MODULE);
        wdt_hal_init(&obj->hal, TWDT_INSTANCE, TWDT_PRESCALER, true);
        // Assign the driver object
        p_twdt_obj = obj;
        portEXIT_CRITICAL(&spinlock);
    }
    portENTER_CRITICAL(&spinlock);
    wdt_hal_write_protect_disable(&p_twdt_obj->hal);
    // Configure 1st stage timeout and behavior
    wdt_hal_config_stage(&p_twdt_obj->hal, WDT_STAGE0, timeout * (1000000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_INT);
    // Configure 2nd stage timeout and behavior
    wdt_hal_config_stage(&p_twdt_obj->hal, WDT_STAGE1, timeout * (2 * 1000000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_RESET_SYSTEM);
    // Enable the WDT
    wdt_hal_enable(&p_twdt_obj->hal);
    wdt_hal_write_protect_enable(&p_twdt_obj->hal);
    portEXIT_CRITICAL(&spinlock);
    ret = ESP_OK;
err:
    return ret;
}

esp_err_t esp_task_wdt_deinit(void)
{
    esp_err_t ret;

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, err, TAG, "task watchdog was never initialized");
    ESP_GOTO_ON_FALSE(SLIST_EMPTY(&p_twdt_obj->entries_slist), ESP_ERR_INVALID_STATE, err, TAG, "all tasks must be deleted");
    // Disable hardware timer and the interrupt
    wdt_hal_write_protect_disable(&p_twdt_obj->hal);
    wdt_hal_disable(&p_twdt_obj->hal);
    wdt_hal_write_protect_enable(&p_twdt_obj->hal);
    wdt_hal_deinit(&p_twdt_obj->hal);
    esp_intr_disable(p_twdt_obj->intr_handle);
    // Unassign driver object
    twdt_obj_t *obj = p_twdt_obj;
    p_twdt_obj = NULL;
    portEXIT_CRITICAL(&spinlock);

    // Free driver resources
    ESP_ERROR_CHECK(esp_intr_free(obj->intr_handle));   //Deregister interrupt
    free(obj);  //Free p_twdt_obj
    return ESP_OK;

err:
    portEXIT_CRITICAL(&spinlock);
    return ret;
}

esp_err_t esp_task_wdt_add(TaskHandle_t handle)
{
    esp_err_t ret;
    if (handle == NULL) {   //Get handle of current task if none is provided
        handle = xTaskGetCurrentTaskHandle();
    }

    // Allocate entry for task
    twdt_entry_t *entry = calloc(1, sizeof(twdt_entry_t));
    ESP_GOTO_ON_FALSE((entry != NULL), ESP_ERR_NO_MEM, alloc_err, TAG, "insufficient memory");
    entry->task_handle = handle;
    entry->has_reset = false;

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, state_err, TAG, "task watchdog was never initialized");
    // Check if the task is an entry, and if all entries have been reset
    bool all_reset;
    twdt_entry_t *found_entry = find_entry_from_handle_and_check_all_reset(handle, &all_reset);
    ESP_GOTO_ON_FALSE((found_entry == NULL), ESP_ERR_INVALID_ARG, state_err, TAG, "task is already subscribed");
    // Add task to entry list
    SLIST_INSERT_HEAD(&p_twdt_obj->entries_slist, entry, slist_entry);
    if (all_reset) {   //Reset hardware timer if all other tasks in list have reset in
        reset_hw_timer();
    }
    portEXIT_CRITICAL(&spinlock);       //Nested critical if Legacy

    // If the task was the idle task, register the idle hook callback to appropriate core
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        if (handle == xTaskGetIdleTaskHandleForCPU(i)) {
            ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_hook_cb, i));
            break;
        }
    }
    return ESP_OK;

state_err:
    portEXIT_CRITICAL(&spinlock);
    free(entry);
alloc_err:
    return ret;
}

esp_err_t esp_task_wdt_reset(void)
{
    esp_err_t ret;
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, err, TAG, "task watchdog was never initialized");
    // Find entry for task
    bool all_reset;
    twdt_entry_t *entry;
    entry = find_entry_from_handle_and_check_all_reset(handle, &all_reset);
    ESP_GOTO_ON_FALSE((entry != NULL), ESP_ERR_NOT_FOUND, err, TAG, "task not found");
    // Mark entry as reset and issue timer reset if all entries have been reset
    entry->has_reset = true;    //Reset the task if it's on the task list
    if (all_reset) {    //Reset if all other tasks in list have reset in
        reset_hw_timer();
    }
    ret = ESP_OK;
err:
    portEXIT_CRITICAL(&spinlock);

    return ret;
}

esp_err_t esp_task_wdt_delete(TaskHandle_t handle)
{
    esp_err_t ret;
    if (handle == NULL) {
        handle = xTaskGetCurrentTaskHandle();
    }

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, err, TAG, "task watchdog was never initialized");
    // Find entry for task
    bool all_reset;
    twdt_entry_t *entry;
    entry = find_entry_from_handle_and_check_all_reset(handle, &all_reset);
    ESP_GOTO_ON_FALSE((entry != NULL), ESP_ERR_NOT_FOUND, err, TAG, "task not found");
    // Remove entry
    SLIST_REMOVE(&p_twdt_obj->entries_slist, entry, twdt_entry, slist_entry);
    // Reset hardware timer if all remaining tasks have reset
    if (all_reset) {
        reset_hw_timer();
    }
    portEXIT_CRITICAL(&spinlock);

    // Free the entry
    free(entry);
    // If idle task, deregister idle hook callback form appropriate core
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        if (handle == xTaskGetIdleTaskHandleForCPU(i)) {
            esp_deregister_freertos_idle_hook_for_cpu(idle_hook_cb, i);
            break;
        }
    }
    return ESP_OK;

err:
    portEXIT_CRITICAL(&spinlock);
    return ret;
}

esp_err_t esp_task_wdt_status(TaskHandle_t handle)
{
    esp_err_t ret;
    if (handle == NULL) {
        handle = xTaskGetCurrentTaskHandle();
    }

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, err, TAG, "task watchdog was never initialized");
    // Find entry for task
    bool all_reset;
    twdt_entry_t *entry;
    entry = find_entry_from_handle_and_check_all_reset(handle, &all_reset);
    (void) all_reset;   // Unused
    ret = (entry != NULL) ? ESP_OK : ESP_ERR_NOT_FOUND;
err:
    portEXIT_CRITICAL(&spinlock);

    return ret;
}
