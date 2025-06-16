/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <assert.h>
#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "esp32c5/rom/spi_flash.h"
#include "esp32c5/rom/efuse.h"
#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"
#include "esp_rom_efuse.h"
#include "soc/efuse_reg.h"
#include "soc/spi_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/soc_caps.h"
#include "flash_qio_mode.h"
#include "bootloader_flash_config.h"
#include "bootloader_common.h"
#include "bootloader_flash_priv.h"
#include "bootloader_init.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/mspi_ll.h"
#include "bootloader_flash_override.h"

void bootloader_flash_update_id()
{
    esp_rom_spiflash_chip_t *chip = &rom_spiflash_legacy_data->chip;
    chip->device_id = bootloader_read_flash_id();
}

void bootloader_flash_update_size(uint32_t size)
{
    rom_spiflash_legacy_data->chip.chip_size = size;
}

void IRAM_ATTR bootloader_flash_cs_timing_config()
{
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME_V, 0, SPI_MEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_SETUP_TIME_V, 0, SPI_MEM_CS_SETUP_TIME_S);
}

void IRAM_ATTR bootloader_init_mspi_clock(void)
{
    // Set source mspi pll clock as 80M in bootloader stage.
    // SPLL clock on C5 is 480MHz , and mspi_pll needs 80MHz
    // in this stage, set divider as 6
    _mspi_timing_ll_set_flash_clk_src(0, FLASH_CLK_SRC_SPLL);
    // MSPI0 and MSPI1 share this core clock register, but only setting to MSPI0 register is valid
    mspi_timing_ll_set_core_clock(MSPI_TIMING_LL_MSPI_ID_0, MSPI_TIMING_LL_CORE_CLOCK_MHZ_DEFAULT);
}

void IRAM_ATTR bootloader_flash_clock_config(const esp_image_header_t *pfhdr)
{
    bootloader_init_mspi_clock();
    uint32_t spi_clk_div = 0;
    switch (pfhdr->spi_speed) {
    case ESP_IMAGE_SPI_SPEED_DIV_1:
        spi_clk_div = 1;
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_2:
        spi_clk_div = 2;
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_3:
        spi_clk_div = 3;
        break;
    case ESP_IMAGE_SPI_SPEED_DIV_4:
        spi_clk_div = 4;
        break;
    default:
        break;
    }
    esp_rom_spiflash_config_clk(spi_clk_div, 0);
}

static const char *TAG = "boot.esp32c5";

