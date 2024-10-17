/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "esp_rom_spiflash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PERIPHS_SPI_FLASH_CMD                 SPI1_MEM_C_CMD_REG
#define PERIPHS_SPI_FLASH_ADDR                SPI1_MEM_C_ADDR_REG
#define PERIPHS_SPI_FLASH_CTRL                SPI1_MEM_C_CTRL_REG
#define PERIPHS_SPI_FLASH_CTRL1               SPI1_MEM_C_CTRL1_REG
#define PERIPHS_SPI_FLASH_STATUS              SPI1_MEM_C_RD_STATUS_REG
#define PERIPHS_SPI_FLASH_USRREG              SPI1_MEM_C_USER_REG
#define PERIPHS_SPI_FLASH_USRREG1             SPI1_MEM_C_USER1_REG
#define PERIPHS_SPI_FLASH_USRREG2             SPI1_MEM_C_USER2_REG
#define PERIPHS_SPI_FLASH_C0                  SPI1_MEM_C_W0_REG
#define PERIPHS_SPI_FLASH_C1                  SPI1_MEM_C_W1_REG
#define PERIPHS_SPI_FLASH_C2                  SPI1_MEM_C_W2_REG
#define PERIPHS_SPI_FLASH_C3                  SPI1_MEM_C_W3_REG
#define PERIPHS_SPI_FLASH_C4                  SPI1_MEM_C_W4_REG
#define PERIPHS_SPI_FLASH_C5                  SPI1_MEM_C_W5_REG
#define PERIPHS_SPI_FLASH_C6                  SPI1_MEM_C_W6_REG
#define PERIPHS_SPI_FLASH_C7                  SPI1_MEM_C_W7_REG
#define PERIPHS_SPI_FLASH_TX_CRC              SPI1_MEM_C_TX_CRC_REG

#define SPI_MEM_FREAD_QIO  SPI1_MEM_C_FREAD_QIO
#define SPI_MEM_FREAD_DIO  SPI1_MEM_C_FREAD_DIO
#define SPI_MEM_FREAD_QUAD  SPI1_MEM_C_FREAD_QUAD
#define SPI_MEM_FREAD_DUAL  SPI1_MEM_C_FREAD_DUAL
#define SPI_MEM_FWRITE_QIO  SPI1_MEM_C_FWRITE_QIO
#define SPI_MEM_FWRITE_DIO  SPI1_MEM_C_FWRITE_DIO
#define SPI_MEM_FWRITE_QUAD  SPI1_MEM_C_FWRITE_QUAD
#define SPI_MEM_FWRITE_DUAL  SPI1_MEM_C_FWRITE_DUAL
#define SPI_MEM_FASTRD_MODE  SPI1_MEM_C_FASTRD_MODE


#define SPI0_R_QIO_DUMMY_CYCLELEN             5
#define SPI0_R_QIO_ADDR_BITSLEN               23
#define SPI0_R_FAST_DUMMY_CYCLELEN            7
#define SPI0_R_DIO_DUMMY_CYCLELEN             3
#define SPI0_R_FAST_ADDR_BITSLEN              23
#define SPI0_R_SIO_ADDR_BITSLEN               23

#define SPI1_R_QIO_DUMMY_CYCLELEN             5
#define SPI1_R_QIO_ADDR_BITSLEN               23
#define SPI1_R_FAST_DUMMY_CYCLELEN            7
#define SPI1_R_DIO_DUMMY_CYCLELEN             3
#define SPI1_R_DIO_ADDR_BITSLEN               23
#define SPI1_R_FAST_ADDR_BITSLEN              23
#define SPI1_R_SIO_ADDR_BITSLEN               23

#define ESP_ROM_SPIFLASH_W_SIO_ADDR_BITSLEN   23

#define ESP_ROM_SPIFLASH_TWO_BYTE_STATUS_EN   SPI_MEM_WRSR_2B

//SPI address register
#define ESP_ROM_SPIFLASH_BYTES_LEN            24
#define ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM  32
#define ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM   16
#define ESP_ROM_SPIFLASH_BUFF_BYTE_READ_BITS  0xf

