/*
 * SPDX-FileCopyrightText: 2013 Armink
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 Port Demo Application
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbport.h"
#include "sdkconfig.h"

static const char *TAG = "MBM_TIMER";

/* ----------------------- Variables ----------------------------------------*/
static xTimerContext_t* pxTimerContext = NULL;

/* ----------------------- Start implementation -----------------------------*/
static void IRAM_ATTR vTimerAlarmCBHandler(void *param)
{
    pxMBMasterPortCBTimerExpired(); // Timer expired callback function
    pxTimerContext->xTimerState = TRUE;
    ESP_EARLY_LOGD(TAG, "Timer mode: (%d) triggered", xMBMasterGetCurTimerMode());
}

BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
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
#if CONFIG_FMB_TIMER_USE_ISR_DISPATCH_METHOD
        .dispatch_method = ESP_TIMER_ISR,
#else
        .dispatch_method = ESP_TIMER_TASK,
#endif
        .name = "MBM_T35timer"
    };
    // Create Modbus timer
    esp_err_t xErr = esp_timer_create(&xTimerConf, &(pxTimerContext->xTimerIntHandle));
    if (xErr) {
        return FALSE;
    }
    return TRUE;
}

// Set timer alarm value
static BOOL xMBMasterPortTimersEnable(uint64_t xToutUs)
{
    MB_PORT_CHECK(pxTimerContext && (pxTimerContext->xTimerIntHandle), FALSE,
                                "timer is not initialized.");
    MB_PORT_CHECK((xToutUs > 0), FALSE,
                            "incorrect tick value for timer = (0x%llu).", xToutUs);
    esp_timer_stop(pxTimerContext->xTimerIntHandle);
    esp_timer_start_once(pxTimerContext->xTimerIntHandle, xToutUs);
    pxTimerContext->xTimerState = FALSE;
    return TRUE;
}

void vMBMasterPortTimersT35Enable(void)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    uint64_t xToutUs = (pxTimerContext->usT35Ticks * MB_TIMER_TICK_TIME_US);

    // Set current timer mode, don't change it.
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
    // Set timer alarm
    (void)xMBMasterPortTimersEnable(xToutUs);
#endif
}

void vMBMasterPortTimersConvertDelayEnable(void)
{
    // Covert time in milliseconds into ticks
    uint64_t xToutUs = (MB_MASTER_DELAY_MS_CONVERT * 1000);

    // Set current timer mode
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
    ESP_LOGD(MB_PORT_TAG,"%s Convert delay enable.", __func__);
    (void)xMBMasterPortTimersEnable(xToutUs);
}

void vMBMasterPortTimersRespondTimeoutEnable(void)
{
    uint64_t xToutUs = (MB_MASTER_TIMEOUT_MS_RESPOND * 1000);

    vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
    ESP_LOGD(MB_PORT_TAG,"%s Respond enable timeout.", __func__);
    (void)xMBMasterPortTimersEnable(xToutUs);
}

void MB_PORT_ISR_ATTR
vMBMasterPortTimersDisable()
{
    // Disable timer alarm
    esp_timer_stop(pxTimerContext->xTimerIntHandle);
}

void vMBMasterPortTimerClose(void)
{
    // Delete active timer
    if (pxTimerContext) {
        if (pxTimerContext->xTimerIntHandle) {
            esp_timer_stop(pxTimerContext->xTimerIntHandle);
            esp_timer_delete(pxTimerContext->xTimerIntHandle);
        }
        free(pxTimerContext);
        pxTimerContext = NULL;
    }
}
