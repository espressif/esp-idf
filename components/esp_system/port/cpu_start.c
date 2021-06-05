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
#include <stdbool.h>

#include "esp_attr.h"
#include "esp_err.h"

#include "esp_log.h"
#include "esp_system.h"

#include "esp_efuse.h"
#include "cache_err_int.h"
#include "esp_clk_internal.h"

#include "esp_rom_efuse.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#include "esp32/rtc.h"
#include "esp32/rom/cache.h"
#include "esp32/rom/rtc.h"
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rtc.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/spiram.h"
#include "esp32s2/dport_access.h"
#include "esp32s2/memprot.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rtc.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/rtc.h"
#include "esp32s3/spiram.h"
#include "esp32s3/dport_access.h"
#include "esp32s3/memprot.h"
#include "soc/assist_debug_reg.h"
#include "soc/cache_memory.h"
#include "soc/system_reg.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rtc.h"
#include "esp32c3/rom/cache.h"
#include "esp32c3/rom/rtc.h"
#include "soc/cache_memory.h"
#include "esp32c3/memprot.h"
#endif

#include "bootloader_flash_config.h"
#include "esp_private/crosscore_int.h"
#include "esp_flash_encrypt.h"

#include "hal/rtc_io_hal.h"
#include "hal/gpio_hal.h"
#include "hal/wdt_hal.h"
#include "soc/rtc.h"
#include "soc/efuse_reg.h"
#include "soc/periph_defs.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/spinlock.h"

#if CONFIG_ESP32_TRAX || CONFIG_ESP32S2_TRAX
#include "trax.h"
#endif

#include "bootloader_mem.h"

#if CONFIG_APP_BUILD_TYPE_ELF_RAM
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/spi_flash.h"
#endif
#endif // CONFIG_APP_BUILD_TYPE_ELF_RAM

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


#include "esp_private/startup_internal.h"
#include "esp_private/system_internal.h"

extern int _bss_start;
extern int _bss_end;
extern int _rtc_bss_start;
extern int _rtc_bss_end;

extern int _vector_table;

static const char *TAG = "cpu_start";

#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern int _ext_ram_bss_start;
extern int _ext_ram_bss_end;
#endif
#ifdef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
extern int _iram_bss_start;
extern int _iram_bss_end;
#endif
#endif // CONFIG_IDF_TARGET_ESP32

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
static volatile bool s_cpu_up[SOC_CPU_CORES_NUM] = { false };
static volatile bool s_cpu_inited[SOC_CPU_CORES_NUM] = { false };

static volatile bool s_resume_cores;
#endif

// If CONFIG_SPIRAM_IGNORE_NOTFOUND is set and external RAM is not found or errors out on testing, this is set to false.
bool g_spiram_ok = true;

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
void startup_resume_other_cores(void)
{
    s_resume_cores = true;
}

void IRAM_ATTR call_start_cpu1(void)
{
    cpu_hal_set_vecbase(&_vector_table);

    ets_set_appcpu_boot_addr(0);

    bootloader_init_mem();

#if CONFIG_ESP_CONSOLE_UART_NONE
    esp_rom_install_channel_putc(1, NULL);
    esp_rom_install_channel_putc(2, NULL);
#else // CONFIG_ESP_CONSOLE_UART_NONE
    esp_rom_install_uart_printf();
    esp_rom_uart_set_as_console(CONFIG_ESP_CONSOLE_UART_NUM);
#endif

#if CONFIG_IDF_TARGET_ESP32
    DPORT_REG_SET_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_PDEBUG_ENABLE | DPORT_APP_CPU_RECORD_ENABLE);
    DPORT_REG_CLR_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_RECORD_ENABLE);
#else
    REG_WRITE(ASSIST_DEBUG_CORE_1_RCD_PDEBUGENABLE_REG, 1);
    REG_WRITE(ASSIST_DEBUG_CORE_1_RCD_RECORDING_REG, 1);
