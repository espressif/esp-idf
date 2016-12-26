/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_TIMER_H__
#define __ESP_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_types.h"
/* timer related */
typedef void os_timer_func_t(void *timer_arg);

typedef struct _os_timer_t {
    struct _os_timer_t *timer_next;
    void               *timer_handle;
    uint32_t             timer_expire;
    uint32_t             timer_period;
    os_timer_func_t    *timer_func;
    bool               timer_repeat_flag;
    void               *timer_arg;
} os_timer_t;

/** \defgroup Timer_APIs Software timer APIs
  * @brief Software timer APIs
  *
  * Timers of the following interfaces are software timers. Functions of the timers are executed during the tasks.
  * Since a task can be stopped, or be delayed because there are other tasks with higher priorities, the following os_timer interfaces cannot guarantee the precise execution of the timers.
  *    -  For the same timer, os_timer_arm (or os_timer_arm_us)  cannot be invoked repeatedly. os_timer_disarm should be invoked first.
  *    -  os_timer_setfn can only be invoked when the timer is not enabled, i.e., after  os_timer_disarm or before os_timer_arm (or os_timer_arm_us).
  *
  */

/** @addtogroup Timer_APIs
  * @{
  */

/**
  * @brief     Set the timer callback function.
  *
  * @attention 1. The callback function must be set in order to enable the timer.
  * @attention 2. Operating system scheduling is disabled in timer callback.
  *
  * @param     os_timer_t *ptimer : Timer structure
  * @param     os_timer_func_t *pfunction : timer callback function
  * @param     void *parg : callback function parameter
  *
  * @return    null
  */
void os_timer_setfn(os_timer_t *ptimer, os_timer_func_t *pfunction, void *parg);

/**
  * @brief  Enable the millisecond timer.
  *
  * @param  os_timer_t *ptimer : timer structure
  * @param  uint32_t milliseconds : Timing, unit: millisecond, the maximum value allowed is 0x41893
  * @param  bool repeat_flag : Whether the timer will be invoked repeatedly or not
  *
  * @return null
  */
void os_timer_arm(os_timer_t *ptimer, uint32_t msec, bool repeat_flag);

/**
  * @brief  Disarm the timer
  *
  * @param  os_timer_t *ptimer : Timer structure
  *
  * @return null
  */
void os_timer_disarm(os_timer_t *ptimer);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
