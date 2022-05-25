/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/wdt_hal.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_debug_helpers.h"
#include "esp_freertos_hooks.h"
#include "esp_task_wdt.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/system_internal.h"
#include "esp_private/crosscore_int.h"

// --------------------------------------------------- Definitions -----------------------------------------------------

// ----------------------- Macros --------------------------

// HAL related variables and constants
#define TWDT_INSTANCE           WDT_MWDT0
#define TWDT_TICKS_PER_US       MWDT0_TICKS_PER_US
#define TWDT_PRESCALER          MWDT0_TICK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz

// ---------------------- Typedefs -------------------------

/**
 * @brief Structure used for each subscribed task
 */
typedef struct twdt_entry twdt_entry_t;
struct twdt_entry {
    SLIST_ENTRY(twdt_entry) slist_entry;
    TaskHandle_t task_handle;   // NULL if user entry
    const char *user_name;      // NULL if task entry
    bool has_reset;
};

// Structure used to hold run time configuration of the TWDT
typedef struct twdt_obj twdt_obj_t;
struct twdt_obj {
    wdt_hal_context_t hal;
    SLIST_HEAD(entry_list_head, twdt_entry) entries_slist;
    uint32_t idle_core_mask;    // Current core's who's idle tasks are subscribed
    bool panic; // Flag to trigger panic when TWDT times out
    intr_handle_t intr_handle;
};

// ----------------------- Objects -------------------------

static const char *TAG = "task_wdt";
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
static twdt_obj_t *p_twdt_obj = NULL;

#if CONFIG_FREERTOS_SMP
#define CORE_USER_NAME_LEN      8   // Long enough for "CPU XXX"
static esp_task_wdt_user_handle_t core_user_handles[portNUM_PROCESSORS] = {NULL};
static char core_user_names[portNUM_PROCESSORS][CORE_USER_NAME_LEN];
#endif

// ----------------------------------------------------- Private -------------------------------------------------------

// ---------------------- Callbacks ------------------------

/**
 * @brief User ISR callback placeholder
 *
 * This function is called by task_wdt_isr function (ISR for when TWDT times out). It can be redefined in user code to
 * handle TWDT events.
 *
 * @note It has the same limitations as the interrupt function. Do not use ESP_LOGI functions inside.
 */
void __attribute__((weak)) esp_task_wdt_isr_user_handler(void)
{

}

/**
 * @brief Idle hook callback
 *
 * Idle hook callback called by the idle tasks to feed the TWDT
 *
 * @return Whether the idle tasks should continue idling
 */
static bool idle_hook_cb(void)
{
#if CONFIG_FREERTOS_SMP
    esp_task_wdt_reset_user(core_user_handles[xPortGetCoreID()]);
#else
    esp_task_wdt_reset();
#endif
    return true;
}

// ----------------------- Helpers -------------------------

/**
 * @brief Reset hardware timer and reset flags of each entry
 */
static void reset_hw_timer(void)
{
    // All tasks have reset; time to reset the hardware timer.
    wdt_hal_write_protect_disable(&p_twdt_obj->hal);
    wdt_hal_feed(&p_twdt_obj->hal);
    wdt_hal_write_protect_enable(&p_twdt_obj->hal);
    //Clear the has_reset flag in each entry
    twdt_entry_t *entry;
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        entry->has_reset = false;
    }
}

/**
 * @brief Checks whether a user entry exists and if all other entries have been reset
 *
 * @param[in] user_entry User entry
 * @param[out] all_reset Whether all entries have been reset
 * @return Whether the user entry exists
 */
static bool find_entry_and_check_all_reset(twdt_entry_t *user_entry, bool *all_reset)
{
    bool found_user_entry = false;
    bool found_non_reset = false;

    twdt_entry_t *entry;
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        if (entry == user_entry) {
            found_user_entry = true;
        } else if (entry->has_reset == false) {
            found_non_reset = true;
        }
    }

    *all_reset = !found_non_reset;
    return found_user_entry;
}

/**
 * @brief Find whether a task entry exists, and checks if all other entries have been reset
 *
 * @param[in] handle Task handle
 * @param[out] all_reset Whether all entries have been reset
 * @return Task entry, or NULL if not found
 */
