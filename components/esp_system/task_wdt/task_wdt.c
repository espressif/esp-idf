/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/freertos_debug.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_debug_helpers.h"
#include "esp_freertos_hooks.h"
#include "esp_task_wdt.h"
#include "esp_private/system_internal.h"
#include "esp_private/crosscore_int.h"
#include "esp_private/esp_task_wdt.h"
#include "esp_private/esp_task_wdt_impl.h"

#if CONFIG_IDF_TARGET_ARCH_RISCV
#include "riscv/rvruntime-frames.h"
#endif //CONFIG_IDF_TARGET_ARCH_RISCV

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
#include "esp_private/eh_frame_parser.h"
#endif // CONFIG_ESP_SYSTEM_USE_EH_FRAME

#if CONFIG_IDF_TARGET_ARCH_RISCV && !CONFIG_ESP_SYSTEM_USE_EH_FRAME
/* Function used to print all the registers pointed by the given frame .*/
extern void panic_print_registers(const void *frame, int core);
#endif // CONFIG_IDF_TARGET_ARCH_RISCV && !CONFIG_ESP_SYSTEM_USE_EH_FRAME

/* We will use this function in order to simulate an `abort()` occurring in
 * a different context than the one it's called from. */
extern void xt_unhandled_exception(void *frame);

/* Forward declaration of the idle hook callback */
static bool idle_hook_cb(void);

/* Global flag set to make the `panic` mechanism think a real `abort()` was
 * called. This is used in the ISR handler, in case we have to panic when
 * a task doesn't feed its timer. */
extern bool g_panic_abort;

/* Global flag marking whether the current ISR is a Task Watchdog ISR. */
bool g_twdt_isr = false;

// --------------------------------------------------- Definitions -----------------------------------------------------

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
    twdt_ctx_t impl_ctx;
    SLIST_HEAD(entry_list_head, twdt_entry) entries_slist;
    uint32_t idle_core_mask;    // Current core's who's idle tasks are subscribed
    bool panic; // Flag to trigger panic when TWDT times out
    bool waiting_for_task; // Flag to start the timer as soon as a task is added
};

// ----------------------- Objects -------------------------

static const char *TAG = "task_wdt";
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
static twdt_obj_t *p_twdt_obj = NULL;

#if CONFIG_FREERTOS_SMP
#define CORE_USER_NAME_LEN      8   // Long enough for "CPU XXX"
static esp_task_wdt_user_handle_t core_user_handles[CONFIG_FREERTOS_NUMBER_OF_CORES] = {NULL};
static char core_user_names[CONFIG_FREERTOS_NUMBER_OF_CORES][CORE_USER_NAME_LEN];
#endif

// ----------------------------------------------------- Private -------------------------------------------------------

// ----------------------- Helpers -------------------------

/**
 * @brief Reset the timer and reset flags of each entry
 * When entering this function, the spinlock has already been taken, no need to take it back.
 */
static void task_wdt_timer_feed(void)
{
    esp_task_wdt_impl_timer_feed(p_twdt_obj->impl_ctx);

    /* Clear the has_reset flag in each entry */
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
    // Start the timer if it has not been started yet and was waiting on a task to registered
    if (p_twdt_obj->waiting_for_task) {
        esp_task_wdt_impl_timer_restart(p_twdt_obj->impl_ctx);
        p_twdt_obj->waiting_for_task = false;
    }
    if (all_reset) {   //Reset hardware timer if all other tasks in list have reset in
        task_wdt_timer_feed();
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
    /* Stop the timer if we don't have any more tasks/objects to watch */
    if (SLIST_EMPTY(&p_twdt_obj->entries_slist)) {
        p_twdt_obj->waiting_for_task = true;
        esp_task_wdt_impl_timer_stop(p_twdt_obj->impl_ctx);
    } else {
        p_twdt_obj->waiting_for_task = false;
    }
    /* Reset hardware timer if all remaining tasks have reset and if the list of tasks is not empty */
    if (!p_twdt_obj->waiting_for_task && all_reset) {
        task_wdt_timer_feed();
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
            esp_deregister_freertos_idle_hook_for_cpu(idle_hook_cb, core_num);
            ESP_ERROR_CHECK(esp_task_wdt_delete_user(core_user_handles[core_num]));
            core_user_handles[core_num] = NULL;
#else // CONFIG_FREERTOS_SMP
            TaskHandle_t idle_task_handle = xTaskGetIdleTaskHandleForCore(core_num);
            assert(idle_task_handle);
            esp_deregister_freertos_idle_hook_for_cpu(idle_hook_cb, core_num);
            ESP_ERROR_CHECK(esp_task_wdt_delete(idle_task_handle));
#endif // CONFIG_FREERTOS_SMP
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
#else // CONFIG_FREERTOS_SMP
            TaskHandle_t idle_task_handle = xTaskGetIdleTaskHandleForCore(core_num);
            assert(idle_task_handle);
            ESP_ERROR_CHECK(esp_task_wdt_add(idle_task_handle));
            ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_hook_cb, core_num));
#endif // CONFIG_FREERTOS_SMP
        }
        core_mask >>= 1;
        core_num++;
    }
}

