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
 * File: $Id: port.c,v 1.2 2006/09/04 14:39:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_timer.h"
#include "sys/time.h"
#include "esp_netif.h"

/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "net/if.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "mbframe.h"
#include "port_tcp_slave.h"
#include "esp_modbus_common.h"      // for common types for network options

#if MB_TCP_ENABLED

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DISCONNECT_TIMEOUT       ( CONFIG_FMB_TCP_CONNECTION_TOUT_SEC * 1000000 ) // disconnect timeout in uS
#define MB_TCP_RESP_TIMEOUT_MS          ( MB_MASTER_TIMEOUT_MS_RESPOND - 1 ) // slave response time limit
#define MB_TCP_NET_LISTEN_BACKLOG       ( SOMAXCONN )

/* ----------------------- Prototypes ---------------------------------------*/
void vMBPortEventClose( void );

/* ----------------------- Static variables ---------------------------------*/
static const char *TAG = "MB_TCP_SLAVE_PORT";
static int xListenSock = -1;
static SemaphoreHandle_t xShutdownSemaphore = NULL;
static MbSlavePortConfig_t xConfig = { 0 };

/* ----------------------- Static functions ---------------------------------*/
// The helper function to get time stamp in microseconds
static int64_t xMBTCPGetTimeStamp(void)
{
    int64_t xTimeStamp = esp_timer_get_time();
    return xTimeStamp;
}

static void vxMBTCPPortMStoTimeVal(USHORT usTimeoutMs, struct timeval *pxTimeout)
{
    pxTimeout->tv_sec = usTimeoutMs / 1000;
    pxTimeout->tv_usec = (usTimeoutMs - (pxTimeout->tv_sec * 1000)) * 1000;
}

static QueueHandle_t xMBTCPPortRespQueueCreate(void)
{
    QueueHandle_t xRespQueueHandle = xQueueCreate(2, sizeof(void*));
    MB_PORT_CHECK((xRespQueueHandle != NULL), NULL, "TCP respond queue creation failure.");
    return xRespQueueHandle;
}

static void vMBTCPPortRespQueueDelete(QueueHandle_t xRespQueueHandle)
{
    vQueueDelete(xRespQueueHandle);
}

static void* vxMBTCPPortRespQueueRecv(QueueHandle_t xRespQueueHandle)
{
    void* pvResp = NULL;
    MB_PORT_CHECK(xRespQueueHandle != NULL, NULL, "Response queue is not initialized.");
    BaseType_t xStatus = xQueueReceive(xRespQueueHandle,
                                        (void*)&pvResp,
                                        pdMS_TO_TICKS(MB_TCP_RESP_TIMEOUT_MS));
    MB_PORT_CHECK((xStatus == pdTRUE), NULL, "Could not get respond confirmation.");
    MB_PORT_CHECK((pvResp), NULL, "Incorrect response processing.");
    return pvResp;
}

static BOOL vxMBTCPPortRespQueueSend(QueueHandle_t xRespQueueHandle, void* pvResp)
{
    MB_PORT_CHECK(xRespQueueHandle != NULL, FALSE, "Response queue is not initialized.");
    BaseType_t xStatus = xQueueSend(xConfig.xRespQueueHandle,
                                    (const void*)&pvResp,
                                    pdMS_TO_TICKS(MB_TCP_RESP_TIMEOUT_MS));
    MB_PORT_CHECK((xStatus == pdTRUE), FALSE, "FAIL to send to response queue.");
    return TRUE;
}

static void vMBTCPPortServerTask(void *pvParameters);

