/* esp_event (event loop library) basic example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef EVENT_SOURCE_H_
#define EVENT_SOURCE_H_

#include "esp_event.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

// This example makes use of two event sources: a periodic timer, and a task.

// Declarations for event source 1: periodic timer
#define TIMER_EXPIRIES_COUNT        3        // number of times the periodic timer expires before being stopped
#define TIMER_PERIOD                1000000  // period of the timer event source in microseconds

extern esp_timer_handle_t g_timer;           // the periodic timer object

// Declare an event base
ESP_EVENT_DECLARE_BASE(TIMER_EVENTS);        // declaration of the timer events family

enum {                                       // declaration of the specific events under the timer event family
    TIMER_EVENT_STARTED,                     // raised when the timer is first started
    TIMER_EVENT_EXPIRY,                      // raised when a period of the timer has elapsed
    TIMER_EVENT_STOPPED                      // raised when the timer has been stopped
};

// Declarations for event source 2: task
#define TASK_ITERATIONS_COUNT        5       // number of times the task iterates
#define TASK_ITERATIONS_UNREGISTER   3       // count at which the task event handler is unregistered
#define TASK_PERIOD                  500     // period of the task loop in milliseconds

ESP_EVENT_DECLARE_BASE(TASK_EVENTS);         // declaration of the task events family

enum {
    TASK_ITERATION_EVENT,                    // raised during an iteration of the loop within the task
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef EVENT_SOURCE_H_
