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

#include "freertos/FreeRTOS.h"
#include "soc/gpio_sig_map.h"
#include "freertos/task.h"
#include "AP80/Ap80xx.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "esp_timer.h"
#include "Utility.h"
#include "string.h"
#include "crc.h"

// Define the SPI port
#define AUDIO_IFACE_PORT        SPI_NUM_SPI2
#define TX_SPI_BUF_LEN          (2*1024)
#define RX_SPI_BUF_LEN          (1024)

#define RX_SPI_LEN              (1024)

#define DECODETYPELEN           (4)
#define RESENDCOUNT             (2)
#define AP80GPIOINITNUM         (19)

#define SPIM_STREAM_CONTROL_GET_STATUS() (gpio_get_level(GPIO_NUM_22))

#define divDBG(level,fmt, ...) do {                     \
        if(level > 3){                                  \
            printf("[DECODE](%d) ", __LINE__);          \
            printf(fmt, ##__VA_ARGS__);                 \
            printf("\n");                               \
        }                                               \
    } while(0)
    
#define divByteS(level,indata,len) do {   \
        if(level > 3){                    \
            printfByteS(indata, len);     \
        }                                 \
    } while(0)

uint8_t SPIM_SEND_BUFFER[sizeof(EAUCmdContext) + EAU_SYNC_WORD_LENGTH + EAU_SEND_DATA_LENGTH + EAU_CRC_VALUE_LENGTH];
uint8_t SPIM_RECV_BUFFFR_ALL[sizeof(EAUCmdContext) + EAU_SYNC_WORD_LENGTH + EAU_SEND_DATA_LENGTH + EAU_CRC_VALUE_LENGTH];
uint8_t SPIM_RECV_BUFFER[RX_SPI_LEN];

EAUCmdContext         *CommandContext  = (EAUCmdContext *)SPIM_SEND_BUFFER;
EAUCmdResponseContext *ResponseContext = (EAUCmdResponseContext *)SPIM_SEND_BUFFER;

static bool spiRecv(uint8_t *RecvData, uint16_t Recvlen);
static void uint8ChangeFormat(uint32_t *inData, uint8_t  *outData, uint16_t len);
static void uint32ChangeFormat(uint8_t *inData, uint32_t *outData, uint16_t len);
static uint8_t sendData(SSPP_CMD CMD, uint16_t value, uint8_t *sendData, uint16_t  sendLen,uint8_t *receiveData, uint16_t  *receiveLen);

static uint32_t             g_waitTime    = 20000;
static bool                 g_noResponse  = false;
uint8_t                     g_enableDma   = 0;      //The default is not able to DMA
static xSemaphoreHandle     ap80_mutex_sendData;

#ifdef AP80XXENABLEDMA
static spi_dma_attr_t       spiDmaAp80xxTxObj;
static spi_dma_attr_t       spiDmaAp80xxRxObj;
#endif

static os_timer_t           timer;
static bool                 divTimeOut      = false;
xQueueHandle                apQueRecv       = NULL;
xQueueHandle                apQueSend       = NULL;
xQueueHandle                apFlowControl   = NULL;

static void timerOut()
{
    divTimeOut = true;
}

