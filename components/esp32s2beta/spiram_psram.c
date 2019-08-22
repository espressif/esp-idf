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
#include "esp32s2beta/rom/ets_sys.h"
#include "esp32s2beta/rom/spi_flash.h"
#include "esp32s2beta/rom/gpio.h"
#include "esp32s2beta/rom/cache.h"
#include "soc/io_mux_reg.h"
#include "soc/dport_reg.h"
#include "soc/apb_ctrl_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/efuse_reg.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/periph_ctrl.h"

#if CONFIG_SPIRAM
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

#define PSRAM_KGD(id)          (((id) >> PSRAM_ID_KGD_S) & PSRAM_ID_KGD_M)
#define PSRAM_EID(id)          (((id) >> PSRAM_ID_EID_S) & PSRAM_ID_EID_M)
#define PSRAM_IS_VALID(id)     (PSRAM_KGD(id) == PSRAM_ID_KGD)

// PSRAM_EID = 0x26 or 0x4x ----> 64MBit psram
// PSRAM_EID = 0x20 ------------> 32MBit psram
#define PSRAM_IS_64MBIT(id)       ((PSRAM_EID(id) == 0x26) || ((PSRAM_EID(id) & 0xf0) == 0x40))
#define PSRAM_IS_32MBIT_VER0(id)  (PSRAM_EID(id) == 0x20)

// IO-pins for PSRAM. These need to be in the VDD_SIO power domain because all chips we
// currently support are 1.8V parts.
// WARNING: PSRAM shares all but the CS and CLK pins with the flash, so these defines
// hardcode the flash pins as well, making this code incompatible with either a setup
// that has the flash on non-standard pins or ESP32s with built-in flash.
#define FLASH_CLK_IO      SPI_CLK_GPIO_NUM  //Psram clock is a delayed version of this in 40MHz mode
#define FLASH_CS_IO       SPI_CS0_GPIO_NUM
#define PSRAM_CS_IO       26
#define PSRAM_SPIQ_IO     SPI_Q_GPIO_NUM
#define PSRAM_SPID_IO     SPI_D_GPIO_NUM
#define PSRAM_SPIWP_IO    SPI_WP_GPIO_NUM
#define PSRAM_SPIHD_IO    SPI_HD_GPIO_NUM
#define PSRAM_INTERNAL_IO_28       28
#define PSRAM_INTERNAL_IO_29       29
#define PSRAM_IO_MATRIX_DUMMY_20M   0
#define PSRAM_IO_MATRIX_DUMMY_40M   0
#define PSRAM_IO_MATRIX_DUMMY_80M   0

#define _SPI_CACHE_PORT   0
#define _SPI_FLASH_PORT   1
#define _SPI_80M_CLK_DIV  1
#define _SPI_40M_CLK_DIV  2
#define _SPI_20M_CLK_DIV  4

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
        WRITE_PERI_REG(SPI_MEM_W0_REG(spi_num)+i*4, 0);
    }
}

//set basic SPI write mode
static void psram_set_basic_write_mode(psram_spi_num_t spi_num)
{
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_QIO);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FCMD_QUAD_M);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_DIO);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_QUAD);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_DUAL);
}
//set QPI write mode
static void psram_set_qio_write_mode(psram_spi_num_t spi_num)
{
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_QIO);
    SET_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FCMD_QUAD_M);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_DIO);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_QUAD);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_FWRITE_DUAL);
}
//set QPI read mode
static void psram_set_qio_read_mode(psram_spi_num_t spi_num)
{
    SET_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_QIO);
    SET_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FCMD_QUAD_M);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_QUAD);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_DUAL);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_DIO);
}
//set SPI read mode
static void psram_set_basic_read_mode(psram_spi_num_t spi_num)
{
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_QIO);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FCMD_QUAD_M);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_QUAD);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_DUAL);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FREAD_DIO);
}


