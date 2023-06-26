/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "hal/parlio_types.h"
#include "hal/parlio_hal.h"
#include "esp_heap_caps.h"
#include "driver/parlio_types.h"

#if CONFIG_PARLIO_ISR_IRAM_SAFE
#define PARLIO_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define PARLIO_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif

#if SOC_PARLIO_TX_RX_SHARE_INTERRUPT
#define PARLIO_INTR_ALLOC_FLAG_SHARED ESP_INTR_FLAG_SHARED
#else
#define PARLIO_INTR_ALLOC_FLAG_SHARED 0
#endif

#if CONFIG_PARLIO_ISR_IRAM_SAFE
#define PARLIO_INTR_ALLOC_FLAG   (ESP_INTR_FLAG_LOWMED | PARLIO_INTR_ALLOC_FLAG_SHARED | ESP_INTR_FLAG_IRAM)
#else
#define PARLIO_INTR_ALLOC_FLAG   (ESP_INTR_FLAG_LOWMED | PARLIO_INTR_ALLOC_FLAG_SHARED)
#endif

#define PARLIO_PM_LOCK_NAME_LEN_MAX 16

#ifdef __cplusplus
extern "C" {
#endif

enum {
    PARLIO_TX_QUEUE_READY,
    PARLIO_TX_QUEUE_PROGRESS,
    PARLIO_TX_QUEUE_COMPLETE,
    PARLIO_TX_QUEUE_MAX,
};

typedef enum {
    PARLIO_TX_FSM_INIT_WAIT,
    PARLIO_TX_FSM_INIT,
    PARLIO_TX_FSM_ENABLE_WAIT,
    PARLIO_TX_FSM_ENABLE,
    PARLIO_TX_FSM_RUN_WAIT,
    PARLIO_TX_FSM_RUN,
} parlio_tx_fsm_t;

typedef struct parlio_group_t {
    int group_id;             // group ID, index from 0
    portMUX_TYPE spinlock;    // to protect per-group register level concurrent access
    parlio_hal_context_t hal; // hal layer for each group
    parlio_tx_unit_handle_t tx_units[SOC_PARLIO_TX_UNITS_PER_GROUP]; // tx unit handles
} parlio_group_t;

parlio_group_t *parlio_acquire_group_handle(int group_id);

void parlio_release_group_handle(parlio_group_t *group);

#ifdef __cplusplus
}
#endif
