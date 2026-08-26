/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "bootloader_flash_override.h"
#include "bootloader_flash_priv.h"
#include "bootloader_flash_custom.h"
#include "esp_rom_spiflash.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"


// All used flash chips that need special QIO operations need to be added here. The simplest way is to copy all entries from bootloader_..c from ESP-IDF and add your own at the beginning.
const DRAM_ATTR bootloader_qio_info_t bootloader_flash_qe_support_list_user[] = {
    /*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,               QIE Bit */
    { "GD",          0xC8,   0x4000, 0xFFFF,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
    { "EON",         0x1C,   0x7000, 0xFF00,    bootloader_read_status_otp_mode_8b,    bootloader_write_status_otp_mode_8b,   6 },

    /* Final entry is default entry, if no other IDs have matched.

       This approach works for chips including:
       GigaDevice (mfg ID 0xC8, flash IDs including 4016),
       FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
       BY25Q32 (mfg ID 0x68, flash IDs including 4016)
    */
    { NULL,          0xFF,    0xFFFF, 0xFFFF,   bootloader_read_status_8b_rdsr2,       bootloader_write_status_8b_wrsr2,      1 },
};

const DRAM_ATTR bootloader_qio_info_t* bootloader_flash_qe_support_list = bootloader_flash_qe_support_list_user;

uint8_t DRAM_ATTR bootloader_flash_qe_list_count = (sizeof(bootloader_flash_qe_support_list_user) / sizeof(bootloader_qio_info_t));
