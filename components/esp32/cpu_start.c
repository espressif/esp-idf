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

#include "esp32/rom/ets_sys.h"
#include "esp32/rom/uart.h"
#include "esp32/rom/rtc.h"
#include "esp32/rom/cache.h"

#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_periph.h"
#include "soc/rtc_wdt.h"
#include "soc/efuse_periph.h"

#include "driver/rtc_io.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

#include "esp_heap_caps_init.h"
#include "sdkconfig.h"
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
#include "esp32/brownout.h"
#include "esp_int_wdt.h"
#include "esp_task.h"
#include "esp_task_wdt.h"
#include "esp_phy_init.h"
#include "esp32/cache_err_int.h"
#include "esp_coexist_internal.h"
#include "esp_core_dump.h"
#include "esp_app_trace.h"
#include "esp_private/dbg_stubs.h"
#include "esp_flash_encrypt.h"
#include "esp32/spiram.h"
#include "esp_clk_internal.h"
#include "esp_timer.h"
#include "esp_pm.h"
#include "esp_private/pm_impl.h"
#include "trax.h"
#include "esp_ota_ops.h"
#include "esp_efuse.h"
#include "bootloader_flash_config.h"

#ifdef CONFIG_APP_BUILD_TYPE_ELF_RAM
#include "esp32/rom/efuse.h"
#include "esp32/rom/spi_flash.h"
#endif // CONFIG_APP_BUILD_TYPE_ELF_RAM

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default"))) __attribute__((noreturn));
void start_cpu0_default(void) IRAM_ATTR __attribute__((noreturn));
#if !CONFIG_FREERTOS_UNICORE
static void IRAM_ATTR call_start_cpu1(void) __attribute__((noreturn));
void start_cpu1(void) __attribute__((weak, alias("start_cpu1_default"))) __attribute__((noreturn));
void start_cpu1_default(void) IRAM_ATTR __attribute__((noreturn));
static bool app_cpu_started = false;
#endif //!CONFIG_FREERTOS_UNICORE

static void do_global_ctors(void);
static void main_task(void* args);
extern void app_main(void);
extern esp_err_t esp_pthread_init(void);

extern int _bss_start;
extern int _bss_end;
extern int _rtc_bss_start;
extern int _rtc_bss_end;
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern int _ext_ram_bss_start;
extern int _ext_ram_bss_end;
#endif
extern int _init_start;
extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);
extern volatile int port_xSchedulerRunning[2];

static const char* TAG = "cpu_start";

struct object { long placeholder[ 10 ]; };
void __register_frame_info (const void *begin, struct object *ob);
extern char __eh_frame[];

//If CONFIG_SPIRAM_IGNORE_NOTFOUND is set and external RAM is not found or errors out on testing, this is set to false.
static bool s_spiram_okay=true;

/*
 * We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
 * and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
 */

void IRAM_ATTR call_start_cpu0(void)
{
#if CONFIG_FREERTOS_UNICORE
    RESET_REASON rst_reas[1];
#else
    RESET_REASON rst_reas[2];
#endif
    cpu_configure_region_protection();
    cpu_init_memctl();

    //Move exception vectors to IRAM
    asm volatile (\
                  "wsr    %0, vecbase\n" \
                  ::"r"(&_init_start));

    rst_reas[0] = rtc_get_reset_reason(0);

#if !CONFIG_FREERTOS_UNICORE
    rst_reas[1] = rtc_get_reset_reason(1);
#endif

    // from panic handler we can be reset by RWDT or TG0WDT
    if (rst_reas[0] == RTCWDT_SYS_RESET || rst_reas[0] == TG0WDT_SYS_RESET
#if !CONFIG_FREERTOS_UNICORE
        || rst_reas[1] == RTCWDT_SYS_RESET || rst_reas[1] == TG0WDT_SYS_RESET
#endif
    ) {
#ifndef CONFIG_BOOTLOADER_WDT_ENABLE
        rtc_wdt_disable();
#endif
    }

    //Clear BSS. Please do not attempt to do any complex stuff (like early logging) before this.
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

    /* Unless waking from deep sleep (implying RTC memory is intact), clear RTC bss */
    if (rst_reas[0] != DEEPSLEEP_RESET) {
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start) * sizeof(_rtc_bss_start));
    }

