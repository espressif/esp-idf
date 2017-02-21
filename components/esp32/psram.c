// Copyright 2013-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_types.h"
#include "rom/ets_sys.h"
#include "esp_psram.h"
#include "soc/io_mux_reg.h"
#include "soc/dport_reg.h"
#include "rom/gpio.h"
#include "soc/gpio_sig_map.h"
#include "esp_attr.h"
#include "rom/cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "string.h"
#include "rom/spi_flash.h"
#include "esp_err.h"

#define PSRAM_READ              0x03
#define PSRAM_FAST_READ         0x0B
#define PSRAM_FAST_READ_QUAD    0xEB
#define PSRAM_WRITE             0x02
#define PSRAM_QUAD_WRITE        0x38
#define PSRAM_ENTER_QMODE       0x35
#define PSRAM_EXIT_QMODE        0xF5
#define PSRAM_RESET_EN          0x66
#define PSRAM_RESET             0x99
#define PSRAM_SET_BURST_LEN     0xC0
#define PSRAM_DEVICE_ID         0x9F

typedef enum {
    PSRAM_SPI_1  = 0x1,
    PSRAM_SPI_2,
    PSRAM_SPI_3,
    PSRAM_SPI_MAX ,
} psram_spi_num_t;

static psram_cache_mode_t g_PsramMode = PSRAM_CACHE_MAX;
extern void Cache_Flush(int);

//For now, we only use F40M + S40M, and we don't have to go through gpio matrix
#define GPIO_MATRIX_FOR_40M   0
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


static void psram_clear_spi_fifo(psram_spi_num_t spiNum)
{
    int i;
    for(i=0;i<16;i++){
        WRITE_PERI_REG(SPI_W0_REG(spiNum)+i*4,0);
    }
}

//set basic SPI write mode
static void psram_set_basic_write_mode(psram_spi_num_t spiNum)
{
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_QIO); //F WRITE QIO
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_DIO); //F WRITE DIO
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_QUAD); //F WRITE QUAD
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_DUAL); //F WRITE DUAL
}
//set QPI write mode
static void psram_set_qio_write_mode(psram_spi_num_t spiNum)
{
    SET_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_QIO); //F WRITE QIO
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_DIO); //F WRITE DIO
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_QUAD); //F WRITE QUAD
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),SPI_FWRITE_DUAL); //F WRITE DUAL
}
//set QPI read mode
static void psram_set_qio_read_mode(psram_spi_num_t spiNum)
{
    SET_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_QIO); //f read qio
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_QUAD); //f read quad
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_DUAL); //f read dual
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_DIO); //f read dio
}
//set SPI read mode
static void psram_set_basic_read_mode(psram_spi_num_t spiNum)
{
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_QIO); //f read qio
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_QUAD); //f read quad
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_DUAL); //f read dual
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum),SPI_FREAD_DIO); //f read dio
}

//start sending and wait for finishing
static IRAM_ATTR void psram_cmd_start(psram_spi_num_t spiNum, psram_cmd_mode_t cmd_mode)
{
    //get cs1
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS1_DIS_M);
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS0_DIS_M);

    uint32_t wr_mode_bkp = (READ_PERI_REG(SPI_USER_REG(spiNum)) >> SPI_FWRITE_DUAL_S) & 0xf;
    uint32_t rd_mode_bkp = READ_PERI_REG(SPI_CTRL_REG(spiNum)) & (SPI_FREAD_DIO_M|SPI_FREAD_DUAL_M|SPI_FREAD_QUAD_M|SPI_FREAD_QIO_M);
    if(cmd_mode == PSRAM_CMD_SPI) {
        psram_set_basic_write_mode(spiNum);
        psram_set_basic_read_mode(spiNum);
    } else if (cmd_mode == PSRAM_CMD_QPI) {
        psram_set_qio_write_mode(spiNum);
        psram_set_qio_read_mode(spiNum);
    }

    //WAIT SPI0 IDLE
    //READ THREE TIMES TO MAKE SURE?
    while( READ_PERI_REG(SPI_EXT2_REG(0))!= 0);
    while( READ_PERI_REG(SPI_EXT2_REG(0))!= 0);
    while( READ_PERI_REG(SPI_EXT2_REG(0))!= 0);
    SET_PERI_REG_MASK( DPORT_HOST_INF_SEL_REG, 1<<14);

    // Start send data
    SET_PERI_REG_MASK(SPI_CMD_REG(spiNum), SPI_USR);
    while ((READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR));

    CLEAR_PERI_REG_MASK( DPORT_HOST_INF_SEL_REG, 1<<14);

    //recover spi mode
    SET_PERI_REG_BITS(SPI_USER_REG(spiNum), 0xf, wr_mode_bkp, SPI_FWRITE_DUAL_S);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum), (SPI_FREAD_DIO_M|SPI_FREAD_DUAL_M|SPI_FREAD_QUAD_M|SPI_FREAD_QIO_M));
    SET_PERI_REG_MASK(SPI_CTRL_REG(spiNum), rd_mode_bkp);

    //return cs to cs0
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1),SPI_CS1_DIS_M);
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1),SPI_CS0_DIS_M);
}

