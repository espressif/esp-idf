/* Custom chip driver example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include <string.h>
#include <sys/param.h> // For MIN/MAX
#include "esp_log.h"
#include "spi_flash_chip_generic.h"
#include "spi_flash/spi_flash_defs.h"


// Not for all the vendors
#define CMD_ENTER_OTP       0x3A

static const char chip_name[] = "eon";

/* Driver for EON flash chip */

esp_err_t spi_flash_chip_eon_probe(esp_flash_t *chip, uint32_t flash_id)
{
    /* Check manufacturer and product IDs match our desired masks */
    const uint8_t MFG_ID = 0x1C;
    const uint16_t DEV_ID = 0x7000;
    if (flash_id >> 16 != MFG_ID || (flash_id & 0xFF00) != DEV_ID) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t spi_flash_chip_eon_enter_otp_mode(esp_flash_t* chip)
{
    spi_flash_trans_t trans = {
        .command = CMD_ENTER_OTP,
    };
    return chip->host->driver->common_command(chip->host, &trans);
}

static esp_err_t spi_flash_chip_eon_exit_otp_mode(esp_flash_t* chip)
{
    spi_flash_trans_t trans = {
        .command = CMD_WRDI,
    };
    return chip->host->driver->common_command(chip->host, &trans);
}

esp_err_t spi_flash_chip_eon_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode)
{
    esp_err_t ret;
    ret = spi_flash_chip_eon_enter_otp_mode(chip);
    if (ret != ESP_OK) {
        return ret;
    }

    // On "eon" chips, this involves checking
    // bit 1 (QE) of RDSR2 (35h) result
    // (it works this way on GigaDevice & Fudan Micro chips, probably others...)
    const uint8_t BIT_QE = 1 << 6;
    uint32_t sr;
    ret = spi_flash_common_read_status_8b_rdsr(chip, &sr);
    if (ret == ESP_OK) {
        *out_io_mode = ((sr & BIT_QE)? SPI_FLASH_QOUT: 0);
    }

    //unconditionally exit OTP mode
    esp_err_t ret_exit = spi_flash_chip_eon_exit_otp_mode(chip);
    if (ret != ESP_OK) {
        return ret;
    }
    return ret_exit;
}

esp_err_t spi_flash_chip_eon_set_io_mode(esp_flash_t *chip)
{
    if (!esp_flash_is_quad_mode(chip)) {
        return ESP_OK;
    }

    esp_err_t ret;
    ret = spi_flash_chip_eon_enter_otp_mode(chip);
    if (ret != ESP_OK) {
        return ret;
    }

    // On "eon" chips, this involves checking
    // bit 6 (QE) of RDSR (05h) result
    const uint32_t BIT_QE = 1 << 6;
    ret = spi_flash_common_set_io_mode(chip,
                                        spi_flash_common_write_status_8b_wrsr,
                                        spi_flash_common_read_status_8b_rdsr,
                                        BIT_QE);

    //unconditionally exit OTP mode
    esp_err_t ret_exit = spi_flash_chip_eon_exit_otp_mode(chip);
    if (ret != ESP_OK) {
        return ret;
    }
    return ret_exit;
}

esp_err_t spi_flash_chip_eon_suspend_cmd_conf(esp_flash_t *chip)
{
    return ESP_ERR_NOT_SUPPORTED;
}

spi_flash_caps_t spi_flash_chip_eon_get_caps(esp_flash_t *chip)
{
    spi_flash_caps_t caps_flags = 0;
    // 32-bit-address flash is not supported
    // flash-suspend is not supported
    // flash read unique id is not supported.
    return caps_flags;
}

// The issi chip can use the functions for generic chips except from set read mode and probe,
// So we only replace these two functions.
const spi_flash_chip_t esp_flash_chip_eon = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,
    .probe = spi_flash_chip_eon_probe,
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_generic_erase_sector,
    .erase_block = spi_flash_chip_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_generic_read,
    .write = spi_flash_chip_generic_write,
    .program_page = spi_flash_chip_generic_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,
    .set_io_mode = spi_flash_chip_eon_set_io_mode,
    .get_io_mode = spi_flash_chip_eon_get_io_mode,

    .read_reg = spi_flash_chip_generic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_eon_suspend_cmd_conf,
    .get_chip_caps = spi_flash_chip_eon_get_caps,
};
