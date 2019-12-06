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
 * FreeModbus Libary: ESP32 Port Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *
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
 * File: $Id: portevent.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "sdkconfig.h"
#include "port_serial_slave.h"
/* ----------------------- Variables ----------------------------------------*/
static xQueueHandle xQueueHdl;

#define MB_EVENT_QUEUE_SIZE     (6)
#define MB_EVENT_QUEUE_TIMEOUT  (pdMS_TO_TICKS(CONFIG_FMB_EVENT_QUEUE_TIMEOUT))

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    BOOL bStatus = FALSE;
    if((xQueueHdl = xQueueCreate(MB_EVENT_QUEUE_SIZE, sizeof(eMBEventType))) != NULL)
    {
        vQueueAddToRegistry(xQueueHdl, "MbPortEventQueue");
        bStatus = TRUE;
    }
    return bStatus;
}

void
vMBPortEventClose( void )
{
    if(xQueueHdl != NULL)
    {
        vQueueDelete(xQueueHdl);
        xQueueHdl = NULL;
    }
}

BOOL MB_PORT_ISR_ATTR
xMBPortEventPost( eMBEventType eEvent )
{
    BaseType_t xStatus, xHigherPriorityTaskWoken = pdFALSE;
    assert(xQueueHdl != NULL);
    
    if( (BOOL)xPortInIsrContext() == TRUE )
    {
        xStatus = xQueueSendFromISR(xQueueHdl, (const void*)&eEvent, &xHigherPriorityTaskWoken);
        MB_PORT_CHECK((xStatus == pdTRUE), FALSE, "%s: Post message failure.", __func__);
        if ( xHigherPriorityTaskWoken )
        {
            portYIELD_FROM_ISR();
        }
    }
    else
    {
        xStatus = xQueueSend(xQueueHdl, (const void*)&eEvent, MB_EVENT_QUEUE_TIMEOUT);
        MB_PORT_CHECK((xStatus == pdTRUE), FALSE, "%s: Post message failure.", __func__);
    }
    return TRUE;
}

BOOL
xMBPortEventGet(eMBEventType * peEvent)
{
    assert(xQueueHdl != NULL);
    BOOL xEventHappened = FALSE;

    if (xQueueReceive(xQueueHdl, peEvent, portMAX_DELAY) == pdTRUE) {
        xEventHappened = TRUE;
    }
    return xEventHappened;
}

xQueueHandle
xMBPortEventGetHandle(void)
{
    if(xQueueHdl != NULL) //
    {
        return xQueueHdl;
    }
    return NULL;
}