/* ----------------------- Begin implementation -----------------------------*/
BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
    BOOL bOkay = FALSE;

    xConfig.pxMbClientInfo = calloc(MB_TCP_PORT_MAX_CONN + 1, sizeof(MbClientInfo_t*));
    if (!xConfig.pxMbClientInfo) {
        ESP_LOGE(TAG, "TCP client info allocation failure.");
        return FALSE;
    }
    for(int idx = 0; idx < MB_TCP_PORT_MAX_CONN; xConfig.pxMbClientInfo[idx] = NULL, idx++);

    xConfig.xRespQueueHandle = xMBTCPPortRespQueueCreate();
    if (!xConfig.xRespQueueHandle) {
        ESP_LOGE(TAG, "Response queue allocation failure.");
        return FALSE;
    }

    xConfig.usPort = usTCPPort;
    xConfig.eMbProto = MB_PROTO_TCP;
    xConfig.usClientCount = 0;
    xConfig.pvNetIface = NULL;
    xConfig.xIpVer = MB_PORT_IPV4;
    xConfig.pcBindAddr = NULL;

    // Create task for packet processing
    BaseType_t xErr = xTaskCreatePinnedToCore(vMBTCPPortServerTask,
                                    "tcp_slave_task",
                                    MB_TCP_STACK_SIZE,
                                    NULL,
                                    MB_TCP_TASK_PRIO,
                                    &xConfig.xMbTcpTaskHandle,
                                    MB_PORT_TASK_AFFINITY);
    vTaskSuspend(xConfig.xMbTcpTaskHandle);
    if (xErr != pdTRUE)
    {
        ESP_LOGE(TAG, "Server task creation failure.");
        vTaskDelete(xConfig.xMbTcpTaskHandle);
    } else {
        ESP_LOGI(TAG, "Protocol stack initialized.");
        bOkay = TRUE;
    }
    return bOkay;
}

void vMBTCPPortSlaveSetNetOpt(void* pvNetIf, eMBPortIpVer xIpVersion, eMBPortProto xProto, CHAR* pcBindAddrStr)
{
    // Set network options
    xConfig.pvNetIface = pvNetIf;
    xConfig.eMbProto = xProto;
    xConfig.xIpVer = xIpVersion;
    xConfig.pcBindAddr = pcBindAddrStr;
}

static int xMBTCPPortAcceptConnection(int xListenSockId, char** pcIPAddr)
{
    MB_PORT_CHECK(pcIPAddr, -1, "Wrong IP address pointer.");
    MB_PORT_CHECK((xListenSockId > 0), -1, "Incorrect listen socket ID.");

    // Address structure large enough for both IPv4 or IPv6 address
    struct sockaddr_storage xSrcAddr;
    CHAR cAddrStr[128];
    int xSockId = -1;
    CHAR* pcStr = NULL;
    socklen_t xSize = sizeof(struct sockaddr_storage);

    // Accept new socket connection if not active
    xSockId = accept(xListenSockId, (struct sockaddr *)&xSrcAddr, &xSize);
    if (xSockId < 0) {
        ESP_LOGE(TAG, "Unable to accept connection: errno=%d", errno);
        close(xSockId);
    } else {
        // Get the sender's ip address as string
        if (xSrcAddr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&xSrcAddr)->sin_addr.s_addr, cAddrStr, sizeof(cAddrStr) - 1);
        }
#if CONFIG_LWIP_IPV6
        else if (xSrcAddr.ss_family == PF_INET6) {
            inet6_ntoa_r(((struct sockaddr_in6 *)&xSrcAddr)->sin6_addr, cAddrStr, sizeof(cAddrStr) - 1);
        }
#endif
        else {
            // Make sure ss_family is valid
            abort();
        }
        ESP_LOGI(TAG, "Socket (#%d), accept client connection from address: %s", xSockId, cAddrStr);
        pcStr = calloc(1, strlen(cAddrStr) + 1);
        if (pcStr && pcIPAddr) {
            memcpy(pcStr, cAddrStr, strlen(cAddrStr));
            pcStr[strlen(cAddrStr)] = '\0';
            *pcIPAddr = pcStr; // Set IP address of connected client
        }
    }
    return xSockId;
}

