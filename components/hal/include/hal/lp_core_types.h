/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LP_CORE_SUPPORTED

#if SOC_LP_CORE_SUPPORT_ETM
/**
 * @brief LP-Core event types enum
 */
typedef enum {
    LP_CORE_EVENT_ERR_INTR,     /*!< Exception triggered on LP-Core */
    LP_CORE_EVENT_START_INTR,   /*!< LP-Core clock has been turned on */
    LP_CORE_EVENT_MAX,          /*!< Maximum number of LP-Core events */
} lp_core_etm_event_type_t;

/**
 * @brief LP-Core task types enum
 */
typedef enum {
    LP_CORE_TASK_WAKEUP_CPU,         /*!< LP-Core wake-up task */
    LP_CORE_TASK_MAX,                /*!< Maximum number of LP-Core tasks */
} lp_core_etm_task_type_t;

#endif //SOC_LP_CORE_SUPPORT_ETM

#endif //SOC_LP_CORE_SUPPORTED

#ifdef __cplusplus
}
#endif