#if CONFIG_SPIRAM_BOOT_INIT
    esp_spiram_init_cache();
    if (esp_spiram_init() != ESP_OK) {
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
        ESP_EARLY_LOGE(TAG, "Failed to init external RAM, needed for external .bss segment");
        abort();
#endif

#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGI(TAG, "Failed to init external RAM; continuing without it.");
        s_spiram_okay = false;
#else
        ESP_EARLY_LOGE(TAG, "Failed to init external RAM!");
        abort();
#endif
    }
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

#if !CONFIG_FREERTOS_UNICORE
    if (REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_DIS_APP_CPU)) {
        ESP_EARLY_LOGE(TAG, "Running on single core chip, but application is built with dual core support.");
        ESP_EARLY_LOGE(TAG, "Please enable CONFIG_FREERTOS_UNICORE option in menuconfig.");
        abort();
    }
    ESP_EARLY_LOGI(TAG, "Starting app cpu, entry point is %p", call_start_cpu1);

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    esp_flash_encryption_init_checks();
#endif

    //Flush and enable icache for APP CPU
    Cache_Flush(1);
    Cache_Read_Enable(1);
    esp_cpu_unstall(1);
    // Enable clock and reset APP CPU. Note that OpenOCD may have already
    // enabled clock and taken APP CPU out of reset. In this case don't reset
    // APP CPU again, as that will clear the breakpoints which may have already
    // been set.
    if (!DPORT_GET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN)) {
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_C_REG, DPORT_APPCPU_RUNSTALL);
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
    }
    ets_set_appcpu_boot_addr((uint32_t)call_start_cpu1);

    while (!app_cpu_started) {
        ets_delay_us(100);
    }
#else
    ESP_EARLY_LOGI(TAG, "Single core mode");
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
#endif


#if CONFIG_SPIRAM_MEMTEST
    if (s_spiram_okay) {
        bool ext_ram_ok=esp_spiram_test();
        if (!ext_ram_ok) {
            ESP_EARLY_LOGE(TAG, "External RAM failed memory test!");
            abort();
        }
    }
#endif
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    memset(&_ext_ram_bss_start, 0, (&_ext_ram_bss_end - &_ext_ram_bss_start) * sizeof(_ext_ram_bss_start));
#endif
    /* Initialize heap allocator. WARNING: This *needs* to happen *after* the app cpu has booted.
       If the heap allocator is initialized first, it will put free memory linked list items into
       memory also used by the ROM. Starting the app cpu will let its ROM initialize that memory,
       corrupting those linked lists. Initializing the allocator *after* the app cpu has booted
       works around this problem.
       With SPI RAM enabled, there's a second reason: half of the SPI RAM will be managed by the
       app CPU, and when that is not up yet, the memory will be inaccessible and heap_caps_init may
       fail initializing it properly. */
    heap_caps_init();

    ESP_EARLY_LOGI(TAG, "Pro cpu start user code");
    start_cpu0();
}

#if !CONFIG_FREERTOS_UNICORE

static void wdt_reset_cpu1_info_enable(void)
{
    DPORT_REG_SET_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_PDEBUG_ENABLE | DPORT_APP_CPU_RECORD_ENABLE);
    DPORT_REG_CLR_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_RECORD_ENABLE);
}