static BOOL xMBTCPPortCloseConnection(MbClientInfo_t* pxInfo)
{
    MB_PORT_CHECK(pxInfo, FALSE, "Client info is NULL.");

    if (pxInfo->xSockId == -1) {
        ESP_LOGE(TAG, "Wrong socket info or disconnected socket: %d.", pxInfo->xSockId);
        return FALSE;
    }
    if (shutdown(pxInfo->xSockId, SHUT_RDWR) == -1) {
        ESP_LOGE(TAG, "Socket (#%d), shutdown failed: errno %d", pxInfo->xSockId, errno);
    }
    close(pxInfo->xSockId);
    pxInfo->xSockId = -1;
    if (xConfig.usClientCount) {
        xConfig.usClientCount--; // decrement counter of client connections
    } else {
        xConfig.pxCurClientInfo = NULL;
    }
    return TRUE;
}

static int xMBTCPPortRxPoll(MbClientInfo_t* pxClientInfo, ULONG xTimeoutMs)
{
    int xRet = ERR_CLSD;
    struct timeval xTimeVal;
    fd_set xReadSet;
    int64_t xStartTimeStamp = 0;

    // Receive data from connected client
    if (pxClientInfo && pxClientInfo->xSockId > -1) {
        // Set receive timeout
        vxMBTCPPortMStoTimeVal(xTimeoutMs, &xTimeVal);
        xStartTimeStamp = xMBTCPGetTimeStamp();
        while (1)
        {
            FD_ZERO(&xReadSet);
            FD_SET(pxClientInfo->xSockId, &xReadSet);
            xRet = select(pxClientInfo->xSockId + 1, &xReadSet, NULL, NULL, &xTimeVal);
            if (xRet == -1)
            {
                // If select an error occurred
                xRet = ERR_CLSD;
                break;
            } else if (xRet == 0) {
                // timeout occurred
                if ((xStartTimeStamp + xTimeoutMs * 1000) > xMBTCPGetTimeStamp()) {
                    ESP_LOGD(TAG, "Socket (#%d) Read timeout.", pxClientInfo->xSockId);
                    xRet = ERR_TIMEOUT;
                    break;
                }
            }
            if (FD_ISSET(pxClientInfo->xSockId, &xReadSet)) {
                // If new buffer received then read Modbus packet into buffer
                MB_PORT_CHECK((pxClientInfo->usTCPBufPos + pxClientInfo->usTCPFrameBytesLeft < MB_TCP_BUF_SIZE),
                                    ERR_BUF, "Socket (#%d), incorrect request buffer size = %d, ignore.",
                                    pxClientInfo->xSockId,
                                    (pxClientInfo->usTCPBufPos + pxClientInfo->usTCPFrameBytesLeft));
                int xLength = recv(pxClientInfo->xSockId, &pxClientInfo->pucTCPBuf[pxClientInfo->usTCPBufPos],
                                      pxClientInfo->usTCPFrameBytesLeft, MSG_DONTWAIT);
                if (xLength < 0) {
                    // If an error occurred during receiving
                    ESP_LOGE(TAG, "Receive failed: length=%d, errno=%d", xLength, errno);
                    xRet = (err_t)xLength;
                    break;
                } else if (xLength == 0) {
                    // Socket connection closed
                    ESP_LOGD(TAG, "Socket (#%d)(%s), connection closed.",
                                                        pxClientInfo->xSockId, pxClientInfo->pcIpAddr);
                    xRet = ERR_CLSD;
                    break;
                } else {
                    // New data received
                    pxClientInfo->usTCPBufPos += xLength;
                    pxClientInfo->usTCPFrameBytesLeft -= xLength;
                    if (pxClientInfo->usTCPBufPos >= MB_TCP_FUNC) {
                        // Length is a byte count of Modbus PDU (function code + data) and the
                        // unit identifier.
                        xLength = (int)MB_TCP_GET_FIELD(pxClientInfo->pucTCPBuf, MB_TCP_LEN);
                        // Is the frame already complete.
                        if (pxClientInfo->usTCPBufPos < (MB_TCP_UID + xLength)) {
                            // The incomplete frame is received
                            pxClientInfo->usTCPFrameBytesLeft = xLength + MB_TCP_UID - pxClientInfo->usTCPBufPos;
                        } else if (pxClientInfo->usTCPBufPos == (MB_TCP_UID + xLength)) {
#if MB_TCP_DEBUG
                            prvvMBTCPLogFrame(TAG, (UCHAR*)&pxClientInfo->pucTCPBuf[0], pxClientInfo->usTCPBufPos);
#endif
                            // Copy TID field from incoming packet
                            pxClientInfo->usTidCnt = MB_TCP_GET_FIELD(pxClientInfo->pucTCPBuf, MB_TCP_TID);
                            xRet = pxClientInfo->usTCPBufPos;
                            break;
                        } else if ((pxClientInfo->usTCPBufPos + xLength) >= MB_TCP_BUF_SIZE) {
                            ESP_LOGE(TAG, "Incorrect buffer received (%u) bytes.", xLength);
                            // This should not happen. We can't deal with such a client and
                            // drop the connection for security reasons.
                            xRet = ERR_BUF;
                            break;
                        }
                    } // if ( pxClientInfo->usTCPBufPos >= MB_TCP_FUNC )
                } // if data received
            } // if (FD_ISSET(pxClientInfo->xSockId, &xReadSet))
        } // while (1)
    }
    return (xRet);
}

