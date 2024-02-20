/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_attr.h"
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
esp_ipc_isr_func_t volatile esp_ipc_func;      // the function which will be run in the ipc_isr context
void * volatile esp_ipc_func_arg;              // the argument of esp_ipc_func()

typedef enum {
    STALL_STATE_IDLE      = 0,
    STALL_STATE_RUNNING   = 1,
} stall_state_t;

static stall_state_t volatile s_stall_state = STALL_STATE_IDLE;
static int32_t volatile s_count_of_nested_calls[CONFIG_FREERTOS_NUMBER_OF_CORES] = { 0 };
static BaseType_t s_stored_interrupt_level;
static uint32_t volatile esp_ipc_isr_finish_cmd;

/**
 * @brief Type of calling
 */
typedef enum {
    IPC_ISR_WAIT_FOR_START = 0,   /*!< The caller is waiting for the start */
    IPC_ISR_WAIT_FOR_END   = 1,   /*!< The caller is waiting for the end */
} esp_ipc_isr_wait_t;

#define IPC_ISR_ENTER_CRITICAL() portENTER_CRITICAL_SAFE(&s_ipc_isr_mux)
#define IPC_ISR_EXIT_CRITICAL()  portEXIT_CRITICAL_SAFE(&s_ipc_isr_mux)

static void esp_ipc_isr_call_and_wait(esp_ipc_isr_func_t func, void* arg, esp_ipc_isr_wait_t wait_for);

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

void IRAM_ATTR esp_ipc_isr_call(esp_ipc_isr_func_t func, void* arg)
{
    IPC_ISR_ENTER_CRITICAL();
    esp_ipc_isr_call_and_wait(func, arg, IPC_ISR_WAIT_FOR_START);
    IPC_ISR_EXIT_CRITICAL();
}

void IRAM_ATTR esp_ipc_isr_call_blocking(esp_ipc_isr_func_t func, void* arg)
{
    IPC_ISR_ENTER_CRITICAL();
    esp_ipc_isr_call_and_wait(func, arg, IPC_ISR_WAIT_FOR_END);
    IPC_ISR_EXIT_CRITICAL();
}

// This asm function is from esp_ipc_isr_routines.S.
// It is waiting for the finish_cmd command in a loop.
void esp_ipc_isr_waiting_for_finish_cmd(void* finish_cmd);

/*
 * esp_ipc_isr_stall_other_cpu is used for:
 *  - stall other CPU,
 *  - do protection when dual core access DPORT internal register and APB register via DPORT simultaneously.
 * This function will be initialize after FreeRTOS startup.
 * When cpu0 wants to access DPORT register, it should notify cpu1 enter in high-priority interrupt for be mute.
 * When cpu1 already in high-priority interrupt, cpu0 can access DPORT register.
 * Currently, cpu1 will wait for cpu0 finish access and exit high-priority interrupt.
 */
void IRAM_ATTR esp_ipc_isr_stall_other_cpu(void)
{
#if CONFIG_FREERTOS_SMP
    /*
    Temporary workaround to prevent deadlocking on the SMP FreeRTOS kernel lock after stalling the other CPU.
    See IDF-5257
    */
    taskENTER_CRITICAL();
#endif
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
            esp_ipc_isr_finish_cmd = 0;
            esp_ipc_isr_call_and_wait(&esp_ipc_isr_waiting_for_finish_cmd, (void*)&esp_ipc_isr_finish_cmd, IPC_ISR_WAIT_FOR_START);
            return;
        }

        /* Interrupts are already disabled by the parent, we're nested here. */
#if CONFIG_FREERTOS_SMP
        portRESTORE_INTERRUPTS(intLvl);
#else
        portCLEAR_INTERRUPT_MASK_FROM_ISR(intLvl);
#endif
    }
}

void IRAM_ATTR esp_ipc_isr_release_other_cpu(void)
{
    if (s_stall_state == STALL_STATE_RUNNING) {
        const uint32_t cpu_id = xPortGetCoreID();
        if (--s_count_of_nested_calls[cpu_id] == 0) {
            esp_ipc_isr_finish_cmd = 1;
            // Make sure end flag is cleared and esp_ipc_isr_waiting_for_finish_cmd is done.
            while (!esp_ipc_isr_end_fl) {};
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
#if CONFIG_FREERTOS_SMP
    /*
    Temporary workaround to prevent deadlocking on the SMP FreeRTOS kernel lock after stalling the other CPU.
    See IDF-5257
    */
    taskEXIT_CRITICAL();
#endif
}

void IRAM_ATTR esp_ipc_isr_stall_pause(void)
{
    IPC_ISR_ENTER_CRITICAL();
    s_stall_state = STALL_STATE_IDLE;
    IPC_ISR_EXIT_CRITICAL();
}

void IRAM_ATTR esp_ipc_isr_stall_abort(void)
{
    //Note: We don't enter a critical section here as we are calling this from a panic.
    s_stall_state = STALL_STATE_IDLE;
}

void IRAM_ATTR esp_ipc_isr_stall_resume(void)
{
    IPC_ISR_ENTER_CRITICAL();
    s_stall_state = STALL_STATE_RUNNING;
    IPC_ISR_EXIT_CRITICAL();
}

/* End public API functions */

/* Private functions*/

static void IRAM_ATTR esp_ipc_isr_call_and_wait(esp_ipc_isr_func_t func, void* arg, esp_ipc_isr_wait_t wait_for)
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

    if (wait_for == IPC_ISR_WAIT_FOR_START) {
        while (!esp_ipc_isr_start_fl) {};
    } else {
        // IPC_ISR_WAIT_FOR_END
        while (!esp_ipc_isr_end_fl) {};
    }
}

/* End private functions*/