/**
 * The behavior of the Task Watchdog depends on the configuration from the `menuconfig`.
 * It can be summarized as follow, regardless of the target:
 * +------------------------+--------------------------------+--------------------------+
 * | \  Panic configuration |                                |                          |
 * |  +------------------+  | Panic Enabled                  | Panic Disabled           |
 * | TWDT triggered on    \ |                                |                          |
 * +------------------------+--------------------------------+--------------------------+
 * |                        | - Current core backtrace       | - Current core backtrace |
 * | Both Cores             | - Crosscore TWDT abort         | - Crosscore backtrace    |
 * |                        | - Wait for other core to abort |                          |
 * +------------------------+--------------------------------+--------------------------+
 * | Other Core             | - Crosscore TWDT abort         | - Crosscore backtrace    |
 * +------------------------+--------------------------------+--------------------------+
 * | Current Core           | - Abort from current CPU       | - Current core backtrace |
 * +------------------------+--------------------------------+--------------------------+
 *
 */

static UBaseType_t get_task_affinity(const TaskHandle_t xTask)
{
    if (xTask == NULL) {
        /* User entry, we cannot predict on which core it is scheduled to run,
         * so let's mark all cores as failing */
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
        return BIT(1) | BIT(0);
#else
        return BIT(0);
#endif
    }

#if CONFIG_FREERTOS_SMP
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
    return vTaskCoreAffinityGet(xTask);
#else
    return BIT(0);
#endif
#else
    BaseType_t task_affinity = xTaskGetCoreID(xTask);
    if (task_affinity == 0 || task_affinity == 1) {
        return BIT(task_affinity);
    }
    return BIT(1) | BIT(0);
#endif
}

/**
 * Function simulating an abort coming from the interrupted task of the current
 * core.
 * It is called either by the function right below or by a crosscore interrupt,
 * in the case where the other core (than the main one) has to abort because one
 * of his tasks didn't reset the TWDT on time.
 */
void task_wdt_timeout_abort(bool current_core)
{
    TaskSnapshot_t snapshot = { 0 };
    BaseType_t __attribute__((unused)) ret = pdTRUE;

    ESP_EARLY_LOGE(TAG, "Aborting.");
    esp_reset_reason_set_hint(ESP_RST_TASK_WDT);
    ret = vTaskGetSnapshot(xTaskGetCurrentTaskHandle(), &snapshot);
    assert(ret == pdTRUE);
    g_panic_abort = true;
    /* We should set this flag as late as possible, as this function may
     * be called after a crosscore interrupt. Indeed, a higher interrupt may occur
     * after calling the crosscore interrupt, if its handler fails, this flag
     * shall not be set.
     * This flag will tell the coredump component (if activated) that yes, we are in
     * an ISR context, but it is intended, it is not because an ISR encountered an
     * exception. If we don't set such flag, later tested by coredump, the latter would
     * switch the execution frame/context we are giving it to the interrupt stack.
     * For details about this behavior in the TODO task: IDF-5694
     */
    g_twdt_isr = true;
    void *frame = (void *) snapshot.pxTopOfStack;

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME | CONFIG_IDF_TARGET_ARCH_XTENSA
    if (current_core) {
        ESP_EARLY_LOGE(TAG, "Print CPU %d (current core) backtrace", xPortGetCoreID());
    } else {
        ESP_EARLY_LOGE(TAG, "Print CPU %d backtrace", xPortGetCoreID());
    }
#endif

    xt_unhandled_exception(frame);
}

static void task_wdt_timeout_handling(int cores_fail, bool panic)
{
    const int current_core = xPortGetCoreID();

    if (panic) {
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        const int other_core = !current_core;

        if ((cores_fail & BIT(0)) && (cores_fail & BIT(1))) {
            /* In the case where both CPUs have failing tasks, print the current CPU backtrace and then let the
             * other core fail. */
            ESP_EARLY_LOGE(TAG, "Print CPU %d (current core) backtrace", current_core);
            esp_backtrace_print(100);
            /* TODO: the interrupt we send should have the highest priority */
            esp_crosscore_int_send_twdt_abort(other_core);
            /* We are going to abort, on the other core, we have nothing to
             * do anymore here, just wait until we crash */
            while (1) {}
        } else if (cores_fail & BIT(other_core)) {
            /* If only the other core is failing, we can tell it to abort. */
            esp_crosscore_int_send_twdt_abort(other_core);
            while (1) {}
        }
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        /* Current core is failing, abort right now */
        task_wdt_timeout_abort(true);
    } else {
        /* Print backtrace of the core that failed to reset the watchdog */
        if (cores_fail & BIT(current_core)) {
            ESP_EARLY_LOGE(TAG, "Print CPU %d (current core) backtrace", current_core);
            esp_backtrace_print(100);
        }
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        const int other_core = !current_core;
        if (cores_fail & BIT(other_core)) {
            ESP_EARLY_LOGE(TAG, "Print CPU %d backtrace", other_core);
            esp_crosscore_int_send_print_backtrace(other_core);
        }
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    }
}

