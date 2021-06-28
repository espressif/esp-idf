/*
 * Events.h
 *
 * Defines the events.
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

#ifndef MAIN_EVENTS_HPP_
#define MAIN_EVENTS_HPP_

#include <cstdlib>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

typedef enum {
    evt_none = 0,
    button_active,
    refresh_hw_status
} evt_t;

typedef QueueHandle_t EventQueueHandle_t;
EventQueueHandle_t EventQueueCreate(size_t size);

class C_Event {
public :
    C_Event(evt_t evt):
        m_evt(evt) {};
    C_Event() {};
    evt_t getEvt(void)
    {
        return m_evt;
    };
protected:
    evt_t m_evt;
};

class C_HWStatus {
public:
    C_HWStatus(bool LedStatus):
        m_LedStatus(LedStatus) {};
    void LedStatus(bool stat)
    {
        m_LedStatus = stat;
    };
    bool getLedStatus()
    {
        return m_LedStatus;
    };
private :
    bool m_LedStatus;
};

#endif /* MAIN_EVENTS_HPP_ */