static twdt_entry_t *find_entry_from_task_handle_and_check_all_reset(TaskHandle_t handle, bool *all_reset)
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
 * @brief Create a task/user entry and add it to the task WDT
 *
 * @param[in] is_task Whether the entry is a task entry or user entry
 * @param[in] entry_data Data associated with the entry (either a task handle or user entry name)
 * @param[out] entry_ret Pointer to created entry
 * @return ESP_OK if entry was added, failure otherwise
 */
static esp_err_t add_entry(bool is_task, void *entry_data, twdt_entry_t **entry_ret)
{
    esp_err_t ret;

    // Allocate entry object
    twdt_entry_t *entry = calloc(1, sizeof(twdt_entry_t));
    if (entry == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (is_task) {
        entry->task_handle = (TaskHandle_t)entry_data;
    } else {
        entry->user_name = (const char *)entry_data;
    }

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE_ISR((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, state_err, TAG, "task watchdog was never initialized");
    // Check if the task is an entry, and if all entries have been reset
    bool all_reset;
    if (is_task) {
        twdt_entry_t *entry_found = find_entry_from_task_handle_and_check_all_reset(entry->task_handle, &all_reset);
        ESP_GOTO_ON_FALSE_ISR((entry_found == NULL), ESP_ERR_INVALID_ARG, state_err, TAG, "task is already subscribed");
    } else {
        bool entry_found = find_entry_and_check_all_reset(entry, &all_reset);
        ESP_GOTO_ON_FALSE_ISR(!entry_found, ESP_ERR_INVALID_ARG, state_err, TAG, "user is already subscribed");
    }
    // Add entry to list
    SLIST_INSERT_HEAD(&p_twdt_obj->entries_slist, entry, slist_entry);
    if (all_reset) {   //Reset hardware timer if all other tasks in list have reset in
        reset_hw_timer();
    }
    portEXIT_CRITICAL(&spinlock);
    *entry_ret = entry;
    return ESP_OK;

state_err:
    portEXIT_CRITICAL(&spinlock);
    free(entry);
    return ret;
}

/**
 * @brief Delete a task/user entry
 *
 * @param[in] is_task Whether the entry is a task entry or user entry
 * @param[in] entry_data Data associated with the entry (either a task handle or user entry name)
 * @return ESP_OK if entry was deleted, failure otherwise
 */
static esp_err_t delete_entry(bool is_task, void *entry_data)
{
    esp_err_t ret;

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE_ISR((p_twdt_obj != NULL), ESP_ERR_INVALID_STATE, err, TAG, "task watchdog was never initialized");
    // Find entry for task
    bool all_reset;
    twdt_entry_t *entry;
    if (is_task) {
        entry = find_entry_from_task_handle_and_check_all_reset((TaskHandle_t)entry_data, &all_reset);
        ESP_GOTO_ON_FALSE_ISR((entry != NULL), ESP_ERR_NOT_FOUND, err, TAG, "task not found");
    } else {
        entry = (twdt_entry_t *)entry_data;
        bool entry_found = find_entry_and_check_all_reset(entry, &all_reset);
        ESP_GOTO_ON_FALSE_ISR(entry_found, ESP_ERR_NOT_FOUND, err, TAG, "user not found");
    }
    // Remove entry
    SLIST_REMOVE(&p_twdt_obj->entries_slist, entry, twdt_entry, slist_entry);
    // Reset hardware timer if all remaining tasks have reset
    if (all_reset) {
        reset_hw_timer();
    }
    portEXIT_CRITICAL(&spinlock);
    free(entry);
    return ESP_OK;

err:
    portEXIT_CRITICAL(&spinlock);
    return ret;
}

/**
 * @brief Unsubscribe the idle tasks of one or more cores
 *
 * @param core_mask
 */
static void unsubscribe_idle(uint32_t core_mask)
{
    int core_num = 0;
    while (core_mask != 0) {
        if (core_mask & 0x1) {
#if CONFIG_FREERTOS_SMP
        assert(core_user_handles[core_num]);
        ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_hook_cb, core_num));
        ESP_ERROR_CHECK(esp_task_wdt_delete_user(core_user_handles[core_num]));
        core_user_handles[core_num] = NULL;
#else
        TaskHandle_t idle_task_handle = xTaskGetIdleTaskHandleForCPU(core_num);
        assert(idle_task_handle);
        esp_deregister_freertos_idle_hook_for_cpu(idle_hook_cb, core_num);
        ESP_ERROR_CHECK(esp_task_wdt_delete(idle_task_handle));
#endif
        }
        core_mask >>= 1;
        core_num++;
    }
}


