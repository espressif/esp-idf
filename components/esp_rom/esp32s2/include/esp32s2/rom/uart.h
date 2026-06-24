/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_UART_H_
#define _ROM_UART_H_

#include "esp_types.h"
#include "esp_attr.h"
#include "ets_sys.h"
#include "soc/soc.h"
#include "soc/uart_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup uart_apis, uart configuration and communication related apis
  * @brief uart apis
  */

/** @addtogroup uart_apis
  * @{
  */

#define RX_BUFF_SIZE                     0x400
#define TX_BUFF_SIZE                     100

//uart int enable register ctrl bits
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
#define FRAME_FLAG                       0x7E

typedef enum {
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
    uint8_t *pRcvMsgBuff;
    uint8_t *pWritePos;
    uint8_t *pReadPos;
    uint8_t  TrigLvl;
    RcvMsgBuffState BuffState;
} RcvMsgBuff;

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

typedef struct {
    UartBautRate     baut_rate;
    UartBitsNum4Char data_bits;
    UartExistParity  exist_parity;
    UartParityMode   parity;    // chip size in byte
    UartStopBitsNum  stop_bits;
    UartFlowCtrl     flow_ctrl;
    uint8_t          buff_uart_no;  //indicate which uart use tx/rx buffer
    RcvMsgBuff       rcv_buff;
//    TrxMsgBuff       trx_buff;
    RcvMsgState      rcv_state;
    int              received;
} UartDevice;

/**
  * @brief Init uart device struct value and reset uart0/uart1 rx.
  *        Please do not call this function in SDK.
  *
  * @param  rxBuffer, must be a pointer to RX_BUFF_SIZE bytes or NULL
  *
  * @return None
  */
void uartAttach(void *rxBuffer);

/**
  * @brief Init uart0 or uart1 for UART download booting mode.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t uart_no : 0 for UART0, else for UART1.
  *
  * @param  uint32_t clock : clock used by uart module, to adjust baudrate.
  *
  * @return None
  */
void Uart_Init(uint8_t uart_no, uint32_t clock);

/**
  * @brief Modify uart baudrate.
  *        This function will reset RX/TX fifo for uart.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @param  uint32_t DivLatchValue : (clock << 4)/baudrate.
  *
  * @return None
  */
void uart_div_modify(uint8_t uart_no, uint32_t DivLatchValue);


/**
  * @brief Re-calculate UART baudrate divisor for a given (changed)
  *        clock speed.
  *        This function will not reset RX/TX fifo for uart.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @param  uint32_t clock : clock used by uart module, to adjust baudrate.
  *
  * @return None
  */
void uart_div_reinit(uint8_t uart_no, uint32_t clock);


/**
  * @brief Init uart0 or uart1 for UART download booting mode.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @param  uint8_t is_sync : 0, only one UART module, easy to detect, wait until detected;
  *                           1, two UART modules, hard to detect, detect and return.
  *
  * @return None
  */
int uart_baudrate_detect(uint8_t uart_no, uint8_t is_sync);

/**
  * @brief Switch printf channel of uart_tx_one_char.
  *        Please do not call this function when printf.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @return None
  */
void uart_tx_switch(uint8_t uart_no);

/**
  * @brief Switch message exchange channel for UART download booting.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @return None
  */
void uart_buff_switch(uint8_t uart_no);

/**
  * @brief Output a char to printf channel, wait until fifo not full.
  *
  * @param  None
  *
  * @return OK.
  */
ETS_STATUS uart_tx_one_char(uint8_t TxChar);

/**
  * @brief Output a char to message exchange channel, wait until fifo not full.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return OK.
  */
ETS_STATUS uart_tx_one_char2(uint8_t TxChar);

/**
  * @brief Wait until uart tx full empty.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @return None.
  */
void uart_tx_flush(uint8_t uart_no);

/**
  * @brief Wait until uart tx full empty and the last char send ok.
  *
  * @param  uart_no : 0 for UART0, 1 for UART1, 2 for UART2
  *
  * The function defined in ROM code has a bug, so we define the correct version
  * here for compatibility.
  */
void uart_tx_wait_idle(uint8_t uart_no);

/**
  * @brief Get an input char from message channel.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *pRxChar : the pointer to store the char.
  *
  * @return OK for successful.
  *         FAIL for failed.
  */
