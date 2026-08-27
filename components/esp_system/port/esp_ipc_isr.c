/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_private/esp_system_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "esp_private/esp_ipc_isr.h"
#include "esp_private/esp_ipc_isr_port.h"
#include "esp_ipc_isr.h"
#include "sdkconfig.h"

static portMUX_TYPE s_ipc_isr_mux = portMUX_INITIALIZER_UNLOCKED;
uint32_t volatile esp_ipc_isr_start_fl;        // the flag shows that it is about to run esp_ipc_func()
uint32_t volatile esp_ipc_isr_end_fl = 1;      // the flag shows that esp_ipc_func() is done
uint32_t volatile esp_ipc_isr_stall_fl;        // the flag shows that the other CPU entered the stall loop
esp_ipc_isr_func_t volatile esp_ipc_func;      // the function which will be run in the ipc_isr context
void * volatile esp_ipc_func_arg;              // the argument of esp_ipc_func()
esp_ipc_isr_args_t volatile esp_ipc_isr_stall_args; // the context for the stalled CPU

// This asm function is from esp_ipc_isr_routines.S.
// It is waiting for the finish_cmd command in a loop.
void esp_ipc_isr_waiting_for_finish_cmd(void* arg);

typedef enum {
    STALL_STATE_IDLE      = 0,
    STALL_STATE_RUNNING   = 1,
} stall_state_t;

static stall_state_t volatile s_stall_state = STALL_STATE_IDLE;
static int32_t volatile s_count_of_nested_calls[CONFIG_FREERTOS_NUMBER_OF_CORES] = { 0 };
static BaseType_t s_stored_interrupt_level;
static bool volatile s_other_cpu_stalled = false;

/**
 * @brief Type of calling
 */
typedef enum {
    IPC_ISR_WAIT_FOR_START      = (1 << 0),   /*!< The caller is waiting for the start */
    IPC_ISR_WAIT_FOR_END        = (1 << 1),   /*!< The caller is waiting for the end */
    IPC_ISR_SAFE_STALL          = (1 << 2),   /*!< Safe stall mode: run the stall command loop and verify the stalled CPU context */
} esp_ipc_isr_wait_t;

#define IPC_ISR_ENTER_CRITICAL() portENTER_CRITICAL_SAFE(&s_ipc_isr_mux)
#define IPC_ISR_EXIT_CRITICAL()  portEXIT_CRITICAL_SAFE(&s_ipc_isr_mux)

static esp_err_t ipc_isr_call_and_wait(esp_ipc_isr_func_t func, void* arg, esp_ipc_isr_wait_t wait_for);
static esp_err_t ipc_isr_stall_other_cpu(esp_ipc_isr_wait_t wait_for);

/* Initializing IPC_ISR */

void esp_ipc_isr_init(void)
{
    const uint32_t cpuid = xPortGetCoreID();
    esp_ipc_isr_port_init(cpuid);

    if (cpuid != 0) {
        s_stall_state = STALL_STATE_RUNNING;
    }
}

/* End initializing IPC_ISR */

/* Public API functions */

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_call(esp_ipc_isr_func_t func, void* arg)
{
    IPC_ISR_ENTER_CRITICAL();
    if (s_other_cpu_stalled) {
        /* If the other CPU is already stalled, wait for a previous call
           to finish and initiate a new call */
        while (esp_ipc_isr_stall_args.func != NULL) {}
        esp_ipc_isr_stall_args.arg = arg;
        esp_ipc_isr_stall_args.func = (void*)func;
        // do not wait for the user's callback function to finish
    } else {
        ipc_isr_call_and_wait(func, arg, IPC_ISR_WAIT_FOR_START);
    }
    IPC_ISR_EXIT_CRITICAL();
}

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_call_blocking(esp_ipc_isr_func_t func, void* arg)
{
    IPC_ISR_ENTER_CRITICAL();
    if (s_other_cpu_stalled) {
        // If the other CPU is already stalled, wait for a previous call to finish and initiate a new call
        while (esp_ipc_isr_stall_args.func != NULL) {}
        esp_ipc_isr_stall_args.arg = arg;
        esp_ipc_isr_stall_args.func = (void*)func;
        /* Wait for the user's callback function to complete.
           The esp_ipc_isr_waiting_for_finish_cmd function will reset
           the func field in esp_ipc_isr_stall_args once the callback
           has finished, indicating that the other CPU is ready to accept
           new callbacks. */
        while (esp_ipc_isr_stall_args.func != NULL) {}
    } else {
        ipc_isr_call_and_wait(func, arg, IPC_ISR_WAIT_FOR_END);
    }
    IPC_ISR_EXIT_CRITICAL();
}

/*
 * esp_ipc_isr_stall_other_cpu is used for:
 *  - stall other CPU,
 *  - do protection when dual core access DPORT internal register and APB register via DPORT simultaneously.
 * This function will be initialize after FreeRTOS startup.
 * When cpu0 wants to access DPORT register, it should notify cpu1 enter in high-priority interrupt for be mute.
 * When cpu1 already in high-priority interrupt, cpu0 can access DPORT register.
 * Currently, cpu1 will wait for cpu0 finish access and exit high-priority interrupt.
 */
void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_stall_other_cpu(void)
{
    ipc_isr_stall_other_cpu(IPC_ISR_WAIT_FOR_START);
}

/*
 * Stall the other CPU core only if it is in a "safe" state (not in a critical section or ISR).
 * Returns ESP_OK if the stall was successful, or ESP_ERR_NOT_ALLOWED if the other core
 * is currently in a critical section or ISR context.
 */
esp_err_t ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_stall_other_cpu_safe(void)
{
    return ipc_isr_stall_other_cpu(IPC_ISR_WAIT_FOR_START | IPC_ISR_SAFE_STALL);
}

