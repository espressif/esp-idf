/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_PROFILE_CSIS_H_
#define HOST_BLUEDROID_PROFILE_CSIS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_bluedroid_csis_init(void *csis_svc, uint8_t count);

int bt_le_bluedroid_csis_start(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_PROFILE_CSIS_H_ */