//start sending cmd/addr and receving data
static void IRAM_ATTR psram_recv_start(psram_spi_num_t spiNum,uint32_t* pRxData,uint16_t rxByteLen, psram_cmd_mode_t cmd_mode)
{
    //get cs1
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS1_DIS_M);
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS0_DIS_M);

    uint32_t cmd_mode_bkp = (READ_PERI_REG(SPI_USER_REG(spiNum)) >> SPI_FWRITE_DUAL_S) & 0xf;
    uint32_t rd_mode_bkp = READ_PERI_REG(SPI_CTRL_REG(spiNum)) & (SPI_FREAD_DIO_M|SPI_FREAD_DUAL_M|SPI_FREAD_QUAD_M|SPI_FREAD_QIO_M);
    if(cmd_mode == PSRAM_CMD_SPI) {
        psram_set_basic_write_mode(spiNum);
        psram_set_basic_read_mode(spiNum);
    } else if (cmd_mode == PSRAM_CMD_QPI) {
        psram_set_qio_write_mode(spiNum);
        psram_set_qio_read_mode(spiNum);
    }

    //WAIT SPI0 IDLE
    //READ THREE TIMES TO MAKE SURE?
    while( READ_PERI_REG(SPI_EXT2_REG(0))!= 0);
    while( READ_PERI_REG(SPI_EXT2_REG(0))!= 0);
    while( READ_PERI_REG(SPI_EXT2_REG(0))!= 0);
    SET_PERI_REG_MASK( DPORT_HOST_INF_SEL_REG, 1<<14);

    // Start send data
    SET_PERI_REG_MASK(SPI_CMD_REG(spiNum), SPI_USR);
    while ((READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR));

    CLEAR_PERI_REG_MASK( DPORT_HOST_INF_SEL_REG, 1<<14);

    //recover spi mode
    SET_PERI_REG_BITS(SPI_USER_REG(spiNum), 0xf, cmd_mode_bkp, SPI_FWRITE_DUAL_S);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum), (SPI_FREAD_DIO_M|SPI_FREAD_DUAL_M|SPI_FREAD_QUAD_M|SPI_FREAD_QIO_M));
    SET_PERI_REG_MASK(SPI_CTRL_REG(spiNum), rd_mode_bkp);

    //return cs to cs0
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1),SPI_CS1_DIS_M);
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1),SPI_CS0_DIS_M);

    int idx = 0;
    // Read data out
    do {
        *pRxData++ =  READ_PERI_REG(SPI_W0_REG(spiNum) + (idx << 2));
    } while (++idx < ((rxByteLen / 4) + ((rxByteLen % 4) ? 1 : 0)));
}

