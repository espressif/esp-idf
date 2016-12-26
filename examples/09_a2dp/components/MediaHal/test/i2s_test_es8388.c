/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "driver/i2s.h"
#include "driver/gpio.h"

#include "stdarg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <stdio.h>
#include <string.h>
#include "audio_log.h"
#include "ES8388.h"
#include "i2c_soft.h"
//#if 0

// I2STest: 1 -> I2S moudle test slef,Tx->Rx
// I2STest: 2 -> I2S moudle read wave data to decoder(WM8960/WM8978)
// I2STest: others -> Reverse.
#define I2STest         2
#define I2SBUFSZ (81920)
char *I2sBuf=(char*)0x3ffa8000; //0x3FFBC000
char *bufWrPos;
char *bufRdPos;


static i2s_dma_attr_t i2sDmaTxObj, i2sDmaRxObj;
static uint32_t recvCnt =0;
static uint32_t setBufLen = (1024*3); // buffer length bytes

extern uint8_t musicTable[];
extern uint32_t tabSize;

static uint32_t loadSize = 0;
static uint32_t initVlaue = 0;

SemaphoreHandle_t xSemPlayEnd;
xQueueHandle uartQueue;
extern SemaphoreHandle_t xSemUartCmd; 

extern void __ShowI2S_REGValue();
#define SHOWI2SREG()    __ShowI2S_REGValue()


int BufFill()
{
    int fill = bufWrPos - bufRdPos;
    if (fill < 0) fill += I2SBUFSZ;
    return fill;
}

void BufRead(char *dest, int n) {

    while (n--) {
		*dest++ = *bufRdPos++;
		if (bufRdPos==I2sBuf+I2SBUFSZ) {
            bufRdPos=I2sBuf;
        }      
	}
}

void BufWrite(char *src, int n) {
	while (n--) {
		*bufWrPos++ = initVlaue++;//*src++;
		if (bufWrPos==I2sBuf+I2SBUFSZ) {
            bufWrPos=I2sBuf;
        }           
	}
}
int32_t flag =0;


// @brief Fill test data to buffer
static void set_buffer2Zero(uint32_t *buffer, uint32_t len)
{
    uint32_t i;
    uint8_t *pbuff = (uint8_t *)buffer;
    for (i = 0; i < (len); i++) {
       *pbuff++ = 0;
    }
}


// @brief Read receive ping-pong buffer to buffer
void readToRecvBuf_es8388(uint32_t *buffer, uint32_t len)
{
    uint32_t i;
    uint8_t *pbuff = (uint8_t*)buffer;
    BaseType_t xHigherPriorityTaskWoken;
    printf("buffer=%x,recvCnt=%d, bufWrPos=%x\r\n", (uint32_t)buffer,recvCnt, (uint32_t)bufWrPos);

	  
    for (i = 0; i < len ; i++) {
        *bufWrPos++ = *pbuff++;
       //printf("%08x",*buffer++);
        recvCnt++;
     	if (recvCnt > (I2SBUFSZ)) {            
            printf("receive buffer,full, bufWrPos=%x recvCnt=%d\r\n",  (uint32_t)bufWrPos, recvCnt);
            bufWrPos=I2sBuf;
            recvCnt = 0;
            i2s_stop(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_RX);
            xSemaphoreGiveFromISR(xSemPlayEnd, &xHigherPriorityTaskWoken); 
        }
    }
}

