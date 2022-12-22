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

/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_netif.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "port.h"
#include "mbport.h"
#include "mbframe.h"
#include "port_tcp_master.h"

#if MB_MASTER_TCP_ENABLED

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_CONNECTION_TIMEOUT_MS    ( 20 )      // Connection timeout in mS
#define MB_TCP_RECONNECT_TIMEOUT        ( 5000000 ) // Connection timeout in uS

#define MB_EVENT_REQ_DONE_MASK          (   EV_MASTER_PROCESS_SUCCESS | \
                                            EV_MASTER_ERROR_RESPOND_TIMEOUT | \
                                            EV_MASTER_ERROR_RECEIVE_DATA | \
                                            EV_MASTER_ERROR_EXECUTE_FUNCTION )

#define MB_EVENT_REQ_ERR_MASK           ( EV_MASTER_PROCESS_SUCCESS )

#define MB_EVENT_WAIT_TOUT_MS           ( 3000 )

#define MB_TCP_READ_TICK_MS             ( 1 )
#define MB_TCP_READ_BUF_RETRY_CNT       ( 4 )
#define MB_SLAVE_FMT(fmt)               "Slave #%d, Socket(#%d)(%s)"fmt

/* ----------------------- Types & Prototypes --------------------------------*/
void vMBPortEventClose( void );

/* ----------------------- Static variables ---------------------------------*/
static const char *TAG = "MB_TCP_MASTER_PORT";
static MbPortConfig_t xMbPortConfig;
static EventGroupHandle_t xMasterEventHandle = NULL;
static SemaphoreHandle_t xShutdownSemaphore = NULL;
static EventBits_t xMasterEvent = 0;

/* ----------------------- Static functions ---------------------------------*/
static void vMBTCPPortMasterTask(void *pvParameters);

/* ----------------------- Begin implementation -----------------------------*/

// Waits for stack start event to start Modbus event processing
BOOL xMBTCPPortMasterWaitEvent(EventGroupHandle_t xEventHandle, EventBits_t xEvent, USHORT usTimeout)
{
    xMasterEventHandle = xEventHandle;
    xMasterEvent = xEvent;
    BaseType_t status = xEventGroupWaitBits(xMasterEventHandle,
                                               (BaseType_t)(xEvent),
                                               pdFALSE, // do not clear start bit
                                               pdFALSE,
                                               usTimeout);
    return (BOOL)(status & xEvent);
}

BOOL
xMBMasterTCPPortInit( USHORT usTCPPort )
{
    BOOL bOkay = FALSE;

    xMbPortConfig.pxMbSlaveInfo = calloc(MB_TCP_PORT_MAX_CONN, sizeof(MbSlaveInfo_t*));
    if (!xMbPortConfig.pxMbSlaveInfo) {
        ESP_LOGE(TAG, "TCP slave info alloc failure.");
        return FALSE;
    }
    for(int idx = 0; idx < MB_TCP_PORT_MAX_CONN; xMbPortConfig.pxMbSlaveInfo[idx] = NULL, idx++);

    xMbPortConfig.xConnectQueue = NULL;
    xMbPortConfig.usPort = usTCPPort;
    xMbPortConfig.usMbSlaveInfoCount = 0;
    xMbPortConfig.ucCurSlaveIndex = 1;
    xMbPortConfig.pxMbSlaveCurrInfo = NULL;

    xMbPortConfig.xConnectQueue = xQueueCreate(2, sizeof(MbSlaveAddrInfo_t));
    if (xMbPortConfig.xConnectQueue == 0)
    {
        // Queue was not created and must not be used.
        ESP_LOGE(TAG, "TCP master queue creation failure.");
        return FALSE;
    }

    // Create task for packet processing
    BaseType_t xErr = xTaskCreatePinnedToCore(vMBTCPPortMasterTask,
                                    "tcp_master_task",
                                    MB_TCP_STACK_SIZE,
                                    NULL,
                                    MB_TCP_TASK_PRIO,
                                    &xMbPortConfig.xMbTcpTaskHandle,
                                    MB_PORT_TASK_AFFINITY);
    if (xErr != pdTRUE)
    {
        ESP_LOGE(TAG, "TCP master task creation failure.");
        (void)vTaskDelete(xMbPortConfig.xMbTcpTaskHandle);
    } else {
        ESP_LOGI(TAG, "TCP master stack initialized.");
        bOkay = TRUE;
    }

    vTaskSuspend(xMbPortConfig.xMbTcpTaskHandle);
    return bOkay;
}

static MbSlaveInfo_t* vMBTCPPortMasterFindSlaveInfo(UCHAR ucSlaveAddr)
{
    int xIndex;
    BOOL xFound = false;
    for (xIndex = 0; xIndex < xMbPortConfig.usMbSlaveInfoCount; xIndex++) {
        if (xMbPortConfig.pxMbSlaveInfo[xIndex]->ucSlaveAddr == ucSlaveAddr) {
            xMbPortConfig.pxMbSlaveCurrInfo = xMbPortConfig.pxMbSlaveInfo[xIndex];
            xFound = TRUE;
            xMbPortConfig.ucCurSlaveIndex = xIndex;
        }
    }
    if (!xFound) {
        xMbPortConfig.pxMbSlaveCurrInfo = NULL;
        ESP_LOGE(TAG, "Slave info for short address %d not found.", ucSlaveAddr);
    }
    return xMbPortConfig.pxMbSlaveCurrInfo;
}

static MbSlaveInfo_t* vMBTCPPortMasterGetCurrInfo(void)
{
    if (!xMbPortConfig.pxMbSlaveCurrInfo) {
        ESP_LOGE(TAG, "Incorrect current slave info.");
    }
    return xMbPortConfig.pxMbSlaveCurrInfo;
}

// Start Modbus event state machine
static void vMBTCPPortMasterStartPoll(void)
{
    if (xMasterEventHandle) {
        // Set the mbcontroller start flag
        EventBits_t xFlags = xEventGroupSetBits(xMasterEventHandle,
                                                (EventBits_t)xMasterEvent);
        if (!(xFlags & xMasterEvent)) {
            ESP_LOGE(TAG, "Fail to start TCP stack.");
        }
    } else {
        ESP_LOGE(TAG, "Fail to start polling. Incorrect event handle...");
    }
}