//setup spi command/addr/data/dummy in user mode
static int psram_cmd_config(psram_spi_num_t spiNum, psram_cmd_t* pInData)
{
    uint8_t idx = 0;
    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);

    // Set command by user.
    if (pInData->cmdBitLen != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,pInData->cmdBitLen-1, SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);
        // Load command,bit15-0 is cmd value.
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_VALUE, pInData->cmd, SPI_USR_COMMAND_VALUE_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pInData->addrBitLen != 0) {
        SET_PERI_REG_BITS(SPI_USER1_REG(spiNum), SPI_USR_ADDR_BITLEN,(pInData->addrBitLen- 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_ADDR);
        // Set address
        //SET_PERI_REG_BITS(SPI_ADDR_REG(spiNum), SPI_USR_ADDR_VALUE, *pInData->addr, SPI_USR_ADDR_VALUE_S);
        WRITE_PERI_REG(SPI_ADDR_REG(spiNum), *pInData->addr);

    } else{
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1_REG(spiNum), SPI_USR_ADDR_BITLEN,0, SPI_USR_ADDR_BITLEN_S);
    }
    // Set data by user.
    uint32_t* pTxVal = pInData->txData;
    if (pInData->txDataBitLen != 0 ) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
        // Load send buffer
        int len = ((pInData->txDataBitLen / 32) + ((pInData->txDataBitLen % 32) ? 1 : 0));
        if(pTxVal != NULL) {
            do {
                WRITE_PERI_REG((SPI_W0_REG(spiNum) + (idx << 2)), *pTxVal++);
            } while(++idx < len);
        }
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spiNum), SPI_USR_MOSI_DBITLEN, (pInData->txDataBitLen - 1), SPI_USR_MOSI_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spiNum), SPI_USR_MOSI_DBITLEN,0, SPI_USR_MOSI_DBITLEN_S);
    }
    // Set rx data by user.
    if (pInData->rxDataBitLen != 0  ) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spiNum), SPI_USR_MISO_DBITLEN, (pInData->rxDataBitLen -1  ), SPI_USR_MISO_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spiNum), SPI_USR_MISO_DBITLEN, 0, SPI_USR_MISO_DBITLEN_S);
    }
    if(pInData->dummyBitLen != 0){
        SET_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1),SPI_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_USER1_REG(PSRAM_SPI_1),SPI_USR_DUMMY_CYCLELEN_V,pInData->dummyBitLen-1,SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
    }else{
        CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1),SPI_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_USER1_REG(PSRAM_SPI_1),SPI_USR_DUMMY_CYCLELEN_V,0,SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
    }
    return 0;
}

//read psram data in fast read mode
static void psram_read_data(psram_spi_num_t spiNum,uint32_t* dst,uint32_t src,uint32_t len)
{
    uint32_t addr = 0;
    uint32_t dummy_bits = 0;
    psram_cmd_t pDat;
    addr = (PSRAM_FAST_READ <<24) | src;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            dummy_bits = 4+extra_dummy;
            pDat.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            dummy_bits = 4+extra_dummy;
            pDat.cmdBitLen = 2;
            break;
    }
    pDat.cmd = 0;
    pDat.addr = &addr;
    pDat.addrBitLen = 4*8;
    pDat.txDataBitLen = 0;
    pDat.txData = NULL;
    pDat.rxDataBitLen = len*8 ;
    pDat.rxData = dst;
    pDat.dummyBitLen = dummy_bits;
    psram_cmd_config(spiNum,&pDat);
    psram_clear_spi_fifo(spiNum);
    psram_recv_start(spiNum,pDat.rxData,pDat.rxDataBitLen/8, PSRAM_CMD_QPI);
}

//read psram data in fast read quad mode
static void psram_read_data_quad(psram_spi_num_t spiNum,uint32_t* dst,uint32_t src,uint32_t len)
{
    uint32_t addr = (PSRAM_FAST_READ_QUAD <<24) | src;
    uint32_t dummy_bits = 0;
    psram_cmd_t pDat;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            dummy_bits = 6+extra_dummy;
            pDat.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            dummy_bits = 6+extra_dummy;
            pDat.cmdBitLen = 2;
            break;
    }
    pDat.cmd = 0;
    pDat.addr = &addr;
    pDat.addrBitLen = 4*8;
    pDat.txDataBitLen = 0;
    pDat.txData = NULL;
    pDat.rxDataBitLen = len*8 ;
    pDat.rxData = dst;
    pDat.dummyBitLen = dummy_bits;
    psram_cmd_config(spiNum,&pDat);
    psram_clear_spi_fifo(spiNum);
    psram_recv_start(spiNum,pDat.rxData,pDat.rxDataBitLen/8, PSRAM_CMD_QPI);
}

