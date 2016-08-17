// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _ROM_SPI_FLASH_H_
#define _ROM_SPI_FLASH_H_

#include <stdint.h>
#include <stdbool.h>

#include "esp_attr.h"

#include "soc/spi_register.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct{
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

void SelectSpiQIO(uint8_t wp_gpio_num, uint32_t ishspi) ROMFN_ATTR;
void SetSpiDrvs(uint8_t wp_gpio_num, uint32_t ishspi, uint8_t* drvs) ROMFN_ATTR;
void SelectSpiFunction(uint32_t ishspi) ROMFN_ATTR;
SpiFlashOpResult SPIEraseChip(void) ROMFN_ATTR;
SpiFlashOpResult SPIEraseBlock(uint32_t block_num) ROMFN_ATTR;
SpiFlashOpResult SPIEraseSector(uint32_t sector_num) ROMFN_ATTR;
SpiFlashOpResult SPIWrite(uint32_t dest_addr, const uint32_t* src, int32_t len) ROMFN_ATTR;
void SPI_Write_Encrypt_Enable() ROMFN_ATTR;
SpiFlashOpResult SPI_Prepare_Encrypt_Data(uint32_t flash_addr, uint32_t* data) ROMFN_ATTR;
void SPI_Write_Encrypt_Disable() ROMFN_ATTR;
SpiFlashOpResult SPI_Encrypt_Write(uint32_t flash_addr, uint32_t* data, uint32_t len) ROMFN_ATTR;
SpiFlashOpResult SPIRead(uint32_t src_addr, uint32_t* dest, int32_t len) ROMFN_ATTR;
SpiFlashOpResult SPIReadModeCnfig(SpiFlashRdMode mode, bool legacy) ROMFN_ATTR;
SpiFlashOpResult SPIMasterReadModeCnfig(SpiFlashRdMode mode) ROMFN_ATTR;
SpiFlashOpResult SPIClkConfig(uint8_t freqdiv, uint8_t spi) ROMFN_ATTR;
uint16_t SPI_Common_Command(SpiCommonCmd * cmd) ROMFN_ATTR;
SpiFlashOpResult SPIUnlock() ROMFN_ATTR;
SpiFlashOpResult SPIEraseArea(uint32_t start_addr, uint32_t area_len) ROMFN_ATTR;
SpiFlashOpResult SPILock() ROMFN_ATTR;
SpiFlashOpResult SPIParamCfg(uint32_t deviceId, uint32_t chip_size, uint32_t block_size, uint32_t sector_size, uint32_t page_size, uint32_t status_mask) ROMFN_ATTR;
SpiFlashOpResult SPI_user_command_read(uint32_t * status, uint8_t cmd) ROMFN_ATTR;
void spi_cache_sram_init() ROMFN_ATTR;

//ETS_STATUS ets_unpack_flash_code(uint32_t pos, uint32_t *entry_addr, bool jump, bool sb_need_check, bool config) ROMFN_ATTR;
//ETS_STATUS ets_unpack_flash_code_legacy(uint32_t pos, uint32_t *entry_addr, bool jump, bool config) ROMFN_ATTR;

void spi_flash_attach(uint32_t ishspi, bool legacy) ROMFN_ATTR;

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SPI_FLASH_H_ */
