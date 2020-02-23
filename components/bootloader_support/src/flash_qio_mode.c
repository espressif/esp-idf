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
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#include "esp32/rom/efuse.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/spi_flash.h"
#include "esp32s2/rom/efuse.h"
#include "soc/spi_mem_struct.h"
#endif
#include "soc/spi_struct.h"
#include "soc/spi_reg.h"
#include "soc/efuse_periph.h"
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"

/* SPI flash controller */
#if CONFIG_IDF_TARGET_ESP32
#define SPIFLASH SPI1
#elif CONFIG_IDF_TARGET_ESP32S2
#define SPIFLASH SPIMEM1
#endif

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
#define CMD_OTPEN      0x3A /* Enable OTP mode, not all SPI flash uses this command */

static const char *TAG = "qio_mode";

typedef unsigned (*read_status_fn_t)(void);
typedef void (*write_status_fn_t)(unsigned);

typedef struct __attribute__((packed))
{
    const char *manufacturer;
    uint8_t mfg_id; /* 8-bit JEDEC manufacturer ID */
    uint16_t flash_id; /* 16-bit JEDEC flash chip ID */
    uint16_t id_mask; /* Bits to match on in flash chip ID */
    read_status_fn_t read_status_fn;
    write_status_fn_t write_status_fn;
    uint8_t status_qio_bit;
} qio_info_t;

/* Read 8 bit status using RDSR command */
static unsigned read_status_8b_rdsr(void);
/* Read 8 bit status (second byte) using RDSR2 command */
static unsigned read_status_8b_rdsr2(void);
/* read 16 bit status using RDSR & RDSR2 (low and high bytes) */
static unsigned read_status_16b_rdsr_rdsr2(void);

/* Write 8 bit status using WRSR */
static void write_status_8b_wrsr(unsigned new_status);
/* Write 8 bit status (second byte) using WRSR2 */
static void write_status_8b_wrsr2(unsigned new_status);
/* Write 16 bit status using WRSR */
static void write_status_16b_wrsr(unsigned new_status);

/* Read 8 bit status of XM25QU64A  */
static unsigned read_status_8b_xmc25qu64a(void);
/* Write 8 bit status of XM25QU64A */
static void write_status_8b_xmc25qu64a(unsigned new_status);

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
    /*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,               QIE Bit */
    { "MXIC",        0xC2,   0x2000, 0xFF00,    read_status_8b_rdsr,        write_status_8b_wrsr,       6 },
    { "ISSI",        0x9D,   0x4000, 0xCF00,    read_status_8b_rdsr,        write_status_8b_wrsr,       6 }, /* IDs 0x40xx, 0x70xx */
    { "WinBond",     0xEF,   0x4000, 0xFF00,    read_status_16b_rdsr_rdsr2, write_status_16b_wrsr,      9 },
    { "GD",          0xC8,   0x6000, 0xFF00,    read_status_16b_rdsr_rdsr2, write_status_16b_wrsr,      9 },
    { "XM25QU64A",   0x20,   0x3817, 0xFFFF,    read_status_8b_xmc25qu64a,  write_status_8b_xmc25qu64a, 6 },

    /* Final entry is default entry, if no other IDs have matched.

       This approach works for chips including:
       GigaDevice (mfg ID 0xC8, flash IDs including 4016),
       FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
    */
    { NULL,          0xFF,    0xFFFF, 0xFFFF,   read_status_8b_rdsr2,       write_status_8b_wrsr2,      1 },
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
uint32_t bootloader_read_flash_id(void)
{
    uint32_t id = execute_flash_command(CMD_RDID, 0, 0, 24);
    id = ((id & 0xff) << 16) | ((id >> 16) & 0xff) | (id & 0xff00);
    return id;
}

