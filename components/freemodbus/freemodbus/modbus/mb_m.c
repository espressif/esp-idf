/*
 * SPDX-FileCopyrightText: 2013 Armink
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
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
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/20 11:18:10 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mbport.h"
#if MB_MASTER_RTU_ENABLED
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED
#include "mbtcp.h"
#include "mbtcp_m.h"
#endif

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_MASTER_TCP_ENABLED

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 1
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR ucMBMasterDestAddress;
static BOOL xMBRunInMasterMode = FALSE;
static volatile eMBMasterErrorEventType eMBMasterCurErrorType;
static volatile USHORT usMasterSendPDULength;
static volatile eMBMode eMBMasterCurrentMode;

/*------------------------ Shared variables ---------------------------------*/

volatile UCHAR ucMasterSndBuf[MB_SERIAL_BUF_SIZE];
volatile UCHAR ucMasterRcvBuf[MB_SERIAL_BUF_SIZE];
volatile eMBMasterTimerMode eMasterCurTimerMode;
volatile BOOL xFrameIsBroadcast = FALSE;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 * Using for Modbus Master,Add by Armink 20130813
 */
static peMBFrameSend peMBMasterFrameSendCur;
static pvMBFrameStart pvMBMasterFrameStartCur;
static pvMBFrameStop pvMBMasterFrameStopCur;
static peMBFrameReceive peMBMasterFrameReceiveCur;
static pvMBFrameClose pvMBMasterFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 * Using for Modbus Master,Add by Armink 20130813
 */
BOOL( *pxMBMasterFrameCBByteReceived ) ( void );

BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( void );

BOOL( *pxMBMasterPortCBTimerExpired ) ( void );

BOOL( *pxMBMasterFrameCBReceiveFSMCur ) ( void );

BOOL( *pxMBMasterFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
#if MB_MASTER_TCP_ENABLED
eMBErrorCode
eMBMasterTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBMasterTCPDoInit( ucTCPPort ) ) != MB_ENOERR ) {
        eMBState = STATE_DISABLED;
    }
    else if( !xMBMasterPortEventInit(  ) ) {
        /* Port dependent event module initialization failed. */
        eStatus = MB_EPORTERR;
    } else {
        pvMBMasterFrameStartCur = eMBMasterTCPStart;
        pvMBMasterFrameStopCur = eMBMasterTCPStop;
        peMBMasterFrameReceiveCur = eMBMasterTCPReceive;
        peMBMasterFrameSendCur = eMBMasterTCPSend;
        pxMBMasterPortCBTimerExpired = xMBMasterTCPTimerExpired;
        pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterTCPPortClose : NULL;
        ucMBMasterDestAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBMasterCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;

        // initialize the OS resource for modbus master.
        vMBMasterOsResInit();
        if( xMBMasterPortTimersInit( MB_MASTER_TIMEOUT_MS_RESPOND * MB_TIMER_TICS_PER_MS ) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }

    }
    return eStatus;
}
#endif

eMBErrorCode
eMBMasterSerialInit( eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    switch (eMode)
    {
#if MB_MASTER_RTU_ENABLED > 0
    case MB_RTU:
        pvMBMasterFrameStartCur = eMBMasterRTUStart;
        pvMBMasterFrameStopCur = eMBMasterRTUStop;
        peMBMasterFrameSendCur = eMBMasterRTUSend;
        peMBMasterFrameReceiveCur = eMBMasterRTUReceive;
        pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
        pxMBMasterFrameCBByteReceived = xMBMasterRTUReceiveFSM;
        pxMBMasterFrameCBTransmitterEmpty = xMBMasterRTUTransmitFSM;
        pxMBMasterPortCBTimerExpired = xMBMasterRTUTimerExpired;
        eMBMasterCurrentMode = MB_ASCII;

        eStatus = eMBMasterRTUInit(ucPort, ulBaudRate, eParity);
        break;
#endif
#if MB_MASTER_ASCII_ENABLED > 0
    case MB_ASCII:
        pvMBMasterFrameStartCur = eMBMasterASCIIStart;
        pvMBMasterFrameStopCur = eMBMasterASCIIStop;
        peMBMasterFrameSendCur = eMBMasterASCIISend;
        peMBMasterFrameReceiveCur = eMBMasterASCIIReceive;
        pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
        pxMBMasterFrameCBByteReceived = xMBMasterASCIIReceiveFSM;
        pxMBMasterFrameCBTransmitterEmpty = xMBMasterASCIITransmitFSM;
        pxMBMasterPortCBTimerExpired = xMBMasterASCIITimerT1SExpired;
        eMBMasterCurrentMode = MB_RTU;

        eStatus = eMBMasterASCIIInit(ucPort, ulBaudRate, eParity );
        break;
#endif
    default:
        eStatus = MB_EINVAL;
        break;
    }

    if (eStatus == MB_ENOERR)
    {
        if (!xMBMasterPortEventInit())
        {
            /* port dependent event module initalization failed. */
            eStatus = MB_EPORTERR;
        }
        else
        {
            eMBState = STATE_DISABLED;
        }
        /* initialize the OS resource for modbus master. */
        vMBMasterOsResInit();
    }
    return eStatus;
}

