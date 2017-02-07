#include "esp_types.h"
#include "rom/ets_sys.h"
#include "psram.h"
//#include "spi.h"
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

static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode);



static void psram_clear_spi_fifo(psram_spi_num_t spiNum)
{
	int i;
	for(i=0;i<16;i++){
		WRITE_PERI_REG(SPI_W0_REG(spiNum)+i*4,0);
	}
}

static void disp_fifo(psram_spi_num_t spiNum)
{
    int i;
    for(i=0;i<16;i++){
        ets_printf(" FIFO[%d]: 0x%08x\n",i, READ_PERI_REG(SPI_W0_REG(spiNum)+i*4));
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
//	psram_set_basic_write_mode(spiNum);
//	psram_set_basic_read_mode(spiNum);
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
void IRAM_ATTR  psram_spi_init(psram_spi_num_t spiNum,psram_cache_mode_t mode)
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
esp_err_t IRAM_ATTR psram_enable(psram_cache_mode_t mode)   //psram init
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
//    /* @param  uint32_t ishspi: 0 for spi, 1 for hspi, flash pad decided by strapping
//    *              else, bit[5:0] spiclk, bit[11:6] spiq, bit[17:12] spid, bit[23:18] spics0, bit[29:24] spihd
//    *
//    * @return None
//    */
//    uint32_t ishspi = ( (6 & 0x3f) << 0)  //clk
//                    | ( (7 & 0x3f) << 6)  //d0
//                    | ( (8 & 0x3f) << 12)  //d1
//                    | ( (11 & 0x3f) << 18) //cs
//                    | ( (9 & 0x3f) << 24); //d2
//    SelectSpiFunction(ishspi);
//    spi_dummy_len_fix(1, 2);
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
    psram_cache_init(mode);
    return ESP_OK;
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_mode_t psram_cache_mode)
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
    CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL1_REG , DPORT_PRO_CACHE_MASK_DRAM1);//use Dram1 to visit ext sram.
    SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CMMU_SRAM_PAGE_MODE, 0, DPORT_PRO_CMMU_SRAM_PAGE_MODE_S); //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(0), SPI_CS1_DIS_M); //ENABLE SPI0 CS1 TO PSRAM(CS0--FLASH; CS1--SRAM)
}

typedef enum {
    SPI_INT_SRC_TRANS_DONE = SPI_TRANS_DONE,
    SPI_INT_SRC_WR_STA_DONE = SPI_SLV_WR_STA_DONE,
    SPI_INT_SRC_RD_STA_DONE = SPI_SLV_RD_STA_DONE,
    SPI_INT_SRC_WR_BUF_DONE = SPI_SLV_WR_BUF_DONE,
    SPI_INT_SRC_RD_BUF_DONE = SPI_SLV_RD_BUF_DONE,
    SPI_INT_SRC_ONE_BUF_RECV_DONE = SPI_IN_SUC_EOF_INT_ENA,
    SPI_INT_SRC_ONE_BUF_SEND_DONE = SPI_OUT_EOF_INT_ENA,
} spi_int_src_t;

/**
 * @brief DMA queue description.
 */
typedef struct {
    uint32_t  block_size: 12;
    uint32_t  data_length: 12;
    uint32_t  unused: 5;
    uint32_t  sub_sof: 1;
    uint32_t  eof: 1;
    uint32_t  owner: 1;
    uint32_t  buf_ptr;
    uint32_t  next_link_ptr;
} dma_queue_t;

/**
 * @brief Initialize DMA and create a SPI DMA instance.
 *
 */