// Stop Modbus event state machine
static void vMBTCPPortMasterStopPoll(void)
{
    if (xMasterEventHandle) {
        // Set the mbcontroller start flag
        EventBits_t xFlags = xEventGroupClearBits(xMasterEventHandle,
                                                (EventBits_t)xMasterEvent);
        if (!(xFlags & xMasterEvent)) {
            ESP_LOGE(TAG, "Fail to stop polling.");
        }
    } else {
        ESP_LOGE(TAG, "Fail to stop polling. Incorrect event handle...");
    }
}

// The helper function to get time stamp in microseconds
static int64_t xMBTCPGetTimeStamp(void)
{
    int64_t xTimeStamp = esp_timer_get_time();
    return xTimeStamp;
}

static void vMBTCPPortMasterMStoTimeVal(USHORT usTimeoutMs, struct timeval *tv)
{
    tv->tv_sec = usTimeoutMs / 1000;
    tv->tv_usec = (usTimeoutMs - (tv->tv_sec * 1000)) * 1000;
}

static void xMBTCPPortMasterCheckShutdown(void) {
    // First check if the task is not flagged for shutdown
    if (xShutdownSemaphore) {
        xSemaphoreGive(xShutdownSemaphore);
        vTaskDelete(NULL);
    }
}

static BOOL xMBTCPPortMasterCloseConnection(MbSlaveInfo_t* pxInfo)
{
    if (!pxInfo) {
        return FALSE;
    }
    if (pxInfo->xSockId == -1) {
        ESP_LOGE(TAG, "Wrong socket info or disconnected socket: %d, skip.", pxInfo->xSockId);
        return FALSE;
    }
    if (shutdown(pxInfo->xSockId, SHUT_RDWR) == -1) {
        ESP_LOGV(TAG, "Shutdown failed sock %d, errno=%d", pxInfo->xSockId, errno);
    }
    close(pxInfo->xSockId);
    pxInfo->xSockId = -1;
    return TRUE;
}

void vMBTCPPortMasterSetNetOpt(void* pvNetIf, eMBPortIpVer xIpVersion, eMBPortProto xProto)
{
    xMbPortConfig.pvNetIface = pvNetIf;
    xMbPortConfig.eMbProto = xProto;
    xMbPortConfig.eMbIpVer = xIpVersion;
}

// Function returns time left for response processing according to response timeout
static int64_t xMBTCPPortMasterGetRespTimeLeft(MbSlaveInfo_t* pxInfo)
{
    if (!pxInfo) {
        return 0;
    }
    int64_t xTimeStamp = xMBTCPGetTimeStamp() - pxInfo->xSendTimeStamp;
    return (xTimeStamp > (1000 * MB_MASTER_TIMEOUT_MS_RESPOND)) ? 0 :
                    (MB_MASTER_TIMEOUT_MS_RESPOND - (xTimeStamp / 1000) - 1);
}

// Wait socket ready to read state
static int vMBTCPPortMasterRxCheck(int xSd, fd_set* pxFdSet, int xTimeMs)
{
    fd_set xReadSet = *pxFdSet;
    fd_set xErrorSet = *pxFdSet;
    int xRes = 0;
    struct timeval xTimeout;

    vMBTCPPortMasterMStoTimeVal(xTimeMs, &xTimeout);
    xRes = select(xSd + 1, &xReadSet, NULL, &xErrorSet, &xTimeout);
    if (xRes == 0) {
        // No respond from slave during timeout
        xRes = ERR_TIMEOUT;
    } else if ((xRes < 0) || FD_ISSET(xSd, &xErrorSet)) {
        xRes = -1;
    }

    *pxFdSet = xReadSet;
    return xRes;
}

static int xMBTCPPortMasterGetBuf(MbSlaveInfo_t* pxInfo, UCHAR* pucDstBuf, USHORT usLength, uint16_t xTimeMs)
{
    int xLength = 0;
    UCHAR* pucBuf = pucDstBuf;
    USHORT usBytesLeft = usLength;
    struct timeval xTime;

    MB_PORT_CHECK((pxInfo && pxInfo->xSockId > -1), -1, "Try to read incorrect socket = #%d.", pxInfo->xSockId);

    // Set receive timeout for socket <= slave respond time
    xTime.tv_sec = xTimeMs / 1000;
    xTime.tv_usec = (xTimeMs % 1000) * 1000;
    setsockopt(pxInfo->xSockId, SOL_SOCKET, SO_RCVTIMEO, &xTime, sizeof(xTime));

    // Receive data from connected client
    while (usBytesLeft > 0) {
        xMBTCPPortMasterCheckShutdown();
        xLength = recv(pxInfo->xSockId, pucBuf, usBytesLeft, 0);
        if (xLength < 0) {
            if (errno == EAGAIN) {
                // Read timeout occurred, check the timeout and return
            } else if (errno == ENOTCONN) {
                // Socket connection closed
                ESP_LOGE(TAG, "Socket(#%d)(%s) connection closed.",
                                            pxInfo->xSockId, pxInfo->pcIpAddr);
                return ERR_CONN;
            } else {
                // Other error occurred during receiving
                ESP_LOGE(TAG, "Socket(#%d)(%s) receive error, length=%d, errno=%d",
                                            pxInfo->xSockId, pxInfo->pcIpAddr, xLength, errno);
                return -1;
            }
        } else if (xLength) {
            pucBuf += xLength;
            usBytesLeft -= xLength;
        }
        if (xMBTCPPortMasterGetRespTimeLeft(pxInfo) == 0) {
            return ERR_TIMEOUT;
        }
    }
    return usLength;
}