#ifdef AP80XXENABLEDMA
uint32_t sendcount0       = 0;
uint32_t sendcount1       = 0;
uint32_t recvCount        = 0;
//#include "freertos/portmacro.h"
static void spiDmaAp80xxIsr(void *para)
{
    uint32_t        regvalue;
    uint32_t        statusW, statusR;    
    char            pQueSend = 0x55;
    portBASE_TYPE   xHigherPriorityTaskWoken;
    
    regvalue = spi_int_status_get(SPI_NUM_SPI2);
    spi_int_clear(SPI_NUM_SPI2);
    if (regvalue & SPI_INT_SRC_WR_BUF_DONE) {
        divDBG(2, "SPI2_SLV_WR_BUF_DONE\n\r");
    } else if (regvalue & SPI_INT_SRC_RD_BUF_DONE) {
        divDBG(2, "SPI2_SLV_RD_BUF_DONE\n\r");
    } else if (regvalue & SPI_INT_SRC_RD_STA_DONE) {
        statusR = READ_PERI_REG(SPI_RD_STATUS_REG(SPI_NUM_SPI2));
        statusW = READ_PERI_REG(SPI_SLV_WR_STATUS_REG(SPI_NUM_SPI2));
        divDBG(2, "statusR %x statusW %x\n\r", statusR, statusW);
    } else if (regvalue & SPI_INT_SRC_WR_STA_DONE) {
        statusR = READ_PERI_REG(SPI_RD_STATUS_REG(SPI_NUM_SPI2));
        statusW = READ_PERI_REG(SPI_SLV_WR_STATUS_REG(SPI_NUM_SPI2));
        divDBG(2, "statusR %x statusW %x\n\r", statusR, statusW);
    } else if ((regvalue & SPI_INT_SRC_TRANS_DONE) && ((regvalue & 0xf) == 0)) {
        divDBG(2, "send SPI2_TRANS_DONE %d", sendcount0++);
    }
    
    uint32_t spi2DmaIntStatus =  spi_dma_int_status_get(SPI_NUM_SPI2);
    spi_dma_int_clear(SPI_NUM_SPI2);
    if (SPI_INT_SRC_ONE_BUF_SEND_DONE == (spi2DmaIntStatus & SPI_INT_SRC_ONE_BUF_SEND_DONE)) {
        if(xQueueSendFromISR(apQueSend, &pQueSend, &xHigherPriorityTaskWoken) == pdPASS)
        {
            //portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
        //ets_printf("TX SPI2\n");
    }

    if (SPI_INT_SRC_ONE_BUF_RECV_DONE == (spi2DmaIntStatus & SPI_INT_SRC_ONE_BUF_RECV_DONE)) {
        if(xQueueSendFromISR(apQueRecv, &pQueSend, &xHigherPriorityTaskWoken) == pdPASS)
        {
            //portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
        //ets_printf("RX SPI2\n");
    }
    divDBG(2, "SPI2_INT_DONE");
}

static int32_t user_spi_dma_memcpy(spi_dma_attr_t *obj, void *src, uint32_t total, uint32_t *copied, uint32_t len)
{
    if ((NULL == obj)
        || (NULL == src)) {
        return -1;
    }
    uint32_t *bufAddress = NULL;
    uint8_t *srcDat = (uint8_t*)src;
    switch (obj->dir) {
    case SPI_DMA_DIR_OUT: {
        if (0 == *copied) {
            divDBG(2, "len=%d, buflen=%d\r\n", len, obj->buf->len);
            if (len <= obj->buf->len) {
                dma_buf_len_set(obj->buf, obj->buf->ping, len);
                memcpy(obj->buf->ping->buffer_addr, srcDat, len);
                *copied = len;
            } else {
                dma_buf_len_set(obj->buf, obj->buf->ping, obj->buf->len);
                memcpy(obj->buf->ping->buffer_addr, srcDat, obj->buf->len);
                srcDat += obj->buf->len;
                if (len < (obj->buf->len * 2)) {
                    dma_buf_len_set(obj->buf, obj->buf->pong, (len - obj->buf->len));
                    memcpy(obj->buf->pong->buffer_addr, srcDat, (len - obj->buf->len));
                    *copied = len;
                } else {
                    dma_buf_len_set(obj->buf, obj->buf->pong, obj->buf->len);
                    memcpy(obj->buf->pong->buffer_addr, srcDat, (obj->buf->len));
                    *copied = obj->buf->len * 2;
                }
            }
        } else {
            if ((spi_dma_status_get(obj)) == obj->buf->ping->buffer_addr)  {
                uint32_t remdLen = total - *copied;
                if (remdLen < (obj->buf->len + total % obj->buf->len)) {
                    dma_buf_len_set(obj->buf, obj->buf->ping, remdLen);
                    memcpy(obj->buf->ping->buffer_addr, srcDat, remdLen);
                    *copied += remdLen;
                } else {
                    memcpy(obj->buf->ping->buffer_addr, srcDat, obj->buf->len);
                    *copied += obj->buf->len;
                }
                printf("Pi%d,remdLen%d,sent%d\r\n", len, remdLen, *copied);
            } else if ((spi_dma_status_get(obj)) == obj->buf->pong->buffer_addr) {
                uint32_t remdLen = total - *copied;
                if (remdLen < (obj->buf->len + total % obj->buf->len)) {
                    dma_buf_len_set(obj->buf, obj->buf->pong, remdLen);
                    memcpy(obj->buf->pong->buffer_addr, srcDat, remdLen);
                    *copied += remdLen;
                } else {
                    memcpy(obj->buf->pong->buffer_addr, srcDat, obj->buf->len);
                    *copied += obj->buf->len;
                }
                divDBG(2, "Po%d,remdLen%d,sent%d\r\n", len, remdLen, *copied);
            }

        }
        break;
    }
    case SPI_DMA_DIR_IN: {
        if ((spi_dma_status_get(obj)) ==  obj->buf->ping->buffer_addr) {
            bufAddress = obj->buf->ping->buffer_addr;
            if (len < obj->buf->len) {
                dma_buf_len_set(obj->buf, obj->buf->ping, len);
            }
        } else if ((spi_dma_status_get(obj)) == obj->buf->pong->buffer_addr) {
            bufAddress = obj->buf->pong->buffer_addr;
            if (len < obj->buf->len) {
                dma_buf_len_set(obj->buf, obj->buf->pong, len);
            }
        } else {
            bufAddress = obj->buf->ping->buffer_addr;
            dma_buf_len_set(obj->buf, obj->buf->ping, len);
            if (obj->buf_size < len) {
                dma_buf_len_set(obj->buf, obj->buf->pong, (len - obj->buf_size));
            }
        }
        if (bufAddress) {
            memcpy(src, bufAddress, obj->buf_size);
        }
        break;
    }
    default:
        len = 0;
        break;
    }
    return len;
}
#endif

/**
 * For receiving data buf, format processing
 */
static void uint32ChangeFormat(uint8_t *inData, uint32_t *outData, uint16_t len)
{
    uint8_t i, j;
    if (len == 0)
    {
        return;
    }
    for (i = 0; i < len / sizeof(uint32_t); i++)
    {
        outData[i] = (uint32_t) (inData[4 * i + 0] << 0) | (inData[4 * i + 1] << 8) | (inData[4 * i + 2] << 16) | (inData[4 * i + 3] << 24);
    }
    for (j = 0; j < (len % 4); j++)
    {
        outData[i] |= (inData[4 * i + j] << (8 * j));
    }
}

/**
 * For receiving data buf, format processing
 */
static void uint8ChangeFormat(uint32_t *inData, uint8_t *outData, uint16_t len)
{
    uint8_t i, j;
    if (len == 0)
    {
        return;
    }
    for (i = 0; i < (len / sizeof(uint32_t)); i++)
    {
        outData[4 * i + 0] = inData[i] >> 0;
        outData[4 * i + 1] = inData[i] >> 8;
        outData[4 * i + 2] = inData[i] >> 16;
        outData[4 * i + 3] = inData[i] >> 24;
    }
    for (j = 0; j < (len % 4); j++)
    {
        outData[4 * i + j] = (uint8_t)(inData[i] >> (8 * j));
    }
}

static bool FindSpiStreamDataSyncWord()
{
    uint32_t SyncWord;
    uint32_t Cnt = 0;
    uint8_t *p = (uint8_t *)&SyncWord;

    while (1)
    {
        SyncWord = 0;
        if (spiRecv(p, 1))
        {
            if (p[0] == EAU_SYNC_BYTE)
            {
                return false;
            }
        }
        if (Cnt++ > 100)
        {
            return false;
        }
    }
}

/**
 * To send data by way of SPI
 */
static bool spiSend(uint8_t *sendData, uint16_t len)
{
    uint32_t      data[16] = { 0 };
    spi_data_t  pDat;
    if (len > 64)
        return false;
    pDat.cmd_len  = 0;
    pDat.addr_len = 0;

    pDat.tx_data_len = len;    
    pDat.tx_data    = data;
    
    uint32ChangeFormat((uint8_t *)sendData, data, pDat.tx_data_len);
    spi_master_send_data(AUDIO_IFACE_PORT, &pDat);

    return true;
}

/**
 * Receiving data by way of SPI
 */
static bool spiRecv(uint8_t *recvData, uint16_t len)
{
    uint32_t data[16] = { 0 };
    spi_data_t pDat;
    if (len > 64)
        return false;
    pDat.cmd_len  = 0;
    pDat.addr_len = 0;

    pDat.rx_data_len = len;
    pDat.rx_data = data;
    
    spi_master_recv_data(AUDIO_IFACE_PORT, &pDat);

    uint8ChangeFormat(data, recvData, len);
    return true;
}
#ifdef AP80XXENABLEDMA
/**
 * Send data by way of SPI(Dma)
 */
static bool spiDmaSend(uint8_t *sendData, uint16_t sendLen)
{    
    uint32_t sendlen        = 0x00;
    char     pQueSend       = 0x00;
    uint8_t *pSendRd        = sendData;
    if (sendLen > TX_SPI_BUF_LEN)
        return false;
    
    spi_dma_rest(&spiDmaAp80xxTxObj);
    user_spi_dma_memcpy(&spiDmaAp80xxTxObj, pSendRd, sendLen, &sendlen, sendLen);    
    xQueueReset(apQueSend);
    
    spi_dma_start(&spiDmaAp80xxTxObj, sendLen);
    
    if(xQueueReceive(apQueSend, &pQueSend, 150 / portTICK_RATE_MS) == pdFALSE)
    {
        return false;
    }    
    return true;
}

/**
 * Receiving data by way of SPI(Dma)
 */
static bool spiDmaRecv(uint8_t *recvData, uint16_t recvlen)
{
    uint8_t  *recvDataPtr   = recvData;
    uint16_t wantedRecvLen  = recvlen;
    uint32_t sendlen        = 0x00;
    char     pQueRecv       = 0x00;
    if (wantedRecvLen > RX_SPI_BUF_LEN)
        return false;
    wantedRecvLen = wantedRecvLen;
    //printf("start recv\n");
    //spi_dma_int_clear(SPI_NUM_SPI2);
    xQueueReset(apQueRecv);
    spi_dma_rest(&spiDmaAp80xxTxObj);
    spiDmaAp80xxRxObj.dir = SPI_DMA_DIR_IN;    
    spi_dma_dest_add_set(&spiDmaAp80xxRxObj);
    spi_dma_start(&spiDmaAp80xxRxObj, wantedRecvLen);
    
    if(xQueueReceive(apQueRecv, &pQueRecv, 200 / portTICK_RATE_MS) == pdFALSE)
    {
        //printf("recv time out\n");
        return false;
    }
    
    user_spi_dma_memcpy(&spiDmaAp80xxRxObj, recvDataPtr, wantedRecvLen, &sendlen, wantedRecvLen);
    
    return true;
}
#endif
typedef void (*xt_handler)(void *);

static void ap_intr_callback(void *arg)
{
    //printf("in gpio_intr\n");
    portBASE_TYPE   xHigherPriorityTaskWoken;
    char            pQueSend = 0x55;
    uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG);
    SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG,  1 << GPIO_NUM_22);
    
    if(xQueueSendFromISR(apFlowControl, &pQueSend, &xHigherPriorityTaskWoken) == pdPASS)
    {
        ;
    }
}

