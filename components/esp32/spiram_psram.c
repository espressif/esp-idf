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

//Commands for PSRAM chip
#define PSRAM_READ              0x03
#define PSRAM_FAST_READ         0x0B
#define PSRAM_FAST_READ_DUMMY   0x3
#define PSRAM_FAST_READ_QUAD    0xEB
#define PSRAM_WRITE             0x02
#define PSRAM_QUAD_WRITE        0x38
#define PSRAM_ENTER_QMODE       0x35
#define PSRAM_EXIT_QMODE        0xF5
#define PSRAM_RESET_EN          0x66
#define PSRAM_RESET             0x99
#define PSRAM_SET_BURST_LEN     0xC0
#define PSRAM_DEVICE_ID         0x9F

#if CONFIG_SPIRAM_TYPE_ESPPSRAM32

#define PSRAM_MFG_ID_M          0xff
#define PSRAM_MFG_ID_S             8
#define PSRAM_MFG_ID_V          0x5d

#endif

// IO-pins for PSRAM. These need to be in the VDD_SIO power domain because all chips we
// currently support are 1.8V parts.
// WARNING: PSRAM shares all but the CS and CLK pins with the flash, so these defines
// hardcode the flash pins as well, making this code incompatible with either a setup
// that has the flash on non-standard pins or ESP32s with built-in flash.
#define FLASH_CLK_IO      6  //Psram clock is a delayed version of this in 40MHz mode
#define FLASH_CS_IO       11
#define PSRAM_CLK_IO      17
#define PSRAM_CS_IO       16
#define PSRAM_SPIQ_IO     7
#define PSRAM_SPID_IO     8
#define PSRAM_SPIWP_IO    10
#define PSRAM_SPIHD_IO    9

#define PSRAM_INTERNAL_IO_28       28
#define PSRAM_INTERNAL_IO_29       29
#define PSRAM_IO_MATRIX_DUMMY_40M   1
#define PSRAM_IO_MATRIX_DUMMY_80M   2

#if CONFIG_FLASHMODE_QIO
#define SPI_CACHE_DUMMY    SPI0_R_QIO_DUMMY_CYCLELEN   //qio 3
#elif CONFIG_FLASHMODE_QOUT
#define SPI_CACHE_DUMMY    SPI0_R_FAST_DUMMY_CYCLELEN  //qout 7
#elif CONFIG_FLASHMODE_DIO
#define SPI_CACHE_DUMMY    SPI0_R_DIO_DUMMY_CYCLELEN   //dio 3
#elif CONFIG_FLASHMODE_DOUT
#define SPI_CACHE_DUMMY    SPI0_R_FAST_DUMMY_CYCLELEN  //dout 7
#endif

static const char* TAG = "psram";
typedef enum {
    PSRAM_SPI_1  = 0x1,
    PSRAM_SPI_2,
    PSRAM_SPI_3,
    PSRAM_SPI_MAX ,
} psram_spi_num_t;

static psram_cache_mode_t s_psram_mode = PSRAM_CACHE_MAX;

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