static int vMBTCPPortMasterReadPacket(MbSlaveInfo_t* pxInfo)
{
    int xLength = 0;
    int xRet = 0;
    USHORT usTidRcv = 0;

    // Receive data from connected client
    if (pxInfo) {
        MB_PORT_CHECK((pxInfo->xSockId > 0), -1, "Try to read incorrect socket = #%d.", pxInfo->xSockId);
        // Read packet header
        xRet = xMBTCPPortMasterGetBuf(pxInfo, &pxInfo->pucRcvBuf[0],
                                        MB_TCP_UID, xMBTCPPortMasterGetRespTimeLeft(pxInfo));
        if (xRet < 0) {
            pxInfo->xRcvErr = xRet;
            return xRet;
        } else if (xRet != MB_TCP_UID) {
            ESP_LOGD(TAG, "Socket (#%d)(%s), Fail to read modbus header. ret=%d",
                                                                pxInfo->xSockId, pxInfo->pcIpAddr, xRet);
            pxInfo->xRcvErr = ERR_VAL;
            return ERR_VAL;
        }
        // If we have received the MBAP header we can analyze it and calculate
        // the number of bytes left to complete the current request.
        xLength = (int)MB_TCP_GET_FIELD(pxInfo->pucRcvBuf, MB_TCP_LEN);
        xRet = xMBTCPPortMasterGetBuf(pxInfo, &pxInfo->pucRcvBuf[MB_TCP_UID],
                                        xLength, xMBTCPPortMasterGetRespTimeLeft(pxInfo));
        if (xRet < 0) {
            pxInfo->xRcvErr = xRet;
            return xRet;
        } else if (xRet != xLength) {
            // Received incorrect or fragmented packet.
            ESP_LOGD(TAG, "Socket(#%d)(%s) incorrect packet, length=%d, TID=0x%02x, errno=%d(%s)",
                                               pxInfo->xSockId, pxInfo->pcIpAddr, pxInfo->usRcvPos,
                                               usTidRcv, errno, strerror(errno));
            pxInfo->xRcvErr = ERR_VAL;
            return ERR_VAL;
        }
        usTidRcv = MB_TCP_GET_FIELD(pxInfo->pucRcvBuf, MB_TCP_TID);

        // Check transaction identifier field in the incoming packet.
        if ((pxInfo->usTidCnt - 1) != usTidRcv) {
            ESP_LOGD(TAG, "Socket (#%d)(%s), incorrect TID(0x%02x)!=(0x%02x) received, discard data.",
                                                pxInfo->xSockId, pxInfo->pcIpAddr, usTidRcv, (pxInfo->usTidCnt - 1));
            pxInfo->xRcvErr = ERR_BUF;
            return ERR_BUF;
        }
        pxInfo->usRcvPos += xRet + MB_TCP_UID;
        ESP_LOGD(TAG, "Socket(#%d)(%s) get data, length=%d, TID=0x%02x, errno=%d(%s)",
                                           pxInfo->xSockId, pxInfo->pcIpAddr, pxInfo->usRcvPos,
                                           usTidRcv, errno, strerror(errno));
        pxInfo->xRcvErr = ERR_OK;
        return pxInfo->usRcvPos;
    }
    return -1;
}

static err_t xMBTCPPortMasterSetNonBlocking(MbSlaveInfo_t* pxInfo)
{
    if (!pxInfo) {
        return ERR_CONN;
    }
    // Set non blocking attribute for socket
    ULONG ulFlags = fcntl(pxInfo->xSockId, F_GETFL);
    if (fcntl(pxInfo->xSockId, F_SETFL, ulFlags | O_NONBLOCK) == -1) {
        ESP_LOGE(TAG, "Socket(#%d)(%s), fcntl() call error=%d",
                                              pxInfo->xSockId, pxInfo->pcIpAddr, errno);
        return ERR_WOULDBLOCK;
    }
    return ERR_OK;
}