static void gpio_intr_init(uint8_t gpio_num)
{
    gpio_config_t gpio_test;
	memset(&gpio_test,0,sizeof(gpio_test));
    gpio_test.pin_bit_mask=(uint64_t)((uint64_t)(1)<<gpio_num);
    
	gpio_test.mode          = GPIO_MODE_INPUT;
	gpio_test.intr_type     = GPIO_INTR_NEGEDGE;
	gpio_test.pull_up_en    = GPIO_PULLUP_ENABLE;
	gpio_test.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    gpio_config(&gpio_test);
	//Register gpio intr callback
    gpio_isr_register(AP80GPIOINITNUM, ap_intr_callback, NULL);
    //gpio_isr_register(ap_intr_callback, NULL, 0, NULL);
    //gpio_install_isr_service(0);
    //gpio_isr_handler_add(22, ap_intr_callback, NULL);
}

uint32_t receivePulse(uint16_t timeOut)
{
    uint8_t     ret         = 0;
    char        pQueSend    = 0;
    uint32_t    currentTime = 0;
#ifndef PORTING_LY_DONT
    uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG);     //read status to get interrupt status for GPIO0-31

    //printf("%08x\n",gpio_intr_status);
    SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG, 1 << GPIO_NUM_22);      //Clear intr for gpio0-gpio31            
    gpio_intr_enable(GPIO_NUM_22);                                  //open intr
    xQueueReset(apFlowControl);
    
    currentTime = system_get_time();
    if(xQueueReceive(apFlowControl, &pQueSend, timeOut / portTICK_RATE_MS) == pdFALSE)
    {
        divDBG(4, "send data stup 04 %d gpio is %d",system_get_time() - currentTime,SPIM_STREAM_CONTROL_GET_STATUS());
        ret = 1;
    }
    gpio_intr_disable(GPIO_NUM_22);                                 //close intr
#else
    if(SPIM_STREAM_CONTROL_GET_STATUS())
    {
        uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG);     //read status to get interrupt status for GPIO0-31

        SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG, 1 << GPIO_NUM_22);      //Clear intr for gpio0-gpio31            
        gpio_intr_enable(GPIO_NUM_22);                                  //open intr
        xQueueReset(apFlowControl);

        currentTime = system_get_time();
        if(xQueueReceive(apFlowControl, &pQueSend, timeOut / portTICK_RATE_MS) == pdFALSE)
        {
            divDBG(4, "send data stup 04 %d gpio is %d", system_get_time() - currentTime,SPIM_STREAM_CONTROL_GET_STATUS());
            ret = 1;
        }
        gpio_intr_disable(GPIO_NUM_22);                                 //close intr        
    }
