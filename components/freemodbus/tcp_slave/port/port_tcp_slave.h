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
    QueueHandle_t xRespQueueHandle;      /*!< Response queue handle */
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

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
