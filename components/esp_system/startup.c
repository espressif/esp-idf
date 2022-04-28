/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_err.h"

#include "esp_system.h"
#include "esp_log.h"

#include "sdkconfig.h"

#include "soc/soc_caps.h"
#include "hal/wdt_hal.h"
#include "hal/uart_types.h"
#include "hal/uart_ll.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_heap_caps_init.h"
#include "esp_spi_flash.h"
#include "esp_flash_internal.h"
#include "esp_newlib.h"
#include "esp_vfs_dev.h"
#include "esp_timer.h"
#include "esp_efuse.h"
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"
#include "esp_sleep.h"
#include "esp_xt_wdt.h"

#if __has_include("esp_ota_ops.h")
#include "esp_ota_ops.h"
#define HAS_ESP_OTA 1
#endif

/***********************************************/
// Headers for other components init functions
#include "esp_coexist_internal.h"

#if CONFIG_ESP_COREDUMP_ENABLE
#include "esp_core_dump.h"
#endif

#if CONFIG_APPTRACE_ENABLE
#include "esp_app_trace.h"
#endif

#include "esp_private/dbg_stubs.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#include "esp_private/pm_impl.h"
#endif

#include "esp_pthread.h"
#include "esp_vfs_console.h"
#include "esp_private/esp_clk.h"

#include "esp_private/brownout.h"

#include "esp_rom_sys.h"

// [refactor-todo] make this file completely target-independent
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spiram.h"
#endif
/***********************************************/

#include "esp_private/startup_internal.h"

// Ensure that system configuration matches the underlying number of cores.
// This should enable us to avoid checking for both everytime.
#if !(SOC_CPU_CORES_NUM > 1) && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    #error "System has been configured to run on multiple cores, but target SoC only has a single core."
#endif

uint64_t g_startup_time = 0;

#if SOC_APB_BACKUP_DMA
// APB DMA lock initialising API
extern void esp_apb_backup_dma_lock_init(void);
#endif

// App entry point for core 0
extern void esp_startup_start_app(void);

// Entry point for core 0 from hardware init (port layer)
void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default"))) __attribute__((noreturn));

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
// Entry point for core [1..X] from hardware init (port layer)
void start_cpu_other_cores(void) __attribute__((weak, alias("start_cpu_other_cores_default"))) __attribute__((noreturn));

// App entry point for core [1..X]
void esp_startup_start_app_other_cores(void) __attribute__((weak, alias("esp_startup_start_app_other_cores_default"))) __attribute__((noreturn));

static volatile bool s_system_inited[SOC_CPU_CORES_NUM] = { false };

const sys_startup_fn_t g_startup_fn[SOC_CPU_CORES_NUM] = { [0] = start_cpu0,
#if SOC_CPU_CORES_NUM > 1
    [1 ... SOC_CPU_CORES_NUM - 1] = start_cpu_other_cores
#endif
};

static volatile bool s_system_full_inited = false;
#else
const sys_startup_fn_t g_startup_fn[1] = { start_cpu0 };
#endif

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
// workaround for C++ exception crashes
void _Unwind_SetNoFunctionContextInstall(unsigned char enable) __attribute__((weak, alias("_Unwind_SetNoFunctionContextInstall_Default")));
// workaround for C++ exception large memory allocation
void _Unwind_SetEnableExceptionFdeSorting(unsigned char enable);

static IRAM_ATTR void _Unwind_SetNoFunctionContextInstall_Default(unsigned char enable __attribute__((unused)))
{
    (void)0;
}
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS

static const char* TAG = "cpu_start";

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

/**
 * Xtensa gcc is configured to emit a .ctors section, RISC-V gcc is configured with --enable-initfini-array
 * so it emits an .init_array section instead.
 * But the init_priority sections will be sorted for iteration in ascending order during startup.
 * The rest of the init_array sections is sorted for iteration in descending order during startup, however.
 * Hence a different section is generated for the init_priority functions which is looped
 * over in ascending direction instead of descending direction.
 * The RISC-V-specific behavior is dependent on the linker script ld/esp32c3/sections.ld.in.
 */
