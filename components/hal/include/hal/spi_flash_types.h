/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include <esp_bit_defs.h>
#include "esp_flash_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Definition of a common transaction. Also holds the return value. */
typedef struct {
    uint8_t reserved;           ///< Reserved, must be 0.
    uint8_t mosi_len;           ///< Output data length, in bytes
    uint8_t miso_len;           ///< Input data length, in bytes
    uint8_t address_bitlen;     ///< Length of address in bits, set to 0 if command does not need an address
    uint32_t address;           ///< Address to perform operation on
    const uint8_t *mosi_data;   ///< Output data to salve
    uint8_t *miso_data;         ///< [out] Input data from slave, little endian
    uint32_t flags;             ///< Flags for this transaction. Set to 0 for now.
#define SPI_FLASH_TRANS_FLAG_CMD16          BIT(0)  ///< Send command of 16 bits
#define SPI_FLASH_TRANS_FLAG_IGNORE_BASEIO  BIT(1)  ///< Not applying the basic io mode configuration for this transaction
#define SPI_FLASH_TRANS_FLAG_BYTE_SWAP      BIT(2)  ///< Used for DTR mode, to swap the bytes of a pair of rising/falling edge
#define SPI_FLASH_TRANS_FLAG_PE_CMD         BIT(3)  ///< Indicates that this transaction is to erase/program flash chip.
    uint16_t command;           ///< Command to send
    uint8_t dummy_bitlen;       ///< Basic dummy bits to use
    uint32_t io_mode;           ///< Flash working mode when `SPI_FLASH_IGNORE_BASEIO` is specified.
} spi_flash_trans_t;

/**
 * @brief SPI flash clock speed values, always refer to them by the enum rather
 * than the actual value (more speed may be appended into the list).
 *
 * A strategy to select the maximum allowed speed is to enumerate from the
 * ``ESP_FLSH_SPEED_MAX-1`` or highest frequency supported by your flash, and
 * decrease the speed until the probing success.
 */
typedef enum esp_flash_speed_s {
    ESP_FLASH_5MHZ = 5, ///< The flash runs under 5MHz
    ESP_FLASH_10MHZ = 10,    ///< The flash runs under 10MHz
    ESP_FLASH_20MHZ = 20,    ///< The flash runs under 20MHz
    ESP_FLASH_26MHZ = 26,    ///< The flash runs under 26MHz
    ESP_FLASH_40MHZ = 40,    ///< The flash runs under 40MHz
    ESP_FLASH_80MHZ = 80,    ///< The flash runs under 80MHz
    ESP_FLASH_120MHZ = 120,   ///< The flash runs under 120MHz, 120MHZ can only be used by main flash after timing tuning in system. Do not use this directely in any API.
    ESP_FLASH_SPEED_MAX, ///< The maximum frequency supported by the host is ``ESP_FLASH_SPEED_MAX-1``.
} esp_flash_speed_t __attribute__((deprecated));

// These bits are not quite like "IO mode", but are able to be appended into the io mode and used by the HAL.
#define SPI_FLASH_CONFIG_CONF_BITS      BIT(31) ///< OR the io_mode with this mask, to enable the dummy output feature or replace the first several dummy bits into address to meet the requirements of conf bits. (Used in DIO/QIO/OIO mode)

/** @brief Mode used for reading from SPI flash */
typedef enum {
    SPI_FLASH_SLOWRD = 0, ///< Data read using single I/O, some limits on speed
    SPI_FLASH_FASTRD, ///< Data read using single I/O, no limit on speed
    SPI_FLASH_DOUT,   ///< Data read using dual I/O
    SPI_FLASH_DIO,    ///< Both address & data transferred using dual I/O
    SPI_FLASH_QOUT,   ///< Data read using quad I/O
    SPI_FLASH_QIO,    ///< Both address & data transferred using quad I/O
#define SPI_FLASH_OPI_FLAG 16    ///< A flag for flash work in opi mode, the io mode below are opi, above are SPI/QSPI mode. DO NOT use this value in any API.
    SPI_FLASH_OPI_STR = SPI_FLASH_OPI_FLAG,///< Only support on OPI flash, flash read and write under STR mode
    SPI_FLASH_OPI_DTR,///< Only support on OPI flash, flash read and write under DTR mode
    SPI_FLASH_READ_MODE_MAX,    ///< The fastest io mode supported by the host is ``ESP_FLASH_READ_MODE_MAX-1``.
} esp_flash_io_mode_t;

/// Configuration structure for the flash chip suspend feature.
typedef struct {
    uint32_t sus_mask;     ///< SUS/SUS1/SUS2 bit in flash register.
    struct {
        uint32_t cmd_rdsr    :8;             ///< Read flash status register(2) command.
        uint32_t sus_cmd     :8;             ///< Flash suspend command.
        uint32_t res_cmd     :8;             ///< Flash resume command.
        uint32_t reserved    :8;             ///< Reserved, set to 0.
    };
} spi_flash_sus_cmd_conf;

/// Structure for flash encryption operations.
typedef struct
{
    /**
     * @brief Enable the flash encryption
    */
    void (*flash_encryption_enable)(void);
    /**
     * @brief Disable the flash encryption
    */
    void (*flash_encryption_disable)(void);
    /**
     * Prepare flash encryption before operation.
     *
     * @param address The destination address in flash for the write operation.
     * @param buffer Data for programming
     * @param size Size to program.
     *
     * @note address and buffer must be 8-word aligned.
     */
    void (*flash_encryption_data_prepare)(uint32_t address, const uint32_t* buffer, uint32_t size);
    /**
     * @brief flash data encryption operation is done.
     */
    void (*flash_encryption_done)(void);
    /**
     * Destroy encrypted result
    */
    void (*flash_encryption_destroy)(void);
    /**
     * Check if is qualified to encrypt the buffer
     *
     * @param address the address of written flash partition.
     * @param length Buffer size.
     */
    bool (*flash_encryption_check)(uint32_t address, uint32_t length);
} spi_flash_encryption_t;