//start sending cmd/addr and optionally, receiving data
static void IRAM_ATTR psram_cmd_recv_start(psram_spi_num_t spi_num, uint32_t* pRxData, uint16_t rxByteLen,
        psram_cmd_mode_t cmd_mode)
{
    //get cs1
    CLEAR_PERI_REG_MASK(SPI_MEM_MISC_REG(PSRAM_SPI_1), SPI_MEM_CS1_DIS_M);
    SET_PERI_REG_MASK(SPI_MEM_MISC_REG(PSRAM_SPI_1), SPI_MEM_CS0_DIS_M);

    uint32_t mode_backup = (READ_PERI_REG(SPI_MEM_USER_REG(spi_num)) >> SPI_MEM_FWRITE_DUAL_S) & 0xf;
#ifdef FAKE_QPI
    uint32_t rd_mode_backup = READ_PERI_REG(SPI_MEM_CTRL_REG(spi_num)) & (SPI_MEM_FREAD_DIO_M | SPI_MEM_FREAD_DUAL_M | SPI_MEM_FREAD_QUAD_M | SPI_MEM_FREAD_QIO_M);
#else
    uint32_t rd_mode_backup = READ_PERI_REG(SPI_MEM_CTRL_REG(spi_num)) & (SPI_MEM_FREAD_DIO_M | SPI_MEM_FREAD_DUAL_M | SPI_MEM_FREAD_QUAD_M | SPI_MEM_FREAD_QIO_M | SPI_MEM_FCMD_QUAD);
#endif
    if (cmd_mode == PSRAM_CMD_SPI) {
        psram_set_basic_write_mode(spi_num);
        psram_set_basic_read_mode(spi_num);
    } else if (cmd_mode == PSRAM_CMD_QPI) {
        psram_set_qio_write_mode(spi_num);
        psram_set_qio_read_mode(spi_num);
    }

    // Start send data
    SET_PERI_REG_MASK(SPI_MEM_CMD_REG(spi_num), SPI_MEM_USR);
    while ((READ_PERI_REG(SPI_MEM_CMD_REG(spi_num)) & SPI_MEM_USR));

    //recover spi mode
    SET_PERI_REG_BITS(SPI_MEM_USER_REG(spi_num), (pRxData?SPI_MEM_FWRITE_DUAL_M:0xf), mode_backup, SPI_MEM_FWRITE_DUAL_S);
#ifdef FAKE_QPI
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), (SPI_MEM_FREAD_DIO_M|SPI_MEM_FREAD_DUAL_M|SPI_MEM_FREAD_QUAD_M|SPI_MEM_FREAD_QIO_M));
#else
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), (SPI_MEM_FREAD_DIO_M|SPI_MEM_FREAD_DUAL_M|SPI_MEM_FREAD_QUAD_M|SPI_MEM_FREAD_QIO_M|SPI_MEM_FCMD_QUAD));
#endif
    SET_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), rd_mode_backup);

    //return cs to cs0
    SET_PERI_REG_MASK(SPI_MEM_MISC_REG(PSRAM_SPI_1), SPI_MEM_CS1_DIS_M);
    CLEAR_PERI_REG_MASK(SPI_MEM_MISC_REG(PSRAM_SPI_1), SPI_MEM_CS0_DIS_M);

    if (pRxData) {
        int idx = 0;
        // Read data out
        do {
            *pRxData++ = READ_PERI_REG(SPI_MEM_W0_REG(spi_num) + (idx << 2));
        } while (++idx < ((rxByteLen / 4) + ((rxByteLen % 4) ? 1 : 0)));
    }
}

static uint32_t backup_usr[3];
static uint32_t backup_usr1[3];
static uint32_t backup_usr2[3];



