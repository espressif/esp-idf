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

#ifndef _ROM_UART_H_
#define _ROM_UART_H_

#include "esp_types.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RX_BUFF_SIZE                     0x100
#define TX_BUFF_SIZE                     100

//uart int enalbe register ctrl bits
#define UART_RCV_INTEN                   BIT0
#define UART_TRX_INTEN                   BIT1
#define UART_LINE_STATUS_INTEN           BIT2

//uart int identification ctrl bits
#define UART_INT_FLAG_MASK               0x0E

//uart fifo ctrl bits
#define UART_CLR_RCV_FIFO                BIT1
#define UART_CLR_TRX_FIFO                BIT2
#define UART_RCVFIFO_TRG_LVL_BITS        BIT6

//uart line control bits
#define  UART_DIV_LATCH_ACCESS_BIT       BIT7

//uart line status bits
#define  UART_RCV_DATA_RDY_FLAG          BIT0
#define  UART_RCV_OVER_FLOW_FLAG         BIT1
#define  UART_RCV_PARITY_ERR_FLAG        BIT2
#define  UART_RCV_FRAME_ERR_FLAG         BIT3
#define  UART_BRK_INT_FLAG               BIT4
#define  UART_TRX_FIFO_EMPTY_FLAG        BIT5
#define  UART_TRX_ALL_EMPTY_FLAG         BIT6   // include fifo and shift reg
#define  UART_RCV_ERR_FLAG               BIT7

//send and receive message frame head
#define FRAME_FLAG                         0x7E

typedef enum{
    UART_LINE_STATUS_INT_FLAG  = 0x06,
    UART_RCV_FIFO_INT_FLAG     = 0x04,
    UART_RCV_TMOUT_INT_FLAG    = 0x0C,
    UART_TXBUFF_EMPTY_INT_FLAG = 0x02
} UartIntType;   //consider bit0 for int_flag

typedef enum {
    RCV_ONE_BYTE      = 0x0,
    RCV_FOUR_BYTE     = 0x1,
    RCV_EIGHT_BYTE    = 0x2,
    RCV_FOURTEEN_BYTE = 0x3
} UartRcvFifoTrgLvl;

typedef enum {
    FIVE_BITS  = 0x0,
    SIX_BITS   = 0x1,
    SEVEN_BITS = 0x2,
    EIGHT_BITS = 0x3
} UartBitsNum4Char;

typedef enum {
    ONE_STOP_BIT      = 1,
    ONE_HALF_STOP_BIT = 2,
    TWO_STOP_BIT      = 3
} UartStopBitsNum;

typedef enum {
     NONE_BITS = 0,
     ODD_BITS  = 2,
     EVEN_BITS = 3

} UartParityMode;

typedef enum {
    STICK_PARITY_DIS = 0,
    STICK_PARITY_EN  = 2
} UartExistParity;

typedef enum {
    BIT_RATE_9600   = 9600,
    BIT_RATE_19200  = 19200,
    BIT_RATE_38400  = 38400,
    BIT_RATE_57600  = 57600,
    BIT_RATE_115200 = 115200,
    BIT_RATE_230400 = 230400,
    BIT_RATE_460800 = 460800,
    BIT_RATE_921600 = 921600
} UartBautRate;

typedef enum {
    NONE_CTRL,
    HARDWARE_CTRL,
    XON_XOFF_CTRL
} UartFlowCtrl;

typedef enum {
    EMPTY,
    UNDER_WRITE,
    WRITE_OVER 
} RcvMsgBuffState;

typedef struct {
//    uint32_t     RcvBuffSize;
    uint8_t *pRcvMsgBuff;
    uint8_t *pWritePos;
    uint8_t *pReadPos;
    uint8_t  TrigLvl; //JLU: may need to pad
    RcvMsgBuffState BuffState;
}RcvMsgBuff;

typedef struct {
    uint32_t  TrxBuffSize;
    uint8_t  *pTrxBuff;
} TrxMsgBuff;

typedef enum {
    BAUD_RATE_DET,
    WAIT_SYNC_FRM,
    SRCH_MSG_HEAD,
    RCV_MSG_BODY,
    RCV_ESC_CHAR,
} RcvMsgState;

typedef struct{
    UartBautRate     baut_rate;
    UartBitsNum4Char data_bits;
    UartExistParity  exist_parity;
    UartParityMode   parity;    // chip size in byte
    UartStopBitsNum  stop_bits;
    UartFlowCtrl     flow_ctrl;
    uint8_t          buff_uart_no;  //indicate which uart use tx/rx buffer
    uint8_t          tx_uart_no;
    RcvMsgBuff       rcv_buff;
//    TrxMsgBuff       trx_buff;
    RcvMsgState      rcv_state;
    int              received;
} UartDevice;

void Uart_Init(uint8_t uart_no, uint32_t clock) ROMFN_ATTR;
STATUS UartTxString(uint8_t* pString) ROMFN_ATTR;
STATUS UartRxString(uint8_t* pString, uint8_t MaxStrlen) ROMFN_ATTR;

STATUS uart_tx_one_char(uint8_t TxChar) ROMFN_ATTR;//for print
STATUS uart_tx_one_char2(uint8_t TxChar) ROMFN_ATTR;//for send message
STATUS uart_rx_one_char(uint8_t* pRxChar) ROMFN_ATTR;
char uart_rx_one_char_block(void) ROMFN_ATTR;
void uart_rx_intr_handler(void * para) ROMFN_ATTR;
STATUS uart_rx_readbuff( RcvMsgBuff* pRxBuff, uint8_t* pRxByte) ROMFN_ATTR;
STATUS UartGetCmdLn(uint8_t * pCmdLn) ROMFN_ATTR;
UartDevice * GetUartDevice() ROMFN_ATTR;

void uartToggleInterrupt(bool en) ROMFN_ATTR;

STATUS SendMsg(uint8_t *pData, uint16_t DataLen) ROMFN_ATTR;

STATUS RcvMsg(uint8_t *pData, uint16_t MaxDataLen, uint8_t is_sync) ROMFN_ATTR;

void uartAttach() ROMFN_ATTR;
void uart_div_modify(uint8_t uart_no, uint32_t DivLatchValue) ROMFN_ATTR;
int uart_baudrate_detect(uint8_t uart_no, uint8_t is_sync) ROMFN_ATTR;
void uart_buff_switch(uint8_t uart_no)  ROMFN_ATTR;
void uart_tx_flush(uint8_t uart_no) ROMFN_ATTR;
void uart_tx_wait_idle(uint8_t uart_no) ROMFN_ATTR;
extern UartDevice UartDev;

#ifdef __cplusplus
}
#endif

#endif /* _ROM_UART_H_ */