//write data to psram
static void psram_write_data(uint32_t dst,uint32_t* src,uint32_t len)
{
    uint32_t addr = (PSRAM_QUAD_WRITE <<24) | dst;
    psram_cmd_t pDat;
    int dummy_bits = 0;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            dummy_bits = 0 + 0;
            pDat.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            dummy_bits = 0 + 0;
            pDat.cmdBitLen = 2;
            break;
    }
    pDat.cmd = 0;
    pDat.addr = &addr;
    pDat.addrBitLen = 32;
    pDat.txData = src;
    pDat.txDataBitLen = len*8;
    pDat.rxData = NULL;
    pDat.rxDataBitLen = 0;
    pDat.dummyBitLen = dummy_bits;
    psram_cmd_config(PSRAM_SPI_1, &pDat);
    psram_cmd_start(PSRAM_SPI_1, PSRAM_CMD_QPI);
}

static void psram_dma_cmd_write_config(uint32_t dst, uint32_t len, uint32_t dummy_bits)
{
    uint32_t addr = (PSRAM_QUAD_WRITE << 24) | dst;
    psram_cmd_t pDat;
    switch(g_PsramMode) {
        case PSRAM_CACHE_F80M_S80M:
            pDat.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            pDat.cmdBitLen = 2;
            break;
    }
    pDat.cmd = 0;
    pDat.addr = &addr;
    pDat.addrBitLen = 32;
    pDat.txData = NULL;
    pDat.txDataBitLen = len * 8;
    pDat.rxData = NULL;
    pDat.rxDataBitLen = 0;
    pDat.dummyBitLen = dummy_bits;
    psram_cmd_config(PSRAM_SPI_1, &pDat);
}

static void psram_dma_qio_read_config(psram_spi_num_t spiNum, uint32_t src, uint32_t len)
{
    uint32_t addr = (PSRAM_FAST_READ_QUAD <<24) | src;
    uint32_t dummy_bits = 0;
    psram_cmd_t pDat;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            dummy_bits = 6+extra_dummy;
            pDat.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            dummy_bits = 6+extra_dummy;
            pDat.cmdBitLen = 2;
            break;
    }
    pDat.cmd = 0;
    pDat.addr = &addr;
    pDat.addrBitLen = 4*8;
    pDat.txDataBitLen = 0;
    pDat.txData = NULL;
    pDat.rxDataBitLen = len*8 ;
    pDat.rxData = NULL;
    pDat.dummyBitLen = dummy_bits;
    psram_cmd_config(spiNum,&pDat);
//    psram_clear_spi_fifo(spiNum);
}

//read psram id
static void psram_read_id(uint32_t* dev_id)
{
    psram_spi_num_t spiNum = PSRAM_SPI_1;
//  psram_set_basic_write_mode(spiNum);
//  psram_set_basic_read_mode(spiNum);
    uint32_t addr = (PSRAM_DEVICE_ID <<24) | 0;
    uint32_t dummy_bits = 0;
    psram_cmd_t pDat;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            dummy_bits = 0+extra_dummy;
            pDat.cmdBitLen = 0;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            dummy_bits = 0+extra_dummy;
            pDat.cmdBitLen = 2;   //this two bits is used for delay one byte in qio mode
            break;
    }
    pDat.cmd = 0;
    pDat.addr = &addr;
    pDat.addrBitLen = 4*8;
    pDat.txDataBitLen = 0;
    pDat.txData = NULL;
    pDat.rxDataBitLen = 4*8 ;
    pDat.rxData = dev_id;
    pDat.dummyBitLen = dummy_bits;
    psram_cmd_config(spiNum,&pDat);
    psram_clear_spi_fifo(spiNum);
    psram_recv_start(spiNum,pDat.rxData,pDat.rxDataBitLen/8, PSRAM_CMD_SPI);
}

