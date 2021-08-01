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
#include "bootloader_flash_config.h"
#include "esp_attr.h"
#include "flash_qio_mode.h"
#include "esp_log.h"
#include "esp_err.h"
#include "rom/ets_sys.h"
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
#define CMD_OTPEN      0x3A /* Enable OTP mode, not all SPI flash uses this command */
#define CMD_RDSFDP     0x5A /* Read the SFDP of the flash */

#define BYTESHIFT(VAR, IDX)    (((VAR) >> ((IDX) * 8)) & 0xFF)


static DRAM_ATTR char TAG[] = "qio_mode";

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

/* Read 8 bit status of XM25QU64A  */
static unsigned read_status_8b_xmc25qu64a();
/* Write 8 bit status of XM25QU64A */
static void write_status_8b_xmc25qu64a(unsigned new_status);

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
uint32_t IRAM_ATTR bootloader_read_flash_id(void)
{
    uint32_t id = execute_flash_command(CMD_RDID, 0, 0, 24);
    id = ((id & 0xff) << 16) | ((id >> 16) & 0xff) | (id & 0xff00);
    return id;
}

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

    enable_qio_mode(chip_data[i].read_status_fn,
                    chip_data[i].write_status_fn,
                    chip_data[i].status_qio_bit);
}

static esp_err_t enable_qio_mode(read_status_fn_t read_status_fn,
                            write_status_fn_t write_status_fn,
                            uint8_t status_qio_bit)
{
    uint32_t status;
    const uint32_t spiconfig = ets_efuse_get_spiconfig();

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

    int wp_pin = bootloader_flash_get_wp_pin();
    esp_rom_spiflash_select_qio_pins(wp_pin, spiconfig);

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

static unsigned read_status_8b_xmc25qu64a()
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

IRAM_ATTR static uint32_t bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    assert(mosi_len <= 32);
    assert(miso_len <= 32);
    uint32_t old_ctrl_reg = SPIFLASH.ctrl.val;
    SPIFLASH.ctrl.val = SPI_WP_REG_M; // keep WP high while idle, otherwise leave DIO mode
    //command phase
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;
    SPIFLASH.user2.usr_command_value = command;
    //addr phase
    SPIFLASH.user.usr_addr = addr_len > 0;
    SPIFLASH.user1.usr_addr_bitlen = addr_len - 1;
    SPIFLASH.addr = (addr_len > 0)? (address << (32-addr_len)) : 0;
    //dummy phase
    if (miso_len > 0) {
        uint32_t total_dummy = dummy_len + g_rom_spiflash_dummy_len_plus[1];
        SPIFLASH.user.usr_dummy = total_dummy > 0;
        SPIFLASH.user1.usr_dummy_cyclelen = total_dummy - 1;
    } else {
        SPIFLASH.user.usr_dummy = 0;
        SPIFLASH.user1.usr_dummy_cyclelen = 0;
    }
    //output data
    SPIFLASH.user.usr_mosi = mosi_len > 0;
    SPIFLASH.mosi_dlen.usr_mosi_dbitlen = mosi_len ? (mosi_len - 1) : 0;
    SPIFLASH.data_buf[0] = mosi_data;
    //input data
    SPIFLASH.user.usr_miso = miso_len > 0;
    SPIFLASH.miso_dlen.usr_miso_dbitlen = miso_len ? (miso_len - 1) : 0;

    SPIFLASH.cmd.usr = 1;
    while(SPIFLASH.cmd.usr != 0)
    { }
    SPIFLASH.ctrl.val = old_ctrl_reg;

    uint32_t ret = SPIFLASH.data_buf[0];
    if (miso_len < 32) {
        //set unused bits to 0
        ret &= ~(UINT32_MAX << miso_len);
    }
    return ret;
}

static uint32_t IRAM_ATTR execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
{
    const uint8_t addr_len = 0;
    const uint8_t address = 0;
    const uint8_t dummy_len = 0;

    return bootloader_flash_execute_command_common(command, addr_len, address,
            dummy_len, mosi_len, mosi_data, miso_len);
}

// cmd(0x5A) + 24bit address + 8 cycles dummy
uint32_t IRAM_ATTR bootloader_flash_read_sfdp(uint32_t sfdp_addr, unsigned int miso_byte_num)
{
    assert(miso_byte_num <= 4);
    const uint8_t command = CMD_RDSFDP;
    const uint8_t addr_len = 24;
    const uint8_t dummy_len = 8;
    const uint8_t mosi_len = 0;
    const uint32_t mosi_data = 0;
    const uint8_t miso_len = miso_byte_num * 8;

    return bootloader_flash_execute_command_common(command, addr_len, sfdp_addr,
            dummy_len, mosi_len, mosi_data, miso_len);
}

/*******************************************************************************
 * XMC startup flow
 ******************************************************************************/