/**
 * @brief Subscribes the idle tasks of one or more cores
 *
 * @param core_mask Bit mask of cores to subscribe
 */
static void subscribe_idle(uint32_t core_mask)
{
    int core_num = 0;
    while (core_mask != 0) {
        if (core_mask & 0x1) {
#if CONFIG_FREERTOS_SMP
        snprintf(core_user_names[core_num], CORE_USER_NAME_LEN, "CPU %d", (uint8_t)core_num);
        ESP_ERROR_CHECK(esp_task_wdt_add_user((const char *)core_user_names[core_num], &core_user_handles[core_num]));
        ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_hook_cb, core_num));
#else
        TaskHandle_t idle_task_handle = xTaskGetIdleTaskHandleForCPU(core_num);
        assert(idle_task_handle);
        ESP_ERROR_CHECK(esp_task_wdt_add(idle_task_handle));
        ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_hook_cb, core_num));
#endif
        }
        core_mask >>= 1;
        core_num++;
    }
}

/**
 * @brief TWDT timeout ISR function
 *
 * The ISR checks which entries have not been reset, prints some debugging information, and triggers a panic if
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
    ESP_EARLY_LOGE(TAG, "Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:");
    twdt_entry_t *entry;
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        if (!entry->has_reset) {
            if (entry->task_handle) {
#if CONFIG_FREERTOS_SMP
#if configNUM_CORES > 1
                // Log the task's name and its affinity
                ESP_EARLY_LOGE(TAG, " - %s (0x%x)", pcTaskGetName(entry->task_handle), vTaskCoreAffinityGet(entry->task_handle));
#else // configNUM_CORES > 1
                // Log the task's name
                ESP_EARLY_LOGE(TAG, " - %s", pcTaskGetName(entry->task_handle));
#endif // configNUM_CORES > 1
#else // CONFIG_FREERTOS_SMP
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
#endif // CONFIG_FREERTOS_SMP
            } else {
                ESP_EARLY_LOGE(TAG, " - %s", entry->user_name);
            }
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

esp_err_t esp_task_wdt_init(const esp_task_wdt_config_t *config)
{
    ESP_RETURN_ON_FALSE((config != NULL && config->idle_core_mask < (1 << portNUM_PROCESSORS)), ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    ESP_RETURN_ON_FALSE(p_twdt_obj == NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT already initialized");
    esp_err_t ret;

    twdt_obj_t *obj = NULL;
    if (p_twdt_obj == NULL) {
        // Allocate and initialize TWDT driver object
        obj = calloc(1, sizeof(twdt_obj_t));
        ESP_GOTO_ON_FALSE((obj != NULL), ESP_ERR_NO_MEM, err, TAG, "insufficient memory");
        SLIST_INIT(&obj->entries_slist);
        obj->panic = config->trigger_panic;
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
    wdt_hal_config_stage(&p_twdt_obj->hal, WDT_STAGE0, config->timeout_ms * (1000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_INT);
    // Configure 2nd stage timeout and behavior
    wdt_hal_config_stage(&p_twdt_obj->hal, WDT_STAGE1, config->timeout_ms * (2 * 1000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_RESET_SYSTEM);
    // Enable the WDT
    wdt_hal_enable(&p_twdt_obj->hal);
    wdt_hal_write_protect_enable(&p_twdt_obj->hal);
    // Update which core's idle tasks are subscribed
    uint32_t old_core_mask = p_twdt_obj->idle_core_mask;
    p_twdt_obj->idle_core_mask = config->idle_core_mask;
    portEXIT_CRITICAL(&spinlock);
    if (old_core_mask) {
        // Unsubscribe all previously watched core idle tasks
        unsubscribe_idle(old_core_mask);
    }
    if (config->idle_core_mask) {
        // Subscribe the new cores idle tasks
        subscribe_idle(config->idle_core_mask);
    }

    ret = ESP_OK;
err:
    return ret;
}

esp_err_t esp_task_wdt_deinit(void)
{
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");

    esp_err_t ret;
    // Unsubscribe all previously watched core idle tasks
    unsubscribe_idle(p_twdt_obj->idle_core_mask);

    portENTER_CRITICAL(&spinlock);
    // Check TWDT state
    ESP_GOTO_ON_FALSE_ISR(SLIST_EMPTY(&p_twdt_obj->entries_slist), ESP_ERR_INVALID_STATE, err, TAG, "Tasks/users still subscribed");
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
    ESP_ERROR_CHECK(esp_intr_free(obj->intr_handle));   // Deregister interrupt
    free(obj);  // Free p_twdt_obj
    return ESP_OK;

err:
    portEXIT_CRITICAL(&spinlock);
    subscribe_idle(p_twdt_obj->idle_core_mask); // Resubscribe idle tasks
    return ret;
}

esp_err_t esp_task_wdt_add(TaskHandle_t task_handle)
{
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    esp_err_t ret;
    if (task_handle == NULL) {   // Get handle of current task if none is provided
        task_handle = xTaskGetCurrentTaskHandle();
    }

    twdt_entry_t *entry;
    ret = add_entry(true, (void *)task_handle, &entry);
    (void) entry; // Returned entry pointer not used
    return ret;
}

esp_err_t esp_task_wdt_add_user(const char *user_name, esp_task_wdt_user_handle_t *user_handle_ret)
{
    ESP_RETURN_ON_FALSE((user_name != NULL && user_handle_ret != NULL), ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    esp_err_t ret;
    twdt_entry_t *entry;
    ret = add_entry(false, (void *)user_name, &entry);
    if (ret == ESP_OK) {
        *user_handle_ret = (esp_task_wdt_user_handle_t)entry;
    }
    return ret;
}

esp_err_t esp_task_wdt_reset(void)
{
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    esp_err_t ret;
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();

    portENTER_CRITICAL(&spinlock);
    // Find entry from task handle
    bool all_reset;
    twdt_entry_t *entry;
    entry = find_entry_from_task_handle_and_check_all_reset(handle, &all_reset);
    ESP_GOTO_ON_FALSE_ISR((entry != NULL), ESP_ERR_NOT_FOUND, err, TAG, "task not found");
    // Mark entry as reset and issue timer reset if all entries have been reset
    entry->has_reset = true;    // Reset the task if it's on the task list
    if (all_reset) {    // Reset if all other tasks in list have reset in
        reset_hw_timer();
    }
    ret = ESP_OK;
err:
    portEXIT_CRITICAL(&spinlock);

    return ret;
}

esp_err_t esp_task_wdt_reset_user(esp_task_wdt_user_handle_t user_handle)
{
    ESP_RETURN_ON_FALSE(user_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    esp_err_t ret;

    portENTER_CRITICAL(&spinlock);
    // Check if entry exists
    bool all_reset;
    twdt_entry_t *entry = (twdt_entry_t *)user_handle;
    bool entry_found = find_entry_and_check_all_reset(entry, &all_reset);
    ESP_GOTO_ON_FALSE_ISR(entry_found, ESP_ERR_NOT_FOUND, err, TAG, "user handle not found");
    // Mark entry as reset and issue timer reset if all entries have been reset
    entry->has_reset = true;    // Reset the task if it's on the task list
    if (all_reset) {    // Reset if all other tasks in list have reset in
        reset_hw_timer();
    }
    ret = ESP_OK;
err:
    portEXIT_CRITICAL(&spinlock);

    return ret;
}

esp_err_t esp_task_wdt_delete(TaskHandle_t task_handle)
{
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    esp_err_t ret;
    if (task_handle == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    ret = delete_entry(true, (void *)task_handle);
    return ret;
}

esp_err_t esp_task_wdt_delete_user(esp_task_wdt_user_handle_t user_handle)
{
    ESP_RETURN_ON_FALSE(user_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    return delete_entry(false, (void *)user_handle);
}

esp_err_t esp_task_wdt_status(TaskHandle_t task_handle)
{
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");
    esp_err_t ret;
    if (task_handle == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    portENTER_CRITICAL(&spinlock);
    // Find entry for task
    bool all_reset;
    twdt_entry_t *entry;
    entry = find_entry_from_task_handle_and_check_all_reset(task_handle, &all_reset);
    (void) all_reset;   // Unused
    ret = (entry != NULL) ? ESP_OK : ESP_ERR_NOT_FOUND;
    portEXIT_CRITICAL(&spinlock);

    return ret;
}
