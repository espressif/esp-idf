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
#include "esp_log.h"                // for ESP_LOGE macro
#include "sdkconfig.h"

#define INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define PR_END_EXTERN_C             }

#define MB_PORT_TAG "MB_PORT_COMMON"

#define MB_BAUD_RATE_DEFAULT        (115200)
#define MB_QUEUE_LENGTH             (CONFIG_FMB_QUEUE_LENGTH)

#define MB_SERIAL_TASK_PRIO         (CONFIG_FMB_SERIAL_TASK_PRIO)
#define MB_SERIAL_TASK_STACK_SIZE   (CONFIG_FMB_SERIAL_TASK_STACK_SIZE)
#define MB_SERIAL_TOUT              (3) // 3.5*8 = 28 ticks, TOUT=3 -> ~24..33 ticks

// Set buffer size for transmission
#define MB_SERIAL_BUF_SIZE          (CONFIG_FMB_SERIAL_BUF_SIZE)

// common definitions for serial port implementations
#define MB_SERIAL_TX_TOUT_MS        (2200) // maximum time for transmission of longest allowed frame buffer
#define MB_SERIAL_TX_TOUT_TICKS     pdMS_TO_TICKS(MB_SERIAL_TX_TOUT_MS) // timeout for transmission
#define MB_SERIAL_RX_TOUT_MS        (1)
#define MB_SERIAL_RX_TOUT_TICKS     pdMS_TO_TICKS(MB_SERIAL_RX_TOUT_MS) // timeout for receive

#define MB_SERIAL_RESP_LEN_MIN      (4)

// The task affinity for Modbus stack tasks
#define MB_PORT_TASK_AFFINITY           (CONFIG_FMB_PORT_TASK_AFFINITY)

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
#ifdef __cplusplus
PR_END_EXTERN_C
#endif /* __cplusplus */

#endif /* PORT_COMMON_H_ */
