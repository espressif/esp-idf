/*
 Driver bits for PSRAM chips (at the moment only the ESP-PSRAM32 chip).
*/

// Copyright 2013-2017 Espressif Systems (Shanghai) PTE LTD
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


#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "spiram_psram.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/gpio.h"
#include "rom/cache.h"
#include "soc/io_mux_reg.h"
#include "soc/dport_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/efuse_reg.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/periph_ctrl.h"

#if CONFIG_SPIRAM_SUPPORT
#include "soc/rtc.h"

//Commands for PSRAM chip
#define PSRAM_READ                 0x03
#define PSRAM_FAST_READ            0x0B
#define PSRAM_FAST_READ_DUMMY      0x3
#define PSRAM_FAST_READ_QUAD       0xEB
#define PSRAM_FAST_READ_QUAD_DUMMY 0x5
#define PSRAM_WRITE                0x02
#define PSRAM_QUAD_WRITE           0x38
#define PSRAM_ENTER_QMODE          0x35
#define PSRAM_EXIT_QMODE           0xF5
#define PSRAM_RESET_EN             0x66
#define PSRAM_RESET                0x99
#define PSRAM_SET_BURST_LEN        0xC0
#define PSRAM_DEVICE_ID            0x9F

typedef enum {
    PSRAM_CLK_MODE_NORM = 0,    /*!< Normal SPI mode */
    PSRAM_CLK_MODE_DCLK = 1,    /*!< Two extra clock cycles after CS is set high level */
} psram_clk_mode_t;

#define PSRAM_ID_KGD_M          0xff
#define PSRAM_ID_KGD_S             8
#define PSRAM_ID_KGD            0x5d
#define PSRAM_ID_EID_M          0xff
#define PSRAM_ID_EID_S            16

// Use the [7:5](bit7~bit5) of EID to distinguish the psram size:
//
//   BIT7  |  BIT6  |  BIT5  |  SIZE(MBIT)
//   -------------------------------------
//    0    |   0    |   0    |     16
//    0    |   0    |   1    |     32
//    0    |   1    |   0    |     64
#define PSRAM_EID_SIZE_M         0x07
#define PSRAM_EID_SIZE_S            5

typedef enum {
    PSRAM_EID_SIZE_16MBITS = 0,
    PSRAM_EID_SIZE_32MBITS = 1,
    PSRAM_EID_SIZE_64MBITS = 2,
} psram_eid_size_t;

#define PSRAM_KGD(id)         (((id) >> PSRAM_ID_KGD_S) & PSRAM_ID_KGD_M)
#define PSRAM_EID(id)         (((id) >> PSRAM_ID_EID_S) & PSRAM_ID_EID_M)
#define PSRAM_SIZE_ID(id)     ((PSRAM_EID(id) >> PSRAM_EID_SIZE_S) & PSRAM_EID_SIZE_M)
#define PSRAM_IS_VALID(id)    (PSRAM_KGD(id) == PSRAM_ID_KGD)

// For the old version 32Mbit psram, using the spicial driver */
#define PSRAM_IS_32MBIT_VER0(id)  (PSRAM_EID(id) == 0x20)
#define PSRAM_IS_64MBIT_TRIAL(id) (PSRAM_EID(id) == 0x26)

// IO-pins for PSRAM.
// WARNING: PSRAM shares all but the CS and CLK pins with the flash, so these defines
// hardcode the flash pins as well, making this code incompatible with either a setup
// that has the flash on non-standard pins or ESP32s with built-in flash.
#define FLASH_CLK_IO                6
#define FLASH_CS_IO                11
#define FLASH_SPIQ_SD0_IO           7
#define FLASH_SPID_SD1_IO           8
#define FLASH_SPIWP_SD3_IO         10
#define FLASH_SPIHD_SD2_IO          9

#define PSRAM_CLK_IO               17
#define PSRAM_CS_IO                16
#define PSRAM_SPIQ_SD0_IO           7
#define PSRAM_SPID_SD1_IO           8
#define PSRAM_SPIWP_SD3_IO         10
#define PSRAM_SPIHD_SD2_IO          9

// IO-pins of ESP32-PICO-D4 for PSRAM. PSRAM share clock with flash.
// The CS IO can be overwrite via menuconfig.
#define PICO_FLASH_CLK_IO                6
#define PICO_FLASH_CS_IO                16
#define PICO_FLASH_SPIQ_SD0_IO          17
#define PICO_FLASH_SPID_SD1_IO           8
#define PICO_FLASH_SPIWP_SD3_IO          7
#define PICO_FLASH_SPIHD_SD2_IO         11

