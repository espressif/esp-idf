// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROM_SPI_FLASH_H_
#define _ROM_SPI_FLASH_H_

#include <stdint.h>
#include <stdbool.h>

#include "esp_attr.h"

#include "soc/spi_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup spi_flash_apis, spi flash operation related apis
  * @brief spi_flash apis
  */

/** @addtogroup spi_flash_apis
  * @{
  */

/*************************************************************
 *                            Note
 *************************************************************
 * 1. ESP32 chip have 4 SPI slave/master, however, SPI0 is
 *    used as an SPI master to access Flash and ext-SRAM by
 *    Cache module. It will support Decryto read for Flash,
 *    read/write for ext-SRAM. And SPI1 is also used as an
 *    SPI master for Flash read/write and ext-SRAM read/write.
 *    It will support Encrypto write for Flash.
 * 2. As an SPI master, SPI support Highest clock to 80M,
 *    however, Flash with 80M Clock should be configured
 *    for different Flash chips. If you want to use 80M
 *    clock We should use the SPI that is certified by
 *    Espressif. However, the certification is not started
 *    at the time, so please use 40M clock at the moment.
 * 3. SPI Flash can use 2 lines or 4 lines mode. If you
 *    use 2 lines mode, you can save two pad SPIHD and
 *    SPIWP for gpio. ESP32 support configured SPI pad for
 *    Flash, the configuration is stored in efuse and flash.
 *    However, the configurations of pads should be certified
 *    by Espressif. If you use this function, please use 40M
 *    clock at the moment.
 * 4. ESP32 support to use Common SPI command to configure
 *    Flash to QIO mode, if you failed to configure with fix
 *    command. With Common SPI Command, ESP32 can also provide
 *    a way to use same Common SPI command groups on different
 *    Flash chips.
 * 5. This functions are not protected by packeting, Please use the
 *************************************************************
 */

#define PERIPHS_SPI_FLASH_CMD               SPI_CMD(1)
#define PERIPHS_SPI_FLASH_ADDR              SPI_ADDR(1)
#define PERIPHS_SPI_FLASH_CTRL              SPI_CTRL(1)
#define PERIPHS_SPI_FLASH_CTRL1             SPI_CTRL1(1)
#define PERIPHS_SPI_FLASH_STATUS            SPI_RD_STATUS(1)
#define PERIPHS_SPI_FLASH_USRREG            SPI_USER(1)
#define PERIPHS_SPI_FLASH_USRREG1           SPI_USER1(1)
#define PERIPHS_SPI_FLASH_USRREG2           SPI_USER2(1)
#define PERIPHS_SPI_FLASH_C0                SPI_W0(1)
#define PERIPHS_SPI_FLASH_C1                SPI_W1(1)
#define PERIPHS_SPI_FLASH_C2                SPI_W2(1)
#define PERIPHS_SPI_FLASH_C3                SPI_W3(1)
#define PERIPHS_SPI_FLASH_C4                SPI_W4(1)
#define PERIPHS_SPI_FLASH_C5                SPI_W5(1)
#define PERIPHS_SPI_FLASH_C6                SPI_W6(1)
#define PERIPHS_SPI_FLASH_C7                SPI_W7(1)
#define PERIPHS_SPI_FLASH_TX_CRC            SPI_TX_CRC(1)

#define SPI0_R_QIO_DUMMY_CYCLELEN           3
#define SPI0_R_QIO_ADDR_BITSLEN             31
#define SPI0_R_FAST_DUMMY_CYCLELEN          7
#define SPI0_R_DIO_DUMMY_CYCLELEN           3
#define SPI0_R_FAST_ADDR_BITSLEN            23
#define SPI0_R_SIO_ADDR_BITSLEN             23

#define SPI1_R_QIO_DUMMY_CYCLELEN           3
#define SPI1_R_QIO_ADDR_BITSLEN             31
#define SPI1_R_FAST_DUMMY_CYCLELEN          7
#define SPI1_R_DIO_DUMMY_CYCLELEN           3
#define SPI1_R_DIO_ADDR_BITSLEN             31
#define SPI1_R_FAST_ADDR_BITSLEN            23
#define SPI1_R_SIO_ADDR_BITSLEN             23

#define SPI_W_SIO_ADDR_BITSLEN              23

#define TWO_BYTE_STATUS_EN                  SPI_WRSR_2B

