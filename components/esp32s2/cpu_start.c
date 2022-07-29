// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"

#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/rom/uart.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/dport_access.h"
#include "esp32s2/brownout.h"
#include "esp32s2/cache_err_int.h"
#include "esp32s2/spiram.h"
#include "esp32s2/memprot.h"

#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/periph_defs.h"
#include "hal/wdt_hal.h"
#include "driver/rtc_io.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_heap_caps_init.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_flash_internal.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_spi_flash.h"
#include "esp_private/crosscore_int.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_newlib.h"
#include "esp_int_wdt.h"
#include "esp_task.h"
#include "esp_task_wdt.h"
#include "esp_phy_init.h"
#include "esp_coexist_internal.h"
#include "esp_debug_helpers.h"
#include "esp_core_dump.h"
#include "esp_app_trace.h"
#include "esp_private/dbg_stubs.h"
#include "esp_clk_internal.h"
#include "esp_timer.h"
#include "esp_pm.h"
#include "esp_private/pm_impl.h"
#include "trax.h"
#include "esp_ota_ops.h"
#include "esp_efuse.h"
#include "bootloader_mem.h"

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default"))) __attribute__((noreturn));
void start_cpu0_default(void) IRAM_ATTR __attribute__((noreturn));

static void do_global_ctors(void);
static void main_task(void *args);
extern void app_main(void);
extern esp_err_t esp_pthread_init(void);

extern int _bss_start;
extern int _bss_end;
extern int _rtc_bss_start;
extern int _rtc_bss_end;
extern int _init_start;
extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);
extern volatile int port_xSchedulerRunning[2];

static const char *TAG = "cpu_start";

struct object {
    long placeholder[ 10 ];
};
void __register_frame_info (const void *begin, struct object *ob);
extern char __eh_frame[];

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
// workaround for C++ exception large memory allocation
void _Unwind_SetEnableExceptionFdeSorting(unsigned char enable);
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS

//If CONFIG_SPIRAM_IGNORE_NOTFOUND is set and external RAM is not found or errors out on testing, this is set to false.
static bool s_spiram_okay = true;

/*
 * We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
 * and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
 */

void IRAM_ATTR call_start_cpu0(void)
{
    RESET_REASON rst_reas;

    bootloader_init_mem();

    // Move exception vectors to IRAM
    cpu_hal_set_vecbase(&_init_start);

    rst_reas = rtc_get_reset_reason(0);

    // from panic handler we can be reset by RWDT or TG0WDT
    if (rst_reas == RTCWDT_SYS_RESET || rst_reas == TG0WDT_SYS_RESET) {
#ifndef CONFIG_BOOTLOADER_WDT_ENABLE
        wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif
    }

    //Clear BSS. Please do not attempt to do any complex stuff (like early logging) before this.
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

    /* Unless waking from deep sleep (implying RTC memory is intact), clear RTC bss */
    if (rst_reas != DEEPSLEEP_RESET) {
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start) * sizeof(_rtc_bss_start));
    }

    /* Configure the mode of instruction cache : cache size, cache associated ways, cache line size. */
    extern void esp_config_instruction_cache_mode(void);
    esp_config_instruction_cache_mode();

    /* If we need use SPIRAM, we should use data cache, or if we want to access rodata, we also should use data cache.
       Configure the mode of data : cache size, cache associated ways, cache line size.
       Enable data cache, so if we don't use SPIRAM, it just works. */
#if CONFIG_SPIRAM_BOOT_INIT
    extern void esp_config_data_cache_mode(void);
    esp_config_data_cache_mode();
    Cache_Enable_DCache(0);
#endif

    /* In SPIRAM code, we will reconfigure data cache, as well as instruction cache, so that we can:
       1. make data buses works with SPIRAM
       2. make instruction and rodata work with SPIRAM, still through instruction cache */
#if CONFIG_SPIRAM_BOOT_INIT
    if (esp_spiram_init() != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGI(TAG, "Failed to init external RAM; continuing without it.");
        s_spiram_okay = false;
#else
        ESP_EARLY_LOGE(TAG, "Failed to init external RAM!");
        abort();
#endif
    }
    esp_spiram_init_cache();