// ---------------------- Callbacks ------------------------

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
#else // CONFIG_FREERTOS_SMP
    esp_task_wdt_reset();
#endif // CONFIG_FREERTOS_SMP
    return true;
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
    esp_task_wdt_impl_timeout_triggered(p_twdt_obj->impl_ctx);

    /* Keep a bitmap of CPU cores having tasks that have not reset TWDT.
     * Bit 0 represents core 0, bit 1 represents core 1, and so on. */
    int cpus_fail = 0;
    bool panic = p_twdt_obj->panic;

    if (esp_task_wdt_print_triggered_tasks(NULL, NULL, &cpus_fail) != ESP_OK) {
        // If there are no entries, there's nothing to do.
        portEXIT_CRITICAL_ISR(&spinlock);
        return;
    }

    ESP_EARLY_LOGE(TAG, "%s", DRAM_STR("Tasks currently running:"));
    for (int x = 0; x < CONFIG_FREERTOS_NUMBER_OF_CORES; x++) {
        ESP_EARLY_LOGE(TAG, "CPU %d: %s", x, pcTaskGetName(xTaskGetCurrentTaskHandleForCore(x)));
    }
    portEXIT_CRITICAL_ISR(&spinlock);

    /* Run user ISR handler.
     * This function has been declared as weak, thus, it may be possible that it was not defines.
     * to check this, we can directly test its address. In any case, the linker will get rid of
     * this `if` when linking, this means that if the function was not defined, the whole `if`
     * block will be discarded (zero runtime overhead), else only the function call will be kept.
     */
    if (esp_task_wdt_isr_user_handler != NULL) {
        esp_task_wdt_isr_user_handler();
    }

    // Trigger configured timeout behavior (e.g., panic or print backtrace)
    assert(cpus_fail != 0);
    task_wdt_timeout_handling(cpus_fail, panic);
}

// ----------------------------------------------------- Public --------------------------------------------------------

