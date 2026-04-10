/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTDM_COEX_H_
#define _BTDM_COEX_H_

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t btdm_coex_init(void);

void btdm_coex_deinit(void);

esp_err_t btdm_coex_enable(void);

void btdm_coex_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_COEX_H_ */
