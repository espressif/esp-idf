// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stddef.h>
#include <stdint.h>
#include "flash_qio_mode.h"
#include "esp_log.h"
#include "esp_err.h"
#include "rom/spi_flash.h"
#include "rom/efuse.h"
#include "soc/spi_struct.h"
#include "soc/efuse_reg.h"
#include "sdkconfig.h"

/* SPI flash controller */
#define SPIFLASH SPI1

/* SPI commands (actual on-wire commands not SPI controller bitmasks)
   Suitable for use with the execute_flash_command static function.
*/
#define CMD_RDID       0x9F
#define CMD_WRSR       0x01
#define CMD_WRSR2      0x31 /* Not all SPI flash uses this command */
#define CMD_WREN       0x06
#define CMD_WRDI       0x04
#define CMD_RDSR       0x05
#define CMD_RDSR2      0x35 /* Not all SPI flash uses this command */

static const char *TAG = "qio_mode";

typedef unsigned (*read_status_fn_t)();
typedef void (*write_status_fn_t)(unsigned);

typedef struct __attribute__((packed)) {
    const char *manufacturer;
    uint8_t mfg_id; /* 8-bit JEDEC manufacturer ID */
    uint16_t flash_id; /* 16-bit JEDEC flash chip ID */
    uint16_t id_mask; /* Bits to match on in flash chip ID */
    read_status_fn_t read_status_fn;
    write_status_fn_t write_status_fn;
    uint8_t status_qio_bit;
} qio_info_t;

/* Read 8 bit status using RDSR command */
static unsigned read_status_8b_rdsr();
/* Read 8 bit status (second byte) using RDSR2 command */
static unsigned read_status_8b_rdsr2();
/* read 16 bit status using RDSR & RDSR2 (low and high bytes) */
static unsigned read_status_16b_rdsr_rdsr2();

/* Write 8 bit status using WRSR */
static void write_status_8b_wrsr(unsigned new_status);
/* Write 8 bit status (second byte) using WRSR2 */
static void write_status_8b_wrsr2(unsigned new_status);
/* Write 16 bit status using WRSR */
static void write_status_16b_wrsr(unsigned new_status);

#define ESP32_D2WD_WP_GPIO 7 /* ESP32-D2WD has this GPIO wired to WP pin of flash */

#ifndef CONFIG_BOOTLOADER_SPI_WP_PIN // Set in menuconfig if SPI flasher config is set to a quad mode
#define CONFIG_BOOTLOADER_SPI_WP_PIN ESP32_D2WD_WP_GPIO
#endif

/* Array of known flash chips and data to enable Quad I/O mode

   Manufacturer & flash ID can be tested by running "esptool.py
   flash_id"

   If manufacturer ID matches, and flash ID ORed with flash ID mask
   matches, enable_qio_mode() will execute "Read Cmd", test if bit
   number "QIE Bit" is set, and if not set it will call "Write Cmd"
   with this bit set.

   Searching of this table stops when the first match is found.
 */
const static qio_info_t chip_data[] = {
/*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,          QIE Bit */
    { "MXIC",        0xC2,   0x2000, 0xFF00,    read_status_8b_rdsr,        write_status_8b_wrsr,  6 },
    { "ISSI",        0x9D,   0x4000, 0xCF00,    read_status_8b_rdsr,        write_status_8b_wrsr,  6 }, /* IDs 0x40xx, 0x70xx */
    { "WinBond",     0xEF,   0x4000, 0xFF00,    read_status_16b_rdsr_rdsr2, write_status_16b_wrsr, 9 },
    { "GD",          0xC8,   0x6000, 0xFF00,    read_status_16b_rdsr_rdsr2, write_status_16b_wrsr, 9 },

    /* Final entry is default entry, if no other IDs have matched.

       This approach works for chips including:
       GigaDevice (mfg ID 0xC8, flash IDs including 4016),
       FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
    */
    { NULL,          0xFF,    0xFFFF, 0xFFFF,   read_status_8b_rdsr2,       write_status_8b_wrsr2, 1 },
};

#define NUM_CHIPS (sizeof(chip_data) / sizeof(qio_info_t))

static esp_err_t enable_qio_mode(read_status_fn_t read_status_fn,
                            write_status_fn_t write_status_fn,
                            uint8_t status_qio_bit);

/* Generic function to use the "user command" SPI controller functionality
   to send commands to the SPI flash and read the respopnse.

   The command passed here is always the on-the-wire command given to the SPI flash unit.
*/
static uint32_t execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len);

/* dummy_len_plus values defined in ROM for SPI flash configuration */
extern uint8_t g_rom_spiflash_dummy_len_plus[];

void bootloader_enable_qio_mode(void)
{
    uint32_t old_ctrl_reg;
    uint32_t raw_flash_id;
    uint8_t mfg_id;
    uint16_t flash_id;
    int i;

    ESP_LOGD(TAG, "Probing for QIO mode enable...");
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    /* Set up some of the SPIFLASH user/ctrl variables which don't change
       while we're probing using execute_flash_command() */
    old_ctrl_reg = SPIFLASH.ctrl.val;
    SPIFLASH.ctrl.val = SPI_WP_REG; // keep WP high while idle, otherwise leave DIO mode
    SPIFLASH.user.usr_dummy = 0;
    SPIFLASH.user.usr_addr = 0;
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;

    raw_flash_id = execute_flash_command(CMD_RDID, 0, 0, 24);
    ESP_LOGD(TAG, "Raw SPI flash chip id 0x%x", raw_flash_id);

    mfg_id = raw_flash_id & 0xFF;
    flash_id = (raw_flash_id >> 16) | (raw_flash_id & 0xFF00);
    ESP_LOGD(TAG, "Manufacturer ID 0x%02x chip ID 0x%04x", mfg_id, flash_id);

    for (i = 0; i < NUM_CHIPS-1; i++) {
        const qio_info_t *chip = &chip_data[i];
        if (mfg_id == chip->mfg_id && (flash_id & chip->id_mask) == (chip->flash_id & chip->id_mask)) {
            ESP_LOGI(TAG, "Enabling QIO for flash chip %s", chip_data[i].manufacturer);
            break;
        }
    }

    if (i == NUM_CHIPS - 1) {
        ESP_LOGI(TAG, "Enabling default flash chip QIO");
    }

    esp_err_t res = enable_qio_mode(chip_data[i].read_status_fn,
                                    chip_data[i].write_status_fn,
                                    chip_data[i].status_qio_bit);
    if (res != ESP_OK) {
        // Restore SPI flash CTRL setting, to keep us in DIO/DOUT mode
        SPIFLASH.ctrl.val = old_ctrl_reg;
    }
}