#if CONFIG_IDF_TARGET_ESP32S2
#define FLASH_WRAP_CMD   0x77
typedef enum {
    FLASH_WRAP_MODE_8B  = 0,
    FLASH_WRAP_MODE_16B = 2,
    FLASH_WRAP_MODE_32B = 4,
    FLASH_WRAP_MODE_64B = 6,
    FLASH_WRAP_MODE_DISABLE = 1
} spi_flash_wrap_mode_t;
static esp_err_t spi_flash_wrap_set(spi_flash_wrap_mode_t mode)
{
    uint32_t reg_bkp_ctrl = SPIFLASH.ctrl.val;
    uint32_t reg_bkp_usr  = SPIFLASH.user.val;
    SPIFLASH.user.fwrite_dio = 0;
    SPIFLASH.user.fwrite_dual = 0;
    SPIFLASH.user.fwrite_qio = 1;
    SPIFLASH.user.fwrite_quad = 0;
    SPIFLASH.ctrl.fcmd_dual = 0;
    SPIFLASH.ctrl.fcmd_quad = 0;
    SPIFLASH.user.usr_dummy = 0;
    SPIFLASH.user.usr_addr = 1;
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;
    SPIFLASH.user2.usr_command_value = FLASH_WRAP_CMD;
    SPIFLASH.user1.usr_addr_bitlen = 23;
    SPIFLASH.addr = 0;
    SPIFLASH.user.usr_miso = 0;
    SPIFLASH.user.usr_mosi = 1;
    SPIFLASH.mosi_dlen.usr_mosi_bit_len = 7;
    SPIFLASH.data_buf[0] = (uint32_t) mode << 4;;
    SPIFLASH.cmd.usr = 1;
    while (SPIFLASH.cmd.usr != 0) {
    }

    SPIFLASH.ctrl.val = reg_bkp_ctrl;
    SPIFLASH.user.val = reg_bkp_usr;
    return ESP_OK;
}
#endif

void bootloader_enable_qio_mode(void)
{
    uint32_t raw_flash_id;
    uint8_t mfg_id;
    uint16_t flash_id;
    int i;

    ESP_LOGD(TAG, "Probing for QIO mode enable...");
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    raw_flash_id = g_rom_flashchip.device_id;
    ESP_LOGD(TAG, "Raw SPI flash chip id 0x%x", raw_flash_id);

    mfg_id = (raw_flash_id >> 16) & 0xFF;
    flash_id = raw_flash_id & 0xFFFF;
    ESP_LOGD(TAG, "Manufacturer ID 0x%02x chip ID 0x%04x", mfg_id, flash_id);

    for (i = 0; i < NUM_CHIPS - 1; i++) {
        const qio_info_t *chip = &chip_data[i];
        if (mfg_id == chip->mfg_id && (flash_id & chip->id_mask) == (chip->flash_id & chip->id_mask)) {
            ESP_LOGI(TAG, "Enabling QIO for flash chip %s", chip_data[i].manufacturer);
            break;
        }
    }

    if (i == NUM_CHIPS - 1) {
        ESP_LOGI(TAG, "Enabling default flash chip QIO");
    }
    enable_qio_mode(chip_data[i].read_status_fn,
                    chip_data[i].write_status_fn,
                    chip_data[i].status_qio_bit);
#if CONFIG_IDF_TARGET_ESP32S2
    spi_flash_wrap_set(FLASH_WRAP_MODE_DISABLE);
#endif
}

static esp_err_t enable_qio_mode(read_status_fn_t read_status_fn,
                                 write_status_fn_t write_status_fn,
                                 uint8_t status_qio_bit)
{
    uint32_t status;
    const uint32_t spiconfig = ets_efuse_get_spiconfig();

#if CONFIG_IDF_TARGET_ESP32
    if (spiconfig != EFUSE_SPICONFIG_SPI_DEFAULTS && spiconfig != EFUSE_SPICONFIG_HSPI_DEFAULTS) {
        // spiconfig specifies a custom efuse pin configuration. This config defines all pins -except- WP,
        // which is compiled into the bootloader instead.
        //
        // Most commonly an overriden pin mapping means ESP32-D2WD or ESP32-PICOD4.
        //Warn if chip is ESP32-D2WD/ESP32-PICOD4 but someone has changed the WP pin
        //assignment from that chip's WP pin.
        uint32_t pkg_ver = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG);
        if (CONFIG_BOOTLOADER_SPI_WP_PIN != ESP32_D2WD_WP_GPIO  &&
                (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5 ||
                 pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2 ||
                 pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4)) {
            ESP_LOGW(TAG, "Chip is ESP32-D2WD/ESP32-PICOD4 but flash WP pin is different value to internal flash");
        }
    }