// @brief Configrate the I2S pin.
void i2s_pin_init(i2s_mode_t mode)
{
    if (mode == I2S_MODE_MASTER_TX) {   
        // MCLK
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1); //);
        SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, 0, CLK_OUT1_S);

        gpio_matrix_out(25, I2S0O_WS_OUT_IDX, 0, 0);
        gpio_matrix_out(26, I2S0O_BCK_OUT_IDX, 0, 0);
        gpio_matrix_out(22, I2S0O_DATA_OUT23_IDX, 0, 0);

        ets_printf("I2SPinInit Master_Tx \r\n");

    } else if (mode == I2S_MODE_SLAVE_TX) {

        // signal select
        gpio_matrix_in(21, I2S0I_WS_IN_IDX, 0);
        gpio_matrix_in(22, I2S0I_BCK_IN_IDX, 0);
        gpio_matrix_out(23, I2S0O_DATA_OUT23_IDX, 0, 0);
        // MCLK
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        // Select gpio function
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_GPIO21);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_GPIO22);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO23_U, FUNC_GPIO23_GPIO23);
        // pull up or pull down

        SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO21_U, FUN_IE | FUN_PU);
        SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO22_U, FUN_IE | FUN_PU);
        PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO23_U);
    } else if (mode == I2S_MODE_MASTER_RX) {
        // signal select
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1); //);
        SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, 0, CLK_OUT1_S);

        gpio_matrix_out(26, I2S0O_WS_OUT_IDX, 0, 0);
        gpio_matrix_out(25, I2S0O_BCK_OUT_IDX, 0, 0);
        gpio_matrix_in(22, I2S0I_DATA_IN15_IDX, 0);
    
    } else if (mode == I2S_MODE_SLAVE_RX) {
        // signal select
        gpio_matrix_in(16, I2S0I_WS_IN_IDX, 0);
        gpio_matrix_in(17, I2S0I_BCK_IN_IDX, 0);
        gpio_matrix_in(18, I2S0I_DATA_IN15_IDX, 0);
        // Select gpio function
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO16_U, FUNC_GPIO16_GPIO16);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO17_U, FUNC_GPIO17_GPIO17);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO18_U, FUNC_GPIO18_GPIO18);
        // pull up or pull down

        SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO16_U, FUN_IE | FUN_PU);
        SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO17_U, FUN_IE | FUN_PU);
        SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO18_U, FUN_IE | FUN_PU);
    }
}

// @brief Fill test data to buffer
void loadMusicData_es8388(uint32_t *buffer, uint32_t len)
{
    uint32_t i;
    uint8_t *pbuff = (uint8_t*)buffer;
    BaseType_t xHigherPriorityTaskWoken;
    printf("bufAddr=%x, bufRdPos=%x\r\n", (uint32_t)pbuff, (uint32_t)bufRdPos);
    for (i = 0; i < len; i++) {
        *pbuff++ = *bufRdPos++;
        loadSize++;
        if (loadSize>tabSize) {            
            printf("Music play finished. musicTable=%x bufRdPos=%x,loadSize=%x\r\n", (uint32_t)musicTable, 
                (uint32_t)bufRdPos, loadSize);
            bufRdPos= (char*)musicTable;
            loadSize = 0;
            i2s_stop(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_TX);                       
            xSemaphoreGiveFromISR(xSemPlayEnd, &xHigherPriorityTaskWoken); 
        }
    }
}
// @brief I2S interrupt service routine.
void I2SDmaIsr_es8388(void *para)
{
    portBASE_TYPE HPTaskAwoken=0;
	int dummy;
    uint32_t intrStatus = i2s_int_status_get(I2S_NUM_I2S0);    
    i2s_int_clear(I2S_NUM_I2S0, 0xffffffff);
    printf("intrStatus12=%x\r\n",intrStatus); 
    if (0 == intrStatus) {
        //No interested interrupts pending
        return;
    }
    //clear all intrs
    //process every intr
    if (intrStatus & I2S_INT_SRC_ONE_BUF_SEND_DONE) {
        ////find the DMA which sends the interrupt signal
        uint32_t *bufAddr = NULL;
        bufAddr  = i2s_dma_status_get(I2S_NUM_I2S0, &i2sDmaTxObj);        
        if (NULL == bufAddr) {
           printf("bufAddr is null\r\n"); 
        }
        loadMusicData_es8388(bufAddr, setBufLen);
    }

    //Receiver data record
    if (intrStatus & I2S_INT_SRC_ONE_BUF_RECV_DONE) {
        uint32_t *recvBuf = NULL;
       // recvBuf = i2s_dma_status_get(I2S_NUM_I2S0, &i2sDmaRxObj);
       // readToRecvBuf_es8388(recvBuf, setBufLen);
    }
}