//setup spi command/addr/data/dummy in user mode
static int psram_cmd_config(psram_spi_num_t spi_num, psram_cmd_t* pInData)
{
    while (READ_PERI_REG(SPI_MEM_CMD_REG(spi_num)) & SPI_MEM_USR);
    backup_usr[spi_num]=READ_PERI_REG(SPI_MEM_USER_REG(spi_num));
    backup_usr1[spi_num]=READ_PERI_REG(SPI_MEM_USER1_REG(spi_num));
    backup_usr2[spi_num]=READ_PERI_REG(SPI_MEM_USER2_REG(spi_num));
    // Set command by user.
    if (pInData->cmdBitLen != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_MEM_USER2_REG(spi_num), SPI_MEM_USR_COMMAND_BITLEN, pInData->cmdBitLen - 1,
                SPI_MEM_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_COMMAND);
        // Load command,bit15-0 is cmd value.
        SET_PERI_REG_BITS(SPI_MEM_USER2_REG(spi_num), SPI_MEM_USR_COMMAND_VALUE, pInData->cmd, SPI_MEM_USR_COMMAND_VALUE_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_MEM_USER2_REG(spi_num), SPI_MEM_USR_COMMAND_BITLEN, 0, SPI_MEM_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pInData->addrBitLen != 0) {
        SET_PERI_REG_BITS(SPI_MEM_USER1_REG(spi_num), SPI_MEM_USR_ADDR_BITLEN, (pInData->addrBitLen - 1), SPI_MEM_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_ADDR);
        // Set address
        WRITE_PERI_REG(SPI_MEM_ADDR_REG(spi_num), *pInData->addr);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_ADDR);
        SET_PERI_REG_BITS(SPI_MEM_USER1_REG(spi_num), SPI_MEM_USR_ADDR_BITLEN, 0, SPI_MEM_USR_ADDR_BITLEN_S);
    }
    // Set data by user.
    uint32_t* p_tx_val = pInData->txData;
    if (pInData->txDataBitLen != 0) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_MOSI);
        // Load send buffer
        int len = (pInData->txDataBitLen + 31) / 32;
        if (p_tx_val != NULL) {
            memcpy((void*)SPI_MEM_W0_REG(spi_num), p_tx_val, len * 4);
        }
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MEM_MOSI_DLEN_REG(spi_num), SPI_MEM_USR_MOSI_DBITLEN, (pInData->txDataBitLen - 1),
                SPI_MEM_USR_MOSI_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_MOSI);
        SET_PERI_REG_BITS(SPI_MEM_MOSI_DLEN_REG(spi_num), SPI_MEM_USR_MOSI_DBITLEN, 0, SPI_MEM_USR_MOSI_DBITLEN_S);
    }
    // Set rx data by user.
    if (pInData->rxDataBitLen != 0) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_MISO);
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MEM_MISO_DLEN_REG(spi_num), SPI_MEM_USR_MISO_DBITLEN, (pInData->rxDataBitLen - 1),
                SPI_MEM_USR_MISO_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_MISO);
        SET_PERI_REG_BITS(SPI_MEM_MISO_DLEN_REG(spi_num), SPI_MEM_USR_MISO_DBITLEN, 0, SPI_MEM_USR_MISO_DBITLEN_S);
    }
    if (pInData->dummyBitLen != 0) {
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(PSRAM_SPI_1), SPI_MEM_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_MEM_USER1_REG(PSRAM_SPI_1), SPI_MEM_USR_DUMMY_CYCLELEN_V, pInData->dummyBitLen - 1,
                SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(PSRAM_SPI_1), SPI_MEM_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_MEM_USER1_REG(PSRAM_SPI_1), SPI_MEM_USR_DUMMY_CYCLELEN_V, 0, SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY
    }
    return 0;
}

void psram_cmd_end(int spi_num) {
    while (READ_PERI_REG(SPI_MEM_CMD_REG(spi_num)) & SPI_MEM_USR);
    WRITE_PERI_REG(SPI_MEM_USER_REG(spi_num), backup_usr[spi_num]);
    WRITE_PERI_REG(SPI_MEM_USER1_REG(spi_num), backup_usr1[spi_num]);
    WRITE_PERI_REG(SPI_MEM_USER2_REG(spi_num), backup_usr2[spi_num]);
}

#ifdef FAKE_QPI
//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    uint32_t cmd_exit_qpi;
    cmd_exit_qpi = PSRAM_EXIT_QMODE;
    ps_cmd.txDataBitLen = 8;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
            case PSRAM_CACHE_S80M:
                break;
            case PSRAM_CACHE_S40M:
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
            case PSRAM_CACHE_S80M:
                break;
            case PSRAM_CACHE_S40M:
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
            case PSRAM_CACHE_S80M:
                break;
            case PSRAM_CACHE_S40M:
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
#else /* FAKE_QPI */
//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    ps_cmd.txData = NULL;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.cmd = PSRAM_EXIT_QMODE;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.addr = 0;
    ps_cmd.addrBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_QPI);
    psram_cmd_end(spi_num);
}


//switch psram burst length(32 bytes or 1024 bytes)
//datasheet says it should be 1024 bytes by default
static void psram_set_wrap_burst_length(psram_spi_num_t spi_num, psram_cmd_mode_t mode)
{
    psram_cmd_t ps_cmd;
    ps_cmd.cmd = 0xC0;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.addr = 0;
    ps_cmd.addrBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, mode);
    psram_cmd_end(spi_num);
}

