/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"

#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_common.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"

#include "esp_cpu.h"
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include "hal/gpio_hal.h"
#include "xtensa/config/core.h"
#include "xt_instr_macros.h"

#include "esp32/rom/cache.h"
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "esp_efuse.h"

static const char *TAG = "boot.esp32";

#define FLASH_CLK_IO SPI_CLK_GPIO_NUM
#define FLASH_CS_IO SPI_CS0_GPIO_NUM
#define FLASH_SPIQ_IO SPI_Q_GPIO_NUM
#define FLASH_SPID_IO SPI_D_GPIO_NUM
#define FLASH_SPIWP_IO SPI_WP_GPIO_NUM
#define FLASH_SPIHD_IO SPI_HD_GPIO_NUM

void bootloader_configure_spi_pins(int drv)
{
    uint32_t pkg_ver = bootloader_common_get_chip_ver_pkg();

    if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5 ||
        pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2 ||
        pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 ||
        pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        // For ESP32D2WD or ESP32-PICO series,the SPI pins are already configured
        // flash clock signal should come from IO MUX.
        gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, drv, FUN_DRV_S);
    } else {
        const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
        if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
            esp_rom_gpio_connect_out_signal(FLASH_CS_IO, SPICS0_OUT_IDX, 0, 0);
            esp_rom_gpio_connect_out_signal(FLASH_SPIQ_IO, SPIQ_OUT_IDX, 0, 0);
            esp_rom_gpio_connect_in_signal(FLASH_SPIQ_IO, SPIQ_IN_IDX, 0);
            esp_rom_gpio_connect_out_signal(FLASH_SPID_IO, SPID_OUT_IDX, 0, 0);
            esp_rom_gpio_connect_in_signal(FLASH_SPID_IO, SPID_IN_IDX, 0);
            esp_rom_gpio_connect_out_signal(FLASH_SPIWP_IO, SPIWP_OUT_IDX, 0, 0);
            esp_rom_gpio_connect_in_signal(FLASH_SPIWP_IO, SPIWP_IN_IDX, 0);
            esp_rom_gpio_connect_out_signal(FLASH_SPIHD_IO, SPIHD_OUT_IDX, 0, 0);
            esp_rom_gpio_connect_in_signal(FLASH_SPIHD_IO, SPIHD_IN_IDX, 0);
            //select pin function gpio
            gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_DATA0_U, PIN_FUNC_GPIO);
            gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_DATA1_U, PIN_FUNC_GPIO);
            gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_DATA2_U, PIN_FUNC_GPIO);
            gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_DATA3_U, PIN_FUNC_GPIO);
            gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_CMD_U, PIN_FUNC_GPIO);
            // flash clock signal should come from IO MUX.
            // set drive ability for clock
            gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, drv, FUN_DRV_S);

#if CONFIG_SPIRAM_TYPE_ESPPSRAM32 || CONFIG_SPIRAM_TYPE_ESPPSRAM64
            uint32_t flash_id = g_rom_flashchip.device_id;
            if (flash_id == FLASH_ID_GD25LQ32C) {
                // Set drive ability for 1.8v flash in 80Mhz.
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA0_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA1_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA2_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_DATA3_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CMD_U, FUN_DRV, 3, FUN_DRV_S);
                SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, 3, FUN_DRV_S);
            }
#endif
        }
    }
}

static void bootloader_reset_mmu(void)
{
    /* completely reset MMU in case serial bootloader was running */
    Cache_Read_Disable(0);
#if !CONFIG_FREERTOS_UNICORE
    Cache_Read_Disable(1);
#endif
    Cache_Flush(0);
#if !CONFIG_FREERTOS_UNICORE
    Cache_Flush(1);
#endif
    mmu_init(0);
#if !CONFIG_FREERTOS_UNICORE
    /* The lines which manipulate DPORT_APP_CACHE_MMU_IA_CLR bit are
        necessary to work around a hardware bug. */
    DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
    mmu_init(1);
    DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
#endif

    /* normal ROM boot exits with DROM0 cache unmasked,
        but serial bootloader exits with it masked. */
    DPORT_REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MASK_DROM0);
#if !CONFIG_FREERTOS_UNICORE
    DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DROM0);
#endif
}

