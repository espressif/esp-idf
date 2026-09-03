/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
 * @brief Bit definitions used by the LL layer to describe supported data burst sizes
 */
typedef enum {
    GDMA_BURST_SIZE_SUPPORT_4   = 1U << 2,
    GDMA_BURST_SIZE_SUPPORT_8   = 1U << 3,
    GDMA_BURST_SIZE_SUPPORT_16  = 1U << 4,
    GDMA_BURST_SIZE_SUPPORT_32  = 1U << 5,
    GDMA_BURST_SIZE_SUPPORT_64  = 1U << 6,
    GDMA_BURST_SIZE_SUPPORT_128 = 1U << 7,
} gdma_burst_size_support_t;

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