//send reset command to psram, in spi mode
static void psram_reset_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    ps_cmd.txData = NULL;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.addr = NULL;
    ps_cmd.addrBitLen = 0;
    ps_cmd.cmd = PSRAM_RESET_EN;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);

    memset(&ps_cmd, 0, sizeof(ps_cmd));
    ps_cmd.txData = NULL;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.addr = NULL;
    ps_cmd.addrBitLen = 0;
    ps_cmd.cmd = PSRAM_RESET;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);
}

esp_err_t psram_enable_wrap(uint32_t wrap_size)
{
    switch (wrap_size) {
        case 32:
            psram_set_wrap_burst_length(PSRAM_SPI_1, PSRAM_CMD_QPI);
            return ESP_OK;
        case 16:
        case 64:
        default:
            return ESP_FAIL;
    }
}

bool psram_support_wrap_size(uint32_t wrap_size)
{
    switch (wrap_size) {
        case 0:
        case 32:
            return true;
        case 16:
        case 64:
        default:
            return false;
    }

}

static void psram_read_id(uint32_t* dev_id)
{
    psram_spi_num_t spi_num = PSRAM_SPI_1;
    psram_disable_qio_mode(spi_num);
    uint32_t dummy_bits = 0;
    uint32_t addr = 0;
    psram_cmd_t ps_cmd;
    switch (s_psram_mode) {
        case PSRAM_CACHE_S80M:
            dummy_bits = 0 + extra_dummy;
            break;
        case PSRAM_CACHE_S40M:
        case PSRAM_CACHE_S26M:
        case PSRAM_CACHE_S20M:
        default:
            dummy_bits = 0 + extra_dummy;
            break;
    }
    ps_cmd.cmd = PSRAM_DEVICE_ID;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.addr = &addr;
    ps_cmd.addrBitLen = 24;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.rxDataBitLen = 3 * 8;
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
    ps_cmd.cmd = PSRAM_ENTER_QMODE;
    ps_cmd.cmdBitLen = 8;   //this two bits is used to delay 2 clock cycle
    ps_cmd.addr = NULL;
    ps_cmd.addrBitLen = 0;
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
#endif /* FAKE_QPI */

//spi param init for psram
void IRAM_ATTR psram_spi_init(psram_spi_num_t spi_num, psram_cache_mode_t mode)
{
    uint8_t k;
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_CS_SETUP);
#warning "psram_spi_init: part of configuration missing for esp32s2beta"
#if 0
    // SPI_CPOL & SPI_CPHA
    CLEAR_PERI_REG_MASK(SPI_MEM_MISC_REG(spi_num), SPI_MEM_CK_IDLE_EDGE);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_CK_OUT_EDGE);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_WR_BIT_ORDER);
    CLEAR_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_RD_BIT_ORDER);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_DOUTDIN);
#endif
    // May be not must to do.
    WRITE_PERI_REG(SPI_MEM_USER1_REG(spi_num), 0);
#if 0
    // SPI mode type
    CLEAR_PERI_REG_MASK(SPI_MEM_SLAVE_REG(spi_num), SPI_MEM_SLAVE_MODE);
#endif
    // Set SPI speed for non-80M mode. (80M mode uses APB clock directly.)
    if (mode!=PSRAM_CACHE_S80M) {
        k = 2;      //Main divider. Divide by 2 so we get 40MHz
         //clear bit 31, set SPI clock div
        CLEAR_PERI_REG_MASK(SPI_MEM_CLOCK_REG(spi_num), SPI_MEM_CLK_EQU_SYSCLK);
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num),
                (((k - 1) & SPI_MEM_CLKCNT_N) << SPI_MEM_CLKCNT_N_S) |
                ((((k + 1) / 2 - 1) & SPI_MEM_CLKCNT_H) << SPI_MEM_CLKCNT_H_S) | //50% duty cycle
                (((k - 1) & SPI_MEM_CLKCNT_L) << SPI_MEM_CLKCNT_L_S));
    }
    // Enable MOSI
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_CS_SETUP | SPI_MEM_CS_HOLD | SPI_MEM_USR_MOSI);
    memset((void*)SPI_MEM_W0_REG(spi_num), 0, 16 * 4);
}

