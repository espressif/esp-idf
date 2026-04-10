/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "esp_err.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "test_panic.h"
#include "test_memprot.h"

#include "sdkconfig.h"
#include "soc/soc_caps.h"

/* Test Utility Functions */

#define BOOT_CMD_MAX_LEN (128)

#define HANDLE_TEST(test_name, name_) \
    if (strcmp(test_name, #name_) == 0) { \
        name_(); \
        die("Test function has returned"); \
    }

static const char* get_test_name(void)
{
    static char test_name_str[BOOT_CMD_MAX_LEN] = {0};
    bool print_prompt = true;

    /* Not using blocking fgets(stdin) here, as QEMU doesn't yet implement RX timeout interrupt,
     * which is required for the UART driver and blocking stdio to work.
     */
    int c = EOF;
    char *p = test_name_str;
    const char *end = test_name_str + sizeof(test_name_str) - 1;
    while (p < end) {
        if (print_prompt) {
            printf("Enter test name: ");
            fflush(stdout);
            print_prompt = false;
        }
        c = getchar();
        if (c == EOF) {
            vTaskDelay(pdMS_TO_TICKS(10));
        } else if (c == '\r' || c == '\n') {
            /* terminate the line */
            puts("\n\r");
            fflush(stdout);
            print_prompt = true;
            if (p != test_name_str) {
                *p = '\0';
                break;
            }
        } else if (c >= '0' && c <= 'z') {
            /* echo the received character */
            putchar(c);
            fflush(stdout);
            /* and save it */
            *p = c;
            ++p;
        }
    }

    return test_name_str;
}

/* app_main */

void app_main(void)
{
    /* Needed to allow the tick hook to set correct INT WDT timeouts */
    vTaskDelay(2);

    /* Test script sends to command over UART. Read it and determine how to proceed. */
    const char* test_name = get_test_name();
    if (test_name == NULL) {
        /* Nothing to do */
        return;
    }
    printf("Got test name: %s\n", test_name);

    HANDLE_TEST(test_name, test_abort);
    HANDLE_TEST(test_name, test_abort_cache_disabled);
    HANDLE_TEST(test_name, test_int_wdt);
    HANDLE_TEST(test_name, test_task_wdt_cpu0);
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    HANDLE_TEST(test_name, test_hw_stack_guard_cpu0);
#if !CONFIG_FREERTOS_UNICORE
    HANDLE_TEST(test_name, test_hw_stack_guard_cpu1);
#endif // CONFIG_FREERTOS_UNICORE
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
    HANDLE_TEST(test_name, test_panic_extram_stack_heap);
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    HANDLE_TEST(test_name, test_panic_extram_stack_bss);
#endif
#endif
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
    HANDLE_TEST(test_name, test_capture_dram);
#endif
#if !CONFIG_FREERTOS_UNICORE
    HANDLE_TEST(test_name, test_task_wdt_cpu1);
    HANDLE_TEST(test_name, test_panic_handler_stuck1);
    HANDLE_TEST(test_name, test_panic_handler_crash1);
#endif
    HANDLE_TEST(test_name, test_loadprohibited);
    HANDLE_TEST(test_name, test_storeprohibited);
    HANDLE_TEST(test_name, test_cache_error);
    HANDLE_TEST(test_name, test_int_wdt_cache_disabled);
    HANDLE_TEST(test_name, test_stack_overflow);
    HANDLE_TEST(test_name, test_illegal_instruction);
    HANDLE_TEST(test_name, test_instr_fetch_prohibited);
    HANDLE_TEST(test_name, test_ub);
    HANDLE_TEST(test_name, test_assert);
    HANDLE_TEST(test_name, test_assert_cache_disabled);
    HANDLE_TEST(test_name, test_assert_cache_write_back_error_can_print_backtrace);
    HANDLE_TEST(test_name, test_tcb_corrupted);
    HANDLE_TEST(test_name, test_panic_handler_stuck0);
    HANDLE_TEST(test_name, test_panic_handler_crash0);
#if CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT
    HANDLE_TEST(test_name, test_panic_halt);
#endif /* CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT */
#if CONFIG_ESP_SYSTEM_USE_FRAME_POINTER
    HANDLE_TEST(test_name, test_panic_print_backtrace);
#endif
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH
    HANDLE_TEST(test_name, test_setup_coredump_summary);
    HANDLE_TEST(test_name, test_coredump_summary);
#endif
#if CONFIG_IDF_TARGET_ESP32
    HANDLE_TEST(test_name, test_illegal_access);
#endif

#if CONFIG_TEST_MEMPROT

    HANDLE_TEST(test_name, test_iram_reg1_write_violation);
    HANDLE_TEST(test_name, test_iram_reg2_write_violation);
    HANDLE_TEST(test_name, test_iram_reg3_write_violation);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    HANDLE_TEST(test_name, test_non_cache_iram_reg1_write_violation);
    HANDLE_TEST(test_name, test_non_cache_iram_reg2_write_violation);
    HANDLE_TEST(test_name, test_non_cache_iram_reg3_write_violation);
    HANDLE_TEST(test_name, test_non_cache_iram_reg4_write_violation);
#endif

    /* TODO: IDF-6820: ESP32-S2 -> Fix incorrect panic reason: Unhandled debug exception */
    HANDLE_TEST(test_name, test_iram_reg4_write_violation);

    /* TODO: IDF-6820: ESP32-S2-> Fix multiple panic reasons in different runs */
    HANDLE_TEST(test_name, test_dram_reg1_execute_violation);

    HANDLE_TEST(test_name, test_dram_reg2_execute_violation);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    HANDLE_TEST(test_name, test_non_cache_dram_reg1_execute_violation);
    HANDLE_TEST(test_name, test_non_cache_dram_reg2_execute_violation);
#endif

#if CONFIG_SOC_RTC_FAST_MEM_SUPPORTED
    HANDLE_TEST(test_name, test_rtc_fast_reg1_execute_violation);
    HANDLE_TEST(test_name, test_rtc_fast_reg2_execute_violation);

    /* TODO: IDF-6820: ESP32-S2-> Fix multiple panic reasons in different runs */
    HANDLE_TEST(test_name, test_rtc_fast_reg3_execute_violation);
#endif

#if SOC_DCACHE_SUPPORTED
    HANDLE_TEST(test_name, test_dcache_read_violation);

    /* TODO: IDF-6820: ESP32-S2-> Fix multiple panic reasons in different runs */
    HANDLE_TEST(test_name, test_dcache_write_violation);
#endif

#if CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED
    HANDLE_TEST(test_name, test_rtc_slow_reg1_execute_violation);
    HANDLE_TEST(test_name, test_rtc_slow_reg2_execute_violation);
#endif

#if CONFIG_ESP_SYSTEM_MEMPROT
    HANDLE_TEST(test_name, test_irom_reg_write_violation);
    HANDLE_TEST(test_name, test_drom_reg_write_violation);
    HANDLE_TEST(test_name, test_drom_reg_execute_violation);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    HANDLE_TEST(test_name, test_non_cache_irom_reg_write_violation);
    HANDLE_TEST(test_name, test_non_cache_drom_reg_write_violation);
    HANDLE_TEST(test_name, test_non_cache_drom_reg_execute_violation);
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && SOC_MMU_DI_VADDR_SHARED
    HANDLE_TEST(test_name, test_spiram_xip_irom_alignment_reg_execute_violation);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    HANDLE_TEST(test_name, test_non_cache_spiram_xip_irom_alignment_reg_execute_violation);
#endif
#endif
#endif

#if CONFIG_SPIRAM_RODATA && !CONFIG_IDF_TARGET_ESP32S2
    HANDLE_TEST(test_name, test_spiram_xip_drom_alignment_reg_execute_violation);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    HANDLE_TEST(test_name, test_non_cache_spiram_xip_drom_alignment_reg_execute_violation);
#endif
#endif

#ifdef CONFIG_SOC_CPU_HAS_PMA
    HANDLE_TEST(test_name, test_invalid_memory_region_write_violation);
    HANDLE_TEST(test_name, test_invalid_memory_region_execute_violation);
#endif
#endif

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY && CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    HANDLE_TEST(test_name, test_panic_extram_attr);
#endif

    die("Unknown test name");
}