#endif
    return ret;
}

/**
 * Spi initialization
 */
void apInit()
{
    spi_attr_t hSpiAttr;
    gpio_config_t  GPIO_InitStructure;
    
    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitStructure));
    hSpiAttr.bit_order  = SPI_BIT_ORDER_MSB_FIRST;
    hSpiAttr.speed      = SPI_SPEED_10MHZ;
    hSpiAttr.mode       = SPI_MODE_MASTER;
    hSpiAttr.sub_mode   = SPI_SUBMODE_0;
    hSpiAttr.half_mode  = SPI_WORK_MODE_FULL;

    spi_init(AUDIO_IFACE_PORT, &hSpiAttr);
    
    gpio_matrix_out(GPIO_NUM_18, HSPICLK_OUT_IDX,0,0);
    gpio_matrix_out(GPIO_NUM_23, HSPID_OUT_IDX,  0,0);
    gpio_matrix_in(GPIO_NUM_19,  HSPIQ_IN_IDX,   0);
    gpio_matrix_out(GPIO_NUM_5,  HSPICS0_OUT_IDX, 0,0);

    #if AP80XXENABLEINT
    apFlowControl = xQueueCreate(1, sizeof(char));
    gpio_intr_init(22);
    gpio_intr_disable(GPIO_NUM_22);             //close intr
    #else
    GPIO_InitStructure.pin_bit_mask         = GPIO_SEL_22;
    GPIO_InitStructure.mode        			= GPIO_MODE_INPUT;
    GPIO_InitStructure.intr_type            = GPIO_PIN_INTR_DISABLE;
    gpio_config(&GPIO_InitStructure);
    #endif

    os_timer_setfn(&timer, timerOut, NULL);

    #ifdef AP80XXENABLEDMA
    //if(g_enableDma)
    {   //start spi dma init
        spiDmaAp80xxTxObj.buf         = NULL;
        spiDmaAp80xxTxObj.buf_size    = TX_SPI_BUF_LEN;
        spiDmaAp80xxTxObj.mode        = SPI_MODE_MASTER;
        spiDmaAp80xxTxObj.channel     = SPI_DMA_CHANNEL_0;
        spiDmaAp80xxTxObj.dir         = SPI_DMA_DIR_OUT;
        spiDmaAp80xxTxObj.spi_num     = AUDIO_IFACE_PORT;
        
        int dmaRegt = spi_dma_init(&spiDmaAp80xxTxObj, spiDmaAp80xxIsr);        
        if (dmaRegt) {
            printf("spi_dma_init init failed ret=%d\n", dmaRegt);
            return ;
        }
        spi_dma_dest_add_set(&spiDmaAp80xxTxObj);
        
        spiDmaAp80xxRxObj.buf         = NULL;
        spiDmaAp80xxRxObj.buf_size    = RX_SPI_BUF_LEN;
        spiDmaAp80xxRxObj.mode        = SPI_MODE_MASTER;
        spiDmaAp80xxRxObj.channel     = SPI_DMA_CHANNEL_0;
        spiDmaAp80xxRxObj.dir         = SPI_DMA_DIR_IN;
        spiDmaAp80xxRxObj.spi_num     = AUDIO_IFACE_PORT;
        
        dmaRegt = spi_dma_init(&spiDmaAp80xxRxObj, spiDmaAp80xxIsr);
        if (dmaRegt) {
            printf("spi_dma_init init failed ret=%d\n", dmaRegt);
        }
        spi_dma_dest_add_set(&spiDmaAp80xxRxObj);
       
        apQueRecv = xQueueCreate(1, sizeof(char));
        apQueSend = xQueueCreate(1, sizeof(char));
    }
    #endif

    if (sys_mutex_new(&ap80_mutex_sendData) != 0)
    {
        printf("pthread_mutex_init error\n");
    }
    printf("spi init master mode\n");
}


/**
 * Audio playback process of sending data
 */
