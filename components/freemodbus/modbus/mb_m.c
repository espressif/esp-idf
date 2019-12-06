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
#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii_m.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 1
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBMasterDestAddress;
static BOOL     xMBRunInMasterMode = FALSE;
static volatile eMBMasterErrorEventType eMBMasterCurErrorType;

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
eMBErrorCode
eMBMasterInit( eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
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
        vMBMasterRunResRelease( );
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
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    int             i , j;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBMasterEventType    eEvent;
    eMBMasterErrorEventType errorType;

    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED ) {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBMasterPortEventGet( &eEvent ) == TRUE ) {
        switch ( eEvent )
        {
        case EV_MASTER_NO_EVENT:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_NO_EVENT", __func__);
            // Something went wrong and task unblocked but there are no any events set
            assert(0);
            break;
        case EV_MASTER_PROCESS_SUCCESS:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_PROCESS_SUCCESS", __func__);
            break;
        case EV_MASTER_ERROR_RESPOND_TIMEOUT:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_ERROR_RESPOND_TIMEOUT", __func__);
            break;
        case EV_MASTER_ERROR_RECEIVE_DATA:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_ERROR_RECEIVE_DATA", __func__);
            break;
        case EV_MASTER_ERROR_EXECUTE_FUNCTION:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_ERROR_EXECUTE_FUNCTION", __func__);
            break;
        case EV_MASTER_READY:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_READY", __func__);
            break;
        case EV_MASTER_FRAME_RECEIVED:
            eStatus = peMBMasterFrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength );
            // Check if the frame is for us. If not ,send an error process event.
            if ( ( eStatus == MB_ENOERR ) && ( ucRcvAddress == ucMBMasterGetDestAddress() ) ) {
                ESP_LOGD(MB_PORT_TAG, "%s: Packet data received successfully (%u).", __func__, eStatus);
                ( void ) xMBMasterPortEventPost( EV_MASTER_EXECUTE );
            } else {
                vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                ESP_LOGD(MB_PORT_TAG, "%s: Packet data receive failed (addr=%u)(%u).", __func__, ucRcvAddress, eStatus);
                ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
            }
            break;
        case EV_MASTER_EXECUTE:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_EXECUTE", __func__);
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            /* If receive frame has exception. The receive function code highest bit is 1.*/
            if(ucFunctionCode >> 7) {
                eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF];
            } else {
                for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++) {
                    /* No more function handlers registered. Abort. */
                    if (xMasterFuncHandlers[i].ucFunctionCode == 0) {
                        break;
                    }
                    else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode) {
                        vMBMasterSetCBRunInMasterMode(TRUE);
                        /* If master request is broadcast,
                         * the master need execute function for all slave.
                         */
                        if ( xMBMasterRequestIsBroadcast() ) {
                            usLength = usMBMasterGetPDUSndLength();
                            for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++){
                                vMBMasterSetDestAddress(j);
                                eException = xMasterFuncHandlers[i].pxHandler(ucMBFrame, &usLength);
                            }
                        } else {
                            eException = xMasterFuncHandlers[i].pxHandler(ucMBFrame, &usLength);
                        }
                        vMBMasterSetCBRunInMasterMode(FALSE);
                        break;
                    }
                }
            }
            /* If master has exception ,Master will send error process.Otherwise the Master is idle.*/
            if (eException != MB_EX_NONE) {
                vMBMasterSetErrorType(EV_ERROR_EXECUTE_FUNCTION);
                ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
            } else {
                vMBMasterCBRequestSuccess( );
                vMBMasterRunResRelease( );
            }
            break;
        case EV_MASTER_FRAME_SENT:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_FRAME_SENT", __func__);
            /* Master is busy now. */
            vMBMasterGetPDUSndBuf( &ucMBFrame );
            eStatus = peMBMasterFrameSendCur( ucMBMasterGetDestAddress(), ucMBFrame, usMBMasterGetPDUSndLength() );
            if (eStatus != MB_ENOERR) {
                ESP_LOGD(MB_PORT_TAG, "%s:Frame send error. %d", __func__, eStatus);
            }

            break;
        case EV_MASTER_FRAME_TRANSMITTED:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_FRAME_TRANSMITTED", __func__);
            break;
        case EV_MASTER_ERROR_PROCESS:
            ESP_LOGD(MB_PORT_TAG, "%s:EV_MASTER_ERROR_PROCESS", __func__);
            /* Execute specified error process callback function. */
            errorType = eMBMasterGetErrorType();
            vMBMasterGetPDUSndBuf( &ucMBFrame );
            switch (errorType) {
            case EV_ERROR_RESPOND_TIMEOUT:
                vMBMasterErrorCBRespondTimeout(ucMBMasterGetDestAddress(),
                        ucMBFrame, usMBMasterGetPDUSndLength());
                break;
            case EV_ERROR_RECEIVE_DATA:
                vMBMasterErrorCBReceiveData(ucMBMasterGetDestAddress(),
                        ucMBFrame, usMBMasterGetPDUSndLength());
                break;
            case EV_ERROR_EXECUTE_FUNCTION:
                vMBMasterErrorCBExecuteFunction(ucMBMasterGetDestAddress(),
                        ucMBFrame, usMBMasterGetPDUSndLength());
                break;
            default:
                ESP_LOGD(MB_PORT_TAG, "%s: incorrect error type.", __func__);
                break;
            }
            vMBMasterRunResRelease();
            break;
        }
    } else {
        // xMBMasterPortEventGet has unbloked the task but the event bits are not set
        ESP_LOGD(MB_PORT_TAG, "%s: task event wait failure.", __func__);
    }
    return MB_ENOERR;
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
eMBMasterErrorEventType eMBMasterGetErrorType( void )
{
    return eMBMasterCurErrorType;
}

// Set Modbus Master current error event type.
void IRAM_ATTR vMBMasterSetErrorType( eMBMasterErrorEventType errorType )
{
    eMBMasterCurErrorType = errorType;
}

#endif // MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
