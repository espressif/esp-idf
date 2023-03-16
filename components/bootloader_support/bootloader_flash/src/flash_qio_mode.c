/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdint.h>
#include "bootloader_flash_config.h"
#include "flash_qio_mode.h"
#include "sdkconfig.h"
#include "bootloader_flash_priv.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_rom_spiflash.h"
#include "esp_rom_efuse.h"
#include "flash_qio_mode.h"
#include "soc/efuse_periph.h"
#include "soc/io_mux_reg.h"
#include "esp_private/spi_flash_os.h"


static const char *TAG = "qio_mode";

/* Array of known flash chips and data to enable Quad I/O mode

   Manufacturer & flash ID can be tested by running "esptool.py
   flash_id"

   If manufacturer ID matches, and flash ID ORed with flash ID mask
   matches, enable_qio_mode() will execute "Read Cmd", test if bit
   number "QIE Bit" is set, and if not set it will call "Write Cmd"
   with this bit set.

   Searching of this table stops when the first match is found.
 */
const bootloader_qio_info_t __attribute__((weak)) bootloader_flash_qe_support_list[] = {
    /*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,               QIE Bit */
    { "MXIC",        0xC2,   0x2000, 0xFF00,    bootloader_read_status_8b_rdsr,        bootloader_write_status_8b_wrsr,       6 },
    { "ISSI",        0x9D,   0x4000, 0xCF00,    bootloader_read_status_8b_rdsr,        bootloader_write_status_8b_wrsr,       6 }, /* IDs 0x40xx, 0x70xx */
    { "WinBond",     0xEF,   0x4000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
    { "GD",          0xC8,   0x6000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
    { "XM25QU64A",   0x20,   0x3817, 0xFFFF,    bootloader_read_status_8b_xmc25qu64a,  bootloader_write_status_8b_xmc25qu64a, 6 },
    { "TH",          0xcd,   0x6000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },

    /* Final entry is default entry, if no other IDs have matched.

       This approach works for chips including:
       GigaDevice (mfg ID 0xC8, flash IDs including 4016),
       FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
       BY25Q32 (mfg ID 0x68, flash IDs including 4016)
    */
    { NULL,          0xFF,    0xFFFF, 0xFFFF,   bootloader_read_status_8b_rdsr2,       bootloader_write_status_8b_wrsr2,      1 },
};

#define NUM_CHIPS (sizeof(bootloader_flash_qe_support_list) / sizeof(bootloader_qio_info_t))

static esp_err_t enable_qio_mode(bootloader_flash_read_status_fn_t read_status_fn,
                                 bootloader_flash_write_status_fn_t write_status_fn,
                                 uint8_t status_qio_bit);

/* Generic function to use the "user command" SPI controller functionality
   to send commands to the SPI flash and read the respopnse.

   The command passed here is always the on-the-wire command given to the SPI flash unit.
*/

void bootloader_enable_qio_mode(void)
{
    uint32_t raw_flash_id;
    uint8_t mfg_id;
    uint16_t flash_id;
    size_t i;

    ESP_LOGD(TAG, "Probing for QIO mode enable...");
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    raw_flash_id = g_rom_flashchip.device_id;
    ESP_LOGD(TAG, "Raw SPI flash chip id 0x%"PRIx32, raw_flash_id);

    mfg_id = (raw_flash_id >> 16) & 0xFF;
    flash_id = raw_flash_id & 0xFFFF;
    ESP_LOGD(TAG, "Manufacturer ID 0x%02x chip ID 0x%04x", mfg_id, flash_id);

    for (i = 0; i < NUM_CHIPS - 1; i++) {
        const bootloader_qio_info_t *chip = &bootloader_flash_qe_support_list[i];
        if (mfg_id == chip->mfg_id && (flash_id & chip->id_mask) == (chip->flash_id & chip->id_mask)) {
            ESP_LOGI(TAG, "Enabling QIO for flash chip %s", bootloader_flash_qe_support_list[i].manufacturer);
            break;
        }
    }

    if (i == NUM_CHIPS - 1) {
        ESP_LOGI(TAG, "Enabling default flash chip QIO");
    }
    enable_qio_mode(bootloader_flash_qe_support_list[i].read_status_fn,
                    bootloader_flash_qe_support_list[i].write_status_fn,
                    bootloader_flash_qe_support_list[i].status_qio_bit);
#if SOC_CACHE_SUPPORT_WRAP
    spi_flash_wrap_probe();
    spi_flash_wrap_disable();
#endif
}

static void s_flash_set_qio_pins(void)
{
#if SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE

#if CONFIG_IDF_TARGET_ESP32
    esp_rom_spiflash_select_qio_pins(bootloader_flash_get_wp_pin(), esp_rom_efuse_get_flash_gpio_info());
#else
    esp_rom_spiflash_select_qio_pins(esp_rom_efuse_get_flash_wp_gpio(), esp_rom_efuse_get_flash_gpio_info());
#endif // CONFIG_IDF_TARGET_ESP32

#else
    // ESP32C2/ESP32C6 doesn't support configure mspi pins. So the second
    // parameter is set to 0, means that chip uses default SPI pins
    // and wp_gpio_num parameter(the first parameter) is ignored.
    esp_rom_spiflash_select_qio_pins(0, 0);
#endif // SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
}


static esp_err_t enable_qio_mode(bootloader_flash_read_status_fn_t read_status_fn,
                                 bootloader_flash_write_status_fn_t write_status_fn,
                                 uint8_t status_qio_bit)
{
    uint32_t status;

    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    status = read_status_fn();
    ESP_LOGD(TAG, "Initial flash chip status 0x%"PRIx32, status);

    if ((status & (1 << status_qio_bit)) == 0) {
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        write_status_fn(status | (1 << status_qio_bit));

        esp_rom_spiflash_wait_idle(&g_rom_flashchip);

        status = read_status_fn();
        ESP_LOGD(TAG, "Updated flash chip status 0x%"PRIx32, status);
        if ((status & (1 << status_qio_bit)) == 0) {
            ESP_LOGE(TAG, "Failed to set QIE bit, not enabling QIO mode");
            return ESP_FAIL;
        }

    } else {
        ESP_LOGD(TAG, "QIO mode already enabled in flash");
    }

    ESP_LOGD(TAG, "Enabling QIO mode...");

    esp_rom_spiflash_read_mode_t mode;
#if CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    mode = ESP_ROM_SPIFLASH_QOUT_MODE;
#else
    mode = ESP_ROM_SPIFLASH_QIO_MODE;
#endif

    esp_rom_spiflash_config_readmode(mode);

    s_flash_set_qio_pins();
    return ESP_OK;
}

IRAM_ATTR unsigned bootloader_read_status_8b_rdsr(void)
{
    return bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
}

IRAM_ATTR unsigned bootloader_read_status_8b_rdsr2(void)
{
    return bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8);
}

IRAM_ATTR unsigned bootloader_read_status_8b_rdsr3(void)
{
    return bootloader_execute_flash_command(CMD_RDSR3, 0, 0, 8);
}

IRAM_ATTR unsigned bootloader_read_status_16b_rdsr_rdsr2(void)
{
    return bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8) | (bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8) << 8);
}

IRAM_ATTR void bootloader_write_status_8b_wrsr(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR, new_status, 8, 0);
}

IRAM_ATTR void bootloader_write_status_8b_wrsr2(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR2, new_status, 8, 0);
}

IRAM_ATTR void bootloader_write_status_8b_wrsr3(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR3, new_status, 8, 0);
}

IRAM_ATTR void bootloader_write_status_16b_wrsr(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR, new_status, 16, 0);
}

IRAM_ATTR unsigned bootloader_read_status_8b_xmc25qu64a(void)
{
    bootloader_execute_flash_command(CMD_OTPEN, 0, 0, 0);  /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    uint32_t read_status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
    return read_status;
}

IRAM_ATTR void bootloader_write_status_8b_xmc25qu64a(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_OTPEN, 0, 0, 0);  /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    bootloader_execute_flash_command(CMD_WRSR, new_status, 8, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
}