#endif

    s_cpu_up[1] = true;
    ESP_EARLY_LOGI(TAG, "App cpu up.");

    //Take care putting stuff here: if asked, FreeRTOS will happily tell you the scheduler
    //has started, but it isn't active *on this CPU* yet.
    esp_cache_err_int_init();

#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ESP32_TRAX_TWOBANKS
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif
#endif

    s_cpu_inited[1] = true;

    while (!s_resume_cores) {
        esp_rom_delay_us(100);
    }

    SYS_STARTUP_FN();
}

static void start_other_core(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    // If not the single core variant of a target - check this since there is
    // no separate soc_caps.h for the single core variant.
    if (!(chip_info.cores > 1)) {
        ESP_EARLY_LOGE(TAG, "Running on single core variant of a chip, but app is built with multi-core support.");
        ESP_EARLY_LOGE(TAG, "Check that CONFIG_FREERTOS_UNICORE is enabled in menuconfig");
        abort();
    }

    ESP_EARLY_LOGI(TAG, "Starting app cpu, entry point is %p", call_start_cpu1);

#if CONFIG_IDF_TARGET_ESP32
    Cache_Flush(1);
    Cache_Read_Enable(1);
#endif

    esp_cpu_unstall(1);

    // Enable clock and reset APP CPU. Note that OpenOCD may have already
    // enabled clock and taken APP CPU out of reset. In this case don't reset
    // APP CPU again, as that will clear the breakpoints which may have already
    // been set.
#if CONFIG_IDF_TARGET_ESP32
    if (!DPORT_GET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN)) {
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_C_REG, DPORT_APPCPU_RUNSTALL);
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
    }
#elif CONFIG_IDF_TARGET_ESP32S3
    if (!REG_GET_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_CLKGATE_EN)) {
        REG_SET_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_CLKGATE_EN);
        REG_CLR_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_RUNSTALL);
        REG_SET_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_RESETING);
        REG_CLR_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_RESETING);
    }
#endif
    ets_set_appcpu_boot_addr((uint32_t)call_start_cpu1);

    bool cpus_up = false;

    while (!cpus_up) {
        cpus_up = true;
        for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
            cpus_up &= s_cpu_up[i];
        }
        esp_rom_delay_us(100);
    }
}
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE

static void intr_matrix_clear(void)
{
    for (int i = 0; i < ETS_MAX_INTR_SOURCE; i++) {
        intr_matrix_set(0, i, ETS_INVALID_INUM);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        intr_matrix_set(1, i, ETS_INVALID_INUM);
#endif
    }
}

/*
 * We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
 * and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
 */
