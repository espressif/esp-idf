/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "esp_attr.h"
#include "esp_err.h"

#include "esp_log.h"
#include "esp_chip_info.h"

#include "esp_private/cache_err_int.h"
#include "esp_clk_internal.h"

#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_rom_caps.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#include "esp32/rtc.h"
#include "esp32/rom/cache.h"
#include "esp32/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rtc.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/secure_boot.h"
#include "esp32s2/memprot.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rtc.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/secure_boot.h"
#include "esp_memprot.h"
#include "soc/assist_debug_reg.h"
#include "soc/system_reg.h"
#include "esp32s3/rom/opi_flash.h"
#include "hal/cache_hal.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rtc.h"
#include "esp32c3/rom/cache.h"
#include "esp32c3/rom/secure_boot.h"
#include "esp_memprot.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rtc.h"
#include "esp32c6/rom/cache.h"
#include "esp_memprot.h"
#elif CONFIG_IDF_TARGET_ESP32C61
#include "esp32c61/rtc.h"
#include "esp_memprot.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rtc.h"
#include "esp32c5/rom/cache.h"
#include "esp_memprot.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rtc.h"
#include "esp32h2/rom/cache.h"
#include "esp_memprot.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rtc.h"
#include "esp32c2/rom/cache.h"
#include "esp32c2/rom/rtc.h"
#include "esp32c2/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rtc.h"
#include "soc/hp_sys_clkrst_reg.h"
#endif

#if SOC_KEY_MANAGER_SUPPORTED
#include "hal/key_mgr_hal.h"
#endif

#include "esp_private/rtc_clk.h"

#if SOC_INT_CLIC_SUPPORTED
#include "hal/interrupt_clic_ll.h"
#endif // SOC_INT_CLIC_SUPPORTED

#include "esp_private/esp_mmu_map_private.h"
#include "esp_private/image_process.h"
#if CONFIG_SPIRAM
#include "esp_psram.h"
#include "esp_private/mmu_psram_flash.h"
#include "esp_private/esp_psram_extram.h"
#endif

#include "esp_private/spi_flash_os.h"
#include "esp_private/mspi_timing_tuning.h"
#include "bootloader_flash_config.h"
#include "bootloader_flash.h"
#include "esp_private/crosscore_int.h"

#include "esp_private/sleep_gpio.h"
#include "hal/wdt_hal.h"
#include "soc/rtc.h"
#include "hal/cache_ll.h"
#include "hal/efuse_ll.h"
#include "soc/periph_defs.h"
#include "esp_cpu.h"
#include "esp_private/esp_clk.h"

#if CONFIG_ESP32_TRAX || CONFIG_ESP32S2_TRAX || CONFIG_ESP32S3_TRAX
#include "esp_private/trax.h"
#endif

#include "bootloader_mem.h"

#if CONFIG_APP_BUILD_TYPE_RAM
#include "esp_rom_spiflash.h"
#include "bootloader_init.h"
#include "esp_private/bootloader_flash_internal.h"
#include "spi_flash_mmap.h"
#endif // CONFIG_APP_BUILD_TYPE_RAM

//This dependency will be removed in the future
#include "soc/ext_mem_defs.h"

#include "esp_private/startup_internal.h"
#include "esp_private/system_internal.h"

#if SOC_MEM_NON_CONTIGUOUS_SRAM
extern int _bss_start_low, _bss_start_high;
extern int _bss_end_low, _bss_end_high;
#else
extern int _bss_start;
extern int _bss_end;
#endif // SOC_MEM_NON_CONTIGUOUS_SRAM
extern int _rtc_bss_start;
extern int _rtc_bss_end;
#if CONFIG_BT_LE_RELEASE_IRAM_SUPPORTED
extern int _bss_bt_start;
extern int _bss_bt_end;
#endif // CONFIG_BT_LE_RELEASE_IRAM_SUPPORTED
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;

extern int _vector_table;
#if SOC_INT_CLIC_SUPPORTED
extern int _mtvt_table;
#endif

static const char *TAG = "cpu_start";

