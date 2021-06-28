/*
 * OutputManager.h
 *
 * Manages the led according to the messages received in the receive queue.
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
 *
 *   */

#ifndef MAIN_OUTPUT_MANAGER_HPP_
#define MAIN_OUTPUT_MANAGER_HPP_

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "events.hpp"

class C_OutputManager {
public :
    C_OutputManager();
    ~C_OutputManager();
    EventQueueHandle_t get_outEvtQueue() const;
    void Start(const UBaseType_t uxPriority);
    void Stop();
private:
    static void vTaskLoop( void *pvParameters );
    EventQueueHandle_t m_outEvtQueue;
    TaskHandle_t m_TaskHandle;
};

#endif /* MAIN_OUTPUT_MANAGER_HPP_ */
