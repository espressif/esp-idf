/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include "esp_partition.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"
#include "esp_core_dump.h"

#include "esp_private/cache_utils.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/mpu_hal.h"
#include "rom/cache.h"

volatile uint32_t g_panic_handler_stuck = 0;
volatile uint32_t g_panic_handler_crash = 0;

/* Test utility function */

extern void esp_restart_noos(void) __attribute__ ((noreturn));

void die(const char* msg)
{
    printf("Test error: %s\n\n", msg);
    fflush(stdout);
    fsync(fileno(stdout));
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

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD

__attribute__((optimize("-O0")))
static void test_hw_stack_guard_cpu(void* arg)
{
    uint32_t buf[256];
    test_hw_stack_guard_cpu(arg);
}

void test_hw_stack_guard_cpu0(void)
{
    xTaskCreatePinnedToCore(test_hw_stack_guard_cpu, "HWSG0", 512, NULL, 1, NULL, 0);
    while (true) {
        vTaskDelay(100);
    }
}

#if !CONFIG_FREERTOS_UNICORE
void test_hw_stack_guard_cpu1(void)
{
    xTaskCreatePinnedToCore(test_hw_stack_guard_cpu, "HWSG1", 512, NULL, 1, NULL, 1);
    while (true) {
        vTaskDelay(100);
    }
}

#endif // CONFIG_FREERTOS_UNICORE
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM

static void stack_in_extram(void* arg) {
    (void) arg;
    /* Abort instead of using a load/store prohibited to prevent a sanitize error */
    abort();
}

void test_panic_extram_stack_heap(void) {
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
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
static EXT_RAM_BSS_ATTR StackType_t stack[8192];
void test_panic_extram_stack_bss(void)
{
    StaticTask_t handle;

    xTaskCreateStatic(stack_in_extram, "Task_stack_extram", sizeof(stack), NULL, 4, stack, &handle);

    vTaskDelay(1000);
}
#endif
#endif // ESP_COREDUMP_ENABLE_TO_FLASH && FREERTOS_TASK_CREATE_ALLOW_EXT_MEM


void __attribute__((no_sanitize_undefined)) test_panic_handler_stuck(void *arg)
{
    g_panic_handler_stuck = 1;

    /* Cause a panic */
#ifdef __XTENSA__
    asm("ill");     // should be an invalid operation on xtensa targets
#elif __riscv
    asm("unimp");   // should be an invalid operation on RISC-V targets
#endif

    vTaskDelete(NULL);
}

void __attribute__((no_sanitize_undefined)) test_panic_handler_crash(void *arg)
{
    g_panic_handler_crash = 1;

    /* Cause a panic */
#ifdef __XTENSA__
    asm("ill");
#elif __riscv
    asm("unimp");
#endif

    vTaskDelete(NULL);
}

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

void test_panic_handler_stuck1(void)
{
    /* Cause the panic on core 1 */
    xTaskCreatePinnedToCore(test_panic_handler_stuck, "panic_handler_stuck", 2048, NULL, 1, NULL, 1);

    while(1) {
        vTaskDelay(10);
    }
}

void test_panic_handler_crash1(void)
{
    /* Cause the panic on core 1 */
    xTaskCreatePinnedToCore(test_panic_handler_crash, "panic_handler_crash", 2048, NULL, 1, NULL, 1);

    while(1) {
        vTaskDelay(10);
    }
}
#endif

void test_panic_handler_stuck0(void)
{
    /* Cause the panic on core 0 */
    xTaskCreatePinnedToCore(test_panic_handler_stuck, "panic_handler_stuck", 2048, NULL, 1, NULL, 0);

    while(1) {
        vTaskDelay(10);
    }
}

void test_panic_handler_crash0(void)
{
    /* Cause the panic on core 0 */
    xTaskCreatePinnedToCore(test_panic_handler_crash, "panic_handler_crash", 2048, NULL, 1, NULL, 0);

    while(1) {
        vTaskDelay(10);
    }
}

void __attribute__((no_sanitize_undefined)) test_storeprohibited(void)
{
    *(int*) 0x4 = 0;
    test_task_wdt_cpu0(); /* Trap for unhandled asynchronous bus errors */
}

void __attribute__((no_sanitize_undefined)) test_loadprohibited(void)
{
    static int __attribute__((used)) var;
    var = *(int*) 0x4;
    test_task_wdt_cpu0(); /* Trap for unhandled asynchronous bus errors */
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

const char TEST_STR[] = "my_tag";
void test_assert_cache_write_back_error_can_print_backtrace(void)
{
    printf("1) %p\n", TEST_STR);
    *(uint32_t*)TEST_STR = 3; // We changed the rodata string.
    // All chips except ESP32S3 stop execution here and raise a LoadStore error on the line above.
#if CONFIG_IDF_TARGET_ESP32S3
    // On the ESP32S3, the error occurs later when the cache writeback is triggered
    // (in this test, a direct call to Cache_WriteBack_All).
    Cache_WriteBack_All(); // Cache writeback triggers the invalid cache access interrupt.
#endif
    // We are testing that the backtrace is printed instead of TG1WDT.
    printf("2) %p\n", TEST_STR); // never get to this place.
}

void test_assert_cache_write_back_error_can_print_backtrace2(void)
{
    printf("1) %p\n", TEST_STR);
    *(uint32_t*)TEST_STR = 3; // We changed the rodata string.
    // All chips except ESP32S3 stop execution here and raise a LoadStore error on the line above.
    // On the ESP32S3, the error occurs later when the cache writeback is triggered
    // (in this test, a large range of DRAM is mapped and read, causing an error).
    uint8_t temp = 0;
    size_t map_size = SPI_FLASH_SEC_SIZE * 512;
    const void *map;
    spi_flash_mmap_handle_t out_handle;
    esp_err_t err = spi_flash_mmap(0, map_size, SPI_FLASH_MMAP_DATA, &map, &out_handle);
    if (err != ESP_OK) {
        printf("spi_flash_mmap failed %x\n", err);
        return;
    }
    const uint8_t *rodata = map;
    for (size_t i = 0; i < map_size; i++) {
        temp = rodata[i];
    }
    // Cache writeback triggers the invalid cache access interrupt.
    // We are testing that the backtrace is printed instead of TG1WDT.
    printf("2) %p 0x%" PRIx8 " \n", TEST_STR, temp); // never get to this place.
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

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
void test_setup_coredump_summary(void)
{
    if (esp_core_dump_image_erase() != ESP_OK)
        die("Coredump image can not be erased!");
    assert(0);
}

void test_coredump_summary(void)
{
    esp_core_dump_summary_t *summary = malloc(sizeof(esp_core_dump_summary_t));
    if (summary) {
        esp_err_t err = esp_core_dump_get_summary(summary);
        if (err == ESP_OK) {
            printf("App ELF file SHA256: %s\n", (char *)summary->app_elf_sha256);
            printf("Crashed task: %s\n", summary->exc_task);
#if __XTENSA__
            printf("Exception cause: %ld\n", summary->ex_info.exc_cause);
#else
            printf("Exception cause: %ld\n", summary->ex_info.mcause);
#endif
            char panic_reason[200];
            err = esp_core_dump_get_panic_reason(panic_reason, sizeof(panic_reason));
            if (err == ESP_OK) {
                printf("Panic reason: %s\n", panic_reason);
            }
        }
        free(summary);
    }
}
#endif

void test_tcb_corrupted(void)
{
    uint32_t volatile *tcb_ptr = (uint32_t *)xTaskGetIdleTaskHandleForCore(0);
    for (size_t i = 0; i < sizeof(StaticTask_t) / sizeof(uint32_t); i++) {
        tcb_ptr[i] = 0xDEADBEE0;
    }
    vTaskDelay(2);
}

/* NOTE: The following test verifies the behaviour for the
 * Xtensa-specific MPU instructions (Refer WDTLB, DSYNC, WDTIB, ISYNC)
 * used for memory protection.
 *
 * However, this test is not valid for S2 and S3, because they have PMS
 * enabled on top of this, giving unpredictable results.
 */
#if CONFIG_IDF_TARGET_ESP32
void test_illegal_access(void)
{
    intptr_t addr = 0x80000000; // MPU region 4
    volatile int __attribute__((unused)) val = INT16_MAX;

    // Marked as an illegal access region at startup in ESP32, ESP32S2.
    // Make accessible temporarily.
    mpu_hal_set_region_access(4, MPU_REGION_RW);

    val = *((int*) addr);
    printf("[1] val: %d at %p\n", val, (void *)addr);

    // Make access to region illegal again.
    mpu_hal_set_region_access(4, MPU_REGION_ILLEGAL);
    val = *((int*) addr);
    // Does not reach here as device resets due to illegal access
    printf("[2] val: %d at %p\n", val, (void *)addr);
}
#endif

#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
int g_data_var = 42;
int g_bss_var;
char *g_heap_ptr;
COREDUMP_IRAM_DATA_ATTR uint32_t g_cd_iram = 0x4242;
COREDUMP_DRAM_ATTR uint32_t g_cd_dram = 0x4343;
#if SOC_RTC_MEM_SUPPORTED
COREDUMP_RTC_FAST_ATTR uint32_t g_rtc_fast_var;
COREDUMP_RTC_DATA_ATTR uint32_t g_rtc_data_var = 0x55A9;
#endif

void test_capture_dram(void)
{
    g_data_var++;
    g_bss_var = 55;
    g_heap_ptr = strdup("Coredump Test");
    assert(g_heap_ptr);
    g_cd_iram++;
    g_cd_dram++;
#if SOC_RTC_MEM_SUPPORTED
    g_rtc_fast_var = 0xAABBCCDD;
    g_rtc_data_var++;
#endif
    assert(0);
}
#endif
