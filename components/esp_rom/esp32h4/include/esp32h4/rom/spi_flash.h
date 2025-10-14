/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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

//TODO: [ESP32H4] IDF-12388 inherit from verification branch, need check

#define PERIPHS_SPI_FLASH_CMD                 SPI_MEM_CMD_REG(1)
#define PERIPHS_SPI_FLASH_ADDR                SPI_MEM_ADDR_REG(1)
#define PERIPHS_SPI_FLASH_CTRL                SPI_MEM_CTRL_REG(1)
#define PERIPHS_SPI_FLASH_CTRL1               SPI_MEM_CTRL1_REG(1)
#define PERIPHS_SPI_FLASH_STATUS              SPI_MEM_RD_STATUS_REG(1)
#define PERIPHS_SPI_FLASH_USRREG              SPI_MEM_USER_REG(1)
#define PERIPHS_SPI_FLASH_USRREG1             SPI_MEM_USER1_REG(1)
#define PERIPHS_SPI_FLASH_USRREG2             SPI_MEM_USER2_REG(1)
#define PERIPHS_SPI_FLASH_C0                  SPI_MEM_W0_REG(1)
#define PERIPHS_SPI_FLASH_C1                  SPI_MEM_W1_REG(1)
#define PERIPHS_SPI_FLASH_C2                  SPI_MEM_W2_REG(1)
#define PERIPHS_SPI_FLASH_C3                  SPI_MEM_W3_REG(1)
#define PERIPHS_SPI_FLASH_C4                  SPI_MEM_W4_REG(1)
#define PERIPHS_SPI_FLASH_C5                  SPI_MEM_W5_REG(1)
#define PERIPHS_SPI_FLASH_C6                  SPI_MEM_W6_REG(1)
#define PERIPHS_SPI_FLASH_C7                  SPI_MEM_W7_REG(1)
#define PERIPHS_SPI_FLASH_TX_CRC              SPI_MEM_TX_CRC_REG(1)

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
  *                        Values usually read from flash by rom code, function usually callde by rom code.
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

typedef struct {
    uint16_t cmd;                /*!< Command value */
    uint16_t cmdBitLen;          /*!< Command byte length*/
    uint32_t *addr;              /*!< Point to address value*/
    uint32_t addrBitLen;         /*!< Address byte length*/
    uint32_t *txData;            /*!< Point to send data buffer*/
    uint32_t txDataBitLen;       /*!< Send data byte length.*/
    uint32_t *rxData;            /*!< Point to receive data buffer*/
    uint32_t rxDataBitLen;       /*!< Receive Data byte length.*/
    uint32_t dummyBitLen;
} esp_rom_spi_cmd_t;

// Definition of MX25UM25645G Octa Flash
// SPI status register
#define ESP_ROM_SPIFLASH_BUSY_FLAG     BIT0
#define ESP_ROM_SPIFLASH_WRENABLE_FLAG BIT1
#define ESP_ROM_SPIFLASH_BP0           BIT2
#define ESP_ROM_SPIFLASH_BP1           BIT3
#define ESP_ROM_SPIFLASH_BP2           BIT4
#define ESP_ROM_SPIFLASH_WR_PROTECT    (ESP_ROM_SPIFLASH_BP0|ESP_ROM_SPIFLASH_BP1|ESP_ROM_SPIFLASH_BP2)
#define ESP_ROM_SPIFLASH_QE            BIT9

#define FLASH_OP_MODE_RDCMD_DOUT       0x3B
#define ESP_ROM_FLASH_SECTOR_SIZE      0x1000
#define ESP_ROM_FLASH_BLOCK_SIZE_64K   0x10000
#define ESP_ROM_FLASH_PAGE_SIZE        256

// FLASH commands
#define ROM_FLASH_CMD_RDID             0x9F
#define ROM_FLASH_CMD_WRSR             0x01
#define ROM_FLASH_CMD_WRSR2            0x31 /* Not all SPI flash uses this command */
#define ROM_FLASH_CMD_WREN             0x06
#define ROM_FLASH_CMD_WRDI             0x04
#define ROM_FLASH_CMD_RDSR             0x05
#define ROM_FLASH_CMD_RDSR2            0x35 /* Not all SPI flash uses this command */
#define ROM_FLASH_CMD_ERASE_SEC        0x20
#define ROM_FLASH_CMD_ERASE_BLK_32K    0x52
#define ROM_FLASH_CMD_ERASE_BLK_64K    0xD8
#define ROM_FLASH_CMD_OTPEN            0x3A /* Enable OTP mode, not all SPI flash uses this command */
#define ROM_FLASH_CMD_RSTEN            0x66
#define ROM_FLASH_CMD_RST              0x99