#define PICO_PSRAM_CLK_IO                6
#define PICO_PSRAM_CS_IO                CONFIG_PICO_PSRAM_CS_IO
#define PICO_PSRAM_SPIQ_SD0_IO          17
#define PICO_PSRAM_SPID_SD1_IO           8
#define PICO_PSRAM_SPIWP_SD3_IO          7
#define PICO_PSRAM_SPIHD_SD2_IO         11

typedef struct {
    uint8_t flash_clk_io;
    uint8_t flash_cs_io;
    uint8_t flash_spiq_sd0_io;
    uint8_t flash_spid_sd1_io;
    uint8_t flash_spiwp_sd3_io;
    uint8_t flash_spihd_sd2_io;
    uint8_t psram_clk_io;
    uint8_t psram_cs_io;
    uint8_t psram_spiq_sd0_io;
    uint8_t psram_spid_sd1_io;
    uint8_t psram_spiwp_sd3_io;
    uint8_t psram_spihd_sd2_io;
} psram_io_t;

#define PSRAM_INTERNAL_IO_28       28
#define PSRAM_INTERNAL_IO_29       29
#define PSRAM_IO_MATRIX_DUMMY_40M   1
#define PSRAM_IO_MATRIX_DUMMY_80M   2

#define _SPI_CACHE_PORT             0
#define _SPI_FLASH_PORT             1
#define _SPI_80M_CLK_DIV            1
#define _SPI_40M_CLK_DIV            2

//For 4MB PSRAM, we need one more SPI host, select which one to use by kconfig
#ifdef CONFIG_SPIRAM_OCCUPY_HSPI_HOST
#define PSRAM_SPI_MODULE    PERIPH_HSPI_MODULE
#define PSRAM_SPI_HOST      HSPI_HOST
#define PSRAM_CLK_SIGNAL    HSPICLK_OUT_IDX
#define PSRAM_SPI_NUM       PSRAM_SPI_2
#define PSRAM_SPICLKEN      DPORT_SPI2_CLK_EN
#elif defined CONFIG_SPIRAM_OCCUPY_VSPI_HOST
#define PSRAM_SPI_MODULE    PERIPH_VSPI_MODULE
#define PSRAM_SPI_HOST      VSPI_HOST
#define PSRAM_CLK_SIGNAL    VSPICLK_OUT_IDX
#define PSRAM_SPI_NUM       PSRAM_SPI_3
#define PSRAM_SPICLKEN      DPORT_SPI3_CLK_EN
#else   //set to SPI avoid HSPI and VSPI being used
#define PSRAM_SPI_MODULE    PERIPH_SPI_MODULE
#define PSRAM_SPI_HOST      SPI_HOST
#define PSRAM_CLK_SIGNAL    SPICLK_OUT_IDX
#define PSRAM_SPI_NUM       PSRAM_SPI_1
#define PSRAM_SPICLKEN      DPORT_SPI01_CLK_EN
#endif

static const char* TAG = "psram";
typedef enum {
    PSRAM_SPI_1  = 0x1,
    PSRAM_SPI_2,
    PSRAM_SPI_3,
    PSRAM_SPI_MAX ,
} psram_spi_num_t;

static psram_cache_mode_t s_psram_mode = PSRAM_CACHE_MAX;
static psram_clk_mode_t s_clk_mode = PSRAM_CLK_MODE_DCLK;
static uint32_t s_psram_id = 0;

/* dummy_len_plus values defined in ROM for SPI flash configuration */
extern uint8_t g_rom_spiflash_dummy_len_plus[];
static int extra_dummy = 0;
typedef enum {
    PSRAM_CMD_QPI,
    PSRAM_CMD_SPI,
} psram_cmd_mode_t;

typedef struct {
    uint16_t cmd;                /*!< Command value */
    uint16_t cmdBitLen;          /*!< Command byte length*/
    uint32_t *addr;              /*!< Point to address value*/
    uint16_t addrBitLen;         /*!< Address byte length*/
    uint32_t *txData;            /*!< Point to send data buffer*/
    uint16_t txDataBitLen;       /*!< Send data byte length.*/
    uint32_t *rxData;            /*!< Point to recevie data buffer*/
    uint16_t rxDataBitLen;       /*!< Recevie Data byte length.*/
    uint32_t dummyBitLen;
} psram_cmd_t;

static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode, psram_vaddr_mode_t vaddrmode);

static void psram_clear_spi_fifo(psram_spi_num_t spi_num)
{
    int i;
    for (i = 0; i < 16; i++) {
        WRITE_PERI_REG(SPI_W0_REG(spi_num)+i*4, 0);
    }
}