void IRAM_ATTR bootloader_configure_spi_pins(int drv)
{
    uint8_t clk_gpio_num = MSPI_IOMUX_PIN_NUM_CLK;
    uint8_t q_gpio_num   = MSPI_IOMUX_PIN_NUM_MISO;
    uint8_t d_gpio_num   = MSPI_IOMUX_PIN_NUM_MOSI;
    uint8_t cs0_gpio_num = MSPI_IOMUX_PIN_NUM_CS0;
    uint8_t hd_gpio_num  = MSPI_IOMUX_PIN_NUM_HD;
    uint8_t wp_gpio_num  = MSPI_IOMUX_PIN_NUM_WP;
    esp_rom_gpio_pad_set_drv(clk_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(q_gpio_num,   drv);
    esp_rom_gpio_pad_set_drv(d_gpio_num,   drv);
    esp_rom_gpio_pad_set_drv(cs0_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(hd_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(wp_gpio_num, drv);
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
    case ESP_IMAGE_FLASH_SIZE_32MB:
        size = 32;
        break;
    default:
        size = 2;
    }
    // Set flash chip size
    esp_rom_spiflash_config_param(rom_spiflash_legacy_data->chip.device_id, size * 0x100000, 0x10000, 0x1000, 0x100, 0xffff);    // TODO: set mode
}

static void print_flash_info(const esp_image_header_t *bootloader_hdr)
{
    ESP_EARLY_LOGD(TAG, "magic %02x", bootloader_hdr->magic);
    ESP_EARLY_LOGD(TAG, "segments %02x", bootloader_hdr->segment_count);
    ESP_EARLY_LOGD(TAG, "spi_mode %02x", bootloader_hdr->spi_mode);
    ESP_EARLY_LOGD(TAG, "spi_speed %02x", bootloader_hdr->spi_speed);
    ESP_EARLY_LOGD(TAG, "spi_size %02x", bootloader_hdr->spi_size);

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
    ESP_EARLY_LOGI(TAG, "SPI Speed      : %s", str);

    /* SPI mode could have been set to QIO during boot already,
       so test the SPI registers not the flash header */
    esp_rom_spiflash_read_mode_t spi_mode = bootloader_flash_get_spi_mode();
    switch (spi_mode) {
    case ESP_ROM_SPIFLASH_QIO_MODE:
        str = "QIO";
        break;
    case ESP_ROM_SPIFLASH_QOUT_MODE:
        str = "QOUT";
        break;
    case ESP_ROM_SPIFLASH_DIO_MODE:
        str = "DIO";
        break;
    case ESP_ROM_SPIFLASH_DOUT_MODE:
        str = "DOUT";
        break;
    case ESP_ROM_SPIFLASH_FASTRD_MODE:
        str = "FAST READ";
        break;
    default:
        str = "SLOW READ";
        break;
    }
    ESP_EARLY_LOGI(TAG, "SPI Mode       : %s", str);

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
    case ESP_IMAGE_FLASH_SIZE_32MB:
        str = "32MB";
        break;
    default:
        str = "2MB";
        break;
    }
    ESP_EARLY_LOGI(TAG, "SPI Flash Size : %s", str);
}

static void IRAM_ATTR bootloader_init_flash_configure(void)
{
    bootloader_configure_spi_pins(1);
    bootloader_flash_cs_timing_config();
}

static void bootloader_spi_flash_resume(void)
{
    bootloader_execute_flash_command(CMD_RESUME, 0, 0, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
}

esp_err_t bootloader_init_spi_flash(void)
{
    bootloader_init_mspi_clock();

    bootloader_init_flash_configure();
    bootloader_spi_flash_resume();
    if ((void*)bootloader_flash_unlock != (void*)bootloader_flash_unlock_default) {
        ESP_EARLY_LOGD(TAG, "Using overridden bootloader_flash_unlock");
    }
    bootloader_flash_unlock();

#if CONFIG_ESPTOOLPY_FLASHMODE_QIO || CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    bootloader_enable_qio_mode();
#endif

#if CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH
    bootloader_flash_32bits_address_map_enable(bootloader_flash_get_spi_mode());
#endif

    print_flash_info(&bootloader_image_hdr);

    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    update_flash_config(&bootloader_image_hdr);
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);

    //ensure the flash is write-protected
    bootloader_enable_wp();
    return ESP_OK;
}

#if CONFIG_APP_BUILD_TYPE_RAM && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
static void bootloader_flash_set_spi_mode(const esp_image_header_t* pfhdr)
{
    esp_rom_spiflash_read_mode_t mode;
    switch(pfhdr->spi_mode) {
    case ESP_IMAGE_SPI_MODE_QIO:
        mode = ESP_ROM_SPIFLASH_QIO_MODE;
        break;
    case ESP_IMAGE_SPI_MODE_QOUT:
        mode = ESP_ROM_SPIFLASH_QOUT_MODE;
        break;
    case ESP_IMAGE_SPI_MODE_DIO:
        mode = ESP_ROM_SPIFLASH_DIO_MODE;
        break;
    case ESP_IMAGE_SPI_MODE_FAST_READ:
        mode = ESP_ROM_SPIFLASH_FASTRD_MODE;
        break;
    case ESP_IMAGE_SPI_MODE_SLOW_READ:
        mode = ESP_ROM_SPIFLASH_SLOWRD_MODE;
        break;
    default:
        mode = ESP_ROM_SPIFLASH_DIO_MODE;
    }
    esp_rom_spiflash_config_readmode(mode);
}

void bootloader_flash_hardware_init(void)
{
    esp_rom_spiflash_attach(0, false);

    //init cache hal
    cache_hal_init();
    //init mmu
    mmu_hal_init();
    // update flash ID
    bootloader_flash_update_id();
    // Check and run XMC startup flow
    esp_err_t ret = bootloader_flash_xmc_startup();
    assert(ret == ESP_OK);

    /* Alternative of bootloader_init_spi_flash */
    // RAM app doesn't have headers in the flash. Make a default one for it.
    esp_image_header_t WORD_ALIGNED_ATTR hdr = {
        .spi_mode = ESP_IMAGE_SPI_MODE_DIO,
        .spi_speed = ESP_IMAGE_SPI_SPEED_DIV_2,
        .spi_size = ESP_IMAGE_FLASH_SIZE_2MB,
    };

    bootloader_configure_spi_pins(1);
    bootloader_flash_set_spi_mode(&hdr);
    bootloader_flash_clock_config(&hdr);
    // TODO: set proper dummy output
    bootloader_flash_cs_timing_config();

    bootloader_spi_flash_resume();
    bootloader_flash_unlock();

#if CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH
    bootloader_flash_32bits_address_map_enable(bootloader_flash_get_spi_mode());
#endif

    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    update_flash_config(&hdr);
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);

    //ensure the flash is write-protected
    bootloader_enable_wp();
}
#endif //CONFIG_APP_BUILD_TYPE_RAM && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
