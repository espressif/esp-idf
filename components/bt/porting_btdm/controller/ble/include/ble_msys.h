/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MSYS_H_
#define _BLE_MSYS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int ble_msys_init(void);

void ble_msys_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MSYS_H_ */
