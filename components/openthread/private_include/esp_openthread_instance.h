/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize OpenThread instance(s) for the selected build (single or multiple).
 *
 * @return true on success, false otherwise.
 */
bool esp_openthread_instances_init(void);

/**
 * @brief Finalize OpenThread instance(s) for the selected build (single or multiple).
 *
 * @param[in] instance  Instance provided by the caller (may be unused in multi-instance builds).
 */
void esp_openthread_instances_deinit(otInstance *instance);

/**
 * @brief Check whether tasklets are pending.
 *
 * @param[in] instance  Instance provided by the caller (may be unused in multi-instance builds).
 */
bool esp_openthread_tasklets_are_pending(otInstance *instance);

/**
 * @brief Process pending tasklets.
 *
 * @param[in] instance  Instance provided by the caller (may be unused in multi-instance builds).
 */
void esp_openthread_tasklets_process(otInstance *instance);

/**
 * @brief Initialize the NCP/RCP application for the selected instance mode.
 *
 * @param[in] instance  Instance provided by the caller (may be unused in multi-instance builds).
 *
 * @note Only used when CONFIG_OPENTHREAD_RADIO is enabled.
 */
void esp_openthread_ncp_app_init(otInstance *instance);

#ifdef __cplusplus
}
#endif