//set basic SPI write mode
static void psram_set_basic_write_mode(psram_spi_num_t spi_num)
{
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QUAD);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DUAL);
}
//set QPI write mode
static void psram_set_qio_write_mode(psram_spi_num_t spi_num)
{
    SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QUAD);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DUAL);
}
//set QPI read mode
static void psram_set_qio_read_mode(psram_spi_num_t spi_num)
{
    SET_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QIO);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QUAD);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DUAL);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DIO);
}
//set SPI read mode
static void psram_set_basic_read_mode(psram_spi_num_t spi_num)
{
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QIO);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QUAD);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DUAL);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DIO);
}


//start sending cmd/addr and optionally, receiving data
static void IRAM_ATTR psram_cmd_recv_start(psram_spi_num_t spi_num, uint32_t* pRxData, uint16_t rxByteLen,
        psram_cmd_mode_t cmd_mode)
{
    //get cs1
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS1_DIS_M);
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS0_DIS_M);

    uint32_t mode_backup = (READ_PERI_REG(SPI_USER_REG(spi_num)) >> SPI_FWRITE_DUAL_S) & 0xf;
    uint32_t rd_mode_backup = READ_PERI_REG(SPI_CTRL_REG(spi_num)) & (SPI_FREAD_DIO_M | SPI_FREAD_DUAL_M | SPI_FREAD_QUAD_M | SPI_FREAD_QIO_M);
    if (cmd_mode == PSRAM_CMD_SPI) {
        psram_set_basic_write_mode(spi_num);
        psram_set_basic_read_mode(spi_num);
    } else if (cmd_mode == PSRAM_CMD_QPI) {
        psram_set_qio_write_mode(spi_num);
        psram_set_qio_read_mode(spi_num);
    }

    //Wait for SPI0 to idle
    while ( READ_PERI_REG(SPI_EXT2_REG(0)) != 0);
    DPORT_SET_PERI_REG_MASK(DPORT_HOST_INF_SEL_REG, 1 << 14);

    // Start send data
    SET_PERI_REG_MASK(SPI_CMD_REG(spi_num), SPI_USR);
    while ((READ_PERI_REG(SPI_CMD_REG(spi_num)) & SPI_USR));
    DPORT_CLEAR_PERI_REG_MASK(DPORT_HOST_INF_SEL_REG, 1 << 14);

    //recover spi mode
    SET_PERI_REG_BITS(SPI_USER_REG(spi_num), (pRxData?SPI_FWRITE_DUAL_M:0xf), mode_backup, SPI_FWRITE_DUAL_S);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), (SPI_FREAD_DIO_M|SPI_FREAD_DUAL_M|SPI_FREAD_QUAD_M|SPI_FREAD_QIO_M));
    SET_PERI_REG_MASK(SPI_CTRL_REG(spi_num), rd_mode_backup);

    //return cs to cs0
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS1_DIS_M);
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS0_DIS_M);

    if (pRxData) {
        int idx = 0;
        // Read data out
        do {
            *pRxData++ = READ_PERI_REG(SPI_W0_REG(spi_num) + (idx << 2));
        } while (++idx < ((rxByteLen / 4) + ((rxByteLen % 4) ? 1 : 0)));
    }
}

static uint32_t backup_usr[3];
static uint32_t backup_usr1[3];
static uint32_t backup_usr2[3];

