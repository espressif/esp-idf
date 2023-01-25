/*
 * SPDX-FileCopyrightText: 2013 Armink
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
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

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_MASTER_TCP_ENABLED
/* ----------------------- Defines ------------------------------------------*/
// Event bit mask for xMBMasterPortEventGet()
#define MB_EVENT_POLL_MASK  (EventBits_t)( EV_MASTER_READY | \
                                            EV_MASTER_FRAME_RECEIVED | \
                                            EV_MASTER_EXECUTE | \
                                            EV_MASTER_FRAME_SENT | \
                                            EV_MASTER_FRAME_TRANSMIT | \
                                            EV_MASTER_ERROR_PROCESS )

// Event bit mask for eMBMasterWaitRequestFinish()
#define MB_EVENT_REQ_MASK   (EventBits_t)( EV_MASTER_PROCESS_SUCCESS | \
                                            EV_MASTER_ERROR_RESPOND_TIMEOUT | \
                                            EV_MASTER_ERROR_RECEIVE_DATA | \
                                            EV_MASTER_ERROR_EXECUTE_FUNCTION )

#define MB_EVENT_RESOURCE   (EventBits_t)( 0x0080 )

/* ----------------------- Variables ----------------------------------------*/
static EventGroupHandle_t xResourceMasterHdl;
static EventGroupHandle_t xEventGroupMasterHdl;
static EventGroupHandle_t xEventGroupMasterConfirmHdl;

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBMasterPortEventInit( void )
{
    xEventGroupMasterHdl = xEventGroupCreate();
    xEventGroupMasterConfirmHdl = xEventGroupCreate();
    MB_PORT_CHECK((xEventGroupMasterHdl != NULL) && (xEventGroupMasterConfirmHdl != NULL),
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

eMBMasterEventType
xMBMasterPortFsmWaitConfirmation( eMBMasterEventType eEventMask, ULONG ulTimeout)
{
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits( xEventGroupMasterConfirmHdl,  // The event group being tested.
                                    eEventMask,                 // The bits within the event group to wait for.
                                    pdFALSE,                    // Keep masked bits.
                                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                                    ulTimeout);                 // Wait timeout for either bit to be set.
    if (ulTimeout && uxBits) {
        // Clear confirmation events that where set in the mask
        xEventGroupClearBits( xEventGroupMasterConfirmHdl, (uxBits & eEventMask) );
    }
    return (eMBMasterEventType)(uxBits & eEventMask);
}

BOOL
xMBMasterPortEventGet( eMBMasterEventType* eEvent )
{
    EventBits_t uxBits;
    BOOL    xEventHappened = FALSE;
    uxBits = xEventGroupWaitBits( xEventGroupMasterHdl, // The event group being tested.
                                    MB_EVENT_POLL_MASK, // The bits within the event group to wait for.
                                    pdTRUE,             // Masked bits should be cleared before returning.
                                    pdFALSE,            // Don't wait for both bits, either bit will do.
                                    portMAX_DELAY);     // Wait forever for either bit to be set.
    // Check if poll event is correct
    if (MB_PORT_CHECK_EVENT(uxBits, MB_EVENT_POLL_MASK)) {
        *eEvent = (eMBMasterEventType)(uxBits & MB_EVENT_POLL_MASK);
        // Set event bits in confirmation group (for synchronization with port task)
        xEventGroupSetBits( xEventGroupMasterConfirmHdl, *eEvent );
        xEventHappened = TRUE;
    } else {
        ESP_LOGE(MB_PORT_TAG,"%s: Incorrect event triggered = %d.", __func__, uxBits);
        *eEvent = (eMBMasterEventType)uxBits;
        xEventHappened = FALSE;
    }
    return xEventHappened;
}

// This function is initialize the OS resource for modbus master.
void vMBMasterOsResInit( void )
{
    xResourceMasterHdl = xEventGroupCreate();
    xEventGroupSetBits(xResourceMasterHdl, MB_EVENT_RESOURCE);
    MB_PORT_CHECK((xResourceMasterHdl != NULL), ; , "Resource create error.");
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
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits( xResourceMasterHdl,   // The event group being tested.
                                    MB_EVENT_RESOURCE,  // The bits within the event group to wait for.
                                    pdTRUE,             // Masked bits should be cleared before returning.
                                    pdFALSE,            // Don't wait for both bits, either bit will do.
                                    lTimeOut);          // Resource wait timeout.
    MB_PORT_CHECK((uxBits == MB_EVENT_RESOURCE), FALSE , "Take resource failure.");
    ESP_LOGD(MB_PORT_TAG,"%s:Take resource (%x) (%lu ticks).", __func__, uxBits,  lTimeOut);
    return TRUE;
}

/**
 * This function is release Modbus Master running resource.
 * Note:The resource is define by Operating System. If you not use OS this function can be empty.
 */
void vMBMasterRunResRelease( void )
{
    EventBits_t uxBits = xEventGroupSetBits( xResourceMasterHdl, MB_EVENT_RESOURCE );
    if (uxBits != MB_EVENT_RESOURCE) {
        // The returned resource mask may be = 0, if the task waiting for it is unblocked.
        // This is not an error but expected behavior.
        ESP_LOGD(MB_PORT_TAG,"%s: Release resource (%x) fail.", __func__, uxBits);
    }
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
    ESP_LOG_BUFFER_HEX_LEVEL("Err rcv buf", (void*)pucPDUData, (uint16_t)ucPDULength, ESP_LOG_DEBUG);
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
    ESP_LOGD(MB_PORT_TAG,"%s:Callback execute data handler failure.", __func__);
    ESP_LOG_BUFFER_HEX_LEVEL("Exec func buf", (void*)pucPDUData, (uint16_t)ucPDULength, ESP_LOG_DEBUG);
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
            // if we wait for certain event bits but get from poll subset
            ESP_LOGE(MB_PORT_TAG,"%s: incorrect event set = 0x%x", __func__, xRecvedEvent);
        }
        xEventGroupSetBits( xEventGroupMasterConfirmHdl, (xRecvedEvent & MB_EVENT_REQ_MASK) );
        if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_PROCESS_SUCCESS)) {
            eErrStatus = MB_MRE_NO_ERR;
        } else if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_RESPOND_TIMEOUT)) {
            eErrStatus = MB_MRE_TIMEDOUT;
        } else if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_RECEIVE_DATA)) {
            eErrStatus = MB_MRE_REV_DATA;
        } else if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_EXECUTE_FUNCTION)) {
            eErrStatus = MB_MRE_EXE_FUN;
        }
    } else {
        ESP_LOGE(MB_PORT_TAG,"%s: Incorrect event or timeout xRecvedEvent = 0x%x", __func__, uxBits);
        // https://github.com/espressif/esp-idf/issues/5275
        // if a no event is received, that means vMBMasterPortEventClose()
        // has been closed, so event group has been deleted by FreeRTOS, which
        // triggers the send of 0 value to the event group to unlock this task
        // waiting on it. For this patch, handles it as a time out without assert.
        eErrStatus = MB_MRE_TIMEDOUT;
    }
    return eErrStatus;
}

void vMBMasterPortEventClose(void)
{
    vEventGroupDelete(xEventGroupMasterHdl);
    vEventGroupDelete(xEventGroupMasterConfirmHdl);
    vEventGroupDelete(xResourceMasterHdl);
}

#endif
