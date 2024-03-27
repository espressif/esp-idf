/*
 * SPDX-FileCopyrightText: 2016 Wind River Systems, Inc.
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_KERNEL_H_
#define _BLE_MESH_KERNEL_H_

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "mesh/config.h"
#include "mesh/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#ifdef CONFIG_BT_BLUEDROID_PINNED_TO_CORE
#define BLE_MESH_ADV_TASK_CORE  (CONFIG_BT_BLUEDROID_PINNED_TO_CORE < CONFIG_FREERTOS_NUMBER_OF_CORES ? CONFIG_BT_BLUEDROID_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define BLE_MESH_ADV_TASK_CORE  (0)
#endif
#endif

#ifdef CONFIG_BT_NIMBLE_ENABLED
#ifdef CONFIG_BT_NIMBLE_PINNED_TO_CORE
#define BLE_MESH_ADV_TASK_CORE  (CONFIG_BT_NIMBLE_PINNED_TO_CORE < CONFIG_FREERTOS_NUMBER_OF_CORES ? CONFIG_BT_NIMBLE_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define BLE_MESH_ADV_TASK_CORE  (0)
#endif
#endif

#define BLE_MESH_ADV_TASK_STACK_SIZE    3072
#define BLE_MESH_ADV_TASK_NAME          "mesh_adv_task"
#define BLE_MESH_ADV_TASK_PRIO          (configMAX_PRIORITIES - 5)

/**
 * @brief Put the current thread to sleep.
 *
 * This routine puts the current thread to sleep for @a duration
 * milliseconds.
 *
 * @param duration Number of milliseconds to sleep.
 *
 * @return N/A
 */
void k_sleep(int32_t duration);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_KERNEL_H_ */
