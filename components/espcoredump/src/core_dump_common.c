/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_memory_layout.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/freertos_debug.h"
#include "esp_rom_sys.h"
#include "esp_core_dump_port.h"
#include "esp_core_dump_common.h"
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

const static char TAG[] __attribute__((unused)) = "esp_core_dump_common";

#if CONFIG_ESP_COREDUMP_ENABLE

#define COREDUMP_GET_MEMORY_SIZE(end, start) (end - start)

/**
 * @brief Memory regions to dump, defined at compile time.
 */
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
#if !CONFIG_IDF_TARGET_ESP32P4
extern int _bss_start;
extern int _bss_end;
extern int _data_start;
extern int _data_end;
#else
extern int _bss_start_low;
extern int _bss_end_low;
extern int _data_start_low;
extern int _data_end_low;
extern int _bss_start_high;
extern int _bss_end_high;
extern int _data_start_high;
extern int _data_end_high;
#endif
#endif

/* Regions for the user defined variable locations */
extern int _coredump_dram_start;
extern int _coredump_dram_end;
extern int _coredump_iram_start;
extern int _coredump_iram_end;
#if SOC_RTC_MEM_SUPPORTED
extern int _coredump_rtc_start;
extern int _coredump_rtc_end;
extern int _coredump_rtc_fast_start;
extern int _coredump_rtc_fast_end;
#endif

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
static core_dump_stack_context_t s_stack_context;

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

    /* watchpoint 1 can be used for task stack overflow detection, reuse it, it is no more necessary */
    //esp_cpu_clear_watchpoint(1);
    //esp_cpu_set_watchpoint(1, s_coredump_stack, 1, ESP_WATCHPOINT_STORE);

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Save the current area we are watching to restore it later */
    esp_hw_stack_guard_get_bounds(xPortGetCoreID(), &s_stack_context.sp_min, &s_stack_context.sp_max);
    /* Since the stack is going to change, make sure we disable protection or an exception would be triggered */
    esp_hw_stack_guard_monitor_stop();
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

    /* Replace the stack pointer depending on the architecture, but save the
     * current stack pointer, in order to be able too restore it later.
     * This function must be inlined. */
    esp_core_dump_replace_sp(s_core_dump_sp, &s_stack_context);
    ESP_COREDUMP_LOGI("Backing up stack @ 0x%" PRIx32 " and use core dump stack @ %p",
                      s_stack_context.sp, esp_cpu_get_sp());

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Re-enable the stack guard to check if the stack is big enough for coredump generation  */
    esp_hw_stack_guard_set_bounds((uint32_t) s_coredump_stack, (uint32_t) s_core_dump_sp);
    esp_hw_stack_guard_monitor_start();
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD
}

/**
 * @brief Calculate how many bytes are free on the stack set up earlier.
 *
 * @return Size, in bytes, of the available space on the stack.
 */
FORCE_INLINE_ATTR uint32_t esp_core_dump_free_stack_space(const uint8_t *pucStackByte)
{
    uint32_t ulCount = 0U;
    while (ulCount < ESP_COREDUMP_STACK_SIZE &&
            *pucStackByte == (uint8_t)COREDUMP_STACK_FILL_BYTE) {
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
#if CONFIG_ESP_COREDUMP_LOGS
    uint32_t bytes_free = esp_core_dump_free_stack_space(s_coredump_stack);
    ESP_COREDUMP_LOGI("Core dump used %" PRIu32 " bytes on stack. %" PRIu32 " bytes left free.",
                      s_core_dump_sp - s_coredump_stack - bytes_free, bytes_free);
#endif

    /* Restore the stack pointer. */
    ESP_COREDUMP_LOGI("Restoring stack @ 0x%" PRIx32, s_stack_context.sp);
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    esp_hw_stack_guard_monitor_stop();
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    esp_core_dump_restore_sp(&s_stack_context);
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Monitor the same stack area that was set before replacing the stack pointer */
    esp_hw_stack_guard_set_bounds(s_stack_context.sp_min, s_stack_context.sp_max);
    esp_hw_stack_guard_monitor_start();
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD
}

#else // CONFIG_ESP_COREDUMP_STACK_SIZE == 0

/* Here, we are not going to use a custom stack for coredump. Make sure the current configuration doesn't require one. */
#if CONFIG_ESP_COREDUMP_USE_STACK_SIZE
#pragma error "CONFIG_ESP_COREDUMP_STACK_SIZE must not be 0 in the current configuration"
#endif // ESP_COREDUMP_USE_STACK_SIZE

FORCE_INLINE_ATTR void esp_core_dump_setup_stack(void)
{
    /* If we are in ISR set watchpoint to the end of ISR stack */
    if (esp_core_dump_in_isr_context()) {
        uint8_t* topStack = esp_core_dump_get_isr_stack_top();
        esp_cpu_clear_watchpoint(1);
        esp_cpu_set_watchpoint(1, topStack + xPortGetCoreID()*configISR_STACK_SIZE, 1, ESP_CPU_WATCHPOINT_STORE);
    } else {
        /* for tasks user should enable stack overflow detection in menuconfig
        TODO: if not enabled in menuconfig enable it ourselves */
    }
}

FORCE_INLINE_ATTR void esp_core_dump_report_stack_usage(void)
{
}

#endif // CONFIG_ESP_COREDUMP_STACK_SIZE > 0

static void* s_exc_frame = NULL;

inline static void esp_core_dump_write_internal(panic_info_t *info)
{
    bool isr_context = esp_core_dump_in_isr_context();

    s_exc_frame = (void *)info->frame;

    esp_core_dump_setup_stack();
    esp_core_dump_port_init(info, isr_context);
    esp_err_t err = esp_core_dump_store();
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Core dump write failed with error=%d", err);
    }
    esp_core_dump_report_stack_usage();
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
    ESP_COREDUMP_LOG_PROCESS("Switched task %p to ISR stack [%" PRIx32 "...%" PRIx32 "]", task->tcb_addr,
                             task->stack_start,
                             task->stack_end);
}