#endif

    ESP_EARLY_LOGI(TAG, "Pro cpu up.");
    if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {
        const esp_app_desc_t *app_desc = esp_ota_get_app_description();
        ESP_EARLY_LOGI(TAG, "Application information:");
#ifndef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
        ESP_EARLY_LOGI(TAG, "Project name:     %s", app_desc->project_name);
#endif
#ifndef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
        ESP_EARLY_LOGI(TAG, "App version:      %s", app_desc->version);
#endif
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
        ESP_EARLY_LOGI(TAG, "Secure version:   %d", app_desc->secure_version);
#endif
#ifdef CONFIG_APP_COMPILE_TIME_DATE
        ESP_EARLY_LOGI(TAG, "Compile time:     %s %s", app_desc->date, app_desc->time);
#endif
        char buf[17];
        esp_ota_get_app_elf_sha256(buf, sizeof(buf));
        ESP_EARLY_LOGI(TAG, "ELF file SHA256:  %s...", buf);
        ESP_EARLY_LOGI(TAG, "ESP-IDF:          %s", app_desc->idf_ver);
    }
    ESP_EARLY_LOGI(TAG, "Single core mode");

#if CONFIG_SPIRAM_MEMTEST
    if (s_spiram_okay) {
        bool ext_ram_ok = esp_spiram_test();
        if (!ext_ram_ok) {
            ESP_EARLY_LOGE(TAG, "External RAM failed memory test!");
            abort();
        }
    }
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    extern void instruction_flash_page_info_init(void);
    instruction_flash_page_info_init();
#endif
#if CONFIG_SPIRAM_RODATA
    extern void rodata_flash_page_info_init(void);
    rodata_flash_page_info_init();
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    extern void esp_spiram_enable_instruction_access(void);
    esp_spiram_enable_instruction_access();
#endif
#if CONFIG_SPIRAM_RODATA
    extern void esp_spiram_enable_rodata_access(void);
    esp_spiram_enable_rodata_access();
#endif

#if CONFIG_ESP32S2_INSTRUCTION_CACHE_WRAP || CONFIG_ESP32S2_DATA_CACHE_WRAP
    uint32_t icache_wrap_enable = 0, dcache_wrap_enable = 0;
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_WRAP
    icache_wrap_enable = 1;
#endif
#if CONFIG_ESP32S2_DATA_CACHE_WRAP
    dcache_wrap_enable = 1;
#endif
    extern void esp_enable_cache_wrap(uint32_t icache_wrap_enable, uint32_t dcache_wrap_enable);
    esp_enable_cache_wrap(icache_wrap_enable, dcache_wrap_enable);
#endif

    /* Initialize heap allocator */
    heap_caps_init();

    ESP_EARLY_LOGI(TAG, "Pro cpu start user code");
    start_cpu0();
}

static void intr_matrix_clear(void)
{
    //Clear all the interrupt matrix register
    for (int i = ETS_WIFI_MAC_INTR_SOURCE; i < ETS_MAX_INTR_SOURCE; i++) {
        intr_matrix_set(0, i, ETS_INVALID_INUM);
    }
}

void start_cpu0_default(void)
{
    esp_err_t err;
    esp_setup_syscall_table();

    if (s_spiram_okay) {
#if CONFIG_SPIRAM_BOOT_INIT && (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
        esp_err_t r = esp_spiram_add_to_heapalloc();
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "External RAM could not be added to heap!");
            abort();
        }
#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
        r = esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool!");
            abort();
        }
#endif
#if CONFIG_SPIRAM_USE_MALLOC
        heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
#endif
#endif
    }

//Enable trace memory and immediately start trace.
#if CONFIG_ESP32S2_TRAX
    trax_enable(TRAX_ENA_PRO);
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif
    esp_clk_init();
    esp_perip_clk_init();
    intr_matrix_clear();

#ifndef CONFIG_ESP_CONSOLE_UART_NONE
#ifdef CONFIG_PM_ENABLE
    const int uart_clk_freq = REF_CLK_FREQ;
    /* When DFS is enabled, use REFTICK as UART clock source */
    CLEAR_PERI_REG_MASK(UART_CONF0_REG(CONFIG_ESP_CONSOLE_UART_NUM), UART_TICK_REF_ALWAYS_ON);
#else
    const int uart_clk_freq = APB_CLK_FREQ;
#endif // CONFIG_PM_DFS_ENABLE
    uart_div_modify(CONFIG_ESP_CONSOLE_UART_NUM, (uart_clk_freq << 4) / CONFIG_ESP_CONSOLE_UART_BAUDRATE);
#endif // CONFIG_ESP_CONSOLE_UART_NONE

#if CONFIG_ESP32S2_BROWNOUT_DET
    esp_brownout_init();
#endif

    rtc_gpio_force_hold_dis_all();

#ifdef CONFIG_VFS_SUPPORT_IO
    esp_vfs_dev_uart_register();
#endif // CONFIG_VFS_SUPPORT_IO

#if defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_UART_NONE)
    esp_reent_init(_GLOBAL_REENT);
    const char *default_uart_dev = "/dev/uart/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM);
    _GLOBAL_REENT->_stdin  = fopen(default_uart_dev, "r");
    _GLOBAL_REENT->_stdout = fopen(default_uart_dev, "w");
    _GLOBAL_REENT->_stderr = fopen(default_uart_dev, "w");