void i2sDmaBufInit_es8388()
{   
    int dmaRegt;
    // Tx DMA buffer init
    memset(&i2sDmaTxObj, 0, sizeof(i2sDmaTxObj));
    i2sDmaTxObj.buf = NULL;
    i2sDmaTxObj.buf_size = setBufLen;
    i2sDmaTxObj.mode = I2S_MODE_MASTER_TX;
    dmaRegt = i2s_dma_init(I2S_NUM_I2S0, &i2sDmaTxObj, I2SDmaIsr_es8388);
    if (dmaRegt) {
        LOGE("I2SDmaInit init failed ret=%d", dmaRegt);
        return;
    }
    LOGD("i2sDmaTx buf=%x dataPing=%x pongAddr=%x\r\n", i2sDmaTxObj.buf, 
        i2s_dma_ping_buf_get(&i2sDmaTxObj), i2s_dma_pong_buf_get(&i2sDmaTxObj));
    i2s_dma_dest_add_set(I2S_NUM_I2S0, &i2sDmaTxObj);
    i2s_dma_enable(I2S_NUM_I2S0, I2S_MODE_MASTER_TX);

    // Rx DMA buffer init
    memset(&i2sDmaRxObj, 0, sizeof(i2sDmaRxObj));
    i2sDmaRxObj.buf = NULL;
    i2sDmaRxObj.buf_size = setBufLen;
    i2sDmaRxObj.mode = I2S_MODE_MASTER_RX;
    dmaRegt = i2s_dma_init(I2S_NUM_I2S0, &i2sDmaRxObj, I2SDmaIsr_es8388);
    if (dmaRegt) {
        LOGE("I2SDmaInit init failed ret=%d", dmaRegt);
        return;
    }
    LOGD("i2sDmaRx buf=%x dataPing=%x pongAddr=%x\r\n", i2sDmaRxObj.buf, 
        i2s_dma_ping_buf_get(&i2sDmaRxObj), i2s_dma_pong_buf_get(&i2sDmaRxObj));
    i2s_dma_dest_add_set(I2S_NUM_I2S0, &i2sDmaRxObj);
    i2s_dma_enable(I2S_NUM_I2S0, I2S_MODE_MASTER_RX);    

}

void i2sTxModeInit_es8388()
{
    i2s_attr_t  i2sAttr;
    i2s_reset(I2S_NUM_I2S0);
    memset(&i2sAttr, 0, sizeof(i2sAttr));
//    int x,y,z;
//    int F,M;
//    F= 40000000;
//    M = 2048000;
    
//    printf("Freq read\r\n");
//    int ret = 0;
//    int pre_ret = 0;
//    for (z=0; z<256;z++) {        
//        for (y=0; y<32;y++) {            
//            for (x=2; x<=10;x++) {
//                ret =  (F*x+4*F) - ((4*M*z)+(2*z*M*y));
//                if (ret < pre_ret) {               
//                    printf("Found beter freq,x:%d,y:%d,z:%d\r\n", z, y, x);
//                    break;
//                }
//                else {
//                    pre_ret = ret;
//                }
//            }
//        }
//        
//    }
//    printf("Freq,x:%d,y:%d,z:%d\r\n", z, y, x);
    i2sAttr.tx_mode.type = I2S_MODULE_WORK_TYPE_I2S;
    i2sAttr.tx_mode.mode = I2S_MODE_MASTER_TX;
    i2sAttr.tx_mode.rate = I2S_SAMPLE_RATE_8K;
    i2sAttr.tx_mode.bits = I2S_BITS_PER_SAMPLE_16BIT;
    i2sAttr.tx_mode.std = I2S_STD_FORMAT_I2S;
    i2sAttr.tx_mode.iface = I2S_IFACE_TYPE_DMA;
    i2sAttr.tx_mode.channel = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_init(I2S_NUM_I2S0, &i2sAttr);
    
    i2sDmaBufInit_es8388();
    
    SHOWI2SREG();
}

void i2sRxModeInit_es8388()
{
    i2s_attr_t  i2sAttr;
    memset(&i2sAttr, 0, sizeof(i2sAttr));
    i2sAttr.rx_mode.type = I2S_MODULE_WORK_TYPE_I2S;
    i2sAttr.rx_mode.mode = I2S_MODE_MASTER_RX;
    i2sAttr.rx_mode.rate = I2S_SAMPLE_RATE_8K;
    i2sAttr.rx_mode.bits = I2S_BITS_PER_SAMPLE_16BIT;
    i2sAttr.rx_mode.std = I2S_STD_FORMAT_I2S;
    i2sAttr.rx_mode.iface = I2S_IFACE_TYPE_DMA;
    i2sAttr.rx_mode.channel = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_init(I2S_NUM_I2S0, &i2sAttr);
    SHOWI2SREG();
}
//进入PCM 放音模式 		  
void recorder_enter_play_mode_es8388(void)
{  // ES8388_ADC_StandBy();
    //ES8388_PowerOn_Init();
    ES8388_DAC_Play();
	vTaskDelay(5);
    LOGD("Playback the recorder...");
    i2s_stop(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_RX);
    int i;
    // Set data buffer     
    bufRdPos = I2sBuf; 
    bufWrPos = I2sBuf; 
  //  BufRead("agc", 81920);
    uint8_t *recvBuf = (uint8_t*)I2sBuf;

  #if 0

	for(i = 0; i < (I2SBUFSZ); i++) {
        os_printf("%02x ",*recvBuf++);
    }

	os_printf("\r\n\r\n");
	#endif

	tabSize = I2SBUFSZ;
    uint32_t *dataPing = i2s_dma_ping_buf_get(&i2sDmaTxObj);
    uint32_t *dataPong = i2s_dma_pong_buf_get(&i2sDmaTxObj);
    loadMusicData_es8388(dataPing, setBufLen);
    loadMusicData_es8388(dataPong, setBufLen);
    i2sTxModeInit_es8388();
    
    i2s_start(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_TX); 
}