ETS_STATUS uart_rx_one_char(uint8_t *pRxChar);

/**
  * @brief Get an input char from message channel, wait until successful.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return char : input char value.
  */
char uart_rx_one_char_block(void);

/**
  * @brief Get an input string line from message channel.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *pString : the pointer to store the string.
  *
  * @param  uint8_t MaxStrlen : the max string length, include '\0'.
  *
  * @return OK.
  */
ETS_STATUS UartRxString(uint8_t *pString, uint8_t MaxStrlen);

/**
  * @brief Process uart received information in the interrupt handler.
  *        Please do not call this function in SDK.
  *
  * @param  void *para : the message receive buffer.
  *
  * @return None
  */
void uart_rx_intr_handler(void *para);

/**
  * @brief Get an char from receive buffer.
  *        Please do not call this function in SDK.
  *
  * @param  RcvMsgBuff *pRxBuff : the pointer to the struct that include receive buffer.
  *
  * @param  uint8_t *pRxByte : the pointer to store the char.
  *
  * @return OK for successful.
  *         FAIL for failed.
  */
ETS_STATUS uart_rx_readbuff( RcvMsgBuff *pRxBuff, uint8_t *pRxByte);

/**
  * @brief Get all chars from receive buffer.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *pCmdLn : the pointer to store the string.
  *
  * @return OK for successful.
  *         FAIL for failed.
  */
ETS_STATUS UartGetCmdLn(uint8_t *pCmdLn);

/**
  * @brief Get uart configuration struct.
  *        Please do not call this function in SDK.
  *
  * @param  None
  *
  * @return UartDevice * : uart configuration struct pointer.
  */
UartDevice *GetUartDevice(void);

/**
  * @brief Send an packet to download tool, with SLIP escaping.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *p : the pointer to output string.
  *
  * @param  int len : the string length.
  *
  * @return None.
  */
void send_packet(uint8_t *p, int len);

/**
  * @brief Receive an packet from download tool, with SLIP escaping.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *p : the pointer to input string.
  *
  * @param  int len : If string length > len, the string will be truncated.
  *
  * @param  uint8_t is_sync : 0, only one UART module;
  *                           1, two UART modules.
  *
  * @return int : the length of the string.
  */
int recv_packet(uint8_t *p, int len, uint8_t is_sync);

/**
  * @brief Send an packet to download tool, with SLIP escaping.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *pData : the pointer to input string.
  *
  * @param  uint16_t DataLen : the string length.
  *
  * @return OK for successful.
  *         FAIL for failed.
  */
ETS_STATUS SendMsg(uint8_t *pData, uint16_t DataLen);

/**
  * @brief Receive an packet from download tool, with SLIP escaping.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t *pData : the pointer to input string.
  *
  * @param  uint16_t MaxDataLen : If string length > MaxDataLen, the string will be truncated.
  *
  * @param  uint8_t is_sync : 0, only one UART module;
  *                           1, two UART modules.
  *
  * @return OK for successful.
  *         FAIL for failed.
  */
ETS_STATUS RcvMsg(uint8_t *pData, uint16_t MaxDataLen, uint8_t is_sync);

/**
  * @brief Check if this UART is in download connection.
  *        Please do not call this function in SDK.
  *
  * @param  uint8_t uart_no : 0 for UART0, 1 for UART1.
  *
  * @return ETS_NO_BOOT = 0 for no.
  *         SEL_UART_BOOT = BIT(1) for yes.
  */
uint8_t UartConnCheck(uint8_t uart_no);

/**
  * @brief Initialize the USB ACM UART
  * Needs to be fed a buffer of at least 128 bytes (ESP_ROM_CDC_ACM_WORK_BUF_MIN),
  * plus any rx buffer you may want to have.
  *
  * @param cdc_acm_work_mem Pointer to work mem for CDC-ACM code
  * @param cdc_acm_work_mem_len Length of work mem
  */
void Uart_Init_USB(void *cdc_acm_work_mem, int cdc_acm_work_mem_len);


/**
  * @brief Install handler to reset the chip when a RTS change has been detected on the CDC-ACM 'UART'.
  */
void uart_usb_enable_reset_on_rts(void);


extern UartDevice UartDev;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_UART_H_ */