#define ROM_FLASH_CMD_SE4B             0x21
#define ROM_FLASH_CMD_SE4B_OCT         0xDE21
#define ROM_FLASH_CMD_BE4B             0xDC
#define ROM_FLASH_CMD_BE4B_OCT         0x23DC
#define ROM_FLASH_CMD_RSTEN_OCT        0x9966
#define ROM_FLASH_CMD_RST_OCT          0x6699

#define ROM_FLASH_CMD_FSTRD4B_STR      0x13EC
#define ROM_FLASH_CMD_FSTRD4B_DTR      0x11EE
#define ROM_FLASH_CMD_FSTRD4B          0x0C
#define ROM_FLASH_CMD_PP4B             0x12
#define ROM_FLASH_CMD_PP4B_OCT         0xED12

#define ROM_FLASH_CMD_RDID_OCT         0x609F
#define ROM_FLASH_CMD_WREN_OCT         0xF906
#define ROM_FLASH_CMD_RDSR_OCT         0xFA05
#define ROM_FLASH_CMD_RDCR2            0x71
#define ROM_FLASH_CMD_RDCR2_OCT        0x8E71
#define ROM_FLASH_CMD_WRCR2            0x72
#define ROM_FLASH_CMD_WRCR2_OCT        0x8D72

// Definitions for GigaDevice GD25LX256E Flash
#define ROM_FLASH_CMD_RDFSR_GD            0x70
#define ROM_FLASH_CMD_RD_GD               0x03
#define ROM_FLASH_CMD_RD4B_GD             0x13
#define ROM_FLASH_CMD_FSTRD_GD            0x0B
#define ROM_FLASH_CMD_FSTRD4B_GD          0x0C
#define ROM_FLASH_CMD_FSTRD_OOUT_GD       0x8B
#define ROM_FLASH_CMD_FSTRD4B_OOUT_GD     0x7C
#define ROM_FLASH_CMD_FSTRD_OIOSTR_GD     0xCB
#define ROM_FLASH_CMD_FSTRD4B_OIOSTR_GD   0xCC
#define ROM_FLASH_CMD_FSTRD4B_OIODTR_GD   0xFD

#define ROM_FLASH_CMD_PP_GD               0x02
#define ROM_FLASH_CMD_PP4B_GD             0x12
#define ROM_FLASH_CMD_PP_OOUT_GD          0x82
#define ROM_FLASH_CMD_PP4B_OOUT_GD        0x84
#define ROM_FLASH_CMD_PP_OIO_GD           0xC2
#define ROM_FLASH_CMD_PP4B_OIOSTR_GD      0x8E

#define ROM_FLASH_CMD_SE_GD               0x20
#define ROM_FLASH_CMD_SE4B_GD             0x21
#define ROM_FLASH_CMD_BE32K_GD            0x52
#define ROM_FLASH_CMD_BE32K4B_GD          0x5C
#define ROM_FLASH_CMD_BE64K_GD            0xD8
#define ROM_FLASH_CMD_BE64K4B_GD          0xDC

#define ROM_FLASH_CMD_EN4B_GD             0xB7
#define ROM_FLASH_CMD_DIS4B_GD            0xE9

// spi user mode command config
/**
 * @brief Config the spi user command
 * @param spi_num spi port
 * @param pcmd pointer to accept the spi command struct
 */
void esp_rom_spi_cmd_config(int spi_num, esp_rom_spi_cmd_t* pcmd);

/**
 * @brief Start a spi user command sequence
 * @param spi_num spi port
 * @param rx_buf buffer pointer to receive data
 * @param rx_len receive data length in byte
 * @param cs_en_mask decide which cs to use, 0 for cs0, 1 for cs1
 * @param is_write_erase to indicate whether this is a write or erase operation, since the CPU would check permission
 */
void esp_rom_spi_cmd_start(int spi_num, uint8_t* rx_buf, uint16_t rx_len, uint8_t cs_en_mask, bool is_write_erase);

// set SPI read/write mode
/**
 * @brief Set SPI operation mode
 * @param spi_num spi port
 * @param mode Flash Read Mode
 */
void esp_rom_spi_set_op_mode(int spi_num, esp_rom_spiflash_read_mode_t mode);

#ifdef __cplusplus
}
#endif