#endif

    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    status = read_status_fn();
    ESP_LOGD(TAG, "Initial flash chip status 0x%x", status);

    if ((status & (1 << status_qio_bit)) == 0) {
        execute_flash_command(CMD_WREN, 0, 0, 0);
        write_status_fn(status | (1 << status_qio_bit));

        esp_rom_spiflash_wait_idle(&g_rom_flashchip);

        status = read_status_fn();
        ESP_LOGD(TAG, "Updated flash chip status 0x%x", status);
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

#if CONFIG_IDF_TARGET_ESP32
    esp_rom_spiflash_select_qio_pins(CONFIG_BOOTLOADER_SPI_WP_PIN, spiconfig);
#elif CONFIG_IDF_TARGET_ESP32S2
    if (ets_efuse_get_wp_pad() <= MAX_PAD_GPIO_NUM) {
        esp_rom_spiflash_select_qio_pins(ets_efuse_get_wp_pad(), spiconfig);
    } else {
        esp_rom_spiflash_select_qio_pins(CONFIG_BOOTLOADER_SPI_WP_PIN, spiconfig);
    }
#endif
    return ESP_OK;
}

static unsigned read_status_8b_rdsr(void)
{
    return execute_flash_command(CMD_RDSR, 0, 0, 8);
}

static unsigned read_status_8b_rdsr2(void)
{
    return execute_flash_command(CMD_RDSR2, 0, 0, 8);
}

static unsigned read_status_16b_rdsr_rdsr2(void)
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

static unsigned read_status_8b_xmc25qu64a(void)
{
    execute_flash_command(CMD_OTPEN, 0, 0, 0);  /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    uint32_t read_status = execute_flash_command(CMD_RDSR, 0, 0, 8);
    execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
    return read_status;
}

static void write_status_8b_xmc25qu64a(unsigned new_status)
{
    execute_flash_command(CMD_OTPEN, 0, 0, 0);  /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    execute_flash_command(CMD_WRSR, new_status, 8, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
}

static uint32_t execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
{
    uint32_t old_ctrl_reg = SPIFLASH.ctrl.val;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.ctrl.val = SPI_WP_REG_M; // keep WP high while idle, otherwise leave DIO mode
#elif CONFIG_IDF_TARGET_ESP32S2
    SPIFLASH.ctrl.val = SPI_MEM_WP_REG_M; // keep WP high while idle, otherwise leave DIO mode
#endif
    SPIFLASH.user.usr_dummy = 0;
    SPIFLASH.user.usr_addr = 0;
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;

    SPIFLASH.user2.usr_command_value = command;
    SPIFLASH.user.usr_miso = miso_len > 0;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.miso_dlen.usr_miso_dbitlen = miso_len ? (miso_len - 1) : 0;
#elif CONFIG_IDF_TARGET_ESP32S2
    SPIFLASH.miso_dlen.usr_miso_bit_len = miso_len ? (miso_len - 1) : 0;
#endif
    SPIFLASH.user.usr_mosi = mosi_len > 0;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.mosi_dlen.usr_mosi_dbitlen = mosi_len ? (mosi_len - 1) : 0;
#elif CONFIG_IDF_TARGET_ESP32S2
    SPIFLASH.mosi_dlen.usr_mosi_bit_len = mosi_len ? (mosi_len - 1) : 0;
#endif
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
    while (SPIFLASH.cmd.usr != 0) {
    }

    SPIFLASH.ctrl.val = old_ctrl_reg;
    return SPIFLASH.data_buf[0];
}