// Create a listening socket on pcBindIp: Port
static int
vMBTCPPortBindAddr(const CHAR* pcBindIp)
{
    int xPar, xRet;
    int xListenSockFd = -1;
    struct addrinfo xHint;
    struct addrinfo* pxAddrList;
    struct addrinfo* pxCurAddr;
    CHAR* pcStr = NULL;

    memset( &xHint, 0, sizeof( xHint ) );

    // Bind to IPv6 and/or IPv4, but only in the desired protocol
    // Todo: Find a reason why AF_UNSPEC does not work for IPv6
    xHint.ai_family = (xConfig.xIpVer == MB_PORT_IPV4) ? AF_INET : AF_INET6;
    xHint.ai_socktype = (xConfig.eMbProto == MB_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    // The LWIP has an issue when connection to IPv6 socket
    xHint.ai_protocol = (xConfig.eMbProto == MB_PROTO_UDP) ? IPPROTO_UDP : IPPROTO_TCP;
    xHint.ai_flags = AI_NUMERICSERV;

    if (pcBindIp == NULL) {
        xHint.ai_flags |= AI_PASSIVE;
    } else {
        xHint.ai_flags |= AI_CANONNAME;
    }

    if (asprintf(&pcStr, "%u", xConfig.usPort) == -1) {
        abort();
    }

    xRet = getaddrinfo(pcBindIp, pcStr, &xHint, &pxAddrList);
    free(pcStr);

    if (xRet != 0) {
        return -1;
    }

    // Try the sockaddr until a binding succeeds
    for (pxCurAddr = pxAddrList; pxCurAddr != NULL; pxCurAddr = pxCurAddr->ai_next)
    {
        xListenSockFd = (int)socket(pxCurAddr->ai_family, pxCurAddr->ai_socktype,
                                        pxCurAddr->ai_protocol);
        if (xListenSockFd < 0)
        {
            continue;
        }

        xPar = 1;
        // Allow multi client connections
        if (setsockopt(xListenSockFd, SOL_SOCKET, SO_REUSEADDR,
                        (const char*)&xPar, sizeof(xPar)) != 0)
        {
            close(xListenSockFd);
            xListenSockFd = -1;
            continue;
        }

        if (bind(xListenSockFd, (struct sockaddr *)pxCurAddr->ai_addr,
                                        (socklen_t)pxCurAddr->ai_addrlen) != 0 )
        {
            close(xListenSockFd);
            xListenSockFd = -1;
            continue;
        }

        // Listen only makes sense for TCP
        if (xConfig.eMbProto == MB_PROTO_TCP)
        {
            if (listen(xListenSockFd, MB_TCP_NET_LISTEN_BACKLOG) != 0)
            {
                ESP_LOGE(TAG, "Error occurred during listen: errno=%d", errno);
                close(xListenSockFd);
                xListenSockFd = -1;
                continue;
            }
        }
        // Bind was successful
        pcStr = (pxCurAddr->ai_canonname == NULL) ? (CHAR*)"\0" : pxCurAddr->ai_canonname;
        ESP_LOGI(TAG, "Socket (#%d), listener %s on port: %d, errno=%d",
                                            xListenSockFd, pcStr, xConfig.usPort, errno);
        break;
    }

    freeaddrinfo(pxAddrList);
    return(xListenSockFd);
}

static void
vMBTCPPortFreeClientInfo(MbClientInfo_t* pxClientInfo)
{
    if (pxClientInfo) {
        if (pxClientInfo->pucTCPBuf) {
            free((void*)pxClientInfo->pucTCPBuf);
        }
        if (pxClientInfo->pcIpAddr) {
            free((void*)pxClientInfo->pcIpAddr);
        }
        free((void*)pxClientInfo);
    }
}


static void vMBTCPPortServerTask(void *pvParameters)
{
    int xErr = 0;
    fd_set xReadSet;
    int i;
    CHAR* pcClientIp = NULL;
    struct timeval xTimeVal;

    // Main connection cycle
    while (1) {
        // Create listen socket
        xListenSock = vMBTCPPortBindAddr(xConfig.pcBindAddr);
        if (xListenSock < 0) {
            continue;
        }

        // Connections handling cycle
        while (1) {
            //clear the socket set
            FD_ZERO(&xReadSet);
            //add master socket to set
            FD_SET(xListenSock, &xReadSet);
            int xMaxSd = xListenSock;
            xConfig.usClientCount = 0;

            vxMBTCPPortMStoTimeVal(1, &xTimeVal);
            // Initialize read set and file descriptor according to
            // all registered connected clients
            for (i = 0; i < MB_TCP_PORT_MAX_CONN; i++) {
                if ((xConfig.pxMbClientInfo[i] != NULL) && (xConfig.pxMbClientInfo[i]->xSockId > 0)) {
                    // calculate max file descriptor for select
                    xMaxSd = (xConfig.pxMbClientInfo[i]->xSockId > xMaxSd) ?
                            xConfig.pxMbClientInfo[i]->xSockId : xMaxSd;
                    FD_SET(xConfig.pxMbClientInfo[i]->xSockId, &xReadSet);
                    xConfig.usClientCount++;
                }
            }

            // Wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
            xErr = select(xMaxSd + 1 , &xReadSet , NULL , NULL , NULL);
            if ((xErr < 0) && (errno != EINTR)) {
                // First check if the task is not flagged for shutdown
                if (xListenSock == -1 && xShutdownSemaphore) {
                    xSemaphoreGive(xShutdownSemaphore);
                    vTaskDelete(NULL);
                }
                // error occurred during wait for read
                ESP_LOGE(TAG, "select() errno = %d.", errno);
                continue;
            } else if (xErr == 0) {
                // If timeout happened, something is wrong
                ESP_LOGE(TAG, "select() timeout, errno = %d.", errno);
            }

            // If something happened on the master socket, then its an incoming connection.
            if (FD_ISSET(xListenSock, &xReadSet) && xConfig.usClientCount < MB_TCP_PORT_MAX_CONN) {
                MbClientInfo_t* pxClientInfo = NULL;
                // find first empty place to insert connection info
                for (i = 0; i < MB_TCP_PORT_MAX_CONN; i++) {
                    pxClientInfo = xConfig.pxMbClientInfo[i];
                    if (pxClientInfo == NULL) {
                        break;
                    }
                }
                // if request for new connection but no space left
                if (pxClientInfo != NULL) {
                    if (xConfig.pxMbClientInfo[MB_TCP_PORT_MAX_CONN] == NULL) {
                        ESP_LOGE(TAG, "Fail to accept connection %d, only %d connections supported.", i + 1, MB_TCP_PORT_MAX_CONN);
                    }
                    xConfig.pxMbClientInfo[MB_TCP_PORT_MAX_CONN] = pxClientInfo; // set last connection info
                } else {
                    // allocate memory for new client info
                    pxClientInfo = calloc(1, sizeof(MbClientInfo_t));
                    if (!pxClientInfo) {
                        ESP_LOGE(TAG, "Client info allocation fail.");
                        vMBTCPPortFreeClientInfo(pxClientInfo);
                        pxClientInfo = NULL;
                    } else {
                        // Accept new client connection
                        pxClientInfo->xSockId = xMBTCPPortAcceptConnection(xListenSock, &pcClientIp);
                        if (pxClientInfo->xSockId < 0) {
                            ESP_LOGE(TAG, "Fail to accept connection for client %d.", (xConfig.usClientCount - 1));
                            // Accept connection fail, then free client info and continue polling.
                            vMBTCPPortFreeClientInfo(pxClientInfo);
                            pxClientInfo = NULL;
                            continue;
                        }
                        pxClientInfo->pucTCPBuf = calloc(MB_TCP_BUF_SIZE, sizeof(UCHAR));
                        if (!pxClientInfo->pucTCPBuf) {
                            ESP_LOGE(TAG, "Fail to allocate buffer for client %d.", (xConfig.usClientCount - 1));
                            vMBTCPPortFreeClientInfo(pxClientInfo);
                            pxClientInfo = NULL;
                            continue;
                        }
                        // Fill the connection info structure
                        xConfig.pxMbClientInfo[i] = pxClientInfo;
                        pxClientInfo->xIndex = i;
                        xConfig.usClientCount++;
                        pxClientInfo->pcIpAddr = pcClientIp;
                        pxClientInfo->xRecvTimeStamp = xMBTCPGetTimeStamp();
                        xConfig.pxMbClientInfo[MB_TCP_PORT_MAX_CONN] = NULL;
                        pxClientInfo->usTCPFrameBytesLeft = MB_TCP_FUNC;
                        pxClientInfo->usTCPBufPos = 0;
                    }
                }
            }
            // Handle data request from client
            if (xErr > 0) {
                // Handling client connection requests
                for (i = 0; i < MB_TCP_PORT_MAX_CONN; i++) {
                    MbClientInfo_t* pxClientInfo = xConfig.pxMbClientInfo[i];
                    if ((pxClientInfo != NULL) && (pxClientInfo->xSockId > 0)) {
                        if (FD_ISSET(pxClientInfo->xSockId, &xReadSet)) {
                            // Other sockets are ready to be read
                            xErr = xMBTCPPortRxPoll(pxClientInfo, MB_TCP_READ_TIMEOUT_MS);
                            // If an invalid data received from socket or connection fail
                            // or if timeout then drop connection and restart
                            if (xErr < 0) {
                                uint64_t xTimeStamp = xMBTCPGetTimeStamp();
                                // If data update is timed out
                                switch(xErr)
                                {
                                    case ERR_TIMEOUT:
                                        ESP_LOGE(TAG, "Socket (#%d)(%s), data receive timeout, time[us]: %d, close active connection.",
                                                                            pxClientInfo->xSockId, pxClientInfo->pcIpAddr,
                                                                            (int)(xTimeStamp - pxClientInfo->xRecvTimeStamp));
                                        break;
                                    case ERR_CLSD:
                                        ESP_LOGE(TAG, "Socket (#%d)(%s), connection closed by peer.",
                                                                            pxClientInfo->xSockId, pxClientInfo->pcIpAddr);
                                        break;
                                    case ERR_BUF:
                                    default:
                                        ESP_LOGE(TAG, "Socket (#%d)(%s), read data error: %d",
                                                                            pxClientInfo->xSockId, pxClientInfo->pcIpAddr, xErr);
                                        break;
                                }

                                if (xShutdownSemaphore) {
                                    xSemaphoreGive(xShutdownSemaphore);
                                    vTaskDelete(NULL);
                                }

                                // Close client connection
                                xMBTCPPortCloseConnection(pxClientInfo);

                                // This client does not respond, then unregister it
                                vMBTCPPortFreeClientInfo(pxClientInfo);
                                xConfig.pxMbClientInfo[i] = NULL;
                                xConfig.pxMbClientInfo[MB_TCP_PORT_MAX_CONN] = NULL;
                                // If no any active connections, break
                                if (!xConfig.usClientCount) {
                                    xConfig.pxCurClientInfo = NULL;
                                    break;
                                }
                            } else {
                                pxClientInfo->xRecvTimeStamp = xMBTCPGetTimeStamp();

                                // set current client info to active client from which we received request
                                xConfig.pxCurClientInfo = pxClientInfo;

                                // Complete frame received, inform state machine to process frame
                                xMBPortEventPost(EV_FRAME_RECEIVED);

                                ESP_LOGD(TAG, "Socket (#%d)(%s), get packet TID=0x%X, %d bytes.",
                                                                    pxClientInfo->xSockId, pxClientInfo->pcIpAddr,
                                                                    pxClientInfo->usTidCnt, xErr);

                                // Wait while response is not processed by stack by timeout
                                UCHAR* pucSentBuffer = vxMBTCPPortRespQueueRecv(xConfig.xRespQueueHandle);
                                if (pucSentBuffer == NULL) {
                                    ESP_LOGE(TAG, "Response time exceeds configured %d [ms], ignore packet.",
                                                                        MB_TCP_RESP_TIMEOUT_MS);
                                } else  {
                                    USHORT usSentTid = MB_TCP_GET_FIELD(pucSentBuffer, MB_TCP_TID);
                                    if (usSentTid != pxClientInfo->usTidCnt) {
                                        ESP_LOGE(TAG, "Sent TID(%x) != Recv TID(%x), ignore packet.",
                                                                            usSentTid, pxClientInfo->usTidCnt);
                                    }
                                }

                                // Get time stamp of last data update
                                pxClientInfo->xSendTimeStamp = xMBTCPGetTimeStamp();
                                ESP_LOGD(TAG, "Client %d, Socket(#%d), processing time = %d (us).",
                                                            pxClientInfo->xIndex, pxClientInfo->xSockId,
                                                            (int)(pxClientInfo->xSendTimeStamp - pxClientInfo->xRecvTimeStamp));
                            }
                        } else {
                            if (pxClientInfo) {
                                // client is not ready to be read
                                int64_t xTime = xMBTCPGetTimeStamp() - pxClientInfo->xRecvTimeStamp;
                                if (xTime > MB_TCP_DISCONNECT_TIMEOUT) {
                                    ESP_LOGE(TAG, "Client %d, Socket(#%d) do not answer for %d (us). Drop connection...",
                                                                    pxClientInfo->xIndex, pxClientInfo->xSockId, (int)(xTime));
                                    xMBTCPPortCloseConnection(pxClientInfo);

                                    // This client does not respond, then delete registered data
                                    vMBTCPPortFreeClientInfo(pxClientInfo);
                                    xConfig.pxMbClientInfo[i] = NULL;
                                }
                            } else {
                                ESP_LOGE(TAG, "Client %d is disconnected.", i);
                            }
                        }
                    } // if ((pxClientInfo != NULL)
                } // Handling client connection requests
            }
        } // while(1) // Handle connection cycle
    } // Main connection cycle
    vTaskDelete(NULL);
}

void
vMBTCPPortClose( )
{
    // Release resources for the event queue.

    // Try to exit the task gracefully, so select could release its internal callbacks
    // that were allocated on the stack of the task we're going to delete
    xShutdownSemaphore = xSemaphoreCreateBinary();
    vTaskResume(xConfig.xMbTcpTaskHandle);
    if (xShutdownSemaphore == NULL || // if no semaphore (alloc issues) or couldn't acquire it, just delete the task
        xSemaphoreTake(xShutdownSemaphore, 2*pdMS_TO_TICKS(CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND)) != pdTRUE) {
        ESP_LOGE(TAG, "Task couldn't exit gracefully within timeout -> abruptly deleting the task");
        vTaskDelete(xConfig.xMbTcpTaskHandle);
    }
    if (xShutdownSemaphore) {
        vSemaphoreDelete(xShutdownSemaphore);
        xShutdownSemaphore = NULL;
    }

    vMBPortEventClose( );
}

void vMBTCPPortEnable( void )
{
    vTaskResume(xConfig.xMbTcpTaskHandle);
}

void
vMBTCPPortDisable( void )
{
    vTaskSuspend(xConfig.xMbTcpTaskHandle);
    for (int i = 0; i < MB_TCP_PORT_MAX_CONN; i++) {
        MbClientInfo_t* pxClientInfo = xConfig.pxMbClientInfo[i];
        if ((pxClientInfo != NULL) && (pxClientInfo->xSockId > 0)) {
            xMBTCPPortCloseConnection(pxClientInfo);
            vMBTCPPortFreeClientInfo(pxClientInfo);
            xConfig.pxMbClientInfo[i] = NULL;
        }
    }
    free(xConfig.pxMbClientInfo);
    close(xListenSock);
    xListenSock = -1;
    vMBTCPPortRespQueueDelete(xConfig.xRespQueueHandle);
}

BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
    BOOL xRet = FALSE;
    if (xConfig.pxCurClientInfo) {
        *ppucMBTCPFrame = &xConfig.pxCurClientInfo->pucTCPBuf[0];
        *usTCPLength = xConfig.pxCurClientInfo->usTCPBufPos;

        // Reset the buffer.
        xConfig.pxCurClientInfo->usTCPBufPos = 0;
        xConfig.pxCurClientInfo->usTCPFrameBytesLeft = MB_TCP_FUNC;
        xRet = TRUE;
    }
    return xRet;
}