typedef void (* spi_flash_func_t)(void);
typedef esp_rom_spiflash_result_t (* spi_flash_op_t)(void);
typedef esp_rom_spiflash_result_t (* spi_flash_erase_t)(uint32_t);
typedef esp_rom_spiflash_result_t (* spi_flash_rd_t)(uint32_t, uint32_t*, int);
typedef esp_rom_spiflash_result_t (* spi_flash_wr_t)(uint32_t, const uint32_t*, int);
typedef esp_rom_spiflash_result_t (* spi_flash_ewr_t)(uint32_t, const void*, uint32_t);
typedef esp_rom_spiflash_result_t (* spi_flash_wren_t)(void*);
typedef esp_rom_spiflash_result_t (* spi_flash_erase_area_t)(uint32_t, uint32_t);

typedef struct {
    uint8_t pp_addr_bit_len;
    uint8_t se_addr_bit_len;
    uint8_t be_addr_bit_len;
    uint8_t rd_addr_bit_len;
    uint32_t read_sub_len;
    uint32_t write_sub_len;
    spi_flash_op_t unlock;
    spi_flash_erase_t erase_sector;
    spi_flash_erase_t erase_block;
    spi_flash_rd_t read;
    spi_flash_wr_t write;
    spi_flash_ewr_t encrypt_write;
    spi_flash_func_t check_sus;
    spi_flash_wren_t wren;
    spi_flash_op_t wait_idle;
    spi_flash_erase_area_t erase_area;
} spiflash_legacy_funcs_t;

typedef struct {
    uint8_t  data_length;
    uint8_t  read_cmd0;
    uint8_t  read_cmd1;
    uint8_t  write_cmd;
    uint16_t data_mask;
    uint16_t data;
} esp_rom_spiflash_common_cmd_t;

/**
  * @brief SPI Flash init, clock divisor is 4, use 1 line Slow read mode.
  *    Please do not call this function in SDK.
  *
  * @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
  *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
  *
  * @param  uint8_t legacy: always keeping false.
  *
  * @return None
  */
void esp_rom_spiflash_attach(uint32_t ishspi, bool legacy);

