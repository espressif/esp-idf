/*
 * SPDX-FileCopyrightText: 2010 Christian Walter
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 Port Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
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
 * File: $Id: portother.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "sdkconfig.h"

#if CONFIG_FMB_TIMER_PORT_ENABLED

static const char *TAG = "MBS_TIMER";

static xTimerContext_t* pxTimerContext = NULL;

/* ----------------------- Start implementation -----------------------------*/
static void IRAM_ATTR vTimerAlarmCBHandler(void *param)
{
    pxMBPortCBTimerExpired(); // Timer expired callback function
    pxTimerContext->xTimerState = TRUE;
    ESP_EARLY_LOGD(TAG, "Slave timeout triggered.");
}
#endif

BOOL xMBPortTimersInit(USHORT usTimeOut50us)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    MB_PORT_CHECK((usTimeOut50us > 0), FALSE,
            "Modbus timeout discreet is incorrect.");
    MB_PORT_CHECK(!pxTimerContext, FALSE,
                "Modbus timer is already created.");
    pxTimerContext = calloc(1, sizeof(xTimerContext_t));
    if (!pxTimerContext) {
        return FALSE;
    }
    pxTimerContext->xTimerIntHandle = NULL;
    // Save timer reload value for Modbus T35 period
    pxTimerContext->usT35Ticks = usTimeOut50us;
    esp_timer_create_args_t xTimerConf = {
        .callback = vTimerAlarmCBHandler,
        .arg = NULL,
#if (MB_TIMER_SUPPORTS_ISR_DISPATCH_METHOD && CONFIG_FMB_TIMER_USE_ISR_DISPATCH_METHOD)
        .dispatch_method = ESP_TIMER_ISR,
#else
        .dispatch_method = ESP_TIMER_TASK,
#endif
        .name = "MBS_T35timer"
    };
    // Create Modbus timer
    esp_err_t xErr = esp_timer_create(&xTimerConf, &(pxTimerContext->xTimerIntHandle));
    if (xErr) {
        return FALSE;
    }
#endif
    return TRUE;
}

void vMBPortTimersEnable(void)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    MB_PORT_CHECK((pxTimerContext && pxTimerContext->xTimerIntHandle), ; ,
                                "timer is not initialized.");
    uint64_t xToutUs = (pxTimerContext->usT35Ticks * MB_TIMER_TICK_TIME_US);
    esp_timer_stop(pxTimerContext->xTimerIntHandle);
    esp_timer_start_once(pxTimerContext->xTimerIntHandle, xToutUs);
    pxTimerContext->xTimerState = FALSE;
#endif
}

void MB_PORT_ISR_ATTR
vMBPortTimersDisable(void)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    // Disable timer alarm
    esp_timer_stop(pxTimerContext->xTimerIntHandle);
#endif
}

void vMBPortTimerClose(void)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    // Delete active timer
    if (pxTimerContext) {
        if (pxTimerContext->xTimerIntHandle) {
            esp_timer_stop(pxTimerContext->xTimerIntHandle);
            esp_timer_delete(pxTimerContext->xTimerIntHandle);
        }
        free(pxTimerContext);
        pxTimerContext = NULL;
    }
#endif
}
