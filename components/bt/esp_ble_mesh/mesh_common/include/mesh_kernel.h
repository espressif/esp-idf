/*
 * Copyright (c) 2016, Wind River Systems, Inc.
 * Additional Copyright (c) 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_KERNEL_H_
#define _BLE_MESH_KERNEL_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "mesh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_BLUEDROID_ENABLED
#ifdef CONFIG_BLUEDROID_PINNED_TO_CORE
#define BLE_MESH_ADV_TASK_CORE  (CONFIG_BLUEDROID_PINNED_TO_CORE < portNUM_PROCESSORS ? CONFIG_BLUEDROID_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define BLE_MESH_ADV_TASK_CORE  (0)
#endif
#endif

#ifdef CONFIG_NIMBLE_ENABLED
#ifdef CONFIG_NIMBLE_PINNED_TO_CORE
#define BLE_MESH_ADV_TASK_CORE  (CONFIG_NIMBLE_PINNED_TO_CORE < portNUM_PROCESSORS ? CONFIG_NIMBLE_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define BLE_MESH_ADV_TASK_CORE  (0)
#endif
#endif

#define BLE_MESH_ADV_TASK_STACK_SIZE    3072

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
void k_sleep(s32_t duration);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_KERNEL_H_ */

