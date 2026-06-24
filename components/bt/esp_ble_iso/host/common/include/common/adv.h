/*
 * SPDX-FileCopyrightText: 2017-2021 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_ADV_H_
#define HOST_COMMON_ADV_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_ext_adv *bt_le_ext_adv_find(uint8_t adv_handle);

int bt_le_ext_adv_new_safe(uint8_t adv_handle);

int bt_le_ext_adv_delete_safe(uint8_t adv_handle);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_ADV_H_ */
