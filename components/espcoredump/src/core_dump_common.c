// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_memory_layout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task_snapshot.h"
#include "esp_rom_sys.h"
#include "esp_core_dump_port.h"
#include "esp_core_dump_common.h"
#include "core_dump_elf.h"
#include "core_dump_binary.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_common";

#if CONFIG_ESP_COREDUMP_ENABLE

#define COREDUMP_GET_MEMORY_SIZE(end, start) (end - start)

/**
 * @brief Memory regions to dump, defined at compile time.
 */
extern int _coredump_dram_start;
extern int _coredump_dram_end;
extern int _coredump_iram_start;
extern int _coredump_iram_end;
extern int _coredump_rtc_start;
extern int _coredump_rtc_end;
extern int _coredump_rtc_fast_start;
extern int _coredump_rtc_fast_end;

/**
 * @brief In the menconfig, it is possible to specify a specific stack size for
 * core dump generation.
 */
#if CONFIG_ESP_COREDUMP_STACK_SIZE > 0

/**
 * @brief If stack size has been specified for the core dump generation, create
 * a stack that will be used during the whole core dump generation.
 */
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
    /* Increase stack size in verbose mode */
    #define ESP_COREDUMP_STACK_SIZE (CONFIG_ESP_COREDUMP_STACK_SIZE+100)
#else
    #define ESP_COREDUMP_STACK_SIZE CONFIG_ESP_COREDUMP_STACK_SIZE
#endif


#define COREDUMP_STACK_FILL_BYTE (0xa5U)

static uint8_t s_coredump_stack[ESP_COREDUMP_STACK_SIZE];
static uint8_t* s_core_dump_sp = NULL;
static uint8_t* s_core_dump_backup = NULL;

/**
 * @brief Function setting up the core dump stack.
 *
 * @note This function **must** be aligned as it modifies the
 * stack pointer register.
 */
FORCE_INLINE_ATTR void esp_core_dump_setup_stack(void)
{
    s_core_dump_sp = (uint8_t *)((uint32_t)(s_coredump_stack + ESP_COREDUMP_STACK_SIZE - 1) & ~0xf);
    memset(s_coredump_stack, COREDUMP_STACK_FILL_BYTE, ESP_COREDUMP_STACK_SIZE);

    /* watchpoint 1 can be used for task stack overflow detection, re-use it, it is no more necessary */
	//esp_cpu_clear_watchpoint(1);
	//esp_cpu_set_watchpoint(1, s_coredump_stack, 1, ESP_WATCHPOINT_STORE);

    /* Replace the stack pointer depending on the architecture, but save the
     * current stack pointer, in order to be able too restore it later.
     * This function must be inlined. */
    s_core_dump_backup = esp_core_dump_replace_sp(s_core_dump_sp);
    ESP_COREDUMP_LOGI("Backing up stack @ %p and use core dump stack @ %p",
                      s_core_dump_backup, esp_cpu_get_sp());
}

/**
 * @brief Calculate how many bytes are free on the stack set up earlier.
 *
 * @return Size, in bytes, of the available space on the stack.
 */
FORCE_INLINE_ATTR uint32_t esp_core_dump_free_stack_space(const uint8_t *pucStackByte)
{
    uint32_t ulCount = 0U;
    while ( ulCount < ESP_COREDUMP_STACK_SIZE &&
           *pucStackByte == (uint8_t)COREDUMP_STACK_FILL_BYTE )
    {
        pucStackByte -= portSTACK_GROWTH;
        ulCount++;
    }
    ulCount /= sizeof(uint8_t);
    return ulCount;
}

/**
 * @brief Print how many bytes have been used on the stack to create the core
 * dump.
 */
FORCE_INLINE_ATTR void esp_core_dump_report_stack_usage(void)
{
    uint32_t bytes_free = esp_core_dump_free_stack_space(s_coredump_stack);
    ESP_COREDUMP_LOGI("Core dump used %u bytes on stack. %u bytes left free.",
        s_core_dump_sp - s_coredump_stack - bytes_free, bytes_free);

    /* Restore the stack pointer. */
    ESP_COREDUMP_LOGI("Restoring stack @ %p", s_core_dump_backup);
    esp_core_dump_replace_sp(s_core_dump_backup);
}

#else
FORCE_INLINE_ATTR void esp_core_dump_setup_stack(void)
{
    /* If we are in ISR set watchpoint to the end of ISR stack */
    if (esp_core_dump_in_isr_context()) {
        uint8_t* topStack = esp_core_dump_get_isr_stack_top();
        esp_cpu_clear_watchpoint(1);
        esp_cpu_set_watchpoint(1, topStack+xPortGetCoreID()*configISR_STACK_SIZE, 1, ESP_WATCHPOINT_STORE);
    } else {
        /* for tasks user should enable stack overflow detection in menuconfig
        TODO: if not enabled in menuconfig enable it ourselves */
    }
}


FORCE_INLINE_ATTR void esp_core_dump_report_stack_usage(void)
{
}
#endif

static void* s_exc_frame = NULL;

inline void esp_core_dump_write(panic_info_t *info, core_dump_write_config_t *write_cfg)
{
#ifndef CONFIG_ESP_ENABLE_COREDUMP_TO_NONE
    esp_err_t err = ESP_ERR_NOT_SUPPORTED;
    s_exc_frame = (void*) info->frame;

    esp_core_dump_setup_stack();
    esp_core_dump_port_init(info);
#if CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN
    err = esp_core_dump_write_binary(write_cfg);
#elif CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    err = esp_core_dump_write_elf(write_cfg);
#endif
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Core dump write binary failed with error=%d", err);
    }
    esp_core_dump_report_stack_usage();
