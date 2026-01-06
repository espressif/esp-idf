/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "soc/soc_memory_layout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/freertos_debug.h"
#include "esp_rom_sys.h"
#include "esp_core_dump_port.h"
#include "esp_core_dump_common.h"
#include "esp_cpu.h"

const static char TAG[] __attribute__((unused)) = "esp_core_dump_common";

#define COREDUMP_GET_MEMORY_SIZE(end, start) (end - start)

/**
 * @brief Memory regions to dump, defined at compile time.
 */
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
#if CONFIG_ESP32P4_SELECTS_REV_LESS_V3
extern int _bss_start_low;
extern int _bss_end_low;
extern int _data_start_low;
extern int _data_end_low;
extern int _bss_start_high;
extern int _bss_end_high;
extern int _data_start_high;
extern int _data_end_high;
#else
extern int _bss_start;
extern int _bss_end;
extern int _data_start;
extern int _data_end;
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
extern int _coredump_rtc_noinit_start;
extern int _coredump_rtc_noinit_end;
#else
extern int _coredump_noinit_start;
extern int _coredump_noinit_end;
#endif

static void* s_exc_frame = NULL;
static uint32_t s_coredump_sp = 0;

/**
 * @brief Configuration validation: If USE_STACK_SIZE is enabled, STACK_SIZE must be > 0
 */
#if CONFIG_ESP_COREDUMP_USE_STACK_SIZE && CONFIG_ESP_COREDUMP_STACK_SIZE == 0
#error "CONFIG_ESP_COREDUMP_STACK_SIZE must not be 0 when CONFIG_ESP_COREDUMP_USE_STACK_SIZE is enabled"
#endif

/**
 * @brief Write ELF core dump and log any errors.
 *
 * This function wraps esp_core_dump_write_elf() and handles error logging.
 * Can be called from both C and assembly code.
 *
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
void esp_core_dump_write_elf_and_check(void)
{
    esp_err_t err = esp_core_dump_write_elf();
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Core dump write failed with error=%d", err);
    }
}

/**
 * @brief In the menconfig, it is possible to specify a specific stack size for core dump generation.
 */
#if CONFIG_ESP_COREDUMP_STACK_SIZE > 0

/**
 * @brief If stack size has been specified for the core dump generation, create
 * a stack that will be used during the whole core dump generation.
 */
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
/* Increase stack size in verbose mode */
#define ESP_COREDUMP_STACK_SIZE (CONFIG_ESP_COREDUMP_STACK_SIZE + 100)
#else
#define ESP_COREDUMP_STACK_SIZE CONFIG_ESP_COREDUMP_STACK_SIZE
#endif

#define COREDUMP_STACK_FILL_BYTE (0xa5U)

static uint8_t s_coredump_stack[ESP_COREDUMP_STACK_SIZE];

void esp_core_dump_setup_stack(void)
{
    s_coredump_sp = (uint32_t)(s_coredump_stack + ESP_COREDUMP_STACK_SIZE - 1) & ~0xf;
    memset(s_coredump_stack, COREDUMP_STACK_FILL_BYTE, ESP_COREDUMP_STACK_SIZE);

    /* Stack overflow detection. Watchpoint is set to the end of the core dump stack */
    esp_cpu_clear_watchpoint(1);
    esp_cpu_set_watchpoint(1, s_coredump_stack, 1, ESP_CPU_WATCHPOINT_STORE);
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
 * @brief Print how many bytes have been used on the stack to create the core dump.
 *
 */
void esp_core_dump_report_stack_usage(uint32_t new_sp)
{
    uint32_t __attribute__((unused)) bytes_free = esp_core_dump_free_stack_space(s_coredump_stack);
    ESP_COREDUMP_LOGI("Core dump used %" PRIu32 " bytes on stack. %" PRIu32 " bytes left free.",
                      new_sp - (uint32_t)s_coredump_stack - bytes_free, bytes_free);
}

void esp_core_dump_report_backup_stack(uint32_t old_sp)
{
    ESP_COREDUMP_LOGI("Backing up stack @ 0x%" PRIx32 " and use core dump stack @ %p",
                      old_sp, esp_cpu_get_sp());
}

void esp_core_dump_report_restore_stack(uint32_t old_sp)
{
    ESP_COREDUMP_LOGI("Restoring stack @ 0x%" PRIx32, old_sp);
}

#else // CONFIG_ESP_COREDUMP_STACK_SIZE == 0

static uint8_t *s_coredump_stack = NULL;

void esp_core_dump_setup_stack(void) { /* No stack setup is needed */ }

/**
 * @brief Setup watchpoint for stack overflow detection when no custom stack is configured.
 *
 * If we are in ISR context, sets up a watchpoint at the end of the ISR stack.
 * For tasks, stack overflow detection should be enabled in menuconfig.
 * TODO: If not enabled in menuconfig enable it ourselves.
 */
static void esp_core_dump_enable_stack_overflow_detection(void)
{
    if (esp_core_dump_in_isr_context()) {
        uint8_t *topStack = esp_core_dump_get_isr_stack_top();
        esp_cpu_clear_watchpoint(1);
        esp_cpu_set_watchpoint(1, topStack + xPortGetCoreID() * configISR_STACK_SIZE, 1, ESP_CPU_WATCHPOINT_STORE);
    }
}

void esp_core_dump_port_write(uint32_t new_stack, uint32_t new_sp)
{
    (void)new_stack;
    (void)new_sp;

    esp_core_dump_enable_stack_overflow_detection();
    esp_core_dump_write_elf_and_check();
}

#endif // CONFIG_ESP_COREDUMP_STACK_SIZE > 0

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

static const struct {
    int *start;
    int *end;
} s_memory_sections[COREDUMP_MEMORY_MAX] = {
    [COREDUMP_MEMORY_IRAM] = { &_coredump_iram_start, &_coredump_iram_end },
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
#if CONFIG_ESP32P4_SELECTS_REV_LESS_V3
    [COREDUMP_MEMORY_DRAM_BSS] = { &_bss_start_low, &_bss_end_low },
    [COREDUMP_MEMORY_DRAM_DATA] = { &_data_start_low, &_data_end_low },
    [COREDUMP_MEMORY_DRAM_BSS_HIGH] = { &_bss_start_high, &_bss_end_high },
    [COREDUMP_MEMORY_DRAM_DATA_HIGH] = { &_data_start_high, &_data_end_high },
#else
    [COREDUMP_MEMORY_DRAM_BSS] = { &_bss_start, &_bss_end },
    [COREDUMP_MEMORY_DRAM_DATA] = { &_data_start, &_data_end },
#endif
#else
    [COREDUMP_MEMORY_DRAM] = { &_coredump_dram_start, &_coredump_dram_end },
#endif
#if SOC_RTC_MEM_SUPPORTED
    [COREDUMP_MEMORY_RTC] = { &_coredump_rtc_start, &_coredump_rtc_end },
    [COREDUMP_MEMORY_RTC_FAST] = { &_coredump_rtc_fast_start, &_coredump_rtc_fast_end },
    [COREDUMP_MEMORY_NOINIT] = { &_coredump_rtc_noinit_start, &_coredump_rtc_noinit_end },
#else
    [COREDUMP_MEMORY_NOINIT] = { &_coredump_noinit_start, &_coredump_noinit_end },
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

    s_exc_frame = (void *)info->frame;
    esp_core_dump_print_write_start();

    esp_core_dump_port_init(info, esp_core_dump_in_isr_context());
    esp_core_dump_setup_stack();
    esp_core_dump_port_write((uint32_t)s_coredump_stack, s_coredump_sp);

    esp_core_dump_print_write_end();
}
