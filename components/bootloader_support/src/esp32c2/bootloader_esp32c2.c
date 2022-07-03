/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/assist_debug_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include "soc/extmem_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/system_reg.h"
#include "esp32c2/rom/efuse.h"
#include "esp32c2/rom/ets_sys.h"
#include "esp32c2/rom/rtc.h"
#include "bootloader_common.h"
#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_console.h"
#include "bootloader_flash_priv.h"
#include "esp_efuse.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "hal/mmu_ll.h"

static const char *TAG = "boot.esp32c2";

void IRAM_ATTR bootloader_configure_spi_pins(int drv)
{
    // IDF-4066
    const uint32_t spiconfig = 0;
    uint8_t clk_gpio_num = SPI_CLK_GPIO_NUM;
    uint8_t q_gpio_num   = SPI_Q_GPIO_NUM;
    uint8_t d_gpio_num   = SPI_D_GPIO_NUM;
    uint8_t cs0_gpio_num = SPI_CS0_GPIO_NUM;
    uint8_t hd_gpio_num  = SPI_HD_GPIO_NUM;
    uint8_t wp_gpio_num  = SPI_WP_GPIO_NUM;
    if (spiconfig == 0) {

    }
    esp_rom_gpio_pad_set_drv(clk_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(q_gpio_num,   drv);
    esp_rom_gpio_pad_set_drv(d_gpio_num,   drv);
    esp_rom_gpio_pad_set_drv(cs0_gpio_num, drv);
    if (hd_gpio_num <= MAX_PAD_GPIO_NUM) {
        esp_rom_gpio_pad_set_drv(hd_gpio_num, drv);
    }
    if (wp_gpio_num <= MAX_PAD_GPIO_NUM) {
        esp_rom_gpio_pad_set_drv(wp_gpio_num, drv);
    }
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
    cache_hal_disable(CACHE_TYPE_ALL);
    // Set flash chip size
    esp_rom_spiflash_config_param(rom_spiflash_legacy_data->chip.device_id, size * 0x100000, 0x10000, 0x1000, 0x100, 0xffff);    // TODO: set mode
    cache_hal_enable(CACHE_TYPE_ALL);
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
        str = "30MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_3:
        str = "20MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_4:
        str = "15MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_1:
        str = "60MHz";
        break;
    default:
        str = "15MHz";
        break;
    }
    ESP_LOGI(TAG, "SPI Speed      : %s", str);

    /* SPI mode could have been set to QIO during boot already,
       so test the SPI registers not the flash header */
    uint32_t spi_ctrl = REG_READ(SPI_MEM_CTRL_REG(0));
    if (spi_ctrl & SPI_MEM_FREAD_QIO) {
        str = "QIO";
    } else if (spi_ctrl & SPI_MEM_FREAD_QUAD) {
        str = "QOUT";
    } else if (spi_ctrl & SPI_MEM_FREAD_DIO) {
        str = "DIO";
    } else if (spi_ctrl & SPI_MEM_FREAD_DUAL) {
        str = "DOUT";
    } else if (spi_ctrl & SPI_MEM_FASTRD_MODE) {
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

static void bootloader_print_mmu_page_size(void)
{
    mmu_page_size_t page_size = mmu_ll_get_page_size(0);
    int size = (page_size == MMU_PAGE_16KB ? 16 :
                page_size == MMU_PAGE_32KB ? 32 :
                page_size == MMU_PAGE_64KB ? 64 : 0);
    ESP_LOGI(TAG, "MMU Page Size  : %dK", size);
}

static void IRAM_ATTR bootloader_init_flash_configure(void)
{
    bootloader_flash_dummy_config(&bootloader_image_hdr);
    bootloader_flash_cs_timing_config();
}

static void bootloader_spi_flash_resume(void)
{
    bootloader_execute_flash_command(CMD_RESUME, 0, 0, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
}

static esp_err_t bootloader_init_spi_flash(void)
{
    bootloader_init_flash_configure();

    bootloader_spi_flash_resume();
    bootloader_flash_unlock();

#if CONFIG_ESPTOOLPY_FLASHMODE_QIO || CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    bootloader_enable_qio_mode();
#endif

    bootloader_print_mmu_page_size();
    print_flash_info(&bootloader_image_hdr);
    update_flash_config(&bootloader_image_hdr);
    //ensure the flash is write-protected
    bootloader_enable_wp();
    return ESP_OK;
}

static void wdt_reset_cpu0_info_enable(void)
{
    REG_SET_BIT(SYSTEM_CPU_PERI_CLK_EN_REG, SYSTEM_CLK_EN_ASSIST_DEBUG);
    REG_CLR_BIT(SYSTEM_CPU_PERI_RST_EN_REG, SYSTEM_RST_EN_ASSIST_DEBUG);
    REG_WRITE(ASSIST_DEBUG_CORE_0_RCD_EN_REG, ASSIST_DEBUG_CORE_0_RCD_PDEBUGEN | ASSIST_DEBUG_CORE_0_RCD_RECORDEN);
}

static void wdt_reset_info_dump(int cpu)
{
    (void) cpu;
    // saved PC was already printed by the ROM bootloader.
    // nothing to do here.
}

static void bootloader_check_wdt_reset(void)
{
    int wdt_rst = 0;
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if (rst_reason == RESET_REASON_CORE_RTC_WDT || rst_reason == RESET_REASON_CORE_MWDT0 ||
        rst_reason == RESET_REASON_CPU0_MWDT0 || rst_reason == RESET_REASON_CPU0_RTC_WDT) {
        ESP_LOGW(TAG, "PRO CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (wdt_rst) {
        // if reset by WDT dump info from trace port
        wdt_reset_info_dump(0);
    }
    wdt_reset_cpu0_info_enable();
}

static void bootloader_super_wdt_auto_feed(void)
{
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, RTC_CNTL_SWD_WKEY_VALUE);
    REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_AUTO_FEED_EN);
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, 0);
}

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;

    bootloader_super_wdt_auto_feed();
    // protect memory region
    bootloader_init_mem();
    /* check that static RAM is after the stack */
    assert(&_bss_start <= &_bss_end);
    assert(&_data_start <= &_data_end);
    // clear bss section
    bootloader_clear_bss_section();
    // init eFuse virtual mode (read eFuses to RAM)
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
#ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    esp_efuse_init_virtual_mode_in_ram();
#endif
#endif
    //init cache hal
    cache_hal_init();
    //reset mmu
    mmu_hal_init();
    // config mmu page size
    mmu_ll_set_page_size(0, SPI_FLASH_MMU_PAGE_SIZE);
    // config clock
    bootloader_clock_configure();
    // initialize console, from now on, we can use esp_log
    bootloader_console_init();
    /* print 2nd bootloader banner */
    bootloader_print_banner();
    // update flash ID
    bootloader_flash_update_id();
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