void IRAM_ATTR call_start_cpu1(void)
{
    asm volatile (\
                  "wsr    %0, vecbase\n" \
                  ::"r"(&_init_start));

    ets_set_appcpu_boot_addr(0);
    cpu_configure_region_protection();
    cpu_init_memctl();

#if CONFIG_ESP_CONSOLE_UART_NONE
    ets_install_putc1(NULL);
    ets_install_putc2(NULL);
#else // CONFIG_ESP_CONSOLE_UART_NONE
    uartAttach();
    ets_install_uart_printf();
    uart_tx_switch(CONFIG_ESP_CONSOLE_UART_NUM);
#endif

    wdt_reset_cpu1_info_enable();
    ESP_EARLY_LOGI(TAG, "App cpu up.");
    app_cpu_started = 1;
    start_cpu1();
}
#endif //!CONFIG_FREERTOS_UNICORE

static void intr_matrix_clear(void)
{
    //Clear all the interrupt matrix register
    for (int i = ETS_WIFI_MAC_INTR_SOURCE; i <= ETS_CACHE_IA_INTR_SOURCE; i++) {
        intr_matrix_set(0, i, ETS_INVALID_INUM);
#if !CONFIG_FREERTOS_UNICORE
        intr_matrix_set(1, i, ETS_INVALID_INUM);
#endif
    }
}