//SPI address register
#define SPI_FLASH_BYTES_LEN                 24
#define SPI_BUFF_BYTE_WRITE_NUM             32
#define SPI_BUFF_BYTE_READ_NUM              64
#define SPI_BUFF_BYTE_READ_BITS             0x3f

//SPI status register
#define  SPI_FLASH_BUSY_FLAG                BIT0
#define  SPI_FLASH_WRENABLE_FLAG            BIT1
#define  SPI_FLASH_BP0                      BIT2
#define  SPI_FLASH_BP1                      BIT3
#define  SPI_FLASH_BP2                      BIT4
#define  FLASH_WR_PROTECT                   (SPI_FLASH_BP0|SPI_FLASH_BP1|SPI_FLASH_BP2)
#define  SPI_FLASH_QE                       BIT9

typedef enum {
    SPI_FLASH_QIO_MODE = 0,
    SPI_FLASH_QOUT_MODE,
    SPI_FLASH_DIO_MODE,
    SPI_FLASH_DOUT_MODE,
    SPI_FLASH_FASTRD_MODE,
    SPI_FLASH_SLOWRD_MODE
} SpiFlashRdMode;

typedef enum {
    SPI_FLASH_RESULT_OK,
    SPI_FLASH_RESULT_ERR,
    SPI_FLASH_RESULT_TIMEOUT
} SpiFlashOpResult;

typedef struct {
    uint32_t deviceId;
    uint32_t chip_size;    // chip size in bytes
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
    uint32_t status_mask;
} SpiFlashChip;

typedef struct {
    uint8_t  data_length;
    uint8_t  read_cmd0;
    uint8_t  read_cmd1;
    uint8_t  write_cmd;
    uint16_t data_mask;
    uint16_t data;
} SpiCommonCmd;

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
void spi_dummy_len_fix(uint8_t spi, uint8_t freqdiv);

/**
  * @brief Select SPI Flash to QIO mode when WP pad is read from Flash.
  *    Please do not call this function in SDK.
  *
  * @param  uint8_t wp_gpio_num: WP gpio number.
  *
  * @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
  *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
  *
  * @return None
  */
void SelectSpiQIO(uint8_t wp_gpio_num, uint32_t ishspi);

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
  *                        Values usually read from falsh by rom code, function usually callde by rom code.
  *                        if value with bit(3) set, the value is valid, bit[2:0] is the real value.
  *
  * @return None
  */
void SetSpiDrvs(uint8_t wp_gpio_num, uint32_t ishspi, uint8_t *drvs);

/**
  * @brief Select SPI Flash function for pads.
  *    Please do not call this function in SDK.
  *
  * @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
  *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
  *
  * @return None
  */
void SelectSpiFunction(uint32_t ishspi);

/**
  * @brief SPI Flash init, clock divisor is 4, use 1 line Slow read mode.
  *    Please do not call this function in SDK.
  *
  * @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
  *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
  *
  * @param  uint8_t legacy: In legacy mode, more SPI command is used in line.
  *
  * @return None
  */
void spi_flash_attach(uint32_t ishspi, bool legacy);

/**
  * @brief SPI Read Flash status register. We use CMD 0x05 (RDSR).
  *    Please do not call this function in SDK.
  *
  * @param  SpiFlashChip *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t *status : The pointer to which to return the Flash status value.
  *
  * @return SPI_FLASH_RESULT_OK : read OK.
  *         SPI_FLASH_RESULT_ERR : read error.
  *         SPI_FLASH_RESULT_TIMEOUT : read timeout.
  */
SpiFlashOpResult SPI_read_status(SpiFlashChip *spi, uint32_t *status);

/**
  * @brief SPI Read Flash status register bits 8-15. We use CMD 0x35 (RDSR2).
  *        Please do not call this function in SDK.
  *
  * @param  SpiFlashChip *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t *status : The pointer to which to return the Flash status value.
  *
  * @return SPI_FLASH_RESULT_OK : read OK.
  *         SPI_FLASH_RESULT_ERR : read error.
  *         SPI_FLASH_RESULT_TIMEOUT : read timeout.
  */
SpiFlashOpResult SPI_read_status_high(uint32_t *status);