static void vMBTCPPortSetKeepAlive(MbSlaveInfo_t* pxInfo)
{
    int optval = 1;
    setsockopt(pxInfo->xSockId, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

// Check connection for timeout helper
static err_t xMBTCPPortMasterCheckAlive(MbSlaveInfo_t* pxInfo, ULONG xTimeoutMs)
{
    fd_set xWriteSet;
    fd_set xErrorSet;
    err_t xErr = -1;
    struct timeval xTimeVal;

    if (pxInfo && pxInfo->xSockId != -1) {
        FD_ZERO(&xWriteSet);
        FD_ZERO(&xErrorSet);
        FD_SET(pxInfo->xSockId, &xWriteSet);
        FD_SET(pxInfo->xSockId, &xErrorSet);
        vMBTCPPortMasterMStoTimeVal(xTimeoutMs, &xTimeVal);
        // Check if the socket is writable
        xErr = select(pxInfo->xSockId + 1, NULL, &xWriteSet, &xErrorSet, &xTimeVal);
        if ((xErr < 0) || FD_ISSET(pxInfo->xSockId, &xErrorSet)) {
            if (errno == EINPROGRESS) {
                xErr = ERR_INPROGRESS;
            } else {
                ESP_LOGV(TAG, MB_SLAVE_FMT(" connection, select write err(errno) = %d(%d)."),
                                                    pxInfo->xIndex, pxInfo->xSockId, pxInfo->pcIpAddr, xErr, errno);
                xErr = ERR_CONN;
            }
        } else if (xErr == 0) {
            ESP_LOGV(TAG, "Socket(#%d)(%s), connection timeout occurred, err(errno) = %d(%d).",
                                        pxInfo->xSockId, pxInfo->pcIpAddr, xErr, errno);
            return ERR_INPROGRESS;
        } else {
            int xOptErr = 0;
            ULONG ulOptLen = sizeof(xOptErr);
            // Check socket error
            xErr = getsockopt(pxInfo->xSockId, SOL_SOCKET, SO_ERROR, (void*)&xOptErr, (socklen_t*)&ulOptLen);
            if (xOptErr != 0) {
                ESP_LOGD(TAG, "Socket(#%d)(%s), sock error occurred (%d).",
                                            pxInfo->xSockId, pxInfo->pcIpAddr, xOptErr);
                return ERR_CONN;
            }
            ESP_LOGV(TAG, "Socket(#%d)(%s), is alive.",
                                        pxInfo->xSockId, pxInfo->pcIpAddr);
            return ERR_OK;
        }
    } else {
        xErr = ERR_CONN;
    }
    return xErr;
}

// Resolve host name and/or fill the IP address structure
static BOOL xMBTCPPortMasterCheckHost(const CHAR* pcHostStr, ip_addr_t* pxHostAddr)
{
    MB_PORT_CHECK((pcHostStr), FALSE, "Wrong host name or IP.");
    CHAR cStr[45];
    CHAR* pcStr = &cStr[0];
    ip_addr_t xTargetAddr;
    struct addrinfo xHint;
    struct addrinfo* pxAddrList;
    memset(&xHint, 0, sizeof(xHint));
    // Do name resolution for both protocols
    xHint.ai_family = AF_UNSPEC;
    xHint.ai_flags = AI_ADDRCONFIG; // get IPV6 address if supported, otherwise IPV4
    memset(&xTargetAddr, 0, sizeof(xTargetAddr));

    // convert domain name to IP address
    // Todo: check EAI_FAIL error when resolve host name
    int xRet = getaddrinfo(pcHostStr, NULL, &xHint, &pxAddrList);

    if (xRet != 0) {
        ESP_LOGE(TAG, "Incorrect host name or IP: %s", pcHostStr);
        return FALSE;
    }
    if (pxAddrList->ai_family == AF_INET) {
        struct in_addr addr4 = ((struct sockaddr_in *) (pxAddrList->ai_addr))->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(&xTargetAddr), &addr4);
        pcStr = ip4addr_ntoa_r(ip_2_ip4(&xTargetAddr), cStr, sizeof(cStr));
    }
#if CONFIG_LWIP_IPV6
    else {
        struct in6_addr addr6 = ((struct sockaddr_in6 *) (pxAddrList->ai_addr))->sin6_addr;
        inet6_addr_to_ip6addr(ip_2_ip6(&xTargetAddr), &addr6);
        pcStr = ip6addr_ntoa_r(ip_2_ip6(&xTargetAddr), cStr, sizeof(cStr));
    }
#endif
    if (pxHostAddr) {
        *pxHostAddr = xTargetAddr;
    }
    ESP_LOGI(TAG, "Host[IP]: \"%s\"[%s]", pxAddrList->ai_canonname, pcStr);
    freeaddrinfo(pxAddrList);
    return TRUE;
}

BOOL xMBTCPPortMasterAddSlaveIp(const USHORT usIndex, const CHAR* pcIpStr, UCHAR ucSlaveAddress)
{
    BOOL xRes = FALSE;
    MbSlaveAddrInfo_t xSlaveAddrInfo = { 0 };
    MB_PORT_CHECK(xMbPortConfig.xConnectQueue != NULL, FALSE, "Wrong slave IP address to add.");
    if (pcIpStr && (usIndex != 0xFF)) {
        xRes = xMBTCPPortMasterCheckHost(pcIpStr, NULL);
    }
    if (xRes || !pcIpStr) {
        xSlaveAddrInfo.pcIPAddr = pcIpStr;
        xSlaveAddrInfo.usIndex = usIndex;
        xSlaveAddrInfo.ucSlaveAddr = ucSlaveAddress;
        BaseType_t xStatus = xQueueSend(xMbPortConfig.xConnectQueue, (void*)&xSlaveAddrInfo, 100);
        MB_PORT_CHECK((xStatus == pdTRUE), FALSE, "FAIL to add slave IP address: [%s].", pcIpStr);
    }
    return xRes;
}

// Unblocking connect function
static err_t xMBTCPPortMasterConnect(MbSlaveInfo_t* pxInfo)
{
    if (!pxInfo) {
        return ERR_CONN;
    }

    err_t xErr = ERR_OK;
    CHAR cStr[128];
    CHAR* pcStr = NULL;
    ip_addr_t xTargetAddr;
    struct addrinfo xHint;
    struct addrinfo* pxAddrList;
    struct addrinfo* pxCurAddr;

    memset(&xHint, 0, sizeof(xHint));
    // Do name resolution for both protocols
    //xHint.ai_family = AF_UNSPEC; Todo: Find a reason why AF_UNSPEC does not work
    xHint.ai_flags = AI_ADDRCONFIG; // get IPV6 address if supported, otherwise IPV4
    xHint.ai_family = (xMbPortConfig.eMbIpVer == MB_PORT_IPV4) ? AF_INET : AF_INET6;
    xHint.ai_socktype = (pxInfo->xMbProto == MB_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    xHint.ai_protocol = (pxInfo->xMbProto == MB_PROTO_UDP) ? IPPROTO_UDP : IPPROTO_TCP;
    memset(&xTargetAddr, 0, sizeof(xTargetAddr));

    if (asprintf(&pcStr, "%u", xMbPortConfig.usPort) == -1) {
        abort();
    }

    // convert domain name to IP address
    int xRet = getaddrinfo(pxInfo->pcIpAddr, pcStr, &xHint, &pxAddrList);
    free(pcStr);
    if (xRet != 0) {
        ESP_LOGE(TAG, "Cannot resolve host: %s", pxInfo->pcIpAddr);
        return ERR_CONN;
    }

    for (pxCurAddr = pxAddrList; pxCurAddr != NULL; pxCurAddr = pxCurAddr->ai_next) {
        if (pxCurAddr->ai_family == AF_INET) {
            struct in_addr addr4 = ((struct sockaddr_in *) (pxCurAddr->ai_addr))->sin_addr;
            inet_addr_to_ip4addr(ip_2_ip4(&xTargetAddr), &addr4);
            pcStr = ip4addr_ntoa_r(ip_2_ip4(&xTargetAddr), cStr, sizeof(cStr));
        }
#if CONFIG_LWIP_IPV6
        else if (pxCurAddr->ai_family == AF_INET6) {
            struct in6_addr addr6 = ((struct sockaddr_in6 *) (pxCurAddr->ai_addr))->sin6_addr;
            inet6_addr_to_ip6addr(ip_2_ip6(&xTargetAddr), &addr6);
            pcStr = ip6addr_ntoa_r(ip_2_ip6(&xTargetAddr), cStr, sizeof(cStr));
            // Set scope id to fix routing issues with local address
            ((struct sockaddr_in6 *) (pxCurAddr->ai_addr))->sin6_scope_id =
                                esp_netif_get_netif_impl_index(xMbPortConfig.pvNetIface);
        }
#endif
        if (pxInfo->xSockId <= 0) {
            pxInfo->xSockId = socket(pxCurAddr->ai_family, pxCurAddr->ai_socktype, pxCurAddr->ai_protocol);
            if (pxInfo->xSockId < 0) {
                ESP_LOGE(TAG, "Unable to create socket: #%d, errno %d", pxInfo->xSockId, errno);
                xErr = ERR_IF;
                continue;
            }
        } else {
            ESP_LOGV(TAG, "Socket (#%d)(%s) created.", pxInfo->xSockId, cStr);
        }

        // Set non blocking attribute for socket
        xMBTCPPortMasterSetNonBlocking(pxInfo);

        // Can return EINPROGRESS as an error which means
        // that connection is in progress and should be checked later
        xErr = connect(pxInfo->xSockId, (struct sockaddr*)pxCurAddr->ai_addr, pxCurAddr->ai_addrlen);
        if ((xErr < 0) && (errno == EINPROGRESS || errno == EALREADY)) {
            // The unblocking connect is pending (check status later) or already connected
            ESP_LOGV(TAG, "Socket(#%d)(%s) connection is pending, errno %d (%s).",
                                        pxInfo->xSockId, cStr, errno, strerror(errno));

            // Set keep alive flag in socket options
            vMBTCPPortSetKeepAlive(pxInfo);
            xErr = xMBTCPPortMasterCheckAlive(pxInfo, MB_TCP_CONNECTION_TIMEOUT_MS);
            continue;
        } else if ((xErr < 0) && (errno == EISCONN)) {
            // Socket already connected
            xErr = ERR_OK;
            continue;
        } else if (xErr != ERR_OK) {
            // Other error occurred during connection
            ESP_LOGV(TAG, MB_SLAVE_FMT(" unable to connect, error=%d, errno %d (%s)"),
                                                pxInfo->xIndex, pxInfo->xSockId, cStr, xErr, errno, strerror(errno));
            xMBTCPPortMasterCloseConnection(pxInfo);
            xErr = ERR_CONN;
        } else {
            ESP_LOGI(TAG, MB_SLAVE_FMT(", successfully connected."),
                                                  pxInfo->xIndex, pxInfo->xSockId, cStr);
            continue;
        }
    }
    freeaddrinfo(pxAddrList);
    return xErr;
}

// Find the first slave info whose descriptor is set in xFdSet
static MbSlaveInfo_t* xMBTCPPortMasterGetSlaveReady(fd_set* pxFdSet)
{
    MbSlaveInfo_t* pxInfo = NULL;

    // Slave connection loop
    for (int xIndex = 0; (xIndex < MB_TCP_PORT_MAX_CONN); xIndex++) {
        pxInfo = xMbPortConfig.pxMbSlaveInfo[xIndex];
        if (pxInfo) {
            // Is this response for current processing slave
            if (FD_ISSET(pxInfo->xSockId, pxFdSet)) {
                FD_CLR(pxInfo->xSockId, pxFdSet);
                return pxInfo;
            }
        }
    }
    return (MbSlaveInfo_t*)NULL;
}

static int xMBTCPPortMasterCheckConnState(fd_set* pxFdSet)
{
    fd_set xConnSetCheck = *pxFdSet;
    MbSlaveInfo_t* pxInfo = NULL;
    int64_t xTime = 0;
    int xErr = 0;
    int xCount = 0;
    do {
        xTime = xMBTCPGetTimeStamp();
        pxInfo = xMBTCPPortMasterGetSlaveReady(&xConnSetCheck);
        if (pxInfo) {
            xErr = xMBTCPPortMasterCheckAlive(pxInfo, 0);
            if ((xErr < 0) && (((xTime - pxInfo->xRecvTimeStamp) > MB_TCP_RECONNECT_TIMEOUT) ||
                                ((xTime - pxInfo->xSendTimeStamp) > MB_TCP_RECONNECT_TIMEOUT))) {
                ESP_LOGI(TAG, MB_SLAVE_FMT(", slave is down, off_time[r][w](us) = [%ju][%ju]."),
                                                            pxInfo->xIndex,
                                                            pxInfo->xSockId,
                                                            pxInfo->pcIpAddr,
                                                            (int64_t)(xTime - pxInfo->xRecvTimeStamp),
                                                            (int64_t)(xTime - pxInfo->xSendTimeStamp));
                xCount++;
            }
        }
    } while (pxInfo && (xCount < MB_TCP_PORT_MAX_CONN));
    return xCount;
}

static void xMBTCPPortMasterFsmSetError(eMBMasterErrorEventType xErrType, eMBMasterEventType xPostEvent)
{
    vMBMasterPortTimersDisable();
    vMBMasterSetErrorType(xErrType);
    xMBMasterPortEventPost(xPostEvent);
}

static void vMBTCPPortMasterTask(void *pvParameters)
{
    MbSlaveInfo_t* pxInfo;
    MbSlaveInfo_t* pxCurrInfo;

    fd_set xConnSet;
    fd_set xReadSet;
    int xMaxSd = 0;
    err_t xErr = ERR_ABRT;
    USHORT usSlaveConnCnt = 0;
    int64_t xTime = 0;

    // Register each slave in the connection info structure
    while (1) {
        MbSlaveAddrInfo_t xSlaveAddrInfo = { 0 };
        BaseType_t xStatus = xQueueReceive(xMbPortConfig.xConnectQueue, (void*)&xSlaveAddrInfo, pdMS_TO_TICKS(MB_EVENT_WAIT_TOUT_MS));
	    xMBTCPPortMasterCheckShutdown();
        if (xStatus != pdTRUE) {
            ESP_LOGE(TAG, "Fail to register slave IP.");
        } else {
            if (xSlaveAddrInfo.pcIPAddr == NULL && xMbPortConfig.usMbSlaveInfoCount && xSlaveAddrInfo.usIndex == 0xFF) {
                break;
            }
            if (xMbPortConfig.usMbSlaveInfoCount > MB_TCP_PORT_MAX_CONN) {
                ESP_LOGE(TAG, "Exceeds maximum connections limit=%d.", MB_TCP_PORT_MAX_CONN);
                break;
            }
            pxInfo = calloc(1, sizeof(MbSlaveInfo_t));
            if (!pxInfo) {
                ESP_LOGE(TAG, "Slave(#%d), info structure allocation fail.",
                                                    xMbPortConfig.usMbSlaveInfoCount);
                free(pxInfo);
                break;
            }
            pxInfo->pucRcvBuf = calloc(MB_TCP_BUF_SIZE, sizeof(UCHAR));
            if (!pxInfo->pucRcvBuf) {
                ESP_LOGE(TAG, "Slave(#%d), receive buffer allocation fail.",
                                                    xMbPortConfig.usMbSlaveInfoCount);
                free(pxInfo->pucRcvBuf);
                break;
            }
            pxInfo->usRcvPos = 0;
            pxInfo->pcIpAddr = xSlaveAddrInfo.pcIPAddr;
            pxInfo->xSockId = -1;
            pxInfo->xError = -1;
            pxInfo->xRecvTimeStamp = xMBTCPGetTimeStamp();
            pxInfo->xSendTimeStamp = xMBTCPGetTimeStamp();
            pxInfo->xMbProto = MB_PROTO_TCP;
            pxInfo->ucSlaveAddr = xSlaveAddrInfo.ucSlaveAddr;
            pxInfo->xIndex = xSlaveAddrInfo.usIndex;
            pxInfo->usTidCnt = (USHORT)(xMbPortConfig.usMbSlaveInfoCount << 8U);
            // Register slave
            xMbPortConfig.pxMbSlaveInfo[xMbPortConfig.usMbSlaveInfoCount++] = pxInfo;
            ESP_LOGI(TAG, "Add slave IP: %s", xSlaveAddrInfo.pcIPAddr);
        }
    }

    // Main connection cycle
    while (1)
    {
        ESP_LOGI(TAG, "Connecting to slaves...");
        xTime = xMBTCPGetTimeStamp();
        usSlaveConnCnt = 0;
        CHAR ucDot = '.';
        while(usSlaveConnCnt < xMbPortConfig.usMbSlaveInfoCount) {
            usSlaveConnCnt = 0;
            FD_ZERO(&xConnSet);
            ucDot ^= 0x03;
            // Slave connection loop
            for (UCHAR ucCnt = 0; (ucCnt < MB_TCP_PORT_MAX_CONN); ucCnt++) {
                pxInfo = xMbPortConfig.pxMbSlaveInfo[ucCnt];
                // if slave descriptor is NULL then it is end of list or connection closed.
                if (!pxInfo) {
                    ESP_LOGV(TAG, "Index: %d is not initialized, skip.", ucCnt);
                    if (xMbPortConfig.usMbSlaveInfoCount) {
                        continue;
                    }
                    break;
                }
                putchar(ucDot);
                xErr = xMBTCPPortMasterConnect(pxInfo);
                switch(xErr)
                {
                    case ERR_CONN:
                    case ERR_INPROGRESS:
                        // In case of connection errors remove the socket from set
                        if (FD_ISSET(pxInfo->xSockId, &xConnSet)) {
                            FD_CLR(pxInfo->xSockId, &xConnSet);
                            ESP_LOGE(TAG, MB_SLAVE_FMT(" connect failed, error = %d."),
                                                                            pxInfo->xIndex, pxInfo->xSockId,
                                                                            (char*)pxInfo->pcIpAddr, xErr);
                            if (usSlaveConnCnt) {
                                usSlaveConnCnt--;
                            }
                        }
                        break;
                    case ERR_OK:
                        // if connection is successful, add the descriptor into set
                        if (!FD_ISSET(pxInfo->xSockId, &xConnSet)) {
                            FD_SET(pxInfo->xSockId, &xConnSet);
                            usSlaveConnCnt++;
                            xMaxSd = (pxInfo->xSockId > xMaxSd) ? pxInfo->xSockId : xMaxSd;
                            ESP_LOGD(TAG, MB_SLAVE_FMT(", connected %d slave(s), error = %d."),
                                                                pxInfo->xIndex, pxInfo->xSockId,
                                                                pxInfo->pcIpAddr,
                                                                usSlaveConnCnt, xErr);
                            // Update time stamp for connected slaves
                            pxInfo->xRecvTimeStamp = xMBTCPGetTimeStamp();
                            pxInfo->xSendTimeStamp = xMBTCPGetTimeStamp();
                        }
                        break;
                    default:
                        ESP_LOGE(TAG, MB_SLAVE_FMT(", unexpected error = %d."),
                                                            pxInfo->xIndex,
                                                            pxInfo->xSockId,
                                                            pxInfo->pcIpAddr, xErr);
                        break;
                }
                if (pxInfo) {
                    pxInfo->xError = xErr;
                }
                xMBTCPPortMasterCheckShutdown();
            }
        }
        ESP_LOGI(TAG, "Connected %d slaves, start polling...", usSlaveConnCnt);

        vMBTCPPortMasterStartPoll(); // Send event to start stack

        // Slave receive data loop
        while(usSlaveConnCnt) {
            xReadSet = xConnSet;
            // Check transmission event to clear appropriate bit.
            xMBMasterPortFsmWaitConfirmation(EV_MASTER_FRAME_TRANSMIT, pdMS_TO_TICKS(MB_EVENT_WAIT_TOUT_MS));
            // Synchronize state machine with send packet event
            if (xMBMasterPortFsmWaitConfirmation(EV_MASTER_FRAME_SENT, pdMS_TO_TICKS(MB_EVENT_WAIT_TOUT_MS))) {
                ESP_LOGD(TAG, "FSM Synchronized with sent event.");
            }
            // Get slave info for the current slave.
            pxCurrInfo = vMBTCPPortMasterGetCurrInfo();
            if (!pxCurrInfo) {
                ESP_LOGE(TAG, "Incorrect connection options for slave index: %d.",
                                            xMbPortConfig.ucCurSlaveIndex);
                vMBTCPPortMasterStopPoll();
                xMBTCPPortMasterCheckShutdown();
                break; // incorrect slave descriptor, reconnect.
            }
            xTime = xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo);
            ESP_LOGD(TAG, "Set select timeout, left time: %ju ms.",
                                        xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo));
            // Wait respond from current slave during respond timeout
            int xRes = vMBTCPPortMasterRxCheck(pxCurrInfo->xSockId, &xReadSet, xTime);
            if (xRes == ERR_TIMEOUT) {
                // No respond from current slave, process timeout.
                // Need to drop response later if it is received after timeout.
                ESP_LOGD(TAG, "Select timeout, left time: %ju ms.",
                                                    xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo));
                xTime = xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo);
                // Wait completion of last transaction
                xMBMasterPortFsmWaitConfirmation(MB_EVENT_REQ_DONE_MASK, pdMS_TO_TICKS(xTime + 1));
                xMBTCPPortMasterCheckShutdown();
                continue;
            } else if (xRes < 0) {
                // Select error (slave connection or r/w failure).
                ESP_LOGD(TAG, MB_SLAVE_FMT(", socket select error. Slave disconnected?"),
                            pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr);
                xTime = xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo);
                // Wait completion of last transaction
                xMBMasterPortFsmWaitConfirmation(MB_EVENT_REQ_DONE_MASK, pdMS_TO_TICKS(xTime));
                // Stop polling process
                vMBTCPPortMasterStopPoll();
                xMBTCPPortMasterCheckShutdown();
                // Check disconnected slaves, do not need a result just to print information.
                xMBTCPPortMasterCheckConnState(&xConnSet);
                break;
            } else {
                // Check to make sure that active slave data is ready
                if (FD_ISSET(pxCurrInfo->xSockId, &xReadSet)) {
                    int xRet = ERR_BUF;
                    for (int retry = 0; (xRet == ERR_BUF) && (retry < MB_TCP_READ_BUF_RETRY_CNT); retry++) {
                        xRet = vMBTCPPortMasterReadPacket(pxCurrInfo);
                        // The error ERR_BUF means received response to previous request
                        // (due to timeout) with the same socket ID and incorrect TID,
                        // then ignore it and try to get next response buffer.
                    }
                    if (xRet > 0) {
                        // Response received correctly, send an event to stack
                        xMBTCPPortMasterFsmSetError(EV_ERROR_INIT, EV_MASTER_FRAME_RECEIVED);
                        ESP_LOGD(TAG, MB_SLAVE_FMT(", frame received."),
                                    pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr);
                    } else if ((xRet == ERR_TIMEOUT) || (xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo) == 0)) {
                        // Timeout occurred when receiving frame, process respond timeout
                        xMBTCPPortMasterFsmSetError(EV_ERROR_RESPOND_TIMEOUT, EV_MASTER_ERROR_PROCESS);
                        ESP_LOGD(TAG, MB_SLAVE_FMT(", frame read timeout."),
                                    pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr);
                    } else if (xRet == ERR_BUF) {
                        // After retries a response with incorrect TID received, process failure.
                        xMBTCPPortMasterFsmSetError(EV_ERROR_RECEIVE_DATA, EV_MASTER_ERROR_PROCESS);
                        ESP_LOGW(TAG, MB_SLAVE_FMT(", frame error."),
                                    pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr);
                    } else {
                        ESP_LOGE(TAG, MB_SLAVE_FMT(", critical error=%d."),
                                    pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr, xRet);
                        // Stop polling process
                        vMBTCPPortMasterStopPoll();
                        xMBTCPPortMasterCheckShutdown();
                        // Check disconnected slaves, do not need a result just to print information.
                        xMBTCPPortMasterCheckConnState(&xConnSet);
                        break;
                    }
                    xTime = xMBTCPPortMasterGetRespTimeLeft(pxCurrInfo);
                    ESP_LOGD(TAG, "Slave #%d, data processing left time %ju [ms].", pxCurrInfo->xIndex, xTime);
                    // Wait completion of Modbus frame processing before start of new transaction.
                    if (xMBMasterPortFsmWaitConfirmation(MB_EVENT_REQ_DONE_MASK, pdMS_TO_TICKS(xTime))) {
                        ESP_LOGD(TAG, MB_SLAVE_FMT(", data processing completed."),
                                pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr);
                    }
                    xTime = xMBTCPGetTimeStamp() - pxCurrInfo->xSendTimeStamp;
                    ESP_LOGD(TAG, MB_SLAVE_FMT(", processing time[us] = %ju."),
                                                    pxCurrInfo->xIndex, pxCurrInfo->xSockId, pxCurrInfo->pcIpAddr, xTime);
                }
            }
            xMBTCPPortMasterCheckShutdown();
        } // while(usMbSlaveInfoCount)
    } // while (1)
    vTaskDelete(NULL);
}

