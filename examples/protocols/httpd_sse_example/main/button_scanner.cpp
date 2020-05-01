/*
 * ButtonScanner.cpp
 *
 * Scans the button and sends an event in the outQueue.
 *
 * This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   V 1.0 2020.04.28 Nicolas Korber (nicolas.korber_at_bluewin.ch)
 *
 *   */

#include <esp32/rom/gpio.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include "button_scanner.hpp"

const gpio_num_t BUTTON = GPIO_NUM_0;

static const char *TAG = "Button Scanner";

const uint32_t StackDepth = 2000;

C_ButtonScanner::C_ButtonScanner() :
    m_OutEvtQueue( NULL ),
    m_TaskHandle( NULL ),
    m_LastInput( 0 ),
    m_BtnStatus( false )
{
    gpio_pad_select_gpio(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
}

C_ButtonScanner::~C_ButtonScanner()
{
    Stop();
}

void C_ButtonScanner::Start(const UBaseType_t uxPriority, EventQueueHandle_t outEvtQueue)
{
    m_OutEvtQueue = outEvtQueue;
    xTaskCreate( vTaskScanButtons,
                 "ScanButton",
                 StackDepth,
                 this,
                 uxPriority,
                 &m_TaskHandle );
    if (NULL != m_TaskHandle) {
        ESP_LOGI(TAG, "Scan button task started");
    }
}


void C_ButtonScanner::Stop()
{
    if (NULL != m_TaskHandle) {
        vTaskDelete( m_TaskHandle );
    }
}

bool C_ButtonScanner::SendEvent(evt_t evt)
{
    C_Event Event = C_Event(evt);
    if (pdTRUE == xQueueSendToBack(m_OutEvtQueue, &Event, ( TickType_t ) 0 )) {
        // keep track if event placed in queue
        ESP_LOGD(TAG, "Button event queued");
        return (true);
    }
    return (false);
}

void C_ButtonScanner::vTaskScanButtons( void *pvParameters )
{
    C_ButtonScanner *pScanner = reinterpret_cast<C_ButtonScanner *>(pvParameters);
    while (1) {
        // Debounce
        uint16_t tmp = gpio_get_level(BUTTON) ^ 0x0001;
        pScanner->m_LastInput = pScanner->m_LastInput << 1 | (tmp & 0x0001);
        switch (pScanner->m_BtnStatus) {
        case false:
            if (0xFFFF == pScanner->m_LastInput) {
                // Button activated
                pScanner->m_BtnStatus = true;
            }
            break;
        case true:
            if (0x0000 == pScanner->m_LastInput) {
                // Button released
                if (pScanner->SendEvent(button_active)) {
                    // message queued
                    pScanner->m_BtnStatus = false;
                }
            }
            break;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

