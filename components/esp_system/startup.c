/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/chip_revision.h"
#include "hal/wdt_hal.h"
#include "hal/uart_types.h"
#include "hal/uart_ll.h"
#include "hal/efuse_hal.h"

#include "esp_heap_caps_init.h"
#include "spi_flash_mmap.h"
#include "esp_flash_internal.h"
#include "esp_newlib.h"
#include "esp_timer.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"
#include "esp_xt_wdt.h"
#include "esp_cpu.h"

#include "esp_partition.h"

/***********************************************/
// Headers for other components init functions

#if CONFIG_ESP_CRYPTO_FORCE_ECC_CONSTANT_TIME_POINT_MUL
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"
#endif

#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
#include "private/esp_coexist_internal.h"
#endif

#if __has_include("esp_app_desc.h")
#define WITH_APP_IMAGE_INFO
#include "esp_app_desc.h"
#endif

#if CONFIG_ESP_COREDUMP_ENABLE
#include "esp_core_dump.h"
#endif

#include "esp_private/dbg_stubs.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#include "esp_private/pm_impl.h"
#endif

#if CONFIG_VFS_SUPPORT_IO
#include "esp_vfs_dev.h"
#include "esp_vfs_console.h"
#endif

#include "esp_pthread.h"
#include "esp_private/esp_clk.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/brownout.h"

#include "esp_rom_caps.h"
#include "esp_rom_sys.h"

#if SOC_BOD_SUPPORTED
#include "hal/brownout_ll.h"
#endif

#if CONFIG_SPIRAM
#include "esp_psram.h"
#include "esp_private/esp_psram_extram.h"
#endif
/***********************************************/

#include "esp_private/startup_internal.h"

// Ensure that system configuration matches the underlying number of cores.
// This should enable us to avoid checking for both everytime.
#if !(SOC_CPU_CORES_NUM > 1) && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    #error "System has been configured to run on multiple cores, but target SoC only has a single core."
#endif

// Set efuse ROM_LOG_MODE on first boot
//
// For CONFIG_BOOT_ROM_LOG_ALWAYS_ON (default) or undefined (ESP32), leave
// ROM_LOG_MODE undefined (no need to call this function during startup)
#if CONFIG_BOOT_ROM_LOG_ALWAYS_OFF
#define ROM_LOG_MODE ESP_EFUSE_ROM_LOG_ALWAYS_OFF
#elif CONFIG_BOOT_ROM_LOG_ON_GPIO_LOW
#define ROM_LOG_MODE ESP_EFUSE_ROM_LOG_ON_GPIO_LOW
#elif CONFIG_BOOT_ROM_LOG_ON_GPIO_HIGH
#define ROM_LOG_MODE ESP_EFUSE_ROM_LOG_ON_GPIO_HIGH
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
        ESP_LOGD(TAG, "calling init function: %p", *p);
        (*p)();
    }
#endif

    for (p = &__init_array_end - 1; p >= &__init_array_start; --p) {
        ESP_LOGD(TAG, "calling init function: %p", *p);
        (*p)();
    }
}

/**
 * @brief Call component init functions defined using ESP_SYSTEM_INIT_Fn macros.
 * The esp_system_init_fn_t structures describing these functions are collected into
 * an array [_esp_system_init_fn_array_start, _esp_system_init_fn_array_end) by the
 * linker. The functions are sorted by their priority value.
 * The sequence of the init function calls (sorted by priority) is documented in
 * system_init_fn.txt file.
 */
static void do_system_init_fn(void)
{
    extern esp_system_init_fn_t _esp_system_init_fn_array_start;
    extern esp_system_init_fn_t _esp_system_init_fn_array_end;

    esp_system_init_fn_t *p;

    int core_id = esp_cpu_get_core_id();
    for (p = &_esp_system_init_fn_array_start; p < &_esp_system_init_fn_array_end; ++p) {
        if (p->cores & BIT(core_id)) {
            ESP_LOGD(TAG, "calling init function: %p on core: %d", p->fn, core_id);
            esp_err_t err = (*(p->fn))();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "init function %p has failed (0x%x), aborting", p->fn, err);
                abort();
            }
        }
    }

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_system_inited[core_id] = true;
#endif
}

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
static void  esp_startup_start_app_other_cores_default(void)
{
    while (1) {
        esp_rom_delay_us(UINT32_MAX);
    }
}

/* This function has to be in IRAM, as while it is running on CPU1, CPU0 may do some flash operations
 * (e.g. initialize the core dump), which means that cache will be disabled.
 */
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

#if CONFIG_SPIRAM_BOOT_INIT && (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if (esp_psram_is_initialized()) {
        esp_err_t r=esp_psram_extram_add_to_heap_allocator();
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "External RAM could not be added to heap!");
            abort();
        }
#if CONFIG_SPIRAM_USE_MALLOC
        heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
#endif
    }
#endif

#if CONFIG_ESP_BROWNOUT_DET
    // [refactor-todo] leads to call chain rtc_is_register (driver) -> esp_intr_alloc (esp32/esp32s2) ->
    // malloc (newlib) -> heap_caps_malloc (heap), so heap must be at least initialized
    esp_brownout_init();
#else
#if SOC_CAPS_NO_RESET_BY_ANA_BOD
    brownout_ll_ana_reset_enable(false);