extern void vMBMasterPortEventClose(void);
extern void vMBMasterPortTimerClose(void);

void
vMBMasterTCPPortEnable(void)
{
    vTaskResume(xMbPortConfig.xMbTcpTaskHandle);
}

void
vMBMasterTCPPortDisable(void)
{
    // Try to exit the task gracefully, so select could release its internal callbacks
    // that were allocated on the stack of the task we're going to delete
    xShutdownSemaphore = xSemaphoreCreateBinary();
    // if no semaphore (alloc issues) or couldn't acquire it, just delete the task
    if (xShutdownSemaphore == NULL || xSemaphoreTake(xShutdownSemaphore, pdMS_TO_TICKS(MB_EVENT_WAIT_TOUT_MS)) != pdTRUE) {
        ESP_LOGW(TAG, "Modbus port task couldn't exit gracefully within timeout -> abruptly deleting the task.");
        vTaskDelete(xMbPortConfig.xMbTcpTaskHandle);
    }
    if (xShutdownSemaphore) {
        vSemaphoreDelete(xShutdownSemaphore);
        xShutdownSemaphore = NULL;
    }
    for (USHORT ucCnt = 0; ucCnt < MB_TCP_PORT_MAX_CONN; ucCnt++) {
        MbSlaveInfo_t* pxInfo = xMbPortConfig.pxMbSlaveInfo[ucCnt];
        if (pxInfo) {
            xMBTCPPortMasterCloseConnection(pxInfo);
            if (pxInfo->pucRcvBuf) {
                free(pxInfo->pucRcvBuf);
            }
            free(pxInfo);
            xMbPortConfig.pxMbSlaveInfo[ucCnt] = NULL;
        }
    }
    free(xMbPortConfig.pxMbSlaveInfo);
}

