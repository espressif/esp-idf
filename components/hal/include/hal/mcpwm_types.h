// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

typedef enum {
    MCPWM_TIMER_DIRECTION_UP,   /*!< Counting direction: Increase */
    MCPWM_TIMER_DIRECTION_DOWN, /*!< Counting direction: Decrease */
} mcpwm_timer_direction_t;

typedef enum {
    MCPWM_TIMER_EVENT_ZERO, /*!< MCPWM timer counts to zero */
    MCPWM_TIMER_EVENT_PEAK, /*!< MCPWM timer counts to peak */
} mcpwm_timer_event_t;

typedef enum {
    MCPWM_TIMER_COUNT_MODE_PAUSE,   /*!< MCPWM timer paused */
    MCPWM_TIMER_COUNT_MODE_UP,      /*!< MCPWM timer counting up */
    MCPWM_TIMER_COUNT_MODE_DOWN,    /*!< MCPWM timer counting down */
    MCPWM_TIMER_COUNT_MODE_UP_DOWN, /*!< MCPWM timer counting up and down */
} mcpwm_timer_count_mode_t;

typedef enum {
    MCPWM_TIMER_STOP_AT_ZERO,       /*!< MCPWM timer stops when couting to zero */
    MCPWM_TIMER_STOP_AT_PEAK,       /*!< MCPWM timer stops when counting to peak */
    MCPWM_TIMER_START_NO_STOP,      /*!< MCPWM timer starts couting */
    MCPWM_TIMER_START_STOP_AT_ZERO, /*!< MCPWM timer starts counting and stops when couting to zero */
    MCPWM_TIMER_START_STOP_AT_PEAK, /*!< MCPWM timer starts counting and stops when counting to peak */
} mcpwm_timer_execute_cmd_t;

typedef enum {
    MCPWM_GEN_ACTION_KEEP,   /*!< Generator action: Keep the same level */
    MCPWM_GEN_ACTION_LOW,    /*!< Generator action: Force to low level */
    MCPWM_GEN_ACTION_HIGH,   /*!< Generator action: Force to high level */
    MCPWM_GEN_ACTION_TOGGLE, /*!< Generator action: Toggle level */
} mcpwm_generator_action_t;

typedef enum {
    MCPWM_TRIP_TYPE_CBC, /*!< CBC trip type, shut down the operator cycle by cycle*/
    MCPWM_TRIP_TYPE_OST, /*!< OST trip type, shut down the operator in one shot */
} mcpwm_trip_type_t;