void recorder_enter_rec_mode_es8388(void)
{
    LOGD("start to recorder...");
    i2s_stop(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_TX);
    i2c_master_gpio_init();
    ES8388_PowerOn_Init();
	ES8388_ADC_Record();
    i2sRxModeInit_es8388();
	
    i2sDmaBufInit_es8388();
	
    i2s_pin_init(I2S_MODE_MASTER_RX);

    i2s_start(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_RX);
}

void PlayWavMusic_es8388()
{    
    // i2c_master_gpio_init();
    // ES8388_PowerOn_Init();
	// ES8388_DAC_Play();
    vTaskDelay(100);
    LOGD("Enter play music...");
    i2sTxModeInit_es8388();
    i2s_pin_init(I2S_MODE_MASTER_TX);
    uint32_t *dataPing = i2s_dma_ping_buf_get(&i2sDmaTxObj);
    uint32_t *dataPong = i2s_dma_pong_buf_get(&i2sDmaTxObj);

    // Test wav music
    bufRdPos = (char*)musicTable;
    tabSize = 196174;
    set_buffer2Zero(dataPing, setBufLen);    
    set_buffer2Zero(dataPong, setBufLen);
//    loadMusicData_es8388(dataPing, setBufLen);
//    loadMusicData_es8388(dataPong, setBufLen);
    vTaskDelay(3000);

    i2s_start(I2S_MODULE_WORK_TYPE_I2S, I2S_NUM_I2S0, I2S_MODE_MASTER_TX);
    SHOWI2SREG();
}
void rec_play_mode_es8388()
{
	// Both of recorder and paly mode 
	LOGD("start to rec_play_mode...");
    i2s_stop(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_TX);
	ES8388_ADC_Record();
    i2sRxModeInit_es8388();
    i2s_start(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_RX);
	   //i2s_stop(I2S_MODULE_WORK_TYPE_I2S,I2S_NUM_I2S0, I2S_MODE_MASTER_TX);
	ES8388_DAC_Play();
}


void i2s_test_es8388_interface(void* para)
{
    LOGD("I2S module init ok");
    uint32_t recvDat = 0;
    
    PlayWavMusic_es8388();
//    recorder_enter_rec_mode_es8388();
    for(;;)
    {
//        if(xSemaphoreTake(xSemUartCmd, portMAX_DELAY) == pdTRUE )
//        {
//            printf("xSemaphoreTake received\r\n");
//            PlayWavMusic();
//        }
		xQueueReceive(uartQueue, &recvDat, portMAX_DELAY);
        LOGD("recvDat=%x", recvDat);
        switch (recvDat) 
            {
            case '1':
                PlayWavMusic_es8388();
                break;
            case '2':
                recorder_enter_rec_mode_es8388();
                break;
            case '3':
                recorder_enter_play_mode_es8388();
                break;
            case '4':
				rec_play_mode_es8388();
                break;
            default:
                LOGD("Cmd does not support.");
                break;                 
        }
    }
    vTaskDelete(NULL);
}

void i2s_test_es8388_init(void)
{
    
    LOGD("i2s_test_es8388_init task creating\r\n");
    xSemPlayEnd = xSemaphoreCreateBinary();
    xTaskCreate(i2s_test_es8388_interface, "i2s_test_es8388_interface", 4096, NULL, 4, NULL);
    uartQueue = xQueueCreate(1, sizeof(uint32_t*));
    
    LOGD("i2s_test_es8388_init task create ok\r\n");
}
//#endif
