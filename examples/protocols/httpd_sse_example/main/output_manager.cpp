/*
 * OutManager.cpp
 *
 * Manages the led according to the messages received in the receive queue.
 * Informs also the http server.
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
 *   V 1.0 2020.04.29 Nicolas Korber (nicolas.korber_at_bluewin.ch)

 *   */

#include <algorithm>
#include <esp_log.h>
#include <cstring>
#include <driver/gpio.h>
#include "httpd_sse_server.hpp"
#include "output_manager.hpp"

const gpio_num_t LED = GPIO_NUM_5;

static const char *TAG = "Output Manager";

const uint32_t StackDepth = 3000;

C_OutputManager::C_OutputManager() :
    m_TaskHandle( NULL )
{
    m_outEvtQueue = EventQueueCreate(10);
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_INPUT_OUTPUT);
}

C_OutputManager::~C_OutputManager()
{
    Stop();
}

void C_OutputManager::Start(const UBaseType_t uxPriority)
{

    xTaskCreate( vTaskLoop, "RelayManager", StackDepth, this, uxPriority, &m_TaskHandle );
    if (NULL != m_TaskHandle) {
        ESP_LOGI(TAG, "Write led task started");
    }
}

void C_OutputManager::Stop()
{
    if (NULL != m_TaskHandle) {
        vTaskDelete( m_TaskHandle );
    }
}

EventQueueHandle_t C_OutputManager::get_outEvtQueue() const
{
    return m_outEvtQueue;
}

void C_OutputManager::vTaskLoop( void *pvParameters )
{
    C_OutputManager *pManager = reinterpret_cast<C_OutputManager *>(pvParameters);
    while (1) {
        // Treat event if any (non blocking)
        C_Event event;
        while (pdTRUE == xQueueReceive(pManager->m_outEvtQueue, &event, ( TickType_t ) 0 )) {
            evt_t evt = event.getEvt();
            uint16_t newVal = 0;
            C_HWStatus *pHWStatus = NULL;
            switch (evt) {
            case button_active:
                newVal = gpio_get_level(LED) ^ 0x0001;
                gpio_set_level(LED, newVal);
                pHWStatus = new C_HWStatus(1 == newVal);
                HardwareUpdated(pHWStatus);
//              free(pHWStatus); will be freed when used
                break;
            case refresh_hw_status:
                // send http server
                pHWStatus = new C_HWStatus(1 == gpio_get_level(LED));
                HardwareUpdated(pHWStatus);
//              free(pHWStatus);  will be freed when used
                break;
            default:
                break;
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
