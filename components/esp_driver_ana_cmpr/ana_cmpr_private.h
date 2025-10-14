/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "driver/ana_cmpr_types.h"
#include "soc/soc_caps.h"
#include "hal/ana_cmpr_ll.h"
#include "soc/ana_cmpr_periph.h"

#define TAG "ana_cmpr"

#if CONFIG_ANA_CMPR_OBJ_CACHE_SAFE
#define ANA_CMPR_MEM_ALLOC_CAPS         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ANA_CMPR_MEM_ALLOC_CAPS         MALLOC_CAP_DEFAULT
#endif

#if CONFIG_ANA_CMPR_ISR_CACHE_SAFE
// the interrupt source of Analog Comparator is marked as shared because
// it may share with other GPIO interrupt events
// it may share with different analog comparator units
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED)
#else
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_SHARED)
#endif

#define ANA_CMPR_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANA_CMPR_FSM_INIT,        // Comparator is in the initialization stage, not enabled yet
    ANA_CMPR_FSM_ENABLE,      // Comparator is enabled
    ANA_CMPR_FSM_WAIT,        // Comparator is in the middle of state change, so busy, other operations should wait
} ana_cmpr_fsm_t;

/**
 * @brief Get the analog comparator unit id from the handle
 *
 * @param cmpr The handle of the analog comparator unit
 * @return The id of the analog comparator unit
 */
ana_cmpr_unit_t ana_cmpr_get_unit_id(ana_cmpr_handle_t cmpr);

#ifdef __cplusplus
}
#endif