//switch psram burst length(32 bytes or 1024 bytes)
//datasheet says it should be 1024 bytes by default
//but they sent us a correction doc and told us it is 32 bytes for these samples
static void psram_set_burst_length(psram_spi_num_t spiNum)
{
    psram_cmd_t pDat;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            pDat.cmd = 0xC0;
            pDat.cmdBitLen = 8;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            pDat.cmd = 0x0030;
            pDat.cmdBitLen = 10;
            break;
    }
    pDat.addr = 0;
    pDat.addrBitLen = 0;
    pDat.txData = NULL;
    pDat.txDataBitLen = 0;
    pDat.rxData = NULL;
    pDat.rxDataBitLen = 0;
    pDat.dummyBitLen = 0;
    psram_cmd_config(spiNum, &pDat);
    psram_cmd_start(spiNum, PSRAM_CMD_QPI);
}

//send reset command to psram(right now,we only send this command in QPI mode)
//seems not working
static void psram_reset_mode(psram_spi_num_t spiNum)
{
    psram_cmd_t pDat;
    uint32_t cmd_rst = 0x99066;
    pDat.txData = &cmd_rst;
    pDat.txDataBitLen = 20;
    pDat.addr = NULL;
    pDat.addrBitLen = 0;
    pDat.cmd = 0;
    pDat.cmdBitLen = 0;
    pDat.rxData = NULL;
    pDat.rxDataBitLen = 0;
    pDat.dummyBitLen = 0;
    psram_cmd_config(spiNum, &pDat);
    psram_cmd_start(spiNum, PSRAM_CMD_QPI);
}
//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(psram_spi_num_t spiNum)
{
    psram_cmd_t pDat;
    uint32_t cmd_exit_qpi;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            cmd_exit_qpi = PSRAM_EXIT_QMODE;
            pDat.txDataBitLen = 8;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            cmd_exit_qpi = PSRAM_EXIT_QMODE<<8;
            pDat.txDataBitLen = 16;
            break;
    }
    pDat.txData = &cmd_exit_qpi;
    pDat.cmd = 0;
    pDat.cmdBitLen = 0;
    pDat.addr = 0;
    pDat.addrBitLen = 0;
    pDat.rxData = NULL;
    pDat.rxDataBitLen = 0;
    pDat.dummyBitLen = 0;
    psram_cmd_config(spiNum, &pDat);
    psram_cmd_start(spiNum, PSRAM_CMD_QPI);
}
//enter QPI mode
static void IRAM_ATTR psram_enable_qio_mode(psram_spi_num_t spiNum)
{
    psram_cmd_t pDat;
    switch(g_PsramMode){
        case PSRAM_CACHE_F80M_S80M:
            pDat.cmd = PSRAM_ENTER_QMODE;
            pDat.cmdBitLen = 8;
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            pDat.cmd = 0x400d;
            pDat.cmdBitLen = 10;
            break;
    }
    pDat.addr = 0;
    pDat.addrBitLen = 0;
    pDat.txData = NULL;
    pDat.txDataBitLen = 0;
    pDat.rxData = NULL;
    pDat.rxDataBitLen = 0;
    pDat.dummyBitLen = 0;
    psram_cmd_config(spiNum, &pDat);
    psram_cmd_start(spiNum, PSRAM_CMD_SPI);
}