static uint8_t sendData(SSPP_CMD CMD, uint16_t value, uint8_t *sendData, uint16_t  sendLen,uint8_t *receiveData, uint16_t  *receiveLen)
{
#ifdef AP80XXENABLEDMA
    sys_mutex_lock(&ap80_mutex_sendData);
    EAUCmdContext        *CommandContext   = (EAUCmdContext *)SPIM_SEND_BUFFER;
    EAUCmdResponseContext *ResponseContext = (EAUCmdResponseContext *)SPIM_RECV_BUFFER;
    uint8_t  *sendDataFramePtr  = (uint8_t *) &SPIM_SEND_BUFFER[sizeof(EAUCmdContext)];
    uint8_t  *ptr               = NULL;
    uint8_t   result            = ERROR_TIME_OUT;
    uint16_t  sendDataLen       = sendLen;
    uint32_t  cnt               = 0;
    bool      flag              = false;
    int8_t    reSendCnt         = RESENDCOUNT;
    int16_t   sendRemLen        = 0;
    uint8_t   protocolMode      = 0;
    divTimeOut                  = false;
    
    if(sendData == NULL && sendLen == 0 && receiveData == NULL)
    {
        protocolMode = 0;
    }
    else if(sendData != NULL && sendLen != 0 && receiveData == NULL)
    {
        protocolMode = 2;
    }
    else if(sendData == NULL && sendLen == 0 && receiveData != NULL)
    {
        protocolMode = 1;
    }
    else if(sendData != NULL && sendLen != 0 && receiveData != NULL)
    {
        protocolMode = 3;
    } else {
        result = ERROR_PROMODE;
        goto error;
    }

    if(protocolMode & BIT1)
    {
        if (sendLen >= EAU_SEND_DATA_LENGTH - EAU_SEND_FARMA_LENGTH)
        {
            result = ERROR_PARAMETER;
            goto error;
        }
    }
    
    do
    {        
        divDBG(2, "send data stup 01 %d",system_get_time());
        
        //1. Send EAU_CMD_DATA command
        CommandContext->SyncWord  = EAU_SYNC_BYTE;
        CommandContext->Command   = CMD;
        if(CMD == EAU_CMD_VOL_SET)
        {
            CommandContext->Content   = value;
        }
        else
        {
            CommandContext->Content   = sendDataLen;
        }

        CommandContext->CrcValue  = GetCRC16NBS((uint8_t *)CommandContext, sizeof(EAUCmdContext) - EAU_CRC_VALUE_LENGTH);

        if(protocolMode & BIT1)
        {
            *sendDataFramePtr         = EAU_SYNC_BYTE;

            MEMCPY(sendDataFramePtr   + EAU_SYNC_WORD_LENGTH, sendData, sendDataLen);

            uint16_t crc = GetCRC16NBS(sendDataFramePtr + 1, CommandContext->Content);
            sendDataFramePtr[sendDataLen + EAU_SYNC_WORD_LENGTH + 0] = (uint8_t) crc;
            sendDataFramePtr[sendDataLen + EAU_SYNC_WORD_LENGTH + 1] = crc >> 8;            
            sendRemLen = sizeof(EAUCmdContext) + EAU_SYNC_WORD_LENGTH + sendDataLen + EAU_CRC_VALUE_LENGTH;
        } else {
            sendRemLen = sizeof(EAUCmdContext);
        }
        #ifdef AP80DEBUG      
        if((*sendData == 0xE7) && (*(sendData + 1) == 0x04))
        {
            //printfByteS(debugData, sendRemLen);
            //printf("%x%x%x%x",SPIM_SEND_BUFFER[sendRemLen - 1],SPIM_SEND_BUFFER[sendRemLen - 2],SPIM_SEND_BUFFER[sendRemLen - 3],SPIM_SEND_BUFFER[sendRemLen - 4]);
        }
        #endif
        
#if AP80XXENABLEINT == 0
        os_timer_arm(&timer, 1000, false);
        while (SPIM_STREAM_CONTROL_GET_STATUS())
        {
            if(divTimeOut)
            {
                result = ERROR_DEVICE_TIME_OUT;
                divDBG(4, "send data stup 04 %d",system_get_time());
                goto error;
            }
        };
#else
        if(receivePulse(5000))
        {
            result = ERROR_DEVICE_TIME_OUT;
            divDBG(4, "Hardware decoder busy %d",SPIM_STREAM_CONTROL_GET_STATUS());
            goto error;
        }
#endif
        divDBG(2, "stup 02 %d",system_get_time());
        
        if (!spiDmaSend((uint8_t *) SPIM_SEND_BUFFER, (uint16_t) sendRemLen))
        {
            divDBG(4, "send data time out (no dma int)");
            goto error;
        }
        
        divDBG(2, "stup 04 %d %x %x",system_get_time(),(uint32_t)spiDmaAp80xxTxObj.buf->ping->buffer_addr,(uint32_t)spiDmaAp80xxTxObj.buf->pong->buffer_addr);
        //divDBG(4, "stup 04 %d %x %x",system_get_time(),(uint32_t)spiDmaAp80xxTxObj.buf->ping->buffer_addr,(uint32_t)spiDmaAp80xxTxObj.buf->pong->buffer_addr);
        
        memset(spiDmaAp80xxTxObj.buf->ping->buffer_addr, 0, TX_SPI_BUF_LEN);
        memset(spiDmaAp80xxTxObj.buf->pong->buffer_addr, 0, TX_SPI_BUF_LEN);
        memset(spiDmaAp80xxRxObj.buf->ping->buffer_addr, 0, RX_SPI_LEN);
        memset(spiDmaAp80xxRxObj.buf->pong->buffer_addr, 0, RX_SPI_LEN);        

        //if(g_noResponse)
        {
            result = OK_RESPONSE;
            reSendCnt--;
            goto error;
        }
#if AP80XXENABLEINT == 1
        if(receivePulse(1000))
        {
            result = ERROR_TIME_OUT;
            divDBG(4, "Hardware decoder no response");
            goto error;
        }
#endif
        
        //3. Receive response
        uint8_t    *pRecvRd         = SPIM_RECV_BUFFER;
        uint8_t    *pRecvAllData    = SPIM_RECV_BUFFFR_ALL;
        uint32_t    recvCount       = 0;
        uint8_t     packetResponse  = 0;
        uint16_t    packetDataLen   = 0;
        uint16_t    recvBufValidLen = 0;
        
        ptr  = (uint8_t *)&ResponseContext->SyncWord;
        cnt  = 0;
        flag = false;
        do {
            //vTaskDelay(1 / portTICK_PERIOD_MS);
            spiDmaRecv(pRecvRd, RX_SPI_LEN);
            
            divByteS(2, pRecvRd, RX_SPI_LEN);
            
            for(int i = 0; i < RX_SPI_LEN; i++)
            {
                if(pRecvRd[i] == EAU_SYNC_BYTE)
                {
                    memcpy(pRecvAllData, pRecvRd + i,RX_SPI_LEN - i);
                    recvBufValidLen += (RX_SPI_LEN - i);
                    divDBG(2, "decode recv response is 0:");
                    if(i >= RX_SPI_LEN - sizeof(EAUCmdContext))
                    {
                        divDBG(2, "recv heard len is %d", i);
                        spiDmaRecv(pRecvRd, RX_SPI_LEN);
                        memcpy(pRecvAllData + RX_SPI_LEN - i, pRecvRd,RX_SPI_LEN);
                        recvBufValidLen += RX_SPI_LEN;
                    }
                    ResponseContext = (EAUCmdResponseContext *) pRecvAllData;
                    
                    packetResponse  = ResponseContext->Response &  0x000F;
                    packetDataLen   = (uint16_t) (ResponseContext->Response >> 0x04);
                    divByteS(2, pRecvAllData, 0x10);
                    while(packetDataLen > recvBufValidLen) {
                        divDBG(2, "recv heard len is %d %d", i, recvBufValidLen);
                        divByteS(2, pRecvAllData, 0x10);
                        spiDmaRecv(pRecvRd, RX_SPI_LEN);
                        memcpy(pRecvAllData + recvBufValidLen, pRecvRd,RX_SPI_LEN);
                        recvBufValidLen += RX_SPI_LEN;
                    }
                    divDBG(2, "decode recv response is: %d", recvBufValidLen);
                    divByteS(2, pRecvAllData, 0x60);
                    flag = true;
                    goto tag;
                }
            }
            
            if(++recvCount > 1000)
            {
                divDBG(4, "decode recv response time out");
                break;
            }
        }while(1);

        #ifdef AP80DEBUG
        if(!flag)
        {
            divDBG(4, "send error01 %x",debugCRC);
            printfByteS(debugData, 0x200);
            divDBG(4, "send error02");
            printfByteS(sendData, 0x80);            
            divDBG(4, "1 recv heard is %x %x %x %x recvCount %d", sendData[0], sendData[1], sendData[2], sendData[3],recvCount);
        }
        #endif
        
    tag:
        if(flag)
        {
            if (ResponseContext->Command == CMD)
            {
                ptr = (uint8_t *)&ResponseContext->SyncWord;
                
                //divDBG(4, "recv heard is %x %x %x %x recvCount %d", ptr[0], ptr[1], ptr[2], ptr[3],recvCount);
                divDBG(2, "response = %d dataLen = %d recvCount = %d",packetResponse,packetDataLen,recvCount);
                if(packetResponse == EAU_CMD_RESP_OKSEND || packetResponse == EAU_CMD_RESP_OKCMD)
                {
                    result = OK_RESPONSE;
                    if(packetDataLen > 0 && (protocolMode & BIT0))
                    {
                        uint16_t     localCrc   = 0x00;
                        uint16_t     recvCrc    = 0x00;

                        ptr = pRecvAllData + sizeof(EAUCmdContext) + EAU_SYNC_WORD_LENGTH;
                        localCrc = GetCRC16NBS(ptr, packetDataLen);
                        recvCrc  = *(uint16_t*)(ptr + packetDataLen);
                        if(localCrc == recvCrc)
                        {
                            if(*receiveLen != 0 && *receiveLen < packetDataLen)
                            {
                                result = ERROR_PARAMETER;
                                goto reSend;
                            }
                            memcpy(receiveData, ptr, packetDataLen);
                            *receiveLen = packetDataLen;
                            
                            divDBG(2, "recv ok packetDataLen is %d %d", *receiveLen,packetDataLen);
                        }
                        else
                        {
                            result = ERROR_CRC;
                            divDBG(4, "[ERROR] recv CRC error localCrc %X recvCrc %X", localCrc, recvCrc);
                            divByteS(2, ptr, packetDataLen);
                            goto reSend;
                        }
                    }
                }
                else if (packetResponse == EAU_CMD_RESP_NEXTSEND)
                {
                    result = ERROR_STREAM_FULL;
                }
                else if (packetResponse == EAU_CMD_RESP_RESEND)
                {
                    result = ERROR_CRC;
                }
                else if(packetResponse == EAU_CMD_RESP_ERCMD)
                {
                    result = ERROR_OTHER;
                }
                else if(packetResponse == EAU_CMD_RESP_STATEERROR)
                {
                    result = ERROR_START;
                }
                else
                {
                    divDBG(4, "response = %d dataLen = %d",packetResponse,packetDataLen);
                    result = ERROR_RESPONSE;
                }

                if (packetResponse == EAU_CMD_RESP_INITERROR)
                {
                    result = ERROR_INIT;
                    goto reSend;
                }
            }
            else
            {
                divDBG(4, "cmd = %x response = %x dataLen = %x",ResponseContext->Command,packetResponse,packetDataLen);
                divByteS(2, pRecvAllData, 64);
                result = ERROR_CMD;
            }
        }
        reSend:
        {
            ;
        }
    }
    while ((--reSendCnt >= 0) && (result != OK_RESPONSE));
error:
    os_timer_disarm(&timer);
    if(reSendCnt != RESENDCOUNT - 1 && reSendCnt != RESENDCOUNT)
        divDBG(4, "SPI resend %d %d",reSendCnt,sendLen);
    sys_mutex_unlock(&ap80_mutex_sendData);
    return result;
    
#else
    
    sys_mutex_lock(&ap80_mutex_sendData);
    EAUCmdContext        *CommandContext   = (EAUCmdContext *)SPIM_SEND_BUFFER;
    EAUCmdResponseContext *ResponseContext = (EAUCmdResponseContext *)SPIM_SEND_BUFFER;
    uint8_t  *sendDataFramePtr = (uint8_t *) &SPIM_SEND_BUFFER[sizeof(EAUCmdContext)];
    uint8_t  *ptr               = NULL;
    uint8_t   result            = 1;
    uint16_t  sendDataLen       = sendLen;
    uint32_t  cnt               = 0;
    bool      flag              = false;
    uint16_t  timeOut           = g_waitTime;
    int8_t   reSendCnt          = RESENDCOUNT;
    int16_t  sendRemLen         = 0;
    uint16_t  pos               = 0;
    uint8_t   protocolMode      = 0;
    divTimeOut                  = false;

    if(sendData == NULL && sendLen == 0 && receiveData == NULL)
    {
        protocolMode = 0;
    }
    else if(sendData != NULL && sendLen != 0 && receiveData == NULL)
    {
        protocolMode = 2;
    }
    else if(sendData == NULL && sendLen == 0 && receiveData != NULL)
    {
        protocolMode = 1;
    }
    else if(sendData != NULL && sendLen != 0 && receiveData != NULL)
    {
        protocolMode = 3;
    } else {
        result = ERROR_PROMODE;
        goto error;
    }

    if(protocolMode & BIT1)
    {
        if (sendLen >= EAU_SEND_DATA_LENGTH - EAU_SEND_FARMA_LENGTH)
        {
            result = ERROR_PARAMETER;
            goto error;
        }
    }

    do
    {
        //1. Send EAU_CMD_DATA command
        CommandContext->SyncWord  = EAU_SYNC_BYTE;
        CommandContext->Command   = CMD;
        if(CMD == EAU_CMD_VOL_SET)
        {
            CommandContext->Content   = value;
        }
        else
        {
            CommandContext->Content   = sendDataLen;
        }

        CommandContext->CrcValue  = GetCRC16NBS((uint8_t *)CommandContext, sizeof(EAUCmdContext) - EAU_CRC_VALUE_LENGTH);

        pos                       = 0;

        if(protocolMode & BIT1)
        {

            *sendDataFramePtr         = EAU_SYNC_BYTE;

            MEMCPY(sendDataFramePtr   + EAU_SYNC_WORD_LENGTH, sendData, sendDataLen);

            uint16_t crc = GetCRC16NBS(sendDataFramePtr + 1, CommandContext->Content);
            sendDataFramePtr[sendDataLen + EAU_SYNC_WORD_LENGTH + 0] = (uint8_t) crc;
            sendDataFramePtr[sendDataLen + EAU_SYNC_WORD_LENGTH + 1] = crc >> 8;

            sendRemLen = sizeof(EAUCmdContext) + EAU_SYNC_WORD_LENGTH + sendDataLen + EAU_CRC_VALUE_LENGTH;
        } else {
            sendRemLen = sizeof(EAUCmdContext);
        }

        os_timer_arm(&timer, 500, false);
        while (SPIM_STREAM_CONTROL_GET_STATUS())
        {
            if(divTimeOut)
            {
                result = ERROR_DEVICE_TIME_OUT;
                goto error;
            }
        };
        divDBG(2, "stup 02");
        
        if(g_noResponse)
        {
            result = OK_RESPONSE;
            reSendCnt--;
            goto error;
        }
        
        do
        {
            if(sendRemLen >= EAU_SEND_FARMA_LENGTH)
            {
                spiSend(SPIM_SEND_BUFFER + pos, EAU_SEND_FARMA_LENGTH);
                pos += EAU_SEND_FARMA_LENGTH;
            }
            else
            {
                spiSend(SPIM_SEND_BUFFER + pos, sendRemLen);
                break;
            }
            sendRemLen -= EAU_SEND_FARMA_LENGTH;
        }
        while(sendRemLen >= 0);
        divDBG(2, "stup 03");

        //3. Receive response
        ptr = (uint8_t *)&ResponseContext->SyncWord;

        cnt  = 0;
        flag = false;

        while (1)
        {
            spiRecv(ptr, 1);
            if (ptr[0] == EAU_SYNC_BYTE)
            {
                spiRecv((uint8_t *)&ResponseContext->Command, (sizeof(EAUCmdContext) - EAU_SYNC_WORD_LENGTH));
                divDBG(2, "Command:%02x Response:%04x\n",ResponseContext->Command,ResponseContext->Response);
                if(ResponseContext->Command == CMD)
                {
                    flag = true;
                    break;
                }
            }
            if (cnt++ > timeOut)
            {
                divDBG(2, "Cnt %08x",cnt);
                result = ERROR_TIME_OUT;
                break;
            }
        }

        if (flag)
        {
            if (ResponseContext->Command == CMD)
            {
                uint8_t response = ResponseContext->Response &  0x00FF;
                uint8_t dataLen  = ResponseContext->Response >> 0x08;

                divDBG(1, "response = %d dataLen = %d",response,dataLen);

                if(response == EAU_CMD_RESP_OKSEND || response == EAU_CMD_RESP_OKCMD)
                {
                    result = OK_RESPONSE;
                    if(dataLen > 0 && (protocolMode & BIT0))
                    {
                        uint8_t      recvLen    = 0x00;

                        uint16_t     LocalCrc   = 0x00;
                        uint16_t     recvCrc    = 0x00;
                        uint8_t      recvCrcLen = 0x00;
                        if(FindSpiStreamDataSyncWord())
                        {
                            uint8_t      *RecvBuff       = (uint8_t*)ResponseContext;
                            int16_t  recvRemLen      = dataLen;
                            pos                     = 0;

                            do
                            {
                                if(recvRemLen >= EAU_SEND_FARMA_LENGTH)
                                {
                                    spiRecv(RecvBuff + pos, EAU_RECV_FARMA_LENGTH);
                                    pos += EAU_SEND_FARMA_LENGTH;
                                }
                                else
                                {
                                    spiRecv(RecvBuff + pos, recvRemLen);
                                    break;
                                }
                                recvRemLen -= EAU_SEND_FARMA_LENGTH;
                            }while(recvRemLen >= 0);

                            spiRecv((uint8_t *) &recvCrc, EAU_CRC_VALUE_LENGTH);

                            LocalCrc = GetCRC16NBS(RecvBuff, dataLen);
                            if(LocalCrc == recvCrc)
                            {
                                if(*receiveLen != 0 && *receiveLen < dataLen)
                                {
                                    result = ERROR_PARAMETER;
                                    goto error;
                                }
                                memcpy(receiveData, RecvBuff, dataLen);
                                *receiveLen = dataLen;
                            }
                            else
                            {
                                result = ERROR_CRC;
                                goto error;
                            }
                        }
                    }
                }
                else if (response == EAU_CMD_RESP_NEXTSEND)
                {
                    result = ERROR_STREAM_FULL;
                }
                else if (response == EAU_CMD_RESP_RESEND)
                {
                    result = ERROR_CRC;
                }
                else if(response == EAU_CMD_RESP_ERCMD)
                {
                    result = ERROR_OTHER;
                }
                else if(response == EAU_CMD_RESP_STATEERROR)
                {
                    result = ERROR_START;
                }
                else
                {
                    result = ERROR_RESPONSE;
                }

                if (response == EAU_CMD_RESP_INITERROR)
                {
                    result = ERROR_INIT;
                    goto error;
                }
            }
            else
            {
                result = ERROR_CMD;
            }
        }
        else
        {
            result = ERROR_TIME_OUT;
        }
    }
    while ((--reSendCnt >= 0) && (result != 0));
error:
    os_timer_disarm(&timer);
    if(reSendCnt != RESENDCOUNT - 1)
        divDBG(4, "SPI resend %d %d",reSendCnt,sendLen);
    sys_mutex_unlock(&ap80_mutex_sendData);
    return result;
#endif
    
}

