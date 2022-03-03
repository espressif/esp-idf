/*
 * SPDX-FileCopyrightText: 2006 Christian Walter
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 TCP Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
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
    UCHAR ucSlaveAddr;          /*!< Slave short address */
    UCHAR* pucRcvBuf;           /*!< Receive buffer pointer */
    USHORT usRcvPos;            /*!< Receive buffer position */
    int pcPort;                 /*!< TCP/UDP port number */
    eMBPortProto xMbProto;      /*!< Protocol type */
    int64_t xSendTimeStamp;     /*!< Send request time stamp */
    int64_t xRecvTimeStamp;     /*!< Receive response time stamp */
    uint16_t usTidCnt;          /*!< Transaction identifier (TID) for slave */
} MbSlaveInfo_t;

typedef struct {
    TaskHandle_t  xMbTcpTaskHandle;     /*!< Master TCP/UDP handling task handle */
    QueueHandle_t xConnectQueue;        /*!< Master connection queue */
    USHORT usPort;                      /*!< Master TCP/UDP port number */
    USHORT usMbSlaveInfoCount;          /*!< Master count of connected slaves */
    USHORT ucCurSlaveIndex;             /*!< Master current processing slave index */
    eMBPortIpVer eMbIpVer;              /*!< Master IP version */
    eMBPortProto eMbProto;              /*!< Master protocol type */
    void* pvNetIface;                   /*!< Master netif interface pointer */
    MbSlaveInfo_t** pxMbSlaveInfo;      /*!< Master information structure for each connected slave */
    MbSlaveInfo_t* pxMbSlaveCurrInfo;   /*!< Master current slave information */
} MbPortConfig_t;

typedef struct {
    USHORT usIndex;                     /*!< index of the address info */
    const char* pcIPAddr;               /*!< represents the IP address of the slave */
    UCHAR ucSlaveAddr;                  /*!< slave unit ID (UID) field for MBAP frame  */
} MbSlaveAddrInfo_t;

/* ----------------------- Function prototypes ------------------------------*/

// The functions below are used by Modbus controller interface to configure Modbus port.
/**
 * Registers slave IP address
 *
 * @param usIndex index of element in the configuration
 * @param pcIpStr IP address to register
 * @param ucSlaveAddress slave element index
 *
 * @return TRUE if address registered successfully, else FALSE
 */
BOOL xMBTCPPortMasterAddSlaveIp(const USHORT usIndex, const CHAR* pcIpStr, UCHAR ucSlaveAddress);

/**
 * Keeps FSM event handle and mask then wait for Master stack to start
 *
 * @param xEventHandle Master event handle
 * @param xEvent event mask to start Modbus stack FSM
 * @param usTimeout - timeout in ticks to wait for stack to start
 *
 * @return TRUE if stack started, else FALSE
 */
BOOL xMBTCPPortMasterWaitEvent(EventGroupHandle_t xEventHandle, EventBits_t xEvent, USHORT usTimeout);

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

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