BOOL
xMBTCPPortSendResponse( UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
    BOOL bFrameSent = FALSE;
    fd_set xWriteSet;
    fd_set xErrorSet;
    int xErr = -1;
    struct timeval xTimeVal;

    if (xConfig.pxCurClientInfo) {
        FD_ZERO(&xWriteSet);
        FD_ZERO(&xErrorSet);
        FD_SET(xConfig.pxCurClientInfo->xSockId, &xWriteSet);
        FD_SET(xConfig.pxCurClientInfo->xSockId, &xErrorSet);
        vxMBTCPPortMStoTimeVal(MB_TCP_SEND_TIMEOUT_MS, &xTimeVal);
        // Check if socket writable
        xErr = select(xConfig.pxCurClientInfo->xSockId + 1, NULL, &xWriteSet, &xErrorSet, &xTimeVal);
        if ((xErr == -1) || FD_ISSET(xConfig.pxCurClientInfo->xSockId, &xErrorSet)) {
            ESP_LOGE(TAG, "Socket(#%d) , send select() error = %d.",
                    xConfig.pxCurClientInfo->xSockId, errno);
            return FALSE;
        }

        // Apply TID field from request to the frame before send response
        pucMBTCPFrame[MB_TCP_TID] = (UCHAR)(xConfig.pxCurClientInfo->usTidCnt >> 8U);
        pucMBTCPFrame[MB_TCP_TID + 1] = (UCHAR)(xConfig.pxCurClientInfo->usTidCnt & 0xFF);

        // Write message into socket and disable Nagle's algorithm
        xErr = send(xConfig.pxCurClientInfo->xSockId, pucMBTCPFrame, usTCPLength, TCP_NODELAY);
        if (xErr < 0) {
            ESP_LOGE(TAG, "Socket(#%d), fail to send data, errno = %d",
                    xConfig.pxCurClientInfo->xSockId, errno);
            xConfig.pxCurClientInfo->xError = xErr;
        } else {
            bFrameSent = TRUE;
            vxMBTCPPortRespQueueSend(xConfig.xRespQueueHandle, (void*)pucMBTCPFrame);
        }
    } else {
        ESP_LOGD(TAG, "Port is not active. Release lock.");
        vxMBTCPPortRespQueueSend(xConfig.xRespQueueHandle, (void*)pucMBTCPFrame);
    }
    return bFrameSent;
}

#endif //#if MB_TCP_ENABLED
