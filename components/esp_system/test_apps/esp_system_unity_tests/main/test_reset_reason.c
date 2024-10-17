/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <inttypes.h>
#include "unity.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "soc/rtc.h"
#include "hal/wdt_hal.h"
#if CONFIG_IDF_TARGET_ARCH_RISCV
#include "riscv/rv_utils.h"
#endif
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "hal/cache_ll.h"
#endif

#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))

#define CHECK_VALUE 0x89abcdef

// TODO: IDF-9564
#if (CONFIG_SOC_RTC_FAST_MEM_SUPPORTED || CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED) && !CONFIG_IDF_TARGET_ESP32P4
#define CHECK_RTC_MEM 1
#endif //CONFIG_SOC_RTC_FAST_MEM_SUPPORTED || CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32P4
#define INT_WDT_HW_ESP_RST  ESP_RST_WDT // On P4 there is only one reset reason for MWDT0/1
#else
#define INT_WDT_HW_ESP_RST  ESP_RST_INT_WDT
#endif // CONFIG_IDF_TARGET_ESP32P4

/* This test needs special test runners: rev1 silicon, and SPI flash with
 * fast start-up time. Otherwise reset reason will be RTCWDT_RESET.
 */
TEST_CASE("reset reason ESP_RST_POWERON", "[reset][ignore]")
{
    TEST_ASSERT_EQUAL(ESP_RST_POWERON, esp_reset_reason());
}

static __NOINIT_ATTR uint32_t s_noinit_val;

#if CHECK_RTC_MEM
static RTC_NOINIT_ATTR uint32_t s_rtc_noinit_val;
static RTC_DATA_ATTR uint32_t s_rtc_data_val;
static RTC_BSS_ATTR uint32_t s_rtc_bss_val;
/* There is no practical difference between placing something into RTC_DATA and
 * RTC_RODATA. This only checks a usage pattern where the variable has a non-zero
 * initializer (should be initialized by the bootloader).
 */
static RTC_RODATA_ATTR uint32_t s_rtc_rodata_val = CHECK_VALUE;
static RTC_FAST_ATTR uint32_t s_rtc_force_fast_val;
static RTC_SLOW_ATTR uint32_t s_rtc_force_slow_val;
#endif //CHECK_RTC_MEM

static void setup_values(void)
{
    s_noinit_val = CHECK_VALUE;
#if CHECK_RTC_MEM
    s_rtc_noinit_val = CHECK_VALUE;
    s_rtc_data_val = CHECK_VALUE;
    s_rtc_bss_val = CHECK_VALUE;
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(CHECK_VALUE, s_rtc_rodata_val,
                                    "s_rtc_rodata_val should already be set up");
    s_rtc_force_fast_val = CHECK_VALUE;
    s_rtc_force_slow_val = CHECK_VALUE;
#endif //CHECK_RTC_MEM

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* If internal data is behind a cache it might not be written to the physical memory when we crash
       force a full writeback here to ensure this */
    cache_ll_writeback_all(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA, CACHE_LL_ID_ALL);
#endif
}

#if SOC_DEEP_SLEEP_SUPPORTED
static void do_deep_sleep(void)
{
    setup_values();
    esp_sleep_enable_timer_wakeup(10000);
    esp_deep_sleep_start();
}

static void check_reset_reason_deep_sleep(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());

#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_force_slow_val);
#endif //CHECK_RTC_MEM

}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_DEEPSLEEP", "[reset_reason]",
                          do_deep_sleep,
                          check_reset_reason_deep_sleep);

#endif // SOC_DEEP_SLEEP_SUPPORTED

static void do_exception(void)
{
    setup_values();
    *(int*)(0x0) = 0;
}

static void do_abort(void)
{
    setup_values();
    abort();
}

static void check_reset_reason_panic(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
#endif //CHECK_RTC_MEM
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_PANIC after exception", "[reset_reason]",
                          do_exception,
                          check_reset_reason_panic);

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_PANIC after abort", "[reset_reason]",
                          do_abort,
                          check_reset_reason_panic);

static void do_restart(void)
{
    setup_values();
    esp_restart();
}

#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
static void do_restart_from_app_cpu(void)
{
    setup_values();
    xTaskCreatePinnedToCore((TaskFunction_t) &do_restart, "restart", 2048, NULL, 5, NULL, 1);
    vTaskDelay(2);
}
#endif

static void check_reset_reason_sw(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
#endif //CHECK_RTC_MEM
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_SW after restart", "[reset_reason]",
                          do_restart,
                          check_reset_reason_sw);

#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_SW after restart from APP CPU", "[reset_reason]",
                          do_restart_from_app_cpu,
                          check_reset_reason_sw);
#endif

static void do_int_wdt(void)
{
    setup_values();
#if CONFIG_FREERTOS_SMP
    BaseType_t prev_level = portDISABLE_INTERRUPTS();
#else
    BaseType_t prev_level = portSET_INTERRUPT_MASK_FROM_ISR();
#endif
    (void) prev_level;
    while (1);
}

static void do_int_wdt_hw(void)
{
    setup_values();
#if CONFIG_IDF_TARGET_ARCH_RISCV
    rv_utils_intr_global_disable();
#else
    XTOS_SET_INTLEVEL(XCHAL_NMILEVEL);
#endif
    while (1) { }
}