bool ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_is_other_cpu_stalled(void)
{
    return s_other_cpu_stalled;
}

static esp_err_t ESP_SYSTEM_IRAM_ATTR ipc_isr_stall_other_cpu(esp_ipc_isr_wait_t wait_for)
{
    if (s_stall_state == STALL_STATE_RUNNING) {
#if CONFIG_FREERTOS_SMP
        BaseType_t intLvl = portDISABLE_INTERRUPTS();
#else
        BaseType_t intLvl = portSET_INTERRUPT_MASK_FROM_ISR();
#endif
        const uint32_t cpu_id = xPortGetCoreID();
        if (s_count_of_nested_calls[cpu_id]++ == 0) {
            IPC_ISR_ENTER_CRITICAL();
            s_stored_interrupt_level = intLvl;
            esp_ipc_isr_stall_args.cmd = ESP_IPC_ISR_CMD_RESET_STATE;
            esp_ipc_isr_stall_args.func = NULL;
            esp_ipc_isr_stall_args.arg = NULL;
            esp_ipc_isr_stall_args.interrupted_context_unsafe = 1;
            s_other_cpu_stalled = false;
            esp_ipc_isr_stall_fl = 0;
            esp_err_t error = ipc_isr_call_and_wait(esp_ipc_isr_waiting_for_finish_cmd, NULL, wait_for);
            if (error != ESP_OK) {
                esp_ipc_isr_release_other_cpu();
            } else {
                // Publish the stalled state only after the other CPU enters the stall loop.
                while (!esp_ipc_isr_stall_fl) {};
                s_other_cpu_stalled = true;
            }
            return error;
        }

        /* Interrupts are already disabled by the parent, we're nested here. */
#if CONFIG_FREERTOS_SMP
        portRESTORE_INTERRUPTS(intLvl);
#else
        portCLEAR_INTERRUPT_MASK_FROM_ISR(intLvl);
#endif
        return ESP_OK;
    }
    return ESP_FAIL;
}

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_release_other_cpu(void)
{
    if (s_stall_state == STALL_STATE_RUNNING) {
        const uint32_t cpu_id = xPortGetCoreID();
        if (--s_count_of_nested_calls[cpu_id] == 0) {
            s_other_cpu_stalled = false;
            esp_ipc_isr_stall_args.cmd = ESP_IPC_ISR_CMD_FINISH;
            // Make sure end flag is set and esp_ipc_isr_waiting_for_finish_cmd is done.
            while (!esp_ipc_isr_end_fl) {};
            esp_ipc_isr_stall_fl = 0;
            IPC_ISR_EXIT_CRITICAL();
#if CONFIG_FREERTOS_SMP
            portRESTORE_INTERRUPTS(s_stored_interrupt_level);
#else
            portCLEAR_INTERRUPT_MASK_FROM_ISR(s_stored_interrupt_level);
#endif
        } else if (s_count_of_nested_calls[cpu_id] < 0) {
            assert(0);
        }
    }
}

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_stall_pause(void)
{
    IPC_ISR_ENTER_CRITICAL();
    s_stall_state = STALL_STATE_IDLE;
    IPC_ISR_EXIT_CRITICAL();
}

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_stall_abort(void)
{
    //Note: We don't enter a critical section here as we are calling this from a panic.
    s_stall_state = STALL_STATE_IDLE;
}

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_stall_resume(void)
{
    IPC_ISR_ENTER_CRITICAL();
    s_stall_state = STALL_STATE_RUNNING;
    IPC_ISR_EXIT_CRITICAL();
}

/* End public API functions */

/* Private functions*/

static esp_err_t ESP_SYSTEM_IRAM_ATTR ipc_isr_call_and_wait(esp_ipc_isr_func_t func, void* arg, esp_ipc_isr_wait_t wait_for)
{
    const uint32_t cpu_id = xPortGetCoreID();

    // waiting for the end of the previous call
    while (!esp_ipc_isr_end_fl) {};

    esp_ipc_func = func;
    esp_ipc_func_arg = arg;

    esp_ipc_isr_start_fl = 0;
    esp_ipc_isr_end_fl = 0;

    // Trigger an interrupt on the opposite core.
    esp_ipc_isr_port_int_trigger(!cpu_id);
    // IPC_ISR handler will be called and `...isr_start` and `...isr_end` will be updated there

    if (wait_for & IPC_ISR_WAIT_FOR_START) {
        while (!esp_ipc_isr_start_fl) {};
    }

    if (wait_for & IPC_ISR_SAFE_STALL) {
        /* The handler snapshots the interrupted context before publishing
         * esp_ipc_isr_start_fl. A non-zero value means the other CPU was in an
         * ISR or interrupt-masked section, including the small window where
         * interrupts are already masked but FreeRTOS nesting counters are not
         * updated yet.
         */
        bool safe_to_continue = (esp_ipc_isr_stall_args.interrupted_context_unsafe == 0);
        if (!safe_to_continue) {
            return ESP_ERR_NOT_ALLOWED;
        }
        /* IPC_ISR_SAFE_STALL is used only with the stall command-loop
         * callback. At this point, esp_ipc_isr_waiting_for_finish_cmd()
         * is already running on the other CPU and keeps it blocked while
         * this CPU checks whether the interrupted context was safe. If it
         * was safe, return without waiting for callback completion: later
         * IPC ISR stall APIs will either execute callbacks on the stalled
         * CPU or release it from the loop.
         */
        return ESP_OK;
    } else if (wait_for & IPC_ISR_WAIT_FOR_END) {
        // Blocking callers wait until the IPC ISR handler finishes the callback.
        while (!esp_ipc_isr_end_fl) {};
    }
    return ESP_OK;
}

/* End private functions*/