/**
  * @brief Write status to Falsh status register.
  *        Please do not call this function in SDK.
  *
  * @param  SpiFlashChip *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t status_value : Value to .
  *
  * @return SPI_FLASH_RESULT_OK : write OK.
  *         SPI_FLASH_RESULT_ERR : write error.
  *         SPI_FLASH_RESULT_TIMEOUT : write timeout.
  */
SpiFlashOpResult SPI_write_status(SpiFlashChip *spi, uint32_t status_value);

/**
  * @brief Use a command to Read Flash status register.
  *        Please do not call this function in SDK.
  *
  * @param  SpiFlashChip *spi : The information for Flash, which is exported from ld file.
  *
  * @param  uint32_t*status : The pointer to which to return the Flash status value.
  *
  * @return SPI_FLASH_RESULT_OK : read OK.
  *         SPI_FLASH_RESULT_ERR : read error.
  *         SPI_FLASH_RESULT_TIMEOUT : read timeout.
  */
SpiFlashOpResult SPI_user_command_read(uint32_t *status, uint8_t cmd);

/**
  * @brief Config SPI Flash read mode when init.
  *        Please do not call this function in SDK.
  *
  * @param  SpiFlashRdMode mode : QIO/QOUT/DIO/DOUT/FastRD/SlowRD.
  *
  * @param  uint8_t legacy: In legacy mode, more SPI command is used in line.
  *
  * @return SPI_FLASH_RESULT_OK : config OK.
  *         SPI_FLASH_RESULT_ERR : config error.
  *         SPI_FLASH_RESULT_TIMEOUT : config timeout.
  */
SpiFlashOpResult SPIReadModeCnfig(SpiFlashRdMode mode, bool legacy);

/**
  * @brief Config SPI Flash read mode when Flash is running in some mode.
  *        Please do not call this function in SDK.
  *
  * @param  SpiFlashRdMode mode : QIO/QOUT/DIO/DOUT/FastRD/SlowRD.
  *
  * @return SPI_FLASH_RESULT_OK : config OK.
  *         SPI_FLASH_RESULT_ERR : config error.
  *         SPI_FLASH_RESULT_TIMEOUT : config timeout.
  */
SpiFlashOpResult SPIMasterReadModeCnfig(SpiFlashRdMode mode);

/**
  * @brief Config SPI Flash clock divisor.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t freqdiv: clock divisor.
  *
  * @param  uint8_t spi: 0 for SPI0, 1 for SPI1.
  *
  * @return SPI_FLASH_RESULT_OK : config OK.
  *         SPI_FLASH_RESULT_ERR : config error.
  *         SPI_FLASH_RESULT_TIMEOUT : config timeout.
  */
SpiFlashOpResult SPIClkConfig(uint8_t freqdiv, uint8_t spi);

/**
  * @brief Send CommonCmd to Flash so that is can go into QIO mode, some Flash use different CMD.
  *        Please do not call this function in SDK.
  *
  * @param  SpiCommonCmd *cmd : A struct to show the action of a command.
  *
  * @return uint16_t  0 : do not send command any more.
  *                   1 : go to the next command.
  *                   n > 1 : skip (n - 1) commands.
  */
uint16_t SPI_Common_Command(SpiCommonCmd *cmd);

/**
  * @brief Unlock SPI write protect.
  *        Please do not call this function in SDK.
  *
  * @param  None.
  *
  * @return SPI_FLASH_RESULT_OK : Unlock OK.
  *         SPI_FLASH_RESULT_ERR : Unlock error.
  *         SPI_FLASH_RESULT_TIMEOUT : Unlock timeout.
  */
SpiFlashOpResult SPIUnlock(void);

/**
  * @brief SPI write protect.
  *        Please do not call this function in SDK.
  *
  * @param  None.
  *
  * @return SPI_FLASH_RESULT_OK : Lock OK.
  *         SPI_FLASH_RESULT_ERR : Lock error.
  *         SPI_FLASH_RESULT_TIMEOUT : Lock timeout.
  */
SpiFlashOpResult SPILock(void);

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
  * @return SPI_FLASH_RESULT_OK : Update OK.
  *         SPI_FLASH_RESULT_ERR : Update error.
  *         SPI_FLASH_RESULT_TIMEOUT : Update timeout.
  */
SpiFlashOpResult SPIParamCfg(uint32_t deviceId, uint32_t chip_size, uint32_t block_size, uint32_t sector_size, uint32_t page_size, uint32_t status_mask);

