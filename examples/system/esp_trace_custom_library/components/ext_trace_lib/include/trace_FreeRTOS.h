/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * External Trace Library - FreeRTOS trace hooks
 *
 * This header is pulled in (via esp_trace_freertos_impl.h) from FreeRTOSConfig.h
 * Do NOT include any FreeRTOS header here — keep this file restricted to:
 *   - forward declarations of the C functions called by the macros below,
 *   - the trace*() macro definitions themselves.
 *
 * Macros are allowed to reference FreeRTOS identifiers (pxTCB, xTicksToWait, ...)
 * by name — they are resolved later, when the macro is expanded inside the
 * FreeRTOS kernel .c files where those names are already in scope.
 *
 * Only the trace*() macros this example actually hooks are defined here.
 * Anything left undefined falls back to FreeRTOS's own empty default (see
 * the #ifndef guards in freertos/FreeRTOS.h).
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration so we can pass an encoder pointer from the adapter to
 * init_trace_lib() without pulling in esp_trace_port_encoder.h. */
typedef struct esp_trace_encoder esp_trace_encoder_t;

void init_trace_lib(esp_trace_encoder_t *enc);
void trace_lib_start(void);
void trace_lib_stop(void);

/* Hook implementations — defined in trace_FreeRTOS.c.
 * Kept void*-typed to avoid depending on FreeRTOS types in this header. */
void trace_lib_task_switched_in(void);
void trace_lib_task_create(void *pxNewTCB);
void trace_lib_isr_enter(uint32_t irq);
void trace_lib_isr_exit(void);
void trace_lib_isr_exit_to_scheduler(void);
void trace_lib_queue_send(void *pxQueue);
void trace_lib_queue_receive(void *pxQueue);
void trace_lib_queue_create(void *pxNewQueue);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------ *
 *  Active hooks (forwarded to trace_FreeRTOS.c)
 * ------------------------------------------------------------------ */
#define traceTASK_SWITCHED_IN()                          trace_lib_task_switched_in()
#define traceTASK_CREATE(pxNewTCB)                       trace_lib_task_create(pxNewTCB)
#define traceISR_ENTER(n)                                trace_lib_isr_enter(n)
#define traceISR_EXIT()                                  trace_lib_isr_exit()
#define traceISR_EXIT_TO_SCHEDULER()                     trace_lib_isr_exit_to_scheduler()
#define traceQUEUE_SEND(pxQueue)                         trace_lib_queue_send(pxQueue)
#define traceQUEUE_RECEIVE(pxQueue)                      trace_lib_queue_receive(pxQueue)
#define traceQUEUE_CREATE(pxNewQueue)                    trace_lib_queue_create(pxNewQueue)

/* All other trace*() macros fall back to FreeRTOS's default empty defines
 * (#ifndef ... #define ... empty in freertos/FreeRTOS.h); no need to list
 * them here. Add a mapping above when you want to hook one. */