/*
 * Psram mode init will overwrite original flash speed mode, so that it is possible to change psram and flash speed after OTA.
 * Flash read mode(QIO/QOUT/DIO/DOUT) will not be changed in app bin. It is decided by bootloader, OTA can not change this mode.
 */
static void IRAM_ATTR psram_gpio_config(psram_cache_mode_t mode)
{
    int spi_cache_dummy = 0;
    uint32_t rd_mode_reg = READ_PERI_REG(SPI_MEM_CTRL_REG(0));
    if (rd_mode_reg & (SPI_MEM_FREAD_QIO_M | SPI_MEM_FREAD_DIO_M)) {
        spi_cache_dummy = SPI0_R_QIO_DUMMY_CYCLELEN;
    } else if (rd_mode_reg & (SPI_MEM_FREAD_QUAD_M | SPI_MEM_FREAD_DUAL_M)) {
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
    } else {
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
    }
    // In bootloader, all the signals are already configured,
    // We keep the following code in case the bootloader is some older version.
    gpio_matrix_out(FLASH_CS_IO, SPICS0_OUT_IDX, 0, 0);
    gpio_matrix_out(PSRAM_SPIQ_IO, SPIQ_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPIQ_IO, SPIQ_IN_IDX, 0);
    gpio_matrix_out(PSRAM_SPID_IO, SPID_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPID_IO, SPID_IN_IDX, 0);
    gpio_matrix_out(PSRAM_SPIWP_IO, SPIWP_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPIWP_IO, SPIWP_IN_IDX, 0);
    gpio_matrix_out(PSRAM_SPIHD_IO, SPIHD_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPIHD_IO, SPIHD_IN_IDX, 0);

#warning "psram_gpio_config: parts not implemented for esp32s2beta"

    switch (mode) {
        case PSRAM_CACHE_S40M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
                g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
                g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
                SET_PERI_REG_BITS(SPI_MEM_USER1_REG(_SPI_CACHE_PORT), SPI_MEM_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_80M, SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY
                esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_CACHE_PORT);
                esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_FLASH_PORT);
#endif
            break;
        case PSRAM_CACHE_S80M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_80M;
#if 0
            g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
            g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
            SET_PERI_REG_BITS(SPI_MEM_USER1_REG(_SPI_CACHE_PORT), SPI_MEM_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_80M, SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY

            CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_MEM_FREAD_QIO | SPI_MEM_FREAD_QUAD | SPI_MEM_FREAD_DIO | SPI_MEM_FREAD_DUAL | SPI_MEM_FASTRD_MODE);
            esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_CACHE_PORT);
            CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_MEM_FREAD_QIO | SPI_MEM_FREAD_QUAD | SPI_MEM_FREAD_DIO | SPI_MEM_FREAD_DUAL | SPI_MEM_FASTRD_MODE);
            esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_FLASH_PORT);

#endif
            break;
        case PSRAM_CACHE_S26M:
        case PSRAM_CACHE_S20M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_20M;
#if 0
            g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_20M;
            g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_20M;
            SET_PERI_REG_BITS(SPI_MEM_USER1_REG(_SPI_CACHE_PORT), SPI_MEM_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_20M, SPI_MEM_USR_DUMMY_CYCLELEN_S);  //DUMMY

            CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_MEM_FREAD_QIO | SPI_MEM_FREAD_QUAD | SPI_MEM_FREAD_DIO | SPI_MEM_FREAD_DUAL | SPI_MEM_FASTRD_MODE);
            esp_rom_spiflash_config_clk(_SPI_20M_CLK_DIV, _SPI_CACHE_PORT);
            CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_MEM_FREAD_QIO | SPI_MEM_FREAD_QUAD | SPI_MEM_FREAD_DIO | SPI_MEM_FREAD_DUAL | SPI_MEM_FASTRD_MODE);
            esp_rom_spiflash_config_clk(_SPI_20M_CLK_DIV, _SPI_FLASH_PORT);
#endif

        default:
            break;
    }
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_USR_DUMMY); // dummy en
    //select pin function gpio
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPIHD_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPIWP_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPICS0_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPIQ_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPID_U, PIN_FUNC_GPIO);
    // flash clock signal should come from IO MUX.
    // set drive ability for clock
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPICLK_U, FUNC_SPICLK_SPICLK);
}