/**
 * Audio playback start
 */
uint8_t apPlayStart(const char *parameter, uint8_t len, DecoderType decType)
{
    uint8_t data[64];
    if(len > 60)
        return ERROR_PARAMETER;
    memcpy(data + DECODETYPELEN, parameter, len);
    *(uint32_t*) data = decType;
    return  sendData(EAU_CMD_START, 0, data, len + DECODETYPELEN,NULL, 0);
}

/**
 * Send the data
 */
uint8_t apSendData(uint8_t *sendBuf, uint16_t  sendLen)
{
    apSetResponse(false);
    uint8_t ret = sendData(EAU_CMD_DATA, 0, sendBuf, sendLen , NULL, NULL);
    apSetResponse(true);
    return ret;
}
/**
 * Set audio volume
 */
uint8_t apVolSet(uint16_t vol)
{
    // Vol range: 0~32
    uint16_t apVol = vol;
    return sendData(EAU_CMD_VOL_SET, apVol, NULL, 0,NULL, 0);
}

/**
 * Set audio volume mute
 */
uint8_t apVolMuteEnable(void)
{
    return sendData(EAU_CMD_MUTE, 0, NULL, 0,NULL, 0);
}

/**
 * Set audio volume unmute
 */
uint8_t apVolMuteDisable(void)
{
    return sendData(EAU_CMD_UNMUTE, 0, NULL, 0,NULL, 0);
}

