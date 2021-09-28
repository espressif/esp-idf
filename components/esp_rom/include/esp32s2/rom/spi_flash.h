/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_SPI_FLASH_H_
#define _ROM_SPI_FLASH_H_

#ifndef CONFIG_IDF_TARGET_ESP32S2
#error This file should only be included for ESP32-S2 target
#endif

#include <stdint.h>
#include <stdbool.h>

#include "esp_attr.h"
#include "soc/spi_mem_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

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

//SPI status register
#define  ESP_ROM_SPIFLASH_BUSY_FLAG           BIT0
#define  ESP_ROM_SPIFLASH_WRENABLE_FLAG       BIT1
#define  ESP_ROM_SPIFLASH_BP0                 BIT2
#define  ESP_ROM_SPIFLASH_BP1                 BIT3
#define  ESP_ROM_SPIFLASH_BP2                 BIT4
#define  ESP_ROM_SPIFLASH_WR_PROTECT          (ESP_ROM_SPIFLASH_BP0|ESP_ROM_SPIFLASH_BP1|ESP_ROM_SPIFLASH_BP2)
#define  ESP_ROM_SPIFLASH_QE                  BIT9

#define FLASH_ID_GD25LQ32C  0xC86016

typedef enum {
    ESP_ROM_SPIFLASH_RESULT_OK,
    ESP_ROM_SPIFLASH_RESULT_ERR,
    ESP_ROM_SPIFLASH_RESULT_TIMEOUT
} esp_rom_spiflash_result_t;

extern uint8_t g_rom_spiflash_dummy_len_plus[];

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SPI_FLASH_H_ */