#endif // SOC_CAPS_NO_RESET_BY_ANA_BOD
#endif // CONFIG_ESP_BROWNOUT_DET

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
#if ESP_ROM_NEEDS_SWSETUP_WORKAROUND
    /*
    - This workaround for printf functions using 32-bit time_t after the 64-bit time_t upgrade
    - The 32-bit time_t usage is triggered through ROM Newlib functions printf related functions calling __swsetup_r() on
      the first call to a particular file pointer (i.e., stdin, stdout, stderr)
    - Thus, we call the toolchain version of __swsetup_r() now (before any printf calls are made) to setup all of the
      file pointers. Thus, the ROM newlib code will never call the ROM version of __swsetup_r().
    - See IDFGH-7728 for more details
    */
    extern int __swsetup_r(struct _reent *, FILE *);
    __swsetup_r(_GLOBAL_REENT, _GLOBAL_REENT->_stdout);
    __swsetup_r(_GLOBAL_REENT, _GLOBAL_REENT->_stderr);
    __swsetup_r(_GLOBAL_REENT, _GLOBAL_REENT->_stdin);
#endif // ESP_ROM_NEEDS_SWSETUP_WORKAROUND
#else // defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_NONE)
    _REENT_SMALL_CHECK_INIT(_GLOBAL_REENT);
#endif // defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_NONE)

    esp_err_t err __attribute__((unused));

    err = esp_pthread_init();
    assert(err == ESP_OK && "Failed to init pthread module!");

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
#if CONFIG_SPI_FLASH_ROM_IMPL
    spi_flash_rom_impl_init();
#endif

    esp_flash_app_init();
    esp_err_t flash_ret = esp_flash_init_default_chip();
    assert(flash_ret == ESP_OK);
    (void)flash_ret;
#if CONFIG_SPI_FLASH_BROWNOUT_RESET
    spi_flash_needs_reset_check();
#endif // CONFIG_SPI_FLASH_BROWNOUT_RESET
#endif // !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    const esp_partition_t *efuse_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_EFUSE_EM, NULL);
    if (efuse_partition) {
        esp_efuse_init_virtual_mode_in_flash(efuse_partition->address, efuse_partition->size);
    }
#endif
#endif

#if CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    // For anti-rollback case, recheck security version before we boot-up the current application
    assert(esp_efuse_check_secure_version(esp_app_get_description()->secure_version) == true && "Incorrect secure version of app");
#endif

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    esp_flash_encryption_init_checks();
#endif

#if defined(CONFIG_SECURE_BOOT) || defined(CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT)
    // Note: in some configs this may read flash, so placed after flash init
    esp_secure_boot_init_checks();
#endif

#if SOC_EFUSE_ECDSA_USE_HARDWARE_K
    if (esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY, NULL)) {
        // ECDSA key purpose block is present and hence permanently enable
        // the hardware TRNG supplied k mode (most secure mode)
        if (!CONFIG_IDF_TARGET_ESP32H2 || (CONFIG_IDF_TARGET_ESP32H2 && !ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102))) {
            err = esp_efuse_write_field_bit(ESP_EFUSE_ECDSA_FORCE_USE_HARDWARE_K);
            assert(err == ESP_OK && "Failed to enable ECDSA hardware k mode");
        }
    }
#endif

#if CONFIG_ESP_CRYPTO_FORCE_ECC_CONSTANT_TIME_POINT_MUL
    bool force_constant_time = true;
#if CONFIG_IDF_TARGET_ESP32H2
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
        force_constant_time = false;
    }
#endif
    if (!esp_efuse_read_field_bit(ESP_EFUSE_ECC_FORCE_CONST_TIME) && force_constant_time) {
        ESP_EARLY_LOGD(TAG, "Forcefully enabling ECC constant time operations");
        esp_err_t err = esp_efuse_write_field_bit(ESP_EFUSE_ECC_FORCE_CONST_TIME);
        assert(err == ESP_OK && "Failed to enable ECC constant time operations");
    }
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

#ifdef ROM_LOG_MODE
    esp_efuse_set_rom_log_scheme(ROM_LOG_MODE);
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

#ifdef WITH_APP_IMAGE_INFO
    // Display information about the current running image.
    if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {
        const esp_app_desc_t *app_desc = esp_app_get_description();
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
        esp_app_get_elf_sha256(buf, sizeof(buf));
        ESP_EARLY_LOGI(TAG, "ELF file SHA256:  %s...", buf);
        ESP_EARLY_LOGI(TAG, "ESP-IDF:          %s", app_desc->idf_ver);

        ESP_EARLY_LOGI(TAG, "Min chip rev:     v%d.%d", CONFIG_ESP_REV_MIN_FULL / 100, CONFIG_ESP_REV_MIN_FULL % 100);
        ESP_EARLY_LOGI(TAG, "Max chip rev:     v%d.%d %s",CONFIG_ESP_REV_MAX_FULL / 100, CONFIG_ESP_REV_MAX_FULL % 100,
                       efuse_ll_get_disable_wafer_version_major() ? "(constraint ignored)" : "");
        unsigned revision = efuse_hal_chip_revision();
        ESP_EARLY_LOGI(TAG, "Chip rev:         v%d.%d", revision / 100, revision % 100);
    }
#endif

    // Initialize core components and services.
    do_core_init();

    // Execute constructors.
    do_global_ctors();

    // Execute init functions of other components; blocks
    // until all cores finish (when !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE).
    do_secondary_init();

    // Now that the application is about to start, disable boot watchdog
#ifndef CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE
    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
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

ESP_SYSTEM_INIT_FN(init_components0, BIT(0), 200)
{
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

    return ESP_OK;
}