//int spi_dma_init(spi_dma_attr_t *obj, void *isr)
int psram_dma_tx(int dma_channel, uint32_t addr, uint32_t* buf, size_t data_len)
{
    int spi_num = 1;
    // Reset DMA
    SET_PERI_REG_MASK(SPI_DMA_CONF_REG(spi_num), SPI_OUT_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(spi_num), SPI_OUTLINK_START);
    CLEAR_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(spi_num), SPI_INLINK_START);
    CLEAR_PERI_REG_MASK(SPI_DMA_CONF_REG(spi_num), SPI_OUT_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);

    // Select DMA channel.
    SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_channel, ((spi_num - 1) * 2));

    SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MOSI);//////add

    // enable send intr
    SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(spi_num ), SPI_INT_SRC_ONE_BUF_SEND_DONE);
    SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(spi_num ), SPI_INT_SRC_ONE_BUF_RECV_DONE);

    // Clear all of interrupt source
    //spi_int_clear(obj->spi_num);
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spi_num), SPI_INT_SRC_TRANS_DONE
                        | SPI_INT_SRC_WR_STA_DONE
                        | SPI_INT_SRC_RD_STA_DONE
                        | SPI_INT_SRC_WR_BUF_DONE
                        | SPI_INT_SRC_RD_BUF_DONE);


    dma_queue_t* dma_link = (dma_queue_t*) malloc( sizeof(dma_queue_t));
    dma_link->block_size  = data_len;
    dma_link->data_length = data_len;
    dma_link->buf_ptr =  (uint32_t)buf;
    dma_link->eof = 1;
    dma_link->next_link_ptr = (uint32_t)NULL;
    dma_link->owner = 1;//0: cpu  1: dma
    dma_link->sub_sof = 0;
    dma_link->unused = 0;

    SET_PERI_REG_BITS(SPI_DMA_OUT_LINK_REG(spi_num), SPI_OUTLINK_ADDR, ((uint32_t )dma_link),
        SPI_OUTLINK_ADDR_S);
    SET_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(spi_num), SPI_OUTLINK_START);

    // 1. Waiting DMA controller fill TX FIFO
    while ((READ_PERI_REG(SPI_DMA_RSTATUS_REG(spi_num))&0x80000000));
    psram_dma_cmd_write_config(addr, data_len, 0);
    psram_cmd_start(spi_num, PSRAM_CMD_QPI);
    free(dma_link);
    return 0;
}



int psram_dma_rx(int dma_channel, uint32_t addr, uint32_t* buf, size_t data_len)
{
    int spi_num = 1;
    // Reset DMA
    SET_PERI_REG_MASK(SPI_DMA_CONF_REG(spi_num), SPI_OUT_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(spi_num), SPI_OUTLINK_START);
    CLEAR_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(spi_num), SPI_INLINK_START);
    CLEAR_PERI_REG_MASK(SPI_DMA_CONF_REG(spi_num), SPI_OUT_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);

    // Select DMA channel.
    SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, DPORT_SPI3_DMA_CHAN_SEL_V, dma_channel, ((spi_num - 1) * 2));

    SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MISO);//////add

    // enable send intr
    SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(spi_num ), SPI_INT_SRC_ONE_BUF_SEND_DONE);
    SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(spi_num ), SPI_INT_SRC_ONE_BUF_RECV_DONE);

    // Clear all of interrupt source
    //spi_int_clear(obj->spi_num);
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spi_num), SPI_INT_SRC_TRANS_DONE
                        | SPI_INT_SRC_WR_STA_DONE
                        | SPI_INT_SRC_RD_STA_DONE
                        | SPI_INT_SRC_WR_BUF_DONE
                        | SPI_INT_SRC_RD_BUF_DONE);


    dma_queue_t* rx_dma_link = (dma_queue_t*) malloc( sizeof(dma_queue_t));
    rx_dma_link->block_size  = data_len;
    rx_dma_link->data_length = data_len;
    rx_dma_link->buf_ptr =  (uint32_t)buf;
    rx_dma_link->eof = 1;
    rx_dma_link->next_link_ptr = (uint32_t)NULL;
    rx_dma_link->owner = 1;//0: cpu  1: dma
    rx_dma_link->sub_sof = 0;
    rx_dma_link->unused = 0;

    SET_PERI_REG_BITS(SPI_DMA_IN_LINK_REG(spi_num), SPI_INLINK_ADDR, ((uint32_t )rx_dma_link),
        SPI_INLINK_ADDR_S);
    SET_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(spi_num), SPI_INLINK_START);

    psram_dma_qio_read_config( spi_num, addr, data_len);
    psram_cmd_start(spi_num, PSRAM_CMD_QPI);

    //add semaphore to wait trans done, instead of while loop.
    free(rx_dma_link);
    return 0;
}

