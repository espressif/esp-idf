/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2015 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
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
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* conditions are met:                                                *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this list of conditions and the following disclaimer.    *
*                                                                    *
* o Redistributions in binary form must reproduce the above          *
*   copyright notice, this list of conditions and the following      *
*   disclaimer in the documentation and/or other materials provided  *
*   with the distribution.                                           *
*                                                                    *
* o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
*   nor the names of its contributors may be used to endorse or      *
*   promote products derived from this software without specific     *
*   prior written permission.                                        *
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
*       SystemView version: V2.42                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Config_FreeRTOS.c
Purpose : Sample setup configuration of SystemView with FreeRTOS.
Revision: $Rev: 3734 $
*/
#include "freertos/FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"
#include "esp_app_trace.h"
#include "esp_app_trace_util.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "soc/interrupts.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#include "esp32/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#include "esp32s2beta/clk.h"
#endif


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

// Determine which timer to use as timestamp source
#if CONFIG_SYSVIEW_TS_SOURCE_CCOUNT
#define TS_USE_CCOUNT 1
#elif CONFIG_SYSVIEW_TS_SOURCE_ESP_TIMER
#define TS_USE_ESP_TIMER 1
#else
#define TS_USE_TIMERGROUP 1
#endif

#if TS_USE_TIMERGROUP
#include "driver/timer.h"

// Timer group timer divisor
#define SYSVIEW_TIMER_DIV       2

// Frequency of the timestamp.
#define SYSVIEW_TIMESTAMP_FREQ  (esp_clk_apb_freq() / SYSVIEW_TIMER_DIV)

// Timer ID and group ID
#if defined(CONFIG_SYSVIEW_TS_SOURCE_TIMER_00) || defined(CONFIG_SYSVIEW_TS_SOURCE_TIMER_01)
#define TS_TIMER_ID 0
#else
#define TS_TIMER_ID 1
#endif // TIMER_00 || TIMER_01

#if defined(CONFIG_SYSVIEW_TS_SOURCE_TIMER_00) || defined(CONFIG_SYSVIEW_TS_SOURCE_TIMER_10)
#define TS_TIMER_GROUP 0
#else
#define TS_TIMER_GROUP 1
#endif // TIMER_00 || TIMER_10

#endif // TS_USE_TIMERGROUP

#if TS_USE_ESP_TIMER
// esp_timer provides 1us resolution
#define SYSVIEW_TIMESTAMP_FREQ  (1000000)
#endif // TS_USE_ESP_TIMER

#if TS_USE_CCOUNT
// CCOUNT is incremented at CPU frequency
#if CONFIG_IDF_TARGET_ESP32
#define SYSVIEW_TIMESTAMP_FREQ  (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000)
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#define SYSVIEW_TIMESTAMP_FREQ  (CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ * 1000000)
#endif

#endif // TS_USE_CCOUNT

// System Frequency.
#define SYSVIEW_CPU_FREQ        (esp_clk_cpu_freq())

// The lowest RAM address used for IDs (pointers)
#define SYSVIEW_RAM_BASE        (SOC_DROM_LOW)

#if CONFIG_FREERTOS_CORETIMER_0
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER0_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif
#if CONFIG_FREERTOS_CORETIMER_1
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER1_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif

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
    SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",D="SYSVIEW_DEVICE_NAME",C=Xtensa,O=FreeRTOS");
    snprintf(irq_str, sizeof(irq_str), "I#%d=SysTick", SYSTICK_INTR_ID);
    SEGGER_SYSVIEW_SendSysDesc(irq_str);
    size_t isr_count = sizeof(esp_isr_names)/sizeof(esp_isr_names[0]);
    for (size_t i = 0; i < isr_count; ++i) {
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
    timer_config_t config = {
        .alarm_en = 0,
        .auto_reload = 0,
        .counter_dir = TIMER_COUNT_UP,
        .divider = SYSVIEW_TIMER_DIV,
        .counter_en = 0
    };
    /* Configure timer */
    timer_init(TS_TIMER_GROUP, TS_TIMER_ID, &config);
    /* Load counter value */
    timer_set_counter_value(TS_TIMER_GROUP, TS_TIMER_ID, 0x00000000ULL);
    /* Start counting */
    timer_start(TS_TIMER_GROUP, TS_TIMER_ID);
#endif // TS_USE_TIMERGROUP
}

void SEGGER_SYSVIEW_Conf(void) {
    U32 disable_evts = 0;

    SEGGER_SYSVIEW_TS_Init();
    SEGGER_SYSVIEW_Init(SYSVIEW_TIMESTAMP_FREQ, SYSVIEW_CPU_FREQ,
                        &SYSVIEW_X_OS_TraceAPI, _cbSendSystemDesc);
    SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);

#if !CONFIG_SYSVIEW_EVT_OVERFLOW_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_OVERFLOW;
#endif
#if !CONFIG_SYSVIEW_EVT_ISR_ENTER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_ENTER;
#endif
#if !CONFIG_SYSVIEW_EVT_ISR_EXIT_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_EXIT;
#endif
#if !CONFIG_SYSVIEW_EVT_TASK_START_EXEC_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_START_EXEC;
#endif
#if !CONFIG_SYSVIEW_EVT_TASK_STOP_EXEC_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_STOP_EXEC;
#endif
#if !CONFIG_SYSVIEW_EVT_TASK_START_READY_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_START_READY;
#endif
#if !CONFIG_SYSVIEW_EVT_TASK_STOP_READY_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_STOP_READY;
#endif
#if !CONFIG_SYSVIEW_EVT_TASK_CREATE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_CREATE;
#endif
#if !CONFIG_SYSVIEW_EVT_TASK_TERMINATE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TASK_TERMINATE;
#endif
#if !CONFIG_SYSVIEW_EVT_IDLE_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_IDLE;
#endif
#if !CONFIG_SYSVIEW_EVT_ISR_TO_SCHEDULER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_ISR_TO_SCHEDULER;
#endif
#if !CONFIG_SYSVIEW_EVT_TIMER_ENTER_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TIMER_ENTER;
#endif
#if !CONFIG_SYSVIEW_EVT_TIMER_EXIT_ENABLE
    disable_evts |= SYSVIEW_EVTMASK_TIMER_EXIT;
#endif
  SEGGER_SYSVIEW_DisableEvents(disable_evts);
}

U32 SEGGER_SYSVIEW_X_GetTimestamp(void)
{
#if TS_USE_TIMERGROUP
    uint64_t ts = 0;
    timer_get_counter_value(TS_TIMER_GROUP, TS_TIMER_ID, &ts);
    return (U32) ts; // return lower part of counter value
#elif TS_USE_CCOUNT
    return portGET_RUN_TIME_COUNTER_VALUE();
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