void IRAM_ATTR call_start_cpu0(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    RESET_REASON rst_reas[SOC_CPU_CORES_NUM];
#else
    RESET_REASON rst_reas[1];
#endif

#ifdef __riscv
    // Configure the global pointer register
    // (This should be the first thing IDF app does, as any other piece of code could be
    // relaxed by the linker to access something relative to __global_pointer$)
    __asm__ __volatile__ (
        ".option push\n"
        ".option norelax\n"
        "la gp, __global_pointer$\n"
        ".option pop"
    );
#endif

    // Move exception vectors to IRAM
    cpu_hal_set_vecbase(&_vector_table);

    rst_reas[0] = rtc_get_reset_reason(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    rst_reas[1] = rtc_get_reset_reason(1);
#endif

#ifndef CONFIG_BOOTLOADER_WDT_ENABLE
    // from panic handler we can be reset by RWDT or TG0WDT
    if (rst_reas[0] == RTCWDT_SYS_RESET || rst_reas[0] == TG0WDT_SYS_RESET
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
            || rst_reas[1] == RTCWDT_SYS_RESET || rst_reas[1] == TG0WDT_SYS_RESET
#endif
       ) {
        wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
#endif

    //Clear BSS. Please do not attempt to do any complex stuff (like early logging) before this.
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

#if defined(CONFIG_IDF_TARGET_ESP32) && defined(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
    // Clear IRAM BSS
    memset(&_iram_bss_start, 0, (&_iram_bss_end - &_iram_bss_start) * sizeof(_iram_bss_start));
#endif

    /* Unless waking from deep sleep (implying RTC memory is intact), clear RTC bss */
    if (rst_reas[0] != DEEPSLEEP_RESET) {
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start) * sizeof(_rtc_bss_start));
    }

#if CONFIG_IDF_TARGET_ESP32S2
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
#endif

#if CONFIG_IDF_TARGET_ESP32S3
    /* Configure the mode of instruction cache : cache size, cache line size. */
    extern void rom_config_instruction_cache_mode(uint32_t cfg_cache_size, uint8_t cfg_cache_ways, uint8_t cfg_cache_line_size);
    rom_config_instruction_cache_mode(CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE, CONFIG_ESP32S3_ICACHE_ASSOCIATED_WAYS, CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_SIZE);

    /* If we need use SPIRAM, we should use data cache.
       Configure the mode of data : cache size, cache line size.*/
    Cache_Suspend_DCache();
    extern void rom_config_data_cache_mode(uint32_t cfg_cache_size, uint8_t cfg_cache_ways, uint8_t cfg_cache_line_size);
    rom_config_data_cache_mode(CONFIG_ESP32S3_DATA_CACHE_SIZE, CONFIG_ESP32S3_DCACHE_ASSOCIATED_WAYS, CONFIG_ESP32S3_DATA_CACHE_LINE_SIZE);
    Cache_Resume_DCache(0);
#endif // CONFIG_IDF_TARGET_ESP32S3

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3
    /* Configure the Cache MMU size for instruction and rodata in flash. */
    extern uint32_t Cache_Set_IDROM_MMU_Size(uint32_t irom_size, uint32_t drom_size);
    extern int _rodata_reserved_start;
    uint32_t rodata_reserved_start_align = (uint32_t)&_rodata_reserved_start & ~(MMU_PAGE_SIZE - 1);
    uint32_t cache_mmu_irom_size = ((rodata_reserved_start_align - SOC_DROM_LOW) / MMU_PAGE_SIZE) * sizeof(uint32_t);
    Cache_Set_IDROM_MMU_Size(cache_mmu_irom_size, CACHE_DROM_MMU_MAX_END - cache_mmu_irom_size);
#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3

    bootloader_init_mem();
#if CONFIG_SPIRAM_BOOT_INIT
    if (esp_spiram_init() != ESP_OK) {
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
        ESP_EARLY_LOGE(TAG, "Failed to init external RAM, needed for external .bss segment");
        abort();
#endif
#endif

#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGI(TAG, "Failed to init external RAM; continuing without it.");
        g_spiram_ok = false;
#else
        ESP_EARLY_LOGE(TAG, "Failed to init external RAM!");
        abort();
#endif
    }
    if (g_spiram_ok) {
        esp_spiram_init_cache();
    }
#endif

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_cpu_up[0] = true;
#endif

    ESP_EARLY_LOGI(TAG, "Pro cpu up.");

#if SOC_CPU_CORES_NUM > 1 // there is no 'single-core mode' for natively single-core processors
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    start_other_core();
#else
    ESP_EARLY_LOGI(TAG, "Single core mode");
#if CONFIG_IDF_TARGET_ESP32
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN); // stop the other core
#elif CONFIG_IDF_TARGET_ESP32S3
    REG_CLR_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_CLKGATE_EN);
#endif
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#endif // SOC_CPU_CORES_NUM > 1

