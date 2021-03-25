/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbport.h,v 1.19 2010/06/06 13:54:40 wolti Exp $
 */

#ifndef _MB_PORT_H
#define _MB_PORT_H

#include "mbconfig.h"   // for options

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#if CONFIG_UART_ISR_IN_IRAM
#define MB_PORT_SERIAL_ISR_FLAG ESP_INTR_FLAG_IRAM
#else
#define MB_PORT_SERIAL_ISR_FLAG ESP_INTR_FLAG_LOWMED
#endif

#if MB_PORT_TIMER_ISR_IN_IRAM
#define MB_PORT_ISR_ATTR IRAM_ATTR
#define MB_PORT_TIMER_ISR_FLAG ESP_INTR_FLAG_IRAM
#else
#define MB_PORT_ISR_ATTR
#define MB_PORT_TIMER_ISR_FLAG ESP_INTR_FLAG_LOWMED
#endif

/* ----------------------- Type definitions ---------------------------------*/

typedef enum
{
    EV_READY = 0x01,                   /*!< Startup finished. */
    EV_FRAME_RECEIVED = 0x02,          /*!< Frame received. */
    EV_EXECUTE = 0x04,                 /*!< Execute function. */
    EV_FRAME_SENT = 0x08,              /*!< Frame sent. */
    EV_FRAME_TRANSMIT = 0x10           /*!< Frame transmit. */
} eMBEventType;

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
typedef enum {
    EV_MASTER_NO_EVENT = 0x0000,
    EV_MASTER_READY = 0x0001,                   /*!< Startup finished. */
    EV_MASTER_FRAME_RECEIVED = 0x0002,          /*!< Frame received. */
    EV_MASTER_EXECUTE = 0x0004,                 /*!< Execute function. */
    EV_MASTER_FRAME_SENT = 0x0008,              /*!< Frame sent. */
    EV_MASTER_FRAME_TRANSMIT = 0x0010,          /*!< Frame transmission. */
    EV_MASTER_ERROR_PROCESS = 0x0020,           /*!< Frame error process. */
    EV_MASTER_PROCESS_SUCCESS = 0x0040,         /*!< Request process success. */
    EV_MASTER_ERROR_RESPOND_TIMEOUT = 0x0080,   /*!< Request respond timeout. */
    EV_MASTER_ERROR_RECEIVE_DATA = 0x0100,      /*!< Request receive data error. */
    EV_MASTER_ERROR_EXECUTE_FUNCTION = 0x0200   /*!< Request execute function error. */
} eMBMasterEventType;

typedef enum {
    EV_ERROR_INIT,             /*!< No error, initial state. */
    EV_ERROR_RESPOND_TIMEOUT,  /*!< Slave respond timeout. */
    EV_ERROR_RECEIVE_DATA,     /*!< Receive frame data error. */
    EV_ERROR_EXECUTE_FUNCTION, /*!< Execute function error. */
    EV_ERROR_OK                /*!< No error, processing completed. */
} eMBMasterErrorEventType;
#endif

/*! \ingroup modbus
 * \brief Parity used for characters in serial mode.
 *
 * The parity which should be applied to the characters sent over the serial
 * link. Please note that this values are actually passed to the porting
 * layer and therefore not all parity modes might be available.
 */
typedef enum
{
    MB_PAR_NONE,                /*!< No parity. */
    MB_PAR_ODD,                 /*!< Odd parity. */
    MB_PAR_EVEN                 /*!< Even parity. */
} eMBParity;

/* ----------------------- Supporting functions -----------------------------*/
BOOL            xMBPortEventInit( void );

BOOL            xMBPortEventPost( eMBEventType eEvent );

BOOL            xMBPortEventGet(  /*@out@ */ eMBEventType * eEvent );

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
BOOL            xMBMasterPortEventInit( void );

BOOL            xMBMasterPortEventPost( eMBMasterEventType eEvent );

BOOL            xMBMasterPortEventGet(  /*@out@ */ eMBMasterEventType * eEvent );

void            vMBMasterOsResInit( void );

BOOL            xMBMasterRunResTake( LONG time );

void            vMBMasterRunResRelease( void );
#endif // #if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
/* ----------------------- Serial port functions ----------------------------*/

BOOL            xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate,
                                   UCHAR ucDataBits, eMBParity eParity );

void            vMBPortClose( void );

void            xMBPortSerialClose( void );

void            vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable );

BOOL            xMBPortSerialGetByte( CHAR * pucByte );

BOOL            xMBPortSerialPutByte( CHAR ucByte );

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
BOOL            xMBMasterPortSerialInit( UCHAR ucPort, ULONG ulBaudRate,
                                   UCHAR ucDataBits, eMBParity eParity );

void            vMBMasterPortClose( void );

void            xMBMasterPortSerialClose( void );

void            vMBMasterPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable );

BOOL            xMBMasterPortSerialGetByte( CHAR * pucByte );

BOOL            xMBMasterPortSerialPutByte( CHAR ucByte );
#endif

/* ----------------------- Timers functions ---------------------------------*/
BOOL            xMBPortTimersInit( USHORT usTimeOut50us );

void            xMBPortTimersClose( void );

void            vMBPortTimersEnable( void );

void            vMBPortTimersDisable( void );

void            vMBPortTimersDelay( USHORT usTimeOutMS );

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
BOOL            xMBMasterPortTimersInit( USHORT usTimeOut50us );

void            xMBMasterPortTimersClose( void );

void            vMBMasterPortTimersT35Enable( void );

void            vMBMasterPortTimersConvertDelayEnable( void );

void            vMBMasterPortTimersRespondTimeoutEnable( void );

void            vMBMasterPortTimersDisable( void );
#endif

/* ----------------- Callback for the master error process ------------------*/
void            vMBMasterErrorCBRespondTimeout( UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                                USHORT ucPDULength );

void            vMBMasterErrorCBReceiveData( UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                             USHORT ucPDULength );

void            vMBMasterErrorCBExecuteFunction( UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                                 USHORT ucPDULength );

void            vMBMasterCBRequestSuccess( void );

/* ----------------------- Callback for the protocol stack ------------------*/
/*!
 * \brief Callback function for the porting layer when a new byte is
 *   available.
 *
 * Depending upon the mode this callback function is used by the RTU or
 * ASCII transmission layers. In any case a call to xMBPortSerialGetByte()
 * must immediately return a new character.
 *
 * \return <code>TRUE</code> if a event was posted to the queue because
 *   a new byte was received. The port implementation should wake up the
 *   tasks which are currently blocked on the eventqueue.
 */
extern          BOOL( *pxMBFrameCBByteReceived ) ( void );

extern          BOOL( *pxMBFrameCBTransmitterEmpty ) ( void );

extern          BOOL( *pxMBPortCBTimerExpired ) ( void );
#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
extern          BOOL( *pxMBMasterFrameCBByteReceived ) ( void );

extern          BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( void );

extern          BOOL( *pxMBMasterPortCBTimerExpired ) ( void );
#endif
/* ----------------------- TCP port functions -------------------------------*/
#if MB_TCP_ENABLED
BOOL            xMBTCPPortInit( USHORT usTCPPort );

void            vMBTCPPortClose( void );

void            vMBTCPPortDisable( void );

BOOL            xMBTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength );

BOOL            xMBTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