/**
 * Pause audio decode
 */
uint8_t apPlayPause(void)
{
     return sendData(EAU_CMD_PAUSE, 0, NULL, 0,NULL, 0);
}

/**
 * Resume audio decode
 */
uint8_t apPlayResume(void)
{
    return  sendData(EAU_CMD_RESUME, 0, NULL, 0,NULL, 0);
}

/**
 * Stop audio decode
 */
uint8_t apPlayStop(void)
{
    return sendData(EAU_CMD_STOP, 0, NULL, 0,NULL, 0);
}

/**
 * get song file info
 */
uint8_t apGetInfo(SongInformation *songInfo)
{
    uint8_t   result    = 0;
    uint16_t  revLen    = 0;
    uint8_t   recvbuf[SPIDATABUFMAXLEN];

    result = sendData(EAU_CMD_INFO_GET, 0, NULL, 0, recvbuf, &revLen);

    if(result == OK_RESPONSE)
        MEMCPY((uint8_t *) songInfo, recvbuf, revLen);

    return     result;
}

/**
 * rest decode time
 */
uint8_t apRestDecodeTime(void)
{
    return sendData(EAU_CMD_TIME_REST, 0, NULL, 0,NULL, 0);
}

/**
 * get decode time
 */
uint8_t apGetDecodeTime(DecodeTime *time)
{
    uint8_t   result    = 1;
    uint16_t  revLen    = sizeof(DecodeTime);
    uint8_t   recvbuf[sizeof(DecodeTime)];

    result = sendData(EAU_CMD_TIME_GET, 0, NULL, 0, recvbuf, &revLen);
    if(result == OK_RESPONSE)
        MEMCPY((uint8_t *) time, recvbuf, revLen);

    return     result;
}