void start_cpu0_default(void)
{
    esp_err_t err;
    esp_setup_syscall_table();

    if (s_spiram_okay) {
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

//Enable trace memory and immediately start trace.
#if CONFIG_ESP32_TRAX
#if CONFIG_ESP32_TRAX_TWOBANKS
    trax_enable(TRAX_ENA_PRO_APP);
#else
    trax_enable(TRAX_ENA_PRO);
#endif
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

#if CONFIG_ESP32_BROWNOUT_DET
    esp_brownout_init();
#endif
#if CONFIG_ESP32_DISABLE_BASIC_ROM_CONSOLE
    esp_efuse_disable_basic_rom_console();
#endif
    rtc_gpio_force_hold_dis_all();
    esp_vfs_dev_uart_register();
    esp_reent_init(_GLOBAL_REENT);
#ifndef CONFIG_ESP_CONSOLE_UART_NONE
    const char* default_uart_dev = "/dev/uart/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM);
    _GLOBAL_REENT->_stdin  = fopen(default_uart_dev, "r");
    _GLOBAL_REENT->_stdout = fopen(default_uart_dev, "w");
    _GLOBAL_REENT->_stderr = fopen(default_uart_dev, "w");
#else
    _GLOBAL_REENT->_stdin  = (FILE*) &__sf_fake_stdin;
    _GLOBAL_REENT->_stdout = (FILE*) &__sf_fake_stdout;
    _GLOBAL_REENT->_stderr = (FILE*) &__sf_fake_stderr;
#endif
    esp_timer_init();
    esp_set_time_from_rtc();
#if CONFIG_ESP32_APPTRACE_ENABLE
    err = esp_apptrace_init();
    assert(err == ESP_OK && "Failed to init apptrace module on PRO CPU!");
#endif
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_SYSVIEW_Conf();
#endif
#if CONFIG_ESP32_DEBUG_STUBS_ENABLE
    esp_dbg_stubs_init();
#endif
    err = esp_pthread_init();
    assert(err == ESP_OK && "Failed to init pthread module!");

    do_global_ctors();
#if CONFIG_ESP_INT_WDT
    esp_int_wdt_init();
    //Initialize the interrupt watch dog for CPU0.
    esp_int_wdt_cpu_init();
#endif
    esp_cache_err_int_init();
    esp_crosscore_int_init();
#ifndef CONFIG_FREERTOS_UNICORE
    esp_dport_access_int_init();
#endif

    bootloader_flash_update_id();
#if !CONFIG_SPIRAM_BOOT_INIT
    // Read the application binary image header. This will also decrypt the header if the image is encrypted.
    esp_image_header_t fhdr = {0};
#ifdef CONFIG_APP_BUILD_TYPE_ELF_RAM
    fhdr.spi_mode = ESP_IMAGE_SPI_MODE_DIO;
    fhdr.spi_speed = ESP_IMAGE_SPI_SPEED_40M;
    fhdr.spi_size = ESP_IMAGE_FLASH_SIZE_4MB;

    extern void esp_rom_spiflash_attach(uint32_t, bool);
    esp_rom_spiflash_attach(ets_efuse_get_spiconfig(), false);
    esp_rom_spiflash_unlock();
#else
    // This assumes that DROM is the first segment in the application binary, i.e. that we can read
    // the binary header through cache by accessing SOC_DROM_LOW address.
    memcpy(&fhdr, (void*) SOC_DROM_LOW, sizeof(fhdr));
#endif // CONFIG_APP_BUILD_TYPE_ELF_RAM

    // If psram is uninitialized, we need to improve some flash configuration.
    bootloader_flash_clock_config(&fhdr);
    bootloader_flash_gpio_config(&fhdr);
    bootloader_flash_dummy_config(&fhdr);
    bootloader_flash_cs_timing_config();
#endif //!CONFIG_SPIRAM_BOOT_INIT

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
    esp_pm_config_esp32_t cfg = {
        .max_freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = xtal_freq,
    };
    esp_pm_configure(&cfg);
#endif //CONFIG_PM_DFS_INIT_AUTO
#endif //CONFIG_PM_ENABLE

#if CONFIG_ESP32_ENABLE_COREDUMP
    esp_core_dump_init();
    size_t core_data_sz = 0;
    size_t core_data_addr = 0;
    if (esp_core_dump_image_get(&core_data_addr, &core_data_sz) == ESP_OK && core_data_sz > 0) {
        ESP_LOGI(TAG, "Found core dump %d bytes in flash @ 0x%x", core_data_sz, core_data_addr);
    }
#endif

#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    esp_coex_adapter_register(&g_coex_adapter_funcs);
#endif

    portBASE_TYPE res = xTaskCreatePinnedToCore(&main_task, "main",
                                                ESP_TASK_MAIN_STACK, NULL,
                                                ESP_TASK_MAIN_PRIO, NULL, 0);
    assert(res == pdTRUE);
    ESP_LOGI(TAG, "Starting scheduler on PRO CPU.");
    vTaskStartScheduler();
    abort(); /* Only get to here if not enough free heap to start scheduler */
}

#if !CONFIG_FREERTOS_UNICORE
void start_cpu1_default(void)
{
    // Wait for FreeRTOS initialization to finish on PRO CPU
    while (port_xSchedulerRunning[0] == 0) {
        ;
    }
#if CONFIG_ESP32_TRAX_TWOBANKS
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif
#if CONFIG_ESP32_APPTRACE_ENABLE
    esp_err_t err = esp_apptrace_init();
    assert(err == ESP_OK && "Failed to init apptrace module on APP CPU!");
#endif
#if CONFIG_ESP_INT_WDT
    //Initialize the interrupt watch dog for CPU1.
    esp_int_wdt_cpu_init();
#endif
    //Take care putting stuff here: if asked, FreeRTOS will happily tell you the scheduler
    //has started, but it isn't active *on this CPU* yet.
    esp_cache_err_int_init();
    esp_crosscore_int_init();
    esp_dport_access_int_init();

    ESP_EARLY_LOGI(TAG, "Starting scheduler on APP CPU.");
    xPortStartScheduler();
    abort(); /* Only get to here if FreeRTOS somehow very broken */
}
#endif //!CONFIG_FREERTOS_UNICORE

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
size_t __cxx_eh_arena_size_get(void)
{
    return CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE;
}
#endif

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

static void main_task(void* args)
{
#if !CONFIG_FREERTOS_UNICORE
    // Wait for FreeRTOS initialization to finish on APP CPU, before replacing its startup stack
    while (port_xSchedulerRunning[1] == 0) {
        ;
    }
#endif
    //Enable allocation in region where the startup stacks were located.
    heap_caps_enable_nonos_stack_heaps();

    // Now we have startup stack RAM available for heap, enable any DMA pool memory
#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
    esp_err_t r = esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
    if (r != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool (error 0x%x)", r);
        abort();
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

