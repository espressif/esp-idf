/*
 * SPDX-FileCopyrightText: 2013 Armink
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: port.c,v 1.60 2015/02/01 9:18:05 Armink $
 */

/* ----------------------- System includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sys/lock.h"
#include "port.h"

/* ----------------------- Variables ----------------------------------------*/
static _lock_t s_port_lock;
static UCHAR ucPortMode = 0;

/* ----------------------- Start implementation -----------------------------*/
inline void
vMBPortEnterCritical(void)
{
    _lock_acquire(&s_port_lock);
}

inline void
vMBPortExitCritical(void)
{
    _lock_release(&s_port_lock);
}

UCHAR
ucMBPortGetMode( void )
{
    return ucPortMode;
}

void
vMBPortSetMode( UCHAR ucMode )
{
    ENTER_CRITICAL_SECTION();
    ucPortMode = ucMode;
    EXIT_CRITICAL_SECTION();
}

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_SLAVE_RTU_ENABLED || MB_SLAVE_ASCII_ENABLED

BOOL xMBPortSerialWaitEvent(QueueHandle_t xMbUartQueue, uart_event_t* pxEvent, ULONG xTimeout)
{
    BOOL xResult = (BaseType_t)xQueueReceive(xMbUartQueue, (void*)pxEvent, (TickType_t) xTimeout);
    ESP_LOGD(MB_PORT_TAG, "%s, UART event: %d ", __func__, pxEvent->type);
    return xResult;
}

#endif

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED

/*
 * The function is called from ASCII/RTU module to get processed data buffer. Sets the
 * received buffer and its length using parameters.
 */
__attribute__ ((weak))
BOOL xMBMasterPortSerialGetResponse( UCHAR **ppucMBSerialFrame, USHORT * usSerialLength )
{
    ESP_LOGD(MB_PORT_TAG, " %s default", __func__);
    return TRUE;
}

/*
 * The function is called from ASCII/RTU module to set processed data buffer
 * to be sent in transmitter state machine.
 */
__attribute__ ((weak))
BOOL xMBMasterPortSerialSendRequest( UCHAR *pucMBSerialFrame, USHORT usSerialLength )
{
    ESP_LOGD(MB_PORT_TAG, "%s default", __func__);
    return TRUE;
}

#endif

#if MB_SLAVE_RTU_ENABLED || MB_SLAVE_ASCII_ENABLED

__attribute__ ((weak))
BOOL xMBPortSerialGetRequest( UCHAR **ppucMBSerialFrame, USHORT * usSerialLength )
{
    ESP_LOGD(MB_PORT_TAG, "%s default", __func__);
    return TRUE;
}

__attribute__ ((weak))
BOOL xMBPortSerialSendResponse( UCHAR *pucMBSerialFrame, USHORT usSerialLength )
{
    ESP_LOGD(MB_PORT_TAG, "%s default", __func__);
    return TRUE;
}

#endif

#if MB_TCP_DEBUG

// This function is kept to realize legacy freemodbus frame logging functionality
void
prvvMBTCPLogFrame( const CHAR * pucMsg, UCHAR * pucFrame, USHORT usFrameLen )
{
    int             i;
    int             res = 0;
    int             iBufPos = 0;
    size_t          iBufLeft = MB_TCP_FRAME_LOG_BUFSIZE;
    static CHAR     arcBuffer[MB_TCP_FRAME_LOG_BUFSIZE];

    assert( pucFrame != NULL );

    for ( i = 0; i < usFrameLen; i++ ) {
        // Print some additional frame information.
        switch ( i )
        {
        case 0:
            // TID = Transaction Identifier.
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, "| TID = " );
            break;
        case 2:
            // PID = Protocol Identifier.
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | PID = " );
            break;
        case 4:
            // Length
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | LEN = " );
            break;
        case 6:
            // UID = Unit Identifier.
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | UID = " );
            break;
        case 7:
            // MB Function Code.
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | FUNC = " );
            break;
        case 8:
            // MB PDU rest.
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | DATA = " );
            break;
        default:
            res = 0;
            break;
        }
        if( res == -1 ) {
            break;
        }
        else {
            iBufPos += res;
            iBufLeft -= res;
        }

        // Print the data.
        res = snprintf( &arcBuffer[iBufPos], iBufLeft, "%02X", pucFrame[i] );
        if( res == -1 ) {
            break;
        } else {
            iBufPos += res;
            iBufLeft -= res;
        }
    }

    if( res != -1 ) {
        // Append an end of frame string.
        res = snprintf( &arcBuffer[iBufPos], iBufLeft, " |" );
        if( res != -1 ) {
            ESP_LOGD(pucMsg, "%s", arcBuffer);
        }
    }
}
#endif
