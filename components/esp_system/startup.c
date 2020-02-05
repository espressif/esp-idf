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

#include "esp_attr.h"
#include "esp_err.h"

#include "soc/rtc_wdt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_heap_caps_init.h"
#include "esp_system.h"
#include "esp_flash_internal.h"
#include "nvs_flash.h"
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
#include "esp_core_dump.h"
#include "esp_app_trace.h"
#include "esp_private/dbg_stubs.h"
#include "esp_flash_encrypt.h"
#include "esp_clk_internal.h"
#include "esp_timer.h"
#include "esp_pm.h"
#include "esp_private/pm_impl.h"
#include "esp_ota_ops.h"

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/uart.h"
#include "esp32/dport_access.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/uart.h"
#include "esp32s2/dport_access.h"
#endif

#include "sys_funcs.h"

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default"))) __attribute__((noreturn));
void start_cpu0_default(void) IRAM_ATTR __attribute__((noreturn));
#if !CONFIG_FREERTOS_UNICORE
void start_cpu1(void) __attribute__((weak, alias("start_cpu1_default"))) __attribute__((noreturn));
void start_cpu1_default(void) IRAM_ATTR __attribute__((noreturn));
#endif //!CONFIG_FREERTOS_UNICORE

extern void app_main(void);
extern esp_err_t esp_pthread_init(void);

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);
extern volatile int port_xSchedulerRunning[2];

static const char* TAG = "cpu_start";

struct object { long placeholder[ 10 ]; };
void __register_frame_info (const void *begin, struct object *ob);
extern char __eh_frame[];

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

static void do_system_init_fn(void)
{
    extern esp_system_init_fn_t _esp_system_init_fn_array_start;
    extern esp_system_init_fn_t _esp_system_init_fn_array_end;

    esp_system_init_fn_t *p;

    for (p = &_esp_system_init_fn_array_end - 1; p >= &_esp_system_init_fn_array_start; --p) {
        if (p->cores & BIT(cpu_hal_get_core_id())) {
            (*(p->fn))();
        }
    }
}

static void main_task(void* args)
{
#if !CONFIG_FREERTOS_UNICORE
    // Wait for FreeRTOS initialization to finish on APP CPU, before replacing its startup stack
    while (port_xSchedulerRunning[1] == 0) {
        ;
    }
#endif

    // Now we have startup stack RAM available for heap, enable any DMA pool memory
#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
    if (g_spiram_ok) {
        esp_err_t r = esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool (error 0x%x)", r);
            abort();
        }
    }
#endif

    //Initialize task wdt if configured to do so
#ifdef CONFIG_ESP_TASK_WDT_PANIC
    ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, true));
#elif CONFIG_ESP_TASK_WDT
    ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, false));
#endif

    //Add IDLE 0 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    if(idle_0 != NULL){
        ESP_ERROR_CHECK(esp_task_wdt_add(idle_0));
    }
#endif
    //Add IDLE 1 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
    TaskHandle_t idle_1 = xTaskGetIdleTaskHandleForCPU(1);
    if(idle_1 != NULL){
        ESP_ERROR_CHECK(esp_task_wdt_add(idle_1));
    }
    esp_spiram_init_cache();
#endif

    // Now that the application is about to start, disable boot watchdog
#ifndef CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE
    rtc_wdt_disable();
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

#if !CONFIG_FREERTOS_UNICORE
void start_cpu1_default(void)
{
    // Wait for FreeRTOS initialization to finish on PRO CPU
    while (port_xSchedulerRunning[0] == 0) {
        ;
    }

#if CONFIG_APPTRACE_ENABLE
    esp_err_t err = esp_apptrace_init();
    assert(err == ESP_OK && "Failed to init apptrace module on APP CPU!");
#endif
#if CONFIG_ESP_INT_WDT
    //Initialize the interrupt watch dog for CPU1.
    esp_int_wdt_cpu_init();
#endif

    esp_crosscore_int_init();
    esp_dport_access_int_init();

    ESP_EARLY_LOGI(TAG, "Starting scheduler on APP CPU.");
    xPortStartScheduler();
    abort(); /* Only get to here if FreeRTOS somehow very broken */
}
#endif //!CONFIG_FREERTOS_UNICORE

/*
 * We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
 * and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
 */
void IRAM_ATTR start_cpu0_default(void)
{
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

    /* Initialize heap allocator */
    heap_caps_init();

    ESP_EARLY_LOGI(TAG, "Pro cpu start user code");

    esp_err_t err;
    esp_setup_syscall_table();

    if (g_spiram_ok) {
#if CONFIG_SPIRAM_BOOT_INIT && (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
        esp_err_t r=esp_spiram_add_to_heapalloc();
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "External RAM could not be added to heap!");
            abort();
        }
#if CONFIG_SPIRAM_USE_MALLOC
        heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
#endif
#endif
    }

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

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    esp_flash_encryption_init_checks();
#endif

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
#if CONFIG_ESP_DEBUG_STUBS_ENABLE
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
#if CONFIG_ESP_INT_WDT
    esp_int_wdt_init();
    //Initialize the interrupt watch dog for CPU0.
    esp_int_wdt_cpu_init();
#else
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    assert(!soc_has_cache_lock_bug() && "ESP32 Rev 3 + Dual Core + PSRAM requires INT WDT enabled in project config!");
#endif
#endif

    esp_crosscore_int_init();

#ifndef CONFIG_FREERTOS_UNICORE
    esp_dport_access_int_init();
#endif


    spi_flash_init();
    /* init default OS-aware flash access critical section */
    spi_flash_guard_set(&g_flash_guard_default_ops);

    esp_flash_app_init();
    esp_err_t flash_ret = esp_flash_init_default_chip();
    assert(flash_ret == ESP_OK);

#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ESP32_ENABLE_COREDUMP
    esp_core_dump_init();
#endif
#endif

#ifdef CONFIG_PM_ENABLE
    esp_pm_impl_init();
#ifdef CONFIG_PM_DFS_INIT_AUTO
    int xtal_freq = (int) rtc_clk_xtal_freq_get();
    esp_pm_config_esp32_t cfg = {
        .max_freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = xtal_freq,
    };
    esp_pm_configure(&cfg);
#endif //CONFIG_PM_DFS_INIT_AUTO
#endif //CONFIG_PM_ENABLE

#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    esp_coex_adapter_register(&g_coex_adapter_funcs);
    coex_pre_init();
#endif
#endif

    do_system_init_fn();

    portBASE_TYPE res = xTaskCreatePinnedToCore(&main_task, "main",
                                                ESP_TASK_MAIN_STACK, NULL,
                                                ESP_TASK_MAIN_PRIO, NULL, 0);
    assert(res == pdTRUE);

    ESP_LOGI(TAG, "Starting scheduler on PRO CPU.");
    vTaskStartScheduler();
    abort(); /* Only get to here if not enough free heap to start scheduler */
}