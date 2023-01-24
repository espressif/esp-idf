/*
 * SPDX-FileCopyrightText: 2010 Christian Walter
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 Port
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
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
 * File: $Id: port.h,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

#ifndef PORT_COMMON_H_
#define PORT_COMMON_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"         // for queue

#include "esp_log.h"                // for ESP_LOGE macro
#include "esp_timer.h"
#include "driver/uart.h"            // for uart_event_t

#if __has_include("driver/gptimer.h")
#include "driver/gptimer.h"
#else
#include "driver/timer.h"
#endif

#include "mbconfig.h"

#define INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define PR_END_EXTERN_C             }

#define MB_PORT_TAG                     "MB_PORT_COMMON"

#define MB_BAUD_RATE_DEFAULT            (115200)
#define MB_QUEUE_LENGTH                 (CONFIG_FMB_QUEUE_LENGTH)

#define MB_SERIAL_TASK_PRIO             (CONFIG_FMB_PORT_TASK_PRIO)
#define MB_SERIAL_TASK_STACK_SIZE       (CONFIG_FMB_PORT_TASK_STACK_SIZE)
#define MB_SERIAL_TOUT                  (3) // 3.5*8 = 28 ticks, TOUT=3 -> ~24..33 ticks

// Set buffer size for transmission
#define MB_SERIAL_BUF_SIZE              (CONFIG_FMB_SERIAL_BUF_SIZE)

// common definitions for serial port implementations
#define MB_SERIAL_TX_TOUT_MS            (2200) // maximum time for transmission of longest allowed frame buffer
#define MB_SERIAL_TX_TOUT_TICKS         (pdMS_TO_TICKS(MB_SERIAL_TX_TOUT_MS)) // timeout for transmission
#define MB_SERIAL_RX_TOUT_MS            (1)
#define MB_SERIAL_RX_TOUT_TICKS         (pdMS_TO_TICKS(MB_SERIAL_RX_TOUT_MS)) // timeout for receive

#define MB_SERIAL_RESP_LEN_MIN          (4)

// Common definitions for TCP port
#define MB_TCP_BUF_SIZE                 (256 + 7) // Must hold a complete Modbus TCP frame.
#define MB_TCP_DEFAULT_PORT             (CONFIG_FMB_TCP_PORT_DEFAULT)
#define MB_TCP_STACK_SIZE               (CONFIG_FMB_PORT_TASK_STACK_SIZE)
#define MB_TCP_TASK_PRIO                (CONFIG_FMB_PORT_TASK_PRIO)

// The task affinity for Modbus stack tasks
#define MB_PORT_TASK_AFFINITY           (CONFIG_FMB_PORT_TASK_AFFINITY)

#define MB_TCP_READ_TIMEOUT_MS          (100) // read timeout in mS
#define MB_TCP_READ_TIMEOUT             (pdMS_TO_TICKS(MB_TCP_READ_TIMEOUT_MS))
#define MB_TCP_SEND_TIMEOUT_MS          (500) // send event timeout in mS
#define MB_TCP_SEND_TIMEOUT             (pdMS_TO_TICKS(MB_TCP_SEND_TIMEOUT_MS))
#define MB_TCP_PORT_MAX_CONN            (CONFIG_FMB_TCP_PORT_MAX_CONN)

#define MB_TCP_FRAME_LOG_BUFSIZE        (256)

#define MB_PORT_HAS_CLOSE               (1) // Define to explicitly close port on destroy

// Define number of timer reloads per 1 mS
#define MB_TIMER_TICS_PER_MS            (20UL)
#define MB_TIMER_TICK_TIME_US           (1000 / MB_TIMER_TICS_PER_MS) // 50uS = one discreet for timer

#define MB_TCP_DEBUG                    (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG) // Enable legacy debug output in TCP module.

#define MB_TCP_GET_FIELD(buffer, field) ((USHORT)((buffer[field] << 8U) | buffer[field + 1]))

#define MB_PORT_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(MB_PORT_TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return ret_val; \
    }

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif /* __cplusplus */

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#if MB_TCP_DEBUG
typedef enum
{
    MB_LOG_DEBUG,
    MB_LOG_INFO,
    MB_LOG_WARN,
    MB_LOG_ERROR
} eMBPortLogLevel;
#endif

typedef enum
{
    MB_PROTO_TCP,
    MB_PROTO_UDP,
} eMBPortProto;

typedef enum {
    MB_PORT_IPV4 = 0,                     /*!< TCP IPV4 addressing */
    MB_PORT_IPV6 = 1                      /*!< TCP IPV6 addressing */
} eMBPortIpVer;

typedef struct {
    esp_timer_handle_t xTimerIntHandle;
    USHORT usT35Ticks;
    BOOL xTimerState;
} xTimerContext_t;

void vMBPortEnterCritical(void);
void vMBPortExitCritical(void);

#define ENTER_CRITICAL_SECTION( ) { ESP_EARLY_LOGD(MB_PORT_TAG,"%s: Port enter critical.", __func__); \
                                    vMBPortEnterCritical(); }

#define EXIT_CRITICAL_SECTION( )  { vMBPortExitCritical(); \
                                    ESP_EARLY_LOGD(MB_PORT_TAG,"%s: Port exit critical", __func__); }

#define MB_PORT_CHECK_EVENT( event, mask ) ( event & mask )
#define MB_PORT_CLEAR_EVENT( event, mask ) do { event &= ~mask; } while(0)

#define MB_PORT_PARITY_GET(parity) ((parity != UART_PARITY_DISABLE) ? \
                                        ((parity == UART_PARITY_ODD) ? MB_PAR_ODD : MB_PAR_EVEN) : MB_PAR_NONE)

// Legacy Modbus logging function
#if MB_TCP_DEBUG
void vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule,
                    const CHAR * szFmt, ... );
void prvvMBTCPLogFrame( const CHAR * pucMsg, UCHAR * pucFrame, USHORT usFrameLen );
#endif

void vMBPortSetMode( UCHAR ucMode );
UCHAR ucMBPortGetMode( void );

BOOL xMBPortSerialWaitEvent(QueueHandle_t xMbUartQueue, uart_event_t* pxEvent, ULONG xTimeout);

#ifdef __cplusplus
PR_END_EXTERN_C
#endif /* __cplusplus */

#endif /* PORT_COMMON_H_ */
