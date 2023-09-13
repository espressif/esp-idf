/*
 * SPDX-FileCopyrightText: 1995-2021 SEGGER Microcontroller GmbH
 *
 * SPDX-License-Identifier: BSD-1-Clause
 *
 * SPDX-FileContributor: 2017-2022 Espressif Systems (Shanghai) CO LTD
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
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"
#include "esp_app_trace.h"
#include "esp_app_trace_util.h"
#include "esp_intr_alloc.h"
#include "esp_cpu.h"
#include "soc/soc.h"
#include "soc/interrupts.h"
#include "esp_private/esp_clk.h"

extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

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
#if CONFIG_IDF_TARGET_ARCH_XTENSA
#define SYSVIEW_CORE_NAME       "xtensa"
#elif CONFIG_IDF_TARGET_ARCH_RISCV
#define SYSVIEW_CORE_NAME       "riscv"
#endif

// Determine which timer to use as timestamp source
#if CONFIG_APPTRACE_SV_TS_SOURCE_CCOUNT
#define TS_USE_CCOUNT 1
#elif CONFIG_APPTRACE_SV_TS_SOURCE_ESP_TIMER
#define TS_USE_ESP_TIMER 1
#else
#define TS_USE_TIMERGROUP 1
#endif

#if TS_USE_TIMERGROUP
#include "driver/gptimer.h"

// Timer group timer divisor
#define SYSVIEW_TIMER_DIV       2

// Frequency of the timestamp, using APB as GPTimer source clock
#define SYSVIEW_TIMESTAMP_FREQ  (esp_clk_apb_freq() / SYSVIEW_TIMER_DIV)

// GPTimer handle
gptimer_handle_t s_sv_gptimer;

#endif // TS_USE_TIMERGROUP

#if TS_USE_ESP_TIMER
// esp_timer provides 1us resolution
#define SYSVIEW_TIMESTAMP_FREQ  (1000000)
#endif // TS_USE_ESP_TIMER

#if TS_USE_CCOUNT
// CCOUNT is incremented at CPU frequency
#define SYSVIEW_TIMESTAMP_FREQ  (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000)
#endif // TS_USE_CCOUNT

// System Frequency.
#define SYSVIEW_CPU_FREQ        (esp_clk_cpu_freq())

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
#define SEGGER_LOCK_WAIT_TMO  ESP_APPTRACE_TMO_INFINITE

static esp_apptrace_lock_t s_sys_view_lock = {.mux = portMUX_INITIALIZER_UNLOCKED, .int_state = 0};

/*********************************************************************
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
    char irq_str[32];
    SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",D="SYSVIEW_DEVICE_NAME",C="SYSVIEW_CORE_NAME",O=FreeRTOS");
    snprintf(irq_str, sizeof(irq_str), "I#%d=SysTick", SYSTICK_INTR_ID);
    SEGGER_SYSVIEW_SendSysDesc(irq_str);
    size_t isr_count = sizeof(esp_isr_names)/sizeof(esp_isr_names[0]);
    for (size_t i = 0; i < isr_count; ++i) {
        if (esp_isr_names[i] == NULL || (ETS_INTERNAL_INTR_SOURCE_OFF + i) == SYSTICK_INTR_ID)
            continue;
        snprintf(irq_str, sizeof(irq_str), "I#%d=%s", ETS_INTERNAL_INTR_SOURCE_OFF + i, esp_isr_names[i]);
        SEGGER_SYSVIEW_SendSysDesc(irq_str);
    }
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
static void SEGGER_SYSVIEW_TS_Init(void)
{
    /* We only need to initialize something if we use Timer Group.
     * esp_timer and ccount can be used as is.
     */
#if TS_USE_TIMERGROUP
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = SYSVIEW_TIMESTAMP_FREQ,
    };
    // pick any free GPTimer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&config, &s_sv_gptimer));
    /* Start counting */
    gptimer_enable(s_sv_gptimer);
    gptimer_start(s_sv_gptimer);
#endif // TS_USE_TIMERGROUP
}

void SEGGER_SYSVIEW_Conf(void) {
    U32 disable_evts = 0;

    SEGGER_SYSVIEW_TS_Init();
    SEGGER_SYSVIEW_Init(SYSVIEW_TIMESTAMP_FREQ, SYSVIEW_CPU_FREQ,
                        &SYSVIEW_X_OS_TraceAPI, _cbSendSystemDesc);
    SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);

#if !CONFIG_APPTRACE_SV_EVT_OVERFLOW_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_OVERFLOW;
#endif
#if !CONFIG_APPTRACE_SV_EVT_ISR_ENTER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_ENTER;
#endif
#if !CONFIG_APPTRACE_SV_EVT_ISR_EXIT_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_EXIT;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TASK_START_EXEC_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_START_EXEC;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TASK_STOP_EXEC_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_STOP_EXEC;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TASK_START_READY_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_START_READY;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TASK_STOP_READY_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_STOP_READY;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TASK_CREATE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_CREATE;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TASK_TERMINATE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_TERMINATE;
#endif
#if !CONFIG_APPTRACE_SV_EVT_IDLE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_IDLE;
#endif
#if !CONFIG_APPTRACE_SV_EVT_ISR_TO_SCHED_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_TO_SCHEDULER;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TIMER_ENTER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TIMER_ENTER;
#endif
#if !CONFIG_APPTRACE_SV_EVT_TIMER_EXIT_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TIMER_EXIT;
#endif
  SEGGER_SYSVIEW_DisableEvents(disable_evts);
}

U32 SEGGER_SYSVIEW_X_GetTimestamp(void)
{
#if TS_USE_TIMERGROUP
    uint64_t ts = 0;
    gptimer_get_raw_count(s_sv_gptimer, &ts);
    return (U32) ts; // return lower part of counter value
#elif TS_USE_CCOUNT
    return esp_cpu_get_cycle_count();
#elif TS_USE_ESP_TIMER
    return (U32) esp_timer_get_time(); // return lower part of counter value
#endif
}

void SEGGER_SYSVIEW_X_RTT_Lock(void)
{
}

void SEGGER_SYSVIEW_X_RTT_Unlock(void)
{
}

unsigned SEGGER_SYSVIEW_X_SysView_Lock(void)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, SEGGER_LOCK_WAIT_TMO);
    esp_apptrace_lock_take(&s_sys_view_lock, &tmo);
    // to be recursive save IRQ status on the stack of the caller to keep it from overwriting
    return s_sys_view_lock.int_state;
}

void SEGGER_SYSVIEW_X_SysView_Unlock(unsigned int_state)
{
    s_sys_view_lock.int_state = int_state;
    esp_apptrace_lock_give(&s_sys_view_lock);
}

/*************************** End of file ****************************/
