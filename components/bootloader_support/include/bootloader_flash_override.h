/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned (*bootloader_flash_read_status_fn_t)(void);
typedef void (*bootloader_flash_write_status_fn_t)(unsigned);

typedef struct __attribute__((packed))
{
    const char *manufacturer;
    uint8_t mfg_id; /* 8-bit JEDEC manufacturer ID */
    uint16_t flash_id; /* 16-bit JEDEC flash chip ID */
    uint16_t id_mask; /* Bits to match on in flash chip ID */
    bootloader_flash_read_status_fn_t read_status_fn;
    bootloader_flash_write_status_fn_t write_status_fn;
    uint8_t status_qio_bit;
} bootloader_qio_info_t;

/**
 * @brief Read 8 bit status using RDSR command
 *
 * @return Value of SR1.
 */
unsigned bootloader_read_status_8b_rdsr(void);

/**
 * @brief Read 8 bit status (second byte) using RDSR2 command
 *
 * @return Value of SR2
 */
unsigned bootloader_read_status_8b_rdsr2(void);

/**
 * @brief Read 16 bit status using RDSR & RDSR2 (low and high bytes)
 *
 * @return Value of SR2#SR1.
 */
unsigned bootloader_read_status_16b_rdsr_rdsr2(void);

/**
 * @brief Write 8 bit status using WRSR
 */
void bootloader_write_status_8b_wrsr(unsigned new_status);

/**
 * @brief Write 8 bit status (second byte) using WRSR2.
 */
void bootloader_write_status_8b_wrsr2(unsigned new_status);

/**
 * @brief Write 16 bit status using WRSR, (both write SR1 and SR2)
 */
void bootloader_write_status_16b_wrsr(unsigned new_status);

/**
 * @brief Read 8 bit status of XM25QU64A.
 *
 * @return Value of 8 bit SR.
 */
unsigned bootloader_read_status_8b_xmc25qu64a(void);

/**
 * @brief Write 8 bit status for XM25QU64A
 */
void bootloader_write_status_8b_xmc25qu64a(unsigned new_status);

/* Array of known flash chips and data to enable Quad I/O mode

   Manufacturer & flash ID can be tested by running "esptool.py
   flash_id"

   If manufacturer ID matches, and flash ID ORed with flash ID mask
   matches, enable_qio_mode() will execute "Read Cmd", test if bit
   number "QIE Bit" is set, and if not set it will call "Write Cmd"
   with this bit set.

   Searching of this table stops when the first match is found.
 */
extern const bootloader_qio_info_t __attribute__((weak)) bootloader_flash_qe_support_list[];

/**
  * @brief Unlock Flash write protect.
  *        Please do not call this function in SDK.
  *
  * @note This can be overridden because it's attribute weak.
  */
esp_err_t IRAM_ATTR __attribute__((weak)) bootloader_flash_unlock(void);

#ifdef __cplusplus
}
#endif