eMBErrorCode
eMBMasterClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBMasterFrameCloseCur != NULL )
        {
            pvMBMasterFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBMasterFrameStartCur(  );
        /* Release the resource, because it created in busy state */
        //vMBMasterRunResRelease( );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        pvMBMasterFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterPoll( void )
{
    static UCHAR    *ucMBSendFrame = NULL;
    static UCHAR    *ucMBRcvFrame = NULL;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;
    static BOOL     xTransactionIsActive = FALSE;
    int             i;
    int             j;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBMasterEventType    eEvent;
    eMBMasterErrorEventType errorType;

    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if ( xMBMasterPortEventGet( &eEvent ) == TRUE )
    {
        while( eEvent ) {
            // In some cases it is possible that more than one event set
            // together (even from one subset mask) than process them consistently
            if ( MB_PORT_CHECK_EVENT( eEvent, EV_MASTER_READY ) ) {
                ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_READY", __func__);
                MB_PORT_CLEAR_EVENT( eEvent, EV_MASTER_READY );
            } else if ( MB_PORT_CHECK_EVENT( eEvent, EV_MASTER_FRAME_TRANSMIT ) ) {
                ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_FRAME_TRANSMIT", __func__);
                /* Master is busy now. */
                vMBMasterGetPDUSndBuf( &ucMBSendFrame );
                ESP_LOG_BUFFER_HEX_LEVEL("POLL transmit buffer", (void*)ucMBSendFrame, usMBMasterGetPDUSndLength(), ESP_LOG_DEBUG);
                eStatus = peMBMasterFrameSendCur( ucMBMasterGetDestAddress(), ucMBSendFrame, usMBMasterGetPDUSndLength() );
                if (eStatus != MB_ENOERR)
                {
                    vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                    ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                    ESP_LOGE( MB_PORT_TAG, "%s:Frame send error. %d", __func__, eStatus );
                }
                MB_PORT_CLEAR_EVENT( eEvent, EV_MASTER_FRAME_TRANSMIT );
            } else if ( MB_PORT_CHECK_EVENT( eEvent, EV_MASTER_FRAME_SENT ) ) {
                ESP_LOGD( MB_PORT_TAG, "%s:EV_MASTER_FRAME_SENT", __func__ );
                ESP_LOG_BUFFER_HEX_LEVEL("POLL sent buffer", (void*)ucMBSendFrame, usMBMasterGetPDUSndLength(), ESP_LOG_DEBUG);
                xTransactionIsActive = TRUE;
                MB_PORT_CLEAR_EVENT( eEvent, EV_MASTER_FRAME_SENT );
            } else if ( MB_PORT_CHECK_EVENT( eEvent, EV_MASTER_FRAME_RECEIVED ) ) {
                if (xTransactionIsActive) {
                    eStatus = peMBMasterFrameReceiveCur( &ucRcvAddress, &ucMBRcvFrame, &usLength);
                    MB_PORT_CHECK(ucMBRcvFrame, MB_EILLSTATE, "Receive buffer initialization fail.");
                    MB_PORT_CHECK(ucMBSendFrame, MB_EILLSTATE, "Send buffer initialization fail.");
                    // Check if the frame is for us. If not ,send an error process event.
                    if ( ( eStatus == MB_ENOERR ) && ( ( ucRcvAddress == ucMBMasterGetDestAddress() )
                                                    || ( ucRcvAddress == MB_TCP_PSEUDO_ADDRESS) ) ) {
                        if ( ( ucMBRcvFrame[MB_PDU_FUNC_OFF]  & ~MB_FUNC_ERROR ) == ( ucMBSendFrame[MB_PDU_FUNC_OFF] ) ) {
                            ESP_LOGD(MB_PORT_TAG, "%s: Packet data received successfully (%u).", __func__, eStatus);
                            ESP_LOG_BUFFER_HEX_LEVEL("POLL receive buffer", (void*)ucMBRcvFrame, (uint16_t)usLength, ESP_LOG_DEBUG);

                            ( void ) xMBMasterPortEventPost( EV_MASTER_EXECUTE );
                        } else {
                            ESP_LOGE( MB_PORT_TAG, "Drop incorrect frame, receive_func(%u) != send_func(%u)",
                                            ucMBRcvFrame[MB_PDU_FUNC_OFF], ucMBSendFrame[MB_PDU_FUNC_OFF]);
                            vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                            ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                        }
                    } else {
                        vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                        ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                        ESP_LOGD( MB_PORT_TAG, "%s: Packet data receive failed (addr=%u)(%u).",
                                               __func__, ucRcvAddress, eStatus);
                    }
                } else {
                    // Ignore the `EV_MASTER_FRAME_RECEIVED` event because the respond timeout occurred
                    // and this is likely respond to previous transaction
                    ESP_LOGE( MB_PORT_TAG, "Drop data received outside of transaction.");
                }
                MB_PORT_CLEAR_EVENT( eEvent, EV_MASTER_FRAME_RECEIVED );
            } else if ( MB_PORT_CHECK_EVENT( eEvent, EV_MASTER_EXECUTE ) ) {
                MB_PORT_CHECK(ucMBRcvFrame, MB_EILLSTATE, "receive buffer initialization fail.");
                ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_EXECUTE", __func__);
                ucFunctionCode = ucMBRcvFrame[MB_PDU_FUNC_OFF];
                eException = MB_EX_ILLEGAL_FUNCTION;
                /* If receive frame has exception. The receive function code highest bit is 1.*/
                if (ucFunctionCode & MB_FUNC_ERROR)
                {
                    eException = (eMBException)ucMBRcvFrame[MB_PDU_DATA_OFF];
                } else {
                    for ( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
                    {
                        /* No more function handlers registered. Abort. */
                        if (xMasterFuncHandlers[i].ucFunctionCode == 0)
                        {
                            break;
                        }
                        if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode)
                        {
                            vMBMasterSetCBRunInMasterMode(TRUE);
                            /* If master request is broadcast,
                             * the master need execute function for all slave.
                             */
                            if ( xMBMasterRequestIsBroadcast() )
                            {
                                usLength = usMBMasterGetPDUSndLength();
                                for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++)
                                {
                                    vMBMasterSetDestAddress(j);
                                    eException = xMasterFuncHandlers[i].pxHandler(ucMBRcvFrame, &usLength);
                                }
                            }
                            else
                            {
                                eException = xMasterFuncHandlers[i].pxHandler( ucMBRcvFrame, &usLength );
                            }
                            vMBMasterSetCBRunInMasterMode( FALSE );
                            break;
                        }
                    }
                }
                /* If master has exception, will send error process event. Otherwise the master is idle.*/
                if ( eException != MB_EX_NONE )
                {
                    vMBMasterSetErrorType( EV_ERROR_EXECUTE_FUNCTION );
                    ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                }
                else
                {
                    if ( eMBMasterGetErrorType( ) == EV_ERROR_INIT ) {
                        vMBMasterSetErrorType(EV_ERROR_OK);
                        ESP_LOGD( MB_PORT_TAG, "%s: set event EV_ERROR_OK", __func__ );
                        ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                    }
                }
                MB_PORT_CLEAR_EVENT( eEvent, EV_MASTER_EXECUTE );
            } else if ( MB_PORT_CHECK_EVENT( eEvent, EV_MASTER_ERROR_PROCESS ) ) {
                ESP_LOGD( MB_PORT_TAG, "%s:EV_MASTER_ERROR_PROCESS", __func__ );
                /* Execute specified error process callback function. */
                errorType = eMBMasterGetErrorType( );
                vMBMasterGetPDUSndBuf( &ucMBSendFrame );
                switch ( errorType )
                {
                    case EV_ERROR_RESPOND_TIMEOUT:
                        vMBMasterErrorCBRespondTimeout( ucMBMasterGetDestAddress( ),
                                ucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                        break;
                    case EV_ERROR_RECEIVE_DATA:
                        vMBMasterErrorCBReceiveData( ucMBMasterGetDestAddress( ),
                                ucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                        break;
                    case EV_ERROR_EXECUTE_FUNCTION:
                        vMBMasterErrorCBExecuteFunction( ucMBMasterGetDestAddress( ),
                                ucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                        break;
                    case EV_ERROR_OK:
                        vMBMasterCBRequestSuccess( );
                        break;
                    default:
                        ESP_LOGE( MB_PORT_TAG, "%s: incorrect error type = %d.", __func__, errorType);
                        break;
                }
                vMBMasterPortTimersDisable( );
                vMBMasterSetErrorType( EV_ERROR_INIT );
                MB_PORT_CLEAR_EVENT( eEvent, EV_MASTER_ERROR_PROCESS );
                vMBMasterRunResRelease( );
            }
        }
    } else {
        // Something went wrong and task unblocked but there are no any correct events set
        ESP_LOGE( MB_PORT_TAG, "%s: Unexpected event triggered 0x%02x.", __func__, eEvent );
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

// Get whether the Modbus Master is run in master mode.
BOOL xMBMasterGetCBRunInMasterMode( void )
{
    return xMBRunInMasterMode;
}

// Set whether the Modbus Master is run in master mode.
void vMBMasterSetCBRunInMasterMode( BOOL IsMasterMode )
{
    xMBRunInMasterMode = IsMasterMode;
}

// Get Modbus Master send destination address.
UCHAR ucMBMasterGetDestAddress( void )
{
    return ucMBMasterDestAddress;
}

// Set Modbus Master send destination address.
void vMBMasterSetDestAddress( UCHAR Address )
{
    ucMBMasterDestAddress = Address;
}

// Get Modbus Master current error event type.
eMBMasterErrorEventType inline eMBMasterGetErrorType( void )
{
    return eMBMasterCurErrorType;
}

// Set Modbus Master current error event type.
void IRAM_ATTR vMBMasterSetErrorType( eMBMasterErrorEventType errorType )
{
    eMBMasterCurErrorType = errorType;
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf( UCHAR ** pucFrame )
{
    *pucFrame = ( UCHAR * ) &ucMasterSndBuf[MB_SEND_BUF_PDU_OFF];
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength( USHORT SendPDULength )
{
    usMasterSendPDULength = SendPDULength;
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength( void )
{
    return usMasterSendPDULength;
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode( eMBMasterTimerMode eMBTimerMode )
{
    eMasterCurTimerMode = eMBTimerMode;
}

/* Get Modbus Master current timer mode.*/
eMBMasterTimerMode MB_PORT_ISR_ATTR xMBMasterGetCurTimerMode( void )
{
    return eMasterCurTimerMode;
}

/* The master request is broadcast? */
BOOL MB_PORT_ISR_ATTR xMBMasterRequestIsBroadcast( void )
{
    return xFrameIsBroadcast;
}

/* The master request is broadcast? */
void vMBMasterRequestSetType( BOOL xIsBroadcast )
{
    xFrameIsBroadcast = xIsBroadcast;
}

// Get Modbus Master communication mode.
eMBMode ucMBMasterGetCommMode(void)
{
    return eMBMasterCurrentMode;
}

#endif // MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_MASTER_TCP_ENABLED