static esp_err_t bootloader_check_rated_cpu_clock(void)
{
    int rated_freq = bootloader_clock_get_rated_freq_mhz();
    if (rated_freq < CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ) {
        ESP_LOGE(TAG, "Chip CPU frequency rated for %dMHz, configured for %dMHz. Modify CPU frequency in menuconfig",
                 rated_freq, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void update_flash_config(const esp_image_header_t *bootloader_hdr)
{
    uint32_t size;
    switch (bootloader_hdr->spi_size) {
    case ESP_IMAGE_FLASH_SIZE_1MB:
        size = 1;
        break;
    case ESP_IMAGE_FLASH_SIZE_2MB:
        size = 2;
        break;
    case ESP_IMAGE_FLASH_SIZE_4MB:
        size = 4;
        break;
    case ESP_IMAGE_FLASH_SIZE_8MB:
        size = 8;
        break;
    case ESP_IMAGE_FLASH_SIZE_16MB:
        size = 16;
        break;
    default:
        size = 2;
    }
    Cache_Read_Disable(0);
    // Set flash chip size
    esp_rom_spiflash_config_param(g_rom_flashchip.device_id, size * 0x100000, 0x10000, 0x1000, 0x100, 0xffff);
    // TODO: set mode
    // TODO: set frequency
    Cache_Flush(0);
    Cache_Read_Enable(0);
}

static void print_flash_info(const esp_image_header_t *bootloader_hdr)
{
    ESP_LOGD(TAG, "magic %02x", bootloader_hdr->magic);
    ESP_LOGD(TAG, "segments %02x", bootloader_hdr->segment_count);
    ESP_LOGD(TAG, "spi_mode %02x", bootloader_hdr->spi_mode);
    ESP_LOGD(TAG, "spi_speed %02x", bootloader_hdr->spi_speed);
    ESP_LOGD(TAG, "spi_size %02x", bootloader_hdr->spi_size);

    const char *str;
    switch (bootloader_hdr->spi_speed) {
    case ESP_IMAGE_SPI_SPEED_DIV_2:
        str = "40MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_3:
        str = "26.7MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_4:
        str = "20MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_1:
        str = "80MHz";
        break;
    default:
        str = "20MHz";
        break;
    }
    ESP_LOGI(TAG, "SPI Speed      : %s", str);

    /* SPI mode could have been set to QIO during boot already,
       so test the SPI registers not the flash header */
    uint32_t spi_ctrl = REG_READ(SPI_CTRL_REG(0));
    if (spi_ctrl & SPI_FREAD_QIO) {
        str = "QIO";
    } else if (spi_ctrl & SPI_FREAD_QUAD) {
        str = "QOUT";
    } else if (spi_ctrl & SPI_FREAD_DIO) {
        str = "DIO";
    } else if (spi_ctrl & SPI_FREAD_DUAL) {
        str = "DOUT";
    } else if (spi_ctrl & SPI_FASTRD_MODE) {
        str = "FAST READ";
    } else {
        str = "SLOW READ";
    }
    ESP_LOGI(TAG, "SPI Mode       : %s", str);

    switch (bootloader_hdr->spi_size) {
    case ESP_IMAGE_FLASH_SIZE_1MB:
        str = "1MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_2MB:
        str = "2MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_4MB:
        str = "4MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_8MB:
        str = "8MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_16MB:
        str = "16MB";
        break;
    default:
        str = "2MB";
        break;
    }
    ESP_LOGI(TAG, "SPI Flash Size : %s", str);
}

static void IRAM_ATTR bootloader_init_flash_configure(void)
{
    bootloader_flash_gpio_config(&bootloader_image_hdr);
    bootloader_flash_dummy_config(&bootloader_image_hdr);
    bootloader_flash_cs_timing_config();
}

static esp_err_t bootloader_init_spi_flash(void)
{
    bootloader_init_flash_configure();
#ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig != ESP_ROM_EFUSE_FLASH_DEFAULT_SPI && spiconfig != ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI) {
        ESP_LOGE(TAG, "SPI flash pins are overridden. Enable CONFIG_SPI_FLASH_ROM_DRIVER_PATCH in menuconfig");
        return ESP_FAIL;
    }
#endif

    bootloader_flash_unlock();

#if CONFIG_ESPTOOLPY_FLASHMODE_QIO || CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    bootloader_enable_qio_mode();
#endif

    print_flash_info(&bootloader_image_hdr);
    update_flash_config(&bootloader_image_hdr);
    //ensure the flash is write-protected
    bootloader_enable_wp();
    return ESP_OK;
}

static void wdt_reset_cpu0_info_enable(void)
{
    //We do not reset core1 info here because it didn't work before cpu1 was up. So we put it into call_start_cpu1.
    DPORT_REG_SET_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_PDEBUG_ENABLE | DPORT_PRO_CPU_RECORD_ENABLE);
    DPORT_REG_CLR_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_RECORD_ENABLE);
}

static void wdt_reset_info_dump(int cpu)
{
    uint32_t inst = 0, pid = 0, stat = 0, data = 0, pc = 0,
             lsstat = 0, lsaddr = 0, lsdata = 0, dstat = 0;
    const char *cpu_name = cpu ? "APP" : "PRO";

    if (cpu == 0) {
        stat = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_STATUS_REG);
        pid = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PID_REG);
        inst = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGINST_REG);
        dstat = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGSTATUS_REG);
        data = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGDATA_REG);
        pc = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGPC_REG);
        lsstat = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGLS0STAT_REG);
        lsaddr = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGLS0ADDR_REG);
        lsdata = DPORT_REG_READ(DPORT_PRO_CPU_RECORD_PDEBUGLS0DATA_REG);
    } else {
#if !CONFIG_FREERTOS_UNICORE
        stat = DPORT_REG_READ(DPORT_APP_CPU_RECORD_STATUS_REG);
        pid = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PID_REG);
        inst = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGINST_REG);
        dstat = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGSTATUS_REG);
        data = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGDATA_REG);
        pc = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGPC_REG);
        lsstat = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGLS0STAT_REG);
        lsaddr = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGLS0ADDR_REG);
        lsdata = DPORT_REG_READ(DPORT_APP_CPU_RECORD_PDEBUGLS0DATA_REG);