void
vMBMasterTCPPortClose(void)
{
    vQueueDelete(xMbPortConfig.xConnectQueue);
    vMBMasterPortTimerClose();
    // Release resources for the event queue.
    vMBMasterPortEventClose();
}

BOOL
xMBMasterTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
    MbSlaveInfo_t* pxInfo = vMBTCPPortMasterGetCurrInfo();
    *ppucMBTCPFrame = pxInfo->pucRcvBuf;
    *usTCPLength = pxInfo->usRcvPos;

    // Reset the buffer.
    pxInfo->usRcvPos = 0;
    // Save slave receive timestamp
    if (pxInfo->xRcvErr == ERR_OK && *usTCPLength > 0) {
        pxInfo->xRecvTimeStamp = xMBTCPGetTimeStamp();
        return TRUE;
    }
    return FALSE;
}

int xMBMasterTCPPortWritePoll(MbSlaveInfo_t* pxInfo, const UCHAR * pucMBTCPFrame, USHORT usTCPLength, ULONG xTimeout)
{
    // Check if the socket is alive (writable and SO_ERROR == 0)
    int xRes = (int)xMBTCPPortMasterCheckAlive(pxInfo, xTimeout);
    if ((xRes < 0) && (xRes != ERR_INPROGRESS))
    {
        ESP_LOGE(TAG, MB_SLAVE_FMT(", is not writable, error: %d, errno %d"),
                                    pxInfo->xIndex, pxInfo->xSockId, pxInfo->pcIpAddr, xRes, errno);
        return xRes;
    }
    xRes = send(pxInfo->xSockId, pucMBTCPFrame, usTCPLength, TCP_NODELAY);
    if (xRes < 0) {
        ESP_LOGE(TAG, MB_SLAVE_FMT(", send data error: %d, errno %d"),
                                        pxInfo->xIndex, pxInfo->xSockId, pxInfo->pcIpAddr, xRes, errno);
    }
    return xRes;
}

