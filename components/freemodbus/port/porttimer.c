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
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
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
 * File: $Id: portother.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "sdkconfig.h"
#include "mb.h"
#include "mbport.h"
#include "driver/timer.h"
#include "port_serial_slave.h"
#include "sdkconfig.h"

#if CONFIG_FMB_TIMER_PORT_ENABLED

#define MB_US50_FREQ            (20000) // 20kHz 1/20000 = 50mks
#define MB_DISCR_TIME_US        (50)    // 50uS = one discreet for timer

#define MB_TIMER_PRESCALLER     ((TIMER_BASE_CLK / MB_US50_FREQ) - 1);
#define MB_TIMER_SCALE          (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define MB_TIMER_DIVIDER        ((TIMER_BASE_CLK / 1000000UL) * MB_DISCR_TIME_US - 1) // divider for 50uS
#define MB_TIMER_WITH_RELOAD    (1)

static const USHORT usTimerIndex = CONFIG_FMB_TIMER_INDEX; // Modbus Timer index used by stack
static const USHORT usTimerGroupIndex = CONFIG_FMB_TIMER_GROUP; // Modbus Timer group index used by stack
static timer_isr_handle_t xTimerIntHandle;                      // Timer interrupt handle

/* ----------------------- Start implementation -----------------------------*/
static void IRAM_ATTR vTimerGroupIsr(void *param)
{
    assert((int)param == usTimerIndex);
    // Retrieve the counter value from the timer that reported the interrupt
    timer_group_clr_intr_status_in_isr(usTimerGroupIndex, usTimerIndex);
    (void)pxMBPortCBTimerExpired(); // Timer callback function
    // Enable alarm
    timer_group_enable_alarm_in_isr(usTimerGroupIndex, usTimerIndex);
}
#endif

BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    MB_PORT_CHECK((usTim1Timerout50us > 0), FALSE,
            "Modbus timeout discreet is incorrect.");
    esp_err_t xErr;
    timer_config_t config;
    config.alarm_en = TIMER_ALARM_EN;
    config.auto_reload = MB_TIMER_WITH_RELOAD;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = MB_TIMER_PRESCALLER;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;
    // Configure timer
    xErr = timer_init(usTimerGroupIndex, usTimerIndex, &config);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
            "timer init failure, timer_init() returned (0x%x).", xErr);
    // Stop timer counter
    xErr = timer_pause(usTimerGroupIndex, usTimerIndex);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "stop timer failure, timer_pause() returned (0x%x).", xErr);
    // Reset counter value
    xErr = timer_set_counter_value(usTimerGroupIndex, usTimerIndex, 0x00000000ULL);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "timer set value failure, timer_set_counter_value() returned (0x%x).", xErr);
    // wait3T5_us = 35 * 11 * 100000 / baud; // the 3.5T symbol time for baudrate
    // Set alarm value for usTim1Timerout50us * 50uS
    xErr = timer_set_alarm_value(usTimerGroupIndex, usTimerIndex, (uint32_t)(usTim1Timerout50us));
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "failure to set alarm failure, timer_set_alarm_value() returned (0x%x).",
                    (uint32_t)xErr);
    // Register ISR for timer
    xErr = timer_isr_register(usTimerGroupIndex, usTimerIndex, vTimerGroupIsr,
                                (void*)(uint32_t)usTimerIndex, MB_PORT_TIMER_ISR_FLAG, &xTimerIntHandle);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "timer set value failure, timer_isr_register() returned (0x%x).",
                    (uint32_t)xErr);
#endif
    return TRUE;
}

void vMBPortTimersEnable(void)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    ESP_ERROR_CHECK(timer_pause(usTimerGroupIndex, usTimerIndex));
    ESP_ERROR_CHECK(timer_set_counter_value(usTimerGroupIndex, usTimerIndex, 0ULL));
    ESP_ERROR_CHECK(timer_enable_intr(usTimerGroupIndex, usTimerIndex));
    ESP_ERROR_CHECK(timer_start(usTimerGroupIndex, usTimerIndex));
#endif
}

void MB_PORT_ISR_ATTR
vMBPortTimersDisable(void)
{
#if CONFIG_FMB_TIMER_PORT_ENABLED
    if( (BOOL)xPortInIsrContext() ) {
        timer_group_set_counter_enable_in_isr(usTimerGroupIndex, usTimerIndex, TIMER_PAUSE);
    } else {
        ESP_ERROR_CHECK(timer_pause(usTimerGroupIndex, usTimerIndex));
        ESP_ERROR_CHECK(timer_set_counter_value(usTimerGroupIndex, usTimerIndex, 0ULL));
        // Disable timer interrupt
        ESP_ERROR_CHECK(timer_disable_intr(usTimerGroupIndex, usTimerIndex));
    }
#endif
}

void vMBPortTimerClose(void)
{
#ifdef CONFIG_FMB_TIMER_PORT_ENABLED
    ESP_ERROR_CHECK(timer_deinit(usTimerGroupIndex, usTimerIndex));
    ESP_ERROR_CHECK(esp_intr_free(xTimerIntHandle));
#endif
}