#endif
    }

    if (DPORT_RECORD_PDEBUGINST_SZ(inst) == 0 &&
            DPORT_RECORD_PDEBUGSTATUS_BBCAUSE(dstat) == DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_WAITI) {
        ESP_LOGW(TAG, "WDT reset info: %s CPU PC=0x%x (waiti mode)", cpu_name, pc);
    } else {
        ESP_LOGW(TAG, "WDT reset info: %s CPU PC=0x%x", cpu_name, pc);
    }
    ESP_LOGD(TAG, "WDT reset info: %s CPU STATUS        0x%08x", cpu_name, stat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PID           0x%08x", cpu_name, pid);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGINST    0x%08x", cpu_name, inst);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGSTATUS  0x%08x", cpu_name, dstat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGDATA    0x%08x", cpu_name, data);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGPC      0x%08x", cpu_name, pc);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0STAT 0x%08x", cpu_name, lsstat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0ADDR 0x%08x", cpu_name, lsaddr);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0DATA 0x%08x", cpu_name, lsdata);
}

static void bootloader_check_wdt_reset(void)
{
    int wdt_rst = 0;
    soc_reset_reason_t rst_reas[2];

    rst_reas[0] = esp_rom_get_reset_reason(0);
    rst_reas[1] = esp_rom_get_reset_reason(1);
    if (rst_reas[0] == RESET_REASON_CORE_RTC_WDT || rst_reas[0] == RESET_REASON_CORE_MWDT0 || rst_reas[0] == RESET_REASON_CORE_MWDT1 ||
        rst_reas[0] == RESET_REASON_CPU0_MWDT0 || rst_reas[0] == RESET_REASON_CPU0_RTC_WDT) {
        ESP_LOGW(TAG, "PRO CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (rst_reas[1] == RESET_REASON_CORE_RTC_WDT || rst_reas[1] == RESET_REASON_CORE_MWDT0 || rst_reas[1] == RESET_REASON_CORE_MWDT1 ||
        rst_reas[1] == RESET_REASON_CPU1_MWDT1 || rst_reas[1] == RESET_REASON_CPU1_RTC_WDT) {
        ESP_LOGW(TAG, "APP CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (wdt_rst) {
        // if reset by WDT dump info from trace port
        wdt_reset_info_dump(0);
#if !CONFIG_FREERTOS_UNICORE
        wdt_reset_info_dump(1);
#endif
    }
    wdt_reset_cpu0_info_enable();
}

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

#if XCHAL_ERRATUM_572
    uint32_t memctl = XCHAL_CACHE_MEMCTL_DEFAULT;
    WSR(MEMCTL, memctl);
#endif // XCHAL_ERRATUM_572

    bootloader_init_mem();

    // check that static RAM is after the stack
#ifndef NDEBUG
    {
        assert(&_bss_start <= &_bss_end);
        assert(&_data_start <= &_data_end);
        int *sp = esp_cpu_get_sp();
        assert(sp < &_bss_start);
        assert(sp < &_data_start);
    }
#endif
    // clear bss section
    bootloader_clear_bss_section();
    // init eFuse virtual mode (read eFuses to RAM)
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
#ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    esp_efuse_init_virtual_mode_in_ram();
#endif
#endif
    // bootst up vddsdio
    bootloader_common_vddsdio_configure();
    // reset MMU
    bootloader_reset_mmu();
    // check rated CPU clock
    if ((ret = bootloader_check_rated_cpu_clock()) != ESP_OK) {
        goto err;
    }
    // config clock
    bootloader_clock_configure();
    // initialize uart console, from now on, we can use esp_log
    bootloader_console_init();
    /* print 2nd bootloader banner */
    bootloader_print_banner();
    // update flash ID
    bootloader_flash_update_id();
    // Check and run XMC startup flow
    if ((ret = bootloader_flash_xmc_startup()) != ESP_OK) {
        ESP_LOGE(TAG, "failed when running XMC startup flow, reboot!");
        goto err;
    }
    // read bootloader header
    if ((ret = bootloader_read_bootloader_header()) != ESP_OK) {
        goto err;
    }
    // read chip revision and check if it's compatible to bootloader
    if ((ret = bootloader_check_bootloader_validity()) != ESP_OK) {
        goto err;
    }
    // initialize spi flash
    if ((ret = bootloader_init_spi_flash()) != ESP_OK) {
        goto err;
    }
    // check whether a WDT reset happend
    bootloader_check_wdt_reset();
    // config WDT
    bootloader_config_wdt();
    // enable RNG early entropy source
    bootloader_enable_random();
err:
    return ret;
}
