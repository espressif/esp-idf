/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "soc/cpu.h"
#include "soc/soc.h"
#include "soc/dport_access.h"
#ifdef CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#else
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "esp_intr_alloc.h"
#include "esp_ipc_isr.h"
#include "xtensa/core-macros.h"
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
static int32_t volatile s_count_of_nested_calls[portNUM_PROCESSORS] = { 0 };
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

static void esp_ipc_isr_init_cpu(void* arg)
{
    (void) arg;
    const uint32_t cpuid = xPortGetCoreID();
    uint32_t intr_source = ETS_FROM_CPU_INTR2_SOURCE + cpuid; // ETS_FROM_CPU_INTR2_SOURCE and ETS_FROM_CPU_INTR3_SOURCE
    ESP_INTR_DISABLE(ETS_IPC_ISR_INUM);
    intr_matrix_set(cpuid, intr_source, ETS_IPC_ISR_INUM);
    ESP_INTR_ENABLE(ETS_IPC_ISR_INUM);

    /* If this fails then the minimum stack size for this config is too close to running out */
    assert(uxTaskGetStackHighWaterMark(NULL) > 128);

    if (cpuid != 0) {
        s_stall_state = STALL_STATE_RUNNING;
    }
    vTaskDelete(NULL);
}

void esp_ipc_isr_init(void)
{
    for (unsigned i = 0; i < portNUM_PROCESSORS; ++i) {
        portBASE_TYPE res = xTaskCreatePinnedToCore(esp_ipc_isr_init_cpu, "ipc_isr_init", configMINIMAL_STACK_SIZE, NULL, 5, NULL, i);
        assert(res == pdTRUE);
        (void)res;
    }
}

/* End initializing IPC_ISR */


/* Public API functions */

void IRAM_ATTR esp_ipc_isr_asm_call(esp_ipc_isr_func_t func, void* arg)
{
    IPC_ISR_ENTER_CRITICAL();
    esp_ipc_isr_call_and_wait(func, arg, IPC_ISR_WAIT_FOR_START);
    IPC_ISR_EXIT_CRITICAL();
}

void IRAM_ATTR esp_ipc_isr_asm_call_blocking(esp_ipc_isr_func_t func, void* arg)
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
    if (s_stall_state == STALL_STATE_RUNNING) {
        BaseType_t intLvl = portSET_INTERRUPT_MASK_FROM_ISR();
        const uint32_t cpu_id = xPortGetCoreID();
        if (s_count_of_nested_calls[cpu_id]++ == 0) {
            IPC_ISR_ENTER_CRITICAL();
            s_stored_interrupt_level = intLvl;
            esp_ipc_isr_finish_cmd = 0;
            esp_ipc_isr_call_and_wait(&esp_ipc_isr_waiting_for_finish_cmd, (void*)&esp_ipc_isr_finish_cmd, IPC_ISR_WAIT_FOR_START);
            return;
        }

        /* Interrupts are already disabled by the parent, we're nested here. */
        portCLEAR_INTERRUPT_MASK_FROM_ISR(intLvl);
    }
}

void IRAM_ATTR esp_ipc_isr_release_other_cpu(void)
{
    if (s_stall_state == STALL_STATE_RUNNING) {
        const uint32_t cpu_id = xPortGetCoreID();
        if (--s_count_of_nested_calls[cpu_id] == 0) {
            esp_ipc_isr_finish_cmd = 1;
            IPC_ISR_EXIT_CRITICAL();
            portCLEAR_INTERRUPT_MASK_FROM_ISR(s_stored_interrupt_level);
        } else if (s_count_of_nested_calls[cpu_id] < 0) {
            assert(0);
        }
    }
}

void IRAM_ATTR esp_ipc_isr_stall_pause(void)
{
    IPC_ISR_ENTER_CRITICAL();
    s_stall_state = STALL_STATE_IDLE;
    IPC_ISR_EXIT_CRITICAL();
}

void IRAM_ATTR esp_ipc_isr_stall_abort(void)
{
    s_stall_state = STALL_STATE_IDLE;
}

void IRAM_ATTR esp_ipc_isr_stall_resume(void)
{
    IPC_ISR_ENTER_CRITICAL();
    s_stall_state = STALL_STATE_RUNNING;
    IPC_ISR_EXIT_CRITICAL();
}

void esp_dport_access_stall_other_cpu_start(void) __attribute__((alias("esp_ipc_isr_stall_other_cpu")));
void esp_dport_access_stall_other_cpu_end(void) __attribute__((alias("esp_ipc_isr_release_other_cpu")));
void esp_dport_access_int_pause(void) __attribute__((alias("esp_ipc_isr_stall_pause")));
void esp_dport_access_int_abort(void) __attribute__((alias("esp_ipc_isr_stall_abort")));
void esp_dport_access_int_resume(void) __attribute__((alias("esp_ipc_isr_stall_resume")));

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

    if (cpu_id == 0) {
        // it runs an interrupt on cpu1
        DPORT_REG_WRITE(SYSTEM_CPU_INTR_FROM_CPU_3_REG, SYSTEM_CPU_INTR_FROM_CPU_3);
    } else {
        // it runs an interrupt on cpu0
        DPORT_REG_WRITE(SYSTEM_CPU_INTR_FROM_CPU_2_REG, SYSTEM_CPU_INTR_FROM_CPU_2);
    }

    // IPC_ISR handler will be called and `...isr_start` and `...isr_end` will be updated there

    if (wait_for == IPC_ISR_WAIT_FOR_START) {
        while (!esp_ipc_isr_start_fl) {};
    } else {
        // IPC_ISR_WAIT_FOR_END
        while (!esp_ipc_isr_end_fl) {};
    }
}

/* End private functions*/