static void IRAM_ATTR psram_gpio_config(psram_cache_mode_t mode)
{
    gpio_matrix_out(6, SPICLK_OUT_IDX, 0, 0);
    gpio_matrix_out(11, SPICS0_OUT_IDX, 0, 0);

    gpio_matrix_out(7, SPIQ_OUT_IDX, 0, 0);
    gpio_matrix_in(7,SPIQ_IN_IDX, 0);
    gpio_matrix_out(8, SPID_OUT_IDX, 0, 0);
    gpio_matrix_in(8, SPID_IN_IDX, 0);
    gpio_matrix_out(10, SPIWP_OUT_IDX, 0, 0);
    gpio_matrix_in(10, SPIWP_IN_IDX, 0);
    gpio_matrix_out(9, SPIHD_OUT_IDX, 0, 0);
    gpio_matrix_in(9, SPIHD_IN_IDX, 0);

    switch(mode){
        case PSRAM_CACHE_F80M_S80M:
        case PSRAM_CACHE_F80M_S40M:
            SET_PERI_REG_MASK(SPI_USER_REG(0),SPI_USR_DUMMY); // dummy en
            SET_PERI_REG_BITS(SPI_USER1_REG(0),SPI_USR_DUMMY_CYCLELEN_V,3+extra_dummy,SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            break;
        case PSRAM_CACHE_F40M_S40M:
        default:
            SET_PERI_REG_MASK(SPI_USER_REG(0),SPI_USR_DUMMY); // dummy en
            SET_PERI_REG_BITS(SPI_USER1_REG(0),SPI_USR_DUMMY_CYCLELEN_V,3+extra_dummy,SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
            break;
    }
    //drive ability
    SET_PERI_REG_BITS( PERIPHS_IO_MUX_SD_CLK_U,  FUN_DRV, 3 ,FUN_DRV_S);
    SET_PERI_REG_BITS( PERIPHS_IO_MUX_SD_DATA0_U,FUN_DRV, 3 ,FUN_DRV_S);
    SET_PERI_REG_BITS( PERIPHS_IO_MUX_SD_DATA1_U,FUN_DRV, 3 ,FUN_DRV_S);
    SET_PERI_REG_BITS( PERIPHS_IO_MUX_SD_DATA2_U,FUN_DRV, 3 ,FUN_DRV_S);
    SET_PERI_REG_BITS( PERIPHS_IO_MUX_SD_DATA3_U,FUN_DRV, 3 ,FUN_DRV_S);
    SET_PERI_REG_BITS( PERIPHS_IO_MUX_SD_CMD_U,  FUN_DRV, 3 ,FUN_DRV_S);
    //select pin function gpio
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U,2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U,2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA2_U,2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U,2);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U,2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U,2);
}