static void do_global_ctors(void)
{
#if __riscv
    extern void (*__init_priority_array_start)(void);
    extern void (*__init_priority_array_end)(void);
#endif

    extern void (*__init_array_start)(void);
    extern void (*__init_array_end)(void);

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    struct object { long placeholder[ 10 ]; };
    void __register_frame_info (const void *begin, struct object *ob);
    extern char __eh_frame[];

    static struct object ob;
    __register_frame_info( __eh_frame, &ob );
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS

    void (**p)(void);

#if __riscv
    for (p = &__init_priority_array_start; p < &__init_priority_array_end; ++p) {
        ESP_EARLY_LOGD(TAG, "calling init function: %p", *p);
        (*p)();
    }
#endif

    for (p = &__init_array_end - 1; p >= &__init_array_start; --p) {
        ESP_EARLY_LOGD(TAG, "calling init function: %p", *p);
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

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_system_inited[cpu_hal_get_core_id()] = true;
#endif
}

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
static void  esp_startup_start_app_other_cores_default(void)
{
    while (1) {
        esp_rom_delay_us(UINT32_MAX);
    }
}

static void IRAM_ATTR start_cpu_other_cores_default(void)
{
    do_system_init_fn();

    while (!s_system_full_inited) {
        esp_rom_delay_us(100);
    }

    esp_startup_start_app_other_cores();
}
#endif

static void do_core_init(void)
{
    /* Initialize heap allocator. WARNING: This *needs* to happen *after* the app cpu has booted.
       If the heap allocator is initialized first, it will put free memory linked list items into
       memory also used by the ROM. Starting the app cpu will let its ROM initialize that memory,
       corrupting those linked lists. Initializing the allocator *after* the app cpu has booted
       works around this problem.
       With SPI RAM enabled, there's a second reason: half of the SPI RAM will be managed by the
       app CPU, and when that is not up yet, the memory will be inaccessible and heap_caps_init may
       fail initializing it properly. */
    heap_caps_init();

    // When apptrace module is enabled, there will be SEGGER_SYSVIEW calls in the newlib init.
    // SEGGER_SYSVIEW relies on apptrace module
    // apptrace module uses esp_timer_get_time to determine timeout conditions.
    // esp_timer early initialization is required for esp_timer_get_time to work.
    esp_timer_early_init();
    esp_newlib_init();

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

#if CONFIG_ESP_BROWNOUT_DET
    // [refactor-todo] leads to call chain rtc_is_register (driver) -> esp_intr_alloc (esp32/esp32s2) ->
    // malloc (newlib) -> heap_caps_malloc (heap), so heap must be at least initialized
    esp_brownout_init();
#endif

    esp_newlib_time_init();

#if CONFIG_VFS_SUPPORT_IO
    // VFS console register.
    esp_err_t vfs_err = esp_vfs_console_register();
    assert(vfs_err == ESP_OK && "Failed to register vfs console");
#endif

#if defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_NONE)
    const static char *default_stdio_dev = "/dev/console/";
    esp_reent_init(_GLOBAL_REENT);
    _GLOBAL_REENT->_stdin  = fopen(default_stdio_dev, "r");
    _GLOBAL_REENT->_stdout = fopen(default_stdio_dev, "w");
    _GLOBAL_REENT->_stderr = fopen(default_stdio_dev, "w");
#else // defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_NONE)
    _REENT_SMALL_CHECK_INIT(_GLOBAL_REENT);
#endif // defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_NONE)

    esp_err_t err __attribute__((unused));

    err = esp_pthread_init();
    assert(err == ESP_OK && "Failed to init pthread module!");

    spi_flash_init();
    /* init default OS-aware flash access critical section */
    spi_flash_guard_set(&g_flash_guard_default_ops);

    esp_flash_app_init();
    esp_err_t flash_ret = esp_flash_init_default_chip();
    assert(flash_ret == ESP_OK);
    (void)flash_ret;

#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    const esp_partition_t *efuse_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_EFUSE_EM, NULL);
    if (efuse_partition) {
        esp_efuse_init_virtual_mode_in_flash(efuse_partition->address, efuse_partition->size);
    }
#endif
#endif

#if CONFIG_SECURE_DISABLE_ROM_DL_MODE
    err = esp_efuse_disable_rom_download_mode();
    assert(err == ESP_OK && "Failed to disable ROM download mode");
#endif

#if CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    err = esp_efuse_enable_rom_secure_download_mode();
    assert(err == ESP_OK && "Failed to enable Secure Download mode");
#endif

#if CONFIG_ESP32_DISABLE_BASIC_ROM_CONSOLE
    esp_efuse_disable_basic_rom_console();
#endif

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    esp_flash_encryption_init_checks();
#endif

#if defined(CONFIG_SECURE_BOOT) || defined(CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT)
    // Note: in some configs this may read flash, so placed after flash init
    esp_secure_boot_init_checks();
#endif

#if CONFIG_ESP_XT_WDT
    esp_xt_wdt_config_t cfg = {
        .timeout                = CONFIG_ESP_XT_WDT_TIMEOUT,
        .auto_backup_clk_enable = CONFIG_ESP_XT_WDT_BACKUP_CLK_ENABLE,
    };
    err = esp_xt_wdt_init(&cfg);
    assert(err == ESP_OK && "Failed to init xtwdt");
#endif
}

static void do_secondary_init(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // The port layer transferred control to this function with other cores 'paused',
    // resume execution so that cores might execute component initialization functions.
    startup_resume_other_cores();
#endif

    // Execute initialization functions esp_system_init_fn_t assigned to the main core. While
    // this is happening, all other cores are executing the initialization functions
    // assigned to them since they have been resumed already.
    do_system_init_fn();

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // Wait for all cores to finish secondary init.
    volatile bool system_inited = false;

    while (!system_inited) {
        system_inited = true;
        for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
            system_inited &= s_system_inited[i];
        }
        esp_rom_delay_us(100);
    }
#endif
}