BOOL
xMBMasterTCPPortSendResponse( UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
    BOOL bFrameSent = FALSE;
    USHORT ucCurSlaveIndex = ucMBMasterGetDestAddress();
    MbSlaveInfo_t* pxInfo = vMBTCPPortMasterFindSlaveInfo(ucCurSlaveIndex);

    // If the slave is correct and active then send data
    // otherwise treat slave as died and skip
    if (pxInfo != NULL) {
        if (pxInfo->xSockId < 0) {
            ESP_LOGD(TAG, MB_SLAVE_FMT(", send to died slave, error = %d"),
                                                  pxInfo->xIndex, pxInfo->xSockId, pxInfo->pcIpAddr, pxInfo->xError);
        } else {
            // Apply TID field to the frame before send
            pucMBTCPFrame[MB_TCP_TID] = (UCHAR)(pxInfo->usTidCnt >> 8U);
            pucMBTCPFrame[MB_TCP_TID + 1] = (UCHAR)(pxInfo->usTidCnt & 0xFF);
            int xRes = xMBMasterTCPPortWritePoll(pxInfo, pucMBTCPFrame, usTCPLength, MB_TCP_SEND_TIMEOUT_MS);
            if (xRes < 0) {
                ESP_LOGE(TAG, MB_SLAVE_FMT(", send data failure, err(errno) = %d(%d)."),
                                            pxInfo->xIndex, pxInfo->xSockId, pxInfo->pcIpAddr, xRes, errno);
                bFrameSent = FALSE;
                pxInfo->xError = xRes;
            } else {
                bFrameSent = TRUE;
                ESP_LOGD(TAG, MB_SLAVE_FMT(", send data successful: TID=0x%02x, %d (bytes), errno %d"),
                                                    pxInfo->xIndex, pxInfo->xSockId, pxInfo->pcIpAddr, pxInfo->usTidCnt, xRes, errno);
                pxInfo->xError = 0;
                pxInfo->usRcvPos = 0;
                if (pxInfo->usTidCnt < (USHRT_MAX - 1)) {
                    pxInfo->usTidCnt++;
                } else {
                    pxInfo->usTidCnt = (USHORT)(pxInfo->xIndex << 8U);
                }
            }
            pxInfo->xSendTimeStamp = xMBTCPGetTimeStamp();
        }
    } else {
        ESP_LOGD(TAG, "Send data to died slave, address = %d", ucCurSlaveIndex);
    }
    vMBMasterPortTimersRespondTimeoutEnable();
    xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    return bFrameSent;
}

// Timer handler to check timeout of socket response
BOOL MB_PORT_ISR_ATTR
xMBMasterTCPTimerExpired(void)
{
    BOOL xNeedPoll = FALSE;

    vMBMasterPortTimersDisable();
    // If timer mode is respond timeout, the master event then turns EV_MASTER_EXECUTE status.
    if (xMBMasterGetCurTimerMode() == MB_TMODE_RESPOND_TIMEOUT) {
        vMBMasterSetErrorType(EV_ERROR_RESPOND_TIMEOUT);
        xNeedPoll = xMBMasterPortEventPost(EV_MASTER_ERROR_PROCESS);
    }

    return xNeedPoll;
}

#endif //#if MB_MASTER_TCP_ENABLED
