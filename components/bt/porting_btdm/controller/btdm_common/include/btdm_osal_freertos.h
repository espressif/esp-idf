/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BTDM_OSAL_FREERTOS_H_
#define _BTDM_OSAL_FREERTOS_H_

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "sdkconfig.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "btdm_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((defined(CONFIG_BT_NIMBLE_USE_ESP_TIMER) && CONFIG_BT_NIMBLE_USE_ESP_TIMER) ||                \
     (defined(CONFIG_BT_LE_USE_ESP_TIMER) && CONFIG_BT_LE_USE_ESP_TIMER))
/* Use esp timer instead of FreeRTOS timer to implement the callout. */
#define BTDM_OSAL_USE_ESP_TIMER (1)
#else
#define BTDM_OSAL_USE_ESP_TIMER (0)
#endif

struct btdm_osal_event_freertos {
    bool queued;
    void (*fn)(struct btdm_osal_event *ev);
    void *arg;
};

struct btdm_osal_eventq_freertos {
    QueueHandle_t q;
};

struct btdm_osal_callout_freertos {
#if BTDM_OSAL_USE_ESP_TIMER
    esp_timer_handle_t handle;
#else
    TimerHandle_t handle;
#endif
    struct btdm_osal_eventq *evq;
    struct btdm_osal_event ev;
};

struct btdm_osal_mutex_freertos {
    SemaphoreHandle_t handle;
};

struct btdm_osal_sem_freertos {
    SemaphoreHandle_t handle;
};

typedef void btdm_osal_event_fn_freertos(struct btdm_osal_event_freertos *ev);

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_OSAL_FREERTOS_H_ */