static void start_cpu0_default(void)
{

    ESP_EARLY_LOGI(TAG, "Pro cpu start user code");
    int cpu_freq = esp_clk_cpu_freq();
    ESP_EARLY_LOGI(TAG, "cpu freq: %d Hz", cpu_freq);

#if HAS_ESP_OTA // [refactor-todo] find a better way to handle this.
    // Display information about the current running image.
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
#endif //HAS_ESP_OTA

    // Initialize core components and services.
    do_core_init();

    // Execute constructors.
    do_global_ctors();

    // Execute init functions of other components; blocks
    // until all cores finish (when !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE).
    do_secondary_init();

    // Now that the application is about to start, disable boot watchdog
#ifndef CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE
    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif

#if SOC_CPU_CORES_NUM > 1 && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_system_full_inited = true;
#endif

    esp_startup_start_app();
    while (1);
}

IRAM_ATTR ESP_SYSTEM_INIT_FN(init_components0, BIT(0))
{
    esp_timer_init();

#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND && !CONFIG_PM_SLP_DISABLE_GPIO
    // Configure to isolate (disable the Input/Output/Pullup/Pulldown
    // function of the pin) all GPIO pins in sleep state
    esp_sleep_config_gpio_isolate();
    // Enable automatic switching of GPIO configuration
    esp_sleep_enable_gpio_switch(true);
#endif

#if CONFIG_APPTRACE_ENABLE
    esp_err_t err = esp_apptrace_init();
    assert(err == ESP_OK && "Failed to init apptrace module on PRO CPU!");
#endif
#if CONFIG_APPTRACE_SV_ENABLE
    SEGGER_SYSVIEW_Conf();
#endif

#if CONFIG_ESP_DEBUG_STUBS_ENABLE
    esp_dbg_stubs_init();
#endif

#if defined(CONFIG_PM_ENABLE)
    esp_pm_impl_init();
#endif

#if CONFIG_ESP_COREDUMP_ENABLE
    esp_core_dump_init();
#endif

#if SOC_APB_BACKUP_DMA
    esp_apb_backup_dma_lock_init();
#endif

#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    esp_coex_adapter_register(&g_coex_adapter_funcs);
    coex_pre_init();
#endif

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    ESP_EARLY_LOGD(TAG, "Setting C++ exception workarounds.");
    _Unwind_SetNoFunctionContextInstall(1);
    _Unwind_SetEnableExceptionFdeSorting(0);
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS
}
