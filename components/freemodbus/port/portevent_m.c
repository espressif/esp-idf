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
 * File: $Id: portevent.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbport.h"
#include "mbconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "port.h"
#include "mbport.h"
#include "freertos/semphr.h"
#include "port_serial_master.h"

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED
/* ----------------------- Defines ------------------------------------------*/
// Event bit mask for xMBMasterPortEventGet()
#define MB_EVENT_POLL_MASK  (EventBits_t)( EV_MASTER_READY | \
                                            EV_MASTER_FRAME_RECEIVED | \
                                            EV_MASTER_EXECUTE | \
                                            EV_MASTER_FRAME_SENT | \
                                            EV_MASTER_FRAME_TRANSMITTED | \
                                            EV_MASTER_ERROR_PROCESS )

// Event bit mask for eMBMasterWaitRequestFinish()
#define MB_EVENT_REQ_MASK   (EventBits_t)( EV_MASTER_PROCESS_SUCCESS | \
                                            EV_MASTER_ERROR_RESPOND_TIMEOUT | \
                                            EV_MASTER_ERROR_RECEIVE_DATA | \
                                            EV_MASTER_ERROR_EXECUTE_FUNCTION )

#define MB_CHECK_EVENT(event, mask) (event & mask)

/* ----------------------- Variables ----------------------------------------*/
static SemaphoreHandle_t xSemaphorMasterHdl;
static EventGroupHandle_t xEventGroupMasterHdl;

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBMasterPortEventInit( void )
{
    xEventGroupMasterHdl = xEventGroupCreate();
    MB_PORT_CHECK((xEventGroupMasterHdl != NULL),
                    FALSE, "mb stack event group creation error.");
    return TRUE;
}

BOOL MB_PORT_ISR_ATTR
xMBMasterPortEventPost( eMBMasterEventType eEvent )
{
    BOOL bStatus = FALSE;
    eMBMasterEventType eTempEvent = eEvent;

    if( (BOOL)xPortInIsrContext() == TRUE )
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t xResult = xEventGroupSetBitsFromISR( xEventGroupMasterHdl,
                                                        (EventBits_t) eTempEvent,
                                                        &xHigherPriorityTaskWoken );
        // Was the message posted successfully?
        if( xResult == pdPASS ) {
            // If xHigherPriorityTaskWoken is now set to pdTRUE
            // then a context switch should be requested.
            if (xHigherPriorityTaskWoken) portYIELD_FROM_ISR();
            bStatus = TRUE;
        } else {
            bStatus = FALSE;
        }
    }
    else
    {
        // Set event bits if the function is called from task
        // The return result is not checked here because
        // It might be that event bit was cleared automatically as a
        // task that was waiting for the bit was removed from the Blocked state.
        (void) xEventGroupSetBits( xEventGroupMasterHdl, (EventBits_t)eTempEvent );
        bStatus = TRUE;
    }
    return bStatus;
}

BOOL
xMBMasterPortEventGet( eMBMasterEventType * eEvent)
{
    EventBits_t uxBits;
    BOOL    xEventHappened = FALSE;
    uxBits = xEventGroupWaitBits(
            xEventGroupMasterHdl,   // The event group being tested.
            MB_EVENT_POLL_MASK,     // The bits within the event group to wait for.
            pdTRUE,                 // Masked bits should be cleared before returning.
            pdFALSE,                // Don't wait for both bits, either bit will do.
            portMAX_DELAY);        // Wait forever for either bit to be set.

    // Check if poll event is correct
    if (uxBits & MB_EVENT_POLL_MASK) {
        *eEvent = (eMBMasterEventType)(uxBits);
        xEventHappened = TRUE;
    } else {
        ESP_LOGE(MB_PORT_TAG,"%s: Incorrect event triggered.", __func__);
        xEventHappened = FALSE;
    }
    return xEventHappened;
}