//setup spi command/addr/data/dummy in user mode
static int psram_cmd_config(psram_spi_num_t spi_num, psram_cmd_t* pInData)
{
    while (READ_PERI_REG(SPI_CMD_REG(spi_num)) & SPI_USR);
    backup_usr[spi_num]=READ_PERI_REG(SPI_USER_REG(spi_num));
    backup_usr1[spi_num]=READ_PERI_REG(SPI_USER1_REG(spi_num));
    backup_usr2[spi_num]=READ_PERI_REG(SPI_USER2_REG(spi_num));
    // Set command by user.
    if (pInData->cmdBitLen != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2_REG(spi_num), SPI_USR_COMMAND_BITLEN, pInData->cmdBitLen - 1,
                SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_COMMAND);
        // Load command,bit15-0 is cmd value.
        SET_PERI_REG_BITS(SPI_USER2_REG(spi_num), SPI_USR_COMMAND_VALUE, pInData->cmd, SPI_USR_COMMAND_VALUE_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2_REG(spi_num), SPI_USR_COMMAND_BITLEN, 0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pInData->addrBitLen != 0) {
        SET_PERI_REG_BITS(SPI_USER1_REG(spi_num), SPI_USR_ADDR_BITLEN, (pInData->addrBitLen - 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_ADDR);
        // Set address
        WRITE_PERI_REG(SPI_ADDR_REG(spi_num), *pInData->addr);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1_REG(spi_num), SPI_USR_ADDR_BITLEN, 0, SPI_USR_ADDR_BITLEN_S);
    }
    // Set data by user.
    uint32_t* p_tx_val = pInData->txData;
    if (pInData->txDataBitLen != 0) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MOSI);
        // Load send buffer
        int len = (pInData->txDataBitLen + 31) / 32;
        if (p_tx_val != NULL) {
            memcpy((void*)SPI_W0_REG(spi_num), p_tx_val, len * 4);
        }
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spi_num), SPI_USR_MOSI_DBITLEN, (pInData->txDataBitLen - 1),
                SPI_USR_MOSI_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MOSI);
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spi_num), SPI_USR_MOSI_DBITLEN, 0, SPI_USR_MOSI_DBITLEN_S);
    }
    // Set rx data by user.
    if (pInData->rxDataBitLen != 0) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MISO);
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spi_num), SPI_USR_MISO_DBITLEN, (pInData->rxDataBitLen - 1),
                SPI_USR_MISO_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spi_num), SPI_USR_MISO_DBITLEN, 0, SPI_USR_MISO_DBITLEN_S);
    }
    if (pInData->dummyBitLen != 0) {
        SET_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_USER1_REG(PSRAM_SPI_1), SPI_USR_DUMMY_CYCLELEN_V, pInData->dummyBitLen - 1,
                SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_USER1_REG(PSRAM_SPI_1), SPI_USR_DUMMY_CYCLELEN_V, 0, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
    }
    return 0;
}

static void psram_cmd_end(int spi_num) {
    while (READ_PERI_REG(SPI_CMD_REG(spi_num)) & SPI_USR);
    WRITE_PERI_REG(SPI_USER_REG(spi_num), backup_usr[spi_num]);
    WRITE_PERI_REG(SPI_USER1_REG(spi_num), backup_usr1[spi_num]);
    WRITE_PERI_REG(SPI_USER2_REG(spi_num), backup_usr2[spi_num]);
}

//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    uint32_t cmd_exit_qpi;
    cmd_exit_qpi = PSRAM_EXIT_QMODE;
    ps_cmd.txDataBitLen = 8;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
            case PSRAM_CACHE_F80M_S80M:
                break;
            case PSRAM_CACHE_F80M_S40M:
            case PSRAM_CACHE_F40M_S40M:
            default:
                cmd_exit_qpi = PSRAM_EXIT_QMODE << 8;
                ps_cmd.txDataBitLen = 16;
                break;
        }
    }
    ps_cmd.txData = &cmd_exit_qpi;
    ps_cmd.cmd = 0;
    ps_cmd.cmdBitLen = 0;
    ps_cmd.addr = 0;
    ps_cmd.addrBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_QPI);
    psram_cmd_end(spi_num);
}

//read psram id
static void psram_read_id(uint32_t* dev_id)
{
    psram_spi_num_t spi_num = PSRAM_SPI_1;
    psram_disable_qio_mode(spi_num);
    uint32_t dummy_bits = 0 + extra_dummy;
    psram_cmd_t ps_cmd;

    uint32_t addr = 0;
    ps_cmd.addrBitLen = 3 * 8;
    ps_cmd.cmd = PSRAM_DEVICE_ID;
    ps_cmd.cmdBitLen = 8;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
            case PSRAM_CACHE_F80M_S80M:
                break;
            case PSRAM_CACHE_F80M_S40M:
            case PSRAM_CACHE_F40M_S40M:
            default:
                ps_cmd.cmdBitLen = 2;   //this two bits is used to delay 2 clock cycle
                ps_cmd.cmd = 0;
                addr = (PSRAM_DEVICE_ID << 24) | 0;
                ps_cmd.addrBitLen = 4 * 8;
                break;
        }
    }
    ps_cmd.addr = &addr;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.rxDataBitLen = 4 * 8;
    ps_cmd.rxData = dev_id;
    ps_cmd.dummyBitLen = dummy_bits;

    psram_cmd_config(spi_num, &ps_cmd);
    psram_clear_spi_fifo(spi_num);
    psram_cmd_recv_start(spi_num, ps_cmd.rxData, ps_cmd.rxDataBitLen / 8, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);
}