#endif
}

void __attribute__((weak)) esp_core_dump_init(void)
{
    /* do nothing by default */
}

/**
 * Common functions related to core dump generation.
 */
static void esp_core_dump_switch_task_stack_to_isr(core_dump_task_header_t *task,
                                                   core_dump_mem_seg_header_t *stack)
{
    if (stack != NULL) {
        stack->start = task->stack_start;
        stack->size = esp_core_dump_get_memory_len(task->stack_start, task->stack_end);
    }
    task->stack_start = (uint32_t) s_exc_frame;
    task->stack_end = esp_core_dump_get_isr_stack_end();
    ESP_COREDUMP_LOG_PROCESS("Switched task %x to ISR stack [%x...%x]", task->tcb_addr,
                                                                        task->stack_start,
                                                                        task->stack_end);
}

inline void esp_core_dump_reset_tasks_snapshots_iter(void)
{
    esp_core_dump_reset_fake_stacks();
}

inline void *esp_core_dump_get_next_task(void *handle)
{
    return pxTaskGetNext(handle);
}

bool esp_core_dump_get_task_snapshot(void *handle, core_dump_task_header_t *task,
                                     core_dump_mem_seg_header_t *interrupted_stack)
{
    TaskSnapshot_t rtos_snapshot = { 0 };

    if (interrupted_stack != NULL) {
        interrupted_stack->size = 0;
    }

    vTaskGetSnapshot(handle, &rtos_snapshot);
    task->tcb_addr = handle;
    task->stack_start = (uint32_t)rtos_snapshot.pxTopOfStack;
    task->stack_end = (uint32_t)rtos_snapshot.pxEndOfStack;

    if (!xPortInterruptedFromISRContext() && handle == esp_core_dump_get_current_task_handle()) {
        // Set correct stack top for current task; only modify if we came from the task,
        // and not an ISR that crashed.
        task->stack_start = (uint32_t) s_exc_frame;
    }
    if (!esp_core_dump_check_task(task)) {
        ESP_COREDUMP_LOG_PROCESS("Task %x is broken!", handle);
        return false;
    }
    if (handle == esp_core_dump_get_current_task_handle()) {
        ESP_COREDUMP_LOG_PROCESS("Crashed task %x", handle);
        esp_core_dump_port_set_crashed_tcb((uint32_t)handle);
        if (xPortInterruptedFromISRContext()) {
            esp_core_dump_switch_task_stack_to_isr(task, interrupted_stack);
        }
    }
    return true;
}

uint32_t esp_core_dump_get_user_ram_segments(void)
{
    uint32_t total_sz = 0;

    // count number of memory segments to insert into ELF structure
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_dram_end, &_coredump_dram_start) > 0 ? 1 : 0;
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_rtc_end, &_coredump_rtc_start) > 0 ? 1 : 0;
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_rtc_fast_end, &_coredump_rtc_fast_start) > 0 ? 1 : 0;
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_iram_end, &_coredump_iram_start) > 0 ? 1 : 0;

    return total_sz;
}

uint32_t esp_core_dump_get_user_ram_size(void)
{
    uint32_t total_sz = 0;

    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_dram_end, &_coredump_dram_start);
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_rtc_end, &_coredump_rtc_start);
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_rtc_fast_end, &_coredump_rtc_fast_start);
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_iram_end, &_coredump_iram_start);

    return total_sz;
}

int esp_core_dump_get_user_ram_info(coredump_region_t region, uint32_t *start)
{
    int total_sz = -1;

    ESP_COREDUMP_DEBUG_ASSERT(start != NULL);

    switch (region) {
        case COREDUMP_MEMORY_DRAM:
            *start = (uint32_t)&_coredump_dram_start;
            total_sz = (uint8_t *)&_coredump_dram_end - (uint8_t *)&_coredump_dram_start;
            break;

        case COREDUMP_MEMORY_IRAM:
            *start = (uint32_t)&_coredump_iram_start;
            total_sz = (uint8_t *)&_coredump_iram_end - (uint8_t *)&_coredump_iram_start;
            break;

        case COREDUMP_MEMORY_RTC:
            *start = (uint32_t)&_coredump_rtc_start;
            total_sz = (uint8_t *)&_coredump_rtc_end - (uint8_t *)&_coredump_rtc_start;
            break;

        case COREDUMP_MEMORY_RTC_FAST:
            *start = (uint32_t)&_coredump_rtc_fast_start;
            total_sz = (uint8_t *)&_coredump_rtc_fast_end - (uint8_t *)&_coredump_rtc_fast_start;
            break;

        default:
            break;
    }

    return total_sz;
}

inline bool esp_core_dump_tcb_addr_is_sane(uint32_t addr)
{
    return esp_core_dump_mem_seg_is_sane(addr, esp_core_dump_get_tcb_len());
}

inline bool esp_core_dump_in_isr_context(void)
{
    return xPortInterruptedFromISRContext();
}

inline core_dump_task_handle_t esp_core_dump_get_current_task_handle()
{
    return (core_dump_task_handle_t) xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID());
}

#endif