void psram_cmd_end(int spi_num) {
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
    switch (s_psram_mode) {
        case PSRAM_CACHE_F80M_S80M:
            cmd_exit_qpi = PSRAM_EXIT_QMODE;
            ps_cmd.txDataBitLen = 8;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            cmd_exit_qpi = PSRAM_EXIT_QMODE << 8;
            ps_cmd.txDataBitLen = 16;
            break;
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
    uint32_t addr = (PSRAM_DEVICE_ID << 24) | 0;
    uint32_t dummy_bits = 0;
    psram_cmd_t ps_cmd;
    switch (s_psram_mode) {
        case PSRAM_CACHE_F80M_S80M:
            dummy_bits = 0 + extra_dummy;
            ps_cmd.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            dummy_bits = 0 + extra_dummy;
            ps_cmd.cmdBitLen = 2;   //this two bits is used to delay 2 clock cycle
            break;
    }
    ps_cmd.cmd = 0;
    ps_cmd.addr = &addr;
    ps_cmd.addrBitLen = 4 * 8;
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
    switch (s_psram_mode) {
        case PSRAM_CACHE_F80M_S80M:
            ps_cmd.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            ps_cmd.cmdBitLen = 2;
            break;
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

//spi param init for psram
void IRAM_ATTR psram_spi_init(psram_spi_num_t spi_num, psram_cache_mode_t mode)
{
    uint8_t i, k;
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
    // Set SPI speed for non-80M mode. (80M mode uses APB clock directly.)
    if (mode!=PSRAM_CACHE_F80M_S80M) {
        i = 1;      //Pre-divider
        k = 2;      //Main divider. Divide by 2 so we get 40MHz
         //clear bit 31, set SPI clock div
        CLEAR_PERI_REG_MASK(SPI_CLOCK_REG(spi_num), SPI_CLK_EQU_SYSCLK);
        WRITE_PERI_REG(SPI_CLOCK_REG(spi_num),
                (((i - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
                (((k - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
                ((((k + 1) / 2 - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) | //50% duty cycle
                (((k - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));
    }
    // Enable MOSI
    SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CS_SETUP | SPI_CS_HOLD | SPI_USR_MOSI);
    memset((void*)SPI_W0_REG(spi_num), 0, 16 * 4);
}

static void IRAM_ATTR psram_gpio_config(psram_cache_mode_t mode)
{
    gpio_matrix_out(FLASH_CS_IO, SPICS0_OUT_IDX, 0, 0);
    gpio_matrix_out(PSRAM_SPIQ_IO, SPIQ_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPIQ_IO, SPIQ_IN_IDX, 0);
    gpio_matrix_out(PSRAM_SPID_IO, SPID_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPID_IO, SPID_IN_IDX, 0);
    gpio_matrix_out(PSRAM_SPIWP_IO, SPIWP_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPIWP_IO, SPIWP_IN_IDX, 0);
    gpio_matrix_out(PSRAM_SPIHD_IO, SPIHD_OUT_IDX, 0, 0);
    gpio_matrix_in(PSRAM_SPIHD_IO, SPIHD_IN_IDX, 0);

    switch (mode) {
        case PSRAM_CACHE_F80M_S40M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
            g_rom_spiflash_dummy_len_plus[1] = PSRAM_IO_MATRIX_DUMMY_40M;
            SET_PERI_REG_BITS(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN_V, SPI_CACHE_DUMMY + PSRAM_IO_MATRIX_DUMMY_80M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            //set drive ability for clock
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[PSRAM_CLK_IO], FUN_DRV, 2, FUN_DRV_S);
            break;
        case PSRAM_CACHE_F80M_S80M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_80M;
            g_rom_spiflash_dummy_len_plus[1] = PSRAM_IO_MATRIX_DUMMY_80M;
            SET_PERI_REG_BITS(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN_V, SPI_CACHE_DUMMY + PSRAM_IO_MATRIX_DUMMY_80M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            //set drive ability for clock
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[PSRAM_CLK_IO], FUN_DRV, 3, FUN_DRV_S);
            break;
        case PSRAM_CACHE_F40M_S40M:
            extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
            g_rom_spiflash_dummy_len_plus[1] = PSRAM_IO_MATRIX_DUMMY_40M;
            SET_PERI_REG_BITS(SPI_USER1_REG(0), SPI_USR_DUMMY_CYCLELEN_V, SPI_CACHE_DUMMY + PSRAM_IO_MATRIX_DUMMY_40M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            //set drive ability for clock
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SD_CLK_U, FUN_DRV, 2, FUN_DRV_S);
            SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[PSRAM_CLK_IO], FUN_DRV, 2, FUN_DRV_S);
            break;
        default:
            break;
    }
    SET_PERI_REG_MASK(SPI_USER_REG(0), SPI_USR_DUMMY); // dummy en

    //select pin function gpio
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA2_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, PIN_FUNC_GPIO);
    //flash clock signal should come from IO MUX.
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, FUNC_SD_CLK_SPICLK);
}

//psram gpio init , different working frequency we have different solutions
esp_err_t IRAM_ATTR psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode)   //psram init
{
    uint32_t chip_ver = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG);
    uint32_t pkg_ver = chip_ver & 0x7;
    if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5) {
        ESP_EARLY_LOGE(TAG, "ESP32D2WD do not support psram yet");
        return ESP_FAIL;
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2) {
        ESP_EARLY_LOGE(TAG, "ESP32PICOD2 do not support psram yet");
        return ESP_FAIL;
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4) {
        ESP_EARLY_LOGE(TAG, "ESP32PICOD4  do not support psram yet");
        return ESP_FAIL;
    }

    /*   note: If the third mode(80Mhz+80Mhz) is enabled, VSPI port will be occupied by the system,
         Application code should never touch VSPI hardware in this case.  We try to stop applications
         from doing this using the drivers by claiming the port for ourselves*/
    if (mode == PSRAM_CACHE_F80M_S80M) {
        periph_module_enable(PERIPH_VSPI_MODULE);
        bool r=spicommon_periph_claim(VSPI_HOST);
        if (!r) {
            return ESP_ERR_INVALID_STATE;
        }
    }

    WRITE_PERI_REG(GPIO_ENABLE_W1TC_REG, BIT(PSRAM_CLK_IO) | BIT(PSRAM_CS_IO));   //DISABLE OUPUT FOR IO16/17
    assert(mode < PSRAM_CACHE_MAX && "we don't support any other mode for now.");
    s_psram_mode = mode;

    periph_module_enable(PERIPH_SPI_MODULE);

    WRITE_PERI_REG(SPI_EXT3_REG(0), 0x1);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_PREP_HOLD_M);

    switch (mode) {
        case PSRAM_CACHE_F80M_S80M:
            psram_spi_init(PSRAM_SPI_1, mode);
            CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_CS_HOLD);
            gpio_matrix_out(PSRAM_CS_IO, SPICS1_OUT_IDX, 0, 0);
            gpio_matrix_out(PSRAM_CLK_IO, VSPICLK_OUT_IDX, 0, 0);
            //use spi3 clock,but use spi1 data/cs wires
            //We get a solid 80MHz clock from SPI3 by setting it up, starting a transaction, waiting until it 
            //is in progress, then cutting the clock (but not the reset!) to that peripheral.
            WRITE_PERI_REG(SPI_ADDR_REG(PSRAM_SPI_3), 32 << 24);
            WRITE_PERI_REG(SPI_CLOCK_REG(PSRAM_SPI_3), SPI_CLK_EQU_SYSCLK_M);   //SET 80M AND CLEAR OTHERS
            SET_PERI_REG_MASK(SPI_CMD_REG(PSRAM_SPI_3), SPI_FLASH_READ_M);
            uint32_t spi_status;
            while (1) {
                spi_status = READ_PERI_REG(SPI_EXT2_REG(PSRAM_SPI_3));
                if (spi_status != 0 && spi_status != 1) {
                    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI_CLK_EN_2);
                    break;
                }
            }
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            psram_spi_init(PSRAM_SPI_1, mode);
            CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_CS_HOLD);
            gpio_matrix_out(PSRAM_CS_IO, SPICS1_OUT_IDX, 0, 0);
            /* We need to delay CLK to the PSRAM with respect to the clock signal as output by the SPI peripheral.
            We do this by routing it signal to signal 224/225, which are used as a loopback; the extra run through 
            the GPIO matrix causes the delay. We use GPIO20 (which is not in any package but has pad logic in 
            silicon) as a temporary pad for this. So the signal path is: 
            SPI CLK --> GPIO28 --> signal224(in then out) --> internal GPIO29 --> signal225(in then out) --> GPIO17(PSRAM CLK)
            */
            gpio_matrix_out(PSRAM_INTERNAL_IO_28, SPICLK_OUT_IDX, 0, 0);
            gpio_matrix_in(PSRAM_INTERNAL_IO_28, SIG_IN_FUNC224_IDX, 0);
            gpio_matrix_out(PSRAM_INTERNAL_IO_29, SIG_IN_FUNC224_IDX, 0, 0);
            gpio_matrix_in(PSRAM_INTERNAL_IO_29, SIG_IN_FUNC225_IDX, 0);
            gpio_matrix_out(PSRAM_CLK_IO, SIG_IN_FUNC225_IDX, 0, 0);
            break;
    }
    CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_CS_SETUP_M);
    psram_gpio_config(mode);
    WRITE_PERI_REG(GPIO_ENABLE_W1TS_REG, BIT(PSRAM_CS_IO)| BIT(PSRAM_CLK_IO));
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[PSRAM_CS_IO], PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[PSRAM_CLK_IO], PIN_FUNC_GPIO);
    uint32_t id;
    psram_read_id(&id);
    if (((id >> PSRAM_MFG_ID_S) & PSRAM_MFG_ID_M) != PSRAM_MFG_ID_V) {
        return ESP_FAIL;
    }
    psram_enable_qio_mode(PSRAM_SPI_1);

    psram_cache_init(mode, vaddrmode);
    return ESP_OK;
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode, psram_vaddr_mode_t vaddrmode)
{
    CLEAR_PERI_REG_MASK(SPI_CLOCK_REG(0), SPI_CLK_EQU_SYSCLK_M);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKDIV_PRE_V, 0, SPI_CLKDIV_PRE_S);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKCNT_N, 1, SPI_CLKCNT_N_S);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKCNT_H, 0, SPI_CLKCNT_H_S);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKCNT_L, 1, SPI_CLKCNT_L_S);

    switch (psram_cache_mode) {
        case PSRAM_CACHE_F80M_S80M:
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31));   //flash 1 div clk,80+40;
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0. FLASH DIV 2+SRAM DIV4
            WRITE_PERI_REG(SPI_CLOCK_REG(0), SPI_CLK_EQU_SYSCLK_M);   //SET 1DIV CLOCK AND RESET OTHER PARAMS
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_RD_SRAM_DUMMY_M);   //enable cache read dummy
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_DUMMY + extra_dummy,
                    SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_RCMD_M); //enable user mode for cache read command
            break;
        case PSRAM_CACHE_F80M_S40M:
            SET_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31)); //flash 1 div clk
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0.
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_RD_SRAM_DUMMY_M); //enable cache read dummy
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_DUMMY + extra_dummy,
                    SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_RCMD_M); //enable user mode for cache read command
            break;
        case PSRAM_CACHE_F40M_S40M:
        default:
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31)); //flash 1 div clk
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_RD_SRAM_DUMMY_M); //enable cache read dummy
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_DUMMY + extra_dummy,
                    SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_RCMD_M); //enable user mode for cache read command
            break;
    }
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_WCMD_M);     // cache write command enable
    SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_ADDR_BITLEN_V, 23, SPI_SRAM_ADDR_BITLEN_S); //write address for cache command.
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_SRAM_QIO_M);     //enable qio mode for cache command
    CLEAR_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_SRAM_DIO_M);     //disable dio mode for cache command


    //config sram cache r/w command
    switch (psram_cache_mode) {
        case PSRAM_CACHE_F80M_S80M: //in this mode , no delay is needed
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
                    SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE,
                    SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
                    SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ,
                    SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b
            break;
        case PSRAM_CACHE_F80M_S40M: //is sram is @40M, need 2 cycles of delay
        case PSRAM_CACHE_F40M_S40M:
        default:
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 15,
                    SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S); //read command length, 2 bytes(1byte for delay),sending in qio mode in cache
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, ((PSRAM_FAST_READ) << 8),
                    SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b, read command value,(0x00 for delay,0x0b for cmd)
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 15,
                    SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S); //write command length,2 bytes(1byte for delay,send in qio mode in cache)
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, ((PSRAM_QUAD_WRITE) << 8),
                    SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38, write command value,(0x00 for delay)
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
