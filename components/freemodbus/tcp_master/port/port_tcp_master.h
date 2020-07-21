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

#include "lwip/sys.h"
#include "freertos/event_groups.h"
#include "port.h"

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
    int xIndex;                 /*!< Slave information index */
    int xSockId;                /*!< Socket ID of slave */
    int xError;                 /*!< Socket error */
    int xRcvErr;                /*!< Socket receive error */
    const char* pcIpAddr;       /*!< TCP/UDP IP address */
    UCHAR* pucRcvBuf;           /*!< Receive buffer pointer */
    USHORT usRcvPos;            /*!< Receive buffer position */
    int pcPort;                 /*!< TCP/UDP port number */
    eMBPortProto xMbProto;      /*!< Protocol type */
    int64_t xSendTimeStamp;     /*!< Send request time stamp */
    int64_t xRecvTimeStamp;     /*!< Receive response time stamp */
    uint16_t usTidCnt;          /*!< Transaction identifier (TID) for slave */
} MbSlaveInfo_t;

typedef struct {
    TaskHandle_t  xMbTcpTaskHandle; /*!< Master TCP/UDP handling task handle */
    QueueHandle_t xConnectQueue;    /*!< Master connection queue */
    USHORT usPort;                  /*!< Master TCP/UDP port number */
    USHORT usMbSlaveInfoCount;      /*!< Master count of connected slaves */
    USHORT ucCurSlaveIndex;         /*!< Master current processing slave index */
    eMBPortIpVer eMbIpVer;          /*!< Master IP version */
    eMBPortProto eMbProto;          /*!< Master protocol type */
    void* pvNetIface;               /*!< Master netif interface pointer */
    MbSlaveInfo_t** pxMbSlaveInfo;  /*!< Master information structure for each connected slave */
} MbPortConfig_t;

/* ----------------------- Function prototypes ------------------------------*/

// The functions below are used by Modbus controller interface to configure Modbus port.
/**
 * Registers slave IP address
 *
 * @param pcIpStr IP address to register
 *
 * @return TRUE if address registered successfully, else FALSE
 */
BOOL xMBTCPPortMasterAddSlaveIp(const CHAR* pcIpStr);

/**
 * Keeps FSM event handle and mask then wait for Master stack to start
 *
 * @param xEventHandle Master event handle
 * @param xEvent event mask to start Modbus stack FSM
 *
 * @return TRUE if stack started, else FALSE
 */
BOOL xMBTCPPortMasterWaitEvent(EventGroupHandle_t xEventHandle, EventBits_t xEvent);

/**
 * Set network options for Master port
 *
 * @param pvNetIf netif interface pointer
 * @param xIpVersion IP version option for the Master port
 * @param xProto Protocol version option for the Master port
 *
 * @return None
 */
void vMBTCPPortMasterSetNetOpt(void* pvNetIf, eMBPortIpVer xIpVersion, eMBPortProto xProto);

/**
 * Resume TCP/UDP Master processing task
 *
 * @return None
 */
void vMBTCPPortMasterTaskStart(void);

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