esp_err_t esp_task_wdt_init(const esp_task_wdt_config_t *config)
{
    ESP_RETURN_ON_FALSE((config != NULL && config->idle_core_mask < (1 << CONFIG_FREERTOS_NUMBER_OF_CORES)), ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    ESP_RETURN_ON_FALSE(p_twdt_obj == NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT already initialized");
    esp_err_t ret = ESP_OK;
    twdt_obj_t *obj = NULL;

    /* Allocate and initialize the global object */
    obj = calloc(1, sizeof(twdt_obj_t));
    ESP_GOTO_ON_FALSE((obj != NULL), ESP_ERR_NO_MEM, err, TAG, "insufficient memory");
    SLIST_INIT(&obj->entries_slist);
    obj->panic = config->trigger_panic;

    /* Allocate the timer itself, NOT STARTED */
    ret = esp_task_wdt_impl_timer_allocate(config, task_wdt_isr, &obj->impl_ctx);
    if (ret != ESP_OK) {
        goto err;
    }

    /* No error so far, we can assign it to the driver object */
    p_twdt_obj = obj;

    /* Update which core's idle tasks are subscribed */
    p_twdt_obj->idle_core_mask = config->idle_core_mask;
    if (config->idle_core_mask) {
        /* Subscribe the new cores idle tasks */
        subscribe_idle(config->idle_core_mask);
    }

    /* Start the timer only if we are watching some tasks */
    if (!SLIST_EMPTY(&p_twdt_obj->entries_slist)) {
        p_twdt_obj->waiting_for_task = false;
        esp_task_wdt_impl_timer_restart(p_twdt_obj->impl_ctx);
    } else {
        p_twdt_obj->waiting_for_task = true;
    }

    return ESP_OK;
err:
    free(obj);
    return ret;
}

esp_err_t esp_task_wdt_reconfigure(const esp_task_wdt_config_t *config)
{
    ESP_RETURN_ON_FALSE((config != NULL && config->idle_core_mask < (1 << CONFIG_FREERTOS_NUMBER_OF_CORES)), ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT not initialized yet");
    uint32_t old_core_mask = 0;
    esp_err_t ret = ESP_OK;

    /* Stop the timer to make sure we don't get into the ISR while reconfiguring the TWDT */
    portENTER_CRITICAL(&spinlock);
    ret = esp_task_wdt_impl_timer_stop(p_twdt_obj->impl_ctx);
    if (ret != ESP_OK) {
        goto err;
    }

    /* We can start reconfiguring the tasks */
    p_twdt_obj->panic = config->trigger_panic;

    /* Reconfigure the timer underneath (without restarting it) */
    ret = esp_task_wdt_impl_timer_reconfigure(p_twdt_obj->impl_ctx, config);
    if (ret != ESP_OK) {
        goto err;
    }

    old_core_mask = p_twdt_obj->idle_core_mask;
    /* If the new mask is different than the old one, we have to subscribe the new idle tasks */
    if (old_core_mask != config->idle_core_mask) {
        p_twdt_obj->idle_core_mask = config->idle_core_mask;

        /* Unsubscribe all previously watched core idle tasks */
        unsubscribe_idle(old_core_mask);

        if (config->idle_core_mask) {
            /* Subscribe the new cores idle tasks */
            subscribe_idle(config->idle_core_mask);
        }
    }

    /* Start the timer only if we are watching some tasks */
    if (!SLIST_EMPTY(&p_twdt_obj->entries_slist)) {
        esp_task_wdt_impl_timer_restart(p_twdt_obj->impl_ctx);
    }

    portEXIT_CRITICAL(&spinlock);
err:
    return ESP_OK;
}

esp_err_t esp_task_wdt_stop(void)
{
    esp_err_t ret = ESP_OK;

    /* If the timer has not been initialized, do not attempt to stop it */
    if (p_twdt_obj == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        portENTER_CRITICAL(&spinlock);
        ret = esp_task_wdt_impl_timer_stop(p_twdt_obj->impl_ctx);
        portEXIT_CRITICAL(&spinlock);
    }

    return ret;
}

esp_err_t esp_task_wdt_restart(void)
{
    esp_err_t ret = ESP_OK;

    /* If the timer has not been initialized, do not attempt to stop it */
    if (p_twdt_obj == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        portENTER_CRITICAL(&spinlock);
        ret = esp_task_wdt_impl_timer_restart(p_twdt_obj->impl_ctx);
        portEXIT_CRITICAL(&spinlock);
    }

    return ret;
}

esp_err_t esp_task_wdt_deinit(void)
{
    esp_err_t ret;

    ESP_RETURN_ON_FALSE(p_twdt_obj != NULL, ESP_ERR_INVALID_STATE, TAG, "TWDT was never initialized");

    // Unsubscribe all previously watched core idle tasks
    unsubscribe_idle(p_twdt_obj->idle_core_mask);

    // Check TWDT state
    ESP_GOTO_ON_FALSE_ISR(SLIST_EMPTY(&p_twdt_obj->entries_slist), ESP_ERR_INVALID_STATE, err, TAG, "Tasks/users still subscribed");

    // Disable the timer
    esp_task_wdt_impl_timer_stop(p_twdt_obj->impl_ctx);

    // Free driver resources
    esp_task_wdt_impl_timer_free(p_twdt_obj->impl_ctx);

    // Free the global object
    free(p_twdt_obj);
    p_twdt_obj = NULL;

    return ESP_OK;

err:
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
        task_wdt_timer_feed();
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
        task_wdt_timer_feed();
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

esp_err_t esp_task_wdt_print_triggered_tasks(task_wdt_msg_handler msg_handler, void *opaque, int *cpus_fail)
{
    if (SLIST_EMPTY(&p_twdt_obj->entries_slist)) {
        return ESP_FAIL;
    }

    twdt_entry_t *entry;
    const char *caption = "Task watchdog got triggered. "
                          "The following tasks/users did not reset the watchdog in time:";

    if (msg_handler == NULL) {
        ESP_EARLY_LOGE(TAG, "%s", caption);
    } else {
        msg_handler(opaque, caption);
    }

    // Find what entries triggered the TWDT timeout (i.e., which entries have not been reset)
    SLIST_FOREACH(entry, &p_twdt_obj->entries_slist, slist_entry) {
        if (!entry->has_reset) {
            const char *cpu;
            const char *name = entry->task_handle ? pcTaskGetName(entry->task_handle) : entry->user_name;
            const UBaseType_t affinity = get_task_affinity(entry->task_handle);
            if (cpus_fail) {
                *cpus_fail |= affinity;
            }
            if (affinity == BIT(0)) {
                cpu = " (CPU 0)";
            } else if (affinity == BIT(1)) {
                cpu = " (CPU 1)";
            } else {
                cpu = " (CPU 0/1)";
            }
            if (msg_handler == NULL) {
                ESP_EARLY_LOGE(TAG, " - %s%s", name, cpu);
            } else {
                msg_handler(opaque, "\n - ");
                msg_handler(opaque, name);
                msg_handler(opaque, cpu);
            }
        }
    }
    return ESP_OK;
}
