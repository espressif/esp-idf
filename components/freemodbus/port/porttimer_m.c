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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbport.h"
#include "port_serial_master.h"

#define MB_US50_FREQ            (20000) // 20kHz 1/20000 = 50mks
#define MB_TICK_TIME_US         (50)    // 50uS = one tick for timer

#define MB_TIMER_PRESCALLER     ((TIMER_BASE_CLK / MB_US50_FREQ) - 1);
#define MB_TIMER_SCALE          (TIMER_BASE_CLK / TIMER_DIVIDER)
#define MB_TIMER_DIVIDER        ((TIMER_BASE_CLK / 1000000UL) * MB_TICK_TIME_US - 1) // divider for 50uS
#define MB_TIMER_WITH_RELOAD    (1)

// Timer group and timer number to measure time (configurable in KConfig)
#define MB_TIMER_INDEX CONFIG_FMB_TIMER_INDEX
#define MB_TIMER_GROUP CONFIG_FMB_TIMER_GROUP

#define MB_TIMER_IO_LED 0

/* ----------------------- Variables ----------------------------------------*/
static USHORT usT35TimeOut50us;

static const USHORT usTimerIndex = MB_TIMER_INDEX;      // Initialize Modbus Timer index used by stack,
static const USHORT usTimerGroupIndex = MB_TIMER_GROUP; // Timer group index used by stack

/* ----------------------- static functions ---------------------------------*/

static void IRAM_ATTR vTimerGroupIsr(void *param)
{
    assert((int)param == usTimerIndex);
    // Retrieve the the counter value from the timer that reported the interrupt
    timer_group_intr_clr_in_isr(usTimerGroupIndex, usTimerIndex);
    (void)pxMBMasterPortCBTimerExpired(); // Timer expired callback function
    // Enable alarm
    timer_group_enable_alarm_in_isr(usTimerGroupIndex, usTimerIndex);
}

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
    MB_PORT_CHECK((usTimeOut50us > 0), FALSE,
            "Modbus timeout discreet is incorrect.");
    // Save timer reload value for Modbus T35 period
    usT35TimeOut50us = usTimeOut50us;
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
            "timer init failure, timer_init() returned (0x%x).", (uint32_t)xErr);
    // Stop timer counter
    xErr = timer_pause(usTimerGroupIndex, usTimerIndex);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "stop timer failure, timer_pause() returned (0x%x).", (uint32_t)xErr);
    // Reset counter value
    xErr = timer_set_counter_value(usTimerGroupIndex, usTimerIndex, 0x00000000ULL);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "timer set value failure, timer_set_counter_value() returned (0x%x).",
                    (uint32_t)xErr);
    // wait3T5_us = 35 * 11 * 100000 / baud; // the 3.5T symbol time for baudrate
    // Set alarm value for usTimeOut50us * 50uS
    xErr = timer_set_alarm_value(usTimerGroupIndex, usTimerIndex, (uint32_t)(usTimeOut50us));
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "failure to set alarm failure, timer_set_alarm_value() returned (0x%x).",
                    (uint32_t)xErr);
    // Register ISR for timer
    xErr = timer_isr_register(usTimerGroupIndex, usTimerIndex,
                                vTimerGroupIsr, (void*)(uint32_t)usTimerIndex, ESP_INTR_FLAG_IRAM, NULL);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "timer set value failure, timer_isr_register() returned (0x%x).",
                    (uint32_t)xErr);
    return TRUE;
}

// Set alarm value for usTimerTimeOut50us * 50uS
static BOOL xMBMasterPortTimersEnable(USHORT usTimerTics50us)
{
    MB_PORT_CHECK((usTimerTics50us > 0), FALSE,
                            "incorrect tick value for timer = (0x%x).",
                            (uint32_t)usTimerTics50us);
    esp_err_t xErr;
    xErr = timer_pause(usTimerGroupIndex, usTimerIndex); // stop timer
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                        "timer pause failure, timer_pause() returned (0x%x).",
                        (uint32_t)xErr);
    xErr = timer_set_counter_value(usTimerGroupIndex, usTimerIndex, 0ULL); // reset timer
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                        "timer set counter failure, timer_set_counter_value() returned (0x%x).",
                        (uint32_t)xErr);
    // Set alarm value to number of 50uS ticks
    xErr = timer_set_alarm_value(usTimerGroupIndex, usTimerIndex,
                                            (uint32_t)(usTimerTics50us));
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                    "timer set alarm failure, timer_set_alarm_value() returned (0x%x).",
                    (uint32_t)xErr);
    xErr = timer_enable_intr(usTimerGroupIndex, usTimerIndex);
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                            "timer enable interrupt failure, timer_enable_intr() returned (0x%x).",
                            (uint32_t)xErr);
    xErr = timer_start(usTimerGroupIndex, usTimerIndex); // start timer
    MB_PORT_CHECK((xErr == ESP_OK), FALSE,
                            "timer start failure, timer_start() returned (0x%x).",
                            (uint32_t)xErr);
    //ESP_LOGD(MB_PORT_TAG,"%s Init timer.", __func__);
    return TRUE;
}

void vMBMasterPortTimersT35Enable(void)
{
    USHORT usTimerTicks = usT35TimeOut50us;

    // Set current timer mode, don't change it.
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
    // Set timer period
    (void)xMBMasterPortTimersEnable(usTimerTicks);
}

void vMBMasterPortTimersConvertDelayEnable(void)
{
    // Covert time in milliseconds into ticks
    USHORT usTimerTicks = ((MB_MASTER_DELAY_MS_CONVERT * 1000) / MB_TICK_TIME_US);

    // Set current timer mode
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
    ESP_LOGD(MB_PORT_TAG,"%s Convert delay enable.", __func__);
    (void)xMBMasterPortTimersEnable(usTimerTicks);
}

void vMBMasterPortTimersRespondTimeoutEnable(void)
{
    USHORT usTimerTicks = (MB_MASTER_TIMEOUT_MS_RESPOND * 1000 / MB_TICK_TIME_US);

    vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
    ESP_LOGD(MB_PORT_TAG,"%s Respond enable timeout.", __func__);
    (void)xMBMasterPortTimersEnable(usTimerTicks);
}

void vMBMasterPortTimersDisable(void)
{
    // Stop timer and then reload timer counter value
    ESP_ERROR_CHECK(timer_pause(usTimerGroupIndex, usTimerIndex));
    ESP_ERROR_CHECK(timer_set_counter_value(usTimerGroupIndex, usTimerIndex, 0ULL));
    // Disable timer interrupt
    ESP_ERROR_CHECK(timer_disable_intr(usTimerGroupIndex, usTimerIndex));
}

void vMBMasterPortTimerClose(void)
{
    ESP_ERROR_CHECK(timer_pause(usTimerGroupIndex, usTimerIndex));
    ESP_ERROR_CHECK(timer_disable_intr(usTimerGroupIndex, usTimerIndex));
}