#ifdef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
extern int _iram_bss_start;
extern int _iram_bss_end;
#endif

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
static volatile bool s_cpu_up[SOC_CPU_CORES_NUM] = { false };
static volatile bool s_cpu_inited[SOC_CPU_CORES_NUM] = { false };

static volatile bool s_resume_cores;
#endif

static void core_intr_matrix_clear(void)
{
    uint32_t core_id = esp_cpu_get_core_id();

    for (int i = 0; i < ETS_MAX_INTR_SOURCE; i++) {
#if SOC_INT_CLIC_SUPPORTED
        interrupt_clic_ll_route(core_id, i, ETS_INVALID_INUM);
#else
        esp_rom_route_intr_matrix(core_id, i, ETS_INVALID_INUM);
#endif  // SOC_INT_CLIC_SUPPORTED
    }

#if SOC_INT_CLIC_SUPPORTED
    for (int i = 0; i < 32; i++) {
        /* Set all the CPU interrupt lines to vectored by default, as it is on other RISC-V targets */
        esprv_int_set_vectored(i, true);
    }
#endif // SOC_INT_CLIC_SUPPORTED
}

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
void startup_resume_other_cores(void)
{
    s_resume_cores = true;
}

void IRAM_ATTR call_start_cpu1(void)
{
#ifdef __riscv
    // Configure the global pointer register
    // (This should be the first thing IDF app does, as any other piece of code could be
    // relaxed by the linker to access something relative to __global_pointer$)
    __asm__ __volatile__(
        ".option push\n"
        ".option norelax\n"
        "la gp, __global_pointer$\n"
        ".option pop"
    );
#endif  //#ifdef __riscv

#if SOC_BRANCH_PREDICTOR_SUPPORTED
    esp_cpu_branch_prediction_enable();
#endif  //#if SOC_BRANCH_PREDICTOR_SUPPORTED

    esp_cpu_intr_set_ivt_addr(&_vector_table);
#if SOC_INT_CLIC_SUPPORTED
    /* When hardware vectored interrupts are enabled in CLIC,
     * the CPU jumps to this base address + 4 * interrupt_id.
     */
    esp_cpu_intr_set_mtvt_addr(&_mtvt_table);
#endif

    ets_set_appcpu_boot_addr(0);

    bootloader_init_mem();

#if CONFIG_ESP_CONSOLE_NONE
    esp_rom_install_channel_putc(1, NULL);
    esp_rom_install_channel_putc(2, NULL);
#elif !CONFIG_ESP_CONSOLE_USB_CDC
    esp_rom_install_uart_printf();
    esp_rom_output_set_as_console(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
#endif

#if CONFIG_IDF_TARGET_ESP32
    DPORT_REG_SET_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_PDEBUG_ENABLE | DPORT_APP_CPU_RECORD_ENABLE);
    DPORT_REG_CLR_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_RECORD_ENABLE);
#elif CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7688
#else
    REG_WRITE(ASSIST_DEBUG_CORE_1_RCD_PDEBUGENABLE_REG, 1);
    REG_WRITE(ASSIST_DEBUG_CORE_1_RCD_RECORDING_REG, 1);
#endif

    s_cpu_up[1] = true;
    ESP_EARLY_LOGD(TAG, "App cpu up");

    // Clear interrupt matrix for APP CPU core
    core_intr_matrix_clear();

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    //Take care putting stuff here: if asked, FreeRTOS will happily tell you the scheduler
    //has started, but it isn't active *on this CPU* yet.
    esp_cache_err_int_init();
#endif

#if (CONFIG_IDF_TARGET_ESP32 && CONFIG_ESP32_TRAX_TWOBANKS) || \
    (CONFIG_IDF_TARGET_ESP32S3 && CONFIG_ESP32S3_TRAX_TWOBANKS)
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
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

    ESP_EARLY_LOGD(TAG, "Starting app cpu, entry point is %p", call_start_cpu1);

#if CONFIG_IDF_TARGET_ESP32 && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    Cache_Flush(1);
    Cache_Read_Enable(1);
