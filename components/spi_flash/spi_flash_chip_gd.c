/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/param.h> // For MIN/MAX
#include "esp_log.h"
#include "sdkconfig.h"

#include "esp_flash_chips/spi_flash_defs.h"
#include "esp_flash_chips/spi_flash_chip_generic.h"
#include "esp_flash_chips/spi_flash_chip_gd.h"

#define ADDR_32BIT(addr)            (addr >= (1<<24))

#define REGION_32BIT(start, len)    ((start) + (len) > (1<<24))

extern esp_err_t spi_flash_chip_winbond_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length);
extern esp_err_t spi_flash_chip_winbond_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);
extern esp_err_t spi_flash_chip_winbond_erase_sector(esp_flash_t *chip, uint32_t start_address);
extern esp_err_t spi_flash_chip_winbond_erase_block(esp_flash_t *chip, uint32_t start_address);

#define spi_flash_chip_gd_read spi_flash_chip_winbond_read
#define spi_flash_chip_gd_page_program spi_flash_chip_winbond_page_program
#define spi_flash_chip_gd_erase_sector spi_flash_chip_winbond_erase_sector
#define spi_flash_chip_gd_erase_block spi_flash_chip_winbond_erase_block

spi_flash_caps_t spi_flash_chip_gd_get_caps(esp_flash_t *chip)
{
    spi_flash_caps_t caps_flags = 0;
    // 32M-bits address support
    if ((chip->chip_id & 0xFF) >= 0x19) {
        caps_flags |= SPI_FLASH_CHIP_CAP_32MB_SUPPORT;
    }
#if CONFIG_SPI_FLASH_AUTO_SUSPEND
    switch (chip->chip_id) {
    /* The flash listed here can support suspend */
    case 0xC84016:
    case 0xC84017:
    case 0xC84018:
    case 0xC84319:
        caps_flags |= SPI_FLASH_CHIP_CAP_SUSPEND;
        break;
    default:
        break;
    }
#endif
    // flash-suspend is not supported
    // flash read unique id.
    caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
    return caps_flags;
}

esp_err_t spi_flash_chip_gd_detect_size(esp_flash_t *chip, uint32_t *size)
{
    uint32_t id = chip->chip_id;
    *size = 0;

    /* Can't detect size unless the high byte of the product ID matches the same convention, which is usually 0x40 or
     * 0xC0 or similar. */
    if (((id & 0xFFFF) == 0x0000) || ((id & 0xFFFF) == 0xFFFF)) {
        return ESP_ERR_FLASH_UNSUPPORTED_CHIP;
    }

    *size = 1 << (id & 0xFF);
    return ESP_OK;
}

#ifndef CONFIG_SPI_FLASH_ROM_IMPL

#define FLASH_ID_MASK       0xFF00
#define FLASH_SIZE_MASK     0xFF
#define GD25Q_PRODUCT_ID    0x4000
#define GD25LQ_PRODUCT_ID   0x6000
#define GD25UF_PRODUCT_ID   0x8300
#define GD55F_PRODUCT_ID    0x4300

#define WRSR_16B_REQUIRED(chip_id) (((chip_id) & FLASH_ID_MASK) == GD25LQ_PRODUCT_ID || \
                                    ((chip_id) & FLASH_SIZE_MASK) <= 0x15)

/* Driver for GD flash chip */

esp_err_t spi_flash_chip_gd_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0xC8;
    if (flash_id >> 16 != MFG_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    uint32_t product_id = flash_id & FLASH_ID_MASK;
    if (product_id != GD25Q_PRODUCT_ID && product_id != GD25LQ_PRODUCT_ID && product_id != GD25UF_PRODUCT_ID && product_id != GD55F_PRODUCT_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t spi_flash_chip_gd_set_io_mode(esp_flash_t *chip)
{
    if (WRSR_16B_REQUIRED(chip->chip_id)) {
        const uint32_t qe = 1<<9;
        return spi_flash_common_set_io_mode(chip,
                                            spi_flash_common_write_status_16b_wrsr,
                                            spi_flash_common_read_status_16b_rdsr_rdsr2,
                                            qe);
    } else {
        const uint32_t qe = 1<<1;
        return spi_flash_common_set_io_mode(chip,
                                            spi_flash_common_write_status_8b_wrsr2,
                                            spi_flash_common_read_status_8b_rdsr2,
                                            qe);
    }
}

esp_err_t spi_flash_chip_gd_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode)
{
    /* GD uses bit 1 of SR2 as Quad Enable */
    const uint8_t BIT_QE = 1 << 1;
    uint32_t sr;
    esp_err_t ret = spi_flash_common_read_status_8b_rdsr2(chip, &sr);
    if (ret == ESP_OK) {
        *out_io_mode = ((sr & BIT_QE)? SPI_FLASH_QOUT: 0);
    }
    return ret;
}
#endif //CONFIG_SPI_FLASH_ROM_IMPL

esp_err_t spi_flash_chip_gd_suspend_cmd_conf(esp_flash_t *chip)
{
    spi_flash_sus_cmd_conf sus_conf = {
        .sus_mask = 0x84,
        .cmd_rdsr = CMD_RDSR2,
        .sus_cmd = CMD_SUSPEND,
        .res_cmd = CMD_RESUME,
    };

    return chip->host->driver->sus_setup(chip->host, &sus_conf);
}

static const char chip_name[] = "gd";

// The issi chip can use the functions for generic chips except from set read mode and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_gd = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_gd_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_gd_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_gd_erase_sector,
    .erase_block = spi_flash_chip_gd_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_gd_read,
    .write = spi_flash_chip_generic_write,
    .program_page = spi_flash_chip_gd_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,
    .set_io_mode = spi_flash_chip_gd_set_io_mode,
    .get_io_mode = spi_flash_chip_gd_get_io_mode,

    .read_reg = spi_flash_chip_generic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_gd_suspend_cmd_conf,
    .read_unique_id = spi_flash_chip_generic_read_unique_id,
    .get_chip_caps = spi_flash_chip_gd_get_caps,
    .config_host_io_mode = spi_flash_chip_generic_config_host_io_mode,
};
