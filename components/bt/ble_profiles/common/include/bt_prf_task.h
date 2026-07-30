/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file bt_prf_task.h
 * @brief Shared BLE profile task (event queue worker)
 *
 * A single, process-wide event queue served by one dedicated FreeRTOS task,
 * shared by BLE profiles that need to run their business logic off the host
 * task. Profiles post short, non-blocking handlers here instead of each
 * spawning its own task, which keeps task count and RAM bounded.
 *
 * Profiles post work with bt_prf_task_eventq(): obtain the underlying queue and
 * drive it directly with a caller-owned ::bt_osal_event embedded in the profile's
 * own context. This avoids per-post allocation and lets an in-flight event
 * coalesce (an already-queued event is never enqueued twice), and it is the
 * queue to bind a ::bt_osal_callout to for delayed work.
 *
 * Built only when CONFIG_BT_PRF_TASK_ENABLED is set. A profile that relies on
 * the shared task should `select BT_PRF_TASK_ENABLED` in its own Kconfig.
 *
 * @note This is the profile-facing policy layer on top of the OS-primitive OSAL
 *       (see bt_osal.h). The handler runs in the shared task's context, so it
 *       MUST be non-blocking and quick; hand long or blocking work off to a
 *       dedicated task.
 */

#pragma once

#include "bt_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bring up the shared BLE profile task
 *
 * @note The active host bring-up already calls this (esp_bluedroid_init() /
 *       nimble_port_init()), so profiles do NOT need to. Exposed mainly for tests.
 */
bt_osal_error_t bt_prf_task_init(void);

/**
 * @brief Tear down the shared BLE profile task
 *
 * @note The active host tear-down already calls this (esp_bluedroid_deinit() /
 *       nimble_port_deinit()), so profiles do NOT need to. Exposed mainly for tests.
 */
bt_osal_error_t bt_prf_task_deinit(void);

/**
 * @brief Get the shared profile task's event queue
 *
 * Use it to drive the queue directly with a caller-owned event (zero-allocation
 * posting via bt_osal_eventq_put()) or to bind a ::bt_osal_callout for delayed
 * work that runs in the shared task's context.
 *
 * @return Pointer to the shared event queue, or NULL if the task is not running
 */
struct bt_osal_eventq *bt_prf_task_eventq(void);

/**
 * @brief Check whether the shared BLE profile task is running
 *
 * @return true if bt_prf_task_init() has succeeded and the task is up
 */
bool bt_prf_task_is_running(void);

#ifdef __cplusplus
}
#endif