///Slowest io mode supported by ESP32, currently SlowRd
#define SPI_FLASH_READ_MODE_MIN SPI_FLASH_SLOWRD

struct spi_flash_host_driver_s;
typedef struct spi_flash_host_driver_s spi_flash_host_driver_t;

/** SPI Flash Host driver instance */
typedef struct {
    const struct spi_flash_host_driver_s* driver;  ///< Pointer to the implementation function table
    // Implementations can wrap this structure into their own ones, and append other data here
} spi_flash_host_inst_t ;


/** Host driver configuration and context structure. */
struct spi_flash_host_driver_s {
    /**
     * Configure the device-related register before transactions. This saves
     * some time to re-configure those registers when we send continuously
     */
    esp_err_t (*dev_config)(spi_flash_host_inst_t *host);
    /**
     * Send an user-defined spi transaction to the device.
     */
    esp_err_t (*common_command)(spi_flash_host_inst_t *host, spi_flash_trans_t *t);
    /**
     * Read flash ID.
     */
    esp_err_t (*read_id)(spi_flash_host_inst_t *host, uint32_t *id);
    /**
     * Erase whole flash chip.
     */
    void (*erase_chip)(spi_flash_host_inst_t *host);
    /**
     * Erase a specific sector by its start address.
     */
    void (*erase_sector)(spi_flash_host_inst_t *host, uint32_t start_address);
    /**
     * Erase a specific block by its start address.
     */
    void (*erase_block)(spi_flash_host_inst_t *host, uint32_t start_address);
    /**
     * Read the status of the flash chip.
     */
    esp_err_t (*read_status)(spi_flash_host_inst_t *host, uint8_t *out_sr);
    /**
     * Disable write protection.
     */
    esp_err_t (*set_write_protect)(spi_flash_host_inst_t *host, bool wp);
    /**
     * Program a page of the flash. Check ``max_write_bytes`` for the maximum allowed writing length.
     */
    void (*program_page)(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length);
    /**
     * @brief Check whether the SPI host supports direct write
     *
     * When cache is disabled, SPI1 doesn't support directly write when buffer isn't internal.
     */
    bool (*supports_direct_write)(spi_flash_host_inst_t *host, const void *p);
    /**
     * Slicer for write data. The `program_page` should be called iteratively with the return value
     * of this function.
     *
     * @param address Beginning flash address to write
     * @param len Length request to write
     * @param align_addr Output of the aligned address to write to
     * @param page_size Physical page size of the flash chip
     * @return Length that can be actually written in one `program_page` call
     */
    int (*write_data_slicer)(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_addr,
                             uint32_t page_size);
    /**
     * Read data from the flash. Check ``max_read_bytes`` for the maximum allowed reading length.
     */
    esp_err_t (*read)(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len);
    /**
     * @brief Check whether the SPI host supports direct read
     *
     * When cache is disabled, SPI1 doesn't support directly read when the given buffer isn't internal.
     */
    bool (*supports_direct_read)(spi_flash_host_inst_t *host, const void *p);
    /**
     * Slicer for read data. The `read` should be called iteratively with the return value
     * of this function.
     *
     * @param address Beginning flash address to read
     * @param len Length request to read
     * @param align_addr Output of the aligned address to read
     * @param page_size Physical page size of the flash chip
     * @return Length that can be actually read in one `read` call
     */
    int (*read_data_slicer)(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_addr, uint32_t page_size);
    /**
     * Check the host status, 0:busy, 1:idle, 2:suspended.
     */
    uint32_t (*host_status)(spi_flash_host_inst_t *host);
    /**
     * Configure the host to work at different read mode. Responsible to compensate the timing and set IO mode.
     */
    esp_err_t (*configure_host_io_mode)(spi_flash_host_inst_t *host, uint32_t command,
                                        uint32_t addr_bitlen, int dummy_bitlen_base,
                                        esp_flash_io_mode_t io_mode);
    /**
     *  Internal use, poll the HW until the last operation is done.
     */
    void (*poll_cmd_done)(spi_flash_host_inst_t *host);
    /**
     * For some host (SPI1), they are shared with a cache. When the data is
     * modified, the cache needs to be flushed. Left NULL if not supported.
     */
    esp_err_t (*flush_cache)(spi_flash_host_inst_t* host, uint32_t addr, uint32_t size);

    /**
     * Suspend check erase/program operation, reserved for ESP32-C3 and ESP32-S3 spi flash ROM IMPL.
     */
    void (*check_suspend)(spi_flash_host_inst_t *host);

    /**
     * Resume flash from suspend manually
     */
    void (*resume)(spi_flash_host_inst_t *host);

    /**
     * Set flash in suspend status manually
     */
    void (*suspend)(spi_flash_host_inst_t *host);

    /**
     * Suspend feature setup for setting cmd and status register mask.
     */
    esp_err_t (*sus_setup)(spi_flash_host_inst_t *host, const spi_flash_sus_cmd_conf *sus_conf);
};

#ifdef __cplusplus
}
#endif
