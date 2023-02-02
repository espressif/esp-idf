/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp32c2/rom/gpio.h"
#include "esp32c2/rom/spi_flash.h"
#include "esp32c2/rom/efuse.h"
#include "soc/gpio_periph.h"
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
#include "hal/cache_hal.h"
#include "hal/mmu_ll.h"

#define FLASH_IO_MATRIX_DUMMY_40M   0
#define FLASH_IO_MATRIX_DUMMY_80M   0

void bootloader_flash_update_id()
{
    esp_rom_spiflash_chip_t *chip = &rom_spiflash_legacy_data->chip;
    chip->device_id = bootloader_read_flash_id();
}

void IRAM_ATTR bootloader_flash_cs_timing_config()
{
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME_V, 0, SPI_MEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_SETUP_TIME_V, 0, SPI_MEM_CS_SETUP_TIME_S);
}

void IRAM_ATTR bootloader_flash_clock_config(const esp_image_header_t *pfhdr)
{
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

void IRAM_ATTR bootloader_flash_set_dummy_out(void)
{
    REG_SET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FDUMMY_OUT | SPI_MEM_D_POL | SPI_MEM_Q_POL);
    REG_SET_BIT(SPI_MEM_CTRL_REG(1), SPI_MEM_FDUMMY_OUT | SPI_MEM_D_POL | SPI_MEM_Q_POL);
}

void IRAM_ATTR bootloader_flash_dummy_config(const esp_image_header_t *pfhdr)
{
    bootloader_configure_spi_pins(1);
    bootloader_flash_set_dummy_out();
}

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
    ESP_EARLY_LOGD(TAG, "magic %02x", bootloader_hdr->magic);
    ESP_EARLY_LOGD(TAG, "segments %02x", bootloader_hdr->segment_count);
    ESP_EARLY_LOGD(TAG, "spi_mode %02x", bootloader_hdr->spi_mode);
    ESP_EARLY_LOGD(TAG, "spi_speed %02x", bootloader_hdr->spi_speed);
    ESP_EARLY_LOGD(TAG, "spi_size %02x", bootloader_hdr->spi_size);

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
    ESP_EARLY_LOGI(TAG, "SPI Speed      : %s", str);

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
    default:
        str = "2MB";
        break;
    }
    ESP_EARLY_LOGI(TAG, "SPI Flash Size : %s", str);
}

static void bootloader_print_mmu_page_size(void)
{
    mmu_page_size_t page_size = mmu_ll_get_page_size(0);
    int size = (page_size == MMU_PAGE_16KB ? 16 :
                page_size == MMU_PAGE_32KB ? 32 :
                page_size == MMU_PAGE_64KB ? 64 : 0);
    ESP_EARLY_LOGI(TAG, "MMU Page Size  : %dK", size);
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

esp_err_t bootloader_init_spi_flash(void)
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
