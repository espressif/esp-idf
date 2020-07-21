/* Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
  * FreeModbus Libary: RT-Thread Port
  * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id: port.c,v 1.60 2015/02/01 9:18:05 Armink $
  */

/* ----------------------- System includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "freertos/FreeRTOS.h"
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
