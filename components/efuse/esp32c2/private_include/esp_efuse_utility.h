/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define COUNT_EFUSE_REG_PER_BLOCK 8      /* The number of registers per block. */

#define ESP_EFUSE_SECURE_VERSION_NUM_BLOCK EFUSE_BLK0

#define ESP_EFUSE_FIELD_CORRESPONDS_CODING_SCHEME(scheme, max_num_bit)

#ifdef __cplusplus
}
#endif