/**
  * @brief SPI Read Flash status register. We use CMD 0x05 (RDSR).
  *    Please do not call this function in SDK.
  *
  * @param  esp_rom_spiflash_chip_t *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t *status : The pointer to which to return the Flash status value.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : read OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : read error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : read timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_read_status(esp_rom_spiflash_chip_t *spi, uint32_t *status);

/**
  * @brief SPI Read Flash status register bits 8-15. We use CMD 0x35 (RDSR2).
  *        Please do not call this function in SDK.
  *
  * @param  esp_rom_spiflash_chip_t *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t *status : The pointer to which to return the Flash status value.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : read OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : read error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : read timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_read_statushigh(esp_rom_spiflash_chip_t *spi, uint32_t *status);

/**
  * @brief Write status to Flash status register.
  *        Please do not call this function in SDK.
  *
  * @param  esp_rom_spiflash_chip_t *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t status_value : Value to .
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : write OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : write error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : write timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_write_status(esp_rom_spiflash_chip_t *spi, uint32_t status_value);

/**
  * @brief Use a command to Read Flash status register.
  *        Please do not call this function in SDK.
  *
  * @param  esp_rom_spiflash_chip_t *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t*status : The pointer to which to return the Flash status value.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : read OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : read error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : read timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_read_user_cmd(uint32_t *status, uint8_t cmd);

/**
  * @brief Config SPI Flash read mode when init.
  *        Please do not call this function in SDK.
  *
  * @param  esp_rom_spiflash_read_mode_t mode : QIO/QOUT/DIO/DOUT/FastRD/SlowRD.
  *
  * This function does not try to set the QIO Enable bit in the status register, caller is responsible for this.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : config OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : config error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : config timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_config_readmode(esp_rom_spiflash_read_mode_t mode);

/**
  * @brief Config SPI Flash clock divisor.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t freqdiv: clock divisor.
  *
  * @param  uint8_t spi: 0 for SPI0, 1 for SPI1.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : config OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : config error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : config timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_config_clk(uint8_t freqdiv, uint8_t spi);

/**
  * @brief Clear all SR bits except QE bit.
  *        Please do not call this function in SDK.
  *
  * @param  None.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Unlock OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Unlock error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Unlock timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_clear_bp(void);

/**
  * @brief Clear all SR bits except QE bit.
  *        Please do not call this function in SDK.
  *
  * @param  None.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Unlock OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Unlock error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Unlock timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_unlock(void);

/**
  * @brief Update SPI Flash parameter.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t deviceId : Device ID read from SPI, the low 32 bit.
  *
  * @param  uint32_t chip_size : The Flash size.
  *
  * @param  uint32_t block_size : The Flash block size.
  *
  * @param  uint32_t sector_size : The Flash sector size.
  *
  * @param  uint32_t page_size : The Flash page size.
  *
  * @param  uint32_t status_mask : The Mask used when read status from Flash(use single CMD).
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Update OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Update error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Update timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_config_param(uint32_t deviceId, uint32_t chip_size, uint32_t block_size,
                                                        uint32_t sector_size, uint32_t page_size, uint32_t status_mask);

/**
  * @brief Erase whole flash chip.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Erase OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Erase error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Erase timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_erase_chip(void);

/**
  * @brief Erase a 64KB block of flash
  *        Uses SPI flash command D8H.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t block_num : Which block to erase.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Erase OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Erase error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Erase timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_erase_block(uint32_t block_num);

/**
  * @brief Erase a sector of flash.
  *        Uses SPI flash command 20H.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t sector_num : Which sector to erase.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Erase OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Erase error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Erase timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_erase_sector(uint32_t sector_num);

/**
  * @brief Erase some sectors.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t start_addr : Start addr to erase, should be sector aligned.
  *
  * @param  uint32_t area_len : Length to erase, should be sector aligned.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Erase OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Erase error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Erase timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_erase_area(uint32_t start_addr, uint32_t area_len);

/**
  * @brief Write Data to Flash, you should Erase it yourself if need.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t dest_addr : Address to write, should be 4 bytes aligned.
  *
  * @param  const uint32_t *src : The pointer to data which is to write.
  *
  * @param  uint32_t len : Length to write, should be 4 bytes aligned.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Write OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Write error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Write timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_write(uint32_t dest_addr, const uint32_t *src, int32_t len);

/**
  * @brief Read Data from Flash, you should Erase it yourself if need.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t src_addr : Address to read, should be 4 bytes aligned.
  *
  * @param  uint32_t *dest : The buf to read the data.
  *
  * @param  uint32_t len : Length to read, should be 4 bytes aligned.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Read OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Read error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Read timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_read(uint32_t src_addr, uint32_t *dest, int32_t len);

/**
  * @brief SPI1 go into encrypto mode.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return None
  */
void esp_rom_spiflash_write_encrypted_enable(void);

/**
  * @brief Prepare 32 Bytes data to encrpto writing, you should Erase it yourself if need.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t flash_addr : Address to write, should be 32 bytes aligned.
  *
  * @param  uint32_t *data : The pointer to data which is to write.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Prepare OK.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Prepare error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Prepare timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_prepare_encrypted_data(uint32_t flash_addr, uint32_t *data);

/**
  * @brief SPI1 go out of encrypto mode.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return None
  */
void esp_rom_spiflash_write_encrypted_disable(void);

/**
  * @brief Write data to flash with transparent encryption.
  * @note Sectors to be written should already be erased.
  *
  * @note Please do not call this function in SDK.
  *
  * @param  uint32_t flash_addr : Address to write, should be 32 byte aligned.
  *
  * @param  uint32_t *data : The pointer to data to write. Note, this pointer must
  *                          be 32 bit aligned and the content of the data will be
  *                          modified by the encryption function.
  *
  * @param  uint32_t len : Length to write, should be 32 bytes aligned.
  *
  * @return ESP_ROM_SPIFLASH_RESULT_OK : Data written successfully.
  *         ESP_ROM_SPIFLASH_RESULT_ERR : Encryption write error.
  *         ESP_ROM_SPIFLASH_RESULT_TIMEOUT : Encrypto write timeout.
  */
esp_rom_spiflash_result_t esp_rom_spiflash_write_encrypted(uint32_t flash_addr, uint32_t *data, uint32_t len);


/** @brief Wait until SPI flash write operation is complete
 *
 * @note Please do not call this function in SDK.
 *
 * Reads the Write In Progress bit of the SPI flash status register,
 * repeats until this bit is zero (indicating write complete).
 *
 * @return ESP_ROM_SPIFLASH_RESULT_OK : Write is complete
 *         ESP_ROM_SPIFLASH_RESULT_ERR : Error while reading status.
 */
