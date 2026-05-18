/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_PROFILE_TMAS_H_
#define HOST_BLUEDROID_PROFILE_TMAS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_bluedroid_tmas_init(void);

int bt_le_bluedroid_tmas_start(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_PROFILE_TMAS_H_ */
