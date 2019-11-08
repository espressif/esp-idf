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
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbport.h"
#include "mbrtu.h"
#include "mbconfig.h"

#include <string.h>
#include "driver/uart.h"
#include "soc/dport_access.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "port_serial_master.h"

/* ----------------------- Defines ------------------------------------------*/

#define MB_BAUD_RATE_DEFAULT        (115200)
#define MB_QUEUE_LENGTH             (CONFIG_FMB_QUEUE_LENGTH)

#define MB_SERIAL_TASK_PRIO         (CONFIG_FMB_SERIAL_TASK_PRIO)
#define MB_SERIAL_TASK_STACK_SIZE   (CONFIG_FMB_SERIAL_TASK_STACK_SIZE)
#define MB_SERIAL_TOUT              (3) // 3.5*8 = 28 ticks, TOUT=3 -> ~24..33 ticks

// Set buffer size for transmission
#define MB_SERIAL_BUF_SIZE          (CONFIG_FMB_SERIAL_BUF_SIZE)
#define MB_SERIAL_TX_TOUT_MS        (100)
#define MB_SERIAL_TX_TOUT_TICKS     pdMS_TO_TICKS(MB_SERIAL_TX_TOUT_MS) // timeout for transmission

/* ----------------------- Static variables ---------------------------------*/
static const CHAR *TAG = "MB_MASTER_SERIAL";

// A queue to handle UART event.
static QueueHandle_t xMbUartQueue;
static TaskHandle_t  xMbTaskHandle;

// The UART hardware port number
static UCHAR ucUartNumber = UART_NUM_MAX - 1;

static BOOL bRxStateEnabled = FALSE; // Receiver enabled flag
static BOOL bTxStateEnabled = FALSE; // Transmitter enabled flag

static UCHAR ucBuffer[MB_SERIAL_BUF_SIZE]; // Temporary buffer to transfer received data to modbus stack
static USHORT uiRxBufferPos = 0;    // position in the receiver buffer

void vMBMasterPortSerialEnable(BOOL bRxEnable, BOOL bTxEnable)
{
    // This function can be called from xMBRTUTransmitFSM() of different task
    if (bTxEnable) {
        bTxStateEnabled = TRUE;
    } else {
        bTxStateEnabled = FALSE;
    }
    if (bRxEnable) {
        bRxStateEnabled = TRUE;
        vTaskResume(xMbTaskHandle); // Resume receiver task
    } else {
        vTaskSuspend(xMbTaskHandle); // Block receiver task
        bRxStateEnabled = FALSE;
    }
}

static void vMBMasterPortSerialRxPoll(size_t xEventSize)
{
    USHORT usLength;

    if (bRxStateEnabled) {
        if (xEventSize > 0) {
            xEventSize = (xEventSize > MB_SERIAL_BUF_SIZE) ?  MB_SERIAL_BUF_SIZE : xEventSize;
            // Get received packet into Rx buffer
            usLength = uart_read_bytes(ucUartNumber, &ucBuffer[0], xEventSize, portMAX_DELAY);
            uiRxBufferPos = 0;
            for(USHORT usCnt = 0; usCnt < usLength; usCnt++ ) {
                // Call the Modbus stack callback function and let it fill the stack buffers.
                ( void )pxMBMasterFrameCBByteReceived(); // calls callback xMBRTUReceiveFSM()
            }
            // The buffer is transferred into Modbus stack and is not needed here any more
            uart_flush_input(ucUartNumber);
            ESP_LOGD(TAG, "Receive: %d(bytes in buffer)\n", (uint32_t)usLength);
        }
    } else {
        ESP_LOGE(TAG, "%s: bRxState disabled but junk data (%d bytes) received. ", __func__, (uint16_t)xEventSize);
    }
}

BOOL xMBMasterPortSerialTxPoll(void)
{
    USHORT usCount = 0;
    BOOL bNeedPoll = FALSE;

    if( bTxStateEnabled ) {
        // Continue while all response bytes put in buffer or out of buffer
        while((bNeedPoll == FALSE) && (usCount++ < MB_SERIAL_BUF_SIZE)) {
            // Calls the modbus stack callback function to let it fill the UART transmit buffer.
            bNeedPoll = pxMBMasterFrameCBTransmitterEmpty( ); // calls callback xMBRTUTransmitFSM();
        }
        ESP_LOGD(TAG, "MB_TX_buffer sent: (%d) bytes.", (uint16_t)(usCount - 1));
        // Waits while UART sending the packet
        esp_err_t xTxStatus = uart_wait_tx_done(ucUartNumber, MB_SERIAL_TX_TOUT_TICKS);
        bTxStateEnabled = FALSE;
        vMBMasterPortSerialEnable( TRUE, FALSE );
        MB_PORT_CHECK((xTxStatus == ESP_OK), FALSE, "mb serial sent buffer failure.");
        return TRUE;
    }
    return FALSE;
}