#endif // #if CONFIG_IDF_TARGET_ESP32 && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

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
#elif CONFIG_IDF_TARGET_ESP32P4
    if (!REG_GET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN)) {
        REG_SET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN);
    }
    if (REG_GET_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_CORE1_GLOBAL)) {
        REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_CORE1_GLOBAL);
    }
#endif

#if SOC_KEY_MANAGER_SUPPORTED
    // The following operation makes the Key Manager to use eFuse key for ECDSA and XTS-AES operation by default
    // This is to keep the default behavior same as the other chips
    // If the Key Manager configuration is already locked then following operation does not have any effect
    key_mgr_hal_set_key_usage(ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
    key_mgr_hal_set_key_usage(ESP_KEY_MGR_XTS_AES_128_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
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

#if !SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#if CONFIG_IDF_TARGET_ESP32
static void restore_app_mmu_from_pro_mmu(void)
{
    const int mmu_reg_num = 2048;
    volatile uint32_t* from = (uint32_t*)DR_REG_FLASH_MMU_TABLE_PRO;
    volatile uint32_t* to = (uint32_t*)DR_REG_FLASH_MMU_TABLE_APP;
    for (int i = 0; i < mmu_reg_num; i++) {
        *(to++) = *(from++);
    }
}
#endif
// This function is needed to make the multicore app runnable on a unicore bootloader (built with FREERTOS UNICORE).
// It does some cache settings for other CPUs.
void IRAM_ATTR do_multicore_settings(void)
{
    // We intentionally do not check the cache settings before changing them,
    // because it helps to get the application to run on older bootloaders.
#ifdef CONFIG_IDF_TARGET_ESP32
    if (!efuse_ll_get_disable_app_cpu()) {
        Cache_Read_Disable(1);
        Cache_Flush(1);
        DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
        mmu_init(1);
        DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
        // We do not enable cache for CPU1 now because it will be done later in start_other_core().
    }
    restore_app_mmu_from_pro_mmu();
#endif

    cache_bus_mask_t cache_bus_mask_core0 = cache_ll_l1_get_enabled_bus(0);
#ifndef CONFIG_IDF_TARGET_ESP32
    // 1. disable the cache before changing its settings.
    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif
    for (unsigned core = 1; core < SOC_CPU_CORES_NUM; core++) {
        // 2. change cache settings. All cores must have the same settings.
        cache_ll_l1_enable_bus(core, cache_bus_mask_core0);
    }
#ifndef CONFIG_IDF_TARGET_ESP32
    // 3. enable the cache after changing its settings.
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif
}
#endif // !SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE

/*
 * We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
 * and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
 */
void IRAM_ATTR call_start_cpu0(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    soc_reset_reason_t rst_reas[SOC_CPU_CORES_NUM];
#else
    soc_reset_reason_t __attribute__((unused)) rst_reas[1];
#endif

#ifdef __riscv
    if (esp_cpu_dbgr_is_attached()) {
        /* Let debugger some time to detect that target started, halt it, enable ebreaks and resume.
           500ms should be enough. */
        for (uint32_t ms_num = 0; ms_num < 2; ms_num++) {
            esp_rom_delay_us(100000);
        }
    }
    // Configure the global pointer register
    // (This should be the first thing IDF app does, as any other piece of code could be
    // relaxed by the linker to access something relative to __global_pointer$)
    __asm__ __volatile__(
        ".option push\n"
        ".option norelax\n"
        "la gp, __global_pointer$\n"
        ".option pop"
    );
#endif

#if SOC_BRANCH_PREDICTOR_SUPPORTED
    esp_cpu_branch_prediction_enable();
#endif
    // Move exception vectors to IRAM
    esp_cpu_intr_set_ivt_addr(&_vector_table);
#if SOC_INT_CLIC_SUPPORTED
    /* When hardware vectored interrupts are enabled in CLIC,
     * the CPU jumps to this base address + 4 * interrupt_id.
     */
    esp_cpu_intr_set_mtvt_addr(&_mtvt_table);
#endif

    rst_reas[0] = esp_rom_get_reset_reason(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    rst_reas[1] = esp_rom_get_reset_reason(1);
#endif

    //Clear BSS. Please do not attempt to do any complex stuff (like early logging) before this.
#if SOC_MEM_NON_CONTIGUOUS_SRAM
    memset(&_bss_start_low, 0, (&_bss_end_low - &_bss_start_low) * sizeof(_bss_start_low));
    memset(&_bss_start_high, 0, (&_bss_end_high - &_bss_start_high) * sizeof(_bss_start_high));
#else
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));
#endif // SOC_MEM_NON_CONTIGUOUS_SRAM

#if CONFIG_BT_LE_RELEASE_IRAM_SUPPORTED
    // Clear Bluetooth bss
    memset(&_bss_bt_start, 0, (&_bss_bt_end - &_bss_bt_start) * sizeof(_bss_bt_start));
#endif // CONFIG_BT_LE_RELEASE_IRAM_SUPPORTED

#if defined(CONFIG_IDF_TARGET_ESP32) && defined(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
    // Clear IRAM BSS
    memset(&_iram_bss_start, 0, (&_iram_bss_end - &_iram_bss_start) * sizeof(_iram_bss_start));
#endif

#if SOC_RTC_FAST_MEM_SUPPORTED || SOC_RTC_SLOW_MEM_SUPPORTED
    /* Unless waking from deep sleep (implying RTC memory is intact), clear RTC bss */
    if (rst_reas[0] != RESET_REASON_CORE_DEEP_SLEEP) {
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start) * sizeof(_rtc_bss_start));
    }