#if CONFIG_SPIRAM_MEMTEST
    if (g_spiram_ok) {
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

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    memset(&_ext_ram_bss_start, 0, (&_ext_ram_bss_end - &_ext_ram_bss_start) * sizeof(_ext_ram_bss_start));
#endif

//Enable trace memory and immediately start trace.
#if CONFIG_ESP32_TRAX || CONFIG_ESP32S2_TRAX
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ESP32_TRAX_TWOBANKS
    trax_enable(TRAX_ENA_PRO_APP);
#else
    trax_enable(TRAX_ENA_PRO);
#endif
#elif CONFIG_IDF_TARGET_ESP32S2
    trax_enable(TRAX_ENA_PRO);
#endif
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif // CONFIG_ESP32_TRAX || CONFIG_ESP32S2_TRAX

    esp_clk_init();
    esp_perip_clk_init();

    // Now that the clocks have been set-up, set the startup time from RTC
    // and default RTC-backed system time provider.
    g_startup_time = esp_rtc_get_time_us();

    intr_matrix_clear();

#ifdef CONFIG_ESP_CONSOLE_UART
    uint32_t clock_hz = rtc_clk_apb_freq_get();
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3
    clock_hz = UART_CLK_FREQ_ROM; // From esp32-s3 on, UART clock source is selected to XTAL in ROM
#endif
    esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_rom_uart_set_clock_baudrate(CONFIG_ESP_CONSOLE_UART_NUM, clock_hz, CONFIG_ESP_CONSOLE_UART_BAUDRATE);
#endif

#if SOC_RTCIO_HOLD_SUPPORTED
    rtcio_hal_unhold_all();
#else
    gpio_hal_force_unhold_all();
#endif

    esp_cache_err_int_init();

#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE_LOCK
    esp_memprot_set_prot(true, true, NULL);
#else
    esp_memprot_set_prot(true, false, NULL);
#endif
#endif

    bootloader_flash_update_id();
    // Read the application binary image header. This will also decrypt the header if the image is encrypted.
    __attribute__((unused)) esp_image_header_t fhdr = {0};
#ifdef CONFIG_APP_BUILD_TYPE_ELF_RAM
    fhdr.spi_mode = ESP_IMAGE_SPI_MODE_DIO;
    fhdr.spi_speed = ESP_IMAGE_SPI_SPEED_40M;
    fhdr.spi_size = ESP_IMAGE_FLASH_SIZE_4MB;

    extern void esp_rom_spiflash_attach(uint32_t, bool);
    esp_rom_spiflash_attach(esp_rom_efuse_get_flash_gpio_info(), false);
    esp_rom_spiflash_unlock();
#else
    // This assumes that DROM is the first segment in the application binary, i.e. that we can read
    // the binary header through cache by accessing SOC_DROM_LOW address.
    memcpy(&fhdr, (void *) SOC_DROM_LOW, sizeof(fhdr));
#endif // CONFIG_APP_BUILD_TYPE_ELF_RAM

#if CONFIG_IDF_TARGET_ESP32
#if !CONFIG_SPIRAM_BOOT_INIT
    // If psram is uninitialized, we need to improve some flash configuration.
    bootloader_flash_clock_config(&fhdr);
    bootloader_flash_gpio_config(&fhdr);
    bootloader_flash_dummy_config(&fhdr);
    bootloader_flash_cs_timing_config();
#endif //!CONFIG_SPIRAM_BOOT_INIT
#endif //CONFIG_IDF_TARGET_ESP32

#if CONFIG_SPI_FLASH_SIZE_OVERRIDE
    int app_flash_size = esp_image_get_flash_size(fhdr.spi_size);
    if (app_flash_size < 1 * 1024 * 1024) {
        ESP_LOGE(TAG, "Invalid flash size in app image header.");
        abort();
    }
    bootloader_flash_update_size(app_flash_size);
#endif //CONFIG_SPI_FLASH_SIZE_OVERRIDE

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_cpu_inited[0] = true;

    volatile bool cpus_inited = false;

    while (!cpus_inited) {
        cpus_inited = true;
        for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
            cpus_inited &= s_cpu_inited[i];
        }
        esp_rom_delay_us(100);
    }
#endif

#ifdef ROM_LOG_MODE
    esp_efuse_set_rom_log_scheme(ROM_LOG_MODE);
#endif

    SYS_STARTUP_FN();
}