inline void esp_core_dump_reset_tasks_snapshots_iter(void)
{
    esp_core_dump_reset_fake_stacks();
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

    if (!esp_core_dump_in_isr_context() && handle == esp_core_dump_get_current_task_handle()) {
        // Set correct stack top for current task; only modify if we came from the task,
        // and not an ISR that crashed.
        task->stack_start = (uint32_t) s_exc_frame;
    }
    if (!esp_core_dump_check_task(task)) {
        ESP_COREDUMP_LOG_PROCESS("Task %p is broken!", handle);
        return false;
    }
    if (handle == esp_core_dump_get_current_task_handle()) {
        ESP_COREDUMP_LOG_PROCESS("Crashed task %p", handle);
        esp_core_dump_port_set_crashed_tcb((uint32_t)handle);
        if (esp_core_dump_in_isr_context()) {
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
#if SOC_RTC_MEM_SUPPORTED
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_rtc_end, &_coredump_rtc_start) > 0 ? 1 : 0;
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_rtc_fast_end, &_coredump_rtc_fast_start) > 0 ? 1 : 0;
#endif
    total_sz += COREDUMP_GET_MEMORY_SIZE(&_coredump_iram_end, &_coredump_iram_start) > 0 ? 1 : 0;

    return total_sz;
}

static const struct {
    int *start;
    int *end;
} s_memory_sections[COREDUMP_MEMORY_MAX] = {
    [COREDUMP_MEMORY_IRAM] = { &_coredump_iram_start, &_coredump_iram_end },
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
#if !CONFIG_IDF_TARGET_ESP32P4
    [COREDUMP_MEMORY_DRAM_BSS] = { &_bss_start, &_bss_end },
    [COREDUMP_MEMORY_DRAM_DATA] = { &_data_start, &_data_end },
#else
    [COREDUMP_MEMORY_DRAM_BSS] = { &_bss_start_low, &_bss_end_low },
    [COREDUMP_MEMORY_DRAM_DATA] = { &_data_start_low, &_data_end_low },
    [COREDUMP_MEMORY_DRAM_BSS_HIGH] = { &_bss_start_high, &_bss_end_high },
    [COREDUMP_MEMORY_DRAM_DATA_HIGH] = { &_data_start_high, &_data_end_high },
#endif
#else
    [COREDUMP_MEMORY_DRAM] = { &_coredump_dram_start, &_coredump_dram_end },
#endif
#if SOC_RTC_MEM_SUPPORTED
    [COREDUMP_MEMORY_RTC] = { &_coredump_rtc_start, &_coredump_rtc_end },
    [COREDUMP_MEMORY_RTC_FAST] = { &_coredump_rtc_fast_start, &_coredump_rtc_fast_end },
#endif
};

int esp_core_dump_get_user_ram_info(coredump_region_t region, uint32_t *start)
{
    int total_sz = -1;

    ESP_COREDUMP_DEBUG_ASSERT(start != NULL);

    if (region >= COREDUMP_MEMORY_START && region < COREDUMP_MEMORY_MAX) {
        total_sz = (uint8_t *)s_memory_sections[region].end - (uint8_t *)s_memory_sections[region].start;
        *start = (uint32_t)s_memory_sections[region].start;
    }

    return total_sz;
}

#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
void esp_core_dump_get_own_stack_info(uint32_t *addr, uint32_t *size)
{
#if CONFIG_ESP_COREDUMP_STACK_SIZE > 0
    /* Custom stack reserved for the coredump */
    *addr = (uint32_t)s_coredump_stack;
    *size = sizeof(s_coredump_stack);
#else
    /* Shared stack with the crashed task */
    core_dump_task_handle_t handle =  esp_core_dump_get_current_task_handle();
    TaskSnapshot_t rtos_snapshot = { 0 };
    vTaskGetSnapshot(handle, &rtos_snapshot);
    StaticTask_t *current = (StaticTask_t *)handle;
    *addr = (uint32_t)current->pxDummy6; //pxStack
    *size = (uint32_t)rtos_snapshot.pxTopOfStack - (uint32_t)current->pxDummy6; /* free */
#endif
}

#endif /* CONFIG_ESP_COREDUMP_CAPTURE_DRAM */

inline bool esp_core_dump_tcb_addr_is_sane(uint32_t addr)
{
    return esp_core_dump_mem_seg_is_sane(addr, esp_core_dump_get_tcb_len());
}

inline bool esp_core_dump_in_isr_context(void)
{
#if CONFIG_ESP_TASK_WDT_EN
    /* This function will be used to check whether a panic occurred in an ISR.
     * In that case, the execution frame must be switch to the interrupt stack.
     * However, in case where the task watchdog ISR calls the panic handler,
     * `xPortInterruptedFromISRContext` returns true, BUT, we don't want to
     * switch the frame to the ISR context. Thus, check that we are not
     * coming from TWDT ISR. This should be refactored.
     * TODO: IDF-5694. */
    extern bool g_twdt_isr;
    return xPortInterruptedFromISRContext() && !g_twdt_isr;
#else // CONFIG_ESP_TASK_WDT_EN
    return xPortInterruptedFromISRContext();
#endif // CONFIG_ESP_TASK_WDT_EN
}

void esp_core_dump_write(panic_info_t *info)
{
#if CONFIG_ESP_COREDUMP_ENABLE_TO_UART && CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
    return;
#endif

    esp_core_dump_print_write_start();
    esp_core_dump_write_internal(info);
    esp_core_dump_print_write_end();
}

#endif