#endif

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    // It helps to fix missed cache settings for other cores. It happens when bootloader is unicore.
    do_multicore_settings();
#endif

    // When the APP is loaded into ram for execution, some hardware initialization behaviors
    // in the bootloader are still necessary
#if CONFIG_APP_BUILD_TYPE_RAM
    bootloader_init();
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    bootloader_flash_hardware_init();
#endif  //#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
#endif  //#if CONFIG_APP_BUILD_TYPE_RAM

#if CONFIG_IDF_TARGET_ESP32P4
#define RWDT_RESET           RESET_REASON_CORE_RWDT
#define MWDT_RESET           RESET_REASON_CORE_MWDT
#else
#define RWDT_RESET           RESET_REASON_CORE_RTC_WDT
#define MWDT_RESET           RESET_REASON_CORE_MWDT0
#endif

#ifndef CONFIG_BOOTLOADER_WDT_ENABLE
    // from panic handler we can be reset by RWDT or TG0WDT
    if (rst_reas[0] == RWDT_RESET || rst_reas[0] == MWDT_RESET
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
            || rst_reas[1] == RWDT_RESET || rst_reas[1] == MWDT_RESET
#endif
       ) {
        wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
#endif

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
#if CONFIG_IDF_TARGET_ESP32S2
    /* Configure the mode of instruction cache : cache size, cache associated ways, cache line size. */
    extern void esp_config_instruction_cache_mode(void);
    esp_config_instruction_cache_mode();

    /* If we need use SPIRAM, we should use data cache, or if we want to access rodata, we also should use data cache.
       Configure the mode of data : cache size, cache associated ways, cache line size.
       Enable data cache, so if we don't use SPIRAM, it just works. */
    extern void esp_config_data_cache_mode(void);
    esp_config_data_cache_mode();
    Cache_Enable_DCache(0);
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

#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-5670, add cache init API
    extern void esp_config_l2_cache_mode(void);
    esp_config_l2_cache_mode();
#endif

#if ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE
#if CONFIG_APP_BUILD_TYPE_ELF_RAM
    // For RAM loadable ELF case, we don't need to reserve IROM/DROM as instructions and data
    // are all in internal RAM. If the RAM loadable ELF has any requirement to memory map the
    // external flash then it should use flash or partition mmap APIs.
    uint32_t cache_mmu_irom_size = 0;
    __attribute__((unused)) uint32_t cache_mmu_drom_size = 0;
#else // CONFIG_APP_BUILD_TYPE_ELF_RAM
    uint32_t _instruction_size = (uint32_t)&_instruction_reserved_end - (uint32_t)&_instruction_reserved_start;
    uint32_t cache_mmu_irom_size = ((_instruction_size + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE) * sizeof(uint32_t);

    uint32_t _rodata_size = (uint32_t)&_rodata_reserved_end - (uint32_t)&_rodata_reserved_start;
    __attribute__((unused)) uint32_t cache_mmu_drom_size = ((_rodata_size + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE) * sizeof(uint32_t);
#endif // !CONFIG_APP_BUILD_TYPE_ELF_RAM

    /* Configure the Cache MMU size for instruction and rodata in flash. */
    Cache_Set_IDROM_MMU_Size(cache_mmu_irom_size, CACHE_DROM_MMU_MAX_END - cache_mmu_irom_size);
#endif // ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE

#if CONFIG_ESPTOOLPY_OCT_FLASH && !CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT
    bool efuse_opflash_en = efuse_ll_get_flash_type();
    if (!efuse_opflash_en) {
        ESP_DRAM_LOGE(TAG, "Octal Flash option selected, but EFUSE not configured!");
        abort();
    }
#endif

    esp_mspi_pin_init();
    // For Octal flash, it's hard to implement a read_id function in OPI mode for all vendors.
    // So we have to read it here in SPI mode, before entering the OPI mode.
    bootloader_flash_update_id();

    // Configure the power related stuff. After this the MSPI timing tuning can be done.
    esp_rtc_init();

    /**
     * This function initialise the Flash chip to the user-defined settings.
     *
     * In bootloader, we only init Flash (and MSPI) to a preliminary state, for being flexible to
     * different chips.
     * In this stage, we re-configure the Flash (and MSPI) to required configuration
     */
    spi_flash_init_chip_state();
#if SOC_MEMSPI_SRC_FREQ_120M
    // This function needs to be called when PLL is enabled. Needs to be called after spi_flash_init_chip_state in case
    // some state of flash is modified.
    mspi_timing_flash_tuning();
#endif

    esp_mmu_map_init();

#if !CONFIG_APP_BUILD_TYPE_ELF_RAM
#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
    ESP_ERROR_CHECK(image_process());
#endif
#endif

#if CONFIG_SPIRAM_BOOT_INIT
    if (esp_psram_init() != ESP_OK) {
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
        ESP_DRAM_LOGE(TAG, "Failed to init external RAM, needed for external .bss segment");
        abort();
#endif

#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGI(TAG, "Failed to init external RAM; continuing without it.");
#else
        ESP_DRAM_LOGE(TAG, "Failed to init external RAM!");
        abort();
#endif
    }
#endif

    //----------------------------------Separator-----------------------------//
    /**
     * @note
     * After this stage, you can access the flash through the cache, i.e. run code which is not placed in IRAM
     * or print string which locates on flash
     */
    esp_mspi_pin_reserve();

#endif // !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    ESP_EARLY_LOGI(TAG, "Unicore app");
#else
    ESP_EARLY_LOGI(TAG, "Multicore app");
#endif

    bootloader_init_mem();

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_cpu_up[0] = true;
#endif

    ESP_EARLY_LOGD(TAG, "Pro cpu up");

#if SOC_CPU_CORES_NUM > 1 // there is no 'single-core mode' for natively single-core processors
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    start_other_core();
#else
    ESP_EARLY_LOGI(TAG, "Single core mode");
#if CONFIG_IDF_TARGET_ESP32
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN); // stop the other core
#elif CONFIG_IDF_TARGET_ESP32S3
    REG_CLR_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_CLKGATE_EN);
#if SOC_APPCPU_HAS_CLOCK_GATING_BUG
    /* The clock gating signal of the App core is invalid. We use RUNSTALL and RESETTING
       signals to ensure that the App core stops running in single-core mode. */
    REG_SET_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_RUNSTALL);
    REG_CLR_BIT(SYSTEM_CORE_1_CONTROL_0_REG, SYSTEM_CONTROL_CORE_1_RESETING);