/**
  * @brief Erase whole flash chip.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return SPI_FLASH_RESULT_OK : Erase OK.
  *         SPI_FLASH_RESULT_ERR : Erase error.
  *         SPI_FLASH_RESULT_TIMEOUT : Erase timeout.
  */
SpiFlashOpResult SPIEraseChip(void);

/**
  * @brief Erase a 64KB block of flash
  *        Uses SPI flash command D8H.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t block_num : Which block to erase.
  *
  * @return SPI_FLASH_RESULT_OK : Erase OK.
  *         SPI_FLASH_RESULT_ERR : Erase error.
  *         SPI_FLASH_RESULT_TIMEOUT : Erase timeout.
  */
SpiFlashOpResult SPIEraseBlock(uint32_t block_num);

/**
  * @brief Erase a sector of flash.
  *        Uses SPI flash command 20H.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t sector_num : Which sector to erase.
  *
  * @return SPI_FLASH_RESULT_OK : Erase OK.
  *         SPI_FLASH_RESULT_ERR : Erase error.
  *         SPI_FLASH_RESULT_TIMEOUT : Erase timeout.
  */
SpiFlashOpResult SPIEraseSector(uint32_t sector_num);

/**
  * @brief Erase some sectors.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t start_addr : Start addr to erase, should be sector aligned.
  *
  * @param  uint32_t area_len : Length to erase, should be sector aligned.
  *
  * @return SPI_FLASH_RESULT_OK : Erase OK.
  *         SPI_FLASH_RESULT_ERR : Erase error.
  *         SPI_FLASH_RESULT_TIMEOUT : Erase timeout.
  */
SpiFlashOpResult SPIEraseArea(uint32_t start_addr, uint32_t area_len);

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
  * @return SPI_FLASH_RESULT_OK : Write OK.
  *         SPI_FLASH_RESULT_ERR : Write error.
  *         SPI_FLASH_RESULT_TIMEOUT : Write timeout.
  */
SpiFlashOpResult SPIWrite(uint32_t dest_addr, const uint32_t *src, int32_t len);

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
  * @return SPI_FLASH_RESULT_OK : Read OK.
  *         SPI_FLASH_RESULT_ERR : Read error.
  *         SPI_FLASH_RESULT_TIMEOUT : Read timeout.
  */
SpiFlashOpResult SPIRead(uint32_t src_addr, uint32_t *dest, int32_t len);

/**
  * @brief SPI1 go into encrypto mode.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return None
  */
void SPI_Write_Encrypt_Enable(void);

/**
  * @brief Prepare 32 Bytes data to encrpto writing, you should Erase it yourself if need.
  *        Please do not call this function in SDK.
  *
  * @param  uint32_t flash_addr : Address to write, should be 32 bytes aligned.
  *
  * @param  uint32_t *data : The pointer to data which is to write.
  *
  * @return SPI_FLASH_RESULT_OK : Prepare OK.
  *         SPI_FLASH_RESULT_ERR : Prepare error.
  *         SPI_FLASH_RESULT_TIMEOUT : Prepare timeout.
  */
SpiFlashOpResult SPI_Prepare_Encrypt_Data(uint32_t flash_addr, uint32_t *data);

/**
  * @brief SPI1 go out of encrypto mode.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return None
  */
void SPI_Write_Encrypt_Disable(void);

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
  * @return SPI_FLASH_RESULT_OK : Data written successfully.
  *         SPI_FLASH_RESULT_ERR : Encryption write error.
  *         SPI_FLASH_RESULT_TIMEOUT : Encrypto write timeout.
  */
SpiFlashOpResult SPI_Encrypt_Write(uint32_t flash_addr, uint32_t *data, uint32_t len);


/** @brief Wait until SPI flash write operation is complete
 *
 * @note Please do not call this function in SDK.
 *
 * Reads the Write In Progress bit of the SPI flash status register,
 * repeats until this bit is zero (indicating write complete).
 *
 * @return SPI_FLASH_RESULT_OK : Write is complete
 *         SPI_FLASH_RESULT_ERR : Error while reading status.
 */
SpiFlashOpResult SPI_Wait_Idle(SpiFlashChip *spi);


/** @brief Global SpiFlashChip structure used by ROM functions
 *
 */
extern SpiFlashChip g_rom_flashchip;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SPI_FLASH_H_ */