// This function is initialize the OS resource for modbus master.
void vMBMasterOsResInit( void )
{
    xSemaphorMasterHdl = xSemaphoreCreateBinary();
    MB_PORT_CHECK((xSemaphorMasterHdl != NULL), ; , "%s: OS semaphore create error.", __func__);
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource take result
 */
BOOL xMBMasterRunResTake( LONG lTimeOut )
{
    BaseType_t xStatus = pdTRUE;

    // If waiting time is -1. It will wait forever
    xStatus = xSemaphoreTake(xSemaphorMasterHdl, lTimeOut );
    MB_PORT_CHECK((xStatus == pdTRUE), FALSE , "%s:Take resource failure.", __func__);
    ESP_LOGV(MB_PORT_TAG,"%s:Take resource (%lu ticks).", __func__, lTimeOut);
    return TRUE;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 */
void vMBMasterRunResRelease( void )
{
    BaseType_t xStatus = pdFALSE;
    xStatus = xSemaphoreGive(xSemaphorMasterHdl);
    MB_PORT_CHECK((xStatus == pdTRUE), ; , "%s: resource release failure.", __func__);
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength) 
{
    BOOL ret = xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);
    MB_PORT_CHECK((ret == TRUE), ; , "%s: Post event 'EV_MASTER_ERROR_RESPOND_TIMEOUT' failed!", __func__);
    ESP_LOGD(MB_PORT_TAG,"%s:Callback respond timeout.", __func__);
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 */
void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength) 
{
    BOOL ret = xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
    MB_PORT_CHECK((ret == TRUE), ; , "%s: Post event 'EV_MASTER_ERROR_RECEIVE_DATA' failed!", __func__);
    ESP_LOGD(MB_PORT_TAG,"%s:Callback receive data timeout failure.", __func__);
}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength) 
{
    BOOL ret = xMBMasterPortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);
    MB_PORT_CHECK((ret == TRUE), ; , "%s: Post event 'EV_MASTER_ERROR_EXECUTE_FUNCTION' failed!", __func__);
    ESP_LOGD(MB_PORT_TAG,"%s:Callback execute data timeout failure.", __func__);
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system. Do not execute too much waiting process.
 */
void vMBMasterCBRequestSuccess( void ) {
     /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    BOOL ret = xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCCESS);
    MB_PORT_CHECK((ret == TRUE), ; , "%s: Post event 'EV_MASTER_PROCESS_SUCCESS' failed!", __func__);
    ESP_LOGD(MB_PORT_TAG,"%s: Callback request success.", __func__);
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish( void ) {
    eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
    eMBMasterEventType xRecvedEvent;

    EventBits_t uxBits = xEventGroupWaitBits( xEventGroupMasterHdl, // The event group being tested.
                                                MB_EVENT_REQ_MASK,  // The bits within the event group to wait for.
                                                pdTRUE,             // Masked bits should be cleared before returning.
                                                pdFALSE,            // Don't wait for both bits, either bit will do.
                                                portMAX_DELAY );    // Wait forever for either bit to be set.
    xRecvedEvent = (eMBMasterEventType)(uxBits);
    if (xRecvedEvent) {
        ESP_LOGD(MB_PORT_TAG,"%s: returned event = 0x%x", __func__, xRecvedEvent);
        if (!(xRecvedEvent & MB_EVENT_REQ_MASK)) {
            // if we wait for certain event bits but get other then set in mask
            ESP_LOGE(MB_PORT_TAG,"%s: incorrect event set = 0x%x", __func__, xRecvedEvent);
        }
        if MB_CHECK_EVENT(xRecvedEvent, EV_MASTER_PROCESS_SUCCESS) {
            eErrStatus = MB_MRE_NO_ERR;
        } else if MB_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_RESPOND_TIMEOUT){
            eErrStatus = MB_MRE_TIMEDOUT;
        } else if MB_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_RECEIVE_DATA){
            eErrStatus = MB_MRE_REV_DATA;
        } else if MB_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_EXECUTE_FUNCTION){
            eErrStatus = MB_MRE_EXE_FUN;
        }
    } else {
        ESP_LOGE(MB_PORT_TAG,"%s: Incorrect event or timeout xRecvedEvent = 0x%x", __func__, uxBits);
        assert(0);
    }
    return eErrStatus;
}

void vMBMasterPortEventClose(void)
{
    vEventGroupDelete(xEventGroupMasterHdl);
    vSemaphoreDelete(xSemaphorMasterHdl);
}

#endif