//enter QPI mode
static esp_err_t IRAM_ATTR psram_enable_qio_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    uint32_t addr = (PSRAM_ENTER_QMODE << 24) | 0;

    ps_cmd.cmdBitLen = 0;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
            case PSRAM_CACHE_F80M_S80M:
                break;
            case PSRAM_CACHE_F80M_S40M:
            case PSRAM_CACHE_F40M_S40M:
            default:
                ps_cmd.cmdBitLen = 2;
                break;
        }
    }
    ps_cmd.cmd = 0;
    ps_cmd.addr = &addr;
    ps_cmd.addrBitLen = 8;
    ps_cmd.txData = NULL;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);
    return ESP_OK;
}

void psram_set_cs_timing(psram_spi_num_t spi_num, psram_clk_mode_t clk_mode)
{
    if (clk_mode == PSRAM_CLK_MODE_NORM) { 
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
        // Set cs time.
        SET_PERI_REG_BITS(SPI_CTRL2_REG(spi_num), SPI_HOLD_TIME_V, 1, SPI_HOLD_TIME_S);
        SET_PERI_REG_BITS(SPI_CTRL2_REG(spi_num), SPI_SETUP_TIME_V, 0, SPI_SETUP_TIME_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    }
}

//spi param init for psram
void IRAM_ATTR psram_spi_init(psram_spi_num_t spi_num, psram_cache_mode_t mode)
{
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spi_num), SPI_TRANS_DONE << 5);
    SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CS_SETUP);
    // SPI_CPOL & SPI_CPHA
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(spi_num), SPI_CK_IDLE_EDGE);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CK_OUT_EDGE);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_WR_BIT_ORDER);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_RD_BIT_ORDER);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_DOUTDIN);
    // May be not must to do.
    WRITE_PERI_REG(SPI_USER1_REG(spi_num), 0);
    // SPI mode type
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spi_num), SPI_SLAVE_MODE);
    memset((void*)SPI_W0_REG(spi_num), 0, 16 * 4);
    psram_set_cs_timing(spi_num, s_clk_mode);
}

/*
 * Psram mode init will overwrite original flash speed mode, so that it is possible to change psram and flash speed after OTA.
 * Flash read mode(QIO/QOUT/DIO/DOUT) will not be changed in app bin. It is decided by bootloader, OTA can not change this mode.
 */
static void IRAM_ATTR psram_gpio_config(psram_io_t psram_io, psram_cache_mode_t mode)
{
    int spi_cache_dummy = 0;
    uint32_t rd_mode_reg = READ_PERI_REG(SPI_CTRL_REG(0));
    if (rd_mode_reg & (SPI_FREAD_QIO_M | SPI_FREAD_DIO_M)) {
        spi_cache_dummy = SPI0_R_QIO_DUMMY_CYCLELEN;
    } else if (rd_mode_reg & (SPI_FREAD_QUAD_M | SPI_FREAD_DUAL_M)) {
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
    } else {
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
    }
    // In bootloader, all the signals are already configured,
    // We keep the following code in case the bootloader is some older version.
    gpio_matrix_out(psram_io.flash_cs_io, SPICS0_OUT_IDX, 0, 0);
    gpio_matrix_out(psram_io.psram_spiq_sd0_io, SPIQ_OUT_IDX, 0, 0);
    gpio_matrix_in(psram_io.psram_spiq_sd0_io, SPIQ_IN_IDX, 0);
    gpio_matrix_out(psram_io.psram_spid_sd1_io, SPID_OUT_IDX, 0, 0);
    gpio_matrix_in(psram_io.psram_spid_sd1_io, SPID_IN_IDX, 0);
    gpio_matrix_out(psram_io.psram_spiwp_sd3_io, SPIWP_OUT_IDX, 0, 0);
    gpio_matrix_in(psram_io.psram_spiwp_sd3_io, SPIWP_IN_IDX, 0);
    gpio_matrix_out(psram_io.psram_spihd_sd2_io, SPIHD_OUT_IDX, 0, 0);
    gpio_matrix_in(psram_io.psram_spihd_sd2_io, SPIHD_IN_IDX, 0);

    switch (mode) {
        case PSRAM_CACHE_F80M_S40M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
            g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
            g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
            SET_PERI_REG_BITS(SPI_USER1_REG(_SPI_CACHE_PORT), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_80M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_CACHE_PORT);
            esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_FLASH_PORT);
            //set drive ability for clock
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_clk_io], FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.psram_clk_io], FUN_DRV, 2, FUN_DRV_S);
            break;
        case PSRAM_CACHE_F80M_S80M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_80M;
            g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
            g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
            SET_PERI_REG_BITS(SPI_USER1_REG(_SPI_CACHE_PORT), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_80M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_CACHE_PORT);
            esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_FLASH_PORT);
            //set drive ability for clock
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_clk_io], FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.psram_clk_io], FUN_DRV, 3, FUN_DRV_S);
            break;
        case PSRAM_CACHE_F40M_S40M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
            g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
            g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
            SET_PERI_REG_BITS(SPI_USER1_REG(_SPI_CACHE_PORT), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_40M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_CACHE_PORT);
            esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_FLASH_PORT);
            //set drive ability for clock
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_clk_io], FUN_DRV, 2, FUN_DRV_S);
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.psram_clk_io], FUN_DRV, 2, FUN_DRV_S);
            break;
        default:
            break;
    }
    SET_PERI_REG_MASK(SPI_USER_REG(0), SPI_USR_DUMMY); // dummy en

    //select pin function gpio
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.flash_spiq_sd0_io],  PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.flash_spid_sd1_io],  PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.flash_spihd_sd2_io], PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.flash_spiwp_sd3_io], PIN_FUNC_GPIO);
    //flash clock signal should come from IO MUX.
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.flash_clk_io], FUNC_SD_CLK_SPICLK);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.flash_cs_io],  PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.psram_cs_io],  PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[psram_io.psram_clk_io], PIN_FUNC_GPIO);

    uint32_t flash_id = g_rom_flashchip.device_id;
    if (flash_id == FLASH_ID_GD25LQ32C) {
        // Set drive ability for 1.8v flash in 80Mhz.
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_spiq_sd0_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_spid_sd1_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_spihd_sd2_io], FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_spiwp_sd3_io], FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_cs_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.flash_clk_io], FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.psram_cs_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io.psram_clk_io], FUN_DRV_V, 3, FUN_DRV_S);
    }
}