static esp_err_t enable_qio_mode(read_status_fn_t read_status_fn,
                            write_status_fn_t write_status_fn,
                            uint8_t status_qio_bit)
{
    uint32_t status;
    const uint32_t spiconfig = ets_efuse_get_spiconfig();

    if (spiconfig != EFUSE_SPICONFIG_SPI_DEFAULTS && spiconfig != EFUSE_SPICONFIG_HSPI_DEFAULTS) {
        // spiconfig specifies a custom efuse pin configuration. This config defines all pins -except- WP,
        // which is compiled into the bootloader instead.
        //
        // Most commonly an overriden pin mapping means ESP32-D2WD. Warn if chip is ESP32-D2WD
        // but someone has changed the WP pin assignment from that chip's WP pin.
        uint32_t chip_ver = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_RESERVE);
        uint32_t pkg_ver = chip_ver & 0x7;
        const int PKG_VER_ESP32_D2WD = 2; // TODO: use chip detection API once available
        if (pkg_ver == PKG_VER_ESP32_D2WD && CONFIG_BOOTLOADER_SPI_WP_PIN != ESP32_D2WD_WP_GPIO) {
            ESP_LOGW(TAG, "Chip is ESP32-D2WD but flash WP pin is different value to internal flash");
        }
    }

    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    status = read_status_fn();
    ESP_LOGD(TAG, "Initial flash chip status 0x%x", status);

    if ((status & (1<<status_qio_bit)) == 0) {
        execute_flash_command(CMD_WREN, 0, 0, 0);
        write_status_fn(status | (1<<status_qio_bit));

        esp_rom_spiflash_wait_idle(&g_rom_flashchip);

        status = read_status_fn();
        ESP_LOGD(TAG, "Updated flash chip status 0x%x", status);
        if ((status & (1<<status_qio_bit)) == 0) {
            ESP_LOGE(TAG, "Failed to set QIE bit, not enabling QIO mode");
            return ESP_FAIL;
        }

    } else {
        ESP_LOGD(TAG, "QIO mode already enabled in flash");
    }

    ESP_LOGD(TAG, "Enabling QIO mode...");

    esp_rom_spiflash_read_mode_t mode;
#if CONFIG_FLASHMODE_QOUT
    mode = ESP_ROM_SPIFLASH_QOUT_MODE;
#else
    mode = ESP_ROM_SPIFLASH_QIO_MODE;
#endif

    esp_rom_spiflash_config_readmode(mode);

    esp_rom_spiflash_select_qio_pins(CONFIG_BOOTLOADER_SPI_WP_PIN, spiconfig);

    return ESP_OK;
}

static unsigned read_status_8b_rdsr()
{
    return execute_flash_command(CMD_RDSR, 0, 0, 8);
}

static unsigned read_status_8b_rdsr2()
{
    return execute_flash_command(CMD_RDSR2, 0, 0, 8);
}

static unsigned read_status_16b_rdsr_rdsr2()
{
    return execute_flash_command(CMD_RDSR, 0, 0, 8) | (execute_flash_command(CMD_RDSR2, 0, 0, 8) << 8);
}

static void write_status_8b_wrsr(unsigned new_status)
{
    execute_flash_command(CMD_WRSR, new_status, 8, 0);
}

static void write_status_8b_wrsr2(unsigned new_status)
{
    execute_flash_command(CMD_WRSR2, new_status, 8, 0);
}

static void write_status_16b_wrsr(unsigned new_status)
{
    execute_flash_command(CMD_WRSR, new_status, 16, 0);
}

static uint32_t execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
{
    SPIFLASH.user2.usr_command_value = command;
    SPIFLASH.user.usr_miso = miso_len > 0;
    SPIFLASH.miso_dlen.usr_miso_dbitlen = miso_len ? (miso_len - 1) : 0;
    SPIFLASH.user.usr_mosi = mosi_len > 0;
    SPIFLASH.mosi_dlen.usr_mosi_dbitlen = mosi_len ? (mosi_len - 1) : 0;
    SPIFLASH.data_buf[0] = mosi_data;

    if (g_rom_spiflash_dummy_len_plus[1]) {
        /* When flash pins are mapped via GPIO matrix, need a dummy cycle before reading via MISO */
        if (miso_len > 0) {
            SPIFLASH.user.usr_dummy = 1;
            SPIFLASH.user1.usr_dummy_cyclelen = g_rom_spiflash_dummy_len_plus[1] - 1;
        } else {
            SPIFLASH.user.usr_dummy = 0;
            SPIFLASH.user1.usr_dummy_cyclelen = 0;
        }
    }

    SPIFLASH.cmd.usr = 1;
    while(SPIFLASH.cmd.usr != 0)
    { }

    return SPIFLASH.data_buf[0];
}
