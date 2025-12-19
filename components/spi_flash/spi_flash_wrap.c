/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "rom/spi_flash.h"

#include "esp_flash_chips/spi_flash_defs.h"
#include "esp_flash_chips/spi_flash_override.h"
#include "esp_private/spi_flash_os.h"

// TODO: These dependencies will be removed after remove bootloader_flash to G0.IDF-4609
#include "bootloader_flash_override.h"
#include "bootloader_flash_priv.h"

/***********************************************************************************
 * Flash wrap feature (also called burst read on some flash chips)
 *
 * Different flash chips enter wrap (burst read) mode in different strategies.
 * 1. Command 0xC0 + 8 Bytes.
 * 2. Command 0x77 + 24 dummy + 8 Bytes.
 **********************************************************************************/

#if SOC_SPI_MEM_SUPPORT_WRAP

const static char *FLASH_WRAP_TAG = "flash wrap";

// TODO: This function will be changed after remove bootloader_flash to G0.IDF-4609
extern uint32_t bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len);

esp_err_t spi_flash_wrap_probe_c0(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;

    switch (flash_id) {
    /* The flash listed here should enter the wrap with command 0xC0 */
    case 0xC22018:
        break;
    default:
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

/**
 * @brief Burst read with command 0xC0 + 8 Bytes
 *
 * |------------|-----------------------------|
 * |    data    |         wrap depth          |
 * |     00h    |             8               |
 * |     01h    |             16              |
 * |     02h    |             32              |
 * |     03h    |             64              |
 * |------------|-----------------------------|
 */
esp_err_t spi_flash_wrap_enable_c0(spi_flash_wrap_size_t wrap_size)
{
    uint8_t wrap_code = (uint8_t) (__builtin_ctz(wrap_size) - 3);
    bootloader_flash_execute_command_common(CMD_BURST_RD, 0, 0, 0, 8, wrap_code, 0);
    return ESP_OK;
}

/**
 * @brief Burst read with command 0x77 + 24 Dummy + 8 Bytes
 *
 * |-------------------|-----------------------------|
 * |    data(W6,W5)    |         wrap depth          |
 * |        00h        |             8               |
 * |        01h        |             16              |
 * |        02h        |             32              |
 * |        03h        |             64              |
 * |-------------------|-----------------------------|
 */
esp_err_t spi_flash_wrap_enable_77(spi_flash_wrap_size_t wrap_size)
{
    uint8_t wrap_code = (uint8_t) (((__builtin_ctz(wrap_size) - 3) * 2) << 4);
    // According to the special format, we need enable QIO_FWRITE for command 77h and clear it after this command is done.
    REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    bootloader_flash_execute_command_common(CMD_WRAP, 0, 0, 6, 8, wrap_code, 0);
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    return ESP_OK;
}

/**
 * @brief Burst read is cleared by setting 0x1xh,
 *        so we set 0x10 to disable this feature.
 */
esp_err_t spi_flash_wrap_clear_c0(void)
{
    bootloader_flash_execute_command_common(CMD_BURST_RD, 0, 0, 0, 8, 0x10, 0);
    return ESP_OK;
}

/**
 * @brief Burst read is cleared by setting W4 bit 1,
 *        so we set 0x10 to disable this feature.
 */
esp_err_t spi_flash_wrap_clear_77(void)
{
    // According to the special format, we need enable QIO_FWRITE for command 77h and clear it after this command is done.
    REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    bootloader_flash_execute_command_common(CMD_WRAP, 0, 0, 6, 8, 0x10, 0);
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    return ESP_OK;
}

const spi_flash_wrap_info_t __attribute__((weak)) spi_flash_wrap_list[] = {
    /* method                probe                 chip wrap set              chip wrap clear  */
    {"C0H+8B",     spi_flash_wrap_probe_c0,     spi_flash_wrap_enable_c0, spi_flash_wrap_clear_c0},
    {"default",        NULL,                    spi_flash_wrap_enable_77, spi_flash_wrap_clear_77},
};

static const spi_flash_wrap_info_t *chip_wrap = NULL;

esp_err_t spi_flash_wrap_probe(void)
{
    uint32_t flash_chip_id = g_rom_flashchip.device_id;
    const spi_flash_wrap_info_t *chip = spi_flash_wrap_list;
    esp_err_t ret = ESP_OK;
    while (chip->probe) {
        ret = chip->probe(flash_chip_id);
        if (ret == ESP_OK) {
            break;
        }
        chip++;
    }
    chip_wrap = chip;
    return ret;
}

esp_err_t spi_flash_wrap_enable(spi_flash_wrap_size_t wrap_size)
{
    return chip_wrap->chip_wrap_set(wrap_size);
}

esp_err_t spi_flash_wrap_disable(void)
{
    return chip_wrap->chip_wrap_clr();
}

bool spi_flash_support_wrap_size(uint32_t wrap_size)
{
    // Only QIO mode supports wrap.
    if (!REG_GET_BIT(PERIPHS_SPI_FLASH_CTRL, SPI_MEM_FREAD_QIO)) {
        ESP_EARLY_LOGE(FLASH_WRAP_TAG, "flash wrap is only supported in QIO mode");
        abort();
    }
    // Only following size can be wrapped.
    switch (wrap_size) {
    case 0:
    case 8:
    case 16:
    case 32:
    case 64:
        return true;
    default:
        return false;
    }
}

#endif // SOC_SPI_MEM_SUPPORT_WRAP