psram_size_t psram_get_size()
{
    if ((PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_64MBITS) || PSRAM_IS_64MBIT_TRIAL(s_psram_id)) {
        return PSRAM_SIZE_64MBITS;
    } else if (PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_32MBITS) {
        return PSRAM_SIZE_32MBITS;
    } else if (PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_16MBITS) {
        return PSRAM_SIZE_16MBITS;
    } else {
        return PSRAM_SIZE_MAX;
    }
}

//psram gpio init , different working frequency we have different solutions
esp_err_t IRAM_ATTR psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode)   //psram init
{
    psram_io_t psram_io;
    uint32_t chip_ver = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG);
    uint32_t pkg_ver = chip_ver & 0x7;
    if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5) {
        ESP_EARLY_LOGE(TAG, "ESP32D2WD do not support psram yet");
        return ESP_FAIL;
    } else if ((pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2) || (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4)) {
        ESP_EARLY_LOGI(TAG, "This chip is ESP32-PICO");
        rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
        if (cfg.tieh != RTC_VDDSDIO_TIEH_3_3V) {
            ESP_EARLY_LOGE(TAG, "VDDSDIO is not 3.3V");
            return ESP_FAIL;
        }
        psram_io.flash_clk_io       = PICO_FLASH_CLK_IO;
        psram_io.flash_cs_io        = PICO_FLASH_CS_IO;
        psram_io.flash_spiq_sd0_io  = PICO_FLASH_SPIQ_SD0_IO;
        psram_io.flash_spid_sd1_io  = PICO_FLASH_SPID_SD1_IO;
        psram_io.flash_spiwp_sd3_io = PICO_FLASH_SPIWP_SD3_IO;
        psram_io.flash_spihd_sd2_io = PICO_FLASH_SPIHD_SD2_IO;
        psram_io.psram_clk_io       = PICO_PSRAM_CLK_IO;
        psram_io.psram_cs_io        = PICO_PSRAM_CS_IO;
        psram_io.psram_spiq_sd0_io  = PICO_PSRAM_SPIQ_SD0_IO;
        psram_io.psram_spid_sd1_io  = PICO_PSRAM_SPID_SD1_IO;
        psram_io.psram_spiwp_sd3_io = PICO_PSRAM_SPIWP_SD3_IO;
        psram_io.psram_spihd_sd2_io = PICO_PSRAM_SPIHD_SD2_IO;

        s_clk_mode = PSRAM_CLK_MODE_NORM;
    } else {
        psram_io.flash_clk_io       = FLASH_CLK_IO;
        psram_io.flash_cs_io        = FLASH_CS_IO;
        psram_io.flash_spiq_sd0_io  = FLASH_SPIQ_SD0_IO;
        psram_io.flash_spid_sd1_io  = FLASH_SPID_SD1_IO;
        psram_io.flash_spiwp_sd3_io = FLASH_SPIWP_SD3_IO;
        psram_io.flash_spihd_sd2_io = FLASH_SPIHD_SD2_IO;
        psram_io.psram_clk_io       = PSRAM_CLK_IO;
        psram_io.psram_cs_io        = PSRAM_CS_IO;
        psram_io.psram_spiq_sd0_io  = PSRAM_SPIQ_SD0_IO;
        psram_io.psram_spid_sd1_io  = PSRAM_SPID_SD1_IO;
        psram_io.psram_spiwp_sd3_io = PSRAM_SPIWP_SD3_IO;
        psram_io.psram_spihd_sd2_io = PSRAM_SPIHD_SD2_IO;
    }

    WRITE_PERI_REG(GPIO_ENABLE_W1TC_REG, BIT(psram_io.psram_clk_io) | BIT(psram_io.psram_cs_io));   //DISABLE OUPUT FOR IO16/17
    assert(mode < PSRAM_CACHE_MAX && "we don't support any other mode for now.");
    s_psram_mode = mode;

    WRITE_PERI_REG(SPI_EXT3_REG(0), 0x1);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_PREP_HOLD_M);

    psram_spi_init(PSRAM_SPI_1, mode);
    
    gpio_matrix_out(psram_io.psram_cs_io, SPICS1_OUT_IDX, 0, 0);
    switch (mode) {
        case PSRAM_CACHE_F80M_S80M:
            gpio_matrix_out(psram_io.psram_clk_io, SPICLK_OUT_IDX, 0, 0);
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
                /* We need to delay CLK to the PSRAM with respect to the clock signal as output by the SPI peripheral.
                We do this by routing it signal to signal 224/225, which are used as a loopback; the extra run through
                the GPIO matrix causes the delay. We use GPIO20 (which is not in any package but has pad logic in
                silicon) as a temporary pad for this. So the signal path is:
                SPI CLK --> GPIO28 --> signal224(in then out) --> internal GPIO29 --> signal225(in then out) --> GPIO17(PSRAM CLK)
                */
                gpio_matrix_out(PSRAM_INTERNAL_IO_28,  SPICLK_OUT_IDX, 0, 0);
                gpio_matrix_in(PSRAM_INTERNAL_IO_28,   SIG_IN_FUNC224_IDX, 0);
                gpio_matrix_out(PSRAM_INTERNAL_IO_29,  SIG_IN_FUNC224_IDX, 0, 0);
                gpio_matrix_in(PSRAM_INTERNAL_IO_29,   SIG_IN_FUNC225_IDX, 0);
                gpio_matrix_out(psram_io.psram_clk_io, SIG_IN_FUNC225_IDX, 0, 0);
            } else {
                gpio_matrix_out(psram_io.psram_clk_io, SPICLK_OUT_IDX, 0, 0);
            }
            break;
    }
    #if CONFIG_BOOTLOADER_VDDSDIO_BOOST_1_9V
        // For flash 80Mhz, we must update ldo voltage in case older version of bootloader didn't do this.
        rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
        if (cfg.enable == 1 && cfg.tieh == RTC_VDDSDIO_TIEH_1_8V) {    // VDDSDIO regulator is enabled @ 1.8V
            cfg.drefh = 3;
            cfg.drefm = 3;
            cfg.drefl = 3;
            cfg.force = 1;
            rtc_vddsdio_set_config(cfg);
            ets_delay_us(10);                     // wait for regulator to become stable
        }
    #endif
    // GPIO related settings
    psram_gpio_config(psram_io, mode);
    psram_read_id(&s_psram_id);
    if (!PSRAM_IS_VALID(s_psram_id)) {
        return ESP_FAIL;
    }
    
    if (PSRAM_IS_32MBIT_VER0(s_psram_id)) {
        s_clk_mode = PSRAM_CLK_MODE_DCLK;
        if (mode == PSRAM_CACHE_F80M_S80M) {
            /*   note: If the third mode(80Mhz+80Mhz) is enabled for 32MBit 1V8 psram, VSPI port will be 
                 occupied by the system.
                 Application code should never touch VSPI hardware in this case.  We try to stop applications
                 from doing this using the drivers by claiming the port for ourselves */
            periph_module_enable(PERIPH_VSPI_MODULE);
            bool r=spicommon_periph_claim(VSPI_HOST);
            if (!r) {
                return ESP_ERR_INVALID_STATE;
            }
            gpio_matrix_out(psram_io.psram_clk_io, PSRAM_CLK_SIGNAL, 0, 0);
            //use spi3 clock,but use spi1 data/cs wires
            //We get a solid 80MHz clock from SPI3 by setting it up, starting a transaction, waiting until it
            //is in progress, then cutting the clock (but not the reset!) to that peripheral.
            WRITE_PERI_REG(SPI_ADDR_REG(PSRAM_SPI_NUM), 32 << 24);
            SET_PERI_REG_MASK(SPI_CMD_REG(PSRAM_SPI_NUM), SPI_FLASH_READ_M);
            uint32_t spi_status;
            while (1) {
                spi_status = READ_PERI_REG(SPI_EXT2_REG(PSRAM_SPI_NUM));
                if (spi_status != 0 && spi_status != 1) {
                    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, PSRAM_SPICLKEN);
                    break;
                }
            }
        }
    } else {
        // For other psram, we don't need any extra clock cycles after cs get back to high level
        s_clk_mode = PSRAM_CLK_MODE_NORM;
        gpio_matrix_out(PSRAM_INTERNAL_IO_28, SIG_GPIO_OUT_IDX, 0, 0);
        gpio_matrix_out(PSRAM_INTERNAL_IO_29, SIG_GPIO_OUT_IDX, 0, 0);
        gpio_matrix_out(psram_io.psram_clk_io, SPICLK_OUT_IDX, 0, 0);
    }
    
    // Update cs timing according to psram driving method.
    psram_set_cs_timing(PSRAM_SPI_1, s_clk_mode);
    psram_set_cs_timing(_SPI_CACHE_PORT, s_clk_mode);
    psram_enable_qio_mode(PSRAM_SPI_1);
    psram_cache_init(mode, vaddrmode);
    return ESP_OK;
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode, psram_vaddr_mode_t vaddrmode)
{
    switch (psram_cache_mode) {
        case PSRAM_CACHE_F80M_S80M:
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31));   //flash 1 div clk,80+40;
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0. FLASH DIV 2+SRAM DIV4
            break;
        case PSRAM_CACHE_F80M_S40M:
            SET_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31)); //flash 1 div clk
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0.
            break;
        case PSRAM_CACHE_F40M_S40M:
        default:
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31)); //flash 1 div clk
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div
            break;
    }
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_WCMD_M);     // cache write command enable
    SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_ADDR_BITLEN_V, 23, SPI_SRAM_ADDR_BITLEN_S); //write address for cache command.
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_SRAM_QIO_M);     //enable qio mode for cache command
    CLEAR_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_SRAM_DIO_M);     //disable dio mode for cache command

    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_RD_SRAM_DUMMY_M);   //enable cache read dummy
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_RCMD_M); //enable user mode for cache read command
    SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
            SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE,
            SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
    SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
            SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ_QUAD,
            SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b
    SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_QUAD_DUMMY + extra_dummy,
            SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy

    //config sram cache r/w command
    switch (psram_cache_mode) {
        case PSRAM_CACHE_F80M_S80M: //in this mode , no delay is needed
            break;
        case PSRAM_CACHE_F80M_S40M: //is sram is @40M, need 2 cycles of delay
        case PSRAM_CACHE_F40M_S40M:
        default:
            if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
                SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 15,
                        SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S); //read command length, 2 bytes(1byte for delay),sending in qio mode in cache
                SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, ((PSRAM_FAST_READ_QUAD) << 8),
                        SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b, read command value,(0x00 for delay,0x0b for cmd)
                SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 15,
                        SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S); //write command length,2 bytes(1byte for delay,send in qio mode in cache)
                SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, ((PSRAM_QUAD_WRITE) << 8),
                        SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38, write command value,(0x00 for delay)
                SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_QUAD_DUMMY + extra_dummy,
                        SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            }
            break;
    }

    DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL|DPORT_PRO_DRAM_SPLIT);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL|DPORT_APP_DRAM_SPLIT);
    if (vaddrmode == PSRAM_VADDR_MODE_LOWHIGH) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL);
    } else if (vaddrmode == PSRAM_VADDR_MODE_EVENODD) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_SPLIT);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_SPLIT);
    }

    DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MASK_DRAM1|DPORT_PRO_CACHE_MASK_OPSDRAM); //use Dram1 to visit ext sram.
    //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
    DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CMMU_SRAM_PAGE_MODE, 0, DPORT_PRO_CMMU_SRAM_PAGE_MODE_S);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DRAM1|DPORT_APP_CACHE_MASK_OPSDRAM); //use Dram1 to visit ext sram.
    //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
    DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CMMU_SRAM_PAGE_MODE, 0, DPORT_APP_CMMU_SRAM_PAGE_MODE_S);

    CLEAR_PERI_REG_MASK(SPI_PIN_REG(0), SPI_CS1_DIS_M); //ENABLE SPI0 CS1 TO PSRAM(CS0--FLASH; CS1--SRAM)
}

#endif // CONFIG_SPIRAM_SUPPORT