//spi param init for psram
void IRAM_ATTR psram_spi_init(psram_spi_num_t spiNum,psram_cache_mode_t mode)
{
    uint8_t i, k;
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_TRANS_DONE << 5);
    SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_CS_SETUP);
    // SPI_CPOL & SPI_CPHA
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(spiNum), SPI_CK_IDLE_EDGE);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),  SPI_CK_OUT_EDGE);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum), SPI_WR_BIT_ORDER);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum), SPI_RD_BIT_ORDER);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_DOUTDIN);
    // May be not must to do.
    WRITE_PERI_REG(SPI_USER1_REG(spiNum), 0);
    // SPI mode type
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_SLAVE_MODE);
    switch(mode){
        case PSRAM_CACHE_F80M_S80M:
            WRITE_PERI_REG(SPI_CLOCK_REG(spiNum), SPI_CLK_EQU_SYSCLK); // 80Mhz speed
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            i = (2 / 40) ? (2 / 40) : 1;
            k = 2 / i;
            CLEAR_PERI_REG_MASK(SPI_CLOCK_REG(spiNum), SPI_CLK_EQU_SYSCLK);
            WRITE_PERI_REG(SPI_CLOCK_REG(spiNum),
                    (((i - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
                    (((k - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
                    ((((k + 1) / 2 - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
                    (((k - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div
            break;
    }
    // Enable MOSI
    SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_CS_SETUP | SPI_CS_HOLD | SPI_USR_MOSI);
    for (i = 0; i < 16; ++i) {
        WRITE_PERI_REG((SPI_W0_REG(spiNum) + (i << 2)), 0);
    }
}


//psram gpio init , different working frequency we have different solutions
esp_err_t IRAM_ATTR psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode)   //psram init
{
    WRITE_PERI_REG(GPIO_ENABLE_W1TC_REG,BIT16|BIT17);//DISALBE OUPUT FOR IO16/17

    g_PsramMode = mode;

    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG,BIT16);//DPORT_SPI_CLK_EN
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,BIT16);//DPORT_SPI_RST
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG,BIT1);//DPORT_SPI_CLK_EN_1
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,BIT1);//DPORT_SPI_RST_1
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG,BIT6);//DPORT_SPI_CLK_EN_2
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,BIT6);//DPORT_SPI_RST_2

    WRITE_PERI_REG( SPI_EXT3_REG(0), 0x1);
    CLEAR_PERI_REG_MASK( SPI_USER_REG(PSRAM_SPI_1), SPI_USR_PREP_HOLD_M);


    switch(mode){
        case PSRAM_CACHE_F80M_S80M:
            psram_spi_init(PSRAM_SPI_1, mode);
            extra_dummy = 2;
            CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_CS_HOLD);
            gpio_matrix_out(16, SPICS1_OUT_IDX, 0, 0);
            gpio_matrix_out(17, VSPICLK_OUT_IDX, 0, 0);
            //use spi3 clock,but use spi1 data/cs wires
            WRITE_PERI_REG(SPI_ADDR_REG(PSRAM_SPI_3), 32<<24);
            WRITE_PERI_REG(SPI_CLOCK_REG(PSRAM_SPI_3),SPI_CLK_EQU_SYSCLK_M);//SET 80M AND CLEAR OTHERS
            SET_PERI_REG_MASK(SPI_CMD_REG(PSRAM_SPI_3),SPI_FLASH_READ_M);
            uint32_t spi_status;
            while(1){
                spi_status = READ_PERI_REG(SPI_EXT2_REG(PSRAM_SPI_3));
                if(spi_status != 0 && spi_status != 1){
                    CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG,BIT16);//DPORT_SPI_CLK_EN
                    break;
                }
            }
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
#if GPIO_MATRIX_FOR_40M
            extra_dummy = 1;
#else
            extra_dummy = 0;
#endif
            psram_spi_init(PSRAM_SPI_1, mode);
            CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_CS_HOLD);
            gpio_matrix_out(16, SPICS1_OUT_IDX, 0, 0);
            gpio_matrix_in(6,SIG_IN_FUNC224_IDX,0);
            gpio_matrix_out(20,SIG_IN_FUNC224_IDX,0,0);
            gpio_matrix_in(20,SIG_IN_FUNC225_IDX,0);
            gpio_matrix_out(17,SIG_IN_FUNC225_IDX,0,0);
            break;
    }
    CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1),SPI_CS_SETUP_M);

#if GPIO_MATRIX_FOR_40M
    psram_gpio_config(mode);
