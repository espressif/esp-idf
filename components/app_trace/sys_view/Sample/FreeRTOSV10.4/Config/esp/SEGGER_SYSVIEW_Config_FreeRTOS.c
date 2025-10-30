/*
 * SPDX-FileCopyrightText: 1995-2021 SEGGER Microcontroller GmbH
 *
 * SPDX-License-Identifier: BSD-1-Clause
 *
 * SPDX-FileContributor: 2017-2025 Espressif Systems (Shanghai) CO LTD
 */
/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2021 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.42                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Config_FreeRTOS.c
Purpose : Sample setup configuration of SystemView with FreeRTOS.
Revision: $Rev: 7745 $
*/
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "soc/interrupts.h"
#include "esp_sysview_trace.h"
#include "esp_trace_port_encoder.h"
#include "esp_trace_port_transport.h"
#include "esp_trace_util.h"

extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

/* Encoder reference for accessing lock functions
 * This is set by SEGGER_RTT_ESP_SetEncoder() during encoder initialization */
extern esp_trace_encoder_t* SEGGER_SYSVIEW_GetEncoder(void);

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
// The application name to be displayed in SystemViewer
#define SYSVIEW_APP_NAME        "FreeRTOS Application"

// The target device name
#define SYSVIEW_DEVICE_NAME     CONFIG_IDF_TARGET
// The target core name
#define SYSVIEW_CORE_NAME       "core0" // In dual core, this will be renamed by OpenOCD as core1

// The lowest RAM address used for IDs (pointers)
#define SYSVIEW_RAM_BASE        (SOC_DROM_LOW)

#ifdef CONFIG_FREERTOS_TICK_SUPPORT_CORETIMER
#if CONFIG_FREERTOS_CORETIMER_0
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER0_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif
#if CONFIG_FREERTOS_CORETIMER_1
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER1_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif

#elif CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER
    #define SYSTICK_INTR_ID (ETS_SYSTIMER_TARGET0_INTR_SOURCE)
#endif // CONFIG_FREERTOS_TICK_SUPPORT_CORETIMER

// SystemView is single core specific: it implies that SEGGER_SYSVIEW_LOCK()
// disables IRQs (disables rescheduling globally). So we can not use finite timeouts for locks and return error
// in case of expiration, because error will not be handled and SEGGER's code will go further implying that
// everything is fine, so for multi-core env we have to wait on underlying lock forever
#define SEGGER_LOCK_WAIT_TMO  ESP_TRACE_TMO_INFINITE

/*********************************************************************
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
    char irq_str[32] = "I#";
    SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",D="SYSVIEW_DEVICE_NAME",C="SYSVIEW_CORE_NAME",O=FreeRTOS");
    strcat(itoa(SYSTICK_INTR_ID, irq_str + 2, 10), "=SysTick");
    SEGGER_SYSVIEW_SendSysDesc(irq_str);
    size_t isr_count = sizeof(esp_isr_names)/sizeof(esp_isr_names[0]);
    for (size_t i = 0; i < isr_count; ++i) {
        if (esp_isr_names[i] == NULL || (ETS_INTERNAL_INTR_SOURCE_OFF + i) == SYSTICK_INTR_ID)
            continue;
        strcat(itoa(ETS_INTERNAL_INTR_SOURCE_OFF + i, irq_str + 2, 10), "=");
        strncat(irq_str, esp_isr_names[i], sizeof(irq_str) - strlen(irq_str) - 1);
        SEGGER_SYSVIEW_SendSysDesc(irq_str);
    }
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
void SEGGER_SYSVIEW_Conf(void) {
    U32 disable_evts = 0;

    int timestamp_freq = esp_trace_timestamp_init();
    SEGGER_SYSVIEW_Init(timestamp_freq, esp_trace_cpu_freq_get(),
                        &SYSVIEW_X_OS_TraceAPI, _cbSendSystemDesc);
    SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);

#if !CONFIG_SEGGER_SYSVIEW_EVT_OVERFLOW_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_OVERFLOW;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_ISR_ENTER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_ENTER;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_ISR_EXIT_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_EXIT;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TASK_START_EXEC_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_START_EXEC;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TASK_STOP_EXEC_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_STOP_EXEC;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TASK_START_READY_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_START_READY;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TASK_STOP_READY_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_STOP_READY;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TASK_CREATE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_CREATE;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TASK_TERMINATE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_TERMINATE;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_IDLE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_IDLE;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_ISR_TO_SCHED_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_TO_SCHEDULER;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TIMER_ENTER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TIMER_ENTER;
#endif
#if !CONFIG_SEGGER_SYSVIEW_EVT_TIMER_EXIT_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TIMER_EXIT;
#endif
  SEGGER_SYSVIEW_DisableEvents(disable_evts);
}

U32 SEGGER_SYSVIEW_X_GetTimestamp(void)
{
    return esp_trace_timestamp_get();
}

void SEGGER_SYSVIEW_X_RTT_Lock(void)
{
}

void SEGGER_SYSVIEW_X_RTT_Unlock(void)
{
}

unsigned SEGGER_SYSVIEW_X_SysView_Lock(void)
{
    esp_trace_encoder_t *encoder = SEGGER_SYSVIEW_GetEncoder();
    if (encoder) {
        // Use encoder-level lock
        return encoder->vt->take_lock(encoder, SEGGER_LOCK_WAIT_TMO);
    }
    return 0;
}

void SEGGER_SYSVIEW_X_SysView_Unlock(unsigned int_state)
{
    esp_trace_encoder_t *encoder = SEGGER_SYSVIEW_GetEncoder();
    if (encoder) {
        // Use encoder-level unlock
        encoder->vt->give_lock(encoder, int_state);
    }
}

/*************************** End of file ****************************/