/**
 * To set the current mode for encode
 */
uint8_t apSetEncodeMode(void)
{
    return  sendData(EAU_CMD_ENCODE_MODE, 0, NULL, 0, NULL, NULL);
}

/**
 * To set the current mode for decode
 */
uint8_t apSetDecodeMode(void)
{
    return  sendData(EAU_CMD_DECODE_MODE, 0, NULL, 0, NULL, NULL);
}

/**
 * Read the data from mic
 */
uint8_t apMicReadData(uint8_t *readData, uint16_t  *readLen)
{
    uint8_t   result    = 0;
    uint16_t  revLen    = 0;
    result = sendData(EAU_CMD_READ_DATA, 0, NULL, 0 , readData, &revLen);
    if(result == OK_RESPONSE)
    {
        *readLen = revLen;
    }
    return result;
}

/**
 * Lin Switch
 */
uint8_t apLinSwitch(void)
{
    return  sendData(EAU_CMD_LIN_SWITCH, 0, NULL, 0, NULL, NULL);
}

/**
 * Mic Switch
 */
uint8_t apMicSwitch(void)
{
    return  sendData(EAU_CMD_MIC_SWITCH, 0, NULL, 0, NULL, NULL);
}

/**
 * start encode
 */
uint8_t apEncodeStart(void)
{
    return  sendData(EAU_CMD_ENCODE_START, 0, NULL, 0, NULL, NULL);
}

/**
 * set encode info such as num_channels bitrate
 */
uint8_t apSetEncodeInfo(setEnccodeInfo *encodeInfo)
{
    if(encodeInfo == NULL)
        return ERROR_OTHER;
    return  sendData(EAU_CMD_INFO_SET, 0, (uint8_t *)encodeInfo, sizeof(setEnccodeInfo), NULL, NULL);
}

/**
 * set decode info such as num_channels bitrate
 */
uint8_t apSetDecodeInfo(setDecodeInfo *decodeInfo)
{
    if(decodeInfo == NULL)
        return ERROR_OTHER;
    return  sendData(EAU_CMD_INFO_SET, 0, (uint8_t *)decodeInfo, sizeof(setDecodeInfo), NULL, NULL);
}

/**
 * The wait time setup command sent
 */
uint8_t apSetWaitTime(uint32_t waitTime)
{
    if(waitTime < 1000)
        return ERROR_PARAMETER;
    g_waitTime = waitTime;
    return OK_RESPONSE;
}

void apSetResponse(bool responseEnable)
{
    g_noResponse = !responseEnable;
}