#endif


    WRITE_PERI_REG(GPIO_ENABLE_W1TS_REG,BIT16|BIT17);//GPIO_Pin_16 | GPIO_Pin_17
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO16_U,2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO17_U,2);

    uint32_t id;
    psram_read_id(&id);
    if(((id >> 8) & 0xff )!= 0x5d) {
        return ESP_FAIL;
    }
    psram_enable_qio_mode(PSRAM_SPI_1);
    psram_cache_init(mode, vaddrmode);
    return ESP_OK;
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode, psram_vaddr_mode_t vaddrmode)
{
    CLEAR_PERI_REG_MASK(SPI_CLOCK_REG(0),SPI_CLK_EQU_SYSCLK_M);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0),SPI_CLKDIV_PRE_V,0,SPI_CLKDIV_PRE_S);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0),SPI_CLKCNT_N,1,SPI_CLKCNT_N_S);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0),SPI_CLKCNT_H,0,SPI_CLKCNT_H_S);
    SET_PERI_REG_BITS(SPI_CLOCK_REG(0),SPI_CLKCNT_L,1,SPI_CLKCNT_L_S);

    switch(psram_cache_mode){
        case PSRAM_CACHE_F80M_S80M:
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0),BIT(31));//flash 1 div clk,80+40;
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0),BIT(30));//pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0. FLASH DIV 2+SRAM DIV4
            WRITE_PERI_REG(SPI_CLOCK_REG(0),SPI_CLK_EQU_SYSCLK_M);//SET 1DIV CLOCK AND RESET OTHER PARAMS
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_USR_RD_SRAM_DUMMY_M);//enable cache read dummy
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0),SPI_SRAM_DUMMY_CYCLELEN_V,3+extra_dummy,SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_CACHE_SRAM_USR_RCMD_M);//enable user mode for cache read command
            break;
        case PSRAM_CACHE_F80M_S40M:
            SET_PERI_REG_MASK(SPI_DATE_REG(0),BIT(31));//flash 1 div clk
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0),BIT(30));//pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0.
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_USR_RD_SRAM_DUMMY_M);//enable cache read dummy
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0),SPI_SRAM_DUMMY_CYCLELEN_V,3+extra_dummy,SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_CACHE_SRAM_USR_RCMD_M);//enable user mode for cache read command
            break;
        case PSRAM_CACHE_F40M_S40M:
        default:
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0),BIT(31));//flash 1 div clk
            CLEAR_PERI_REG_MASK(SPI_DATE_REG(0),BIT(30));//pre clk div
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_USR_RD_SRAM_DUMMY_M);//enable cache read dummy
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0),SPI_SRAM_DUMMY_CYCLELEN_V,3+extra_dummy,SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy
            SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_CACHE_SRAM_USR_RCMD_M);//enable user mode for cache read command
            break;
    }
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_CACHE_SRAM_USR_WCMD_M);     // cache write command enable
    SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0),SPI_SRAM_ADDR_BITLEN_V,23,SPI_SRAM_ADDR_BITLEN_S);//write address for cache command.
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_USR_SRAM_QIO_M);//enable qio mode for cache command
    CLEAR_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0),SPI_USR_SRAM_DIO_M);//disable dio mode for cache command

    //config sram cache r/w command
    switch(psram_cache_mode) {
        case PSRAM_CACHE_F80M_S80M: //in this mode , no delay is needed
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
                SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, 0x38,
                SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
                SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, 0x0b,
                SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b
            break;
        case PSRAM_CACHE_F80M_S40M: //is sram is @40M, need 2 cycles of delay
        case PSRAM_CACHE_F40M_S40M:
            default:
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 15,
                SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S); //read command length, 2 bytes(1byte for delay),sending in qio mode in cache
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, 0x0b00,
                SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b, read command value,(0x00 for delay,0x0b for cmd)
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 15,
                SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S); //write command length,2 bytes(1byte for delay,send in qio mode in cache)
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, 0x3800,
                SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38, write command value,(0x00 for delay)
            break;
    }


    CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG , DPORT_PRO_DRAM_HL|DPORT_PRO_DRAM_SPLIT);
    CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG , DPORT_APP_DRAM_HL|DPORT_APP_DRAM_SPLIT);
    if (vaddrmode == PSRAM_VADDR_MODE_LOWHIGH) {
        SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG , DPORT_PRO_DRAM_SPLIT);
        SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG , DPORT_APP_DRAM_SPLIT);
    } else if (vaddrmode == PSRAM_VADDR_MODE_EVENODD) {
        SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG , DPORT_PRO_DRAM_HL);
        SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG , DPORT_APP_DRAM_HL);
    }

    CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL1_REG , DPORT_PRO_CACHE_MASK_DRAM1|DPORT_PRO_CACHE_MASK_OPSDRAM);//use Dram1 to visit ext sram.
    SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CMMU_SRAM_PAGE_MODE, 0, DPORT_PRO_CMMU_SRAM_PAGE_MODE_S); //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
 
    CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG , DPORT_APP_CACHE_MASK_DRAM1|DPORT_APP_CACHE_MASK_OPSDRAM);//use Dram1 to visit ext sram.
    SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CMMU_SRAM_PAGE_MODE, 0, DPORT_APP_CMMU_SRAM_PAGE_MODE_S); //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
 
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(0), SPI_CS1_DIS_M); //ENABLE SPI0 CS1 TO PSRAM(CS0--FLASH; CS1--SRAM)
}

