/*
 * ButtonScanner.h
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

#ifndef MAIN_BUTTON_SCANNER_HPP_
#define MAIN_BUTTON_SCANNER_HPP_

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "events.hpp"

class C_ButtonScanner {
public :
    C_ButtonScanner();
    ~C_ButtonScanner();
    void Start(const UBaseType_t uxPriority, EventQueueHandle_t outEvtQueue);
    void Stop();
private:
    static void vTaskScanButtons( void *pvParameters );
    bool SendEvent(evt_t evt);
    EventQueueHandle_t m_OutEvtQueue;
    TaskHandle_t m_TaskHandle;
    uint16_t m_LastInput;
    bool m_BtnStatus;
};

#endif /* MAIN_BUTTON_SCANNER_HPP_ */