psram_size_t psram_get_size(void)
{
    if (PSRAM_IS_32MBIT_VER0(s_psram_id)) {
        return PSRAM_SIZE_32MBITS;
    } else if (PSRAM_IS_64MBIT(s_psram_id)) {
        return PSRAM_SIZE_64MBITS;
    } else {
        return PSRAM_SIZE_MAX;
    }
}

//psram gpio init , different working frequency we have different solutions
esp_err_t IRAM_ATTR psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode)   //psram init
{

    assert(mode < PSRAM_CACHE_MAX && "we don't support any other mode for now.");
    s_psram_mode = mode;

    periph_module_enable(PERIPH_SPI_MODULE);

#warning "psram_enable: some code disabled for esp32s2beta"
#if 0
    WRITE_PERI_REG(SPI_MEM_EXT3_REG(0), 0x1);
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(PSRAM_SPI_1), SPI_MEM_USR_PREP_HOLD_M);
#endif


    switch (mode) {
        case PSRAM_CACHE_S80M:
        case PSRAM_CACHE_S40M:
        case PSRAM_CACHE_S26M:
        case PSRAM_CACHE_S20M:
        default:
            psram_spi_init(PSRAM_SPI_1, mode);
            CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(PSRAM_SPI_1), SPI_MEM_CS_HOLD);
            gpio_matrix_out(PSRAM_CS_IO, SPICS1_OUT_IDX, 0, 0);
#ifdef FAKE_QPI
            /* We need to delay CLK to the PSRAM with respect to the clock signal as output by the SPI peripheral.
            We do this by routing it signal to signal 220/221, which are used as a loopback; the extra run through
            the GPIO matrix causes the delay. We use GPIO20 (which is not in any package but has pad logic in
            silicon) as a temporary pad for this. So the signal path is:
            SPI CLK --> GPIO28 --> signal220(in then out) --> internal GPIO29 --> signal221(in then out) --> GPIO17(PSRAM CLK)
            */
            gpio_matrix_out(PSRAM_INTERNAL_IO_28, SPICLK_OUT_IDX, 0, 0);
            gpio_matrix_in(PSRAM_INTERNAL_IO_28, SIG_IN_FUNC220_IDX, 0);
            gpio_matrix_out(PSRAM_INTERNAL_IO_29, SIG_IN_FUNC220_IDX, 0, 0);
            gpio_matrix_in(PSRAM_INTERNAL_IO_29, SIG_IN_FUNC221_IDX, 0);
            gpio_matrix_out(PSRAM_CLK_IO, SIG_IN_FUNC221_IDX, 0, 0);
#else
            REG_SET_FIELD(SPI_MEM_SRAM_CMD_REG(0), SPI_MEM_SCLK_MODE, 1);
            REG_SET_FIELD(SPI_MEM_CTRL1_REG(1), SPI_MEM_CLK_MODE, 1);
#endif

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
    CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(PSRAM_SPI_1), SPI_MEM_CS_SETUP_M);
    psram_gpio_config(mode);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[PSRAM_CS_IO], PIN_FUNC_GPIO);
    psram_read_id(&s_psram_id);
    if (!PSRAM_IS_VALID(s_psram_id)) {
        return ESP_FAIL;
    }
    uint32_t flash_id = g_rom_flashchip.device_id;
    if (flash_id == FLASH_ID_GD25LQ32C) {
        // Set drive ability for 1.8v flash in 80Mhz.
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPIHD_U, FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPIWP_U, FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPICS0_U, FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPICLK_U, FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPIQ_U, FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPID_U, FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[PSRAM_CS_IO], FUN_DRV, 3, FUN_DRV_S);
    }
    if (PSRAM_IS_64MBIT(s_psram_id)) {
        // For this psram, we don't need any extra clock cycles after cs get back to high level
        s_clk_mode = PSRAM_CLK_MODE_NORM;
        REG_SET_FIELD(SPI_MEM_SRAM_CMD_REG(0), SPI_MEM_SCLK_MODE, 0);
        REG_SET_FIELD(SPI_MEM_CTRL1_REG(1), SPI_MEM_CLK_MODE, 0);
    } else if (PSRAM_IS_32MBIT_VER0(s_psram_id)) {
        s_clk_mode = PSRAM_CLK_MODE_DCLK;
        if (mode == PSRAM_CACHE_S80M) {
        }
    }
    psram_reset_mode(PSRAM_SPI_1);
    psram_enable_qio_mode(PSRAM_SPI_1);
    psram_cache_init(mode, vaddrmode);
    return ESP_OK;
}

