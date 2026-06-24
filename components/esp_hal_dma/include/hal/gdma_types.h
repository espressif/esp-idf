/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __has_include("hal/gdma_channel.h")
#include "hal/gdma_channel.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of GDMA channel direction
 */
typedef enum {
    GDMA_CHANNEL_DIRECTION_TX, /*!< GDMA channel direction: TX */
    GDMA_CHANNEL_DIRECTION_RX, /*!< GDMA channel direction: RX */
} gdma_channel_direction_t;

/**
 * @brief GDMA channel events that supported by the ETM module
 */
typedef enum {
    GDMA_ETM_EVENT_EOF, /*!< Event that the GDMA engine meets EOF descriptor */
    GDMA_ETM_EVENT_MAX, /*!< Maximum number of events */
} gdma_etm_event_type_t;

/**
 * @brief GDMA channel tasks that supported by the ETM module
 */
typedef enum {
    GDMA_ETM_TASK_START, /*!< Start the GDMA machine */
    GDMA_ETM_TASK_MAX,   /*!< Maximum number of events */
} gdma_etm_task_type_t;

#ifdef __cplusplus
}
#endif