//---------------------------
//--   below is test code  --
//---------------------------
#if 1
void psram_write_once(uint32_t loop_num,uint32_t write_addr,uint32_t mode,uint32_t repeat)
{
//	psram_enable(PSRAM_CACHE_F80M_S40M);
//	psram_enable_qio_mode(PSRAM_SPI_1);
	uint32_t data_w[8];
	int i;
	for(i=0;i<8;i++) {
		data_w[i]=  ((i+1)<<24)|((i+1)<<16)|((i+1)<<8)|(i+1);
	}
	ets_printf("WRITE DATA IN QMODE\n");
	for(i = 0;i<loop_num;i++){
		psram_write_data(write_addr+32*i,data_w,32);
	}
	vTaskDelay(500);
}

void psram_rw_loop_test(uint32_t loop_num,uint32_t write_addr,uint32_t mode,uint32_t repeat)
{
//	psram_enable(PSRAM_CACHE_F80M_S40M);
//	psram_enable_qio_mode(PSRAM_SPI_1);
	uint32_t data_w[8];
	int i;
	int test_num = 0;
	do{
		ets_printf("-----test num: %d------\n",test_num++);
		ets_printf("---write----0x%08x\n",(test_num%2 == 0? 0x55555555:0xaaaaaaaa));
		for(i=0;i<8;i++) {
			data_w[i] = (test_num%2 == 0? 0x55555555:0xaaaaaaaa);
		}
		for(i = 0;i<loop_num;i++){
			psram_write_data(write_addr+32*i,data_w,32);
		}
		ets_printf("----read----\n");
		int fail_flg = 0;
		uint32_t data_r[8]={0};
		for(i = 0;i<loop_num;i++){
			if((i%1000 == 0)){
				ets_printf("addr [%d]: 0x%08x\n",i,write_addr+32*i);
			}
			memset(data_r,0,sizeof(data_r));
			psram_clear_spi_fifo(PSRAM_SPI_1);
			int dummy_num = 0;
			psram_read_data_quad(PSRAM_SPI_1,data_r,write_addr+32*i,32);
			int k = 0;
			int ii = 0;
			for(k=0;k<8;k++){
				if(data_r[k] != (test_num%2 == 0? 0x55555555:0xaaaaaaaa)){
					ets_printf("ERROR!!! @%d  [0x%08x]\n",k,write_addr+32*i);
					ets_printf("data_r[%d] : 0x%08x\n",k,data_r[k]);
					ets_printf("set val: 0x%08x\n",(test_num%2 == 0? 0x55555555:0xaaaaaaaa));
					uint32_t vtmp = 0x9999;
					psram_read_data(PSRAM_SPI_1,&vtmp,write_addr+32*i+k*4,4);
					ets_printf("spi1 read again11 addr[0x%08x]: 0x%08x\n",write_addr+32*i+k*4,vtmp);
					vtmp = 0x9999;
					psram_read_data(PSRAM_SPI_1,&vtmp,write_addr+32*i+k*4,4);
					ets_printf("spi1 read again22 addr[0x%08x]: 0x%08x\n",write_addr+32*i+k*4,vtmp);
					for(ii = 0;ii<8;ii++){
						//ets_printf("data[%d]: 0x%08x\n",ii,data_r[ii]);
					}
					fail_flg = 1;
				}
			}
		}
		if(fail_flg == 0){
			ets_printf("TEST PASS!!!\n");
			continue;
		}
		psram_clear_spi_fifo(PSRAM_SPI_1);
		break;
	}while(repeat == 0 ? 0 :1 );
	ets_printf("TEST FAILED...\n");
}