esp_rom_spiflash_result_t esp_rom_spiflash_wait_idle(esp_rom_spiflash_chip_t *spi);


/** @brief Enable Quad I/O pin functions
 *
 * @note Please do not call this function in SDK.
 *
 * Sets the HD & WP pin functions for Quad I/O modes, based on the
 * efuse SPI pin configuration.
 *
 * @param wp_gpio_num - Number of the WP pin to reconfigure for quad I/O.
 *
 * @param spiconfig - Pin configuration, as returned from ets_efuse_get_spiconfig().
 * - If this parameter is 0, default SPI pins are used and wp_gpio_num parameter is ignored.
 * - If this parameter is 1, default HSPI pins are used and wp_gpio_num parameter is ignored.
 * - For other values, this parameter encodes the HD pin number and also the CLK pin number. CLK pin selection is used
 *   to determine if HSPI or SPI peripheral will be used (use HSPI if CLK pin is the HSPI clock pin, otherwise use SPI).
 *   Both HD & WP pins are configured via GPIO matrix to map to the selected peripheral.
 */
void esp_rom_spiflash_select_qio_pins(uint8_t wp_gpio_num, uint32_t spiconfig);

/**
 * @brief Clear WEL bit unconditionally.
 *
 * @return always ESP_ROM_SPIFLASH_RESULT_OK
 */
esp_rom_spiflash_result_t esp_rom_spiflash_write_disable(void);

/**
 * @brief Set WREN bit.
 *
 * @param  esp_rom_spiflash_chip_t *spi : The information for Flash, which is exported from ld file.
 *
 * @return always ESP_ROM_SPIFLASH_RESULT_OK
 */
esp_rom_spiflash_result_t esp_rom_spiflash_write_enable(esp_rom_spiflash_chip_t *spi);

/**
  * @brief Fix the bug in SPI hardware communication with Flash/Ext-SRAM in High Speed.
  *    Please do not call this function in SDK.
  *
  * @param  uint8_t spi: 0 for SPI0(Cache Access), 1 for SPI1(Flash read/write).
  *
  * @param  uint8_t freqdiv: Pll is 80M, 4 for 20M, 3 for 26.7M, 2 for 40M, 1 for 80M.
  *
  * @return None
  */
void esp_rom_spiflash_fix_dummylen(uint8_t spi, uint8_t freqdiv);

/**
  * @brief Set SPI Flash pad drivers.
  *    Please do not call this function in SDK.
  *
  * @param  uint8_t wp_gpio_num: WP gpio number.
  *
  * @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
  *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
  *
  * @param  uint8_t *drvs: drvs[0]-bit[3:0] for cpiclk, bit[7:4] for spiq, drvs[1]-bit[3:0] for spid, drvs[1]-bit[7:4] for spid
  *            drvs[2]-bit[3:0] for spihd, drvs[2]-bit[7:4] for spiwp.
  *                        Values usually read from flash by rom code, function usually called by rom code.
  *                        if value with bit(3) set, the value is valid, bit[2:0] is the real value.
  *
  * @return None
  */
void esp_rom_spiflash_set_drvs(uint8_t wp_gpio_num, uint32_t ishspi, uint8_t *drvs);

/**
  * @brief Select SPI Flash function for pads.
  *    Please do not call this function in SDK.
  *
  * @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
  *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
  *
  * @return None
  */
void esp_rom_spiflash_select_padsfunc(uint32_t ishspi);

/**
  * @brief Send CommonCmd to Flash so that is can go into QIO mode, some Flash use different CMD.
  *        Please do not call this function in SDK.
  *
  * @param  esp_rom_spiflash_common_cmd_t *cmd : A struct to show the action of a command.
  *
  * @return uint16_t  0 : do not send command any more.
  *                   1 : go to the next command.
  *                   n > 1 : skip (n - 1) commands.
  */
uint16_t esp_rom_spiflash_common_cmd(esp_rom_spiflash_common_cmd_t *cmd);

extern const spiflash_legacy_funcs_t *rom_spiflash_legacy_funcs;

#ifdef __cplusplus
}
#endif
