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
  * FreeModbus Libary: ESP32 TCP Port
  * Copyright (C) 2006 Christian Walter <wolti@sil.at>
  * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
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
  * File: $Id: port.h,v 1.2 2006/09/04 14:39:20 wolti Exp $
  */

#ifndef _PORT_TCP_SLAVE_H
#define _PORT_TCP_SLAVE_H

/* ----------------------- Platform includes --------------------------------*/
#include "esp_log.h"

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "port.h"
#include "esp_modbus_common.h"      // for common types for network options

/* ----------------------- Defines ------------------------------------------*/

#ifndef TRUE
#define TRUE                    1
#endif

#ifndef FALSE
#define FALSE                   0
#endif

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Type definitions ---------------------------------*/
typedef struct {
    int xIndex;                     /*!< Modbus info index */
    int xSockId;                    /*!< Socket id */
    int xError;                     /*!< TCP/UDP sock error */
    const char* pcIpAddr;           /*!< TCP/UDP IP address (string) */
    UCHAR* pucTCPBuf;               /*!< buffer pointer */
    USHORT usTCPBufPos;             /*!< buffer active position */
    USHORT usTCPFrameBytesLeft;     /*!< buffer left bytes to receive transaction */
    int64_t xSendTimeStamp;         /*!< send request timestamp */
    int64_t xRecvTimeStamp;         /*!< receive response timestamp */
    USHORT usTidCnt;                /*!< last TID counter from packet */
} MbClientInfo_t;

typedef struct {
    TaskHandle_t xMbTcpTaskHandle;      /*!< Server task handle */
    xQueueHandle xRespQueueHandle;      /*!< Response queue handle */
    MbClientInfo_t* pxCurClientInfo;    /*!< Current client info */
    MbClientInfo_t** pxMbClientInfo;    /*!< Pointers to information about connected clients */
    USHORT usPort;                      /*!< TCP/UDP port number */
    CHAR* pcBindAddr;                   /*!< IP address to bind */
    eMBPortProto eMbProto;              /*!< Protocol type used by port */
    USHORT usClientCount;               /*!< Client connection count */
    void* pvNetIface;                   /*!< Network netif interface pointer for port */
    eMBPortIpVer xIpVer;                /*!< IP protocol version */
} MbSlavePortConfig_t;

/* ----------------------- Function prototypes ------------------------------*/

/**
 * Function to setup communication options for TCP/UDP Modbus port
 *
 * @param pvNetIf netif interface pointer
 * @param xIpVersion IP version
 * @param xProto protocol type option
 * @param pcBindAddr IP bind address
 *
 * @return error code
 */
void vMBTCPPortSlaveSetNetOpt(void* pvNetIf, eMBPortIpVer xIpVersion, eMBPortProto xProto, CHAR* pcBindAddr);

/**
 * Resume TCP Slave processing task
 *
 * @return None
 */
void vMBTCPPortSlaveStartServerTask(void);

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