#endif
#endif // CONFIG_IDF_TARGET_ESP32
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#endif // SOC_CPU_CORES_NUM > 1

#if CONFIG_SPIRAM_MEMTEST
    if (esp_psram_is_initialized()) {
        bool ext_ram_ok = esp_psram_extram_test();
        if (!ext_ram_ok) {
            ESP_EARLY_LOGE(TAG, "External RAM failed memory test!");
            abort();
        }
    }
#endif  //CONFIG_SPIRAM_MEMTEST

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
//TODO: IDF-5023, replace with MMU driver
#if CONFIG_IDF_TARGET_ESP32S3
    int s_instr_flash2spiram_off = 0;
    int s_rodata_flash2spiram_off = 0;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    s_instr_flash2spiram_off = instruction_flash2spiram_offset();
#endif
#if CONFIG_SPIRAM_RODATA
    s_rodata_flash2spiram_off = rodata_flash2spiram_offset();
#endif
    Cache_Set_IDROM_MMU_Info(cache_mmu_irom_size / sizeof(uint32_t), \
                             cache_mmu_drom_size / sizeof(uint32_t), \
                             (uint32_t)&_rodata_reserved_start, \
                             (uint32_t)&_rodata_reserved_end, \
                             s_instr_flash2spiram_off, \
                             s_rodata_flash2spiram_off);