// UART receive event task
static void vUartTask(void* pvParameters)
{
    uart_event_t xEvent;
    for(;;) {
        if (xQueueReceive(xMbUartQueue, (void*)&xEvent, portMAX_DELAY) == pdTRUE) { // portMAX_DELAY
            ESP_LOGD(TAG, "MB_uart[%d] event:", ucUartNumber);
            switch(xEvent.type) {
                //Event of UART receiving data
                case UART_DATA:
                    ESP_LOGD(TAG,"Receive data, len: %d.", xEvent.size);
                    // Read received data and send it to modbus stack
                    vMBMasterPortSerialRxPoll(xEvent.size);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGD(TAG, "hw fifo overflow.");
                    xQueueReset(xMbUartQueue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGD(TAG, "ring buffer full.");
                    xQueueReset(xMbUartQueue);
                    uart_flush_input(ucUartNumber);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGD(TAG, "uart rx break.");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGD(TAG, "uart parity error.");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGD(TAG, "uart frame error.");
                    break;
                default:
                    ESP_LOGD(TAG, "uart event type: %d.", xEvent.type);
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    esp_err_t xErr = ESP_OK;
    MB_PORT_CHECK((eParity <= MB_PAR_EVEN), FALSE, "mb serial set parity failure.");
    // Set communication port number
    ucUartNumber = ucPORT;
    // Configure serial communication parameters
    UCHAR ucParity = UART_PARITY_DISABLE;
    UCHAR ucData = UART_DATA_8_BITS;
    switch(eParity){
        case MB_PAR_NONE:
            ucParity = UART_PARITY_DISABLE;
            break;
        case MB_PAR_ODD:
            ucParity = UART_PARITY_ODD;
            break;
        case MB_PAR_EVEN:
            ucParity = UART_PARITY_EVEN;
            break;
    }
    switch(ucDataBits){
        case 5:
            ucData = UART_DATA_5_BITS;
            break;
        case 6:
            ucData = UART_DATA_6_BITS;
            break;
        case 7:
            ucData = UART_DATA_7_BITS;
            break;
        case 8:
            ucData = UART_DATA_8_BITS;
            break;
        default:
            ucData = UART_DATA_8_BITS;
            break;
    }
    uart_config_t xUartConfig = {
        .baud_rate = ulBaudRate,
        .data_bits = ucData,
        .parity = ucParity,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 2,
    };
    // Set UART config
    xErr = uart_param_config(ucUartNumber, &xUartConfig);
    MB_PORT_CHECK((xErr == ESP_OK),
            FALSE, "mb config failure, uart_param_config() returned (0x%x).", (uint32_t)xErr);
    // Install UART driver, and get the queue.
    xErr = uart_driver_install(ucUartNumber, MB_SERIAL_BUF_SIZE, MB_SERIAL_BUF_SIZE,
                                    MB_QUEUE_LENGTH, &xMbUartQueue, MB_PORT_SERIAL_ISR_FLAG);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
            "mb serial driver failure, uart_driver_install() returned (0x%x).", (uint32_t)xErr);
    // Set timeout for TOUT interrupt (T3.5 modbus time)
    xErr = uart_set_rx_timeout(ucUartNumber, MB_SERIAL_TOUT);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
            "mb serial set rx timeout failure, uart_set_rx_timeout() returned (0x%x).", (uint32_t)xErr);
    // Create a task to handle UART events
    BaseType_t xStatus = xTaskCreate(vUartTask, "uart_queue_task", MB_SERIAL_TASK_STACK_SIZE,
                                        NULL, MB_SERIAL_TASK_PRIO, &xMbTaskHandle);
    if (xStatus != pdPASS) {
        vTaskDelete(xMbTaskHandle);
        // Force exit from function with failure
        MB_PORT_CHECK(FALSE, FALSE,
                "mb stack serial task creation error. xTaskCreate() returned (0x%x).",
                (uint32_t)xStatus);
    } else {
        vTaskSuspend(xMbTaskHandle); // Suspend serial task while stack is not started
    }
    uiRxBufferPos = 0;
    ESP_LOGD(MB_PORT_TAG,"%s Init serial.", __func__);
    return TRUE;
}

void vMBMasterPortSerialClose(void)
{
    (void)vTaskDelete(xMbTaskHandle);
    ESP_ERROR_CHECK(uart_driver_delete(ucUartNumber));
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
    // Send one byte to UART transmission buffer
    // This function is called by Modbus stack
    UCHAR ucLength = uart_write_bytes(ucUartNumber, &ucByte, 1);
    return (ucLength == 1);
}

// Get one byte from intermediate RX buffer
BOOL xMBMasterPortSerialGetByte(CHAR* pucByte)
{
    assert(pucByte != NULL);
    MB_PORT_CHECK((uiRxBufferPos < MB_SERIAL_BUF_SIZE),
            FALSE, "mb stack serial get byte failure.");
    *pucByte = ucBuffer[uiRxBufferPos];
    uiRxBufferPos++;
    return TRUE;
}