static void check_reset_reason_int_wdt_sw(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_INT_WDT, esp_reset_reason());
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
#endif //CHECK_RTC_MEM
}

static void check_reset_reason_int_wdt_hw(void)
{
    TEST_ASSERT_EQUAL(INT_WDT_HW_ESP_RST, esp_reset_reason());
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
#endif //CHECK_RTC_MEM
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_INT_WDT after interrupt watchdog (panic)",
                          "[reset_reason]",
                          do_int_wdt,
                          check_reset_reason_int_wdt_sw);

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_INT_WDT after interrupt watchdog (hw)",
                          "[reset_reason]",
                          do_int_wdt_hw,
                          check_reset_reason_int_wdt_hw);

#if CONFIG_ESP_TASK_WDT_EN
static void do_task_wdt(void)
{
    setup_values();
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 1000,
        .idle_core_mask = (1 << 0), // Watch core 0 idle
        .trigger_panic = true,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    while (1);
}

static void check_reset_reason_task_wdt(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_TASK_WDT, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
#endif //CHECK_RTC_MEM
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_TASK_WDT after task watchdog",
                          "[reset_reason]",
                          do_task_wdt,
                          check_reset_reason_task_wdt);
#endif // CONFIG_ESP_TASK_WDT_EN

static void do_rtc_wdt(void)
{
    setup_values();
    // Enable RTC watchdog for 0.1 second
    wdt_hal_context_t rtc_wdt_ctx;
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = rtc_clk_slow_freq_get_hz() / 10;
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, true);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    while (1);
}

static void check_reset_reason_any_wdt(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_WDT, esp_reset_reason());
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
#endif //CHECK_RTC_MEM
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_WDT after RTC watchdog",
                          "[reset_reason]",
                          do_rtc_wdt,
                          check_reset_reason_any_wdt);

static void do_brownout(void)
{
    setup_values();
    printf("Manual test: lower the supply voltage to cause brownout\n");
    vTaskSuspend(NULL);
}

static void check_reset_reason_brownout(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_BROWNOUT, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
#if CHECK_RTC_MEM
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
#endif //CHECK_RTC_MEM
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_BROWNOUT after brownout event",
                          "[reset_reason][ignore]",
                          do_brownout,
                          check_reset_reason_brownout);

#ifdef CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
#ifndef CONFIG_FREERTOS_UNICORE
#if CONFIG_IDF_TARGET_ARCH_XTENSA
#include "xt_instr_macros.h"
#include "xtensa/config/specreg.h"

static int size_stack = 1024 * 4;
static StackType_t *start_addr_stack;

static int fibonacci(int n, void* func(void))
{
    int tmp1 = n, tmp2 = n;
    uint32_t base, start;
    RSR(WINDOWBASE, base);
    RSR(WINDOWSTART, start);
    printf("WINDOWBASE = %-2"PRIi32"   WINDOWSTART = 0x%"PRIx32"\n", base, start);
    if (n <= 1) {
        StackType_t *last_addr_stack = esp_cpu_get_sp();
        StackType_t *used_stack = (StackType_t *)(start_addr_stack - last_addr_stack);
        printf("addr_stack = %p, used[%p]/all[0x%x] space in stack\n", last_addr_stack, used_stack, size_stack);
        func();
        return n;
    }
    int fib = fibonacci(n - 1, func) + fibonacci(n - 2, func);
    printf("fib = %d\n", (tmp1 - tmp2) + fib);
    return fib;
}

static void test_task(void *func)
{
    start_addr_stack = esp_cpu_get_sp();
    if (esp_ptr_external_ram(start_addr_stack)) {
        printf("restart_task: uses external stack, addr_stack = %p\n", start_addr_stack);
    } else {
        printf("restart_task: uses internal stack, addr_stack = %p\n", start_addr_stack);
    }
    fibonacci(35, func);
}

static void func_do_exception(void)
{
    *((int *) 0) = 0;
}

static void init_restart_task(void)
{
    StackType_t *stack_for_task = (StackType_t *) heap_caps_calloc(1, size_stack, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    printf("init_task: current addr_stack = %p, stack_for_task = %p\n", esp_cpu_get_sp(), stack_for_task);
    static StaticTask_t task_buf;
    xTaskCreateStaticPinnedToCore(test_task, "test_task", size_stack, esp_restart, 5, stack_for_task, &task_buf, 1);
    while (1) { };
}

static void init_task_do_exception(void)
{
    StackType_t *stack_for_task = (StackType_t *) heap_caps_calloc(1, size_stack, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    printf("init_task: current addr_stack = %p, stack_for_task = %p\n", esp_cpu_get_sp(), stack_for_task);
    static StaticTask_t task_buf;
    xTaskCreateStaticPinnedToCore(test_task, "test_task", size_stack, func_do_exception, 5, stack_for_task, &task_buf, 1);
    while (1) { };
}

static void test1_finish(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    printf("test - OK\n");
}

static void test2_finish(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
    printf("test - OK\n");
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_SW after restart in a task with spiram stack", "[spiram_stack]",
                          init_restart_task,
                          test1_finish);

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_PANIC after an exception in a task with spiram stack", "[spiram_stack]",
                          init_task_do_exception,
                          test2_finish);

#endif //CONFIG_IDF_TARGET_ARCH_XTENSA
#endif // CONFIG_FREERTOS_UNICORE
#endif // CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM

/* Not tested here: ESP_RST_SDIO */