#endif // CONFIG_IDF_TARGET_ESP32S3

#if CONFIG_ESP32S2_INSTRUCTION_CACHE_WRAP || CONFIG_ESP32S2_DATA_CACHE_WRAP || \
    CONFIG_ESP32S3_INSTRUCTION_CACHE_WRAP || CONFIG_ESP32S3_DATA_CACHE_WRAP
    uint32_t icache_wrap_enable = 0, dcache_wrap_enable = 0;
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_WRAP || CONFIG_ESP32S3_INSTRUCTION_CACHE_WRAP
    icache_wrap_enable = 1;
#endif
#if CONFIG_ESP32S2_DATA_CACHE_WRAP || CONFIG_ESP32S3_DATA_CACHE_WRAP
    dcache_wrap_enable = 1;
#endif
    extern void esp_enable_cache_wrap(uint32_t icache_wrap_enable, uint32_t dcache_wrap_enable);
    esp_enable_cache_wrap(icache_wrap_enable, dcache_wrap_enable);
#endif

#if CONFIG_ESP32S3_DATA_CACHE_16KB
    Cache_Invalidate_DCache_All();
    Cache_Occupy_Addr(SOC_DROM_LOW, 0x4000);
#endif

#if CONFIG_IDF_TARGET_ESP32C2
// TODO : IDF-5020
#if CONFIG_ESP32C2_INSTRUCTION_CACHE_WRAP
    extern void esp_enable_cache_wrap(uint32_t icache_wrap_enable);
    esp_enable_cache_wrap(1);
#endif
#endif
#endif // !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    esp_psram_bss_init();
#endif

//Enable trace memory and immediately start trace.
#if CONFIG_ESP32_TRAX || CONFIG_ESP32S2_TRAX || CONFIG_ESP32S3_TRAX
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#if CONFIG_ESP32_TRAX_TWOBANKS || CONFIG_ESP32S3_TRAX_TWOBANKS
    trax_enable(TRAX_ENA_PRO_APP);
#else
    trax_enable(TRAX_ENA_PRO);
#endif
#elif CONFIG_IDF_TARGET_ESP32S2
    trax_enable(TRAX_ENA_PRO);
#endif
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif // CONFIG_ESP32_TRAX || CONFIG_ESP32S2_TRAX || CONFIG_ESP32S3_TRAX

    esp_clk_init();
    esp_perip_clk_init();

    // Now that the clocks have been set-up, set the startup time from RTC
    // and default RTC-backed system time provider.
    g_startup_time = esp_rtc_get_time_us();

    // Clear interrupt matrix for PRO CPU core
    core_intr_matrix_clear();

#ifndef CONFIG_IDF_ENV_FPGA // TODO: on FPGA it should be possible to configure this, not currently working with APB_CLK_FREQ changed
#ifdef CONFIG_ESP_CONSOLE_UART
    uint32_t clock_hz = esp_clk_apb_freq();
