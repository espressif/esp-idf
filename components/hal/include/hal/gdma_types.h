/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