void psram_read_test(uint32_t loop_num,uint32_t write_addr,uint32_t mode,uint32_t repeat)
{
	int i;
	int test_num = 0;
	do{
		ets_printf("-----test num: %d------\n",test_num++);
		int fail_flg = 0;
		uint32_t data_r[8]={0};
		for(i = 0;i<loop_num;i++){
			if((i%100 == 0)){
				ets_printf("addr [%d]: 0x%08x\n",i,write_addr+32*i);
			}

			memset(data_r,0,sizeof(data_r));
			psram_clear_spi_fifo(PSRAM_SPI_1);
			int dummy_num = 0;
			psram_read_data(PSRAM_SPI_1,data_r,write_addr+32*i,32);
			int k = 0;
			int ii = 0;

			for(k=0;k<8;k++){
				if(data_r[k] != (((k+1)<<24)|((k+1)<<16)|((k+1)<<8)|(k+1))){
					ets_printf("ERROR!!! @%d\n",k);
					ets_printf("data_r[%d] : 0x%08x\n",k,data_r[k]);
					ets_printf("set val: 0x%08x\n",(((k+1)<<24)|((k+1)<<16)|((k+1)<<8)|(k+1)));

					for(ii = 0;ii<16;ii++){
						ets_printf("data[%d]: 0x%08x\n",ii,data_r[ii]);
					}
					fail_flg = 1;
					//return;
				}
			}
		}
		if(fail_flg == 0){
			ets_printf("TEST PASS!!!\n");
			continue;
		}
		psram_clear_spi_fifo(PSRAM_SPI_1);
		break;
	}while(repeat == 0 ? 0 :1 );
}

void psram_rw_test(uint32_t loop_num,uint32_t write_addr,uint32_t mode,uint32_t repeat)
{
//    psram_enable_qio_mode(PSRAM_SPI_1);
    while(repeat--)
    {
        uint32_t data_w[16];
        int i;
        for(i = 0; i < 16; i++) {
            data_w[i] = ((i + 1) << 24) | ((i + 1) << 16) | ((i + 1) << 8) | (i + 1);
        }
        ets_printf("WRITE DATA IN QMODE\n");
        for(i = 0; i < loop_num; i++) {
            psram_write_data(write_addr + 32 * i, data_w, 32);
        }
//        vTaskDelay(500);
        int fail_flg = 0;
        uint32_t data_r[16] = {0};
        memset(data_r, 0, sizeof(data_r));
        for(i = 0; i < loop_num; i++) {
            if((i % 10 == 0)) {
                ets_printf("addr i: %d\n", i);
            }
            memset(data_r, 0, sizeof(data_r));
            psram_clear_spi_fifo(PSRAM_SPI_1);
            int dummy_num = 0;
            psram_read_data_quad(PSRAM_SPI_1, data_r, write_addr + 32 * i, 32);
            int k = 0;
            int ii = 0;
            for(k = 0; k < 8; k++) {
                if(data_r[k] != (((k + 1) << 24) | ((k + 1) << 16) | ((k + 1) << 8) | (k + 1))) {
                    ets_printf("ERROR!!! @%d\n", k);
                    ets_printf("data_r[%d] : 0x%08x\n", k, data_r[k]);
                    ets_printf("set val: 0x%08x\n", (((k + 1) << 24) | ((k + 1) << 16) | ((k + 1) << 8) | (k + 1)));
                    for(ii = 0; ii < 16; ii++) {
                        ets_printf("data[%d]: 0x%08x\n", ii, data_r[ii]);
                    }
                    fail_flg = 1;
                    return;
                }
            }
        }
        if(fail_flg == 0) {
            ets_printf("TEST PASS!!!\n");
        }
        ets_printf("END OF TEST.\n");
        psram_clear_spi_fifo(PSRAM_SPI_1);
    }

}
#endif