static void IRAM_ATTR psram_clock_set(psram_spi_num_t spi_num, int8_t freqdiv)
{
    uint32_t  freqbits;
    if (1 >= freqdiv) {
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), SPI_MEM_SCLK_EQU_SYSCLK);
    } else {
        freqbits = (((freqdiv-1)<<SPI_MEM_SCLKCNT_N_S)) | (((freqdiv/2-1)<<SPI_MEM_SCLKCNT_H_S)) | ((freqdiv-1)<<SPI_MEM_SCLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), freqbits);
    }
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode, psram_vaddr_mode_t vaddrmode)
{
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_RD_SRAM_DUMMY_M);   //enable cache read dummy
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_RCMD_M); //enable user mode for cache read command
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
            SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE,
            SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
            SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ_QUAD,
            SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_RDUMMY_CYCLELEN_V, PSRAM_FAST_READ_QUAD_DUMMY + extra_dummy,
            SPI_MEM_SRAM_RDUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy

    switch (psram_cache_mode) {
        case PSRAM_CACHE_S80M:
            psram_clock_set(0, 1);
            break;
        case PSRAM_CACHE_S40M:
            psram_clock_set(0, 2);
            break;
        case PSRAM_CACHE_S26M:
            psram_clock_set(0, 3);
            break;
        case PSRAM_CACHE_S20M:
            psram_clock_set(0, 4);
            break;
        default:
            psram_clock_set(0, 2);
            break;
    }
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_WCMD_M);     // cache write command enable
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_ADDR_BITLEN_V, 23, SPI_MEM_SRAM_ADDR_BITLEN_S); //write address for cache command.
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_SRAM_QIO_M);     //enable qio mode for cache command
    CLEAR_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_SRAM_DIO_M);     //disable dio mode for cache command

    //config sram cache r/w command
    switch (psram_cache_mode) {
        case PSRAM_CACHE_S80M: //in this mode , no delay is needed
            break;
        case PSRAM_CACHE_S40M: //is sram is @40M, need 2 cycles of delay
        case PSRAM_CACHE_S26M:
        case PSRAM_CACHE_S20M:
        default:
#ifdef FAKE_QPI
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 15,
                    SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S); //read command length, 2 bytes(1byte for delay),sending in qio mode in cache
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V, ((PSRAM_FAST_READ_QUAD) << 8),
                    SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b, read command value,(0x00 for delay,0x0b for cmd)
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN, 15,
                    SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S); //write command length,2 bytes(1byte for delay,send in qio mode in cache)
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE, ((PSRAM_QUAD_WRITE) << 8),
                    SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38, write command value,(0x00 for delay)
#else
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
                    SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S); //read command length, 2 bytes(1byte for delay),sending in qio mode in cache
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ_QUAD,
                    SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b, read command value,(0x00 for delay,0x0b for cmd)
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
                    SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S); //write command length,2 bytes(1byte for delay,send in qio mode in cache)
            SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE,
                    SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38, write command value,(0x00 for delay)
#endif
            break;
    }

#if !CONFIG_FREERTOS_UNICORE
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL|DPORT_PRO_DRAM_SPLIT);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL|DPORT_APP_DRAM_SPLIT);
    if (vaddrmode == PSRAM_VADDR_MODE_LOWHIGH) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL);
    } else if (vaddrmode == PSRAM_VADDR_MODE_EVENODD) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_SPLIT);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_SPLIT);
    }
#endif

    Cache_Resume_DCache(0);

    CLEAR_PERI_REG_MASK(SPI_MEM_MISC_REG(0), SPI_MEM_CS1_DIS_M); //ENABLE SPI0 CS1 TO PSRAM(CS0--FLASH; CS1--SRAM)
    if (s_clk_mode == PSRAM_CLK_MODE_NORM) {    //different
        REG_SET_FIELD(SPI_MEM_SRAM_CMD_REG(0), SPI_MEM_SCLK_MODE, 0);
        REG_SET_FIELD(SPI_MEM_CTRL1_REG(1), SPI_MEM_CLK_MODE, 0);

        SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD);
        // Set cs time.
        SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME_V, 1, SPI_MEM_CS_HOLD_TIME_S);
    }
}
#endif // CONFIG_SPIRAM