#define XMC_SUPPORT CONFIG_BOOTLOADER_FLASH_XMC_SUPPORT
#define XMC_VENDOR_ID 0x20

#if BOOTLOADER_BUILD
#define BOOTLOADER_FLASH_LOG(level, ...)    ESP_LOG##level(TAG, ##__VA_ARGS__)
#else
#define BOOTLOADER_FLASH_LOG(level, ...)    ESP_DRAM_LOG##level(TAG, ##__VA_ARGS__)

#define ESP_DRAM_LOGE( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_ERROR,   E, ##__VA_ARGS__)
#define ESP_DRAM_LOGW( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_WARN,   E, ##__VA_ARGS__)
#define ESP_DRAM_LOGI( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_INFO,   E, ##__VA_ARGS__)
#define ESP_DRAM_LOGD( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_DEBUG,   E, ##__VA_ARGS__)
#define ESP_DRAM_LOGV( tag, format, ... ) ESP_DRAM_LOG_IMPL(tag, format, ESP_LOG_VERBOSE,   E, ##__VA_ARGS__)

#define ESP_DRAM_LOG_IMPL(tag, format, log_level, log_tag_letter, ...) do {                       \
        if (LOG_LOCAL_LEVEL >= (log_level)) {                                                  \
            ets_printf(DRAM_STR(#log_tag_letter " %s: " format "\n"), tag, ##__VA_ARGS__); \
        }} while(0)

#endif

#if XMC_SUPPORT
//strictly check the model
static IRAM_ATTR bool is_xmc_chip_strict(uint32_t rdid)
{
    uint32_t vendor_id = BYTESHIFT(rdid, 2);
    uint32_t mfid = BYTESHIFT(rdid, 1);
    uint32_t cpid = BYTESHIFT(rdid, 0);

    if (vendor_id != XMC_VENDOR_ID) {
        return false;
    }

    bool matched = false;
    if (mfid == 0x40) {
        if (cpid >= 0x13 && cpid <= 0x20) {
            matched = true;
        }
    } else if (mfid == 0x41) {
        if (cpid >= 0x17 && cpid <= 0x20) {
            matched = true;
        }
    } else if (mfid == 0x50) {
        if (cpid >= 0x15 && cpid <= 0x16) {
            matched =  true;
        }
    }
    return matched;
}

esp_err_t IRAM_ATTR bootloader_flash_xmc_startup(void)
{
    // If the RDID value is a valid XMC one, may skip the flow
    const bool fast_check = true;
    if (fast_check && is_xmc_chip_strict(g_rom_flashchip.device_id)) {
        BOOTLOADER_FLASH_LOG(D, "XMC chip detected by RDID (%08X), skip.", g_rom_flashchip.device_id);
        return ESP_OK;
    }

    // Check the Manufacturer ID in SFDP registers (JEDEC standard). If not XMC chip, no need to run the flow
    const int sfdp_mfid_addr = 0x10;
    uint8_t mf_id = (bootloader_flash_read_sfdp(sfdp_mfid_addr, 1) & 0xff);
    if (mf_id != XMC_VENDOR_ID) {
        BOOTLOADER_FLASH_LOG(D, "non-XMC chip detected by SFDP Read (%02X), skip.", mf_id);
        return ESP_OK;
    }

    BOOTLOADER_FLASH_LOG(I, "XM25QHxxC startup flow");
    // Enter DPD
    execute_flash_command(0xB9, 0, 0, 0);
    // Enter UDPD
    execute_flash_command(0x79, 0, 0, 0);
    // Exit UDPD
    execute_flash_command(0xFF, 0, 0, 0);
    // Delay tXUDPD
    ets_delay_us(2000);
    // Release Power-down
    execute_flash_command(0xAB, 0, 0, 0);
    ets_delay_us(20);
    // Read flash ID and check again
    g_rom_flashchip.device_id = bootloader_read_flash_id();
    if (!is_xmc_chip_strict(g_rom_flashchip.device_id)) {
        BOOTLOADER_FLASH_LOG(E, "XMC flash startup fail");
        return ESP_FAIL;
    }

    return ESP_OK;
}

#else
//only compare the vendor id
static IRAM_ATTR bool is_xmc_chip(uint32_t rdid)
{
    uint32_t vendor_id = (rdid >> 16) & 0xFF;
    return (vendor_id == XMC_VENDOR_ID);
}

esp_err_t IRAM_ATTR bootloader_flash_xmc_startup(void)
{
    if (is_xmc_chip(g_rom_flashchip.device_id)) {
        BOOTLOADER_FLASH_LOG(E, "XMC chip detected (%08X) while support disabled.", g_rom_flashchip.device_id);
        return ESP_FAIL;
    }
    return ESP_OK;
}

#endif //XMC_SUPPORT