#if ESP_ROM_UART_CLK_IS_XTAL
    clock_hz = esp_clk_xtal_freq(); // From esp32-s3 on, UART clock source is selected to XTAL in ROM
#endif
    esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);

    // In a single thread mode, the freertos is not started yet. So don't have to use a critical section.
    int __DECLARE_RCC_ATOMIC_ENV __attribute__((unused));  // To avoid build errors about spinlock's __DECLARE_RCC_ATOMIC_ENV
    esp_rom_uart_set_clock_baudrate(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM, clock_hz, CONFIG_ESP_CONSOLE_UART_BAUDRATE);
#endif
#endif

#if SOC_DEEP_SLEEP_SUPPORTED //TODO: IDF-8638, IDF-9245
    // Need to unhold the IOs that were hold right before entering deep sleep, which are used as wakeup pins
    if (rst_reas[0] == RESET_REASON_CORE_DEEP_SLEEP) {
        esp_deep_sleep_wakeup_io_reset();
    }
#endif  //#if SOC_DEEP_SLEEP_SUPPORTED

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    esp_cache_err_int_init();
#endif

#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE && !CONFIG_ESP_SYSTEM_MEMPROT_TEST
    // Memprot cannot be locked during OS startup as the lock-on prevents any PMS changes until a next reboot
    // If such a situation appears, it is likely an malicious attempt to bypass the system safety setup -> print error & reset

#if CONFIG_IDF_TARGET_ESP32S2
    if (esp_memprot_is_locked_any()) {
#else
    bool is_locked = false;
    if (esp_mprot_is_conf_locked_any(&is_locked) != ESP_OK || is_locked) {
#endif
        ESP_EARLY_LOGE(TAG, "Memprot feature locked after the system reset! Potential safety corruption, rebooting.");
        esp_restart_noos();
    }

    //default configuration of PMS Memprot
    esp_err_t memp_err = ESP_OK;
#if CONFIG_IDF_TARGET_ESP32S2 //specific for ESP32S2 unless IDF-3024 is merged
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE_LOCK
    memp_err = esp_memprot_set_prot(PANIC_HNDL_ON, MEMPROT_LOCK, NULL);
#else
    memp_err = esp_memprot_set_prot(PANIC_HNDL_ON, MEMPROT_UNLOCK, NULL);
#endif
#else //CONFIG_IDF_TARGET_ESP32S2 specific end
    esp_memp_config_t memp_cfg = ESP_MEMPROT_DEFAULT_CONFIG();
#if !CONFIG_ESP_SYSTEM_MEMPROT_FEATURE_LOCK
    memp_cfg.lock_feature = false;
#endif
    memp_err = esp_mprot_set_prot(&memp_cfg);
#endif //other IDF_TARGETS end

    if (memp_err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to set Memprot feature (0x%08X: %s), rebooting.", memp_err, esp_err_to_name(memp_err));
        esp_restart_noos();
    }
#endif //CONFIG_ESP_SYSTEM_MEMPROT_FEATURE && !CONFIG_ESP_SYSTEM_MEMPROT_TEST

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    // External devices (including SPI0/1, cache) should be initialized

#if !CONFIG_APP_BUILD_TYPE_RAM
    // Normal startup flow. We arrive here with the help of 1st, 2nd bootloader. There are valid headers (app/bootloader)

    // Read the application binary image header. This will also decrypt the header if the image is encrypted.
    __attribute__((unused)) esp_image_header_t fhdr = {0};

    // This assumes that DROM is the first segment in the application binary, i.e. that we can read
    // the binary header through cache by accessing SOC_DROM_LOW address.
    hal_memcpy(&fhdr, (void *) SOC_DROM_LOW, sizeof(fhdr));

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
        ESP_EARLY_LOGE(TAG, "Invalid flash size in app image header.");
        abort();
    }
    bootloader_flash_update_size(app_flash_size);
#endif //CONFIG_SPI_FLASH_SIZE_OVERRIDE
#else
    // CONFIG_APP_BUILD_TYPE_RAM && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    bootloader_flash_unlock();
#endif
#endif //!CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

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

    SYS_STARTUP_FN();
}
