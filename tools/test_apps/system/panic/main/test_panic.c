/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "esp_partition.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "esp_private/cache_utils.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Test utility function */

extern void esp_restart_noos(void) __attribute__ ((noreturn));

void die(const char* msg)
{
    printf("Test error: %s\n\n", msg);
    fflush(stdout);
    usleep(1000);
    /* Don't use abort here as it would enter the panic handler */
    esp_restart_noos();
}

/* implementations of the test functions */

void test_abort(void)
{
    abort();
}

void IRAM_ATTR test_abort_cache_disabled(void)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    abort();
}

void test_int_wdt(void)
{
    portDISABLE_INTERRUPTS();
    while (true) {
        ;
    }
}

void test_task_wdt_cpu0(void)
{
    while (true) {
        ;
    }
}

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY

static void stack_in_extram(void* arg) {
    (void) arg;
    /* Abort instead of using a load/store prohibited to prevent a sanitize error */
    abort();
}

void test_panic_extram_stack(void) {
    /* Start by initializing a Task which has a stack in external RAM */
    StaticTask_t handle;
    const uint32_t stack_size = 8192;
    void* stack = heap_caps_malloc(stack_size, MALLOC_CAP_SPIRAM);

    /* Make sure the stack is in external RAM */
    if (!esp_ptr_external_ram(stack)) {
        die("Allocated stack is not in external RAM!\n");
    }

    xTaskCreateStatic(stack_in_extram, "Task_stack_extram", stack_size, NULL, 4, (StackType_t*) stack, &handle);

    vTaskDelay(1000);
}


#endif // ESP_COREDUMP_ENABLE_TO_FLASH && SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY


#if !CONFIG_FREERTOS_UNICORE
static void infinite_loop(void* arg) {
    (void) arg;
    while(1) {
        ;
    }
}

void test_task_wdt_cpu1(void)
{
    xTaskCreatePinnedToCore(infinite_loop, "Infinite loop", 1024, NULL, 1, NULL, 1);
    while (true) {
        vTaskDelay(1);
    }
}

void test_task_wdt_both_cpus(void)
{
    xTaskCreatePinnedToCore(infinite_loop, "Infinite loop", 1024, NULL, 4, NULL, 1);
    /* Give some time to the task on CPU 1 to be scheduled */
    vTaskDelay(1);
    xTaskCreatePinnedToCore(infinite_loop, "Infinite loop", 1024, NULL, 4, NULL, 0);
    while (true) {
        ;
    }
}
#endif

void __attribute__((no_sanitize_undefined)) test_storeprohibited(void)
{
    *(int*) 0x1 = 0;
}

void IRAM_ATTR test_cache_error(void)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    die("this should not be printed");
}

void IRAM_ATTR test_int_wdt_cache_disabled(void)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    portDISABLE_INTERRUPTS();
    while (true) {
        ;
    }
}

void test_assert(void)
{
    assert(0);
}

void IRAM_ATTR test_assert_cache_disabled(void)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    assert(0);
}

/**
 * This function overwrites the stack beginning from the valid area continuously towards and beyond
 * the end of the stack (stack base) of the current task.
 * This is to test stack protection measures like a watchpoint at the end of the stack.
 *
 * @note: This test DOES NOT write beyond the stack limit. It only writes up to exactly the limit itself.
 *        The FreeRTOS stack protection mechanisms all trigger shortly before the end of the stack.
 */
void test_stack_overflow(void)
{
    register uint32_t* sp asm("sp");
    TaskStatus_t pxTaskStatus;
    vTaskGetInfo(NULL, &pxTaskStatus, pdFALSE, pdFALSE);
    uint32_t *end = (uint32_t*) pxTaskStatus.pxStackBase;

    // offset - 20 bytes from SP in order to not corrupt the current frame.
    // Need to write from higher to lower addresses since the stack grows downwards and the watchpoint/canary is near
    // the end of the stack (lowest address).
    for (uint32_t* ptr = sp - 5; ptr != end; --ptr) {
        *ptr = 0;
    }

    // trigger a context switch to initiate checking the FreeRTOS stack canary
    vTaskDelay(pdMS_TO_TICKS(0));
}

void test_illegal_instruction(void)
{
#if __XTENSA__
    __asm__ __volatile__("ill");
#elif __riscv
    __asm__ __volatile__("unimp");
#endif
}

void test_instr_fetch_prohibited(void)
{
    typedef void (*fptr_t)(void);
    volatile fptr_t fptr = (fptr_t) 0x4;
    fptr();
}

void test_ub(void)
{
    uint8_t stuff[1] = {rand()};
    printf("%d\n", stuff[rand()]);
}