#else // defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_UART_NONE)
    _REENT_SMALL_CHECK_INIT(_GLOBAL_REENT);
#endif // defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_UART_NONE)
    // After setting _GLOBAL_REENT, ESP_LOGIx can be used instead of ESP_EARLY_LOGx.

#if CONFIG_SECURE_DISABLE_ROM_DL_MODE
    err = esp_efuse_disable_rom_download_mode();
    assert(err == ESP_OK && "Failed to disable ROM download mode");
#endif
#if CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    err = esp_efuse_enable_rom_secure_download_mode();
    assert(err == ESP_OK && "Failed to enable Secure Download mode");
#endif

    esp_timer_init();
    esp_set_time_from_rtc();
#if CONFIG_APPTRACE_ENABLE
    err = esp_apptrace_init();
    assert(err == ESP_OK && "Failed to init apptrace module on PRO CPU!");
#endif
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_SYSVIEW_Conf();
#endif
#if CONFIG_ESP32S2_DEBUG_STUBS_ENABLE
    esp_dbg_stubs_init();
#endif
    err = esp_pthread_init();
    assert(err == ESP_OK && "Failed to init pthread module!");

#if CONFIG_ESP32S2_MEMPROT_FEATURE
#if CONFIG_ESP32S2_MEMPROT_FEATURE_LOCK
    esp_memprot_set_prot(true, true);
#else
    esp_memprot_set_prot(true, false);
#endif
#endif

    do_global_ctors();

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    ESP_EARLY_LOGD(TAG, "Setting C++ exception workarounds.");
    _Unwind_SetEnableExceptionFdeSorting(0);
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS

#if CONFIG_ESP_INT_WDT
    esp_int_wdt_init();
    //Initialize the interrupt watch dog
    esp_int_wdt_cpu_init();
#endif
    esp_cache_err_int_init();
    esp_crosscore_int_init();
    spi_flash_init();
    /* init default OS-aware flash access critical section */
    spi_flash_guard_set(&g_flash_guard_default_ops);

    esp_flash_app_init();
    esp_err_t flash_ret = esp_flash_init_default_chip();
    assert(flash_ret == ESP_OK);

#ifdef CONFIG_PM_ENABLE
    esp_pm_impl_init();
#ifdef CONFIG_PM_DFS_INIT_AUTO
    int xtal_freq = (int) rtc_clk_xtal_freq_get();
    esp_pm_config_esp32s2_t cfg = {
        .max_freq_mhz = CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = xtal_freq,
    };
    esp_pm_configure(&cfg);
#endif //CONFIG_PM_DFS_INIT_AUTO
#endif //CONFIG_PM_ENABLE

#if CONFIG_ESP32_ENABLE_COREDUMP
    esp_core_dump_init();
#endif

    portBASE_TYPE res = xTaskCreatePinnedToCore(&main_task, "main",
                        ESP_TASK_MAIN_STACK, NULL,
                        ESP_TASK_MAIN_PRIO, NULL, 0);
    assert(res == pdTRUE);

    ESP_LOGI(TAG, "Starting scheduler on PRO CPU.");
    vTaskStartScheduler();
    abort(); /* Only get to here if not enough free heap to start scheduler */
}

/**
 * This function overwrites a the same function of libsupc++ (part of libstdc++).
 * Consequently, libsupc++ will then follow our configured exception emergency pool size.
 *
 * It will be called even with -fno-exception for user code since the stdlib still uses exceptions.
 */
size_t __cxx_eh_arena_size_get(void)
{
#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    return CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE;
#else
    return 0;
#endif
}

static void do_global_ctors(void)
{
#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    static struct object ob;
    __register_frame_info( __eh_frame, &ob );
#endif

    void (**p)(void);
    for (p = &__init_array_end - 1; p >= &__init_array_start; --p) {
        (*p)();
    }
}

static void main_task(void *args)
{
    //Enable allocation in region where the startup stacks were located.
    heap_caps_enable_nonos_stack_heaps();

    //Initialize task wdt if configured to do so
#ifdef CONFIG_ESP_TASK_WDT_PANIC
    ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, true));
#elif CONFIG_ESP_TASK_WDT
    ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, false));
#endif

    //Add IDLE 0 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    if (idle_0 != NULL) {
        ESP_ERROR_CHECK(esp_task_wdt_add(idle_0));
    }
#endif

    // Now that the application is about to start, disable boot watchdog
#ifndef CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE
    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif

#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    const esp_partition_t *efuse_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_EFUSE_EM, NULL);
    if (efuse_partition) {
        esp_efuse_init(efuse_partition->address, efuse_partition->size);
    }
#endif

    app_main();
    vTaskDelete(NULL);
}

